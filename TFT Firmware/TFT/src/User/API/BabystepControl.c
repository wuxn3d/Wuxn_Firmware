#include "BabystepControl.h"
#include "includes.h"

static float babystep_value = BABYSTEP_DEFAULT_VALUE;


float babystepReset(void)
{
  babystep_value = BABYSTEP_DEFAULT_VALUE;

  return babystep_value;
}


float babystepGetValue(void)
{
  return babystep_value;
}


float babystepResetValue(void)
{
  if (babystep_value == BABYSTEP_DEFAULT_VALUE)  
    return babystep_value;

  int step_count;
  float last_unit;
  float processed_baby_step = 0.0f;
  int8_t neg = 1;

  if (babystep_value < 0.0f)
    neg = -1;

  step_count = (babystep_value * neg) / BABYSTEP_MAX_STEP;
  for (; step_count > 0; step_count--)
  {
    if (infoMachineSettings.firmwareType == FW_SMOOTHIEWARE)
      mustStoreCmd("G43.2 Z%.2f\n", -(BABYSTEP_MAX_STEP * neg));
    else
      mustStoreCmd("M290 Z%.2f\n", -(BABYSTEP_MAX_STEP * neg));
    processed_baby_step += BABYSTEP_MAX_STEP;
  }

  last_unit = (babystep_value * neg) - processed_baby_step;
  if (last_unit > 0.0f)
  {
    if (infoMachineSettings.firmwareType == FW_SMOOTHIEWARE)
      mustStoreCmd("G43.2 Z%.2f\n", -(last_unit * neg));
    else
      mustStoreCmd("M290 Z%.2f\n", -(last_unit * neg));
    processed_baby_step += last_unit;
  }

  babystep_value -= (processed_baby_step * neg);

  return babystep_value;
}


float babystepDecreaseValue(float unit)
{
  if (babystep_value > BABYSTEP_MIN_VALUE)
  {
    float diff = babystep_value - BABYSTEP_MIN_VALUE;

    unit = (diff > unit) ? unit : diff;
    if (infoMachineSettings.firmwareType == FW_SMOOTHIEWARE)
      mustStoreCmd("G43.2 Z-%.2f\n", unit);
    else
      mustStoreCmd("M290 Z-%.2f\n", unit);
    babystep_value -= unit;
  }

  return babystep_value;
}


float babystepIncreaseValue(float unit)
{
  if (babystep_value < BABYSTEP_MAX_VALUE)
  {
    float diff = BABYSTEP_MAX_VALUE - babystep_value;

    unit = (diff > unit) ? unit : diff;
    if (infoMachineSettings.firmwareType == FW_SMOOTHIEWARE)
      mustStoreCmd("G43.2 Z%.2f\n", unit);
    else
      mustStoreCmd("M290 Z%.2f\n", unit);
    babystep_value += unit;
  }

  return babystep_value;
}


float babystepUpdateValueByEncoder(float unit, int8_t direction)
{
  float overall_unit = (direction > 0) ? (direction * unit) : (-direction * unit);  

  if (direction < 0)  
    babystepDecreaseValue(overall_unit);
  else
    babystepIncreaseValue(overall_unit);

  return babystep_value;
}
