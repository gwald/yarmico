/*
 *
 PUBLIC DOMAIN 2022
 THIS CODE IS A HACKY MESS - READ IT AT YOUR OWN RISK!
 https://github.com/gwald/yarmico
 *
 */

#include "platform_NY.h"

#include "yarmico_API.h"


#define RAND_RAM_START    0xBFC7F000 //0xBFC00000 // start of BIOS area
volatile u8 *g_random =  (volatile u8 *)RAND_RAM_START; //RAND_RAM_START; //0xBFC7FF32; // 0x1F801000; // move point 1 char per inc.




void LoadTexture(u32 address);
void RenderFinish( void);

// #define CODEWORRIOR_DEBUGGING 1


#ifdef CODEWORRIOR_DEBUGGING // used for debugging


#ifdef __MWERKS__ // CodeWarrior for NY precompile define
#define __FUNCTION__ "CW NOT SUPPORTED"
#endif // CodeWarrior for NY

#endif //CODEWORRIOR_DEBUGGING

#include <strings.h>






// from: https://stackoverflow.com/questions/6127503/shuffle-array-in-c
// PSX RANDOM


// fixed to NTSC resolutions, HW will control FPS and 2010+ TV's will display what is output

#define YARMICO_SCREEN_border_COLOUR 0,0,0

#define YARMICO_SCREEN_WIDTH_BORDER_START_240X240_256X240     8
#define YARMICO_SCREEN_HEIGHT_BORDER_START_240X240_256X240	0


#define YARMICO_SCREEN_WIDTH_BORDER_START_320X240_256X240 	0
#define YARMICO_SCREEN_HEIGHT_BORDER_START_320X240_256X240	0

#define YARMICO_SCREEN_WIDTH_BORDER_START_160X120_256X240 	48
#define YARMICO_SCREEN_HEIGHT_BORDER_START_160X120_256X240	60

#define YARMICO_SCREEN_WIDTH_BORDER_START_120X120_256X240 	68
#define YARMICO_SCREEN_HEIGHT_BORDER_START_120X120_256X240	60






#if YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_320X240_640X480 //320x240 up scaled
#define YARMICO_SCREEN_WIDTH	640
#define YARMICO_SCREEN_HEIGHT	480

#elif YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_320X240 //  	320x240 no up scaling

#define YARMICO_SCREEN_WIDTH	320
#define YARMICO_SCREEN_HEIGHT	240


#elif YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_240x240 // 240x240 boarded widths

#define YARMICO_SCREEN_WIDTH	256
#define YARMICO_SCREEN_HEIGHT	240



#elif YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_160x120 // 160x120 boarded widths and heights

#define YARMICO_SCREEN_WIDTH	256
#define YARMICO_SCREEN_HEIGHT	240


#elif YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_120x120 // 120x120 boarded widths and heights

#define YARMICO_SCREEN_WIDTH	256
#define YARMICO_SCREEN_HEIGHT	240


#endif



#define PSX_ANALOG_CENTER 125





#define CLEAR_BG_RED	0
#define CLEAR_BG_GREEN	0
#define CLEAR_BG_BLUE	0



#define INIT_BG_RED		0
#define INIT_BG_GREEN	0
#define INIT_BG_BLUE	0

#define	TRUE			1
#define FALSE			0

#define ORDERING_TABLE_LENGTH	16
#define MAX_NO_PACKETS		64000


long		g_CurrentBuffer;
GsOT		g_TableHeader [2];
GsOT_TAG        TableArray [2][1<<ORDERING_TABLE_LENGTH];
PACKET          Packet_Memory [2][MAX_NO_PACKETS];

//rem not needed volatile u_long PAD,PADBUFFER;
volatile u_char *bb0,*bb1;

#ifdef _DEBUG_
#undef YARMICO_PLAY_BGM
#define YARMICO_PLAY_BGM 0 // 1 YES 0 NO
#endif



// PSX DEFINE's






#ifdef _RELEASE_

#define LOG_RAND_ADD //  printf
#define LOG_TIMS  //  printf
#define LOG_DEL  //  printf
#define LOG_DISP //   printf
#define LOG_FUNC //  printf
#define LOG_PAD  //   printf
#define LOG_CDAUDIO  //  printf
#define LOG_DATA // printf
#define LOG_MAIN //printf

#else

#define LOG_DATA    // printf
#define LOG_RAND_ADD  // printf
#define LOG_RAND  // printf
#define LOG_TIMS   // printf
#define LOG_DEL   // printf
#define LOG_DISP   //  printf
#define LOG_FUNC  // printf
#define LOG_PAD    // printf
#define LOG_CDAUDIO   // printf
#define LOG_MAIN  //printf


#endif



//from SCEI NY card example code
// file: cardio.h
//	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.

typedef	struct	{
	int	fileEntry;
	char	fileName[15][64];
} _FINF;

typedef struct {
	char	Magic[2];
	char	Type;
	char	BlockEntry;
	char	Title[64];
	char	reserve[28];
	char	Clut[32];
	char	Icon[3][128];
} _CARD;

//from SCEI NY card example code

//NOTE: CW doesn't like TestCard and halts
// #define TestCard 1;//



#define CDAUDIO_MAX 100
#define CDAUDIO_MIN 80

// reverse PSX OT draw order (painters algo) so it's similar to 32blit
#define OT_POS_RECT    6000
#define OT_POS_SPRITE  2000



u32 g_OT_POS_CNT;



#ifdef USE_TPAGE00
GsSPRITE g_TPAGE00;
#endif


#ifdef USE_TPAGE01
GsSPRITE g_TPAGE01;
#endif


#ifdef USE_TPAGE02
GsSPRITE g_TPAGE02;
#endif



#ifdef USE_TPAGE03
GsSPRITE g_TPAGE03;
#endif


#ifdef USE_TPAGE04
GsSPRITE g_TPAGE04;
#endif

#ifdef USE_TPAGE05
GsSPRITE g_TPAGE05;
#endif




GsBOXF g_box={0};




#define SAVE_CHECK_STRING_START   YARMICO_PROJECT_TITLE
#define SAVE_CHECK_STRING_END     YARMICO_PROJECT_TITLE


//memcard code from SCEI NY card example code
#define MEMCARD_DEVNAME "bu10:"
char *g_memcard_filename=MEMCARD_DEVNAME YARMICO_PROJECT_CODE_STRING;


void exit_to_siocons(void);


void _error()
{
	printf("memcard error no. _get_errno:%d\n", _get_errno());
	// see #include "sys/errno.h"
}





u32 yarmico_save_data( char *data, u32 size)
{
#ifndef __MWERKS__
	//	struct DIRENTRY d;

	long  chan;
	long n=10;
	u_long flag=0;
	char  buf[512+128]; //_CARD + t_save_data
	s32 fd;
	//, ret;
	_CARD *card;
	//long i;




	/* 4bit 16x16 ball pattern */
	u32	ball16x16[] = {
			0x00000000,0x00000000,0x21000000,0x00000332,
			0x42110000,0x00055554,0x54321000,0x00677665,
			0x65432100,0x07888776,0x76543100,0x09998887,
			0x77654210,0x9aa99988,0x87765420,0xabbbaa99,
			0x98765420,0xbcccbba9,0x98876530,0xcddddcba,
			0xa9876530,0xdeeeddcb,0xa9887500,0x0effeddb,
			0xb9987500,0x0efffedc,0xba986000,0x00fffedc,
			0xba970000,0x000eeedc,0xa9000000,0x00000dcb
	};



	u32	ballcolor[8] = {
			0x80860000,0x80c980c8,0x812c810a,0x816e814d,
			0x81b18190,0x81f481d3,0x821581f4,0x82378216
	};





	// try to read new bigger new format first, it will fail if it's the old smaller format
#ifndef __MWERKS__
	fd = TestCard(1); // slot 2
#else
	fd = 0; // slot 2
#endif
	/*
	0: Card missing
	1: Card present
	2: New card detected
	4: Uninitialized card detected
	 */




	if(fd == 0)
	{
		LOG_DATA(" _create() failed TestCard -  Card missing\n");
		return 0;  // failed 0: Card missing
	}






	// try formating it first
	if(fd > 1)
	{

		char name[32];


		LOG_DATA(" amico_load_data() TestCard 2 or 4 -  Card needs formating\n");

		fd = format(MEMCARD_DEVNAME);

		if(fd == 0)
		{
			LOG_DATA("formating failed on %s format(DevName)\n", MEMCARD_DEVNAME);
			return 0;// failed
		}
		else
		{
			LOG_DATA("formating OK %s format(DevName)\n", MEMCARD_DEVNAME);
		}
	}




	memcpy( &buf[512],data,128);






	fd = open(g_memcard_filename, O_CREAT|(1<<16));


	if(fd<0)
	{

		LOG_DATA("%s just do WO opening\n", g_memcard_filename);
		fd = open(g_memcard_filename, O_WRONLY );

		if(fd<0)
		{
			LOG_DATA(" g_memcard_FD _error %s \n", g_memcard_filename);
			_error();
			return 0;
		}
	}

	LOG_DATA("%s closing\n", g_memcard_filename);
	close(fd);


	LOG_DATA("%s WO opening\n", g_memcard_filename);
	fd = open(g_memcard_filename, O_WRONLY);


	LOG_DATA("create file\n\n", g_memcard_filename);
	//LOG_DATA("DIRENTRY 40 size - %d\n\n", sizeof(struct DIRENTRY) ); // not used CBS maintaining directories
	LOG_DATA("_CARD 512 size - %d\n\n", sizeof(_CARD) );
	LOG_DATA("g_saved_data 128 size - %d\n\n", size );


	LOG_DATA("create new %s\n", g_memcard_filename);


	// assumed card/file already opened and working!

	LOG_DATA("%s lseek(fd,0,SEEK_SET);\n", g_memcard_filename);
	lseek(fd,0,SEEK_SET);



	card=(_CARD *)buf;
	card->Magic[0]='S';
	card->Magic[1]='C';
	card->Type=0x11;
	card->BlockEntry=1;
	strcpy(card->Title, YARMICO_PROJECT_CODE_STRING);
	//strcat(card->Title, file_no);
	memcpy(card->Clut,ballcolor,32);
	memcpy(card->Icon[0],ball16x16,128);
	memcpy(card->Icon[1],ball16x16,128);
	write(fd,buf,512+128);
	return close(fd);
#else
	return 0;
#endif
}

// assumed card already open and file created
u32 yarmico_load_data(char *data, u32 size)
{
#ifndef __MWERKS__
	//	struct DIRENTRY d;

	char  buf[512+128]; //_CARD + t_save_data
	s32 fd;//, ret;
	//_CARD *card;
	long i;


	//LOG_DATA("DIRENTRY 40 size - %d\n\n", sizeof(struct DIRENTRY) ); // not used CBS maintaining directories
	LOG_DATA("g_memcard_filename %s\n\n", g_memcard_filename );
	LOG_DATA("g_saved_data 128 size - %d\n\n", size );


	// try to read new bigger new format first, it will fail if it's the old smaller format
#ifndef __MWERKS__
	fd = TestCard(1); // slot 2
#else
	fd = 0; // slot 2
#endif
	/*
	0: Card missing
	1: Card present
	2: New card detected
	4: Uninitialized card detected
	 */

	if(fd != 1)
	{
		LOG_DATA(" amico_load_data() failed TestCard\n");
		return 0; // card missing or not formated etc
	}


	// fd = open(g_memcard_filename, O_CREAT|(1<<16));
	fd = open(g_memcard_filename, O_RDONLY );

	if(fd<0)
	{
		LOG_DATA(" g_memcard_FD _error %d \n", fd);
		_error();
		return 0;
	}

	LOG_DATA("g_memcard_FD %d lseek(fd,0,SEEK_SET);\n", fd);
	lseek(fd,0,SEEK_SET);


	i = read(fd,buf,512+128);


	LOG_DATA("%d = read(fd,buf,512+128);\n", i);
	memcpy( data,&buf[512], 128);
	// no closing close(g_memcard_FD);



	return close(fd);
#else
	return 0;
#endif
}


u32 yarmico_delete_data(u32 slot)
{

#ifndef __MWERKS__
	u32 ret = TestCard(1); // slot 2
	if(ret == 1)
		return delete( g_memcard_filename	);
#else

#endif

	return 1;
}





// Spirit loading, display res and rendering code mostly 'borrowed' from:
// lib2dh.c
// Hi-res version
// Complete 2D development library
// High performance macros & functions
// Compiled by Harvey Cotton


volatile u_char *bb0,*bb1;


void InitSprite(GsSPRITE *sprite, u32 address, u32 x1, u32 y1)
{
	u_long depth1,depth2;
	GsIMAGE tim;


	printf("InitSprite %#010x\n", address);

	LoadTexture(address);

	printf("InitSprite GsGetTimInfo %#010x\n", address);
	GsGetTimInfo((u_long *)(address+4),&tim);
	depth2=4>>(depth1=tim.pmode&3);

	printf("InitSprite depth2 %d \n", depth2);

	printf("InitSprite tim.pw %d \n", tim.pw);
	printf("InitSprite tim.ph %d \n", tim.ph);

	sprite->attribute=depth1<<24;
	sprite->x=x1;
	sprite->y=y1;
	sprite->w=tim.pw*depth2;
	sprite->h=tim.ph;
	sprite->tpage=GetTPage(depth1,0,tim.px,tim.py);
	sprite->u=tim.px%64;
	sprite->v=tim.py%256;

	if (depth1!=2)
	{
		sprite->cx=tim.cx;
		sprite->cy=tim.cy;
	}

	sprite->r=sprite->g=sprite->b=127;
	sprite->mx=(tim.pw*depth2)>>1;
	sprite->my=tim.ph>>1;
	sprite->scalex=sprite->scaley=ONE;
	sprite->rotate=0;

	printf("\n end InitSprite \n");
}
// Called before everything is drawn
void RenderPrepare ()
{
	LOG_PAD("\n g_CurrentBuffer %d \n", g_CurrentBuffer);
	//g_CurrentBuffer=GsGetActiveBuff();
	g_CurrentBuffer=!g_CurrentBuffer;
	LOG_PAD("\n GsGetActiveBuff %d \n", g_CurrentBuffer);
	GsSetWorkBase((PACKET*)Packet_Memory[g_CurrentBuffer]);
	LOG_PAD("\n GsSetWorkBase %d \n", g_CurrentBuffer);
	GsClearOt(0,0,&g_TableHeader[g_CurrentBuffer]);
	LOG_PAD("\n GsClearOt %d \n", g_CurrentBuffer);


#if YARMICO_NATIVE_DISPLAY == YARMICO_NATIVE_DISPLAY_120x120 || YARMICO_NATIVE_DISPLAY == YARMICO_NATIVE_DISPLAY_160x120
#ifdef YARMICO_PICO_SHOW_SIDES

	if(g_yarmico_lores_upscale[0] == 0) //do boarders
	{

		g_box.r = 0;
		g_box.g = 0;
		g_box.b = 0;
		g_box.y = g_box.x = 0;

		g_box.w =  g_yarmico_lores_upscale[1]; // width
		g_box.h = 240;


		GsSortBoxFill(&g_box, &g_TableHeader[g_CurrentBuffer],  1000);


		g_box.x =  256-g_yarmico_lores_upscale[1]; // width
		GsSortBoxFill(&g_box, &g_TableHeader[g_CurrentBuffer], 1000);



		g_box.x =0;

		g_box.w =  256;
		g_box.h = g_yarmico_lores_upscale[2]; // hight


		GsSortBoxFill(&g_box, &g_TableHeader[g_CurrentBuffer],  1000);


		g_box.y =  240-g_yarmico_lores_upscale[2]; // width
		GsSortBoxFill(&g_box, &g_TableHeader[g_CurrentBuffer],  1000);
	}

#endif //#ifdef YARMICO_PICO_HIDE_SIDES
#endif //#if YARMICO_NATIVE_DISPLAY == YARMICO_NATIVE_DISPLAY_120x120 || YARMICO_NATIVE_DISPLAY == YARMICO_NATIVE_DISPLAY_160x120

}


// display code borrowed from lib2dh
// Hi-res version
// Complete 2D development library
// High performance macros & functions
// Compiled by Harvey Cotton

// Initialises everything
void InitialiseGraphics (void)
{
	// Setup graphics mode
	//RenderClear();
	// SetVideoMode(VIDEO_MODE); // moved into main()

	LOG_MAIN("\n InitialiseGraphics  YARMICO_SCREEN_WIDTH %d YARMICO_SCREEN_HEIGHT %d \n", YARMICO_SCREEN_WIDTH, YARMICO_SCREEN_HEIGHT);


	printf("\n BIOS found: [%s]", PLAYSTATION_BIOS_INFO() );
	printf("\n Using hardware region: %c\n", PLAYSTATION_REGION_CHECK() );


	if( PLAYSTATION_REGION_CHECK()  == 'E') // if true, it's PAL
	{
		SetVideoMode(MODE_PAL); // 1=PAL/50Hz with NTSC res

		StartRCnt(0); // restart counter to cnter make smaller

		printf("*WARNING*\n");
		printf("PAL PSX detected, using PAL (50Hz) with NTSC resolution!!\n\n");

	}
	else
	{
		// slow down to get new seed



		SetVideoMode(MODE_NTSC); // 0=NTSC/60Hz
		// SetVideoMode(MODE_PAL);  // force PAL mode // 1=PAL/50Hz with NTSC res
		printf("NTSC PSX detected.\n");






		VSync(0);
		DrawSync(0);
	}



#if YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_320X240_640X480
	GsInitGraph(YARMICO_SCREEN_WIDTH,YARMICO_SCREEN_HEIGHT,GsINTER+GsOFSGPU,0,0);
	GsDRAWENV.isbg=1;
	GsDRAWENV.dfe=0;
	GsDISPENV.isinter=1;
	GsDISPENV.isrgb24=0;
#else
	GsInitGraph(YARMICO_SCREEN_WIDTH,YARMICO_SCREEN_HEIGHT,GsOFSGPU,1,0);
	GsDefDispBuff(0, 0, 0, YARMICO_SCREEN_HEIGHT);

#endif

	g_yarmico_lores_upscale[0] = YARMICO_DEFAULT_DISPLAY_UPSCALE;





	if(YARMICO_HEIGHT == 120)
	{
		if(g_yarmico_lores_upscale[0]) // if scale up to 256X240
		{
			if(YARMICO_WIDTH == 160) // 160 to 256 width
			{
				g_yarmico_lores_upscale[0] = 6553; //fixed point scale factor 160* 1.6 = 256 so  1.6 * ONE = 6553.6
				g_yarmico_lores_upscale[1] = 0;
				g_yarmico_lores_upscale[2] = 0;
			}
			else //120 to 256 width
			{

				g_yarmico_lores_upscale[0] = 8704; //fixed point scale factor 120\D72.133333   =255.99996  4096\D72.133333  = 8738.131968 - 8704%32=0
				g_yarmico_lores_upscale[1] = 0;
				g_yarmico_lores_upscale[2] = 0;
			}
		}
		else // use big borders
		{
			if(YARMICO_WIDTH == 160)
			{
				g_yarmico_lores_upscale[1] = YARMICO_SCREEN_WIDTH_BORDER_START_160X120_256X240;
				g_yarmico_lores_upscale[2] = YARMICO_SCREEN_HEIGHT_BORDER_START_160X120_256X240;
			}
			else //120
			{
				g_yarmico_lores_upscale[1] = YARMICO_SCREEN_WIDTH_BORDER_START_120X120_256X240;
				g_yarmico_lores_upscale[2] = YARMICO_SCREEN_HEIGHT_BORDER_START_120X120_256X240;
			}
		}
	}






	GsDRAWENV.r0=INIT_BG_RED;
	GsDRAWENV.g0=INIT_BG_GREEN;
	GsDRAWENV.b0=INIT_BG_BLUE;

	/*
	GsDRAWENV.clip.x=0;
	GsDRAWENV.clip.y=0;
	GsDRAWENV.clip.w=YARMICO_SCREEN_WIDTH;
	GsDRAWENV.clip.h=YARMICO_SCREEN_HEIGHT;

	GsDRAWENV.r0=BG_RED;
	GsDRAWENV.g0=BG_GREEN;
	GsDRAWENV.b0=BG_BLUE;

	GsDRAWENV.isbg=1;
	GsDRAWENV.dfe=0;

	GsDISPENV.disp.x=0;
	GsDISPENV.disp.y=0;
	GsDISPENV.disp.w=YARMICO_SCREEN_WIDTH;
	GsDISPENV.disp.h=YARMICO_SCREEN_HEIGHT;

	GsDISPENV.screen.x=5;
	GsDISPENV.screen.w=HALF_WIDTH;
	GsDISPENV.screen.h=HALF_HEIGHT;

	GsDISPENV.isinter=1;
	GsDISPENV.isrgb24=0;
	 */

	// Initialise ordering table
	g_TableHeader[0].length=ORDERING_TABLE_LENGTH;
	g_TableHeader[1].length=ORDERING_TABLE_LENGTH;
	g_TableHeader[0].org=TableArray[0];
	g_TableHeader[1].org=TableArray[1];

	// Initialise font - not used
	//FntLoad(960,256);

	// Initialise control pads
	// GetPadBuf(&bb0,&bb1); // moved into main()
}


// Transfer a texture from memory to video ram
void LoadTexture(u32 address)
{
	GsIMAGE tim;
	RECT rect;

	printf("LoadTexture %#010x\n", address);

	printf("GsGetTimInfo *(address+4) = %#010x\n", *(u32 *)(address+4) );
	DrawSync(0);
	GsGetTimInfo((u_long *)(address+4),&tim);

	rect.x=tim.px;
	rect.y=tim.py;
	rect.w=tim.pw;
	rect.h=tim.ph;

	printf("LoadImage %#010x\n", address);
	LoadImage(&rect,tim.pixel);
	printf("DrawSync %#010x\n", address);
	DrawSync(0);

	if ((tim.pmode>>3)&0x01)
	{
		rect.x=tim.cx;
		rect.y=tim.cy;
		rect.w=tim.cw;
		rect.h=tim.ch;
		printf("LoadImage clut %#010x\n", address);
		LoadImage(&rect,tim.clut);
		printf("DrawSync %#010x\n", address);
		DrawSync(0);
	}

	printf("DrawSync %#010x\n", address);
	//DrawSync(0);

	printf("LoadTexture exit %#010x\n", address);
}


// Wipes screen and ordering tables
void RenderClear ()
{
	RECT temp={0,0,YARMICO_SCREEN_WIDTH,YARMICO_SCREEN_HEIGHT};

	GsDRAWENV.r0=0;
	GsDRAWENV.g0=0;
	GsDRAWENV.b0=0;

	ClearImage(&temp,CLEAR_BG_RED,CLEAR_BG_GREEN,CLEAR_BG_BLUE);
	GsClearOt(0,0,&g_TableHeader[0]);
	GsClearOt(0,0,&g_TableHeader[1]);
	GsSortClear(CLEAR_BG_RED,CLEAR_BG_GREEN,CLEAR_BG_BLUE,&g_TableHeader[0]);
	GsSortClear(CLEAR_BG_RED,CLEAR_BG_GREEN,CLEAR_BG_BLUE,&g_TableHeader[1]);
	DrawSync(0);
	//ResetGraph(0);

	RenderFinish();

	ClearImage(&temp,CLEAR_BG_RED,CLEAR_BG_GREEN,CLEAR_BG_BLUE);
	GsClearOt(0,0,&g_TableHeader[0]);
	GsClearOt(0,0,&g_TableHeader[1]);
	GsSortClear(CLEAR_BG_RED,CLEAR_BG_GREEN,CLEAR_BG_BLUE,&g_TableHeader[0]);
	GsSortClear(CLEAR_BG_RED,CLEAR_BG_GREEN,CLEAR_BG_BLUE,&g_TableHeader[1]);
	// DrawSync(0);

	//issues with the pad data?
	//	bb0 = bb1 = (volatile u_char *) NULL;
	//	GetPadBuf(&bb0,&bb1);
}
// Called after everything is drawn
void RenderFinish( void)
{
	//FntPrint(0, "\n VSYNC:%d\n",VSync(0));

#if YARMICO_SCREEN_WIDTH != 640
	VSync(0);
	DrawSync(0);
#endif
	//ResetGraph(1);
	GsSwapDispBuff();
	//clear automatically done via GsDRAWENV
#if YARMICO_SCREEN_WIDTH != 640
	GsSortClear(CLEAR_BG_RED,CLEAR_BG_GREEN,CLEAR_BG_BLUE,&g_TableHeader[g_CurrentBuffer]);
#endif
	GsDrawOt(&g_TableHeader[g_CurrentBuffer]);
	//  FntFlush(-1);

	g_OT_POS_CNT=0;
}

// save to:







void yarmico_rectangle(u8 r, u8 g, u8 b, u32 screen_posX, u32 screen_posY, u32 w, u32 h )
{

	g_box.r = r;
	g_box.g = g;
	g_box.b = b;




	{ // SCREEN RES / SCALING
#if YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_320X240_640X480     //  	 up scaling
		g_box.x = screen_posX << 1; //times 2 for hi res
		g_box.y = screen_posY << 1; //times 2 for hi res


		g_box.w = w << 1; //times 2 for hi res
		g_box.h = h << 1; //times 2 for hi res



#elif YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_320X240//  	no scaling
		tim->x = screen_posX;
		tim->y = screen_posY;

		g_box.w = w;
		g_box.h = h;

#elif YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_240x240//  	no scaling with borders
		g_box.x = screen_posX ;
		g_box.y = screen_posY;
		g_box.x +=YARMICO_SCREEN_WIDTH_BORDER_START_240X240_256X240;
		g_box.y +=YARMICO_SCREEN_HEIGHT_BORDER_START_240X240_256X240;


		g_box.w = w;
		g_box.h = h;


#else

		if(g_yarmico_lores_upscale[0]==0)// no scaling up
		{
			g_box.x = screen_posX;
			g_box.y = screen_posY;
			g_box.x +=g_yarmico_lores_upscale[1];
			g_box.y +=g_yarmico_lores_upscale[2];


			g_box.w = w;
			g_box.h = h;
		}
		else
		{

			g_box.x = (screen_posX *g_yarmico_lores_upscale[0])/ONE;//(screen_posX *g_yarmico_lores_upscale[0]); //times to 256 W
			g_box.y = screen_posY << 1; //times 2 to 240



			// g_box.w = (w *g_yarmico_lores_upscale[0]); //times 2 for hi res
			g_box.w = (w *g_yarmico_lores_upscale[0])>>12; // /ONE; //times 2 for hi res
			g_box.h = (h << 1) ; //times 2 for hi res

		}

#endif
	} // SCREEN RES / SCALING

#if 0 //old
	//g_yarmico_scale
#if YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_320X240_640X480//  	 up scaling
	g_box.x = screen_posX << 1; //times 2 for hi res
	g_box.y = screen_posY << 1; //times 2 for hi res

	g_box.w = w << 1; //times 2 for hi res
	g_box.h = h << 1; //times 2 for hi res
#else

	g_box.x = screen_posX ; //times 2 for hi res
	g_box.y = screen_posY; //times 2 for hi res

	g_box.w = w ; //times 2 for hi res
	g_box.h = h ; //times 2 for hi res


#if DISPLAY_TARGET > YARMICO_HIRES1 // boarded
	g_box.x +=YARMICO_SCREEN_WIDTH_START;
	g_box.y +=YARMICO_SCREEN_HEIGHT_START;
#endif
#endif //old

#endif



	GsSortBoxFill(&g_box, &g_TableHeader[g_CurrentBuffer],  OT_POS_RECT-g_OT_POS_CNT);
	g_OT_POS_CNT++;
	LOG_DISP("\n GsSortBoxFill  yarmico_rectangle yarmico_rectangle yarmico_rectangle %d \n",!(w&1));
}

void yarmico_sprite(u8 tpage, u32 texture_u, u32 texture_v, u32 texture_w, u32 texture_h, s32 posX, s32 posY, s32 scale , u8 h_flip, u8 v_flip)
{
	GsSPRITE *tim;
	u32 scale_flip;
	LOG_DISP("yarmico_sprite func: TPAGE %d texture_u %d texture_v %d  texture_w  %d texture_h  %d posX  %d posY  %d scale  %d \n", tpage, texture_u,  texture_v ,  texture_w ,  texture_h ,  posX ,  posY ,  scale	);

	//scale *=2;
	//scale -= 100;


	scale_flip = 0;

	switch( tpage)
	{

#ifdef USE_TPAGE00
	case 0:
		tim = &g_TPAGE00;
		break;
#endif // #ifdef USE_TPAGE00


#ifdef USE_TPAGE01

	case 1:
		tim = &g_TPAGE01;
		break;

#endif



#ifdef USE_TPAGE02

	case 2:
		tim = &g_TPAGE02;
		break;
#endif


#ifdef USE_TPAGE03

	case 3:
		tim = &g_TPAGE03;
		break;
#endif



#ifdef USE_TPAGE04

	case 4:
		tim = &g_TPAGE04;
		break;
#endif



#ifdef USE_TPAGE05

	case 5:
		tim = &g_TPAGE05;
		break;
#endif


	default:
		printf("error no TPAGE defined\n");
		exit(1);

	}





	tim->u = (unsigned char) texture_u;
	tim->v = (unsigned char) texture_v;
	tim->w = (unsigned short) texture_w;
	tim->h = (unsigned short) texture_h;



#if 0 // use sprites centers
	tim->mx = (signed short) (tim->w/2);
	tim->my = (signed short) (tim->h/2);
#else // use sprite top left corner
	tim->mx = 0;
	tim->my = 0;
#endif

#if YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_320X240_640X480//  	 up scaling
	tim->x = posX << 1; //times 2 for hi res
	tim->y = posY << 1; //times 2 for hi res

	scale *= 2; //scale one *2

	tim->scalex = tim->scaley = (signed short)  scale;

#elif YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_320X240//  	no scaling
	tim->x = posX ;
	tim->y = posY;

	tim->scalex = tim->scaley = (signed short)  scale;

#elif YARMICO_NATIVE_DISPLAY==YARMICO_NATIVE_DISPLAY_240x240//  	no scaling with borders
	tim->x = posX ;
	tim->y = posY;
	tim->x +=YARMICO_SCREEN_WIDTH_BORDER_START_240X240_256X240;
	tim->y +=YARMICO_SCREEN_HEIGHT_BORDER_START_240X240_256X240;

	scale *= ONE; // one
	scale /= 100;


	if(scale > 32767)
		scale = 32767;

	tim->scalex = tim->scaley = (signed short)  scale;


#else

	if(g_yarmico_lores_upscale[0]==0)// no scaling up
	{
		tim->x = posX + g_yarmico_lores_upscale[1];
		tim->y = posY +g_yarmico_lores_upscale[2];

		tim->scalex = tim->scaley = (signed short)  scale;
	}
	else
	{
		s32 scale_y;
		scale_y = scale;


		scale *= g_yarmico_lores_upscale[0]; //scale width ONE + difference
		scale >>= ONE_SHIFT; // / ONE;

		scale_flip = 1; // should be something more complicated, but 1 is fine for double

		tim->x = (posX * scale) >> ONE_SHIFT; // / ONE; //times to 256 W
		tim->y = (posY << 1)-1 ; //times 2 to 240 and -1 ??

		tim->scalex = (signed short)  scale;

		scale_y <<= 1;   // ONE*2
		tim->scaley = (signed short)  scale_y;

		// TODO: hack, I don't know why resize requires +1 with scaling??
		// tim->w++;
		// tim->h++;


	}


#endif




	if(h_flip)
	{


		if(scale_flip ==0)
		{
			// i don't know why this is needed?
			// Subject: weird pixel vanishing in sprite
			// Date: 19 Jul 2001 18:03:57 GMT
#if 1
			tim->u--;
			tim->w++;
			tim->h++;
			tim->scalex -=512;
			tim->scaley -=256;
#endif

		}

		tim->scalex = -tim->scalex;	 //	to flip the sprite left->right
		tim->x += tim->w << scale_flip; // move to the right
	}

	if(v_flip)
	{
		tim->scaley = -tim->scaley;	 //	to flip the sprite up->down
		tim->y += tim->h << scale_flip; // move down

		if(scale_flip ==0)
		{
			// i don't know why this is needed?
			tim->y+=1;
			tim->w+=1;
			tim->scaley--;
		}
	}


	if( v_flip+h_flip+scale == ONE) // no flipping or scaling, use fast sprite
		GsSortFastSprite(tim,&g_TableHeader[g_CurrentBuffer],OT_POS_SPRITE-g_OT_POS_CNT);	// register the sprite in the ordering table
	else
		GsSortSprite(tim,&g_TableHeader[g_CurrentBuffer],OT_POS_SPRITE-g_OT_POS_CNT);	// register the sprite in the ordering table

	g_OT_POS_CNT++;


	LOG_DISP("\n %0xd \n", tpage);
	// GsSortFastSprite(&g_TPAGE04,&g_TableHeader[g_CurrentBuffer], 100);
	/*
	 *
	 * pri is the priority (i.e. position) of the sprite in the ordering table. The highest priority is obtained by setting pri to 0. The lowest priority is determined by the size of the ordering table. If the value of pri is greater than the size of the ordering table, it is clipped to the maximum size.
	 *
	 */
	LOG_DISP("\n func: yarmico_sprite END \n" );
}


#if 0
void yarmico_text_init(void)
{
	int i;
	LOG_DEL("\n initFont\n");
	// chars are 16x16
	//0 - 2 lines x 32 chars - top left (text is always left aligned
	// int FntOpen (    int x,    int y,    int w,    int h,    int isbg,    int n ) // n = Maximum number of characters that can be drawn
	//0 top line
	i = FntOpen(0,8+PSX_CLIPPED_TOP, 160*4,1*16, 0,160);
	LOG_DEL("FntOpen %d\n",i);
	//1 top line 2
	i = FntOpen(0,16+8+PSX_CLIPPED_TOP, 160*4,1*16, 0,160);
	LOG_DEL("FntOpen %d\n",i);
	// screen width 80 chars
	// FntPrint(0 , "12345678901234567890123456789012345678901234567890123456789012345678901234567890\n");

	//1 - 2 lines center (text is always left aligned
	//ret =  FntOpen(QUARTER_WIDTH-64,HALF_HEIGHT-64,128,32,1,64);

	// 2 "PRESS A" 		// FntPrint(1 , "~c0f0PRESS A\n");
	i = FntOpen(HALF_WIDTH-3*16,HALF_HEIGHT+16, YARMICO_SCREEN_WIDTH,2*16,0,160);
	LOG_DEL("FntOpen %d\n",i);
	//
	//2 - 2 lines center (text is always left aligned
	//ret =  FntOpen(HALF_WIDTH,HALF_HEIGHT,128,32,1,64);
	//LOG_DEL("FntOpen %d\n",ret);


#if 0
	// done once only! g_TPAGE01
	for(i=0; i< MAX_SHILLS; i++) 
	{
		// init and reverse order
		LOG_DEL("%d g_shill_list %s \n",i, g_texture_map_shills[i].name);

		switch( (u32) g_texture_map_shills[i].TPAGE)
		{
		case 1:
			g_texture_map_shills[i].TPAGE = &g_TPAGE01;
			break;
		case 2:
			g_texture_map_shills[i].TPAGE = &g_TPAGE02;
			break;
		case 3:
			g_texture_map_shills[i].TPAGE = &g_TPAGE03;
			break;
		case 4:
			g_texture_map_shills[i].TPAGE = &g_TPAGE04;
			break;
		}

		LOG_DISP("%d g_shill_list %s \n",i, g_texture_map_haters[i].name);
		switch( (u32) g_texture_map_haters[i].TPAGE)
		{
		case 1:
			g_texture_map_haters[i].TPAGE = &g_TPAGE01;
			break;
		case 2:
			g_texture_map_haters[i].TPAGE = &g_TPAGE02;
			break;
		case 3:
			g_texture_map_haters[i].TPAGE = &g_TPAGE03;
			break;
		case 4:
			g_texture_map_haters[i].TPAGE = &g_TPAGE04;
			break;
		}

		LOG_DISP("%d g_texture_map_shills %d \n",i, g_texture_map_shills[i].TPAGE);
		LOG_DISP("%d g_texture_map_haters %d \n",i, g_texture_map_haters[i].TPAGE);

		LOG_DISP("\n \n");
	}
#endif
	LOG_DISP("\n initFont - EXIT\n");
}

#endif


#define YARMICO_MAIN_CONTROLLER_BUTTONS g_dpad

#define YARMICO_MAIN_CONTROLLER_START PADLstart //Button::MENU
#define YARMICO_MAIN_CONTROLLER_SELECT PADLselect //Button::MENU
#define YARMICO_MAIN_CONTROLLER_TRIANGLE  PADLtriangle //YARMICO_MAIN_CONTROLLER_BUTTONS & Button::X) // up
#define YARMICO_MAIN_CONTROLLER_CROSS  PADLcross //YARMICO_MAIN_CONTROLLER_BUTTONS & Button::B) // down
#define YARMICO_MAIN_CONTROLLER_CIRCLE  PADLcircle // YARMICO_MAIN_CONTROLLER_BUTTONS & Button::A) // right
#define YARMICO_MAIN_CONTROLLER_SQUARE  PADLsquare // YARMICO_MAIN_CONTROLLER_BUTTONS & Button::Y) // left
#define YARMICO_MAIN_CONTROLLER_DPAD_UP   PADLup // YARMICO_MAIN_CONTROLLER_BUTTONS & Button::DPAD_UP)
#define YARMICO_MAIN_CONTROLLER_DPAD_DOWN  PADLdown // YARMICO_MAIN_CONTROLLER_BUTTONS & Button::DPAD_DOWN)
#define YARMICO_MAIN_CONTROLLER_DPAD_RIGHT   PADLright // buttonsYARMICO_MAIN_CONTROLLER_BUTTONS & Button::DPAD_RIGHT)
#define YARMICO_MAIN_CONTROLLER_DPAD_LEFT   PADLleft // buttonsYARMICO_MAIN_CONTROLLER_BUTTONS & Button::DPAD_LEFT)



#define MAIN_VOL 127





void SoundInit(void)
{

	int vabID;

	g_stereo_counter_random =0;
	//newsgroups/scee/yaroze/programming/sound/Which_function_Plays_Sound_effects





#ifdef SNDVH_DATA

	LOG_AUDIO("\n SoundInit SNDVH_DATA %llx", SNDVH_DATA);

	LOG_AUDIO("\n SoundInit SNDVB_DATA %llx", SNDVB_DATA);



	VSync(0);

	DrawSync(0);



	LOG_AUDIO("\n SoundInit sync ");



	LOG_AUDIO("\n SoundInit SNDVB_DATA SsVabTransfer ");

	vabID = SsVabTransfer( (unsigned char *)SNDVH_DATA, (unsigned char *)SNDVB_DATA,  -1, 1);  // open sound stuff


	VSync(0);
	DrawSync(0);




	LOG_AUDIO("\n\n SoundInit SFX vab_id :%d", vabID);


	if (vabID < 0)
	{

		LOG_AUDIO("\n SoundInit  SFX vabID :%d\n", vabID);

		LOG_AUDIO("\n SoundInit  SFX: VAB fail (LINE:%d)(FILE:%s)\n",     __LINE__,__FILE__);


		// TODO: NY exits when reloading without clean data in RAM
		// exit(1);



	}



#endif


	VSync(0);
	DrawSync(0);


	// set reverb

	//	SetupReverb();



	// set main volume

	SsSetMVol(MAIN_VOL,MAIN_VOL);



	LOG_AUDIO("\n SoundInit finished");




}



void yarmico_play_BGM_audio(int side)
{

	//	Start CD playing audio
#if YARMICO_PLAY_BGM==1

	//static int	track[] = {2,3,4, 0}; //single track - zero is ending null, 1 is data track, 2 is first audio track
	static int	track[] = {2,3,4,5,6,7,8, 0}; //play whatever is on the CD?



	LOG_CDAUDIO( "yarmico_play_BG_AUDIO side %d\n", side );

	// tracks     0ffset
	// 2 haters     0
	// 3 shills     1
	// 4 intro      2

	/* Wait for music to stop - give it some time, will you ? */
	VSync(0);
	VSync(0);





	CdPlay( 0,0,0 ); //stop

	SsSetSerialAttr( SS_CD,SS_REV,SS_SOFF );
	SsSetSerialAttr( SS_CD,SS_MIX,SS_SON );

	//{2,3,4, 0};
	if(side == SIDE_SHILL)
	{
		side = 1; // shills start on last audio track (offset of 3)

	}
	else if(side == SIDE_HATER)
	{

		side = 2; // hater track start on first audio track (offset of 2)

	}
	else // play intro
	{

		side = 0; // first audio track (offset of 2)

	}



	SsSetSerialVol( SS_CD,g_SET_BGM_VOL,g_SET_BGM_VOL ); // full audio


	/*
	 *
0  Stop playback immediately.
1  The tracks specified by tracks are played consecutively, and playback is stopped when all the specified tracks have been played.
2  The tracks specified by tracks are played consecutively, and playback is returned to the start and repeated when all the specified tracks have been played.
3  The index of the tracks array for the track currently being played is returned. (No other action is taken.)
	 *
	 */

	CdPlay(2, track, side); // mode (2) - always repeat tracks, always start at 0 (first audio track)

	/* more wait for music to stop - give it some time, will you ? */
	VSync(0);
	VSync(0);

#endif





	LOG_CDAUDIO( "yarmico_play_BG_AUDIO side %d FINISHED\n", side );

}





void yarmico_BGM_quiet(int quiet) // 1 true reduce vol - 0 false increase back to normal BG volume
{

	if(g_SET_BGM_VOL)
	{
		if(quiet)
		{
			SsSetSerialVol( SS_CD, g_SET_BGM_VOL>>2, g_SET_BGM_VOL>>2 ); // low audio
		}
		else
		{
			SsSetSerialVol( SS_CD,g_SET_BGM_VOL>>1,g_SET_BGM_VOL>>1 ); // full audio
		}
	}

}
void yarmico_stop_all_SFX(void)
{
	SsUtAllKeyOff(0);

#if YARMICO_PICO_SOUND == TRUE
	g_yarmico_pico_play_sound.time_frames = 0;
#endif
}



u32 yarmico_play_SFX(u32 id)
{
	u32 left, right, 	tone = 38;

	LOG_AUDIO("yarmico_play_SFX %d   \n", id );

	// audio is sped up by 4x for audio compression, uncompressed by the PSX hardware pitch change
	// SsUtAllKeyOff(0);
	/*
	 * u16 r;
	SsUtKeyOff(
			 1, // short voice,
			 0,  // short vabId,
			 id,   // short prog,
			 0,   // short tone,
			 0   // short note

	);
	r =

	LOG_AUDIO("yarmico_play_SFX %d %d \n", r, id);

	 */




	LOG_AUDIO("yarmico_play_SFX g_stereo_counter_random = %d  \n",  g_stereo_counter_random);

	// 0 = left, 1 = center, 2 = right
	if(g_stereo_counter_random&1)
	{
		left = 0; // make quieter
		right = g_SET_SFX_VOL;
	}
	else 	// if(stereo&2)
	{
		left = g_SET_SFX_VOL;
		right = 0; // make quieter
	}
	/*
	else
	{
		left = g_SET_SFX_VOL>>1;
		right = g_SET_SFX_VOL>>1;
	}
	 */

	g_stereo_counter_random++;




	return SsUtKeyOn(0,
			id, //prog Program number
			0, //tone Tone number
			tone, //60-22,  //note NOTE specification (note number) in half tones - 64 is no pitch change
			127, //fine Fine pitch (100/127 cents)
			left, //left 127 MAX VOL
			right //right 127 MAX VOL
	);

}

/* Sound playback termination */

void SoundClose(void)
{
	//short i;


	/* Release all acquired voices */
	SsUtAllKeyOff (0);

	/* Stop the game music */
	SsSeqStop(0);
	SsSeqStop(0);

	/* Wait for music to stop - give it some time, will you ? */
	VSync(0);

	VSync(0);





	SsSeqClose(0);

	SsSeqClose(0);


	CdPlay( 0,0,0 ); //stop


	SsVabClose(0);

	SsVabClose(1);



	return;

}


#if 0
u32 NY_button_seed_random(void)
{

	//return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
	u32 g_dpad =  (~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
	if(PAD > 0)
	{
		// printf("NY_button_seed_random %d\n", PAD);
		return Net_Yaroze_seed_rand(PAD );

	}

	return 0;

}
#endif

u32 g_dpad;


void exit_to_siocons(void)
{



	// load siocon term for fun!


	/* Matt Verran's code from
		Subject: loading an exe and using Exec()
		Date: Sun, 15 Apr 2001 10:23:17 +0100
		From: Matt Verran
		Newsgroups: scee.yaroze.freetalk.english
	 */

	if( CdSearchFile(0, "\\DTL_S30.35;1") == 0 )
	{
		LOG_MAIN("\n DTL_S30.35;1 not found \n");
	}
	else
	{
		struct EXEC  *exec_struct;
		s32 result;

		LOG_MAIN("\n DTL_S30.35;1  found, de init \n");
#ifndef CODEWORRIOR_DEBUGGING
		SoundClose();
		game_deinit();
#endif

		ResetGraph(3);


		LOG_MAIN("\n Loading DTL_S30.35;1\n");

		exec_struct = CdReadExec( "\\DTL_S30.35;1" );
		result = CdReadSync(0, 0);

		LOG_MAIN("\n found loaded, result:  %d    \n\n", result);

		if (result==0)
		{


			printf("\n executing \n");
			EnterCriticalSection();
			Exec(exec_struct, 0, 0);
			ExitCriticalSection();
		}
	}

}





void init(void)
{

	u8 dpad;

	// Initialise control pads first for codewarrior!
	GetPadBuf(&bb0,&bb1);


#ifdef CODEWORRIOR_DEBUGGING
	// load exe via unirom/nops and exit to siocons, leaving assets untouched, then launch in CW debugger
#ifndef __MWERKS__ //todo: 0000 test
	exit_to_siocons();
#endif // CodeWarrior for NY

#endif


	/*
	// auto detect video mode PAL/NTSC
	// https://problemkaputt.de/psx-spx.htm#gpustatusregister
	//  20    Video Mode                  (0=NTSC/60Hz, 1=PAL/50Hz)     ;GP1(08h).3
	 *
  GPU Registers
  1F801810h.Write 4   GP0 Send GP0 Commands/Packets (Rendering and VRAM Access)
  1F801814h.Write 4   GP1 Send GP1 Commands (Display Control)
  1F801810h.Read  4   GPUREAD Read responses to GP0(C0h) and GP1(10h) commands
  1F801814h.Read  4   GPUSTAT Read GPU Status Register

GPU Command Summary
Commands/Packets consist of a 8bit command number (MSBs) and a 24bit parameter (LSBs),
which are written as 32bit value to GP0 or GP1.
#define GPU_GP0_(x)	*((volatile unsigned char*)(0x1f801810 + (x<<4)))
#define GPU_GP1_(x)	*((volatile u32*)(0x1f801814 + (x<<4)))

	 */



	// use display, pad and unknown RAM data as rand seed?
	{
		// must be looked at before GsInitGraph is called!

		// PAL Reg_GPUSTAT D614260A
		// after GsInitGraph
		// PAL Reg_GPUSTAT D6CE0000

		g_yarmico_start_time = PLAYSTATION_REGION_CHECK();
		LOG_MAIN("\n( *((volatile u32 *)GPUSTAT_REG) ) %X\n", g_yarmico_start_time);
		LOG_MAIN("\nReg_GPUSTAT %X\n", g_yarmico_start_time);

		if(g_yarmico_start_time) // if 1, it's PAL
		{

			StartRCnt(0); // restart counter to get new seed
			SetVideoMode(MODE_PAL); // 1=PAL/50Hz with NTSC res
			g_random +=10;
			LOG_MAIN("*WARNING*\n");
			LOG_MAIN("PAL PSX detected, using PAL (50Hz) with NTSC 640x480 resolution!\n");

		}
		else
		{
			VSync(0); VSync(0);// slow down to get new seed
			VSync(0);VSync(0);VSync(0);

			SetVideoMode(MODE_NTSC); // 0=NTSC/60Hz
			// SetVideoMode(MODE_PAL); // force PAL mode
			LOG_MAIN("NTSC PSX detected.\n");

			VSync(0);VSync(0);VSync(0);
			VSync(0);VSync(0);VSync(0);

		}

	}


	LOG_MAIN("\n Random start: 0x%X\n", (u32)g_random);




	g_random += ( (*  ((volatile char *)0xBFC7FF45)) - '0')*10; // 2 = 20
	g_random += ( (*  ((volatile char *)0xBFC7FF45+2)) - '0' )*100; // 2 = 200


	g_random += ( (*  ((volatile char *)0xBFC7FF45+4)) - '0')*33; // 1 = 33
	g_random += ( (*  ((volatile char *)0xBFC7FF45+5) ) - '0')*3; // 2 = 6


	g_random += ( (*  ((volatile char *)0xBFC7FF45+7) ) - '0')*15; // 0 = 0
	g_random += ( (*  ((volatile char *)0xBFC7FF45+8) ) - '0')*9; // 4 = 36


	g_random += ( (*  ((volatile char *)0xBFC7FF45+10) ) - '0'); // 9 = 9
	g_random += ( (*  ((volatile char *)0xBFC7FF45+11) ) - '0')*10; // 5 = 50



	VSync(0);
	InitialiseGraphics();
	VSync(0);
	LOG_MAIN("\n InitialiseGraphics GetRCnt(0) %X\n", GetRCnt(0));




	LOG_MAIN("\n Random start: 0x%X\n", (u32)g_random);
	VSync(0);

#ifndef __MWERKS__
	//GsInitGraph must be called before TestCard or CW goes poo poo????
	g_dpad =  TestCard (  0	);



	LOG_MAIN("TestCard 1 status: %d\n",g_dpad );


	// use memcard status as rand seed
	if(g_dpad)
		g_random += g_dpad * 64;

	VSync(0);

	g_dpad =  TestCard (  1	);
	LOG_MAIN("TestCard 2 status: %d\n", g_dpad);
#endif

	if(g_dpad)
		g_random += g_dpad * 128;


	VSync(0);

	LOG_MAIN("\n GetRCnt(0) %X\n", GetRCnt(0));
	VSync(0);


	// init sound first, top of heap
	SoundInit();
	LOG_MAIN("\n SoundInit GetRCnt(0) %X\n", GetRCnt(0));



	{
		// use pad data as random seed
		LOG_MAIN("\n PAD1 HW DATA 0x%X\n", ~(*(bb0+1) )&0xFF);
		LOG_MAIN("\n PAD2 HW DATA 0x%X\n", ~(*(bb1+1) )&0xFF);

		g_random += ( (~(*(bb0+1))) & 0xFF); //controller type port 1
		g_random += ( (~(*(bb1+1))) & 0xFF); //controller type port 2

	}



	//ResetRCnt(0);
#if 0

	printf("\n( *((volatile u32 *)GPUSTAT_REG) ) %X\n", ( *((volatile u32 *)GPUSTAT_REG) ));
	g_yarmico_start_time = ( *((volatile u32 *)GPUSTAT_REG) );
	g_yarmico_start_time +=((rand()& 0xFFFF) );
	printf("\nReg_GPUSTAT + rand %X\n", g_yarmico_start_time);
	srand(g_yarmico_start_time);
#endif




	// goto EXIT;






#ifdef USE_TPAGE00
	LOG_TIMS("\n 0 InitSprite \n");
	InitSprite(&g_TPAGE00, (u_long )TPAGE00_DATA,  10 , 32);


#ifdef USE_TPAGE01
	LOG_TIMS("\n 1 InitSprite \n");
	InitSprite(&g_TPAGE01, (u_long )TPAGE01_DATA,  10 , 32);

#endif



#ifdef USE_TPAGE02
	LOG_TIMS("\n 2 InitSprite \n");
	InitSprite(&g_TPAGE02, (u_long )TPAGE02_DATA,  148 , 32);

#endif




#ifdef USE_TPAGE03
	LOG_TIMS("\n 3 InitSprite \n");
	InitSprite(&g_TPAGE03, (u_long )TPAGE03_DATA,  276 , 32);

#endif




#ifdef USE_TPAGE04
	LOG_TIMS("\n 4 InitSprite \n");
	InitSprite(&g_TPAGE04, (u_long )TPAGE04_DATA,  404 , 32);

#endif




#ifdef USE_TPAGE05
	LOG_TIMS("\n 5 InitSprite \n");
	InitSprite(&g_TPAGE05, (u_long )TPAGE05_DATA,  516 , 32);

#endif


	LOG_TIMS("\n finished InitSprite \n");



	LOG_MAIN("Heap end (1st data) %#010x\n",  TPAGE00_DATA);
	LOG_MAIN("Heap start  (exe start)  %#010x\n",  MAIN_DATA);
	LOG_MAIN("Stack size  %#010x %d /(1024*1024*1024) should equal 2mb \n\n",  MAIN_DATA - TPAGE00_DATA, MAIN_DATA - TPAGE00_DATA );


	/*

 void InitHeap (
        void *head,
        long size
)

Arguments
head Pointer to heap start address
size Heap size (multiple of 4 bytes)

	The heap pointed to by head is initialized. Memory management functions are also initialized, so that malloc() and the other memory allocation functions can be used.
	The effective heap size is actually less than size because of overhead.
	Notes
	Multiple calls to InitHeap() are permitted only if they do not overlap, otherwise memory management functions will not operate properly
	 */

	// reclaim TIM DATA space as heap - probably a bad idea, but it works
	// InitHeap((unsigned long *)TPAGE00_DATA, (unsigned long) MAIN_DATA - TPAGE00_DATA);


#else

	InitHeap((unsigned long *)MAIN_DATA, (unsigned long) 1024*1024 ); // 1MB?

#endif // #ifdef USE_TPAGE00

	g_CurrentBuffer=GsGetActiveBuff();

	RenderClear();


	g_yarmico_frame_counter = g_OT_POS_CNT =  0;



	VSync(0);
	DrawSync(0);





	game_init();
}

void yarmico_play_pico_sound_platform(u32 turn_sound_off) //platform that plays it back
{

	if(turn_sound_off )
	{
		SsUtAllKeyOff(0);
		return;
	}

	// not very flexible - maybe use SEQ/midi
	SsUtKeyOn(0, // vabId,
			0, //square prog Program number
			0, //  Tone number
			g_yarmico_pico_play_sound.frequency>>3, //60-22,  //note NOTE specification (note number) in half tones - 64 is no pitch change
			127, //fine Fine pitch (100/127 cents)
			120, //left 127 MAX VOL
			120 //right 127 MAX VOL
	);

}

extern void display_netyaroze_title_screen(u32 TIM_address);


int  main ()
{
#if YARMICO_HEIGHT == 120
	u32 res_timer;

	res_timer=0;
#endif

	//#ifdef _RELEASE_
#ifdef NY_TITLE_DATA
	display_netyaroze_title_screen(NY_TITLE_DATA);
#endif
	//#endif
	init();


	do
	{

		yarmico_process_controller();

		if(g_yarmico_controller_special_buttons & YARMICO_CONTROLER_BUT_HOME_SELECT
				&& g_yarmico_controller_special_buttons & YARMICO_CONTROLER_BUT_MENU_START)
		{
			// let game know to deinit?
			game_deinit();
			exit_to_siocons();
		}

#if YARMICO_HEIGHT == 120
		if( res_timer)
			res_timer--;
		else
		{
			res_timer =YARMICO_BUTTON_PRESS_TIME;



#if YARMICO_HEIGHT == 120

			if(g_yarmico_controller_special_buttons & YARMICO_CONTROLER_BUT_HOME_SELECT)
			{

				if(g_yarmico_lores_upscale[0])
				{

					if(YARMICO_WIDTH == 160)
					{
						g_yarmico_lores_upscale[1] = YARMICO_SCREEN_WIDTH_BORDER_START_160X120_256X240;
						g_yarmico_lores_upscale[2] = YARMICO_SCREEN_HEIGHT_BORDER_START_160X120_256X240;
					}
					else //120
					{
						g_yarmico_lores_upscale[1] = YARMICO_SCREEN_WIDTH_BORDER_START_120X120_256X240;
						g_yarmico_lores_upscale[2] = YARMICO_SCREEN_HEIGHT_BORDER_START_120X120_256X240;
					}
					g_yarmico_lores_upscale[0] = 0;
				}
				else // use big borders
				{

					if(YARMICO_WIDTH == 160) // 160 to 256 width
					{
						g_yarmico_lores_upscale[0] = 6552; //fixed point scale factor 160* 1.6 = 256 so  1.6 * ONE = 6553.6 6552%8 is 0
						g_yarmico_lores_upscale[1] = 0;
						g_yarmico_lores_upscale[2] = 0;
					}
					else //120 to 256 width
					{

						g_yarmico_lores_upscale[0] = 8704; //fixed point scale factor 120\D72.133333   =255.99996  4096\D72.133333  = 8738.131968 - 8704%32=0
						g_yarmico_lores_upscale[1] = 0;
						g_yarmico_lores_upscale[2] = 0;
					}


				}

			}


		}
#endif //#if YARMICO_HEIGHT == 120




		LOG_PAD("\n g_function_loop %p \n",g_gameloop_function);

#endif



		RenderPrepare();


		//would like to use the scratch pad for g_function_loop, but it uses a lot of lib calls
		game_update();
#if YARMICO_PICO_SOUND == TRUE
		if(g_yarmico_pico_play_sound.time_frames)
			yarmico_play_pico_sound();
#endif

#if DISPLAY_TARGET > YARMICO_HIRES2
		//RenderClear();
		RenderFinish();
#else
		RenderFinish();
#endif




	}while(1);


	exit_to_siocons();

#if 0
	// todo Net Yaroze exit back to caller
	END_IT:
	//ResetGraph(3);
	printf("END");
	ResetGraph(3);
	exit(0);
	//((void (*)())0xBFC00000)();
#endif


}


#define YARMICO_MAIN_CONTROLLER_SOURCE_BUTTONS 	g_dpad



#define YARMICO_MAIN_CONTROLLER_DPAD_UP   PADLup // YARMICO_MAIN_CONTROLLER_BUTTONS & Button::DPAD_UP)
#define YARMICO_MAIN_CONTROLLER_DPAD_DOWN	PADLdown // YARMICO_MAIN_CONTROLLER_BUTTONS & Button::DPAD_DOWN)
#define YARMICO_MAIN_CONTROLLER_DPAD_RIGHT	PADLright // buttonsYARMICO_MAIN_CONTROLLER_BUTTONS & Button::DPAD_RIGHT)
#define YARMICO_MAIN_CONTROLLER_DPAD_LEFT		PADLleft // buttonsYARMICO_MAIN_CONTROLLER_BUTTONS & Button::DPAD_LEFT)


#define YARMICO_MAIN_CONTROLLER_TRIANGLE  PADLtriangle //YARMICO_MAIN_CONTROLLER_BUTTONS & Button::X) // up
#define YARMICO_MAIN_CONTROLLER_CROSS  	PADLcross //YARMICO_MAIN_CONTROLLER_BUTTONS & Button::B) // down
#define YARMICO_MAIN_CONTROLLER_CIRCLE  	PADLcircle // YARMICO_MAIN_CONTROLLER_BUTTONS & Button::A) // right
#define YARMICO_MAIN_CONTROLLER_SQUARE  	PADLsquare // YARMICO_MAIN_CONTROLLER_BUTTONS & Button::Y) // left



#define YARMICO_MAIN_CONTROLLER_ANALOG_OR_SHOULDERS

#define YARMICO_MAIN_CONTROLLER_START 	PADLstart //Button::MENU
#define YARMICO_MAIN_CONTROLLER_SELECT 	PADLselect //Button::MENU




void yarmico_process_controller(void)
{
	u8 ldir_pad, laction_buttons, lspecial_buttons;

	LOG_PAD("yarmico_process_controller \n");

	ldir_pad=0;
	laction_buttons=0;
	lspecial_buttons=0;


	g_dpad=  (~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));


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



	LOG_PAD("g_dpad  %d ldir_pad %d laction_buttons %d lspecial_buttons %d \n", g_dpad, ldir_pad, laction_buttons, lspecial_buttons);

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


		if(PAD1rh < 55) //shoot left
			laction_buttons |= YARMICO_CONTROLER_BUT_L;
		else if(PAD1rh > 200) //shoot right
			laction_buttons |= YARMICO_CONTROLER_BUT_R;

		if(PAD1rv < 55) //shoot up
			laction_buttons |= YARMICO_CONTROLER_BUT_U;
		else if(PAD1rv > 200) //shoot down
			laction_buttons |= YARMICO_CONTROLER_BUT_D;

	}



#elif YARMICO_CONTROLLER_ANALOG_TYPE == YARMICO_CONTROLLER_ANALOG_RETURN_ANALOG_DATA
	{
		yarmico_analog_stick sticks={0};

		if ( (*(bb0+1) >> 4) == 0x7 || (*(bb0+1) >> 4) == 0x5 ) // analog on
		{

			// info from
			// Analog pad tester.  Useful for finding the typical null-zone ranges
			// to prevent unwanted movement in your games.
			// Type SIOCONS AUTO to load and run.

			// by James Shaughnessy james@manc.u-net.com
			// http://www.netyaroze-europe.com/~shaughnj

			// 125 = 0
			// 55 < left or up
			//200 > right or down

			u8 PAD1rh, PAD1rv, PAD1lh, PAD1lv;
			PAD1rh = *(bb0+4);
			PAD1rv = *(bb0+5);

			PAD1lh = *(bb0+6);
			PAD1lv = *(bb0+7);

			sticks.right_stick_x = *(bb0+4);
			sticks.right_stick_y  = *(bb0+5);




			if(PAD1lh > PSX_ANALOG_CENTER+YARMICO_ANALOG_DEAD_ZONE)
				sticks.left_stick_x = (PAD1lh-PSX_ANALOG_CENTER)*ONE;// +right

			else if(PAD1lh < PSX_ANALOG_CENTER-YARMICO_ANALOG_DEAD_ZONE)
				sticks.left_stick_x = (PAD1lh-PSX_ANALOG_CENTER)*-ONE;// -left


			if(PAD1lv > PSX_ANALOG_CENTER+YARMICO_ANALOG_DEAD_ZONE)
				sticks.left_stick_y = (PAD1lv-PSX_ANALOG_CENTER)*-ONE; // -down

			else if(PAD1lv < PSX_ANALOG_CENTER-YARMICO_ANALOG_DEAD_ZONE)
				sticks.left_stick_y = (PAD1lv-PSX_ANALOG_CENTER)*ONE; // +up



			if(PAD1rh > PSX_ANALOG_CENTER+YARMICO_ANALOG_DEAD_ZONE)
				sticks.right_stick_x = (PAD1rh-PSX_ANALOG_CENTER)*ONE;// +right

			else if(PAD1rh < PSX_ANALOG_CENTER-YARMICO_ANALOG_DEAD_ZONE)
				sticks.right_stick_x = (PAD1rh-PSX_ANALOG_CENTER)*-ONE;// -left


			if(PAD1rv > PSX_ANALOG_CENTER+YARMICO_ANALOG_DEAD_ZONE)
				sticks.right_stick_y = (PAD1rv-PSX_ANALOG_CENTER)*-ONE; // -down

			else if(PAD1rv < PSX_ANALOG_CENTER-YARMICO_ANALOG_DEAD_ZONE)
				sticks.right_stick_y = (PAD1rv-PSX_ANALOG_CENTER)*ONE; // +up


			if(analog_stick)
				g_yarmico_controller_analog_sticks=sticks;


		}

	}


#endif

	g_yarmico_controller_dpad = ldir_pad;

	g_yarmico_controller_action_buttons = laction_buttons;

	g_yarmico_controller_special_buttons=lspecial_buttons;


	LOG_PAD("yarmico_process_controller FIN %d %d %d \n", g_yarmico_controller_dpad, g_yarmico_controller_action_buttons, g_yarmico_controller_special_buttons);

}

// remove srand define, use actual srand
#undef srand



// probably a better rand function then this mess!
//https://github.com/ptitSeb/freespace2/blob/500ee249f7033aac9b389436b1737a404277259c/src/math/staticrand.cpp#L122


// from https://stackoverflow.com/questions/322938/recommended-way-to-initialize-srand
// Robert Jenkins' 96 bit Mix Function
unsigned int mix(unsigned int a, unsigned int b, unsigned int c)
{
	a=a-b;  a=a-c;  a=a^(c >> 13);
	b=b-c;  b=b-a;  b=b^(a << 8);
	c=c-a;  c=c-b;  c=c^(b >> 13);
	a=a-b;  a=a-c;  a=a^(c >> 12);
	b=b-c;  b=b-a;  b=b^(a << 16);
	c=c-a;  c=c-b;  c=c^(b >> 5);
	a=a-b;  a=a-c;  a=a^(c >> 3);
	b=b-c;  b=b-a;  b=b^(a << 10);
	c=c-a;  c=c-b;  c=c^(b >> 15);
	return c;
}




#ifdef NY_TITLE_DATA

// a lot of duplicated screen init stuff, but saves stuffing around with different resolutions
// see platform_NY.h for more info
void display_netyaroze_title_screen(u32 TIM_address)
{
	LOG_MAIN("DISPLAY_NY_TITLE  g_yarmico_game_time %d\n", g_yarmico_game_time);


	g_yarmico_start_time = PLAYSTATION_REGION_CHECK();
	LOG_MAIN("\n( *((volatile u32 *)GPUSTAT_REG) ) %X\n", g_yarmico_start_time);
	LOG_MAIN("\nReg_GPUSTAT %X\n", g_yarmico_start_time);

	if(g_yarmico_start_time == 'E') // if E, it's PAL
	{

		StartRCnt(0); // restart counter to get new seed
		SetVideoMode(MODE_PAL); // 1=PAL/50Hz with NTSC res
		//	GsInitGraph(640,480,GsINTER+GsOFSGPU,0,0);
		LOG_MAIN("*WARNING*\n");
		LOG_MAIN("PAL PSX detected, using PAL (50Hz) with NTSC 640x480 resolution!\n");

	}
	else
	{
		VSync(0); VSync(0);// slow down to get new seed
		VSync(0);VSync(0);VSync(0);


		SetVideoMode(MODE_NTSC); // 0=NTSC/60Hz
		// GsInitGraph(512,480,GsINTER+GsOFSGPU,0,0);
		// SetVideoMode(MODE_PAL); // force PAL mode
		LOG_MAIN("NTSC PSX detected.\n");

		VSync(0);VSync(0);VSync(0);
		VSync(0);VSync(0);VSync(0);

	}

	GsInitGraph(640,480,GsINTER+GsOFSGPU,0,0);




	GsDRAWENV.r0=0;
	GsDRAWENV.g0=0;
	GsDRAWENV.b0=0;

	// Initialise ordering table
	g_TableHeader[0].length=ORDERING_TABLE_LENGTH;
	g_TableHeader[1].length=ORDERING_TABLE_LENGTH;
	g_TableHeader[0].org=TableArray[0];
	g_TableHeader[1].org=TableArray[1];



	// splash screens
	{


		GsSPRITE title;


		LOG_MAIN("CdPlay( 0,0,0 ); //stop  \n");



		// title image sized 256x256 4bit, compact and fits perfectly in a TPAGE
		InitSprite(&title, (u_long )TIM_address,  320 , 230);

		LOG_MAIN("InitSprite(&title, (u_long )NY_TITLE_DATA,  320 , 230);  \n");

		//scale up
		title.scalex =  (signed short) 10240; //4096*2.5;
		title.scaley =  (signed short) 7496; //4096+3400;



		//fade in
		//if(0)
		do{

			RenderPrepare();
			title.r =title.g = title.b = g_yarmico_game_time;
			GsSortSprite( &title,&g_TableHeader[g_CurrentBuffer],OT_POS_SPRITE-g_OT_POS_CNT);	// register the sprite in the ordering table
			// MACRO_SLOW_DOWN
			// if(g_yarmico_game_time & 32)
			// 		render_text(65, 180, 100, (char *)"- MAJOR SEIZURE WARNING -");



			//	render_text(8, 180, 100, (char *)"- MAJOR SEIZURE AND LANGUAGE WARNING -");

			RenderFinish();
			DrawSync(0);
		}while(g_yarmico_game_time++ < YARMICO_TITLE_BRIGHTNESS_LEVEL); // mid brightness

		LOG_MAIN("DISPLAY_NY_TITLE start finished }while(g_yarmico_game_time++ < 128); \n");

		g_yarmico_game_time = 0;

		// hold
		do{

			RenderPrepare();

			GsSortSprite( &title,&g_TableHeader[g_CurrentBuffer],OT_POS_SPRITE-g_OT_POS_CNT);	// register the sprite in the ordering table




			//	render_text(8, 180, 100, (char *)"- MAJOR SEIZURE AND LANGUAGE WARNING -");

			RenderFinish();
			DrawSync(0);

		}while(g_yarmico_game_time++ < YARMICO_TITLE_HOLD_COUNT);


		LOG_MAIN("DISPLAY_NY_TITLE hold finished while(g_yarmico_game_time++ < 300); \n");


		g_yarmico_game_time = 0;
		// fade out
		do{

			RenderPrepare();
			title.r = title.g = title.b = YARMICO_TITLE_BRIGHTNESS_LEVEL-g_yarmico_game_time;
			GsSortSprite( &title,&g_TableHeader[g_CurrentBuffer],OT_POS_SPRITE-g_OT_POS_CNT);	// register the sprite in the ordering table

			//render_text(8, 180, 100, (char *)"- MAJOR SEIZURE AND LANGUAGE WARNING -");

			RenderFinish();
			DrawSync(0);
		}while(g_yarmico_game_time++ < YARMICO_TITLE_BRIGHTNESS_LEVEL);



		LOG_MAIN("DISPLAY_NY_TITLE fad out finished }while(g_yarmico_game_time++ < 127); \n");


		g_yarmico_game_time = 0;

		do{
			RenderPrepare();
			RenderFinish();
			DrawSync(0);
		}while(g_yarmico_game_time++ < 100);

		LOG_MAIN("DISPLAY_NY_TITLE blank \n");



	}

	LOG_MAIN("DISPLAY_NY_TITLE back to start \n");
	//yarmico_play_BGM_audio(3);//intro

	DrawSync(0);

	g_yarmico_game_time =  0;



}

#endif


// from https://problemkaputt.de/psx-spx.htm

#define RAND_RAM_INC 8
#define RAND_RAM_LOOPBACK 0xBFC7FF80 // 0xBFC7FF80 //  BFC80000h  End       (End of 512kBYTE BIOS ROM)
#define RAND_RAM_START    0xBFC7F000 //0xBFC00000 // start of BIOS area
#define RAND_RAM_BOUNDARY 8 //32
#define LOG_RAND //printf
// most likely slower and buggy but it's more random
// dont use in game loop tho!
u32 random(void)
{

	u32 r, v1, v2, v3;

	LOG_RAND("\n NY_random g_random1 0x%X \n",(u32)g_random );

#if 1

	g_random+=RAND_RAM_INC;

	// loop back



	if( (u32)g_random >=  RAND_RAM_LOOPBACK)
	{
		g_random = (volatile u8 *) RAND_RAM_START; //(RAND_RAM_START + (g_random - RAND_RAM_START));
		// g_random -= ((u32)g_random)%RAND_RAM_BOUNDARY;
	}

	LOG_RAND("\n NY_random g_random2 0x%X \n",(u32)g_random );
	LOG_RAND("\n NY_random g_random3 VAL 0x%X \n",(*( (volatile u8 *)g_random) ) );


	// return GetRCnt(0);


	LOG_RAND("\n NY_random return: 0x%X v1:0x%X v2:0x%X v3:0x%X - g_random #0x%X\n", r, v1, v2, v3, g_random );


	v1 = *g_random | *(g_random+1) << 8 | *(g_random+2) << 16 | *(g_random+3) << 24  ;
	v1 +=  VSync(-1);

	v2 = *(g_random+4)   | *(g_random+5) << 8 | *(g_random+6) << 16 | *(g_random+7) << 24  ;
	// v2 += ( (~(*(bb0+1))) );


	v3 = GetRCnt(0);
	v3 += g_yarmico_frame_counter;
	// v3 += ( (~(*(bb1+1))) );

	r =  mix( v1, v2, v3 );
	// r =   v1+ v2+ v3;


	// r >>= 8;
	LOG_RAND_ADD("0x%X %X \n", g_random, r );
	//r &= 0xFFFFF;
	LOG_RAND("\n NY_random return: 0x%X v1:0x%X v2:0x%X v3:0x%X - g_random #0x%X\n", r, v1, v2, v3, g_random );
	//	LOG_RAND("\n NY_random g_random2 0x%X \n",(u32)g_random );

	//	return r & 0xFFFF;
	// sending a full u32 crashes the PSX somewhere??

#else

	r =0xFFFF;
#endif

	return r;//+g_stereo_counter_random;

}
