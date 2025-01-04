//
// This header is included by default in all source files.
//
#ifndef COMMON_H
#define COMMON_H

#ifdef MSVC
#pragma runtime_checks("scu", off)
#endif

#define DECOMP_CUSTOM 1 // Logic that is added to aid decompilation, etc
#define DEBUG_BUILD 1 // Logic that appears only in debug builds

extern const char *build_rev;
extern const char *build_date;
extern const char *build_ui_widget_text;



#define NONE -1
#define MAXIMUM_GAMEPADS 4
#define MAXIMUM_NUMBER_OF_LOCAL_PLAYERS 4
#define MAXIMUM_STRING_SIZE 0x2000

//sound
#define NUMBER_OF_SOUND_CLASSES 51
#define MAXIMUM_SOUND_INSTANCES_PER_DEFINITION 16
#define MAXIMUM_SOUND_INSTANCES_PER_OBJECT_PER_DEFINITION 16

#define SOUND_LOOP_TYPE_INFINITE 2
#define SOUND_LOOP_TYPE_ONE_SHOT 0

#define PLAYBACK_STATE_PLAYING 1
#define PLAYBACK_STATE_STOPPED 0

#define MAXIMUM_COLLISION_USER_STACK_DEPTH 32



#define MAXIMUM_SOUND_CHANNELS 0xFF

//Bink
#define MAX_BINK_POINTER_BLOCKS 16 //just a guess for now

//savegame
#define NUMBER_OF_SAVED_GAME_FILE_TYPES 1
#define NUMBER_OF_MEMORY_UNITS 8
#define MAXIMUM_ENUMERATED_SAVED_GAME_FILES_ANY_TYPE_PER_MEMORY_UNIT 99
#define MAXIMUM_SAVED_GAME_NAME_LENGTH 0x7f

//ai/actors.c
#define MAXIMUM_NUMBER_OF_UNITS_PER_SWARM 0xf


#include "types.h"
#include "inlines.h"
#include "decl.h"


#define assert_halt(cond)                                    \
    do {                                                     \
        if (!(cond)) {                                       \
            display_assert(#cond, __FILE__, __LINE__, true); \
            system_exit(-1);                                 \
        }                                                    \
    } while (0)

#define assert_halt_msg(cond, msg)                         \
    do {                                                   \
        if (!(cond)) {                                     \
            display_assert(msg, __FILE__, __LINE__, true); \
            system_exit(-1);                               \
        }                                                  \
    } while (0)

#define CLAMP(x, low, high) \
  ((x) < (low) ? (low) : ((x) > (high) ? (high) : (x)))

static const int _scenario_type_main_menu = 2;

#ifdef DEBUG_BUILD
#define strlen csstrlen
#endif

#endif
