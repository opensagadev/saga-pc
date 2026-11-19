#pragma once

#include "nu2api/nucore/common.h"

struct WORLDINFO_s;

typedef struct ARCADE_MODE_s { // size 0xc
    i16 *text;                 // 0x0
    i32 target;                // 0x4
    u32 flags;                 // 0x8
} ARCADE_MODE;

typedef struct ARCADE_MENU_ENTRY_s { // size 0x8
    i16 *text;                       // 0x0
    i8 index;                        // 0x4
    u8 count;                        // 0x5
    u16 reserved_06;                 // 0x6
} ARCADE_MENU_ENTRY;

/* REPLACE existing ARCADE_ITEM 24-byte declaration, preserve every old field. */
typedef struct ARCADE_ITEM_s { // size 0x28

    union { // size 0x28

        struct {                                  // size 0x28
            i16 *level_text;                      // 0x00
            i8 level;                             // 0x04
            u8 level_count;                       // 0x05
            u16 reserved_06;                      // 0x06
            i16 *mode_text;                       // 0x08
            i8 mode;                              // 0x0c
            u8 mode_count;                        // 0x0d
            u16 reserved_0e;                      // 0x0e
            i16 *play_text;                       // 0x10
            i8 play;                              // 0x14
            u8 play_count;                        // 0x15
            u16 reserved_16;                      // 0x16
            ARCADE_MENU_ENTRY reset_points_entry; // 0x18
            ARCADE_MENU_ENTRY launch_entry;       // 0x20
        };

        ARCADE_MENU_ENTRY entries[5];
    }; // 0x00
} ARCADE_ITEM;

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

typedef struct AREA_LEVEL_LOAD { // size 0x8
    i16 level;                   // 0x0
    i16 first_level;             // 0x2
    i16 second_level;            // 0x4
    u8 flags;                    // 0x6
    u8 reserved_07;              // 0x7
} AREA_LEVEL_LOAD;

#ifdef __cplusplus
extern "C" {
#endif
    void LevelStreaming_Update(struct WORLDINFO_s *);
    void WorldInfo_Activate(void);
    void WorldInfo_ClearAllIfScreenFaded(void);
#ifdef __cplusplus
}
#endif
