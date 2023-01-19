#include "ProbeTest.h"
#include "includes.h"

enum ProbeTestStates { s_ProbeTestIntro, s_ProbeTest, s_ExitProbeTest };
enum ProbeTestStates ProbeTestState = s_ProbeTestIntro;

char tempMsg[120];


void ProbeTestUserSaidYes();
void ProbeTestUserSaidNo();

const MENUITEMS ProbeTestItems = {
  
  LABEL_PROBE_TEST,
  
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

void ProbeTestYesNoDialog(void)
{
  setDialogText(ProbeTestItems.title.index, (uint8_t *) tempMsg, LABEL_YES, LABEL_NO);
  showDialog(DIALOG_TYPE_QUESTION, ProbeTestUserSaidYes, ProbeTestUserSaidNo, NULL);
}

void ProbeTestBackDialog(void)
{
  setDialogText(ProbeTestItems.title.index, (uint8_t *) tempMsg, LABEL_BACKGROUND, LABEL_BACK);
  showDialog(DIALOG_TYPE_QUESTION, NULL, ProbeTestUserSaidNo, NULL);
}

void ProbeTestUserSaidYes(void) 
{
  switch (ProbeTestState)
  {
  case s_ProbeTestIntro:  
    ProbeTestState = s_ProbeTest;
    mustStoreCmd("G0 Z100\n");  
    sprintf(tempMsg, "%s", textSelect(LABEL_PROBE_TEST_INTRO));
    ProbeTestYesNoDialog();
    break;

  case s_ProbeTest: 
    sprintf(tempMsg, "%s %s", textSelect(LABEL_PROBE), textSelect(LABEL_PASSED_TEST));
    ProbeTestBackDialog();
    break;

  default:
    break;
  }
}

void ProbeTestUserSaidNo(void) 
{
  switch (ProbeTestState)
  {
  case s_ProbeTest: 
    ProbeTestState = s_ExitProbeTest;
    sprintf(tempMsg, "%s %s",textSelect(LABEL_PROBE), textSelect(LABEL_FAILED_TEST_REFER));  
    ProbeTestBackDialog();
    break;

  case s_ExitProbeTest:
    infoMenu.cur--;
    break;

  default:
    break;
  }
}

void menuProbeTest(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  
  menuDrawPage(&ProbeTestItems);

  while (infoMenu.menu[infoMenu.cur] == menuProbeTest)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_4:  
        ProbeTestState = s_ProbeTestIntro;
        ProbeTestUserSaidYes(); 
        break;

      case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuXYZForce;
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


