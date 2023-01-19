#ifndef _PREHEATMENU_H_
#define _PREHEATMENU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"
#include "Configuration.h"

typedef enum
{
  BOTH = 0,
  BED_PREHEAT = 1,
  NOZZLE0_PREHEAT = 2,
} TOOLPREHEAT;

#define notmuch PICON_LG_WIDTH - PREHEATICONSPACE / 2


#define PREHEATICONSPACE 50 
#define PREHEATNOZICON_X (LCD_WIDTH / 2 - PICON_LG_WIDTH - PREHEATICONSPACE / 2) 
                                                                                 
                                                                                 
#define PREHEATBEDICON_X (LCD_WIDTH / 2 + PREHEATICONSPACE / 2) 

#define X_MOTION_ICON_WIDTH 95
#define MOTORTEST_XY_XMOTOR_X  (LCD_WIDTH / 2 - X_MOTION_ICON_WIDTH - PREHEATICONSPACE / 2) 
#define MOTORTEST_XY_YMOTOR_X  (LCD_WIDTH / 2 + PREHEATICONSPACE / 2)  

void menuPreheat(void);
void wPurgeDialog(void);
void wSetZTriggerCheckFlag(bool bValue);
void wDisplayHeatingScreen(void);  

#ifdef __cplusplus
}
#endif

#endif
