#include "Move.h"
#include "includes.h"



#define X_MOVE_GCODE "G1 X%.2f F%d\n"
#define Y_MOVE_GCODE "G1 Y%.2f F%d\n"
#define Z_MOVE_GCODE "G1 Z%.2f F%d\n"
#define GANTRY_UPDATE_DELAY 500  

const char *const xyzMoveCmd[] = {X_MOVE_GCODE, Y_MOVE_GCODE, Z_MOVE_GCODE};
static uint8_t item_moveLen_index = 1;
AXIS nowAxis = X_AXIS;

void storeMoveCmd(AXIS xyz, int8_t direction)
{
  
  storeCmd(xyzMoveCmd[xyz], (infoSettings.invert_axis[xyz] ? -direction : direction) * moveLenSteps[item_moveLen_index],
           ((xyz != Z_AXIS) ? infoSettings.xy_speed[infoSettings.move_speed] : infoSettings.z_speed[infoSettings.move_speed]));
  
  nowAxis = xyz;
}

#define LOAD_XYZ_LABEL_INDEX(p0, dir0, p1, dir1, axis) do{ \
                                                            moveItems.items[p0].label.index = LABEL_##axis##_##dir0; \
                                                            moveItems.items[p1].label.index = LABEL_##axis##_##dir1; \
                                                         }while(0)

void menuMove(void)
{
  MENUITEMS moveItems = {
    
    LABEL_MOVE,
    
    {
      #ifdef ALTERNATIVE_MOVE_MENU
        {ICON_Z_DEC,                   LABEL_Z_DEC},
        {ICON_Y_INC,                   LABEL_Y_INC},
        {ICON_Z_INC,                   LABEL_Z_INC},
        {ICON_01_MM,                   LABEL_01_MM},
        {ICON_X_DEC,                   LABEL_X_DEC},
        {ICON_Y_DEC,                   LABEL_Y_DEC},
        {ICON_X_INC,                   LABEL_X_INC},
        {ICON_BACK,                    LABEL_BACK},
      #else
        {ICON_X_INC,                   LABEL_X_INC},
        {ICON_Y_INC,                   LABEL_Y_INC},
        {ICON_Z_INC,                   LABEL_Z_INC},
        {ICON_01_MM,                   LABEL_01_MM},
        {ICON_X_DEC,                   LABEL_X_DEC},
        {ICON_Y_DEC,                   LABEL_Y_DEC},
        {ICON_Z_DEC,                   LABEL_Z_DEC},
        {ICON_BACK,                    LABEL_BACK},
      #endif
    }
  };

  KEY_VALUES key_num;

  
  uint8_t table[TOTAL_AXIS][2] =
  #ifdef ALTERNATIVE_MOVE_MENU
    /*-------*-------*-------*---------*
     | Z-(0) | Y+(1) | Z+(2) | unit(3) |
     *-------*-------*-------*---------*
     | X-(4) | Y-(5) | X+(6) | back(7) |
     *-------*-------*-------*---------*/
    
    {{6, 4}, {1, 5}, {2, 0}}
  #else
    /*-------*-------*-------*---------*
     | X+(0) | Y+(1) | Z+(2) | unit(3) |
     *-------*-------*-------*---------*
     | X-(4) | Y-(5) | Z-(6) | back(7) |
     *-------*-------*-------*---------*/
    
    {{0, 4}, {1, 5}, {2, 6}}
  #endif
    ;
  if (infoSettings.invert_axis[X_AXIS] != 1)
    LOAD_XYZ_LABEL_INDEX(table[X_AXIS][0], INC, table[X_AXIS][1], DEC, X);  
  else
    LOAD_XYZ_LABEL_INDEX(table[X_AXIS][0], DEC, table[X_AXIS][1], INC, X);  

  if (infoSettings.invert_axis[Y_AXIS] != 1)
    LOAD_XYZ_LABEL_INDEX(table[Y_AXIS][0], INC, table[Y_AXIS][1], DEC, Y);
  else
    LOAD_XYZ_LABEL_INDEX(table[Y_AXIS][0], DEC, table[Y_AXIS][1], INC, Y);

  if (infoSettings.invert_axis[Z_AXIS] != 1)
    LOAD_XYZ_LABEL_INDEX(table[Z_AXIS][0], INC, table[Z_AXIS][1], DEC, Z);
  else
    LOAD_XYZ_LABEL_INDEX(table[Z_AXIS][0], DEC, table[Z_AXIS][1], INC, Z);

  menuDrawPage(&moveItems);
  mustStoreCmd("G91\n");
  mustStoreCmd("M114\n");
  drawXYZ();

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuMove)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      #ifdef ALTERNATIVE_MOVE_MENU
        case KEY_ICON_0: storeMoveCmd(Z_AXIS, -1); break;  
        case KEY_ICON_1: storeMoveCmd(Y_AXIS, 1); break;   
        case KEY_ICON_2: storeMoveCmd(Z_AXIS, 1); break;   

        case KEY_ICON_3:
          item_moveLen_index = (item_moveLen_index + 1) % ITEM_MOVE_LEN_NUM;
          moveItems.items[key_num] = itemMoveLen[item_moveLen_index];
          menuDrawItem(&moveItems.items[key_num], key_num);
          break;

        case KEY_ICON_4: storeMoveCmd(X_AXIS, -1); break;  
        case KEY_ICON_5: storeMoveCmd(Y_AXIS, -1); break;  
        case KEY_ICON_6: storeMoveCmd(X_AXIS, 1); break;   

        case KEY_ICON_7: ReturnToReadyMenu(); break;
      #else
        case KEY_ICON_0: storeMoveCmd(X_AXIS, 1); break;   
        case KEY_ICON_1: storeMoveCmd(Y_AXIS, 1); break;   
        case KEY_ICON_2: storeMoveCmd(Z_AXIS, 1); break;   

        case KEY_ICON_3:
          item_moveLen_index = (item_moveLen_index + 1) % ITEM_MOVE_LEN_NUM;
          moveItems.items[key_num] = itemMoveLen[item_moveLen_index];
          menuDrawItem(&moveItems.items[key_num], key_num);
          break;

        case KEY_ICON_4: storeMoveCmd(X_AXIS, -1); break;  
        case KEY_ICON_5: storeMoveCmd(Y_AXIS, -1); break;  
        case KEY_ICON_6: storeMoveCmd(Z_AXIS, -1); break;  

        case KEY_ICON_7: infoMenu.cur--; break;
      #endif
        default:
          #if LCD_ENCODER_SUPPORT
            if (encoderPosition)
            {
              storeMoveCmd(nowAxis, encoderPosition > 0 ? 1 : -1);
              encoderPosition = 0;
            }
          #endif
          break;
    }
    loopProcess();
    update_gantry();
  }
  mustStoreCmd("G90\n");
}

void update_gantry(void)
{
  if (nextScreenUpdate(GANTRY_UPDATE_DELAY))
  {
    coordinateQuery();
    drawXYZ();
  }
}

void drawXYZ(void)
{
  char tempstr[20];
  GUI_SetColor(INFOBOX_ICON_COLOR);

  sprintf(tempstr, "X:%.2f  ", coordinateGetAxisActual(X_AXIS));
  GUI_DispString(START_X + 1 * SPACE_X + 1 * ICON_WIDTH, (ICON_START_Y - BYTE_HEIGHT) / 2, (uint8_t *)tempstr);

  sprintf(tempstr, "Y:%.2f  ", coordinateGetAxisActual(Y_AXIS));
  GUI_DispString(START_X + 2 * SPACE_X + 2 * ICON_WIDTH, (ICON_START_Y - BYTE_HEIGHT) / 2, (uint8_t *)tempstr);

  sprintf(tempstr, "Z:%.2f  ", coordinateGetAxisActual(Z_AXIS));
  GUI_DispString(START_X + 3 * SPACE_X + 3 * ICON_WIDTH, (ICON_START_Y - BYTE_HEIGHT) / 2, (uint8_t *)tempstr);

  GUI_SetColor(infoSettings.font_color);
}
