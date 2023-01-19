#include "MBL.h"
#include "includes.h"

static uint8_t curUnit_index = 0;
uint8_t mblPoint = 0;
bool mblRunning = false;


static inline void mblStart(void)
{
  mblRunning = true;
  mblPoint = 0;

  probeHeightEnable();  

  
  mustStoreCmd("G29 S1\n");                            
  probeHeightStart(infoSettings.level_z_raise, true);  
  probeHeightRelative();                               
}


static inline void mblStop(void)
{
  mblRunning = false;

  if (infoMachineSettings.zProbe == ENABLED)
    probeHeightStop(infoSettings.level_z_raise);  

  probeHeightAbsolute();  

  probeHeightDisable();  
}


static inline void mblAbort(void)
{
  
  mustStoreCmd("G29 S5\n");  

  mblStop();

  BUZZER_PLAY(sound_error);

  popupReminder(DIALOG_TYPE_ERROR, LABEL_MBL_SETTINGS, LABEL_PROCESS_ABORTED);
}


void mblUpdateStatus(bool succeeded)
{
  mblStop();

  if (succeeded)  
  {
    BUZZER_PLAY(sound_success);

    LABELCHAR(tempMsg, LABEL_BL_COMPLETE);

    if (infoMachineSettings.EEPROM == 1)
    {
      sprintf(&tempMsg[strlen(tempMsg)], "\n %s", textSelect(LABEL_EEPROM_SAVE_INFO));

      setDialogText(LABEL_MBL_SETTINGS, (uint8_t *) tempMsg, LABEL_CONFIRM, LABEL_CANCEL);
      showDialog(DIALOG_TYPE_SUCCESS, saveEepromSettings, NULL, NULL);
    }
    else
    {
      popupReminder(DIALOG_TYPE_SUCCESS, LABEL_MBL_SETTINGS, (uint8_t *) tempMsg);
    }
  }
  else  
  {
    BUZZER_PLAY(sound_error);

    popupReminder(DIALOG_TYPE_ERROR, LABEL_MBL_SETTINGS, LABEL_PROCESS_ABORTED);
  }
}


void mblNotifyError(bool isStarted)
{
  LABELCHAR(tempMsg, LABEL_MBL);

  if (!isStarted)
    sprintf(&tempMsg[strlen(tempMsg)], " %s", textSelect(LABEL_OFF));
  else
    sprintf(&tempMsg[strlen(tempMsg)], " %s", textSelect(LABEL_ON));

  addToast(DIALOG_TYPE_ERROR, tempMsg);
}

void mblDrawHeader(uint8_t *point)
{
  char tempstr[20];

  if (point != NULL)
  {
    sprintf(tempstr, "P%-4d", *point);

    GUI_SetColor(infoSettings.sd_reminder_color);
  }
  else
  {
    sprintf(tempstr, "%-15s", textSelect(LABEL_OFF));

    GUI_SetColor(infoSettings.reminder_color);
  }

  GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *) tempstr);

  GUI_SetColor(infoSettings.font_color);
  setLargeFont(true);
  GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *)"mm");
  setLargeFont(false);
}

void mblDrawValue(float val)
{
  char tempstr[20];

  sprintf(tempstr, "  %.2f  ", val);

  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *) tempstr);
  setLargeFont(false);
}

void menuMBL(void)
{
  
  MENUITEMS mblItems = {
    
    LABEL_MBL_SETTINGS,
    
    {
      {ICON_DEC,                     LABEL_DEC},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_INC,                     LABEL_INC},
      {ICON_001_MM,                  LABEL_001_MM},
      {ICON_RESET_VALUE,             LABEL_RESET},
      {ICON_RESUME,                  LABEL_START},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  #ifdef FRIENDLY_Z_OFFSET_LANGUAGE
    mblItems.items[0].icon = ICON_NOZZLE_DOWN;
    mblItems.items[0].label.index = LABEL_DOWN;
    mblItems.items[3].icon = ICON_NOZZLE_UP;
    mblItems.items[3].label.index = LABEL_UP;
  #endif

  KEY_VALUES key_num = KEY_IDLE;
  float now, curValue;
  float unit;

  now = curValue = coordinateGetAxisActual(Z_AXIS);

  mblItems.items[KEY_ICON_4] = itemMoveLen[curUnit_index];

  if (mblRunning)
  {
    #ifdef WUXN_CODE
    mblItems.items[KEY_ICON_6].icon = ICON_BACKGROUND;
    #else
    mblItems.items[KEY_ICON_6].icon = ICON_PAGE_DOWN;
    #endif
    mblItems.items[KEY_ICON_6].label.index = LABEL_NEXT;
  }

  menuDrawPage(&mblItems);
  mblDrawHeader(!mblRunning ? NULL : &mblPoint);
  mblDrawValue(now);

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuMBL)
  {
    unit = moveLenSteps[curUnit_index];

    curValue = coordinateGetAxisActual(Z_AXIS);

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      
      case KEY_ICON_0:
        if (!mblRunning)
          mblNotifyError(false);
        else
          probeHeightMove(unit, -1);
        break;

      case KEY_INFOBOX:
        if (mblRunning)
          mblNotifyError(true);
        else
          infoMenu.menu[++infoMenu.cur] = menuUnifiedHeat;
        break;

      
      case KEY_ICON_3:
        if (!mblRunning)
          mblNotifyError(false);
        else
          probeHeightMove(unit, 1);
        break;

      
      case KEY_ICON_4:
        curUnit_index = (curUnit_index + 1) % ITEM_FINE_MOVE_LEN_NUM;

        mblItems.items[key_num] = itemMoveLen[curUnit_index];

        menuDrawItem(&mblItems.items[key_num], key_num);
        break;

      
      case KEY_ICON_5:
        if (!mblRunning)
          mblNotifyError(false);
        else
          probeHeightMove(curValue, -1);
        break;

      
      case KEY_ICON_6:
        if (!mblRunning)
        { 
          mblStart();

          #ifdef WUXN_CODE
          mblItems.items[key_num].icon = ICON_BACKGROUND;
          #else
          mblItems.items[key_num].icon = ICON_PAGE_DOWN;
          #endif
          mblItems.items[key_num].label.index = LABEL_NEXT;

          menuDrawItem(&mblItems.items[key_num], key_num);

          ++mblPoint;
          mblDrawHeader(&mblPoint);
        }
        else
        {
          storeCmd("G29 S2\n");  

          probeHeightStart(infoSettings.level_z_raise, true);  

          ++mblPoint;
          mblDrawHeader(&mblPoint);
        }
        break;

      case KEY_ICON_7:
        if (mblRunning)
          mblAbort();

        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            if (!mblRunning)
              mblNotifyError(false);
            else
              probeHeightMove(unit, encoderPosition > 0 ? 1 : -1);

            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (now != curValue)
    {
      now = curValue;
      mblDrawValue(now);
    }

    probeHeightQueryCoord();

    loopProcess();
  }
}