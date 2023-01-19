#include "common.h"
#include "includes.h"


uint8_t currentTool = NOZZLE0;
uint8_t currentFan = 0;
uint8_t currentSpeedID = 0;
static uint32_t lastTime = 0;


const ITEM itemTool[MAX_HEATER_COUNT] =
{

  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_BED,                     LABEL_BED},
  {ICON_BACKGROUND,              LABEL_BACKGROUND},
};


const ITEM itemDegreeSteps[ITEM_DEGREE_NUM] =
{

  {ICON_1_DEGREE,                LABEL_1_DEGREE},
  {ICON_5_DEGREE,                LABEL_5_DEGREE},
  {ICON_10_DEGREE,               LABEL_10_DEGREE},
};


const uint8_t degreeSteps[ITEM_DEGREE_NUM] = {1, 5, 10};


const ITEM itemSpeed[ITEM_SPEED_NUM] =
{

  {ICON_SLOW_SPEED,              LABEL_SLOW},
  {ICON_NORMAL_SPEED,            LABEL_NORMAL},
  {ICON_FAST_SPEED,              LABEL_FAST},
};


const ITEM itemPercent[ITEM_PERCENT_STEPS_NUM] =
{

  {ICON_E_1_MM,                  LABEL_1_PERCENT},
  {ICON_E_5_MM,                  LABEL_5_PERCENT},
  {ICON_E_10_MM,                 LABEL_10_PERCENT},
};


const uint8_t percentSteps[ITEM_PERCENT_STEPS_NUM] = {1, 5, 10};


const ITEM itemMoveLen[ITEM_MOVE_LEN_NUM] =
{

  {ICON_001_MM,                  LABEL_001_MM},
  {ICON_01_MM,                   LABEL_01_MM},
  {ICON_1_MM,                    LABEL_1_MM},
  {ICON_10_MM,                   LABEL_10_MM},
  {ICON_100_MM,                  LABEL_100_MM},
};


const float moveLenSteps[ITEM_MOVE_LEN_NUM] = {0.01f, 0.1f, 1, 10, 100};


const ITEM itemExtLenSteps[ITEM_EXT_LEN_NUM] =
{

  {ICON_E_1_MM,                  LABEL_1_MM},
  {ICON_E_5_MM,                  LABEL_5_MM},
  {ICON_E_10_MM,                 LABEL_10_MM},
  {ICON_E_100_MM,                LABEL_100_MM},
  {ICON_E_200_MM,                LABEL_200_MM},
};


const float extlenSteps[ITEM_EXT_LEN_NUM] = {1.0f, 5.0f, 10.0f, 100.0f, 200.0f};


const LABEL itemToggle[ITEM_TOGGLE_NUM] =
{
  LABEL_OFF,
  LABEL_ON
};

const uint16_t iconToggle[ITEM_TOGGLE_NUM] =
{
  ICONCHAR_TOGGLE_OFF,
  ICONCHAR_TOGGLE_ON
};



bool nextScreenUpdate(uint32_t duration)
{
  uint32_t curTime = OS_GetTimeMs();
  if (curTime > (lastTime + duration))
  {
    lastTime = curTime;
    return true;
  }
  else
  {
    return false;
  }
}
