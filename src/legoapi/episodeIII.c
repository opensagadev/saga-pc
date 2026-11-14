#include <string.h>
#include "saga.h"

/* Episode III FixUpLevels callbacks (Cruiser_A/C/D, Grievous_A, Kashyyyk_A/B/C, Vader_A/B/C). */

typedef struct cruiserc_pc_s {       // size 0x14
    GIZAIMESSAGE *dooku_fight;       // 0x00
    GAMEOBJECT *count_dooku;         // 0x04
    GIZAIMESSAGE *free_palpatine;    // 0x08
    GAMEOBJECT *palpatine;           // 0x0c
    AILOCATOR_PC *palpatine_locator; // 0x10
} CRUISERC_PC;

typedef struct cruiserd_lift_anim_pc_s { // size 0x60
    NUMTX matrix;                        // 0x00
    f32 tfactor;                         // 0x40
    u8 unknown_44[8];                    // 0x44
    f32 ltime;                           // 0x4c
    u32 playing : 1;                     // 0x50:0
    u32 flags_rest : 31;                 // 0x50:1
    f32 fparam1;                         // 0x54
    f32 prev_eval_time;                  // 0x58
    u16 animation_index;                 // 0x5c
} CRUISERD_LIFT_ANIM_PC;

typedef struct vaderc_pc_s {           // size 0x98
    GIZAIMESSAGE *final_fight_message; // 0x00
    AILOCATOR_PC *big_jump_locator;    // 0x04
    NUSPECIALHANDLE rocks[10];         // 0x08
    i16 platform_ids[10];              // 0x80
    u8 progress_mask;                  // 0x94
    u8 players_dead;                   // 0x95
    u8 reserved_96[2];                 // 0x96
} VADERC_PC;

typedef struct terrain_platform_reader_pc_s { // size 0x6c
    u8 opaque_00_40[0x40];                    // 0x00
    NUMTX *scene_object;                      // 0x40
    u8 opaque_44_6c[0x28];                    // 0x44
} TERRAIN_PLATFORM_READER_PC;

typedef struct terrain_set_reader_pc_s {   // size 0x6c
    u8 opaque_00_68[0x68];                 // 0x00
    TERRAIN_PLATFORM_READER_PC *platforms; // 0x68
} TERRAIN_SET_READER_PC;

void GrievousA_Init(WORLDINFO *world) {
}

void KashyyykA_Init(WORLDINFO *world) {
}

void KashyyykA_Update(WORLDINFO *world) {
}

void KashyyykB_Init(WORLDINFO *world) {
}

void KashyyykB_Reset(WORLDINFO *world) {
}

void KashyyykB_Update(WORLDINFO *world) {
}

void KashyyykC_Init(WORLDINFO *world) {
}

void KashyyykC_Update(WORLDINFO *world) {
}

void VaderB_Init(WORLDINFO *world) {
}

void VaderB_DrawPanel(WORLDINFO *world) {
}

STATIC_ASSERT(offsetof(DOOR_s, name) == 0);
STATIC_ASSERT(offsetof(DOOR_s, active) == 0xf7);
