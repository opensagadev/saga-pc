#include <stdio.h>
#include "saga.h"
#include <math.h>
#pragma bss_seg(".bss$MaulA_ai_message")

typedef struct racepod_resource_s {    // size 0xa0
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
} RACEPOD_RESOURCE_PC;

typedef struct podrace_resource_s {        // size 0xb030
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
} PODRACE_RESOURCE;

/* Episode I level callbacks installed by FixUpLevels. */

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
void KaminoEGizmoMissing(char *name) {
}

typedef struct podrace_netpacket_reader_s { // size 0x14
    f32 start_countdown;                    // 0x00
    f32 remaining_time;                     // 0x04
    f32 available_time;                     // 0x08
    u32 unknown_0c;                         // 0x0c
    i32 lap;                                // 0x10
} PODRACE_NETPACKET_READER;

/* Pod sprint (Episode I bonus) level callbacks. */
typedef struct PODSPRINT_VIEW {   // size 0x94
    NUGSPLINE *finish_line;       // 0x00
    NUGSPLINE *halfway;           // 0x04
    PODSPRINT_AI ai[6];           // 0x08
    GIZAIMESSAGE *lap_msg;        // 0x68
    GIZAIMESSAGE *max_speed_msg;  // 0x6c
    GIZAIMESSAGE *min_speed_msg;  // 0x70
    GIZAIMESSAGE *speed_step_msg; // 0x74
    GAMEOBJECT *pacemaker;        // 0x78
    AIAREA *boulders;             // 0x7c
    f32 arrow_timer;              // 0x80
    f32 countdown;                // 0x84
    f32 lap_time;                 // 0x88
    u8 unknown_8c[2];             // 0x8c
    i8 ai_state;                  // 0x8e
    i8 ai_index;                  // 0x8f
    u8 unknown_90[2];             // 0x90

    union { // size 0x2
        u16 flags;

        struct {                     // size 0x2
            i16 halfway : 1;         // 0x92:0
            i16 unknown_1 : 1;       // 0x92:1
            i16 finished : 2;        // 0x92:2
            i16 reset_requested : 1; // 0x92:4
            i16 unknown_5 : 11;      // 0x92:5
        } bits;
    }; // 0x92
} PODSPRINT_VIEW;

STATIC_ASSERT(sizeof(PODSPRINT_VIEW) == sizeof(PODSPRINT_PC));

typedef struct pod_race_panel_netpacket_s { // size 0x14
    f32 start_countdown;                    // 0x00
    f32 remaining_time;                     // 0x04
    f32 available_time;                     // 0x08
    f32 lap_alpha;                          // 0x0c
    i32 lap;                                // 0x10
} POD_RACE_PANEL_NETPACKET;
