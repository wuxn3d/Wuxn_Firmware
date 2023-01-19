#ifndef _WAUTOFILAMENTLOAD_H_
//#define _WAUTOFILAMENTLOADCOORDINATE_H_  //062622
#define _WAUTOFILAMENTLOAD_H_ //062622

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
//#include "variants.h"

//bool f_wAutoFilamentLoadTime;/062622 next line moved here from wAutoFilamentLoad.c
enum wAutoFilamentStates {s_wAutoFilamentNotReady, s_wAutoFilamentLoadReady, s_wAutoFilamentPreheat, s_wAutoFilamentPurge,
                          s_wAutoFilamentCheckColor, s_wAutoFilamentColorCorrect };
bool f_wAutoFilamentLoadSentMe;
bool f_suspendLoopwAutoFilament;
extern bool FilamentPresent (void);

void initwAutoFilamentLoad (void); // 062622
void loopwAutoFilament (void);


#ifdef __cplusplus
}
#endif

#endif