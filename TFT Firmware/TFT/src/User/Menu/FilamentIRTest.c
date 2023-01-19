#include "FilamentIRTest.h"
#include "includes.h"

enum FilamentIRTestStates { s_FilamentIRTestIdle, s_FilamentIRTestIntroY, s_FilamentIRTestIntroN, s_ExitFilamentIRTest };
enum FilamentIRTestStates FilamentIRTestState = s_FilamentIRTestIdle;

char tempMsg[120];


void FilamentIRTestUserSaidYes();
void FilamentIRTestUserSaidNo();

const MENUITEMS FilamentIRTestItems = {
  
  LABEL_FILAMENTIRTEST,
  
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

void FilamentIRTestYesNoDialog(void)
{
  setDialogText(FilamentIRTestItems.title.index, (uint8_t *) tempMsg, LABEL_YES, LABEL_NO);
  showDialog(DIALOG_TYPE_QUESTION, FilamentIRTestUserSaidYes, FilamentIRTestUserSaidNo, NULL);
}

void FilamentIRTestBackDialog(void)
{
  setDialogText(FilamentIRTestItems.title.index, (uint8_t *) tempMsg, LABEL_BACKGROUND, LABEL_BACK);
  showDialog(DIALOG_TYPE_QUESTION, NULL, FilamentIRTestUserSaidNo, NULL);
}

void FilamentIRTestUserSaidYes(void) 
{
  switch (FilamentIRTestState)
  {
  case s_FilamentIRTestIntroY:  
  case s_FilamentIRTestIntroN: 
    FilamentIRTestState = s_ExitFilamentIRTest;
    sprintf(tempMsg, "%s %s", textSelect(LABEL_FILAMENTIRTEST), textSelect(LABEL_PASSED_TEST));
    FilamentIRTestBackDialog();
    break;

  default:
    break;
  }
}

void FilamentIRTestUserSaidNo(void) 
{
  switch (FilamentIRTestState)
  {
  case s_FilamentIRTestIntroY: 
  case s_FilamentIRTestIntroN: 
    FilamentIRTestState = s_ExitFilamentIRTest;
    sprintf(tempMsg, "%s %s", textSelect(LABEL_FILAMENTIRTEST), textSelect(LABEL_FAILED_TEST_REFER));
    FilamentIRTestBackDialog();
    break;

  case s_ExitFilamentIRTest:
    infoMenu.cur--;
    break;

  default:
    break;
  }
}

void menuFilamentIRTest(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  
  menuDrawPage(&FilamentIRTestItems);

  while (infoMenu.menu[infoMenu.cur] == menuFilamentIRTest)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_4:  
        if(FilamentPresent() == true)
        {
          FilamentIRTestState = s_FilamentIRTestIntroY;
          sprintf(tempMsg, "%s", textSelect(LABEL_FILAMENTIRTEST_INTRO_Y));
        }
        else
        {
          FilamentIRTestState = s_FilamentIRTestIntroN;
          sprintf(tempMsg, "%s", textSelect(LABEL_FILAMENTIRTEST_INTRO_N));  
        }
        FilamentIRTestYesNoDialog();
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


