#include "saga.h"

#include <stdio.h>
#pragma bss_seg(".bss$pause_rndr_mtl")
static void *pause_rndr_mtl = 0;
#pragma bss_seg(".bss$pause_rt")
static i32 pause_rt = 0;

typedef struct STILL_RENDER_MATERIAL_PC { // size 0x2bc
    u8 unknown_00[0x40];                  // 0x000
    u32 attributes;                       // 0x040
    u8 unknown_44[0x2c];                  // 0x044
    f32 opacity;                          // 0x070
    i16 texture_id;                       // 0x074
    i16 sort_priority;                    // 0x076
    u8 unknown_78[0x3c];                  // 0x078
    PC_MATERIAL_SHADER_DESC_VIEW shader;  // 0x0b4
} STILL_RENDER_MATERIAL_PC;

typedef struct permanent_material_reader_pc_s {
    u8 unknown_0000[0x40];
    u32 attributes;
    u32 shader_flags;
    u8 unknown_0048[0xc];
    f32 diffuse[3];
    u8 unknown_0060[0x10];
    f32 opacity;
    i16 texture_id;
    u16 sort_priority;
} PERMANENT_MATERIAL_READER_PC;

extern "C" NUMTL *FadeMtl;
extern "C" NUMTL *FadeMtl2;
extern "C" NUMTL *ScreenFadeMtl;
extern "C" {
    extern void (*RenderBackdropAlphaFnPc)(f32 *);
}
extern "C" GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsFrameRenderTexturePc(i32);

typedef struct CUT_DISPLAY_VIEW_PC { // size 0xec
    u8 unknown_00[0x6c];             // 0x00
    f32 camera_near_clip;            // 0x6c
    u8 unknown_70[0x78];             // 0x70
    u16 camera_far_clip;             // 0xe8
} CUT_DISPLAY_VIEW_PC;

extern "C" f32 CutSceneCameraFocalLengthPc;

typedef struct CUT_BORDER_RECORD_PC { // size 0x54
    u8 unknown_00[0x50];              // 0x00
    u32 flags;                        // 0x50
} CUT_BORDER_RECORD_PC;

extern "C" NUMTL *FadeMtl;
extern "C" {
    void NuRndrGradRect2di(i32, i32, i32, i32, u32 *, NUMTL *);
}
extern "C" u8 ScreenGrabNeeded;
extern "C" f32 pause_fade;
extern "C" NUVPCURRENT *NuVpGetCurrentViewport(void);
extern "C" i32 ScreenStillXPc;
extern "C" i32 ScreenStillYPc;
extern "C" i32 ScreenStillWidthPc;
extern "C" i32 ScreenStillHeightPc;
extern "C" f32 ScreenPauseFadeSpeedPc;

extern "C" {

    void FadeLoop_SetObj(NUGSCN *scene, char *name) {
        FadeLoop_ObjScene = scene;
        if (scene != NULL && NuSpecialFind(scene, &FadeLoop_ObjHSpecial, name, 1) == 0) {
            FadeLoop_ObjScene = NULL;
        }
    }

    i32 fun_005644f0() {
        return FadeLoop_ObjScene != NULL;
    }

    void FadeLoop_DrawObj(float alpha) {
        if (FadeLoop_ObjScene != NULL && NuSpecialExistsFn(&FadeLoop_ObjHSpecial)) {
            NUVEC scale = {0.125f, 0.125f, 0.125f};
            NUMTX matrix;
            NuMtxSetScale(&matrix, &scale);
            matrix.m32 = 1.0f;
            NuSpecialDrawAtAlpha(&FadeLoop_ObjHSpecial, &matrix, alpha);
        }
    }
}

extern "C" void CreateFadeMaterials(void) {
    FadeMtl = NuMtlCreate(1);
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes |= 0xc000;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes & ~0x20u) | 0x10u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes & ~0x10000u) | 0x20000u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes & ~0x200u) | 0x100u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes & ~0x800u) | 0x400u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes |= 0x40000;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes & ~0xcu) | 3u;
    NuMtlUpdate(FadeMtl);

    FadeMtl2 = NuMtlCreate(1);
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes |= 0xc000;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes & ~0x20u) | 0x10u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes & ~0x10000u) | 0x20000u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes & ~0x200u) | 0x100u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes & ~0x800u) | 0x400u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes |= 0x40000;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes & ~0xeu) | 1u;
    NuMtlUpdate(FadeMtl2);

    ScreenFadeMtl = NuMtlCreate(1);
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes |= 0xc000;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes & ~0x200u) | 0x100u;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes & ~0x800u) | 0x400u;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes & ~0x20u) | 0x10u;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes & ~0x10000u) | 0x20000u;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes |= 0x40000;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->opacity = 1.0f;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes & ~0xeu) | 1u;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->texture_id = 0;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->sort_priority = 0x7ffe;
    NuMtlUpdate(ScreenFadeMtl);
}

extern "C" {
    static f32 RenderBackdropTargetTopRPc = 0.0f;
    static f32 RenderBackdropTargetTopGPc = 0.0f;
    static f32 RenderBackdropTargetTopBPc = 0.0f;
    static f32 RenderBackdropTargetBottomRPc = 0.0f;
    static f32 RenderBackdropTargetBottomGPc = 0.0f;
    static f32 RenderBackdropTargetBottomBPc = 24.0f;

    void BackDrop_ClearColourTargets(void) {
        RenderBackdropTargetTopRPc = 0.0f;
        RenderBackdropTargetTopGPc = 0.0f;
        RenderBackdropTargetTopBPc = 0.0f;
        RenderBackdropTargetBottomRPc = 0.0f;
        RenderBackdropTargetBottomGPc = 0.0f;
        RenderBackdropTargetBottomBPc = 0.0f;
    }

    static f32 RenderBackdropWaitPc = 0.0f;

    void BackDrop_ResetColours(void) {
        RenderBackdropTopRPc = 0.0f;
        RenderBackdropTopGPc = 0.0f;
        RenderBackdropTopBPc = 0.0f;
        RenderBackdropBottomRPc = 0.0f;
        RenderBackdropBottomGPc = 0.0f;
        RenderBackdropBottomBPc = 0.0f;
        RenderBackdropWaitPc = 0.0001f;
        BackDrop_ClearColourTargets();
    }

    void BackDrop_UpdateColours(i32 instant) {
        if (RenderBackdropBlackPc) {
            RenderBackdropTopRPc = 0.0f;
            RenderBackdropTopGPc = 0.0f;
            RenderBackdropTopBPc = 0.0f;
            RenderBackdropBottomRPc = 0.0f;
            RenderBackdropBottomGPc = 0.0f;
            RenderBackdropBottomBPc = 0.0f;
            return;
        }
        if (RenderBackdropWaitPc > 0.0f) {
            RenderBackdropWaitPc -= FRAMETIME;
            if (!(RenderBackdropWaitPc <= 0.0f)) {
                return;
            }
            if (instant) {
                RenderBackdropTargetTopBPc = 0.0f;
                RenderBackdropTargetTopGPc = 0.0f;
                RenderBackdropTargetTopRPc = 0.0f;
                RenderBackdropTargetBottomBPc = 0.0f;
                RenderBackdropTargetBottomGPc = 0.0f;
                RenderBackdropTargetBottomRPc = 0.0f;
            } else {
                BackDrop_ClearColourTargets();
            }
            return;
        }
        if (RenderBackdropTopRPc == RenderBackdropTargetTopRPc && RenderBackdropTopGPc == RenderBackdropTargetTopGPc &&
            RenderBackdropTopBPc == RenderBackdropTargetTopBPc &&
            RenderBackdropBottomRPc == RenderBackdropTargetBottomRPc &&
            RenderBackdropBottomGPc == RenderBackdropTargetBottomGPc &&
            RenderBackdropBottomBPc == RenderBackdropTargetBottomBPc) {
            RenderBackdropWaitPc = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 2.5 + 2.5);
            return;
        }
        {
            f32 step = (f32)(FRAMETIME * 10.0);
            RenderBackdropTopRPc = SeekLinearF(RenderBackdropTopRPc, RenderBackdropTargetTopRPc, step);
            RenderBackdropTopGPc = SeekLinearF(RenderBackdropTopGPc, RenderBackdropTargetTopGPc, step);
            RenderBackdropTopBPc = SeekLinearF(RenderBackdropTopBPc, RenderBackdropTargetTopBPc, step);
            RenderBackdropBottomRPc = SeekLinearF(RenderBackdropBottomRPc, RenderBackdropTargetBottomRPc, step);
            RenderBackdropBottomGPc = SeekLinearF(RenderBackdropBottomGPc, RenderBackdropTargetBottomGPc, step);
            RenderBackdropBottomBPc = SeekLinearF(RenderBackdropBottomBPc, RenderBackdropTargetBottomBPc, step);
        }
    }

    void BackDrop_Init(char *path, VARIPTR *buffer, VARIPTR *end) {
        NUGSCN *scene = backdrop_scene;
        if (scene == NULL) {
            scene = NuGScnRead(buffer, *end, path);
            backdrop_scene = scene;
        }
        memset(RenderBackdropSpecialsPc, 0, sizeof(RenderBackdropSpecialsPc));
        if (scene == NULL) {
            return;
        }
        NuSpecialFind(backdrop_scene, &RenderBackdropSpecialsPc[0], "ball1", 1);
        NuSpecialFind(backdrop_scene, &RenderBackdropSpecialsPc[1], "ball2", 1);
        NuSpecialFind(backdrop_scene, &RenderBackdropSpecialsPc[2], "ball3", 1);
        NuSpecialFind(backdrop_scene, &RenderBackdropSpecialsPc[3], "ball4", 1);
    }

    void BackDrop_Update(f32 dt) {
        if (backdrop_scene != NULL) {
            NuGScnUpdate(backdrop_scene, (f32)(dt * 60.0));
        }
    }

    void BackDrop_Draw(f32 alpha, i32 flags) {
        i32 special_index;
        if (backdrop_scene == NULL) {
            return;
        }
        if (flags == 0 && RenderBackdropAlphaFnPc != NULL) {
            RenderBackdropAlphaFnPc(&alpha);
        }
        if (!(alpha > 0.0f)) {
            return;
        }
        for (special_index = 0; special_index < 4; ++special_index) {
            NUSPECIALHANDLE *special = &RenderBackdropSpecialsPc[special_index];
            if (NuSpecialExistsFn(special) != 0) {
                NUMTX matrix = *NuSpecialGetDrawMtx(special);
                NUVEC position;
                u16 angle;
                i32 jitter_index;
                NuSpecialDrawAtAlpha(special, &matrix, alpha);
                position = matrix.translation;
                angle = (u16)qrand();
                for (jitter_index = 0; jitter_index < 3; ++jitter_index) {
                    matrix.translation.x = (f32)(NuTrigTable[angle >> 1] * (f64)0.01f + position.x);
                    matrix.translation.y = (f32)(NuTrigTable[(angle + 0x4000) >> 1 & 0x7fff] * (f64)0.01f + position.y);
                    matrix.translation.z = position.z;
                    NuSpecialDrawAtAlpha(special, &matrix, alpha);
                    angle = (u16)(angle + 0x5555);
                }
            }
        }
    }

    void BackDrop_Dump(void) {
        if (backdrop_scene != NULL) {
            NuGScnRemove(backdrop_scene);
        }
    }

    void DrawParallax(NUSPECIALHANDLE *special) {
        NUVEC scale;
        NUMTX matrix;
        scale.x = scale.y = scale.z = (f32)((f64)pNuCam->far_clip * TerrainScanMargin10Pc * 0.5);
        NuMtxSetScale(&matrix, &scale);
        matrix.m30 = pNuCam->mtx.m30;
        matrix.m31 = pNuCam->mtx.m31;
        matrix.m32 = pNuCam->mtx.m32;
        NuSpecialDrawAt(special, &matrix);
    }

    static f32 GameFogDuration = 0;
    static void *GameFogSet = 0;
    static i32 GameFogSnap = 0;
    static f32 GameFogTime = 0;

    void GameFog_Reset() {
        GameFogSnap = 1;
        GameFogDuration = 0.0f;
        GameFogSet = 0;
        GameFogTime = 0.0f;
    }

    static f32 GameFogDefaultFStopPc = 22.0f;
    static GAMEFOG_STATE_PC GameFogNew = {{0}};
    static f32 dof_fstop = 0;

    void GameFog_Update(WORLDINFO *world) {
        RTL_FOG_MIGRATION_PC *previous = (RTL_FOG_MIGRATION_PC *)GameFogSet;
        RTL_FOG_MIGRATION_PC *fog;
        f32 blend;
        if (world->rtl_set != NULL &&
            !(CUTSTOPGAME != 0 && CutStopInfo != NULL && (((CUTINFO *)CutStopInfo)->flags & 4) != 0)) {
            fog = rtlGetFogSet((RTL_SET_PC *)world->rtl_set, &pNuCam->mtx.translation);
        } else {
            fog = NULL;
        }
        GameFogSet = fog;
        if (GameFogSnap == 0 && fog != previous) {
            GameFogTime = 0.0f;
            GameFogDuration = 3.0f;
            GameFogOld = GameFog;
        }
        if (GameFogSnap != 0 || fog != previous) {
            if (fog != NULL) {
                GameFogNew.colour_channels[0] = (f32)(fog->colour >> 24 & 0xff);
                GameFogNew.colour_channels[1] = (f32)(fog->colour >> 16 & 0xff);
                GameFogNew.colour_channels[2] = (f32)(fog->colour >> 8 & 0xff);
                GameFogNew.colour_channels[3] = (f32)(fog->colour & 0xff);
                GameFogNew.start = fog->start;
                GameFogNew.end = fog->end;
                GameFogNew.density = fog->density;
                GameFogNew.low_quality_density = (f32)fog->low_quality_density;
                GameFogNew.low_quality_colour_channels[0] = (f32)(fog->low_quality_colour >> 24 & 0xff);
                GameFogNew.low_quality_colour_channels[1] = (f32)(fog->low_quality_colour >> 16 & 0xff);
                GameFogNew.low_quality_colour_channels[2] = (f32)(fog->low_quality_colour >> 8 & 0xff);
                GameFogNew.low_quality_colour_channels[3] = (f32)(fog->low_quality_colour & 0xff);
            } else {
                GameFogNew.colour_channels[0] = GameFogNew.colour_channels[1] = GameFogNew.colour_channels[2] =
                    GameFogNew.colour_channels[3] = 0.0f;
                GameFogNew.start = 5.0f;
                GameFogNew.end = 20.0f;
                GameFogNew.density = 0.0f;
                GameFogNew.low_quality_density = 0.0f;
                GameFogNew.low_quality_colour_channels[0] = GameFogNew.low_quality_colour_channels[1] =
                    GameFogNew.low_quality_colour_channels[2] = GameFogNew.low_quality_colour_channels[3] = 0.0f;
            }
        }
        if (fog != NULL) {
            dof_fstop = (f32)((f64)fog->depth_of_field_fstop / 10.0);
            if (dof_fstop == 0.0f) {
                dof_fstop = GameFogDefaultFStopPc;
            }
        } else {
            dof_fstop = GameFogDefaultFStopPc;
        }
        if (GameFogSnap != 0) {
            GameFog = GameFogNew;
            GameFogDuration = 0.0f;
            GameFogTime = 0.0f;
        } else {
            if (GameFogTime < GameFogDuration) {
                GameFogTime += FRAMETIME;
                if (GameFogTime > GameFogDuration) {
                    GameFogTime = GameFogDuration;
                }
            }
            if (GameFogTime >= GameFogDuration) {
                GameFog = GameFogNew;
            } else {
                blend = (f32)(1.0 -
                              ((f64)NU_SIN_LUT((f64)(GameFogTime / GameFogDuration) * 32768.0 + 16384.0) + 1.0) * 0.5);
                GameFog.colour_channels[0] = GameFogOld.colour_channels[0] +
                                             (GameFogNew.colour_channels[0] - GameFogOld.colour_channels[0]) * blend;
                GameFog.colour_channels[1] = GameFogOld.colour_channels[1] +
                                             (GameFogNew.colour_channels[1] - GameFogOld.colour_channels[1]) * blend;
                GameFog.colour_channels[2] = GameFogOld.colour_channels[2] +
                                             (GameFogNew.colour_channels[2] - GameFogOld.colour_channels[2]) * blend;
                GameFog.colour_channels[3] = GameFogOld.colour_channels[3] +
                                             (GameFogNew.colour_channels[3] - GameFogOld.colour_channels[3]) * blend;
                GameFog.start = GameFogOld.start + (GameFogNew.start - GameFogOld.start) * blend;
                GameFog.end = GameFogOld.end + (GameFogNew.end - GameFogOld.end) * blend;
                GameFog.density = GameFogOld.density + (GameFogNew.density - GameFogOld.density) * blend;
                GameFog.low_quality_density = GameFogOld.low_quality_density +
                                              (GameFogNew.low_quality_density - GameFogOld.low_quality_density) * blend;
                GameFog.low_quality_colour_channels[0] =
                    GameFogOld.low_quality_colour_channels[0] +
                    (GameFogNew.low_quality_colour_channels[0] - GameFogOld.low_quality_colour_channels[0]) * blend;
                GameFog.low_quality_colour_channels[1] =
                    GameFogOld.low_quality_colour_channels[1] +
                    (GameFogNew.low_quality_colour_channels[1] - GameFogOld.low_quality_colour_channels[1]) * blend;
                GameFog.low_quality_colour_channels[2] =
                    GameFogOld.low_quality_colour_channels[2] +
                    (GameFogNew.low_quality_colour_channels[2] - GameFogOld.low_quality_colour_channels[2]) * blend;
                GameFog.low_quality_colour_channels[3] =
                    GameFogOld.low_quality_colour_channels[3] +
                    (GameFogNew.low_quality_colour_channels[3] - GameFogOld.low_quality_colour_channels[3]) * blend;
            }
        }
        GameFogSnap = 0;
        GameFog.colour = ((((u32)GameFog.colour_channels[0] << 8) + (u32)GameFog.colour_channels[1] << 8) +
                              (u32)GameFog.colour_channels[2]
                          << 8) +
                         (u32)GameFog.colour_channels[3];
        GameFog.low_quality_colour =
            ((((u32)GameFog.low_quality_colour_channels[0] << 8) + (u32)GameFog.low_quality_colour_channels[1] << 8) +
                 (u32)GameFog.low_quality_colour_channels[2]
             << 8) +
            (u32)GameFog.low_quality_colour_channels[3];
    }
}

extern "C" void GameDisplaySettings(LEVEL_DISPLAY_PC *display, i32 *colours) {
    CUT_DISPLAY_VIEW_PC *cut = (CUT_DISPLAY_VIEW_PC *)CutStopInfo;
    i32 top_r;
    i32 top_g;
    i32 top_b;
    i32 bottom_r;
    i32 bottom_g;
    i32 bottom_b;
    f32 alpha;

    if (cut != NULL && cut->camera_near_clip != 0.0f) {
        pNuCam->near_clip = cut->camera_near_clip;
    } else {
        pNuCam->near_clip = display->near_clip;
    }
    cut = (CUT_DISPLAY_VIEW_PC *)CutStopInfo;
    if (cut != NULL && cut->camera_far_clip != 0) {
        pNuCam->far_clip = (f32)cut->camera_far_clip;
    } else {
        pNuCam->far_clip = (f32)(u16)display->far_clip;
    }
    if (WORLD->current_level != TITLES_LDATA && (WORLD->current_level->flags & 0x400) == 0 &&
        WORLD->current_level != STATUS_LDATA && WORLD->current_level != CREDITS_LDATA && !(MainRenderTime < 1.0f)) {
        colours[0] = (((0xffff8000 | display->bg_blue_top) << 8 | display->bg_green_top) << 8) | display->bg_red_top;
        colours[1] =
            (((0xffff8000 | display->bg_blue_bottom) << 8 | display->bg_green_bottom) << 8) | display->bg_red_bottom;
        return;
    }
    BackDrop_UpdateColours(0);
    top_r = (i32)RenderBackdropTopRPc;
    top_g = (i32)RenderBackdropTopGPc;
    top_b = (i32)RenderBackdropTopBPc;
    bottom_r = (i32)RenderBackdropBottomRPc;
    bottom_g = (i32)RenderBackdropBottomGPc;
    bottom_b = (i32)RenderBackdropBottomBPc;
    alpha = 1.0f;
    if (RenderBackdropAlphaFnPc != NULL) {
        RenderBackdropAlphaFnPc(&alpha);
        if (alpha != 1.0f) {
            top_r = (i32)(top_r * alpha);
            top_g = (i32)(top_g * alpha);
            top_b = (i32)(top_b * alpha);
            bottom_r = (i32)(bottom_r * alpha);
            bottom_g = (i32)(bottom_g * alpha);
            bottom_b = (i32)(bottom_b * alpha);
        }
    }
    colours[0] = (((0xffff8000 | (top_b & 0xff)) << 8 | (top_g & 0xff)) << 8) | (top_r & 0xff);
    colours[1] = (((0xffff8000 | (bottom_b & 0xff)) << 8 | (bottom_g & 0xff)) << 8) | (bottom_r & 0xff);
}

extern "C" void WidescreenCode(i32 widescreen) {
    f32 aspect = 0.5625f;
    if (PcWindowOptions.aspect_mode == 2) {
        aspect = 0.625f;
    }
    if (widescreen != 0) {
        pNuCam->aspect = aspect;
        if (!CutSceneCameraCTRL) {
            pNuCam->fov = (f32)((aspect + 0.75) * 0.5);
        } else if (CutSceneCameraFocalLengthPc == 0.0f) {
            pNuCam->fov = aspect;
        } else {
            pNuCam->fov = (f32)(NuCameraFocalLenToFOV(CutSceneCameraFocalLengthPc) * (aspect / 0.75));
        }
        SmartTextSetWidescreen(0.75f, 1.0f);
    } else {
        pNuCam->aspect = 0.75f;
        if (!CutSceneCameraCTRL) {
            pNuCam->fov = 0.75f;
        } else if (CutSceneCameraFocalLengthPc == 0.0f) {
            pNuCam->fov = 0.75f;
        } else {
            pNuCam->fov = NuCameraFocalLenToFOV(CutSceneCameraFocalLengthPc);
        }
        SmartTextSetWidescreen(1.0f, 1.0f);
    }
}

extern "C" {

    void KeepPointOnScreen(NUVEC *position, NUVEC *velocity) {
        if ((f64)position->x < (f64)-0.85f) {
            position->x = -0.85f;
            if (velocity != NULL) {
                velocity->x = 0.0f;
            }
        } else if ((f64)position->x > (f64)0.85f) {
            position->x = 0.85f;
            if (velocity != NULL) {
                velocity->x = 0.0f;
            }
        }
        if ((f64)position->y < (f64)-0.85f) {
            position->y = -0.85f;
            if (velocity != NULL) {
                velocity->y = 0.0f;
            }
        } else if ((f64)position->y > (f64)0.85f) {
            position->y = 0.85f;
            if (velocity != NULL) {
                velocity->y = 0.0f;
            }
        }
    }
}

extern "C" void UpdateCutBorders(void) {
    f32 target_scale;
    if (CUTSTOPGAME != 0 || (LEGOCAMMODE_DOORCUT != -1 && GameCam->mode == LEGOCAMMODE_DOORCUT) ||
        (LEGOCAMMODE_OBSTACLE != -1 && GameCam->mode == LEGOCAMMODE_OBSTACLE && ObstacleCamBorders != 0)) {
        target_scale = 1.0f;
    } else {
        target_scale = 0.0f;
    }
    CutBorderScale = SeekLinearF(CutBorderScale, target_scale, FRAMETIME + FRAMETIME);
}

extern "C" void DrawCutBorders(i32 widescreen) {
    f32 border_scale;
    i32 height;
    if (!(CutBorderScale > 0.0f)) {
        return;
    }
    NuRndrBeginScene(1);
    if ((CutStopInfo != NULL && (((CUT_BORDER_RECORD_PC *)CutStopInfo)->flags & 0x4000) != 0) ||
        (WORLD->cutscene_sys != NULL && WORLD->cutscene_sys->count == 1 &&
         (((CUT_BORDER_RECORD_PC *)WORLD->cutscene_sys->records[0])->flags & 0x4000) != 0)) {
        border_scale = 0.21276596188545227f;
    } else {
        border_scale = 0.1f;
    }
    if (widescreen != 0) {
        border_scale = (f32)((border_scale * 4.0 - 0.5) / 3.0);
    }
    if (border_scale < 0.02f) {
        border_scale = 0.02f;
    }
    height = (i32)(NuTrigTable[((i32)(CutBorderScale * 16384.0) >> 1) & 0x7fff] * (border_scale * 3616.0));
    NuRndrRect2di(0, 0, 0x2800, height, 0x80ffffff, FadeMtl);
    NuRndrRect2di(0, 0xe20 - height, 0x2800, height, 0x80ffffff, FadeMtl);
    NuRndrEndScene();
}

extern "C" void InitStillRender(VARIPTR *buffer, VARIPTR end) {
    pause_rt = NuTexCreateFramebufferCopy(buffer, end);
    STILL_RENDER_MATERIAL_PC *material = (STILL_RENDER_MATERIAL_PC *)NuMtlCreate3D(1);
    material->texture_id = (i16)pause_rt;
    material->opacity = 1.0f;
    u32 attributes = (material->attributes & 0xfffee5d1u) | 0x6e511u;
    u32 vertex_format = (material->shader.vertex_format & 0xffffcfff) | 0x40800;
    pause_rndr_mtl = material;
    material->sort_priority = 0x7fff;
    material->attributes = attributes;
    material->shader.flags = 0x1000;
    material->shader.packed_diffuse_colour = 0xffffffff;
    material->shader.texture_unit_count = 1;
    material->shader.vertex_format = vertex_format;
    NuMtlUpdate((NUMTL *)material);
    pause_rndr_on = 0;
}

/* Resizes the pause-screen still to a new backbuffer size after a D3D reset. */
extern "C" void ResizeStillRenderPc(i32 width, i32 height) {
    GRAPHICS_RENDER_TEXTURE_PC *texture = PCGraphicsFrameRenderTexturePc(pause_rt);
    PCGraphicsResizeEffectTexture(texture, width, height, -1);
}

extern "C" void DrawPauseScreenWipe(void) {
    i32 x;
    i32 y;
    i32 width;
    i32 height;
    f32 u0;
    f32 v0;
    f32 u1;
    f32 v1;

    NuRndrBeginScene(1);
    x = -0x20;
    u0 = 0.0f;
    v0 = 0.0f;
    u1 = 1.0f;
    v1 = 1.0f;
    y = -0xc;
    width = 0x2814;
    height = 0xe08;
    {
        u32 colours_left[4];
        u32 colours_right[4];
        u32 colours_top[4];
        u32 colours_bottom[4];
        if ((FadeSys.direction & 3) != 0) {
            if ((FadeSys.direction & 1) != 0) {
                colours_left[0] = 0x808080;
                colours_left[1] = 0x80808080;
                colours_left[2] = 0x808080;
                colours_left[3] = 0x80808080;
                x = (i32)((1.0f - FadeSys.fade) * 10260.0f);
                u0 = 1.0f - FadeSys.fade;
                width -= x;
                NuRndrGradRectUV2di(x - 0x402, y, 0x402, height, u0 - 0.1f, 0.0f, u0, 1.0f, colours_left,
                                    (NUMTL *)pause_rndr_mtl);
            } else {
                colours_right[0] = 0x80808080;
                colours_right[1] = 0x808080;
                colours_right[2] = 0x80808080;
                colours_right[3] = 0x808080;
                width = (i32)(FadeSys.fade * 10260.0f);
                u1 = FadeSys.fade;
                NuRndrGradRectUV2di(width - 0x20, -0xc, 0x402, 0xe08, u1, 0.0f, u1 + 0.1f, 1.0f, colours_right,
                                    (NUMTL *)pause_rndr_mtl);
            }
        } else if ((FadeSys.direction & 0xc) != 0) {
            if ((FadeSys.direction & 4) != 0) {
                colours_top[0] = 0x808080;
                colours_top[1] = 0x808080;
                colours_top[2] = 0x80808080;
                colours_top[3] = 0x80808080;
                y = (i32)((1.0f - FadeSys.fade) * 3592.0f);
                v0 = 1.0f - FadeSys.fade;
                height -= y;
                NuRndrGradRectUV2di(-0x20, y - 0x167, 0x2814, 0x167, 0.0f, v0 - 0.1f, 1.0f, v0, colours_top,
                                    (NUMTL *)pause_rndr_mtl);
            } else {
                colours_bottom[0] = 0x80808080;
                colours_bottom[1] = 0x80808080;
                colours_bottom[2] = 0x808080;
                colours_bottom[3] = 0x808080;
                height = (i32)(FadeSys.fade * 3592.0f);
                v1 = FadeSys.fade;
                NuRndrGradRectUV2di(-0x20, height - 0xc, 0x2814, 0x167, 0.0f, v1, 1.0f, v1 + 0.1f, colours_bottom,
                                    (NUMTL *)pause_rndr_mtl);
            }
        }
    }
    NuRndrRectUV2di(x, y, width, height, u0, v0, u1, v1, (i32)0x80808080, (NUMTL *)pause_rndr_mtl);
    NuRndrEndScene();
}

extern "C" void fun_00565b90() {
}

extern "C" void GrabStillScreen(void) {
    if (ScreenGrabNeeded != 0) {
        ScreenGrabNeeded = 0;
        NuRndrBeginScene(1);
        NuBackbufferCopy(pause_rt);
        NuRndrEndScene();
        pause_fade = 0.0f;
    }
}

extern "C" void NeedScreenGrab(i32 needed) {
    ScreenGrabNeeded = needed != 0;
}

extern "C" u8 IsGrabbingScreen(void) {
    return ScreenGrabNeeded;
}

extern "C" void DrawStillScreen(i32 clear) {
    NuRndrBeginScene(1);
    NuVpGetCurrentViewport();
    if (clear != 0) {
        NuRndrClear(0x500, 0, 1.0f);
    }
    if (MainRenderTime < 1.0f) {
        const i32 colour = (((i32)(MainRenderTime * 128.0f)) << 24) | 0x00808080;
        u32 colours[4] = {colour, colour, colour, colour};
        NuRndrGradRectUV2di(ScreenStillXPc, ScreenStillYPc, ScreenStillWidthPc, ScreenStillHeightPc, 0.0f, 0.0f, 1.0f,
                            1.0f, colours, (NUMTL *)pause_rndr_mtl);
    } else {
        NuRndrRectUV2di(ScreenStillXPc, ScreenStillYPc, ScreenStillWidthPc, ScreenStillHeightPc, 0.0f, 0.0f, 1.0f, 1.0f,
                        0x80808080u, (NUMTL *)pause_rndr_mtl);
    }
    NuRndrEndScene();
}

extern "C" void DrawPauseFade(void) {
    if (editor_active != 0 || screendump != 0) {
        return;
    }
    if (Paused == 0 && NetPaused == 0) {
        pause_fade = MAX(pause_fade - FRAMETIME * ScreenPauseFadeSpeedPc, 0.0f);
    } else {
        pause_fade = MIN(FRAMETIME * ScreenPauseFadeSpeedPc + pause_fade, 1.0f);
    }
}

extern "C" void fun_00565d80() {
    Paused = 0;
    old_pause_state = 0;
}

extern "C" void HandleStillRender(void) {
    GetMenuID();
    if (pause_rndr_on != 0 && FadeSys.pending_type == -1) {
        NuRndrBeginScene(1);
        if (MainRenderTime < 1.0f) {
            if (back_rgba[0] == back_rgba[1]) {
                NuRndrClear(0xf00, back_rgba[0], 1.0f);
            } else {
                NuRndrGradClear(0xf00, back_rgba[0], back_rgba[1], 1.0f);
            }
            BackDrop_Draw(1.0f - MainRenderTime, 0);
        } else {
            NuRndrClear(0x300, 0, 1.0f);
        }
        NuRndrEndScene();
    }
    if (LevelCompletionFallbackFlagPC != 2 && pause_rndr_on != 0 && pause_rt != 0 && FadeSys.pending_type == -1) {
        DrawStillScreen(clear_screen_onstill);
    }
    if (Paused != 0) {
        if (old_pause_state == 0 && FadeSys.pending_type == -1) {
            ScreenGrabNeeded = 1;
        }
    } else if (old_pause_state != 0) {
        pause_rndr_on = 0;
    }
    old_pause_state = Paused;
    if (waiting_for_level != -1 &&
        (gone_through_door_to_new_level != 0 || cut_waiting_for_new_level != 0 || waiting_for_new_level != 0)) {
        pause_rndr_on = 1;
    } else if (NewLData == CREDITS_LDATA && LastLData == STATUS_LDATA) {
        pause_rndr_on = 1;
    } else if (NewLData == STATUS_LDATA && WORLD->area_index != -1 && (WORLD->area[WORLD->area_index].flags & 2) != 0) {
        pause_rndr_on = 1;
    } else if ((NewLData == STATUS_LDATA && GAMEDEMO != 0) || NewLData == CREDITS_LDATA) {
        pause_rndr_on = 1;
    } else if (NewLData == HUB_LDATA && WORLD->area_index != -1 && (WORLD->area[WORLD->area_index].flags & 2) != 0) {
        pause_rndr_on = 1;
    } else if (MainRenderTime > 0.0f && MainRenderTime < 1.0f) {
        pause_rndr_on = 1;
    } else if (Paused != 0 && ScreenGrabNeeded == 0) {
        if (PauseRenderOffFn != NULL && PauseRenderOffFn() != 0) {
            pause_rndr_on = 0;
        } else {
            pause_rndr_on = 1;
        }
    } else if (LevelCompletionFallbackFlagPC == 0 || LevelCompletionFallbackFlagPC == 1) {
        pause_rndr_on = 0;
    } else {
        pause_rndr_on = 1;
    }
    if (screendump != 0) {
        pause_rndr_on = 0;
    }
    LevelCompletionFallbackFlagPC = 0;
    clear_screen_onstill = 1;
}

extern "C" {
    NUMTL *SolidMtl = 0;
}

extern "C" void DrawFadeScreenWipe(void) {
    i32 x = 0;
    i32 y = 0;
    i32 width;
    i32 height;
    i32 gradient_position;
    i32 skip_solid;
    f32 edge;
    u32 colours[4];

    NuRndrBeginScene(1);
    skip_solid = FadeSys.fade < 0.0f;
    width = 0x2800;
    height = 0xe00;
    if ((FadeSys.direction & 3) != 0) {
        if ((FadeSys.direction & 1) != 0) {
            if (FadeSys.rate > 0.0f) {
                edge = FadeSys.fade;
                colours[0] = 0x80000000u;
                colours[1] = 0;
                colours[2] = 0x80000000u;
                colours[3] = 0;
                x = 0;
                width = (i32)(edge * 10240.0f);
                gradient_position = width;
            } else {
                edge = 1.0f - FadeSys.fade;
                colours[0] = 0;
                colours[1] = 0x80000000u;
                colours[2] = 0;
                colours[3] = 0x80000000u;
                x = (i32)(edge * 10240.0f);
                width -= x;
                gradient_position = (i32)(edge * 10240.0f - 1024.0f);
            }
        } else {
            if (FadeSys.rate > 0.0f) {
                edge = 1.0f - FadeSys.fade;
                colours[0] = 0;
                colours[1] = 0x80000000u;
                colours[2] = 0;
                colours[3] = 0x80000000u;
                x = (i32)(edge * 10240.0f);
                width -= x;
                gradient_position = (i32)(edge * 10240.0f - 1024.0f);
            } else {
                edge = FadeSys.fade;
                colours[0] = 0x80000000u;
                colours[1] = 0;
                colours[2] = 0x80000000u;
                colours[3] = 0;
                x = 0;
                width = (i32)(edge * 10240.0f);
                gradient_position = width;
            }
        }
        NuRndrGradRect2di(gradient_position - 0x10, -0x10, 0x410, 0xe10, colours, FadeMtl2);
    } else if ((FadeSys.direction & 0xc) != 0) {
        if ((FadeSys.direction & 4) != 0) {
            if (FadeSys.rate > 0.0f) {
                edge = FadeSys.fade;
                colours[0] = 0x80000000u;
                colours[1] = 0x80000000u;
                colours[2] = 0;
                colours[3] = 0;
                y = 0;
                height = (i32)(edge * 3584.0f);
                gradient_position = height;
            } else {
                edge = 1.0f - FadeSys.fade;
                colours[0] = 0;
                colours[1] = 0;
                colours[2] = 0x80000000u;
                colours[3] = 0x80000000u;
                y = (i32)(edge * 3584.0f);
                height -= y;
                gradient_position = (i32)(edge * 3584.0f - 358.0f);
            }
        } else {
            if (FadeSys.rate > 0.0f) {
                edge = 1.0f - FadeSys.fade;
                colours[0] = 0;
                colours[1] = 0;
                colours[2] = 0x80000000u;
                colours[3] = 0x80000000u;
                y = (i32)(edge * 3584.0f);
                height -= y;
                gradient_position = (i32)(edge * 3584.0f - 358.0f);
            } else {
                edge = FadeSys.fade;
                colours[0] = 0x80000000u;
                colours[1] = 0x80000000u;
                colours[2] = 0;
                colours[3] = 0;
                y = 0;
                height = (i32)(edge * 3584.0f);
                gradient_position = height;
            }
        }
        NuRndrGradRect2di(-0x10, gradient_position - 0x10, 0x2810, 0x176, colours, FadeMtl2);
    }
    if (skip_solid == 0) {
        NuRndrRect2di(x - 0x10, y - 0x10, width + 0x10, height + 0x10, 0, SolidMtl);
    }
    NuRndrEndScene();
}

extern "C" void NoRender(void) {
    pNuCam->mtx = NuMtxSetIdentityTemplate;
    NuCameraSet(pNuCam);
    NuRndrBeginScene(1);
    if (back_rgba[0] == back_rgba[1]) {
        NuRndrClear(0xf00, back_rgba[0], 1.0f);
    } else {
        NuRndrGradClear(0xf00, back_rgba[0], back_rgba[1], 1.0f);
    }
    if (MainRenderTime != 1.0f) {
        BackDrop_Draw(1.0f - MainRenderTime, 0);
    }
    NuRndrEndScene();
}

extern "C" {

    void FinishLoop(i32 frames) {
        if (FinishLoop_On == 0) {
            return;
        }

        if (finishloop_backdroponly != 0) {
            pNuCam->mtx = NuMtxSetIdentityTemplate;
            NuCameraSet(pNuCam);
            do {
                NuFrameBegin();
                NuRndrBeginScene(1);
                NuRndrClear(0xb00, 0, 1.0f);
                BackDrop_Draw(1.0f, 1);
                NuRndrEndScene();
                edGraEnableTerrainSwap();
                NuFrameEnd();
                edGraDisableTerrainSwap();
            } while (--frames > 0);
            finishloop_backdroponly = 0;
            return;
        }

        if (FadeSys.pending_type != -1) {
            do {
                NuFrameBegin();
                FadeSys.Draw();
                edGraEnableTerrainSwap();
                NuFrameEnd();
                edGraDisableTerrainSwap();
            } while (--frames > 0);
            return;
        }

        do {
            NuFrameBegin();
            NuRndrBeginScene(1);
            NuRndrClear(0xb00, 0, 1.0f);
            NuRndrEndScene();
            edGraEnableTerrainSwap();
            NuFrameEnd();
            edGraDisableTerrainSwap();
        } while (--frames > 0);
    }

    void FadeLoop(char *text, i32 direction, float duration, void (*draw)(float)) {
        f32 rate;
        f32 alpha;
        f32 target;
        if (direction == 0) {
            alpha = 0.0f;
            target = 1.0f;
        } else {
            alpha = 1.0f;
            target = 0.0f;
        }
        if (duration == 0.0f) {
            duration = 0.1f;
        }
        rate = 1.0f / duration;
        FRAMETIME = DEFAULTFRAMETIME;
        while (alpha != target) {
            NuFrameBegin();
            alpha = SeekLinearF(alpha, target, rate * FRAMETIME);
            NuRndrBeginScene(1);
            NuRndrClear(0xb00, 0, 1.0f);
            if (FadeLoop_ObjScene != NULL) {
                FadeLoop_DrawObj(alpha);
            }
            if (text != NULL) {
                SetQFont2D();
                Text3D(text, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0, (u8)(alpha * 0.0f), (u8)(alpha * 191.0f),
                       (u8)(alpha * 255.0f));
            }
            if (draw != NULL) {
                draw(alpha);
            }
            NuRndrEndScene();
            edGraEnableTerrainSwap();
            FRAMETIME = NuFrameEnd();
            edGraDisableTerrainSwap();
        }
        if (direction == 1) {
            FadeLoop_SetObj(NULL, NULL);
            FinishLoop(2);
        }
    }
}

void ChallangeCash_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    (void)packet;
}
