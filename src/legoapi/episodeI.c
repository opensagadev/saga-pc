#include <stdio.h>
#include "saga.h"
#include <math.h>
#pragma bss_seg(".bss$MaulA_ai_message")

typedef struct racepod_resource_pc_s {
    NUMTX matrix;                      // 0x00
    NUVEC4 previous_axis;              // 0x40
    NUVEC4 previous_position;          // 0x50
    NUVEC4 previous_displacement;      // 0x60
    i32 pitch;                         // 0x70
    i32 yaw;                           // 0x74
    i32 unknown_0078;                  // 0x78
    f32 speed;                         // 0x7c
    FLIGHT_SPLINE_RESOURCE_PC *spline; // 0x80
    f32 start;                         // 0x84
    i16 model_id;                      // 0x88
    i16 unknown_008a;                  // 0x8a
    f32 distance;                      // 0x8c
    GAMEOBJECT *object;                // 0x90
    void *next;                        // 0x94
    u8 unknown_0098[8];                // 0x98
} RACEPOD_RESOURCE_PC;                 // size 0xa0

typedef struct podrace_resource_pc_s {
    FLIGHT_SPLINE_RESOURCE_PC splines[32]; // 0x0000
    RACEPOD_RESOURCE_PC lap_entries[16];   // 0xa600
    f32 lap_countdown;                     // 0xb000
    f32 mushroom_timer;                    // 0xb004
    f32 lap_display;                       // 0xb008
    f32 previous_lap_display;              // 0xb00c
    f32 max_lap_time;                      // 0xb010
    f32 lap_time_increment;                // 0xb014
    i32 attempts_per_increment;            // 0xb018
    i32 attempts;                          // 0xb01c
    u8 flags;                              // 0xb020
    u8 unknown_b021[15];                   // 0xb021
} PODRACE_RESOURCE_PC;                     // size 0xb030

STATIC_ASSERT(offsetof(SNIPER_s, prev) == 0);
STATIC_ASSERT(offsetof(SNIPER_s, pos) == 0xc);
STATIC_ASSERT(offsetof(SNIPER_s, fire_timer) == 0x18);
STATIC_ASSERT(offsetof(SNIPER_s, state) == 0x1c);

typedef struct pod_race_countdown_reader_pc_s {
    u8 unknown_0000[0xb000];    // 0x0000
    f32 lap_countdown;          // 0xb000
} POD_RACE_COUNTDOWN_READER_PC; // size 0xb004

typedef struct pod_merge_packet_reader_pc_s {
    u8 unknown_00[0x10];      // 0x00
    f32 blend_source_time;    // 0x10
    f32 blend_target_time;    // 0x14
    u8 unknown_18[0x22];      // 0x18
    i16 requested_animation;  // 0x3a
    u8 unknown_3c[6];         // 0x3c
    i16 overlay_animation;    // 0x42
    f32 overlay_weight;       // 0x44
} POD_MERGE_PACKET_READER_PC; // size 0x48

/* Episode I level callbacks installed by FixUpLevels. */
typedef struct podrace_levflag_pc_s {
    u8 podrace_state;  // 0x0
    u8 mushroom_state; // 0x1
    u8 remaining[14];  // 0x2
} PODRACE_LEVFLAG_PC;  // size 0x10

typedef struct gungan_a_pc_s {
    i16 unknown_000;           // 0x000
    i16 count;                 // 0x002
    AILOCATOR_PC *origins[32]; // 0x004
    AILOCATOR_PC *targets[32]; // 0x084
    f32 spawn_timer;           // 0x104
    i16 model_index;           // 0x108
    i16 unknown_10a;           // 0x10a
    i16 models[4];             // 0x10c
} GUNGAN_A_PC;                 // size 0x114

typedef struct mine_entry_pc_s {
    u32 active;     // 0x00
    NUVEC pos;      // 0x04
    f32 grow;       // 0x10
    f32 grow_alpha; // 0x14
    u16 rotx;       // 0x18
    u16 roty;       // 0x1a
} MINE_ENTRY_PC;    // size 0x1c

typedef struct minesys_pc_s {
    NUSPECIALHANDLE mine_special; // 0x000
    MINE_ENTRY_PC mines[64];      // 0x00c
    f32 mine_radius;              // 0x70c
    f32 spawn_timer;              // 0x710
    AIAREA_PC *nomine_areas[10];  // 0x714
    i16 nomine_count;             // 0x73c
    i16 mine_debris;              // 0x73e
    i32 mine_part;                // 0x740
    f32 update_timer;             // 0x744
} MINESYS_PC;                     // size 0x748

typedef struct client_mines_pc_s {
    NUVEC positions[64];   // 0x000
    u32 present_words[2];  // 0x300
    u32 exploded_words[2]; // 0x308
    u32 unknown_310[2];    // 0x310
} CLIENT_MINES_PC;         // size 0x318

typedef struct cut_instance_reader_pc_s {
    u8 opaque_00_88[0x88]; // 0x00
    u32 flags_88;          // 0x88
    u32 unknown_8c;        // 0x8c
    f32 current_frame;     // 0x90
} CUT_INSTANCE_READER_PC;  // size 0x94

void RetakeG_Reset(WORLDINFO *world) {
}

void MaulA_Update(WORLDINFO *world) {
}

void MaulD_Init(WORLDINFO *world) {
}

void MaulD_Update(WORLDINFO *world) {
}

void MaulE_Init(WORLDINFO *world) {
}

void MaulE_Update(WORLDINFO *world) {
}

void MaulF_Update(WORLDINFO *world) {
}

void AnakinsFlightB_Update(WORLDINFO *world) {
}

void AnakinsFlightB_Draw(WORLDINFO *world) {
}

/* Empty level debug hook shared with the Episode II callbacks. */
void KaminoEGizmoMissingPc(char *name) {
}

typedef struct podrace_netpacket_reader_pc_s {
    f32 start_countdown;       // 0x00
    f32 remaining_time;        // 0x04
    f32 available_time;        // 0x08
    u32 unknown_0c;            // 0x0c
    i32 lap;                   // 0x10
} PODRACE_NETPACKET_READER_PC; // size 0x14

/* Pod sprint (Episode I bonus) level callbacks. */
typedef struct PODSPRINT_VIEW_PC {
    NUGSPLINE *finish_line;       // 0x00
    NUGSPLINE *halfway;           // 0x04
    PODSPRINT_AI_PC ai[6];        // 0x08
    GIZAIMESSAGE *lap_msg;        // 0x68
    GIZAIMESSAGE *max_speed_msg;  // 0x6c
    GIZAIMESSAGE *min_speed_msg;  // 0x70
    GIZAIMESSAGE *speed_step_msg; // 0x74
    GAMEOBJECT *pacemaker;        // 0x78
    AIAREA_PC *boulders;          // 0x7c
    f32 arrow_timer;              // 0x80
    f32 countdown;                // 0x84
    f32 lap_time;                 // 0x88
    u8 unknown_8c[2];             // 0x8c
    i8 ai_state;                  // 0x8e
    i8 ai_index;                  // 0x8f
    u8 unknown_90[2];             // 0x90

    union {        // 0x92
        u16 flags; // 0x92

        struct {                     // 0x92
            i16 halfway : 1;         // 0x92:0
            i16 unknown_1 : 1;       // 0x92:1
            i16 finished : 2;        // 0x92:2
            i16 reset_requested : 1; // 0x92:4
            i16 unknown_5 : 11;      // 0x92:5
        } bits;
    };
} PODSPRINT_VIEW_PC; // size 0x94

STATIC_ASSERT(sizeof(PODSPRINT_VIEW_PC) == sizeof(PODSPRINT_PC));

typedef struct PODSPRINT_NETPACKET_PC {
    i16 ai_state;
    u8 unknown_02[4];
    i16 countdown;
    i16 lap_time;
} PODSPRINT_NETPACKET_PC;

typedef struct pod_race_panel_netpacket_pc_s {
    f32 start_countdown;       // 0x00
    f32 remaining_time;        // 0x04
    f32 available_time;        // 0x08
    f32 lap_alpha;             // 0x0c
    i32 lap;                   // 0x10
} POD_RACE_PANEL_NETPACKET_PC; // size 0x14

STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
STATIC_ASSERT(sizeof(LEVEL_OBJECT_RUNTIME_PC) == 0x10);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, active) == 0x0e);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, special) == 0);
