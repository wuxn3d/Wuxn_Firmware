#include "Leveling.h"
#include "includes.h"

const MENUITEMS manualLevelingItems = {
  
  LABEL_LEVELING,
  
  {
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_DISABLE_STEPPERS,        LABEL_XY_UNLOCK},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACK,                    LABEL_BACK},
  }
};

void moveToLevelingPoint(uint8_t point)
{
  int16_t x_left_point = infoSettings.machine_size_min[X_AXIS] + infoSettings.level_edge,
          x_right_point = infoSettings.machine_size_max[X_AXIS] - infoSettings.level_edge,
          y_lower_point = infoSettings.machine_size_min[Y_AXIS] + infoSettings.level_edge,
          y_upper_point = infoSettings.machine_size_max[Y_AXIS] - infoSettings.level_edge;

  if (infoSettings.invert_axis[X_AXIS])
  {
    int16_t temp = x_left_point;  
    x_left_point = x_right_point;
    x_right_point = temp;
  }
  if (infoSettings.invert_axis[Y_AXIS])
  {
    int16_t temp = y_lower_point;  
    y_lower_point = y_upper_point;
    y_upper_point = temp;
  }

  int16_t pointPosition[5][2] = {
    
    {x_left_point,  y_lower_point},
    
    {x_right_point, y_lower_point},
    
    {x_right_point, y_upper_point},
    
    {x_left_point,  y_upper_point},
    
    {(x_left_point + x_right_point) / 2, (y_lower_point + y_upper_point) / 2},
  };

  if (coordinateIsKnown() == false)
  {
    storeCmd("G28\n");
  }

  storeCmd("G0 Z%.3f F%d\n", infoSettings.level_z_raise, infoSettings.level_feedrate[FEEDRATE_Z]);
  storeCmd("G0 X%d Y%d F%d\n", pointPosition[point][0], pointPosition[point][1], infoSettings.level_feedrate[FEEDRATE_XY]);
  storeCmd("G0 Z%.3f F%d\n", infoSettings.level_z_pos, infoSettings.level_feedrate[FEEDRATE_Z]);
}

void menuManualLeveling(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&manualLevelingItems);

  while (infoMenu.menu[infoMenu.cur] == menuManualLeveling)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        moveToLevelingPoint(3);
        break;

      case KEY_ICON_1:
        moveToLevelingPoint(2);
        break;

      case KEY_ICON_2:
        {
          char tempstr[30];
          sprintf(tempstr, "Min:%d | Max:%d", LEVELING_EDGE_DISTANCE_MIN, LEVELING_EDGE_DISTANCE_MAX);

          int val = numPadInt((uint8_t *)tempstr, infoSettings.level_edge, LEVELING_EDGE_DISTANCE_DEFAULT, false);
          infoSettings.level_edge = NOBEYOND(LEVELING_EDGE_DISTANCE_MIN, val, LEVELING_EDGE_DISTANCE_MAX);

          menuDrawPage(&manualLevelingItems);
        }
        break;

      case KEY_ICON_3:
        storeCmd("M84 X Y E\n");
        break;

      case KEY_ICON_4:
        moveToLevelingPoint(0);
        break;

      case KEY_ICON_5:
        moveToLevelingPoint(1);
        break;

      case KEY_ICON_6:
        moveToLevelingPoint(4);
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();
  }
}
