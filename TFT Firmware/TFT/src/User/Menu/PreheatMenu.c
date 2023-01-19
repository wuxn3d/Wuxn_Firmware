#include "PreheatMenu.h"
#include "includes.h"

extern HEATER heater;  
TOOLPREHEAT whichHeatersToHeat = BOTH;  
extern bool fPreheatFromMainMenu;  
extern bool bPreheatLoopCheck_AchievedTemperature;           
extern bool bPreheatAskFilamentTypeFlag;    
extern bool bFilamentPrevState;
extern bool bFilamentLoadGCodeSent;
extern bool f_wAutoFilamentLoadSentMe;  
extern bool f_bedLevelingSentMe;  
extern bool bCalledByFirstLayerFlag;    
extern bool bFirstLayerPatternFlag;
extern uint8_t lastcmd;  
extern uint8_t curExt_index;  

bool bLongPurge = TRUE;               

extern const GUI_POINT printinfo_points[6];  
extern const GUI_RECT printinfo_val_rect[6]; 

uint32_t ulPreHeatCheckTimer = 0;
bool bFilamentStateCheckFlag = FALSE;        
bool bFilamentBootStateUpdate = FALSE;       
bool bPreheat_DisplayPreheatMenu = FALSE;    
uint16_t unIndexDisplayIcon = 0;
bool bBabyStepStartDialogFlag = FALSE;       
static bool bRaisedHeadOnceInPurge = FALSE;         
extern bool bDontDimLCD;
extern bool bCalledByEStepsFlag;
extern bool f_estepsSentMe;  

HEATER nowHeat; 


                                                                                 
                                                                                 

                                                                
#define PREHEATICONABOVELOWERPOPUPEDGE (20 + PICON_HEIGHT) 
#define PREHEATICON_Y (ICON_HEIGHT + 30) 
const GUI_RECT preheatIconValRect[2] = {  
  {PREHEATNOZICON_X + PICON_VAL_X,     PREHEATICON_Y + PICON_VAL_Y,
   PREHEATNOZICON_X + PICON_VAL_LG_EX, PREHEATICON_Y + PICON_VAL_Y + BYTE_HEIGHT},

  {PREHEATBEDICON_X + PICON_VAL_X,     PREHEATICON_Y + PICON_VAL_Y,
   PREHEATBEDICON_X + PICON_VAL_LG_EX, PREHEATICON_Y + PICON_VAL_Y + BYTE_HEIGHT},
};

static void wPreheatDialogbox(void);
static void wPurgeDialogUserSaidYes(void);
static void wPreheatDialogSelectFilamentType(void);
static void wSelectFilamentTypeUserSaidNext(void);
static void wUnloadDialog(void);

const GUI_POINT preheat_title = {ICON_WIDTH / 2, PREHEAT_TITLE_Y };
const GUI_POINT preheat_val_tool = {ICON_WIDTH - BYTE_WIDTH / 2, PREHEAT_TOOL_Y};
const GUI_POINT preheat_val_bed = {ICON_WIDTH - BYTE_WIDTH / 2, PREHEAT_BED_Y};


/*
const ITEM itemToolPreheat[] = {
  
  {ICON_PREHEAT_BOTH,            LABEL_PREHEAT_BOTH},
  {ICON_BED,                     LABEL_BED},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
};
*/

MENUITEMS preheatItems = {
   
  LABEL_PREHEATS,
  
   {
    {ICON_PREHEAT,                 LABEL_BACKGROUND},
    {ICON_PREHEAT,                 LABEL_BACKGROUND},
    {ICON_PREHEAT,                 LABEL_BACKGROUND},
    {ICON_PREHEAT,                 LABEL_BACKGROUND},
    {ICON_PREHEAT,                 LABEL_BACKGROUND},
    {ICON_PREHEAT,                 LABEL_BACKGROUND},
    
    
    {ICON_HEAT,                    LABEL_CUSTOM},  
    {ICON_BACK,                    LABEL_BACK},
  }
};

const MENUITEMS Preheat_blankMenuItems = {
  
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


void refreshPreheatIcon(int8_t preheatnum, int8_t icon_index, const ITEM * menuitem)
{
  STRINGS_STORE preheatnames;
  W25Qxx_ReadBuffer((uint8_t*)&preheatnames,STRINGS_STORE_ADDR,sizeof(STRINGS_STORE));

  LIVE_INFO lvIcon;
  lvIcon.enabled[0] = true;
  lvIcon.enabled[1] = true;
  lvIcon.enabled[2] = true;

  
  lvIcon.lines[0].h_align = CENTER;
  lvIcon.lines[0].v_align = TOP;
  lvIcon.lines[0].fn_color = WHITE;
  lvIcon.lines[0].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[0].pos = preheat_title;
  lvIcon.lines[0].large_font = false;

  
  lvIcon.lines[1].h_align = RIGHT;
  lvIcon.lines[1].v_align = CENTER;
  lvIcon.lines[1].fn_color = WHITE;
  lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[1].pos = preheat_val_tool;
  lvIcon.lines[1].large_font = false;

  
  lvIcon.lines[2].h_align = RIGHT;
  lvIcon.lines[2].v_align = CENTER;
  lvIcon.lines[2].fn_color = WHITE;
  lvIcon.lines[2].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[2].pos = preheat_val_bed;
  lvIcon.lines[2].large_font = false;

  lvIcon.lines[0].text = (uint8_t *)preheatnames.preheat_name[preheatnum];

  char temptool[5];
  char tempbed[5];
  sprintf(temptool, "%d", infoSettings.preheat_temp[preheatnum]);
  sprintf(tempbed, "%d", infoSettings.preheat_bed[preheatnum]);
  lvIcon.lines[1].text = (uint8_t *)temptool;
  lvIcon.lines[2].text = (uint8_t *)tempbed;

  showLiveInfo(icon_index, &lvIcon, menuitem);
}

bool wQuickTemperatureCheck(void)
{
  
  if (inRange(heater.T[heatGetCurrentHotend()].current, heater.T[heatGetCurrentHotend()].target, 2) == true) 
      
  {
    heater.T[heatGetCurrentHotend()].status = SETTLED;
    heater.T[BED].status = SETTLED;
    BUZZER_PLAY(sound_heated);
    bPreheatLoopCheck_AchievedTemperature = TRUE;
    return TRUE;
  }
  else
    return FALSE;
}

void goBackToBedLeveling(void)
{
  infoMenu.menu[infoMenu.cur] = menuBedLevelingPhase2;  
  loopProcess();
}

void goBackToEsteps(void)  
{
  infoMenu.menu[infoMenu.cur] = menuEstepsPhase2;  
  loopProcess();
}

void TemperatureAchieved(void)
{
  bPreheatLoopCheck_AchievedTemperature = FALSE;                                          
  setDialogText(LABEL_PREHEAT, LABEL_TEMPERATURE_ACHIEVED, LABEL_NEXT, LABEL_BACKGROUND); 

  if (fPreheatFromMainMenu == TRUE)
  {
    fPreheatFromMainMenu = FALSE;
    
    showDialog(DIALOG_TYPE_INFO, ReturnToMainMenu, NULL, NULL); 
  }
  else if (f_bedLevelingSentMe == TRUE)  
  {
    f_bedLevelingSentMe = FALSE;  
    showDialog(DIALOG_TYPE_INFO, goBackToBedLeveling, NULL, NULL); 
  }
  else if (bCalledByEStepsFlag == TRUE)  
  {
    bCalledByEStepsFlag = FALSE;
    showDialog(DIALOG_TYPE_INFO, goBackToEsteps, NULL, NULL);
  }
  else 
  {
    if (lastcmd == lastcmd_UNLOAD) 
    {
      lastcmd = lastcmd_NONE; 
      
      showDialog(DIALOG_TYPE_INFO, wUnloadDialog, NULL, NULL); 
    }
    else 
    {
      lastcmd = lastcmd_NONE; 
      
      showDialog(DIALOG_TYPE_INFO, wPurgeDialog, NULL, NULL);
    }
    
  }
  unIndexDisplayIcon = 0;
}

void CancelFromHeating (void)  
{
  TurnOffHeatersAndFans();
  ResetMenuStack();  
}

void wDisplayHeatingScreen(void)
{
  if(bPreheat_DisplayPreheatMenu == true)  
  {
    bPreheat_DisplayPreheatMenu = false;
  }

  bPreheatLoopCheck_AchievedTemperature = FALSE; 
  menuDrawPage(&Preheat_blankMenuItems); 

    
  if(wQuickTemperatureCheck())
  {
    TemperatureAchieved();
  }
  else
  {
   
    setDialogText(LABEL_PREHEAT, LABEL_WARMING_UP,LABEL_CANCEL_NO_SYMBOL, LABEL_BACKGROUND);
    showDialog(DIALOG_TYPE_INFO, CancelFromHeating, NULL, wPreheatDialogbox);
  }
}

void menuPreheat(void)
{
  
  KEY_VALUES  key_num;

  menuDrawPage(&preheatItems);
  for (int i = 0; i < PREHEAT_COUNT; i++)
  {
    refreshPreheatIcon(i, i, &preheatItems.items[i]);
  }

  wPreheatDialogSelectFilamentType();  

  while (infoMenu.menu[infoMenu.cur] == menuPreheat)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
      case KEY_ICON_1:
      case KEY_ICON_2:
      case KEY_ICON_3:
      case KEY_ICON_4:
      case KEY_ICON_5:
        {
          refreshPreheatIcon(key_num, key_num, &preheatItems.items[key_num]);

          if(bPreheat_DisplayPreheatMenu == TRUE)
          {
            switch (whichHeatersToHeat)
            {
              case BOTH:
                heatSetTargetTemp(BED, infoSettings.preheat_bed[key_num]);
                heatSetTargetTemp(heatGetCurrentHotend(), infoSettings.preheat_temp[key_num]);
                  if(f_wAutoFilamentLoadSentMe)
                  {
                    return;  
                  }
              break;

              case BED_PREHEAT:
                heatSetTargetTemp(BED, infoSettings.preheat_bed[key_num]);
                break;

              case NOZZLE0_PREHEAT:
                heatSetTargetTemp(heatGetCurrentHotend(), infoSettings.preheat_temp[key_num]);
                break;
            }
            wDisplayHeatingScreen(); 
          }
        }

      case KEY_ICON_6:

        bPreheat_DisplayPreheatMenu = true; 
        infoMenu.menu[++infoMenu.cur] = menuHeat;
        break;

      case KEY_ICON_7:  
        if(f_wAutoFilamentLoadSentMe)
        {
          
          return; 
          break;
        }
        else
        {
          infoMenu.cur--;
          break;
        }

      default:
        break;
    }

    loopProcess();
  }
}



static void wPreheatDialogSelectFilamentType(void)
{
  if(bPreheatAskFilamentTypeFlag == TRUE)
  {
    setDialogText(LABEL_FIRST_LAYER_OFFSET, LABEL_FIRST_LAYER_DIALOG_V3, LABEL_NEXT, LABEL_BACKGROUND);
    showDialog(DIALOG_TYPE_INFO, wSelectFilamentTypeUserSaidNext, NULL, NULL);
  }
}


static void wSelectFilamentTypeUserSaidNext(void)
{
  bPreheatAskFilamentTypeFlag = FALSE;
}


void wSetZTriggerCheckFlag(bool bValue)
{
  if(bFilamentBootStateUpdate == FALSE ||
     bCalledByFirstLayerFlag == TRUE)  
  {
    bFilamentPrevState = bValue;
    bFilamentBootStateUpdate = TRUE;
  }

  bFilamentStateCheckFlag = bValue;
}

static void wReferToManualUnload(void)  
{
  setDialogText(LABEL_UNLOAD, LABEL_DIAGNOSTICS_ERROR, LABEL_BACKGROUND, LABEL_BACK);
  showDialog(DIALOG_TYPE_ALERT, NULL, ReturnToReadyMenu, NULL);
}


void wCoolDownAfterUnload(void)  
{
  setDialogText(LABEL_WARNING, LABEL_HEATERS_ON, LABEL_YES, LABEL_NO);
  showDialog(DIALOG_TYPE_QUESTION, heatCoolDown, NULL, NULL);
  ReturnToReadyMenu();
}


void wUnloadDialog(void)  
{
  bDontDimLCD = TRUE;
  mustStoreCmd("M702 T%d\n", curExt_index);  
 
 
  setDialogText(LABEL_UNLOAD, LABEL_UNLOAD_STARTED, LABEL_YES, LABEL_NO);
  
  showDialog(DIALOG_TYPE_QUESTION, wCoolDownAfterUnload, wReferToManualUnload, NULL);  
}



void wPurgeDialog(void)
{
  bDontDimLCD = TRUE;
  
  
  if(infoSettings.enable_oobwcw != ENABLE  &&
     bRaisedHeadOnceInPurge == FALSE)
  { 
    
    
    
    mustStoreCmd("G91\n"); 
    
    
    mustStoreCmd("G0 F4000 Z30\n");  
    bRaisedHeadOnceInPurge = TRUE;  
  }
  mustStoreCmd("M83\n");  
  if (bLongPurge == TRUE)
  {
    mustStoreCmd("G1 F100 E30\n");  
    bLongPurge = FALSE;  
  }
  else
  {
    mustStoreCmd("G1 F100 E15\n");  
  }
  mustStoreCmd("G90\n");  
  setDialogText(LABEL_PURGE, LABEL_PURGE_BOX, LABEL_YES, LABEL_NO);
  showDialog(DIALOG_TYPE_QUESTION, wPurgeDialogUserSaidYes, wPurgeDialog, NULL);
}


static inline void reDisplayNozzle(int icon_pos)
{
  char tempstr[10];
  sprintf(tempstr, "%d/%d", heatGetCurrentTemp(NOZZLE0), heatGetTargetTemp(NOZZLE0));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  ICON_ReadDisplay(PREHEATNOZICON_X, PREHEATICON_Y, ICON_PRINTING_NOZZLE);
  GUI_DispString(PREHEATNOZICON_X + PICON_TITLE_X, PREHEATICON_Y + PICON_TITLE_Y,
                 (uint8_t *)heatDisplayID[NOZZLE0]);
  GUI_DispStringInPrect(&preheatIconValRect[icon_pos], (uint8_t *)tempstr);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reDisplayBed(int icon_pos)
{
  char tempstr[10];
  sprintf(tempstr, "%d/%d", heatGetCurrentTemp(BED), heatGetTargetTemp(BED));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  ICON_ReadDisplay(PREHEATBEDICON_X, PREHEATICON_Y, ICON_PRINTING_BED);
  GUI_DispString(PREHEATBEDICON_X + PICON_TITLE_X, PREHEATICON_Y + PICON_TITLE_Y,
                 (uint8_t *)heatDisplayID[BED]);
  GUI_DispStringInPrect(&preheatIconValRect[icon_pos], (uint8_t *)tempstr);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}



static void wPreheatDialogbox(void)
{
  const uint16_t punIconDisplay[] = {ICON_TIMER1, ICON_TIMER2, ICON_TIMER3, ICON_TIMER4};

#define NOZ_ICON_POS 0 
#define BED_ICON_POS 1 

  bPreheat_DisplayPreheatMenu = FALSE;
  bFilamentLoadGCodeSent = FALSE;

  
  if (OS_GetTimeMs() > ulPreHeatCheckTimer)
  {
    ulPreHeatCheckTimer = OS_GetTimeMs() + 1000;
    ICON_ReadDisplay(INFOBOX_WIDTH + 10, ICON_HEIGHT + 30, punIconDisplay[unIndexDisplayIcon]);
    unIndexDisplayIcon++;
    if (unIndexDisplayIcon >= (sizeof(punIconDisplay) / sizeof(uint16_t)))
    {
      unIndexDisplayIcon = 0;
    }
  }
  loopCheckHeater();
  
  
  if (nowHeat.T[NOZZLE0].current != heatGetCurrentTemp(NOZZLE0) ||
      nowHeat.T[NOZZLE0].target != heatGetTargetTemp(NOZZLE0))
  {
    nowHeat.T[NOZZLE0].current = heatGetCurrentTemp(NOZZLE0);
    nowHeat.T[NOZZLE0].target = heatGetTargetTemp(NOZZLE0);
    RAPID_SERIAL_LOOP(); 
    reDisplayNozzle(NOZ_ICON_POS);
  }

  
  if (nowHeat.T[BED].current != heatGetCurrentTemp(BED) ||
      nowHeat.T[BED].target != heatGetTargetTemp(BED))
  {
    nowHeat.T[BED].current = heatGetCurrentTemp(BED);
    nowHeat.T[BED].target = heatGetTargetTemp(BED);
    RAPID_SERIAL_LOOP(); 
    reDisplayBed(BED_ICON_POS);
  }

  if (bPreheatLoopCheck_AchievedTemperature == TRUE) 
    TemperatureAchieved();
}


static void wPurgeDialogUserSaidYes(void)
{
  bRaisedHeadOnceInPurge = FALSE; 

  if (infoSettings.enable_oobwcw == ENABLE)
  {
    OOBWCW_Step_6(); 
  }
  else
  {
    if (bCalledByFirstLayerFlag == TRUE)
    {
      bBabyStepStartDialogFlag = TRUE;
      infoMenu.menu[++infoMenu.cur] = menuBabystep; 
                                                    
    }
    else
    {
      if (bCalledByEStepsFlag == TRUE)
      {
        bCalledByEStepsFlag = FALSE;
        wEstepsShowInstructions1();
      }
      else
      {
        
        ReturnToReadyMenu(); 
      }
    }
  }
}