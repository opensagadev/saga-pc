/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "decomp.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/world/levels/episodeIII_types.h"
#include "nu2api/numath/nuvec.h"
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
