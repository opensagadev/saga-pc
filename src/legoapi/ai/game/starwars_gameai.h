/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void PlayerInitCapabilitiesVariant1(GAMEOBJECT *);
    extern f32 jump_stuck_time;
    i32 StarWars_PrepareR2D2Glide(AIPACKET *, APIOBJECT *, i32);
    i32 StarWars_PrepareHatch(AIPACKET *, APIOBJECT *, i32);
    i32 StarWars_PrepareHoverTube(AIPACKET *, APIOBJECT *, i32);
    i32 StarWars_PrepareBigJump(AIPACKET *, APIOBJECT *, i32);
    i32 StarWars_PrepareTakeOverJump(AIPACKET *, APIOBJECT *, i32);
    i32 StarWars_PrepareObstacle(AIPACKET *, APIOBJECT *, i32);
    i32 StarWars_PrepareJump(AIPACKET *, APIOBJECT *, i32);
#ifdef __cplusplus
}
#endif
