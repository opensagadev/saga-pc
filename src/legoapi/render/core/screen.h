#ifndef SAGA_LEGOAPI_RENDER_CORE_SCREEN_H
#define SAGA_LEGOAPI_RENDER_CORE_SCREEN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h (not all of it earlier in the shim). */

#include "legoapi/world/world.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#endif

typedef struct level_display_pc_s {
    f32 farclip_hack;
    f32 near_clip;
    i16 far_clip;
    u8 bg_red_top;
    u8 bg_red_bottom;
    u8 bg_green_top;
    u8 bg_green_bottom;
    u8 bg_blue_top;
    u8 bg_blue_bottom;
} LEVEL_DISPLAY_PC;

STATIC_ASSERT(sizeof(LEVEL_DISPLAY_PC) == 0x10);
STATIC_ASSERT(offsetof(LEVEL_DISPLAY_PC, far_clip) == 0x08);
STATIC_ASSERT(offsetof(LEVEL_DISPLAY_PC, bg_red_top) == 0x0a);
STATIC_ASSERT(offsetof(LEVEL_DISPLAY_PC, bg_blue_bottom) == 0x0f);
#ifdef __cplusplus
extern "C" {
#endif
    void WidescreenCode(i32);
    void BackDrop_Draw(f32, i32);
    void DrawParallax(NUSPECIALHANDLE *);
    void NoRender(void);
    extern NUSPECIALHANDLE RenderBackdropSpecialsPc[4];
    extern NUGSCN *backdrop_scene;
    extern void (*RenderBackdropAlphaFnPc)(f32 *);
    void UpdateCutBorders(void);
    void GrabStillScreen(void);
    extern u8 ScreenGrabNeeded;
    extern f32 pause_fade;
    extern u32 LevelCompletionFallbackFlagPC;
    extern i32 clear_screen_onstill;
    void CreateFadeMaterials(void);

    extern f32 RenderBackdropTopRPc;
    extern f32 RenderBackdropTopGPc;
    extern f32 RenderBackdropTopBPc;
    extern f32 RenderBackdropBottomRPc;
    extern f32 RenderBackdropBottomGPc;
    extern f32 RenderBackdropBottomBPc;
    extern i32 RenderBackdropBlackPc;
    void BackDrop_ClearColourTargets(void);
    void BackDrop_UpdateColours(i32);
    void BackDrop_Init(char *, VARIPTR *, VARIPTR *);
    void BackDrop_Update(f32);
    extern i32 cut_waiting_for_new_level;
    void InitStillRender(VARIPTR *buffer, VARIPTR end);

    typedef struct gamefog_state_pc_s {
        f32 colour_channels[4];
        f32 start;
        f32 end;
        f32 density;
        u32 colour;
        f32 low_quality_density;
        f32 low_quality_colour_channels[4];
        u32 low_quality_colour;
    } GAMEFOG_STATE_PC;

    STATIC_ASSERT(offsetof(GAMEFOG_STATE_PC, colour) == 0x1c);
    STATIC_ASSERT(sizeof(GAMEFOG_STATE_PC) == 0x38);
    extern GAMEFOG_STATE_PC GameFog;
    extern GAMEFOG_STATE_PC GameFogOld;
    void GameFog_Update(WORLDINFO *world);
    extern i32 back_rgba[2];
    extern i32 old_pause_state;
    void NeedScreenGrab(i32);
#ifdef __cplusplus
}
#endif

#endif
