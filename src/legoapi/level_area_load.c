#include "saga.h"

typedef void(__cdecl *LEVELSETDEFAULTS_PC)(LEVELDATA_PC *level);

#define LEVEL_BYTE_CALLBACK(name, field)                                                                               \
    void __cdecl name(NUFPAR_PC *parser) {                                                                             \
        levelconfig_ldata->field = (u8)NuFParGetInt(parser);                                                           \
    }

#define LEVEL_WORD_CALLBACK(name, field)                                                                               \
    void __cdecl name(NUFPAR_PC *parser) {                                                                             \
        levelconfig_ldata->field = (i16)NuFParGetInt(parser);                                                          \
    }

LEVEL_BYTE_CALLBACK(LC_BL_max_tightropes, max_tightropes)
LEVEL_BYTE_CALLBACK(LC_BL_max_signals, max_signals)
LEVEL_BYTE_CALLBACK(LC_BL_max_levers, max_levers)
LEVEL_BYTE_CALLBACK(LC_BL_max_technos, max_technos)
LEVEL_BYTE_CALLBACK(LC_BL_max_zipups, max_zipups)
LEVEL_BYTE_CALLBACK(LC_BL_max_grapples, max_grapples)
LEVEL_BYTE_CALLBACK(LC_BL_max_obstacles, max_obstacles)
LEVEL_WORD_CALLBACK(LC_BL_max_obstacle_objects, max_obstacle_objects)
LEVEL_BYTE_CALLBACK(LC_BL_max_turrets, max_turrets)
LEVEL_BYTE_CALLBACK(LC_BL_max_buildits, max_buildits)
LEVEL_WORD_CALLBACK(LC_BL_max_buildit_objects, max_buildit_objects)
LEVEL_BYTE_CALLBACK(LC_BL_max_spinneranim_objs, max_spinneranim_objs)
LEVEL_BYTE_CALLBACK(LC_BL_max_shards, max_shards)
LEVEL_BYTE_CALLBACK(LC_BL_max_minicuts, max_minicuts)
LEVEL_BYTE_CALLBACK(LC_BL_max_minicutParts, max_minicut_stages)
LEVEL_BYTE_CALLBACK(LC_BL_max_climb_objects, max_climb_objects)
LEVEL_BYTE_CALLBACK(LC_BL_max_attractos, max_attractos)
LEVEL_BYTE_CALLBACK(LC_BL_max_timers, max_timers)
LEVEL_BYTE_CALLBACK(LC_BL_max_guidelines, max_guidelines)
LEVEL_BYTE_CALLBACK(LC_BL_max_ledges, max_ledges)
LEVEL_BYTE_CALLBACK(LC_BL_max_securitydoors, max_securitydoors)
LEVEL_BYTE_CALLBACK(LC_BL_max_tubes, max_tubes)
LEVEL_WORD_CALLBACK(LC_BL_max_pickups, max_pickups)
LEVEL_WORD_CALLBACK(LC_BL_max_gameantinodes, max_gameantinodes)
LEVEL_BYTE_CALLBACK(LC_BL_max_gizpanels, max_gizpanels)
LEVEL_BYTE_CALLBACK(LC_BL_max_force, max_force)
LEVEL_WORD_CALLBACK(LC_BL_max_force_objects, max_force_objects)
LEVEL_BYTE_CALLBACK(LC_BL_max_gizspecials, max_gizspecials)
LEVEL_BYTE_CALLBACK(LC_BL_max_hatmachines, max_hatmachines)
LEVEL_BYTE_CALLBACK(LC_BL_max_doors, max_doors)
LEVEL_BYTE_CALLBACK(LC_BL_max_teleports, max_teleports)
LEVEL_BYTE_CALLBACK(LC_BL_max_gizrandoms, max_gizrandoms)
LEVEL_BYTE_CALLBACK(LC_BL_max_torpmachines, max_torpmachines)
LEVEL_BYTE_CALLBACK(LC_BL_max_bridges, max_bridges)
LEVEL_BYTE_CALLBACK(LC_BL_max_plugs, max_plugs)
LEVEL_WORD_CALLBACK(LC_BL_max_bombgen_objects, max_bombgen_objects)
LEVEL_BYTE_CALLBACK(LC_BL_max_bombgens, max_bombgens)

void __cdecl LC_BL_max_spinners(NUFPAR_PC *parser) {
    levelconfig_ldata->max_spinners = (u8)NuFParGetInt(parser);
    if (levelconfig_ldata->max_spinners > 8) {
        levelconfig_ldata->max_spinners = 8;
    }
}

void __cdecl LC_BL_max_gizmoblowuptypes(NUFPAR_PC *parser) {
    levelconfig_ldata->max_gizmoblowuptypes = (i16)NuFParGetInt(parser);
    if ((u16)levelconfig_ldata->max_gizmoblowuptypes > 0xff) {
        levelconfig_ldata->max_gizmoblowuptypes = 0xff;
    }
}

void __cdecl LC_BL_max_gizmoblowups(NUFPAR_PC *parser) {
    levelconfig_ldata->max_gizmoblowups = (i16)NuFParGetInt(parser);
    if ((u16)levelconfig_ldata->max_gizmoblowups > 0x200) {
        levelconfig_ldata->max_gizmoblowups = 0x200;
    }
}

void __cdecl LC_BL_max_pushblocks(NUFPAR_PC *parser) {
    levelconfig_ldata->max_pushblocks = (u8)NuFParGetInt(parser);
    if (levelconfig_ldata->max_pushblocks > 0x10) {
        levelconfig_ldata->max_pushblocks = 0x10;
    }
}

void __cdecl LC_BL_max_pushblock_endpos(NUFPAR_PC *parser) {
    levelconfig_ldata->max_pushblock_endpos = (u8)NuFParGetInt(parser);
    if (levelconfig_ldata->max_pushblock_endpos > 0x40) {
        levelconfig_ldata->max_pushblock_endpos = 0x40;
    }
}

void __cdecl LC_BL_wind_speed(NUFPAR_PC *parser) {
    levelconfig_ldata->wind_speed = NuFParGetFloat(parser);
}

void __cdecl LC_BL_wind_size(NUFPAR_PC *parser) {
    levelconfig_ldata->wind_size = NuFParGetFloat(parser);
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
    if (level_count == 0) {
        return NULL;
    }

    buffer_end->void_ptr = current_level;
    if (count_out != NULL) {
        *count_out = level_count;
    }
    return level_data;
}
