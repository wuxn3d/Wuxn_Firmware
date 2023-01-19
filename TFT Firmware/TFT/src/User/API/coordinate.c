#include "coordinate.h"
#include "string.h"
#include "includes.h"

const char axis_id[TOTAL_AXIS] = {'X', 'Y', 'Z', 'E'};  
#ifdef WUXN_CODE
  const char * axis_string[TOTAL_AXIS] = {"X-Axis", "Y-Axis", "Z-Axis", "E_Axis"};  
  char tempMsg[120];
  bool f_waitingToConfirmMove;
  const char *const wMoveCmd[] = {wX_MOVE_GCODE, wY_MOVE_GCODE, wZ_MOVE_GCODE};
#endif

static COORDINATE targetPosition = {{0.0f, 0.0f, 0.0f, 0.0f}, 3000};
static COORDINATE curPosition = {{0.0f, 0.0f, 0.0f, 0.0f}, 3000};

/**
 * Obtained from "M114 E" instead of "M114", Because the coordinates of "M114" are not real-time coordinates.
 * It may be replaced by "M114 R".
 */
static float extruderPostion = 0.0f;

static bool relative_mode = false;
static bool relative_e = false;


static bool position_known = false;
#ifdef WUXN_CODE
bool coordinateQueryWait = false;
#else
static bool coordinateQueryWait = false;
#endif

bool coorGetRelative(void)
{
  return relative_mode;
}

void coorSetRelative(bool mode)
{
  relative_mode = mode;
}

bool eGetRelative(void)
{
  return relative_e;
}

void eSetRelative(bool mode)
{
  relative_e = mode;
}

bool coordinateIsKnown(void)
{
  return position_known;
}

void coordinateSetKnown(bool known)
{
  position_known = known;
}

float coordinateGetAxisTarget(AXIS axis)
{
  return targetPosition.axis[axis];
}

void coordinateSetAxisTarget(AXIS axis, float position)
{
  bool r = (axis == E_AXIS) ? relative_e || relative_mode : relative_mode;

  if (r == false)
  {
    targetPosition.axis[axis] = position;
  }
  else
  {
    targetPosition.axis[axis] += position;
  }
}

uint32_t coordinateGetFeedRate(void)
{
  return targetPosition.feedrate;
}

void coordinateSetFeedRate(uint32_t feedrate)
{
  targetPosition.feedrate = feedrate;
}

void coordinateGetAll(COORDINATE *tmp)
{
  memcpy(tmp, &targetPosition, sizeof(targetPosition));
}

float coordinateGetExtruderActual(void)
{
  return extruderPostion;
}

void coordinateSetExtruderActualSteps(float steps)
{
  curPosition.axis[E_AXIS] = extruderPostion = steps / getParameter(P_STEPS_PER_MM, E_AXIS);
}

float coordinateGetAxisActual(AXIS axis)
{
  return curPosition.axis[axis];
}

void coordinateSetAxisActual(AXIS axis, float position)
{
  curPosition.axis[axis] = position;
}

void coordinateQuerySetWait(bool wait)
{
  coordinateQueryWait = wait;
}

void coordinateQuery(void)
{
  if (infoHost.connected == true && infoHost.wait == false && !coordinateQueryWait)
  {
    coordinateQueryWait = storeCmd("M114\n");
  }
}

#ifdef WUXN_CODE
void wConfirmedMove(AXIS axis, float destination)



{
 
  coordinateSetAxisTarget(axis, destination); 
  mustStoreCmd(wMoveCmd[axis], coordinateGetAxisTarget(axis));
  mustStoreCmd("M400\n");  
  mustStoreCmd("M114\n");  
  
}
#endif

