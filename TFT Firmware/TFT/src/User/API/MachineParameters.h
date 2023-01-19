#ifndef _MACHINEPARAMETERS_H_
#define _MACHINEPARAMETERS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "menu.h"
#include "coordinate.h"

typedef enum
{
  P_STEPS_PER_MM = 0,
  P_MAX_FEED_RATE,
  P_MAX_ACCELERATION,
  P_ACCELERATION,
  P_JERK,
  P_JUNCTION_DEVIATION,
  P_HOME_OFFSET,
  P_FWRETRACT,
  P_FWRECOVER,
  P_AUTO_RETRACT,
  P_HOTEND_OFFSET,
  P_ABL_STATE,
  P_PROBE_OFFSET,
  P_LIN_ADV,
  P_FILAMENT_SETTING,
  P_CURRENT,
  P_BUMPSENSITIVITY,
  P_HYBRID_THRESHOLD,
  P_STEALTH_CHOP,
  P_MBL_OFFSET,
  
  PARAMETERS_COUNT,
} PARAMETER_NAME;

typedef enum
{
  P_SAVE_SETTINGS = 0,
  P_RESTORE_SETTINGS,
  P_RESET_SETTINGS,
  P_SETTINGS_COUNT,
} PARAMETER_SETTINGS;

typedef enum
{
  X_STEPPER = 0,
  Y_STEPPER,
  Z_STEPPER,
  E_STEPPER,
  E2_STEPPER,
  STEPPER_COUNT
} STEPPERS;

typedef enum
{
  VAL_TYPE_INT,
  VAL_TYPE_NEG_INT,
  VAL_TYPE_FLOAT,
  VAL_TYPE_NEG_FLOAT,
} VAL_TYPE;

typedef struct
{
  float StepsPerMM[STEPPER_COUNT];
  float MaxFeedRate[STEPPER_COUNT];
  float MaxAcceleration[STEPPER_COUNT];
  float Acceleration[3];
  float Jerk[4];
  float JunctionDeviation[1];
  float HomeOffset[3];
  float FwRetract[4];
  float FwRecover[4];
  float AutoRetract[1];
  float HotendOffset[3];
  float ABLState[2];
  float ProbeOffset[3];
  float LinAdvance[2];
  float FilamentSetting[3];
  float Current[STEPPER_COUNT];
  float BumpSensitivity[3];
  float HybridThreshold[STEPPER_COUNT];
  float StealthChop[5];
  float MblOffset[1];
} PARAMETERS;

extern PARAMETERS infoParameters;

extern char *const axisDisplayID[STEPPER_COUNT];
extern const LABEL accel_disp_ID[];
extern const LABEL junction_deviation_disp_ID[];
extern const LABEL retract_disp_ID[];
extern const LABEL recover_disp_ID[];
extern const LABEL retract_auto_ID[];


void setParameterStatus(PARAMETER_NAME name, bool status);


uint8_t getParameterStatus(PARAMETER_NAME name);


uint8_t getEnabledParameterCount(void);



PARAMETER_NAME getEnabledParameter(uint8_t index);

float getParameter(PARAMETER_NAME name, uint8_t index);
void setParameter(PARAMETER_NAME name, uint8_t index, float val);


uint8_t getParameterElementCount(PARAMETER_NAME para);


VAL_TYPE getParameterValType(PARAMETER_NAME para, uint8_t index);


void setDualStepperStatus(uint8_t index, bool status);


bool getDualStepperStatus(uint8_t index);


void sendParameterCmd(PARAMETER_NAME para_index, uint8_t stepper_index, float Value);


void saveEepromSettings(void);


void restoreEepromSettings(void);


void resetEepromSettings(void);

#ifdef WUXN_CODE

void  wSaveEepromSettingsUserSaidYes(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
