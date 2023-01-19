#include "CaseLight.h"
#include "includes.h"

const ITEM itemCaseLight[2] = {
  
  {ICON_BACKGROUND,              LABEL_BACKGROUND},
  {ICON_BACKGROUND,              LABEL_BACKGROUND},
};

static inline void updateCaseLightIcon(MENUITEMS * curmenu, bool state)
{
  curmenu->items[KEY_ICON_4] = itemCaseLight[state ? 1 : 0];
}

void caseLightBrightnessReDraw()
{
  char tempstr[20];
  sprintf(tempstr, "  %d%%  ", caseLightGetBrightnessPercent());
  setLargeFont(true);
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *)tempstr);
  setLargeFont(false);
}

void menuCaseLight(void)
{
  
  MENUITEMS caseLightItems = {
    
    LABEL_CASE_LIGHT,
    
    {
      {ICON_DEC,                     LABEL_DEC},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_INC,                     LABEL_INC},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  
  caseLightValueQuery();

  bool currentCaseLightState = caseLightGetState();
  bool previousCaseLightState = currentCaseLightState;
  uint8_t currentCaseLightBrightness = caseLightGetBrightness();
  uint8_t previousCaseLightBrightness = currentCaseLightBrightness;

  updateCaseLightIcon(&caseLightItems, currentCaseLightState);
  menuDrawPage(&caseLightItems);
  caseLightBrightnessReDraw();

  while (infoMenu.menu[infoMenu.cur] == menuCaseLight)
  {
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      case KEY_ICON_0:
        caseLightChangeBrightnessPrecent(-10);
        caseLightBrightnessReDraw();
        break;

      case KEY_ICON_3:
        caseLightChangeBrightnessPrecent(10);
        caseLightBrightnessReDraw();
        break;

      case KEY_ICON_4:
        caseLightToggleState();
        menuDrawPage(&caseLightItems);
        caseLightBrightnessReDraw();
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            caseLightChangeBrightnessPrecent(encoderPosition);
            caseLightBrightnessReDraw();
            encoderPosition = 0;
          }
        #endif
        break;
    }

    currentCaseLightState = caseLightGetState();
    if (previousCaseLightState != currentCaseLightState)
    {
      
      previousCaseLightState = currentCaseLightState;
      updateCaseLightIcon(&caseLightItems, currentCaseLightState);
      menuDrawItem(&caseLightItems.items[KEY_ICON_4], KEY_ICON_4);
      caseLightBrightnessReDraw();
    }

    currentCaseLightBrightness = caseLightGetBrightness();
    if (previousCaseLightBrightness != currentCaseLightBrightness)
    {
      previousCaseLightBrightness = currentCaseLightBrightness;
      caseLightBrightnessReDraw();
    }

    loopProcess();
  }
}
