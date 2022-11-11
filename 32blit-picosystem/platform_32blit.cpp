/*
 *


 THIS CODE IS A HACKY MESS - READ IT AT YOUR OWN RISK!


#define INSTANCE_YARMICO_API
 *
 */




#include "yarmico_API.h"

#include "platform_32blit.hpp"




#ifdef USE_TPAGE00
Surface* g_TPAGE00;
#endif


#ifdef USE_TPAGE01
Surface* g_TPAGE01;
#endif


#ifdef USE_TPAGE02
Surface* g_TPAGE02;
#endif



#ifdef USE_TPAGE03
Surface* g_TPAGE03;
#endif


#ifdef USE_TPAGE04
Surface* g_TPAGE04;
#endif

#ifdef USE_TPAGE05
Surface* g_TPAGE05;
#endif


#define SOUND_ADD_DATA_POINTER 512
#define SOUND_MINUS_LEN 512

// audio implementation based from 32blitDoom -  https://github.com/Daft-Freak/32BlitDoom
struct active_sound
{
	short rate;
	unsigned char *data;
	uint32_t length;
	uint32_t offset;
};

unsigned int g_ms_start;



#if HARDWARE_TARGET!=HARDWARE_TARGET_PICOSYSTEM

// CHANNEL_COUNT depends on target 32blit device is 8channels, everything else SDL based is configured to 16
#define SOUND_ALL_CHANNELS (CHANNEL_COUNT )

#define SOUND_BGM_CHANNEL  (SOUND_ALL_CHANNELS-1) // last
#define SOUND_ALL_FX_CHANNELS (SOUND_BGM_CHANNEL -1)

#else // no BGM

// CHANNEL_COUNT depends on target 32blit device is 8channels, everything else is 16
#define SOUND_ALL_CHANNELS (CHANNEL_COUNT )

#define SOUND_BGM_CHANNEL  !ERROR-NO-BGM-USED!
#define SOUND_ALL_FX_CHANNELS (SOUND_ALL_CHANNELS)

#endif // #if HARDWARE_TARGET!=HARDWARE_TARGET_PICOSYSTEM



//samples/HW 8000 11025 22050, no 44100 support;
// if too much audio, use smaller sample rate, ie for 32blit or HTML5, everything 22k

//TODO AUDIO IS HARDCODED :/
#ifdef __EMSCRIPTEN__
/*


add -s TOTAL_MEMORY=67108864 to the LINKFLAGS

//Flags used by the linker during all build types.\\
CMAKE_EXE_LINKER_FLAGS:STRING= -s TOTAL_MEMORY=67108864


It should automatically update the linker settings: CMakeFiles/game.dir/link.txt \\
If not, add it to the line after em++
 */


#define AUDIO_SAMPLE_RATE_BGM 11025
#define AUDIO_SAMPLE_RATE_SFX 22050


#elif  __arm__ // 32blit

#define AUDIO_SAMPLE_RATE_BGM 8000
#define AUDIO_SAMPLE_RATE_SFX 11025


#else
// PC windows/linux
#define AUDIO_SAMPLE_RATE_BGM 22050 //22050
#define AUDIO_SAMPLE_RATE_SFX 22050 //22050 //22050

#endif






#ifdef __ANDROID__
// ouya use hi-def audio
#undef AUDIO_SAMPLE_RATE_BGM  
#undef AUDIO_SAMPLE_RATE_SFX  

#define AUDIO_SAMPLE_RATE_BGM 22050 //22050
#define AUDIO_SAMPLE_RATE_SFX 22050 //22050


#endif


#define MAX_BGM_TRACKS  2
static active_sound g_yarmico_channel_sounds[SOUND_ALL_CHANNELS];
char g_yarmico_last_channel;
char g_yarmico_BGM_Track;


extern "C" void yarmico_play_BGM_audio(int side);

extern "C" void game_update(void);







void RefillBuffer(blit::AudioChannel &channel)
{



	auto sound = (active_sound *)(channel.user_data);

	LOG_SND("RefillBuffer start -sound->rate %d sound->offset %d  sound->length %d  \n",sound->rate, sound->offset, sound->length );

	if(sound->offset >= sound->length)
	{
#if YARMICO_USE_BGM == TRUE
		if((void *)sound == (void *)(&g_yarmico_channel_sounds[SOUND_BGM_CHANNEL])) // BGM stopped!
		{
			//sound->offset =0; // restart it?

			if(g_yarmico_BGM_Track == MAX_BGM_TRACKS)
				g_yarmico_BGM_Track =0;
			else
				g_yarmico_BGM_Track++;

			yarmico_play_BGM_audio(g_yarmico_BGM_Track);
			return; // exit out, nothing to play here!
		}
		else
#endif
		{
			sound->offset = sound->length = 0;
			sound->data = NULL;
			channel.off();
		}
		// LOG_SND("RefillBuffer ended and off \n");
		return;
	}

	int i = 0;

	// 32blit default
	if(sound->rate == 22050) //22050 )
	{

		/*
		 *
		 *
 #speed up
  ffmpeg -i $f  -filter:a "atempo=1.5" -vn -y $f-1.wav
  # cp  $f  $f-1.wav

  #pitch up
  ffmpeg -i $f-1.wav -af asetrate=44100*1.1,aresample=44100 -filter:a "volume=0.75" -y  $f-2.wav
		 *
		 */

#ifdef __EMSCRIPTEN__ // it clips alot! I don't know why
		for(i = 0; i < 64 && sound->offset < sound->length; i++, sound->offset++)
			channel.wave_buffer[i] = (sound->data[sound->offset] - 127) * 256;// 128;
#else

		for(i = 0; i < 64 && sound->offset < sound->length; i++, sound->offset++)
			channel.wave_buffer[i] = (sound->data[sound->offset] - 127) * 256;
#endif



	}
	// next common here and easy to encode
	else if(sound->rate == 11025)
	{

		/*
		 *
		 *
	 #speed up
	  ffmpeg -i $f  -filter:a "atempo=1.5" -vn -y $f-1.wav
	  # cp  $f  $f-1.wav

	  #pitch up
	  ffmpeg -i $f-1.wav -af asetrate=44100*1.1,aresample=44100 -filter:a "volume=0.75" -y  $f-2.wav
		 *
		 */

#ifdef __EMSCRIPTEN__ // it clips alot! I don't know why
		for(i = 0; i < 64 && sound->offset < sound->length; i+=2,sound->offset++)
		{
			channel.wave_buffer[i + 1] = (sound->data[sound->offset]  - 127 ) * 128;
		}
#else

		for(i = 0; i < 64 && sound->offset < sound->length; i+=2,sound->offset++)
		{
			channel.wave_buffer[i] =   	channel.wave_buffer[i + 1] = (sound->data[sound->offset]  - 127 ) * 256;
		}

#endif
	}
	// really desperate for space and cycles, hacky, sounds bad but works fine!
	else if(sound->rate == 8000)
	{


		/*
		 *

	ffmpeg -i $f  -ac 1  -filter:a "volume=0.70"  -vn    -y  1.wav
	ffmpeg -i 1.wav -filter:a "atempo=1.50"  -vn    -y  2.wav
	ffmpeg -i  2.wav -af asetrate=44100*1.50,aresample=44100  -y  3.wav

	BGM created in audacity -


	sox works BUT encoding ads alot of hiss!
	using audacity batch PITA but better sounds
	convert audio to mono wav (MS) 44100 u16bit first (tracks - Mix - Mix stereo down to mono)
	load all wav files in audacity
	set project rate (bottom left screen) to 8k, 11k or 22k
	file -> export -> export multiple
	enter new folder ie: 8000 11025 or 22050
	export as wav (MS) u8bit PCM first - test
	reimport and export multiple
	select all
	change speed to 30-50% faster - I can't remember exactly... 40%??
	format: other uncompressed files
	header: RAW hearder-less
	endcoding: unsigned 8bit PCM
	copy into include folder and rebuild app
		 */



		// __EMSCRIPTEN__   // it clips alot! I don't know why


#ifdef __EMSCRIPTEN__ // it clips alot! I don't know why
		for(i = 0; i < 64 && sound->offset < sound->length; i += 2,sound->offset++)
		{
			channel.wave_buffer[i] =  	channel.wave_buffer[i+1] =  channel.wave_buffer[i+2] =		channel.wave_buffer[i+3] =   ( sound->data[sound->offset] - 127 ) * 128;
		}
#else
		for(i = 0; i < 64 && sound->offset < sound->length; i +=4, sound->offset++)
		{
			channel.wave_buffer[i] =  	channel.wave_buffer[i+1] =  channel.wave_buffer[i+2] =		channel.wave_buffer[i+3] =   ( sound->data[sound->offset] - 127 ) * 256;

		}
#endif





	}
#if 0 // not used really crappy!
	else if(sound->rate == 4000) //4k is 8k sped up ie extra extra compressed!
	{


		/*
		 *
		 * must be sped up tempo and pitched

ffmpeg -i $f  -ac 1  -filter:a "volume=0.85"  -vn    -y  1.wav
ffmpeg -i 1.wav -filter:a "atempo=2.0"  -vn    -y  2.wav
ffmpeg -i  2.wav -af asetrate=44100*2.75,aresample=44100  -y  3.wav
		 */




		for(i = 0; i < 64 && sound->offset < sound->length; i +=8, sound->offset++)
		{
			channel.wave_buffer[i] =  	channel.wave_buffer[i+1] =  channel.wave_buffer[i+2] =		channel.wave_buffer[i+3] =  
					channel.wave_buffer[i+4] =  	channel.wave_buffer[i+5] =  channel.wave_buffer[i+6] =		channel.wave_buffer[i+7] =   ( sound->data[sound->offset] - 127 ) * 256;

		}





	}

#endif// 4k audio, really crappy!


	LOG_SND("RefillBuffer finished -  found  %d sound->offset %d  sound->length %d  \n", i, sound->offset, sound->length );

	// pad end
	for(;i < 64; i++)
		channel.wave_buffer[i] = 0;



}



void sound_stop_all_SFX(void)
{

#if YARMICO_PICO_SOUND == TRUE
	g_yarmico_pico_play_sound.time_frames = 0;
#endif

	for(int i = 0; i < SOUND_ALL_FX_CHANNELS; i++)
	{
		LOG_SND("yarmico_stop_all_SFX  blit::channels[%d] \n", i);
		blit::channels[i].off();
	}


}



unsigned char blit_play_SFX_32blit(int id, int channel, int vol)
{

	// need to load the sound

	LOG_SND("blit_play_SFX_32blit id:%d chan:%d vol:%d \n", id , channel, vol);


	unsigned int lumplen;
	unsigned char *data;

#if HARDWARE_TARGET!=HARDWARE_TARGET_PICOSYSTEM

	//originally
	//16
	//32
	data += SOUND_ADD_DATA_POINTER; // 512;
	lumplen -=  SOUND_MINUS_LEN; // 512;

	// need to load the sound

	LOG_SND("play_SFX id:%d chan:%d vol:%d \n", id , channel, vol);


	// lumpnum = sfxinfo->lumpnum;
#ifdef SFX_00

	switch(id)
	{

	case 0:
		data = (unsigned char *) asset_sfx_00;
		lumplen =  asset_sfx_00_length;
		break;

	case 1:
		data = (unsigned char *) asset_sfx_01;
		lumplen = asset_sfx_01_length;
		break;
	case 2:
		data = (unsigned char *) asset_sfx_02;
		lumplen =  asset_sfx_02_length;
		break;
	case 3:
		data = (unsigned char *) asset_sfx_03;
		lumplen =  asset_sfx_03_length;
		break;
	case 4:
		data = (unsigned char *) asset_sfx_04;
		lumplen =  asset_sfx_04_length;
		break;
	case 5:
		data = (unsigned char *) asset_sfx_05;
		lumplen = asset_sfx_05_length;
		break;
	case 6:
		data = (unsigned char *) asset_sfx_06;
		lumplen =  asset_sfx_06_length;
		break;
	case 7:
		data = (unsigned char *) asset_sfx_07;
		lumplen =  asset_sfx_07_length;
		break;
	case 8:
		data = (unsigned char *) asset_sfx_08;
		lumplen = asset_sfx_08_length;
		break;
	case 9:
		data = (unsigned char *) asset_sfx_09;
		lumplen =  asset_sfx_09_length;
		break;
	case 10:
		data = (unsigned char *) asset_sfx_10;
		lumplen =  asset_sfx_10_length;
		break;
	}

	LOG_SND("play_SFX length %d \n",  lumplen);


	g_yarmico_channel_sounds[channel].rate = AUDIO_SAMPLE_RATE_SFX;//11025; //8000; // 22050 ;
	g_yarmico_channel_sounds[channel].data = data;
	g_yarmico_channel_sounds[channel].length = lumplen;
	g_yarmico_channel_sounds[channel].offset = 0;
#endif //#ifdef SFX_00

#ifdef __EMSCRIPTEN__
	blit::channels[channel].volume = vol; //0xffff; //vol * 111;
#else
	blit::channels[channel].volume = vol; //0xffff; //vol * 111;
#endif




	//re init?
	blit::channels[channel].waveforms = blit::Waveform::WAVE;
	blit::channels[channel].user_data = (void *)(&g_yarmico_channel_sounds[channel]);
	blit::channels[channel].wave_buffer_callback =  &RefillBuffer;

	blit::channels[channel].adsr = 0xFFFF00;
	blit::channels[channel].trigger_sustain();


#else // HARDWARE_TARGET is HARDWARE_TARGET_PICOSYSTEM
	g_yarmico_pico_play_sound.SFX =  YARMICO_PICO_SOUND_SFX2_UP_DOWN;
	g_yarmico_pico_play_sound.play_back = YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK;

	g_yarmico_pico_play_sound.SFX_trigger2 = 1;
	g_yarmico_pico_play_sound.SFX_hold2 = 5; // 5 frame skips
	g_yarmico_pico_play_sound.dir_speed = 100;

	g_yarmico_pico_play_sound.time_frames = 10;

	YARMICO_MACRO_SET_PICO_AUDIO(165, 100, 100, 0, 0, 2 );

#endif



	LOG_SND("blit_play_SFX_32blit finished %d %d \n", id , channel);
	return true;
}



static unsigned char Init32blit_audio(void)
{
	//LOG_SND("Init32blit_audio %d \n", 0x0);


	for(u32 i=0; i < SOUND_ALL_CHANNELS; i++)
	{
		LOG_SND("Init32blit_audio blit::channels[%d] \n", i);
		blit::channels[i].off();
		blit::channels[i].waveforms = blit::Waveform::WAVE;
		blit::channels[i].user_data = (void *)(&g_yarmico_channel_sounds[i]);
		blit::channels[i].wave_buffer_callback = RefillBuffer;
	}



	LOG_SND("Init32blit_audio finished \n\n");
	return true;
}


// audio implementation based from 32blitDoom -  https://github.com/Daft-Freak/32BlitDoom




#if 0 //OG 32blit doom
static int I_Blit_StartSound(void *sfxinfo, int channel, int vol, int sep)
{
	if(channel >= SOUND_ALL_CHANNELS)
		return -1;

	//  blit::channels[channel].off();


	if(!play_SFX(0, channel))
		return -1;

	blit::channels[channel].adsr = 0xFFFF00;
	blit::channels[channel].trigger_sustain();

	blit::channels[channel].volume = vol * 111;
	// I_Blit_UpdateSoundParams(channel, vol, sep);

	return channel;
}

static void I_Blit_StopSound(int handle)
{
	if (handle >= SOUND_ALL_CHANNELS)
		return;

	blit::channels[handle].off();
}

static unsigned char I_Blit_SoundIsPlaying(int handle)
{
	if(handle > SOUND_CHANNELS)
		return false;

	return blit::channels[handle].adsr_phase != blit::ADSRPhase::OFF;
}

#endif






extern "C" {



#if 0 // turned to macro -> #define  LOG_PAD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#ifdef __ANDROID__

#include <stdarg.h>

int android_debugf( const char *format, ...)
{
	int result;
	va_list args;

	va_start(args, format);
	__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, format, args );
	va_end(args);

	__android_log_print(ANDROID_LOG_DEBUG,    LOG_TAG, "\nThe value of 0 is %d", 0);
	return result;
}

#endif //#ifdef __ANDROID__

#endif


#if 0
/*
 *
 *
 *

extern "C" 	int android_debugf( const char *format, ...);


#define LOG_DATA   android_debugf
#define LOG_EXIT    android_debugf
#define printf     android_debugf
#define LOG_DEL    android_debugf
#define LOG_PAD    android_debugf

 *
 *
 *
 *
 *
 *
 *
 *



// REMOVE MOUSE HANDLING
//around line 55 in 32blit-sdl/main.cpp
		case SDL_MOUSEBUTTONDOWN:
//mgarcia			blit_input->handle_mouse(event.button.button, event.type == SDL_MOUSEBUTTONDOWN, event.button.x, event.button.y);
			break;

		case SDL_MOUSEMOTION:
			if (event.motion.state & SDL_BUTTON_LMASK) {
//mgarcia				blit_input->handle_mouse(SDL_BUTTON_LEFT, event.motion.state & SDL_MOUSEBUTTONDOWN, event.motion.x, event.motion.y);
			}
			break;



// line 69 in the if statement	if (!blit_input->handle_keyboard(event.key.keysym.sym, event.type == SDL_KEYDOWN)){
 //mgarcia fullscreen
				 if (event.key.keysym.sym == SDLK_b && event.type == SDL_KEYDOWN)
				 {
					SDL_ShowCursor(is_fullscreen);
					is_fullscreen = !is_fullscreen;
					SDL_SetWindowFullscreen(window, is_fullscreen );
					return;
				}
//mgarcia fullscreen


// put around line 28
 unsigned int is_fullscreen = 0; //mgarcia


 /home/dev/32blit-sdk/32blit-sdl/Input.cpp

  remapped
//mgarcia
	{SDLK_j,       blit::Button::Y}, // left
	{SDLK_l,       blit::Button::A}, // right

	{SDLK_i,       blit::Button::X}, // up
	{SDLK_k,       blit::Button::B}, // down
//mgarcia


changed WASD to:
	{SDLK_e,       blit::Button::DPAD_UP},
	{SDLK_s,       blit::Button::DPAD_LEFT},
	{SDLK_d,       blit::Button::DPAD_DOWN},
	{SDLK_f,       blit::Button::DPAD_RIGHT},


	// removed 1 and 2 buttons
	// mgarcia {SDLK_1,       blit::Button::HOME},
	// garcia {SDLK_2,       blit::Button::MENU},
	{SDLK_SPACE,       blit::Button::HOME}, //mgarcia - made space as pause/back
	{SDLK_3,       blit::Button::JOYSTICK},

  {SDLK_ESCAPE,  blit::Button::MENU},






// my action buttons, around line 66


	// action buttons
#if 0 // ORIG
	{SDL_CONTROLLER_BUTTON_A,           blit::Button::A},
	{SDL_CONTROLLER_BUTTON_B,           blit::Button::B},
	{SDL_CONTROLLER_BUTTON_X,           blit::Button::X},
	{SDL_CONTROLLER_BUTTON_Y,           blit::Button::Y},

#else // mgarcia mapped using https://generalarcade.com/gamepadtool/

	{SDL_CONTROLLER_BUTTON_X,           blit::Button::Y}, // left
	{SDL_CONTROLLER_BUTTON_B,           blit::Button::A}, //  right



	{SDL_CONTROLLER_BUTTON_Y,           blit::Button::X}, // up
	{SDL_CONTROLLER_BUTTON_A,           blit::Button::B}, // down
#endif

 *

// Adding more channels for PC's
FILE: 32blit-sdk/32blit/audio/audio.hpp

 // default mgarcia #define CHANNEL_COUNT 8


#ifdef __EMSCRIPTEN__
#define CHANNEL_COUNT 8


#elif  __arm__
#define CHANNEL_COUNT 8


#else
 // PC windows/linux
#define CHANNEL_COUNT 16

#endif



OPTIONAL remove 32BLIT timeout!
~/32blit-sdk/32blit-stm32/Src/power.cpp
Replaced these functions with:

 void update() {
      return;
  }

  void update_active() {
	  return;
  }

cd  ~/32blit-sdk/build.stm32
run:
make firmware-update
make firmware-update.flash

I had random linker issues... remove " quotes from link file: /home/dev/32blit-sdk/build.stm32/firmware-update/CMakeFiles/firmware-update.dir/link.txt




 */
#endif





void SoundInit(void)
{
	LOG_SND("SoundInit  \n");

	Init32blit_audio();
	g_yarmico_last_channel = 0;
	g_SET_BGM_VOL = g_SET_SFX_VOL =   0xffff;
	LOG_SND("SoundInit fin  \n");

}


void yarmico_stop_all_SFX(void)
{
	sound_stop_all_SFX();
}




void yarmico_BGM_quiet(int quiet) // 1 true reduce vol - 0 false increase back to normal BG volume
{

#if YARMICO_USE_BGM == TRUE


	if(g_SET_BGM_VOL)
	{
		if(quiet)
		{
			blit::channels[SOUND_BGM_CHANNEL].volume = g_SET_BGM_VOL >>2; //vol * 111;
		}
		else
		{
			blit::channels[SOUND_BGM_CHANNEL].volume = g_SET_BGM_VOL; //vol * 111;
		}
	}

#endif

}



void yarmico_play_BGM_audio(int track)
{
	unsigned int lumplen;
	unsigned char *data;

#if YARMICO_USE_BGM == TRUE

	// return;

	// need to load the sound

	LOG_SND("yarmico_play_BG_audio %d \n", track);



	if(g_SET_BGM_VOL < 2)
		return;

	g_yarmico_BGM_Track = track;

#ifdef USE_BGM_00
	switch(track)
	{

	case 0:
		data = (unsigned char *) asset_bgm_00;
		lumplen =  asset_bgm_00_length;
		break;

#ifdef USE_BGM_01
	case 1:
		data = (unsigned char *) asset_bgm_01;
		lumplen =  asset_bgm_01_length;
		break;
#endif



#ifdef USE_BGM_02
	case 2:
		data = (unsigned char *) asset_bgm_02;
		lumplen =  asset_bgm_02_length;
		break;
#endif


#ifdef USE_BGM_03
	case 3:
		data = (unsigned char *) asset_bgm_03;
		lumplen =  asset_bgm_03_length;
		break;
#endif


#ifdef USE_BGM_04
	case 4:
		data = (unsigned char *) asset_bgm_04;
		lumplen =  asset_bgm_04_length;
		break;
#endif

#ifdef USE_BGM_05
	case 5:
		data = (unsigned char *) asset_bgm_05;
		lumplen =  asset_bgm_05_length;
		break;
#endif


	default:
		printf("playing bgm bad track\n");
		exit(1);
		break;

	}
#else
	LOG_MAIN("no bgm defined\n");
	exit(1);
#endif







	LOG_SND("yarmico_play_BG_audio length %d \n",  lumplen);



	g_yarmico_channel_sounds[SOUND_BGM_CHANNEL].rate = AUDIO_SAMPLE_RATE_BGM;
	g_yarmico_channel_sounds[SOUND_BGM_CHANNEL].data = data;
	g_yarmico_channel_sounds[SOUND_BGM_CHANNEL].length = lumplen;
	g_yarmico_channel_sounds[SOUND_BGM_CHANNEL].offset = 0;

#ifdef __EMSCRIPTEN__
	blit::channels[SOUND_BGM_CHANNEL].volume = g_SET_BGM_VOL; // 0x5fff; //vol * 111;
#else
	blit::channels[SOUND_BGM_CHANNEL].volume = g_SET_BGM_VOL; //0xffff; //vol * 111;
#endif



	//re init?
	blit::channels[SOUND_BGM_CHANNEL].waveforms = blit::Waveform::WAVE;
	blit::channels[SOUND_BGM_CHANNEL].user_data = (void *)(&g_yarmico_channel_sounds[SOUND_BGM_CHANNEL]);
	blit::channels[SOUND_BGM_CHANNEL].wave_buffer_callback =  &RefillBuffer;

	blit::channels[SOUND_BGM_CHANNEL].adsr = 0xFFFF00;
	blit::channels[SOUND_BGM_CHANNEL].trigger_sustain();

	// don't need the original lump any more

	//  W_ReleaseLumpNum(lumpnum);


	LOG_SND("yarmico_play_BG_audio finished %d channel  %d \n", track, SOUND_BGM_CHANNEL);
#endif // #if YARMICO_USE_BGM == TRUE

	return;



}




void yarmico_clear_SFX(void)
{
	LOG_SND("yarmico_clear_SFX  \n");


	for(int i=0; i< SOUND_ALL_CHANNELS; i++)
		blit::channels[i].off();
}


u32 yarmico_play_SFX(u32 id)
{

	g_stereo_counter_random++;
	LOG_SND("yarmico_play_SFX  %d %d \n", id , g_SET_SFX_VOL);




	LOG_SND("setup_start_Sound  %d blit::channels[i].adsr_phase %d \n", g_yarmico_last_channel, blit::channels[g_yarmico_last_channel].adsr_phase);




	//if(blit::channels[g_yarmico_last_channel].adsr_phase == ADSRPhase::OFF)
	{

		LOG_SND("setup_start_Sound  	play_SFX(id, g_yarmico_last_channel, g_SET_SFX_VOL);  returning %d blit::channels[i].adsr_phase %d \n", g_yarmico_last_channel, blit::channels[g_yarmico_last_channel].adsr_phase);
		blit_play_SFX_32blit(id, g_yarmico_last_channel, g_SET_SFX_VOL);
		LOG_SND("setup_start_Sound  done returning %d blit::channels[i].adsr_phase %d \n", g_yarmico_last_channel, blit::channels[g_yarmico_last_channel].adsr_phase);

		// blit::channels[i].volume = vol * 111; //0xffff; //vol * 111;


	}


	g_yarmico_last_channel++;

	if(g_yarmico_last_channel >=  SOUND_ALL_FX_CHANNELS)
		g_yarmico_last_channel =0;




	LOG_SND("yarmico_play_SFX fin  %d %d \n", id , g_SET_SFX_VOL);

	return 0;

}

/* Sound playback termination */

void SoundClose(void)
{



	return;

}







u8 g_BG_colour_r;
u8 g_BG_colour_g;
u8 g_BG_colour_b;







void yarmico_rectangle(u8 r, u8 g, u8 b, u32 screen_posX, u32 screen_posY, u32 w, u32 h )
{
	screen.alpha = 255;
	screen.pen = Pen(r, g, b);

#if HARDWARE_TARGET!=HARDWARE_TARGET_PICOSYSTEM

#if YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_120x120
	screen_posX += 20;

#endif

#endif

	screen.rectangle( Rect(screen_posX, screen_posY, w, h)  );
}




void yarmico_sprite(u8 TPAGE, u32 texture_u, u32 texture_v, u32 texture_w, u32 texture_h, s32 posX, s32 posY, s32 scale , u8 h_flip, u8 v_flip)
{
	float fscale;

#ifdef USE_TPAGE00
	switch(TPAGE)
	{

	// tpages def via yml file

	case 0:
		screen.sprites = g_TPAGE00;
		break;

#ifdef USE_TPAGE01
	case 1:
		screen.sprites = g_TPAGE01;
		break;
#endif

#ifdef USE_TPAGE02
	case 2:
		screen.sprites = g_TPAGE02;
		break;
#endif

#ifdef USE_TPAGE03
	case 3:
		screen.sprites = g_TPAGE03;
		break;
#endif

#ifdef USE_TPAGE04
	case 4:
		screen.sprites = g_TPAGE04;
		break;
#endif


#ifdef USE_TPAGE05
	case 5:
		screen.sprites = g_TPAGE05;
		break;
#endif


	default:
		LOG_MAIN("bad TPAGE defined\n");
		exit(1);

	}
#else
	LOG_MAIN("error no TPAGE defined\n");
	exit(1);
#endif

	//screen.alpha = alpha; not used


	fscale = scale/ONE;

	// //screen.sprite(Rect(0, 0, 32/8, 64/8), Point(64,  -100) ,  Point(0, 0), 6.f);
	if(h_flip)
		h_flip = SpriteTransform::HORIZONTAL;
	else
		h_flip = SpriteTransform::NONE;


	if(v_flip)
		h_flip |= SpriteTransform::VERTICAL;



#if HARDWARE_TARGET!=HARDWARE_TARGET_PICOSYSTEM

#if YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_120x120
	posX += 20;

#endif

#endif

	// screen.sprite( Rect(texture_u>>3, texture_v>>3, texture_w>>3, texture_h>>3), Point(posX, posY) ,  Point(texture_w>>1, texture_h>>1), fscale, h_flip);
	screen.sprite( Rect(texture_u>>3, texture_v>>3, texture_w>>3, texture_h>>3), Point(posX, posY) ,  Point(0, 0), fscale, h_flip);


	// posX += texture_w/2;
	// posY += texture_h/2;



}


} // extern "C" --- the rest is 32blit C++ !!!











u32 yarmico_random(void)
{

	// 32blit seeded random function
	return blit::random();
}


#if HARDWARE_TARGET == HARDWARE_TARGET_SDL2 // SDL2 stuff only



#include "load_gamecontrollerdb.hpp"
#include "gamecontrollerdb.hpp" // serialized gamecontrollerdb.txt string and size
/*
 assets.yml
 gamecontrollerdb.hpp:
 gamecontrollerdb.txt:
    name: gamecontrollerdb
 */

#endif // SDL2 stuff only


extern "C" void game_init(void);

void yarmico_init_32blit(void)
{


	LOG_MAIN(" void init(void)  \n");

  g_yarmico_pico_play_sound = {0};


#if HARDWARE_TARGET == HARDWARE_TARGET_SDL2 // SDL2 stuff only

	load_gamecontrollerdb( (const char *) gamecontrollerdb, gamecontrollerdb_length );


#endif //#if HARDWARE_TARGET==HARDWARE_TARGET_SDL2 // SDL2 stuff only



	// all systems use hires
	set_screen_mode(USE_SCREEN_MODE );



	joystick.x = joystick.y = tilt.x = tilt.y = tilt.z = 0;

	g_stereo_counter_random = 0;


#ifdef USE_TPAGE00
	g_TPAGE00 = Surface::load(tpage00);
#endif

#ifdef USE_TPAGE01
	g_TPAGE01 = Surface::load(tpage01);
#endif

#ifdef USE_TPAGE02
	g_TPAGE02 = Surface::load(tpage02);
#endif

#ifdef USE_TPAGE03
	g_TPAGE03 = Surface::load(tpage03);
#endif

#ifdef USE_TPAGE04
	g_TPAGE04 = Surface::load(tpage04);
#endif

#ifdef USE_TPAGE05
	g_TPAGE05 = Surface::load(tpage05);
#endif

	SoundInit();

	LOG_SND("SoundInit fin - yarmico_init  \n");






	// do again, to make sure - especially for android??!
	joystick.x = joystick.y = tilt.x = tilt.z = tilt.y = 0.0f;

	game_init();


}

#if 0
// yarmico_play_pico_piezo(880, 100, 100,  0xffff , 1000 )
//yarmico_init_pico_piezo(880, 100, 100,  0, 0);
void yarmico_init_pico_piezo(u8 frequency, u8 attack_ms, u8 decay_ms,  u8 sustain , u8 release_ms )
{
	/*

    Attack - is the time taken for initial run-up of level from nil to peak, beginning when the key is pressed.
    Decay - is the time taken for the subsequent run down from the attack level to the designated sustain level.
    Sustain - is the level during the main sequence of the sound's duration, until the key is released.
    Release - is the time taken for the level to decay from the sustain level to zero after the key is released.

	blit::channels[0].frequency = 880;
			blit::channels[0].attack_ms = 100;
			blit::channels[0].decay_ms = 100;
			blit::channels[0].sustain = 0;
			blit::channels[0].release_ms = 0;
	 */
	channels[YARMICO_PICO_PIEZO_CHANNEL].waveforms   = Waveform::SQUARE;
	channels[YARMICO_PICO_PIEZO_CHANNEL].frequency  = frequency;
	channels[YARMICO_PICO_PIEZO_CHANNEL].attack_ms   = attack_ms; //2
	channels[YARMICO_PICO_PIEZO_CHANNEL].decay_ms    = decay_ms; //6
	channels[YARMICO_PICO_PIEZO_CHANNEL].sustain     = 0;// 0xffff; //sustain;  //0xffff
	channels[YARMICO_PICO_PIEZO_CHANNEL].release_ms  = 10000;//release_ms; //1
	//	channels[YARMICO_PICO_PIEZO_CHANNEL].trigger_decay();

}

void yarmico_play_pico_piezo(u8 frequency, u8 release_ms )
{
	/*

    Attack - is the time taken for initial run-up of level from nil to peak, beginning when the key is pressed.
    Decay - is the time taken for the subsequent run down from the attack level to the designated sustain level.
    Sustain - is the level during the main sequence of the sound's duration, until the key is released.
    Release - is the time taken for the level to decay from the sustain level to zero after the key is released.
	 */

	channels[YARMICO_PICO_PIEZO_CHANNEL].waveforms   = Waveform::SQUARE;
	channels[YARMICO_PICO_PIEZO_CHANNEL].frequency  = frequency;
	channels[YARMICO_PICO_PIEZO_CHANNEL].release_ms  = release_ms;
	//channels[YARMICO_PICO_PIEZO_CHANNEL].trigger_attack();
	channels[YARMICO_PICO_PIEZO_CHANNEL].trigger_sustain();

}
#endif

void yarmico_LED_32blit(u8 r, u8 g, u8 b)
{
	blit::LED = Pen(    r,	      g,      b   );
}

#if YARMICO_PICO_SOUND == TRUE
void yarmico_play_pico_sound_platform(u32 turn_sound_off) //platform that plays it back
{
	s32 temp;
	u32 play_back = g_yarmico_pico_play_sound.play_back;
	//u16 freq = g_yarmico_pico_play_sound.frequency;

	if(turn_sound_off )
	{
		channels[YARMICO_PICO_PIEZO_CHANNEL].off();
		return;
	}

	if(play_back == YARMICO_PICO_SOUND_PLAYBACK_OFF)
	{
		g_yarmico_pico_play_sound.time_frames = 0;
		channels[YARMICO_PICO_PIEZO_CHANNEL].off();
		return;
	}

	g_yarmico_pico_play_sound.time_frames--;

	if(g_yarmico_pico_play_sound.SFX)
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

				return; //off
			}
		}

	}

#if 0
default:
	g_yarmico_pico_play_sound.SFX_counter--;
	if( g_yarmico_pico_play_sound.SFX_counter == 0)
	{
		g_yarmico_pico_play_sound.time_frames = 0;
		channels[YARMICO_PICO_PIEZO_CHANNEL].off();
	}
	break;
#endif




	channels[YARMICO_PICO_PIEZO_CHANNEL].waveforms   = Waveform::SQUARE;
	channels[YARMICO_PICO_PIEZO_CHANNEL].frequency  =   g_yarmico_pico_play_sound.frequency;
	channels[YARMICO_PICO_PIEZO_CHANNEL].attack_ms   = g_yarmico_pico_play_sound.attack_ms; //2
	channels[YARMICO_PICO_PIEZO_CHANNEL].decay_ms    = g_yarmico_pico_play_sound.decay_ms; //6
	channels[YARMICO_PICO_PIEZO_CHANNEL].sustain     = g_yarmico_pico_play_sound.sustain;// 0xffff; //sustain;  //0xffff
	channels[YARMICO_PICO_PIEZO_CHANNEL].release_ms  = g_yarmico_pico_play_sound.release_ms;//release_ms; //1
	/*
	 *
#define  YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK 		0
#define  YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_DECAY		1
#define  YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_SUSTAIN		2
#define  YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_RELEASE		4
#define  YARMICO_PICO_SOUND_PLAYBACK_OFF 					8
	 */

	switch(g_yarmico_pico_play_sound.play_back)
	{
	case YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_ATTACK:
		channels[YARMICO_PICO_PIEZO_CHANNEL].trigger_attack();
		break;
	case YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_DECAY:
		channels[YARMICO_PICO_PIEZO_CHANNEL].trigger_decay();
		break;
	case YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_SUSTAIN:
		channels[YARMICO_PICO_PIEZO_CHANNEL].trigger_sustain();
		break;

	case YARMICO_PICO_SOUND_PLAYBACK_TRIGGER_RELEASE:
		channels[YARMICO_PICO_PIEZO_CHANNEL].trigger_release();
		break;

	case YARMICO_PICO_SOUND_PLAYBACK_OFF:
		g_yarmico_pico_play_sound.time_frames = 0;
		channels[YARMICO_PICO_PIEZO_CHANNEL].off();
		break;


	}



}
#endif //#if YARMICO_PICO_SOUND == TRUE

#define YARMICO_MAIN_CONTROLLER_BUTTONS blit::buttons
// buttons.state

#define YARMICO_MAIN_CONTROLLER_START Button::MENU



#define YARMICO_MAIN_CONTROLLER_START Button::MENU
#define YARMICO_MAIN_CONTROLLER_SELECT Button::MENU
#define YARMICO_MAIN_CONTROLLER_TRIANGLE  Button::X //YARMICO_MAIN_CONTROLLER_BUTTONS & Button::X) // up
#define YARMICO_MAIN_CONTROLLER_CROSS  Button::B //YARMICO_MAIN_CONTROLLER_BUTTONS & Button::B) // down
#define YARMICO_MAIN_CONTROLLER_CIRCLE  Button::A // YARMICO_MAIN_CONTROLLER_BUTTONS & Button::A) // right
#define YARMICO_MAIN_CONTROLLER_SQUARE  Button::Y // YARMICO_MAIN_CONTROLLER_BUTTONS & Button::Y) // left
#define YARMICO_MAIN_CONTROLLER_DPAD_UP   Button::DPAD_UP // YARMICO_MAIN_CONTROLLER_BUTTONS & Button::DPAD_UP)
#define YARMICO_MAIN_CONTROLLER_DPAD_DOWN  Button::DPAD_DOWN // YARMICO_MAIN_CONTROLLER_BUTTONS & Button::DPAD_DOWN)
#define YARMICO_MAIN_CONTROLLER_DPAD_RIGHT   Button::DPAD_RIGHT // buttonsYARMICO_MAIN_CONTROLLER_BUTTONS & Button::DPAD_RIGHT)
#define YARMICO_MAIN_CONTROLLER_DPAD_LEFT   Button::DPAD_LEFT // buttonsYARMICO_MAIN_CONTROLLER_BUTTONS & Button::DPAD_LEFT)



/*
    bool button_a = blit::buttons & blit::Button::A; // O
    bool button_b = blit::buttons & blit::Button::B; // X
    bool button_x = blit::buttons & blit::Button::X; // /\
    bool button_y = blit::buttons & blit::Button::Y; // []
 */

//OUYA_ANDROID android API-16 Android devices >= 4.1.2
//NEW_ANDROID = android API-23 Android devices >= 6.0



void yarmico_process_controller_32blit(void)
{
	u8 ldir_pad, laction_buttons, lspecial_buttons;

	//	LOG_PAD("yarmico_process_controller \n");

	ldir_pad=0;
	laction_buttons=0;
	lspecial_buttons=0;


	if(YARMICO_MAIN_CONTROLLER_BUTTONS & YARMICO_MAIN_CONTROLLER_START)
		lspecial_buttons |= YARMICO_CONTROLER_BUT_MENU_START;

	if(YARMICO_MAIN_CONTROLLER_BUTTONS & YARMICO_MAIN_CONTROLLER_SELECT)
		lspecial_buttons |= YARMICO_CONTROLER_BUT_HOME_SELECT;


	if( YARMICO_MAIN_CONTROLLER_BUTTONS & 0x0F  )// bits 0 1 2 3 are L and R buttons
		lspecial_buttons |= YARMICO_CONTROLER_BUT_ANALOG_SHOULDERS;


	// DPAD

	if(YARMICO_MAIN_CONTROLLER_BUTTONS & YARMICO_MAIN_CONTROLLER_DPAD_UP)
		ldir_pad |= YARMICO_CONTROLER_BUT_U;

	else if(YARMICO_MAIN_CONTROLLER_BUTTONS & YARMICO_MAIN_CONTROLLER_DPAD_DOWN)
		ldir_pad |= YARMICO_CONTROLER_BUT_D;

	if(YARMICO_MAIN_CONTROLLER_BUTTONS & YARMICO_MAIN_CONTROLLER_DPAD_LEFT)
		ldir_pad |= YARMICO_CONTROLER_BUT_L;

	else if(YARMICO_MAIN_CONTROLLER_BUTTONS & YARMICO_MAIN_CONTROLLER_DPAD_RIGHT)
		ldir_pad |= YARMICO_CONTROLER_BUT_R;


	// face buttons


	if(YARMICO_MAIN_CONTROLLER_BUTTONS & YARMICO_MAIN_CONTROLLER_TRIANGLE)
		laction_buttons |= YARMICO_CONTROLER_BUT_U;

	else if(YARMICO_MAIN_CONTROLLER_BUTTONS & YARMICO_MAIN_CONTROLLER_CROSS)
		laction_buttons |= YARMICO_CONTROLER_BUT_D;

	if(YARMICO_MAIN_CONTROLLER_BUTTONS & YARMICO_MAIN_CONTROLLER_SQUARE)
		laction_buttons |= YARMICO_CONTROLER_BUT_L;

	else if(YARMICO_MAIN_CONTROLLER_BUTTONS & YARMICO_MAIN_CONTROLLER_CIRCLE)
		laction_buttons |= YARMICO_CONTROLER_BUT_R;

#if 0

	LOG_PAD(" ldir_pad %d laction_buttons %d lspecial_buttons %d \n",  ldir_pad, laction_buttons, lspecial_buttons);

#if YARMICO_CONTROLLER_ANALOG_TYPE == YARMICO_CONTROLLER_ANALOG_IGNORE

#elif YARMICO_CONTROLLER_ANALOG_TYPE == YARMICO_CONTROLLER_ANALOG_PROCESS_LEFT_STICK_AS_DPAD

	if ( (*(bb0+1) >> 4) == 0x7 || (*(bb0+1) >> 4) == 0x5 ) // analog on
	{
		u8 PAD1rh, PAD1rv, PAD1lh, PAD1lv;
		PAD1rh = *(bb0+4);
		PAD1rv = *(bb0+5);

		PAD1lh = *(bb0+6);
		PAD1lv = *(bb0+7);

		// info from
		// Analog pad tester.  Useful for finding the typical null-zone ranges
		// to prevent unwanted movement in your games.
		// Type SIOCONS AUTO to load and run.

		// by James Shaughnessy james@manc.u-net.com
		// http://www.netyaroze-europe.com/~shaughnj

		// 125 = 0
		// 55 < left or up
		//200 > right or down

		if(PAD1lh < 55) //left dir
			ldir_pad |= YARMICO_CONTROLER_BUT_L;

		else if(PAD1lh > 200) //right dir
			ldir_pad |= YARMICO_CONTROLER_BUT_R;

		if(PAD1lv < 55) //up dir
			ldir_pad |= YARMICO_CONTROLER_BUT_U;

		else if(PAD1lv > 200) //down dir
			ldir_pad |= YARMICO_CONTROLER_BUT_D;



	}


#elif YARMICO_CONTROLLER_ANALOG_TYPE == YARMICO_CONTROLLER_ANALOG_PROCESS_LEFT_AND_RIGHT_STICK_AS_DPAD_AND_FACE_BUTTONS

	{

		//analog sticks based on uni-1 <- 0 -> +1

		s32 PAD1rh, PAD1rv, PAD1lh, PAD1lv;
		PAD1rh = tilt.x *255;
		PAD1rv = tilt.y *255;

		PAD1lh = joystick.x *255;
		PAD1lv = joystick.y *255;






		if(PAD1lh >  YARMICO_ANALOG_DEAD_ZONE)
			ldir_pad |= YARMICO_CONTROLER_BUT_R;// +right

		else if(PAD1lh < -YARMICO_ANALOG_DEAD_ZONE)
			ldir_pad |= YARMICO_CONTROLER_BUT_L; // -left


		if(PAD1lv > YARMICO_ANALOG_DEAD_ZONE)
			ldir_pad |= YARMICO_CONTROLER_BUT_D; // -down

		else if(PAD1lv < -YARMICO_ANALOG_DEAD_ZONE)
			ldir_pad |= YARMICO_CONTROLER_BUT_U; // +up



		if(PAD1rh > YARMICO_ANALOG_DEAD_ZONE)
			laction_buttons |= YARMICO_CONTROLER_BUT_R;// +right

		else if(PAD1rh < -YARMICO_ANALOG_DEAD_ZONE)
			laction_buttons |= YARMICO_CONTROLER_BUT_L; // -left


		if(PAD1rv > YARMICO_ANALOG_DEAD_ZONE)
			laction_buttons |= YARMICO_CONTROLER_BUT_D; // -down

		else if(PAD1rv < -YARMICO_ANALOG_DEAD_ZONE)
			laction_buttons |= YARMICO_CONTROLER_BUT_U; // +up


	}



#elif YARMICO_CONTROLLER_ANALOG_TYPE == YARMICO_CONTROLLER_ANALOG_RETURN_ANALOG_DATA
	{
		yarmico_analog_stick sticks={0};


		{

			//analog sticks based on uni-1 <- 0 -> +1

			s32 PAD1rh, PAD1rv, PAD1lh, PAD1lv;
			PAD1rh = tilt.x *255;
			PAD1rv = tilt.y *255;

			PAD1lh = joystick.x *255;
			PAD1lv = joystick.y *255;






			if(PAD1lh >  YARMICO_ANALOG_DEAD_ZONE)
				sticks.left_stick_x = (PAD1lh)*ONE;// +right

			else if(PAD1lh < -YARMICO_ANALOG_DEAD_ZONE)
				sticks.left_stick_x = (PAD1lh)*-ONE;// -left


			if(PAD1lv > YARMICO_ANALOG_DEAD_ZONE)
				sticks.left_stick_y = (PAD1lv)*-ONE; // -down

			else if(PAD1lv < -YARMICO_ANALOG_DEAD_ZONE)
				sticks.left_stick_y = (PAD1lv)*ONE; // +up



			if(PAD1rh > YARMICO_ANALOG_DEAD_ZONE)
				sticks.right_stick_x = (PAD1rh)*ONE;// +right

			else if(PAD1rh < -YARMICO_ANALOG_DEAD_ZONE)
				sticks.right_stick_x = (PAD1rh)*-ONE;// -left


			if(PAD1rv > YARMICO_ANALOG_DEAD_ZONE)
				sticks.right_stick_y = (PAD1rv)*-ONE; // -down

			else if(PAD1rv < -YARMICO_ANALOG_DEAD_ZONE)
				sticks.right_stick_y = (PAD1rv)*ONE; // +up


			if(analog_stick)
						g_yarmico_controller_analog_sticks=sticks;


		}

	}
#endif

#endif


	g_yarmico_controller_dpad = ldir_pad;

	g_yarmico_controller_action_buttons = laction_buttons;

	g_yarmico_controller_special_buttons=lspecial_buttons;



}

// save to:
// linux:   ~/.local/share/

u32 yarmico_save_data(char *data, u32 size)
{
	LOG_DATA(" yarmico_save_data(), sizeof(t_save_data) %d \n",  size);

	blit::write_save(data, size, 0); // returns void :/
	return 1;


}

u32 yarmico_load_data(char *data,  u32 size)
{
	LOG_DATA("int yarmico_load_data  %d \n",  size);
	return	blit::read_save(data, size, 0);

}

u32 yarmico_delete_data(u32 slot)
{
	LOG_DATA("int yarmico_delete_data  %d \n",  slot);
	return	1;// blit::remove_save(slot);

}

//32blit function
void render(uint32_t time)
{


	screen.alpha = 255;
	screen.pen = blit::Pen(0, 0, 0);

#if HARDWARE_TARGET == HARDWARE_TARGET_SDL2 || HARDWARE_TARGET == HARDWARE_TARGET_32BLIT_HANDHELD

		#if YARMICO_NATIVE_DISPLAY == YARMICO_NATIVE_DISPLAY_120x120
			screen.rectangle( Rect(0,0, 160, YARMICO_SCREEN_HEIGHT)  );// clear actual screen
			game_update();

			#ifdef YARMICO_PICO_SHOW_SIDES
				screen.pen = blit::Pen(64, 64, 64);

				screen.rectangle( Rect(0,0, 20, YARMICO_SCREEN_HEIGHT)  );
				screen.rectangle( Rect(160-20,0, 20, YARMICO_SCREEN_HEIGHT)  );
			#endif

		#else
			screen.rectangle( Rect(0,0, YARMICO_SCREEN_WIDTH, YARMICO_SCREEN_HEIGHT)  );
			game_update();

		#endif //  #if YARMICO_NATIVE_DISPLAY == YARMICO_NATIVE_DISPLAY_120x120

#else
	screen.rectangle( Rect(0,0, YARMICO_SCREEN_WIDTH, YARMICO_SCREEN_HEIGHT)  );
	game_update();
#endif


	yarmico_process_controller_32blit();


#if YARMICO_PICO_SOUND == TRUE
	if(g_yarmico_pico_play_sound.time_frames)
		yarmico_play_pico_sound();
#endif

}


//32blit function
void init() {
	yarmico_init_32blit();



}


//32blit function
void update(uint32_t time) {
	// this is called many times a loop, i don't get why?

	g_ms_start =  time;

	//RefillBuffer(blit::channels[0]);
}



#ifdef __cplusplus
extern "C" {
#endif



void yarmico_process_controller(void)
{
	// C++ function
	yarmico_process_controller_32blit();
}




#ifdef __cplusplus
} // extern "C" {
#endif


