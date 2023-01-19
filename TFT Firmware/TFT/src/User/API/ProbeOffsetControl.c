#include "ProbeOffsetControl.h"
#include "includes.h"

static float z_offset_value = PROBE_Z_OFFSET_DEFAULT_VALUE;
static bool probe_offset_enabled = false;


void probeOffsetEnable(bool skipZOffset)
{
  probe_offset_enabled = true;

  probeHeightEnable();  

  
  mustStoreCmd("G28\n");                          
  probeHeightStop(infoSettings.z_raise_probing);  

  if (infoSettings.xy_offset_probing)  
  {
    probeHeightRelative();                                  
    mustStoreCmd("G1 X%.2f Y%.2f\n",
                 getParameter(P_PROBE_OFFSET, X_STEPPER),
                 getParameter(P_PROBE_OFFSET, Y_STEPPER));  
  }

  if (skipZOffset)
  {
    probeHeightStart(-probeOffsetGetValue(), false);  
    probeOffsetSetValue(0.0f);                        
  }
  else
  {
    probeHeightStart(0.0f, false);  
  }

  probeHeightRelative();  
}


void probeOffsetDisable(void)
{
  probe_offset_enabled = false;

  
  mustStoreCmd("G28\n");                          
  probeHeightStop(infoSettings.z_raise_probing);  
  probeHeightAbsolute();                          

  probeHeightDisable();  
}


bool probeOffsetGetStatus(void)
{
  return probe_offset_enabled;
}


float probeOffsetSetValue(float value)
{
  mustStoreCmd("M851 Z%.2f\n", value);
  mustStoreCmd("M851\n");  
  z_offset_value = value;

  return z_offset_value;
}


float probeOffsetGetValue(void)
{
  z_offset_value = getParameter(P_PROBE_OFFSET, Z_STEPPER);

  return z_offset_value;
}


float probeOffsetResetValue(void)
{
  if (z_offset_value == PROBE_Z_OFFSET_DEFAULT_VALUE)  
    return z_offset_value;

  float unit = z_offset_value - PROBE_Z_OFFSET_DEFAULT_VALUE;

  z_offset_value = PROBE_Z_OFFSET_DEFAULT_VALUE;
  mustStoreCmd("M851 Z%.2f\n", z_offset_value);  
  mustStoreCmd("G1 Z%.2f\n", -unit);             

  return z_offset_value;
}


float probeOffsetDecreaseValue(float unit)
{
  if (z_offset_value > PROBE_Z_OFFSET_MIN_VALUE)
  {
    float diff = z_offset_value - PROBE_Z_OFFSET_MIN_VALUE;

    unit = (diff > unit) ? unit : diff;
    z_offset_value -= unit;
    mustStoreCmd("M851 Z%.2f\n", z_offset_value);  
    mustStoreCmd("G1 Z%.2f\n", -unit);             
  }

  return z_offset_value;
}


float probeOffsetIncreaseValue(float unit)
{
  if (z_offset_value < PROBE_Z_OFFSET_MAX_VALUE)
  {
    float diff = PROBE_Z_OFFSET_MAX_VALUE - z_offset_value;

    unit = (diff > unit) ? unit : diff;
    z_offset_value += unit;
    mustStoreCmd("M851 Z%.2f\n", z_offset_value);  
    mustStoreCmd("G1 Z%.2f\n", unit);              
  }

  return z_offset_value;
}


float probeOffsetUpdateValueByEncoder(float unit, int8_t direction)
{
  float overall_unit = (direction > 0) ? (direction * unit) : (-direction * unit);  

  if (direction < 0)  
    probeOffsetDecreaseValue(overall_unit);
  else
    probeOffsetIncreaseValue(overall_unit);

  return z_offset_value;
}
