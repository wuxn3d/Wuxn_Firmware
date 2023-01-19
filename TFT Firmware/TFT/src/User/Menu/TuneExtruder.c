#include "TuneExtruder.h"
#include "includes.h"

#define ITEM_TUNE_EXTRUDER_LEN_NUM 4

static uint8_t degreeSteps_index = 1;
static uint8_t curExtStep_index = 0;
static uint8_t c_heater = NOZZLE0;

extern bool bPreheat_DisplayPreheatMenu;
uint32_t ulEStepsPreHeatCheckTimer = 0;
uint16_t unEStepsIndexDisplayIcon = 0;
extern bool bPreheatLoopCheck_AchievedTemperature;
extern bool bDontDimLCD;
bool bCalledByEStepsFlag = FALSE;  
extern bool bPreheat_DisplayPreheatMenu; 
extern TOOLPREHEAT whichHeatersToHeat;   
extern bool bFilamentStateCheckFlag;     

static void wEStepsOutro(void);
static void wEStepsOutroFinished(void);

MENUITEMS tuneExtruderItems = {
    
    LABEL_TUNE_EXT_TEMP,
    
    {
        {ICON_DEC, LABEL_DEC},
        {ICON_BACKGROUND, LABEL_BACKGROUND},
        {ICON_BACKGROUND, LABEL_BACKGROUND},
        {ICON_INC, LABEL_INC},
        {ICON_NOZZLE, LABEL_NOZZLE},
        {ICON_5_DEGREE, LABEL_5_DEGREE},
        {ICON_BACKGROUND, LABEL_BACKGROUND},
        {ICON_BACK, LABEL_BACK},
    }};


static void wEStepsOutro(void)
{
  setDialogText(LABEL_TUNE_EXT_TEMP, LABEL_BACKGROUND, LABEL_FINISHED, LABEL_BACKGROUND);
  showDialog(DIALOG_TYPE_INFO, wEStepsOutroFinished, NULL, NULL);
}


static void wEStepsOutroFinished(void)
{
  wClearFactoryResetStatus(); 
  ReturnToWizardsMenu();
}


void showExtrudeTemperature(uint8_t index, bool skip_header)
{
  char tempstr[20];

  setLargeFont(true);

  if (!skip_header)
  {
    sprintf(tempstr, "%-15s", heatDisplayID[index]);
    setLargeFont(false);
    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *)tempstr);
    setLargeFont(true);
    GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *)"ºC");
  }

  sprintf(tempstr, "%4d/%-4d", heatGetCurrentTemp(index), heatGetTargetTemp(index));
  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *)tempstr);
  setLargeFont(false);
}

static inline void turnHeaterOff(void)
{
  heatSetTargetTemp(c_heater, 0);
  ReturnToWizardsMenu();
}

static inline void returnToTuning(void)
{
  infoMenu.cur--;
}

void showNewESteps(const float measured_length, const float old_esteps, float *new_esteps)
{
  char tempstr[20];

  
  *new_esteps = (100 * old_esteps) / (100 - (measured_length - 20));

  GUI_DispString(exhibitRect.x0, exhibitRect.y0, textSelect(LABEL_TUNE_EXT_MEASURED));

  sprintf(tempstr, "  %0.2fmm  ", measured_length);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *)tempstr);

  sprintf(tempstr, (char *)textSelect(LABEL_TUNE_EXT_OLD_ESTEP), old_esteps);
  GUI_DispString(exhibitRect.x0, exhibitRect.y1 - BYTE_HEIGHT, (uint8_t *)tempstr);

  sprintf(tempstr, (char *)textSelect(LABEL_TUNE_EXT_NEW_ESTEP), *new_esteps);
  GUI_DispString(exhibitRect.x0, exhibitRect.y1, (uint8_t *)tempstr);
}

static inline void extrudeFilament(void)
{
  storeCmd("G28\n"); 
  
  mustStoreScript("G90\nG0 F3000 X125 Z125\n"); 
                                                
  mustStoreScript("M83\nG1 F100 E100\nM82\n");  
  infoMenu.menu[++infoMenu.cur] = menuNewExtruderESteps;
}


void wEstepsShowInstructions2(void)
{
  
  setDialogText(LABEL_TUNE_EXTRUDER, LABEL_TUNE_EXT_INST2, LABEL_EXTRUDE, LABEL_BACK); 
  
  showDialog(DIALOG_TYPE_QUESTION, extrudeFilament, wEstepsShowInstructions1, NULL);
}

void wEstepsShowInstructions1(void)
{
  setDialogText(LABEL_TUNE_EXTRUDER, LABEL_TUNE_EXT_MARK120MM, LABEL_CONTINUE, LABEL_CANCEL);
  showDialog(DIALOG_TYPE_QUESTION, wEstepsShowInstructions2, ReturnToWizardsMenu, NULL);
}

/*  070722 no longer used
static void wEStepsPreheatDialogbox(void)
{
  const uint16_t punIconDisplay[] = {ICON_TIMER1, ICON_TIMER2, ICON_TIMER3, ICON_TIMER4};

  bPreheat_DisplayPreheatMenu = FALSE;

  
  if(OS_GetTimeMs() > ulEStepsPreHeatCheckTimer)
  {
    ulEStepsPreHeatCheckTimer = OS_GetTimeMs() + 1000;
    ICON_ReadDisplay(INFOBOX_WIDTH + 10, ICON_HEIGHT + 30, punIconDisplay[unEStepsIndexDisplayIcon]);
    unEStepsIndexDisplayIcon++;
    if(unEStepsIndexDisplayIcon >= (sizeof(punIconDisplay)/ sizeof(uint16_t)))
    {
      unEStepsIndexDisplayIcon = 0;
    }
  }
  loopCheckHeater();
  if(bPreheatLoopCheck_AchievedTemperature == TRUE)
  {
    bPreheatLoopCheck_AchievedTemperature = FALSE;  
    setDialogText(LABEL_PREHEAT, LABEL_TEMPERATURE_ACHIEVED, LABEL_NEXT, LABEL_BACKGROUND);
    showDialog(DIALOG_TYPE_INFO, wEstepsShowInstructions1, NULL, NULL);
    unEStepsIndexDisplayIcon = 0;
  }
}
*/

void wEstepsSetTemperature(void)
{
  infoMenu.menu[++infoMenu.cur] = menuPreheat; 
  loopProcess();
}

void wSelectFilamentTemperature(void)
{
  bPreheat_DisplayPreheatMenu = TRUE;   
  whichHeatersToHeat = NOZZLE0_PREHEAT; 
  setDialogText(LABEL_TUNE_EXT_TEMP, LABEL_DIAGNOSTICS_DIALOG_V4, LABEL_NEXT, LABEL_BACKGROUND);
  showDialog(DIALOG_TYPE_INFO, wEstepsSetTemperature, NULL, NULL);
}

void wEStepsIntroDialogbox(void) 
{
  
  mustStoreCmd("M119\n");               
  loopProcess();                        
  if (bFilamentStateCheckFlag == FALSE) 
  {
    setDialogText(LABEL_LOAD, LABEL_NO_FILAMENT, LABEL_NEXT, LABEL_BACKGROUND); 
    showDialog(DIALOG_TYPE_ERROR, ReturnToReadyMenu, NULL, NULL);               
  }
  else 
  {
    bCalledByEStepsFlag = TRUE;
    setDialogText(LABEL_TUNE_EXT_TEMP, LABEL_ESTEPS_PROCEED, LABEL_YES, LABEL_NO);
    showDialog(DIALOG_TYPE_INFO, wSelectFilamentTemperature, ReturnToWizardsMenu, NULL);
    loopProcess();
  }
}

void menuEstepsPhase1(void)
{
  heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);

  menuDrawPage(&tuneExtruderItems);
  showExtrudeTemperature(c_heater, false);

#if LCD_ENCODER_SUPPORT
  encoderPosition = 0;
#endif

  bDontDimLCD = TRUE;

  wEStepsIntroDialogbox(); 
}

void menuEstepsPhase2(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  int16_t lastCurrent = heatGetCurrentTemp(c_heater);
  int16_t lastTarget = heatGetTargetTemp(c_heater);
  int16_t actCurrent;
  int16_t actTarget;

  /* 070722   if (heatGetTargetTemp(c_heater) < infoSettings.min_ext_temp)
    { 
      bCalledByEStepsFlag = TRUE;
      heatSetTargetTemp(NOZZLE0, infoSettings.min_ext_temp);
      setDialogText(LABEL_PREHEAT, LABEL_WARMING_UP, LABEL_BACKGROUND, LABEL_BACKGROUND);
      showDialog(DIALOG_TYPE_INFO, NULL, NULL, wEStepsPreheatDialogbox);
    }
    else
    { 
      wEstepsShowInstructions1();
    }
    */
  menuDrawPage(&tuneExtruderItems);
  showExtrudeTemperature(c_heater, false);
  wEstepsShowInstructions1();

  while (infoMenu.menu[infoMenu.cur] == menuEstepsPhase2)
  {
    actCurrent = heatGetCurrentTemp(c_heater);
    actTarget = heatGetTargetTemp(c_heater);

    key_num = menuKeyGetValue();
    switch (key_num)
    {
    case KEY_ICON_0:
      heatSetTargetTemp(c_heater, actTarget - degreeSteps[degreeSteps_index]);
      break;

    case KEY_INFOBOX:
    {
      char titlestr[30];
      sprintf(titlestr, "Min:0 | Max:%i", infoSettings.max_temp[c_heater]);

      int16_t val = numPadInt((uint8_t *)titlestr, actTarget, infoSettings.min_ext_temp, false);
      val = NOBEYOND(0, val, infoSettings.max_temp[c_heater]);

      if (val != actTarget)
        heatSetTargetTemp(c_heater, val);

      menuDrawPage(&tuneExtruderItems);
      showExtrudeTemperature(c_heater, false);
    }
    break;

    case KEY_ICON_3:
      heatSetTargetTemp(c_heater, actTarget + degreeSteps[degreeSteps_index]);
      break;

    case KEY_ICON_4:
      do
      {
        c_heater = (c_heater + 1) % MAX_HOTEND_COUNT;
      } while (!heaterIsValid(c_heater));

      showExtrudeTemperature(c_heater, false);
      break;

    case KEY_ICON_5:
      degreeSteps_index = (degreeSteps_index + 1) % ITEM_DEGREE_NUM;
      tuneExtruderItems.items[key_num] = itemDegreeSteps[degreeSteps_index];

      menuDrawItem(&tuneExtruderItems.items[key_num], key_num);
      break;

    case KEY_ICON_7:
      /* 070722
       if (heatGetTargetTemp(c_heater) > 0)
       {
         setDialogText(tuneExtruderItems.title.index, LABEL_TUNE_EXT_HEATOFF, LABEL_YES, LABEL_NO);
         showDialog(DIALOG_TYPE_QUESTION, turnHeaterOff, returnToTuning, NULL);
       }
       */
      for (uint8_t i = 0; i < infoSettings.hotend_count; i++)
      {
        if (heatGetTargetTemp(i) > 0)
        {
          setDialogText(LABEL_WARNING, LABEL_HEATERS_ON, LABEL_YES, LABEL_NO);
          showDialog(DIALOG_TYPE_QUESTION, heatCoolDown, NULL, NULL);
          break;
        }
      }
      ReturnToWizardsMenu(); 
      break;

    default:
#if LCD_ENCODER_SUPPORT
      if (encoderPosition)
      {
        heatSetTargetTemp(c_heater, actTarget + degreeSteps[degreeSteps_index] * encoderPosition);
        encoderPosition = 0;
      }
#endif
      break;
    }

    if (lastCurrent != actCurrent || lastTarget != actTarget)
    {
      lastCurrent = actCurrent;
      lastTarget = actTarget;
      showExtrudeTemperature(c_heater, true);
    }

    loopProcess();
  }

  
  if (heatHasWaiting() == false)
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
}

void menuNewExtruderESteps(void)
{

  
  
  
  MENUITEMS newExtruderESteps = {
      
      LABEL_TUNE_EXT_ADJ_ESTEPS,
      
      {
          {ICON_DEC, LABEL_DEC},
          {ICON_BACKGROUND, LABEL_BACKGROUND},
          {ICON_BACKGROUND, LABEL_BACKGROUND},
          {ICON_INC, LABEL_INC},
          {ICON_EEPROM_SAVE, LABEL_SAVE},
          {ICON_1_MM, LABEL_1_MM},
          {ICON_RESET_VALUE, LABEL_RESET},
          {ICON_BACK, LABEL_BACK},
      }};

  KEY_VALUES key_num = KEY_IDLE;

  float measured_length;
  float now = measured_length = 20.00f;
  float old_esteps, new_esteps; 

  mustStoreCmd("M503 S0\n");

  old_esteps = getParameter(P_STEPS_PER_MM, E_AXIS); 
  newExtruderESteps.items[KEY_ICON_5] = itemMoveLen[curExtStep_index];

  menuDrawPage(&newExtruderESteps);
  showNewESteps(measured_length, old_esteps, &new_esteps);

#if LCD_ENCODER_SUPPORT
  encoderPosition = 0;
#endif

  while (infoMenu.menu[infoMenu.cur] == menuNewExtruderESteps)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
    case KEY_ICON_0:
      measured_length -= moveLenSteps[curExtStep_index];
      break;

    case KEY_ICON_3:
      measured_length += moveLenSteps[curExtStep_index];
      break;

    case KEY_ICON_4:
    {
      LABELCHAR(tempStr, LABEL_TUNE_EXT_ESTEPS_SAVED);
      char tempMsg[120];

      storeCmd("M92 T0 E%0.2f\n", new_esteps);
      
      if (infoMachineSettings.EEPROM == 1)
      {
        mustStoreCmd("M500\n");
      }

      sprintf(tempMsg, tempStr, new_esteps);
      setDialogText(newExtruderESteps.title.index, (uint8_t *)tempMsg, LABEL_CONFIRM, LABEL_BACKGROUND);
      showDialog(DIALOG_TYPE_INFO, wEStepsOutro, NULL, NULL);
      break;
    }

    case KEY_ICON_5:
      curExtStep_index = (curExtStep_index + 1) % ITEM_TUNE_EXTRUDER_LEN_NUM;
      newExtruderESteps.items[key_num] = itemMoveLen[curExtStep_index];

      menuDrawItem(&newExtruderESteps.items[key_num], key_num);
      break;

    case KEY_ICON_6:
      measured_length = 0.0f;
      break;

    case KEY_ICON_7:
      if (heatGetTargetTemp(c_heater) > 0)
      {
        setDialogText(tuneExtruderItems.title.index, LABEL_TUNE_EXT_HEATOFF, LABEL_YES, LABEL_NO);
        showDialog(DIALOG_TYPE_QUESTION, turnHeaterOff, ReturnToWizardsMenu, NULL);
      }
      else
      {
        ReturnToWizardsMenu(); 
      }
      break;

    default:
#if LCD_ENCODER_SUPPORT
      if (encoderPosition)
      {
        measured_length += moveLenSteps[curExtStep_index] * encoderPosition;
        encoderPosition = 0;
      }
#endif
      break;
    }

    if (now != measured_length)
    {
      now = measured_length;
      showNewESteps(measured_length, old_esteps, &new_esteps);
    }

    loopProcess();
  }
}
