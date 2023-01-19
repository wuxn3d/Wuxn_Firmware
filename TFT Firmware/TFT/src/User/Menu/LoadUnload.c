#include "LoadUnload.h"
#include "includes.h"

/*  062622
#define NONE   0
#define LOAD   1
#define UNLOAD 2
*/

extern bool bPreheat_DisplayPreheatMenu;  
extern TOOLPREHEAT whichHeatersToHeat;  
extern bool bFilamentStateCheckFlag;  
extern bool  bPreheatLoopCheck_AchievedTemperature;  

const MENUITEMS loadUnloadItems = {
  
  LABEL_LOAD_UNLOAD,
  
  {
    {ICON_UNLOAD,                  LABEL_UNLOAD},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_LOAD,                    LABEL_LOAD},

    
    {ICON_BACKGROUND,              LABEL_BACKGROUND}, 

    
    {ICON_BACKGROUND,              LABEL_BACKGROUND}, 

    
    {ICON_BACKGROUND,              LABEL_BACKGROUND}, 

    {ICON_BACK,                    LABEL_BACK},
  }
};


uint8_t curExt_index = 0;  

uint8_t lastcmd = lastcmd_NONE;  

bool bCameFromLoadUnload = FALSE; 

void extruderIdReDraw(void)
{
  char tempstr[20];

  sprintf(tempstr, "%2s: %3d/%-3d", heatDisplayID[curExt_index], heatGetCurrentTemp(curExt_index), heatGetTargetTemp(curExt_index));
  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *)tempstr);
  setLargeFont(false);
}


void loadMinTemp_OK(void)
{
  heatSetTargetTemp(curExt_index, infoSettings.min_ext_temp);
}

void waitForFilamentLoad(void)  
{
  while(FilamentPresent() == false);  
  return;
}

void menuLoadUnload(void)
{
  KEY_VALUES key_num = KEY_IDLE;



  while (infoCmd.count != 0)
  {
    loopProcess();
  }

  menuDrawPage(&loadUnloadItems);
  extruderIdReDraw();

  while (infoMenu.menu[infoMenu.cur] == menuLoadUnload)
  {
    key_num = menuKeyGetValue();

    if (infoHost.wait == true && key_num != KEY_IDLE &&
        key_num != KEY_ICON_7)  
    {
      if (lastcmd == lastcmd_UNLOAD)
      { 
        popupReminder(DIALOG_TYPE_INFO, LABEL_UNLOAD, LABEL_UNLOAD_STARTED);
      }
      else if (lastcmd ==lastcmd_LOAD)
      { 
        popupReminder(DIALOG_TYPE_INFO, LABEL_LOAD, LABEL_LOAD_STARTED);
      }
      else
      { 
        popupReminder(DIALOG_TYPE_INFO, LABEL_SCREEN_INFO, LABEL_BUSY);
      }
    }
    else
    {
        switch (key_num)
        {
            case KEY_ICON_0:  

              
              lastcmd = lastcmd_UNLOAD;
              whichHeatersToHeat = NOZZLE0_PREHEAT; 
              bPreheatLoopCheck_AchievedTemperature = FALSE;  
              bPreheat_DisplayPreheatMenu = TRUE;  
              infoMenu.menu[++infoMenu.cur] = menuPreheat;
              break;


            case KEY_ICON_3:  
              
              lastcmd = lastcmd_LOAD;
              whichHeatersToHeat = BOTH; 
              bPreheatLoopCheck_AchievedTemperature = FALSE;  

              
              mustStoreCmd("M119\n");  
              loopProcess();  
              if(bFilamentStateCheckFlag == TRUE)  
              {
                setDialogText(LABEL_LOAD,LABEL_FILAMENT_PRESENT,LABEL_NEXT, LABEL_BACKGROUND); 
                showDialog(DIALOG_TYPE_ERROR, menuLoadUnload, NULL, NULL);   
              }
              else  
              {
                setDialogText(LABEL_LOAD,LABEL_INSERT_FILAMENT,LABEL_BACKGROUND, LABEL_BACKGROUND); 
                showDialog(DIALOG_TYPE_ERROR, waitForFilamentLoad, NULL, NULL);   
                loopProcess(); 
              }
              break;

        case KEY_ICON_7:
          if (!isPrinting())
          {
            for (uint8_t i = 0; i < infoSettings.hotend_count; i++)
            {
              if (heatGetTargetTemp(i) > 0)
              {
                setDialogText(LABEL_WARNING, LABEL_HEATERS_ON, LABEL_YES, LABEL_NO);
                showDialog(DIALOG_TYPE_QUESTION, heatCoolDown, NULL, NULL);
                break;
              }
            }
          }
          lastcmd = lastcmd_NONE;
          
          infoMenu.cur--;  

          break;

        default:
          extruderIdReDraw();
          break;
      }
    }

    loopProcess();
  }
}
