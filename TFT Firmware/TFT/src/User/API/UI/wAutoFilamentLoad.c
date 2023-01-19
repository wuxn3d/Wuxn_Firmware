#include "wAutoFilamentLoad.h" //062622 don't need this line since wAutoFilamentLoad.h is in includes.h
#include "includes.h"

//bool f_wAutoFilamentLoadTime = false;
bool f_wautoFilamentLoadSentMe = true;
bool f_suspendLoopwAutoFilament = false;
//bool f_wAutoFilamentPreheat = false;
extern TOOLPREHEAT whichHeatersToHeat;  // 07022

// 062622 moved to wAutoFialmentLoad.h  enum wAutoFilamentStates {s_wAutoFilamentNotReady, s_wAutoFilamentLoadReady, s_wAutoFilamentPreheat, s_wAutoFilamentPurge,
//                          s_wAutoFilamentCheckColor, s_wAutoFilamentColorCorrect };
enum wAutoFilamentStates wAutoFilamentState = s_wAutoFilamentNotReady;


// 071722 no longer users
/*
const MENUITEMS wAutoFilamentLoadItems = {
  // title
  LABEL_LOAD_UNLOAD,
  // icon                          label
  {
    {ICON_UNLOAD,                  LABEL_UNLOAD},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_LOAD,                    LABEL_LOAD},
    {ICON_NOZZLE,                  LABEL_NOZZLE},
    {ICON_HEAT,                    LABEL_HEAT},
    {ICON_COOLDOWN,                LABEL_COOLDOWN},
    {ICON_BACK,                    LABEL_BACK},
  }
};
*/

KEY_VALUES  key_num;

bool FilamentPresent(void)  // we do it this way so we can replace the test method if needed
{
    return (!FIL_IsRunout());
}

void initwAutoFilamentLoad (void)
{
    if(!FilamentPresent()) // must not be any filament
    {
      wAutoFilamentState = s_wAutoFilamentLoadReady;
    }
    else
    {
      wAutoFilamentState = s_wAutoFilamentNotReady;
    }
}


void loopwAutoFilament (void)
{
  whichHeatersToHeat = BOTH; // 070222
  switch (wAutoFilamentState)
  {
    case s_wAutoFilamentLoadReady:
      if(FilamentPresent()) // transition from no filament to filament
      {
        if(isPrinting())  // we will skip the preheat step since the filament type will not change
          wAutoFilamentState = s_wAutoFilamentPurge;
        else
          wAutoFilamentState = s_wAutoFilamentPreheat;

        mustStoreCmd("G1 F100 E90\n");  // advance filament 90mm
        mustStoreCmd("G1 F1000\n");  // reset feed speed
        //mustStoreCmd("M400\n");  // wait for last M-command to complete
        //mustStoreCmd("M220 S100\n"); // reset feed rates to 100%
        // now we'll use the preheats menu to set the nozzle temperature
      }
      else
      {
        initwAutoFilamentLoad();  // check for no filament
      }
      break;

    case s_wAutoFilamentPreheat:
      f_wAutoFilamentLoadSentMe = true;
      f_suspendLoopwAutoFilament = true;

      menuPreheat();
      f_wAutoFilamentLoadSentMe = false;
      f_suspendLoopwAutoFilament = false;  // 012322 to fix not loading of filament after OOBWCW withou restart
      wAutoFilamentState = s_wAutoFilamentPurge;
    break;

    case s_wAutoFilamentPurge:

    break;

    case s_wAutoFilamentColorCorrect:

    break;

    default:
    break;
  }

}



