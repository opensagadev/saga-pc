#include <stdio.h>
#include "saga.h"
#include <math.h>
#pragma bss_seg(".bss$MaulA_ai_message")

STATIC_ASSERT(offsetof(SNIPER_s, prev) == 0);
STATIC_ASSERT(offsetof(SNIPER_s, pos) == 0xc);
STATIC_ASSERT(offsetof(SNIPER_s, fire_timer) == 0x18);
STATIC_ASSERT(offsetof(SNIPER_s, state) == 0x1c);

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
