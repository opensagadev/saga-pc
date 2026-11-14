#ifndef SAGA_LEGOAPI_WORLD_LEVELS_EPISODEIII_H
#define SAGA_LEGOAPI_WORLD_LEVELS_EPISODEIII_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: decomp.h, legoapi/items/base/apiobject.h, legoapi/items/objects/objectsall.h,
 * legoapi/legoapi_types.h (not all of it earlier in the shim). */

#include "legoapi/world/levels/episodeIII_types.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "decomp.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern LEVEL_INST_ANIM_RESET_VIEW_PC LevInstAnimResetPc;
    extern struct gameobject_s *Grievous_obj;
    extern struct gizaimessage_s *KashyyykA_msg_TotalWookies;
    extern struct gizaimessage_s *KashyyykA_msg_WookiesToRescue;
    extern struct gizaimessage_s *vader_b_complete_msg;
    extern u8 vader_b_playersDead;
    extern NUVEC TempleC_StreamStatusPos;
    extern struct PART_PC *boulder_part[2];
    void Boulder_Move(PART_PC *, f32);
    void Boulder_Kill(PART_PC *, i32);
    extern VADERA_PC vader_a;
#ifdef __cplusplus
}
#endif

#ifndef TYPES_OFF_32
typedef struct cruiserc_pc_s {
    GIZAIMESSAGE *dooku_fight;       // 0x00
    GAMEOBJECT *count_dooku;         // 0x04
    GIZAIMESSAGE *free_palpatine;    // 0x08
    GAMEOBJECT *palpatine;           // 0x0c
    AILOCATOR_PC *palpatine_locator; // 0x10
} CRUISERC_PC;                       // size 0x14

typedef struct cruiserd_lift_anim_pc_s {
    NUMTX matrix;        // 0x00
    f32 tfactor;         // 0x40
    u8 unknown_44[8];    // 0x44
    f32 ltime;           // 0x4c
    u32 playing : 1;     // 0x50:0
    u32 flags_rest : 31; // 0x50:1
    f32 fparam1;         // 0x54
    f32 prev_eval_time;  // 0x58
    u16 animation_index; // 0x5c
} CRUISERD_LIFT_ANIM_PC; // size 0x60

typedef struct vaderc_pc_s {
    GIZAIMESSAGE *final_fight_message; // 0x00
    AILOCATOR_PC *big_jump_locator;    // 0x04
    NUSPECIALHANDLE rocks[10];         // 0x08
    i16 platform_ids[10];              // 0x80
    u8 progress_mask;                  // 0x94
    u8 players_dead;                   // 0x95
    u8 reserved_96[2];                 // 0x96
} VADERC_PC;                           // size 0x98

#endif /* TYPES_OFF_32 */
#endif
