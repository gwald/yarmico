/*
 *
 PUBLIC DOMAIN 2022
 THIS CODE IS A HACKY MESS - READ IT AT YOUR OWN RISK!
 https://github.com/gwald/yarmico
 *
 */


#ifndef YARMICO_API
#define YARMICO_API




#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>






#ifndef TRUE
#define TRUE 1
#endif


#ifndef FALSE
#define FALSE 0
#endif


#define YARMICO_MATH_COORDS_3D 0
#define YARMICO_MATH_COORDS_2D 1





// 32blit Background music, asset_bgm_00
// #define USE_BGM_00
// #define USE_BGM_01
// #define USE_BGM_02
// #define USE_BGM_04
// #define USE_BGM_05



// https://github.com/32blit/32blit-sdk/blob/master/docs/pico.md#board-specific-details
// 	         32blit 	                    PicoSystem 	             VGA Board
//Screen 	320x240 (160x120 lores) 	240x240 (120x120 lores) 	160x120 only

#define YARMICO_NATIVE_DISPLAY_320X240_640X480 0 //  	320x240 scaling 640X480
#define YARMICO_NATIVE_DISPLAY_320X240 1 //  	320x240 no up scaling

//256x240
#define YARMICO_NATIVE_DISPLAY_240x240 2 // 	240x240 boarded widths

//upscalable
#define YARMICO_NATIVE_DISPLAY_160x120 3 // 	160x120 boarded widths and heights
#define YARMICO_NATIVE_DISPLAY_120x120 4 // 	120x120 boarded widths and heights


#define YARMICO_CONTROLLER_ANALOG_IGNORE 0
#define YARMICO_CONTROLLER_ANALOG_PROCESS_LEFT_STICK_AS_DPAD 1
#define YARMICO_CONTROLLER_ANALOG_PROCESS_LEFT_AND_RIGHT_STICK_AS_DPAD_AND_FACE_BUTTONS 2
#define YARMICO_CONTROLLER_ANALOG_RETURN_ANALOG_DATA 3 // u32 - each plain 8bit * 4

#define YARMICO_ANALOG_DEAD_ZONE 10

#include "yarmico_config.h" // build def's changed before different builds, easier then setting make param's PITA!

// #0 titles & date  - ALL CAPS NO SPACES
#ifndef YARMICO_PROJECT_CODE_STRING
#define YARMICO_PROJECT_CODE_STRING "CODE"
#endif

#ifndef YARMICO_PROJECT_TITLE_STRING
#define YARMICO_PROJECT_TITLE_STRING "TITLE"
#endif

#ifndef YARMICO_PROJECT_DATE_STRING
#define YARMICO_PROJECT_DATE_STRING "2020-OCT-10"
#endif



// #1 set the TPAGES USED
//#define USE_TPAGE00
//#define USE_TPAGE01
//#define USE_TPAGE02
//#define USE_TPAGE03
//#define USE_TPAGE04
//#define USE_TPAGE05

// #2 set the font tpage
#ifdef USE_TPAGE00

#ifndef FONT_TPAGE
#define FONT_TPAGE 0 // TPAGE image used where 8x8 font is located
#endif

#ifndef FONT_Y_OFFSET
#define FONT_Y_OFFSET  0 // start Y - the starting Y pixel of the font
#endif

#endif ///USE_TPAGE00



// #3 set 32blit resolution and upscale
#ifndef YARMICO_NATIVE_DISPLAY
//Default set to SDL2
#define YARMICO_NATIVE_DISPLAY YARMICO_NATIVE_DISPLAY_120x120
// #define YARMICO_DEFAULT_DISPLAY_UPSCALE  1 // TRUE or FALSE - Default low res (160/120 width will be up scaled to 256 or 320)
#endif





// #4 set how the analog is handled in the controller function
#ifndef YARMICO_CONTROLLER_ANALOG_TYPE
#define YARMICO_CONTROLLER_ANALOG_TYPE YARMICO_CONTROLLER_ANALOG_PROCESS_LEFT_STICK_AS_DPAD
#endif



#if YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_320X240_640X480
#define YARMICO_WIDTH  320
#define YARMICO_HEIGHT 240

#elif YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_320X240
#define YARMICO_WIDTH  320
#define YARMICO_HEIGHT 240

#elif  YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_240x240
#define YARMICO_WIDTH  240
#define YARMICO_HEIGHT 240

#elif  YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_160x120
#define YARMICO_WIDTH  160
#define YARMICO_HEIGHT 120

#elif  YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_120x120
#define YARMICO_WIDTH  120
#define YARMICO_HEIGHT 120

#endif


// #5 2D or 3D
#ifndef YARMICO_MATH_COORDS
//Default to 2D
#define YARMICO_MATH_COORDS YARMICO_MATH_COORDS_2D

#endif


// build types, defined via cmake, makefile, android.mk or in build.h static or changed via build scripts
#define	HARDWARE_TARGET_NONE 0
#define	HARDWARE_TARGET_NET_YAROZE 1
#define	HARDWARE_TARGET_SDL2 2 // PC/Linux/Android/Ouya/HTML5
#define	HARDWARE_TARGET_32BLIT_HANDHELD 3
#define	HARDWARE_TARGET_PICOSYSTEM 4





#ifndef HARDWARE_TARGET
//Default set to SDL2
#define HARDWARE_TARGET HARDWARE_TARGET_SDL2
#endif




#ifndef YARMICO_USE_BGM
#define YARMICO_PLAY_BGM FALSE // 1 YES 0 NO
#endif


#ifndef YARMICO_PICO_SOUND
#define YARMICO_PICO_SOUND FALSE // 1 YES 0 NO
#endif





#if 0 // SDL



#include <string.h>


#ifdef __ANDROID__


//https://stackoverflow.com/questions/10274920/how-to-get-printf-messages-written-in-ndk-application
#include <android/log.h>

extern int android_debugf( const char *format, ...);

#define  LOG_TAG    "yarmico"

#define  ANDROID_LOG(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  printf ANDROID_LOG 


#endif // #ifdef __ANDROID__



//save location:  ~/.local/share/
#endif






//-------------------------------------------------------------------------------------------------- #define's


#if 0
#define _RELEASE_
#else
#define _DEBUG_
#endif

#if 0
#ifdef UNIT_32BLIT
#undef _DEBUG_
#define _RELEASE_
#endif
#endif


#ifdef _RELEASE_
#undef _DEBUG_
#define MACRO_NO_SAVE_LOADING //return 0;
#else
#define MACRO_NO_SAVE_LOADING //return 0;
#endif



// #define LOG_DEBUG_COL

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef signed char s8;
// #define s8 signed char
typedef signed short s16;
typedef signed int s32;


// THIS IS HOW DEBUGGING IS MOSTLY DONE IN PSX - VIA TTY OUTPUT
// COULD BE FANCIER BUT SIMPLE IS FINE

#ifdef _DEBUG_


#ifdef __ANDROID__

#define LOG_AUDIO 		  ANDROID_LOG
#define LOG_TEXT ANDROID_LOG

#else //#ifdef __ANDROID__


#define LOG_AUDIO // printf
#define LOG_TEXT // printf
#endif





#else


#define LOG_AUDIO //printf
#define LOG_TEXT //printf
#endif




#ifdef INSTANCE_YARMICO_API
#define INSTANCE_API
#else
#define INSTANCE_API extern
#endif





#define YARMICO_BUTTON_PRESS_TIME 8


// DPad (analogy) and AB/XY face buttons use the same binary checks
#define YARMICO_CONTROLER_BUT_NONE 0
#define YARMICO_CONTROLER_BUT_R 1 // 0b0001 // right 1
#define YARMICO_CONTROLER_BUT_L 2 // 0b0010 // left 2
#define YARMICO_CONTROLER_BUT_U 4  // 0b0100 // up 4
#define YARMICO_CONTROLER_BUT_D 8  // 0b1000 // down 8



//special buttons
#define YARMICO_CONTROLER_BUT_ANALOG_SHOULDERS 		 1 // 0b0001 // right 1
#define YARMICO_CONTROLER_BUT_HOME_SELECT				 2 // 0b0010 // left 2
#define YARMICO_CONTROLER_BUT_MENU_START				 4  // 0b0100 // up 4
//#define YARMICO_CONTROLER_BUT_NONE					 8  // 0b1000 // down 8


#define YARMICO_CONTROLER_BUT_ALL_DIR 15
#define YARMICO_CONTROLER_BUT_UP_DOWN 12
#define YARMICO_CONTROLER_BUT_LEFT_RIGHT 3



// circle counter clockwise
#define YARMICO_CONTROLER_BUT_RR 1 // 0b0001 // right right 1
#define YARMICO_CONTROLER_BUT_RU 5 // 0b0101 // right up 5

#define YARMICO_CONTROLER_BUT_UU 4 // 0b0100 // up up 4

#define YARMICO_CONTROLER_BUT_LU 6 // 0b0110 // left up 6
#define YARMICO_CONTROLER_BUT_LL 2 // 0b0010 // left left 2
#define YARMICO_CONTROLER_BUT_LD 10 // 0b1010 // left down 10

#define YARMICO_CONTROLER_BUT_DD 8 // 0b1000 // down down 8
#define YARMICO_CONTROLER_BUT_RD 9 // 0b1001 // right down 9


#if 0
#define CONTROL_BUT_NONE 0
#define CONTROL_BUT_A   1 //  0b0001
#define CONTROL_BUT_B   2 //  0b0010
#define CONTROL_BUT_MENU 4 // 0b0100
#define CONTROL_BUT_HOME 8 // 0b1000
//#define CONTROL_BUT_X    0b0100
//#define CONTROL_BUT_Y    0b1000

#endif

// from PS1
#ifndef ONE
#define ONE 4096
#endif

#define ONE_SHIFT 12 // 4096 >> 12 = 1

//-------------------------------------------------------------------------------------------------- typedef's







//-------------------------------------------------------------------------------------------------- GLOBAL's

#ifdef __cplusplus
extern "C" {
#endif


// based on the PS1 analog data
// max = 125 * ONE
// min = 1 * ONE

typedef struct{
	s32 left_stick_x,  // -left <- 0 -> +right
	left_stick_y;  // -down <- 0 -> +up

	s32 right_stick_x,  // -left <- 0 -> +right
	right_stick_y;  // -down <- 0 -> +up
}yarmico_analog_stick_t;




#define  YARMICO_PICO_SOUND_SFX_NONE 		0
#define  YARMICO_PICO_SOUND_SFX1_ON_OFF 	1
#define  YARMICO_PICO_SOUND_SFX2_UP_DOWN 	2
#define  YARMICO_PICO_SOUND_SFX2_ALT_SIGN 	4

#define  YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK 		0
#define  YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_DECAY		1
#define  YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_SUSTAIN		2
#define  YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_RELEASE		4
#define  YARMICO_PICO_SOUND_PLAYBACK_OFF 					8

#define  YARMICO_PICO_SOUND_LOOP_REPEAT 		2
#define  YARMICO_PICO_SOUND_LOOP_TIMES 		1
#define  YARMICO_PICO_SOUND_LOOP_NONE 		0


#define  YARMICO_MACRO_SET_PICO_AUDIO(_FREQ, _ATT, _DEC, _SUS, _REL, _REPEAT )  g_yarmico_pico_play_sound.frequency =_FREQ; g_yarmico_pico_play_sound.attack_ms =_ATT; g_yarmico_pico_play_sound.decay_ms = _DEC; g_yarmico_pico_play_sound.sustain = _SUS; g_yarmico_pico_play_sound.release_ms = _REL; g_yarmico_pico_play_sound.time_frames_repeat = g_yarmico_pico_play_sound.time_frames = _REPEAT;



#if HARDWARE_TARGET == HARDWARE_TARGET_NET_YAROZE
#define YARMICO_UPDATE_TIME_SHIFT 6
#else
#define YARMICO_UPDATE_TIME_SHIFT 6
#endif


typedef struct{
	u32 time_frames; //frames to play sound
	u32 time_frames_repeat; //frames to repeat play sound

	//from 32blit sdk audio.hpp
	//u8   waveforms     = 0;      // bitmask for enabled waveforms (see AudioWaveform enum for values)
	u16  frequency;//     = 660;    // frequency of the voice (Hz)
	u16  volume;//        = 0xffff; // channel volume (default 50%)
	u16  attack_ms;//     = 2;      // attack period
	u16  decay_ms;//      = 6;      // decay period
	u16  sustain;//       = 0xffff; // sustain volume
	u16  release_ms;//    = 1;      // release period
	u16  pulse_width;//   = 0x7fff; // duty cycle of square wave (default 50%)
	//u16   noise         = 0;      // current noise value

	u8 play_back; //play back what? YARMICO_PICO_SOUND_PLAYBACK_ trigger_decay  trigger_sustain  trigger_release  off
	s8 dir_speed; //+up/-down speed
	u8 SFX;
	u8 SFX_trigger_ON_OFF;
	u8 SFX_hold_ON_OFF;
	u8 SFX_counter_ON_OFF;

	u8 SFX_trigger2;
	u8 SFX_hold2;
	u8 SFX_counter2;

	u8 loop;
	u32 loop_times;

}yarmico_pico_sound_t;



#if HARDWARE_TARGET == HARDWARE_TARGET_NET_YAROZE
INSTANCE_API s8 g_SET_SFX_VOL; //max
INSTANCE_API s8 g_SET_BGM_VOL; //max

#else
INSTANCE_API u16 g_SET_SFX_VOL; //max
INSTANCE_API u16 g_SET_BGM_VOL; //max
#endif


INSTANCE_API  yarmico_pico_sound_t g_yarmico_pico_play_sound;

INSTANCE_API u8 g_yarmico_controller_dpad, g_yarmico_controller_action_buttons, g_yarmico_controller_special_buttons;
INSTANCE_API  yarmico_analog_stick_t  g_yarmico_controller_analog_sticks;

INSTANCE_API u32 g_yarmico_start_time;
INSTANCE_API u32 g_yarmico_game_time;
INSTANCE_API u32 g_yarmico_frame_counter;

INSTANCE_API u32 g_yarmico_lores_upscale[3]; // scale(true/false), border width/hieght

INSTANCE_API void (*g_gameloop_function)(void);




//-------------------------------------------------------------------------------------------------- functions

// game virtual functions - these must be implemented in game code

void game_deinit(void); // player hit ESC or START & SELECT
void game_init(void); // game is responsible for deinit'ing and exiting
void game_update(void); // core loop






void  yarmico_play_pico_sound_platform(u32 turn_sound_off); //platform that plays it back


#define YARMICO_PICO_PIEZO_CHANNEL 0

void  yarmico_play_pico_sound(void);
void  yarmico_LED(u8 r, u8 g, u8 b);





//void  yarmico_intro_splash(void);
u32  yarmico_save_data(char *data, u32 size);
u32  yarmico_load_data(char *data,  u32 size);
u32  yarmico_delete_data(u32 slot);

// common game functions
void  yarmico_rectangle(u8 r, u8 g, u8 b, u32 screen_posX, u32 screen_posY, u32 w, u32 h );
void  yarmico_sprite(u8 TPAGE, u32 texture_u, u32 texture_v, u32 texture_w, u32 texture_h, s32 posX, s32 posY, s32 scale , u8 h_flip, u8 v_flip);
void  yarmico_render_text(u32 posx, u32 posy, s32 scale, char *string);
void  yarmico_do_C64_loading_screen(void);
u32  yarmico_random(void);

// internal platform functions

// void  yarmico_init(void);

u32  yarmico_play_SFX(u32 id); // SFX can NOT be stopped after started!

void  yarmico_play_BGM_audio(int side);
void  yarmico_BGM_quiet(int quiet); // 1 true reduce vol - 0 false increase back to normal BG volume
void  yarmico_stop_all_SFX(void);
void  yarmico_update_time(void);

void  yarmico_process_controller(void);


void game_render(void);
void game_reset(void);


#ifdef __cplusplus
} // extern "C" {
#endif

INSTANCE_API u32 g_stereo_counter_random;

#define ADD_TO_SFX 11 // random TT SFX

#if HARDWARE_TARGET == HARDWARE_TARGET_NET_YAROZE




extern void RenderClear(void);

// probably better random
// https://github.com/ptitSeb/freespace2/blob/500ee249f7033aac9b389436b1737a404277259c/src/math/staticrand.cpp#L37




// PSX VOL is 8bit 0-255
#define VOL_MAX 127
#define BGM_VOL_DEFAULT 100
#define SFX_VOL_DEFAULT VOL_MAX
#define VOL_CHANGE 10


#else


#define BGM_VOL_DEFAULT 37000 //12766
#define SFX_VOL_DEFAULT 40000 //32766
#define VOL_MAX 65535
#define VOL_CHANGE 1023*4


#endif // 32blit





// code from boook Making 8-Bit Arcade Games in C by Steven Hugg
// https://github.com/sehugg/8bitworkshop/blob/4ccf588c80dbcd7ac42fadcf8a07c7c3951e6c6d/presets/williams-z80/game1.c#L717

#define YARMICO_DEFAULT_TILE_DOUBLE_SQUARE_SIZE 16
#define YARMICO_DEFAULT_TILE_FULL_SQUARE_SIZE 8
#define YARMICO_DEFAULT_TILE_HALF_SQUARE_SIZE 4
#define YARMICO_DEFAULT_FIXED_POINT_SIZE




typedef struct  {
	s32 x, y, w, h;
#if YARMICO_MATH_COORDS == YARMICO_MATH_COORDS_3D
	s32 z, d;
#endif
}yarmico_rect_t;

typedef struct  {
	yarmico_rect_t pos_rect;
	u8 tpage;
	u8 u,v;


	//direction
#if YARMICO_MATH_COORDS == YARMICO_MATH_COORDS_3D
	u8 dx, dy, dz;
#else
	u8 dx, dy;
#endif

	u8 dpad; //8bit controller_dpad
	u8 speed;

}  yarmico_item_t;

INSTANCE_API s32 g_yarmico_collision_test_x;
INSTANCE_API u32 g_yarmico_collision_test_xw;
INSTANCE_API s32 g_yarmico_collision_test_y;
INSTANCE_API u32 g_yarmico_collision_test_yh;

INSTANCE_API  yarmico_item_t* g_yarmico_collision_test_item_ptr;


#if YARMICO_MATH_COORDS == YARMICO_MATH_COORDS_3D
INSTANCE_API u32 g_yarmico_collision_test_z;
INSTANCE_API u32 g_yarmico_collision_test_zd;

#define  yarmico_setup_collision_test(_obj, _rect)  g_yarmico_collision_test_item_ptr= &_obj; g_yarmico_collision_test_z=_rect.z;  g_yarmico_collision_test_zd=_rect.z+_rect.d ;g_yarmico_collision_test_x= _rect.x; g_yarmico_collision_test_y= _rect.y; g_yarmico_collision_test_xw= _rect.x+_rect.w; g_yarmico_collision_test_yh=_rect.y+_rect.h;
#else
#define  yarmico_setup_collision_test(_obj, _rect)  g_yarmico_collision_test_item_ptr= &_obj; g_yarmico_collision_test_x= _rect.x; g_yarmico_collision_test_y= _rect.y; g_yarmico_collision_test_xw= _rect.x+_rect.w; g_yarmico_collision_test_yh=_rect.y+_rect.h;

#endif



// 2D Game Collision Detection by Thomas Schwarzl

#define YARMICO_OVERLAP_TEST(_minA, _maxA, _minB, _maxB) ( _minB < _maxA && _minA < _maxB )

#define LOG_COLLISION  // printf
static inline u32  yarmico_rect_collision_test(yarmico_rect_t *b)
{

	s32 bLeft = b->x;
	s32 bRight = bLeft + b->w;
	s32 bTop = b->y;
	s32 bBottom = bTop + b->h;
	s32 ret;



#if YARMICO_MATH_COORDS == YARMICO_MATH_COORDS_3D



	ret =	YARMICO_OVERLAP_TEST(g_yarmico_collision_test_x, g_yarmico_collision_test_xw, bLeft, bRight)
							&&
							YARMICO_OVERLAP_TEST(g_yarmico_collision_test_y, g_yarmico_collision_test_yh, bBottom, bTop);

	if(ret) // if 2D collides, check 3D Z and depth
	{
		u32 bZ = b->z;
		u32 bDepth = bZ + b->d;

		ret =	YARMICO_OVERLAP_TEST(g_yarmico_collision_test_z, g_yarmico_collision_test_zd, bZ, bDepth)
	}


#else

	if(bTop < 0)
	{
		LOG_COLLISION("if(bTop < 0) %d \n", bTop);
		return 1;
	}

	if(bBottom > YARMICO_HEIGHT)
	{
		LOG_COLLISION("if(bBottom > YARMICO_HEIGHT) %d \n", bBottom);
		return 1;
	}

	if(bRight > YARMICO_WIDTH)
	{
		LOG_COLLISION("if(bRight > YARMICO_WIDTH)%d \n", bRight);
		return 1;
	}
	if(bLeft < 0)
	{
		LOG_COLLISION("if(bLeft < 0)%d \n", bLeft);
		return 1;
	}

	ret =	YARMICO_OVERLAP_TEST(g_yarmico_collision_test_x, g_yarmico_collision_test_xw, bLeft, bRight)
							&&
							YARMICO_OVERLAP_TEST(g_yarmico_collision_test_y, g_yarmico_collision_test_yh,  bTop,bBottom);
#endif

	LOG_COLLISION("return ret; %d \n", ret);
	return ret<<2;

}



#endif //#ifndef YARMICO_API


