#ifndef _BABYSTEP_CONTROL_H_
#define _BABYSTEP_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


float babystepReset(void);


float babystepGetValue(void);


float babystepResetValue(void);


float babystepDecreaseValue(float unit);


float babystepIncreaseValue(float unit);


float babystepUpdateValueByEncoder(float unit, int8_t direction);

#ifdef __cplusplus
}
#endif

#endif
