#include "First_Layer.h"
#include "includes.h"

const MENUITEMS First_LayerItems = {
  
  LABEL_FIRST_LAYER_OFFSET,

  

  
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

extern bool bFilamentStateCheckFlag;
extern bool bPreheat_DisplayPreheatMenu;
bool bPreheatAskFilamentTypeFlag = FALSE;
bool bFilamentLoadGCodeSent = FALSE;
static uint32_t ulCheckFilamentTriggerTimer = 0;
bool bCalledByFirstLayerFlag = FALSE;  
extern bool bDontDimLCD;

static void wReturnFirstLayerScreenUserSaidCancel(void);
static void wPreheatFromFirstLayerOffsetZProbeOpen(void);
static void wDisplayDialogFromFirstLayerOffsetUserSaidNo(void);
static void wCheckFilamentOpenToTrigger(void);
static void wPreheatFromFirstLayerOffsetDialogboxUserSaidContinue(void);
static void wReturnFirstLayerOffsetUserSaidRestart(void);


void wPingIRSensorFromFirstLayerOffset(void)
{
  loopProcess();

  bPreheatAskFilamentTypeFlag = TRUE;

  if(bFilamentStateCheckFlag == TRUE)
  {
    bPreheat_DisplayPreheatMenu = TRUE;
    infoMenu.menu[++infoMenu.cur] = menuPreheat;
  }
  else
  {
    ulCheckFilamentTriggerTimer = 0;
    if(infoSettings.enable_oobwcw == TRUE)
    {
      setDialogText(LABEL_FIRST_LAYER_OFFSET, LABEL_SENSORCHECK_INTRO, LABEL_YES, LABEL_NO);
    }
    else  
    {
      setDialogText(LABEL_FIRST_LAYER_OFFSET, LABEL_SENSORCHECK_INTRO2, LABEL_YES, LABEL_NO);
    }
    showDialog(DIALOG_TYPE_QUESTION, wPreheatFromFirstLayerOffsetZProbeOpen, wDisplayDialogFromFirstLayerOffsetUserSaidNo, wCheckFilamentOpenToTrigger);
  }
}


static void wReturnFirstLayerScreenUserSaidCancel(void)
{
  ReturnToWizardsMenu();
}


static void wPreheatFromFirstLayerOffsetZProbeOpen(void)
{
  infoMenu.menu[++infoMenu.cur] = menuPreheat;
}



static void wDisplayDialogFromFirstLayerOffsetUserSaidNo(void)
{
  setDialogText(LABEL_FIRST_LAYER_OFFSET, LABEL_FIRST_LAYER_DIALOG_V2_1, LABEL_CONTINUE, LABEL_RESTART);
  showDialog(DIALOG_TYPE_INFO, wPreheatFromFirstLayerOffsetDialogboxUserSaidContinue, wReturnFirstLayerOffsetUserSaidRestart, NULL);
}


void wSendGcodeAutoFilamentLoad()
{
  loopProcess();

  if(bFilamentStateCheckFlag == TRUE &&
     bFilamentLoadGCodeSent == FALSE)
  {
    wFilamentLoadGCode();
    
    ulCheckFilamentTriggerTimer = 0xFFFFFFFF;
  }
  else
  {
    if (OS_GetTimeMs() > ulCheckFilamentTriggerTimer)
    {
      ulCheckFilamentTriggerTimer = OS_GetTimeMs() + 1000;
      mustStoreCmd("M119\n");  
    }
  }
}


static void wCheckFilamentOpenToTrigger(void)
{
  wSendGcodeAutoFilamentLoad();
}



static void wPreheatFromFirstLayerOffsetDialogboxUserSaidContinue(void)
{
  bPreheatAskFilamentTypeFlag = TRUE;
  infoMenu.menu[++infoMenu.cur] = menuPreheat;
}



static void wReturnFirstLayerOffsetUserSaidRestart(void)
{
  infoMenu.menu[infoMenu.cur] = menuFirst_Layer;
}


void menuFirst_Layer(void)
{
  
  bDontDimLCD = TRUE;

  menuDrawPage(&First_LayerItems);

  bCalledByFirstLayerFlag = TRUE;  

  mustStoreCmd("M119\n");  
  setDialogText(LABEL_FIRST_LAYER_OFFSET, LABEL_FIRST_LAYER_DIALOG_V1, LABEL_CONTINUE, LABEL_CANCEL);
  showDialog(DIALOG_TYPE_INFO, wPingIRSensorFromFirstLayerOffset, wReturnFirstLayerScreenUserSaidCancel, NULL);

  while (infoMenu.menu[infoMenu.cur] == menuFirst_Layer)
  {

    loopProcess();
  }
}
