#include "BedLeveling.h"
#include "includes.h"

#define SET_COUPLING_GCODE_COUNT   6  
#define SET_COUPLING_GCODE_LENGTH  16 
#define Z_ALIGN_GCODE_COUNT        12 
#define Z_ALIGN_GCODE_LENGTH       30
#define MESH_EDIT_GCODE_COUNT      4
#define MESH_EDIT_GCODE_LENGTH     15
#define BED_LEVEL_ICON_ARRAY_SIZE  4

#define ICON_X_POSITION       70               
#define ICON_Y_POSITION       ICON_HEIGHT      
#define PROGRESS_X_POSITION   ICON_X_POSITION  
#define PROGRESS_Y_POSITION   165              
#define ICON_PADDING          120              
#define ICON_X_CENTER_POSITION ICON_X_POSITION + ICON_PADDING
#define PROGRESS_X_CENTER_POSITION ICON_X_CENTER_POSITION

uint8_t ucAutoBedLevelIconIndex = 0;
static int8_t ucIconIndex = 0;        
uint16_t unAutoBedLevelIconXPosition = ICON_X_CENTER_POSITION; 
uint16_t unAutoBedLevelIconYPosition = ICON_Y_POSITION;  
uint32_t ulCheckAutoBedLevelTimer = 0;
extern bool bDontDimLCD;
extern bool bSetCouplingFromWizardFlag;
extern bool f_bedLevelingSentMe;  
extern bool bPreheat_DisplayPreheatMenu;  
extern TOOLPREHEAT whichHeatersToHeat;  
extern bool bPreheat_DisplayPreheatMenu;  
bool f_bedLevelingSentMe = FALSE; 

AutoBedLevelIconStatus AutoBedLevelIconState = AUTO_BEDLEVEL_SET_COUPLING;

uint8_t ucAutoBedLevelGCodeIndex = 0;

static void wAutoBedLevelingUserSaidCancel(void);
void wAutoBedlevelIconDisplay(void);
void wUpdateAutoBedLevelGCode(void);
void wZAlignAndABLGcode(void);
static void wAutoBedLevelComplete(void);
static void wAutoBedLevelOutroDialog(void);
static void wAutoBedLevelOutroUserSaidContinue(void);
void wSetCouplingsGCode(void);  

 MENUITEMS bedLevelingItems = { 
    
    
    LABEL_BACKGROUND,
    
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



void wAutoBedlevelIconDisplay(void)
{
uint8_t pucProgressIcon [4] = {ICON_PB_ZERO, ICON_PB_33, ICON_PB_66, ICON_PB_100};

  if (OS_GetTimeMs() > ulCheckAutoBedLevelTimer)
  {
    
    ulCheckAutoBedLevelTimer = OS_GetTimeMs() + 1000;

    if(AutoBedLevelIconState != AUTO_BEDLEVEL_COMPLETE)
    {
      
      ICON_ReadDisplay(PROGRESS_X_CENTER_POSITION, PROGRESS_Y_POSITION, pucProgressIcon[ucIconIndex]);
      ucIconIndex++;
    }
    if (ucIconIndex == 4)
    {
    
      ucIconIndex = 0;
    }
  }
  loopProcess();

  if(infoCmd.count < CMD_MAX_LIST - 15)
  {
    wUpdateAutoBedLevelGCode();
  }
}


void wUpdateAutoBedLevelGCode(void)
{
  switch(AutoBedLevelIconState)
  {
    case AUTO_BEDLEVEL_SET_COUPLING:
      wSetCouplingsGCode();
      break;

    case AUTO_BEDLEVEL_Z_ALIGN:
      wZAlignAndABLGcode();
      break;

    default:
    break;
  }
}

void wSetCouplingsGCode(void)
{
  const char pucSetCouplingGCode[SET_COUPLING_GCODE_COUNT][SET_COUPLING_GCODE_LENGTH] = {
                                                          "G4 P100\n", 
                                                          "G90\n", 
                                                          "G28\n", 
                                                          "G0 F2500 Z251\n", 
                                                          "G28 Z\n", 
                                                          "G0 F2500 Z10\n"};  


  
  if(ucAutoBedLevelGCodeIndex >= SET_COUPLING_GCODE_COUNT)  
  {
    if(infoCmd.count == 0)
    {
      ucAutoBedLevelGCodeIndex = 0;
      ucAutoBedLevelIconIndex = 0;

      
      if(AutoBedLevelIconState == AUTO_BEDLEVEL_SET_COUPLING &&
         infoSettings.enable_oobwcw)
      {
        Buzzer_play(sound_notify); 
        OOBWCW_Step_2();
      }

      
      if(AutoBedLevelIconState == AUTO_BEDLEVEL_SET_COUPLING  &&
        bSetCouplingFromWizardFlag == TRUE)
      {
        bSetCouplingFromWizardFlag = FALSE; 
        Buzzer_play(sound_notify); 
        ReturnToWizardsMenu();
      }

      
      AutoBedLevelIconState = AUTO_BEDLEVEL_Z_ALIGN;
      ICON_ReadDisplay(PROGRESS_X_CENTER_POSITION, PROGRESS_Y_POSITION, ICON_COMPLETE);
      unAutoBedLevelIconXPosition += PICON_VAL_LG_EX;
    }

  }
  else 
  {
    
    mustStoreCmd(pucSetCouplingGCode[ucAutoBedLevelGCodeIndex]);
    ucAutoBedLevelGCodeIndex++;
  }
}


void wZAlignAndABLGcode(void)
{
  const char pucZAlignAndABLGCode[Z_ALIGN_GCODE_COUNT][Z_ALIGN_GCODE_LENGTH] = {
                                                          "G0 F2500 Z10\n",  
                                                          "G0 F7500 X0 Y0\n", 
                                                          "M190 S60\n", 
                                                          "M48 E L4 P10 S0 V2 X125 Y105\n", 
                                                          "G0 F2500 Z10\n", 
                                                          "G4 S2\n", 
                                                          "G34 I15 T0.05\n", 
                                                          "G0 F2500 Z10\n", 
                                                          "G4 S5\n", 
                                                          "G28\n",  
                                                          "G29\n",  
                                                          "M500\n"};  

  
  if(ucAutoBedLevelGCodeIndex >= Z_ALIGN_GCODE_COUNT)
  {
    if(infoCmd.count == 0)  
    {
      ucAutoBedLevelGCodeIndex = 0;
      ucAutoBedLevelIconIndex = 0;
      AutoBedLevelIconState = AUTO_BEDLEVEL_COMPLETE;
      
      if(infoSettings.enable_oobwcw != TRUE)
      {
        mustStoreCacheCmd("M140 S0\n");  
      }
      wAutoBedLevelComplete();
    }
  }
  else
  {
    
    mustStoreCmd(pucZAlignAndABLGCode[ucAutoBedLevelGCodeIndex]);
    ucAutoBedLevelGCodeIndex++;
  }
}


static void wAutoBedLevelComplete(void)
{
  ICON_ReadDisplay(ICON_X_CENTER_POSITION, ICON_Y_POSITION, ICON_MESH_EDITOR);
  ICON_ReadDisplay(PROGRESS_X_CENTER_POSITION, PROGRESS_Y_POSITION, ICON_COMPLETE);
  loopProcess();
  wAutoBedLevelOutroDialog();
}


void wAutoBedLevelingUserSaidStart(void)
{
  infoMenu.menu[++infoMenu.cur] = menuPreheat;  
}

void wDoTheBedLeveling (void)
{
#define DISPLAY_ICON_Z_ALIGN ICON_ReadDisplay(ICON_X_POSITION + (ICON_PADDING * 1), ICON_HEIGHT, ICON_Z_ALIGN);  
#define DISPLAY_ICON_MESH_EDIT ICON_ReadDisplay(ICON_X_POSITION + (ICON_PADDING * 1), ICON_HEIGHT, ICON_MESH_EDITOR);  
  
  bDontDimLCD = TRUE;

  if(infoSettings.enable_oobwcw == ENABLE)
  {

    if(AutoBedLevelIconState == AUTO_BEDLEVEL_SET_COUPLING)
    {
      setDialogText(LABEL_ABL_SETTINGS, LABEL_BACKGROUND ,LABEL_BACKGROUND, LABEL_BACKGROUND);
      showDialog(DIALOG_TYPE_INFO, NULL, NULL, wAutoBedlevelIconDisplay);
      loopProcess();
      DISPLAY_ICON_Z_ALIGN;
    }

    else if(AutoBedLevelIconState == AUTO_BEDLEVEL_Z_ALIGN)  
    {
      setDialogText(LABEL_BED_LEVELING, LABEL_BACKGROUND ,LABEL_BACKGROUND, LABEL_BACKGROUND);
      showDialog(DIALOG_TYPE_INFO, NULL, NULL, wAutoBedlevelIconDisplay);
      loopProcess();
      DISPLAY_ICON_MESH_EDIT;
    }
  }

  else if(bSetCouplingFromWizardFlag == TRUE) 
  {
    setDialogText(LABEL_ABL_SETTINGS, LABEL_BACKGROUND ,LABEL_BACKGROUND, LABEL_BACKGROUND);
    showDialog(DIALOG_TYPE_INFO, NULL, NULL, wAutoBedlevelIconDisplay);
    loopProcess();
    DISPLAY_ICON_Z_ALIGN;
  }

  else  
  {
    
    
    mustStoreCmd("G28\n");
    AutoBedLevelIconState = AUTO_BEDLEVEL_Z_ALIGN;
    setDialogText(LABEL_BED_LEVELING, LABEL_BACKGROUND ,LABEL_BACKGROUND, LABEL_BACKGROUND);
    showDialog(DIALOG_TYPE_INFO, NULL, NULL, wAutoBedlevelIconDisplay);
    loopProcess();
    DISPLAY_ICON_MESH_EDIT;
  }
  ucAutoBedLevelGCodeIndex = 0;
  loopProcess();
}



static void wAutoBedLevelingUserSaidCancel(void)
{
  ReturnToWizardsMenu();
}


static void wAutoBedLevelOutroDialog(void)
{
  if(infoSettings.enable_oobwcw == ENABLE)
  {
    mustStoreCmd("G0 F4000 Z100\n");  
  }
  else
  {
    mustStoreCmd("G0 F4000 X10 Y200\n");  
    
    TurnOffHeatersAndFans(); 
  }
  setDialogText(LABEL_BED_LEVELING, LABEL_ABL_BOX_V3, LABEL_CONTINUE, LABEL_BACKGROUND);
  showDialog(DIALOG_TYPE_INFO, wAutoBedLevelOutroUserSaidContinue, NULL, NULL);
  loopProcess();
}


static void wAutoBedLevelOutroUserSaidContinue(void)
{
  if(infoSettings.enable_oobwcw == ENABLE)
  {
    OOBWCW_Step_5(); 
  }
  else
  {
    ReturnToWizardsMenu();  
  }
}

void menuBedLeveling(void)
{
  
  
  f_bedLevelingSentMe = TRUE;  
  bPreheat_DisplayPreheatMenu = TRUE; 
  whichHeatersToHeat = BED_PREHEAT;  

  menuDrawPage(&bedLevelingItems);

  setDialogText(LABEL_BED_LEVELING, LABEL_ABL_BOX_V1, LABEL_START, LABEL_CANCEL);
  showDialog(DIALOG_TYPE_INFO, wAutoBedLevelingUserSaidStart, wAutoBedLevelingUserSaidCancel, NULL);

  while (infoMenu.menu[infoMenu.cur] == menuBedLeveling)
  {
    loopProcess();
  }
}


void menuBedLevelingPhase2 (void)  
{
  menuDrawPage(&bedLevelingItems);
  wDoTheBedLeveling(); 

  while (infoMenu.menu[infoMenu.cur] == menuBedLevelingPhase2)
  {
    loopProcess();
  }
}