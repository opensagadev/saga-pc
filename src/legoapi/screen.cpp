#include "saga.h"

typedef struct STILL_RENDER_MATERIAL_PC {
    u8 unknown_00[0x40];
    u32 attributes;
    u8 unknown_44[0x2c];
    f32 opacity;
    i16 texture_id, sort_priority;
    u8 unknown_78[0x3c];
    PC_MATERIAL_SHADER_DESC_VIEW shader;
} STILL_RENDER_MATERIAL_PC;
STATIC_ASSERT(offsetof(STILL_RENDER_MATERIAL_PC, attributes) == 0x40);
STATIC_ASSERT(offsetof(STILL_RENDER_MATERIAL_PC, opacity) == 0x70);
STATIC_ASSERT(offsetof(STILL_RENDER_MATERIAL_PC, texture_id) == 0x74);
STATIC_ASSERT(offsetof(STILL_RENDER_MATERIAL_PC, sort_priority) == 0x76);
STATIC_ASSERT(offsetof(STILL_RENDER_MATERIAL_PC, shader) == 0xb4);

extern "C" void __cdecl InitStillRender(VARIPTR *buffer, VARIPTR end) {
    pause_rt = NuTexCreateFramebufferCopy(buffer, end);
    STILL_RENDER_MATERIAL_PC *material = (STILL_RENDER_MATERIAL_PC *)NuMtlCreate3D(1);
    material->opacity = 1.0f;
    material->texture_id = (i16)pause_rt;
    material->attributes = (material->attributes & 0xfffee5d1u) | 0x6e511u;
    material->shader.vertex_format = (material->shader.vertex_format & 0xffffcfff) | 0x40800;
    pause_rndr_mtl = material;
    material->sort_priority = 0x7fff;
    material->shader.flags = 0x1000;
    material->shader.packed_diffuse_colour = 0xffffffff;
    material->shader.texture_unit_count = 1;
    NuMtlUpdate((NUMTL *)material);
    pause_rndr_on = 0;
}

typedef struct CUT_DISPLAY_VIEW_PC {
    u8 unknown_00[0x6c];
    f32 camera_near_clip;
    u8 unknown_70[0x78];
    u16 camera_far_clip;
} CUT_DISPLAY_VIEW_PC;
STATIC_ASSERT(offsetof(CUT_DISPLAY_VIEW_PC, camera_near_clip) == 0x6c);
STATIC_ASSERT(offsetof(CUT_DISPLAY_VIEW_PC, camera_far_clip) == 0xe8);

extern "C" void __cdecl GameDisplaySettings(LEVEL_DISPLAY_PC *display, i32 *colours) {
    CUT_DISPLAY_VIEW_PC *cut = (CUT_DISPLAY_VIEW_PC *)CutStopInfo;
    LEVELDATA_PC *level;
    i32 top_r, top_g, top_b, bottom_r, bottom_g, bottom_b;
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
    level = WORLD->current_level;
    if (level != TITLES_LDATA && (level->flags & 0x400) == 0 && level != STATUS_LDATA && level != CREDITS_LDATA &&
        !(MainRenderTime < 1.0f)) {
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

extern "C" f32 CutSceneCameraFocalLengthPc;
extern "C" f32 __cdecl NuCameraFocalLenToFOV(f32 focal_length);
extern "C" void __cdecl WidescreenCode(i32 widescreen) {
    f32 aspect = PcWindowOptions.aspect_mode == 2 ? 0.625f : 0.5625f;
    if (widescreen != 0) {
        pNuCam->aspect = aspect;
        if (!CutSceneCameraCTRL)
            pNuCam->fov = (f32)((aspect + 0.75) * 0.5);
        else if (CutSceneCameraFocalLengthPc == 0.0f)
            pNuCam->fov = aspect;
        else
            pNuCam->fov = (f32)(NuCameraFocalLenToFOV(CutSceneCameraFocalLengthPc) * (aspect / 0.75));
        SmartTextSetWidescreen(0.75f, 1.0f);
    } else {
        pNuCam->aspect = 0.75f;
        if (!CutSceneCameraCTRL || CutSceneCameraFocalLengthPc == 0.0f)
            pNuCam->fov = 0.75f;
        else
            pNuCam->fov = NuCameraFocalLenToFOV(CutSceneCameraFocalLengthPc);
        SmartTextSetWidescreen(1.0f, 1.0f);
    }
}
typedef struct CUT_BORDER_RECORD_PC {
    u8 unknown_00[0x50];
    u32 flags;
} CUT_BORDER_RECORD_PC;
STATIC_ASSERT(offsetof(CUT_BORDER_RECORD_PC, flags) == 0x50);
extern "C" NUMTL *FadeMtl;
extern "C" void __cdecl UpdateCutBorders(void) {
    f32 target_scale = 1.0f;
    if (CUTSTOPGAME == 0 && (LEGOCAMMODE_DOORCUT == -1 || GameCam->mode != LEGOCAMMODE_DOORCUT)) {
        if (LEGOCAMMODE_OBSTACLE == -1 || GameCam->mode != LEGOCAMMODE_OBSTACLE || ObstacleCamBorders == 0)
            target_scale = 0.0f;
    }
    CutBorderScale = SeekLinearF(CutBorderScale, target_scale, FRAMETIME + FRAMETIME);
}

extern "C" void __cdecl DrawCutBorders(i32 widescreen) {
    f32 border_scale;
    i32 height;
    if (!(CutBorderScale > 0.0f))
        return;
    NuRndrBeginScene(1);
    if ((CutStopInfo != NULL && (((CUT_BORDER_RECORD_PC *)CutStopInfo)->flags & 0x4000) != 0) ||
        (WORLD->cutscene_sys != NULL && WORLD->cutscene_sys->count == 1 &&
         (((CUT_BORDER_RECORD_PC *)WORLD->cutscene_sys->records[0])->flags & 0x4000) != 0))
        border_scale = 0.21276596188545227f;
    else
        border_scale = 0.1f;
    if (widescreen != 0)
        border_scale = (f32)((border_scale * 4.0 - 0.5) / 3.0);
    if (border_scale < 0.02f)
        border_scale = 0.02f;
    height = (i32)(NuTrigTable[((i32)(CutBorderScale * 16384.0) >> 1) & 0x7fff] * (border_scale * 3584.0));
    NuRndrRect2di(0, 0, 0x2800, height, 0x80ffffff, FadeMtl);
    NuRndrRect2di(0, 0xe20 - height, 0x2800, height, 0x80ffffff, FadeMtl);
    NuRndrEndScene();
}

typedef struct USEFUL_MATERIAL_PC {
    u8 unknown_00[0x40];
    u32 attributes;
} USEFUL_MATERIAL_PC;
STATIC_ASSERT(offsetof(USEFUL_MATERIAL_PC, attributes) == 0x40);

extern "C" {
    extern NUMTL *FadeMtl2;
    NUMTL *SolidMtl = 0;
    void __cdecl NuRndrGradRect2di(i32, i32, i32, i32, u32 *, NUMTL *);
}

extern "C" void __cdecl CreateUsefulMaterials(void) {
    SolidMtl = NuMtlCreate(1);
    ((USEFUL_MATERIAL_PC *)SolidMtl)->attributes &= 0xffff3fffu;
    ((USEFUL_MATERIAL_PC *)SolidMtl)->attributes &= 0xfffffff0u;
    ((USEFUL_MATERIAL_PC *)SolidMtl)->attributes |= 0x40000u;
    NuMtlUpdate(SolidMtl);
    ZipUpLineMaterialPc = NuMtlCreate3D(1);
    ((USEFUL_MATERIAL_PC *)ZipUpLineMaterialPc)->attributes &= 0xffff3fffu;
    ((USEFUL_MATERIAL_PC *)ZipUpLineMaterialPc)->attributes &= 0xfffffff0u;
    ((USEFUL_MATERIAL_PC *)ZipUpLineMaterialPc)->attributes |= 0x40000u;
    NuMtlUpdate((NUMTL *)ZipUpLineMaterialPc);
}

extern "C" void __cdecl DrawFadeScreenWipe(void) {
    i32 x = 0;
    i32 y = 0;
    i32 width = 0x2800;
    i32 height = 0xe00;
    i32 gradient_position;
    i32 skip_solid;
    f32 edge;
    u32 colours[4];

    NuRndrBeginScene(1);
    skip_solid = FadeSys.fade < 0.0f;
    if ((FadeSys.direction & 3) != 0) {
        if (((FadeSys.direction & 1) != 0 && FadeSys.rate > 0.0f) ||
            ((FadeSys.direction & 1) == 0 && !(FadeSys.rate > 0.0f))) {
            edge = FadeSys.fade;
            colours[0] = 0x80000000u;
            colours[1] = 0;
            colours[2] = 0x80000000u;
            colours[3] = 0;
            width = (i32)(edge * 10240.0);
            gradient_position = width;
        } else {
            edge = 1.0f - FadeSys.fade;
            colours[0] = 0;
            colours[1] = 0x80000000u;
            colours[2] = 0;
            colours[3] = 0x80000000u;
            x = (i32)(edge * 10240.0);
            width -= x;
            gradient_position = (i32)(edge * 10240.0 - 1024.0);
        }
        NuRndrGradRect2di(gradient_position - 0x10, -0x10, 0x410, 0xe10, colours, FadeMtl2);
    } else if ((FadeSys.direction & 0xc) != 0) {
        if (((FadeSys.direction & 4) != 0 && FadeSys.rate > 0.0f) ||
            ((FadeSys.direction & 4) == 0 && !(FadeSys.rate > 0.0f))) {
            edge = FadeSys.fade;
            colours[0] = 0x80000000u;
            colours[1] = 0x80000000u;
            colours[2] = 0;
            colours[3] = 0;
            height = (i32)(edge * 3584.0);
            gradient_position = height;
        } else {
            edge = 1.0f - FadeSys.fade;
            colours[0] = 0;
            colours[1] = 0;
            colours[2] = 0x80000000u;
            colours[3] = 0x80000000u;
            y = (i32)(edge * 3584.0);
            height -= y;
            gradient_position = (i32)(edge * 3584.0 - 358.0);
        }
        NuRndrGradRect2di(-0x10, gradient_position - 0x10, 0x2810, 0x176, colours, FadeMtl2);
    }
    if (skip_solid == 0)
        NuRndrRect2di(x - 0x10, y - 0x10, width + 0x10, height + 0x10, 0, SolidMtl);
    NuRndrEndScene();
}

extern "C" void __cdecl DrawPauseScreenWipe(void) {
    i32 x = -0x20;
    i32 y = -0xc;
    i32 width = 0x2814;
    i32 height = 0xe08;
    f32 u0 = 0.0f;
    f32 v0 = 0.0f;
    f32 u1 = 1.0f;
    f32 v1 = 1.0f;
    f32 edge;
    u32 colours[4];

    NuRndrBeginScene(1);
    if ((FadeSys.direction & 3) != 0) {
        if ((FadeSys.direction & 1) != 0) {
            edge = 1.0f - FadeSys.fade;
            colours[0] = 0x808080;
            colours[1] = 0x80808080;
            colours[2] = 0x808080;
            colours[3] = 0x80808080;
            x = (i32)(edge * 10260.0);
            u0 = edge;
            width -= x;
            NuRndrGradRectUV2di(x - 0x402, y, 0x402, height, edge - (f64)0.1f, 0.0f, edge, 1.0f, colours,
                                (NUMTL *)pause_rndr_mtl);
        } else {
            edge = FadeSys.fade;
            colours[0] = 0x80808080;
            colours[1] = 0x808080;
            colours[2] = 0x80808080;
            colours[3] = 0x808080;
            width = (i32)(edge * 10260.0);
            u1 = edge;
            NuRndrGradRectUV2di(width - 0x20, y, 0x402, 0xe08, edge, 0.0f, edge + (f64)0.1f, 1.0f, colours,
                                (NUMTL *)pause_rndr_mtl);
        }
    } else if ((FadeSys.direction & 0xc) != 0) {
        if ((FadeSys.direction & 4) != 0) {
            edge = 1.0f - FadeSys.fade;
            colours[0] = 0x808080;
            colours[1] = 0x808080;
            colours[2] = 0x80808080;
            colours[3] = 0x80808080;
            y = (i32)(edge * 3592.0);
            v0 = edge;
            height -= y;
            NuRndrGradRectUV2di(x, y - 0x167, 0x2814, 0x167, 0.0f, edge - (f64)0.1f, 1.0f, edge, colours,
                                (NUMTL *)pause_rndr_mtl);
        } else {
            edge = FadeSys.fade;
            colours[0] = 0x80808080;
            colours[1] = 0x80808080;
            colours[2] = 0x808080;
            colours[3] = 0x808080;
            height = (i32)(edge * 3592.0);
            v1 = edge;
            NuRndrGradRectUV2di(-0x20, height - 0xc, 0x2814, 0x167, 0.0f, edge, 1.0f, edge + (f64)0.1f, colours,
                                (NUMTL *)pause_rndr_mtl);
        }
    }
    NuRndrRectUV2di(x, y, width, height, u0, v0, u1, v1, (i32)0x80808080, (NUMTL *)pause_rndr_mtl);
    NuRndrEndScene();
}

extern "C" void __cdecl NuCameraSet(NUCAMERA *);
extern "C" void __cdecl BackDrop_Draw(f32, i32);
extern "C" void __cdecl NoRender(void) {
    pNuCam->mtx = NuMtxSetIdentityTemplate;
    NuCameraSet(pNuCam);
    NuRndrBeginScene(1);
    if (back_rgba[0] == back_rgba[1])
        NuRndrClear(0xf00, back_rgba[0], 1.0f);
    else
        NuRndrGradClear(0xf00, back_rgba[0], back_rgba[1], 1.0f);
    if (MainRenderTime != 1.0f)
        BackDrop_Draw((f32)(1.0f - MainRenderTime), 0);
    NuRndrEndScene();
}

extern "C" u8 ScreenGrabNeededPc;
extern "C" f32 ScreenPauseFadePc;
extern "C" void __cdecl NuBackbufferCopy(i32 texture_id);
extern "C" void __cdecl GrabStillScreen(void) {
    if (ScreenGrabNeededPc != 0) {
        ScreenGrabNeededPc = 0;
        NuRndrBeginScene(1);
        NuBackbufferCopy(pause_rt);
        NuRndrEndScene();
        ScreenPauseFadePc = 0.0f;
    }
}

extern "C" void NeedScreenGrab(i32 needed) {
    ScreenGrabNeededPc = needed != 0;
}

extern "C" u8 IsGrabbingScreen(void) {
    return ScreenGrabNeededPc;
}

extern "C" NUVPCURRENT *__cdecl NuVpGetCurrentViewport(void);
extern "C" i32 ScreenStillXPc, ScreenStillYPc, ScreenStillWidthPc, ScreenStillHeightPc;
extern "C" void DrawStillScreen(i32 clear) {
    NuRndrBeginScene(1);
    NuVpGetCurrentViewport();
    if (clear != 0) {
        NuRndrClear(0x500, 0, 1.0f);
    }
    if (MainRenderTime < 1.0f) {
        const u32 colour = (((i32)(MainRenderTime * 128.0f)) << 24) | 0x00808080u;
        u32 colours[4] = {colour, colour, colour, colour};
        NuRndrGradRectUV2di(ScreenStillXPc, ScreenStillYPc, ScreenStillWidthPc, ScreenStillHeightPc, 0.0f, 0.0f, 1.0f,
                            1.0f, colours, (NUMTL *)pause_rndr_mtl);
    } else {
        NuRndrRectUV2di(ScreenStillXPc, ScreenStillYPc, ScreenStillWidthPc, ScreenStillHeightPc, 0.0f, 0.0f, 1.0f, 1.0f,
                        0x80808080u, (NUMTL *)pause_rndr_mtl);
    }
    NuRndrEndScene();
}

extern "C" f32 ScreenPauseFadeSpeedPc;
extern "C" void __cdecl DrawPauseFade(void) {
    if (editor_active != 0 || screendump != 0)
        return;
    if (Paused == 0 && NetPaused == 0)
        ScreenPauseFadePc = MAX(ScreenPauseFadePc - FRAMETIME * ScreenPauseFadeSpeedPc, 0.0f);
    else
        ScreenPauseFadePc = MIN(FRAMETIME * ScreenPauseFadeSpeedPc + ScreenPauseFadePc, 1.0f);
}

extern "C" void __cdecl HandleStillRender(void) {
    GetMenuID();
    if (pause_rndr_on != 0 && FadeSys.pending_type == -1) {
        NuRndrBeginScene(1);
        if (MainRenderTime < 1.0f) {
            if (back_rgba[0] == back_rgba[1])
                NuRndrClear(0xf00, back_rgba[0], 1.0f);
            else
                NuRndrGradClear(0xf00, back_rgba[0], back_rgba[1], 1.0f);
            BackDrop_Draw(1.0f - MainRenderTime, 0);
        } else {
            NuRndrClear(0x300, 0, 1.0f);
        }
        NuRndrEndScene();
    }
    if (LevelCompletionFallbackFlagPC != 2 && pause_rndr_on != 0 && pause_rt != 0 && FadeSys.pending_type == -1)
        DrawStillScreen(clear_screen_onstill);
    if (Paused != 0) {
        if (old_pause_state == 0 && FadeSys.pending_type == -1)
            ScreenGrabNeededPc = 1;
    } else if (old_pause_state != 0) {
        pause_rndr_on = 0;
    }
    old_pause_state = Paused;
    if (waiting_for_level != -1 &&
        (gone_through_door_to_new_level != 0 || cut_waiting_for_new_level != 0 || waiting_for_new_level != 0))
        pause_rndr_on = 1;
    else if (NewLData == CREDITS_LDATA && LastLData == STATUS_LDATA)
        pause_rndr_on = 1;
    else if (NewLData == STATUS_LDATA && WORLD->area_index != -1 && (WORLD->area[WORLD->area_index].flags & 2) != 0)
        pause_rndr_on = 1;
    else if ((NewLData == STATUS_LDATA && GAMEDEMO != 0) || NewLData == CREDITS_LDATA)
        pause_rndr_on = 1;
    else if (NewLData == HUB_LDATA && WORLD->area_index != -1 && (WORLD->area[WORLD->area_index].flags & 2) != 0)
        pause_rndr_on = 1;
    else if (MainRenderTime > 0.0f && MainRenderTime < 1.0f)
        pause_rndr_on = 1;
    else if (Paused != 0 && ScreenGrabNeededPc == 0) {
        if (PauseRenderOffFn != NULL && PauseRenderOffFn() != 0)
            pause_rndr_on = 0;
        else
            pause_rndr_on = 1;
    } else if (LevelCompletionFallbackFlagPC == 0 || LevelCompletionFallbackFlagPC == 1)
        pause_rndr_on = 0;
    else
        pause_rndr_on = 1;
    if (screendump != 0)
        pause_rndr_on = 0;
    LevelCompletionFallbackFlagPC = 0;
    clear_screen_onstill = 1;
}
