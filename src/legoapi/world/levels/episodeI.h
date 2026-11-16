#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/ai/aisys/aiscript_types.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuspline.h"

typedef struct PODSPRINT_AI { // size 0x10
    NUGSPLINE *spline;        // 0x0
    i16 vals[6];              // 0x4
} PODSPRINT_AI;

typedef struct PODSPRINT {                 // size 0x94
    NUGSPLINE *finish_line;                // 0x00
    NUGSPLINE *halfway;                    // 0x04
    PODSPRINT_AI ai[6];                    // 0x08
    struct gizaimessage_s *lap_msg;        // 0x68
    struct gizaimessage_s *max_speed_msg;  // 0x6c
    struct gizaimessage_s *min_speed_msg;  // 0x70
    struct gizaimessage_s *speed_step_msg; // 0x74
    GAMEOBJECT *pacemaker;                 // 0x78
    AIAREA *boulders;                      // 0x7c
    f32 arrow_timer;                       // 0x80
    f32 countdown;                         // 0x84
    f32 lap_time;                          // 0x88
    i16 pacemaker_laps;                    // 0x8c laps the pacemaker finished; each slows it by speed_step_msg
    i8 ai_state;                           // 0x8e
    i8 ai_index;                           // 0x8f
    u8 unknown_90[2];                      // 0x90

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
} PODSPRINT;

/* PODSPRINT.flags, the named bitfields */
#define PODSPRINT_HALFWAY 0x1
#define PODSPRINT_RESET_REQUESTED 0x10

#ifdef __cplusplus
extern "C" {
#endif

    f32 PodSprint_InStartCountdown(WORLDINFO *);
    extern i32 Lap;
    i32 Action_CreatePod(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    /* ActivePodRace is a private static of episodeI.c. */
    extern GAMEOBJECT *PodPacemaker;
    extern f32 PodRaceSplineIncrement;

    extern GAMEOBJECT *PodPacemaker;
    i32 Action_SetLapTime(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
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
    extern i16 dat_007f19ac;
    extern i16 dat_007f19b0;
    extern i16 dat_007f19b4;
    f32 PodSprint_RollMul(GAMEOBJECT *);
    i32 Action_MushroomCollapse(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    i32 Action_NewSebulba(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    i32 Action_Sebulba(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, i32, i32, f32);
    void AnakinsFlightB_Init(WORLDINFO *);
    void GunganA_Init(WORLDINFO *);
    void GunganA_Update(WORLDINFO *);
    void MaulA_Init(WORLDINFO *);
    void MaulA_Panel(WORLDINFO *);
    void MaulA_Reset(WORLDINFO *);
    void MaulF_Init(WORLDINFO *);
    void MaulF_Panel(WORLDINFO *);
    void MaulF_Reset(WORLDINFO *);
    void PodRaceAInit(WORLDINFO *);
    void PodRaceAReset(WORLDINFO *);
    void PodRaceA_AlwaysUpdate(WORLDINFO *);
    void PodRaceA_Draw(WORLDINFO *);
    void PodRaceA_Update(WORLDINFO *);
    void PodRaceBInit(WORLDINFO *);
    void PodRaceBReset(WORLDINFO *);
    void PodRaceBUpdate(WORLDINFO *);
    void PodRaceCInit(WORLDINFO *);
    void PodRaceCReset(WORLDINFO *);
    void PodRaceCUpdate(WORLDINFO *);
    void PodRace_IncreaseLap(void);
    void PodSprintA_Init(WORLDINFO *);
    void PodSprintA_Panel(WORLDINFO *);
    void PodSprintA_Reset(WORLDINFO *);
    void PodSprintA_Update(WORLDINFO *);
    void RetakeG_Init(WORLDINFO *);
    void RetakeG_Panel(WORLDINFO *);
    void RetakeG_Update(WORLDINFO *);
    void AnakinsFlightB_Draw(WORLDINFO *);
    void AnakinsFlightB_Update(WORLDINFO *);
    void KaminoEGizmoMissing(char *name);
    void MaulA_Update(WORLDINFO *);
    void MaulD_Init(WORLDINFO *);
    void MaulD_Update(WORLDINFO *);
    void MaulE_Init(WORLDINFO *);
    void MaulE_Update(WORLDINFO *);
    void MaulF_Update(WORLDINFO *);
    void RetakeG_Reset(WORLDINFO *);
#ifdef __cplusplus
}
#endif

typedef struct PODSPRINT_NETPACKET {
    i16 ai_state;
    u8 unknown_02[4];
    i16 countdown;
    i16 lap_time;
} PODSPRINT_NETPACKET;

/* One bit per pod race mine, kept as two words for the network packet. */
typedef union mine_bits_u { // size 0x8
    u32 words[2];
    u64 bits;
} MINE_BITS;

typedef struct client_mines_s { // size 0x318
    NUVEC positions[64];        // 0x000
    MINE_BITS present;          // 0x300
    MINE_BITS exploded;         // 0x308
    u32 unknown_310[2];         // 0x310
} CLIENT_MINES;

typedef struct pod_race_countdown_reader_s { // size 0xb004
    u8 unknown_0000[0xb000];                 // 0x0000
    f32 lap_countdown;                       // 0xb000
} POD_RACE_COUNTDOWN_READER;

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
