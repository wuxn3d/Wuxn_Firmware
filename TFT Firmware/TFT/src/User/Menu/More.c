#include "includes.h"
#include "More.h"

const MENUITEMS moreItems = {
  
  LABEL_MORE,
  
  {
    {ICON_HEAT,                    LABEL_HEAT},
    {ICON_FAN_FULL_SPEED,          LABEL_FAN},

    
    {ICON_PERCENTAGE,              LABEL_PERCENTAGE},

    
    {ICON_BACKGROUND,                 LABEL_BACKGROUND},

    
    {ICON_BACKGROUND,                 LABEL_BACKGROUND},

    
    {ICON_BACKGROUND,                 LABEL_BACKGROUND},

    #ifdef LOAD_UNLOAD_M701_M702
      
      {ICON_BACKGROUND,                 LABEL_BACKGROUND},
    #else
      {ICON_GCODE,                   LABEL_TERMINAL},
    #endif
    {ICON_BACK,                    LABEL_BACK},
  }
};


const MENUITEMS moreItemsPaused = {
  
  LABEL_MORE,
  
  {
    {ICON_HEAT,                    LABEL_HEAT},
    {ICON_FAN_FULL_SPEED,          LABEL_FAN},
    {ICON_PERCENTAGE,              LABEL_PERCENTAGE},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    /* 120522 menu change
    {ICON_EXTRUDE,                 LABEL_EXTRUDE},  
    {ICON_EXTRUDE,                 LABEL_LOAD_UNLOAD_SHORT},  
    */
    {ICON_BACKGROUND,              LABEL_BACKGROUND},  
    {ICON_BACKGROUND,              LABEL_BACKGROUND},  
    
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACK,                    LABEL_BACK},

  }
};

void isPauseExtrude(void)
{
  if (setPrintPause(true, false))
    infoMenu.menu[infoMenu.cur] = menuExtrude;
}

void isPauseLoadUnload(void)
{
  if (setPrintPause(true, false))
    infoMenu.menu[infoMenu.cur] = menuLoadUnload;
}

void menuMore(void)
{
  KEY_VALUES key_num;

  if(!isPause())  
    menuDrawPage(&moreItems);
  else  
    menuDrawPage(&moreItemsPaused); 


  while (infoMenu.menu[infoMenu.cur] == menuMore)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuHeat;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuFan;
        break;

      case KEY_ICON_2:  
        infoMenu.menu[++infoMenu.cur] = menuSpeed;
        break;

      /* 120522 menu change
      case KEY_ICON_4: 
        
        if (isPause()) 
        {
          
          
          break;  
        }
        else
        {
          infoMenu.menu[++infoMenu.cur] = menuExtrude;
        }
        break;
       */
      

      
        
        

      /* 120522 menu change
      case KEY_ICON_5:  
        #ifdef LOAD_UNLOAD_M701_M702
          
          if (!isPause())  
          {
            
            
            break;
          }
          else
          {
            infoMenu.menu[++infoMenu.cur] = menuLoadUnload;
          }
        #else
          infoMenu.menu[++infoMenu.cur] = menuTerminal;
        #endif
        break;
        */
       

        
        
        

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();
  }
}
