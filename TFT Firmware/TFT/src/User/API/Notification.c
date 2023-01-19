#include "Notification.h"
#include "includes.h"
#include "my_misc.h"


const GUI_RECT toastRect = {START_X + TITLE_END_Y - (TOAST_Y_PAD * 2), TOAST_Y_PAD, LCD_WIDTH - START_X, TITLE_END_Y - TOAST_Y_PAD};
const GUI_RECT toastIconRect = {START_X, TOAST_Y_PAD, START_X + TITLE_END_Y - (TOAST_Y_PAD * 2), TITLE_END_Y - TOAST_Y_PAD};


static TOAST toastlist[TOAST_MSG_COUNT];

static uint8_t nextToastIndex = 0;   
static uint8_t curToastDisplay = 0;  
static uint32_t nextToastTime = 0;   

static NOTIFICATION msglist[MAX_MSG_COUNT];
static uint8_t nextMsgIndex = 0;  
static void (*notificationHandler)() = NULL;

bool _toastRunning = false;


void addToast(DIALOG_TYPE style, char * text)
{
  wakeLCD();
  TOAST t;
  strncpy(t.text, text, TOAST_MSG_LENGTH);
  t.text[TOAST_MSG_LENGTH - 1] = 0;  
  t.isNew = true;
  toastlist[nextToastIndex] = t;
  nextToastIndex = (nextToastIndex + 1) % TOAST_MSG_COUNT;
}


bool toastRunning(void)
{
  return _toastRunning;
}


bool toastAvailable(void)
{
  for (int i = 0; i < TOAST_MSG_COUNT; i++)
  {
    if (toastlist[i].isNew == true)
      return true;
  }
  return false;
}


void drawToast(bool redraw)
{
  if (!redraw)
    curToastDisplay = (curToastDisplay + 1) % TOAST_MSG_COUNT;

  if (toastlist[curToastDisplay].isNew == true || redraw)
  {
    
    _toastRunning = true;

    
    uint8_t *icon;
    uint8_t cursound;
    if (toastlist[curToastDisplay].style == DIALOG_TYPE_ERROR)
    {
      GUI_SetColor(MAT_RED);
      icon = IconCharSelect(ICONCHAR_ERROR);
      cursound = sound_error;
    }
    else if (toastlist[curToastDisplay].style == DIALOG_TYPE_SUCCESS)
    {
      GUI_SetColor(MAT_GREEN);
      icon = IconCharSelect(ICONCHAR_OK_ROUND);
      cursound = sound_success;
    }
    else
    {
      GUI_SetColor(MAT_BLUE);
      icon = IconCharSelect(ICONCHAR_INFO);
      cursound = sound_toast;
    }

    if (cursound >= 0 && !redraw)
      BUZZER_PLAY(cursound);

    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
    GUI_FillPrect(&toastIconRect);
    GUI_SetColor(WHITE);
    GUI_DispStringInPrect(&toastIconRect, icon);

    
    GUI_SetColor(MAT_LOWWHITE);
    GUI_FillPrect(&toastRect);
    GUI_SetColor(DARKGRAY);
    GUI_DispStringInPrect(&toastRect, (uint8_t *)toastlist[curToastDisplay].text);

    
    toastlist[curToastDisplay].isNew = false;

    
    if (!redraw)
      nextToastTime = OS_GetTimeMs() + TOAST_DURATION;

    GUI_RestoreColorDefault();
  }
}


void loopToast(void)
{
  if (getMenuType() == MENU_TYPE_FULLSCREEN)
    return;

  if (OS_GetTimeMs() > nextToastTime)
  {
    if (toastAvailable())
    {
      drawToast(false);
    }
    else if (_toastRunning == true)
    {
      _toastRunning = false;
      GUI_ClearPrect(&toastIconRect);
      GUI_ClearPrect(&toastRect);
      menuReDrawCurTitle();
    }
  }
}


void addNotification(DIALOG_TYPE style, char *title, char *text, bool ShowDialog)
{
  wakeLCD();

  if (nextMsgIndex > MAX_MSG_COUNT - 1)
  {
    
    for (int i = 0; i < MAX_MSG_COUNT - 1; i++)
    {
      memcpy(&msglist[i], &msglist[i + 1], sizeof(NOTIFICATION));
    }
    nextMsgIndex = MAX_MSG_COUNT - 1;
  }

  
  msglist[nextMsgIndex].style  = style;
  strncpy(msglist[nextMsgIndex].text, text, MAX_MSG_LENGTH);
  msglist[nextMsgIndex].text[MAX_MSG_LENGTH - 1] = 0;  
  strncpy(msglist[nextMsgIndex].title, title, MAX_MSG_TITLE_LENGTH);
  msglist[nextMsgIndex].title[MAX_MSG_TITLE_LENGTH - 1] = 0;  

  if (ShowDialog && infoMenu.menu[infoMenu.cur] != menuNotification)
    popupReminder(style, (uint8_t *)title, (uint8_t *)msglist[nextMsgIndex].text);

  if (nextMsgIndex < MAX_MSG_COUNT) nextMsgIndex += 1;  

  if (notificationHandler != NULL)
    notificationHandler();

  notificationDot();

  statusScreen_setMsg((uint8_t *)title, (uint8_t *)text);
}


void replayNotification(uint8_t index)
{
  if (index < nextMsgIndex)
    popupReminder(msglist[index].style, (uint8_t *)msglist[index].title, (uint8_t *)msglist[index].text);
}


NOTIFICATION *getNotification(uint8_t index)
{
  if (strlen(msglist[index].title) > 0 && strlen(msglist[index].text) > 0)
    return &msglist[index];
  else
    return NULL;
}

bool hasNotification(void)
{
  if (nextMsgIndex == 0)
   return false;
  else
    return true;
}

void clearNotification(void)
{
  nextMsgIndex = 0;
  for (int i = 0; i < MAX_MSG_COUNT; i++)
  {
    msglist[i].text[0] = 0;
    msglist[i].title[0] = 0;
  }
  notificationDot();
  statusScreen_setReady();
}


void titleBarPress(void)
{
  if (getMenuType() == MENU_TYPE_ICON || getMenuType() == MENU_TYPE_LISTVIEW)
  {
    if (infoMenu.menu[infoMenu.cur] != menuNotification)
    {
      infoMenu.menu[++infoMenu.cur] = menuNotification;
    }
  }
}

void setNotificationHandler(void (*handler)())
{
  notificationHandler = handler;
}
