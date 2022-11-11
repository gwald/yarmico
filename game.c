/*
 *
 PUBLIC DOMAIN 2022
 THIS CODE IS A HACKY MESS - READ IT AT YOUR OWN RISK!
 https://github.com/gwald/yarmico
 *
 */

#define YARMICO_INSTANCE_GAME

#include "game.h"

#define LOG_MAIN // printf

#define START_ALIEN_Y 32
#define ALIEN_TEXTURES_PER_ROWS 16 // * 8pixels
#define ALIEN_TEXTURE_ROWS 16

// sin/cos from https://github.com/sehugg/8bitworkshop/blob/master/presets/vector-z80color/game.c#L286
void explosion_add(u32 x, u32 y , u32 side);
void gameloop_info_screen(void);

const s8 sintbl[64] = {
		0, 3, 6, 9, 12, 16, 19, 22, 25, 28, 31, 34, 37, 40, 43, 46,
		49, 51, 54, 57, 60, 63, 65, 68, 71, 73, 76, 78, 81, 83, 85, 88,
		90, 92, 94, 96, 98, 100, 102, 104, 106, 107, 109, 111, 112, 113, 115, 116,
		117, 118, 120, 121, 122, 122, 123, 124, 125, 125, 126, 126, 126, 127, 127, 127,
};

s8 isin(u8 x0) {
	u8 x = x0;
	if (x0 & 0x40) x = 127-x;
	if (x0 & 0x80) {
		return -sintbl[x-128];
	} else {
		return sintbl[x];
	}
}

s8 icos(u8 x) {
	return isin(x+64);
}


void display_UI(void)
{
	char line[25]; //14 chars + NUL

	if( g_player.alive < 0)
		return;

	sprintf(line, "%03d %1d %09d",g_level_disp, g_player.alive, g_score );
	yarmico_rectangle(0,0,0, 0, 0, YARMICO_WIDTH, 8);

	yarmico_render_text(0,0, ONE, line);
}

void move_header(void)
{
#if 1
	//if( (g_game_frame_counter & 31 ) == (0) )
	if( (g_game_frame_counter & (255>>g_level) ) == (0) )

	{
		s32 sin = (isin(64-(g_head_alien.sinX>>2)))*g_level*20;
		//  (g_head_alien.sinX>>2) = limits to 0-63, as per table, making sinX a fixed point value
		//80- because half is will be used for left(-) and the other for right(+)

		if(g_head_alien.dir)
			g_head_alien.posX_fixed += sin;
		else
			g_head_alien.posX_fixed -= sin;

		//	if(g_head_alien.posX_fixed > 120 << ONE_SHIFT)
		//		g_head_alien.posX_fixed = 120 >> ONE_SHIFT;

		g_head_alien.sinX+=g_level;

		if(g_head_alien.sinX>249)
		{
			g_head_alien.pos_rect.y++;
			//g_head_alien.pos_rect.x--;
			g_head_alien.dir = !g_head_alien.dir;
			g_head_alien.sinX=0;

		}

		//		LOG_MAIN("g_head_alien.sinX %d sin %d \n", g_head_alien.sinX, sin);


	}
#endif
	//g_head_alien.posX_fixed += isin( (g_head_alien.w_speed_fixed++)>>ONE_SHIFT  );
	//g_head_alien.posY_fixed += isin( (g_head_alien.h_speed_fixed++)>>ONE_SHIFT  );
}
u32 alien_bullet_add(s32 x, s32 y )
{
	u32 i;
	u32 test;


	for(i=0; i<MAX_BULLETS; i++)
	{
		test = g_aliens_bullets[i].alive;
		if(test == 0)
		{


			LOG_BULLET("bullet_add i %d X %d Y %d \n",i , x, y);

			// bzero(g_bullets+i, sizeof(bullet_t) );
			// g_bullets[b].speed =  (BULLET_SPEED>>4) + g_level + g_wave;
			g_aliens_bullets[i].alive = 1;


			g_aliens_bullets[i].pos_rect.x = x;
			g_aliens_bullets[i].pos_rect.y = y;
			g_aliens_bullets[i].pos_rect.w = 2;
			g_aliens_bullets[i].pos_rect.h = 2;


			// audio
			g_yarmico_pico_play_sound.SFX =  YARMICO_PICO_SOUND_SFX2_ALT_SIGN;// | YARMICO_PICO_SOUND_SFX1_ON_OFF;
			g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
			g_yarmico_pico_play_sound.SFX_trigger_ON_OFF = 1;
			g_yarmico_pico_play_sound.SFX_hold_ON_OFF = 1; // 5 frame skips

			g_yarmico_pico_play_sound.SFX_trigger2 = 1;
			g_yarmico_pico_play_sound.SFX_hold2 = 2; // 5 frame skips
			g_yarmico_pico_play_sound.dir_speed = -50;


			g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_TIMES;
			g_yarmico_pico_play_sound.loop_times = 3;

			YARMICO_MACRO_SET_PICO_AUDIO(665, 100, 100, 0, 0 , 5);


			return 1;
		}
	}
	return 0;
}


u32 player_bullet_add(s32 x, s32 y )
{
	u32 i;
	u32 test;


	for(i=0; i<MAX_BULLETS; i++)
	{
		test = g_player_bullets[i].alive;
		if(test == 0)
		{


			LOG_BULLET("bullet_add i %d X %d Y %d \n",i , x, y);

			// bzero(g_bullets+i, sizeof(bullet_t) );
			// g_bullets[b].speed =  (BULLET_SPEED>>4) + g_level + g_wave;
			g_player_bullets[i].alive = 1;


			g_player_bullets[i].pos_rect.x = x;
			g_player_bullets[i].pos_rect.y = y;
			g_player_bullets[i].pos_rect.w = 2;
			g_player_bullets[i].pos_rect.h = 2;

			// audio
			g_yarmico_pico_play_sound.SFX =  YARMICO_PICO_SOUND_SFX2_ALT_SIGN;// | YARMICO_PICO_SOUND_SFX1_ON_OFF;
			g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
			g_yarmico_pico_play_sound.SFX_trigger_ON_OFF = 1;
			g_yarmico_pico_play_sound.SFX_hold_ON_OFF = 1; // 5 frame skips

			g_yarmico_pico_play_sound.SFX_trigger2 = 1;
			g_yarmico_pico_play_sound.SFX_hold2 = 2; // 5 frame skips
			g_yarmico_pico_play_sound.dir_speed = 50;


			g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_TIMES;
			g_yarmico_pico_play_sound.loop_times = 3;

			YARMICO_MACRO_SET_PICO_AUDIO(665, 100, 100, 0, 0 , 5);


			return 1;
		}
	}
	return 0;
}

void bullet_display(void)
{
	u32 test, i;
	u8 r, g, b;
	yarmico_rect_t pos_rect;
	u32 hit;



	r = 255;
	g = 0;
	b = 0;

	// player hit test on aliens bullets

	for(i=0; i<MAX_BULLETS; i++)
	{
		test = g_aliens_bullets[i].alive;
		if(test)
		{
			// g_bullets[i].alive--;



			g_aliens_bullets[i].dy += (BULLET_INIT_SPEED + g_level *20); // g_bullets[i].speed;
			// g_bullets[i].pos_rect.y += g_bullets[i].dy >> ONE_SHIFT;


			pos_rect.w = pos_rect.h = 2;//g_aliens_bullets[i].pos_rect.w;
			pos_rect.x = g_aliens_bullets[i].pos_rect.x ;
			// pos_rect.y = g_bullets[i].pos_rect.y ;
			pos_rect.y = g_aliens_bullets[i].pos_rect.y + (g_aliens_bullets[i].dy >> ONE_SHIFT);


			hit =  yarmico_rect_collision_test(&pos_rect);

			if(hit)
			{
				g_aliens_bullets[i].dy=g_aliens_bullets[i].alive=0;

				LOG_BULLET("bullet_display hit %d \n", hit );

			}


			if(hit > 1 ) // player hit greater then 1
			{
				//hit player

				if(g_player.hit_cnt == 0)
				{
					explosion_add( pos_rect.x , pos_rect.y , 1);
					g_player.alive--;
					g_player.hit_cnt = 100;
					LOG_BULLET("bullet_display dead i:%d X:%d Y:%d X2:%d Y2:%d %d %d \n", i , g_player_bullets[i].pos_rect.x, g_player_bullets[i].pos_rect.y,  pos_rect.x, pos_rect.y, g_player_bullets[i].dy, (g_player_bullets[i].dy >> ONE_SHIFT));
				}

			}
			else
			{

				yarmico_rectangle( r,g, b, pos_rect.x, pos_rect.y, pos_rect.w , pos_rect.h );
			}

			LOG_BULLET("bullet_display  i %d X %d Y %d Y2 %d \n", i , g_aliens_bullets[i].pos_rect.x, g_aliens_bullets[i].pos_rect.y, pos_rect.y);


		}


	}




	r = 0;
	g = 255;
	b = 0;

	//g_player_bullets

	for(i=0; i<MAX_BULLETS; i++)
	{
		test = g_player_bullets[i].alive;
		if(test)
		{
			// g_bullets[i].alive--;



			g_player_bullets[i].dy += (BULLET_INIT_SPEED + g_level *20); // g_bullets[i].speed;
			// g_bullets[i].pos_rect.y += g_bullets[i].dy >> ONE_SHIFT;

			g_yarmico_collision_test_x =  g_player_bullets[i].pos_rect.x ;
			g_yarmico_collision_test_xw = g_yarmico_collision_test_x + 2;


			g_yarmico_collision_test_y =  g_player_bullets[i].pos_rect.y - (g_player_bullets[i].dy >> ONE_SHIFT); // shoot up
			g_yarmico_collision_test_yh = g_yarmico_collision_test_y + 2;

			//display bullet
			yarmico_rectangle( r,g, b, g_yarmico_collision_test_x, g_yarmico_collision_test_y, 2 ,2 );
			LOG_COLLISION("\n Bull %d x %d y %d  \n",i, g_yarmico_collision_test_x, g_yarmico_collision_test_y );

			// bullet out of bound destoryed at display not here

			hit=0;

			if(g_yarmico_collision_test_y < 0)
			{
				LOG_COLLISION("if(bTop > YARMICO_HEIGHT) %d \n", g_yarmico_collision_test_y);
				hit=1;
			}

			if(g_yarmico_collision_test_yh > YARMICO_HEIGHT)
			{
				LOG_COLLISION("if(bBottom > YARMICO_HEIGHT) %d \n", g_yarmico_collision_test_yh);
				hit=1;
			}

			if(g_yarmico_collision_test_xw > YARMICO_WIDTH)
			{
				LOG_COLLISION("if(bRight > YARMICO_WIDTH)%d \n", g_yarmico_collision_test_xw);
				hit=1;
			}
			if(g_yarmico_collision_test_x < 0)
			{
				LOG_COLLISION("if(bLeft < 0)%d \n", g_yarmico_collision_test_x);
				hit=1;
			}


			if(hit)
			{
				g_player_bullets[i].pos_rect.y = g_player_bullets[i].pos_rect.x = g_player_bullets[i].dy=g_player_bullets[i].alive = 0;
				LOG_BULLET("bullet_display if(hit) %d \n", hit );
				goto DO_NEW_BULLET; // bullet used up - break out of current loop and continue with the bullet loop


			}
			else //check each alien for current bullet collision
			{

				s32 aliens, x, y, wave;

				LOG_BULLET("bullet_display if(hit) else %d \n", hit );

				wave = g_head_alien.total;
				LOG_BULLET("\n");

				for(aliens=0; aliens<wave;aliens++)
				{
					test = g_aliens[aliens].alive;
					if(test)
					{

						x = (g_head_alien.posX_fixed>>ONE_SHIFT) + g_head_alien.pos_rect.x  + g_aliens[aliens].item.pos_rect.x;
						y = (g_head_alien.posY_fixed>>ONE_SHIFT) + g_head_alien.pos_rect.y + g_aliens[aliens].item.pos_rect.y;
						//yarmico_rectangle( r,g, b, g_bullets[i].pos_rect.x, g_bullets[i].pos_rect.y, g_bullets[i].pos_rect.w, g_bullets[i].pos_rect.h );


						if(y < 0 )
							continue;


						pos_rect.w = pos_rect.h = 8;//g_aliens_bullets[i].pos_rect.w;
						pos_rect.x = x;
						// pos_rect.y = g_bullets[i].pos_rect.y ;
						pos_rect.y = y;



						hit =  yarmico_rect_collision_test(&pos_rect);

						LOG_BULLET("%d x %d y %d  hit %d \n",aliens, pos_rect.x, pos_rect.y, hit );


						if(hit>1)
						{

							g_aliens[aliens].alive--;

							if(g_aliens[aliens].alive == 0)
							{
								g_player_bullets[i].pos_rect.y = g_player_bullets[i].pos_rect.x = g_player_bullets[i].dy=g_player_bullets[i].alive = 0;
								g_aliens[aliens].item.dx = g_aliens[aliens].item.dy =  0;

								g_head_alien.total_remaining--;
								explosion_add( x , y , 0);
								g_score++;
								goto DO_NEW_BULLET; // bullet used up - break out of current loop and continue with the bullet loop
							}
							else
							{
								g_score++;
								yarmico_rectangle(255,255,255,  x-8,y-8 , 16, 16 );
								g_player_bullets[i].pos_rect.y = g_player_bullets[i].pos_rect.x = g_player_bullets[i].dy=g_player_bullets[i].alive = 0;
								goto DO_NEW_BULLET; // bullet used up - break out of current loop and continue with the bullet loop

							}

							LOG_BULLET("bullet_display hit %d \n", hit );
							//yarmico_rectangle( 00,0, 255, pos_rect.x, pos_rect.y, pos_rect.w , pos_rect.h);

						}
						//	else
						//	yarmico_rectangle( 255,255, 255, pos_rect.x, pos_rect.y, pos_rect.w , pos_rect.h);

					}


				}
			} //check each alien for current bullet collision

			LOG_BULLET("\n");


			DO_NEW_BULLET: // break out of current loop and continue with the bullet loop

			LOG_BULLET("bullet_display  i %d X %d Y %d Y2 %d \n", i , g_player_bullets[i].pos_rect.x, g_player_bullets[i].pos_rect.y, pos_rect.y);


		}


	}

}



void game_screen_clear(void)
{
	u32 test, i;
	// player hit test on aliens bullets

	for(i=0; i<MAX_BULLETS; i++)
	{
		g_aliens_bullets[i].dy=g_aliens_bullets[i].alive=0;
		g_player_bullets[i].dy=g_player_bullets[i].alive=0;
	}

	for(i=0; i<MAX_EXPLOSIONS; i++)
		g_explosions[i].alive = 0;

	for(i=0; i<MAX_EXPLOSIONS; i++)
		g_aliens[i].alive=0;


	g_head_alien.total_remaining =	g_head_alien.total=0;

	yarmico_stop_all_SFX();
}




void explosion_add(u32 x, u32 y, u32 side )
{
	u32 i;

	LOG_MAIN("explosion_add   X:%d Y:%d \n", x, y );

	for(i=0; i<MAX_EXPLOSIONS; i++)
	{

		if(g_explosions[i].alive == 0)
		{

			g_explosions[i].alive = 222;
			g_explosions[i].pos_rect.x = x;
			g_explosions[i].pos_rect.y = y;
			g_explosions[i].pos_rect.w = g_explosions[i].pos_rect.h = 4;


			if(side)
			{
				// audio
				g_yarmico_pico_play_sound.SFX = YARMICO_PICO_SOUND_SFX1_ON_OFF | YARMICO_PICO_SOUND_SFX2_UP_DOWN;
				g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
				g_yarmico_pico_play_sound.SFX_trigger_ON_OFF = 5;
				g_yarmico_pico_play_sound.SFX_hold_ON_OFF = 5; // 5 frame skips

				g_yarmico_pico_play_sound.SFX_trigger2 = 10;
				g_yarmico_pico_play_sound.SFX_hold2 = 20; // 5 frame skips
				g_yarmico_pico_play_sound.dir_speed = -100;


				g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_TIMES;
				g_yarmico_pico_play_sound.loop_times = 2;

				YARMICO_MACRO_SET_PICO_AUDIO(1665, 100, 100, 0, 0 , 15);
			}
			else
			{
				// audio
				g_yarmico_pico_play_sound.SFX = YARMICO_PICO_SOUND_SFX1_ON_OFF | YARMICO_PICO_SOUND_SFX2_UP_DOWN;
				g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
				g_yarmico_pico_play_sound.SFX_trigger_ON_OFF = 5;
				g_yarmico_pico_play_sound.SFX_hold_ON_OFF = 5; // 5 frame skips

				g_yarmico_pico_play_sound.SFX_trigger2 = 10;
				g_yarmico_pico_play_sound.SFX_hold2 = 20; // 5 frame skips
				g_yarmico_pico_play_sound.dir_speed = 100;


				g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_TIMES;
				g_yarmico_pico_play_sound.loop_times = 2;

				YARMICO_MACRO_SET_PICO_AUDIO(1665, 100, 100, 0, 0 , 15);
			}
			return;
		}
	}


}

void explosion_display(void)
{

	u32 i;
	u8 r, g, b;



	for(i=0; i<MAX_EXPLOSIONS; i++)
	{

		if(g_explosions[i].alive)
		{
			LOG_EXPL("g_explosions[i].alive %d \n ", g_explosions[i].alive );
			g_explosions[i].alive-=3;

			r= g_explosions[i].alive;
			g= g_explosions[i].alive;
			b= g_explosions[i].alive;

			if(  (g_explosions[i].alive   & 1) == 0)
			{
				g_explosions[i].pos_rect.w = g_explosions[i].pos_rect.h =  g_explosions[i].alive >> 4;
				r=255;
				g+=50;
				b-=50;

			}
			else
			{
				g_explosions[i].pos_rect.w = g_explosions[i].pos_rect.h =  g_explosions[i].alive >> 3;
				r=255;
				g-=50;
				b+=50;
				// g_explosions[i].pos_rect.x+=1;
				// g_explosions[i].pos_rect.y+=1;
			}


			yarmico_rectangle(r,g,b, g_explosions[i].pos_rect.x-8, g_explosions[i].pos_rect.y-8, g_explosions[i].pos_rect.w, g_explosions[i].pos_rect.h );

		}


	}
}



u32 game_save(void)
{
	save_t data={1};
	sprintf(data.data.start, DATA_START_CHECK);
	data.data.player = g_player;
	data.data.level = g_level;
	data.data.alien_width = g_alien_width;
	data.data.alien_rows = g_alien_rows;
	data.data.score = g_score;
	data.data.level_disp = g_level_disp;
	sprintf(data.data.end, DATA_END_CHECK);
	yarmico_save_data( (char *) &data, sizeof(save_t));
	return sizeof(data);

}

u32 game_load(void)
{
	save_t data={0};
	u32 ret;
	ret = yarmico_load_data( (char *) &data, sizeof(save_t));

	if(ret)
	{
		g_player = data.data.player;
		g_level = data.data.level;
		g_alien_width = data.data.alien_width;
		g_alien_rows = data.data.alien_rows;
		g_score = data.data.score;
		g_level_disp = data.data.level_disp;

		LOG_MAIN("%s;\n",data.data.start);
		LOG_MAIN("%s;\n",data.data.end);

		LOG_MAIN("%d;\n",g_level);
		LOG_MAIN("%d;\n",g_alien_width);
		LOG_MAIN("%d;\n",g_alien_rows);
		LOG_MAIN("%d;\n",g_score);
		LOG_MAIN("%d;\n",g_level_disp);
		LOG_MAIN("%d;\n",g_player.alive);
		LOG_MAIN("%d;\n",g_player.posX_fixed);

#if 0
		if( strcmp(data.data.start, DATA_START_CHECK) )
			return 0;

		if(  strcmp(data.data.end, DATA_END_CHECK)  )
			return 0;
#endif

	}

	return ret;

}

void init_level( u8 width, u8 height, u8 w_spacing, u8 h_spacing)
{
	u32 h,w,c;

	if(height==0)
		height=1;

	if(width==0)
		width=1;

	LOG_LEVEL("level\n");
	LOG_LEVEL("level width %d\n",width);
	LOG_LEVEL("level height %d\n",height);

	LOG_LEVEL("level w_spacing %d\n",w_spacing);
	LOG_LEVEL("level h_spacing %d\n",h_spacing);
	g_level_disp++;
	c =0;

	g_head_alien.width = width;
	g_head_alien.height = height;

	g_head_alien.total_remaining = g_head_alien.total = (height*width);

	LOG_MAIN("init_level total: %d \n", g_head_alien.total);

	for(h=0; h<height; h++)
		for(w=0; w<width; w++)
		{
			g_aliens[c].alive = h+1;
			g_aliens[c].speed = ALIEN_SPEED;

			LOG_LEVEL("level alien %d\n",c);
			LOG_LEVEL("level g_aliens[i].alive  %d \n", g_aliens[c].alive );
			LOG_LEVEL("level g_aliens[i].speed  %d \n", g_aliens[c].speed );

			LOG_LEVEL("level height i %d\n",h);
			LOG_LEVEL("level width j %d\n",w);



			LOG_LEVEL("level  right i&1 == 0 \n");
			g_aliens[c].item.pos_rect.x = (8*w) * w_spacing; // right


			if( (h&1) ==0)
				; //g_aliens[c].item.pos_rect.x += (8*w_spacing);
			else
				g_aliens[c].item.pos_rect.x -= (8*w_spacing)>>1;


			g_aliens[c].item.pos_rect.y =   (-8*h) * h_spacing; // up


			g_aliens[c].shot_tick = rand()&255;

			g_aliens[c].item.pos_rect.w = g_aliens[c].item.pos_rect.h = 8;


			LOG_LEVEL("level g_aliens[i].item.pos_rect.x  %d \n", g_aliens[c].item.pos_rect.x  );
			LOG_LEVEL("level g_aliens[i].item.pos_rect.y  %d \n", g_aliens[c].item.pos_rect.y  );

			g_aliens[c].item.tpage = 0;
			g_aliens[c].item.u =  8*(c%ALIEN_TEXTURES_PER_ROWS); // + (i>>4)*8
			g_aliens[c].item.v = START_ALIEN_Y +  ( (c/ALIEN_TEXTURES_PER_ROWS)%ALIEN_TEXTURE_ROWS)*8; //8*(i%8)+


			g_aliens[c].item.dx = g_aliens[c].item.dy = 5;
			c++;
		}

	// c--;

	g_head_alien.shoot_cnt=c+1;// set to last, then loops
	g_head_alien.width= g_aliens[c].item.pos_rect.w +8;
	g_head_alien.height= g_aliens[c].item.pos_rect.h +8;

	g_head_alien.pos_rect.x = g_head_alien.pos_rect.y = 0;
	g_head_alien.pos_rect.x -= 8 * ( width )+ (w_spacing*width>>3); // move head left half with
	g_head_alien.pos_rect.x += 4*w_spacing; // center
	g_head_alien.pos_rect.x += YARMICO_WIDTH>>1; // move head to screen center
	g_head_alien.pos_rect.x += 4;
	g_head_alien.pos_rect.y = 0; //hide offscreen

	///g_head_alien.posX_fixed= (1+g_level)*10;
	g_head_alien.posY_fixed= (g_level)*8;




}





void alien_display(void)
{
	u32 test, i;
	s32 x, y;
	u8 wave, r, g, b;



	LOG_ALIEN("alien_display\n");

	r = 255;
	g = 126;
	b = 0;

	wave = g_head_alien.total;

#if HARDWARE_TARGET == HARDWARE_TARGET_PICOSYSTEM
	if( (g_game_frame_counter & 15 ) ==  0 ) //(31- (g_head_alien.shoot_cnt) ) )
#else
		if( (g_game_frame_counter & 127 ) == (0) )
#endif
		{
			g_head_alien.shoot_cnt++;

			if(g_head_alien.shoot_cnt > wave)
				g_head_alien.shoot_cnt = 0;

			LOG_ALIEN2("alien_display flip %d\n", g_head_alien.shoot_cnt);
		}

	for(i=0; i<wave; i++)
	{
		test = g_aliens[i].alive;
		if(test)
		{

			x = (g_head_alien.posX_fixed>>ONE_SHIFT)+ g_head_alien.pos_rect.x  + g_aliens[i].item.pos_rect.x;
			y = (g_head_alien.posY_fixed>>ONE_SHIFT) + g_head_alien.pos_rect.y + g_aliens[i].item.pos_rect.y;
			//yarmico_rectangle( r,g, b, g_bullets[i].pos_rect.x, g_bullets[i].pos_rect.y, g_bullets[i].pos_rect.w, g_bullets[i].pos_rect.h );
			yarmico_sprite(0,  g_aliens[i].item.u, g_aliens[i].item.v, 8, 8, x,  y, ONE, i==g_head_alien.shoot_cnt, 0);


			if(y > YARMICO_HEIGHT)
				{
					g_player.alive = -1;// alien passed player, game over
					return;
				}

			if(i==g_head_alien.shoot_cnt )
			{
				if(g_aliens[i].shot_tick)
				{
					g_aliens[i].shot_tick--;
				}
				else
				{
					g_aliens[i].item.pos_rect.y+=10;

					LOG_BULLET(" (g_head_alien.posX_fixed>>ONE_SHIFT) %d  g_head_alien.pos_rect.x  %d  g_aliens[i].item.pos_rect.x  %d  \n", (g_head_alien.posX_fixed>>ONE_SHIFT) , g_head_alien.pos_rect.x  , g_aliens[i].item.pos_rect.x);
					LOG_BULLET(" x %d y %d \n", x, y);

					if(y < 8)
						y = 8;

					alien_bullet_add(x+3,y+18); // center bottom
					g_head_alien.shoot_cnt++; // next
					// g_head_alien.shoot_cnt =255;

					g_aliens[i].shot_tick= i*(GAME_MAX_LEVEL-g_level); //g_head_alien.total;

					if(i == g_head_alien.total-1)
					{
						g_head_alien.posY_fixed +=g_level*10;

						if(g_head_alien.posY_fixed > 120 << ONE_SHIFT)
							g_head_alien.posY_fixed = 120 >> ONE_SHIFT;
					}


				}

			}


		}


	}

}



void game_update(void)
{
	g_gameloop_function();
}


void gameloop_game(void)
{

	if(g_head_alien.total_remaining==0)
	{

		if(g_game_frame_counter > 32)
		{
			g_game_frame_counter=32;
			game_screen_clear();
			g_yarmico_controller_action_buttons = 0;
			g_button_press_time = 8;


			//	yarmico_play_pico_piezo(1320, 100 );
			g_yarmico_pico_play_sound.SFX = YARMICO_PICO_SOUND_SFX1_ON_OFF | YARMICO_PICO_SOUND_SFX2_ALT_SIGN;
			g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
			g_yarmico_pico_play_sound.SFX_trigger_ON_OFF = 1;
			g_yarmico_pico_play_sound.SFX_hold_ON_OFF = 1; // 5 frame skips

			g_yarmico_pico_play_sound.SFX_trigger2 = 1;
			g_yarmico_pico_play_sound.SFX_hold2 = 2; // 5 frame skips
			g_yarmico_pico_play_sound.dir_speed = 100;

			g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_TIMES;
			g_yarmico_pico_play_sound.loop_times = 5;

			YARMICO_MACRO_SET_PICO_AUDIO(130, 100, 100, 0, 0 ,15 );
		}

		g_game_frame_counter--;

		if(g_game_frame_counter==0)
		{
			g_alien_width++;

			if(g_alien_width>6)
			{

				g_alien_rows++;
				g_alien_width=g_level;
				g_level++;
				g_player.alive++;


			}

			memset(&g_head_alien, 0, sizeof(head_t) );
			g_player.posX_fixed = ((YARMICO_WIDTH>>1)-4) <<ONE_SHIFT;
			g_head_alien.posX_fixed =  (g_level*8) <<ONE_SHIFT;


			game_screen_clear();
			game_save();
			init_level(g_alien_width, g_alien_rows,  2, 1);
		}
		else
		{

			yarmico_do_C64_loading_screen();

			yarmico_rectangle(0,0,0, 0, YARMICO_WIDTH>>1, YARMICO_WIDTH, 8);
			yarmico_render_text(8,YARMICO_WIDTH>>1, ONE, "WAVE CLEARED!");


			// audio
			g_yarmico_pico_play_sound.SFX = YARMICO_PICO_SOUND_SFX1_ON_OFF | YARMICO_PICO_SOUND_SFX2_ALT_SIGN;
			g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
			g_yarmico_pico_play_sound.SFX_trigger_ON_OFF = 10;
			g_yarmico_pico_play_sound.SFX_hold_ON_OFF = 2; // 5 frame skips

			g_yarmico_pico_play_sound.SFX_trigger2 = 11;
			g_yarmico_pico_play_sound.SFX_hold2 = 15; // 5 frame skips
			g_yarmico_pico_play_sound.dir_speed = -10;


			g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_TIMES;
			g_yarmico_pico_play_sound.loop_times = 2;

			YARMICO_MACRO_SET_PICO_AUDIO(665, 100, 100, 0, 0 , 50);





		}


	}
	else
	{

		//	explosion_display();

		//	yarmico_setup_collision_test(big_test, big_test.pos_rect);


		//	yarmico_update_time();

		//LOG_MAIN("update_game \n");



		//if(g_gamestate.time%2==0)
		//	yarmico_play_SFX(0);
		if(g_player.alive< 0)
		{
			yarmico_do_C64_loading_screen();
			display_UI();
			yarmico_rectangle(0,0,0, 0, YARMICO_WIDTH>>1, YARMICO_WIDTH, 8);
			yarmico_render_text(8*3, YARMICO_WIDTH>>1, ONE, "U DED! :'(");

			// audio
			if( (g_game_frame_counter&1) ==0)
			{

				// audio
				g_yarmico_pico_play_sound.SFX =  YARMICO_PICO_SOUND_SFX2_ALT_SIGN | YARMICO_PICO_SOUND_SFX1_ON_OFF;
				g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
				g_yarmico_pico_play_sound.SFX_trigger_ON_OFF = 3;
				g_yarmico_pico_play_sound.SFX_hold_ON_OFF = 3; // 5 frame skips

				g_yarmico_pico_play_sound.SFX_trigger2 = 3;
				g_yarmico_pico_play_sound.SFX_hold2 = 6; // 5 frame skips
				g_yarmico_pico_play_sound.dir_speed = -22;


				g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_TIMES;
				g_yarmico_pico_play_sound.loop_times = 12;

				YARMICO_MACRO_SET_PICO_AUDIO(665, 100, 100, 0, 0 , 150);
			}

			if(g_yarmico_controller_action_buttons )
			{
				g_yarmico_controller_action_buttons = 0;
				g_button_press_time = 132;
				game_screen_clear();
				g_gameloop_function = gameloop_info_screen;// go back to start screen loop
				yarmico_delete_data(0);
			}

			g_game_frame_counter++;
			return;
		}
		else // game play logic
		{


			explosion_display();
			alien_display();


			if(g_yarmico_controller_dpad & YARMICO_CONTROLER_BUT_L)
			{
				g_player.posX_fixed-=FIXED_MOVE_AMOUNT;
				g_player.direction=1;
			}
			else if(g_yarmico_controller_dpad & YARMICO_CONTROLER_BUT_R)
			{
				g_player.posX_fixed+=FIXED_MOVE_AMOUNT;
				g_player.direction=0;
			}


			if(g_yarmico_controller_dpad & YARMICO_CONTROLER_BUT_U)
				g_head_alien.posY_fixed+=FIXED_MOVE_AMOUNT;
			else if(g_yarmico_controller_dpad & YARMICO_CONTROLER_BUT_D)
				g_head_alien.posY_fixed+=FIXED_MOVE_AMOUNT;



			g_yarmico_collision_test_x =   (g_player.posX_fixed>>ONE_SHIFT);

			if(g_yarmico_collision_test_x < 1)
			{
				g_player.posX_fixed +=FIXED_MOVE_AMOUNT;
				g_yarmico_collision_test_x =   (g_player.posX_fixed>>ONE_SHIFT);

			}

			if(g_yarmico_collision_test_x > 112)
			{
				g_player.posX_fixed -=FIXED_MOVE_AMOUNT;
				g_yarmico_collision_test_x =   (g_player.posX_fixed>>ONE_SHIFT);

			}

			g_yarmico_collision_test_xw = g_yarmico_collision_test_x + 8;


			g_yarmico_collision_test_y = (YARMICO_HEIGHT)-8;
			g_yarmico_collision_test_yh = g_yarmico_collision_test_y + 8;



			if(g_button_press_time > 0)
				g_button_press_time--;
			else if(g_yarmico_controller_action_buttons )
			{
				if(player_bullet_add(g_yarmico_collision_test_x+4, g_yarmico_collision_test_y )) //shot good
					g_button_press_time = YARMICO_BUTTON_PRESS_TIME <<3;
			}




			if(g_player.hit_cnt)
			{
				g_player.hit_cnt--;
				yarmico_rectangle( g_game_frame_counter,g_game_frame_counter, g_game_frame_counter,g_yarmico_collision_test_x-2, g_yarmico_collision_test_y-2, 12 , 12 );
			}



			// LOG_MAIN("%d\n", g_button_press_time);


			// LOG_MAIN("g_yarmico_collision_test_x %d \n", g_yarmico_collision_test_x);
			yarmico_sprite(0,      64,64,      8,8,  g_yarmico_collision_test_x, g_yarmico_collision_test_y, ONE,g_player.direction , 0);

			bullet_display();


			move_header();
			display_UI();



		}
		g_game_frame_counter++;
	}




#if 0
	if(yarmico_rect_collision_test(&sprite) )
	{

		//	yarmico_play_pico_piezo(165, 100 );
		g_yarmico_pico_play_sound.SFX =  YARMICO_PICO_SOUND_SFX2_UP_DOWN;
		g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;

		g_yarmico_pico_play_sound.SFX_trigger2 = 1;
		g_yarmico_pico_play_sound.SFX_hold2 = 5; // 5 frame skips
		g_yarmico_pico_play_sound.dir_speed = 100;

		g_yarmico_pico_play_sound.time_frames = 10;

		YARMICO_MACRO_SET_PICO_AUDIO(665, 100, 100, 0, 0 );

		LOG_MAIN(" sprite.posX  %d ,sprite.posY   %d  -   big_test.posX %d ,big_test.posY %d \n", sprite.pos_rect.x,sprite.pos_rect.y, big_test.pos_rect.x,big_test.pos_rect.y );
		//g_game_controller_timer = BUTTON_PRESS_TIME;
	}

	// 	g_yarmico_pico_play_sound
	if(g_game_controller_timer)
		g_game_controller_timer--;
	else
	{
#if 1 //HARDWARE_TARGET ==  HARDWARE_TARGET_PICOSYSTEM
		if(g_yarmico_controller_action_buttons  & YARMICO_CONTROLER_BUT_D )
		{
			//	yarmico_play_pico_piezo(165, 100 );
			g_yarmico_pico_play_sound.SFX =  YARMICO_PICO_SOUND_SFX2_UP_DOWN;
			g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;

			g_yarmico_pico_play_sound.SFX_trigger2 = 1;
			g_yarmico_pico_play_sound.SFX_hold2 = 5; // 5 frame skips
			g_yarmico_pico_play_sound.dir_speed = 100;

			g_yarmico_pico_play_sound.time_frames_repeat = g_yarmico_pico_play_sound.time_frames = 10;
			g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_TIMES;
			g_yarmico_pico_play_sound.loop_times = 5;

			YARMICO_MACRO_SET_PICO_AUDIO(1320, 100, 100, 0, 0 );

			g_game_controller_timer = BUTTON_PRESS_TIME;
		} else if(g_yarmico_controller_action_buttons  & YARMICO_CONTROLER_BUT_R )
		{
			//	yarmico_play_pico_piezo(330, 100 );
			g_yarmico_pico_play_sound.SFX = YARMICO_PICO_SOUND_SFX1_ON_OFF | YARMICO_PICO_SOUND_SFX2_ALT_SIGN;
			g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
			g_yarmico_pico_play_sound.SFX_trigger1 = 1;
			g_yarmico_pico_play_sound.SFX_hold1 = 1; // 5 frame skips

			g_yarmico_pico_play_sound.SFX_trigger2 = 1;
			g_yarmico_pico_play_sound.SFX_hold2 = 2; // 5 frame skips
			g_yarmico_pico_play_sound.dir_speed = 100;


			g_yarmico_pico_play_sound.time_frames_repeat = g_yarmico_pico_play_sound.time_frames = 15;
			g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_TIMES;
			g_yarmico_pico_play_sound.loop_times = 5;

			YARMICO_MACRO_SET_PICO_AUDIO(330, 100, 100, 0, 0 );
			g_game_controller_timer = BUTTON_PRESS_TIME;
		}
		else if(g_yarmico_controller_action_buttons  & YARMICO_CONTROLER_BUT_U )
		{
			//	yarmico_play_pico_piezo(660, 100 );
			g_yarmico_pico_play_sound.SFX = YARMICO_PICO_SOUND_SFX1_ON_OFF | YARMICO_PICO_SOUND_SFX2_ALT_SIGN;
			g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
			g_yarmico_pico_play_sound.SFX_trigger1 = 1;
			g_yarmico_pico_play_sound.SFX_hold1 = 1; // 5 frame skips

			g_yarmico_pico_play_sound.SFX_trigger2 = 1;
			g_yarmico_pico_play_sound.SFX_hold2 = 2; // 5 frame skips
			g_yarmico_pico_play_sound.dir_speed = 100;

			g_yarmico_pico_play_sound.time_frames_repeat = g_yarmico_pico_play_sound.time_frames = 15;
			g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_REPEAT;
			g_yarmico_pico_play_sound.loop_times = 5;

			YARMICO_MACRO_SET_PICO_AUDIO(660, 100, 100, 0, 0 );
			g_game_controller_timer = BUTTON_PRESS_TIME;
		}else if(g_yarmico_controller_action_buttons  & YARMICO_CONTROLER_BUT_L )
		{

			//	yarmico_play_pico_piezo(1320, 100 );
			g_yarmico_pico_play_sound.SFX = YARMICO_PICO_SOUND_SFX1_ON_OFF | YARMICO_PICO_SOUND_SFX2_ALT_SIGN;
			g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
			g_yarmico_pico_play_sound.SFX_trigger1 = 1;
			g_yarmico_pico_play_sound.SFX_hold1 = 1; // 5 frame skips

			g_yarmico_pico_play_sound.SFX_trigger2 = 1;
			g_yarmico_pico_play_sound.SFX_hold2 = 2; // 5 frame skips
			g_yarmico_pico_play_sound.dir_speed = 100;

			g_yarmico_pico_play_sound.time_frames_repeat = g_yarmico_pico_play_sound.time_frames = 15;
			g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_TIMES;
			g_yarmico_pico_play_sound.loop_times = 5;

			YARMICO_MACRO_SET_PICO_AUDIO(130, 100, 100, 0, 0 );
			g_game_controller_timer = BUTTON_PRESS_TIME;
		}
#else

		if(g_yarmico_controller_action_buttons  & YARMICO_CONTROLER_BUT_D )
		{
			yarmico_play_SFX(0);
			g_game_controller_timer = BUTTON_PRESS_TIME;
		} else if(g_yarmico_controller_action_buttons  & YARMICO_CONTROLER_BUT_R )
		{
			yarmico_play_SFX(1);
			g_game_controller_timer = BUTTON_PRESS_TIME;
		}
		else if(g_yarmico_controller_action_buttons  & YARMICO_CONTROLER_BUT_U )
		{
			yarmico_play_SFX(2);
			g_game_controller_timer = BUTTON_PRESS_TIME;
		}else if(g_yarmico_controller_action_buttons  & YARMICO_CONTROLER_BUT_L )
		{

			yarmico_play_SFX(3);
			g_game_controller_timer = BUTTON_PRESS_TIME;
		}
#endif
	}
#endif


}


void gameloop_info_screen(void)
{
	// yarmico_do_C64_loading_screen();
	if( (g_yarmico_game_time & 1) == 0)
	{


		//yarmico_do_C64_loading_screen();

		yarmico_render_text(4,10, ONE, "YARMICO");
		yarmico_render_text(4,20, ONE, "PIXELART: S4M"); //https://s4m-ur4i.itch.io/
		yarmico_render_text(4,30, ONE, "CODING: GWALD");

		yarmico_render_text(4,60, ONE, "NETYAROZE JAM");
		yarmico_render_text(4,70, ONE, "GAMEJAM OCT 22");
		yarmico_render_text(4,90, ONE, "# PICOSYSTEM");
		yarmico_render_text(4,100, ONE, "WITH 32BLIT");



		// audio
		g_yarmico_pico_play_sound.SFX = YARMICO_PICO_SOUND_SFX1_ON_OFF | YARMICO_PICO_SOUND_SFX2_ALT_SIGN;
		g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
		g_yarmico_pico_play_sound.SFX_trigger_ON_OFF = 10;
		g_yarmico_pico_play_sound.SFX_hold_ON_OFF = 2; // 5 frame skips

		g_yarmico_pico_play_sound.SFX_trigger2 = 11;
		g_yarmico_pico_play_sound.SFX_hold2 = 15; // 5 frame skips
		g_yarmico_pico_play_sound.dir_speed = -10;


		g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_TIMES;
		g_yarmico_pico_play_sound.loop_times = 2;

		YARMICO_MACRO_SET_PICO_AUDIO(965, 100, 100, 0, 0 , 100);


	}
	else
	{

		// yarmico_do_C64_loading_screen();

		yarmico_render_text(4,10, ONE, "YARMICO");
		yarmico_render_text(4,40, ONE, "STOP DA THINGS");
		yarmico_render_text(4,50, ONE, "REACHIN DA BOT");

		yarmico_render_text(4,70, ONE, "GITHUB.COM/");
		yarmico_render_text(4,80, ONE, "GWALD/YARMICO");



		// audio
		g_yarmico_pico_play_sound.SFX = YARMICO_PICO_SOUND_SFX1_ON_OFF | YARMICO_PICO_SOUND_SFX2_ALT_SIGN;
		g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
		g_yarmico_pico_play_sound.SFX_trigger_ON_OFF = 5;
		g_yarmico_pico_play_sound.SFX_hold_ON_OFF = 10; // 5 frame skips

		g_yarmico_pico_play_sound.SFX_trigger2 = 10;
		g_yarmico_pico_play_sound.SFX_hold2 = 20; // 5 frame skips
		g_yarmico_pico_play_sound.dir_speed = -100;


		g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_TIMES;
		g_yarmico_pico_play_sound.loop_times = 2;

		YARMICO_MACRO_SET_PICO_AUDIO(665, 100, 100, 0, 0 , 50);



	}

	if(g_button_press_time)
		g_button_press_time--;
	else if(g_yarmico_controller_action_buttons)
	{

		game_screen_clear();

		g_gameloop_function = gameloop_game;
		g_game_frame_counter = g_game_controller_timer = 0;
		g_level=1;
		g_level_disp=g_score=0;
		g_yarmico_controller_action_buttons = 0;
		g_button_press_time = 32;
		g_player.alive=9;

		g_alien_width = 1;
		g_alien_rows = 1;

		g_player.posX_fixed = ((YARMICO_WIDTH>>1)-4) <<ONE_SHIFT;
		g_head_alien.posX_fixed =  (g_level*8) <<ONE_SHIFT;

		game_load(); // loaded something

		init_level(g_alien_width, g_alien_rows,  2, 1);

	}
	yarmico_update_time();


}


void game_deinit(void)
{
	// do nothing
}

void game_init(void) // game is responsible for deinit'ing and exiting
{


	g_button_press_time = YARMICO_BUTTON_PRESS_TIME <<2;

	g_gameloop_function = gameloop_info_screen;
	g_yarmico_game_time = g_game_controller_timer = 0;




#if 0


	g_yarmico_pico_play_sound.SFX = YARMICO_PICO_SOUND_SFX1_ON_OFF | YARMICO_PICO_SOUND_SFX2_UP_DOWN;
	g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;
	g_yarmico_pico_play_sound.SFX_hold1 = 15; // 5 frame skips
	g_yarmico_pico_play_sound.dir_speed = 1000;
	g_yarmico_pico_play_sound.SFX_trigger1 = 10;


	g_yarmico_pico_play_sound.SFX_trigger2 = 1;
	g_yarmico_pico_play_sound.SFX_hold2 = 20; // 5 frame skips
	g_yarmico_pico_play_sound.dir_speed = 1000;

	g_yarmico_pico_play_sound.time_frames_repeat = g_yarmico_pico_play_sound.time_frames = 15;
	g_yarmico_pico_play_sound.loop = YARMICO_PICO_SOUND_LOOP_REPEAT;
	g_yarmico_pico_play_sound.loop_times = 500;

	YARMICO_MACRO_SET_PICO_AUDIO(1730, 100, 100, 10, 0 );



#endif

}
