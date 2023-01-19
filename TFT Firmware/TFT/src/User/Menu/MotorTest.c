#include "MotorTest.h"
#include "includes.h"

enum MotorTestStates { s_MotorTestIdle, s_MotorTestIntro, s_ExitMotorTest };
enum MotorTestStates MotorTestState = s_MotorTestIdle;

char tempMsg[120];


void MotorTestUserSaidYes();
void MotorTestUserSaidNo();

const MENUITEMS MotorTestItems = {
  
  LABEL_MOTORTEST,
  
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

void MotorTestYesNoDialog(void)
{
  setDialogText(MotorTestItems.title.index, (uint8_t *) tempMsg, LABEL_YES, LABEL_NO);
  showDialog(DIALOG_TYPE_QUESTION, MotorTestUserSaidYes, MotorTestUserSaidNo, NULL);
}

void MotorTestBackDialog(void)
{
  setDialogText(MotorTestItems.title.index, (uint8_t *) tempMsg, LABEL_BACKGROUND, LABEL_BACK);
  showDialog(DIALOG_TYPE_QUESTION, NULL, MotorTestUserSaidNo, NULL);
}

void MotorTestUserSaidYes(void)
{
  switch (MotorTestState)
  {
  case s_MotorTestIntro:  
    MotorTestState = s_ExitMotorTest;
    sprintf(tempMsg, "%s %s", textSelect(LABEL_MOTORTEST), textSelect(LABEL_PASSED_TEST));
    MotorTestBackDialog();
    break;

  default:
    break;
  }
}

void MotorTestUserSaidNo(void)
{
  switch (MotorTestState)
  {
  case s_MotorTestIntro: 
    MotorTestState = s_ExitMotorTest;
    sprintf(tempMsg, "%s %s", textSelect(LABEL_MOTORTEST), textSelect(LABEL_MOTORTEST_ADJUST));
    MotorTestBackDialog();
    break;

  case s_ExitMotorTest:
    infoMenu.cur--;
    break;

  default:
    break;
  }
}

void menuMotorTest(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&MotorTestItems);

  while (infoMenu.menu[infoMenu.cur] == menuMotorTest)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_4:  
        if(FilamentPresent() == true)
        {
          mustStoreScript("G0 Z5\nG28\n"); 
        }
        mustStoreScript("G0 X250\nM400\nG0 X0\n");
        mustStoreCmd("M400\n");
        mustStoreScript("G0 Y210\nM400\nG0 Y0\n");
        mustStoreCmd("M400\n");
        mustStoreCmd("G0 Y50\n"); 
        mustStoreScript("G0 Z250\nM400\nG0 Z0\n");
        mustStoreCmd("M400\n");
        
        
        mustStoreCmd("M420 S\n");
        MotorTestState = s_MotorTestIntro;
        sprintf(tempMsg, "%s", textSelect(LABEL_MOTORTEST_INTRO));
        MotorTestYesNoDialog();
        break;

      case KEY_ICON_6: 
        
        
        break;

      case KEY_ICON_7:
        mustStoreCmd("G0 Z5\n");  
        infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();
  }
}


