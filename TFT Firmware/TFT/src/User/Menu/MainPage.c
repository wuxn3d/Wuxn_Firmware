#include "MainPage.h"
#include "includes.h"


bool fPreheatFromMainMenu = FALSE;  
extern TOOLPREHEAT whichHeatersToHeat;  

void menuMain(void)
{
  extern bool bPreheat_DisplayPreheatMenu; 

  
  MENUITEMS mainPageItems = {
    
    LABEL_MAINMENU,
    
    {
      
      {ICON_PREHEATS,                LABEL_PREHEATS},   


      {ICON_HOME_MOVE,               LABEL_UNIFIEDMOVE},
      #ifdef LOAD_UNLOAD_M701_M702
        {ICON_EXTRUDE,                 LABEL_LOAD_UNLOAD_SHORT},
      #else
        {ICON_EXTRUDE,                 LABEL_EXTRUDE},
      #endif
      {ICON_WIZARDS,                 LABEL_WIZARDS},

      
      {ICON_SETTINGS,                LABEL_SETTINGS}, 

      
      {ICON_BACKGROUND,               LABEL_BACKGROUND}, 

      
      {ICON_BACKGROUND,               LABEL_BACKGROUND}, 

      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  if (infoSettings.rrf_macros_enable)
  {
    mainPageItems.items[5].label.index = LABEL_MACROS;
  }

  if (infoSettings.status_screen != 1)
  {
    mainPageItems.items[7].icon = ICON_PRINT;
    mainPageItems.items[7].label.index = LABEL_PRINT;
  }

  menuDrawPage(&mainPageItems);

  while (infoMenu.menu[infoMenu.cur] == menuMain)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        
        bPreheat_DisplayPreheatMenu = TRUE; 
        fPreheatFromMainMenu = TRUE; 
        whichHeatersToHeat = BOTH;  
        infoMenu.menu[++infoMenu.cur] = menuPreheat; 
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuUnifiedMove;
        break;

      case KEY_ICON_2:
        #ifdef LOAD_UNLOAD_M701_M702
          infoMenu.menu[++infoMenu.cur] = menuLoadUnload;
        #else
          infoMenu.menu[++infoMenu.cur] = menuExtrude;
        #endif
        break;

      case KEY_ICON_3: 
        infoMenu.menu[++infoMenu.cur] = menuWizards;
        break;

      case KEY_ICON_4:
        
        infoMenu.menu[++infoMenu.cur] = menuSettings;  

        break;

      case KEY_ICON_5:
        
        break;

      case KEY_ICON_6:
        
        break;

      case KEY_ICON_7:
        if (infoSettings.status_screen != 1)
          infoMenu.menu[++infoMenu.cur] = menuPrint;
        else
          ReturnToReadyMenu();
        break;

      default:
        break;
    }

    loopProcess();
  }
}

