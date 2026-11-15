#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/objects/objectsall.h"
#include "legoapi/world/levels/episodeIII_types.h"
#include "nu2api/numath/nuvec.h"

typedef union LEVEL_INST_ANIM_RESET_VIEW { // size 0x30
    u32 reset_words[12];
    NUINSTANIM *animations[12];

    struct {                    // size 0x30
        u8 unknown_00[0x20];    // 0x00
        f32 miscellaneous_time; // 0x20
        u16 target_deg_mid;     // 0x24
        u16 unknown_26;         // 0x26
        u16 target_deg_far;     // 0x28
        u8 unknown_2a[6];       // 0x2a
    } fields;
} LEVEL_INST_ANIM_RESET_VIEW;

#ifdef __cplusplus
extern "C" {
#endif
    extern LEVEL_INST_ANIM_RESET_VIEW LevInstAnimReset;
    extern struct gameobject_s *Grievous_obj;
    extern struct gizaimessage_s *KashyyykA_msg_TotalWookies;
    extern struct gizaimessage_s *KashyyykA_msg_WookiesToRescue;
    extern struct gizaimessage_s *vader_b_complete_msg;
    extern u8 vader_b_playersDead;
    extern NUVEC TempleC_StreamStatusPos;
    extern struct PART_PC *boulder_part[2];
    void Boulder_Move(PART_PC *, f32);
    void Boulder_Kill(PART_PC *, i32);
    extern VADERA vader_a;
    extern i16 dat_007f1a0c;
#ifdef __cplusplus
}
#endif

typedef struct cruiserd_lift_anim_s { // size 0x60
    NUMTX matrix;                     // 0x00
    f32 tfactor;                      // 0x40
    u8 unknown_44[8];                 // 0x44
    f32 ltime;                        // 0x4c
    u32 playing : 1;                  // 0x50:0
    u32 flags_rest : 31;              // 0x50:1
    f32 fparam1;                      // 0x54
    f32 prev_eval_time;               // 0x58
    u16 animation_index;              // 0x5c
} CRUISERD_LIFT_ANIM;

typedef struct terrain_platform_reader_pc_s { // size 0x6c
    u8 opaque_00_40[0x40];                    // 0x00
    NUMTX *scene_object;                      // 0x40
    u8 opaque_44_6c[0x28];                    // 0x44
} TERRAIN_PLATFORM_READER;

typedef struct terrain_set_reader_s {   // size 0x6c
    u8 opaque_00_68[0x68];              // 0x00
    TERRAIN_PLATFORM_READER *platforms; // 0x68
} TERRAIN_SET_READER;
