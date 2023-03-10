#ifndef _PARAMETERSETTING_H_
#define _PARAMETERSETTING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "menu.h"
#include "MachineParameters.h"


#define GLOBALICON_WIDTH    (BYTE_WIDTH * 2)
#define GLOBALICON_HEIGHT   GLOBALICON_WIDTH
#define GLOBALICON_INTERVAL 2

void menuParameterSettings(void);

void loopTemperatureStatus(void);
int16_t drawTemperatureStatus(void);

#ifdef __cplusplus
}
#endif

#endif
