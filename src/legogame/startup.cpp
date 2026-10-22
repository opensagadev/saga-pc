#include "saga.h"
extern "C" void __cdecl TextInputGlyphNamePc(i32, char *);
extern "C" void __cdecl RegisterLswGizmoTypesPc(VARIPTR *, VARIPTR *);
extern "C" void __cdecl RegisterAlternateGizmoTypesPc(VARIPTR *, VARIPTR *);
extern "C" {
    void __cdecl MiniKits_Init(VARIPTR *, VARIPTR *);
    void __cdecl CharCategories_Init(CHAR_CATEGORY_RECORD_PC *);
    extern CHAR_CATEGORY_RECORD_PC StartupCharCategoriesDefaultPc[6];
    extern CHAR_CATEGORY_RECORD_PC StartupCharCategoriesLswPc[10];
    extern char *StartupCharVariantNamesPc[23];
}
extern "C" void __cdecl TextInputSetNameCallbackPc(void(__cdecl *)(i32, char *));
static i32 __cdecl PcInputCommandAllowedPc(i32 port, i32 command) {
    if (TextInputIsKeyboardDevicePc(port) && ((command >= 4 && command <= 7) || (u32)(command - 8) <= 3))
        return 0;
    return 1;
}
extern "C" void __cdecl Movies_ConfigureList(char *, VARIPTR *, VARIPTR *);
class Fade;
class FadeWipe;
class FadeStillWipe;
class FadeStill;
extern "C" const char MoviesListPathPc[18];
extern "C" {
    extern FADEINFO_s *pFadeInfo;
    extern Fade fade;
    extern FadeWipe fadeWipe;
    extern FadeStillWipe fadeStillWipe;
    extern FadeStill fadeStill;
    void __cdecl NuStringFilterLoad(char *path, VARIPTR *buf, VARIPTR buf_end);
}

extern "C" {

    void __cdecl LoadPermLoadFonts(void);
    void __cdecl Text_LoadFont(char *, VARIPTR *, VARIPTR *);

    extern LEVELDATA_PC *__cdecl Levels_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end, i32 max_count,
                                                      i32 *count_out, void *set_defaults_fn);
    extern AREA_DATA_PC *__cdecl Areas_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end, i32 max_count,
                                                     i32 *count_out);

    void __cdecl Level_SetDefaults(LEVELDATA_PC *level) {
        level->max_gameantinodes = 0x100;
        level->max_gizmoblowups = 0x40;
        level->max_gizmoblowuptypes = 0x40;
        level->max_pickups = 0x100;
        level->max_obstacle_objects = 0x80;
        level->max_buildit_objects = 0x80;
        level->max_force_objects = 0x80;
        level->max_bombgen_objects = 0x10;

        level->max_tightropes = 10;
        level->max_timers = 8;
        level->max_signals = 10;
        level->max_levers = 10;
        level->max_technos = 10;
        level->max_zipups = 10;
        level->max_grapples = 10;
        level->max_obstacles = 0x20;
        level->max_buildits = 0x10;
        level->max_shards = 0x40;
        level->max_spinners = 4;
        level->max_minicuts = 8;
        level->max_minicut_stages = 2;
        level->max_gizspecials = 0x20;
        level->max_attractos = 5;
        level->max_climb_objects = 10;
        level->max_guidelines = 0x20;
        level->max_ledges = 0x60;
        level->max_securitydoors = 5;
        level->max_tubes = 5;
        level->max_gizpanels = 8;
        level->max_hatmachines = 2;
        level->max_force = 0x20;
        level->max_pushblocks = 0x10;
        level->max_pushblock_endpos = 8;
        level->max_doors = 0x10;
        level->max_teleports = 4;
        level->max_gizrandoms = 8;
        level->max_torpmachines = 2;
        level->max_spinneranim_objs = 0x28;
        level->max_turrets = 0x20;
        level->max_bombgens = 8;
        level->max_bridges = 5;
        level->max_plugs = 10;

        level->wind_speed = 3.0f;
        level->wind_size = 512.0f;
    }

    f32 __cdecl LoadPermFadeAlpha(f32 t, f32 duration) {
        f32 end;
        f32 total;

        if (t > 0.0f && t < duration + 0.3f + 0.3f + 0.2f) {
            if (t < 0.3f) {
                return t / 0.3f;
            }
            end = duration + 0.3f;
            if (t < end) {
                return 1.0f;
            }
            total = end + 0.3f;
            if (t < total) {
                return 1.0f - (t - end) / (total - end);
            }
        }
        return 0.0f;
    }

    void __cdecl LoadPerm(void) {
        WORLDINFO *saved_world;
        f32 language_timer;
        f32 legal_timer;
        f32 ready_timer;
        f32 intro_timer;
        f32 tail_timer;
        f32 menu_alpha;
        f32 menu_timer;
        f32 legal_alpha;
        f32 intro_alpha;
        f32 aspect;
        f32 half;
        f32 frame_time;
        NUMTX scale;
        NUVEC scale_vec = {0.125f, 0.125f, 0.125f};
        NUSPECIALHANDLE special = {NULL, NULL, NULL};

        SetBackgroundMusic(1);
        saved_world = WORLD;
        PermDataLoaded = 0;
        WORLD = NULL;

        if (BGLOAD == 0 || LOADEROFF != 0) {
            LoadPerm_LanguageSelect = 3;
            LoadPermData(NULL);
            if (legal_mtl != NULL) {
                NuMtlDestroy(legal_mtl);
                legal_mtl = NULL;
            }
            WORLD = saved_world;
            return;
        }

        FRAMETIME = DEFAULTFRAMETIME;
        pNuCam->mtx = NuMtxSetIdentityTemplate;
        NuCameraSet(pNuCam);
        InitPanel(Game.options_save.widescreen);
        NuQFntSetCoordinateSystem(3);
        NuMtxSetScale(&scale, &scale_vec);
        scale.m32 = 1.0f;
        if (PAL == 0) {
            LoadPerm_LanguageSelect = 3;
        }
        language_timer = 3.3f;
        tail_timer = 0.2f;
        legal_timer = 0.0f;
        ready_timer = 0.0f;
        intro_timer = 0.0f;
        menu_alpha = 0.0f;
        menu_timer = 0.0f;
        bgPostRequest((bgprocdofn *)LoadPermData, NULL, NULL, 0);

        while (PermDataLoaded == 0 || tail_timer > 0.0f) {
            NuFrameBegin();
            NuCameraSet(&global_camera);
            readpads_always = 1;
            ReadPads();
            UpdateGameMenu((GAMEPAD_PC *)&GamePadStorage[0], 0);
            UpdateTimer(&GlobalTimer);
            menu_flash = NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.2f) < 0.1f;
            if (NOSOUND == 0) {
                NuSound3Update();
            }

            if (language_timer < 3.3f && save_icon_scene != NULL) {
                if (language_timer == 0.0f && NuSpecialExistsFn(&special) == 0) {
                    if (NuSpecialFind(save_icon_scene, &special, LoadPermSaveIconSpecialPc, 1) == 0) {
                        language_timer = 3.3f;
                    }
                } else {
                    language_timer += FRAMETIME;
                    if (language_timer > 3.3f) {
                        language_timer = 3.3f;
                    }
                }
            }

            if (menu_timer < 0.5f && language_timer >= 3.3f && Text_IsFontLoaded() != NULL) {
                if (LoadPerm_LanguageSelect == 0) {
                    NewMenu(7, 0, -1);
                    FadeSys.fade = -1.0f;
                    LoadPerm_LanguageSelect = 1;
                }
                if (LoadPerm_LanguageSelect == 1 && GameMenuLevel == 0) {
                    LoadPerm_LanguageSelect = 2;
                }
                if (LoadPerm_LanguageSelect == 2) {
                    menu_timer += FRAMETIME;
                    if (menu_timer >= 0.5f) {
                        LoadPerm_LanguageSelect = 3;
                    }
                }
            }

            if (ready_timer < 1.0f && language_timer >= 3.3f && LoadPerm_LanguageSelect == 3 &&
                LoadPerm_StringsLoaded != 0 && saveload_status == 1 && GameMenuLevel == 0) {
                ready_timer += FRAMETIME;
                if (ready_timer > 1.0f) {
                    ready_timer = 1.0f;
                }
            }

            if (legal_timer < 5.8f && language_timer >= 3.3f && LoadPerm_LanguageSelect == 3 && ready_timer >= 1.0f &&
                loadlegal_done != 0) {
                if (legal_timer == 0.0f && legal_mtl == NULL) {
                    if (legal_tid == 0) {
                        legal_timer = 5.8f;
                    } else {
                        if (LoadPermCreateLegalMtl(&legal_mtl, legal_tid) == 0)
                            legal_timer = 5.8f;
                    }
                } else {
                    legal_timer += FRAMETIME;
                    if (legal_timer > 5.8f) {
                        legal_timer = 5.8f;
                    }
                }
            }

            if (intro_timer < 3.8f && language_timer >= 3.3f && LoadPerm_LanguageSelect == 3 && ready_timer >= 1.0f &&
                legal_timer >= 5.8f && Text_IsFontLoaded() != 0 && LoadPerm_StringsLoaded != 0) {
                if (StartupGameFlag1 == 0 || INTROTEXT_ID != -1) {
                    intro_timer += FRAMETIME;
                    if (intro_timer > 3.8f) {
                        intro_timer = 3.8f;
                    }
                }
            }

            if (PermDataLoaded != 0 && LoadPerm_LanguageSelect == 3 && language_timer >= 3.3f && ready_timer >= 1.0f &&
                legal_timer >= 5.8f && intro_timer >= 3.8f) {
                tail_timer -= FRAMETIME;
                if (tail_timer < 0.0f) {
                    tail_timer = 0.0f;
                }
            }

            NuRndrBeginScene(1);
            NuRndrGradClear(0xf00, (i32)0x80000000, (i32)0x80000000, 1.0f);

            if (tail_timer == 0.2f) {
                menu_alpha = LoadPermFadeAlpha(language_timer, 2.5f);
                if (menu_alpha > 0.0f) {
                    NuSpecialDrawAtAlpha(&special, &scale, menu_alpha);
                }
                DrawMenu(0);

                legal_alpha = LoadPermFadeAlpha(legal_timer, 5.0f);
                if (legal_alpha > 0.0f) {
                    switch (PcWindowOptions.aspect_mode) {
                        case 0:
                            aspect = 1.7777778f;
                            break;
                        case 1:
                            aspect = 1.6f;
                            break;
                        case 2:
                            aspect = PCDeviceBackend.active_aspect;
                            break;
                        default:
                            aspect = 1.3333334f;
                            break;
                    }
                    half = (1.0f - 1.3333334f / aspect) * 0.5f;
                    NuRndrClear(0xb00, 0, 1.0f);
                    ++NuPrimCSPos;
                    NuPrimSetCoordinateSystem(2);
                    NuPrim2DBegin(0x10, 7, legal_mtl);
                    NuPrim2DSetColor((i32)(((u32)(i32)(legal_alpha * 255.0) << 24) | 0x808080));
                    NuPrim2DSetUV(0.0f, 0.0f);
                    NuPrim2DAddXYZ(half + 0.0f, 0.0f, 0.0f);
                    NuPrim2DSetColor((i32)(((u32)(i32)(legal_alpha * 255.0) << 24) | 0x808080));
                    NuPrim2DSetUV(1.0f, 1.0f);
                    NuPrim2DAddXYZ(1.0f - half, 1.0f, 0.0f);
                    NuPrim2DEnd();
                    --NuPrimCSPos;
                    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
                }

                intro_alpha = LoadPermFadeAlpha(intro_timer, 3.0f);
                if (intro_alpha > 0.0f && StartupGameFlag1 != 0) {
                    IntroText_Draw(intro_alpha);
                }
            }

            NuRndrEndScene();
            edGraEnableTerrainSwap();
            frame_time = NuFrameEnd();
            FRAMETIME = frame_time;
            edGraDisableTerrainSwap();
            if (FRAMETIME > MAXFRAMETIME && CUTSTOPGAME == 0) {
                FRAMETIME = MAXFRAMETIME;
            }
        }

        if (legal_mtl != NULL) {
            NuMtlDestroy(legal_mtl);
            legal_mtl = NULL;
        }
        WORLD = saved_world;
    }

    void __cdecl FixUpAreas(void) {
        if (StartupGameFlag1 != 0) {
            Areas_FixUp(AreaFixUpLSW);
            if (LOSTTEMPLE_ADATA != NULL) {
                LOSTTEMPLE_ADATA->flags |= 0x2000;
            }
        } else if (StartupGameFlag0 != 0) {
            Areas_FixUp(AreaFixUpLostTemple);
        } else {
            Areas_FixUp(AreaFixUpGotham);
        }
    }

    void __cdecl LoadPermData(void *process) {
        i16 default_id_a;
        i16 default_id_b;
        u32 aligned;
        PC_CONTROL_CONFIG_PC control_config;
        char blocked_keys[4];

        LoadPermLoadFonts();
        while (LoadPerm_LanguageSelect != 3) {
        }

        Text_InitStringTable(0x70d, &permbuffer_ptr, &permbuffer_end);

        if (StartupGameFlag1 == 0 && StartupGameFlag0 == 0) {
            Text_InitTable(Text_TableMenu, 0, 0x31f);
            Text_InitTable(Text_TableGame, 0x320, 0x70c);
        } else {
            Text_InitTable(Text_TableMain, 0, 0x70c);
        }

        Text_LoadStrings(&permbuffer_ptr, &permbuffer_end);
        Text_InitDefaultStrings();
        LoadPerm_StringsLoaded = 1;

        if (process != NULL) {
            LoadPermLoadLegalTexture();
        }

        NuStringFilterLoad("stuff\\text\\badwords.txt", &permbuffer_ptr, permbuffer_end);
        MusicInfo = ConfigureMusic("audio\\music.txt", &permbuffer_ptr, &permbuffer_end);
        SfxSetFileTable((SFX_FILE_INFO_PC *)MusicInfo);
        InitSfx(&permbuffer_ptr, permbuffer_end, "Audio\\Audio.cfg");
        InitStillRender(&permbuffer_ptr, permbuffer_end);

        OldLevelHackData = GameBufferAlloc(&permbuffer_ptr, &permbuffer_end, 0x80);
        LevelHackData = GameBufferAlloc(&permbuffer_ptr, &permbuffer_end, 0x80);
        LevelHackDataSize = 0;

        LevelProgress_ReserveBufferSpace(&permbuffer_ptr, permbuffer_end);
        pFadeInfo = &FadeSys;
        FadeSys.Init();
        FadeSys.AddFade((FadeBase *)&fade);
        FadeSys.AddFade((FadeBase *)&fadeWipe);
        FadeSys.AddFade((FadeBase *)&fadeStillWipe);
        FadeSys.AddFade((FadeBase *)&fadeStill);
        LevelObjects_InitForGame(ObjTab, &permbuffer_ptr, &permbuffer_end, 0x2ee, 0x1f40);
        LevelSplines_InitForGame(LevelSplinesTable);
        STUBBED_NAMED("LoadPermData: LoadPermLoadSaveIconScene");

        aligned = (permbuffer_ptr.addr + 0xfU) & ~0xfU;
        permbuffer_ptr.addr = aligned + 0x200;
        theMemoryManager.buffer_start = aligned;
        theMemoryManager.buffer_end = aligned + 0x200;
        theMemoryManager.start_anchor = &theMemoryManager.buffer_start;
        theMemoryManager.end_anchor = &theMemoryManager.buffer_end;
        theMemoryManager.unknown_10 = 0;
        theMemoryManager.unknown_14 = 0;
        theMemoryManager.size = 0x200;
        memset(theMemoryManager.free_lists, 0, sizeof(theMemoryManager.free_lists));

        STUBBED_NAMED("LoadPermData: CreateThingManager");

        if (StartupGameFlag0 != 0) {
            STUBBED_NAMED("LoadPermData: startup registration variant (0x46a150)");
        } else if (StartupGameFlag1 != 0) {
            RegisterLswGizmoTypesPc(&permbuffer_ptr, &permbuffer_end);
            GameAnimSys_AllocateLevelProgressData(&permbuffer_ptr, &permbuffer_end, 0x280, 0xc);
            STUBBED_NAMED("LoadPermData: LSW_registerStatusScreen at 0x004642e0");
            initGameHintSys_LSW();
            CutScene_StartFn = CutScene_StartFn_LSW;
            CutScene_PreUpdateFn = CutScene_PreUpdateFn_LSW;
            CutScene_PostUpdateFn = CutScene_PostUpdateFn_LSW;
            CutScene_StoppedFn = CutScene_StoppedFn_LSW;
            CutScene_ReplaceCharacterModelFn = CutScene_ReplaceCharacterModelFn_LSW;
            InitBolt_AddMomentumType = InitBolt_AddMomentumType_LSW;
            Bolt_HitPlatFn = Bolt_HitPlatFn_LSW;
            Bolt_HitCustomFn = Bolt_HitCustomFn_LSW;
            GameBlowUpBlownUpFn = GameBlowUpBlownUpFn_LSW;
            GizObstacle_SetDefaultSFXFn = GizObstacle_SetDefaultSFXFn_LSW;
        } else {
            RegisterAlternateGizmoTypesPc(&permbuffer_ptr, &permbuffer_end);
        }

        if (!(StartupGameFlag0 == 0 && StartupGameFlag1 != 0)) {
            GameAnimSys_AllocateLevelProgressData(&permbuffer_ptr, &permbuffer_end, 0x280, 0xc);
            STUBBED_NAMED("LoadPermData: registration pair (0x45e2e0, 0x457f60)");
        }

        Movies_ConfigureList((char *)MoviesListPathPc, &permbuffer_ptr, &permbuffer_end);
        SetProceduralAnimationFn(NuAnimBuffProceduralAnimation);
        CDataList = ConfigureCharacterList("chars\\chars.txt", &permbuffer_ptr, &permbuffer_end, 0x154, &CHARCOUNT,
                                           0x120, &GCDataList);
        CharScenes_Init(&permbuffer_ptr, &permbuffer_end);
        IconScenes_Init("stuff\\icons\\", &permbuffer_ptr, &permbuffer_end);
        FixUpCharacters(CharacterFixupTablePc);
        MiniKits_Init(&permbuffer_ptr, &permbuffer_end);

        if (StartupGameFlag1 != 0) {
            default_id_a = ID_DEFAULTCHARACTER_LSW_A;
            default_id_b = ID_DEFAULTCHARACTER_LSW_B;
        } else {
            default_id_a = ID_DEFAULTCHARACTER_A;
            default_id_b = ID_DEFAULTCHARACTER_B;
        }
        id_DEFAULTCHARACTER[0] = default_id_a;
        id_DEFAULTCHARACTER[1] = default_id_b;

        CharCategories_Init(StartupGameFlag1 != 0 ? StartupCharCategoriesLswPc : StartupCharCategoriesDefaultPc);
        Cheats_Init(ExtrasCheatsPc);
        PlayerID[0] = id_DEFAULTCHARACTER[0];
        PlayerID[1] = id_DEFAULTCHARACTER[1];
        CharVariants_Init(StartupCharVariantNamesPc, 0x17);

        LDataList = Levels_ConfigureList((char *)LoadPermLevelsPath, &permbuffer_ptr, &permbuffer_end, 0x16d,
                                         &LEVELCOUNT, Level_SetDefaults);
        FixUpLevels(LevelFixupStartupPc);

        ADataList = Areas_ConfigureList((char *)LoadPermAreasPath, &permbuffer_ptr, &permbuffer_end, 0x48, &AREACOUNT);
        FixUpAreas();

        EDataList =
            Episodes_ConfigureList((char *)LoadPermEpisodesPath, &permbuffer_ptr, &permbuffer_end, 6, &EPISODECOUNT);

        NewGame();
        InitGameAfterConfig();

        if (StartupGameFlag1 == 0) {
            STUBBED_NAMED("LoadPermData: cutscene list load (cut\\cutscenes.txt at 0x57a870)");
        }

        APICharacterSysInit(&permbuffer_ptr, permbuffer_end, CHARCOUNT, 0x30, 0xe9, 0x400,
                            (APICHARACTERDATA *)CDataList, SetCreatureLights);
        SetActionInfo((API_ACTION_INFO_PC *)PlayerBlockActionInfoPc, PlayerExtraActionInfoPc);
        gizaimessagesys = CreateGizAIMessageSys(&permbuffer_ptr, &permbuffer_end, 0x40);

        permbuffer_ptr.addr = (permbuffer_ptr.addr + 0xfU) & ~0xfU;

        if (StartupGameFlag1 != 0) {
            BackDrop_Init("stuff\\starfield.gsc", &permbuffer_ptr, &permbuffer_end);
        }

        LoadPerm1();
        LoadPerm2();
        STUBBED_NAMED("LoadPermData: GameThingManager::AddOnceOnlyThings(0x95e438)");
        STUBBED_NAMED("LoadPermData: RegisterHelpers");

        blocked_keys[0] = 0x3b;
        blocked_keys[1] = 0x3c;
        blocked_keys[2] = 1;
        blocked_keys[3] = 0;
        control_config.font = SmartTextFont != NULL ? SmartTextFont : QFont2D;
        control_config.draw_background = LoadPermProgressDraw;
        control_config.rows = NULL;
        control_config.play_sound = PcControlPlaySoundPc;
        control_config.blocked_keys = blocked_keys;
        PcControlRegisterPc(&control_config, &permbuffer_ptr, &permbuffer_end);

        TextInputSetNameCallbackPc(TextInputGlyphNamePc);
        PcInputSetCommandFilterPc(PcInputCommandAllowedPc);

        permbuffer_ptr.addr = (permbuffer_ptr.addr + 0xfU) & ~0xfU;
        PermDataLoaded = 1;
    }

    EPISODEDATA_PC *__cdecl Episodes_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end, i32 max_count,
                                                   i32 *count_out) {
        NUFPAR_PC *parser = NuFParCreate(path);
        EPISODEDATA_PC *episode_base;
        EPISODEDATA_PC *episode;
        i32 episode_count = 0;
        i32 in_episode = 0;

        if (parser == NULL) {
            if (count_out != NULL) {
                *count_out = 0;
            }
            return NULL;
        }

        buffer->addr = (buffer->addr + 3) & ~3u;
        episode_base = (EPISODEDATA_PC *)buffer->void_ptr;
        episode = episode_base;

        while (NuFParGetLine(parser) != 0) {
            char *word;

            NuFParGetWord(parser);
            word = parser->word_buffer_ptr;
            if (word[0] == '\0') {
                continue;
            }

            if (!in_episode) {
                if (episode_count < max_count && NuStrICmp(word, EpisodeConfigKeyword_episode_start) == 0) {
                    episode->name_id = -1;
                    episode->text_id = -1;
                    episode->area_count = 0;
                    episode->index = (u8)episode_count;
                    in_episode = 1;
                }
                continue;
            }

            if (NuStrICmp(word, EpisodeConfigKeyword_episode_end) == 0) {
                in_episode = 0;
                if (episode->area_count != 0) {
                    episode_count++;
                    episode++;
                }
                continue;
            }

            if (NuStrICmp(word, EpisodeConfigKeyword_area) == 0) {
                if (episode->area_count <= 9 && NuFParGetWord(parser) != 0) {
                    AREA_DATA_PC *area;
                    i32 area_index;
                    i32 prior_episode_index;
                    i32 area_slot;
                    i32 found = 0;

                    area = Area_FindByName(parser->word_buffer_ptr, &area_index);
                    if (area_index != -1) {
                        for (area_slot = 0; area_slot < episode->area_count; area_slot++) {
                            if (episode->area_ids[area_slot] == area_index) {
                                found = 1;
                                break;
                            }
                        }

                        for (prior_episode_index = 0; found == 0 && prior_episode_index < episode_count;
                             prior_episode_index++) {
                            EPISODEDATA_PC *prior_episode = &episode_base[prior_episode_index];
                            for (area_slot = 0; area_slot < prior_episode->area_count; area_slot++) {
                                if (prior_episode->area_ids[area_slot] == area_index) {
                                    found = 1;
                                    break;
                                }
                            }
                        }

                        if (found == 0) {
                            i32 area_count = episode->area_count;
                            episode->area_ids[area_count] = (i16)area_index;
                            episode->area_count = (u8)(area_count + 1);
                            if ((area->flags & 6) == 0) {
                                episode->regular_areas++;
                            }
                        }
                    }
                }
                continue;
            }

            if (NuStrICmp(word, AreaConfigKeyword_name_id) == 0) {
                episode->name_id = (i16)NuFParGetInt(parser);
            } else if (NuStrICmp(word, AreaConfigKeyword_text_id) == 0) {
                episode->text_id = (i16)NuFParGetInt(parser);
            }
        }

        NuFParDestroy(parser);
        if (episode_count == 0) {
            return NULL;
        }

        buffer->void_ptr = episode;
        if (count_out != NULL) {
            *count_out = episode_count;
        }
        return episode_base;
    }

    void __cdecl StartPerm(void) {
        permbuffer_ptr.addr = permbuffer_base.addr;
        permbuffer_end.addr = superbuffer_end.addr;
    }

    void __cdecl EndPerm(void) {
        u32 aligned = (permbuffer_ptr.addr + 3U) & ~3U;
        u32 world_start;
        u32 world_mid;

        permbuffer_end.addr = aligned;
        permbuffer_size = (i32)(aligned - permbuffer_base.addr);

        characterbuffer_ptr.addr = aligned;
        characterbuffer_base.addr = aligned;
        world_start = aligned + (u32)CHARACTERBUFFERSIZE;
        characterbuffer_end.addr = world_start;
        superbuffer_ptr.addr = world_start;
        superbuffer_base.addr = world_start;

        world_mid = world_start + ((superbuffer_end.addr - world_start) >> 1);
        WorldInfo[0].giz_buffer.addr = world_start;
        WorldInfo[0].buffer_start = (void *)world_start;
        WorldInfo[0].buffer_end.addr = world_mid;
        WorldInfo[1].giz_buffer.addr = world_mid;
        WorldInfo[1].buffer_start = (void *)world_mid;
        WorldInfo[1].buffer_end.addr = superbuffer_end.addr;
        editbuffer_end.addr = superbuffer_end.addr - (u32)EDITBUFFERENDSIZE;
    }

    void __cdecl ResetCharacterBuffer(i32 force_reset) {
        if (force_reset != 0 || Area == -1 || Area != last_area) {
            characterbuffer_ptr = characterbuffer_base;
            memset(characterbuffer_base.void_ptr, 0, CHARACTERBUFFERSIZE);
            apicharsys->loaded_model_count = apicharsys->permanent_model_count;
            apicharsys->animation_load_attempts = apicharsys->area_animation_count;
            apicharsys->loaded_animation_count = apicharsys->animation_load_attempts;
        }
    }

    void __cdecl LoadPermLoadFonts(void) {
        if (StartupGameFlag0 != 0) {
            Text_LoadFont("stuff\\text\\pc_font", &permbuffer_ptr, &permbuffer_end);
        } else if (StartupGameFlag1 != 0) {
            if (Text_Language == 0)
                Text_LoadFont("stuff\\text\\starwars_font_j", &permbuffer_ptr, &permbuffer_end);
            else
                Text_LoadFont("stuff\\text\\starwars_font", &permbuffer_ptr, &permbuffer_end);
        } else {
            Text_LoadFont("stuff\\text\\batman_font", &permbuffer_ptr, &permbuffer_end);
        }
    }
}
