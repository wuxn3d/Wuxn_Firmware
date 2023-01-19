#include "includes.h"
#include "xpt2046.h"
#include "GPIO_Init.h"
#include "Selectmode.h"

bool freshBoot = true;
bool skipMode = false;

SETTINGS SettingsBeforeOOBWCW; 

const GUI_RECT rect_of_mode[MODE_COUNT] = {
  
  {1 * SPACE_SELEX + 0 * ICON_WIDTH, SPACE_SELEY, 1 * SPACE_SELEX + 1 * ICON_WIDTH, SPACE_SELEY + ICON_HEIGHT},
  {3 * SPACE_SELEX + 1 * ICON_WIDTH, SPACE_SELEY, 3 * SPACE_SELEX + 2 * ICON_WIDTH, SPACE_SELEY + ICON_HEIGHT},
};

const uint8_t icon_mode [MODE_COUNT] = {
  ICON_BACKGROUND,
  ICON_BACKGROUND,
};

void drawModeIcon(void)
{
  for (uint8_t i = 0;i < MODE_COUNT; i++)
  {
    ICON_ReadDisplay(rect_of_mode[i].x0, rect_of_mode[i].y0, icon_mode[i]);
  }

  const GUI_RECT mode_title_rect[MODE_COUNT] = {
    {0,           rect_of_mode[0].y1 + BYTE_HEIGHT / 2, text_startx, rect_of_mode[0].y1 + BYTE_HEIGHT / 2 + BYTE_HEIGHT},
    {text_startx, rect_of_mode[0].y1 + BYTE_HEIGHT / 2, LCD_WIDTH,   rect_of_mode[0].y1 + BYTE_HEIGHT / 2 + BYTE_HEIGHT},
  };

  GUI_RestoreColorDefault();
  GUI_DispStringInPrect(&mode_title_rect[0],(uint8_t *)"Marlin Mode");
  GUI_DispStringInPrect(&mode_title_rect[1],(uint8_t *)"Touch Mode");
}

bool LCD_ReadPen(uint16_t intervals)
{
  static uint32_t TouchTime = 0;
  if (!XPT2046_Read_Pen())
  {
    if (OS_GetTimeMs() - TouchTime >= intervals)
    {
      TouchTime = OS_GetTimeMs();
      return true;
    }
  }
  else
  {
    TouchTime = OS_GetTimeMs();
  }
  return false;
}

static inline MKEY_VALUES MKeyGetValue(void)
{
  return (MKEY_VALUES)KEY_GetValue(COUNT(rect_of_mode), rect_of_mode);
}

void drawSelectedMode(int8_t nowMode)
{
  const uint8_t border_off = 4;
  for (uint8_t i = 0; i < MODE_COUNT; i++)
  {
    GUI_SetColor(i == nowMode ? infoSettings.marlin_mode_font_color : BACKGROUND_COLOR);
    GUI_DrawRect(rect_of_mode[i].x0 - border_off, rect_of_mode[i].y0 - border_off,
                 rect_of_mode[i].x1 + border_off, rect_of_mode[i].y1 + border_off);
  }
  GUI_RestoreColorDefault();
}

void loopCheckMode(void)
{





  if (isPrinting() || infoHost.printing || skipMode)
    return;

  if (infoMenu.menu[infoMenu.cur] == menuMode)
    return;
}


void menuMode(void)
{
  int8_t nowMode = infoSettings.mode;
  TSC_ReDrawIcon = NULL;  

  GUI_Clear(infoSettings.bg_color);
  drawModeIcon();
  drawSelectedMode(nowMode);

  #if LCD_ENCODER_SUPPORT
    while (!XPT2046_Read_Pen() || encoder_ReadBtn(LCD_BUTTON_INTERVALS))
      ;  
  #else
    while (!XPT2046_Read_Pen())
      ;  
  #endif

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuMode)
  {
    MKEY_VALUES key_num = MKeyGetValue();

    if (key_num == MKEY_0 || key_num == MKEY_1)
    {
      nowMode = key_num;
      break;
    }

    #if LCD_ENCODER_SUPPORT
      if (encoderPosition)
      {
        nowMode = NOBEYOND(0, nowMode + encoderPosition, MODE_COUNT - 1);
        drawSelectedMode(nowMode);
        encoderPosition = 0;
      }

      if (encoder_ReadBtn(LCD_BUTTON_INTERVALS))
        break;

      loopCheckEncoderSteps();
    #endif

    #ifdef LCD_LED_PWM_CHANNEL
      loopDimTimer();
    #endif

    if (infoSettings.mode == MODE_SERIAL_TSC || infoSettings.serial_alwaysOn == 1)
      loopBackEnd();
  }

  if (infoSettings.mode != nowMode)
  {
    infoSettings.mode = nowMode;
    storePara();
  }

  switchMode();
}


static inline void setupModeHardware(uint8_t mode)
{
  if (infoSettings.serial_alwaysOn == ENABLED)
    Serial_ReSourceInit();  

  if (mode == MODE_SERIAL_TSC)
  {
    Serial_ReSourceInit();  
    #ifdef BUZZER_PIN  
      Buzzer_Config();
    #endif

    #if LED_COLOR_PIN  
      #ifndef KEEP_KNOB_LED_COLOR_MARLIN_MODE
      knob_LED_Init();
      #endif
    #endif

    #if ENC_ACTIVE_SIGNAL  
     setEncActiveSignal(0);
    #endif
  }
  else
  {
    if (infoSettings.serial_alwaysOn == DISABLED)
      Serial_ReSourceDeInit();  

    #ifdef BUZZER_PIN  
      Buzzer_DeConfig();
    #endif

    #if LED_COLOR_PIN
      #ifndef KEEP_KNOB_LED_COLOR_MARLIN_MODE  
        knob_LED_DeInit();
      #endif
    #endif

    #if ENC_ACTIVE_SIGNAL  
      setEncActiveSignal(1);
    #endif

    #if !defined(MKS_32_V1_4) && !defined(MKS_28_V1_0)
      
      SD_DeInit();
    #endif
  }
}


void switchMode(void)
{
  infoMenu.cur = 0;
  setupModeHardware(infoSettings.mode);

  switch (infoSettings.mode)
  {
    case MODE_SERIAL_TSC:
      GUI_RestoreColorDefault();
     
      #ifdef SHOW_BTT_BOOTSCREEN
        if (freshBoot)
        {
         
         
          heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);
          LOGO_ReadDisplay();
          updateNextHeatCheckTime();  
         
         /*
          while (OS_GetTimeMs() - startUpTime < BTT_BOOTSCREEN_TIME)  
          {
            loopProcess();
          }
          */
          while (infoHost.connected == false)  
          {
            loopProcess();
          }

          
          
          SettingsBeforeOOBWCW = infoSettings;
          
          
          
          
          if(infoSettings.enable_oobwcw == ENABLE )
          {
            infoMenu.menu[infoMenu.cur] = menuStatus;
            infoMenu.menu[++infoMenu.cur] = menuOOBWCW;
          }
	        else if (infoSettings.status_screen == 1)  
         
          {
            infoMenu.menu[infoMenu.cur] = menuStatus;  
          }
          else
          {
            infoMenu.menu[infoMenu.cur] = menuMain;  
          }
          heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
          freshBoot = false;
        }
      #endif 
      break;  

    case MODE_MARLIN:
      #ifdef HAS_EMULATOR
        if (infoSettings.serial_alwaysOn == ENABLED)
          updateNextHeatCheckTime();  
        infoMenu.menu[infoMenu.cur] = menuMarlinMode;
      #endif
      break;
  }
}
