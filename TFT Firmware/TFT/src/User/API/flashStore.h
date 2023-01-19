#ifndef _FLASHSTORE_H_
#define _FLASHSTORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "variants.h"
#include "Settings.h"

#ifdef WUXN_CODE
#define PARA_SIZE 512  
#else
#define PARA_SIZE 256  
#endif

void readStoredPara(void);
void storePara(void);
bool readIsTSCExist(void);
bool readIsRestored(void);

#ifdef __cplusplus
}
#endif

#endif
