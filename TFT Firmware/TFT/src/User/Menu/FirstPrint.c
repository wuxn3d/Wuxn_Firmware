#include "FirstPrint.h"
#include "includes.h"

static void wFirstPrintDialogV1UserSaidCancel(void);
static void wFirstPrintDialogV2UserSaidYes(void);
static void wFirstPrintLeave(void);

extern const MENUITEMS blankMenuItems;
extern bool bFirstPrintFromOOBWCW;


static void wFirstPrintDialogV1UserSaidCancel(void)
{
  setDialogText(LABEL_FIRST_PRINT, LABEL_FIRST_PRINT_V2, LABEL_YES, LABEL_NO);
  showDialog(DIALOG_TYPE_QUESTION, wFirstPrintDialogV2UserSaidYes, wFirstPrintLeave, NULL);
}

static void wFirstPrintDialogV2UserSaidYes(void)
{
  setDialogText(LABEL_FIRST_PRINT, LABEL_FIRST_PRINT_V1, LABEL_CANCEL, LABEL_BACKGROUND);
  showDialog(DIALOG_TYPE_INFO, wFirstPrintLeave, NULL, NULL);
}


static void wFirstPrintLeave(void)
{
  infoMenu.menu[infoMenu.cur] = menuWizards;
}


const MENUITEMS First_PrintItems = {
  
  LABEL_FIRST_PRINT,
  
  {
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_START,                   LABEL_START},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACK,                    LABEL_BACK},
  }
};



void menuFirstPrint(void)
{
KEY_VALUES key_num = KEY_IDLE;
  

  if(bFirstPrintFromOOBWCW == TRUE)
  {
    menuDrawPage(&blankMenuItems);
    setDialogText(LABEL_FIRST_PRINT_GUIDED, LABEL_OOBWCW_PRINT, LABEL_BACKGROUND, LABEL_BACKGROUND);
    showDialog(DIALOG_TYPE_INFO, NULL, NULL, NULL);
  }
  else
  {
    menuDrawPage(&First_PrintItems);
  }


 while (infoMenu.menu[infoMenu.cur] == menuFirstPrint)
  {
    if(bFirstPrintFromOOBWCW == FALSE)
    {
      key_num = menuKeyGetValue();
      switch (key_num)
      {
        case KEY_ICON_5:
          setDialogText(LABEL_FIRST_PRINT, LABEL_FIRST_PRINT_V1, LABEL_CANCEL, LABEL_BACKGROUND);
          showDialog(DIALOG_TYPE_INFO, wFirstPrintDialogV1UserSaidCancel, NULL, NULL);
          break;

        case KEY_ICON_7:
          ReturnToWizardsMenu(); 
          break;

        default:
          break;
      }
    }
    loopProcess();
  }
}

