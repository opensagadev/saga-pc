#include <string.h>
#include "saga.h"

/* Episode III FixUpLevels callbacks (Cruiser_A/C/D, Grievous_A, Kashyyyk_A/B/C, Vader_A/B/C). */

typedef struct cruiserc_pc_s {
    GIZAIMESSAGE *dooku_fight;
    GAMEOBJECT *count_dooku;
    GIZAIMESSAGE *free_palpatine;
    GAMEOBJECT *palpatine;
    AILOCATOR_PC *palpatine_locator;
} CRUISERC_PC;

STATIC_ASSERT(sizeof(CRUISERC_PC) == 0x14);

typedef struct cruiserd_lift_anim_pc_s {
    NUMTX matrix;
    f32 tfactor;
    u8 unknown_44[8];
    f32 ltime;
    u32 playing : 1;
    u32 flags_rest : 31;
    f32 fparam1;
    f32 prev_eval_time;
    u16 animation_index;
} CRUISERD_LIFT_ANIM_PC;

STATIC_ASSERT(offsetof(CRUISERD_LIFT_ANIM_PC, tfactor) == 0x40);
STATIC_ASSERT(offsetof(CRUISERD_LIFT_ANIM_PC, ltime) == 0x4c);
STATIC_ASSERT(offsetof(CRUISERD_LIFT_ANIM_PC, fparam1) == 0x54);
STATIC_ASSERT(offsetof(CRUISERD_LIFT_ANIM_PC, animation_index) == 0x5c);

typedef struct vaderc_pc_s {
    GIZAIMESSAGE *final_fight_message;
    AILOCATOR_PC *big_jump_locator;
    NUSPECIALHANDLE rocks[10];
    i16 platform_ids[10];
    u8 progress_mask;
    u8 players_dead;
    u8 reserved_96[2];
} VADERC_PC;

STATIC_ASSERT(sizeof(VADERC_PC) == 0x98);
STATIC_ASSERT(offsetof(VADERC_PC, platform_ids) == 0x80);
STATIC_ASSERT(offsetof(VADERC_PC, progress_mask) == 0x94);

typedef struct terrain_platform_reader_pc_s {
    u8 opaque_00_40[0x40];
    NUMTX *scene_object;
    u8 opaque_44_6c[0x28];
} TERRAIN_PLATFORM_READER_PC;

STATIC_ASSERT(sizeof(TERRAIN_PLATFORM_READER_PC) == 0x6c);

typedef struct terrain_set_reader_pc_s {
    u8 opaque_00_68[0x68];
    TERRAIN_PLATFORM_READER_PC *platforms;
} TERRAIN_SET_READER_PC;

STATIC_ASSERT(offsetof(TERRAIN_SET_READER_PC, platforms) == 0x68);

void __cdecl GrievousA_Init(WORLDINFO *world) {
}

void __cdecl KashyyykA_Init(WORLDINFO *world) {
}

void __cdecl KashyyykA_Update(WORLDINFO *world) {
}

void __cdecl KashyyykB_Init(WORLDINFO *world) {
}

void __cdecl KashyyykB_Reset(WORLDINFO *world) {
}

void __cdecl KashyyykB_Update(WORLDINFO *world) {
}

void __cdecl KashyyykC_Init(WORLDINFO *world) {
}

void __cdecl KashyyykC_Update(WORLDINFO *world) {
}

void __cdecl VaderB_Init(WORLDINFO *world) {
}

void __cdecl VaderB_DrawPanel(WORLDINFO *world) {
}

STATIC_ASSERT(offsetof(DOOR_s, name) == 0);
STATIC_ASSERT(offsetof(DOOR_s, active) == 0xf7);
