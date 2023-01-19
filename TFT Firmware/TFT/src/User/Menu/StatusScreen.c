#include "StatusScreen.h"

#ifdef TFT70_V3_0
#define KEY_SPEEDMENU         KEY_ICON_3
#define KEY_FLOWMENU          (KEY_SPEEDMENU + 1)
#define KEY_MAINMENU          (KEY_FLOWMENU + 1)
#define SET_SPEEDMENUINDEX(x) setSpeedItemIndex(x)
#else
#define KEY_SPEEDMENU         KEY_ICON_3
#define KEY_MAINMENU          (KEY_SPEEDMENU + 1)
#define SET_SPEEDMENUINDEX(x)
#endif

const MENUITEMS StatusItems = {
  
  
  LABEL_BACKGROUND,
  
  {
    {ICON_STATUS_NOZZLE,           LABEL_BACKGROUND},
    {ICON_STATUS_BED,              LABEL_BACKGROUND},
    {ICON_STATUS_FAN,              LABEL_BACKGROUND},
    {ICON_STATUS_SPEED,            LABEL_BACKGROUND},
    #ifdef TFT70_V3_0
      {ICON_STATUS_FLOW,             LABEL_BACKGROUND},
      {ICON_MAINMENU,                LABEL_MAINMENU},
    #else
      {ICON_MAINMENU,                LABEL_MAINMENU},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
    #endif
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_PRINT,                   LABEL_PRINT},
  }
};

const ITEM SpeedItems[2] = {
  
  {ICON_STATUS_SPEED,            LABEL_BACKGROUND},
  {ICON_STATUS_FLOW,             LABEL_BACKGROUND},
};




#define UPDATE_TOOL_TIME 2000  
static int8_t lastConnection_status = -1;
static bool msgNeedRefresh = false;

static char msgtitle[20];
static char msgbody[MAX_MSG_LENGTH];
SCROLL msgScroll;

const char *const SpeedID[2] = SPEED_ID;


const GUI_POINT ss_title_point = {SSICON_WIDTH - BYTE_WIDTH/2, SSICON_NAME_Y0};
const GUI_POINT ss_val_point   = {SSICON_WIDTH/2, SSICON_VAL_Y0};
#ifdef TFT70_V3_0
  const GUI_POINT ss_val2_point = {SSICON_WIDTH/2, SSICON_VAL2_Y0};
#endif


const  GUI_RECT msgRect = {START_X + 1 * ICON_WIDTH + 1 * SPACE_X + 2,   ICON_START_Y +  1 * ICON_HEIGHT + 1 * SPACE_Y + STATUS_MSG_BODY_YOFFSET,
                           START_X + 3 * ICON_WIDTH + 2 * SPACE_X - 2,   ICON_START_Y +  2 * ICON_HEIGHT + 1 * SPACE_Y - STATUS_MSG_BODY_BOTTOM};


const GUI_RECT RecGantry = {START_X,                        1*SSICON_HEIGHT+0*SPACE_Y+ICON_START_Y + STATUS_GANTRY_YOFFSET,
                            4*ICON_WIDTH+3*SPACE_X+START_X, 1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y - STATUS_GANTRY_YOFFSET};

static void wDrawSoftHome(void);

void drawTemperature(void)
{
  
  char tempstr[45];

  LIVE_INFO lvIcon;
  lvIcon.enabled[0] = true;
  lvIcon.lines[0].h_align = RIGHT;
  lvIcon.lines[0].v_align = TOP;
  lvIcon.lines[0].fn_color = SSICON_NAME_COLOR;
  lvIcon.lines[0].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[0].pos = ss_title_point;
  lvIcon.lines[0].large_font = NAME_LARGE_FONT;

  lvIcon.enabled[1] = true;
  lvIcon.lines[1].h_align = CENTER;
  lvIcon.lines[1].v_align = CENTER;
  lvIcon.lines[1].fn_color = SSICON_VAL_COLOR;
  lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[1].pos = ss_val_point;
  lvIcon.lines[1].large_font = VAL_LARGE_FONT;

  #ifndef TFT70_V3_0
    lvIcon.enabled[2] = false;
  #else
    lvIcon.enabled[2] = true;
    lvIcon.lines[2].h_align = CENTER;
    lvIcon.lines[2].v_align = CENTER;
    lvIcon.lines[2].fn_color = SSICON_VAL2_COLOR;
    lvIcon.lines[2].text_mode = GUI_TEXTMODE_TRANS;
    lvIcon.lines[2].pos = ss_val2_point;
    lvIcon.lines[2].large_font = VAL2_LARGE_FONT;
  #endif

  #ifdef TFT70_V3_0
    char tempstr2[45];

  
    lvIcon.lines[0].text = (uint8_t *)heatDisplayID[currentTool];
    sprintf(tempstr, "%d℃", heatGetCurrentTemp(currentTool));
    sprintf(tempstr2, "%d℃", heatGetTargetTemp(currentTool));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    lvIcon.lines[2].text = (uint8_t *)tempstr2;
    showLiveInfo(0, &lvIcon, &StatusItems.items[0]);

    
    lvIcon.lines[0].text = (uint8_t *)heatDisplayID[BED];
    sprintf(tempstr, "%d℃", heatGetCurrentTemp(BED));
    sprintf(tempstr2, "%d℃", heatGetTargetTemp(BED));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    lvIcon.lines[2].text = (uint8_t *)tempstr2;
    showLiveInfo(1, &lvIcon, &StatusItems.items[1]);

    lvIcon.enabled[2] = false;
  #else

    
    lvIcon.lines[0].text = (uint8_t *)heatDisplayID[currentTool];
    sprintf(tempstr, "%d/%d", heatGetCurrentTemp(currentTool), heatGetTargetTemp(currentTool));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    showLiveInfo(0, &lvIcon, &StatusItems.items[0]);

    
    lvIcon.lines[0].text = (uint8_t *)heatDisplayID[BED];
    sprintf(tempstr, "%d/%d", heatGetCurrentTemp(BED), heatGetTargetTemp(BED));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    showLiveInfo(1, &lvIcon, &StatusItems.items[1]);

  #endif

  
  lvIcon.lines[0].text = (uint8_t *)fanID[currentFan];

  if (infoSettings.fan_percentage == 1)
  {
    sprintf(tempstr, "%d%%", fanGetCurPercent(currentFan));
  }
  else
  {
    sprintf(tempstr, "%d", fanGetCurSpeed(currentFan));
  }
  lvIcon.lines[1].text = (uint8_t *)tempstr;
  showLiveInfo(2, &lvIcon, &StatusItems.items[2]);

  #ifdef TFT70_V3_0
    
    lvIcon.lines[0].text = (uint8_t *)SpeedID[0];
    sprintf(tempstr, "%d%%", speedGetCurPercent(0));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    showLiveInfo(3, &lvIcon, &SpeedItems[0]);

    
    lvIcon.lines[0].text = (uint8_t *)SpeedID[1];
    sprintf(tempstr, "%d%%", speedGetCurPercent(1));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    showLiveInfo(4, &lvIcon, &SpeedItems[1]);

  #else
    
    lvIcon.lines[0].text = (uint8_t *)SpeedID[currentSpeedID];
    sprintf(tempstr, "%d%%", speedGetCurPercent(currentSpeedID));
    lvIcon.lines[1].text = (uint8_t *)tempstr;
    showLiveInfo(3, &lvIcon, &SpeedItems[currentSpeedID]);
  #endif

  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
  GUI_SetColor(GANTRYLBL_COLOR);
  GUI_SetBkColor(infoSettings.status_xyz_bg_color);
  sprintf(tempstr, "   X: %.2f   Y: %.2f   Z: %.2f   ", coordinateGetAxisActual(X_AXIS), coordinateGetAxisActual(Y_AXIS),
          coordinateGetAxisActual(Z_AXIS));
  GUI_DispStringInPrect(&RecGantry, (uint8_t *)tempstr);

  GUI_RestoreColorDefault();
}

void statusScreen_setMsg(const uint8_t *title, const uint8_t *msg)
{
  strncpy(msgtitle, (char *)title, sizeof(msgtitle));
  strncpy(msgbody, (char *)msg, sizeof(msgbody));
  msgNeedRefresh = true;
}

void statusScreen_setReady(void)
{
  strncpy(msgtitle, (char *)textSelect(LABEL_STATUS), sizeof(msgtitle));
  {
    strncpy(msgbody, (char *)machine_type, sizeof(msgbody));
    strcat(msgbody, " ");
    strcat(msgbody, (char *)textSelect(LABEL_READY));
  }
  msgNeedRefresh = true;
}

void drawStatusScreenMsg(void)
{
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  ICON_CustomReadDisplay(rect_of_keySS[17].x0, rect_of_keySS[17].y0, INFOBOX_ADDR);
  GUI_SetColor(INFOMSG_BKCOLOR);
  GUI_DispString(rect_of_keySS[17].x0 + STATUS_MSG_ICON_XOFFSET,
                 rect_of_keySS[17].y0 + STATUS_MSG_ICON_YOFFSET,
                 IconCharSelect(ICONCHAR_INFO));

  GUI_DispString(rect_of_keySS[17].x0 + BYTE_HEIGHT + STATUS_MSG_TITLE_XOFFSET,
                 rect_of_keySS[17].y0 + STATUS_MSG_ICON_YOFFSET,
                 (uint8_t *)msgtitle);
  GUI_SetBkColor(INFOMSG_BKCOLOR);
  GUI_FillPrect(&msgRect);

  Scroll_CreatePara(&msgScroll, (uint8_t *)msgbody, &msgRect);

  GUI_RestoreColorDefault();

  msgNeedRefresh = false;
}

static inline void scrollMsg(void)
{
  GUI_SetBkColor(INFOMSG_BKCOLOR);
  GUI_SetColor(INFOMSG_COLOR);
  Scroll_DispString(&msgScroll,CENTER);
  GUI_RestoreColorDefault();
}

static inline void toggleTool(void)
{
  if (nextScreenUpdate(UPDATE_TOOL_TIME))
  {
    if (infoSettings.hotend_count > 1)
    {
      currentTool = (currentTool+1) % infoSettings.hotend_count;
    }
    if ((infoSettings.fan_count + infoSettings.fan_ctrl_count) > 1)
    {
      currentFan = (currentFan + 1) % (infoSettings.fan_count + infoSettings.fan_ctrl_count);
    }
    currentSpeedID = (currentSpeedID + 1) % 2;
    drawTemperature();

    
    coordinateQuery();
    speedQuery();
    fanSpeedQuery();
  }
}

void menuStatus(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  extern TOOLPREHEAT whichHeatersToHeat;  
  GUI_SetBkColor(infoSettings.bg_color);
  menuDrawPage(&StatusItems);
  GUI_SetColor(infoSettings.status_xyz_bg_color);
  GUI_FillPrect(&RecGantry);
  drawTemperature();
  drawStatusScreenMsg();
  
  infoMenu.cur = 0;
  infoMenu.menu[infoMenu.cur] = menuStatus;

  while (infoMenu.menu[infoMenu.cur] == menuStatus)
  {
    if (infoHost.connected != lastConnection_status)
    {
      statusScreen_setReady();
      lastConnection_status = infoHost.connected;
    }
    if (msgNeedRefresh)
    {
      drawStatusScreenMsg();
    }
    scrollMsg();

	  wDrawSoftHome();

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        heatSetCurrentIndex(currentTool);
        whichHeatersToHeat = NOZZLE0_PREHEAT;  
        infoMenu.menu[++infoMenu.cur] = menuHeat;
        break;
      case KEY_ICON_1:
        heatSetCurrentIndex(BED);
        whichHeatersToHeat = BED_PREHEAT;  
        infoMenu.menu[++infoMenu.cur] = menuHeat;
        break;
      case KEY_ICON_2:
        infoMenu.menu[++infoMenu.cur] = menuFan;
        break;
      case KEY_SPEEDMENU:
        SET_SPEEDMENUINDEX(0);
        infoMenu.menu[++infoMenu.cur] = menuSpeed;
        break;
      #ifdef TFT70_V3_0
        case KEY_FLOWMENU:
          SET_SPEEDMENUINDEX(1);
          infoMenu.menu[++infoMenu.cur] = menuSpeed;
          break;
      #endif
      case KEY_MAINMENU:
        resetLCDidleTimer();
        loopProcess();
        infoMenu.menu[++infoMenu.cur] = menuMain;
        break;
      case KEY_ICON_7:
        infoMenu.menu[++infoMenu.cur] = menuPrint;
        break;

      case KEY_SOFTHOME:
        mustStoreScript("G28\nG0 F2500 Z30\nG0 F9000 X0 Y210\n");
        break;

      case KEY_INFOBOX:
        infoMenu.menu[++infoMenu.cur] = menuNotification;
      default:

        break;
    }
    toggleTool();
    loopProcess();
  }
}


static void wDrawSoftHome(void)
{
  ICON_ReadDisplay(LCD_WIDTH - 40,0 ,ICON_SOFT); 
}