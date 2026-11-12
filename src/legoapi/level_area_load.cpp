#include "saga.h"
#include <string.h>
#include <stdio.h>

/* Retail unit 0x578890-0x57a810 (legoapi/levels.c): definitions in retail address order. */

#define SPLINE_POINT(spline, index) ((NUVEC *)((u8 *)(spline)->pts + (index) * (i16)(spline)->pt_size))
extern "C" {
    extern i32 RenderHubFromCutscenePlayerPc;
    extern const char RenderSplineBountyStartPc[13];
    extern const char RenderSplineShopStartPc[11];
}
STATIC_ASSERT(offsetof(WORLDINFO, camera_splines) == 0x2ac4);
STATIC_ASSERT(offsetof(LEVELSPLINE_PC, scene) == 0);
STATIC_ASSERT(offsetof(LEVELSPLINE_PC, max_points) == 0xa);
STATIC_ASSERT(offsetof(LEVELSPLINE_PC, level) == 0xc);
typedef void(__cdecl *LEVELSETDEFAULTS_PC)(LEVELDATA_PC *level);
extern "C" {
    void __cdecl LevelsConfigDiagnosticPc(char *message);
}
/* Native 0x0048a310, 217 bytes. */
typedef struct level_script_progress_pc_s {
    char name[16];
    f32 params[4];
} LEVEL_SCRIPT_PROGRESS_PC;
typedef struct ai_script_progress_view_pc_s {
    u8 unknown_0000[0x2824];
    LEVEL_SCRIPT_PROGRESS_PC scripts[32];
} AI_SCRIPT_PROGRESS_VIEW_PC;
STATIC_ASSERT(offsetof(AI_SCRIPT_PROGRESS_VIEW_PC, scripts) == 0x2824);
STATIC_ASSERT(sizeof(LEVEL_SCRIPT_PROGRESS_PC) == 0x20);
extern "C" {
    void __cdecl LevelObjectsArenaDiagnosticPc(i32 size);
}
typedef struct startup_level_object_pc_s {
    NUSPECIALHANDLE special;
    i16 platform_id;
    u8 active;
    u8 unknown_f;
} STARTUP_LEVEL_OBJECT_PC;
STATIC_ASSERT(sizeof(STARTUP_LEVEL_OBJECT_PC) == 0x10);
STATIC_ASSERT(offsetof(STARTUP_LEVEL_OBJECT_PC, special) == 0);
STATIC_ASSERT(offsetof(STARTUP_LEVEL_OBJECT_PC, platform_id) == 0xc);
extern "C" {
    i32 __cdecl FindPlatInst(i32 instance);
}
typedef struct WORLD_OBJECT_SETUP_READER_PC {
    u8 unknown_0000[0x2964];
    NUGSCN *icons_gscn;
} WORLD_OBJECT_SETUP_READER_PC;
STATIC_ASSERT(offsetof(WORLD_OBJECT_SETUP_READER_PC, icons_gscn) == 0x2964);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLDINFO, current_gscn) == 0x13c);
STATIC_ASSERT(offsetof(WORLDINFO, area) == 0x12c);
STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, active) == 0xe);
extern "C" {
    extern NUGSCN *area_scene, *vehicle_scene, *big_icon_scene;
    extern i32 KNOBS;
}
STATIC_ASSERT(sizeof(LEVEL_OBJECT_CONFIG_PC) == 8);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_CONFIG_PC, name) == 4);
typedef struct menu_mission_reader_pc_s {
    u8 unknown_00[0x12];
    i8 active_count;
} MENU_MISSION_READER_PC;
STATIC_ASSERT(offsetof(MENU_MISSION_READER_PC, active_count) == 0x12);
extern "C" {
    extern i32 SuperStoryEpisode, hub_from_superstory, hub_from_mission;
    extern i32 newlevelfrommenu_newmenuid, newlevelfrommenu_newmenuy;
}
__declspec(noinline) LEVELDATA_PC *__cdecl Area_FindStatusLevel(AREA_DATA_PC *area, i32 *index_out);

extern "C" {

    void __cdecl Level_RegisterGameConfigKeywords(NUFPCOMJMP *before_load, NUFPCOMJMP *after_load) {
        Level_ConfigBeforeLoad_GameKeywords = before_load;
        Level_ConfigAfterLoad_GameKeywords = after_load;
    }

    LEVELDATA_PC *__cdecl Levels_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end, i32 max_count,
                                               i32 *count_out, void *set_defaults_fn) {
        NUFPAR_PC *parser;
        LEVELDATA_PC *level_data;
        LEVELDATA_PC *current_level;
        i32 in_level_config = 0;
        i32 level_count = 0;
        u32 aligned_address;

        parser = NuFParCreate(path);
        if (parser == NULL)
            LevelsConfigDiagnosticPc(path);
        MAXLDATA = max_count;
        aligned_address = (buffer->addr + 3) & ~3u;
        buffer->addr = aligned_address;
        level_data = (LEVELDATA_PC *)buffer->void_ptr;
        current_level = level_data;

        while (NuFParGetLine(parser) != 0) {
            char *word;

            NuFParGetWord(parser);
            word = parser->word_buffer_ptr;
            if (word[0] == '\0') {
                continue;
            }

            if (in_level_config) {
                if (NuStrICmp(word, LevelConfigKeywordEnd) == 0) {
                    in_level_config = 0;
                    if (current_level->dir[0] == '\0' || current_level->name[0] == '\0' ||
                        (current_level->flags & 0x200) != 0) {
                        continue;
                    }

                    if ((current_level->flags & 0x10000) != 0) {
                        NEWGAME_LDATA = current_level;
                    }
                    if ((current_level->flags & 0x20000) != 0) {
                        LOADGAME_LDATA = current_level;
                    }

                    level_count++;
                    current_level++;
                    continue;
                }

                if (NuStrICmp(word, LevelConfigKeywordDir) == 0) {
                    if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buffer_ptr) < 0x40) {
                        NuStrCpy(current_level->dir, parser->word_buffer_ptr);
                    }
                } else if (NuStrICmp(word, LevelConfigKeywordFile) == 0) {
                    if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buffer_ptr) < 0x20) {
                        NuStrCpy(current_level->name, parser->word_buffer_ptr);
                    }
                }

                if (NuStrICmp(parser->word_buffer_ptr, LevelConfigKeywordTest) == 0) {
                    current_level->flags |= 0x200;
                } else if (NuStrICmp(parser->word_buffer_ptr, LevelConfigKeywordIntro) == 0) {
                    current_level->flags |= 0x20;
                } else if (NuStrICmp(parser->word_buffer_ptr, LevelConfigKeywordMidtro) == 0 ||
                           NuStrICmp(parser->word_buffer_ptr, LevelConfigKeywordCutscene) == 0) {
                    current_level->flags |= 0x40;
                } else if (NuStrICmp(parser->word_buffer_ptr, LevelConfigKeywordOutro) == 0) {
                    current_level->flags |= 0x80;
                } else if (NuStrICmp(parser->word_buffer_ptr, LevelConfigKeywordStatus) == 0) {
                    current_level->flags &= ~0x2u;
                    current_level->flags &= ~0x8u;
                    current_level->flags |= 0x400;
                } else if (NuStrICmp(parser->word_buffer_ptr, LevelConfigKeywordNewGame) == 0) {
                    if (NEWGAME_LDATA == NULL) {
                        current_level->flags |= 0x10000;
                    }
                } else if (NuStrICmp(parser->word_buffer_ptr, LevelConfigKeywordLoadGame) == 0) {
                    if (LOADGAME_LDATA == NULL) {
                        current_level->flags |= 0x20000;
                    }
                }
                continue;
            }

            if (NuStrICmp(word, LevelConfigKeywordStart) == 0 && level_count < MAXLDATA) {
                i32 music_index;

                current_level->dir[0] = '\0';
                current_level->name[0] = '\0';
                current_level->unknown_0060 = -1;
                current_level->index = (i16)level_count;
                current_level->flags = 0xe;
                current_level->callbacks[0] = NULL;
                current_level->callbacks[1] = NULL;
                current_level->callbacks[2] = NULL;
                current_level->callbacks[3] = NULL;
                current_level->callbacks[4] = NULL;
                current_level->callbacks[5] = NULL;
                current_level->callbacks[6] = NULL;

                current_level->display.farclip_hack = PCKeyboardDefaultValue_candidate;
                current_level->display.near_clip = 0.15f;
                current_level->display.far_clip = 20000;
                current_level->display.bg_red_top = 0;
                current_level->display.bg_red_bottom = 0;
                current_level->display.bg_green_top = 0;
                current_level->display.bg_green_bottom = 0;
                current_level->display.bg_blue_top = 0;
                current_level->display.bg_blue_bottom = 0;

                current_level->music_index = -1;
                current_level->sfx_ambient = -1;
                current_level->max_ter_platforms = 0x80;
                current_level->max_ter_groups = 0x100;
                current_level->music_other_index = -1;
                current_level->unknown_009e = -1;
                current_level->mipmap_mode = 3;
                current_level->blob_shadow_alpha = 0x7f;
                current_level->unknown_00a2 = 0xff;
                current_level->area_index = -1;
                current_level->cam_tilt = 0.0f;
                current_level->hover_height = 0.0f;

                current_level->water_ripple_start_rgba[0] = 0x50;
                current_level->water_ripple_start_rgba[1] = 0x50;
                current_level->water_ripple_start_rgba[2] = 0x50;
                current_level->water_ripple_start_rgba[3] = 0x32;
                current_level->water_ripple_end_rgba[0] = 0;
                current_level->water_ripple_end_rgba[1] = 0;
                current_level->water_ripple_end_rgba[2] = 0;
                current_level->water_ripple_end_rgba[3] = 0;

                current_level->water_ripple_life = 0.5f;
                current_level->cam_pullback_dist = 0.5f;
                current_level->cam_lateral_dist = 0.5f;
                current_level->reflection_y = LevelConfigFarDistance;
                current_level->area_level_index = -1;
                current_level->blob_shadow_fade_near = 5;
                current_level->blob_shadow_fade_far = 10;
                current_level->cam_pos_seek = 5;
                current_level->cam_angle_seek = 5;
                current_level->reflection_range = 10;
                current_level->raycast_step = 5;
                current_level->platform_scan_distance = 5;
                current_level->conveyor_x_speed = LevelConfigFarDistance;
                current_level->conveyor_z_speed = LevelConfigFarDistance;

                for (music_index = 0; music_index < 6; music_index++) {
                    current_level->music_tracks[music_index] = -1;
                }

                in_level_config = 1;
                if (set_defaults_fn != NULL) {
                    ((LEVELSETDEFAULTS_PC)set_defaults_fn)(current_level);
                }
            }
        }

        NuFParDestroy(parser);
        if (level_count > 0) {
            buffer->void_ptr = current_level;
            if (count_out != NULL)
                *count_out = level_count;
            return level_data;
        }
        LevelsConfigDiagnosticPc(path);
        return NULL;
    }

    LEVELDATA_PC *__cdecl Level_FindByName(char *name, i32 *index_out) {
        i32 index;

        for (index = 0; index < LEVELCOUNT; ++index) {
            if (NuStrICmp(LDataList[index].name, name) == 0) {
                if (index_out != NULL) {
                    *index_out = index;
                }
                return &LDataList[index];
            }
        }

        if (index_out != NULL) {
            *index_out = -1;
        }
        return NULL;
    }

    void __cdecl Levels_FixUp(LEVELFIXUP_PC *fixup) {
        LEVELDATA_PC *level;
        if (fixup != NULL) {
            for (; fixup->name != NULL; ++fixup) {
                if (fixup->level != NULL) {
                    level = Level_FindByName(fixup->name, NULL);
                    *fixup->level = level;
                    if (level != NULL) {
                        if (fixup->load != NULL)
                            level->callbacks[0] = fixup->load;
                        if (fixup->init != NULL)
                            level->callbacks[1] = fixup->init;
                        if (fixup->reset != NULL)
                            level->callbacks[2] = fixup->reset;
                        if (fixup->update != NULL)
                            level->callbacks[3] = fixup->update;
                        if (fixup->always_update != NULL)
                            level->callbacks[4] = fixup->always_update;
                        if (fixup->draw != NULL)
                            level->callbacks[5] = fixup->draw;
                        if (fixup->draw_status != NULL)
                            level->callbacks[6] = fixup->draw_status;
                    }
                }
            }
        }
    }

    void __cdecl LC_BL_mipmapmode(NUFPAR_PC *parser) {
        levelconfig_ldata->mipmap_mode = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_BL_fix_strobing_anims(NUFPAR_PC *parser) {
        (void)parser;
        levelconfig_ldata->flags |= 0x100;
    }

    void __cdecl LC_BL_max_ter_groups(NUFPAR_PC *parser) {
        levelconfig_ldata->max_ter_groups = (i16)NuFParGetInt(parser);
    }

    void __cdecl LC_BL_max_ter_platforms(NUFPAR_PC *parser) {
        levelconfig_ldata->max_ter_platforms = (i16)NuFParGetInt(parser);
    }

    void __cdecl LevelConfig_BeforeLoad(LEVELDATA_PC *level, char *buffer, NUFPCOMJMP *keywords) {
        NUFPAR_PC *parser = NuFParCreateMem((char *)LevelBeforeLoadName, buffer, 0xffff);

        if (parser == NULL) {
            return;
        }

        levelconfig_ldata = level;
        NuFParPushCom2(parser, LevelConfig_BeforeLoad_GenericKeywords, keywords);
        while (NuFParGetLine(parser) != 0) {
            if (NuFParGetWord(parser) != 0) {
                NuFParInterpretWord(parser);
            }
        }
        NuFParDestroy(parser);
    }

    void __cdecl LC_AL_backr(NUFPAR_PC *parser) {
        levelconfig_ldata->display.bg_red_bottom = (u8)NuFParGetInt(parser);
        levelconfig_ldata->display.bg_red_top = levelconfig_ldata->display.bg_red_bottom;
    }

    void __cdecl LC_AL_backg(NUFPAR_PC *parser) {
        levelconfig_ldata->display.bg_green_bottom = (u8)NuFParGetInt(parser);
        levelconfig_ldata->display.bg_green_top = levelconfig_ldata->display.bg_green_bottom;
    }

    void __cdecl LC_AL_backb(NUFPAR_PC *parser) {
        levelconfig_ldata->display.bg_blue_bottom = (u8)NuFParGetInt(parser);
        levelconfig_ldata->display.bg_blue_top = levelconfig_ldata->display.bg_blue_bottom;
    }

    void __cdecl LC_AL_backr_top(NUFPAR_PC *parser) {
        levelconfig_ldata->display.bg_red_top = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_backg_top(NUFPAR_PC *parser) {
        levelconfig_ldata->display.bg_green_top = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_backb_top(NUFPAR_PC *parser) {
        levelconfig_ldata->display.bg_blue_top = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_backr_bottom(NUFPAR_PC *parser) {
        levelconfig_ldata->display.bg_red_bottom = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_backg_bottom(NUFPAR_PC *parser) {
        levelconfig_ldata->display.bg_green_bottom = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_backb_bottom(NUFPAR_PC *parser) {
        levelconfig_ldata->display.bg_blue_bottom = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_farclip_hack(NUFPAR_PC *parser) {
        f32 value = NuFParGetFloat(parser);

        if (value < 0.1f) {
            value = 0.1f;
        } else if (value > 50.0) {
            value = 50.0;
        }
        levelconfig_ldata->display.farclip_hack = value;
    }

    void __cdecl LC_AL_farclip(NUFPAR_PC *parser) {
        i32 value = NuFParGetInt(parser);

        if (value < 10) {
            value = 10;
        } else if (value > 20000) {
            value = 20000;
        }
        levelconfig_ldata->display.far_clip = (i16)value;
    }

    void __cdecl LC_AL_nearclip(NUFPAR_PC *parser) {
        f32 value = NuFParGetFloat(parser);

        if (value < 0.001f) {
            value = 0.001f;
        } else if (value > 1.0f) {
            value = 1.0f;
        }
        levelconfig_ldata->display.near_clip = value;
    }

    void __cdecl LC_AL_hover_height(NUFPAR_PC *parser) {
        levelconfig_ldata->hover_height = NuFParGetFloat(parser);
    }

    void __cdecl LC_AL_blobshadow_alpha(NUFPAR_PC *parser) {
        levelconfig_ldata->blob_shadow_alpha = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_blobshadow_fadenear(NUFPAR_PC *parser) {
        levelconfig_ldata->blob_shadow_fade_near = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_blobshadow_fadefar(NUFPAR_PC *parser) {
        levelconfig_ldata->blob_shadow_fade_far = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_reflect_range(NUFPAR_PC *parser) {
        levelconfig_ldata->reflection_range = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_reflect_y(NUFPAR_PC *parser) {
        levelconfig_ldata->reflection_y = NuFParGetFloat(parser);
    }

    void __cdecl LC_AL_metal(NUFPAR_PC *parser) {
        levelconfig_ldata->flags |= 0x1000;
        if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, LevelConfigOff) == 0) {
            levelconfig_ldata->flags &= ~0x1000u;
        }
    }

    void __cdecl LC_AL_in_space(NUFPAR_PC *parser) {
        levelconfig_ldata->flags |= 0x40000;
        if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, LevelConfigOff) == 0) {
            levelconfig_ldata->flags &= ~0x40000u;
        }
    }

    void __cdecl LC_AL_override_nopickupgravity(NUFPAR_PC *parser) {
        levelconfig_ldata->flags |= 0x400000;
        if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, LevelConfigOff) == 0) {
            levelconfig_ldata->flags &= ~0x400000u;
        }
    }

    void __cdecl LC_AL_hidden_icons(NUFPAR_PC *parser) {
        if (NuFParGetWord(parser) != 0) {
            if (NuStrICmp(parser->word_buffer_ptr, LevelConfigOn) == 0) {
                levelconfig_ldata->flags &= ~0x800000u;
                return;
            }
            if (NuStrICmp(parser->word_buffer_ptr, LevelConfigOff) == 0) {
                levelconfig_ldata->flags |= 0x800000;
            }
        }
    }

    void __cdecl LC_AL_pickups_to_panel(NUFPAR_PC *parser) {
        levelconfig_ldata->flags |= 0x80000;
        if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, LevelConfigOff) == 0) {
            levelconfig_ldata->flags &= ~0x80000u;
        }
    }

    void __cdecl LC_AL_forget_takeovers(NUFPAR_PC *parser) {
        levelconfig_ldata->flags |= 0x100000;
        if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, LevelConfigOff) == 0) {
            levelconfig_ldata->flags &= ~0x100000u;
        }
    }

    void __cdecl LC_AL_narrow_socks(NUFPAR_PC *parser) {
        levelconfig_ldata->flags |= 0x200000;
        if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, LevelConfigOff) == 0) {
            levelconfig_ldata->flags &= ~0x200000u;
        }
    }

    void __cdecl LC_AL_camera_rain(NUFPAR_PC *parser) {
        levelconfig_ldata->flags |= 0x4000;
        if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, LevelConfigOff) == 0) {
            levelconfig_ldata->flags &= ~0x4000u;
        }
    }

    void __cdecl LC_AL_terrain_rain(NUFPAR_PC *parser) {
        levelconfig_ldata->flags |= 0x8000;
        if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, LevelConfigOff) == 0) {
            levelconfig_ldata->flags &= ~0x8000u;
        }
    }

    void __cdecl LC_AL_double_score(NUFPAR_PC *parser) {
        levelconfig_ldata->flags |= 0x800;
        if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, LevelConfigOff) == 0) {
            levelconfig_ldata->flags &= ~0x800u;
        }
    }

    void __cdecl LC_AL_flat_terrain(NUFPAR_PC *parser) {
        levelconfig_ldata->flags |= 0x10;
        if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, LevelConfigOff) == 0) {
            levelconfig_ldata->flags &= ~0x10u;
        }
    }

    void __cdecl LC_AL_cam_tilt(NUFPAR_PC *parser) {
        levelconfig_ldata->cam_tilt = NuFParGetFloat(parser);
    }

    void __cdecl LC_AL_raycaststep(NUFPAR_PC *parser) {
        levelconfig_ldata->raycast_step = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_plat_scan_dist(NUFPAR_PC *parser) {
        levelconfig_ldata->platform_scan_distance = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_waterripple_startcol_r(NUFPAR_PC *parser) {
        levelconfig_ldata->water_ripple_start_rgba[0] = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_waterripple_startcol_g(NUFPAR_PC *parser) {
        levelconfig_ldata->water_ripple_start_rgba[1] = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_waterripple_startcol_b(NUFPAR_PC *parser) {
        levelconfig_ldata->water_ripple_start_rgba[2] = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_waterripple_startcol_a(NUFPAR_PC *parser) {
        levelconfig_ldata->water_ripple_start_rgba[3] = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_waterripple_endcol_r(NUFPAR_PC *parser) {
        levelconfig_ldata->water_ripple_end_rgba[0] = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_waterripple_endcol_g(NUFPAR_PC *parser) {
        levelconfig_ldata->water_ripple_end_rgba[1] = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_waterripple_endcol_b(NUFPAR_PC *parser) {
        levelconfig_ldata->water_ripple_end_rgba[2] = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_waterripple_endcol_a(NUFPAR_PC *parser) {
        levelconfig_ldata->water_ripple_end_rgba[3] = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_waterripple_life(NUFPAR_PC *parser) {
        levelconfig_ldata->water_ripple_life = NuFParGetFloat(parser);
    }

    void __cdecl LC_AL_cam_pullback_dist(NUFPAR_PC *parser) {
        f32 value = NuFParGetFloat(parser);

        if (value < 0.0f) {
            value = 0.0f;
        }
        levelconfig_ldata->cam_pullback_dist = value;
    }

    void __cdecl LC_AL_cam_lateral_dist(NUFPAR_PC *parser) {
        f32 value = NuFParGetFloat(parser);

        if (value < 0.0f) {
            value = 0.0f;
        }
        levelconfig_ldata->cam_lateral_dist = value;
    }

    void __cdecl LC_AL_campos_seek(NUFPAR_PC *parser) {
        levelconfig_ldata->cam_pos_seek = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_camang_seek(NUFPAR_PC *parser) {
        levelconfig_ldata->cam_angle_seek = (u8)NuFParGetInt(parser);
    }

    void __cdecl LC_AL_shadowtype(NUFPAR_PC *parser) {
        levelconfig_ldata->shadow_type = NuFParGetInt(parser);
    }
}

extern "C" void __cdecl LC_AL_music(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) == 0) {
        return;
    }

    levelconfig_ldata->music_index = GetMusicIndex(parser->word_buffer_ptr, MusicInfo, -1);
    levelconfig_ldata->music_tracks[0] = music_man.GetTrackHandle(1, parser->word_buffer_ptr);
    levelconfig_ldata->music_tracks[2] = music_man.GetTrackHandle(2, parser->word_buffer_ptr);
    levelconfig_ldata->music_tracks[4] = music_man.GetTrackHandle(0x20, parser->word_buffer_ptr);
}

extern "C" void __cdecl LC_AL_music_other(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) == 0) {
        return;
    }

    levelconfig_ldata->music_other_index = GetMusicIndex(parser->word_buffer_ptr, MusicInfo, -1);
    levelconfig_ldata->music_tracks[1] = music_man.GetTrackHandle(1, parser->word_buffer_ptr);
    levelconfig_ldata->music_tracks[3] = music_man.GetTrackHandle(2, parser->word_buffer_ptr);
    levelconfig_ldata->music_tracks[5] = music_man.GetTrackHandle(0x20, parser->word_buffer_ptr);
}
extern "C" {

    void __cdecl LC_AL_sfx_ambient(NUFPAR_PC *parser) {
        if (NuFParGetWord(parser) != 0) {
            levelconfig_ldata->sfx_ambient = (i16)GetSfxId(parser->word_buffer_ptr);
        }
    }

    void __cdecl LC_AL_conveyor(NUFPAR_PC *parser) {
        while (NuFParGetWord(parser) != 0) {
            if (NuStrICmp(parser->word_buffer_ptr, LevelConfigKeyword_xspeed) == 0) {
                levelconfig_ldata->conveyor_x_speed = NuFParGetFloat(parser);
            } else if (NuStrICmp(parser->word_buffer_ptr, LevelConfigKeyword_zspeed) == 0) {
                levelconfig_ldata->conveyor_z_speed = NuFParGetFloat(parser);
            }
        }
    }

    void __cdecl LevelConfig_AfterLoad(LEVELDATA_PC *level, char *buffer, NUFPCOMJMP *keywords) {
        NUFPAR_PC *parser = NuFParCreateMem((char *)LevelAfterLoadName, buffer, 0xffff);

        if (parser == NULL) {
            return;
        }

        levelconfig_ldata = level;
        NuFParPushCom2(parser, LevelConfig_AfterLoad_GenericKeywords, keywords);
        while (NuFParGetLine(parser) != 0) {
            if (NuFParGetWord(parser) != 0) {
                NuFParInterpretWord(parser);
            }
        }
        NuFParDestroy(parser);

        if (level->blob_shadow_fade_near > level->blob_shadow_fade_far) {
            level->blob_shadow_fade_near = level->blob_shadow_fade_far;
        }
        level->flags |= 1;
    }

    void __cdecl LevelSplines_InitForGame(LEVELSPLINE_PC *table) {
        LEVELSPLINE_PC *record;

        LevSplList = table;
        LEVELSPLINECOUNT = 0;
        if (table == NULL || table[0].name == NULL) {
            return;
        }
        for (record = &table[0]; record->name != NULL; record++) {
            if (levspl_i_start == -1 && NuStrICmp(record->name, LevelSplinesStartName) == 0) {
                levspl_i_start = LEVELSPLINECOUNT;
            } else if (levspl_i_startcam == -1 && NuStrICmp(record->name, LevelSplinesStartCamName) == 0) {
                levspl_i_startcam = LEVELSPLINECOUNT;
            }
            LEVELSPLINECOUNT++;
        }
    }

    void __cdecl LevelSplines_InitForLevel(WORLDINFO *world) {
        i32 i;
        world->giz_buffer.addr = (world->giz_buffer.addr + 3u) & ~3u;
        world->camera_splines = (NUGSPLINE **)world->giz_buffer.void_ptr;
        world->giz_buffer.addr += LEVELSPLINECOUNT * sizeof(*world->camera_splines);
        memset(world->camera_splines, 0, LEVELSPLINECOUNT * sizeof(*world->camera_splines));
        if (LevSplList == NULL)
            return;
        for (i = 0; i < LEVELSPLINECOUNT; ++i) {
            LEVELSPLINE_PC *entry = &LevSplList[i];
            NUGSCN *scene;
            NUGSPLINE *spline;
            char name[64];
            if ((entry->area != -1 && world->area_index != entry->area) ||
                (entry->level != -1 && world->level_idx != entry->level))
                continue;
            scene = *(entry->scene != NULL ? entry->scene : &world->current_gscn);
            if (scene != NULL) {
                spline = NuSplineFind(scene, (char *)entry->name);
                world->camera_splines[i] = spline;
                if (spline != NULL && ((entry->min_points > 0 && spline->length < entry->min_points) ||
                                       (entry->max_points > 0 && entry->max_points >= entry->min_points &&
                                        spline->length > entry->max_points)))
                    world->camera_splines[i] = NULL;
            }
            if (levspl_i_start != -1) {
                name[0] = '\0';
                if (Mission_Active(NULL) != NULL)
                    NuStrCpy(name, RenderSplineBountyStartPc);
                else if (world->area_index != -1 && (ADataList[world->area_index].flags & 0x40) &&
                         RenderHubFromCutscenePlayerPc != 0)
                    NuStrCpy(name, RenderSplineShopStartPc);
                if (name[0] != '\0') {
                    spline = NuSplineFind(scene, name);
                    if (spline != NULL && spline->length >= 2) {
                        world->camera_splines[levspl_i_start] = spline;
                        if (levspl_i_startcam != -1)
                            world->camera_splines[levspl_i_startcam] = NULL;
                    }
                }
            }
        }
    }
    LEVEL_OBJECT_CONFIG_PC *ObjTabList = 0;
    i32 LEVELOBJECTCOUNT = 0;
    static i32 LEVELOBJECTMAX = 0;
    static char *ExtraLevelObject_NameTable = 0;
    static i32 ExtraLevelObject_NameTableSize = 0;
    i32 LEVOBJREF_FIRSTOBJ = -1;
    i32 LEVOBJREF_LASTOBJ = -1;
    i32 LEVOBJREF_FIRSTREFOBJ = -1;
    i32 LEVOBJREF_LASTREFOBJ = -1;

    void __cdecl LevelObjects_InitForGame(LEVEL_OBJECT_CONFIG_PC *table, VARIPTR *buffer, VARIPTR *buffer_end,
                                          i32 maximum, i32 name_table_size) {
        i32 reflection_range;
        i32 object_range;
        LEVOBJREF_FIRSTOBJ = -1;
        LEVOBJREF_LASTOBJ = -1;
        LEVOBJREF_FIRSTREFOBJ = -1;
        LEVOBJREF_LASTREFOBJ = -1;
        ObjTabList = table;
        LEVELSPLINECOUNT = 0;
        LEVELOBJECTMAX = maximum;
        for (; table->kind != 0xff; ++table, ++LEVELOBJECTCOUNT) {
            if (table->reflection == 1) {
                LEVOBJREF_LASTOBJ = LEVELOBJECTCOUNT;
                if (LEVOBJREF_FIRSTOBJ == -1)
                    LEVOBJREF_FIRSTOBJ = LEVELOBJECTCOUNT;
            }
            if (table->reflection == 2) {
                LEVOBJREF_LASTREFOBJ = LEVELOBJECTCOUNT;
                if (LEVOBJREF_FIRSTREFOBJ == -1)
                    LEVOBJREF_FIRSTREFOBJ = LEVELOBJECTCOUNT;
            }
        }
        object_range = LEVOBJREF_LASTOBJ - LEVOBJREF_FIRSTOBJ;
        reflection_range = LEVOBJREF_LASTREFOBJ - LEVOBJREF_FIRSTREFOBJ;
        if (reflection_range < object_range)
            LEVOBJREF_LASTOBJ = LEVOBJREF_FIRSTOBJ + reflection_range;
        else if (reflection_range > object_range)
            LEVOBJREF_LASTREFOBJ = LEVOBJREF_FIRSTREFOBJ + object_range;
        if (name_table_size > 0) {
            ExtraLevelObject_NameTable = (char *)buffer->void_ptr;
            ExtraLevelObject_NameTableSize = name_table_size;
            buffer->addr += name_table_size;
            if (buffer->addr >= buffer_end->addr)
                LevelObjectsArenaDiagnosticPc(name_table_size);
        }
    }
    i32 EXTRALEVELOBJECTCOUNT = 0;
    static i32 ExtraLevelObject_NameTableIndex = 0;

    i32 __cdecl LevelObject_AddExtra(char *name, i32 kind) {
        i32 length;
        char *destination;
        if (LEVELOBJECTCOUNT >= LEVELOBJECTMAX)
            return 0;
        if (ExtraLevelObject_NameTable == NULL)
            return 0;
        length = NuStrLen(name);
        if (length + ExtraLevelObject_NameTableIndex + 1 >= ExtraLevelObject_NameTableSize)
            return 0;
        ObjTabList[LEVELOBJECTCOUNT].kind = (u8)kind;
        destination = ExtraLevelObject_NameTable + ExtraLevelObject_NameTableIndex;
        ObjTabList[LEVELOBJECTCOUNT].name = destination;
        ++LEVELOBJECTCOUNT;
        ++EXTRALEVELOBJECTCOUNT;
        NuStrCpy(destination, name);
        ExtraLevelObject_NameTableIndex += length + 1;
        return 1;
    }

    void __cdecl LevelObjects_InitForLevel(WORLDINFO *world) {
        i32 index;
        u32 size;
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->level_objects = (LEVEL_OBJECT_RUNTIME_PC *)world->giz_buffer.void_ptr;
        size = LEVELOBJECTCOUNT * sizeof(LEVEL_OBJECT_RUNTIME_PC);
        world->giz_buffer.addr += size;
        memset(world->level_objects, 0, size);
        if (ObjTabList == NULL)
            return;
        for (index = 0; index < LEVELOBJECTCOUNT; ++index) {
            NUGSCN *scene;
            i32 found;
            i32 area_override;
            if (NuStrICmp(ObjTabList[index].name, "power_up") == 0)
                KNOBS = index;
            found = 0;
            area_override = 0;
            switch ((i8)ObjTabList[index].kind) {
                case 1:
                    scene = world->current_gscn;
                    break;
                case 2:
                    scene = area_scene != NULL ? area_scene : world->current_gscn;
                    break;
                case 3: {
                    i32 character;
                    for (character = 0; !found && character < CHARCOUNT; ++character) {
                        scene = IconScene_FindById(character);
                        if (scene != NULL &&
                            NuSpecialFind(scene, &world->level_objects[index].special, ObjTabList[index].name, 1) != 0)
                            found = 1;
                    }
                    if (found)
                        goto resolved;
                    scene =
                        big_icon_scene != NULL ? big_icon_scene : ((WORLD_OBJECT_SETUP_READER_PC *)world)->icons_gscn;
                    break;
                }
                case 4:
                    scene = save_icon_scene;
                    break;
                case 5:
                    scene = vehicle_scene;
                    break;
                default:
                    scene = area_scene;
                    if (scene != NULL && world->area != NULL && (world->area->flags & 0x400) != 0)
                        area_override = 1;
                    else
                        scene = things_scene;
                    break;
            }
            if (scene == NULL)
                continue;
            if (NuSpecialFind(scene, &world->level_objects[index].special, ObjTabList[index].name, 1) == 0 &&
                area_override != 0)
                NuSpecialFind(things_scene, &world->level_objects[index].special, ObjTabList[index].name, 1);
        resolved:
            world->level_objects[index].active = (u8)NuSpecialExistsFn(&world->level_objects[index].special);
            if (world->level_objects[index].active != 0)
                NuSpecialSetVisibility(&world->level_objects[index].special, 0);
        }
    }

    i32 __cdecl LevelObject_FindIndexFromName(char *name) {
        i32 i;
        if (ObjTabList == NULL)
            return -1;
        if (LEVELOBJECTCOUNT <= 0)
            return -1;
        for (i = 0; i < LEVELOBJECTCOUNT; ++i) {
            if (NuStrICmp(ObjTabList[i].name, name) == 0) {
                return i;
            }
        }
        return -1;
    }

    void __cdecl LevObj_FixUpPlatIDs(WORLDINFO *world) {
        i32 index;
        if (ObjTabList == NULL)
            return;
        for (index = 0; index < LEVELOBJECTCOUNT; ++index) {
            ((STARTUP_LEVEL_OBJECT_PC *)world->level_objects)[index].platform_id = -1;
            if (world->terrain != NULL &&
                NuSpecialExistsFn(&((STARTUP_LEVEL_OBJECT_PC *)world->level_objects)[index].special) &&
                ObjTabList[index].kind == 1)
                ((STARTUP_LEVEL_OBJECT_PC *)world->level_objects)[index].platform_id = (i16)FindPlatInst(
                    NuSpecialGetInstanceix(&((STARTUP_LEVEL_OBJECT_PC *)world->level_objects)[index].special));
        }
    }

    char *__cdecl LevelObject_FindNameFromIndex(i32 index) {
        char *name = "";
        if (ObjTabList == NULL || index < 0 || index >= LEVELOBJECTCOUNT)
            return name;
        name = ObjTabList[index].name;
        return name;
    }

    i32 __cdecl LevelObject_GetReflection(i32 index) {
        if (index != -1 && index >= LEVOBJREF_FIRSTOBJ && index <= LEVOBJREF_LASTOBJ)
            return LEVOBJREF_FIRSTREFOBJ - LEVOBJREF_FIRSTOBJ + index;
        return index;
    }

    i32 __cdecl LevelObject_FindIndexFromName_RefOnly(char *name) {
        i32 end, i;
        if (ObjTabList == NULL)
            return -1;
        if (LEVOBJREF_FIRSTOBJ == -1)
            return -1;
        end = LEVOBJREF_LASTOBJ - LEVOBJREF_FIRSTOBJ + 1;
        for (i = LEVOBJREF_FIRSTOBJ; i < end; ++i)
            if (NuStrICmp(ObjTabList[i].name, name) == 0)
                return i;
        return -1;
    }

    void __cdecl Level_LoadConfigFile(WORLDINFO *world) {
        char name[128];
        i32 bytes_read;

        ConfigBuffer.bytes[0] = '\0';
        sprintf(name, WorldLevelConfigFormat, world->config_file);
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3U;
        bytes_read = NuFileLoadBuffer(name, world->giz_buffer.void_ptr, 0x10000);
        world->config_count = bytes_read;
        if (bytes_read > 0) {
            world->giz_buffer.char_ptr[bytes_read] = '\0';
            bytes_read = Text_StripComments(world->giz_buffer.char_ptr, ConfigBuffer.bytes, 1);
            world->config_count = bytes_read;
        }
    }

    void __cdecl Level_Update(WORLDINFO *world) {
        void(__cdecl * update)(WORLDINFO *) = (void(__cdecl *)(WORLDINFO *))world->current_level->callbacks[3];
        if (update != NULL)
            update(world);
        if (DoubleScoreTime > 0.0f &&
            (i32)((f64)GameTimer.time_elapsed * 2.0) != (i32)((f64)GameTimer.last_time_elapsed * 2.0))
            GameAudio_PlaySfxAndSetVolume(0x35, NULL, DoubleScoreTime);
        if (AreaGlobals_PC.field_00 == 0 && Cheats_CheckFlags(0x2000) != 0)
            AreaGlobals_PC.field_00 = 1;
    }

    void __cdecl Level_Draw(WORLDINFO *world) {
        void(__cdecl * draw)(WORLDINFO *) = (void(__cdecl *)(WORLDINFO *))world->current_level->callbacks[5];
        if (draw != NULL)
            draw(world);
    }
}

void __cdecl GoToNewLevel(i32 level_index) {
    NewLData = &LDataList[level_index];
    if (waiting_for_level != -1)
        waiting_for_new_level = 1;
}

void __cdecl CompleteLevel(WORLDINFO *world) {
    LEVELDATA_PC *level;

    level = NewLData;
    LevelCompletionFallbackFlagPC = 1u;
    if (level == NULL || ((level->flags & 0xe0u) != 0 && FreePlay != 0)) {
        level = Area_FindStatusLevel(world->area, NULL);
        NewLData = level;
    }
    if (level == NULL)
        NewLData = HUB_LDATA;
    if (waiting_for_level != -1)
        waiting_for_new_level = 1;
}
extern "C" {

    void __cdecl ResetLevel(WORLDINFO *world, char *cutscene, i32 unused) {
        CutDeadVehiclePlayer = NULL;
        if (world != NULL && NewCutScene(NULL, world->cutscene_sys, cutscene, 0) != NULL) {
            ResetBits |= 7;
            if (player != NULL && player->dead != 0)
                CutDeadVehiclePlayer = player;
            else if (player2 != NULL && player2->dead != 0)
                CutDeadVehiclePlayer = player2;
            reset_reimport = 1;
        } else {
            NewGameMode();
            ResetBits |= 0x17;
            reset_restart = 1;
            LevelCompletionFallbackFlagPC = 1;
            GizmoTypeStoreProgress((GIZMOSYS_PC *)WORLD->gizmo_sys, WORLD, WORLD->current_level->area_level_index,
                                   gizmopickup_typeid, NULL);
        }
    }

    void __cdecl NewLevelFromMenu(LEVELDATA_PC *level, i32 menu_id, i32 menu_y, i32 ignored) {
        MISSIONDATA_PC *mission;
        (void)ignored;
        LOG_INFO("Menu level request: level=%p menu=%d row=%d", (void *)level, menu_id, menu_y);
        if (no_more_loads == 0) {
            abort_load = 1;
            no_more_loads = 1;
        }
        Door_Reset();
        NewLData = level;
        new_level_from_menu = 1;
        newlevelfrommenu_newmenuid = menu_id;
        newlevelfrommenu_newmenuy = menu_y;
        if (HUB_ADATA != NULL && WORLD->area == HUB_ADATA)
            return;
        if (SuperStory != 0) {
            hub_from_superstory = SuperStoryEpisode;
            return;
        }
        mission = Mission_Active(NULL);
        if (mission != NULL && NewLData != NULL && NewLData == HUB_LDATA)
            hub_from_mission = ((MENU_MISSION_READER_PC *)mission)->active_count;
    }

    void LevelStreaming_DoorOverride(WORLDINFO *world, LEVELDATA_PC *level, f32 radius, f32 *distance) {
        NUVEC position;
        if (level != NULL && Players_AveragePos(&position, NULL) != 0) {
            DOOR_s *door = (DOOR_s *)Door_FindByIndex(world, -1, level->index, &position);
            if (door != NULL) {
                f32 distance_squared = NuVecDistSqr(&position, &door->pos, NULL);
                if (distance_squared < radius * radius) {
                    other_level_override = level->index;
                    if (distance != NULL) {
                        *distance = NuFsqrt(distance_squared);
                    }
                }
            }
        }
    }
}
