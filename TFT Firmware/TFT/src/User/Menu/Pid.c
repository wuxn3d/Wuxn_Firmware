#include "Pid.h"
#include "includes.h"



extern bool gbProbeTestStart;

MENUITEMS pidWaitItems = {
  
  LABEL_PID_TITLE,
  
  {
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
  }
};

const char *const pidCmd[] = PID_CMD;
static int16_t pidHeaterTarget[MAX_HEATER_COUNT] = {0};
static uint8_t curToolIndex = NOZZLE0;
static uint8_t degreeSteps_index = 1;
u32 pidTimeout = 0;
uint8_t pidCounter = 0;
bool pidSucceeded = false;
bool pidRunning = false;
bool pidInitialized = false;

bool  bPidHotendDialogFlag = FALSE;
bool  bPidHotbedDialogFlag = FALSE;  

static void wPidStartupDialog(void);
static void wPidStartupDialogUserSaidYes(void);
static void wPidStartMenuDialog(void);
static void wPidStartMenuDialogUserSaidYes(void);
static void wDisplayAutotuneWindow(uint8_t);
static void wSaveEepromSettingsUserSaidNo(void);
static inline void wPidStartTarget(uint8_t);


void pidUpdateStatus(bool succeeded)
{
  if (pidCounter > 0)
    pidCounter--;

  if (!succeeded)  
    pidSucceeded = false;

  if (pidCounter > 0)  
  {
    #ifdef ENABLE_PID_STATUS_UPDATE_NOTIFICATION
      LABELCHAR(tempMsg, LABEL_PID_TITLE);

      if (succeeded)
      {
        sprintf(&tempMsg[strlen(tempMsg)], " %s", textSelect(LABEL_PROCESS_COMPLETED));
        BUZZER_PLAY(sound_notify);
        addToast(DIALOG_TYPE_INFO, tempMsg);
      }
      else
      {
        sprintf(&tempMsg[strlen(tempMsg)], " %s", textSelect(LABEL_PROCESS_ABORTED));
        BUZZER_PLAY(sound_error);
        addToast(DIALOG_TYPE_ERROR, tempMsg);
      }
    #endif
  }
  else  
  {
    pidRunning = false;

    if (pidSucceeded)  
    {
      BUZZER_PLAY(sound_success);

      LABELCHAR(tempMsg, LABEL_PROCESS_COMPLETED);

      if (infoMachineSettings.EEPROM == 1)
      {
        sprintf(&tempMsg[strlen(tempMsg)], "\n %s", textSelect(LABEL_EEPROM_SAVE_INFO));
        
        if(curToolIndex == NOZZLE0)
        {
          setDialogText(LABEL_HOTEND_PID_AUTOTUNE, (uint8_t *) tempMsg, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_SUCCESS, wSaveEepromSettingsUserSaidYes, wSaveEepromSettingsUserSaidNo, NULL);
        } 
        else if(curToolIndex == BED)
        {
          setDialogText(LABEL_HOTBED_PID_AUTOTUNE, (uint8_t *) tempMsg, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_SUCCESS, wSaveEepromSettingsUserSaidYes, wSaveEepromSettingsUserSaidNo, NULL);
        }
        
      }
      else
      {
        popupReminder(DIALOG_TYPE_SUCCESS, LABEL_PID_TITLE, (uint8_t *) tempMsg);
      }
    }
    else  
    {
      BUZZER_PLAY(sound_error);

      popupReminder(DIALOG_TYPE_ERROR, LABEL_PID_TITLE, LABEL_PROCESS_ABORTED);
    }
  }
}

static inline void pidCheckTimeout(void)
{
  if (pidRunning)
  {
    if (OS_GetTimeMs() > pidTimeout)
    {
      pidRunning = false;


      LABELCHAR(tempMsg, LABEL_TIMEOUT_REACHED);

      sprintf(&tempMsg[strlen(tempMsg)], "\n %s", textSelect(LABEL_PROCESS_ABORTED));
      BUZZER_PLAY(sound_error);
      popupReminder(DIALOG_TYPE_ERROR, LABEL_PID_TITLE, (uint8_t *) tempMsg);
    }
  }
}

static inline void pidUpdateCounter(void)
{
  pidCounter = 0;

  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)  
  {
    if (pidHeaterTarget[i] > 0)
      pidCounter++;
  }
}

void menuPidWait(void)
{
  bool isPressed = false;
  bool isReleased = false;

  
  if(curToolIndex == NOZZLE0)
  { 
    pidWaitItems.title.index = LABEL_HOTEND_PID_AUTOTUNE;
  }
  else if(curToolIndex == BED)
  {
    pidWaitItems.title.index = LABEL_HOTBED_PID_AUTOTUNE;
  }

  menuDrawPage(&pidWaitItems);

  setLargeFont(false);
  GUI_SetColor(WHITE);
  GUI_DispString(TITLE_END_Y/3, LIST_ICON_HEIGHT,LABEL_NOT_TOUCH_SCREEN);
  GUI_SetColor(RED);
  GUI_DispString(SMALLICON_HEIGHT-10, LIST_ICON_HEIGHT, LABEL_NOT);
  GUI_SetColor(WHITE);
  wDisplayAutotuneWindow(curToolIndex);

  while (infoMenu.menu[infoMenu.cur] == menuPidWait)
  {
    if (!isPressed)  
    {
      if (isPress())  
        isPressed = true;
    }
    else if (!isPress())  
    {
      isReleased = true;
    }

    if (isReleased)
      infoMenu.cur--;

    pidCheckTimeout();

    loopProcess();
  }
}

static inline void pidStart(void)
{
  pidRunning = true;
  pidSucceeded = true;

  pidUpdateCounter();  
  pidTimeout = OS_GetTimeMs() + PID_PROCESS_TIMEOUT;  

  mustStoreCmd("M150 R255 U0 B0\n");  
  mustStoreCmd("M106 S255\n");        
  mustStoreCmd("G4 S1\n");            

  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)  
  {
    if (pidHeaterTarget[i] > 0)
    {
      mustStoreCmd("%s S%d\n", pidCmd[i], (int)pidHeaterTarget[i]);  
      mustStoreCmd("G4 S1\n");                                       
    }
  }

  mustStoreCmd("M107\n");             
  mustStoreCmd("M150 R0 U255 B0\n");  

  infoMenu.menu[++infoMenu.cur] = menuPidWait;
}

void pidTemperatureReDraw(bool skip_header)
{
  char tempstr[20];

  setLargeFont(true);

  if (!skip_header)
  {
    sprintf(tempstr, "%-15s", heatDisplayID[curToolIndex]);
    setLargeFont(false);
    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *) tempstr);
    setLargeFont(true);
    GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *)"ºC");
  }

  sprintf(tempstr, "  %d  ", pidHeaterTarget[curToolIndex]);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *) tempstr);
  setLargeFont(false);
}

void menuPid(void)
{
  
  MENUITEMS pidItems = {
    
    LABEL_PID_TITLE,
    
    {
      {ICON_DEC,                     LABEL_DEC},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_INC,                     LABEL_INC},
      {ICON_NOZZLE,                  LABEL_NOZZLE},
      {ICON_5_DEGREE,                LABEL_5_DEGREE},
      {ICON_RESUME,                  LABEL_START},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  if (!pidInitialized)  
  {                     
    for (uint8_t i = NOZZLE0; i < MAX_HEATER_COUNT; i++)
    {
      pidHeaterTarget[i] = 0;
    }

    pidInitialized = true;
  }

  pidItems.items[KEY_ICON_4] = itemTool[curToolIndex];
  pidItems.items[KEY_ICON_5] = itemDegreeSteps[degreeSteps_index];

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  menuDrawPage(&pidItems);
  pidTemperatureReDraw(false);

  wPidStartupDialog();

  while (infoMenu.menu[infoMenu.cur] == menuPid)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        if (pidHeaterTarget[curToolIndex] > 0)
          pidHeaterTarget[curToolIndex] =
              NOBEYOND(0, pidHeaterTarget[curToolIndex] - degreeSteps[degreeSteps_index],
                       infoSettings.max_temp[curToolIndex]);

        pidTemperatureReDraw(true);
        break;

      case KEY_INFOBOX:
      {
        char titlestr[30];
        sprintf(titlestr, "Min:0 | Max:%i", infoSettings.max_temp[curToolIndex]);

        int32_t val = numPadInt((uint8_t *) titlestr, pidHeaterTarget[curToolIndex], 0, false);
        val = NOBEYOND(0, val, infoSettings.max_temp[curToolIndex]);

        if (val != pidHeaterTarget[curToolIndex])  
          pidHeaterTarget[curToolIndex] = val;

        menuDrawPage(&pidItems);
        pidTemperatureReDraw(false);
        break;
      }

      case KEY_ICON_3:
        if (pidHeaterTarget[curToolIndex] < infoSettings.max_temp[curToolIndex])
          pidHeaterTarget[curToolIndex] =
              NOBEYOND(0, pidHeaterTarget[curToolIndex] + degreeSteps[degreeSteps_index],
                       infoSettings.max_temp[curToolIndex]);

        pidTemperatureReDraw(true);
        break;

      case KEY_ICON_4:
        do
        {
          curToolIndex = (curToolIndex + 1) % MAX_HEATER_COUNT;
        } while (!heaterIsValid(curToolIndex) || curToolIndex == CHAMBER);

        pidItems.items[key_num] = itemTool[curToolIndex];

        menuDrawItem(&pidItems.items[key_num], key_num);
        pidTemperatureReDraw(false);
        break;

      case KEY_ICON_5:
        degreeSteps_index = (degreeSteps_index + 1) % ITEM_DEGREE_NUM;
        pidItems.items[key_num] = itemDegreeSteps[degreeSteps_index];

        menuDrawItem(&pidItems.items[key_num], key_num);
        break;

      case KEY_ICON_6:
        wPidStartMenuDialog();
        break;
       
      case KEY_ICON_7:
        ReturnToWizardsMenu();  
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            if (encoderPosition > 0)
            {
              if (pidHeaterTarget[curToolIndex] < infoSettings.max_temp[curToolIndex])
                pidHeaterTarget[curToolIndex] =
                    NOBEYOND(0, pidHeaterTarget[curToolIndex] + degreeSteps[degreeSteps_index],
                             infoSettings.max_temp[curToolIndex]);
            }
            else  
            {
              if (pidHeaterTarget[curToolIndex] > 0)
                pidHeaterTarget[curToolIndex] =
                    NOBEYOND(0, pidHeaterTarget[curToolIndex] - degreeSteps[degreeSteps_index],
                             infoSettings.max_temp[curToolIndex]);
            }

            pidTemperatureReDraw(true);
            encoderPosition = 0;
          }
        #endif
        break;
    }

    pidCheckTimeout();

    loopProcess();
  }
}


static void wSaveEepromSettingsUserSaidNo(void)
{
  infoMenu.cur--;
}

static void wPidStartMenuDialogUserSaidYes(void)
{
  wPidStartTarget(curToolIndex);
}


static void wPidStartupDialogUserSaidYes(void)
{
  bPidHotendDialogFlag = FALSE;  
  bPidHotbedDialogFlag = FALSE;  
}


static void wPidStartupDialog(void)
{ 
  
  if(curToolIndex == NOZZLE0 && bPidHotendDialogFlag == TRUE)
  {
    setDialogText(LABEL_HOTEND_PID_AUTOTUNE, LABEL_DIAGNOSTICS_DIALOG_V4, LABEL_CONFIRM, LABEL_BACKGROUND);
    showDialog(DIALOG_TYPE_INFO, wPidStartupDialogUserSaidYes, NULL, NULL);
  }
  else if(curToolIndex == BED && bPidHotbedDialogFlag == TRUE )
  {
    setDialogText(LABEL_HOTBED_PID_AUTOTUNE, LABEL_DIAGNOSTICS_DIALOG_V6, LABEL_CONFIRM, LABEL_BACKGROUND);
    showDialog(DIALOG_TYPE_INFO, wPidStartupDialogUserSaidYes, NULL, NULL);
  }
}


static void wPidStartMenuDialog()
{ 
  
  if(curToolIndex == NOZZLE0)
  {
    setDialogText(LABEL_HOTEND_PID_AUTOTUNE, LABEL_DIAGNOSTICS_DIALOG_V4_1, LABEL_CONFIRM, LABEL_CANCEL);
    showDialog(DIALOG_TYPE_INFO, wPidStartMenuDialogUserSaidYes, NULL, NULL);
  }
  else if(curToolIndex == BED)
  {
    setDialogText(LABEL_HOTBED_PID_AUTOTUNE, LABEL_DIAGNOSTICS_DIALOG_V4_1, LABEL_CONFIRM, LABEL_CANCEL);
    showDialog(DIALOG_TYPE_INFO, wPidStartMenuDialogUserSaidYes, NULL, NULL);
  }
}


static void wDisplayAutotuneWindow(uint8_t getCurToolIndex)
{
  if(getCurToolIndex == NOZZLE0)
  {
    GUI_DispString(TITLE_END_Y/3, PICON_VAL_LG_EX, LABEL_PID_AUTOTUNE_WINDOW_1);
    ICON_ReadDisplay(TITLE_END_Y / 3, PICON_VAL_LG_EX + POPUP_TITLE_HEIGHT, ICON_NOZZLE);
  }
  else
  {
    GUI_DispString(TITLE_END_Y/3, PICON_VAL_LG_EX, LABEL_PID_AUTOTUNE_WINDOW_2);
    ICON_ReadDisplay(TITLE_END_Y / 3, PICON_VAL_LG_EX + POPUP_TITLE_HEIGHT, ICON_BED);
  }
}


static inline void wPidStartTarget(uint8_t getCurToolIndex)
{
  pidRunning = true;
  pidSucceeded = true;

  pidCounter = 1; 
  pidTimeout = OS_GetTimeMs() + PID_PROCESS_TIMEOUT;  

  mustStoreCmd("M150 R255 U0 B0\n");  
  mustStoreCmd("M106 S0\n");        
  mustStoreCmd("G4 S1\n");            

  if (pidHeaterTarget[getCurToolIndex] > 0)
  {
    mustStoreCmd("%s S%d\n", pidCmd[getCurToolIndex], (int)pidHeaterTarget[getCurToolIndex]);  
    mustStoreCmd("G4 S1\n");                                       
  }

  mustStoreCmd("M107\n");             
  mustStoreCmd("M150 R0 U255 B0\n");  
  
  infoMenu.menu[++infoMenu.cur] = menuPidWait;
}


void wPidMenuHotbedSet()
{
  
  if(curToolIndex == NOZZLE0)
  {
    curToolIndex = BED;
    
    mustStoreCmd("G0 Y105 F6500\n");
    mustStoreCmd("G0 Y110 F6500\n");
    mustStoreCmd("G0 Y0 F2000\n");
    infoMenu.menu[++infoMenu.cur] = menuPid;
    bPidHotbedDialogFlag = TRUE;
  }
  else if(curToolIndex == BED)
  {
    ReturnToWizardsMenu(); 
  }
}