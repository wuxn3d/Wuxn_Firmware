#ifndef _PIN_TFT43_V3_0_H_ // modify to actual filename !!!
#define _PIN_TFT43_V3_0_H_ // modify to actual filename !!!

#include "stm32f2xx.h" 

  #define TFT_RESOLUTION
  #include "./Resolution/TFT_480X272.h"

  #define ROOT_DIR "TFT43"

// Hardware version config
  #define HARDWARE_VERSION "TFT43_V3.0"


#define FIRMWARE_VERSION "Wuxn_TFT_Fork_120522"

// LCD interface
  #define TFTLCD_DRIVER               SSD1963  
  #define TFTLCD_DRIVER_SPEED         0x10     
  #define TFTLCD_0_DEGREE_REG_VALUE   0x00
  #define TFTLCD_180_DEGREE_REG_VALUE 0x03

  #define SSD1963_LCD_PARA
  #define SSD_DCLK_FREQUENCY  12  

  #define SSD_HOR_PULSE_WIDTH 1
  #define SSD_HOR_BACK_PORCH  43
  #define SSD_HOR_FRONT_PORCH 2

  #define SSD_VER_PULSE_WIDTH 1
  #define SSD_VER_BACK_PORCH  12
  #define SSD_VER_FRONT_PORCH 1

  #define NEOPIXEL_PIXELS 2

#define LED_COLOR_PIN PC7
#define WS2812_FAST_WRITE_HIGH() GPIOC->BSRRL = 1<<7
#define WS2812_FAST_WRITE_LOW()  GPIOC->BSRRH = 1<<7

#define STM32_HAS_FSMC  

#define LCD_DATA_16BIT 1  

#define LCD_LED_PIN           PD12
#define LCD_LED_PIN_ALTERNATE GPIO_AF_TIM4
#define LCD_LED_PWM_CHANNEL   _TIM4_CH1

#define SERIAL_PORT   _USART2  
#define SERIAL_PORT_2 _USART1
#define SERIAL_PORT_3 _USART3
#define SERIAL_PORT_4 _UART4

#define XPT2046_CS   PE6
#define XPT2046_SCK  PE5
#define XPT2046_MISO PE4
#define XPT2046_MOSI PE3
#define XPT2046_TPEN PC13

#define SD_SPI_SUPPORT

#ifdef SD_SPI_SUPPORT
  #define SD_LOW_SPEED  7      
  #define SD_HIGH_SPEED 0      
  #define SD_SPI        _SPI1
  #define SD_CS_PIN     PA4
#endif

#define SD_CD_PIN PC4

#define W25Qxx_SPEED  0
#define W25Qxx_SPI    _SPI3
#define W25Qxx_CS_PIN PB6

#define ST7920_EMULATOR 
#ifdef ST7920_EMULATOR
  #define ST7920_SPI _SPI2
#endif

#define BUZZER_PIN PD13

#define LCD_ENCA_PIN   PA8
#define LCD_ENCB_PIN   PC9
#define LCD_BTN_PIN    PC8
#define LCD_ENC_EN_PIN PC6

#define U_DISK_SUPPORT
#define USE_USB_OTG_FS


  #define PS_ON_PIN PC12  

  #define FIL_RUNOUT_PIN PA15  


#endif

