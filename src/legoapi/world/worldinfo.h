#ifndef SAGA_LEGOAPI_WORLD_WORLDINFO_H
#define SAGA_LEGOAPI_WORLD_WORLDINFO_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

struct WORLDINFO_s;

typedef struct ARCADE_MODE_PC_s {
    i16 *text;
    i32 target;
    u32 flags;
} ARCADE_MODE_PC;

STATIC_ASSERT(sizeof(ARCADE_MODE_PC) == 0xc);
STATIC_ASSERT(offsetof(ARCADE_MODE_PC, flags) == 8);

typedef struct ARCADE_MENU_ENTRY_PC_s {
    i16 *text;
    i8 index;
    u8 count;
    u16 unknown_06;
} ARCADE_MENU_ENTRY_PC;

STATIC_ASSERT(sizeof(ARCADE_MENU_ENTRY_PC) == 8);

/* REPLACE existing ARCADE_ITEM_PC24-byte declaration, preserve every old field. */
typedef struct ARCADE_ITEM_PC_s {
    union {
        struct {
            i16 *level_text;
            i8 level;
            u8 level_count;
            u16 unknown_06;
            i16 *mode_text;
            i8 mode;
            u8 mode_count;
            u16 unknown_0e;
            i16 *play_text;
            i8 play;
            u8 play_count;
            u16 unknown_16;
            ARCADE_MENU_ENTRY_PC reset_points_entry;
            ARCADE_MENU_ENTRY_PC launch_entry;
        };

        ARCADE_MENU_ENTRY_PC entries[5];
    };
} ARCADE_ITEM_PC;

STATIC_ASSERT(sizeof(ARCADE_ITEM_PC) == 0x28);
STATIC_ASSERT(offsetof(ARCADE_ITEM_PC, mode) == 0xc);
STATIC_ASSERT(offsetof(ARCADE_ITEM_PC, reset_points_entry) == 0x18);
STATIC_ASSERT(offsetof(ARCADE_ITEM_PC, launch_entry) == 0x20);
#ifdef __cplusplus
extern "C" {
#endif
    extern struct WORLDINFO_s *LWORLD;
    extern i32 other_level;
    extern i32 LevMusicAction;
    extern i32 LevMusicAmbient;
    extern i32 LevMusicOtherAction;
    extern i32 LevMusicOtherAmbient;
#ifdef __cplusplus
}
#endif

typedef struct AREA_LEVEL_LOAD_PC {
    i16 level;
    i16 first_level;
    i16 second_level;
    u8 flags;
    u8 unknown_07;
} AREA_LEVEL_LOAD_PC;

STATIC_ASSERT(sizeof(AREA_LEVEL_LOAD_PC) == 8);
STATIC_ASSERT(offsetof(AREA_LEVEL_LOAD_PC, level) == 0);
STATIC_ASSERT(offsetof(AREA_LEVEL_LOAD_PC, first_level) == 2);
STATIC_ASSERT(offsetof(AREA_LEVEL_LOAD_PC, second_level) == 4);
STATIC_ASSERT(offsetof(AREA_LEVEL_LOAD_PC, flags) == 6);
#ifdef __cplusplus
extern "C" {
#endif
    void LevelStreaming_Update(struct WORLDINFO_s *);
#ifdef __cplusplus
}
#endif

enum { AREAFLAG_SINGLE_BUFFER = 8 };

#endif
