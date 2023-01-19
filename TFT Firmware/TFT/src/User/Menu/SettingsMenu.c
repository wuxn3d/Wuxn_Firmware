#include "SettingsMenu.h"
#include "includes.h"

const MENUITEMS settingsItems = {
  
  LABEL_SETTINGS,
  
  {
    {ICON_SCREEN_SETTINGS,         LABEL_SCREEN_SETTINGS},

    
    {ICON_GCODE,                   LABEL_TERMINAL},  

    
    {ICON_SCREEN_INFO,             LABEL_SCREEN_INFO},

    
    {ICON_BACKGROUND,              LABEL_BACKGROUND},  

    
    {ICON_BACKGROUND,              LABEL_BACKGROUND},  

    
    {ICON_BACKGROUND,              LABEL_BACKGROUND},  

    {ICON_BACKGROUND,              LABEL_BACKGROUND},
    {ICON_BACK,                    LABEL_BACK},
  }
};









static uint8_t firmare_name[64] = "Unknow system";  
uint8_t machine_type[64] = "3D Printer";  
uint8_t access_point[64] = "Connecting...";  
uint8_t ip_address[20] = "0.0.0.0";  

void infoSetFirmwareName(uint8_t *name, uint8_t name_len)
{
  if (name_len > sizeof(firmare_name) - 1)
    name_len = sizeof(firmare_name) - 1;
  uint8_t i;
  for (i = 0; i < name_len; i++)
  {
    firmare_name[i] = name[i];
  }
  firmare_name[i] = 0;
}

void infoSetMachineType(uint8_t *machine, uint8_t type_len)
{
  if (type_len > sizeof(machine_type) - 1)
    type_len = sizeof(machine_type) - 1;
  uint8_t i;
  for (i = 0; i < type_len; i++)
  {
    machine_type[i] = machine[i];
  }
  machine_type[i] = 0;
  statusScreen_setReady();
}

void infoSetAccessPoint(uint8_t *ssid, uint8_t ssid_len)
{
  if (ssid_len > sizeof(access_point) - 1)
    ssid_len = sizeof(access_point) - 1;
  uint8_t i;
  for (i = 0; i < ssid_len; i++)
  {
    access_point[i] = ssid[i];
  }
  access_point[i] = 0;
}

void infoSetIPAddress(uint8_t *ip, uint8_t ip_len)
{
  if (ip_len > sizeof(ip_address) - 1)
    ip_len = sizeof(ip_address) - 1;
  uint8_t i;
  for (i = 0; i < ip_len; i++)
  {
    ip_address[i] = ip[i];
  }
  ip_address[i] = 0;
}


void menuInfo(void)
{
  char buf[128];

  const char *const hardware = "BIGTREETECH_" HARDWARE_VERSION;
  const char *const firmware = FIRMWARE_VERSION"_" STRINGIFY(SOFTWARE_VERSION);

  GUI_Clear(infoSettings.bg_color);
  GUI_SetColor(GRAY);

  
  

  
  

  
  

  
  

  
  

  
  

  

  
  float usedMB = (float)FLASH_USED/1048576;
  sprintf(buf, "Used %.2f%% (%.2fMB/%uMB)", flashUsedPercentage(), usedMB, (W25Qxx_ReadCapacity()/1048576));

  const uint16_t top_y = 0; 
  const uint16_t start_x = strlen("Firmware:") * BYTE_WIDTH;
  const GUI_RECT version[7] = {
    {start_x, top_y + 0*BYTE_HEIGHT, LCD_WIDTH, top_y + 2*BYTE_HEIGHT},
    {start_x, top_y + 2*BYTE_HEIGHT, LCD_WIDTH, top_y + 4*BYTE_HEIGHT},
    {start_x, top_y + 4*BYTE_HEIGHT, LCD_WIDTH, top_y + 5*BYTE_HEIGHT},
    {start_x, top_y + 5*BYTE_HEIGHT, LCD_WIDTH, top_y + 6*BYTE_HEIGHT},
    {start_x, top_y + 6*BYTE_HEIGHT, LCD_WIDTH, top_y + 7*BYTE_HEIGHT},
    {start_x, top_y + 7*BYTE_HEIGHT, LCD_WIDTH, top_y + 8*BYTE_HEIGHT},
    {start_x, top_y + 8*BYTE_HEIGHT, LCD_WIDTH, top_y + 9*BYTE_HEIGHT},
    };
  
  GUI_DispString(0, version[0].y0, (uint8_t *)"System  :");
  GUI_DispString(0, version[1].y0, (uint8_t *)"Machine :");
  GUI_DispString(0, version[2].y0, (uint8_t *)"Board   :");
  GUI_DispString(0, version[3].y0, (uint8_t *)"Firmware:");
  GUI_DispString(0, version[4].y0, (uint8_t *)"SPIFlash:");
  if (infoMachineSettings.firmwareType == FW_REPRAPFW)
  {
    GUI_DispString(0, version[5].y0, (uint8_t *)"WIFI    :");
    GUI_DispString(0, version[6].y0, (uint8_t *)"IP      :");
  }

  
  GUI_SetColor(0xDB40);
  GUI_DispStringInPrectEOL(&version[0], firmare_name);
  GUI_DispStringInPrectEOL(&version[1], machine_type);
  GUI_DispStringInPrectEOL(&version[2], (uint8_t *)hardware);
  GUI_DispStringInPrectEOL(&version[3], (uint8_t *)firmware);
  GUI_DispStringInPrectEOL(&version[4], (uint8_t *)buf);
  if (infoMachineSettings.firmwareType == FW_REPRAPFW)
  {
    GUI_DispStringInPrectEOL(&version[5], (uint8_t *)access_point);
    GUI_DispStringInPrectEOL(&version[6], (uint8_t *)ip_address);
  }

  GUI_SetColor(GRAY);

  GUI_HLine(0, LCD_HEIGHT - (BYTE_HEIGHT*2), LCD_WIDTH);

  GUI_DispStringInRect(20, LCD_HEIGHT - (BYTE_HEIGHT*2), LCD_WIDTH-20, LCD_HEIGHT, textSelect(LABEL_TOUCH_TO_EXIT));

  while (!isPress()) loopBackEnd();
  while (isPress()) loopBackEnd();

  GUI_RestoreColorDefault();
  infoMenu.cur--;
}

void menuSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&settingsItems);

  while (infoMenu.menu[infoMenu.cur] == menuSettings)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuScreenSettings;
        break;

      case KEY_ICON_1:
        
        
        infoMenu.menu[++infoMenu.cur] = menuTerminal;  
        break;

      case KEY_ICON_2:
        
        infoMenu.menu[++infoMenu.cur] = menuInfo;  
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
