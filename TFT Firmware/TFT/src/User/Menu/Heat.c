#include "Heat.h"
#include "includes.h"
#include "Numpad.h"
#include "Settings.h"

static uint8_t degreeSteps_index = 2; 
static uint8_t c_heater = NOZZLE0;

extern bool bCameFromLoadUnload; 

void heatSetCurrentIndex(uint8_t index)
{
  c_heater = index;
}


void showTemperature(uint8_t index, bool skip_header)
{
  char tempstr[20];

  setLargeFont(true);

  if (!skip_header)
  {
    sprintf(tempstr, "%-15s", heatDisplayID[index]);
    setLargeFont(false);
    GUI_DispString(exhibitRect.x0, exhibitRect.y0, (uint8_t *)tempstr);
    setLargeFont(true);
    GUI_DispStringCenter((exhibitRect.x0 + exhibitRect.x1) >> 1, exhibitRect.y0, (uint8_t *)"ºC");
  }

  sprintf(tempstr, "%4d/%-4d", heatGetCurrentTemp(index), heatGetTargetTemp(index));
  GUI_DispStringInPrect(&exhibitRect, (uint8_t *)tempstr);
  setLargeFont(false);
}

void menuHeat(void)
{
  int16_t lastCurrent = heatGetCurrentTemp(c_heater);
  int16_t lastTarget = heatGetTargetTemp(c_heater);

  extern bool bPreheat_DisplayPreheatMenu;  

  
  const ITEM itemHeat[4] = {
    
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
   
    {ICON_STOP,                    LABEL_CANCEL_NO_SYMBOL},
    {ICON_BACK,                    LABEL_BACK},
    {ICON_SETTINGS,                LABEL_SET_LABEL}
  };

  MENUITEMS heatItems = {
    
    LABEL_HEAT,
    
    {
      {ICON_DEC,                     LABEL_DEC},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_INC,                     LABEL_INC},
      {ICON_NOZZLE,                  LABEL_NOZZLE},
      {ICON_5_DEGREE,                LABEL_5_DEGREE},
      {ICON_STOP,                    LABEL_STOP},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;  

  heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);

  heatItems.items[KEY_ICON_4] = itemTool[c_heater];
  heatItems.items[KEY_ICON_5] = itemDegreeSteps[degreeSteps_index];

  if (isPrinting())  
    heatItems.items[KEY_ICON_6] = itemHeat[0];  
  else  
    heatItems.items[KEY_ICON_6] = itemHeat[1];  

  
  if (bPreheat_DisplayPreheatMenu == true)
    heatItems.items[KEY_ICON_7] = itemHeat[3];  
  else
    heatItems.items[KEY_ICON_7] = itemHeat[2];  

  menuDrawPage(&heatItems);
  showTemperature(c_heater, false);


  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuHeat)
  {
    
    key_num = menuKeyGetValue();
    int16_t actCurrent = heatGetCurrentTemp(c_heater);
    int16_t actTarget = heatGetTargetTemp(c_heater);

    switch (key_num)
    {
      case KEY_ICON_0:
        heatSetTargetTemp(c_heater, actTarget - degreeSteps[degreeSteps_index]);
        break;

      case KEY_INFOBOX:
      {
        char titlestr[30];
        sprintf(titlestr, "Min:0 | Max:%i", infoSettings.max_temp[c_heater]);

        int16_t val = numPadInt((uint8_t *) titlestr, actTarget, 0, false);
        val = NOBEYOND(0, val, infoSettings.max_temp[c_heater]);

        if (val != actTarget)
          heatSetTargetTemp(c_heater, val);

        menuDrawPage(&heatItems);
        showTemperature(c_heater, false);
        break;
      }

      case KEY_ICON_3:
        heatSetTargetTemp(c_heater, actTarget + degreeSteps[degreeSteps_index]);
        break;

      case KEY_ICON_4:
        do
        {
          c_heater = (c_heater + 1) % MAX_HEATER_COUNT;
        } while (!heaterIsValid(c_heater));

        heatItems.items[key_num] = itemTool[c_heater];
        menuDrawItem(&heatItems.items[key_num], key_num);
        showTemperature(c_heater, false);
        break;

      case KEY_ICON_5:
        degreeSteps_index = (degreeSteps_index + 1) % ITEM_DEGREE_NUM;
        heatItems.items[key_num] = itemDegreeSteps[degreeSteps_index];
        menuDrawItem(&heatItems.items[key_num], key_num);
        break;

      case KEY_ICON_6:
        if (!isPrinting())  
        {
          heatSetTargetTemp(c_heater, 0);
          TurnOffHeatersAndFans();  
          ReturnToReadyMenu();  
        }
        break;

      case KEY_ICON_7:
        if (isPrinting())
        {
          infoMenu.cur--;
        }
        else if(bPreheat_DisplayPreheatMenu == true)  
        {
          wDisplayHeatingScreen();
        }

        
        else if (bCameFromLoadUnload == TRUE)
        { 
          bCameFromLoadUnload = FALSE;
          Delay_ms(50);
          infoMenu.cur = 0;
          infoMenu.menu[infoMenu.cur] = menuStatus;
          infoMenu.menu[++infoMenu.cur] = menuMain;
          infoMenu.menu[++infoMenu.cur] = menuLoadUnload;
        }
        else if (infoMenu.menu[(infoMenu.cur -1)] == menuStatus)
        {
          ReturnToReadyMenu();
        }
        else
        {
          
          Delay_ms(50);  
          infoMenu.cur--;  
        }

        break;

      default :
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            if (encoderPosition > 0)
              heatSetTargetTemp(c_heater, actTarget + degreeSteps[degreeSteps_index]);
            else  
              heatSetTargetTemp(c_heater, actTarget - degreeSteps[degreeSteps_index]);
            encoderPosition = 0;
          }
        #endif
        break;
    }

    if (lastCurrent != actCurrent || lastTarget != actTarget)
    {
      lastCurrent = actCurrent;
      lastTarget = actTarget;
      showTemperature(c_heater, true);
    }

    loopProcess();
  }

  
  if (heatHasWaiting() == false)
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
}
