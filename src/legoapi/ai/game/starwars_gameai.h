#ifndef SAGA_LEGOAPI_AI_GAME_STARWARS_GAMEAI_H
#define SAGA_LEGOAPI_AI_GAME_STARWARS_GAMEAI_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void PlayerInitCapabilitiesVariant1Pc(GAMEOBJECT *);
    extern f32 jump_stuck_time;
    i32 StarWars_PrepareR2D2Glide(AIPACKET_PC *, APIOBJECT_PC *, i32);
    i32 StarWars_PrepareHatch(AIPACKET_PC *, APIOBJECT_PC *, i32);
    i32 StarWars_PrepareHoverTube(AIPACKET_PC *, APIOBJECT_PC *, i32);
    i32 StarWars_PrepareBigJump(AIPACKET_PC *, APIOBJECT_PC *, i32);
    i32 StarWars_PrepareTakeOverJump(AIPACKET_PC *, APIOBJECT_PC *, i32);
    i32 StarWars_PrepareObstacle(AIPACKET_PC *, APIOBJECT_PC *, i32);
    i32 StarWars_PrepareJump(AIPACKET_PC *, APIOBJECT_PC *, i32);
#ifdef __cplusplus
}
#endif

#endif
