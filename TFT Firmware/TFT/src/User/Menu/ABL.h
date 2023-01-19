#ifndef _ABL_H_
#define _ABL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>


void ablUpdateStatus(bool succeeded);


void ablStart(void);


void menuUBLSave(void);

/*
void menuUBLLoad(void);
*/

#ifdef __cplusplus
}
#endif

#endif
