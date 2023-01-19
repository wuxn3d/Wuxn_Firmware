#include "Tuning.h"
#include "includes.h"

const MENUITEMS TuningItems = {
  
  LABEL_TUNING,
  
  {
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACK,                    LABEL_BACK},
  }
};

void menuTuning(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&TuningItems);

  while (infoMenu.menu[infoMenu.cur] == menuTuning)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuPid;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuEstepsPhase1;
        break;

      case KEY_ICON_2:
        storeCmd("M206\n");
        zOffsetSetMenu(false);  
        infoMenu.menu[++infoMenu.cur] = menuZOffset;
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
