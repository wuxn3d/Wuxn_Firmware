#ifndef _LEDCOLOR_H_
#define _LEDCOLOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "variants.h"
#include "menu.h"


#define LED_OFF                     0x00000000
#define LED_WHITE                   0x00FFFFFF
#define LED_RED                     0x0000FF00
#define LED_ORANGE                  0x008CFF00
#define LED_YELLOW                  0x00FFFF00
#define LED_GREEN                   0x00FF0000
#define LED_BLUE                    0x000000FF
#define LED_INDIGO                  0x00004B82
#define LED_VIOLET                  0x0000FEFE


#define COLOR_LIGHTPINK             0xFFB6C1  
#define COLOR_PINK                  0xFFC0CB  
#define COLOR_CRIMSON               0xDC143C  
#define COLOR_LAVENDERBLUSH         0xFFF0F5  
#define COLOR_PALEVIOLETRED         0xDB7093  
#define COLOR_HOTPINK               0xFF69B4  
#define COLOR_DEEPPINK              0xFF1493  
#define COLOR_MEDIUMVIOLETRED       0xC71585  
#define COLOR_ORCHID                0xDA70D6  
#define COLOR_THISTLE               0xD8BFD8
#define COLOR_PLUM                  0xDDA0DD  
#define COLOR_VIOLET                0xEE82EE  
#define COLOR_MAGENTA               0xFF00FF  
#define COLOR_FUCHSIA               0xFF00FF  
#define COLOR_DARKMAGENTA           0x8B008B  
#define COLOR_PURPLE                0x800080
#define COLOR_MEDIUMORCHID          0xBA55D3  
#define COLOR_DARKVOILET            0x9400D3  
#define COLOR_DARKORCHID            0x9932CC  
#define COLOR_INDIGO                0x4B0082  
#define COLOR_BLUEVIOLET            0x8A2BE2  
#define COLOR_MEDIUMPURPLE          0x9370DB  
#define COLOR_MEDIUMSLATEBLUE       0x7B68EE  
#define COLOR_SLATEBLUE             0x6A5ACD  
#define COLOR_DARKSLATEBLUE         0x483D8B  
#define COLOR_LAVENDER              0xE6E6FA  
#define COLOR_GHOSTWHITE            0xF8F8FF  
#define COLOR_BLUE                  0x0000FF
#define COLOR_MEDIUMBLUE            0x0000CD  
#define COLOR_MIDNIGHTBLUE          0x191970  
#define COLOR_DARKBLUE              0x00008B  
#define COLOR_NAVY                  0x000080  
#define COLOR_ROYALBLUE             0x4169E1  
#define COLOR_CORNFLOWERBLUE        0x6495ED  
#define COLOR_LIGHTSTEELBLUE        0xB0C4DE  
#define COLOR_LIGHTSLATEGRAY        0x778899  
#define COLOR_SLATEGRAY             0x708090  
#define COLOR_DODERBLUE             0x1E90FF  
#define COLOR_ALICEBLUE             0xF0F8FF  
#define COLOR_STEELBLUE             0x4682B4  
#define COLOR_LIGHTSKYBLUE          0x87CEFA  
#define COLOR_SKYBLUE               0x87CEEB  
#define COLOR_DEEPSKYBLUE           0x00BFFF  
#define COLOR_LIGHTBLUE             0xADD8E6  
#define COLOR_POWDERBLUE            0xB0E0E6
#define COLOR_CADETBLUE             0x5F9EA0  
#define COLOR_AZURE                 0xF0FFFF  
#define COLOR_LIGHTCYAN             0xE1FFFF  
#define COLOR_PALETURQUOISE         0xAFEEEE  
#define COLOR_CYAN                  0x00FFFF  
#define COLOR_AQUA                  0x00FFFF  
#define COLOR_DARKTURQUOISE         0x00CED1  
#define COLOR_DARKSLATEGRAY         0x2F4F4F  
#define COLOR_DARKCYAN              0x008B8B  
#define COLOR_TEAL                  0x008080  
#define COLOR_MEDIUMTURQUOISE       0x48D1CC  
#define COLOR_LIGHTSEAGREEN         0x20B2AA  
#define COLOR_TURQUOISE             0x40E0D0  
#define COLOR_AUQAMARIN             0x7FFFAA  
#define COLOR_MEDIUMAQUAMARINE      0x00FA9A  
#define COLOR_MEDIUMSPRINGGREEN     0x00FF7F  
#define COLOR_MINTCREAM             0xF5FFFA  
#define COLOR_SPRINGGREEN           0x3CB371  
#define COLOR_SEAGREEN              0x2E8B57  
#define COLOR_HONEYDEW              0xF0FFF0  
#define COLOR_LIGHTGREEN            0x90EE90  
#define COLOR_PALEGREEN             0x98FB98  
#define COLOR_DARKSEAGREEN          0x8FBC8F  
#define COLOR_LIMEGREEN             0x32CD32  
#define COLOR_LIME                  0x00FF00  
#define COLOR_FORESTGREEN           0x228B22  
#define COLOR_GREEN                 0x008000
#define COLOR_DARKGREEN             0x006400  
#define COLOR_CHARTREUSE            0x7FFF00  
#define COLOR_LAWNGREEN             0x7CFC00  
#define COLOR_GREENYELLOW           0xADFF2F  
#define COLOR_OLIVEDRAB             0x556B2F  
#define COLOR_BEIGE                 0xF5F5DC  
#define COLOR_LIGHTGOLDENRODYELLOW  0xFAFAD2  
#define COLOR_IVORY                 0xFFFFF0  
#define COLOR_LIGHTYELLOW           0xFFFFE0  
#define COLOR_YELLOW                0xFFFF00
#define COLOR_OLIVE                 0x808000  
#define COLOR_DARKKHAKI             0xBDB76B  
#define COLOR_LEMONCHIFFON          0xFFFACD  
#define COLOR_PALEGODENROD          0xEEE8AA  
#define COLOR_KHAKI                 0xF0E68C  
#define COLOR_GOLD                  0xFFD700
#define COLOR_CORNISLK              0xFFF8DC  
#define COLOR_GOLDENROD             0xDAA520  
#define COLOR_FLORALWHITE           0xFFFAF0  
#define COLOR_OLDLACE               0xFDF5E6  
#define COLOR_WHEAT                 0xF5DEB3  
#define COLOR_MOCCASIN              0xFFE4B5  
#define COLOR_ORANGE                0xFFA500  
#define COLOR_PAPAYAWHIP            0xFFEFD5  
#define COLOR_BLANCHEDALMOND        0xFFEBCD  
#define COLOR_NAVAJOWHITE           0xFFDEAD  
#define COLOR_ANTIQUEWHITE          0xFAEBD7  
#define COLOR_TAN                   0xD2B48C  
#define COLOR_BRULYWOOD             0xDEB887  
#define COLOR_BISQUE                0xFFE4C4  
#define COLOR_DARKORANGE            0xFF8C00  
#define COLOR_LINEN                 0xFAF0E6  
#define COLOR_PERU                  0xCD853F  
#define COLOR_PEACHPUFF             0xFFDAB9  
#define COLOR_SANDYBROWN            0xF4A460  
#define COLOR_CHOCOLATE             0xD2691E  
#define COLOR_SADDLEBROWN           0x8B4513  
#define COLOR_SEASHELL              0xFFF5EE  
#define COLOR_SIENNA                0xA0522D  
#define COLOR_LIGHTSALMON           0xFFA07A  
#define COLOR_CORAL                 0xFF7F50  
#define COLOR_ORANGERED             0xFF4500  
#define COLOR_DARKSALMON            0xE9967A  
#define COLOR_TOMATO                0xFF6347
#define COLOR_MISTYROSE             0xFFE4E1  
#define COLOR_SALMON                0xFA8072  
#define COLOR_SNOW                  0xFFFAFA
#define COLOR_LIGHTCORAL            0xF08080  
#define COLOR_ROSYBROWN             0xBC8F8F  
#define COLOR_INDIANRED             0xCD5C5C  
#define COLOR_RED                   0xFF0000
#define COLOR_BROWN                 0xA52A2A  
#define COLOR_FIREBRICK             0xB22222  
#define COLOR_DARKRED               0x8B0000  
#define COLOR_MAROON                0x800000  
#define COLOR_WHITE                 0xFFFFFF
#define COLOR_WHITESMOKE            0xF5F5F5  
#define COLOR_GAINSBORO             0xDCDCDC  
#define COLOR_LIGHTGREY             0xD3D3D3  
#define COLOR_SILVER                0xC0C0C0  
#define COLOR_DARKGRAY              0xA9A9A9  
#define COLOR_GRAY                  0x808080  
#define COLOR_DIMGRAY               0x696969  
#define COLOR_BLACK                 0x000000


#ifdef LED_COLOR_PIN
  #define LED_COLOR_NUM 9
  #define LED_IDLE 2

  extern const LABEL itemLedcolor[LED_COLOR_NUM];

  extern const uint32_t led_color[LED_COLOR_NUM];

  void knob_LED_Init(void);
  void knob_LED_DeInit(void);
  void WS2812_Send_DAT(uint32_t ws2812_dat);
#endif

#ifdef __cplusplus
}
#endif

#endif
