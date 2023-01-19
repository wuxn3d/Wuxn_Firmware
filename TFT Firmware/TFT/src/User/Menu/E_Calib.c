#include "E_Calib.h"
#include "includes.h"

const MENUITEMS E_CalibItems = {
  
  LABEL_E_CALIB,
  
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


void menuE_Calib(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&E_CalibItems);

  while (infoMenu.menu[infoMenu.cur] == menuE_Calib)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();
  }
}
