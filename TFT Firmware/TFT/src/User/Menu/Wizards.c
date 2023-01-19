#include "Wizards.h"
#include "includes.h"

extern bool bPidHotendDialogFlag;  
extern bool bDontDimLCD;   
extern SETTINGS SettingsBeforeOOBWCW;
extern bool bCalledByFirstLayerFlag;
bool bSetCouplingFromWizardFlag = FALSE;
extern AutoBedLevelIconStatus AutoBedLevelIconState;

const MENUITEMS wizardsItems = {
  
  LABEL_WIZARDS,
  
  {
  
    {ICON_DIAGNOSTICS,             LABEL_DIAGS},
    {ICON_Z_ALIGN,                 LABEL_COUPLING_SET},
    {ICON_LEVELING,                LABEL_AUTO_LEVELING},
    {ICON_FIRST_LAYER,             LABEL_FIRST_LAYER},
    {ICON_TEMP_CALIB,              LABEL_TEMP_CALIB},

    
    {ICON_E_CALIB,                 LABEL_E_CALIB},

    {ICON_FACTORYRESET,            LABEL_RESET},
    {ICON_BACK,                    LABEL_BACK},
  }
};

void RestoreDisplayTheme(void)
{
  infoSettings.title_bg_color= SettingsBeforeOOBWCW.title_bg_color;  
  infoSettings.bg_color = SettingsBeforeOOBWCW.bg_color;  
  infoSettings.lcd_idle_brightness = SettingsBeforeOOBWCW.lcd_idle_brightness;  
}


void ResetMenuStack(void)
{
  RestoreDisplayTheme();
  bDontDimLCD = FALSE;
  resetLCDidleTimer();
  infoMenu.cur = 0;
  infoMenu.menu[infoMenu.cur] = menuStatus;
}


void ReturnToReadyMenu(void) 
{
  ResetMenuStack();
}


void ReturnToMainMenu(void)
{
  ResetMenuStack();
  infoMenu.menu[++infoMenu.cur] = menuMain;
}


void ReturnToWizardsMenu(void)
{
  ResetMenuStack();
  infoMenu.menu[++infoMenu.cur] = menuWizards;
}


/*

void ReturnToUnifiedHeatMenu(void)
{
  ReturnToMainMenu();
  infoMenu.menu[++infoMenu.cur] = menuUnifiedHeat;
}
*/

void menuWizards(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&wizardsItems);

  while (infoMenu.menu[infoMenu.cur] == menuWizards)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuDiagnostics;
        break;

      case KEY_ICON_1:
        bSetCouplingFromWizardFlag = TRUE;
        AutoBedLevelIconState = AUTO_BEDLEVEL_SET_COUPLING;
        
        wDoTheBedLeveling(); 
        break;

      case KEY_ICON_2:
        infoMenu.menu[++infoMenu.cur] = menuBedLeveling;
        break;

      case KEY_ICON_3:   
        infoMenu.menu[++infoMenu.cur] = menuFirst_Layer;
        break;

      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuPid;
        bPidHotendDialogFlag = TRUE;  
        break;

       case KEY_ICON_5:
        infoMenu.menu[++infoMenu.cur] = menuEstepsPhase1;  
        break;

      case KEY_ICON_6:
		    wFactoryReset();
        break;

      case KEY_ICON_7:
        infoMenu.cur--;  
        
        break;

      default:
        break;
    }


    loopProcess();
  }
}