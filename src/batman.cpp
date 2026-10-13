#include "saga.h"
#include <string.h>

enum FADETYPE_VALUE {
    FADE_TYPE_NONE = -1,
    FADE_TYPE_SCREEN = 0,
    FADE_TYPE_WIPE = 1,
    FADE_TYPE_STILL_WIPE = 2,
    FADE_TYPE_STILL = 3,
};

struct FADEINFO_s;

class FadeBase {
  public:
    FADEINFO_s *info;
    virtual ~FadeBase();
    virtual void Init(FADEINFO_s *info) = 0;
    virtual void InitFade() = 0;
    virtual void UpdateFade() = 0;
    virtual void DrawFade() = 0;
    virtual FADETYPE_VALUE GetFadeType() const = 0;
};

struct FADEINFO_s {
    u32 direction;
    f32 fade;
    f32 rate;
    i32 busy;
    i32 stage;
    FadeBase *fades[4];
    FADETYPE_VALUE pending_type;
    i32 field_28;
};

STATIC_ASSERT(sizeof(FADEINFO_s) == 0x2c);
STATIC_ASSERT(offsetof(FADEINFO_s, direction) == 0x00);
STATIC_ASSERT(offsetof(FADEINFO_s, fade) == 0x04);
STATIC_ASSERT(offsetof(FADEINFO_s, rate) == 0x08);
STATIC_ASSERT(offsetof(FADEINFO_s, busy) == 0x0c);
STATIC_ASSERT(offsetof(FADEINFO_s, stage) == 0x10);
STATIC_ASSERT(offsetof(FADEINFO_s, fades) == 0x14);
STATIC_ASSERT(offsetof(FADEINFO_s, pending_type) == 0x24);
STATIC_ASSERT(offsetof(FADEINFO_s, field_28) == 0x28);

struct FADETYPE {
    FADETYPE_VALUE type;
};

class FadeSystem : public FADEINFO_s {
  public:
    void Init();
    void Update();
    void Draw();
    void SetStage(char stage);
    i32 SetFade(FADETYPE const &type, u32 frames);
    i32 AddFade(FadeBase *fade);
};

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
    TIMER BonusTimer;

    void __cdecl NuAllocDumpRecords(i32 min_size);

    u8 NuAllocDumpOnLevelPc;
    i32 AreaGlobalsChallengeCountPc;
    i32 NuMainSkipCutBordersPc;
    i64 frameout_count;
    extern i16 TextIdPc8810c0;
    char *Trailer[6] = {"demointro", (char *)&tINTRO, NULL, "indy", (char *)&TextIdPc8810c0, NULL};
}

/* PC splits NuMain's per-frame game update (0x491d50) and world render (0x4927f0) into helpers. */
static void NuMainUpdateFrame(WORLDINFO *world, i32 *frame_flag) {
    STUBBED_NAMED("NuMain: PC update helper at 0x00491d50");
}

static void NuMainRenderWorld(WORLDINFO *world) {
    STUBBED_NAMED("NuMain: PC world render helper at 0x004927f0");
}

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

    NuCommandLine(&argc, &argv);
    WorldInfo_InitOnce();
    InitOnce(argc, argv);

    STUBBED_NAMED("NuMain: DrawSaveSlots callback at 0x00494250");
    MenuInitialise(GameMenuInfo, 0x17, LANGUAGECOUNT, NULL, 1, 0);
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
        saved_world = WORLD;
        WORLD = NULL;
        PLAYERCOUNT = 0;
        STUBBED_NAMED("NuMain: PC trailer Movie_Play and FinishLoop at 0x00492ce5");
        WORLD = saved_world;
    }
    if (openlevels != 0) {
        STUBBED_NAMED("NuMain: Areas_OpenAll at 0x00492d08");
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
            CheckResetBits();
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
                    NuMainUpdateFrame(WORLD, &frame_flag);
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
                        NuMainRenderWorld(WORLD);
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
        if (PcWindowQuit != 0 || NewLData == NULL) {
            break;
        }
        STUBBED_NAMED("NuMain: level transition at 0x004937b1 (NewLData -> Area/Level, music, restart)");
        break;
    }

    STUBBED_NAMED("NuMain: Game_Exit and restart decision at 0x00493c12");
    return 0;
}
