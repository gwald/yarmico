/*
 *
 PUBLIC DOMAIN 2022
 THIS CODE IS A HACKY MESS - READ IT AT YOUR OWN RISK!
 https://github.com/gwald/yarmico
 *
 */


#ifndef TARGET_HW
#define TARGET_HW

// 4bit title screen looses some brightness, make it brighter then normal
#define YARMICO_TITLE_BRIGHTNESS_LEVEL 255 // max u8 brightness - normal is 127


// ticks to hold title screen for
#define YARMICO_TITLE_HOLD_COUNT 10 //because it's really bright, do diming quickly


#include <r3000.h>
#include <asm.h>
#include <libps.h>
#include <sys/file.h>

#include "DATA.H" // similar to build.h




//Playstation BIOS info: version/date/region
#define PLAYSTATION_BIOS_INFO() ( ((volatile char *)0xBFC7FF32+0x13) )

// Auto detect BIOS region
// https://problemkaputt.de/psx-spx.htm#biosversions
// from the Bootmenu/Intro Versions
// The System ROM Version string can be found at BFC7FF32h (except in v1.0).
// BIOS Memory Map: BFC7FF32h GUI Version/Copyright Strings (if any) (separated by one 00h byte)
#define PLAYSTATION_REGION_CHECK()  ( *((volatile char *)0xBFC7FF52) ) // returns 'E' for EU regions, based on BIOS information...  'A' for US, 'I' or something else for JPN




#define HARDWARE_TARGET HARDWARE_TARGET_NET_YAROZE

// NY_TITLE_DATA is the RAM addresstitle splash like the demo disc but 255x255 TIM 4bit, displayed at hi res.
// it's defined in dataman.txt, run dataman in the command line, it generates a header and siocons batch file.
// if NY_TITLE_DATA is not defined, it wont show anything.





/*
 *		Copyright (C) 1996 by Sony Computer Entertainment Inc.
 *				All rights Reserved
 *
 *			pad.h: PadRead() accsess macro
 */


#define PADLup		4096
#define PADLdown	16384
#define PADLleft	32768
#define PADLright	8192
#define PADLtriangle	16
#define PADLcircle	32
#define PADLcross	64
#define PADLsquare	128
#define PADLL1		4
#define PADLL2		1
#define PADLR1		8
#define PADLR2		2
#define PADLstart	2048
#define PADLselect	256



#define PADRup		268435456
#define PADRdown	1073741824
#define PADRleft	2147483648
#define PADRright	536870912
#define PADRtriangle	1048576
#define PADRcircle	2097152
#define PADRcross	4194304
#define PADRsquare	8388608
#define PADRL1		262144
#define PADRL2		65536
#define PADRR1		524288
#define PADRR2		131072
#define PADRstart	134217728
#define PADRselect	16777216


#endif //#ifndef TARGET_HW
