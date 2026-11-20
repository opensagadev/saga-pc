#include "MechInputTouch/MechInputTouch_types.h"
#include "batman.h"
#include "decomp.h"
#include "gameapi/apitext.h"
#include "gameapi/edptl.h"
#include "gameapi/edtools/edgra.h"
#include "gameapi/gui/apimenu.h"
#include "gameframework/saveload.h"
#include "gamelib/nuwind/nuwind.h"
#include "globals.h"
#include "legoapi/actions/character/snake.h"
#include "legoapi/actions/character/streaks.h"
#include "legoapi/actions/character/tagging.h"
#include "legoapi/actions/combat/shovesys.h"
#include "legoapi/actions/movement/dropinout.h"
#include "legoapi/ai/core/gameai.h"
#include "legoapi/ai/game/creature.h"
#include "legoapi/audio/audio.h"
#include "legoapi/audio/sfx.h"
#include "legoapi/audio/specialsfx.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/charshadows.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/config/cheats.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/core/input/timing.h"
#include "legoapi/core/startup/main.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/gizmo/base/gizflow.h"
#include "legoapi/gizmo/base/gizmessage.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/gizmos/transport/teleport.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/batarang.h"
#include "legoapi/items/collect/bolt.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/fx/explosions.h"
#include "legoapi/items/fx/pulses.h"
#include "legoapi/items/fx/ripples.h"
#include "legoapi/items/objects/cable.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/items/objects/things.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/camera.h"
#include "legoapi/lib/hub.h"
#include "legoapi/lib/text.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "legoapi/menus/screens/gamestructure.h"
#include "legoapi/menus/screens/menu.h"
#include "legoapi/menus/screens/movies.h"
#include "legoapi/misc/buffers.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/legoapi_status.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/props/objects/zipup.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/rtl.h"
#include "legoapi/render/core/screen.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/render/fx/particles.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/render/light/lighting.h"
#include "legoapi/render/light/shadow.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/area.h"
#include "legoapi/world/areas.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episodeV.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/missions.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "legoapi/world/worldinfo.h"
#include "legogame/game.h"
#include "legogame/startup.h"
#include "nu2api/huffdecode.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nulgtlaser.h"
#include "nu2api/nu3d/nuobject.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nu3d/nutexanm.h"
#include "nu2api/nu3d/nutimebar_plain.h"
#include "nu2api/nu3d/nuwater.h"
#include "nu2api/nubridge.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/nucore/nuvideo.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic/numusic.h"
#include "nu2api/nusound/nusound.h"
#include "nu2api/nuwater_PC.h"
#include "nu2api/nuwin32/Stubs.h"
#include "nu2api/nuwin32/nu2api_windows.h"
#include <string.h>
#include "legoapi/menus/screens/customise.h"
#include "legoapi/menus/screens/missions.h"
#include "legoapi/world/levels/hub.h"
#include "legoapi/audio/radio.h"
#include "legoapi/characters/motion/camera.h"
#include "legoapi/characters/motion/charplatforms.h"
#include "legoapi/gizmo/base/gizgame.h"
#include "legoapi/items/collect/detonator.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/items/objects/grabber.h"
#include "legoapi/menus/screens/gamestatus_lsw.h"
#include "legoapi/misc/utilities.h"

extern "C" {
    i32 BURNOUTON = 1;
}

struct FADEINFO_s;

extern "C" {
    extern FADEINFO *pFadeInfo;
    void Movie_Play(char *name, VARIPTR *buffer, VARIPTR *end, f32 frame_time, void *callback, f32 volume);
    i32 CutScene_PlayingOrRequested(void *cutscene);
    TIMER BonusTimer = {0};

    void WorldInfo_DrawScene(WORLDINFO *world);
    void PortalDoors_Update(WORLDINFO *world);
    void GameDisplaySettings(LEVEL_DISPLAY *display, i32 *colours);
    void TrafficAnimSys_Update(struct TRAFFICANIMSYS_s *system);
    void TrafficAnimSys_Draw(struct TRAFFICANIMSYS_s *system);

    u8 NuAllocDumpOnLevel = 0;
    i32 editor_active = 0;
    i32 peak_poly_count = 0;
    i32 poly_count = 0;
    i32 openlevels = 0;
    i32 NOAICREATURES = 0;
    i32 NOTERRAINSTOP = 0;
    i32 DRAWBGLOAD = 0;
    i32 NuMainSkipCutBorders = 0;
    i32 LOADEROFF = 0;
    i32 frameout = 0;
    i64 frameout_count = 0;
    i32 HubMainRenderTimeHack = 0;
    DAT_HANDLES NuDatHandles = {0};
    char *Trailer[6] = {"demointro", (char *)&tINTRO, NULL, "indy", (char *)&dat_008810c0, NULL};
    i32 PlayTrailer = -1;
    MENUFNINFO GameMenuInfo[23] = {
        {MENU_TITLES, MenuEnterTitles, MenuDrawTitles, MenuUpdateTitles, NULL, -1, -1, 0, 0, 0, 0},
        {MENU_NEW_GAME, MenuEnterNewGame, MenuDrawNewGame, MenuUpdateNewGame, NULL, -1, -1, 0, 1, 0, 0},
        {MENU_PAUSE_MAIN, NULL, MenuDrawPauseMain, MenuUpdatePauseMain, NULL, -1, -1, 0, 0, 0, 0},
        {MENU_PAUSE_RESTART, NULL, MenuDrawPauseRestart, MenuUpdatePauseRestart, NULL, -1, -1, 0, 0, 0, 0},
        {MENU_OPTIONS, MenuEnterOptions, MenuDrawOptions, MenuUpdateOptions, MenuExitOptions, 0, 0, 0, 1, 1, 0},
        {MENU_EXTRAS, NULL, MenuDrawExtras, MenuUpdateExtras, NULL, 0, 0, 0, 0, 0, 0},
        {MENU_PAUSE_EXIT, NULL, MenuDrawPauseExit, MenuUpdatePauseExit, NULL, -1, -1, 0, 0, 0, 0},
        {MENU_SELECT_LANGUAGE, NULL, MenuDrawSelectLanguage, MenuUpdateSelectLanguage, NULL, 0, 0, 0, 0, 0, 0},
        {MENU_MISSIONS, MenuInitMissions, MenuDrawMissions, MenuUpdateMissions, NULL, -1, -1, 0, 0, 0, 0},
        {MENU_END_MISSION, NULL, MenuDrawEndMission, MenuUpdateEndMission, NULL, -1, -1, 0, 0, 0, 0},
        {MENU_END_CHALLENGE, NULL, MenuDrawEndChallenge, MenuUpdateEndChallenge, NULL, -1, -1, 0, 0, 0, 0},
        {MENU_ARCADE_END, NULL, Arcade_DrawEndMenu, Arcade_UpdateEndMenu, NULL, -1, -1, 0, 0, 0, 0},
        {MENU_CUSTOMISER, NULL, MenuDrawCustomiser, MenuUpdateCustomiser, NULL, -1, -1, 0, 0, 0, 0},
        {MENU_SHOP, NULL, MenuDrawShop, MenuUpdateShop, NULL, -1, -1, 0, 0, 0, 0},
        {MENU_MINIKIT, NULL, MenuDrawMiniKit, MenuUpdateMiniKit, NULL, -1, -1, 0, 0, 0, 0},
        {MENU_SELECT_MODE, MenuInitSelectMode, MenuDrawSelectMode, MenuUpdateSelectMode, NULL, -1, -1, 0, 0, 0, 0},
        {MENU_BONUS_MODE, MenuInitBonusMode, MenuDrawBonusMode, MenuUpdateBonusMode, NULL, -1, -1, 0, 0, 0, 0},
        {MENU_FREE_PLAY, MenuInitFreePlay, MenuDrawFreePlay, MenuUpdateFreePlay, NULL, -1, -1, 0, 0, 0, 0},
        {MENU_CREDITS, NULL, NULL, Credits_UpdateMenu, NULL, -1, -1, 0, 0, 0, 0},
        {MENU_BONUS_WIN, NULL, MenuDrawBonusWin, MenuUpdateBonusWin, NULL, -1, -1, 0, 0, 0, 0},
        {MENU_BONUS_COMPLETE, NULL, MenuDrawBonusComplete, MenuUpdateBonusComplete, NULL, -1, -1, 0, 0, 0, 0},
        {MENU_CONTROL_SETUP, MenuEnterControlSetup, MenuDrawControlSetup, MenuUpdateControlSetup, NULL, -1, -1, 0, 0, 0, 0},
        {MENU_SELECT_CONTROLS, MenuEnterSelectControls, MenuDrawControlSetup, MenuUpdateSelectControls, MenuExitSelectControls, 0, 0, 0, 0,
         0, 0},
    };
    LANGUAGEDATA Game_LanguageList[7] = {{1, apitxt_ENGLISH}, {2, apitxt_FRENCH}, {4, apitxt_GERMAN}, {5, apitxt_ITALIAN},
                                         {3, apitxt_SPANISH}, {8, apitxt_DANISH}, {-1, NULL}};
}

extern "C" {
    i32 NuMainSplineDebugMode = 0;
    i32 NuMainSplineDebugPhase = 0;
}

/* Debug view of the scene splines; NuMainRenderWorld calls it only while
 * NuMainSplineDebugMode is set, and mode 2 animates the dash pattern. */
static void NuMainDrawSplines(NUGSCN *gscn) {
    i32 colours[8] = {-1, -1, -1, -1, 0, 0, 0, 0};
    NURND_VERTEX3D vertices[2];
    NUGSPLINE *spline;
    NUVEC *point;
    i32 animate;
    i32 phase;
    i32 i;
    i32 j;

    if (gscn == NULL) {
        return;
    }
    i = gscn->spline_count;
    NuMainSplineDebugPhase--;
    animate = NuMainSplineDebugMode > 1;
    while (i != 0) {
        i--;
        spline = &gscn->splines[i];
        phase = NuMainSplineDebugPhase * animate;
        point = spline->pts;
        vertices[0].position = *point;
        vertices[0].colour = 0xffff0000;
        for (j = 1; j < spline->length; j++) {
            point = (NUVEC *)((char *)point + spline->pt_size);
            vertices[1].position = *point;
            phase += animate;
            vertices[1].colour = colours[phase & 7];
            NuRndrLine3d(vertices, NULL, NULL);
            vertices[0] = vertices[1];
        }
    }
}

/* PC splits NuMain's per-frame game update and world render into helpers. */
static void NuMainUpdateFrame(WORLDINFO *world, i32 *frame_flag) {
    GAMEOBJECT *object;
    LEVELDATA *level;
    FADETYPE fade_type;
    i32 i;
    void (*callback)(WORLDINFO *);
    NUVEC *wind_objects[8];
    FRAME_PAD *pads[2];
    ThingProcessData packet;

    UpdateFrameCounters();
    PortalDoors_Update(world);
    NuRndrShadowOnOff(NuMainRendererControl);
    if (Paused == 0) {
        GameAISysStartFrame(world->ai_sys);
    }
    AddCoinDelay[1] = 0.0f;
    AddCoinDelay[0] = 0.0f;
    other_level_override = -1;
    Shadow_SetMode();
    GizmoPickups_SetOnOff();
    LOG_INFO_IF(GameTimer.update_count < 2, "NuMainUpdateFrame before DoInput: world=%p", WORLD);
    DoInput(WORLD);
    LOG_INFO_IF(GameTimer.update_count < 2, "NuMainUpdateFrame after DoInput: world=%p", WORLD);
    if (memcard_autosaveenabled != 0 && memcard_autosavedisabled != 0 &&
        (world == NULL || world->area == NULL || (world->area->flags & AREAFLAG_ENDING_AREA) == 0) && NewMode == 0 && NewLData == NULL &&
        FadeSys.fade == 0.0f && editor_active == 0 && (GamePads_IgnoreInputFn == NULL || GamePads_IgnoreInputFn() == 0)) {
        if ((CUTSTOPGAME == 0 || CutScene_IsSkippable(CutStopInfo) != 0) && MiniCutCam == 0 && CutSceneWaiting == 0) {
            if (GameMenu[GameMenuLevel].menu != -1) {
                if (MenuInMemoryCard() == 0 && MenuInfo[GameMenu[GameMenuLevel].menu].id != MENU_NEW_GAME &&
                    MenuInfo[GameMenu[GameMenuLevel].menu].id != MENU_OPTIONS) {
                    NewMenu(MENU_AUTO_SAVE_CANCEL, 0, -1);
                } else {
                    memcard_autosavedisabled = 0;
                    memcard_autosaveenabled = 0;
                }
            } else if (Paused == 0) {
                for (i = 0; i < 2; ++i) {
                    object = Player[i];
                    if (object != NULL && (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                        PauseGame(object->player.pad - GamePadStorage);
                        break;
                    }
                }
            }
        }
    }
    pads[0] = GamePadStorage[0].bound_pad;
    pads[1] = GamePadStorage[1].bound_pad;
    packet.frame_time = FRAMETIME;
    packet.paused = Paused;
    packet.pads = pads;
    packet.controller_count = 2;
    theGameThings->ProcessThings(&packet);
    SAGA_SOUND_CHECK("before sound");
    LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: before sound world=%p", WORLD);
    if (NOSOUND == 0) {
        if (Paused != 0 && GameMenu[GameMenuLevel].menu == 4) {
            NuSound3SetDPL(TempOptions.dolby_surround, 0);
        } else {
            NuSound3SetDPL(Game.options_save.dolby_surround, 0);
        }
        NuSound3Update();
        UpdateLevelSfx(world, Paused);
        NuSound3SetRumblePads(
            Game.options_save.rumble[0] != 0 && Player[0] != NULL && (Player[0]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0
                ? Player[0]->player.pad->bound_pad
                : NULL,
            Game.options_save.rumble[1] != 0 && Player[1] != NULL && (Player[1]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0
                ? Player[1]->player.pad->bound_pad
                : NULL);
        NuSound3UpdateRumble(FRAMETIME);
        SoundUpdate(FRAMETIME);
        ProcessMusicChanges(world->current_level, &Game.options_save);
    }
    SAGA_SOUND_CHECK("after sound");
    LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after sound world=%p", WORLD);
    for (i = 0; i < 8; ++i) {
        if (Player[i] != NULL) {
            wind_objects[i] = &Player[i]->lower_position;
        } else {
            wind_objects[i] = NULL;
        }
    }
    if (Paused != 0 && screendump == 0 && !IsGrabbingScreen()) {
        NuMtlAnimateSetSpeedScale(0.0f);
        NuTexAnimSetMask(2);
        NuRndrGlobalFrameCountPause(1);
        pNuCam->mtx = GameCam->render_mtx;
        NuCameraSet(pNuCam);
        UpdateGameMenu(GamePadStorage, 1);
        for (i = 0; i < 2; ++i) {
            object = Player[i];
            if (object != NULL) {
                UpdateCoinPacket(object->player.coin_packet, (object->flags_1fc_word >> 7) & 1, object->player_index);
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
        LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after CutScenes_Update world=%p", WORLD);
        AdaptiveDifficultyUpdate(&adaptivedifficulty);
        if ((world->current_level->flags & LEVEL_TERRAIN) != 0) {
            TerrainPlatformOldUpdate();
        }
        LOG_INFO_IF(GameTimer.update_count < 2, "ProcessGizFlow before: world=%p", WORLD);
        SAGA_SOUND_CHECK("before ProcessGizFlow");
        ProcessGizFlow(world->giz_flow, FRAMETIME);
        SAGA_SOUND_CHECK("after ProcessGizFlow");
        LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after ProcessGizFlow world=%p", WORLD);
        if (world->ai_path_cnx_control_sys != NULL) {
            AIPathCnxControlSysUpdate(world->ai_path_cnx_control_sys);
        }
        if (world->current_gscn != NULL) {
            NuGScnUpdate(world->current_gscn, FRAMETIME * 60.0f);
        }
        GameAnimSys_Update(world->game_anim_sys);
        SAGA_SOUND_CHECK("after GameAnimSys_Update");
        LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after GameAnimSys_Update world=%p", WORLD);
        edanimUpdateObjects(1.0f);
        NuBridgeUpdate(&plr_lastpos);
        if (Grass_Available != 0) {
            NuWindUpdateArray(wind_objects);
            NuFadeObjUpdateArray(wind_objects);
        }
        rtlProcessLights(world->rtl_set, FRAMETIME);
        CharPlatforms_Update(world->shadow_platform_list);
        Grabber_Update(world);
        TrafficAnimSys_Update(world->trafficanim_sys);
        SAGA_SOUND_CHECK("after TrafficAnimSys_Update");
        LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after TrafficAnimSys_Update world=%p", WORLD);
        GizmoSysEarlyUpdate(world->gizmo_sys, world, FRAMETIME);
        SAGA_SOUND_CHECK("after GizmoSysEarlyUpdate");
        LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after GizmoSysEarlyUpdate world=%p", WORLD);
        if ((world->current_level->flags & LEVEL_TERRAIN) != 0) {
            TerrainPlatformNewUpdate();
        }
        UpdateSpecialSfx(world);
        Teleports_UpdateBeforeGameObjects(world);
        if (CUTSTOPGAME == 0) {
            SAGA_SOUND_CHECK("before ManageGameObjects");
            LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: before ManageGameObjects world=%p", WORLD);
            ManageGameObjects();
            SAGA_SOUND_CHECK("after ManageGameObjects");
            LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after ManageGameObjects world=%p", WORLD);
            UpdateGameObjects(world);
            SAGA_SOUND_CHECK("after UpdateGameObjects");
            LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after UpdateGameObjects world=%p", WORLD);
        } else {
            for (i = 0; i < 8; ++i) {
                object = Player[i];
                if (object != NULL && (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && object->player.pad->bound_pad != NULL) {
                    UpdateRumble(&object->player.pad->rumble);
                }
            }
        }
        ShoveObjectSysReset();
        if (CUTSTOPGAME == 0) {
            if (Cheat_IsOn(0x2c) == 0) {
                adaptivedifficulty.accumulator = 0.0f;
            }
            Teleports_UpdateAfterGameObjects(world);
            Pulses_Update(world->pulses_sys);
            Detonators_Update();
            Batarangs_Update();
            UpdateCables();
            SAGA_SOUND_CHECK("before Level_Update");
            LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: before Level_Update world=%p", WORLD);
            Level_Update(world);
            SAGA_SOUND_CHECK("after Level_Update");
            LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after Level_Update world=%p", WORLD);
            Bolts_Update(world);
            UpdateParts(FRAMETIME);
            SAGA_SOUND_CHECK("after UpdateParts");
            LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after UpdateParts world=%p", WORLD);
            if (brickimpactwait > 0.0f) {
                brickimpactwait -= FRAMETIME;
            }
            if (coinimpactwait > 0.0f) {
                coinimpactwait -= FRAMETIME;
            }
            if (SaberSparksTimer > 0.0f) {
                SaberSparksTimer -= FRAMETIME;
            }
            if (TerrainChatterWait > 0.0f) {
                TerrainChatterWait -= FRAMETIME;
            }
            if (tieonsfxwait > 0.0f) {
                tieonsfxwait -= FRAMETIME;
            }
            if (tieoffsfxwait > 0.0f) {
                tieoffsfxwait -= FRAMETIME;
            }
            UpdateExplosions();
            Tag_UpdateTransfers(world->game_debris_sys->entries[39].effect, world->game_debris_sys->entries[40].effect, 180);
            UpdateRepeatSfx();
            GizmoSysLateUpdate(world->gizmo_sys, world, FRAMETIME);
            SAGA_SOUND_CHECK("after GizmoSysLateUpdate");
            UpdateRadios();
            Hint_Process(FRAMETIME);
            Cheats_FrameUpdate();
        }
        UpdateGameMenu(GamePadStorage, 1);
        Debris(0);
        if (CUTSTOPGAME == 0) {
            DebrisKillPlayers();
        }
        UpdateRippleSet(RippleSet);
        AddCameraRain(world, 0x7a);
        NuMtlAnimateSetSpeedScale(1.0f);
        NuMtlAnimateSetMask(0xffff);
        SAGA_SOUND_CHECK("before MoveGameCamera");
        LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: before MoveGameCamera world=%p", WORLD);
        MoveGameCamera(GameCam);
        SAGA_SOUND_CHECK("after MoveGameCamera");
        LOG_INFO_IF(GameTimer.update_count < 2, "Update stage: after MoveGameCamera world=%p", WORLD);
        GameObjectToCameraDistances();
        SetCameraMatrices();
        if (CUTSTOPGAME == 0) {
            UpdateStreaks(FRAMETIME);
        }
        if ((world->area != NULL && world->area == HUB_ADATA) || world->current_level == TITLES_LDATA ||
            world->current_level == STATUS_LDATA || (world->current_level->flags & LEVEL_STATUS) != 0 ||
            world->current_level == CREDITS_LDATA) {
            BackDrop_Update(FRAMETIME);
        }
    }
    callback = world->current_level->always_update;
    if (callback != NULL) {
        callback(world);
    }
    LevelStreaming_Update(world);
    UpdateCutBorders();
    if (Paused != 0 && editor_active == 0) {
        NuWaterSpeed(0.0f);
    } else {
        NuWaterSpeed((f32)((f64)FRAMETIME / (f64)(1.0f / 60.0f)));
    }
    NuWaterReset();
    GameTiming(world, &Game.game_time);
    UpdateStats();
    *frame_flag = 0;
    GameDisplaySettings(&world->current_level->display, back_rgba);
    if ((NewMode == 0 && NewLData == NULL) || FadeSys.fade != 0.0f) {
        return;
    }
    level = world->current_level;
    if (level != NULL && (level->flags & LEVEL_STATUS) != 0 && HUB_LDATA != NULL && NewLData != HUB_LDATA && NewLData != NULL &&
        NewLData->area_index != -1 && (ADataList[NewLData->area_index].flags & AREAFLAG_ENDING_AREA) == 0) {
        FadeSys.fade = 1.0f;
        FinishLoop_On = 0;
        return;
    }
    if (FadeSys.pending_type == FADE_TYPE_NONE || (NewLData != NULL && WORLD->current_level != NewLData && NewLData == HUB_LDATA)) {
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
                if (SuperStory != 0 && (level->flags & LEVEL_STATUS) != 0 &&
                    (ADataList[NewLData->area_index].flags & AREAFLAG_ENDING_AREA) != 0) {
                    goto fade_wipe;
                }
            }
        }
        if (NewLData == NULL || NewLData->area_index == WORLD->area_index) {
            goto fade_still;
        }
        if (SuperStory != 0 && (WORLD->area->flags & AREAFLAG_ENDING_AREA) == 0) {
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

/* The debris render stack, re-sorted, then both debris passes. */
static void DrawSortedDebris(i32 paused) {
    UpdateDebrisRenderStackPriority();
    SortDebrisRenderStack();
    DebrisDraw(paused ^ editor_active, 1);
    DebrisDraw(paused ^ editor_active, 0);
}

static void NuMainRenderWorld(WORLDINFO *world) {
    i32 paused;

    GameFog_Update(world);
    GameFog_Set();
    SetLevelLights(world->rtl_set, 1.0f);
    SetSpotLightMode();
    if (CUTSTOPGAME == 0) {
        DrawGameObjectsProcess();
    }
    SetShadowLight();
    NuRndrBeginScene(1);
    SpeedBlur_Update();
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
    Grabber_Draw(world);
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
        DrawCables();
        Detonators_Draw();
        Batarangs_Draw();
        TrafficAnimSys_Draw(world->trafficanim_sys);
        GizmoSysDraw(world->gizmo_sys, world, FRAMETIME);
        SpecialMiniKits_Draw(world);
        NuBridgeDraw(NULL);
    }
    if (Grass_Available != 0) {
        NuWindDraw();
        NuFadeObjDraw();
    }
    theGameThings->RenderThings(NULL);
    CutScenes_Draw(world);
    NuRndrShadPolys(PermanentShadowMaterial);
    NuLgtLaserDraw(paused);
    if (world->current_level == TITLES_LDATA || (world->current_level->flags & LEVEL_STATUS) != 0 ||
        world->current_level == CREDITS_LDATA ||
        (world->current_level == STATUS_LDATA && (StatusPacket.flags_word & STATUSPACKET_SUPER_STORY_MODE) != 0)) {
        BackDrop_Draw(1.0f, 0);
    }
    if (MainRenderTime != 1.0f) {
        BackDrop_Draw(1.0f - MainRenderTime, 0);
    }
    NuRndrLine3dDbgFlush();
    DebrisSetCutSceneMode(CUTCAM);
    DebrisDraw(paused ^ editor_active, 1);
    DrawRippleSet(RippleSet);
    SpeedBlur_Apply(world);
    NuRndrEndScene();
    RenderShadowLights(Paused);
    NuRndrBeginScene(1);
    SetDepthOfField();
    NuMainRenderLeaf();
    if (BURNOUTON != 0) {
        BurnoutApply(paused);
    }
    if (NuMainSplineDebugMode != 0) {
        NuMainDrawSplines(world->current_gscn);
    }
    NuRndrEndScene();
    NuRndrFx(paused, &plr_lastpos);
    NuRndrBeginScene(1);
    DrawSortedDebris(paused);
    NuRndrEndSceneEx(0);
    DebrisDrawGlass();
    if (screendump == 0) {
        ViewCamDraw();
    }
}

extern "C" {
    u8 StatusPlayerActive[8] = {0};
}

extern "C" {
    i32 ResetOldFStop = 0;
}

extern "C" int NuMain(int argc, char **argv) {
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
    STATUSPACKET_s *status = &StatusPacket;

    LOG_INFO("Game startup: argc=%d", argc);
    NuCommandLine(&argc, &argv);
    WorldInfo_InitOnce();
    InitOnce(argc, argv);

    MenuInitialise(GameMenuInfo, 0x17, LANGUAGECOUNT, DrawSaveSlots, 1, 0);
    MenuReset();
    edGraInitTerrainSwapProtection();

    NuDatHandles.previous = NuDatGet();
    NuDatHandles.episode[0] = NuDatOpen("Episode_I.dat", &permbuffer_base, NULL);
    NuDatHandles.episode[1] = NuDatOpen("Episode_II.dat", &permbuffer_base, NULL);
    NuDatHandles.episode[2] = NuDatOpen("Episode_III.dat", &permbuffer_base, NULL);
    NuDatHandles.episode[3] = NuDatOpen("Episode_IV.dat", &permbuffer_base, NULL);
    NuDatHandles.episode[4] = NuDatOpen("Episode_V.dat", &permbuffer_base, NULL);
    NuDatHandles.episode[5] = NuDatOpen("Episode_VI.dat", &permbuffer_base, NULL);
    NuDatSet(NuDatHandles.previous);
    NuHGobjReversibleCharacters(1);
    NuHGobjForceShadowsOnCharacters(1);
    GameAISysInit();
    APITransparentInit();
    NuSound3SetLoopHoldTime(0.5f);
    InitShadowLights();
    GamePads_Init();
    bgProcInit();
    FinishLoop(2);
    Text_InitLanguageList(Game_LanguageList);
    Text_SetLanguage(-1);
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
        NuMainTrailerLeaf();
        WORLD = saved_world;
    }
    if (openlevels != 0) {
        Areas_OpenAll(0);
    }
    if (Level == -1) {
        Level = 0;
    }

    Area = LDataList[Level].area_index;
    last_area = -1;
    LastAData = NULL;
    if (Area != -1 && (ADataList[Area].flags & AREAFLAG_BONUS_AREA) != 0) {
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
        if (last_area != -1 && LastLData != NULL && (LastLData->flags & LEVEL_STATUS) != 0 &&
            ADataList[Area].flags != AREAFLAG_ENDING_AREA && HUB_ADATA != NULL && Area != HUB_ADATA->index) {
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
        if (NuAllocDumpOnLevel != 0) {
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
                if (MissionSys != NULL && MissionSys->state != MISSION_STATE_NONE) {
                    message->value = 1.0f;
                }
                message->flags |= 1;
                message->output_values[0] = 1;
                message->output_values[1] = 0;
                message->output_count = 2;
            }
            message = CheckGizAIMessage(gizaimessagesys, "ChallengeMode", NULL);
            if (message != NULL) {
                if (ChallengeMode != CHALLENGE_OFF) {
                    message->value = 1.0f;
                }
                message->flags |= 1;
                message->output_values[0] = 1;
                message->output_values[1] = 0;
                message->output_count = 2;
            }
        }
        if (Area == -1 || ADataList[Area].episode_index == -1 || (ADataList[Area].flags & AREAFLAG_MINIKIT) == 0 ||
            (ADataList[Area].flags & AREAFLAG_BONUS_AREA) != 0) {
            ChallengeMode = CHALLENGE_OFF;
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
            LOG_INFO("NuMain before CheckResetBits: world=%p", WORLD);
            CheckResetBits();
            LOG_INFO("NuMain after CheckResetBits: world=%p", WORLD);
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
            // Three reads of the extern volatile vblank counter: the game loads it three times here.
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

            while (((NewMode == 0 && NewLData == NULL) || FadeSys.fade < 1.0f || pFadeInfo->busy != 0 ||
                    (((waiting_for_level != -1 && (reset_load != 0 || abort_load != 0)) || waiting_for_character != -1) && NewMode == 0)) &&
                   (CutScenes_BGLoadManager(), NuWindowsProcessMessages() != 0)) {
                NuVideoGetSwapMode();
                NuTimeBarSlotReset(0, 0xf);
                _NuTimeBarSlotBegin(0, 0xf, "frmtmr");
                DebrisSetTimeIncrement(FRAMETIME);
                InitPanel(GetMenuID() == MENU_OPTIONS ? TempOptions.widescreen : Game.options_save.widescreen);
                saved_frametime = FRAMETIME;
                if ((waiting_for_level == -1 || gone_through_door_to_new_level == 0) && screendump == 0) {
                    if (ChallengeMode == CHALLENGE_RUNNING && AreaGlobals.challenge_minikits_shown >= 10 &&
                        WORLD->current_level != HUB_LDATA && MiniCutCam == 0 && CutScene_PlayingOrRequested(NULL) == 0 &&
                        FadeSys.fade == 0.0f) {
                        EndChallenge(CHALLENGE_WON, 1);
                    }
                    LOG_INFO_IF(GameTimer.update_count < 2, "NuMain before update: world=%p", WORLD);
                    SAGA_SOUND_CHECK("before frame update");
                    NuMainUpdateFrame(WORLD, &frame_flag);
                    SAGA_SOUND_CHECK("after frame update");
                    LOG_INFO_IF(GameTimer.update_count < 2, "NuMain after update: world=%p", WORLD);
                }
                if (waiting_for_level != -1 && screendump == 0) {
                    WaitingForLevelTime += FRAMETIME;
                }
                if (waiting_for_character != -1 && screendump == 0) {
                    WaitingForCharacterTime += FRAMETIME;
                }
                FRAMETIME = saved_frametime;
                WidescreenCode(GetMenuID() == MENU_OPTIONS ? TempOptions.widescreen : Game.options_save.widescreen);
                UpdateBackgroundMusic();
                NuFrameBegin();
                if (SoundVoiceEnabledGate != 0) {
                    if (pause_rndr_on == 0 || FadeSys.pending_type == FADE_TYPE_WIPE) {
                        if (0.0f < MainRenderTime) {
                            LOG_INFO_IF(GameTimer.update_count < 2, "NuMain before render: world=%p", WORLD);
                            SAGA_SOUND_CHECK("before world render");
                            NuMainRenderWorld(WORLD);
                            SAGA_SOUND_CHECK("after world render");
                            LOG_INFO_IF(GameTimer.update_count < 2, "NuMain after render: world=%p", WORLD);
                        } else {
                            NoRender();
                        }
                    }
                }
                if (NuMainSkipCutBorders == 0) {
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
            ViewCamSetActive(0, NULL);
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
            if ((HUB_ADATA == NULL || last_area != HUB_ADATA->index) && Arcade != 0) {
                if (HUB_ADATA != NULL && Area == HUB_ADATA->index) {
                    hub_from_arcade = last_area;
                }
                Arcade = 0;
            }
        }
        if (NewLData == STATUS_LDATA || (NewLData->flags & LEVEL_STATUS) != 0) {
            InitStatusScreen(WORLD);
            if (NewLData != STATUS_LDATA && status->status_flags != 0) {
                NuMainStatusHelper(&StatusPacket);
            }
        }
        if (netclient != 0) {
            for (player_index = 0; player_index < 8; ++player_index) {
                PlayerProgress[player_index].active = StatusPlayerActive[player_index];
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
            if ((ADataList[Area].flags & AREAFLAG_BONUS_AREA) != 0) {
                NextArea_FreePlay = 1;
                FreePlay = 1;
            }
            if (new_level_from_menu != 0 && Area != last_area) {
                if (HUB_ADATA != NULL && Area == HUB_ADATA->index) {
                    NextArea_FreePlay = 0;
                }
                FreePlay = NextArea_FreePlay;
                if (FreePlay != 0) {
                    if (Player[0] != NULL) {
                        Player[0]->id = -1;
                    }
                    if (Player[1] != NULL) {
                        Player[1]->id = -1;
                    }
                    makefreeplaymodellist = 1;
                }
            }
        }
        if (status->status_flags != 0 && ((STATUS_LDATA != NULL && LastLData == STATUS_LDATA) || (LastLData->flags & LEVEL_STATUS) != 0 ||
                                          (CREDITS_LDATA != NULL && LastLData == CREDITS_LDATA))) {
            for (player_index = 0; player_index < 8; ++player_index) {
                if (player_index < 2) {
                    PlayerList[player_index] = status->player_models[player_index];
                    PlayerProgress[player_index].active = status->player_active[player_index];
                } else {
                    PlayerList[player_index] = -1;
                    PlayerProgress[player_index].active = 0;
                }
                PlayerProgress[player_index].weapon_out = 0;
                PlayerProgress[player_index].coins = 0;
            }
            UsePlayerList = HUB_ADATA != NULL && Area == HUB_ADATA->index && LastLData != TITLES_LDATA ? 1 : 2;
        }
        if (HUB_ADATA != NULL && Area == HUB_ADATA->index && Area != last_area) {
            if (CREDITS_LDATA != NULL && LastLData == CREDITS_LDATA && status->status_flags != 0) {
                RememberPlayerIDs(1, (i16)status->player_models[0], (i16)status->player_models[1]);
            }
            Hub_MakeModelList();
        }
        NuSound3StopRumble();
        FreeGameObjectLights();
        ClearUpAreaData();
        if (NuAllocDumpOnLevel != 0) {
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
