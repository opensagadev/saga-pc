#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/world/level.h"

#ifdef __cplusplus
extern "C" {
#endif

    void Shadow_SetMode(void);
    extern AREA_DATA *NEWTOWN_ADATA;
    extern AREA_DATA *HOTHESCAPE_ADATA;
    extern SoundTable CurrentSFXTAB;
    void AddFootSteps(GAMEOBJECT *);
    extern f32 sfx_wait;
    extern LEVELDATA *dat_0087b264;
    extern i32 CruiserD_LiftChase;
    i32 GameAudio_OverrideFootStep_LSW(GAMEOBJECT *object, i32 on_platform);
    i32 GameAudio_CheckReverb_LSW(void);
#ifdef __cplusplus
}
#endif
