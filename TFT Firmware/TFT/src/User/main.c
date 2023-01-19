#include "includes.h"

HOST   infoHost;   
MENU   infoMenu;   
CLOCKS mcuClocks;  

void mcu_GetClocksFreq(CLOCKS *clk)
{
  RCC_GetClocksFreq(&clk->rccClocks);
  if (clk->rccClocks.PCLK1_Frequency < clk->rccClocks.HCLK_Frequency)  
    clk->PCLK1_Timer_Frequency = clk->rccClocks.PCLK1_Frequency * 2;
  else
    clk->PCLK1_Timer_Frequency = clk->rccClocks.PCLK1_Frequency;

  if (clk->rccClocks.PCLK2_Frequency < clk->rccClocks.HCLK_Frequency)
    clk->PCLK2_Timer_Frequency = clk->rccClocks.PCLK2_Frequency * 2;
  else
    clk->PCLK2_Timer_Frequency = clk->rccClocks.PCLK2_Frequency;
}

void Hardware_GenericInit(void)
{
  mcu_GetClocksFreq(&mcuClocks);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  Delay_init();

  #ifdef DISABLE_JTAG
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  
  #endif

  #ifdef DISABLE_DEBUG
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);  
  #endif

  #if defined(MKS_32_V1_4) || defined (MKS_28_V1_0)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
  #endif

  XPT2046_Init();
  OS_TimerInitMs();        
  W25Qxx_Init();
  LCD_Init();
  readStoredPara();        
  LCD_RefreshDirection();  
  scanUpdates();           
  checkflashSign();        
  initMachineSetting();    

  #ifdef LED_COLOR_PIN
    knob_LED_Init();
  #endif

  #ifdef BUZZER_PIN
    Buzzer_Config();
  #endif

  #if !defined(MKS_32_V1_4) && !defined (MKS_28_V1_0)
    
    SD_DeInit();
  #endif

  #if LCD_ENCODER_SUPPORT
    HW_EncoderInit();
  #endif
  #if ENC_ACTIVE_SIGNAL
    HW_EncActiveSignalInit();
  #endif

  #ifdef PS_ON_PIN
    PS_ON_Init();
  #endif

  #ifdef FIL_RUNOUT_PIN
    FIL_Runout_Init();
  #endif

  #ifdef U_DISK_SUPPORT
    USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &USBH_MSC_cb, &USR_cb);
  #endif

  if (readIsTSCExist() == false)  
  {
    TSC_Calibration();
    storePara();
  }
  else if (readIsRestored())
  {
    storePara();
  }

  #ifdef LCD_LED_PWM_CHANNEL
    Set_LCD_Brightness(LCD_BRIGHTNESS[infoSettings.lcd_brightness]);
  #endif
  switchMode();
}

int main(void)
{
  SystemClockInit();

  SCB->VTOR = VECT_TAB_FLASH;

  Hardware_GenericInit();

  for (; ;)
  {
    (*infoMenu.menu[infoMenu.cur])();
  }
}
