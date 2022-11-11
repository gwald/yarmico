/*
 *
 PUBLIC DOMAIN 2022
 THIS CODE IS A HACKY MESS - READ IT AT YOUR OWN RISK!
 https://github.com/gwald/yarmico
 *
 */


#ifdef __ANDROID__
// ANDROID uses CPP not C
extern "C" {
#endif


#define INSTANCE_YARMICO_API
#include "yarmico_API.h"
// #define printf //
// 12 rows X 20 cells = 240
// 240 / 8bits = 30bytes
// 10 10
// 9 2 9
// 8 4 8




#if YARMICO_PICO_SOUND == TRUE

void yarmico_play_pico_sound(void) // check time_frame before calling
{
	u32 turn_sound_off;
	u32 play_back = g_yarmico_pico_play_sound.play_back;
	//u16 freq = g_yarmico_pico_play_sound.frequency;

	turn_sound_off=0;

	g_yarmico_pico_play_sound.time_frames--;




	if(g_yarmico_pico_play_sound.time_frames == 0)
	{
		if(g_yarmico_pico_play_sound.loop != YARMICO_PICO_SOUND_LOOP_NONE)
		{

			g_yarmico_pico_play_sound.time_frames = g_yarmico_pico_play_sound.time_frames_repeat;

			if(g_yarmico_pico_play_sound.loop == YARMICO_PICO_SOUND_LOOP_REPEAT)
				; // do nothing

			else if(g_yarmico_pico_play_sound.loop == YARMICO_PICO_SOUND_LOOP_TIMES && g_yarmico_pico_play_sound.loop_times)
			{
				g_yarmico_pico_play_sound.loop_times--;

			}
			else
			{
				g_yarmico_pico_play_sound.loop =YARMICO_PICO_SOUND_LOOP_NONE;
				g_yarmico_pico_play_sound.time_frames = 0;
				turn_sound_off = 1;

			}



		}
	}
	else if(play_back == YARMICO_PICO_SOUND_PLAYBACK_OFF)
	{
		g_yarmico_pico_play_sound.time_frames = 0;
		turn_sound_off = 1;
		//	return yarmico_play_pico_sound_platform(turn_sound_off);
	}
	else if(g_yarmico_pico_play_sound.SFX)
	{


		if(g_yarmico_pico_play_sound.SFX & YARMICO_PICO_SOUND_SFX2_ALT_SIGN )
		{
			g_yarmico_pico_play_sound.SFX_counter2++;
			if(g_yarmico_pico_play_sound.SFX_counter2 > g_yarmico_pico_play_sound.SFX_trigger2)
			{
				s32 speed = g_yarmico_pico_play_sound.dir_speed;


				if(g_yarmico_pico_play_sound.time_frames&1)
					speed = -speed;

				g_yarmico_pico_play_sound.frequency+= speed;

				if(g_yarmico_pico_play_sound.SFX_counter2 > g_yarmico_pico_play_sound.SFX_hold2)
				{
					g_yarmico_pico_play_sound.SFX_counter2=0;
				}
			}
		}

		if(g_yarmico_pico_play_sound.SFX & YARMICO_PICO_SOUND_SFX2_UP_DOWN )
		{
			g_yarmico_pico_play_sound.SFX_counter2++;
			if(g_yarmico_pico_play_sound.SFX_counter2 > g_yarmico_pico_play_sound.SFX_trigger2)
			{
				g_yarmico_pico_play_sound.frequency+=g_yarmico_pico_play_sound.dir_speed;
				if(g_yarmico_pico_play_sound.SFX_counter2 > g_yarmico_pico_play_sound.SFX_hold2)
					g_yarmico_pico_play_sound.SFX_counter2=0;

			}

		}

		if(g_yarmico_pico_play_sound.SFX & YARMICO_PICO_SOUND_SFX1_ON_OFF )
		{
			g_yarmico_pico_play_sound.SFX_counter_ON_OFF++;

			if(g_yarmico_pico_play_sound.SFX_counter_ON_OFF > g_yarmico_pico_play_sound.SFX_trigger_ON_OFF)
			{
				if(g_yarmico_pico_play_sound.SFX_counter_ON_OFF > g_yarmico_pico_play_sound.SFX_hold_ON_OFF)
					g_yarmico_pico_play_sound.SFX_counter_ON_OFF=0;


				turn_sound_off = 1;

			}
		}




	}

	yarmico_play_pico_sound_platform(turn_sound_off);

}

#endif //#if YARMICO_PICO_SOUND==TRUE


// update time
void yarmico_update_time(void)
{
	u8 c  = g_yarmico_frame_counter>>YARMICO_UPDATE_TIME_SHIFT; // abit slower then normal minute, but better then too fast!
	u8 t = g_yarmico_game_time;

	c = c & 1;
	t = t & 1;

	//printf("c %d t %d test %d \n", c , t , c != t );

	if(c  != t ) // test for change
		g_yarmico_game_time++;

	g_yarmico_frame_counter++;


}



void intern_black_and_white_BG(u32 side)
{

	if(side)
	{
		yarmico_rectangle( 255, 255, 255,  160, 0, 320,240 );
		yarmico_rectangle(0, 0, 0,    0, 0, 160,240 );
	}
	else
	{

		yarmico_rectangle( 255, 255, 255,   0, 0, 160,240 );
		yarmico_rectangle(0, 0, 0,   160, 0, 320,240 );
	}
}



void yarmico_do_C64_loading_screen(void)
{
	u32 i;
	u32 h;
	u32 t;
	t = 16;
	h = YARMICO_HEIGHT/t;

	// C64 loading bards
	//for(i=0; i<24; i++)
	for(i=0; i<YARMICO_HEIGHT/h; i++)
	{

		char R, G, B;


		switch(rand()%16)
		//switch(i%16)
		{


		// white    0xff    0xff    0xff    2
		case 1:
			R = 0xa0;
			G = 0xa0;
			B = 0xa0;
			break;

			// red    0x88    0x00    0x00    3
		case 2:
			R = 0x88;
			G = 0x0;
			B = 0x0;
			break;

			// cyan    0xaa    0xff    0xee    4
		case 3:
			R = 0xaa;
			G = 0xff;
			B = 0xee;
			break;

			// purple    0xcc    0x44    0xcc    5
		case 4:
			R = 0xcc;
			G = 0x44;
			B = 0xcc;
			break;

			// green    0x00    0xcc    0x55    6
		case 5:
			R = 0;
			G = 0xcc;
			B = 0x55;
			break;

			// blue    0x00    0x00    0xaa    7
		case 6:
			R = 0;
			G = 0;
			B = 0xaa;
			break;

			// yellow    0xee    0xee    0x77    8
		case 7:
			R = 0xee;
			G = 0xee;
			B = 0x77;
			break;

			// orange    0xdd    0x88    0x55    9
		case 8:
			R = 0xdd;
			G = 0x88;
			B = 0x55;
			break;

			// brown    0x66    0x44    0x00    10
		case 9:
			R = 0x66;
			G = 0x44;
			B = 0;
			break;

			// l,red    0xff    0x77    0x77    11
		case 10:
			R = 0xff;
			G = 0x77;
			B = 0x77;
			break;

			// d,gray    0x33    0x33    0x33    12
		case 11:
			R = 0x33;
			G = 0x33;
			B = 0x33;
			break;

			// gray    0x77    0x77    0x77    13
		case 12:
			R = 0x77;
			G = 0x77;
			B = 0x77;
			break;
			// lgreen    0xaa    0xff    0x66    14
		case 13:
			R = 0xaa;
			G = 0xff;
			B = 0x66;
			break;

			// l,blue    0x00    0x88    0xff    15
		case 14:
			R = 0;
			G = 0x88;
			B = 0x88;
			break;

			// l,gray    0xbb    0xbb    0xbb    16
		case 15:
			R = 0xbb;
			G = 0xbb;
			B = 0xbb;
			break;


			// black    0x00    0x00    0x00    1
		default:
			R = 0x0;
			G = 0xbb;
			B = 0x0;
			break;
		}

		yarmico_rectangle(R, G, B, 0, (i*h), YARMICO_WIDTH, h); //+(i&1)

	}

}



// poor man's percent - not in critical loop!
//intern_PSX_calc_percent(g_gamestate.level_bullets_hit,  g_gamestate.level_bullets_used );
//ie g_total_bullets_hit  / g_total_bullets_used
#define LOG_PERC //  printf
#if 1
u32 intern_PSX_calc_percent(s32 numerator , s32 denominator)
{

	u32 ret =0;
	LOG_PERC( "intern_PSX_calc_percent %d / %d \n", numerator ,  denominator );



	if(numerator == 0 )
		return 0;

	if(denominator == 0 )
		return 0;


	if(denominator == numerator )
		return 100;

	if(denominator == 1 )
		return numerator;


	numerator*=100;

	do
	{
		numerator -=denominator;
		ret++;
		LOG_PERC( "intern_PSX_DIV %d ret %d \n", numerator ,  ret );
	}while(numerator>0);

	if(numerator < -50)
		ret--;

	return ret;


}
#endif



//PSX qsort has issues - avoid this one works fine
//https://www.geeksforgeeks.org/generic-implementation-of-quicksort-algorithm-in-c/



/* The GNU glibc version of qsort allocates memory, which we must not
   do if we are invoked by a signal handler.  So provide our own
   sort. https://github.com/gcc-mirror/gcc/blob/master/libbacktrace/sort.c
 */

void swap (char *a, char *b, size_t size)
{
	size_t i;

	for (i = 0; i < size; i++, a++, b++)
	{
		char t;

		t = *a;
		*a = *b;
		*b = t;
	}
}

void _qsort (void *basearg, size_t count, size_t size,
		s32 (*compar) ( void *,  void *) )
{
	char *base = (char *) basearg;
	size_t i;
	size_t mid;

	tail_recurse:
	if (count < 2)
		return;

	/* The symbol table and DWARF tables, which is all we use this
     routine for, tend to be roughly sorted.  Pick the middle element
     in the array as our pivot point, so that we are more likely to
     cut the array in half for each recursion step.  */
	swap (base, base + (count>>1) * size, size);

	mid = 0;
	for (i = 1; i < count; i++)
	{
		if ((*compar) (base, base + i * size) > 0)
		{
			++mid;
			if (i != mid)
				swap (base + mid * size, base + i * size, size);
		}
	}

	if (mid > 0)
		swap (base, base + mid * size, size);

	/* Recurse with the smaller array, loop with the larger one.  That
     ensures that our maximum stack depth is log count.  */
	if (2 * mid < count)
	{
		_qsort (base, mid, size, compar);
		base += (mid + 1) * size;
		count -= mid + 1;
		goto tail_recurse;
	}
	else
	{
		_qsort (base + (mid + 1) * size, count - (mid + 1),
				size, compar);
		count = mid;
		goto tail_recurse;
	}
}




// 8x8
// 0: A - P
// 8: Q - Z - arrows, $ $
// 16: 0 - 9 - . ? ! music
// 24: heart1, 2 checkers5, 6 bull

#define CHAR_SPACE 8
void yarmico_render_text(u32 posX, u32 posY, s32 scale, char *string)
{
	u32 i,  U, V; //len,
	u8 c;

	LOG_TEXT("posX %d, posY %d, scale %d, %s \n", posX,  posY,  scale, string);


#if 0

	// process string ------------
	len=strlen(string);

	LOG_TEXT("len=strlen(string); %d  \n", len);


	if(len > YARMICO_WIDTH>>3) //limit string to screen width
		len = YARMICO_WIDTH>>3;
#endif

	for(i=0;i<=YARMICO_WIDTH>>3; i++)
	{
		c = string[i];

		LOG_TEXT("i: %d c: %c \n", i, c);



		if(c == 0) //NUL
		{
			return;
		}

		if(c == 32) //space
		{
			posX+=CHAR_SPACE;
			continue;
		}


		//first mostlikely cases
		if(c > 47 && c < 58) // 0-9 zero digit
		{
			U = 8 * (c - '0');
			V = 16;
		}
		else if(c > 64 && c < 81) // A - P
		{
			U = 8 * (c - 'A');
			V = 0;
		}
		else if(c > 80 && c < 91) // Q - Z
		{
			U = 8 * (c - 'Q');
			V = 8;
		}
		// special string chars -.!$?
		else if(c == '-')
		{
			U = 8 * 10;
			V = 16;
		}
		else if(c == '.')
		{
			U = 8 * 11;
			V = 16;
		}
		else if(c == '?')
		{
			U = 8 * 13;
			V = 16;
		}
		else if(c == '!')
		{
			U = 8 * 14;
			V = 16;
		}
		else if(c == '/')
		{
			U = 8 * 10;
			V = 8;
		}
		else if(c == '=')
		{
			U = 8 * 11;
			V = 8;
		}
		else if(c == ',')
		{
			U = 8 * 12;
			V = 8;
		}
		else if(c == '@') // Smile face
		{
#if 0
			if( ( (u32)string&1) == 0)
				U = 8 * 13;
			else
				U = 8 * 14;
#endif
			U = 8 * 13;
			V = 8;
		}
		else if(c == '$')
		{
			U = 8 * 14;
			V = 8;
		}

		//last row 8*3=24
		else if(c == '+')
		{
			U = 8 * 7;
			V = 24;
		}


		else if(c == ':')
		{
			U = 8 * 8;
			V = 24;
		}


		else if(c == '\'')
		{
			U = 8 * 9;
			V = 24;

		}



		else if(c == '(')
		{
			U = 8 * 10;
			V = 8*3;

		}
		else if(c == ')')
		{
			U = 8 * 11;
			V = 8*3;
		}
		else if(c == '#') // heart
		{

#if HARDWARE_TARGET == HARDWARE_TARGET_NET_YAROZE
			if( ( (u32)string&1) == 0)
#else
				if( ( (u32)i&1) == 0)
#endif


					U = 8;
				else
					U = 0;

			V = 24;
		}
		else if(c == '_')
		{
			U = 16;
			V = 24;
		}
		else if(c == '[')
		{
			U = 32;
			V = 24;
		}
		else if(c == ']')
		{
			U = 40;
			V = 24;
		}
		else
			//95 underscore
			//			if(c > 96) // small caps chars
		{
			printf(" len %d  (%d char %c) Char lower case! string %s\n",strlen(string),  c, c, string);
			printf(" len %d  (%d char %c) Char lower case! string %s\n",strlen(string),  c, c, string);
			printf(" len %d  (%d char %c) Char lower case! string %s\n",strlen(string),  c, c, string);

			exit(0);//c -= 32; // turn into big capitals
		}



		V += FONT_Y_OFFSET; // start Y 224
		yarmico_sprite(FONT_TPAGE,   U, V, 8, 8,    posX+0,     posY+0,    scale ,  0, 0);
		posX+=CHAR_SPACE;
	}
}










#ifdef __ANDROID__
// ANDROID uses CPP not C
}
#endif



