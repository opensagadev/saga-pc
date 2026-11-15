#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/render/core/rtl.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern NUVEC LightingUnitDirection;
    extern struct PC_SPECIAL_SHADOW_LIGHT *PCShadowCurrentLight;
    extern struct PC_SPECIAL_SHADOW_LIGHT *PCShadowSecondaryLight;
    void SetZeroLights(void);
    void ResetLights(NUVEC *, struct RTL_DATA *, void *);
    void FindAndSetLights(NUVEC *, f32, void *);
    extern i32 ResetOldFStop;
    extern NUVEC APIObjectZeroMomentumPC;
    void InitGameObjectLights(void);

    void LightGameObject(GAMEOBJECT *, void *);
    RTL_SET *rtlGetEnvSet(void);

    extern RTL_DATA LevelLightingData;
    extern NUVEC PanelLightingColours[3];
    extern NUVEC PanelLightingAmbient;
    extern const f64 LightingSurfaceFadeSlope;
    extern f32 Lighting_RedFlashRed;
    extern f32 Lighting_BlueFlickerGreen;
    extern f32 Lighting_BlueFlickerBlue;
    extern i32 Lighting_HighlightFlash;
    extern f32 Lighting_RedFlashGreen;
    extern f32 Lighting_RedFlashBlue;
    extern f32 Lighting_BlueFlickerRed;
    extern i32 (*Lighting_BlueFlickerFn)(GAMEOBJECT *);
    void SetLights(NUVEC *, NUVEC *, NUVEC *, NUVEC *, NUVEC *, NUVEC *, NUVEC *);
    void SetLights_RTLDATA(RTL_DATA *, f32);
    void SetLevelLights(void *, f32);
    void SetPanelLights(f32);
    void FreeGameObjectLights(void);
    void ResetShadowMapRenderingFn(void);
    void EnableShadowMapRenderingFn(void);

    void RenderShadowLights(i32);
    void SetShadowLight(void);
    char *rtlGetEnvPath(void);
    char *rtlGetEnvSceneName(void);
    extern NUCAMERA *EdMainCurrentCamera;
#ifdef __cplusplus
}
#endif
