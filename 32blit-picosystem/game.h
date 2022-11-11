/*
 *
 PUBLIC DOMAIN 2022
 THIS CODE IS A HACKY MESS - READ IT AT YOUR OWN RISK!
 https://github.com/gwald/yarmico
 *
 */


#ifndef GAME_HEADER
#define GAME_HEADER

#include "yarmico_API.h"


#ifdef YARMICO_INSTANCE_GAME
#define INSTANCE
#else
#define INSTANCE extern
#endif


INSTANCE u32 g_game_controller_timer;
INSTANCE u32 g_game_frame_counter;

typedef struct {
	yarmico_item_t item;
	s8 alive;
	u8 hit_cnt;
	u8 speed;
	u8 shot_tick;
	s32 posX_fixed;
	u8 direction;
}player_t;

	// tick if( (g_game_counter & 127 ) == (127- (g_gamestate.wave) ) )//% 250 ==0)
typedef struct {
	yarmico_item_t item;
	s8 alive;
	u8 speed;
	u8 shot_tick;

	struct alien_t *next;
}alien_t;


typedef struct {
	yarmico_rect_t pos_rect;
	u8 alive;
	u32 dy;
//	u8 r,g,b;
//	u8 owner;
}bullet_t;


typedef struct {
	s32 posX_fixed;
	u32 posY_fixed;
	yarmico_rect_t pos_rect;
	u8 width;
	u8 height;
	u8 sinX;
	u8 dir;
	u8 pause;
//	u8 pause_tick;
	u8 shoot_cnt;
//	u32 w_speed_fixed;
//	u32 h_speed_fixed;
	u32 total;
	u32 total_remaining;

}head_t;

typedef struct {
	yarmico_rect_t pos_rect;
	u8 alive;
	u8 speed;
}explosion_t;



#if HARDWARE_TARGET == HARDWARE_TARGET_PICOSYSTEM

#define ALIEN_SPEED 10
#define BULLET_INIT_SPEED 9000
#define FIXED_MOVE_AMOUNT 1024*8
#define MAX_BULLETS 32

#elif HARDWARE_TARGET == HARDWARE_TARGET_NET_YAROZE
#define ALIEN_SPEED 10
#define BULLET_INIT_SPEED 4000
#define FIXED_MOVE_AMOUNT 1024*2
#define MAX_BULLETS 64

#else
#define ALIEN_SPEED 10
#define BULLET_INIT_SPEED 4000
#define FIXED_MOVE_AMOUNT 1024*2
#define MAX_BULLETS 64
#endif


#define MAX_ALIENS 256

#define MAX_EXPLOSIONS 16

#define DATA_START_CHECK "#sTaRtIng BIT 2001#"
#define DATA_END_CHECK   "#EnDiNg bIt 1999#"


#define GAME_MAX_LEVEL 10


typedef struct {
	char start[32];
	player_t player;
	u8 level;
	u8 level_disp;
	 u8 alien_width;
	 u8 alien_rows;
	 u32 score;
	 char end[32];
}save_data_t;



typedef union {
	save_data_t data;
	char block[128]; // playstation requires save to be 128bytes
}save_t;


INSTANCE player_t g_player;
INSTANCE head_t g_head_alien;
INSTANCE alien_t  g_aliens[MAX_ALIENS];
INSTANCE bullet_t  g_aliens_bullets[MAX_BULLETS];
INSTANCE bullet_t  g_player_bullets[MAX_BULLETS];
INSTANCE explosion_t  g_explosions[MAX_EXPLOSIONS];
INSTANCE u8 g_level;
INSTANCE u8 g_level_disp;
INSTANCE u8 g_alien_width;
INSTANCE u8 g_alien_rows;
INSTANCE u32 g_score;
INSTANCE u32 g_button_press_time;


//INSTANCE u8 g_wave;
// INSTANCE u32 g_move_down_fixed;

#define LOG_EXPL // printf
#define LOG_BULLET  //  printf
#define LOG_ALIEN2 // printf
#define LOG_ALIEN // printf
#define LOG_LEVEL // printf




#endif // #ifndef GAME_HEADER
