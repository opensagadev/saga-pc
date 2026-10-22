#include "saga.h"
#include <string.h>

extern "C" {
    extern i32 NuMainRendererControlPc;
    void __cdecl NuRndrShadowOnOff_candidate(i32);
#pragma data_seg(".data$BURNOUTON")
    i32 BURNOUTON = 1;
#pragma data_seg()
}
STATIC_ASSERT(sizeof(BURNOUTON) == 4);

typedef struct main_controller_world_view_pc_s {
    u8 unknown_0000[0x4670];
    AI_CONTROL_SYSTEM_PC *control_system;
} MAIN_CONTROLLER_WORLD_VIEW_PC;
STATIC_ASSERT(offsetof(MAIN_CONTROLLER_WORLD_VIEW_PC, control_system) == 0x4670);

extern "C" void __cdecl DrawSaveSlots(MENU *, f32);

struct FADEINFO_s;

STATIC_ASSERT(sizeof(FADEINFO_s) == 0x2c);
STATIC_ASSERT(offsetof(FADEINFO_s, direction) == 0x00);
STATIC_ASSERT(offsetof(FADEINFO_s, fade) == 0x04);
STATIC_ASSERT(offsetof(FADEINFO_s, rate) == 0x08);
STATIC_ASSERT(offsetof(FADEINFO_s, busy) == 0x0c);
STATIC_ASSERT(offsetof(FADEINFO_s, stage) == 0x10);
STATIC_ASSERT(offsetof(FADEINFO_s, fades) == 0x14);
STATIC_ASSERT(offsetof(FADEINFO_s, pending_type) == 0x24);
STATIC_ASSERT(offsetof(FADEINFO_s, field_28) == 0x28);

STATIC_ASSERT(sizeof(FadeSystem) == 0x2c);

void ClearPause(void);
void legoSetMusicVolume(f32 volume);

extern "C" {
    extern FADEINFO_PC *pFadeInfo;
    void __cdecl GizmoSysSetGame(void);
    void __cdecl ResetCharacterBuffer(i32 keep);
    void __cdecl WorldInfo_Activate(void);
    void __cdecl FindGameCutScenes(void);
    void __cdecl DebrisGlassInit(void);
    void __cdecl GamePlayMusic(LEVELDATA_PC *level, i32 start, OPTIONSSAVE *options);
    void __cdecl CheckResetBits(void);
    void __cdecl Movie_Play(char *name, VARIPTR *buffer, VARIPTR *end, f32 frame_time, void *callback, f32 volume);
    void __cdecl Areas_OpenAll(i32 mode);
    void __cdecl CutScenes_BGLoadManager(void);
    i32 __cdecl CutScene_PlayingOrRequested(void *cutscene);
    void __cdecl DrawCutBorders(i32 widescreen);
    void __cdecl HandleStillRender(void);
    void __cdecl PanelRender(WORLDINFO *world);
    void __cdecl ViewCamSetActive(i32 camera, i32 mode);
    void __cdecl StoreStatusTakeOverObjectSys(void);
    void __cdecl ReleaseAllTakeOvers(void);
    void __cdecl Mission_Clear(MISSIONSYS_s *mission_sys);
    void __cdecl RestoreGameCut(void);
    TIMER BonusTimer = {0};

    void __cdecl NuAllocDumpRecords(i32 min_size);
    void __cdecl WorldInfo_DrawScene(WORLDINFO *world);
    void __cdecl PortalDoors_Update(WORLDINFO *world);
    u8 __cdecl IsGrabbingScreen(void);
    void __cdecl GameDisplaySettings(LEVEL_DISPLAY_PC *display, i32 *colours);
    void __cdecl NuMtlAnimateSetSpeedScale(f32 scale);
    void __cdecl NuMtlAnimateSetMask(i32 mask);
    void __cdecl NuRndrGlobalFrameCountPause(i32 paused);
    void __cdecl SetTexAnimSignals(void);
    void __cdecl CharPlatforms_Update(CHARPLATFORMSYS_s *system);
    void __cdecl TrafficAnimSys_Update(struct TRAFFICANIMSYS_s *system);
    void __cdecl GizmoSysEarlyUpdate(void *system, void *world, f32 delta_time);
    void __cdecl GizmoSysLateUpdate(void *system, void *world, f32 delta_time);
    void __cdecl UpdateGameObjects(WORLDINFO *world);
    void __cdecl MoveGameCamera(GAMECAMERA *camera);
    void __cdecl GameObjectToCameraDistances(void);
    void __cdecl DrawGameObjects(void);
    i32 __cdecl DrawGameObjectsProcess(void);
    void __cdecl GameObjectStuffAfterAnimation(void);
    void __cdecl Faders_Draw(WORLDINFO *world);
    void __cdecl TrafficAnimSys_Draw(struct TRAFFICANIMSYS_s *system);
    void __cdecl GizmoSysDraw(void *system, void *world, f32 delta_time);
    void __cdecl NuLgtLaserDraw(i32 paused);
    void __cdecl SpeedBlur_Apply(WORLDINFO *world);

    u8 NuAllocDumpOnLevelPc = 0;
    i32 AreaGlobalsChallengeCountPc = 0;
    i32 NuMainSkipCutBordersPc = 0;
    i64 frameout_count = 0;
    extern i16 TextIdPc8810c0;
    char *Trailer[6] = {"demointro", (char *)&tINTRO, NULL, "indy", (char *)&TextIdPc8810c0, NULL};
}

/* PC splits NuMain's per-frame game update (0x491d50) and world render (0x4927f0) into helpers. */
static void NuMainUpdateFrame(WORLDINFO *world, i32 *frame_flag) {
    f32 water_speed;
    GAMEOBJECT **player;
    GAMEOBJECT *object;
    LEVELDATA_PC *level;
    FADETYPE fade_type;

    UpdateFrameCounters();
    PortalDoors_Update(world);
    NuRndrShadowOnOff_candidate(NuMainRendererControlPc);
    if (Paused == 0) {
        GameAISysStartFrame(world->ai_sys);
    }
    AddCoinDelay[1] = 0.0f;
    AddCoinDelay[0] = 0.0f;
    other_level_override = -1;
    Shadow_SetMode();
    GizmoPickups_SetOnOff();
    LOG_INFO_IF(GameTimer.update_count < 2, "NuMainUpdateFrame before DoInput: world=%p", (void *)WORLD);
    DoInput(WORLD);
    LOG_INFO_IF(GameTimer.update_count < 2, "NuMainUpdateFrame after DoInput: world=%p", (void *)WORLD);
    if (memcard_autosaveenabled != 0 && memcard_autosavedisabled != 0 &&
        (world == NULL || world->area == NULL || (world->area->flags & 2) == 0) && NewMode == 0 && NewLData == NULL &&
        FadeSys.fade == 0.0f && editor_active == 0 &&
        (GamePads_IgnoreInputFn == NULL || GamePads_IgnoreInputFn() == 0)) {
        if ((CUTSTOPGAME == 0 || CutScene_IsSkippable((CUTINFO *)CutStopInfo) != 0) && MiniCutCam == 0 &&
            CutSceneWaiting == 0) {
            if (GameMenu[GameMenuLevel].menu != -1) {
                if (MenuInMemoryCard() == 0 && MenuInfo[GameMenu[GameMenuLevel].menu].id != 1 &&
                    MenuInfo[GameMenu[GameMenuLevel].menu].id != 4) {
                    NewMenu(0x3f3, 0, -1);
                } else {
                    memcard_autosavedisabled = 0;
                    memcard_autosaveenabled = 0;
                }
            } else if (Paused == 0) {
                for (player = (GAMEOBJECT **)Player; player < (GAMEOBJECT **)&Player[2]; player++) {
                    if (*player != NULL && ((*player)->flags_1fc & 0x80) != 0) {
                        PauseGame((i32)((GAMEPAD_INIT_RECORD *)(*player)->pad - GamePadStorage));
                        break;
                    }
                }
            }
        }
    }
    STUBBED_NAMED("NuMainUpdateFrame: theGameThings->ProcessThings(&packet) via 0x0095e438");
    SAGA_SOUND_CHECK("before sound");
    LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: before sound world=%p", (void *)WORLD);
    if (NOSOUND == 0) {
        STUBBED_NAMED("NuMainUpdateFrame: NuSound3SetDPL(options, 0) at 0x00698820");
        NuSound3Update();
        UpdateLevelSfx(world, Paused);
        STUBBED_NAMED("NuMainUpdateFrame: NuSound3SetRumblePads, NuSound3UpdateRumble at 0x00698920, SoundUpdate at "
                      "0x00653460");
        ProcessMusicChanges(world->current_level, &Game.options_save);
    }
    SAGA_SOUND_CHECK("after sound");
    LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after sound world=%p", (void *)WORLD);
    if (Paused != 0 && screendump == 0 && !IsGrabbingScreen()) {
        NuMtlAnimateSetSpeedScale(0.0f);
        NuTexAnimSetMask(2);
        NuRndrGlobalFrameCountPause(1);
        pNuCam->mtx = GameCam->render_mtx;
        NuCameraSet(pNuCam);
        UpdateGameMenu((GAMEPAD_PC *)GamePadStorage, 1);
        for (player = (GAMEOBJECT **)Player; player < (GAMEOBJECT **)&Player[2]; player++) {
            object = *player;
            if (object != NULL) {
                UpdateCoinPacket((PLAYER_COIN_PACKET_PC *)object->coin_packet, (object->flags_1fc >> 7) & 1,
                                 (i8)object->field_280);
            }
        }
        Debris(1);
    } else {
        NuRndrGlobalFrameCountPause(0);
        rtlFrameUpdate(FRAMETIME);
        DoubleScore = 0;
        SetTexAnimSignals();
        NuTexAnimSetMask(0xffff);
        CutScenes_Start(world);
        CutScenes_Update(world, Paused != 0 && !IsGrabbingScreen());
        SAGA_SOUND_CHECK("after CutScenes_Update");
        LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after CutScenes_Update world=%p", (void *)WORLD);
        AdaptiveDifficultyUpdatePc(&adaptivedifficulty);
        if ((world->current_level->flags & 8) != 0) {
            TerrainPlatformOldUpdate();
        }
        LOG_INFO_IF(GameTimer.update_count < 2, "ProcessGizFlow before: world=%p", (void *)WORLD);
        SAGA_SOUND_CHECK("before ProcessGizFlow");
        ProcessGizFlow(world->giz_flow, FRAMETIME);
        SAGA_SOUND_CHECK("after ProcessGizFlow");
        LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after ProcessGizFlow world=%p", (void *)WORLD);
        if (((MAIN_CONTROLLER_WORLD_VIEW_PC *)world)->control_system != NULL)
            AIPathCnxControlSysUpdate(((MAIN_CONTROLLER_WORLD_VIEW_PC *)world)->control_system);
        if (world->current_gscn != NULL) {
            NuGScnUpdate(world->current_gscn, FRAMETIME * 60.0f);
        }
        GameAnimSys_Update(world->game_anim_sys);
        SAGA_SOUND_CHECK("after GameAnimSys_Update");
        LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after GameAnimSys_Update world=%p", (void *)WORLD);
        edanimUpdateObjects(1.0f);
        NuBridgeUpdate(&plr_lastpos);
        if (Grass_Available != 0) {
            STUBBED_NAMED("NuMainUpdateFrame: NuWindUpdateArray at 0x00546380, NuFadeObjUpdateArray at 0x00545a80");
        }
        rtlProcessLights(world->rtl_set, FRAMETIME);
        CharPlatforms_Update((CHARPLATFORMSYS_s *)world->shadow_platform_list_pc);
        STUBBED_NAMED("NuMainUpdateFrame: Grabber_Update(world) at 0x00471650");
        TrafficAnimSys_Update(world->trafficanim_sys);
        SAGA_SOUND_CHECK("after TrafficAnimSys_Update");
        LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after TrafficAnimSys_Update world=%p", (void *)WORLD);
        GizmoSysEarlyUpdate(world->gizmo_sys, world, FRAMETIME);
        SAGA_SOUND_CHECK("after GizmoSysEarlyUpdate");
        LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after GizmoSysEarlyUpdate world=%p", (void *)WORLD);
        if ((world->current_level->flags & 8) != 0) {
            TerrainPlatformNewUpdate();
        }
        UpdateSpecialSfx(world);
        Teleports_UpdateBeforeGameObjects(world);
        if (CUTSTOPGAME == 0) {
            SAGA_SOUND_CHECK("before ManageGameObjects");
            LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: before ManageGameObjects world=%p", (void *)WORLD);
            ManageGameObjects();
            SAGA_SOUND_CHECK("after ManageGameObjects");
            LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after ManageGameObjects world=%p", (void *)WORLD);
            UpdateGameObjects(world);
            SAGA_SOUND_CHECK("after UpdateGameObjects");
            LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after UpdateGameObjects world=%p", (void *)WORLD);
        } else {
            for (player = (GAMEOBJECT **)Player; player < (GAMEOBJECT **)&Player[8]; player++) {
                object = *player;
                if (object != NULL && (object->flags_1fc & 0x80) != 0 && object->pad->bound_pad != NULL) {
                    UpdateRumble((RUMBLE_PACKET_RESOURCE_PC *)object->pad->unknown_40);
                }
            }
        }
        ShoveObjectSysReset();
        if (CUTSTOPGAME == 0) {
            if (Cheat_IsOn(0x2c) == 0) {
                adaptivedifficulty.accumulator = 0.0f;
            }
            Teleports_UpdateAfterGameObjects(world);
            Pulses_Update((LEVEL_SFX_PULSE_PC *)world->pulses_sys);
            STUBBED_NAMED("NuMainUpdateFrame: Detonators_Update at 0x00426fa0");
            Batarangs_Update();
            STUBBED_NAMED("NuMainUpdateFrame: UpdateCables at 0x00408af0");
            SAGA_SOUND_CHECK("before Level_Update");
            LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: before Level_Update world=%p", (void *)WORLD);
            Level_Update(world);
            SAGA_SOUND_CHECK("after Level_Update");
            LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after Level_Update world=%p", (void *)WORLD);
            Bolts_Update(world);
            UpdateParts(FRAMETIME);
            SAGA_SOUND_CHECK("after UpdateParts");
            LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after UpdateParts world=%p", (void *)WORLD);
            if (KillPartsBrickImpactWaitPc > 0.0f) {
                KillPartsBrickImpactWaitPc -= FRAMETIME;
            }
            if (coinimpactwait > 0.0f) {
                coinimpactwait -= FRAMETIME;
            }
            if (SaberSparksTimer > 0.0f) {
                SaberSparksTimer -= FRAMETIME;
            }
            if (TerrainChatterWaitPc > 0.0f) {
                TerrainChatterWaitPc -= FRAMETIME;
            }
            if (tieonsfxwait > 0.0f) {
                tieonsfxwait -= FRAMETIME;
            }
            if (tieoffsfxwait > 0.0f) {
                tieoffsfxwait -= FRAMETIME;
            }
            UpdateExplosions();
            Tag_UpdateTransfers(world->game_debris_sys->entries[39].effect, world->game_debris_sys->entries[40].effect,
                                180);
            UpdateRepeatSfx();
            GizmoSysLateUpdate(world->gizmo_sys, world, FRAMETIME);
            SAGA_SOUND_CHECK("after GizmoSysLateUpdate");
            STUBBED_NAMED("NuMainUpdateFrame: UpdateRadios at 0x004bcb10, Hint_Process at 0x005769d0");
            Cheats_FrameUpdate();
        }
        UpdateGameMenu((GAMEPAD_PC *)GamePadStorage, 1);
        Debris(0);
        if (CUTSTOPGAME == 0) {
            STUBBED_NAMED("NuMainUpdateFrame: DebrisKillPlayers at 0x0042f4a0");
        }
        UpdateRippleSet(RippleSetPc);
        STUBBED_NAMED("NuMainUpdateFrame: AddCameraRain(world, 0x7a) at 0x005a82c0");
        NuMtlAnimateSetSpeedScale(1.0f);
        NuMtlAnimateSetMask(0xffff);
        SAGA_SOUND_CHECK("before MoveGameCamera");
        LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: before MoveGameCamera world=%p", (void *)WORLD);
        MoveGameCamera(GameCam);
        SAGA_SOUND_CHECK("after MoveGameCamera");
        LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after MoveGameCamera world=%p", (void *)WORLD);
        GameObjectToCameraDistances();
        SetCameraMatrices();
        if (CUTSTOPGAME == 0) {
            UpdateStreaks(FRAMETIME);
        }
        if ((world->area != NULL && world->area == HUB_ADATA) || world->current_level == TITLES_LDATA ||
            world->current_level == STATUS_LDATA || (world->current_level->flags & 0x400) != 0 ||
            world->current_level == CREDITS_LDATA) {
            BackDrop_Update(FRAMETIME);
        }
    }
    if (world->current_level->callbacks[4] != NULL) {
        ((void(__cdecl *)(WORLDINFO *))world->current_level->callbacks[4])(world);
    }
    LevelStreaming_Update(world);
    UpdateCutBorders();
    if (Paused != 0 && editor_active == 0)
        water_speed = 0.0f;
    else
        water_speed = (f32)((f64)FRAMETIME / (f64)(f32)(1.0f / 60.0f));
    NuWaterSpeed(water_speed);
    NuWaterReset();
    GameTiming(world, &Game.game_time);
    UpdateStats();
    *frame_flag = 0;
    GameDisplaySettings(&world->current_level->display, back_rgba);
    if ((NewMode == 0 && NewLData == NULL) || FadeSys.fade != 0.0f) {
        return;
    }
    level = world->current_level;
    if (level != NULL && (level->flags & 0x400) != 0 && HUB_LDATA != NULL && NewLData != HUB_LDATA &&
        NewLData != NULL && NewLData->area_index != -1 && (ADataList[NewLData->area_index].flags & 2) == 0) {
        FadeSys.fade = 1.0f;
        FinishLoop_On = 0;
        return;
    }
    if (FadeSys.pending_type == FADE_TYPE_NONE ||
        (NewLData != NULL && WORLD->current_level != NewLData && NewLData == HUB_LDATA)) {
        level = WORLD->current_level;
        if (level != NULL) {
            if (NewLData == NULL) {
                goto fade_still;
            }
            if (NewLData != level) {
                if (NewLData == HUB_LDATA || level == HUB_LDATA || NewLData == TITLES_LDATA || level == TITLES_LDATA ||
                    NewLData == CREDITS_LDATA) {
                    goto fade_wipe;
                }
                if (SuperStory != 0 && (level->flags & 0x400) != 0 &&
                    (ADataList[NewLData->area_index].flags & 2) != 0) {
                    goto fade_wipe;
                }
            }
        }
        if (NewLData == NULL || NewLData->area_index == WORLD->area_index) {
            goto fade_still;
        }
        if (SuperStory != 0 && (WORLD->area->flags & 2) == 0) {
            goto fade_still;
        }
    fade_wipe:
        fade_type.type = FADE_TYPE_WIPE;
        FadeSys.SetFade(fade_type, 0);
        goto set_stage;
    fade_still:
        fade_type.type = FADE_TYPE_STILL_WIPE;
        FadeSys.SetFade(fade_type, 0);
    }
set_stage:
    FadeSys.SetStage(2);
}

static void NuMainRenderWorld(WORLDINFO *world) {
    i32 paused;

    STUBBED_NAMED("NuMainRenderWorld: GameFog_Update(world) at 0x00564cd0, GameFog_Set at 0x00401820");
    SetLevelLights(world->rtl_set, 1.0f);
    STUBBED_NAMED("NuMainRenderWorld: SetSpotLightMode at 0x00596960");
    if (CUTSTOPGAME == 0) {
        DrawGameObjectsProcess();
    }
    SetShadowLight();
    NuRndrBeginScene(1);
    STUBBED_NAMED("NuMainRenderWorld: SpeedBlur_Update at 0x0042df70");
    if (back_rgba[0] == back_rgba[1]) {
        NuRndrClear(0x1f00, back_rgba[0], 1.0f);
    } else {
        NuRndrGradClear(0xf00, back_rgba[0], back_rgba[1], 1.0f);
    }
    if (CUTSTOPGAME == 0) {
        EnableShadowMapRendering(0);
        DrawParts(Paused);
        ResetShadowMapRendering();
        if (CUTSTOPGAME == 0) {
            DrawGameObjects();
        }
    }
    STUBBED_NAMED("NuMainRenderWorld: Grabber_Draw(world) at 0x00470940");
    SetLevelLights(world->rtl_set, 1.0f);
    if (editor_active == 0 && Paused == 0 && CUTSTOPGAME == 0) {
        CharShadows_Update();
        GameObjectStuffAfterAnimation();
    }
    if (CUTSTOPGAME == 0) {
        CharShadows_Draw();
    }
    paused = screendump != 0 ? save_paused : Paused;
    if (IsGrabbingScreen()) {
        paused = 0;
    }
    if (world->level_objects[1].active != 0 && (CUTSTOPGAME == 0 || CUTDRAWWORLD != 0)) {
        DrawParallax(&world->level_objects[1].special);
    }
    if (CUTSTOPGAME == 0) {
        WorldInfo_DrawScene(world);
        Level_Draw(world);
        Faders_Draw(world);
        DrawStreaks();
        Bolts_Draw(world);
        DrawExplosions();
        ZipUps_DrawLines();
        STUBBED_NAMED("NuMainRenderWorld: DrawCables at 0x00409980, Detonators_Draw at 0x00427070");
        Batarangs_Draw();
        TrafficAnimSys_Draw(world->trafficanim_sys);
        GizmoSysDraw(world->gizmo_sys, world, FRAMETIME);
        SpecialMiniKits_Draw(world);
        NuBridgeDraw(NULL);
    }
    if (Grass_Available != 0) {
        STUBBED_NAMED("NuMainRenderWorld: NuWindDraw at 0x00546e60, NuFadeObjDraw at 0x00545c80");
    }
    STUBBED_NAMED("NuMainRenderWorld: theGameThings->RenderThings(NULL) via 0x0095e438");
    CutScenes_Draw(world);
    NuRndrShadPolys(PermanentShadowMaterialPc);
    NuLgtLaserDraw(paused);
    if (world->current_level == TITLES_LDATA || (world->current_level->flags & 0x400) != 0 ||
        world->current_level == CREDITS_LDATA ||
        (world->current_level == STATUS_LDATA && (StatusPacket.flags_word & 0x400) != 0)) {
        BackDrop_Draw(1.0f, 0);
    }
    if (MainRenderTime != 1.0f) {
        BackDrop_Draw(1.0f - MainRenderTime, 0);
    }
    STUBBED_NAMED("NuMainRenderWorld: NuRndrLine3dDbgFlush at 0x005fecb0");
    DebrisSetCutSceneMode(CUTCAM);
    DebrisDraw(paused ^ editor_active, 1);
    DrawRippleSet(RippleSetPc);
    SpeedBlur_Apply(world);
    NuRndrEndScene();
    RenderShadowLights(Paused);
    NuRndrBeginScene(1);
    STUBBED_NAMED("NuMainRenderWorld: 0x00596520, SetDepthOfField at 0x004017b0");
    if (BURNOUTON != 0)
        BurnoutApply(paused);
    STUBBED_NAMED("NuMainRenderWorld: 0x00491bd0(world->current_gscn) when 0x0087b580 is set");
    NuRndrEndScene();
    NuRndrFx(paused, &plr_lastpos);
    NuRndrBeginScene(1);
    UpdateDebrisRenderStackPriority();
    SortDebrisRenderStack();
    DebrisDraw(paused ^ editor_active, 1);
    DebrisDraw(paused ^ editor_active, 0);
    NuRndrEndSceneEx(0);
    STUBBED_NAMED("NuMainRenderWorld: DebrisDrawGlass at 0x006868f0");
    if (screendump == 0) {
        STUBBED_NAMED("NuMainRenderWorld: ViewCamDraw at 0x0040aa80");
    }
}

typedef struct NU_MAIN_STATUS_PACKET_PC {
    u8 unknown_00[0x9c];
    u16 player_models[2];
    u8 unknown_a0[4];
    u8 player_active[2];
    u8 unknown_a6[0xe];
    u8 status_flags;
} NU_MAIN_STATUS_PACKET_PC;
STATIC_ASSERT(offsetof(NU_MAIN_STATUS_PACKET_PC, player_models) == 0x9c);
STATIC_ASSERT(offsetof(NU_MAIN_STATUS_PACKET_PC, player_active) == 0xa4);
STATIC_ASSERT(offsetof(NU_MAIN_STATUS_PACKET_PC, status_flags) == 0xb4);
STATIC_ASSERT(offsetof(PLAYERPROGRESS, field_7) == 7);
STATIC_ASSERT(offsetof(PLAYERPROGRESS, coins) == 0);

extern "C" {
    u8 StatusPlayerActivePc[8] = {0};
    void __cdecl InitStatusScreen(WORLDINFO *);
    void __cdecl NuMainStatusHelperPc(STATUSPACKET_s *);
    void __cdecl MusicClearAll(void);
    void __cdecl SoundKillAll(void);
    void __cdecl ClearUpAreaData(void);
    void __cdecl WorldInfo_ClearAllIfScreenFaded(void);
    i32 __cdecl MakePlayerList(i32);
}

i32 __cdecl Game_Exit(i32);

extern "C" int __cdecl NuMain(int argc, char **argv) {
    WORLDINFO *saved_world;
    GIZAIMESSAGE *message;
    FADETYPE fade_type;
    i32 current_episode;
    i32 previous_episode;
    i32 frame_flag;
    i32 vbcnt;
    i64 previous_vbcnt;
    f32 saved_frametime;
    i32 player_index;
    NU_MAIN_STATUS_PACKET_PC *status = (NU_MAIN_STATUS_PACKET_PC *)&StatusPacket;

    LOG_INFO("Game startup: argc=%d", argc);
    NuCommandLine(&argc, &argv);
    WorldInfo_InitOnce();
    InitOnce(argc, argv);

    MenuInitialise(GameMenuInfo, 0x17, LANGUAGECOUNT, DrawSaveSlots, 1, 0);
    MenuReset();
    edGraInitTerrainSwapProtection();

    NuDatHandles.previous = NuDatGet();
    NuDatHandles.episode[0] = NuDatOpen(EpisodeIDat, &permbuffer_base, NULL);
    NuDatHandles.episode[1] = NuDatOpen(EpisodeIIDat, &permbuffer_base, NULL);
    NuDatHandles.episode[2] = NuDatOpen(EpisodeIIIDat, &permbuffer_base, NULL);
    NuDatHandles.episode[3] = NuDatOpen(EpisodeIVDat, &permbuffer_base, NULL);
    NuDatHandles.episode[4] = NuDatOpen(EpisodeVDat, &permbuffer_base, NULL);
    NuDatHandles.episode[5] = NuDatOpen(EpisodeVIDat, &permbuffer_base, NULL);
    NuDatSet(NuDatHandles.previous);
    NuHGobjReversibleCharacters(1);
    NuHGobjForceShadowsOnCharacters(1);
    GameAISysInit();
    APITransparentInit();
    NuSound3SetLoopHoldTime(0.5f);
    InitShadowLights();
    GamePads_Init();
    bgProcInit();
    if (nu_input_state != NULL) {
        FinishLoop(2);
    } else {
        STUBBED_NAMED("NuMain: FinishLoop waits for PC input-state setup in InitOnce");
    }
    Text_InitLanguageList(Game_LanguageList);
    Text_SetLanguage(-1);
    // LoadPerm calls NuFrameBegin, which needs the still-unreconstructed input owner.
    if (nu_input_state == NULL) {
        STUBBED_NAMED("NuMain: PC input-state setup before LoadPerm");
        return 0;
    }
    StartPerm();
    LoadPerm();
    EndPerm();

    if (GAMEDEMO != 0) {
        f32 volume = (f32)(Game.options_save.music_volume / 10.0);
        saved_world = WORLD;
        WORLD = NULL;
        PLAYERCOUNT = 0;
        Movie_Play(Trailer[0], &characterbuffer_ptr, &characterbuffer_end, DEFAULTFRAMETIME, NULL, volume);
        FinishLoop(2);
        NuMainTrailerLeafPc();
        WORLD = saved_world;
    }
    if (openlevels != 0) {
        Areas_OpenAll(0);
    }
    if (Level == -1) {
        Level = 0;
    }

    // GAME.DAT contains the level and area lists, but their PC parser path is
    // still stubbed, so stop before indexing absent table data.
    if (LDataList == NULL || ADataList == NULL) {
        STUBBED_NAMED("NuMain: GAME.DAT level and area tables");
        return 0;
    }

    Area = LDataList[Level].area_index;
    last_area = -1;
    LastAData = NULL;
    if (Area >= 0 && (ADataList[Area].flags & 4) != 0) {
        NextArea_FreePlay = 1;
        FreePlay = 1;
    } else if (GAMEDEMO > 1 || (GAMEDEMO != 0 && FreePlay != 0)) {
        FreePlay = 1;
        GAMEDEMO = 2;
    }
    makefreeplaymodellist = 0;
    if (StartupGameFlag1 != 0 && HUB_ADATA != NULL && Area == HUB_ADATA->index) {
        Hub_MakeModelList();
    } else if (FreePlay != 0) {
        NextArea_FreePlay = 0;
        FreePlay = 0;
    }

    NuFrameBegin();
    edGraEnableTerrainSwap();
    NuFrameEnd();
    edGraDisableTerrainSwap();

    for (;;) {
        if (StartupGameFlag1 != 0) {
            LSW_SetIndy(Area);
        }
        if (last_area != -1 && LastLData != NULL && (LastLData->flags & 0x400) != 0 && ADataList[Area].flags != 2 &&
            HUB_ADATA != NULL && Area != HUB_ADATA->index) {
            FinishLoop(2);
        }
        GameAISysSetGame();
        GizmoSysSetGame();
        NuAllocPeakEnd0 = NULL;
        current_episode = Area != -1 ? ADataList[Area].episode_index : -1;
        previous_episode = last_area != -1 ? ADataList[last_area].episode_index : -1;
        if (Area == -1 || current_episode == -1 || (previous_episode != -1 && current_episode != previous_episode)) {
            SuperStory = 0;
        }
        if (NuAllocDumpOnLevelPc != 0) {
            NuAllocDumpRecords(0);
        }
        if (Area != -1 && Area != last_area) {
            if (gizaimessagesys != NULL) {
                ResetGizAIMessageSys(gizaimessagesys);
            }
        } else if (reset_area != 0 && gizaimessagesys != NULL) {
            ClearGizAIMessageSys(gizaimessagesys);
        }

        message = CheckGizAIMessage(gizaimessagesys, "FreePlay", NULL);
        if (message != NULL) {
            message->value = (f32)FreePlay;
            message->flags |= 1;
            message->output_values[0] = 1;
            message->output_values[1] = 0;
            message->output_count = 2;
        }
        if (StartupGameFlag1 != 0) {
            message = CheckGizAIMessage(gizaimessagesys, "MissionMode", NULL);
            if (message != NULL) {
                if (MissionSys != NULL && MissionSys->state != 0) {
                    message->value = 1.0f;
                }
                message->flags |= 1;
                message->output_values[0] = 1;
                message->output_values[1] = 0;
                message->output_count = 2;
            }
            message = CheckGizAIMessage(gizaimessagesys, "ChallengeMode", NULL);
            if (message != NULL) {
                if (ChallengeMode != 0) {
                    message->value = 1.0f;
                }
                message->flags |= 1;
                message->output_values[0] = 1;
                message->output_values[1] = 0;
                message->output_count = 2;
            }
        }
        if (Area == -1 || ADataList[Area].episode_index == -1 || (ADataList[Area].flags & 0x10) == 0 ||
            (ADataList[Area].flags & 4) != 0) {
            ChallengeMode = 0;
            Mission_Clear(NULL);
        }

        ResetCharacterBuffer(0);
        memset(Player, 0, sizeof(Player));
        if (PlayTrailer != -1 && characterbuffer_ptr.addr == characterbuffer_base.addr) {
            Movie_Play(Trailer[PlayTrailer * 3], &characterbuffer_ptr, &characterbuffer_end, DEFAULTFRAMETIME, NULL,
                       (f32)Game.options_save.music_volume / 10.0);
        }
        PlayTrailer = -1;
        LoadAreaCharacters();
        FixUpLayers();
        if (loadareacharacters_loadedlevel == 0) {
            WorldInfo_ReArrangeBuffers(Area, last_area);
        }
        if (level_already_loaded == -1 || level_already_loaded != Level) {
            next_level = Level;
            abort_load = 0;
            WorldInfo_StreamLevel(NULL);
        }
        level_already_loaded = -1;
        ClearLevData();
        dagobah_training = 0;
        party_cant_be_under_cover = 0;
        WorldInfo_Activate();
        FindGameCutScenes();
        DebrisGlassInit();
        InitCables(WORLD);
        InitSnakes(WORLD);
        ResetOldFStop = 1;
        music_man.SetFader(1.0f, 0.0f);
        new_level_from_menu = 0;
        NuSound3SetReverb(0);
        if ((Area != -1 && Area != last_area) || reset_area != 0) {
            NewArea();
        }
        GamePlayMusic(WORLD->current_level, 0, &Game.options_save);
        if (WORLD->current_level == TITLES_LDATA && Game.options_save.music_enabled == 0) {
            legoSetMusicVolume(0.0f);
        }
        reset_area = 0;
        reset_restart = 1;
        no_more_loads = 0;
        ResetTimer(&BonusTimer, 0.0f);
        BonusWinner = -1;
        BonusCoinTotal = 0;
        ResetFrameCounters();
        if (NewMode == 0) {
            reset_load = 1;
        }

        for (;;) {
            Particles_Start(WORLD);
            if (WORLD->page_grass != -1) {
                edgraStartPage((u8)WORLD->page_grass);
            }
            Parts_Start(WORLD);
            if (NOSOUND == 0) {
                NuSound3Update();
            }
            ClearPause();
            NuPause(0);
            if (NOSOUND == 0) {
                NuSound3Update();
            }
            if (FadeSys.pending_type == FADE_TYPE_NONE) {
                fade_type.type = FADE_TYPE_STILL_WIPE;
                FadeSys.SetFade(fade_type, 0);
            }
            FadeSys.SetStage(1);
            MainRenderTargetTime = 1.0f;
            MainRenderTime = 1.0f;
            LOG_INFO("NuMain before CheckResetBits: world=%p", (void *)WORLD);
            CheckResetBits();
            LOG_INFO("NuMain after CheckResetBits: world=%p", (void *)WORLD);
            music_man.SetFader(1.0f, 0.0f);
            GamePlayMusic(WORLD->current_level, 1, &Game.options_save);
            if (WORLD->current_level == TITLES_LDATA && Game.options_save.music_enabled == 0) {
                legoSetMusicVolume(0.0f);
            }
            if (newlevel_resumecutaudio != 0) {
                RestoreGameCut();
                music_man.ResumeTrack(0x10);
                NuSound3Update();
                newlevel_resumecutaudio = 0;
            }
            frame_flag = 0;
            NuRndrSwapStreamBuffers();
            (void)nuvideo_global_vbcnt;
            (void)nuvideo_global_vbcnt;
            (void)nuvideo_global_vbcnt;
            FRAMETIME = DEFAULTFRAMETIME;
            frameout_count = 0;
            frameout = 0;
            if (reset_load != 0) {
                other_level = -1;
                waiting_for_level = -1;
                gone_through_door_to_new_level = 0;
                waiting_for_new_level = 0;
            }
            reset_load = 1;
            NuVideoRollingFrameRateReset();

            while (
                ((NewMode == 0 && NewLData == NULL) || FadeSys.fade < 1.0f || pFadeInfo->busy != 0 ||
                 (((waiting_for_level != -1 && (reset_load != 0 || abort_load != 0)) || waiting_for_character != -1) &&
                  NewMode == 0)) &&
                (CutScenes_BGLoadManager(), NuWindowsProcessMessages() != 0)) {
                NuVideoGetSwapMode();
                NuTimeBarSlotReset(0, 0xf);
                _NuTimeBarSlotBegin(0, 0xf, "frmtmr");
                DebrisSetTimeIncrement(FRAMETIME);
                InitPanel(GetMenuID() == 4 ? TempOptions.widescreen : Game.options_save.widescreen);
                saved_frametime = FRAMETIME;
                if ((waiting_for_level == -1 || gone_through_door_to_new_level == 0) && screendump == 0) {
                    if (ChallengeMode == 1 && AreaGlobalsChallengeCountPc >= 10 && WORLD->current_level != HUB_LDATA &&
                        MiniCutCam == 0 && CutScene_PlayingOrRequested(NULL) == 0 && FadeSys.fade == 0.0f) {
                        EndChallenge(2, 1);
                    }
                    LOG_INFO_IF(GameTimer.update_count < 2, "NuMain before update: world=%p", (void *)WORLD);
                    SAGA_SOUND_CHECK("before frame update");
                    NuMainUpdateFrame(WORLD, &frame_flag);
                    SAGA_SOUND_CHECK("after frame update");
                    LOG_INFO_IF(GameTimer.update_count < 2, "NuMain after update: world=%p", (void *)WORLD);
                }
                if (waiting_for_level != -1 && screendump == 0) {
                    WaitingForLevelTime += FRAMETIME;
                }
                if (waiting_for_character != -1 && screendump == 0) {
                    WaitingForCharacterTime += FRAMETIME;
                }
                FRAMETIME = saved_frametime;
                WidescreenCode(GetMenuID() == 4 ? TempOptions.widescreen : Game.options_save.widescreen);
                UpdateBackgroundMusic();
                NuFrameBegin();
                if (SoundVoiceEnabledGate != 0) {
                    if (pause_rndr_on == 0 || (FadeSys.pending_type == FADE_TYPE_WIPE && MainRenderTime != 0.0f)) {
                        LOG_INFO_IF(GameTimer.update_count < 2, "NuMain before render: world=%p", (void *)WORLD);
                        SAGA_SOUND_CHECK("before world render");
                        NuMainRenderWorld(WORLD);
                        SAGA_SOUND_CHECK("after world render");
                        LOG_INFO_IF(GameTimer.update_count < 2, "NuMain after render: world=%p", (void *)WORLD);
                    } else {
                        NoRender();
                    }
                }
                if (NuMainSkipCutBordersPc == 0) {
                    DrawCutBorders(Game.options_save.widescreen);
                }
                if (MainRenderTime != 1.0f) {
                    clear_screen_onstill = 0;
                    GrabStillScreen();
                }
                HandleStillRender();
                if (HubMainRenderTimeHack != 0) {
                    HubMainRenderTimeHack = 0;
                    MainRenderTargetTime = 1.0f;
                    MainRenderTime = 1.0f;
                }
                if (SoundVoiceEnabledGate != 0) {
                    PanelRender(WORLD);
                }
                CutScenes_End();
                vbcnt = nuvideo_global_vbcnt;
                previous_vbcnt = frameout_count;
                frameout_count = vbcnt;
                frameout = vbcnt - previous_vbcnt - 1 > 0 ? (i32)(vbcnt - previous_vbcnt - 1) : 0;
                if (nurndr_tritot_this_frame > peak_poly_count) {
                    peak_poly_count = nurndr_tritot_this_frame;
                }
                poly_count = nurndr_tritot_this_frame;
                edGraEnableTerrainSwap();
                FRAMETIME = NuFrameEnd();
                edGraDisableTerrainSwap();
                if (FRAMETIME > MAXFRAMETIME && CUTSTOPGAME == 0) {
                    FRAMETIME = MAXFRAMETIME;
                }
                if (frameout > 2) {
                    frameout = 2;
                }
                if (frame_flag != 0) {
                    frame_flag = 0;
                }
                if (screendump != 0) {
                    if (screendump == 1) {
                        Paused = save_paused;
                    }
                    screendump--;
                }
            }

            NuVideoRollingFrameRateReset();
            ClearPause();
            NuPause(0);
            ViewCamSetActive(0, 0);
            if (NewMode == 0 && PcWindowQuit == 0) {
                FinishLoop(2);
            }
            FinishLoop_On = 1;
            NuSound3StopRumble();
            PCAnimationEffectCheckpoint();
            if (WORLD->page_grass != -1) {
                edgraStopPage((i8)WORLD->page_grass);
            }
            Parts_Stop(WORLD);
            Particles_Stop(WORLD);
            CutScenes_Stop(WORLD->cutscene_sys);
            if (NOSOUND == 0) {
                NuSound3FlushLoops();
                NuSound3Update();
            }
            StoreLevelProgress(WORLD);
            if (netclient == 0) {
                StoreStatusTakeOverObjectSys();
            }
            ReleaseAllTakeOvers();
            if (NewMode == 0 || PcWindowQuit != 0) {
                break;
            }
            reset_load = 0;
            LevelChange = 0;
        }

        reset_load = 1;
        if (new_level_from_menu != 0) {
            ClearAreaProgress(WORLD->area_index, 1);
        }
        LastLData = WORLD->current_level;
        if (PlayTrailer != -1) {
            last_area = -1;
        } else {
            last_area = Area;
            LastAData = &ADataList[Area];
        }
        if (LastLData == CREDITS_LDATA) {
            saved_frametime = Game_OptionsSave != NULL ? GameSetMusicVolume(Game_OptionsSave) : 1.0f;
            legoSetMusicVolume(saved_frametime);
        }
        Area = NewLData->area_index;
        Level = NewLData->index;
        if (Area == last_area && Area != -1) {
            ++LevelChangesInArea;
        } else {
            LevelChangesInArea = 0;
        }
        LevelChange = NewLData->index != LastLData->index;
        if (Area != last_area || new_level_from_menu != 0) {
            if (NOSOUND == 0) {
                music_man.StopAll(0);
                MusicClearAll();
                SoundKillAll();
            }
            if ((HUB_ADATA == NULL || last_area != (u8)HUB_ADATA->index) && Arcade != 0) {
                if (HUB_ADATA != NULL && Area == (u8)HUB_ADATA->index) {
                    hub_from_arcade = last_area;
                }
                Arcade = 0;
            }
        }
        if (NewLData == STATUS_LDATA || (NewLData->flags & 0x400) != 0) {
            InitStatusScreen(WORLD);
            if (NewLData != STATUS_LDATA && status->status_flags != 0) {
                NuMainStatusHelperPc(&StatusPacket);
            }
        }
        if (netclient != 0) {
            for (player_index = 0; player_index < 8; ++player_index) {
                PlayerProgress[player_index].active = StatusPlayerActivePc[player_index];
            }
            UsePlayerList = 2;
        } else if (Area != -1 && Area == last_area) {
            MakePlayerList(8);
            UsePlayerList = new_level_from_menu != 0 ? 2 : 1;
        } else {
            if (PLAYERCOUNT > 0) {
                UsePlayerList = 2;
            }
            MakePlayerList(8);
            Door_UseCutCam = 0;
        }
        if (Area == -1) {
            NextArea_FreePlay = 0;
            FreePlay = 0;
        } else {
            if ((ADataList[Area].flags & 4) != 0) {
                NextArea_FreePlay = 1;
                FreePlay = 1;
            }
            if (new_level_from_menu != 0 && Area != last_area) {
                if (HUB_ADATA != NULL && Area == (u8)HUB_ADATA->index) {
                    NextArea_FreePlay = 0;
                }
                FreePlay = NextArea_FreePlay;
                if (FreePlay != 0) {
                    if (Player[0] != NULL) {
                        ((GAMEOBJECT *)Player[0])->id = -1;
                    }
                    if (Player[1] != NULL) {
                        ((GAMEOBJECT *)Player[1])->id = -1;
                    }
                    makefreeplaymodellist = 1;
                }
            }
        }
        if (status->status_flags != 0 &&
            ((STATUS_LDATA != NULL && LastLData == STATUS_LDATA) || (LastLData->flags & 0x400) != 0 ||
             (CREDITS_LDATA != NULL && LastLData == CREDITS_LDATA))) {
            for (player_index = 0; player_index < 8; ++player_index) {
                if (player_index < 2) {
                    PlayerList[player_index] = status->player_models[player_index];
                    PlayerProgress[player_index].active = status->player_active[player_index];
                } else {
                    PlayerList[player_index] = -1;
                    PlayerProgress[player_index].active = 0;
                }
                PlayerProgress[player_index].field_7 = 0;
                PlayerProgress[player_index].coins = 0;
            }
            UsePlayerList = HUB_ADATA != NULL && Area == (u8)HUB_ADATA->index && LastLData != TITLES_LDATA ? 1 : 2;
        }
        if (HUB_ADATA != NULL && Area == (u8)HUB_ADATA->index && Area != last_area) {
            if (CREDITS_LDATA != NULL && LastLData == CREDITS_LDATA && status->status_flags != 0) {
                RememberPlayerIDs(1, (i16)status->player_models[0], (i16)status->player_models[1]);
            }
            Hub_MakeModelList();
        }
        NuSound3StopRumble();
        FreeGameObjectLights();
        ClearUpAreaData();
        if (NuAllocDumpOnLevelPc != 0) {
            NuAllocDumpRecords(0);
        }
        MenuReset();
        WorldInfo_ClearAllIfScreenFaded();
        if (Game_Exit(last_area) != 0) {
            break;
        }
    }
    return 0;
}
