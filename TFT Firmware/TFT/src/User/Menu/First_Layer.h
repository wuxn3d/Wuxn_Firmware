#ifndef _FIRST_LAYER_H_
#define _FIRST_LAYER_H_

#ifdef __cplusplus
extern "C" {
#endif

void menuFirst_Layer(void);
void wSendGcodeAutoFilamentLoad(void);
void wPingIRSensorFromFirstLayerOffset(void);  

#ifdef __cplusplus
}
#endif

#endif
