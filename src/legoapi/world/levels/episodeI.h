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

typedef struct PODSPRINT_AI_PC { // size 0x10
    NUGSPLINE *spline;           // 0x0
    i16 vals[6];                 // 0x4
} PODSPRINT_AI_PC;

typedef struct PODSPRINT_PC {              // size 0x94
    u8 unknown_0[8];                       // 0x00
    PODSPRINT_AI_PC ai[6];                 // 0x08
    struct gizaimessage_s *lap_msg;        // 0x68
    struct gizaimessage_s *max_speed_msg;  // 0x6c
    struct gizaimessage_s *min_speed_msg;  // 0x70
    struct gizaimessage_s *speed_step_msg; // 0x74
    void *field_78;                        // 0x78
    void *boulders;                        // 0x7c
    f32 field_80;                          // 0x80
    f32 speed;                             // 0x84
    f32 field_88;                          // 0x88
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
    i32 Action_CreatePod(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
    /* PodRacePc is a private static of episodeI.c. */
    extern GAMEOBJECT *PodPacemakerPc;
    extern f32 PodRaceSplineIncrementPc;
    extern char PodCreateSplineKeywordPc[11];
    extern char PodCreateStartKeywordPc[14];
    extern char PodCreateEndKeywordPc[12];
    extern char PodCreateTimeKeywordPc[13];
    extern char PodCreatePacemakerKeywordPc[10];
    extern char PodCreateDamagedKeywordPc[8];

    extern GAMEOBJECT *PodPacemakerPc;
    extern char PodRaceMaxLapTimeKeywordPc[12];
    extern char PodRaceLapTimeKeywordPc[9];
    extern char PodRaceAttemptsKeywordPc[25];
    extern char PodRaceIncrementKeywordPc[11];
    i32 Action_SetLapTime(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **, i32, i32, f32);
    void PodDust(WORLDINFO *, GAMEOBJECT *);
    extern i32 PodDustLocatorIndicesPc[2];
    extern f32 PodDustEmissionRatePc;
    extern f32 PodDustHeightScalePc;
    i32 PodRace_InStartCountdown(WORLDINFO *);
    void SetPodMergeAnims(ANIMPACKET_PC *, i32);
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
