#include "Babystep.h"
#include "includes.h"

#define FIRST_LAYER_PATTERN_GCODE_COUNT          86 
#define FIRST_LAYER_PATTERN_GCODES_MAX_LENGTH    35 
#define FIRST_LAYER_ICON_X                       80
#define FIRST_LAYER_ICON_Y                       80

static uint8_t ucFirstLayerPatternGCodeIndex = 0;
extern bool bBabyStepStartDialogFlag;
bool bFirstLayerPatternFlag = FALSE;
extern bool bCalledByFirstLayerFlag;  
float now_z_offset, z_offset, orig_z_offset, new_z_offset; 
float (* offsetSetValue)(float);  

static void wBabyStepStartupDialog(void);
static void wDisplayFirstLayerDiagramDialog(void);
static void wDisplayDialogForAdjustNozzleHeight(void);
static void wSentGcodeForFirstLayerPattern(void);
static void wBabyStepOutro(void);
static void wStartSendFirstLayerPattern(void);


static void wBabyStepStartupDialog(void)
{
  bBabyStepStartDialogFlag = FALSE;

  setDialogText(LABEL_FIRST_LAYER_OFFSET, LABEL_FIRST_LAYER_DIALOG_V6, LABEL_NEXT, LABEL_BACKGROUND);
  showDialog(DIALOG_TYPE_INFO, wDisplayFirstLayerDiagramDialog, NULL, NULL);
}


static void wDisplayFirstLayerDiagramDialog(void)
{
  setDialogText(LABEL_FIRST_LAYER_OFFSET, LABEL_BACKGROUND, LABEL_NEXT, LABEL_BACKGROUND);
  showDialog(DIALOG_TYPE_INFO, wDisplayDialogForAdjustNozzleHeight, NULL, NULL);
  loopProcess();
  ICON_ReadDisplay(FIRST_LAYER_ICON_X, FIRST_LAYER_ICON_Y, ICON_FIRST_LAYER_DIAGRAM);
}


static void wDisplayDialogForAdjustNozzleHeight(void)
{
  setDialogText(LABEL_FIRST_LAYER_OFFSET, LABEL_FIRST_LAYER_DIALOG_V8, LABEL_START, LABEL_CANCEL);
  showDialog(DIALOG_TYPE_INFO, wStartSendFirstLayerPattern, ReturnToWizardsMenu, NULL);
}


static void wStartSendFirstLayerPattern(void)
{
  bFirstLayerPatternFlag = TRUE;
  infoMenu.menu[++infoMenu.cur] = menuBabystep;
}


static void wSentGcodeForFirstLayerPattern(void)
{
  const char pucFirstLayerPatternGcode[FIRST_LAYER_PATTERN_GCODE_COUNT][FIRST_LAYER_PATTERN_GCODES_MAX_LENGTH] = {
                                              "M107\n", 
                                              "M82\n", 
                                              "G92 E0\n",  
                                              "G1 F700 E-1\n", 
       
       
                                              "M204 S1000\n" 
                                              "G28\n",
                                              "G0 F480 Z5\n",
                                              "G0 F6000 X0 Y0\n",
      
                                              "G34 I20 T0.05\n",
                                              "G29\n",
       
                                              "M121\n",
                                              "G0 F6000 X0 Y-20\n",
                                              "G0 F480 Z1\n",
                                              "G92 E0\n",
                                              "G1 F450 X100 E25\n",
                                              "G92 E0\n",
                                              "G1 F500 E-1\n",
                                              "G0 F480 Z0.3\n",
                                              "M120\n",
                                              "M220 S100\n",
                                              "G92 E0\n",
                                              "G1 F1500 E-0.8\n",
                                              "M107\n",
                                              "M204 S1250\n",
                                              "M205 X10 Y10\n",
                                              "G1 F600 Z0.65\n",
                                              
                                              "G0 F12000 X41.586 Y30.386 Z0.65\n",
     
                                              "M204 S1000\n",
                                              
                                              "G1 F600 Z0.25\n",
                                              "G1 F2100 E0.02079\n",
                                              "G1 F1080 X41.614 Y30.386 E0.02209\n",
                                              "G1 X41.614 Y179.266 E6.96076\n",
                                              "G1 X58.508 Y179.468 E7.75706\n",
                                              "G1 X58.586 Y179.249 E7.7679\n",
                                              "G1 X58.586 Y30.386 E14.70577\n",
                                              "G1 X74.814 Y30.386 E15.46209\n",
                                              "G1 X74.814 Y179.266 E22.40076\n",
                                              "G1 X74.932 Y179.508 E22.4133\n",
                                              "G1 X75.151 Y179.586 E22.42414\n",
                                              "G1 X91.466 Y179.586 E23.18451\n",
                                              "G1 X91.708 Y179.468 E23.19706\n",
                                              "G1 X91.786 Y179.249 E23.20789\n",
                                              "G1 X91.786 Y30.386 E30.14577\n",
                                              "G1 X108.014 Y30.386 E30.90209\n",
                                              "G1 X108.014 Y179.266 E37.84075\n",
                                              "G1 X108.132 Y179.508 E37.8533\n",
                                              "G1 X108.351 Y179.586 E37.86413\n",
                                              "G1 X124.666 Y179.586 E38.62451\n",
                                              "G1 X124.908 Y179.468 E38.63706\n",
                                              "G1 X124.986 Y179.249 E38.64789\n",
                                              "G1 X124.986 Y30.386 E45.58576\n",
                                              "G1 X141.214 Y30.386 E46.34208\n",
                                              "G1 X141.214 Y179.266 E53.28075\n",
                                              "G1 X141.332 Y179.508 E53.2933\n",
                                              "G1 X141.551 Y179.586 E53.30413\n",
                                              "G1 X157.866 Y179.586 E54.0645\n",
                                              "G1 X158.108 Y179.468 E54.07705\n",
                                              "G1 X158.186 Y179.249 E54.08789\n",
                                              "G1 X158.186 Y30.386 E61.02576\n",
                                              "G1 X174.414 Y30.386 E61.78208\n",
                                              "G1 X174.414 Y179.266 E68.72075\n",
                                              "G1 X174.532 Y179.508 E68.73329\n",
                                              "G1 X174.751 Y179.586 E68.74413\n",
                                              "G1 X191.066 Y179.586 E69.5045\n",
                                              "G1 X191.308 Y179.468 E69.51705\n",
                                              "G1 X191.386 Y179.249 E69.52788\n",
                                              "G1 X191.386 Y30.386 E76.46576\n",
                                              "G1 X208.414 Y30.386 E77.25936\n",
                                              "G1 X208.414 Y179.614 E84.21425\n",
                                              "G1 X208.386 Y179.614 E84.21555\n",
                                              "G1 X208.386 Y30.734 E91.15422\n",
                                              "G1 X208.268 Y30.492 E91.16677\n",
                                              "G1 X208.049 Y30.414 E91.1776\n",
                                              
                                              "M140 S0\n", 
                                              "M107\n", 
                                              "G91\n",
                                              "G0 F480 Z10\n",
                                              "G90\n",
                                              "G0 F6000 X0 Y210\n",
                                              "M107\n", 
                                              "M104 S0\n", 
                                              "M140 S0\n", 
                                              "M84 X Y E\n",
                                              "M82\n", 
                                              "M104 S0\n",  
                                              "M420 S0\n", 
                                              };

  
  if (ucFirstLayerPatternGCodeIndex >= FIRST_LAYER_PATTERN_GCODE_COUNT)
  {
    if(infoCmd.count == 0)  
    {
      ucFirstLayerPatternGCodeIndex = 0;
      bFirstLayerPatternFlag = FALSE;
      setDialogText(LABEL_FIRST_LAYER_OFFSET, LABEL_FIRST_LAYER_DIALOG_V9, LABEL_YES, LABEL_REPEAT);
      showDialog(DIALOG_TYPE_INFO, wBabyStepOutro, wPingIRSensorFromFirstLayerOffset, NULL);
    }
  }
  else
  {
    mustStoreCmd(pucFirstLayerPatternGcode[ucFirstLayerPatternGCodeIndex]);
    ucFirstLayerPatternGCodeIndex++;
  }
}


static void wBabyStepOutro(void)
{
  if (infoMachineSettings.EEPROM == 1)
  {
   orig_z_offset = offsetSetValue(new_z_offset);  
   saveEepromSettings();
  }
  bCalledByFirstLayerFlag = FALSE;  
  ReturnToWizardsMenu();
}


static uint8_t moveLenSteps_index = 0;

void babyReDraw(float babystep, float z_offset, bool force_z_offset, bool skip_header)
{
  if (!skip_header)
  {
    
    GUI_DispString(exhibitRect.x0, exhibitRect.y0, LABEL_ADJUSTMENT);  

    if (infoMachineSettings.zProbe == ENABLED)
      
      GUI_DispString(exhibitRect.x0, exhibitRect.y0 + BYTE_HEIGHT + LARGE_BYTE_HEIGHT, LABEL_BABYSTEP);  
    else if (infoMachineSettings.leveling == BL_MBL)
      GUI_DispString(exhibitRect.x0, exhibitRect.y0 + BYTE_HEIGHT + LARGE_BYTE_HEIGHT, LABEL_MBL);
    else
      GUI_DispString(exhibitRect.x0, exhibitRect.y0 + BYTE_HEIGHT + LARGE_BYTE_HEIGHT, LABEL_HOME_OFFSET);
  }

  char tempstr[20];

  GUI_POINT point_bs = {exhibitRect.x1, exhibitRect.y0 + BYTE_HEIGHT};
  GUI_POINT point_of = {exhibitRect.x1, exhibitRect.y0 + BYTE_HEIGHT * 2 + LARGE_BYTE_HEIGHT};

  setLargeFont(true);

  sprintf(tempstr, "% 6.2f", babystep);
  GUI_DispStringRight(point_bs.x, point_bs.y, (uint8_t*) tempstr);

  sprintf(tempstr, "% 6.2f", z_offset);

  if (force_z_offset)
    GUI_SetColor(infoSettings.reminder_color);
  else
    GUI_SetColor(infoSettings.font_color);

  GUI_DispStringRight(point_of.x, point_of.y, (uint8_t*) tempstr);

  
  GUI_SetColor(infoSettings.font_color);

  setLargeFont(false);
}


float babyMblOffsetSetValue(float value)
{
  mustStoreCmd("G29 S4 Z%.2f\n", value);
  mustStoreCmd("M420 V1 T1\n");  
  return value;
}


float babyMblOffsetGetValue(void)
{
  return getParameter(P_MBL_OFFSET, 0);
}

void menuBabystep(void)
{
  
  MENUITEMS babyStepItems = {
    
    LABEL_BABYSTEP,
    
    {
      {ICON_DEC,                     LABEL_DEC},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_INC,                     LABEL_INC},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
      {ICON_001_MM,                  LABEL_001_MM},
      {ICON_RESET_VALUE,             LABEL_RESET},
      {ICON_BACKGROUND,              LABEL_BACKGROUND},
    }
  };

  #ifdef FRIENDLY_Z_OFFSET_LANGUAGE
    babyStepItems.items[0].icon = ICON_NOZZLE_DOWN;
    babyStepItems.items[0].label.index = LABEL_DOWN;
    babyStepItems.items[3].icon = ICON_NOZZLE_UP;
    babyStepItems.items[3].label.index = LABEL_UP;
  #endif

  KEY_VALUES key_num = KEY_IDLE;
  float now_babystep, babystep, orig_babystep;
  
  float unit;
  bool force_z_offset;
  float (* offsetGetValue)(void);   
  

  if (infoMachineSettings.zProbe == ENABLED)        
  {
    offsetGetValue = probeOffsetGetValue;
    offsetSetValue = probeOffsetSetValue;
  }
  else if (infoMachineSettings.leveling == BL_MBL)  
  {
    offsetGetValue = babyMblOffsetGetValue;
    offsetSetValue = babyMblOffsetSetValue;
  }
  else                                              
  {
    offsetGetValue = homeOffsetGetValue;
    offsetSetValue = homeOffsetSetValue;
  }

  now_babystep = babystep = orig_babystep = babystepGetValue();
  now_z_offset = z_offset = orig_z_offset = new_z_offset = offsetGetValue();
  force_z_offset = false;

  if (infoMachineSettings.EEPROM == 1)
    {
      babyStepItems.items[KEY_ICON_4].icon = ICON_EEPROM_SAVE;
      babyStepItems.items[KEY_ICON_4].label.index = LABEL_SAVE;
    }
  babyStepItems.items[KEY_ICON_5] = itemMoveLen[moveLenSteps_index];

  
  
  if (bCalledByFirstLayerFlag == TRUE)
  {
    babyStepItems.items[KEY_ICON_7].icon = ICON_STOP;
    babyStepItems.items[KEY_ICON_7].label.index = LABEL_CANCEL;
  }
  else
  {
    babyStepItems.items[KEY_ICON_7].icon = ICON_BACK;
    babyStepItems.items[KEY_ICON_7].label.index = LABEL_BACK;
  }

  menuDrawPage(&babyStepItems);
  babyReDraw(now_babystep, now_z_offset, force_z_offset, false);

  if(bBabyStepStartDialogFlag == TRUE)
  {
    wBabyStepStartupDialog();
  }

  #if LCD_ENCODER_SUPPORT
    encoderPosition = 0;
  #endif

  while (infoMenu.menu[infoMenu.cur] == menuBabystep)
  {
    if(bFirstLayerPatternFlag == TRUE && infoCmd.count < CMD_MAX_LIST - 13)
    {
      wSentGcodeForFirstLayerPattern();
    }

    unit = moveLenSteps[moveLenSteps_index];

    babystep = babystepGetValue();  

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      
      case KEY_ICON_0:
        babystep = babystepDecreaseValue(unit);
        break;

      
      case KEY_ICON_3:
        babystep = babystepIncreaseValue(unit);
	      break;

        
        case KEY_ICON_4:
        if (infoMachineSettings.EEPROM == 1)
        {
          orig_z_offset = offsetSetValue(new_z_offset);  

          setDialogText(babyStepItems.title.index, LABEL_EEPROM_SAVE_INFO, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_QUESTION, saveEepromSettings, NULL, NULL);
        }
        break;


      
      case KEY_ICON_5:
        moveLenSteps_index = (moveLenSteps_index + 1) % ITEM_FINE_MOVE_LEN_NUM;

        babyStepItems.items[key_num] = itemMoveLen[moveLenSteps_index];

        menuDrawItem(&babyStepItems.items[key_num], key_num);
        break;

      
      case KEY_ICON_6:
        orig_babystep = babystepResetValue();

        if (infoMachineSettings.zProbe == ENABLED || infoMachineSettings.leveling == BL_MBL)
          orig_z_offset = offsetSetValue(new_z_offset - babystep);  
        else  
          orig_z_offset = offsetSetValue(new_z_offset + babystep);  
        break;

      case KEY_ICON_7:
        
        if(bCalledByFirstLayerFlag == TRUE)
        {
          clearCmdQueue();  
          ucFirstLayerPatternGCodeIndex = 0;  
          bCalledByFirstLayerFlag = FALSE;
          bFirstLayerPatternFlag = FALSE;  
          TurnOffHeatersAndFans();
          if (infoSettings.send_cancel_gcode == 1)  
            sendPrintCodes(2);

          ReturnToReadyMenu();
        }
        else
        {
          infoMenu.cur--;
        }
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            babystep = babystepUpdateValueByEncoder(unit, encoderPosition > 0 ? 1 : -1);

            encoderPosition = 0;
          }
        #endif
        break;
    }

    z_offset = offsetGetValue();  

    if (now_babystep != babystep || now_z_offset != z_offset)
    {
      if (now_z_offset != z_offset || (orig_babystep - 0.005f <= babystep && babystep <= orig_babystep + 0.005f))
      {
        
        
        
        new_z_offset = now_z_offset = z_offset;

        force_z_offset = false;
      }
      else if (orig_z_offset - 0.005f <= z_offset && z_offset <= orig_z_offset + 0.005f)
      {
        
        
        if (infoMachineSettings.zProbe == ENABLED || infoMachineSettings.leveling == BL_MBL)
          new_z_offset = z_offset + babystep - orig_babystep;
        else  
          new_z_offset = z_offset - (babystep - orig_babystep);

        force_z_offset = true;
      }

      now_babystep = babystep;
      babyReDraw(now_babystep, new_z_offset, force_z_offset, true);
    }

    loopProcess();
  }

  offsetSetValue(new_z_offset);  
}
