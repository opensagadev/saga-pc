#include "saga.h"
#include <stdio.h>

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

extern "C" void __cdecl FinishStatusPacket(i32 choice) {
    u32 flags;
    i32 network;
    LEVELDATA_PC *destination;
    if (LOSTTEMPLE_ADATA != NULL && StatusPacket.area_id == (i16)LOSTTEMPLE_ADATA->index) {
        if (StatusPacket.player0_model == (u16)VehicleBoltId7f17d0Pc)
            StatusPacket.player0_model = (u16)VehicleBoltId7f17d4Pc;
        else if (StatusPacket.player1_model == (u16)VehicleBoltId7f17d0Pc)
            StatusPacket.player1_model = (u16)VehicleBoltId7f17d4Pc;
    }
    RememberPlayerIDs(1, (i16)StatusPacket.player0_model, (i16)StatusPacket.player1_model);
    if (StatusPacket.finish_callback(WORLD, &StatusPacket, choice) != 0)
        return;
    destination = HUB_LDATA;
    network = StartupGameFlag1;
    flags = StatusPacket.flags_word;
    NewLData = destination;
    if (choice == 0) {
        if ((flags & 0x100) != 0 ||
            (network != 0 && (StatusPacket.challenge_state != 0 || StatusPacket.mission_state != 0))) {
            FADETYPE fade = {FADE_TYPE_STILL_WIPE};
            NewLData = Area_FindNextPlayLevel(ADataList[StatusPacket.area->index].levels[0]);
            ResetBits = 0xffffffbfu;
            reset_area = 1;
            FadeSys.SetFade(fade, 0);
            NeedScreenGrab(1);
            GrabStillScreen();
            if (StatusPacket.challenge_state != 0)
                InitChallenge(StatusPacket.area->index);
            if (StatusPacket.mission_state != 0) {
                InitMission(MissionSys, (i8)MissionSys->active_mission->index);
                NewLData = &LDataList[MissionSys->active_mission->level];
            }
            destination = NewLData;
            flags = StatusPacket.flags_word;
            goto destination_selected;
        }
        if (StatusPacket.next_area != -1) {
            AREA_DATA_PC *area = &ADataList[StatusPacket.next_area];
            destination = &LDataList[area->levels[0]];
            NewLData = destination;
            if ((area->flags & 2) != 0)
                LevelCompletionFallbackFlagPC = 1;
            else {
                loadareacharacters_no_backdrop_reset = 1;
                if ((flags & 0x400) != 0)
                    finishloop_backdroponly = 1;
            }
            goto destination_selected;
        }
    }
    if (network != 0 &&
        ((flags & 0x400) != 0 || ((StatusPacket.area->flags & 4) != 0 && (StatusPacket.field_0xb0 & 0x40) == 0))) {
        destination = CREDITS_LDATA;
        NewLData = destination;
    }
destination_selected:
    if ((flags & 0x800) != 0) {
        destination = HUB_LDATA;
        NewLData = destination;
    }
    if (destination->area_index != -1 && (ADataList[destination->area_index].flags & 0x40) != 0 &&
        StatusPacket.mission_state != 0 && MissionSys->active_mission != NULL)
        hub_from_mission = (i8)MissionSys->active_mission->index;
    if (destination == HUB_LDATA) {
        OldBonusScore[0] = 0;
        OldBonusScore[1] = 0;
    } else {
        OldBonusScore[0] = BonusScore[0];
        OldBonusScore[1] = BonusScore[1];
    }
    NuSound3StopRumble();
}

void __cdecl RedBrick_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    volatile f32 blend;
    volatile i32 opacity;
    volatile u16 rotation, pitch;
    volatile f32 panel_y, text_y;
    if (current == 0 || stage->field_0x14 <= 0)
        return;
    if (stage->field_0x18 < 1.0f) {
        volatile i32 angle = (i32)((f64)stage->field_0x18 * 16384.0);
        blend = NuTrigTable[(angle >> 1) & 0x7fff];
    } else {
        volatile f32 fade_start = (f64)stage->field_0x1c - 1.0;
        if (fade_start > stage->field_0x18)
            blend = 1.0f;
        else {
            volatile i32 angle = (i32)((1.0 - ((f64)stage->field_0x18 - (f64)fade_start)) * 16384.0);
            blend = NuTrigTable[(angle >> 1) & 0x7fff];
        }
    }
    opacity = (i32)((f64)blend * 128.0);
    SmartTextEx(TTab[TextIdPc880748], 0.0f, 0.05f, 1.0f, StatusTextScalePc, StatusTextScalePc, StatusTextScalePc, 0,
                (u8)StatusRedPc, (u8)StatusGreenPc, (u8)StatusBluePc, 1.7f, 1, NULL, 0, (u32)opacity);
    rotation = (u16)((f64)NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
    pitch = (u16)((f64)NuTrigTable[rotation & 0x7fff] * 1820.0);
    panel_y = (f64)blend - 1.5;
    DrawPanel3DObjectNoAlpha(0.0f, panel_y, 1.0f, 1.0f, 1.0f, 1.0f, pitch, rotation, 0,
                             &WORLD->level_objects[210].special, 2);
    text_y = (f64)blend * -1.0 + 1.5;
    SmartTextEx(TTab[*ExtrasCheatsPc[packet->area->cheat].text_id], 0.0f, text_y, 1.0f, StatusTextScalePc,
                StatusTextScalePc, StatusTextScalePc, 0, 255, 255, 255, 1.7f, 1, NULL, 0, 128);
}

void __cdecl GoldBrick_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    if (current == 0)
        return;
    if (stage->field_0x14 > 0 && stage->field_0x18 >= 1.0f && (f64)stage->field_0x18 < (f64)stage->field_0x1c - 1.0) {
        char text[256];
        i32 total = (i32)Game.gold_bricks;
        i32 previous = (i32)packet->previous_gold_bricks;
        i32 last = total - previous - 1;
        i32 displayed;
        volatile f32 blend, text_y, panel_y;
        volatile i32 opacity;
        volatile u16 rotation, pitch;
        if (stage->field_0x18 < 2.0f) {
            volatile i32 angle = (i32)(((f64)stage->field_0x18 - 1.0) * 16384.0);
            blend = NuTrigTable[(angle >> 1) & 0x7fff];
        } else {
            volatile f32 fade_start = (f64)stage->field_0x1c - 2.0;
            if (fade_start > stage->field_0x18)
                blend = 1.0f;
            else {
                volatile i32 angle = (i32)((1.0 - ((f64)stage->field_0x18 - (f64)fade_start)) * 16384.0);
                blend = NuTrigTable[(angle >> 1) & 0x7fff];
            }
        }
        if (stage->field_0x18 < 2.0f)
            displayed = previous;
        else if ((f64)last * 2.5 + 2.0 > (f64)stage->field_0x18)
            displayed = (i32)(((f64)stage->field_0x18 - 2.0) / 2.5 + (f64)(previous + 1));
        else
            displayed = total;
        sprintf(text, "%i/%i", displayed, GOLDBRICKPOINTS);
        opacity = (i32)((f64)blend * 128.0);
        text_y = (f64)blend * (f64)-0.99999994f + (f64)1.3f;
        SmartTextEx(text, 0.0f, text_y, 1.0f, 1.5f, 1.5f, 1.5f, 0, 255, 255, 255, 1.7f, 1, NULL, 0, (u32)opacity);
        rotation = (u16)((f64)NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
        pitch = (u16)((f64)NuTrigTable[rotation & 0x7fff] * 1820.0);
        panel_y = (f64)blend * (f64)0.99999994f - (f64)1.3f;
        DrawPanel3DObjectNoAlpha(0.0f, panel_y, 1.0f, 1.0f, 1.0f, 1.0f, pitch, rotation, 0,
                                 &WORLD->level_objects[211].special, 2);
        if (Game.gold_bricks > 60) {
            volatile f32 coin_y = ((f64)STATSPOSY - (f64)STATSPOS2YPc) * (f64)blend + (f64)STATSPOS2YPc;
            CoinTotal_Draw((i32)*packet->score, coin_y, CoinTotalScale, 1, 1.0f, 255, 191, 0);
        }
    }
    if ((stage->field_0x14 == 0 || stage->field_0x18 < 1.0f) && packet->mission_state != 0 &&
        Game.gold_bricks > packet->previous_gold_bricks)
        iconalphaoverride = 0.0f;
}

static void StatusMiniKitRotateXPc(NUMTX *m, i32 a) {
    f32 cosx = NuTrigTable[(((i32)((u32)a + 0x4000u)) >> 1) & 0x7fff];
    f32 sinx = NuTrigTable[(a >> 1) & 0x7fff];
    f32 m01 = m->m01;
    f32 m11 = m->m11;
    f32 m21 = m->m21;
    f32 m31 = m->m31;

    m->m01 = m01 * cosx - m->m02 * sinx;
    m->m02 = m01 * sinx + m->m02 * cosx;
    m->m11 = m11 * cosx - m->m12 * sinx;
    m->m12 = m11 * sinx + m->m12 * cosx;
    m->m21 = m21 * cosx - m->m22 * sinx;
    m->m22 = m21 * sinx + m->m22 * cosx;
    m->m31 = m31 * cosx - m->m32 * sinx;
    m->m32 = m31 * sinx + m->m32 * cosx;
}

static void StatusMiniKitRotateYPc(NUMTX *m, i32 a) {
    f32 cosx = NuTrigTable[(((i32)((u32)a + 0x4000u)) >> 1) & 0x7fff];
    f32 sinx = NuTrigTable[(a >> 1) & 0x7fff];
    f32 m00 = m->m00;
    f32 m10 = m->m10;
    f32 m20 = m->m20;
    f32 m30 = m->m30;

    m->m00 = m00 * cosx + m->m02 * sinx;
    m->m02 = m->m02 * cosx - m00 * sinx;
    m->m10 = m10 * cosx + m->m12 * sinx;
    m->m12 = m->m12 * cosx - m10 * sinx;
    m->m20 = m20 * cosx + m->m22 * sinx;
    m->m22 = m->m22 * cosx - m20 * sinx;
    m->m30 = m30 * cosx + m->m32 * sinx;
    m->m32 = m->m32 * cosx - m30 * sinx;
}

static void StatusMiniKitRotateZPc(NUMTX *m, i32 a) {
    f32 cosx = NuTrigTable[(((i32)((u32)a + 0x4000u)) >> 1) & 0x7fff];
    f32 sinx = NuTrigTable[(a >> 1) & 0x7fff];
    f32 m00 = m->m00;
    f32 m10 = m->m10;
    f32 m20 = m->m20;
    f32 m30 = m->m30;

    m->m00 = m00 * cosx - m->m01 * sinx;
    m->m01 = m00 * sinx + m->m01 * cosx;
    m->m10 = m10 * cosx - m->m11 * sinx;
    m->m11 = m10 * sinx + m->m11 * cosx;
    m->m20 = m20 * cosx - m->m21 * sinx;
    m->m21 = m20 * sinx + m->m21 * cosx;
    m->m30 = m30 * cosx - m->m31 * sinx;
    m->m31 = m30 * sinx + m->m31 * cosx;
}

typedef struct status_world_minikit_reader_pc_s {
    u8 unknown_0000[0x2968];
    MINIKIT_PC minikit;
} STATUS_WORLD_MINIKIT_READER_PC;
STATIC_ASSERT(offsetof(STATUS_WORLD_MINIKIT_READER_PC, minikit) == 0x2968);

extern "C" {
    f32 StatusMiniKitDirectionOffsetPc = 0.035f;
    f32 StatusMiniKitProgressPc = 0.0f;
    STATUS_MINIKIT_DRAW_CACHE_PC StatusMiniKitDrawCachePc[10] = {0};
}

void __cdecl DrawStatusMiniKit(f32 x, f32 y, f32 z, f32 scale, f32 collected_scale, i32 max_pieces,
                               STATUSPACKET_s *packet, f32 phase) {
    STATUS_WORLD_MINIKIT_READER_PC *view = (STATUS_WORLD_MINIKIT_READER_PC *)WORLD;
    if (view->minikit.scene == NULL || view->minikit.pieces == NULL)
        return;
    volatile f32 base_scale = scale;
    volatile f32 previous_scale = collected_scale;
    NUVEC base = {base_scale, base_scale, base_scale};
    NUVEC collected = {previous_scale, previous_scale, previous_scale};
    volatile NUVEC offset = v000;
    i32 direction = (i8)view->minikit.pieces[StatusCurrentMinikitPc].direction;
    if ((u32)direction <= 5u) {
        volatile i32 angle = (i32)((f64)phase * 32768.0);
        switch (direction) {
            case 0:
                offset.x =
                    (f64)NuTrigTable[(angle >> 1) & 0x7fff] * (f64)StatusMiniKitDirectionOffsetPc + (f64)offset.x;
                break;
            case 1:
                offset.x =
                    (f64)offset.x - (f64)NuTrigTable[(angle >> 1) & 0x7fff] * (f64)StatusMiniKitDirectionOffsetPc;
                break;
            case 2:
                offset.y =
                    (f64)NuTrigTable[(angle >> 1) & 0x7fff] * (f64)StatusMiniKitDirectionOffsetPc + (f64)offset.y;
                break;
            case 3:
                offset.y =
                    (f64)offset.y - (f64)NuTrigTable[(angle >> 1) & 0x7fff] * (f64)StatusMiniKitDirectionOffsetPc;
                break;
            case 4:
                offset.z =
                    (f64)offset.z - (f64)NuTrigTable[(angle >> 1) & 0x7fff] * (f64)StatusMiniKitDirectionOffsetPc;
                break;
            case 5:
                offset.z =
                    (f64)NuTrigTable[(angle >> 1) & 0x7fff] * (f64)StatusMiniKitDirectionOffsetPc + (f64)offset.z;
                break;
        }
    }
    for (i32 index = 0; index < (i32)packet->minikit_max; ++index) {
        view = (STATUS_WORLD_MINIKIT_READER_PC *)WORLD;
        if (index >= (i32)view->minikit.piece_count || index >= max_pieces)
            break;
        NUMTX matrix = view->minikit.pieces[index].matrix;
        if (index >= StatusCurrentMinikitPc) {
            NuMtxScale(&matrix, &base);
            volatile f32 progress = index == StatusCurrentMinikitPc ? StatusMiniKitProgressPc : 1.0f;
            volatile i32 angle = (i32)(((f64)progress * 16384.0 + 49152.0) + 16384.0);
            volatile f32 reveal = NuTrigTable[(angle >> 1) & 0x7fff];
            view = (STATUS_WORLD_MINIKIT_READER_PC *)WORLD;
            MINIKIT_PIECE_PC *piece = &view->minikit.pieces[index];
            switch ((i8)piece->direction) {
                case 0:
                    matrix.m30 =
                        (f64)matrix.m30 - ((f64)(i32)piece->direction_index * (f64)0.025f + 0.25) * (f64)reveal;
                    break;
                case 1:
                    matrix.m30 =
                        ((f64)(i32)piece->direction_index * (f64)0.025f + 0.25) * (f64)reveal + (f64)matrix.m30;
                    break;
                case 2:
                    matrix.m31 =
                        (f64)matrix.m31 - ((f64)(i32)piece->direction_index * (f64)0.025f + 0.25) * (f64)reveal;
                    break;
                case 3:
                    matrix.m31 =
                        ((f64)(i32)piece->direction_index * (f64)0.025f + 0.25) * (f64)reveal + (f64)matrix.m31;
                    break;
                case 4:
                    matrix.m32 =
                        ((f64)(i32)piece->direction_index * (f64)0.025f + 0.25) * (f64)reveal + (f64)matrix.m32;
                    break;
                case 5:
                    matrix.m32 =
                        (f64)matrix.m32 - ((f64)(i32)piece->direction_index * (f64)0.025f + 0.25) * (f64)reveal;
                    break;
            }
        } else {
            NuMtxScale(&matrix, &collected);
        }
        if (index <= StatusCurrentMinikitPc) {
            matrix.m30 = (f64)matrix.m30 + (f64)offset.x;
            matrix.m31 = (f64)matrix.m31 + (f64)offset.y;
            matrix.m32 = (f64)matrix.m32 + (f64)offset.z;
        }
        volatile u16 rotation_y = (u16)((f64)NuFmod(GameTimer.time_elapsed, 2.0f) * 0.5 * 65536.0);
        StatusMiniKitRotateYPc(&matrix, (i32)rotation_y);
        volatile u16 phase_z = (u16)((f64)NuFmod(GameTimer.time_elapsed, 2.24f) / (f64)2.24f * 65536.0);
        volatile u16 rotation_z = (u16)((f64)NuTrigTable[phase_z >> 1] * 2730.0);
        StatusMiniKitRotateZPc(&matrix, (i32)rotation_z);
        volatile u16 phase_x = (u16)((f64)NuFmod(GameTimer.time_elapsed, 1.87f) / (f64)1.87f * 65536.0);
        volatile u16 rotation_x = (u16)((f64)NuTrigTable[phase_x >> 1] * 2730.0 - 5461.0);
        StatusMiniKitRotateXPc(&matrix, (i32)rotation_x);
        WORLDINFO *world = WORLD;
        matrix.m30 = (f64)PANEL3DMULX * (f64)x + (f64)matrix.m30;
        matrix.m31 = (f64)PANEL3DMULY * (f64)y + (f64)matrix.m31;
        matrix.m32 = (f64)matrix.m32 + (f64)z;
        if (index < 10) {
            StatusMiniKitDrawCachePc[index].matrix = matrix;
            StatusMiniKitDrawCachePc[index].piece = &((STATUS_WORLD_MINIKIT_READER_PC *)world)->minikit.pieces[index];
            StatusMiniKitDrawCachePc[index].active = 1;
        }
        if (world->level_objects[206].active != 0)
            DrawPanel3DObjectMtxNoAlpha(
                (nuhspecial_s *)&((STATUS_WORLD_MINIKIT_READER_PC *)world)->minikit.pieces[index].special, &matrix);
    }
}

extern "C" {
    f32 StatusAllMiniKitsTitleScalePc = 0.7f;
    f32 StatusAllMiniKitsCharacterTextScalePc = 0.6f;
    f32 StatusAllMiniKitsXPc = -0.6f;
    f32 StatusAllMiniKitsYPc = -0.5f;
    f32 StatusAllMiniKitsScalePc = 0.15f;
    f32 StatusAllMiniKitsZPc = 1.1f;
    f32 StatusAllMiniKitsCountYOffsetPc = -0.3f;
    f32 StatusAllMiniKitsCountScalePc = 0.5f;
    f32 StatusAllMiniKitsQuestionScalePc = 1.2f;
    f32 StatusAllMiniKitsQuestionYOffsetPc = -0.1f;
    f32 StatusAllMiniKitsCompleteScalePc = 0.8f;
    f32 StatusAllMiniKitsCompleteYOffsetPc = -0.2f;
}

void __cdecl AllMiniKits_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    if (current != 0) {
        if (stage->field_0x14 <= 0)
            return;
        volatile f32 title_alpha;
        volatile f32 icon_alpha = 0.0f;
        volatile f32 icon_y = 0.225f;
        if (stage->field_0x18 < 0.5f) {
            title_alpha = (f64)stage->field_0x18 + (f64)stage->field_0x18;
        } else if ((f64)stage->field_0x18 < 3.5) {
            title_alpha = 1.0f;
        } else if (stage->field_0x18 < 4.0f) {
            icon_alpha = ((f64)stage->field_0x18 - 3.5) * 2.0;
            title_alpha = 1.0 - (f64)icon_alpha;
            volatile i32 angle = (i32)((f64)icon_alpha * 16384.0);
            icon_y = (1.0 - (f64)NuTrigTable[(angle >> 1) & 0x7fff]) * -0.5 + (f64)0.225f;
        } else if (stage->field_0x18 < 6.0f) {
            title_alpha = 0.0f;
            icon_alpha = 1.0f;
            volatile i32 angle = (i32)((f64)icon_alpha * 16384.0);
            icon_y = (1.0 - (f64)NuTrigTable[(angle >> 1) & 0x7fff]) * -0.5 + (f64)0.225f;
        } else if ((f64)stage->field_0x18 < 6.5) {
            title_alpha = 0.0f;
            icon_alpha = 1.0 - ((f64)stage->field_0x18 - 6.0) * 2.0;
        } else {
            return;
        }
        if (icon_alpha > 0.0f) {
            DrawCharIcon(VehicleBoltId7f195cPc, 0.0f, icon_y, 0.0f, 0.4f, 0xa7, icon_alpha, icon_alpha, 1, NULL);
            volatile i32 opacity = (i32)((f64)icon_alpha * 128.0);
            SmartTextEx(TTab[CDataList[VehicleBoltId7f195cPc].name_id], 0.0f, -0.1f, 1.0f,
                        StatusAllMiniKitsCharacterTextScalePc, StatusAllMiniKitsCharacterTextScalePc,
                        StatusAllMiniKitsCharacterTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc, (u8)StatusBluePc,
                        1.7f, 1, NULL, 0, (u32)opacity);
        }
        if (title_alpha > 0.0f) {
            volatile u8 opacity = (u8)((f64)title_alpha * 128.0);
            Text3DEx(TTab[TextIdPc880740], 0.0f, 0.225f, 1.0f, StatusAllMiniKitsTitleScalePc,
                     StatusAllMiniKitsTitleScalePc, StatusAllMiniKitsTitleScalePc, 0, (u8)StatusRedPc,
                     (u8)StatusGreenPc, (u8)StatusBluePc, opacity);
        }
        return;
    }
    if (stage->field_0x12 == 0)
        return;
    volatile f32 alpha = getFinishedStatusAlpha(packet);
    volatile i32 reveal_angle = (i32)((f64)alpha * 16384.0);
    volatile f32 reveal = NuTrigTable[(reveal_angle >> 1) & 0x7fff];
    volatile i32 opacity = (i32)((f64)alpha * 128.0);
    volatile f32 timer = GameTimer.time_elapsed_mod_seconds;
    if (timer > 0.25f)
        timer = 0.0f;
    volatile i32 pulse_angle = (i32)((f64)timer * 32768.0 + 16384.0);
    union {
        f32 value;
        u32 bits;
    } pulse;
    pulse.value = NuTrigTable[(pulse_angle >> 1) & 0x7fff];
    pulse.bits &= 0x7fffffffu;
    volatile f32 inverse_pulse = 1.0 - (f64)pulse.value;
    volatile f32 pulse_scale = 1.0 + (f64)inverse_pulse;
    if (Game.area_save[packet->area->index].minikit_count != 0) {
        volatile f32 scale = (f64)StatusAllMiniKitsScalePc * (f64)reveal;
        DrawStatusMiniKit(StatusAllMiniKitsXPc, StatusAllMiniKitsYPc, StatusAllMiniKitsZPc, scale, 1.0f,
                          Game.area_save[packet->area->index].minikit_count, packet, 0.0f);
    } else {
        volatile f32 size = (f64)StatusAllMiniKitsQuestionScalePc * (f64)pulse_scale;
        volatile f32 y = (f64)StatusAllMiniKitsQuestionYOffsetPc + (f64)StatusAllMiniKitsYPc;
        Text3DEx("?", StatusAllMiniKitsXPc, y, StatusAllMiniKitsZPc, size, size, size, 0, 255, 255, 255, opacity);
    }
    if (Game.area_save[packet->area->index].minikit_count == packet->minikit_max) {
        volatile f32 y = (f64)StatusAllMiniKitsCompleteYOffsetPc + (f64)StatusAllMiniKitsYPc;
        Text3DEx("$", StatusAllMiniKitsXPc, y, 1.0f, StatusAllMiniKitsCompleteScalePc, StatusAllMiniKitsCompleteScalePc,
                 StatusAllMiniKitsCompleteScalePc, 0, 255, 0, 127, opacity);
    } else {
        char text[60];
        sprintf(text, "%i/%i", Game.area_save[packet->area->index].minikit_count, packet->minikit_max);
        volatile f32 y = (f64)StatusAllMiniKitsCountYOffsetPc + (f64)StatusAllMiniKitsYPc;
        Text3DEx(text, StatusAllMiniKitsXPc, y, 1.0f, StatusAllMiniKitsCountScalePc, StatusAllMiniKitsCountScalePc,
                 StatusAllMiniKitsCountScalePc, 0, 255, 0, 127, opacity);
    }
}

extern "C" {
    f32 StatusCoinsTotalYPc = 0.3f;
    f32 StatusCoinsScalePc = 2.0f;
    f32 StatusCoinsTitleScalePc = 0.5f;
    f32 StatusCoinsTitleYPc = 0.5f;
}

static f32 __cdecl StatusCoinsRatioPc(f32 numerator, f32 denominator) {
    if (numerator != 0.0f && denominator != 0.0f) {
        volatile f32 result = (f64)numerator / (f64)denominator;
        return result;
    }
    return 0.0f;
}

void __cdecl Coins_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    if (current == 0) {
        if (stage->field_0x12 != 0) {
            volatile i32 angle = (i32)((f64)getFinishedStatusAlpha(packet) * 16384.0);
            volatile f32 y =
                ((f64)STATSPOSY - (f64)STATSPOS2YPc) * (f64)NuTrigTable[(angle >> 1) & 0x7fff] + (f64)STATSPOS2YPc;
            CoinTotal_Draw((i32)*packet->score, y, CoinTotalScale, 1, 1.0f, 255, 191, 0);
        }
        return;
    }
    volatile f32 alpha = 1.0f;
    volatile f32 coin_y = STATSPOS2YPc;
    volatile f32 total_y = STATSPOS2YPc;
    volatile f32 scale = StatusCoinsScalePc;
    switch (stage->field_0x14) {
        case 0:
            alpha = 0.0f;
            break;
        case 1: {
            alpha = stage->field_0x18;
            volatile f32 ratio = (f64)stage->field_0x18 / (f64)stage->field_0x1c;
            volatile i32 angle = (i32)((f64)ratio * 32768.0 + 16384.0);
            volatile f32 blend = 1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5;
            StatusIconYPc = StatusIconsOnOff(StatusCoinsRatioPc(stage->field_0x18, stage->field_0x1c));
            total_y = ((f64)StatusCoinsTotalYPc - (f64)STATSPOS2YPc) * (f64)blend + (f64)STATSPOS2YPc;
            coin_y = ((f64)STATSPOSY - (f64)STATSPOS2YPc) * (f64)blend + (f64)STATSPOS2YPc;
            scale = StatusCoinsScalePc;
            break;
        }
        case 2:
            total_y = StatusCoinsTotalYPc;
            coin_y = STATSPOSY;
            break;
        case 3: {
            alpha = 1.0 - (f64)stage->field_0x18;
            volatile f32 ratio = 1.0 - (f64)stage->field_0x18 / (f64)stage->field_0x1c;
            volatile i32 angle = (i32)((f64)ratio * 32768.0 + 16384.0);
            volatile f32 blend = 1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5;
            total_y = ((f64)StatusCoinsTotalYPc - (f64)STATSPOSY) * (f64)blend + (f64)STATSPOSY;
            coin_y = ((f64)STATSPOSY - (f64)STATSPOS2YPc) * (f64)blend + (f64)STATSPOS2YPc;
            scale = ((f64)StatusCoinsScalePc - (f64)CoinTotalScale) * (f64)blend + (f64)CoinTotalScale;
            break;
        }
    }
    if (stage->field_0x14 > 0) {
        for (i32 player = 0; player < 2; ++player) {
            volatile f32 player_alpha = packet->player_active[player] != 0 ? 1.0f : MissionDropInAlphaPc;
            volatile f32 coin_x = player == 0 ? -PANEL_COINX : PANEL_COINX;
            volatile f32 score_x = player == 0 ? -HudScoreXPc : HudScoreXPc;
            u32 coins = packet->coins_remaining[player];
            i32 object_index = coins >= 1000u ? 0xc3 : coins >= 100u ? 0xbb : 0xb3;
            if (WORLD->level_objects[object_index].active != 0) {
                DrawPanel3DObject(coin_x, coin_y, 1.0f, PANEL_COINSCALE_END, PANEL_COINSCALE_END, PANEL_COINSCALE_END,
                                  0, 0, 0, &WORLD->level_objects[object_index].special, 0, player_alpha);
            }
            char text[256];
            Text_MakeScore(packet->coins_remaining[player], text);
            volatile u8 opacity = (u8)((f64)player_alpha * 128.0);
            Text3DEx(text, score_x, coin_y, 1.0f, HudScoreScalePc, HudScoreScalePc, HudScoreScalePc,
                     player == 0 ? 2 : 8, 255, 191, 0, opacity);
        }
        CoinTotal_Draw((i32)*packet->score, total_y, scale, 1, 1.0f, 255, 191, 0);
    }
    if (alpha < 0.0f)
        alpha = 0.0f;
    else if (alpha > 1.0f)
        alpha = 1.0f;
    volatile u8 opacity = (u8)((f64)alpha * 128.0);
    Text3DEx(TTab[TextIdPc8808d4], 0.0f, StatusCoinsTitleYPc, 1.0f, StatusCoinsTitleScalePc, StatusCoinsTitleScalePc,
             StatusCoinsTitleScalePc, 0, 255, 255, 255, opacity);
}

extern "C" {
    f32 StatusSecondaryTitleYPc = -0.1f;
}

void __cdecl LevelComplete_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    if (stage->type == 26 && packet->mission_state == 2)
        *packet->score = packet->reward_score;
    if (stage->type == 23 && packet->challenge_state == 2)
        *packet->score = packet->reward_score;
    NextStatusStage(packet);
}

void __cdecl LevelComplete_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    if (stage->field_0x14 == 0) {
        stage->field_0x14 = 1;
        stage->field_0x18 = 0.0f;
        stage->field_0x1c = 4.0f;
    } else if (stage->field_0x14 == 1) {
        SetDrawGoldBrick(packet, packet->current_gold_brick);
        volatile f32 previous = stage->field_0x18;
        volatile f32 accumulated = (f64)stage->field_0x18 + (f64)elapsed;
        stage->field_0x18 = accumulated;
        if (stage->field_0x18 >= stage->field_0x1c) {
            if (stage->type == 26 && packet->mission_state == 2)
                *packet->score = packet->reward_score;
            if (stage->type == 23 && packet->challenge_state == 2)
                *packet->score = packet->reward_score;
            NextStatusStage(packet);
            return;
        }
        if (previous < 0.5f && stage->field_0x18 >= 0.5f) {
            if ((stage->type == 23 && packet->challenge_state == 3) ||
                (stage->type == 26 && packet->mission_state == 3))
                GameAudio_PlaySfx(0x32, NULL, 0, 0);
            else
                PlaySfx("StatusAward", NULL);
            NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
            return;
        }
        if ((stage->type != 26 || packet->mission_state != 2) && (stage->type != 23 || packet->challenge_state != 2))
            return;
        volatile f32 reward_end = (f64)stage->field_0x1c - 0.75;
        if (previous < reward_end && stage->field_0x18 >= reward_end) {
            CoinTotalScale = 1.5f;
            NewStatusRumbleBuzz(-1, 0.0f, 0.1f, 0);
            PlaySfx("Shop_BuyCheat", NULL);
        } else if (stage->field_0x18 >= 0.75f && stage->field_0x18 < reward_end) {
            PlaySfx("PickupCoin", NULL);
        }
    }
}

void __cdecl LevelComplete_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    char text[256];
    if (current == 0) {
        if (stage->field_0x12 != 0) {
            volatile f32 alpha = getFinishedStatusAlpha(packet);
            if ((stage->type == 26 && packet->mission_state == 2) ||
                (stage->type == 23 && packet->challenge_state == 2))
                CoinTotal_Draw((i32)*packet->score, STATSPOSY, CoinTotalScale, 1, alpha, 255, 191, 0);
        }
        return;
    }
    if (stage->field_0x14 > 0) {
        volatile f32 alpha;
        if (stage->field_0x18 < 0.5f)
            alpha = (f64)stage->field_0x18 * 2.0;
        else if ((f64)stage->field_0x18 < 3.5)
            alpha = 1.0f;
        else
            alpha = 1.0 - ((f64)stage->field_0x18 - 3.5) * 2.0;
        if (alpha > 0.0f) {
            volatile i32 opacity = (i32)((f64)alpha * 128.0);
            if (stage->type == 14) {
                sprintf(text, "%s", TTab[EDataList[packet->episode_id].text_id]);
                volatile f32 scale = (f64)StatusTextScalePc * 0.75;
                SmartTextEx(text, 0.0f, StatusTitleYPc, 1.0f, scale, scale, scale, 0, (u8)StatusRedPc,
                            (u8)StatusGreenPc, (u8)StatusBluePc, 1.7f, 1, NULL, 0, (u32)opacity);
                SmartTextEx(TTab[TextIdPc88073c], 0.0f, StatusSecondaryTitleYPc, 1.0f, StatusTextScalePc,
                            StatusTextScalePc, StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc,
                            (u8)StatusBluePc, 1.7f, 1, NULL, 0, (u32)opacity);
            } else if (stage->type == 13) {
                if ((PODRACE_ADATA != NULL && packet->area == PODRACE_ADATA) ||
                    (BONUS_GUNSHIP_ADATA != NULL && packet->area == BONUS_GUNSHIP_ADATA)) {
                    volatile f32 y = ((f64)StatusSecondaryTitleYPc + (f64)StatusTitleYPc) * 0.5;
                    SmartTextEx(TTab[TextIdPc880728], 0.0f, y, 1.0f, StatusTextScalePc, StatusTextScalePc,
                                StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc, (u8)StatusBluePc, 1.7f, 1,
                                NULL, 0, (u32)opacity);
                } else {
                    SmartTextEx(TTab[TextIdPc880728], 0.0f, StatusTitleYPc, 1.0f, StatusTextScalePc, StatusTextScalePc,
                                StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc, (u8)StatusBluePc, 1.7f, 1,
                                NULL, 0, (u32)opacity);
                    SmartTextEx(TTab[TextIdPc88072c], 0.0f, StatusSecondaryTitleYPc, 1.0f, StatusTextScalePc,
                                StatusTextScalePc, StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc,
                                (u8)StatusBluePc, 1.7f, 1, NULL, 0, (u32)opacity);
                }
            } else if (stage->type == 8) {
                SmartTextEx(TTab[TextIdPc8810cc], 0.0f, StatusTitleYPc, 1.0f, StatusTextScalePc, StatusTextScalePc,
                            StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc, (u8)StatusBluePc, 1.7f, 1, NULL,
                            0, (u32)opacity);
                SmartTextEx(TTab[TextIdPc8810d0], 0.0f, StatusSecondaryTitleYPc, 1.0f, StatusTextScalePc,
                            StatusTextScalePc, StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc,
                            (u8)StatusBluePc, 1.7f, 1, NULL, 0, (u32)opacity);
            } else if (stage->type == 28) {
                SmartTextEx(TTab[TextIdPc880710], 0.0f, 0.0f, 1.0f, StatusTextScalePc, StatusTextScalePc,
                            StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc, (u8)StatusBluePc, 1.7f, 1, NULL,
                            0, (u32)opacity);
            } else if (stage->type == 23) {
                i16 label;
                i32 red, green, blue;
                if (packet->challenge_state == 2) {
                    label = TextIdPc8806e8;
                    red = green = blue = 255;
                } else {
                    red = StatusRedPc;
                    green = StatusGreenPc;
                    blue = StatusBluePc;
                    label = packet->challenge_state == 3 ? TextIdPc8806f4 : TextIdPc88100c;
                }
                SmartTextEx(TTab[label], 0.0f, 0.0f, 1.0f, StatusTextScalePc, StatusTextScalePc, StatusTextScalePc, 0,
                            (u8)red, (u8)green, (u8)blue, 1.7f, 1, NULL, 0, (u32)opacity);
            } else if (stage->type == 26) {
                i16 label;
                i32 red, green, blue;
                if (packet->mission_state == 2) {
                    label = TextIdPc8806e4;
                    red = green = blue = 255;
                } else {
                    red = StatusRedPc;
                    green = StatusGreenPc;
                    blue = StatusBluePc;
                    label = TextIdPc8806f4;
                }
                SmartTextEx(TTab[label], 0.0f, 0.0f, 1.0f, StatusTextScalePc, StatusTextScalePc, StatusTextScalePc, 0,
                            (u8)red, (u8)green, (u8)blue, 1.7f, 1, NULL, 0, (u32)opacity);
            } else {
                i16 label;
                if (stage->type == 29)
                    label = TextIdPc881014;
                else if (stage->type == 30)
                    label = TextIdPc88101c;
                else if (stage->type == 31)
                    label = TextIdPc881024;
                else if (stage->type == 32)
                    label = TextIdPc88102c;
                else if (stage->type == 33)
                    label = TextIdPc881034;
                else
                    label = TextIdPc88103c;
                SmartTextEx(TTab[label], 0.0f, StatusTitleYPc, 1.0f, StatusTextScalePc, StatusTextScalePc,
                            StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc, (u8)StatusBluePc, 1.7f, 1, NULL,
                            0, (u32)opacity);
                SmartTextEx(TTab[TextIdPc880730], 0.0f, StatusSecondaryTitleYPc, 1.0f, StatusTextScalePc,
                            StatusTextScalePc, StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc,
                            (u8)StatusBluePc, 1.7f, 1, NULL, 0, (u32)opacity);
            }
        }
    }
    if (stage->type != 23 && stage->type != 26)
        return;
    if (stage->field_0x14 == 0)
        iconalphaoverride = 0.0f;
    else {
        volatile f32 fade_start = (f64)stage->field_0x1c - 0.5;
        if (fade_start > stage->field_0x18)
            iconalphaoverride = 0.0f;
        else
            iconalphaoverride = ((f64)stage->field_0x18 - (f64)fade_start) * 2.0;
    }
    if (stage->type == 26) {
        if (packet->mission_state != 2)
            return;
        if (packet->previous_gold_bricks < Game.gold_bricks)
            iconalphaoverride = 0.0f;
    } else if (packet->challenge_state != 2) {
        return;
    }
    volatile f32 progress, slide;
    if (stage->field_0x18 < 0.75f) {
        progress = 0.0f;
        slide = (f64)stage->field_0x18 / (f64)0.7f;
    } else {
        volatile f32 end = (f64)stage->field_0x1c - 0.75;
        if (end > stage->field_0x18) {
            progress = ((f64)stage->field_0x18 - 0.75) / ((f64)end - 0.75);
            slide = 1.0f;
        } else {
            progress = 1.0f;
            slide = 1.0 - ((f64)stage->field_0x18 - (f64)end) / 0.75;
        }
    }
    volatile f32 coin_slide = slide;
    if (stage->field_0x18 >= 0.75f)
        coin_slide = 1.0f;
    u32 score = packet->original_score;
    volatile i32 amount = (i32)((f64)(u32)(packet->reward_score - score) * (f64)progress);
    IncreaseScore(&score, (u64)(i64)amount, 0);
    volatile i32 coin_angle = (i32)((f64)coin_slide * 16384.0);
    volatile f32 coin_y =
        ((f64)STATSPOSY - (f64)STATSPOS2YPc) * (f64)NuTrigTable[(coin_angle >> 1) & 0x7fff] + (f64)STATSPOS2YPc;
    CoinTotal_Draw((i32)score, coin_y, CoinTotalScale, 1, 1.0f, 255, 191, 0);
    if (stage->type == 26) {
        volatile f32 remaining = (f64)(u16)packet->mission->time - (f64)packet->elapsed_time;
        if (remaining < 0.0f)
            remaining = 0.0f;
        volatile f32 time = (1.0 - (f64)progress) * (f64)remaining;
        Text_MakeTime(time, 0, 1, 1, text);
        volatile i32 angle = (i32)((f64)slide * 16384.0);
        volatile f32 y =
            -(((f64)STATSPOSY - (f64)STATSPOS2YPc) * (f64)NuTrigTable[(angle >> 1) & 0x7fff] + (f64)STATSPOS2YPc);
        Text3D(text, 0.0f, y, 1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 191, 0);
    }
    if (stage->type == 23) {
        volatile f32 remaining = (f64)(u16)packet->area->challenge_trial_time - (f64)packet->elapsed_time;
        if (remaining < 0.0f)
            remaining = 0.0f;
        volatile f32 time = (1.0 - (f64)progress) * (f64)remaining;
        Text_MakeTime(time, 0, 1, 1, text);
        volatile i32 angle = (i32)((f64)slide * 16384.0);
        volatile f32 y =
            -(((f64)STATSPOSY - (f64)STATSPOS2YPc) * (f64)NuTrigTable[(angle >> 1) & 0x7fff] + (f64)STATSPOS2YPc);
        Text3D(text, 0.0f, y, 1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 191, 0);
    }
}

extern "C" {
    f32 StatusTrueHeroBarScalePc = 1.75f;
    f32 StatusTrueHeroBarYPc = 0.7f;
    f32 StatusTrueHeroTextScalePc = 0.8f;
    f32 StatusTrueHeroTextYPc = 0.3f;
    f32 StatusTrueHeroNextSoundPercentPc = 5.0f;
    f32 StatusTrueHeroSpDelayPc = 0.0f;
    const char StatusTrueHeroPercentFormatPc[5] = "%i%%";
    const char StatusTrueHeroFullPercentTextPc[5] = "100%";
}

void __cdecl TrueHero_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    NextStatusStage(packet);
}

void __cdecl TrueHero_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0:
            stage->field_0x14 = 1;
            stage->field_0x18 = 0.0f;
            stage->field_0x1c = 1.0f;
            break;
        case 1: {
            volatile f32 time = (f64)elapsed + (f64)stage->field_0x18;
            stage->field_0x18 = time;
            if (stage->field_0x18 >= stage->field_0x1c) {
                stage->field_0x14 = 2;
                stage->field_0x18 = 0.0f;
                stage->field_0x1c = 3.0f;
                packet->true_hero_percent = 0.0f;
                StatusTrueHeroNextSoundPercentPc = 5.0f;
            }
            break;
        }
        case 2: {
            SetDrawGoldBrick(packet, packet->current_gold_brick);
            volatile f32 percent;
            if ((packet->field_0xb0 & 4) == 0) {
                percent = (f64)packet->collected_score * 100.0 / (f64)packet->true_hero_target;
                if (percent > 99.0f)
                    percent = 99.0f;
            } else {
                percent = 100.0f;
            }
            stage->field_0x18 = (f64)elapsed + (f64)stage->field_0x18;
            volatile f32 animated = (100.0 / (f64)stage->field_0x1c) * (f64)elapsed + (f64)packet->true_hero_percent;
            packet->true_hero_percent = animated;
            if (packet->true_hero_percent > percent) {
                packet->true_hero_percent = percent;
            } else if (packet->true_hero_percent >= StatusTrueHeroNextSoundPercentPc) {
                PlaySfx("Status_GoldBarDec", NULL);
                StatusTrueHeroNextSoundPercentPc = (f64)StatusTrueHeroNextSoundPercentPc + 10.0;
            }
            if (stage->field_0x18 > stage->field_0x1c) {
                stage->field_0x18 = 0.0f;
                stage->field_0x1c = 1.0f;
                if (stage->type == 2) {
                    stage->field_0x14 = 6;
                    PlaySfx("TrueJedi_NOT", NULL);
                } else {
                    stage->field_0x14 = 3;
                    PlaySfx("TrueJedi_100pc", NULL);
                }
            }
            break;
        }
        case 3: {
            volatile f32 time = (f64)elapsed + (f64)stage->field_0x18;
            stage->field_0x18 = time;
            if (stage->field_0x18 >= stage->field_0x1c) {
                stage->field_0x14 = 6;
                stage->field_0x18 = 0.0f;
                stage->field_0x1c = 1.0f;
                StatusTrueHeroSpDelayPc = 0.0f;
            }
            break;
        }
        case 6: {
            volatile f32 time = (f64)elapsed + (f64)stage->field_0x18;
            stage->field_0x18 = time;
            if (stage->field_0x18 >= stage->field_0x1c)
                NextStatusStage(packet);
            break;
        }
    }
}

void __cdecl TrueHero_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    if (current == 0)
        return;
    char text[256];
    volatile f32 alpha = 1.0f;
    switch (stage->field_0x14) {
        case 0:
            alpha = 0.0f;
            break;
        case 1: {
            alpha = stage->field_0x18;
            volatile i32 angle =
                (i32)((f64)StatusCoinsRatioPc(stage->field_0x18, stage->field_0x1c) * 32768.0 + 16384.0);
            volatile f32 blend = 1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5;
            volatile f32 scale = ((f64)StatusTrueHeroBarScalePc - 1.0) * (f64)blend + 1.0;
            volatile i32 position_angle = (i32)((f64)blend * 16384.0);
            volatile f32 y =
                ((f64)StatusTrueHeroBarYPc - (f64)STATSPOS2YPc) * (f64)NuTrigTable[(position_angle >> 1) & 0x7fff] +
                (f64)STATSPOS2YPc;
            DrawBuildUpBar(0.0f, y, 0, 100, 1.0f, scale, 1.0f, 0);
            break;
        }
        case 2: {
            volatile f32 time = stage->field_0x18;
            if (time > 1.0f)
                time = 1.0f;
            volatile i32 angle = (i32)((f64)time * 32768.0 + 16384.0);
            volatile f32 blend = 1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5;
            volatile i32 count = (i32)packet->true_hero_percent;
            DrawBuildUpBar(0.0f, StatusTrueHeroBarYPc, count, 100, 1.0f, StatusTrueHeroBarScalePc, 1.0f, 0);
            volatile i32 number = (i32)packet->true_hero_percent;
            sprintf(text, StatusTrueHeroPercentFormatPc, number);
            volatile u8 opacity = (u8)((f64)blend * 128.0);
            Text3DEx(text, 0.0f, StatusTrueHeroTextYPc, 1.0f, StatusTrueHeroTextScalePc, StatusTrueHeroTextScalePc,
                     StatusTrueHeroTextScalePc, 0, 255, 191, 0, opacity);
            break;
        }
        case 3: {
            volatile f32 time;
            if (stage->field_0x18 < 1.0f) {
                time = stage->field_0x18;
            } else {
                volatile i32 compared = stage->field_0x18 < stage->field_0x1c;
                (void)compared;
                time = 1.0f;
            }
            volatile i32 angle = (i32)((f64)time * 32768.0 + 16384.0);
            volatile f32 blend = 1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5;
            if (blend < 1.0f) {
                volatile f32 grow = (f64)blend * 0.5 + 1.0;
                volatile f32 shrink = 1.0 - (f64)blend * 0.5;
                volatile u8 opacity = (u8)((1.0 - (f64)blend) * 64.0);
                volatile f32 scale = (f64)StatusTrueHeroTextScalePc * (f64)grow;
                Text3DEx((char *)StatusTrueHeroFullPercentTextPc, 0.0f, StatusTrueHeroTextYPc, 1.0f, scale, scale,
                         scale, 0, 255, 191, 0, opacity);
                scale = (f64)StatusTrueHeroTextScalePc * (f64)shrink;
                Text3DEx((char *)StatusTrueHeroFullPercentTextPc, 0.0f, StatusTrueHeroTextYPc, 1.0f, scale, scale,
                         scale, 0, 255, 191, 0, opacity);
            }
            volatile i32 count = (i32)packet->true_hero_percent;
            DrawBuildUpBar(0.0f, StatusTrueHeroBarYPc, count, 100, 1.0f, StatusTrueHeroBarScalePc, 1.0f, 0);
            break;
        }
        case 6: {
            alpha = 1.0 - (f64)stage->field_0x18;
            if (stage->type == 2 && alpha > 0.0f) {
                volatile i32 number = (i32)packet->true_hero_percent;
                sprintf(text, StatusTrueHeroPercentFormatPc, number);
                volatile u8 opacity = (u8)((f64)alpha * 128.0);
                Text3DEx(text, 0.0f, StatusTrueHeroTextYPc, 1.0f, StatusTrueHeroTextScalePc, StatusTrueHeroTextScalePc,
                         StatusTrueHeroTextScalePc, 0, 255, 191, 0, opacity);
            }
            volatile f32 ratio = stage->field_0x18 < 1.0f ? 1.0 - (f64)stage->field_0x18 : 0.0;
            volatile i32 angle = (i32)((f64)ratio * 32768.0 + 16384.0);
            volatile f32 blend = 1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5;
            volatile f32 scale = ((f64)StatusTrueHeroBarScalePc - 1.0) * (f64)blend + 1.0;
            volatile i32 count = (i32)packet->true_hero_percent;
            volatile i32 position_angle = (i32)((f64)blend * 16384.0);
            volatile f32 y =
                ((f64)StatusTrueHeroBarYPc - (f64)STATSPOS2YPc) * (f64)NuTrigTable[(position_angle >> 1) & 0x7fff] +
                (f64)STATSPOS2YPc;
            DrawBuildUpBar(0.0f, y, count, 100, 1.0f, scale, 1.0f, 0);
            break;
        }
    }
    if (alpha < 0.0f)
        alpha = 0.0f;
    else if (alpha > 1.0f)
        alpha = 1.0f;
    NuStrCpy(text, TTab[TextIdPc880f9c]);
    if (HubBothTrueJediGoldBricksPc != 0) {
        NuStrCat(text, " ");
        NuStrCat(text, TTab[(packet->field_0xb0 & 0x40) != 0 ? TextIdPc8808a8 : TextIdPc8808a4]);
    }
    volatile u8 opacity = (u8)((f64)alpha * 128.0);
    Text3DEx(text, 0.0f, StatusCoinsTitleYPc, 1.0f, StatusCoinsTitleScalePc, StatusCoinsTitleScalePc,
             StatusCoinsTitleScalePc, 0, 255, 255, 255, opacity);
}

extern "C" {
    f32 StatusMiniKitJibberLengthPc = 0.1f;
    f32 StatusMiniKitSlideTimePc = 0.25f;
    f32 StatusMiniKitPopTimerPc = 0.0f;
    PART_LIGHTING_DATA_PC StatusMiniKitPartLightingPc = {0};
}

i32 __cdecl UpdateNewMiniKits(STATUSPACKET_s *packet, STATUS_STAGE_s *stage) {
    (void)packet;
    if (stage->field_0x18 > stage->field_0x1c) {
        stage->field_0x18 = 0.0f;
        StatusCurrentMinikitPc = (i32)((u32)StatusCurrentMinikitPc + 1u);
        StatusMiniKitPopTimerPc = 0.5f;
        StatusMiniKitProgressPc = 1.0f;
        return 1;
    }
    if (((f64)stage->field_0x1c - StatusMiniKitJibberLengthPc) - StatusMiniKitSlideTimePc < stage->field_0x18 &&
        stage->field_0x18 <= (f64)stage->field_0x1c - StatusMiniKitJibberLengthPc) {
        StatusMiniKitProgressPc = (f32)((((f64)stage->field_0x1c - stage->field_0x18) - StatusMiniKitJibberLengthPc) /
                                        StatusMiniKitSlideTimePc);
    }
    return 0;
}

void __cdecl AddStatusMiniKitParts(void) {
    NUVEC target_position = {0.0f, STATSPOSY, 1.0f};
    for (i32 i = 0; i < 3; ++i) {
        StatusMiniKitPartLightingPc.values[i] = PanelLightingColoursPc[i];
        StatusMiniKitPartLightingPc.values[i + 3] = PanelLightingDirectionsPc[i];
    }
    StatusMiniKitPartLightingPc.values[6] = PanelLightingAmbientPc;
    f32 delay = 0.0f;
    for (i32 i = 0; i < 10; ++i) {
        if (StatusMiniKitDrawCachePc[i].active == 0)
            continue;
        NUVEC velocity = {0.0f, (f32)(((f64)qrand() * (f64)(1.0f / 65535.0f)) * 0.25 + 0.25), 0.0f};
        NuVecRotateZ(&velocity, &velocity, qrand());
        ADDPART_PC part = Default_ADDPART;
        part.matrix = &StatusMiniKitDrawCachePc[i].matrix;
        part.velocity = &velocity;
        part.flags = 0x480;
        part.gravity = 0.0f;
        part.special = &StatusMiniKitDrawCachePc[i].piece->special;
        part.time_step = FRAMETIME;
        part.lighting_data = &StatusMiniKitPartLightingPc;
        AddPart(&part);
        for (i32 coin = 0; coin < 5; ++coin) {
            NUVEC position = {0.0f, (f32)(((f64)qrand() * (f64)(1.0f / 65535.0f)) * (f64)0.1f), 1.0f};
            NuVecRotateZ(&position, &position, qrand());
            ADD_GAME_MESSAGE_PC message = AddGameMsg_Default;
            message.position = &position;
            message.target_scale = 0.75f;
            message.player_index = (i8)(qrand() / 0x8000);
            target_position.x = cointotal_x[(i32)message.player_index];
            message.target_position = &target_position;
            message.target_scale = COINTOTAL_COINSIZE;
            message.icon = GizmoPickupType[2].first_model_id;
            message.special = &WORLD->level_objects[message.icon].special;
            message.flags = 0x112d;
            message.duration = COINMSGTIME;
            message.score = GizmoPickupType[2].score;
            message.end_fn = EndScoreMessage;
            message.delay = delay;
            message.update_fn = GameMsg_DrawAdjustNewPos_CoinToTotal;
            message.target_is_coin_total = 1;
            AddGameMsg(&message);
            if (delay == 0.0f) {
                i32 priority = 1;
                if (StatusPacket.player_active[0] != 0) {
                    if (StatusPacket.player0_buzz_amount < 0.0f)
                        StatusPacket.player0_buzz_amount = 0.0f;
                    if (priority > 0) {
                        i32 previous_priority = StatusPacket.player0_rumble_priority;
                        ++priority;
                        if (priority > previous_priority)
                            StatusPacket.player0_rumble_priority = (u8)priority;
                    }
                }
                if (StatusPacket.player_active[1] != 0) {
                    if (StatusPacket.player1_buzz_amount < 0.0f)
                        StatusPacket.player1_buzz_amount = 0.0f;
                    if (priority > 0) {
                        i32 previous_priority = StatusPacket.player1_rumble_priority;
                        ++priority;
                        if (priority > previous_priority)
                            StatusPacket.player1_rumble_priority = (u8)priority;
                    }
                }
                AddGameDebris(WORLD->game_debris_sys, 0x38, &position);
            }
            delay = (f32)((f64)delay + (f64)0.1f);
        }
    }
    SetPanelLights(1.0f);
}

extern "C" {
    f32 StatusMiniKitEntranceHeightPc = 1.4f;
    f32 StatusMiniKitScalePc = 0.333f;
    f32 StatusMiniKitPieceDurationPc = 0.4f;
    f32 StatusMiniKitXPc = 0.0f;
    f32 StatusMiniKitYPc = 0.0f;
}

void __cdecl MiniKit_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    if ((packet->field_0xb0 & 2) == 0) {
        StatusCurrentMinikitPc = (i32)((u32)StatusCurrentMinikitPc + (u32)StatusNewMinikitCountPc);
        if ((packet->field_0xb0 & 0x10) != 0 && (stage->field_0x14 != 6 || !(stage->field_0x18 >= stage->field_0x1c)))
            IncreaseScore(packet->score, 50000, 0);
    }
    NextStatusStage(packet);
}

void __cdecl MiniKit_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0:
            StatusNewMinikitCountPc = packet->new_minikits;
            if (StatusNewMinikitCountPc > 0)
                StatusCurrentMinikitPc =
                    (i32)((u32)Game.area_save[packet->area->index].minikit_count - (u32)StatusNewMinikitCountPc);
            else
                StatusCurrentMinikitPc = Game.area_save[packet->area->index].minikit_count;
            stage->field_0x18 = 0.0f;
            stage->field_0x14 = 1;
            stage->field_0x1c = 1.0f;
            break;
        case 1: {
            volatile f32 time = (f64)stage->field_0x18 + elapsed;
            stage->field_0x18 = time;
            if (stage->field_0x18 >= stage->field_0x1c) {
                stage->field_0x18 = 0.0f;
                if (StatusNewMinikitCountPc == 0) {
                    PlaySfx("TrueJedi_NOT", NULL);
                    stage->field_0x1c = 1.0f;
                    stage->field_0x14 = 4;
                } else {
                    stage->field_0x14 = 2;
                    stage->field_0x1c = 0.5f;
                    StatusMiniKitProgressPc = 1.0f;
                }
            }
            break;
        }
        case 2: {
            volatile f32 time = (f64)stage->field_0x18 + elapsed;
            stage->field_0x18 = time;
            if (stage->field_0x18 >= stage->field_0x1c) {
                stage->field_0x18 = 0.0f;
                stage->field_0x14 = 3;
                stage->field_0x1c = StatusMiniKitPieceDurationPc;
                PlaySfx("Jp_Ana_Jump", NULL);
            }
            break;
        }
        case 3: {
            if (StatusNewMinikitCountPc > 0) {
                volatile f32 previous = stage->field_0x18;
                volatile f32 time = (f64)stage->field_0x18 + elapsed;
                stage->field_0x18 = time;
                volatile f32 sound_start = (f64)stage->field_0x1c - StatusMiniKitJibberLengthPc;
                if (previous < sound_start && stage->field_0x18 >= sound_start) {
                    NewStatusRumbleBuzz(-1, 0.0f, 0.1f, 0);
                    PlaySfx("MK-Panel", NULL);
                }
                if (UpdateNewMiniKits(packet, stage) == 1) {
                    if (StatusCurrentMinikitPc < packet->minikit_count) {
                        stage->field_0x14 = 2;
                        stage->field_0x1c = 0.5f;
                        stage->field_0x18 = 0.0f;
                    } else {
                        stage->field_0x1c = 1.0f;
                        stage->field_0x18 = 0.0f;
                        stage->field_0x14 = 4;
                    }
                }
            } else {
                stage->field_0x18 = 0.0f;
                stage->field_0x1c = 3.0f;
                stage->field_0x14 = 4;
            }
            if (stage->field_0x14 == 4) {
                if ((packet->field_0xb0 & 0x10) != 0) {
                    stage->field_0x14 = 6;
                    stage->field_0x1c = 1.0f;
                } else {
                    PlaySfx("TrueJedi_NOT", NULL);
                }
            }
            break;
        }
        case 4: {
            volatile f32 time = (f64)stage->field_0x18 + elapsed;
            stage->field_0x18 = time;
            if (stage->field_0x18 >= stage->field_0x1c)
                NextStatusStage(packet);
            break;
        }
        case 6: {
            SetDrawGoldBrick(packet, packet->current_gold_brick);
            volatile f32 previous = stage->field_0x18;
            volatile f32 time = (f64)stage->field_0x18 + elapsed;
            stage->field_0x18 = time;
            if (stage->field_0x18 >= stage->field_0x1c) {
                if (previous < stage->field_0x1c) {
                    AddStatusMiniKitParts();
                    NewStatusRumbleBuzz(-1, 1.0f, 0.1f, 0);
                    PlaySfx("Explode1", NULL);
                } else if (FindGameMsgsWithID(1, 0, -1, NULL) == 0) {
                    PlaySfx("Shop_BuyCheat", NULL);
                    NextStatusStage(packet);
                }
            } else {
                u32 frame = (u32)GameTimer.update_count;
                if (frame % 6u < 3u && (frame - 1u) % 6u >= 3u)
                    NewStatusRumbleBuzz(-1, 0.0f, 0.0f, 2);
            }
            break;
        }
    }
}

void __cdecl MiniKit_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    if (current != 0) {
        volatile f32 title_alpha = 1.0f;
        switch (stage->field_0x14) {
            case 0:
                title_alpha = 0.0f;
                break;
            case 1: {
                title_alpha = stage->field_0x18;
                volatile f32 angle_time;
                if (stage->field_0x18 < 1.0f) {
                    angle_time = stage->field_0x18;
                } else {
                    volatile i32 unused_duration_compare = stage->field_0x1c < stage->field_0x18;
                    (void)unused_duration_compare;
                    angle_time = 1.0f;
                }
                volatile i32 angle = (i32)((f64)angle_time * 32768.0 + 16384.0);
                volatile f32 blend = 1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5;
                if (stage->field_0x18 <= stage->field_0x1c) {
                    volatile f32 y = ((f64)StatusMiniKitYPc - StatusMiniKitEntranceHeightPc) * (f64)blend +
                                     StatusMiniKitEntranceHeightPc;
                    DrawStatusMiniKit(StatusMiniKitXPc, y, StatusAllMiniKitsZPc, StatusMiniKitScalePc, 0.0f,
                                      StatusCurrentMinikitPc, packet, 0.0f);
                } else {
                    DrawStatusMiniKit(StatusMiniKitXPc, StatusMiniKitYPc, StatusAllMiniKitsZPc, StatusMiniKitScalePc,
                                      0.0f, StatusCurrentMinikitPc, packet, 0.0f);
                }
                i32 count = packet->new_minikits > 0 ? StatusCurrentMinikitPc
                                                     : Game.area_save[packet->area->index].minikit_count;
                if (count < 0)
                    count = 0;
                DrawMiniKitCount(blend, 1.0f, count, packet->minikit_max);
                break;
            }
            case 2: {
                volatile f32 collected_scale;
                if (stage->field_0x18 < (f64)stage->field_0x1c - 0.25) {
                    volatile f32 duration = (f64)stage->field_0x1c - 0.25;
                    volatile i32 angle =
                        (i32)(((f64)StatusCoinsRatioPc(stage->field_0x18, duration) * 16384.0 + 49152.0) + 16384.0);
                    collected_scale = (f64)NuTrigTable[(angle >> 1) & 0x7fff] * StatusMiniKitScalePc;
                } else {
                    collected_scale = StatusMiniKitScalePc;
                }
                DrawStatusMiniKit(StatusMiniKitXPc, StatusMiniKitYPc, StatusAllMiniKitsZPc, StatusMiniKitScalePc,
                                  collected_scale, (i32)((u32)StatusCurrentMinikitPc + 1u), packet, 0.0f);
                i32 count = packet->new_minikits > 0 ? StatusCurrentMinikitPc
                                                     : Game.area_save[packet->area->index].minikit_count;
                if (count < 0)
                    count = 0;
                DrawMiniKitCount(1.0f, 1.0f, count, packet->minikit_max);
                break;
            }
            case 3: {
                volatile f32 blend = 0.0f;
                if ((f64)stage->field_0x1c - StatusMiniKitJibberLengthPc < stage->field_0x18)
                    blend = ((f64)stage->field_0x18 - ((f64)stage->field_0x1c - StatusMiniKitJibberLengthPc)) /
                            StatusMiniKitJibberLengthPc;
                DrawStatusMiniKit(StatusMiniKitXPc, StatusMiniKitYPc, StatusAllMiniKitsZPc, StatusMiniKitScalePc,
                                  StatusMiniKitScalePc, (i32)((u32)StatusCurrentMinikitPc + 1u), packet, blend);
                i32 count = packet->new_minikits > 0 ? StatusCurrentMinikitPc
                                                     : Game.area_save[packet->area->index].minikit_count;
                if (blend >= 0.5f)
                    count = (i32)((u32)count + 1u);
                if (count < 0)
                    count = 0;
                volatile i32 angle = (i32)((f64)blend * 32768.0 + 16384.0);
                union {
                    f32 value;
                    u32 bits;
                } pulse;
                pulse.value = NuTrigTable[(angle >> 1) & 0x7fff];
                pulse.bits &= 0x7fffffffu;
                volatile f32 inverse = 1.0 - (f64)pulse.value;
                volatile f32 size = (f64)inverse * 0.25 + 1.0;
                DrawMiniKitCount(1.0f, size, count, packet->minikit_max);
                break;
            }
            case 4: {
                title_alpha = 1.0 - (f64)stage->field_0x18;
                volatile i32 angle =
                    (i32)((f64)StatusCoinsRatioPc(stage->field_0x18, stage->field_0x1c) * 32768.0 + 16384.0);
                volatile f32 blend = 1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5;
                volatile f32 scale =
                    ((f64)StatusAllMiniKitsScalePc - StatusMiniKitScalePc) * (f64)blend + StatusMiniKitScalePc;
                volatile f32 y = ((f64)StatusAllMiniKitsYPc - StatusMiniKitYPc) * (f64)blend + StatusMiniKitYPc;
                volatile f32 x = ((f64)StatusAllMiniKitsXPc - StatusMiniKitXPc) * (f64)blend + StatusMiniKitXPc;
                DrawStatusMiniKit(x, y, StatusAllMiniKitsZPc, scale, 1.0f,
                                  Game.area_save[packet->area->index].minikit_count, packet, 0.0f);
                volatile f32 off = stage->field_0x18 < 1.0f ? 1.0 - (f64)stage->field_0x18 : 0.0;
                volatile i32 count_angle = (i32)((f64)off * 32768.0 + 16384.0);
                volatile f32 count_blend = 1.0 - ((f64)NuTrigTable[(count_angle >> 1) & 0x7fff] + 1.0) * 0.5;
                DrawMiniKitCount(count_blend, 1.0f, Game.area_save[packet->area->index].minikit_count,
                                 packet->minikit_max);
                break;
            }
            case 6: {
                title_alpha = 1.0 - (f64)stage->field_0x18;
                volatile f32 off = stage->field_0x18 < 1.0f ? 1.0 - (f64)stage->field_0x18 : 0.0;
                if (stage->field_0x18 < stage->field_0x1c) {
                    volatile i32 angle = (i32)((f64)off * 32768.0 + 16384.0);
                    volatile f32 blend = 1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5;
                    DrawMiniKitCount(blend, 1.0f, Game.area_save[packet->area->index].minikit_count,
                                     packet->minikit_max);
                    DrawStatusMiniKit(StatusMiniKitXPc, StatusMiniKitYPc, StatusAllMiniKitsZPc, StatusMiniKitScalePc,
                                      1.0f, Game.area_save[packet->area->index].minikit_count, packet, 0.0f);
                }
                break;
            }
        }
        if (title_alpha < 0.0f)
            title_alpha = 0.0f;
        else if (title_alpha > 1.0f)
            title_alpha = 1.0f;
        volatile u8 opacity = (u8)((f64)title_alpha * 128.0);
        Text3DEx(TTab[TextIdPc8808d0], 0.0f, StatusCoinsTitleYPc, 1.0f, StatusCoinsTitleScalePc,
                 StatusCoinsTitleScalePc, StatusCoinsTitleScalePc, 0, 255, 255, 255, opacity);
        return;
    }
    if (stage->field_0x12 == 0)
        return;
    volatile f32 alpha = getFinishedStatusAlpha(packet);
    volatile i32 reveal_angle = (i32)((f64)alpha * 16384.0);
    volatile f32 reveal = NuTrigTable[(reveal_angle >> 1) & 0x7fff];
    volatile i32 opacity = (i32)((f64)alpha * 128.0);
    volatile f32 timer = GameTimer.time_elapsed_mod_seconds;
    if (timer > 0.25f)
        timer = 0.0f;
    volatile i32 pulse_angle = (i32)((f64)timer * 32768.0 + 16384.0);
    union {
        f32 value;
        u32 bits;
    } pulse;
    pulse.value = NuTrigTable[(pulse_angle >> 1) & 0x7fff];
    pulse.bits &= 0x7fffffffu;
    volatile f32 inverse = 1.0 - (f64)pulse.value;
    volatile f32 pulse_scale = 1.0 + (f64)inverse;
    if (Game.area_save[packet->area->index].minikit_count != 0) {
        if (alpha > 0.0f) {
            volatile f32 scale = (f64)StatusAllMiniKitsScalePc * (f64)reveal;
            DrawStatusMiniKit(StatusAllMiniKitsXPc, StatusAllMiniKitsYPc, StatusAllMiniKitsZPc, scale, 1.0f,
                              Game.area_save[packet->area->index].minikit_count, packet, 0.0f);
        }
    } else {
        volatile f32 size = (f64)StatusAllMiniKitsQuestionScalePc * (f64)pulse_scale;
        volatile f32 y = (f64)StatusAllMiniKitsQuestionYOffsetPc + StatusAllMiniKitsYPc;
        Text3DEx("?", StatusAllMiniKitsXPc, y, StatusAllMiniKitsZPc, size, size, size, 0, 255, 255, 255, opacity);
    }
    if (Game.area_save[packet->area->index].minikit_count == packet->minikit_max) {
        volatile f32 y = (f64)StatusAllMiniKitsCompleteYOffsetPc + StatusAllMiniKitsYPc;
        Text3DEx("$", StatusAllMiniKitsXPc, y, 1.0f, StatusAllMiniKitsCompleteScalePc, StatusAllMiniKitsCompleteScalePc,
                 StatusAllMiniKitsCompleteScalePc, 0, 255, 0, 127, opacity);
    } else {
        char text[60];
        sprintf(text, "%i/%i", Game.area_save[packet->area->index].minikit_count, packet->minikit_max);
        volatile f32 y = (f64)StatusAllMiniKitsCountYOffsetPc + StatusAllMiniKitsYPc;
        Text3DEx(text, StatusAllMiniKitsXPc, y, 1.0f, StatusAllMiniKitsCountScalePc, StatusAllMiniKitsCountScalePc,
                 StatusAllMiniKitsCountScalePc, 0, 255, 0, 127, opacity);
    }
}
