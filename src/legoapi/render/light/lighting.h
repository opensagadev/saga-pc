/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/render/core/rtl.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern NUVEC LightingUnitDirectionPc;
    extern struct PC_SPECIAL_SHADOW_LIGHT *PCShadowCurrentLight;
    extern struct PC_SPECIAL_SHADOW_LIGHT *PCShadowSecondaryLight;
    void SetZeroLights(void);
    void ResetLights(NUVEC *, struct RTL_DATA_PC *, void *);
    void FindAndSetLights(NUVEC *, f32, void *);
    extern i32 ResetOldFStop;
    extern NUVEC APIObjectZeroMomentumPC;
    void InitGameObjectLights(void);

    void LightGameObject(GAMEOBJECT *, void *);
    RTL_SET_PC *rtlGetEnvSet(void);

    extern RTL_DATA_PC LevelLightingDataPc;
    extern NUVEC PanelLightingColoursPc[3];
    extern NUVEC PanelLightingAmbientPc;
    extern const f64 LightingSurfaceFadeSlopePc;
    extern f32 Lighting_RedFlashRedPc;
    extern f32 Lighting_BlueFlickerGreenPc;
    extern f32 Lighting_BlueFlickerBluePc;
    extern i32 Lighting_HighlightFlash;
    extern f32 Lighting_RedFlashGreenPc;
    extern f32 Lighting_RedFlashBluePc;
    extern f32 Lighting_BlueFlickerRedPc;
    extern i32 (*Lighting_BlueFlickerFn)(GAMEOBJECT *);
    void SetLights(NUVEC *, NUVEC *, NUVEC *, NUVEC *, NUVEC *, NUVEC *, NUVEC *);
    void SetLights_RTLDATA(RTL_DATA_PC *, f32);
    void SetLevelLights(void *, f32);
    void SetPanelLights(f32);
    void FreeGameObjectLights(void);
    void ResetShadowMapRenderingFn(void);
    void EnableShadowMapRenderingFn(void);

    void RenderShadowLights(i32);
    void SetShadowLight(void);
#ifdef __cplusplus
}
#endif
