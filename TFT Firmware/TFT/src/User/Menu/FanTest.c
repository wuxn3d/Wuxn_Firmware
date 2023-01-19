#include "includes.h"

#ifndef WUXN_CODE  

#include "FanTest.h"




FanTestStates FanTestState = s_FanTest1;

#define PartFanIndex 0
#define HotendFanIndex 2

char tempMsg[120];



void FanTestUserSaidNo();

const MENUITEMS FanTestItems = {
  
  LABEL_FAN_TEST,
  
  {
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    #ifdef WUXN_CODE
    {ICON_BACKGROUND,              LABEL_BACKGROUND},  
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    #else
    {ICON_START_TEST,              LABEL_START},  
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_NEXT,                    LABEL_NEXT},
    #endif
        
    {ICON_BACK,                    LABEL_BACK},
  }
};

void FanTestYesNoDialog()
{
  setDialogText(FanTestItems.title.index, (uint8_t *) tempMsg, LABEL_YES, LABEL_NO);
  showDialog(DIALOG_TYPE_QUESTION, FanTestUserSaidYes, FanTestUserSaidNo, NULL);
}

void FanTestBackDialog()
{
  setDialogText(FanTestItems.title.index, (uint8_t *) tempMsg, LABEL_BACKGROUND, LABEL_BACK);
  showDialog(DIALOG_TYPE_QUESTION, NULL, FanTestUserSaidNo, NULL);
}

void FanTestUserSaidYes(void) {
  switch (FanTestState)
  {
  case s_FanTest1:  
    FanTestState = s_FanTest1Completed;
    fanSetSpeed(PartFanIndex, infoSettings.fan_max[PartFanIndex]); 
    FanTestYesNoDialog();
    showDialog(DIALOG_TYPE_QUESTION, FanTestUserSaidYes, FanTestUserSaidNo, NULL);
    break;

  case s_FanTest1Completed: 
    FanTestState = s_FanTest2;
    fanSetSpeed(PartFanIndex, 0); 
    fanSetSpeed(HotendFanIndex, infoSettings.fan_max[HotendFanIndex]); 
    sprintf(tempMsg, "%s %s\n%s %s", textSelect(LABEL_PART_FAN), textSelect(LABEL_PASSED_TEST),textSelect(LABEL_HOTEND_FAN),textSelect(LABEL_IS_NOW_RUNNING));
    FanTestYesNoDialog();
    break;
  
  case s_FanTest2: 
    FanTestState = s_ExitFanTesting;
    fanSetSpeed(HotendFanIndex, 0); 
    
    sprintf(tempMsg, "%s %s", textSelect(LABEL_HOTEND_FAN), textSelect(LABEL_PASSED_TEST));
    FanTestBackDialog();
    break;

  default:
    break;
  }
}

void FanTestUserSaidNo(void) {
  switch (FanTestState)
  {
  case s_FanTest1Completed: 
    FanTestState = s_ExitFanTesting;
    fanSetSpeed(PartFanIndex, 0); 
    sprintf(tempMsg, "%s %s",textSelect(LABEL_PART_FAN), textSelect(LABEL_FAILED_TEST_REFER));  
    FanTestBackDialog();
    break;

  case s_FanTest2: 
    FanTestState = s_ExitFanTesting;
    fanSetSpeed(HotendFanIndex, 0); 
    sprintf(tempMsg, "%s %s", textSelect(LABEL_HOTEND_FAN), textSelect(LABEL_FAILED_TEST_REFER) );  
    FanTestBackDialog();
    break;

  case s_ExitFanTesting:
    infoMenu.cur--;
    break;

  default:
    break;
  }
}

void menuFanTest(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  
  menuDrawPage(&FanTestItems);

  while (infoMenu.menu[infoMenu.cur] == menuFanTest)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_4:  
        FanTestState = s_FanTest1;
        FanTestUserSaidYes(); 
        break;

      case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuProbeTest;
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

#endif 
