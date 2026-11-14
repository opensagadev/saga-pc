#ifndef SAGA_LEGOAPI_WORLD_LEVELS_EPISODEI_H
#define SAGA_LEGOAPI_WORLD_LEVELS_EPISODEI_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: gameapi/ai/aisys/aisys.h, legoapi/items/base/apiobject.h, legoapi/legoapi_types.h
 * (not all of it earlier in the shim). */

#include "gameapi/ai/aisys/aiscript_types.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#endif

typedef struct PODSPRINT_AI_PC {
    NUGSPLINE *spline;
    i16 vals[6];
} PODSPRINT_AI_PC;

STATIC_ASSERT(sizeof(PODSPRINT_AI_PC) == 0x10);

typedef struct PODSPRINT_PC {
    u8 unknown_0[8];
    PODSPRINT_AI_PC ai[6];
    struct gizaimessage_s *lap_msg;
    struct gizaimessage_s *max_speed_msg;
    struct gizaimessage_s *min_speed_msg;
    struct gizaimessage_s *speed_step_msg;
    void *field_78;
    void *boulders;
    f32 field_80;
    f32 speed;
    f32 field_88;
    u8 unknown_8c[0x2];
    u8 ai_state;
    i8 ai_index;
    u8 unknown_90_b[0x2];
    u8 flags;
    u8 unknown_93[0x1];
} PODSPRINT_PC;

STATIC_ASSERT(offsetof(PODSPRINT_PC, ai) == 8);
STATIC_ASSERT(offsetof(PODSPRINT_PC, ai_index) == 0x8f);
STATIC_ASSERT(sizeof(PODSPRINT_PC) == 0x94);
STATIC_ASSERT(offsetof(PODSPRINT_PC, speed) == 0x84);
#ifdef __cplusplus
extern "C" {
#endif
    extern const char EpisodeVI_ShowHearts[11];

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

#ifndef TYPES_OFF_30
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

typedef struct PODSPRINT_NETPACKET_PC {
    i16 ai_state;
    u8 unknown_02[4];
    i16 countdown;
    i16 lap_time;
} PODSPRINT_NETPACKET_PC;

#endif /* TYPES_OFF_30 */
#endif
