#ifndef _VARIANTS_H_
#define _VARIANTS_H_

#include "../Configuration.h"

/*
* hardware source
* TIM3 for Buzzer timer
* TIM6 for Neopixel RGB
* TIM7 for OS Timer
*/

#define ILI9341 2

#include "pin_TFT43_V3_0.h"

#define LCD_ENCODER_SUPPORT (defined(LCD_ENCA_PIN) && defined(LCD_ENCB_PIN) && defined(LCD_BTN_PIN))
#define ENC_ACTIVE_SIGNAL (defined(LCD_ENC_EN_PIN) && defined(ST7920_EMULATOR) && defined(LCD_ENCODER_SUPPORT))

#define LCD_DRIVER_IS(n) (TFTLCD_DRIVER == n)

#endif
