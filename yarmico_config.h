/*
 *
 PUBLIC DOMAIN 2022
 THIS CODE IS A HACKY MESS - READ IT AT YOUR OWN RISK!
 https://github.com/gwald/yarmico
 *
 */

#ifndef YARMICO_CONFIG
#define YARMICO_CONFIG

#include "build.h"

#ifndef TRUE
#define TRUE 1
#endif


#ifndef FALSE
#define FALSE 0
#endif



#define YARMICO_USE_BGM   FALSE // 1 YES 0 NO
#define YARMICO_PICO_SOUND TRUE // 1 YES 0 NO


// #0 title - ALL CAPS NO SPACES

#define YARMICO_PROJECT_CODE_STRING "YARMICO"
#define YARMICO_PROJECT_TITLE_STRING "YARMICO"
#define YARMICO_PROJECT_DATE_STRING "2022-NOV-11"


// #1 set the TPAGES USED
 #define USE_TPAGE00
//#define USE_TPAGE01
//#define USE_TPAGE02
//#define USE_TPAGE03
//#define USE_TPAGE04
//#define USE_TPAGE05

// #2 set the font tpage

#define FONT_TPAGE 0 // TPAGE image used where 8x8 font is located
#define FONT_Y_OFFSET  0 // start Y - the starting Y pixel of the font


// #3 set 32blit resolution and upscale
// default to SDL2 or picosystem
#define HARDWARE_TARGET HARDWARE_TARGET_NET_YAROZE

#define YARMICO_PICO_SHOW_SIDES

#define YARMICO_NATIVE_DISPLAY YARMICO_NATIVE_DISPLAY_120x120
#define YARMICO_DEFAULT_DISPLAY_UPSCALE  FALSE // TRUE or FALSE - Default low res (160/120 width will be up scaled to 256 or 320)

// #4 set how the analog is handled in the controller function
#define YARMICO_CONTROLLER_ANALOG_TYPE YARMICO_CONTROLLER_ANALOG_IGNORE
// YARMICO_CONTROLLER_ANALOG_PROCESS_LEFT_STICK_AS_DPAD

// #define SFX_00

// #5 2D or 3D
//Default to 2D - 3D untested
#define YARMICO_MATH_COORDS YARMICO_MATH_COORDS_2D




#endif //#ifndef YARMICO_CONFIG
