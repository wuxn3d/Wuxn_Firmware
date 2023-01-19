#include "SendGcode.h"
#include "includes.h"

#define TERMINAL_MAX_CHAR (NOBEYOND(600, RAM_SIZE*45, 4800))
#define MAX_PAGE_COUNT    20

typedef struct
{
  char *   ptr[MAX_PAGE_COUNT];  
  uint8_t  pageHead;             
  uint8_t  pageTail;             
  uint8_t  oldPageHead;          
  uint8_t  pageIndex;            
  uint8_t  oldPageIndex;         
  uint16_t terminalBufTail;      
  uint16_t buffSize;             
  uint8_t  maxPageCount;         
  uint8_t  bufferFull;
} TERMINAL_DATA;

typedef enum
{
  GKEY_SEND = 0,
  GKEY_ABC_123,
  GKEY_SPACE,
  GKEY_DEL,
  GKEY_BACK,
  
  GKEY_IDLE = IDLE_TOUCH,
} GKEY_VALUES;

typedef enum
{
  TERM_PAGE_NUMBER = 0,
  TERM_PAGE_UP,
  TERM_PAGE_DOWN,
  TERM_TOGGLE_ACK,
  TERM_BACK,
  TERM_KEY_COUNT,  
  TERM_IDLE = IDLE_TOUCH,
} TERMINAL_KEY_VALUES;

#ifndef KEYBOARD_MATERIAL_THEME
  #if KEYBOARD_COLOR_LAYOUT == 0
    #define KEY_FONT_COLOR     infoSettings.font_color
    #define KEY_BG_COLOR       infoSettings.bg_color
    #define KEY_BORDER_COLOR   infoSettings.list_border_color
    #define KEY_BORDER_COLOR_2 DARKGRAY
  #elif KEYBOARD_COLOR_LAYOUT == 1
    #define KEY_FONT_COLOR     infoSettings.font_color
    #define KEY_BG_COLOR       infoSettings.list_border_color
    #define KEY_BORDER_COLOR   infoSettings.bg_color
    #define KEY_BORDER_COLOR_2 DARKGRAY
  #else
    #define KEY_FONT_COLOR     BLACK
    #define KEY_BG_COLOR       WHITE
    #define KEY_BORDER_COLOR   0x2174
    #define KEY_BORDER_COLOR_2 DARKGRAY
  #endif

  #define KB_BG_COLOR          BLACK
  #define BAR_FONT_COLOR       WHITE
  #define BAR_BG_COLOR         0x2847 
  #define BAR_BORDER_COLOR     0x4b0d 
  #define TEXTBOX_FONT_COLOR   BLACK
  #define TEXTBOX_BG_COLOR     WHITE
#endif

#define TERM_FONT_COLOR infoSettings.marlin_mode_font_color
#define TERM_BG_COLOR   infoSettings.marlin_mode_bg_color


#define LAYOUT_1_COL_COUNT 6
#define LAYOUT_1_ROW_COUNT 4

#define LAYOUT_2_COL_COUNT 7
#define LAYOUT_2_ROW_COUNT 4

#define LAYOUT_3_COL_COUNT 10
#define LAYOUT_3_ROW_COUNT  6

#if LCD_WIDTH < 480  
  #define KB_TYPE_STANDARD
  #define KB_COL_COUNT LAYOUT_1_COL_COUNT
  #define KB_ROW_COUNT LAYOUT_1_ROW_COUNT
#elif LCD_WIDTH < 800
  #define KB_TYPE_EXTENDED
  #define KB_COL_COUNT LAYOUT_2_COL_COUNT
  #define KB_ROW_COUNT LAYOUT_2_ROW_COUNT
#else
  #define KB_TYPE_QWERTY
  #define KB_COL_COUNT LAYOUT_3_COL_COUNT
  #define KB_ROW_COUNT LAYOUT_3_ROW_COUNT
#endif

#define LAYOUT_QWERTY 0
#define LAYOUT_QWERTZ 1
#define LAYOUT_AZERTY 2

#define GRID_ROW_COUNT (1 + KB_ROW_COUNT + 1)         
#define ROW_HEIGHT     (LCD_HEIGHT / GRID_ROW_COUNT)  
#define CTRL_COL_COUNT 4                              


#define KEY_WIDTH  LCD_WIDTH / KB_COL_COUNT + 0.5
#define KEY_HEIGHT ROW_HEIGHT
#define KEY_COUNT  (1 + (KB_COL_COUNT * KB_ROW_COUNT) + (CTRL_COL_COUNT))  


#define CTRL_WIDTH           (LCD_WIDTH / CTRL_COL_COUNT) 
#define TERMINAL_CTRL_WIDTH  (LCD_WIDTH / TERM_KEY_COUNT) 
#define CTRL_HEIGHT          ROW_HEIGHT


#define TEXTBOX_INSET 4

#define COMMAND_START_ROW 0                              
#define KB_START_ROW      1                              
#define CRTL_START_ROW    (KB_START_ROW + KB_ROW_COUNT)  


#define CURSOR_H_OFFSET 2
#define CURSOR_END_Y    ((KB_START_ROW + KB_ROW_COUNT) * KEY_HEIGHT)


const GUI_RECT textBoxRect = {             0 + TEXTBOX_INSET, (COMMAND_START_ROW + 0) * CTRL_HEIGHT + TEXTBOX_INSET,
                              3 * CTRL_WIDTH - TEXTBOX_INSET, (COMMAND_START_ROW + 1) * CTRL_HEIGHT - TEXTBOX_INSET};


  const GUI_RECT editorKeyRect[KEY_COUNT] = {
  
  {3 * CTRL_WIDTH, COMMAND_START_ROW * CTRL_HEIGHT, 4 * CTRL_WIDTH, (COMMAND_START_ROW + 1) * CTRL_HEIGHT},  

  
  {0 * CTRL_WIDTH, (KB_START_ROW + KB_ROW_COUNT) * KEY_HEIGHT, 1 * CTRL_WIDTH, LCD_HEIGHT},  
  {1 * CTRL_WIDTH, (KB_START_ROW + KB_ROW_COUNT) * KEY_HEIGHT, 2 * CTRL_WIDTH, LCD_HEIGHT},  
  {2 * CTRL_WIDTH, (KB_START_ROW + KB_ROW_COUNT) * KEY_HEIGHT, 3 * CTRL_WIDTH, LCD_HEIGHT},  
  {3 * CTRL_WIDTH, (KB_START_ROW + KB_ROW_COUNT) * KEY_HEIGHT, 4 * CTRL_WIDTH, LCD_HEIGHT},  

  
  {0 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 1 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
  {1 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 2 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
  {2 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 3 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
  {3 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 4 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
  {4 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 5 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
  {5 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 6 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
  #if KB_COL_COUNT > LAYOUT_1_COL_COUNT
    {6 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 7 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
  #endif
  #if KB_COL_COUNT > LAYOUT_2_COL_COUNT
    {7 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 8 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
    {8 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 9 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
    {9 * KEY_WIDTH, (KB_START_ROW + 0) * KEY_HEIGHT, 10 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT},
  #endif

    
    {0 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 1 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
    {1 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 2 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
    {2 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 3 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
    {3 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 4 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
    {4 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 5 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
    {5 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 6 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
  #if KB_COL_COUNT > LAYOUT_1_COL_COUNT
    {6 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 7 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
  #endif
  #if KB_COL_COUNT > LAYOUT_2_COL_COUNT
    {7 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 8 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
    {8 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 9 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
    {9 * KEY_WIDTH, (KB_START_ROW + 1) * KEY_HEIGHT, 10 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT},
  #endif

    
    {0 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 1 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
    {1 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 2 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
    {2 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 3 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
    {3 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 4 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
    {4 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 5 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
    {5 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 6 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
  #if KB_COL_COUNT > LAYOUT_1_COL_COUNT
    {6 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 7 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
  #endif
  #if KB_COL_COUNT > LAYOUT_2_COL_COUNT
    {7 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 8 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
    {8 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 9 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
    {9 * KEY_WIDTH, (KB_START_ROW + 2) * KEY_HEIGHT, 10 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT},
  #endif
    
    {0 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 1 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
    {1 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 2 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
    {2 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 3 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
    {3 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 4 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
    {4 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 5 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
    {5 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 6 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
  #if KB_COL_COUNT > LAYOUT_1_COL_COUNT
    {6 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 7 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
  #endif
  #if KB_COL_COUNT > LAYOUT_2_COL_COUNT
    {7 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 8 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
    {8 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 9 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
    {9 * KEY_WIDTH, (KB_START_ROW + 3) * KEY_HEIGHT, 10 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT},
  #endif

  #if KB_COL_COUNT > LAYOUT_2_COL_COUNT
    
    {0 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 1 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},
    {1 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 2 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},
    {2 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 3 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},
    {3 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 4 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},
    {4 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 5 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},
    {5 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 6 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},
    {6 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 7 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},
    {7 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 8 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},
    {8 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 9 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},
    {9 * KEY_WIDTH, (KB_START_ROW + 4) * KEY_HEIGHT, 10 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT},

    
    {0 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 1 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
    {1 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 2 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
    {2 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 3 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
    {3 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 4 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
    {4 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 5 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
    {5 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 6 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
    {6 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 7 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
    {7 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 8 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
    {8 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 9 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
    {9 * KEY_WIDTH, (KB_START_ROW + 5) * KEY_HEIGHT, 10 * KEY_WIDTH, (KB_START_ROW + 6) * KEY_HEIGHT},
  #endif
};


const GUI_RECT editorAreaRect[3] = {
  {0, COMMAND_START_ROW * CTRL_HEIGHT, LCD_WIDTH, ROW_HEIGHT},                   
  {0, ROW_HEIGHT,                      LCD_WIDTH, LCD_HEIGHT - CTRL_HEIGHT},  
  {0, CRTL_START_ROW * CTRL_HEIGHT,    LCD_WIDTH, LCD_HEIGHT}                    
};

const GUI_RECT terminalKeyRect[TERM_KEY_COUNT] = {
  {0 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT - CTRL_HEIGHT, 1 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT},
  {1 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT - CTRL_HEIGHT, 2 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT},
  {2 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT - CTRL_HEIGHT, 3 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT},
  {3 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT - CTRL_HEIGHT, 4 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT},
  {4 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT - CTRL_HEIGHT, 5 * TERMINAL_CTRL_WIDTH, LCD_HEIGHT},
};

const GUI_RECT terminalAreaRect[2] = {
  {0,            0, LCD_WIDTH, CURSOR_END_Y},  
  {0, CURSOR_END_Y, LCD_WIDTH,   LCD_HEIGHT},  
};


const char * const gcodeKey123[KEY_COUNT] = {
    "Send", "ABC", "Space", "Del", "Back",
  #if KB_COL_COUNT == LAYOUT_1_COL_COUNT
    "1", "2", "3", "M", "G", "T",
    "4", "5", "6", "X", "Y", "Z",
    "7", "8", "9", "E", "F", "R",
    ".", "0", "-", "/", "S", "V",
  #elif KB_COL_COUNT == LAYOUT_2_COL_COUNT
    "1", "2", "3", "M", "G", "T", "V",
    "4", "5", "6", "X", "Y", "Z", "S",
    "7", "8", "9", "E", "F", "R", "Q",
    ".", "0", "-", "/", "I", "J", "P",
  #else
    "A", "B", "C", "D", "E", "F", "G", "1", "2", "3",
    "H", "I", "J", "K", "L", "M", "N", "4", "5", "6",
    "O", "P", "Q", "R", "S", "T", "U", "7", "8", "9",
    "V", "W", "X", "Y", "Z", "(", ")", ".", "0", "-",
    "!", "@", "#", "%", "&", ",", ";", "*", "/", "+",
    "~", "`", "$","\\","\"", "'", ":", "_", "=", "?",
  #endif
};


const char * const gcodeKeyABC[KEY_COUNT] = {
  "Send", "123", "Space", "Del", "Back",
  #if KB_COL_COUNT == LAYOUT_1_COL_COUNT
    "A", "B", "C", "D", "H", "I",
    "J", "K", "L", "N", "O", "P",
    ",", ";", ":", "Q", "U", "W",
    "+", "*", "?", "!", "#", "&",
  #elif KB_COL_COUNT == LAYOUT_2_COL_COUNT
    "A", "B", "C", "D", "H", "K", "L",
    ",", ";", ":", "N", "O", "U", "W",
    "+", "*", "?", "!", "#", "&", "$",
    "/", "=", "(", ")", "@", "_", "%",
  #else
    #if TERMINAL_KEYBOARD_LAYOUT == LAYOUT_QWERTY
      "!", "@", "#", "%", "&", "*", "(", ")", "-", "+",
      "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
      "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
      "A", "S", "D", "F", "G", "H", "J", "K", "L", ";",
      "Z", "X", "C", "V", "B", "N", "M", ",", ".", "/",
      "~", "`", "$","\\","\"", "'", ":", "_", "=", "?",
    #elif TERMINAL_KEYBOARD_LAYOUT == LAYOUT_QWERTZ
      "!", "\"", "$", "%", "&", "/", "(", ")", "=", "?",
      "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
      "Q", "W", "E", "R", "T", "Z", "U", "I", "O", "P",
      "A", "S", "D", "F", "G", "H", "J", "K", "L", "@",
      "Y", "X", "C", "V", "B", "N", "M", ",", ".", "-",
      "|", ";", ":", "_", "#", "~", "+", "*", "'", "\\",
    #elif TERMINAL_KEYBOARD_LAYOUT == LAYOUT_AZERTY
      "#", "@", "~", "&", "(", ")", "_", "'", "\"", "%",
      "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
      "A", "Z", "E", "R", "T", "Y", "U", "I", "O", "P",
      "Q", "S", "D", "F", "G", "H", "J", "K", "L", "M",
      "W", "X", "C", "V", "B", "N", ".", ",", ":", ";",
      "-", "+", "*", "\\", "|", "/", "?","!", "$", "=",
    #endif
  #endif
};

char * terminalBuf;
TERMINAL_DATA * terminalData;
uint8_t curView = 0;

bool numpad =
  #if defined(KB_TYPE_QWERTY)
    false;  
  #else
    true;
  #endif

static inline void keyboardDrawButton(uint8_t index, uint8_t isPressed)
{
  if (index >= COUNT(editorKeyRect))
    return;

 
  #ifdef KEYBOARD_MATERIAL_THEME
    uint16_t fontcolor = KEY_FONT_COLOR;
    uint16_t bgcolor = KEY_BG_COLOR;
    GUI_RECT rectBtn = {editorKeyRect[index].x0 + 3, editorKeyRect[index].y0 + 3,
                        editorKeyRect[index].x1 - 3, editorKeyRect[index].y1 - 3};

    switch (index)
    {
      case GKEY_SEND:
        fontcolor = CTRL_FONT_COLOR;
        bgcolor = MAT_GREEN;
        break;
      case GKEY_BACK:
        fontcolor = CTRL_FONT_COLOR;
        bgcolor = MAT_RED;
        break;
      case GKEY_ABC_123:
        fontcolor = CTRL_FONT_COLOR;
        bgcolor = MAT_SLATE;
        break;
      default:
        break;
    }

    BUTTON btn = {.fontColor  = fontcolor,
                  .backColor  = bgcolor,
                  .context    = (uint8_t *)((numpad) ? gcodeKey123[index] : gcodeKeyABC[index]),
                  .lineColor  = bgcolor,
                  .lineWidth  = 0,
                  .pBackColor = fontcolor,
                  .pFontColor = bgcolor,
                  .pLineColor = fontcolor,
                  .radius     = BTN_ROUND_CORNER,
                  .rect       = rectBtn};

  if (index != GKEY_SEND)
    setLargeFont(true);
  
    GUI_DrawButton(&btn, isPressed);

  #else 

    if (isPressed)
    {
      if (index > GKEY_BACK)
      {
        GUI_SetColor(KEY_BG_COLOR);
        GUI_SetBkColor(KEY_FONT_COLOR);
      }
      else
      {
        GUI_SetColor(BAR_BG_COLOR);
        GUI_SetBkColor(BAR_FONT_COLOR);
      }
    }
    else
    {
      if (index > GKEY_BACK)
      {
        GUI_SetColor(KEY_FONT_COLOR);
        GUI_SetBkColor(KEY_BG_COLOR);
      }
      else
      {
        GUI_SetColor(BAR_FONT_COLOR);
        GUI_SetBkColor(BAR_BG_COLOR);
      }
    }

    if (index != GKEY_SEND)
      setLargeFont(true);

    GUI_ClearRect(editorKeyRect[index].x0 + 1, editorKeyRect[index].y0 + 1, editorKeyRect[index].x1 - 1,
                  editorKeyRect[index].y1 - 1);

    GUI_DispStringInRect(editorKeyRect[index].x0 + 1, editorKeyRect[index].y0 + 1, editorKeyRect[index].x1 - 1,
                        editorKeyRect[index].y1 - 1, (uint8_t *)((numpad) ? gcodeKey123[index] : gcodeKeyABC[index]));

  #endif 

  setLargeFont(false);
}

static inline void drawGcodeText(char *gcode)
{
  GUI_SetColor(TEXTBOX_FONT_COLOR);
  GUI_SetBkColor(TEXTBOX_BG_COLOR);
  GUI_ClearPrect(&textBoxRect);

  if (gcode != NULL)
    GUI_DispStringInRect(textBoxRect.x0 + 1, textBoxRect.y0 + 1, textBoxRect.x1 - 1, textBoxRect.y1 - 1, (uint8_t *)gcode);
}

static inline void drawKeyboard(void)
{
  #ifndef KEYBOARD_MATERIAL_THEME
    GUI_SetColor(KEY_BORDER_COLOR);

    
    for (int i = 0; i < (KB_COL_COUNT - 1); i++)
    {
      GUI_VLine(editorKeyRect[i + GKEY_BACK + 1].x1, editorAreaRect[1].y0, editorAreaRect[1].y1);
    }

    
    for (int i = 0; i < (KB_ROW_COUNT - 1); i++)
    {
      GUI_HLine( editorAreaRect[1].x0, editorKeyRect[(i * KB_COL_COUNT)  + GKEY_BACK + 1].y1, editorAreaRect[1].x1);
    }
  #endif

  GUI_SetColor(BAR_BORDER_COLOR);
  GUI_HLine( editorAreaRect[0].x0, editorAreaRect[0].y1, editorAreaRect[0].x1);
  GUI_HLine( editorAreaRect[2].x0, editorAreaRect[2].y0, editorAreaRect[1].x1);

  for (uint8_t i = 0; i < COUNT(gcodeKeyABC); i++)
  {
    keyboardDrawButton(i, false);
  }
}

static inline void keyboardDrawMenu(void)
{
  setMenu(MENU_TYPE_FULLSCREEN, NULL, COUNT(editorKeyRect), editorKeyRect, keyboardDrawButton, NULL);

  
  GUI_SetBkColor(KB_BG_COLOR);
  GUI_ClearPrect(&editorAreaRect[1]);

  
  GUI_SetBkColor(BAR_BG_COLOR);
  GUI_ClearPrect(&editorAreaRect[0]);
  GUI_ClearPrect(&editorAreaRect[2]);

  #ifndef KEYBOARD_MATERIAL_THEME
    
    GUI_SetColor(BAR_BORDER_COLOR);

    
    GUI_DrawPrect(&textBoxRect);

    GUI_HLine(editorAreaRect[0].x0, editorAreaRect[0].y1, editorAreaRect[0].x1);
    GUI_HLine(editorAreaRect[2].x0, editorAreaRect[2].y0, editorAreaRect[2].x1);
  #endif

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  
  drawKeyboard();
  drawGcodeText(NULL);
}

static inline void menuKeyboardView(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  char gcodeBuf[CMD_MAX_CHAR] = {0};
  uint8_t nowIndex = 0;
  uint8_t lastIndex = 0;

  keyboardDrawMenu();

  while (curView == 1)
  {
    if (infoMenu.menu[infoMenu.cur] != menuTerminal)
      break;

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case GKEY_IDLE:
        break;

      case GKEY_BACK:
        infoMenu.cur--;
        break;

      case GKEY_SEND:
        if (nowIndex)
        {
          gcodeBuf[nowIndex++] = '\n'; 
          gcodeBuf[nowIndex] = 0;
          storeCmd(gcodeBuf);
          gcodeBuf[nowIndex = 0] = 0;
        }

        curView = 2;
        break;

      case GKEY_ABC_123:
        numpad = !numpad;
        drawKeyboard();
        break;

      case GKEY_DEL:
        if (nowIndex)
          gcodeBuf[--nowIndex] = 0;
        break;

      case GKEY_SPACE:
        if (nowIndex < CMD_MAX_CHAR - 1 && nowIndex > 0)
        {
          gcodeBuf[nowIndex++] = ' ';
          gcodeBuf[nowIndex] = 0;
        }
        break;

      default:
        if (nowIndex < CMD_MAX_CHAR - 1)
        {
          gcodeBuf[nowIndex++] = (numpad) ? gcodeKey123[key_num][0] : gcodeKeyABC[key_num][0];
          gcodeBuf[nowIndex] = 0;
        }
        break;
    }

    if (lastIndex != nowIndex)
    {
      lastIndex = nowIndex;
      drawGcodeText(gcodeBuf);
    }
    loopBackEnd();
  }
  GUI_RestoreColorDefault();
}

static inline void saveGcodeTerminalCache( char *str)
{
  uint16_t len = 0;
  uint16_t sign_len = strlen(str);
  if ((terminalData->terminalBufTail + sign_len) < terminalData->buffSize)
  {
    memcpy(terminalBuf + terminalData->terminalBufTail, str, sign_len);
    terminalData->terminalBufTail += sign_len;
  }
  else
  {
    len = (terminalData->terminalBufTail + sign_len) - terminalData->buffSize;
    memcpy(terminalBuf + terminalData->terminalBufTail, str, (sign_len - len));
    terminalData->terminalBufTail = 0;
    memcpy(terminalBuf + terminalData->terminalBufTail, str + (sign_len - len), len);
    terminalData->terminalBufTail += len;
    terminalData->bufferFull = 1;
    terminalData->pageHead++;
  }
}

void terminalCache(char *stream, TERMINAL_SRC src)
{
  if (infoMenu.menu[infoMenu.cur] != menuTerminal) return;

  if (src == TERMINAL_GCODE)
    saveGcodeTerminalCache(">>");

  saveGcodeTerminalCache(stream);
}

static inline void terminalDrawButton(uint8_t index, uint8_t isPressed)
{
  if (index >= TERM_KEY_COUNT || index == 0)
    return;

  const char * terminalKey[] = { "1/1", "<", ">", (char *)textSelect(itemToggle[infoSettings.terminalACK].index), "Back"};

  #ifdef KEYBOARD_MATERIAL_THEME
    uint16_t fontcolor = KEY_FONT_COLOR;
    uint16_t bgcolor = KEY_BG_COLOR;
    GUI_RECT rectBtn = {terminalKeyRect[index].x0 + 3, terminalKeyRect[index].y0 + 3,
                        terminalKeyRect[index].x1 - 3, terminalKeyRect[index].y1 - 3};

    if (index == TERM_BACK)
    {
      fontcolor = CTRL_FONT_COLOR;
      bgcolor = MAT_RED;
    }

    BUTTON btn = {.fontColor  = fontcolor,
                  .backColor  = bgcolor,
                  .context    = (uint8_t *)terminalKey[index],
                  .lineColor  = bgcolor,
                  .lineWidth  = 0,
                  .pBackColor = fontcolor,
                  .pFontColor = bgcolor,
                  .pLineColor = fontcolor,
                  .radius     = BTN_ROUND_CORNER,
                  .rect       = rectBtn};
  #else

  if (isPressed)
  {
    GUI_SetColor(BAR_BG_COLOR);
    GUI_SetBkColor(BAR_FONT_COLOR);
  }
  else
  {
    GUI_SetColor(BAR_FONT_COLOR);
    GUI_SetBkColor(BAR_BG_COLOR);
  }
  #endif 

  setLargeFont(true);

  #ifdef KEYBOARD_MATERIAL_THEME
    GUI_DrawButton(&btn, isPressed);
  #else
  GUI_ClearRect(terminalKeyRect[index].x0 + 1, terminalKeyRect[index].y0 + 1, terminalKeyRect[index].x1 - 1,
                terminalKeyRect[index].y1 - 1);
  GUI_DispStringInRect(terminalKeyRect[index].x0 + 1, terminalKeyRect[index].y0 + 1, terminalKeyRect[index].x1 - 1,
                       terminalKeyRect[index].y1 - 1, (uint8_t *)terminalKey[index]);
  #endif

  setLargeFont(false);
}

static inline void terminalDrawPageNumber(void)
{
  GUI_SetBkColor(BAR_BG_COLOR);
  GUI_ClearRect(terminalKeyRect[TERM_PAGE_NUMBER].x0, terminalKeyRect[TERM_PAGE_NUMBER].y0 + 1,
                terminalKeyRect[TERM_PAGE_NUMBER].x1, terminalKeyRect[TERM_PAGE_NUMBER].y1);

  char tempstr[10];
  sprintf(tempstr, "%d/%d", abs(((terminalData->pageTail - terminalData->pageHead) - terminalData->pageIndex) + 1),
          abs((terminalData->pageTail - terminalData->pageHead) + 1));

  GUI_SetColor(BAR_FONT_COLOR);
  setLargeFont(true);
  GUI_DispStringInPrect(&terminalKeyRect[TERM_PAGE_NUMBER], (uint8_t *)tempstr);
  setLargeFont(false);
}

static inline void terminalDrawMenu(void)
{
  setMenu(MENU_TYPE_FULLSCREEN, NULL, COUNT(terminalKeyRect), terminalKeyRect, terminalDrawButton, NULL);

  
  GUI_SetBkColor(TERM_BG_COLOR);
  GUI_ClearPrect(&terminalAreaRect[0]);

  
  GUI_SetBkColor(BAR_BG_COLOR);
  GUI_ClearPrect(&terminalAreaRect[1]);

  
  GUI_SetColor(BAR_BORDER_COLOR);
  GUI_HLine(terminalAreaRect[0].x0, terminalAreaRect[0].y1, terminalAreaRect[0].x1);

  
  for (uint8_t i = 0; i < COUNT(terminalKeyRect); i++)
  {
    terminalDrawButton(i, false);
  }
  terminalDrawPageNumber();
}

void menuTerminalWindow(void)
{
  #define CURSOR_START_X (terminalAreaRect[0].x0 + CURSOR_H_OFFSET)

  KEY_VALUES key_num = KEY_IDLE;
  CHAR_INFO info;
  uint16_t lastTerminalIndex = 0;
  uint8_t pageBufIndex = 0;
  int16_t cursorX = CURSOR_START_X;
  int16_t cursorY = terminalAreaRect[0].y0;
  terminalDrawMenu();

  while (curView == 2)
  {
    if (infoMenu.menu[infoMenu.cur] != menuTerminal)
      break;

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case TERM_PAGE_UP:  
        if (terminalData->pageIndex < (terminalData->pageTail - terminalData->pageHead))
        {
          terminalData->pageIndex++;
        }
        if ((terminalData->pageTail < terminalData->pageHead)
            && (terminalData->pageIndex < (terminalData->pageTail + terminalData->maxPageCount - terminalData->pageHead)))
        {
          terminalData->pageIndex++;
        }
        break;

      case TERM_PAGE_DOWN:  
        if (terminalData->pageIndex > 0)
          terminalData->pageIndex--;
        break;

      case TERM_TOGGLE_ACK:  
        infoSettings.terminalACK = (infoSettings.terminalACK + 1) % ITEM_TOGGLE_NUM;
        terminalDrawButton(TERM_TOGGLE_ACK, false);
        break;

      case TERM_BACK:  
        curView = 1;
        break;

      default:
        break;
    }

    
    if (terminalData->oldPageIndex != terminalData->pageIndex)
    {
      terminalData->oldPageIndex = terminalData->pageIndex;

      if (terminalData->pageTail >= terminalData->pageIndex)
        pageBufIndex = terminalData->pageTail - terminalData->pageIndex;
      else
        pageBufIndex = terminalData->pageTail + (terminalData->maxPageCount - terminalData->pageIndex);

      lastTerminalIndex = terminalData->ptr[pageBufIndex] - terminalBuf;
      cursorX = CURSOR_START_X;
      cursorY = terminalAreaRect[0].y0;

      GUI_SetBkColor(TERM_BG_COLOR);
      GUI_ClearPrect(&terminalAreaRect[0]);

      terminalDrawPageNumber();
    }

    while (terminalBuf + lastTerminalIndex && (lastTerminalIndex != terminalData->terminalBufTail))
    {
      getCharacterInfo((uint8_t *)(terminalBuf + lastTerminalIndex), &info);

      
      if (cursorX + info.pixelWidth > terminalAreaRect[0].x1 ||
          (terminalBuf[lastTerminalIndex] == '\n' && cursorX != CURSOR_START_X))
      {
        cursorX = CURSOR_START_X;
        cursorY += info.pixelHeight;
      }

      if (terminalBuf[lastTerminalIndex] != '\n')
      {
        if (cursorY + info.pixelHeight > terminalAreaRect[0].y1)  
        {
          if (terminalData->pageIndex != 0)
            break;

          terminalData->pageTail = (terminalData->pageTail + 1) % terminalData->maxPageCount;
          
          terminalData->ptr[terminalData->pageTail] = terminalBuf + lastTerminalIndex;

          
          if ((terminalData->bufferFull == 1) && (terminalData->oldPageHead == terminalData->pageHead))
            terminalData->pageHead++;

          if (terminalData->pageHead >= terminalData->maxPageCount)
            terminalData->pageHead = 0;

          terminalData->oldPageHead = terminalData->pageHead;
          cursorX = CURSOR_START_X;
          cursorY = terminalAreaRect[0].y0;

          GUI_SetBkColor(TERM_BG_COLOR);
          GUI_ClearPrect(&terminalAreaRect[0]);

          terminalDrawPageNumber();
        }

        GUI_SetColor(TERM_FONT_COLOR);
        GUI_SetBkColor(TERM_BG_COLOR);

        GUI_DispOne(cursorX, cursorY, (uint8_t *)(terminalBuf + lastTerminalIndex));
        cursorX += info.pixelWidth;
      }

      lastTerminalIndex += info.bytes;
      if (lastTerminalIndex >= terminalData->buffSize)
        lastTerminalIndex = 0;
    }

    loopBackEnd();
  }

  terminalData->bufferFull = 0;
  terminalData->terminalBufTail = 0;
  terminalBuf[0] = 0;
  terminalData->pageTail = 0;
  terminalData->pageHead = 0;
  terminalData->pageIndex = 0;
  terminalData->oldPageIndex = 0;

  GUI_RestoreColorDefault();
}

void menuTerminal(void)
{
  TERMINAL_DATA termPage = {{terminalBuf}, 0, 0, 0, 0, 0, 0, TERMINAL_MAX_CHAR, MAX_PAGE_COUNT, 0};

  if (isPrinting() || infoHost.printing) 
  {
    termPage.buffSize = (LCD_WIDTH / BYTE_WIDTH * LCD_HEIGHT / BYTE_HEIGHT);
    termPage.maxPageCount = 1;
  }

  char pageBuff[termPage.buffSize];

  terminalBuf = pageBuff;
  terminalData = &termPage;
  curView = 1;

  for(uint8_t i = 0; i < termPage.maxPageCount; i++)
  {
    termPage.ptr[i] = terminalBuf;
  }

  while (infoMenu.menu[infoMenu.cur] == menuTerminal)
  {
    if (curView == 1)
      menuKeyboardView();
    else if (curView == 2)
      menuTerminalWindow();
  }
}
