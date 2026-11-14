/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/menus/screens/gamemenuall.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void SaveSystemInitialiseEx(i32, SAVE_HASH_FN_PC, void *, i32, void *, i32, i32, SAVE_VOID_FN_PC);
    void SetSaveSuccessFn(SAVE_VOID_FN_PC);
#ifdef __cplusplus
}
#endif

typedef struct AI_PATH_CONNECTION_TYPE {           // size 0x4c
    u32 connection_flag;                           // 0x00
    void (*draw)(NUVEC *, f32, NUVEC *, u32, i32); // 0x04
    char name[64];                                 // 0x08
    u32 flags;                                     // 0x48
} AI_PATH_CONNECTION_TYPE;

typedef struct FILE_DIRECTORY_RAW_TIME_PC {
    u8 second;
    u8 minute;
    u8 hour;
    u8 day;
    u8 month;
    u8 year_low;
    u8 year_high;
} FILE_DIRECTORY_RAW_TIME_PC;

typedef struct FILE_DIRECTORY_HOST_ENTRY_PC { // size 0x144
    u32 flags;                                // 0x000
    u8 reserved_04[4];                        // 0x004
    u32 size;                                 // 0x008
    u8 reserved_0c;                           // 0x00c
    FILE_DIRECTORY_RAW_TIME_PC created;       // 0x00d
    u8 reserved_14;                           // 0x014
    FILE_DIRECTORY_RAW_TIME_PC accessed;      // 0x015
    u8 reserved_1c;                           // 0x01c
    FILE_DIRECTORY_RAW_TIME_PC modified;      // 0x01d
    u32 unused_24;                            // 0x024
    u8 reserved_28[0x18];                     // 0x028
    char name[MAX_PATH];                      // 0x040
} FILE_DIRECTORY_HOST_ENTRY_PC;

typedef struct part_runtime_type_s { // size 0x178
    char name[16];                   // 0x000
    char object_names[8][16];        // 0x010
    i16 effect_ids[8];               // 0x090
    i8 effect_pages[8];              // 0x0a0
    i8 scene_indices[8];             // 0x0a8
    i8 variant_count;                // 0x0b0
    u8 variant_mode;                 // 0x0b1
    i8 page;                         // 0x0b2
    u8 unknown_b3;                   // 0x0b3
    f32 particle_scale;              // 0x0b4
    f32 effect_scale;                // 0x0b8
    f32 lifetime;                    // 0x0bc
    f32 lifetime_random;             // 0x0c0
    f32 speed;                       // 0x0c4
    f32 gravity;                     // 0x0c8
    f32 emission_rate;               // 0x0cc
    f32 bounce;                      // 0x0d0
    NUVEC position_random;           // 0x0d4
    NUVEC velocity_random;           // 0x0e0
    f32 emission_period;             // 0x0ec
    f32 emission_period_random;      // 0x0f0
    f32 emission_pause;              // 0x0f4
    f32 emission_pause_random;       // 0x0f8
    i32 rotation[3];                 // 0x0fc
    i32 rotation_random[3];          // 0x108
    u32 flags;                       // 0x114
    i32 trail_effects[2];            // 0x118
    f32 trail_rates[2];              // 0x120
    i32 attached_effect;             // 0x128
    i32 impact_effect;               // 0x12c
    i32 impact_part;                 // 0x130
    char impact_part_name[16];       // 0x134
    i32 kill_effect;                 // 0x144
    i32 sounds[4];                   // 0x148
    u8 sound_modes[4];               // 0x158
    f32 maximum_distance;            // 0x15c
    f32 unknown_160;                 // 0x160
    f32 unknown_164;                 // 0x164
    f32 unknown_168;                 // 0x168
    f32 last_used_time;              // 0x16c
    f32 scale;                       // 0x170
    u32 unknown_174;                 // 0x174
} PART_RUNTIME_TYPE;
