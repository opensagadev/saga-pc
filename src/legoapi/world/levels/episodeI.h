/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aiscript_types.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuspline.h"
#endif

typedef struct PODSPRINT_AI { // size 0x10
    NUGSPLINE *spline;        // 0x0
    i16 vals[6];              // 0x4
} PODSPRINT_AI;

typedef struct PODSPRINT_PC {              // size 0x94
    u8 unknown_0[8];                       // 0x00
    PODSPRINT_AI ai[6];                    // 0x08
    struct gizaimessage_s *lap_msg;        // 0x68
    struct gizaimessage_s *max_speed_msg;  // 0x6c
    struct gizaimessage_s *min_speed_msg;  // 0x70
    struct gizaimessage_s *speed_step_msg; // 0x74
    void *unknown_78;                      // 0x78
    void *boulders;                        // 0x7c
    f32 unknown_80;                        // 0x80
    f32 speed;                             // 0x84
    f32 unknown_88;                        // 0x88
    u8 unknown_8c[0x2];                    // 0x8c
    u8 ai_state;                           // 0x8e
    i8 ai_index;                           // 0x8f
    u8 unknown_90_b[0x2];                  // 0x90
    u8 flags;                              // 0x92
    u8 unknown_93[0x1];                    // 0x93
} PODSPRINT_PC;

#ifdef __cplusplus
extern "C" {
#endif

    f32 PodSprint_InStartCountdown(WORLDINFO *);
    extern i32 Lap;
    i32 Action_CreatePod(AISYS_PC *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    /* PodRacePc is a private static of episodeI.c. */
    extern GAMEOBJECT *PodPacemaker;
    extern f32 PodRaceSplineIncrement;
    extern char PodCreateSplineKeyword[11];
    extern char PodCreateStartKeyword[14];
    extern char PodCreateEndKeyword[12];
    extern char PodCreateTimeKeyword[13];
    extern char PodCreatePacemakerKeyword[10];
    extern char PodCreateDamagedKeyword[8];

    extern GAMEOBJECT *PodPacemaker;
    extern char PodRaceMaxLapTimeKeyword[12];
    extern char PodRaceLapTimeKeyword[9];
    extern char PodRaceAttemptsKeyword[25];
    extern char PodRaceIncrementKeyword[11];
    i32 Action_SetLapTime(AISYS_PC *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    void PodDust(WORLDINFO *, GAMEOBJECT *);
    extern i32 PodDustLocatorIndices[2];
    extern f32 PodDustEmissionRate;
    extern f32 PodDustHeightScale;
    i32 PodRace_InStartCountdown(WORLDINFO *);
    void SetPodMergeAnims(ANIMPACKET *, i32);
    extern u8 gunship_flags[10];
    extern struct gameobject_s *Maul_obj;
    void PodRaceReset(void);
    extern f32 mushroom0_along;
    extern f32 mushroom2_along;
    extern i32 mushroom_collapse;
    extern f32 mushroom_max_time_available;
    extern i32 mushroom_nattempts_per_increment;
    extern f32 mushroom_time_available;
    extern f32 mushroom_time_increment;
    void PodRaceSnipersReset(void);
    void PodSprint_GetIAlongVals(struct nugspline_s *spline, i16 *first_point, i16 *last_point);
    extern i32 do_mine;
    extern f32 max_speed_mul;
    extern f32 min_speed_mul;
    extern f32 sebulba_lerpf;
    extern f32 sockstep;
    extern f32 test_factor;
    extern f32 test_seek_rate;
#ifdef __cplusplus
}
#endif

typedef struct PODSPRINT_NETPACKET {
    i16 ai_state;
    u8 unknown_02[4];
    i16 countdown;
    i16 lap_time;
} PODSPRINT_NETPACKET;

typedef struct client_mines_s { // size 0x318
    NUVEC positions[64];        // 0x000
    u32 present_words[2];       // 0x300
    u32 exploded_words[2];      // 0x308
    u32 unknown_310[2];         // 0x310
} CLIENT_MINES;

typedef struct pod_race_countdown_reader_s { // size 0xb004
    u8 unknown_0000[0xb000];                 // 0x0000
    f32 lap_countdown;                       // 0xb000
} POD_RACE_COUNTDOWN_READER;

typedef struct podrace_levflag_s { // size 0x10
    u8 podrace_state;              // 0x0
    u8 mushroom_state;             // 0x1
    u8 remaining[14];              // 0x2
} PODRACE_LEVFLAG;

typedef struct gungan_a_s { // size 0x114
    i16 unknown_000;        // 0x000
    i16 count;              // 0x002
    AILOCATOR *origins[32]; // 0x004
    AILOCATOR *targets[32]; // 0x084
    f32 spawn_timer;        // 0x104
    i16 model_index;        // 0x108
    i16 unknown_10a;        // 0x10a
    i16 models[4];          // 0x10c
} GUNGAN_A;
