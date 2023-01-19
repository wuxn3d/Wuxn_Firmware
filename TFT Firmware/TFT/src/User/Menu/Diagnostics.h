#ifndef _DIAGNOSTICS_H_
#define _DIAGNOSTICS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ICON_X_POSITION       70               

void menuDiagnostics(void);
void wMotorCheckUserSaidYes();  
void wClearFactoryResetStatus();  
void wFanTest(); 
void wFilamentCheck(void);     

#ifdef __cplusplus
}
#endif

#endif

