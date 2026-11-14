#ifndef SAGA_LEGOAPI_AI_GAME_LSW_HUB_AI_H
#define SAGA_LEGOAPI_AI_GAME_LSW_HUB_AI_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: decomp.h, gameapi/ai/aisys/aisys.h, legoapi/items/base/apiobject.h (not all of it
 * earlier in the shim). */

#include "gameapi/ai/aisys/aiscript_types.h"
#include "legoapi/world/world.h"

#ifndef SAGA_SHIM
#include "decomp.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i16 VehicleBoltId7f1910Pc;
    extern HUB_AI_MUSIC_VIEW_PC HubAiMusicStatePc;
    i32 Hub_Outside(void);
    i32 Hub_GetRandomCharType(void);
    void LSW_Hub_InitAI(WORLDINFO *);
    void LSW_Hub_ResetAI(WORLDINFO *);
    void LSW_Hub_UpdateAI(WORLDINFO *);
    void *Condition_InHubAreaInit(AISYS_PC *, char *, AISCRIPT_PC *);
    f32 Condition_InHubArea(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char *, void *);
#ifdef __cplusplus
}
#endif

#endif
