#include "HomeOffsetControl.h"
#include "includes.h"

static float z_offset_value = HOME_Z_OFFSET_DEFAULT_VALUE;
static bool home_offset_enabled = false;


void homeOffsetEnable(bool skipZOffset)
{
  home_offset_enabled = true;

  probeHeightEnable();  

  
  mustStoreCmd("G28\n");          
  probeHeightStart(0.0f, false);  
  probeHeightRelative();          
}


void homeOffsetDisable(void)
{
  home_offset_enabled = false;

  
  mustStoreCmd("G28\n");  
  probeHeightAbsolute();  

  probeHeightDisable();  
}


bool homeOffsetGetStatus(void)
{
  return home_offset_enabled;
}


float homeOffsetSetValue(float value)
{
  mustStoreCmd("M206 Z%.2f\n", value);
  mustStoreCmd("M206\n");  
  z_offset_value = value;

  return z_offset_value;
}


float homeOffsetGetValue(void)
{
  z_offset_value = getParameter(P_HOME_OFFSET, Z_STEPPER);

  return z_offset_value;
}


float homeOffsetResetValue(void)
{
  if (z_offset_value == HOME_Z_OFFSET_DEFAULT_VALUE)  
    return z_offset_value;

  float unit = z_offset_value - HOME_Z_OFFSET_DEFAULT_VALUE;

  z_offset_value = HOME_Z_OFFSET_DEFAULT_VALUE;
  mustStoreCmd("M206 Z%.2f\n", z_offset_value);  
  mustStoreCmd("G1 Z%.2f\n", unit);              

  return z_offset_value;
}


float homeOffsetDecreaseValue(float unit)
{
  if (z_offset_value > HOME_Z_OFFSET_MIN_VALUE)
  {
    float diff = z_offset_value - HOME_Z_OFFSET_MIN_VALUE;

    unit = (diff > unit) ? unit : diff;
    z_offset_value += unit;
    mustStoreCmd("M206 Z%.2f\n", z_offset_value);  
    mustStoreCmd("G1 Z%.2f\n", -unit);             
  }

  return z_offset_value;
}


float homeOffsetIncreaseValue(float unit)
{
  if (z_offset_value < HOME_Z_OFFSET_MAX_VALUE)
  {
    float diff = HOME_Z_OFFSET_MAX_VALUE - z_offset_value;

    unit = (diff > unit) ? unit : diff;
    z_offset_value -= unit;
    mustStoreCmd("M206 Z%.2f\n", z_offset_value);  
    mustStoreCmd("G1 Z%.2f\n", unit);              
  }

  return z_offset_value;
}


float homeOffsetUpdateValueByEncoder(float unit, int8_t direction)
{
  float overall_unit = (direction > 0) ? (direction * unit) : (-direction * unit);  

  if (direction < 0)  
    homeOffsetDecreaseValue(overall_unit);
  else
    homeOffsetIncreaseValue(overall_unit);

  return z_offset_value;
}
