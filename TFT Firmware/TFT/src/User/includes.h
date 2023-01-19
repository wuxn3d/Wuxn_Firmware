#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#include "Configuration.h"
#include "variants.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "my_misc.h"
#include "printf/printf.h"
#include "os_timer.h"
#include "delay.h"
#include "boot.h"
#include "ScreenShot.h"
#include "Colors.h"
#include "lcd.h"
#include "LCD_Init.h"
#include "lcd_dma.h"
#include "GUI.h"
#include "Language.h"
#include "utf8_decode.h"
#include "uart.h"
#include "Serial.h"
#include "spi.h"
#include "sw_spi.h"
#include "CircularQueue.h"
#include "spi_slave.h"
#include "timer_pwm.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_msc_core.h"
#include "sd.h"
#include "w25qxx.h"
#include "xpt2046.h"
#include "buzzer.h"
#include "LCD_Encoder.h"
#include "ST7920_Emulator.h"
#include "HD44780_Emulator.h"
#include "ui_draw.h"
#include "touch_process.h"
#include "serialConnection.h"
#include "interfaceCmd.h"
#include "coordinate.h"
#include "ff.h"
#include "Vfs/vfs.h"
#include "myfatfs.h"
#include "Gcode/gcode.h"
#include "Gcode/mygcodefs.h"
#include "flashStore.h"
#include "parseACK.h"
#include "Selectmode.h"
#include "MarlinMode.h"
#include "Temperature.h"
#include "Settings.h"
#include "Printing.h"
#include "MachineParameters.h"
#include "FanControl.h"
#include "SpeedControl.h"
#include "BabystepControl.h"
#include "ProbeOffsetControl.h"
#include "ProbeHeightControl.h"
#include "HomeOffsetControl.h"
#include "CaseLightControl.h"
#include "extend.h"
#include "menu.h"
#include "list_item.h"
#include "list_widget.h"
#include "common.h"
#include "Popup.h"
#include "Numpad.h"
#include "Notification.h"
#include "SanityCheck.h"
#include "MainPage.h"
#include "Heat.h"
#include "PreheatMenu.h"
#include "Move.h"
#include "Home.h"
#include "Print.h"
#include "Printing.h"
#include "More.h"
#include "Speed.h"
#include "ledcolor.h"
#include "Parametersetting.h"
#include "NotificationMenu.h"
#include "Babystep.h"
#include "Extrude.h"
#include "LoadUnload.h"
#include "Macros.h"
#include "Fan.h"
#include "SettingsMenu.h"
#include "PrintingMenu.h"
#include "ScreenSettings.h"
#include "FeatureSettings.h"
#include "SendGcode.h"
#include "Leveling.h"
#include "BedLeveling.h"
#include "BedLevelingLayer2.h"
#include "MBL.h"
#include "ABL.h"
#include "BLTouch.h"
#include "ZOffset.h"
#include "PowerFailed.h"
#include "UnifiedMove.h"
#include "UnifiedHeat.h"
#include "StatusScreen.h"
#include "Tuning.h"
#include "Pid.h"
#include "TuneExtruder.h"
#include "MeshTuner.h"
#include "MeshEditor.h"
#include "CaseLight.h"
#include "Wizards.h"
#include "Diagnostics.h"
#include "Skew_Calib.h"
#include "First_Layer.h"
#include "E_Calib.h"
#include "ProbeTest.h"
#include "XYZForce.h"
#include "FactoryReset.h"
#include "wAutoFilamentLoad.h"
#include "FilamentIRTest.h"
#include "MotorTest.h"
#include "FirstPrint.h"  
#include "OOBWCW.h"      

#define MAX_MENU_DEPTH 20       

typedef void (*FP_MENU)(void);

typedef struct
{
  FP_MENU menu[MAX_MENU_DEPTH];  
  uint8_t cur;                   
} MENU;

extern MENU infoMenu;

typedef struct
{
  bool wait;              
  bool rx_ok[_UART_CNT];  
  bool connected;         
  bool printing;          
} HOST;

extern HOST infoHost;

typedef struct
{
  RCC_ClocksTypeDef rccClocks;
  uint32_t PCLK1_Timer_Frequency;
  uint32_t PCLK2_Timer_Frequency;
} CLOCKS;

extern CLOCKS mcuClocks;

#endif
