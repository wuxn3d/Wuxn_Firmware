#ifndef _HOME_OFFSET_CONTROL_H_
#define _HOME_OFFSET_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>


void homeOffsetEnable(bool skipZOffset);


void homeOffsetDisable(void);


bool homeOffsetGetStatus(void);


float homeOffsetSetValue(float value);


float homeOffsetGetValue(void);


float homeOffsetResetValue(void);


float homeOffsetDecreaseValue(float unit);


float homeOffsetIncreaseValue(float unit);


float homeOffsetUpdateValueByEncoder(float unit, int8_t direction);

#ifdef __cplusplus
}
#endif

#endif
