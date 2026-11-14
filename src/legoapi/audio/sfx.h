/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/world/level.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    void Shadow_SetMode(void);
    extern AREA_DATA_PC *NEWTOWN_ADATA;
    extern AREA_DATA_PC *HOTHESCAPE_ADATA;
    extern SoundTable CurrentSFXTAB;
    void AddFootSteps(GAMEOBJECT *);
    extern f32 sfx_wait;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b264;
    extern i32 CruiserD_LiftChase;
    i32 GameAudio_OverrideFootStep_LSW(GAMEOBJECT *object, i32 on_platform);
    i32 GameAudio_CheckReverb_LSW(void);
#ifdef __cplusplus
}
#endif
