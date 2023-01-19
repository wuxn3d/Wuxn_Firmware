#include "Diagnostics.h"
#include "includes.h"


#define ICON_Y_POSITION       ICON_HEIGHT      
#define PROGRESS_X_POSITION   ICON_X_POSITION  
#define PROGRESS_Y_POSITION   165              
#define ICON_PADDING          120              
#define X_MOTOR_GCODE_COUNT    56 
#define Y_MOTOR_GCODE_COUNT    56 
#define Z_MOTOR_GCODE_COUNT    9  
#define GCODES_MAX_LENGTH      25              

extern bool bPidHotendDialogFlag;
extern bool bFilamentStateCheckFlag;          
extern bool bDontDimLCD;
extern bool bLongPurge;
extern bool bFirstPrintFromOOBWCW;

static uint32_t ulCheckCurrentTimer = 0;       
static int8_t ucIconIndex = 0;                 
uint16_t usXPosition = ICON_X_POSITION;        
static uint8_t ucGCodeIndex = 0;               
uint8_t ucPartFanIndex = 0;
static uint32_t ulHotEndFanTimer = 0;

bool gbProbeTestStart = FALSE;                 

enum wMotorTestStatus { MOTOR_TEST_X, MOTOR_TEST_Y, MOTOR_TEST_Z, MOTOR_TEST_COMPLETE };  
enum wMotorTestStatus wMotorTestState = MOTOR_TEST_X;

static void wDiagnosticsV1UserSaidYes(void);     
static void wDiagnosticsViUserSaidNo(void);      
static void wHotEndFanLoop();                    
static void wReferToManualPartFan();             
static void wPartFanUserSaidYes();               

static void wHotFanUserSaidYes();                
static void xReferToManualHotFan();              

static void wMotorTest(void);                    
static void wMotorCheckContinueUserSaidYes(void);
static void wMotorTestIconDisplay(void);         
static void wMotorTestComplete(void);            
static void wUpdateMotorTestGCode(void);         
static void wProcessXGcode(void);                
static void wProcessYGcode(void);                
static void wProcessZGcode(void);                


static void wFilamentCheckUserSaidYes(void);     
static void wReferToManualFilamentTestOOBWCW(void);  
static void wReferToManualFilamentTest(void);    

static void wReferToManualProbeTest(void);       

static void wReturnToDiagnoticsMenu(void);       



const MENUITEMS DiagnosticsItems = {
  
  LABEL_DIAGNOSTICS,
  
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


void menuDiagnostics(void)
{
  bDontDimLCD = TRUE;
  
  menuDrawPage(&DiagnosticsItems);

  setDialogText(LABEL_DIAGNOSTICS, LABEL_DIAGNOSTICS_DIALOG_V1, LABEL_CONTINUE, LABEL_CANCEL);
        showDialog(DIALOG_TYPE_QUESTION, wDiagnosticsV1UserSaidYes, wDiagnosticsViUserSaidNo, NULL);

  while (infoMenu.menu[infoMenu.cur] == menuDiagnostics)
  {
    loopProcess();
  }
}


static void wDiagnosticsV1UserSaidYes(void)
{
  
  setDialogText(LABEL_DIAGNOSTICS, LABEL_WUXN_CALIBRATION, LABEL_YES, LABEL_BACKGROUND);
  showDialog(DIALOG_TYPE_QUESTION, wFanTest, NULL, NULL);
}

static void wDiagnosticsViUserSaidNo(void)
{
  ReturnToWizardsMenu();
}



void wFanTest(void)
{
  setDialogText(LABEL_FAN_TEST, LABEL_DIAGNOSTICS_DIALOG_V2, LABEL_YES, LABEL_NO);
  showDialog(DIALOG_TYPE_QUESTION, wPartFanUserSaidYes, wReferToManualPartFan, NULL);
  fanSetSpeed(ucPartFanIndex, infoSettings.fan_max[ucPartFanIndex]); 
}


void wClearFactoryResetStatus()
{
  
  if(infoSettings.factory_reset == ENABLE )
  {
    SETTINGS now = infoSettings;
    infoSettings.factory_reset = DISABLE;

    if (memcmp(&now, &infoSettings, sizeof(SETTINGS)))
    {
      storePara();
    }
  }
}


static void wPartFanUserSaidYes()
{
  fanSetSpeed(ucPartFanIndex, 0); 
  ulHotEndFanTimer = 0;
  setDialogText(LABEL_FAN_TEST, LABEL_DIAGNOSTICS_DIALOG_V3, LABEL_YES, LABEL_NO);
  showDialog(DIALOG_TYPE_QUESTION, wHotFanUserSaidYes, xReferToManualHotFan, wHotEndFanLoop);
}


static void wHotEndFanLoop()
{
  if (OS_GetTimeMs() > ulHotEndFanTimer)
  {
    
    ulHotEndFanTimer = OS_GetTimeMs() + 1000;
	  mustStoreCmd("M106 P9 S255\n");
  }
}


static void  wHotFanUserSaidYes()
{
  loopProcess();

  if(infoSettings.enable_oobwcw)
  {
    Buzzer_play(sound_notify);
    OOBWCW_Step_3();  
  }
  else
  {
    
    setDialogText(LABEL_PROBE_TEST, LABEL_PROBE_TEST_INTRO, LABEL_YES, LABEL_NO);
    showDialog(DIALOG_TYPE_QUESTION, wFilamentCheck, wReferToManualProbeTest, NULL);
  }
}



static void wMotorTest()
{
  setDialogText(LABEL_MOTORCHECK, LABEL_MOTORCHECK_INTRO, LABEL_START, LABEL_BACKGROUND);
  showDialog(DIALOG_TYPE_INFO, wMotorCheckUserSaidYes, NULL, NULL);
}


void wMotorCheckUserSaidYes()
{
 /* 072322
  if(infoSettings.enable_oobwcw != ENABLE)

  {  
    setDialogText(LABEL_MOTORCHECK, LABEL_BACKGROUND, LABEL_BACKGROUND, LABEL_BACKGROUND);
  }
  else
  {  
    setDialogText(LABEL_MOTORCHECK_XY, LABEL_BACKGROUND, LABEL_BACKGROUND, LABEL_BACKGROUND);
  }
  showDialog(DIALOG_TYPE_INFO, NULL, NULL, wMotorTestIconDisplay);
  loopProcess();
  ICON_ReadDisplay(ICON_X_POSITION + (ICON_PADDING * 0), ICON_Y_POSITION, ICON_X_MOTION);
  ICON_ReadDisplay(ICON_X_POSITION + (ICON_PADDING * 1), ICON_Y_POSITION, ICON_Y_MOTION);
  if(infoSettings.enable_oobwcw != ENABLE)
  {
    
    ICON_ReadDisplay(ICON_X_POSITION + (ICON_PADDING * 2), ICON_Y_POSITION, ICON_Z_MOTION);
  }
*/
  
  
  if(infoSettings.enable_oobwcw == ENABLE) 
  {  
    
    usXPosition = MOTORTEST_XY_XMOTOR_X;  
    setDialogText(LABEL_MOTORCHECK_XY, LABEL_BACKGROUND, LABEL_BACKGROUND, LABEL_BACKGROUND);
    showDialog(DIALOG_TYPE_INFO, NULL, NULL, wMotorTestIconDisplay);
    loopProcess();
    ICON_ReadDisplay(MOTORTEST_XY_XMOTOR_X, ICON_Y_POSITION, ICON_X_MOTION);  
    ICON_ReadDisplay(MOTORTEST_XY_YMOTOR_X, ICON_Y_POSITION, ICON_Y_MOTION);  
  }
  else  
  {
    setDialogText(LABEL_MOTORCHECK, LABEL_BACKGROUND, LABEL_BACKGROUND, LABEL_BACKGROUND);
    showDialog(DIALOG_TYPE_INFO, NULL, NULL, wMotorTestIconDisplay);
    loopProcess();
   
   
    ICON_ReadDisplay(ICON_X_POSITION + (ICON_PADDING * 0), ICON_Y_POSITION, ICON_X_MOTION);
    ICON_ReadDisplay(ICON_X_POSITION + (ICON_PADDING * 1), ICON_Y_POSITION, ICON_Y_MOTION);
    ICON_ReadDisplay(ICON_X_POSITION + (ICON_PADDING * 2), ICON_Y_POSITION, ICON_Z_MOTION);
  }

  wMotorTestState = MOTOR_TEST_X;
  ucGCodeIndex = 0;

}



static void wMotorCheckContinueUserSaidYes()
{
   if(infoSettings.enable_oobwcw == ENABLE)
  {
    OOBWCW_Step_4(); 
  }
  else
  {
    setDialogText(LABEL_DIAGNOSTICS, LABEL_DIAGNOSTICS_PASSED, LABEL_CONTINUE, LABEL_BACKGROUND);
    showDialog(DIALOG_TYPE_INFO, ReturnToWizardsMenu, NULL, NULL);
    
  }
}



void wFilamentCheck()
{
  if(infoSettings.enable_oobwcw == TRUE)
  {
    
    mustStoreCmd("M119\n");
    setDialogText(LABEL_FIL_RUNOUT, LABEL_SENSORCHECK_INTRO, LABEL_YES, LABEL_NO);
    showDialog(DIALOG_TYPE_INFO, wFilamentCheckUserSaidYes, wReferToManualFilamentTestOOBWCW, wSendGcodeAutoFilamentLoad);
  }
  else
  {
    
    mustStoreCmd("M119\n");
    loopProcess();
    if(bFilamentStateCheckFlag == TRUE)
    {
      setDialogText(LABEL_FIL_RUNOUT, LABEL_FILAMENTIRTEST_INTRO_Y, LABEL_YES, LABEL_NO);
    }
    else
    {
      setDialogText(LABEL_FIL_RUNOUT, LABEL_FILAMENTIRTEST_INTRO_N, LABEL_YES, LABEL_NO);
    }
    showDialog(DIALOG_TYPE_INFO, wMotorTest, wReferToManualFilamentTest, NULL);
  }
}

static void wReferToManualPartFan(void)
{
  fanSetSpeed(ucPartFanIndex, 0); 
  setDialogText(LABEL_PART_FAN_ERROR, LABEL_DIAGNOSTICS_ERROR, LABEL_CONTINUE, LABEL_RESTART);
  showDialog(DIALOG_TYPE_ALERT, wPartFanUserSaidYes, wReturnToDiagnoticsMenu, NULL);
}


static void xReferToManualHotFan()
{
  
  setDialogText(LABEL_HOT_FAN_ERROR, LABEL_DIAGNOSTICS_ERROR, LABEL_CONTINUE, LABEL_RESTART);
  showDialog(DIALOG_TYPE_INFO, wHotFanUserSaidYes, wReturnToDiagnoticsMenu, NULL);
}

static void wReferToManualProbeTest(void)
{
  setDialogText(LABEL_PROBE_TEST, LABEL_DIAGNOSTICS_ERROR, LABEL_CONTINUE, LABEL_BACK);
  showDialog(DIALOG_TYPE_ALERT, wFilamentCheck, wReturnToDiagnoticsMenu, NULL);
}

static void wOOBWCWFilamentTestExitToMain(void)
{
  
  RestoreDisplayTheme(); 
  infoSettings.enable_oobwcw = DISABLE;
  storePara();  
  bFirstPrintFromOOBWCW = FALSE;
  TurnOffHeatersAndFans();
  ReturnToReadyMenu();
}

static void wReferToManualFilamentTestOOBWCW(void)
{
  usXPosition = ICON_X_POSITION; 
  setDialogText(LABEL_FIL_RUNOUT, LABEL_DIAGNOSTICS_ERROR, LABEL_BACK, LABEL_MAIN_SCREEN);
  showDialog(DIALOG_TYPE_ALERT, OOBWCW_Step_5, wOOBWCWFilamentTestExitToMain, NULL);
}

static void wReferToManualFilamentTest(void)
{
  setDialogText(LABEL_FIL_RUNOUT, LABEL_DIAGNOSTICS_ERROR, LABEL_CONTINUE, LABEL_BACK);
  showDialog(DIALOG_TYPE_ALERT, wMotorTest, wReturnToDiagnoticsMenu, NULL);
}


/*
static void wReferToManualMotorTest(void)   
                                            
{

  setDialogText(LABEL_MOTORTEST, LABEL_DIAGNOSTICS_ERROR, LABEL_CONTINUE, LABEL_BACK);
  showDialog(DIALOG_TYPE_ALERT, wMotorTest, ReturnToWizardsMenu, NULL);  
}
*/

static void wReturnToDiagnoticsMenu(void)
{
  resetLCDidleTimer();
  bDontDimLCD = FALSE;
  infoMenu.cur = 0;
  infoMenu.menu[infoMenu.cur] = menuStatus;
  infoMenu.menu[++infoMenu.cur] = menuDiagnostics;
}


static void wFilamentCheckUserSaidYes()
{
  if(infoSettings.enable_oobwcw == ENABLE)
  {
    
    bLongPurge = TRUE;
    wPurgeDialog();
  }
  else
  {
    wMotorTest();
  }
}


static void wMotorTestIconDisplay()
{
  uint8_t pucProgressIcon [4] = {ICON_PB_ZERO, ICON_PB_33, ICON_PB_66, ICON_PB_100};

  if (OS_GetTimeMs() > ulCheckCurrentTimer)
  {
    
    ulCheckCurrentTimer = OS_GetTimeMs() + 1000;

      if (wMotorTestState != MOTOR_TEST_COMPLETE)
      {
        
        ICON_ReadDisplay(usXPosition, PROGRESS_Y_POSITION, pucProgressIcon[ucIconIndex]);
        ucIconIndex++;
      }
        if (ucIconIndex == 4)
        {
          
          ucIconIndex = 0;
        }
  }

  loopProcess();

  
  if (infoCmd.count < CMD_MAX_LIST - 10)
  {
    wUpdateMotorTestGCode();
  }
}


static void wUpdateMotorTestGCode()
{
  switch(wMotorTestState)
  {
    case MOTOR_TEST_X:
    {
      wProcessXGcode();
      break;
    }
    case MOTOR_TEST_Y:
    {
      wProcessYGcode();
      break;
    }
    case MOTOR_TEST_Z:
    {
      wProcessZGcode();
      break;
    }
      default:
      break;
  }
}


static void wProcessXGcode()
{
  char pucXTest[X_MOTOR_GCODE_COUNT][GCODES_MAX_LENGTH] = {
                          "G0 F10000 X10\n",
                          "G28 X\n",
                          "G0 F1000 X250\n",
                          "G0 F2000 X1\n", 
                          "G0 F3000 X250\n",
                          "G0 F5000 X1\n", 
                          "G0 F6000 X250\n", 
                          "G0 F7000 X1\n", 
                          "G0 F8000 X250\n", 
                          "G0 F9000 X1\n", 
                          "G0 F10000 X10\n",
                          "G4 P200\n",
                          "G4 S3\n",  
                          "G28 X\n",
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G0 F7500 X20\n",
                          "G0 F7500 X1\n", 
                          "G4 P200\n",
                          "G4 S3\n"  
                        };

  if (ucGCodeIndex >= X_MOTOR_GCODE_COUNT)
  {
    
    if(infoCmd.count == 0)
    {
      ucGCodeIndex = 0;
      ucIconIndex = 0;
      Buzzer_play(sound_success);
      wMotorTestState = MOTOR_TEST_Y;
      ICON_ReadDisplay(usXPosition, PROGRESS_Y_POSITION, ICON_COMPLETE);
      if(infoSettings.enable_oobwcw == ENABLE)  
        
        usXPosition = MOTORTEST_XY_YMOTOR_X;  
      else
        usXPosition += ICON_PADDING;
    }
  }
  else
  {
    mustStoreCmd(pucXTest[ucGCodeIndex]);
    ucGCodeIndex++;
  }
}


static void wProcessYGcode()
{
  char pucYTest[Y_MOTOR_GCODE_COUNT][GCODES_MAX_LENGTH] = {
                            "G0 F10000 Y10\n",
                            "G28 Y\n",
                            "G0 F1000 Y210\n",
                            "G0 F2000 Y1 \n", 
                            "G0 F3000 Y210\n",
                            "G0 F5000 Y1\n", 
                            "G0 F6000 Y210\n", 
                            "G0 F7000 Y1\n", 
                            "G0 F8000 Y210\n", 
                            "G0 F9000 Y1\n", 
                            "G0 F10000 Y10\n",
                            "G4 P200\n",
                            "G4 S3\n",  
                            "G28 Y\n",
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G0 F7500 Y20\n",
                            "G0 F7500 Y1\n", 
                            "G4 P200\n",
                            "G4 S3\n"  
                            };

  if(ucGCodeIndex >= Y_MOTOR_GCODE_COUNT)
  {
    
    if(infoCmd.count == 0)
    {
      ucGCodeIndex = 0;
      ucIconIndex = 0;
      ICON_ReadDisplay(usXPosition , PROGRESS_Y_POSITION, ICON_COMPLETE);
      Buzzer_play(sound_success);
      if(infoSettings.enable_oobwcw == ENABLE)
      {
        wMotorTestState = MOTOR_TEST_COMPLETE;  
        wMotorTestComplete();
      }
      else
      {
        wMotorTestState = MOTOR_TEST_Z;
        usXPosition += ICON_PADDING;
      }

    }
  }
  else
  {
    mustStoreCmd(pucYTest[ucGCodeIndex]);
    ucGCodeIndex++;
  }
}


static void wProcessZGcode()
{
  char pucZTest[Z_MOTOR_GCODE_COUNT][GCODES_MAX_LENGTH] = {
                            "G28\n",
                            "G0 F2500 Z251\n",
                            "G28 Z\n",
                            "G0 F5000 Z15\n",
                            "G4 S3\n",  
                            "G0 F10000 X0 Y210\n",
                            "G4 P200\n",
                            "G4 P100\n",
                            "G4 S3\n"   
                          };
  if (ucGCodeIndex >= Z_MOTOR_GCODE_COUNT)
  {
    
    if(infoCmd.count == 0)
    {
      usXPosition = PROGRESS_X_POSITION;
      ucGCodeIndex = 0;
      ucIconIndex = 0;
      wMotorTestState = MOTOR_TEST_COMPLETE;
      Buzzer_play(sound_success);
      wMotorTestComplete();
    }
  }
  else
  {
    mustStoreCmd(pucZTest[ucGCodeIndex]);
    ucGCodeIndex++;
  }
}


static void wMotorTestComplete()
{
  if(infoSettings.enable_oobwcw != ENABLE)
  {
    setDialogText(LABEL_MOTORCHECK ,LABEL_BACKGROUND ,LABEL_CONTINUE ,LABEL_BACKGROUND);
  }
  else
  {
    setDialogText(LABEL_MOTORCHECK_XY ,LABEL_BACKGROUND ,LABEL_CONTINUE ,LABEL_BACKGROUND);
  }
  showDialog(DIALOG_TYPE_INFO ,wMotorCheckContinueUserSaidYes ,NULL ,wMotorTestIconDisplay);

  loopProcess();
  Buzzer_play(sound_interval);  
  Buzzer_play(sound_notify);
  if(infoSettings.enable_oobwcw == ENABLE)  
  {
    ICON_ReadDisplay(MOTORTEST_XY_XMOTOR_X, ICON_Y_POSITION, ICON_X_MOTION);
    ICON_ReadDisplay(MOTORTEST_XY_XMOTOR_X + (ICON_PADDING * 1), ICON_Y_POSITION, ICON_Y_MOTION);
    ICON_ReadDisplay(MOTORTEST_XY_XMOTOR_X, PROGRESS_Y_POSITION, ICON_COMPLETE);
    ICON_ReadDisplay(MOTORTEST_XY_XMOTOR_X + (ICON_PADDING * 1), PROGRESS_Y_POSITION, ICON_COMPLETE);
  }
  
  else 
  {
   ICON_ReadDisplay(ICON_X_POSITION + (ICON_PADDING * 0), ICON_Y_POSITION, ICON_X_MOTION);
   ICON_ReadDisplay(ICON_X_POSITION + (ICON_PADDING * 1), ICON_Y_POSITION, ICON_Y_MOTION);
   ICON_ReadDisplay(PROGRESS_X_POSITION + (ICON_PADDING * 0), PROGRESS_Y_POSITION, ICON_COMPLETE);
   ICON_ReadDisplay(PROGRESS_X_POSITION + (ICON_PADDING * 1), PROGRESS_Y_POSITION, ICON_COMPLETE);
   ICON_ReadDisplay(ICON_X_POSITION + (ICON_PADDING * 2), ICON_Y_POSITION, ICON_Z_MOTION);
   ICON_ReadDisplay(PROGRESS_X_POSITION + (ICON_PADDING * 2), PROGRESS_Y_POSITION, ICON_COMPLETE);
  }

}

