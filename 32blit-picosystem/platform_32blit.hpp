// make; ll; cp pico.uf2 /media/dev/RPI-RP2/
// rm -rf *; cmake .. -DCMAKE_TOOLCHAIN_FILE=$HOME/32blit-sdk-new/pico.toolchain   -D32BLIT_DIR=$HOME/32blit-sdk-new -DPICO_SDK_PATH=$HOME/pico-sdk -DPICO_EXTRAS_PATH=$HOME/pico-extras -DPICO_BOARD=pimoroni_picosystem  -DCMAKE_BUILD_TYPE=Release -DALLOW_HIRES=0


#ifndef YARMICO_PLATFORM
#define YARMICO_PLATFORM


#include "32blit.hpp"
#include "build.h" // build defines changed before diferent builds


/* created via assets.yml
 *

all upper case in vars will be lower case
 ie: asset_SFX_01 -> asset_sfx_01


assets-images.cpp:
  TPAGE01.png:
   transparent: 0,0,0
   name: tpage01

....

assets-SFX.cpp:
  audio/01.raw:
   name: asset_SFX_01

....

assets-BGM.cpp:
  audio/INTRO.raw:
   name: asset_BGM_INTRO

....

 */

#include "assets-images.hpp"

#if HARDWARE_TARGET !=  HARDWARE_TARGET_PICOSYSTEM

#if YARMICO_USE_BGM == TRUE
#include "assets-SFX.hpp"
#include "assets-BGM.hpp"
#endif


#endif //#if HARDWARE_TARGET !=  HARDWARE_TARGET_PICOSYSTEM


using namespace blit;

// 32blit image --input_file y3-256.png  --output_file y3.cpp  --output_format c_source --symbol_name TEXT  --packed yes
#define LOG_PAD //printf

#define YARMICO_SCREEN_WIDTH_BORDER_START_240X240_256X240     8
#define YARMICO_SCREEN_HEIGHT_BORDER_START_240X240_256X240	0


#define YARMICO_SCREEN_WIDTH_BORDER_START_320X240_256X240 	0
#define YARMICO_SCREEN_HEIGHT_BORDER_START_320X240_256X240	0

#define YARMICO_SCREEN_WIDTH_BORDER_START_160X120_256X240 	48
#define YARMICO_SCREEN_HEIGHT_BORDER_START_160X120_256X240	60

#define YARMICO_SCREEN_WIDTH_BORDER_START_120X120_256X240 	68
#define YARMICO_SCREEN_HEIGHT_BORDER_START_120X120_256X240	60


// https://github.com/32blit/32blit-sdk/blob/master/docs/pico.md#board-specific-details
#if YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_320X240_640X480 //320x240 up scaled
#define YARMICO_SCREEN_WIDTH	640
#define YARMICO_SCREEN_HEIGHT	480
#define USE_SCREEN_MODE ScreenMode::hires

#elif YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_320X240 //  	320x240 no up scaling

#define YARMICO_SCREEN_WIDTH	320
#define YARMICO_SCREEN_HEIGHT	240
#define USE_SCREEN_MODE ScreenMode::hires




#elif YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_240x240 // 240x240 boarded widths for 32blit
// PicoSystem hires
#define YARMICO_SCREEN_WIDTH	240
#define YARMICO_SCREEN_HEIGHT	240
#define USE_SCREEN_MODE ScreenMode::hires


#elif YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_160x120 // 160x120 boarded widths and heights
// 32blit  160x120 lores or VGA Board
#define YARMICO_SCREEN_WIDTH	160
#define YARMICO_SCREEN_HEIGHT	120
#define USE_SCREEN_MODE ScreenMode::lores

#elif YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_120x120 // 120x120 boarded widths and heights
// 32blit/SDL 320x240 hires 160x120 lores
#define YARMICO_SCREEN_WIDTH	120
#define YARMICO_SCREEN_HEIGHT	120
#define USE_SCREEN_MODE ScreenMode::lores

#endif





#ifdef _RELEASE_


#define LOG_DATA  // blit::debugf
#define LOG_EXIT  //  blit::debugf
#define LOG_SND   //  blit::debugf
#define LOG_DEL  //  blit::debugf
#define LOG_PAD  //  blit::debugf
#define LOG_MAIN  //  blit::debugf
#else



			#ifdef __ANDROID__
			#include <android/log.h>
			#define  LOG_TAG    "yarmico"
			#define  LOG_DATA(...) // __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
			#define  LOG_EXIT(...) // __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
			#define  LOG_SND(...)  // __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
			#define  LOG_DEL(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
			#define  LOG_PAD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
			#define  LOG_MAIN(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)


			#else // #ifdef __ANDROID__

			#define LOG_DATA   blit::debugf
			#define LOG_EXIT    blit::debugf
			#define LOG_SND    // blit::debugf
			#define LOG_DEL    blit::debugf
			#define LOG_PAD   // blit::debugf
			#define LOG_MAIN   blit::debugf
			#endif

#endif

#ifdef __ANDROID__
#define LOG_ERR(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#else
#define LOG_ERR blit::debugf
#endif


#endif //#ifndef YARMICO_PLATFORM
