#include "includes.h"
#include "Temperature.h"

const char *const heaterID[MAX_HEATER_COUNT] = HEAT_SIGN_ID;
const char *const heatDisplayID[MAX_HEATER_COUNT] = HEAT_DISPLAY_ID;
const char *const heatCmd[MAX_HEATER_COUNT] = HEAT_CMD;
const char *const heatWaitCmd[MAX_HEATER_COUNT] = HEAT_WAIT_CMD;


HEATER heater = {{}, NOZZLE0}; 
static int16_t lastTarget[MAX_HEATER_COUNT] = {0};
static uint8_t heat_update_seconds = TEMPERATURE_QUERY_SLOW_SECONDS;
static bool heat_update_waiting = false;
static bool heat_send_waiting[MAX_HEATER_COUNT];
bool bPreheatLoopCheck_AchievedTemperature = FALSE;

uint32_t nextHeatCheckTime = 0;
#define AUTOREPORT_TIMEOUT (nextHeatCheckTime + 3000) 

static uint8_t fixHeaterIndex(uint8_t index)
{
  if (infoSettings.hotend_count == 1)
    index = (index < MAX_HOTEND_COUNT) ? NOZZLE0 : index;
  return index;
}


void heatSetTargetTemp(uint8_t index, int16_t temp)
{
  index = fixHeaterIndex(index);
  heater.T[index].target = NOBEYOND(0, temp, infoSettings.max_temp[index]);
  if (heater.T[index].target + 2 > heater.T[index].current)
  {
    heater.T[index].status = HEATING;
  }
  
  
  if (heater.T[index].target < heater.T[index].current + 2)
  {
    heater.T[index].status = COOLING;
  }
  
  
  if (inRange(heater.T[index].current, heater.T[index].target, 2) == true)
  {
    heater.T[index].status = SETTLED;
  }
}


void heatSyncTargetTemp(uint8_t index, int16_t temp)
{
  index = fixHeaterIndex(index);
  lastTarget[index] = heater.T[index].target = temp;
}


uint16_t heatGetTargetTemp(uint8_t index)
{
  index = fixHeaterIndex(index);
  return heater.T[index].target;
}


void heatSetCurrentTemp(uint8_t index, int16_t temp)
{
  index = fixHeaterIndex(index);
  heater.T[index].current = NOBEYOND(-99, temp, 999);

  if (infoMachineSettings.autoReportTemp)
    updateNextHeatCheckTime(); 
}


int16_t heatGetCurrentTemp(uint8_t index)
{
  index = fixHeaterIndex(index);
  return heater.T[index].current;
}


void heatCoolDown(void)
{
  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
  {
    heatSetTargetTemp(i, 0);
  }
}


bool heatGetIsWaiting(uint8_t index)
{
  return (heater.T[index].waiting != WAIT_NONE);
}


bool heatHasWaiting(void)
{
  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
  {
    if (heater.T[i].waiting != WAIT_NONE)
      return true;
  }
  return false;
}


void heatSetIsWaiting(uint8_t tool, HEATER_WAIT isWaiting)
{
  heater.T[tool].waiting = isWaiting;

  if (isWaiting != WAIT_NONE) 
  {
    heatSetUpdateSeconds(TEMPERATURE_QUERY_FAST_SECONDS);
  }
  else if (heatHasWaiting() == false)
  {
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
  }
}

void heatClearIsWaiting(void)
{
  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
  {
    heater.T[i].waiting = WAIT_NONE;
  }
  heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
}


void heatSetCurrentTool(uint8_t tool)
{
  if (tool >= infoSettings.ext_count)
    return;
  heater.toolIndex = tool;
}


uint8_t heatGetCurrentTool(void)
{
  return heater.toolIndex;
}


uint8_t heatGetCurrentHotend(void)
{
  return (infoSettings.hotend_count == 1) ? NOZZLE0 : heater.toolIndex;
}


bool heaterIsValid(uint8_t index)
{
  if (index >= infoSettings.hotend_count && index < MAX_HOTEND_COUNT)
    return false;
  if (!infoSettings.bed_en && index == BED)
    return false;
  if (!infoSettings.chamber_en && index == CHAMBER)
    return false;
  return true;
}


void heatSetUpdateSeconds(uint8_t seconds)
{
  if (heat_update_seconds == seconds)
    return;

  heat_update_seconds = seconds;
  if (infoMachineSettings.autoReportTemp && !heat_update_waiting)
  {
    heat_update_waiting = storeCmd("M155 ");
  }
}


uint8_t heatGetUpdateSeconds(void)
{
  return heat_update_seconds;
}


void heatSyncUpdateSeconds(uint8_t seconds)
{
  heat_update_seconds = seconds;
}


void heatSetUpdateWaiting(bool isWaiting)
{
  heat_update_waiting = isWaiting;
}


void heatSetSendWaiting(uint8_t index, bool isWaiting)
{
  heat_send_waiting[index] = isWaiting;
}


bool heatGetSendWaiting(uint8_t index)
{
  return heat_send_waiting[index];
}

void updateNextHeatCheckTime(void)
{
  nextHeatCheckTime = OS_GetTimeMs() + heat_update_seconds * 1000;
}

void loopCheckHeater(void)
{
  
  
  if (!infoMachineSettings.autoReportTemp)
  {
    do
    {
      
      if (heat_update_waiting == true)
      {
        updateNextHeatCheckTime();
        break;
      }
      if (OS_GetTimeMs() < nextHeatCheckTime)
        break;
      if (requestCommandInfoIsRunning()) 
        break;
      if (storeCmd("M105\n") == false)
        break;
      updateNextHeatCheckTime();
      heat_update_waiting = true;
    } while (0);
  }
  else 
  {
    if (OS_GetTimeMs() > AUTOREPORT_TIMEOUT && !heat_update_waiting)
    {
      heat_update_waiting = storeCmd("M155 ");
      if (heat_update_waiting)
        updateNextHeatCheckTime(); 
    }
  }

  
  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
  {
    if (heater.T[i].waiting == WAIT_NONE)
    {
      continue;
    }
    else if (heater.T[i].waiting == WAIT_HEATING)
    {
      if (heater.T[i].current + 2 <= heater.T[i].target)
        continue;
    }
    else if (heater.T[i].waiting == WAIT_COOLING_HEATING)
    {
      if (inRange(heater.T[i].current, heater.T[i].target, 2) != true)
        continue;
    }

    heater.T[i].waiting = WAIT_NONE;
    if (heatHasWaiting())
      continue;

    if (infoMenu.menu[infoMenu.cur] == menuHeat)
      break;
    heatSetUpdateSeconds(TEMPERATURE_QUERY_SLOW_SECONDS);
  }

  
  for (uint8_t i = 0; (i < MAX_HEATER_COUNT) && (!isPrinting()); i++)
  {
    if (heater.T[i].status == SETTLED)
    {
      continue;
    }
    if (inRange(heater.T[i].current, heater.T[i].target, 2) != true)
    {
      continue;
    }

    switch (heater.T[i].status)
    {
    case HEATING:
      BUZZER_PLAY(sound_heated);
      break;

    case COOLING:
      BUZZER_PLAY(sound_cooled);
      break;

    default:
      break;
    }
    heater.T[i].status = SETTLED;
    bPreheatLoopCheck_AchievedTemperature = TRUE;
  }

  for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++) 
  {
    if (lastTarget[i] != heater.T[i].target)
    {
      lastTarget[i] = heater.T[i].target;
      if (heat_send_waiting[i] != true)
      {
        heat_send_waiting[i] = storeCmd("%s ", heatCmd[i]);
      }
    }
  }
}

void wKillAndTightLoop(void) 
{
  mustStoreCmd("M112\n"); 
  for (;;)
    ; 
}


void wDisplayThermalErrorDialog(uint8_t *pucTitle)
{
  
  setDialogText(pucTitle, LABEL_FIRST_LAYER_DIALOG_V2_1, LABEL_BACKGROUND, LABEL_BACKGROUND); 
  
  showDialog(DIALOG_TYPE_INFO, NULL, NULL, NULL);
  loopProcess(); 
  wKillAndTightLoop();
  
}
