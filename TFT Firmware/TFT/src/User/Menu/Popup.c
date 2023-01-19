#include "Popup.h"
#include "includes.h"

#define X_MAX_CHAR     (LCD_WIDTH / BYTE_WIDTH)
#define MAX_MSG_LINES  4
#define POPUP_MAX_CHAR (X_MAX_CHAR * MAX_MSG_LINES)

static BUTTON bottomSingleBtn = {
  
  POPUP_RECT_SINGLE_CONFIRM, NULL, 5, 1,  DARKGREEN, DARKGREEN,  MAT_LOWWHITE, DARKGREEN, WHITE, DARKGREEN
};

BUTTON bottomDoubleBtn[] = {
  {POPUP_RECT_DOUBLE_CONFIRM, NULL, 5, 1, DARKGREEN, DARKGREEN,  MAT_LOWWHITE, DARKGREEN, WHITE, DARKGREEN},
  {POPUP_RECT_DOUBLE_CANCEL,  NULL, 5, 1, MAT_RED,     MAT_RED,  MAT_LOWWHITE,   MAT_RED, WHITE,   MAT_RED},
};

const GUI_RECT doubleBtnRect[] = {POPUP_RECT_DOUBLE_CONFIRM, POPUP_RECT_DOUBLE_CANCEL};
static const GUI_RECT singleBtnRect = POPUP_RECT_SINGLE_CONFIRM;

static WINDOW window = {
  DIALOG_TYPE_INFO,             
  POPUP_RECT_WINDOW,            
  POPUP_TITLE_HEIGHT,           
  POPUP_BOTTOM_HEIGHT,          
  2,                            
  GRAY,                         
  {DARKGRAY, MAT_LOWWHITE},     
  {DARKGRAY, MAT_LOWWHITE},     
  {DARKGRAY, MAT_LOWWHITE},     
};

static BUTTON *windowButton =  NULL;
static uint16_t buttonNum = 0;

static const GUI_RECT * cur_btn_rect = NULL;
static void (*action_ok)() = NULL;
static void (*action_cancel)() = NULL;
static void (*action_loop)() = NULL;

static bool popup_redraw = false;
static uint8_t popup_title[X_MAX_CHAR];
static uint8_t popup_msg[POPUP_MAX_CHAR];
static uint8_t popup_ok[24];
static uint8_t popup_cancel[24];
static DIALOG_TYPE popup_type;

void windowReDrawButton(uint8_t position, uint8_t pressed)
{
  if (position >= buttonNum)
    return;
  if (pressed >= 2)
    return;
  if (windowButton == NULL)
    return;
  if (windowButton->context == NULL)
    return;

  GUI_DrawButton(windowButton + position, pressed);
}

void popupDrawPage(DIALOG_TYPE type, BUTTON * btn, const uint8_t * title, const uint8_t * context, const uint8_t * yes,
                   const uint8_t * no)
{
  setMenuType(MENU_TYPE_DIALOG);

  if (btn != NULL)  
  {                 
    buttonNum = 0;
    windowButton = btn;

    if (yes && yes[0])
    {
      windowButton[buttonNum++].context = yes;
    }
    if (no && no[0])
    {
      windowButton[buttonNum++].context = no;
    }
  }

  TSC_ReDrawIcon = windowReDrawButton;
  window.type = type;

  if (btn != NULL)  
  {
    GUI_DrawWindow(&window, title, context, true);
    for (uint8_t i = 0; i < buttonNum; i++) GUI_DrawButton(&windowButton[i], 0);
  }
  else  
  {
    GUI_DrawWindow(&window, title, context, false);
  }
}

void menuDialog(void)
{
  while (infoMenu.menu[infoMenu.cur] == menuDialog)
  {
    uint16_t key_num = KEY_GetValue(buttonNum, cur_btn_rect);
    switch (key_num)
    {
      case KEY_POPUP_CONFIRM:
        infoMenu.cur--;
        if (action_ok != NULL)
          action_ok();
        break;

      case KEY_POPUP_CANCEL:
        infoMenu.cur--;
        if (action_cancel != NULL)
          action_cancel();
        break;

      default:
        break;
    }

    if (action_loop != NULL)
      action_loop();

    loopProcess();
  }
}

void popup_strcpy(uint8_t *dst, uint8_t *src, uint16_t size)
{
  if (src)
  {
    strncpy((char *)dst, (char *)src, size);
    dst[size - 1] = 0;
  }
  else
  {
    dst[0] = 0;
  }
}

void _setDialogTitleStr(uint8_t * str)
{
  popup_strcpy(popup_title, str, sizeof(popup_title));
}

void _setDialogMsgStr(uint8_t * str)
{
  popup_strcpy(popup_msg, str, sizeof(popup_msg));
}

uint8_t *getDialogMsgStr()
{
  return (uint8_t *)popup_msg;
}

void _setDialogOkTextStr(uint8_t * str)
{
  popup_strcpy(popup_ok, str, sizeof(popup_ok));
}

void _setDialogCancelTextStr(uint8_t * str)
{
  popup_strcpy(popup_cancel, str, sizeof(popup_cancel));
}

void _setDialogTitleLabel(int16_t index)
{
  uint8_t tempstr[MAX_LANG_LABEL_LENGTH] = {0};
  loadLabelText(tempstr, index);
  popup_strcpy(popup_title, tempstr, sizeof(popup_title));
}

void _setDialogMsgLabel(int16_t index)
{
  uint8_t tempstr[MAX_LANG_LABEL_LENGTH] = {0};
  loadLabelText(tempstr, index);
  popup_strcpy(popup_msg, tempstr, sizeof(popup_msg));
}

void _setDialogOkTextLabel(int16_t index)
{
  uint8_t tempstr[MAX_LANG_LABEL_LENGTH] = {0};
  loadLabelText(tempstr, index);
  popup_strcpy(popup_ok, tempstr, sizeof(popup_ok));
}

void _setDialogCancelTextLabel(int16_t index)
{
  uint8_t tempstr[MAX_LANG_LABEL_LENGTH] = {0};
  loadLabelText(tempstr, index);
  popup_strcpy(popup_cancel, tempstr, sizeof(popup_cancel));
}

/** Show save setting dialog. Set dialog text before calling showDialog
 * @param ok_action - pointer to a function to perform if ok is pressed. (pass NULL if no action need to be performed)
 * @param cancel_action - pointer to a function to perform if Cancel is pressed.(pass NULL if no action need to be performed)
*/
void showDialog(DIALOG_TYPE type, void (*ok_action)(), void (*cancel_action)(), void (*loop_action)())
{
  if (infoSettings.mode == MODE_MARLIN)
    return;

  popup_redraw = true;
  popup_type = type;

  action_ok = ok_action;
  action_cancel = cancel_action;
  action_loop = loop_action;
}


void loopPopup(void)
{
  if (popup_redraw == false)
    return;

  popup_redraw = false;

  wakeLCD();

  
  if (popup_cancel[0])
  {
    popupDrawPage(popup_type, bottomDoubleBtn, popup_title, popup_msg, popup_ok, popup_cancel);
    cur_btn_rect = doubleBtnRect;
  }
  else
  {
    popupDrawPage(popup_type, &bottomSingleBtn, popup_title, popup_msg, popup_ok, NULL);
    cur_btn_rect = &singleBtnRect;
  }
  
  if (infoMenu.menu[infoMenu.cur] != menuDialog)
  { 
    infoMenu.menu[++infoMenu.cur] = menuDialog;
  }
}
