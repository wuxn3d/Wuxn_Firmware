#ifndef _BOOT_H_
#define _BOOT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "variants.h"
#include "stdbool.h"
#include "config.h"

#define W25QXX_SECTOR_SIZE (0x1000) 

#ifndef LOGO_MAX_SIZE
  #define LOGO_MAX_SIZE            0x4B000
  #define WORD_UNICODE_SIZE       0x480000
  #define BYTE_ASCII_SIZE           0x1000
  #define LARGE_FONT_SIZE           0x3000
  #define FLASH_SIGN_SIZE           0x1000    
  #ifdef WUXN_CODE
    #define LANGUAGE_SIZE            0x1A000    
                                                
                                                
  #else
    #define LANGUAGE_SIZE            0x14000    
  #endif
  #define STRINGS_STORE_MAX_SIZE    0x5000 
  #define PRINT_GCODES_MAX_SIZE     0x5000    
  #define CUSTOM_GCODE_MAX_SIZE     0x5000    
  #define ICON_MAX_SIZE             0x5000
  #define INFOBOX_MAX_SIZE          0xB000
  #define SMALL_ICON_MAX_SIZE       0x2000
#endif


#define LOGO_ADDR               0x0
#define WORD_UNICODE            LOGO_MAX_SIZE    
#define BYTE_ASCII_ADDR         (WORD_UNICODE + WORD_UNICODE_SIZE)    
#define LARGE_FONT_ADDR         (BYTE_ASCII_ADDR + BYTE_ASCII_SIZE)   

#define FLASH_SIGN_ADDR         (LARGE_FONT_ADDR + LARGE_FONT_SIZE)       
#define LANGUAGE_ADDR           (FLASH_SIGN_ADDR + FLASH_SIGN_SIZE)   
#define STRINGS_STORE_ADDR      (LANGUAGE_ADDR + LANGUAGE_SIZE)   
#define PRINT_GCODES_ADDR       (STRINGS_STORE_ADDR + STRINGS_STORE_MAX_SIZE)   
#define CUSTOM_GCODE_ADDR       (PRINT_GCODES_ADDR + PRINT_GCODES_MAX_SIZE)     

#define ICON_ADDR(num)          ((num) * ICON_MAX_SIZE + CUSTOM_GCODE_ADDR+CUSTOM_GCODE_MAX_SIZE)
#define INFOBOX_ADDR            (ICON_ADDR(ICON_PREVIEW) + ICON_MAX_SIZE)   
#define SMALL_ICON_START_ADDR   (INFOBOX_ADDR + INFOBOX_MAX_SIZE)
#define SMALL_ICON_ADDR(num)    ((num)*SMALL_ICON_MAX_SIZE + SMALL_ICON_START_ADDR)
#define FLASH_USED              (INFOBOX_ADDR + INFOBOX_MAX_SIZE)   


#define ADMIN_MODE_FILE "admin.txt"
#define FIRMWARE_NAME STRINGIFY(HARDWARE) "." STRINGIFY(SOFTWARE_VERSION)
#define FIRMWARE_NAME_SHORT STRINGIFY(HARDWARE_SHORT) STRINGIFY(SOFTWARE_VERSION_SHORT)
#define BMP_ROOT_DIR "0:" ROOT_DIR "/bmp"
#define FONT_ROOT_DIR "0:" ROOT_DIR "/font"
#define TFT_RESET_FILE "reset.txt"


enum
{
  #define X_ICON(NAME) ICON_##NAME ,
  #ifdef WUXN_CODE
  #include "icon_list_wuxn.inc"
  #else
  #include "icon_list.inc"
  #endif
  #undef  X_ICON




  ICON_PREVIEW,

  ICON_BACKGROUND
};



enum
{
  #define X_SMALLICON(NAME) SMALL_ICON_##NAME ,
  #include "small_icon_list.inc"
  #undef  X_SMALLICON
  
  
    SMALL_ICON_BACKGROUND
};

typedef enum
{
  BMP_SUCCESS,
  BMP_NOTFOUND,
  BMP_NOT24BIT,
  BMP_INVALIDFILE
}BMPUPDATE_STAT;

typedef union
{
  uint16_t color;
  struct
  {
    uint16_t  b:5;
    uint16_t  g:6;
    uint16_t  r:5;
  }RGB;
}GUI_COLOR;

void scanUpdates(void);
void dispIconFail(uint8_t *lbl, BMPUPDATE_STAT bmpState);
BMPUPDATE_STAT bmpDecode(char *bmp, uint32_t addr);

#ifdef __cplusplus
}
#endif

#endif
