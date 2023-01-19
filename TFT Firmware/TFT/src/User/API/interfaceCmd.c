#include "interfaceCmd.h"
#include "includes.h"

GCODE_QUEUE infoCmd;
GCODE_QUEUE infoCacheCmd;  
static uint8_t cmd_index = 0;
static bool ispolling = true;


static bool cmd_seen(char code)
{
  for (cmd_index = 0; infoCmd.queue[infoCmd.index_r].gcode[cmd_index] != 0 && cmd_index < CMD_MAX_CHAR; cmd_index++)
  {
    if (infoCmd.queue[infoCmd.index_r].gcode[cmd_index] == code)
    {
      cmd_index += 1;
      return true;
    }
  }
  return false;
}


static int32_t cmd_value(void)
{
  return (strtol(&infoCmd.queue[infoCmd.index_r].gcode[cmd_index], NULL, 10));
}


static float cmd_float(void)
{
  return (strtod(&infoCmd.queue[infoCmd.index_r].gcode[cmd_index], NULL));
}

#if defined(SERIAL_PORT_2) || defined(BUZZER_PIN)

bool static startsWith(TCHAR *search, TCHAR *string)
{
  return (strstr(string, search) - string == cmd_index) ? true : false;
}
#endif


void commonStoreCmd(GCODE_QUEUE *pQueue, const char* format, va_list va)
{
  vsnprintf(pQueue->queue[pQueue->index_w].gcode, CMD_MAX_CHAR, format, va);

  pQueue->queue[pQueue->index_w].src = SERIAL_PORT;
  pQueue->index_w = (pQueue->index_w + 1) % CMD_MAX_LIST;
  pQueue->count++;
}



bool storeCmd(const char * format,...)
{
  if (strlen(format) == 0) return false;

  GCODE_QUEUE *pQueue = &infoCmd;

  if (pQueue->count >= CMD_MAX_LIST)
  {
    reminderMessage(LABEL_BUSY, STATUS_BUSY);
    return false;
  }

  va_list va;
  va_start(va, format);
  commonStoreCmd(pQueue, format, va);
  va_end(va);

  return true;
}



void mustStoreCmd(const char * format,...)
{
  if (strlen(format) == 0) return;

  GCODE_QUEUE *pQueue = &infoCmd;

  if (pQueue->count >= CMD_MAX_LIST)
    reminderMessage(LABEL_BUSY, STATUS_BUSY);

  while (pQueue->count >= CMD_MAX_LIST)
  {
    loopProcess();
  }

  
  
  va_list va;
  va_start(va, format);
  commonStoreCmd(pQueue, format, va);
  va_end(va);
}



void mustStoreScript(const char * format,...)
{
  if (strlen(format) == 0) return;

  char script[256];
  va_list va;
  va_start(va, format);
  vsnprintf(script, 256, format, va);
  va_end(va);

  char *p = script;
  uint16_t i = 0;
  char cmd[CMD_MAX_CHAR];
  for (;;)
  {
    char c = *p++;
    if (!c) return;
    cmd[i++] = c;

    if (c == '\n')
    {
      cmd[i] = 0;
      mustStoreCmd("%s", cmd);
      i = 0;
    }
  }
}



bool storeCmdFromUART(uint8_t port, const char * gcode)
{
  if (strlen(gcode) == 0) return false;
  GCODE_QUEUE *pQueue = &infoCmd;

  if (pQueue->count >= CMD_MAX_LIST)
  {
    reminderMessage(LABEL_BUSY, STATUS_BUSY);
    return false;
  }

  strncpy(pQueue->queue[pQueue->index_w].gcode, gcode, CMD_MAX_CHAR);

  pQueue->queue[pQueue->index_w].src = port;
  pQueue->index_w = (pQueue->index_w + 1) % CMD_MAX_LIST;
  pQueue->count++;

  return true;
}



void mustStoreCacheCmd(const char * format,...)
{
  GCODE_QUEUE *pQueue = &infoCacheCmd;

  if (pQueue->count == CMD_MAX_LIST) reminderMessage(LABEL_BUSY, STATUS_BUSY);

  while (pQueue->count >= CMD_MAX_LIST)
  {
    loopProcess();
  }

  va_list va;
  va_start(va, format);
  commonStoreCmd(pQueue, format, va);
  va_end(va);
}


bool moveCacheToCmd(void)
{
  if (infoCmd.count >= CMD_MAX_LIST) return false;
  if (infoCacheCmd.count == 0) return false;

  storeCmd("%s", infoCacheCmd.queue[infoCacheCmd.index_r].gcode);
  infoCacheCmd.count--;
  infoCacheCmd.index_r = (infoCacheCmd.index_r + 1) % CMD_MAX_LIST;
  return true;
}


void clearCmdQueue(void)
{
  infoCmd.count = infoCmd.index_w = infoCmd.index_r = 0;
  infoCacheCmd.count = infoCacheCmd.index_w = infoCacheCmd.index_r = 0;
  heatSetUpdateWaiting(false);
  printSetUpdateWaiting(false);
}


void purgeLastCmd(void)
{
  infoCmd.count--;
  infoCmd.index_r = (infoCmd.index_r + 1) % CMD_MAX_LIST;
}


void sendQueueCmd(void)
{
  if (infoHost.wait == true) return;
  if (infoCmd.count == 0) return;

  bool avoid_terminal = false;
  uint16_t  cmd = 0;
  cmd_index = 0;
  
  bool fromTFT = (infoCmd.queue[infoCmd.index_r].src == SERIAL_PORT);

  if (!ispolling && fromTFT)
  { 
    purgeLastCmd();
    return;
  }

  
  if (infoCmd.queue[infoCmd.index_r].gcode[0] == 'N')
  {
    cmd_index = strcspn(infoCmd.queue[infoCmd.index_r].gcode, " ") + 1;
  }

  switch (infoCmd.queue[infoCmd.index_r].gcode[cmd_index])
  {
    
    case 'M':
      cmd = strtol(&infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 1], NULL, 10);
      switch (cmd)
      {
        case 0:
        case 1:
          if (isPrinting() && infoMachineSettings.firmwareType != FW_REPRAPFW)  
            setPrintPause(true, true);
          break;

        case 18:  
        case 84:
          if (cmd_seen('S') && !cmd_seen('Y') && !cmd_seen('Z') && !cmd_seen('E'))
          {
            
            
            
          }
          else
          {
            
            coordinateSetKnown(false);
          }
          break;

        #ifdef SERIAL_PORT_2
          case 20:  
            if (!fromTFT)
            {
              if (startsWith("M20 SD:", infoCmd.queue[infoCmd.index_r].gcode) ||
                  startsWith("M20 U:", infoCmd.queue[infoCmd.index_r].gcode))
              {
                if (startsWith("M20 SD:", infoCmd.queue[infoCmd.index_r].gcode))
                  infoFile.source = TFT_SD;
                else
                  infoFile.source = TFT_UDISK;
                strncpy(infoFile.title, &infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 4], MAX_PATH_LEN);
                
                for (int i = 0; i < MAX_PATH_LEN && infoFile.title[i] != 0; i++)
                {
                  if ((infoFile.title[i] == '*') || (infoFile.title[i] == '\n') || (infoFile.title[i] == '\r'))
                  {
                    infoFile.title[i] = 0;
                    break;
                  }
                }
                Serial_Puts(SERIAL_PORT_2, "Begin file list\n");
                if (mountFS() == true && scanPrintFiles() == true)
                {
                  for (uint16_t i = 0; i < infoFile.fileCount; i++)
                  {
                    Serial_Puts(SERIAL_PORT_2, infoFile.file[i]);
                    Serial_Puts(SERIAL_PORT_2, "\n");
                  }
                  for (uint16_t i = 0; i < infoFile.folderCount; i++)
                  {
                    Serial_Puts(SERIAL_PORT_2, "/");
                    Serial_Puts(SERIAL_PORT_2, infoFile.folder[i]);
                    Serial_Puts(SERIAL_PORT_2, "/\n");
                  }
                }
                Serial_Puts(SERIAL_PORT_2, "End file list\nok\n");
                purgeLastCmd();
                return;
              }
            }
            break;

          case 23:  
            if (!fromTFT)
            {
              if (startsWith("M23 SD:", infoCmd.queue[infoCmd.index_r].gcode) ||
                  startsWith("M23 U:", infoCmd.queue[infoCmd.index_r].gcode))
              {
                if (startsWith("M23 SD:", infoCmd.queue[infoCmd.index_r].gcode))
                  infoFile.source = TFT_SD;
                else
                  infoFile.source = TFT_UDISK;
                resetInfoFile();
                strncpy(infoFile.title, &infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 4], MAX_PATH_LEN);
                
                for (int i = 0; i < MAX_PATH_LEN && infoFile.title[i] != 0 ; i++)
                {
                  if ((infoFile.title[i] == '*') || (infoFile.title[i] == '\n') ||(infoFile.title[i] == '\r'))
                  {
                    infoFile.title[i] = 0;
                    break;
                  }
                }
                Serial_Puts(SERIAL_PORT_2, "echo:Now fresh file: ");
                Serial_Puts(SERIAL_PORT_2, infoFile.title);
                Serial_Puts(SERIAL_PORT_2, "\n");
                FIL tmp;
                if (mountFS() && (f_open(&tmp, infoFile.title, FA_OPEN_EXISTING | FA_READ) == FR_OK))
                {
                  char buf[10];
                  sprintf(buf, "%d", f_size(&tmp));
                  Serial_Puts(SERIAL_PORT_2, "File opened: ");
                  Serial_Puts(SERIAL_PORT_2, infoFile.title);
                  Serial_Puts(SERIAL_PORT_2, " Size: ");
                  Serial_Puts(SERIAL_PORT_2, buf);
                  Serial_Puts(SERIAL_PORT_2, "\nFile selected\n");
                  f_close(&tmp);
                }
                else
                {
                  Serial_Puts(SERIAL_PORT_2, "open failed, File: ");
                  Serial_Puts(SERIAL_PORT_2, infoFile.title);
                  Serial_Puts(SERIAL_PORT_2, "\n");
                }
                Serial_Puts(SERIAL_PORT_2, "ok\n");
                purgeLastCmd();
                return;
              }
            }
            break;

          case 24:  
            if (!fromTFT)
            {
              if ((infoFile.source == TFT_UDISK) || (infoFile.source == TFT_SD))
              {
                if (isPause())
                {
                  setPrintPause(false, false);
                }
                else
                {
                  Serial_Puts(SERIAL_PORT_2, "ok\n");
                  purgeLastCmd();
                  infoMenu.cur = 1;
                  menuBeforePrinting();
                }
                return;
              }
            }
            break;

          case 25:  
            if (!fromTFT)
            {
              if (isPrinting() && !infoHost.printing)
              {
                setPrintPause(true, false);
                Serial_Puts(SERIAL_PORT_2, "ok\n");
                purgeLastCmd();
                return;
              }
            }
            break;

          case 27:  
            if (!fromTFT)
            {
              if (isPrinting() && !infoHost.printing)
              {
                if (cmd_seen('C'))
                {
                  Serial_Puts(SERIAL_PORT_2, "Current file: ");
                  Serial_Puts(SERIAL_PORT_2, infoFile.title);
                  Serial_Puts(SERIAL_PORT_2, ".\n");
                }
                char buf[55];
                sprintf(buf, "%s printing byte %d/%d\n", (infoFile.source == TFT_SD) ? "TFT SD" : "TFT USB", getPrintCur(), getPrintSize());
                Serial_Puts(SERIAL_PORT_2, buf);
                Serial_Puts(SERIAL_PORT_2, "ok\n");
                purgeLastCmd();
                return;
              }
            }
            else
            {
              printSetUpdateWaiting(false);
            }
            break;

          case 28:  
            if (!fromTFT)
              ispolling = false;
            break;

          case 29:  
            if (!fromTFT)
            {
              mustStoreScript("M105\nM114\nM220\n");
              storeCmd("M221 D%d\n", heatGetCurrentTool());
              ispolling = true;
            }
            break;

          case 30:  
            if (!fromTFT)
            {
              if (startsWith("M30 SD:", infoCmd.queue[infoCmd.index_r].gcode) ||
                  startsWith("M30 U:", infoCmd.queue[infoCmd.index_r].gcode))
              {
                if (startsWith("M30 SD:", infoCmd.queue[infoCmd.index_r].gcode)) infoFile.source = TFT_SD;
                else infoFile.source = TFT_UDISK;
                TCHAR filepath[MAX_PATH_LEN];
                strncpy(filepath, &infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 4], MAX_PATH_LEN);
                
                for (int i = 0; i < MAX_PATH_LEN && filepath[i] != 0 ; i++)
                {
                  if ((filepath[i] == '*') || (filepath[i] == '\n') || (filepath[i] == '\r'))
                  {
                    filepath[i] = 0;
                    break;
                  }
                }
                if ((mountFS() == true) && (f_unlink (filepath) == FR_OK))
                {
                  Serial_Puts(SERIAL_PORT_2, "File deleted: ");
                  Serial_Puts(SERIAL_PORT_2, filepath);
                  Serial_Puts(SERIAL_PORT_2, ".\nok\n");
                }
                else
                {
                  Serial_Puts(SERIAL_PORT_2, "Deletion failed, File: ");
                  Serial_Puts(SERIAL_PORT_2, filepath);
                  Serial_Puts(SERIAL_PORT_2, ".\nok\n");
                }
                purgeLastCmd();
                return;
              }
            }
            break;

          case 115:  
            if (!fromTFT && startsWith("M115 TFT", infoCmd.queue[infoCmd.index_r].gcode))
            {
              char buf[50];
              Serial_Puts(SERIAL_PORT_2,
                          "FIRMWARE_NAME: " FIRMWARE_NAME
                          " SOURCE_CODE_URL:https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware\n");
              sprintf(buf, "Cap:HOTEND_NUM:%d\n", infoSettings.hotend_count);
              Serial_Puts(SERIAL_PORT_2, buf);
              sprintf(buf, "Cap:EXTRUDER_NUM:%d\n", infoSettings.ext_count);
              Serial_Puts(SERIAL_PORT_2, buf);
              sprintf(buf, "Cap:FAN_NUM:%d\n", infoSettings.fan_count);
              Serial_Puts(SERIAL_PORT_2, buf);
              sprintf(buf, "Cap:FAN_CTRL_NUM:%d\n", infoSettings.fan_ctrl_count);
              Serial_Puts(SERIAL_PORT_2, buf);
              Serial_Puts(SERIAL_PORT_2, "ok\n");
              purgeLastCmd();
              return;
            }
            break;

          case 125:  
            if (!fromTFT)
            {
              if (isPrinting() && !infoHost.printing)
              {
                setPrintPause(true, false);
                Serial_Puts(SERIAL_PORT_2, "ok\n");
                purgeLastCmd();
                return;
              }
            }
            break;

          case 524:  
            if (!fromTFT && isPrinting() && !infoHost.printing)
            {
              abortPrinting();
              Serial_Puts(SERIAL_PORT_2, "ok\n");
              purgeLastCmd();
              return;
            }
            break;

        #else  
          case 27:  
            printSetUpdateWaiting(false);
            break;
        #endif  

        case 80:  
          #ifdef PS_ON_PIN
            PS_ON_On();
          #endif
          break;

        case 81:  
          #ifdef PS_ON_PIN
            PS_ON_Off();
          #endif
          break;

        case 82:  
          eSetRelative(false);
          break;

        case 83:  
          eSetRelative(true);
          break;

        case 92:  
        {
          if (cmd_seen('X')) setParameter(P_STEPS_PER_MM, X_AXIS, cmd_float());
          if (cmd_seen('Y')) setParameter(P_STEPS_PER_MM, Y_AXIS, cmd_float());
          if (cmd_seen('Z')) setParameter(P_STEPS_PER_MM, Z_AXIS, cmd_float());
          uint8_t i = 0;
          if (cmd_seen('T')) i = cmd_value();
          if (cmd_seen('E')) setParameter(P_STEPS_PER_MM, E_STEPPER + i, cmd_float());
          break;
        }

        case 105:  
          if (fromTFT)
          {
            heatSetUpdateWaiting(false);
            avoid_terminal = !infoSettings.terminalACK;
          }
          break;

        case 155:  
          if (fromTFT)
          {
            heatSetUpdateWaiting(false);
            if (cmd_seen('S'))
            {
              heatSyncUpdateSeconds(cmd_value());
            }
            else if (!cmd_seen('\n'))
            {
              char buf[12];
              sprintf(buf, "S%u\n", heatGetUpdateSeconds());
              strcat(infoCmd.queue[infoCmd.index_r].gcode, (const char*)buf);
            }
          }
          break;

        case 106:  
        {
          uint8_t i = cmd_seen('P') ? cmd_value() : 0;
          if (cmd_seen('S') && fanIsType(i, FAN_TYPE_F))
          {
            fanSetCurSpeed(i, cmd_value());
          }
          else if (!cmd_seen('\n'))
          {
            char buf[12];
            sprintf(buf, "S%u\n", fanGetCurSpeed(i));
            strcat(infoCmd.queue[infoCmd.index_r].gcode, (const char*)buf);
          }
          break;
        }

        case 107:  
        {
          uint8_t i = cmd_seen('P') ? cmd_value() : 0;
          if (fanIsType(i, FAN_TYPE_F)) fanSetCurSpeed(i, 0);
          break;
        }

        case 710:  
        {
          uint8_t i = 0;
          if (cmd_seen('S'))
          {
            i = fanGetTypID(i, FAN_TYPE_CTRL_S);
            fanSetCurSpeed(i, cmd_value());
          }
          if (cmd_seen('I'))
          {
            i = fanGetTypID(0, FAN_TYPE_CTRL_I);
            fanSetCurSpeed(i, cmd_value());
          }
          break;
        }

        case 109: 
          if (fromTFT)
          {
            infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 3] = '4';  
            uint8_t i = cmd_seen('T') ? cmd_value() : heatGetCurrentHotend();
            if (cmd_seen('R'))
            {
              infoCmd.queue[infoCmd.index_r].gcode[cmd_index - 1] = 'S';
              heatSetIsWaiting(i, WAIT_COOLING_HEATING);
            }
            else
            {
              heatSetIsWaiting(i, WAIT_HEATING);
            }
          }
        
        case 104: 
          if (fromTFT)
          {
            uint8_t i = cmd_seen('T') ? cmd_value() : heatGetCurrentHotend();
            if (cmd_seen('S'))
            {
              heatSyncTargetTemp(i, cmd_value());
            }
            else if (!cmd_seen('\n'))
            {
              char buf[12];
              sprintf(buf, "S%u\n", heatGetTargetTemp(i));
              strcat(infoCmd.queue[infoCmd.index_r].gcode,(const char*)buf);
              heatSetSendWaiting(i, false);
            }
          }
          break;

        case 114:  
          #ifdef FIL_RUNOUT_PIN
            if (fromTFT)
              positionSetUpdateWaiting(false);
          #endif
          break;

        case 117:  
        {
          char message[CMD_MAX_CHAR];
          strncpy(message, &infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 4], CMD_MAX_CHAR);
          
          for (int i = 0; i < CMD_MAX_CHAR && message[i] != 0 ; i++)
          {
            if (message[i] == '*')
            {
              message[i] = 0;
              break;
            }
          }
          statusScreen_setMsg((uint8_t *)"M117", (uint8_t *)&message);
          if (infoMenu.menu[infoMenu.cur] != menuStatus)
          {
            addToast(DIALOG_TYPE_INFO, message);
          }
        }
        break;

        case 190:  
          if (fromTFT)
          {
            infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 2] = '4';  
            if (cmd_seen('R'))
            {
              infoCmd.queue[infoCmd.index_r].gcode[cmd_index - 1] = 'S';
              heatSetIsWaiting(BED, WAIT_COOLING_HEATING);
            }
            else
            {
              heatSetIsWaiting(BED, WAIT_HEATING);
            }
          }
        
        case 140:  
          if (fromTFT)
          {
            if (cmd_seen('S'))
            {
              heatSyncTargetTemp(BED, cmd_value());
            }
            else if (!cmd_seen('\n'))
            {
              char buf[12];
              sprintf(buf, "S%u\n", heatGetTargetTemp(BED));
              strcat(infoCmd.queue[infoCmd.index_r].gcode, (const char *)buf);
              heatSetSendWaiting(BED, false);
            }
          }
          break;

        case 191:  
          if (fromTFT)
          {
            infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 2] = '4';  
            if (cmd_seen('R'))
            {
              infoCmd.queue[infoCmd.index_r].gcode[cmd_index - 1] = 'S';
              heatSetIsWaiting(CHAMBER, WAIT_COOLING_HEATING);
            }
            else
            {
              heatSetIsWaiting(CHAMBER, WAIT_HEATING);
            }
          }
        
        case 141:  
          if (fromTFT)
          {
            if (cmd_seen('S'))
            {
              heatSyncTargetTemp(CHAMBER, cmd_value());
            }
            else if (!cmd_seen('\n'))
            {
              char buf[12];
              sprintf(buf, "S%u\n", heatGetTargetTemp(CHAMBER));
              strcat(infoCmd.queue[infoCmd.index_r].gcode, (const char *)buf);
              heatSetSendWaiting(CHAMBER, false);
            }
          }
          break;

        case 200:  
        {
          if (cmd_seen('S')) setParameter(P_FILAMENT_SETTING, 0, cmd_float());
          uint8_t i = 0;
          if (cmd_seen('T')) i = cmd_value();
          if (cmd_seen('D')) setParameter(P_FILAMENT_SETTING, 1 + i, cmd_float());
          break;
        }

        case 201:  
        {
          if (cmd_seen('X')) setParameter(P_MAX_ACCELERATION, X_STEPPER, cmd_float());
          if (cmd_seen('Y')) setParameter(P_MAX_ACCELERATION, Y_STEPPER, cmd_float());
          if (cmd_seen('Z')) setParameter(P_MAX_ACCELERATION, Z_STEPPER, cmd_float());
          uint8_t i = 0;
          if (cmd_seen('T')) i = cmd_value();
          if (cmd_seen('E')) setParameter(P_MAX_ACCELERATION, E_STEPPER + i, cmd_float());
          break;
        }

        case 203:  
        {
          if (cmd_seen('X')) setParameter(P_MAX_FEED_RATE, X_STEPPER, cmd_float());
          if (cmd_seen('Y')) setParameter(P_MAX_FEED_RATE, Y_STEPPER, cmd_float());
          if (cmd_seen('Z')) setParameter(P_MAX_FEED_RATE, Z_STEPPER, cmd_float());
          uint8_t i = 0;
          if (cmd_seen('T')) i = cmd_value();
          if (cmd_seen('E')) setParameter(P_MAX_FEED_RATE, E_STEPPER + i, cmd_float());
          break;
        }

        case 204:  
          if (cmd_seen('P')) setParameter(P_ACCELERATION, 0, cmd_float());
          if (cmd_seen('R')) setParameter(P_ACCELERATION, 1, cmd_float());
          if (cmd_seen('T')) setParameter(P_ACCELERATION, 2, cmd_float());
          break;

        case 205:  
          if (cmd_seen('X')) setParameter(P_JERK, X_AXIS, cmd_float());
          if (cmd_seen('Y')) setParameter(P_JERK, Y_AXIS, cmd_float());
          if (cmd_seen('Z')) setParameter(P_JERK, Z_AXIS, cmd_float());
          if (cmd_seen('E')) setParameter(P_JERK, E_AXIS, cmd_float());
          if (cmd_seen('J')) setParameter(P_JUNCTION_DEVIATION, 0, cmd_float());
          break;

        case 206:  
          if (cmd_seen('X')) setParameter(P_HOME_OFFSET, X_AXIS, cmd_float());
          if (cmd_seen('Y')) setParameter(P_HOME_OFFSET, Y_AXIS, cmd_float());
          if (cmd_seen('Z')) setParameter(P_HOME_OFFSET, Z_AXIS, cmd_float());
          break;

        case 207:  
          if (cmd_seen('S')) setParameter(P_FWRETRACT, 0, cmd_float());
          if (cmd_seen('W')) setParameter(P_FWRETRACT, 1, cmd_float());
          if (cmd_seen('F')) setParameter(P_FWRETRACT, 2, cmd_float());
          if (cmd_seen('Z')) setParameter(P_FWRETRACT, 3, cmd_float());
          break;

        case 208:  
          if (cmd_seen('S')) setParameter(P_FWRECOVER, 0, cmd_float());
          if (cmd_seen('W')) setParameter(P_FWRECOVER, 1, cmd_float());
          if (cmd_seen('F')) setParameter(P_FWRECOVER, 2, cmd_float());
          if (cmd_seen('R')) setParameter(P_FWRECOVER, 3, cmd_float());
          break;

        case 218:  
          if (cmd_seen('X')) setParameter(P_HOTEND_OFFSET, X_AXIS, cmd_float());
          if (cmd_seen('Y')) setParameter(P_HOTEND_OFFSET, Y_AXIS, cmd_float());
          if (cmd_seen('Z')) setParameter(P_HOTEND_OFFSET, Z_AXIS, cmd_float());
          break;

        case 220:  
          if (cmd_seen('S'))
            speedSetCurPercent(0, cmd_value());
          break;

        case 221:  
          if (cmd_seen('S'))
            speedSetCurPercent(1, cmd_value());
          break;

        #ifdef BUZZER_PIN
          case 300:  
            if (cmd_seen('S'))
            {
              uint16_t hz = cmd_value();
              if (cmd_seen('P'))
              {
                uint16_t ms = cmd_value();
                Buzzer_TurnOn(hz, ms);
                if (!fromTFT && startsWith("M300 TFT", infoCmd.queue[infoCmd.index_r].gcode))
                {
                  purgeLastCmd();
                  return;
                }
              }
            }
            break;
        #endif

        case 355:  
        {
          if (cmd_seen('S'))
          {
            caseLightSetState(cmd_value() > 0);
            caseLightSendWaiting(false);
          }
          if (cmd_seen('P'))
          {
            caseLightSetBrightness(cmd_value());
            caseLightSendWaiting(false);
          }
          break;
        }

        case 420:  
          
          
          if (cmd_seen('Z')) setParameter(P_ABL_STATE, 1, cmd_float());
          break;

        case 596:  
        {
          uint8_t k = 0;
          if(cmd_seen('S')) k = cmd_value();
          if(cmd_seen('X')) setParameter(P_STEALTH_CHOP, X_STEPPER, k);
          if(cmd_seen('Y')) setParameter(P_STEALTH_CHOP, Y_STEPPER, k);
          if(cmd_seen('Z')) setParameter(P_STEALTH_CHOP, Z_STEPPER, k);
          uint8_t i = 0;
          if(cmd_seen('T')) i = cmd_value();
          if(cmd_seen('E')) setParameter(P_STEALTH_CHOP, E_STEPPER + i, k);
          break;
        }

        case 600:  
          if (infoSettings.emulate_m600 == 1 && isPrinting())
          {
            setPrintPause(true, false);
            
            purgeLastCmd();
            return;
          }
          break;

        #ifdef NOZZLE_PAUSE_M601
          case 601:  
            if (isPrinting())
            {
              setPrintPause(true, false);
              
              purgeLastCmd();
              return;
            }
            break;
        #endif

        #ifdef LOAD_UNLOAD_M701_M702
          case 701:  
          case 702:  
            infoHost.wait = true;
            break;
        #endif

        case 851:  
        {
          if (cmd_seen('X')) setParameter(P_PROBE_OFFSET, X_AXIS, cmd_float());
          if (cmd_seen('Y')) setParameter(P_PROBE_OFFSET, Y_AXIS, cmd_float());
          if (cmd_seen('Z')) setParameter(P_PROBE_OFFSET, Z_AXIS, cmd_float());
          break;
        }

        case 900:  
        {
          uint8_t i = 0;
          if (cmd_seen('T')) i = cmd_value();
          if (cmd_seen('K')) setParameter(P_LIN_ADV, i, cmd_float());
          break;
        }

        case 906:  
        {
          if (cmd_seen('X')) setParameter(P_CURRENT, X_AXIS, cmd_value());
          if (cmd_seen('Y')) setParameter(P_CURRENT, Y_AXIS, cmd_value());
          if (cmd_seen('Z')) setParameter(P_CURRENT, Z_AXIS, cmd_value());
          if (cmd_seen('I'))
          {
            if (cmd_seen('X')) setDualStepperStatus(X_STEPPER,true);
            if (cmd_seen('Y')) setDualStepperStatus(Y_STEPPER,true);
            if (cmd_seen('Z')) setDualStepperStatus(Z_STEPPER,true);
          }
          uint8_t i = 0;
          if (cmd_seen('T')) i = cmd_value();
          if (cmd_seen('E')) setParameter(P_CURRENT, E_STEPPER + i, cmd_value());
          break;
        }

        case 913:  
        {
          if (cmd_seen('X')) setParameter(P_HYBRID_THRESHOLD, X_STEPPER, cmd_value());
          if (cmd_seen('Y')) setParameter(P_HYBRID_THRESHOLD, Y_STEPPER, cmd_value());
          if (cmd_seen('Z')) setParameter(P_HYBRID_THRESHOLD, Z_STEPPER, cmd_value());
          uint8_t i = 0;
          if (cmd_seen('T')) i = cmd_value();
          if (cmd_seen('E')) setParameter(P_HYBRID_THRESHOLD, E_STEPPER + i, cmd_value());
          break;
        }

        case 914:  
          if (cmd_seen('X')) setParameter(P_BUMPSENSITIVITY, X_STEPPER, cmd_value());
          if (cmd_seen('Y')) setParameter(P_BUMPSENSITIVITY, Y_STEPPER, cmd_value());
          if (cmd_seen('Z')) setParameter(P_BUMPSENSITIVITY, Z_STEPPER, cmd_value());
          break;
      }
      break;  

    case 'G':
      cmd = strtol(&infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 1], NULL, 10);
      switch (cmd)
      {
        case 0:  
        case 1:  
        case 2:  
        case 3:  
        {
          AXIS i;
          for (i = X_AXIS; i < TOTAL_AXIS; i++)
          {
            if (cmd_seen(axis_id[i]))
            {
              coordinateSetAxisTarget(i, cmd_float());
            }
          }
          if (cmd_seen('F'))
          {
            coordinateSetFeedRate(cmd_value());
          }
          break;
        }

        case 28:  
          coordinateSetKnown(true);
          babystepReset();
          storeCmd("M503 S0\n");
          break;

        #if ENABLE_BL_VALUE > 0  
          case 29:  
          {
            if (infoMachineSettings.firmwareType != FW_REPRAPFW)
            {
              if (cmd_seen('A'))
              {
                setParameter(P_ABL_STATE, 0, 1);
                storeCmd("M117 UBL active\n");
              }
              if (cmd_seen('D'))
              {
                setParameter(P_ABL_STATE, 0, 0);
                storeCmd("M117 UBL inactive\n");
              }
            }
            else
            {
              if (cmd_seen('S'))
              {
                uint8_t v = cmd_value();
                if (v == 1)
                {
                  setParameter(P_ABL_STATE, 0, 1);
                }
                else if (v == 2)
                {
                  setParameter(P_ABL_STATE, 0, 0);
                }
              }
            }
          }
          break;
        #endif

        case 90:  
          coorSetRelative(false);
          break;

        case 91:  
          coorSetRelative(true);
          break;

        case 92:  
        {
          bool coorRelative = coorGetRelative();
          bool eRelative = eGetRelative();
          
          coorSetRelative(false);
          eSetRelative(false);
          for (AXIS i = X_AXIS; i < TOTAL_AXIS; i++)
          {
            if (cmd_seen(axis_id[i]))
            {
              coordinateSetAxisTarget(i, cmd_float());
              #ifdef FIL_RUNOUT_PIN
                if (i == E_AXIS)
                {
                  
                  FIL_SFS_SetAlive(true);
                }
              #endif
            }
          }
          
          coorSetRelative(coorRelative);
          eSetRelative(eRelative);
          break;
        }
      }
      break;  

    case 'T':
      cmd = strtol(&infoCmd.queue[infoCmd.index_r].gcode[cmd_index + 1], NULL, 10);
      heatSetCurrentTool(cmd);
      break;

  }  

  setCurrentAckSrc(infoCmd.queue[infoCmd.index_r].src);
  Serial_Puts(SERIAL_PORT, infoCmd.queue[infoCmd.index_r].gcode);

  if (avoid_terminal != true)
    terminalCache(infoCmd.queue[infoCmd.index_r].gcode, TERMINAL_GCODE);

  purgeLastCmd();

  infoHost.wait = infoHost.connected;
}  
