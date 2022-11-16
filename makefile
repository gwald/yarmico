CC=gcc
#-Wall  -Werror  -g
CFLAGS	=  -g  -O3 -Werror
LINKER  =  $(CFLAGS) -Xlinker -Map -Xlinker mapfile.txt -Xlinker -Ttext -Xlinker 8011d9ff # from DATA.H +f 0x80120000

HEADER = DATA.H yarmico_API.h 
PROG	= main.exe
OBJS    = platform_NY.o  yarmico_API.o game.o
PNGS	= TPAGE00.png 
#TPAGE02.png TPAGE03.png TPAGE04.png


all: $(PNGS) $(PROG) 

rebuild: 
	make clean
	echo ---------- building ------
	make all 
	
rerun: 
	make clean
	echo ---------- building ------
	make all 
	make run

$(PROG):  $(OBJS) $(HEADER)
	ls -lk $(OBJS)
	rm -f $@
	rm -f mapfile.txt
	$(CC) $(LINKER) $(OBJS) -o $@
	ls -lk $@
#	strip $@

.c.o:	
	echo building $@
	rm -f $@
	rm -f main.exe
	$(CC) $(CFLAGS) -funsigned-char -c $< -o $@

.png.tim:
	PSX.bat
	
	
psx.exe: $(PROG)   main.exe 
	yarexe DATA.MAK 


	
run: psx.exe
	nopsx psx.exe
	


pcsxr: $(PROG)
	"C:\emulators\pcsxr\pcsxr.exe" -yaroze "C:\Documents and Settings\NetYaroze\Desktop\NY-demos\work\DATA.MAK"

release:
	make clean
	echo ---------- RELEASE ------
	make CFLAGS=" -O3 -Werror  -DNETYAROZE -D_RELEASE_"  all 
	yarexe DATA.MAK 
	make psx.iso
	

psx.iso: psx.exe
	$(info yariso creates iso based on regions (SCEI, SCEA, SCEE) and folder, NYISO or EXEROM folders )
	$(info NY = NYISO   )
	$(info - = EXEROM  )
	rm -f psx.iso
	@echo putting psx.exe into psx.cue psx.bin
	yariso SCEI NY 
	$(info yariso creates psx.iso in C:\psx3\sbin\yariso, you have to copy it back!  )
	cp -u -f C:\psx3\sbin\yariso\psx.iso .
	cp -u -f C:\psx3\sbin\yariso\psx.iso  Z:\pico-backup\


PLAYISO_NOPSX: psx.iso
	$(info nopsx has issues playing the .cue file???)
	call nopsx psx.iso
	

PLAYISO_MED: psx.iso
	$(info mednafen only use cue files; works fine )
	cp -u -f psx.iso C:\misc\PSXCUE\psx.bin
	call C:\emulators\mednafen-1.27.1-win32\mednafen_cue.bat C:\misc\PSXCUE\psx.cue #psx.cue
	

PLAYISO_XEB: psx.iso
	$(info XEBRA only uses cue files; audio CD tracks doesn't work :/  https://emulation.fandom.com/wiki/XEBRA#Loading_Games )
	cp -u -f psx.iso C:\misc\PSXCUE\psx.bin	
	call C:\emulators\xebra210423d\xebra C:\misc\PSXCUE\psx.cue

	


minicons-exec:	$(PROG)
	call cmd /C wincmd sioconz.exe  EXEC.MAK


minicons:	
	call cmd /C wincmd sioconz.exe DATA.MAK

debug:	
	call cmd /C wincmd sioconz.exe GDB.MAK

sioconz:	
	call cmd /C wincmd sioconz.exe   DATA.MAK
# -B115200
# -B1036800


nops-release:	
	make clean
	make CFLAGS=" -O3 -Werror  -DNETYAROZE -D_RELEASE_"  all  
	yarexe DATA.MAK 
	lll psx.exe
#requires --force sometimes? :/
	upx --best --force psx.exe
	lll psx.exe
# TTY DEBUGGING
#	call cmd /C wincmd nops.bat /fast /exe  psx.exe  /m
# NO DEBUGGING  
	nops.bat /fast /exe  psx.exe 

nops-debug:	
	make clean
	make CFLAGS=" -O3 -Werror  -DNETYAROZE -D_DEBUG_"  all  
	yarexe DATA.MAK 
	upx --best psx.exe
	call cmd /C wincmd nops.bat /fast /exe  psx.exe  /m 
	
backup: clean
	backup.bat
	
clean:
	del $(PROG)
	del *.o
	del psx.exe
	del main.exe
	del  psx.iso

