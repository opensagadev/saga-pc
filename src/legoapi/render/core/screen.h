#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nuhspecial.h"

struct nuhspecial_s;

struct NUGSCN_s;
struct WORLDINFO_s;

#ifdef __cplusplus
extern "C" {
#endif
    void WidescreenCode(i32);
    void BackDrop_Draw(f32, i32);
    void DrawParallax(struct nuhspecial_s *);
    void NoRender(void);
    extern nuhspecial_s RenderBackdropSpecials[4];
    extern struct NUGSCN_s *backdrop_scene;
    extern void (*RenderBackdropAlphaFn)(f32 *);
    void UpdateCutBorders(void);
    void GrabStillScreen(void);
    extern u8 ScreenGrabNeeded;
    extern f32 pause_fade;
    extern u32 LevelCompletionFallbackFlagPC;
    extern i32 clear_screen_onstill;
    void CreateFadeMaterials(void);

    extern f32 RenderBackdropTopR;
    extern f32 RenderBackdropTopG;
    extern f32 RenderBackdropTopB;
    extern f32 RenderBackdropBottomR;
    extern f32 RenderBackdropBottomG;
    extern f32 RenderBackdropBottomB;
    extern i32 RenderBackdropBlack;
    void BackDrop_ClearColourTargets(void);
    void BackDrop_UpdateColours(i32);
    void BackDrop_Init(char *, VARIPTR *, VARIPTR *);
    void BackDrop_Update(f32);
    extern i32 cut_waiting_for_new_level;
    void InitStillRender(VARIPTR *buffer, VARIPTR end);

    typedef struct gamefog_state_s {        // size 0x38
        f32 colour_channels[4];             // 0x00
        f32 start;                          // 0x10
        f32 end;                            // 0x14
        f32 density;                        // 0x18
        u32 colour;                         // 0x1c
        f32 low_quality_density;            // 0x20
        f32 low_quality_colour_channels[4]; // 0x24
        u32 low_quality_colour;             // 0x34
    } GAMEFOG_STATE;

    extern GAMEFOG_STATE GameFog;
    extern GAMEFOG_STATE GameFogOld;
    void GameFog_Update(struct WORLDINFO_s *world);
    extern i32 back_rgba[2];
    extern i32 old_pause_state;
    void NeedScreenGrab(i32);
    void ResizeStillRender(i32, i32);
#ifdef __cplusplus
}
#endif
