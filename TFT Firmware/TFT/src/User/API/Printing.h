#ifndef _PRINTING_H_
#define _PRINTING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "variants.h"
#include "ff.h"

#ifndef M27_WATCH_OTHER_SOURCES
  #define M27_WATCH_OTHER_SOURCES false
#endif

#ifndef M27_REFRESH
  #define M27_REFRESH 3
#endif

#ifdef RAPID_SERIAL_COMM
  #define RAPID_SERIAL_LOOP() loopBackEnd()
  #define RAPID_PRINTING_COMM() if (isPrinting() == true && infoSettings.serial_alwaysOn != 1) {loopBackEnd();}
#else
  #define RAPID_SERIAL_LOOP()
  #define RAPID_PRINTING_COMM()
#endif

#define SUMMARY_NAME_LEN 26  

typedef struct
{
  FIL file;
  uint32_t time;        
  uint32_t size;        
  uint32_t cur;         
  uint8_t  progress;
  bool     printing;    
  bool     pause;       
  bool     m0_pause;    
  bool     runout;      
  bool     model_icon;  
} PRINTING;

typedef struct
{
  
  char name[SUMMARY_NAME_LEN + 1];
  uint32_t time;
  float length;
  float weight;
  float cost;
} PRINTSUMMARY;

extern PRINTING infoPrinting;
extern PRINTSUMMARY infoPrintSummary;

bool isPrinting(void);
bool isPause(void);
bool isM0_Pause(void);
void setPrintingTime(uint32_t RTtime);
void setPrintSize(uint32_t size);
uint32_t getPrintSize(void);
uint32_t getPrintCur(void);
bool getPrintRunout(void);
void setPrintRunout(bool runout);
void setPrintCur(uint32_t cur);

void setRunoutAlarmTrue(void);
void setRunoutAlarmFalse(void);
bool getRunoutAlarm(void);

void setPrintModelIcon(bool exist);
bool getPrintModelIcon(void);

uint8_t getPrintProgress(void);
uint32_t getPrintTime(void);
void printSetUpdateWaiting(bool isWaiting);
uint8_t *getCurGcodeName(char *path);
void sendPrintCodes(uint8_t index);

void initPrintSummary(void);
void updateFilamentUsed(void);
void preparePrintSummary(void);

bool setPrintPause(bool is_pause, bool is_m0pause);
void exitPrinting(void);
void endPrinting(void);
void abortPrinting(void);
void printingFinished(void);
void shutdown(void);
void shutdownLoop(void);
void startShutdown(void);

void getGcodeFromFile(void);
void breakAndContinue(void);
void resumeAndPurge(void);
void resumeAndContinue(void);

void loopCheckPrinting(void);

#ifdef __cplusplus
}
#endif

#endif
