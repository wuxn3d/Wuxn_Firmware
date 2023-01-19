#ifndef _COORDINATE_H_
#define _COORDINATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "variants.h"

typedef enum
{
  X_AXIS = 0,
  Y_AXIS,
  Z_AXIS,
  E_AXIS,
  TOTAL_AXIS
} AXIS;


typedef struct
{
  float axis[TOTAL_AXIS];
  uint32_t feedrate;
} COORDINATE;

extern const char axis_id[TOTAL_AXIS];

#ifdef WUXN_CODE
  extern const char * axis_string[TOTAL_AXIS];
  extern bool f_waitingToConfirmMove;
  extern bool coordinateQueryWait;
 
  #define wX_MOVE_GCODE "G1 X%.2f\n" 
  #define wY_MOVE_GCODE "G1 Y%.2f\n"
  #define wZ_MOVE_GCODE "G1 Z%.2f\n"
  extern const char *const wMoveCmd[];
#endif

bool coorGetRelative(void);
void coorSetRelative(bool mode);
bool eGetRelative(void);
void eSetRelative(bool mode);
bool coordinateIsKnown(void);
void coordinateSetKnown(bool known);
float coordinateGetAxisTarget(AXIS axis);
void coordinateSetAxisTarget(AXIS axis, float position);
uint32_t coordinateGetFeedRate(void);
void coordinateSetFeedRate(uint32_t feedrate);
void coordinateGetAll(COORDINATE *tmp);
float coordinateGetExtruderActual(void);
void coordinateSetExtruderActualSteps(float steps);
float coordinateGetAxisActual(AXIS axis);
void coordinateSetAxisActual(AXIS axis, float position);
void coordinateQuerySetWait(bool wait);
void coordinateQuery(void);
#ifdef WUXN_CODE
  void wConfirmedMove(AXIS axis, float destination );  
#endif


#ifdef __cplusplus
}
#endif

#endif
