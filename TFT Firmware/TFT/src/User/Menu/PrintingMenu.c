#include "Printing.h"
#include "includes.h"

extern bool f_RunningM600;  
extern const MENUITEMS Printing_blankMenuItems; 
extern bool f_ReadyToPrintFilamentPresent; 

const GUI_POINT printinfo_points[6] = {
  {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0},
  {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0},
  {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0},
  {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1},
  {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1},
  {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1},
};

const GUI_RECT printinfo_val_rect[6] = {
  {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_X, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y,
   START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_LG_EX, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_X, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y,
   START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_LG_EX, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_X, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y,
   START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_SM_EX, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_X, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y,
   START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_LG_EX, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_X, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y,
   START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_LG_EX, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y + BYTE_HEIGHT},

  {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_X, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y,
   START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_SM_EX, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y + BYTE_HEIGHT},
};

static uint32_t nextLayerDrawTime = 0;
const  char *const Speed_ID[2] = {"Speed", "Flow"};
bool hasFilamentData;
extern bool bFirstPrintFromOOBWCW; 
bool bFirstPrintMessageDisplayed; 
uint32_t ulFirstPrintMessageTimer;  

static void oobwPrintDurationUserSaidYes(void);  

#define TOGGLE_TIME     2000  
#define LAYER_DRAW_TIME 500   


#define LAYER_TITLE "Height"  
#define EXT_ICON_POS 0
#define BED_ICON_POS 1
#define FAN_ICON_POS 2
#define TIM_ICON_POS 3
#define Z_ICON_POS   4
#define SPD_ICON_POS 5

const ITEM itemIsPause[2] = {
  
  {ICON_PAUSE,                   LABEL_PAUSE},
  {ICON_RESUME,                  LABEL_RESUME},
};


const ITEM itemIsPrinting[3] = {
  
  {ICON_BACKGROUND,              LABEL_BACKGROUND},
  {ICON_MAINMENU,                LABEL_MAIN_SCREEN},
  {ICON_BACK,                    LABEL_BACK},
};

#define BLANK_PAUSE_RESUME_KEY itemIsPrinting[0]  

void menuBeforePrinting(void)
{
  
  uint32_t size = 0;

  
  
  

  mustStoreCmd("M119\n");  
  loopProcess();
  if(f_ReadyToPrintFilamentPresent == false)  
  {
    f_ReadyToPrintFilamentPresent = true;  
    abortPrinting();  
    setDialogText(LABEL_FILAMENT_RUNOUT, LABEL_NO_FILAMENT, LABEL_CONFIRM, LABEL_BACKGROUND);
      showDialog(DIALOG_TYPE_QUESTION, abortPrinting, NULL, NULL);

    
  }




  switch (infoFile.source)
  {
    case BOARD_SD: 
      size = request_M23_M36(infoFile.title + 5);
      
      
      
      

      if (size == 0)
      {
        ExitDir();
        infoMenu.cur--;
        return;
      }

      infoPrinting.size = size;

      
      
      request_M24(0);
      
      
      
      
      

      if (infoMachineSettings.autoReportSDStatus == 1)
        request_M27(infoSettings.m27_refresh_time);  
      else
        request_M27(0);

      infoHost.printing = true;  
      break;

    case TFT_UDISK:
    case TFT_SD:  
      if (f_open(&infoPrinting.file, infoFile.title, FA_OPEN_EXISTING | FA_READ) != FR_OK)
      {
        ExitDir();
        infoMenu.cur--;
        return;
      }
      if (powerFailedCreate(infoFile.title) == false)
      {}
      powerFailedlSeek(&infoPrinting.file);

      infoPrinting.size = f_size(&infoPrinting.file);
      infoPrinting.cur = infoPrinting.file.fptr;
      if (infoSettings.send_start_gcode == 1 && infoPrinting.cur == 0)  
      {
        sendPrintCodes(0);
      }
      break;

    default:
      ExitDir();
      infoMenu.cur--;
      return;
  }
  infoPrinting.printing = true;
  infoPrinting.time = 0;
  initPrintSummary();
  infoMenu.menu[infoMenu.cur] = menuPrinting;
  mustStoreCmd("M75\n");  
}

static inline void reValueNozzle(int icon_pos)
{
  char tempstr[10];
  sprintf(tempstr, "%d/%d", heatGetCurrentTemp(currentTool), heatGetTargetTemp(currentTool));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_NOZZLE);
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y,
                 (uint8_t *)heatDisplayID[currentTool]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (uint8_t *)tempstr);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reValueBed(int icon_pos)
{
  char tempstr[10];
  sprintf(tempstr, "%d/%d", heatGetCurrentTemp(BED), heatGetTargetTemp(BED));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_BED);
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y,
                 (uint8_t *)heatDisplayID[BED]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (uint8_t *)tempstr);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reDrawFan(int icon_pos)
{
  char tempstr[10];

  if (infoSettings.fan_percentage == 1)
    sprintf(tempstr, "%d%%", fanGetCurPercent(currentFan));
  else
    sprintf(tempstr, "%d", fanGetCurSpeed(currentFan));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_FAN);
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y,
                 (uint8_t *)fanID[currentFan]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (uint8_t *)tempstr);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reDrawSpeed(int icon_pos)
{
  char tempstr[10];

  if (currentSpeedID == 0)
    ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_SPEED);
  else
    ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_FLOW);

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  sprintf(tempstr, "%d%%", speedGetCurPercent(currentSpeedID));
  GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y,
                 (uint8_t *)Speed_ID[currentSpeedID]);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (uint8_t *)tempstr);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reDrawTime(int icon_pos)
{
  uint8_t hour = infoPrinting.time / 3600;
  uint8_t min = infoPrinting.time % 3600 / 60;
  uint8_t sec = infoPrinting.time % 60;

  GUI_SetNumMode(GUI_NUMMODE_ZERO);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  char tempstr[10];
  sprintf(tempstr, "%02u:%02u:%02u", hour, min, sec);
  ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_TIMER);
  GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (uint8_t *)tempstr);
  GUI_SetNumMode(GUI_NUMMODE_SPACE);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reDrawProgress(int icon_pos)
{
  char buf[6];

  sprintf(buf, "%d%%", infoPrinting.progress);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  GUI_DispString(printinfo_points[3].x + PICON_TITLE_X, printinfo_points[3].y + PICON_TITLE_Y, (uint8_t *)buf);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reDrawLayer(int icon_pos)
{
  if (OS_GetTimeMs() > nextLayerDrawTime)
  {
    char tempstr[10];

    sprintf(tempstr, "%.2fmm",
            (infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(Z_AXIS) : coordinateGetAxisTarget(Z_AXIS));

    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
    ICON_ReadDisplay(printinfo_points[icon_pos].x, printinfo_points[icon_pos].y, ICON_PRINTING_ZLAYER);
    GUI_DispString(printinfo_points[icon_pos].x + PICON_TITLE_X, printinfo_points[icon_pos].y + PICON_TITLE_Y,
                   (uint8_t *)LAYER_TITLE);
    GUI_DispStringInPrect(&printinfo_val_rect[icon_pos], (uint8_t *)tempstr);
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
    nextLayerDrawTime = OS_GetTimeMs() + LAYER_DRAW_TIME;
  }
}

static inline void toggleInfo(void)
{
  if (nextScreenUpdate(TOGGLE_TIME))
  {
    if (infoSettings.hotend_count > 1)
    {
      currentTool = (currentTool + 1) % infoSettings.hotend_count;
      RAPID_SERIAL_LOOP();  
      reValueNozzle(EXT_ICON_POS);
    }

    if ((infoSettings.fan_count + infoSettings.fan_ctrl_count) > 1)
    {
      currentFan = (currentFan + 1) % (infoSettings.fan_count + infoSettings.fan_ctrl_count);
      RAPID_SERIAL_LOOP();  
      reDrawFan(FAN_ICON_POS);
    }

    currentSpeedID = (currentSpeedID + 1) % 2;
    RAPID_SERIAL_LOOP();  
    reDrawSpeed(SPD_ICON_POS);
    speedQuery();

    if (infoFile.source >= BOARD_SD)
      coordinateQuery();

    if (!hasFilamentData && isPrinting())
      updateFilamentUsed();
  }
}

static inline void printingDrawPage(void)
{
  reValueNozzle(EXT_ICON_POS);
  reValueBed(BED_ICON_POS);
  reDrawFan(FAN_ICON_POS);
  reDrawTime(TIM_ICON_POS);
  reDrawProgress(TIM_ICON_POS);
  nextLayerDrawTime = 0;  
  reDrawLayer(Z_ICON_POS);
  reDrawSpeed(SPD_ICON_POS);
}

void drawPrintInfo(void)
{
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  ICON_CustomReadDisplay(rect_of_keySS[17].x0, rect_of_keySS[17].y0, INFOBOX_ADDR);
  GUI_SetColor(INFOMSG_BKCOLOR);
  GUI_DispString(rect_of_keySS[17].x0 + STATUS_MSG_ICON_XOFFSET, rect_of_keySS[17].y0 + STATUS_MSG_ICON_YOFFSET,
                 IconCharSelect(ICONCHAR_INFO));
  GUI_DispStringInRectEOL(rect_of_keySS[17].x0 + BYTE_HEIGHT + STATUS_MSG_TITLE_XOFFSET,
                          rect_of_keySS[17].y0 + STATUS_MSG_ICON_YOFFSET,
                          rect_of_keySS[17].x1 - BYTE_HEIGHT + STATUS_MSG_TITLE_XOFFSET,
                          rect_of_keySS[17].y1 - STATUS_MSG_ICON_YOFFSET,
                          (uint8_t *)textSelect(LABEL_PRINT_FINISHED));
  GUI_SetColor(INFOMSG_COLOR);
  GUI_SetBkColor(INFOMSG_BKCOLOR);
  GUI_DispStringInPrect(&msgRect,LABEL_CLICK_FOR_MORE);
  GUI_RestoreColorDefault();
}

void stopConfirm(void)
{
  clearCmdQueue(); 
  mustStoreCmd("M77\n");  
  abortPrinting();
  TurnOffHeatersAndFans();
  ReturnToReadyMenu();  
}

void printInfoPopup(void)
{
  uint8_t hour = infoPrintSummary.time / 3600;
  uint8_t min = infoPrintSummary.time % 3600 / 60;
  uint8_t sec = infoPrintSummary.time % 60;
  char showInfo[150];
  char tempstr[30];

  sprintf(showInfo, (char*)textSelect(LABEL_PRINT_TIME), hour, min, sec);

  if (infoPrintSummary.length == 0 && infoPrintSummary.weight == 0 && infoPrintSummary.cost == 0)
  {
    strcat(showInfo, (char *)textSelect(LABEL_NO_FILAMENT_STATS));
  }
  else
  {
    if (infoPrintSummary.length > 0)
    {
      sprintf(tempstr, (char *)textSelect(LABEL_FILAMENT_LENGTH), infoPrintSummary.length);
      strcat(showInfo, tempstr);
    }
    if (infoPrintSummary.weight > 0)
    {
      sprintf(tempstr, (char *)textSelect(LABEL_FILAMENT_WEIGHT), infoPrintSummary.weight);
      strcat(showInfo, tempstr);
    }
    if (infoPrintSummary.cost > 0)
    {
      sprintf(tempstr, (char *)textSelect(LABEL_FILAMENT_COST), infoPrintSummary.cost);
      strcat(showInfo, tempstr);
    }
  }
  popupReminder(DIALOG_TYPE_INFO, (uint8_t *)infoPrintSummary.name, (uint8_t *)showInfo);
}

static void oobwPrintDurationUserSaidYes(void)  
{
  
}

void menuPrinting(void)
{
  
  MENUITEMS printingItems = {
    
    LABEL_BACKGROUND,
    
    {
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BABYSTEP},
      {ICON_MORE,                    LABEL_MORE},
      {ICON_STOP,                    LABEL_STOP},
    }
  };

  uint8_t nowFan[MAX_FAN_COUNT] = {0};
  uint16_t curspeed[2] = {0};
  uint32_t time = 0;
  HEATER nowHeat;
  float curLayer = 0;
  bool lastPause = isPause();
  bool lastPrinting = isPrinting();

  memset(&nowHeat, 0, sizeof(HEATER));

  if (lastPrinting == true)
  {
    if (infoMachineSettings.long_filename_support == ENABLED && infoFile.source == BOARD_SD)
      printingItems.title.address = (uint8_t *) infoFile.Longfile[infoFile.fileIndex];
    else
      printingItems.title.address = getCurGcodeName(infoFile.title);
    printingItems.items[KEY_ICON_4] = itemIsPause[lastPause];
    printingItems.items[KEY_ICON_5].icon = (infoFile.source < BOARD_SD && infoPrinting.model_icon) ?
                                            ICON_PREVIEW : ICON_BABYSTEP;
  }
  else 
  {
    printingItems.title.address = (uint8_t *)infoPrintSummary.name;

    #ifdef TFT70_V3_0
      printingItems.items[KEY_ICON_5] = itemIsPrinting[1];  
    #else
      printingItems.items[KEY_ICON_4] = itemIsPrinting[1];  
      printingItems.items[KEY_ICON_5] = itemIsPrinting[0];  
    #endif
      printingItems.items[KEY_ICON_6] = itemIsPrinting[0];  
      printingItems.items[KEY_ICON_7] = itemIsPrinting[2];  
  }

  menuDrawPage(&printingItems);
  printingDrawPage();
  if (lastPrinting == false)  
    drawPrintInfo();

  while (infoMenu.menu[infoMenu.cur] == menuPrinting)
  {
    

    
    if(bFirstPrintFromOOBWCW == TRUE &&
       bFirstPrintMessageDisplayed == FALSE)
    {
      bFirstPrintMessageDisplayed = TRUE;
      setDialogText(LABEL_FIRST_PRINT, LABEL_OOBWCW_PRINT_DURATION, LABEL_CONFIRM, LABEL_BACKGROUND);
      showDialog(DIALOG_TYPE_QUESTION, oobwPrintDurationUserSaidYes, NULL, NULL);
    }

    
    if (nowHeat.T[currentTool].current != heatGetCurrentTemp(currentTool) ||
        nowHeat.T[currentTool].target != heatGetTargetTemp(currentTool))
    {
      nowHeat.T[currentTool].current = heatGetCurrentTemp(currentTool);
      nowHeat.T[currentTool].target = heatGetTargetTemp(currentTool);
      RAPID_SERIAL_LOOP();  
      reValueNozzle(EXT_ICON_POS);
    }

    
    if (nowHeat.T[BED].current != heatGetCurrentTemp(BED) || nowHeat.T[BED].target != heatGetTargetTemp(BED))
    {
      nowHeat.T[BED].current = heatGetCurrentTemp(BED);
      nowHeat.T[BED].target = heatGetTargetTemp(BED);
      RAPID_SERIAL_LOOP();  
      reValueBed(BED_ICON_POS);
    }

    
    if (nowFan[currentFan] != fanGetCurSpeed(currentFan))
    {
      nowFan[currentFan] = fanGetCurSpeed(currentFan);
      RAPID_SERIAL_LOOP();  
      reDrawFan(FAN_ICON_POS);
    }

    
    if (infoPrinting.size != 0)
    {
      
      if (time != infoPrinting.time ||
          infoPrinting.progress != MIN((uint64_t)infoPrinting.cur * 100 / infoPrinting.size, 100))
      {
        time = infoPrinting.time;
        infoPrinting.progress = MIN((uint64_t)infoPrinting.cur * 100 / infoPrinting.size, 100);
        RAPID_SERIAL_LOOP();  
        reDrawTime(TIM_ICON_POS);
        reDrawProgress(TIM_ICON_POS);
      }
    }
    else
    {
      if (infoPrinting.progress != 100)
      {
        infoPrinting.progress = 100;
        reDrawTime(TIM_ICON_POS);
        reDrawProgress(TIM_ICON_POS);
      }
    }

    
    if (curLayer != ((infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(Z_AXIS) : coordinateGetAxisTarget(Z_AXIS)))
    {
      curLayer = (infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(Z_AXIS) : coordinateGetAxisTarget(Z_AXIS);
      RAPID_SERIAL_LOOP();  
      reDrawLayer(Z_ICON_POS);
    }

    
    if (curspeed[currentSpeedID] != speedGetCurPercent(currentSpeedID))
    {
      curspeed[currentSpeedID] = speedGetCurPercent(currentSpeedID);
      RAPID_SERIAL_LOOP();  
      reDrawSpeed(SPD_ICON_POS);
    }

    if(f_RunningM600 == true)
    { 
      printingItems.items[KEY_ICON_4] = BLANK_PAUSE_RESUME_KEY;
      menuDrawItem(&printingItems.items[KEY_ICON_4], KEY_ICON_4);
    }

    
    if (lastPause != isPause())
    {
      lastPause = isPause();
      printingItems.items[KEY_ICON_4] = itemIsPause[lastPause];
      menuDrawItem(&printingItems.items[KEY_ICON_4], KEY_ICON_4);
    }

    
    if (lastPrinting != isPrinting())
    {
      lastPrinting = isPrinting();
      if (lastPrinting != true)  
      {
        if(bFirstPrintFromOOBWCW == TRUE)
        {
          OOBWCW_Step_7();
        }
        else
        {
          preparePrintSummary();

        }
      }
      return;  
    }

    toggleInfo();

    KEY_VALUES key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_4:  
        
        if (f_RunningM600 == true)
          break;

        if (isPrinting())
          setPrintPause(!isPause(), false);
        #ifndef TFT70_V3_0
          else
          {
            exitPrinting();
            clearInfoFile();
            infoMenu.cur = 0;
          }
        #endif
        break;

      case KEY_ICON_5:
        #ifdef TFT70_V3_0
          if (isPrinting())
            infoMenu.menu[++infoMenu.cur] = menuBabystep;
          else
          {
            exitPrinting();
            clearInfoFile();
            infoMenu.cur = 0;
          }
        #else
          infoMenu.menu[++infoMenu.cur] = menuBabystep;
        #endif
        break;

      case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuMore;
        break;

      case KEY_ICON_7:
        if (isPrinting())
        {
          setDialogText(LABEL_WARNING, LABEL_STOP_PRINT, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_ALERT, stopConfirm, NULL, NULL);
        }
        else
        {
          exitPrinting();
          infoMenu.cur--;
        }
        break;

      case KEY_INFOBOX:
        printInfoPopup();
        break;

      default:
        break;
    }

    loopProcess();
  }
}
