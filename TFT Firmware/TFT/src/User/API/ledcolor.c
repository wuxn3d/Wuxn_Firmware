#include "ledcolor.h"
#include "GPIO_Init.h"
#include "includes.h"

#ifdef LED_COLOR_PIN


const LABEL itemLedcolor[LED_COLOR_NUM] = {
  
  LABEL_OFF,
  LABEL_WHITE,
  LABEL_RED,
  LABEL_ORANGE,
  LABEL_YELLOW,
  LABEL_GREEN,
  LABEL_BLUE,
  LABEL_INDIGO,
  LABEL_VIOLET,
};

const  uint32_t led_color[LED_COLOR_NUM] = {
  LED_OFF,
  LED_WHITE,
  LED_RED,
  LED_ORANGE,
  LED_YELLOW,
  LED_GREEN,
  LED_BLUE,
  LED_INDIGO,
  LED_VIOLET
};

void knob_LED_Init(void)
{
  GPIO_InitSet(LED_COLOR_PIN, MGPIO_MODE_OUT_PP, 0);
  GPIO_SetLevel(LED_COLOR_PIN, 0);

  RCC->APB1ENR |= 1 << 4;  
  TIM6->CNT = 0;
  TIM6->PSC = 1 - 1;
  TIM6->SR = (uint16_t)~(1 << 0);

  WS2812_Send_DAT(led_color[infoSettings.knob_led_color]);  
}

void knob_LED_DeInit(void)
{
  GPIO_InitSet(LED_COLOR_PIN, MGPIO_MODE_IPN, 0);
}


#define NEOPIXEL_T0H_US 0.35  
#define NEOPIXEL_T1H_US 2.15  

void WS2812_Send_DAT(uint32_t ws2812_dat)
{
  uint16_t led_num;
  int8_t bit;
  uint16_t cycle = mcuClocks.PCLK1_Timer_Frequency * (0.000001 * (NEOPIXEL_T0H_US + NEOPIXEL_T1H_US)) / 2 - 1;  
  uint16_t code_0_tim_h_cnt = cycle * (NEOPIXEL_T0H_US / (NEOPIXEL_T0H_US + NEOPIXEL_T1H_US));  
  uint16_t code_1_tim_h_cnt = cycle - code_0_tim_h_cnt;

  __disable_irq();  
  TIM6->ARR = cycle;
  TIM6->CR1 |= 0x01;
  for (led_num=0; led_num < NEOPIXEL_PIXELS; led_num++)
  {
    for (bit = 23; bit >= 0; bit--)
    {
      TIM6->CNT = 0;
      WS2812_FAST_WRITE_HIGH();  
      if (ws2812_dat & (1 << bit))
      {
        while (TIM6->CNT < code_1_tim_h_cnt);
      } 
      else
      {
        while (TIM6->CNT < code_0_tim_h_cnt);
      }
      WS2812_FAST_WRITE_LOW();
      while (!TIM6->SR);
      TIM6->SR = 0;
    }
  }
  TIM6->CR1 &= ~0x01;
  __enable_irq();  
}

#endif