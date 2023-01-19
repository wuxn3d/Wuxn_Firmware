#include "ScreenShot.h"
#include "includes.h"

#if defined (SCREEN_SHOT_TO_SD)


typedef struct __attribute__((__packed__))
{
  uint16_t bfType;           
  uint32_t bfSize;           
  uint16_t bfReserved1;      
  uint16_t bfReserved2;      
  uint32_t bfoffBits;        
} BMP_FILEHEADER;


typedef struct __attribute__((__packed__))
{
  uint32_t biSize;           
  uint32_t biWidth;          
  uint32_t biHeight;         
  uint16_t biPlanes;         
  uint16_t biBitCount;       
  uint32_t biCompress;       
  uint32_t biSizeImage;      
  uint32_t biXPelsPerMeter;  
  uint32_t biYPelsPerMeter;  
  uint32_t biClrUsed;        
  uint32_t biClrImportant;   
} BMP_INFOHEADER;

#define ALIGNMENT_4BYTE(n) (((n + 3) >> 2) << 2)

bool screenShotBMP(char *bmp)
{
  BMP_FILEHEADER bmp_file = {
    .bfType = 0x4D42,  
    .bfSize = (ALIGNMENT_4BYTE(LCD_WIDTH)  * LCD_HEIGHT) * 3 + sizeof(BMP_FILEHEADER) + sizeof(BMP_INFOHEADER),
    .bfReserved1 = 0,
    .bfReserved2 = 0,
    .bfoffBits = sizeof(BMP_FILEHEADER) + sizeof(BMP_INFOHEADER),
  };

  BMP_INFOHEADER bmp_info = {
    .biSize =  sizeof(BMP_INFOHEADER),  
    .biWidth = ALIGNMENT_4BYTE(LCD_WIDTH),
    .biHeight = LCD_HEIGHT,
    .biPlanes = 1,
    .biBitCount = 24,  
    .biCompress = 0,
    .biSizeImage = (ALIGNMENT_4BYTE(LCD_WIDTH)  * LCD_HEIGHT) * 3,
    .biXPelsPerMeter = 0x1EC2,
    .biYPelsPerMeter = 0x1EC2,
    .biClrUsed = 0,
    .biClrImportant = 0,
  };

  FIL bmpFile;
  UINT mybw;

  if (f_open(&bmpFile, bmp, FA_CREATE_NEW | FA_WRITE) != FR_OK)
  {
    return false;  
  }

  f_write(&bmpFile, &bmp_file, sizeof(BMP_FILEHEADER), &mybw);
  f_write(&bmpFile, &bmp_info, sizeof(BMP_INFOHEADER), &mybw);

  for (uint16_t y = 0; y < LCD_HEIGHT; y++)
  {
    f_lseek(&bmpFile, bmp_file.bfoffBits + (LCD_HEIGHT - y - 1) * (bmp_info.biWidth * bmp_info.biBitCount / 8));
    for (uint16_t x = 0; x < LCD_WIDTH; x++)
    {
      uint32_t c = LCD_ReadPixel_24Bit(x, y);
      f_write(&bmpFile, (char *)&c, 3, &mybw);
    }
  }

  f_close(&bmpFile);
  return true;
}


void loopScreenShot(void)
{
  /*
  * For "LCD_ENCODER_SUPPORT" TFTLCD
  * Pressed touch screen triggered ScreenShot in Marlin mode
  * Pressed encoder button triggered ScreenShot in other menu
  *
  * For no encoder TFTLCD
  * Long pressed touch screen triggered ScreenShot
  */
  #if LCD_ENCODER_SUPPORT
    bool (*screenShotTriggered)(uint16_t ) = (infoMenu.menu[infoMenu.cur] == menuMarlinMode) ? LCD_ReadPen : encoder_ReadBtn;
    #define SCREEN_SHOT_TRIGGERED() screenShotTriggered(LCD_BUTTON_INTERVALS)
  #else
    #define SCREEN_SHOT_TRIGGERED() LCD_ReadPen(LCD_CHANGE_MODE_INTERVALS)
  #endif

  if (SCREEN_SHOT_TRIGGERED())
  {
    if (!mountSDCard())
      return;

    char screenShotFileName[FF_LFN_BUF] = "ScreenShot";

    if (!f_dir_exists(screenShotFileName))
    {
      f_mkdir(screenShotFileName);
    }
    strcat(screenShotFileName, "/");

    uint16_t i = strlen(screenShotFileName);
    switch (getMenuType())
    {
      case MENU_TYPE_ICON:
        strcat(screenShotFileName, (char *)labelGetAddress(&getCurMenuItems()->title));
        break;

      case MENU_TYPE_LISTVIEW:
        strcat(screenShotFileName, (char *)labelGetAddress(&getCurListItems()->title));
        break;

      default:
        strcat(screenShotFileName, "other");
        break;
    }

    for (uint16_t j = strlen(screenShotFileName); j >= i; j--)
    {
      if (screenShotFileName[j] == '/')
      {
        screenShotFileName[j] = '_';
      }
    }

    uint8_t index = 0;
    char fileName[FF_LFN_BUF];
    do {
      sprintf(fileName, "%s_%d.bmp", screenShotFileName, index);
      index++;
    } while (!screenShotBMP(fileName) && index < 10);

    strcat(fileName, (index < 10) ? " ok!" : " failed!");

    addToast(DIALOG_TYPE_ALERT, fileName);
  }
}

#endif
