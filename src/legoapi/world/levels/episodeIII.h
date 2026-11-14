#ifndef SAGA_LEGOAPI_WORLD_LEVELS_EPISODEIII_H
#define SAGA_LEGOAPI_WORLD_LEVELS_EPISODEIII_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: decomp.h, legoapi/items/base/apiobject.h, legoapi/items/objects/objectsall.h,
 * legoapi/legoapi_types.h (not all of it earlier in the shim). */

#include "legoapi/world/levels/episodeIII_types.h"
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

#endif
