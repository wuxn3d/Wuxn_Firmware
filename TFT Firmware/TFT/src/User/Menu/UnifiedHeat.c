#include "UnifiedHeat.h"
#include "includes.h"

extern bool bPreheat_DisplayPreheatMenu;

const MENUITEMS UnifiedHeatItems = {
  
  LABEL_UNIFIEDHEAT,
  
  {
    {ICON_PREHEATS,                LABEL_PREHEATS},
    {ICON_HEAT,                    LABEL_HEAT},
    {ICON_FAN_FULL_SPEED,          LABEL_FAN},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_COOLDOWN,                LABEL_COOLDOWN},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACK,                    LABEL_BACK},
  }
};

void menuUnifiedHeat(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  extern TOOLPREHEAT whichHeatersToHeat;  

  menuDrawPage(&UnifiedHeatItems);

  while (infoMenu.menu[infoMenu.cur] == menuUnifiedHeat)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        whichHeatersToHeat = BOTH;  
        infoMenu.menu[++infoMenu.cur] = menuPreheat;
        bPreheat_DisplayPreheatMenu = TRUE;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuHeat;
        break;

      case KEY_ICON_2:
        infoMenu.menu[++infoMenu.cur] = menuFan;
        break;

      case KEY_ICON_5:
        heatCoolDown();
        break;

      case KEY_ICON_7:
        ReturnToReadyMenu();
        break;

      default:
        break;
    }

    loopProcess();
  }
}
