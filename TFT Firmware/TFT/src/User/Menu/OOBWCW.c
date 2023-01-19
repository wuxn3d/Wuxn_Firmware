#include "OOBWCW.h"
#include "includes.h"

extern bool bPidHotendDialogFlag;  
extern AutoBedLevelIconStatus AutoBedLevelIconState;
extern uint8_t ucAutoBedLevelGCodeIndex;
extern uint16_t usXPosition;  
extern uint8_t ucPartFanIndex;
extern SETTINGS SettingsBeforeOOBWCW;

bool bFirstPrintFromOOBWCW = false;

void OobwcwBoxUserSaidYes(void);
void OOBWCW_Leave(void);



const MENUITEMS blankMenuItems = {
  
  LABEL_OOBWCW,
  
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

void TurnOffHeatersAndFans(void)
{
  extern bool bPreheatLoopCheck_AchievedTemperature;  
  heatSetTargetTemp(BED, 0);
  heatSetTargetTemp(NOZZLE0, 0);  
  fanSetSpeed(ucPartFanIndex, 0); 
  loopProcess(); 
  mustStoreCmd("M104 S0\n");  
  mustStoreCmd ("M140 S0\n"); 
  mustStoreCmd("M107\n");  
  bPreheatLoopCheck_AchievedTemperature = FALSE;  
  loopProcess();  
}

void menuOOBWCW(void)
{
  GUI_SetColor(WHITE);
  
  
  

  
  
  infoSettings.title_bg_color= lcd_colors[LCD_PURPLE];
  infoSettings.bg_color = lcd_colors[LCD_PURPLE];
  
  infoSettings.lcd_idle_brightness = infoSettings.lcd_brightness;
  menuDrawPage(&blankMenuItems);  

  
  
  #define PLA_NUM 0
  heatSetTargetTemp(BED, infoSettings.preheat_bed[PLA_NUM]);
  heatSetTargetTemp(NOZZLE0, infoSettings.preheat_temp[PLA_NUM]);

  
  setDialogText(LABEL_WUXN, LABEL_WUXN_CALIBRATION, LABEL_YES, LABEL_BACKGROUND);
  showDialog(DIALOG_TYPE_QUESTION, OobwcwBoxUserSaidYes, NULL, NULL);

  while (infoMenu.menu[infoMenu.cur] == menuOOBWCW)
  {
    loopProcess();
  }
}


void OobwcwBoxUserSaidYes()
{
  AutoBedLevelIconState = AUTO_BEDLEVEL_SET_COUPLING;
  ucAutoBedLevelGCodeIndex = 0;
  
  wDoTheBedLeveling(); 
}

void OOBWCW_Step_2(void)
{
  
  wFanTest();  
}

void OOBWCW_Step_3(void)
{
  
  wMotorCheckUserSaidYes();
}

void OOBWCW_Step_4(void)
{
  
  AutoBedLevelIconState = AUTO_BEDLEVEL_Z_ALIGN;
  ucAutoBedLevelGCodeIndex = 0;
  
  wDoTheBedLeveling(); 
}

void OOBWCW_Step_5(void)
{
  
  
  
  wFilamentCheck();
}

void OOBWCW_Step_6(void)
{
  
  RestoreDisplayTheme();  
  infoSettings.enable_oobwcw = DISABLE; 
  storePara();  

  
  usXPosition = ICON_X_POSITION;

  
  bFirstPrintFromOOBWCW = TRUE; 
  infoMenu.menu[++infoMenu.cur] = menuFirstPrint;
}

void OOBWCW_Step_7(void)
{
  
  
  TurnOffHeatersAndFans();
  
  bFirstPrintFromOOBWCW = FALSE;
  
  setDialogText(LABEL_FIRST_PRINT_GUIDED, LABEL_OOBWCW_EVAL, LABEL_CONTINUE, LABEL_BACKGROUND);
  
  showDialog(DIALOG_TYPE_QUESTION, OOBWCW_Leave, NULL, NULL);
  
}

void OOBWCW_Leave(void)
{
  infoMenu.cur = 0;
  infoMenu.menu[infoMenu.cur] = menuStatus;
}
