
// #pragma once

#ifndef LOAD_GAMECONTROLLERDB
#define LOAD_GAMECONTROLLERDB

#ifndef UNIT_32BLIT // SDL2 stuff only

#ifdef __cplusplus
extern "C" {
#endif



#include <sys/types.h>


#include <stdio.h>


# include <stdlib.h>
# include <stddef.h>
# include <stdarg.h>
# include <strings.h>
#include <string.h>

// SDL includes aren't availble on some linking... PITA
// #include "SDL.h"


//mgarcia updated to newer version for controller support

//#include "Input.hpp"
//#include "engine/input.hpp"
//#include "System.hpp"

#define SDL_Log //



#if 0 // NOTES

// SDL2 stuff only
	#include "gamecontrollerdb.hpp" 

/*
 //https://github.com/gabomdq/SDL_GameControllerDB/blob/master/gamecontrollerdb.txt
 gamecontrollerdb.hpp, serialized from gamecontrollerdb.txt file as string and size, ie:
 assets.yml
 gamecontrollerdb.hpp:
 gamecontrollerdb.txt:
    name: gamecontrollerdb
 */
 
 // USEAGE 

load_gamecontrollerdb( (const char *) gamecontrollerdb, gamecontrollerdb_length ); // SDL2 stuff only


 #endif


//mgarcia
//defines private from: https://github.com/libsdl-org/SDL/blob/396411c09093660fead7999827e9f0db5ea2bc0e/src/joystick/SDL_gamecontroller.c#L47
#define SDL_CONTROLLER_PLATFORM_FIELD       "platform:"
#define SDL_CONTROLLER_PLATFORM_FIELD_SIZE  strlen(SDL_CONTROLLER_PLATFORM_FIELD)
#define SDL_CONTROLLER_HINT_FIELD           "hint:"
#define SDL_CONTROLLER_HINT_FIELD_SIZE      strlen(SDL_CONTROLLER_HINT_FIELD)


#define SDL_arraysize(array)    (sizeof(array)/sizeof(array[0]))


// loads embedded gamecontrollerdb.txt, then trys to load  gamecontrollerdb.txt in the current directory, then trys to opens connected controllers
// returns number of opened controllers

//extern "C"

extern "C" void *SDL_GameControllerOpen(int joystick_index);
extern "C" int SDL_NumJoysticks(void);
extern "C" int  SDL_GameControllerAddMapping(const char* mappingString);
extern "C" void *SDL_RWFromFile(const char *file, const char *mode);

extern "C" int  SDL_GameControllerAddMappingsFromRW(void * rw, int freerw);

#define SDL_GameControllerAddMappingsFromFile(file)   SDL_GameControllerAddMappingsFromRW(SDL_RWFromFile(file, "rb"), 1)

extern "C" const char *  SDL_GetPlatform (void);

int  load_gamecontrollerdb(const char *db_str, const uint32_t db_size) 
{
	
	 const char *platform = SDL_GetPlatform();
    int controllers = 0;
    char *buf, *line, *line_end, *tmp, *comma, line_platform[64];
    size_t platform_len;
    
   
   // buf = (char *)SDL_calloc(1, db_size + 1);
    buf = (char *)calloc(1, db_size + 1);
    if (buf == NULL) {
         //    return SDL_SetError("Could not allocate space to read DB into memory");
              printf("Could not allocate space to read DB into memory");
              return -1;
    }
    
       
    memcpy(buf, db_str, db_size );
      
    line = buf;
    
    while (line < buf + db_size) {
       // line_end = SDL_strchr(line, '\n');
        line_end = strchr(line, '\n');
        if (line_end != NULL) {
            *line_end = '\0';
        } else {
            line_end = buf + db_size;
        }
        
        /* Extract and verify the platform */
       // tmp = SDL_strstr(line, SDL_CONTROLLER_PLATFORM_FIELD);
        tmp = strstr(line, SDL_CONTROLLER_PLATFORM_FIELD);
        if (tmp != NULL) {
            tmp += SDL_CONTROLLER_PLATFORM_FIELD_SIZE;
           // comma = SDL_strchr(tmp, ',');
            comma = strchr(tmp, ',');
            if (comma != NULL) {
              //SDL2  platform_len = comma - tmp + 1;
            	 platform_len = comma - tmp;
                if (platform_len + 1 < SDL_arraysize(line_platform)) {
               
                   // SDL_strlcpy(line_platform, tmp, platform_len);
                    strncpy(line_platform, tmp, platform_len);
                    // if (SDL_strncasecmp(line_platform, platform, platform_len) == 0 &&
                    if (strncasecmp(line_platform, platform, platform_len) == 0 &&
                        SDL_GameControllerAddMapping(line) > 0) {
                        controllers++;                       
                    }
                }
            }
        }

        line = line_end + 1;
    }

    free(buf);
 
 
 
	// check for actual gamecontrollerdb.txt	
   SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
   controllers =0;
	
 	// Open all joysticks as game controllers
	for(int n=0; n<SDL_NumJoysticks(); n++) {
		if(SDL_GameControllerOpen(n))
			controllers++;		// open correctly
	}

      
    return controllers;
} //mgarcia



#ifdef __cplusplus
} // extern "C" {
#endif



#endif //#ifndef LOAD_GAMECONTROLLERDB

#endif // #ifndef UNIT_32BLIT // SDL2 stuff only



#if 0 //old orig, SDL2 ver


#include "SDL.h"
//mgarcia updated to newer version for controller support

#include "Input.hpp"
#include "engine/input.hpp"
#include "System.hpp"

//amico again
#define SDL_Log //




//mgarcia
//defines private from: https://github.com/libsdl-org/SDL/blob/396411c09093660fead7999827e9f0db5ea2bc0e/src/joystick/SDL_gamecontroller.c#L47
#define SDL_CONTROLLER_PLATFORM_FIELD       "platform:"
#define SDL_CONTROLLER_PLATFORM_FIELD_SIZE  strlen(SDL_CONTROLLER_PLATFORM_FIELD)
#define SDL_CONTROLLER_HINT_FIELD           "hint:"
#define SDL_CONTROLLER_HINT_FIELD_SIZE      strlen(SDL_CONTROLLER_HINT_FIELD)

// loads embedded gamecontrollerdb.txt, then trys to load  gamecontrollerdb.txt in the current directory, then trys to opens connected controllers
// returns number of opened controllers

extern "C" int  load_gamecontrollerdb(const char *db_str, const uint32_t db_size)
{

	 const char *platform = SDL_GetPlatform();
    int controllers = 0;
    char *buf, *line, *line_end, *tmp, *comma, line_platform[64];
    size_t platform_len;


    buf = (char *)SDL_calloc(1, db_size + 1);
    if (buf == NULL) {
             return SDL_SetError("Could not allocate space to read DB into memory");
    }


    memcpy(buf, db_str, db_size );

    line = buf;

    while (line < buf + db_size) {
        line_end = SDL_strchr(line, '\n');
        if (line_end != NULL) {
            *line_end = '\0';
        } else {
            line_end = buf + db_size;
        }

        /* Extract and verify the platform */
        tmp = SDL_strstr(line, SDL_CONTROLLER_PLATFORM_FIELD);
        if (tmp != NULL) {
            tmp += SDL_CONTROLLER_PLATFORM_FIELD_SIZE;
            comma = SDL_strchr(tmp, ',');
            if (comma != NULL) {
                platform_len = comma - tmp + 1;
                if (platform_len + 1 < SDL_arraysize(line_platform)) {
                    SDL_strlcpy(line_platform, tmp, platform_len);
                    if (SDL_strncasecmp(line_platform, platform, platform_len) == 0 &&
                        SDL_GameControllerAddMapping(line) > 0) {
                        controllers++;
                    }
                }
            }
        }

        line = line_end + 1;
    }

    SDL_free(buf);



	// check for actual gamecontrollerdb.txt
   SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
   controllers =0;

 	// Open all joysticks as game controllers
	for(int n=0; n<SDL_NumJoysticks(); n++) {
		if(SDL_GameControllerOpen(n))
			controllers++;		// open correctly
	}


    return controllers;
} //mgarcia
#endif
