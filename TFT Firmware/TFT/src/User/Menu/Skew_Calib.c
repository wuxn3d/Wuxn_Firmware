#include "Skew_Calib.h"
#include "includes.h"

const MENUITEMS Skew_CalibItems = {
  
  LABEL_SKEW_CALIB,
  
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


void menuSkew_Calib(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&Skew_CalibItems);

  while (infoMenu.menu[infoMenu.cur] == menuSkew_Calib)
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
