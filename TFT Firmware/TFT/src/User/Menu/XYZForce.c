#include "XYZForce.h"
#include "includes.h"
#include "coordinate.h"

bool f_XYZForceTest = false;

enum XYZForceStates { s_idle = 0, s_readyForNextMove, s_waitingToConfirmMove, s_readyToTestCurrent, 
                       s_waitingForCurrentTest, s_XYZForceTestFailed, s_XYZForceTestPassed };
enum XYZForceStates XYZForceState = s_readyForNextMove;

AXIS axis = X_AXIS;  
float currentTarget = 0;

const int16_t MAX_STEPPER_DRIVER_CURRENT = 0; 
const int16_t MIN_STEPPER_DRIVER_CURRENT = 100; 
const uint8_t  MAX_NUMBER_OF_PASSES = 3;  
uint8_t pass = 1;
int16_t measuredDriverCurrent = 0;  


char tempMsg[120];

void clearUpperLabels (void)
{
  sprintf(tempMsg, "            ");
  GUI_DispString(START_X + 2 * SPACE_X + 2 * ICON_WIDTH, (ICON_START_Y - BYTE_HEIGHT) / 2,(uint8_t *)tempMsg);
  GUI_DispString(START_X + 3 * SPACE_X + 3 * ICON_WIDTH, (ICON_START_Y - BYTE_HEIGHT) / 2,(uint8_t *)tempMsg);
}




bool matchPosition(float position)
{
  float TOLERANCE = 0.02;  
  if(abs(coordinateGetAxisActual(axis) - position) <= TOLERANCE)
  {
    
    sprintf(tempMsg, "%s ", "Match");
    GUI_DispString(START_X + 3 * SPACE_X + 3 * ICON_WIDTH, (ICON_START_Y - BYTE_HEIGHT) / 2,(uint8_t *)tempMsg);
    
    return true;
  }
  else
    return false;
}



void XYZForceCleanup(void)
{
   mustStoreCmd("G0 Z5\n"); 
   mustStoreCmd("G28\n");   
  
  
  mustStoreCmd("M420 S\n");          
}

void updatePositionDisplay(void)
{
  if (nextScreenUpdate(500))
  {
    coordinateQuery();
    sprintf(tempMsg, "%c-Axis %.2f", axis_id[axis], coordinateGetAxisActual(axis));
    GUI_DispString(START_X + 2 * SPACE_X + 2 * ICON_WIDTH, (ICON_START_Y - BYTE_HEIGHT) / 2, (uint8_t *)tempMsg);
    if(matchPosition(250))
    {
      sprintf(tempMsg, "%s ", "Match");
      GUI_DispString(START_X + 3 * SPACE_X + 3 * ICON_WIDTH, (ICON_START_Y - BYTE_HEIGHT) / 2,(uint8_t *)tempMsg);
    }
  }
}

void XYZForceMove(void)
{
  coordinateSetAxisTarget(axis, 250.00);
  mustStoreCmd(wMoveCmd[axis], coordinateGetAxisTarget(axis));
  mustStoreCmd("M400\n");  

  coordinateSetAxisTarget(axis, 0.00);
  wConfirmedMove(axis, coordinateGetAxisTarget(axis));
}

bool checkCurrent()
{
  static int16_t averageDriverCurrent = 0;   
  int16_t measuredDriverCurrent = 0;  
  
  
  
  unsigned long checkCurrentTimer = 0;
  checkCurrentTimer = OS_GetTimeMs() + 5000;
  while(OS_GetTimeMs() <= checkCurrentTimer)
    {};



  averageDriverCurrent = (averageDriverCurrent + measuredDriverCurrent) / pass;
  if((averageDriverCurrent >= MIN_STEPPER_DRIVER_CURRENT) && (averageDriverCurrent <= MAX_STEPPER_DRIVER_CURRENT))  
  {
    averageDriverCurrent = 0; 
    pass = 1;
    return(true);
  }
  else
  {
    return(false); 
  }
}

void loopXYZForce()
{
  
  switch (XYZForceState)
  {
    case s_readyForNextMove:
    {
      sprintf(tempMsg, "Testing %c", axis_id[axis]);
      GUI_DispString(START_X + 2 * SPACE_X + 2 * ICON_WIDTH, (ICON_START_Y - BYTE_HEIGHT) / 2, (uint8_t *)tempMsg);
      XYZForceMove();
      XYZForceState = s_waitingToConfirmMove;
      break;
    }

    case s_waitingToConfirmMove: 

      if(coordinateQueryWait == false)
      {
        if(matchPosition(coordinateGetAxisTarget(axis)))
        {
          sprintf(tempMsg, "%c %s", axis_id[axis], "Match");
          GUI_DispString(START_X + 3 * SPACE_X + 3 * ICON_WIDTH, (ICON_START_Y - BYTE_HEIGHT) / 2,(uint8_t *)tempMsg);
          XYZForceState = s_readyToTestCurrent;
        }
        else
        {
          coordinateQuery();  
        }
      }
      break;

    case s_readyToTestCurrent:
      
      XYZForceState = s_waitingForCurrentTest;
      clearUpperLabels();
      break;

    case s_waitingForCurrentTest:
      if(!checkCurrent())  
      {
        if(++pass > MAX_NUMBER_OF_PASSES)  
        {
          XYZForceState = s_XYZForceTestFailed;
          f_XYZForceTest = false;  
          break;        
        }
        else 
        {
          XYZForceState = s_readyForNextMove;
          break;
        }
      }
      else 
      {
        XYZForceState = s_XYZForceTestPassed;  
        f_XYZForceTest = false;      
      }
      
    default:
      break; 
  }
}

void menuXYZForce(void)
{
  const MENUITEMS XYZForceItems = {
    
    LABEL_XYZFORCE,
    
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

  KEY_VALUES key_num = KEY_IDLE;
 
  menuDrawPage(&XYZForceItems);

  XYZForceCleanup();

  while (infoCmd.count || infoHost.wait)
  {
    loopProcess();  
  }


  while (infoMenu.menu[infoMenu.cur] == menuXYZForce)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        axis = X_AXIS;
        pass = 1;
        XYZForceState = s_readyForNextMove;
        f_XYZForceTest = true;
        break;

      case KEY_ICON_1:
        axis = Y_AXIS;
        pass = 1;
        XYZForceState = s_readyForNextMove;
        f_XYZForceTest = true;
        break;

      case KEY_ICON_2:
        axis = Z_AXIS;
        pass = 1;
        XYZForceState = s_readyForNextMove;
        f_XYZForceTest = true;
        break;

      /*
      case KEY_ICON_4:  
        axis = X_AXIS;  
        pass = 1;
        f_XYZForceTest = true;
        break;
      */

      case KEY_ICON_6:
        XYZForceCleanup();
        infoMenu.menu[++infoMenu.cur] = menuMotorTest;
        break;

      case KEY_ICON_7:
        XYZForceCleanup();
        infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();

    
    if(f_XYZForceTest)
    {
      loopXYZForce();
    }

    switch (XYZForceState)
    {
      case s_XYZForceTestPassed:
        
        clearUpperLabels();
        sprintf(tempMsg, "%c-Axis PASSED", axis_id[axis]);
        GUI_DispString(START_X + 2 * SPACE_X + 2 * ICON_WIDTH, (ICON_START_Y - BYTE_HEIGHT) / 2, (uint8_t *)tempMsg);
        break;

      case s_XYZForceTestFailed:
        
        clearUpperLabels();
        sprintf(tempMsg, "%c-Axis FAILED", axis_id[axis]);
        GUI_DispString(START_X + 2 * SPACE_X + 2 * ICON_WIDTH, (ICON_START_Y - BYTE_HEIGHT) / 2, (uint8_t *)tempMsg);
        break;

      default:
        break;
    }
    
  }
} 


