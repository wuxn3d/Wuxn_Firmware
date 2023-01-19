#ifndef _BED_LEVELING_H_
#define _BED_LEVELING_H_

#ifdef __cplusplus
extern "C" {
#endif


void menuBedLeveling(void);
void wSetAutoBedLevelIconStateForMesh(void);
void wSetAutoBedLevelIconStateForComplete(void);
void wAutoBedLevelingUserSaidStart(void);
void wSetCouplingsGCode(void);  
void wAutoBedlevelIconDisplay(void);
void menuBedLevelingPhase2 (void);  
void wDoTheBedLeveling (void);  

typedef enum
{
  AUTO_BEDLEVEL_SET_COUPLING,
  AUTO_BEDLEVEL_Z_ALIGN,
  
  AUTO_BEDLEVEL_COMPLETE
} AutoBedLevelIconStatus;

#ifdef __cplusplus
}
#endif

#endif
