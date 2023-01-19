#ifndef _PROBE_HEIGHT_CONTROL_H_
#define _PROBE_HEIGHT_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>



void probeHeightEnable(void);



void probeHeightDisable(void);


void probeHeightStart(float initialHeight, bool relativeHeight);


void probeHeightStop(float raisedHeight);


void probeHeightRelative(void);


void probeHeightAbsolute(void);


void probeHeightMove(float unit, int8_t direction);


void probeHeightQueryCoord(void);

#ifdef __cplusplus
}
#endif

#endif
