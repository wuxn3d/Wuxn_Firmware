#ifndef _FANTEST_H_
#define _FANTEST_H_

#ifndef WUXN_CODE  

#ifdef __cplusplus
extern "C" {
#endif


typedef enum  
{ 
  s_FanTest1, 
  s_FanTest1Completed, 
  s_FanTest2, 
  s_ExitFanTesting 
} FanTestStates;

void menuFanTest(void);
void FanTestUserSaidYes();

#ifdef __cplusplus
}
#endif

#endif 

#endif
