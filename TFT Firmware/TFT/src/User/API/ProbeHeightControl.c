#include "ProbeHeightControl.h"
#include "includes.h"

#define PROBE_UPDATE_DELAY 200  

static uint32_t nextQueryTime = 0;
static bool curSoftwareEndstops = true;



void probeHeightEnable(void)
{
  curSoftwareEndstops = infoMachineSettings.softwareEndstops;

  if (curSoftwareEndstops)  
  {
    if (infoMachineSettings.firmwareType == FW_REPRAPFW)
      mustStoreCmd("M564 S0 H0\n");
    else
      mustStoreCmd("M211 S0\n");  
  }
}



void probeHeightDisable(void)
{
  if (curSoftwareEndstops)  
  {
    if (infoMachineSettings.firmwareType == FW_REPRAPFW)
      mustStoreCmd("M564 S1 H1\n");
    else
      mustStoreCmd("M211 S1\n");  
  }
}


void probeHeightStart(float initialHeight, bool relativeHeight)
{
  if (relativeHeight)
    probeHeightRelative();                                
  else
    probeHeightAbsolute();                                

  mustStoreCmd("G1 Z%.2f F%d\n",
               initialHeight,
               infoSettings.level_feedrate[FEEDRATE_Z]);  
  probeHeightRelative();                                  
}


void probeHeightStop(float raisedHeight)
{
  probeHeightRelative();                                  
  mustStoreCmd("G1 Z%.2f F%d\n",
               raisedHeight,
               infoSettings.level_feedrate[FEEDRATE_Z]);  
  probeHeightAbsolute();                                  
}


void probeHeightRelative(void)
{
  mustStoreCmd("G91\n");  
}


void probeHeightAbsolute(void)
{
  mustStoreCmd("G90\n");  
}


void probeHeightMove(float unit, int8_t direction)
{
  
  storeCmd("G1 Z%.2f F%d\n", (infoSettings.invert_axis[Z_AXIS] ? -direction : direction) * unit,
           infoSettings.level_feedrate[FEEDRATE_Z]);
}


void probeHeightQueryCoord(void)
{
  if (OS_GetTimeMs() > nextQueryTime)
  {
    coordinateQuery();
    nextQueryTime = OS_GetTimeMs() + PROBE_UPDATE_DELAY;
  }
}
