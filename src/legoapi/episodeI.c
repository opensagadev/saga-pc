#include <stdio.h>
#include "saga.h"
#include <math.h>
#pragma bss_seg(".bss$MaulA_ai_message")

typedef struct racepod_resource_pc_s {
    NUMTX matrix;
    NUVEC4 previous_axis;
    NUVEC4 previous_position;
    NUVEC4 previous_displacement;
    i32 pitch, yaw, unknown_0078;
    f32 speed;
    FLIGHT_SPLINE_RESOURCE_PC *spline;
    f32 start;
    i16 model_id;
    i16 unknown_008a;
    f32 distance;
    GAMEOBJECT *object;
    void *next;
    u8 unknown_0098[8];
} RACEPOD_RESOURCE_PC;

STATIC_ASSERT(sizeof(RACEPOD_RESOURCE_PC) == 0xa0);
STATIC_ASSERT(offsetof(RACEPOD_RESOURCE_PC, pitch) == 0x70);
STATIC_ASSERT(offsetof(RACEPOD_RESOURCE_PC, yaw) == 0x74);
STATIC_ASSERT(offsetof(RACEPOD_RESOURCE_PC, speed) == 0x7c);
STATIC_ASSERT(offsetof(RACEPOD_RESOURCE_PC, spline) == 0x80);
STATIC_ASSERT(offsetof(RACEPOD_RESOURCE_PC, start) == 0x84);
STATIC_ASSERT(offsetof(RACEPOD_RESOURCE_PC, model_id) == 0x88);
STATIC_ASSERT(offsetof(RACEPOD_RESOURCE_PC, distance) == 0x8c);
STATIC_ASSERT(offsetof(RACEPOD_RESOURCE_PC, object) == 0x90);
STATIC_ASSERT(offsetof(RACEPOD_RESOURCE_PC, next) == 0x94);

typedef struct podrace_resource_pc_s {
    FLIGHT_SPLINE_RESOURCE_PC splines[32];
    RACEPOD_RESOURCE_PC lap_entries[16];
    f32 lap_countdown;
    f32 mushroom_timer;
    f32 lap_display;
    f32 previous_lap_display;
    f32 max_lap_time, lap_time_increment;
    i32 attempts_per_increment;
    i32 attempts;
    u8 flags;
    u8 unknown_b021[15];
} PODRACE_RESOURCE_PC;

STATIC_ASSERT(sizeof(PODRACE_RESOURCE_PC) == 0xb030);
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, lap_entries) == 0xa600);
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, mushroom_timer) == 0xb004);
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, previous_lap_display) == 0xb00c);
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, flags) == 0xb020);

STATIC_ASSERT(offsetof(SNIPER_s, prev) == 0);
STATIC_ASSERT(offsetof(SNIPER_s, pos) == 0xc);
STATIC_ASSERT(offsetof(SNIPER_s, fire_timer) == 0x18);
STATIC_ASSERT(offsetof(SNIPER_s, state) == 0x1c);

STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, max_lap_time) == 0xb010);
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, lap_time_increment) == 0xb014);
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, attempts_per_increment) == 0xb018);

typedef struct pod_race_countdown_reader_pc_s {
    u8 unknown_0000[0xb000];
    f32 lap_countdown;
} POD_RACE_COUNTDOWN_READER_PC;

STATIC_ASSERT(offsetof(POD_RACE_COUNTDOWN_READER_PC, lap_countdown) == 0xb000);

typedef struct pod_merge_packet_reader_pc_s {
    u8 unknown_00[0x10];
    f32 blend_source_time;
    f32 blend_target_time;
    u8 unknown_18[0x22];
    i16 requested_animation;
    u8 unknown_3c[6];
    i16 overlay_animation;
    f32 overlay_weight;
} POD_MERGE_PACKET_READER_PC;

STATIC_ASSERT(offsetof(POD_MERGE_PACKET_READER_PC, requested_animation) == 0x3a);
STATIC_ASSERT(offsetof(POD_MERGE_PACKET_READER_PC, blend_source_time) == 0x10);
STATIC_ASSERT(offsetof(POD_MERGE_PACKET_READER_PC, blend_target_time) == 0x14);
STATIC_ASSERT(offsetof(POD_MERGE_PACKET_READER_PC, overlay_animation) == 0x42);
STATIC_ASSERT(offsetof(POD_MERGE_PACKET_READER_PC, overlay_weight) == 0x44);

/* Episode I level callbacks installed by FixUpLevels (retail 0x4d82f0 .. 0x4dd770). */
typedef struct podrace_levflag_pc_s {
    u8 podrace_state;
    u8 mushroom_state;
    u8 remaining[14];
} PODRACE_LEVFLAG_PC;

STATIC_ASSERT(sizeof(PODRACE_LEVFLAG_PC) == 16);

typedef struct gungan_a_pc_s {
    i16 unknown_000;
    i16 count;
    AILOCATOR_PC *origins[32];
    AILOCATOR_PC *targets[32];
    f32 spawn_timer;
    i16 model_index;
    i16 unknown_10a;
    i16 models[4];
} GUNGAN_A_PC;

STATIC_ASSERT(sizeof(GUNGAN_A_PC) == 0x114);
STATIC_ASSERT(offsetof(GUNGAN_A_PC, targets) == 0x84);
STATIC_ASSERT(offsetof(GUNGAN_A_PC, spawn_timer) == 0x104);
STATIC_ASSERT(offsetof(GUNGAN_A_PC, model_index) == 0x108);
STATIC_ASSERT(offsetof(GUNGAN_A_PC, models) == 0x10c);

typedef struct mine_entry_pc_s {
    u32 active;
    NUVEC pos;
    f32 grow;
    f32 grow_alpha;
    u16 rotx;
    u16 roty;
} MINE_ENTRY_PC;

STATIC_ASSERT(sizeof(MINE_ENTRY_PC) == 0x1c);

typedef struct minesys_pc_s {
    NUSPECIALHANDLE mine_special;
    MINE_ENTRY_PC mines[64];
    f32 mine_radius;
    f32 spawn_timer;
    AIAREA_PC *nomine_areas[10];
    i16 nomine_count;
    i16 mine_debris;
    i32 mine_part;
    f32 update_timer;
} MINESYS_PC;

STATIC_ASSERT(sizeof(MINESYS_PC) == 0x748);
STATIC_ASSERT(offsetof(MINESYS_PC, mines) == 0xc);
STATIC_ASSERT(offsetof(MINESYS_PC, mine_radius) == 0x70c);
STATIC_ASSERT(offsetof(MINESYS_PC, spawn_timer) == 0x710);
STATIC_ASSERT(offsetof(MINESYS_PC, nomine_count) == 0x73c);
STATIC_ASSERT(offsetof(MINESYS_PC, mine_part) == 0x740);
STATIC_ASSERT(offsetof(MINESYS_PC, update_timer) == 0x744);

typedef struct client_mines_pc_s {
    NUVEC positions[64];
    u32 present_words[2];
    u32 exploded_words[2];
    u32 unknown_310[2];
} CLIENT_MINES_PC;

STATIC_ASSERT(sizeof(CLIENT_MINES_PC) == 0x318);
STATIC_ASSERT(offsetof(CLIENT_MINES_PC, present_words) == 0x300);

typedef struct cut_instance_reader_pc_s {
    u8 opaque_00_88[0x88];
    u32 flags_88;
    u32 unknown_8c;
    f32 current_frame;
} CUT_INSTANCE_READER_PC;

STATIC_ASSERT(offsetof(CUT_INSTANCE_READER_PC, flags_88) == 0x88);
STATIC_ASSERT(offsetof(CUT_INSTANCE_READER_PC, current_frame) == 0x90);

void __cdecl RetakeG_Reset(WORLDINFO *world) {
}

void __cdecl MaulA_Update(WORLDINFO *world) {
}

void __cdecl MaulD_Init(WORLDINFO *world) {
}

void __cdecl MaulD_Update(WORLDINFO *world) {
}

void __cdecl MaulE_Init(WORLDINFO *world) {
}

void __cdecl MaulE_Update(WORLDINFO *world) {
}

void __cdecl MaulF_Update(WORLDINFO *world) {
}

void __cdecl AnakinsFlightB_Update(WORLDINFO *world) {
}

void __cdecl AnakinsFlightB_Draw(WORLDINFO *world) {
}

/* Empty level debug hook shared with the Episode II callbacks. */
void __cdecl KaminoEGizmoMissingPc(char *name) {
}

typedef struct podrace_netpacket_reader_pc_s {
    f32 start_countdown;
    f32 remaining_time;
    f32 available_time;
    u32 unknown_0c;
    i32 lap;
} PODRACE_NETPACKET_READER_PC;

STATIC_ASSERT(offsetof(PODRACE_NETPACKET_READER_PC, lap) == 0x10);

/* Pod sprint (Episode I bonus) level callbacks. */
typedef struct PODSPRINT_VIEW_PC {
    NUGSPLINE *finish_line;
    NUGSPLINE *halfway;
    PODSPRINT_AI_PC ai[6];
    GIZAIMESSAGE *lap_msg;
    GIZAIMESSAGE *max_speed_msg;
    GIZAIMESSAGE *min_speed_msg;
    GIZAIMESSAGE *speed_step_msg;
    GAMEOBJECT *pacemaker;
    AIAREA_PC *boulders;
    f32 arrow_timer;
    f32 countdown;
    f32 lap_time;
    u8 unknown_8c[2];
    i8 ai_state;
    i8 ai_index;
    u8 unknown_90[2];

    union {
        u16 flags;

        struct {
            i16 halfway : 1;
            i16 unknown_1 : 1;
            i16 finished : 2;
            i16 reset_requested : 1;
            i16 unknown_5 : 11;
        } bits;
    };
} PODSPRINT_VIEW_PC;

STATIC_ASSERT(sizeof(PODSPRINT_VIEW_PC) == sizeof(PODSPRINT_PC));
STATIC_ASSERT(offsetof(PODSPRINT_VIEW_PC, pacemaker) == 0x78);
STATIC_ASSERT(offsetof(PODSPRINT_VIEW_PC, ai_state) == 0x8e);
STATIC_ASSERT(offsetof(PODSPRINT_VIEW_PC, flags) == 0x92);

typedef struct PODSPRINT_NETPACKET_PC {
    i16 ai_state;
    u8 unknown_02[4];
    i16 countdown;
    i16 lap_time;
} PODSPRINT_NETPACKET_PC;

typedef struct pod_race_panel_netpacket_pc_s {
    f32 start_countdown;
    f32 remaining_time;
    f32 available_time;
    f32 lap_alpha;
    i32 lap;
} POD_RACE_PANEL_NETPACKET_PC;

STATIC_ASSERT(sizeof(POD_RACE_PANEL_NETPACKET_PC) == 0x14);
STATIC_ASSERT(offsetof(POD_RACE_PANEL_NETPACKET_PC, lap_alpha) == 0x0c);
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, lap_countdown) == 0xb000);
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, lap_display) == 0xb008);
STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
STATIC_ASSERT(sizeof(LEVEL_OBJECT_RUNTIME_PC) == 0x10);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, active) == 0x0e);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, special) == 0);
