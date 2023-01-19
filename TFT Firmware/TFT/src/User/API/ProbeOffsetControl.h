#ifndef _PROBE_OFFSET_CONTROL_H_
#define _PROBE_OFFSET_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>


void probeOffsetEnable(bool skipZOffset);


void probeOffsetDisable(void);


bool probeOffsetGetStatus(void);


float probeOffsetSetValue(float value);


float probeOffsetGetValue(void);


float probeOffsetResetValue(void);


float probeOffsetDecreaseValue(float unit);


float probeOffsetIncreaseValue(float unit);


float probeOffsetUpdateValueByEncoder(float unit, int8_t direction);

#ifdef __cplusplus
}
#endif

#endif
