#include "batman.h"
#include "decomp.h"
#include "globals.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/screens/gamestructure.h"
#include "legoapi/misc/buffers.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/screen.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/world/area.h"
#include "legoapi/world/areas.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "legogame/game.h"
#include "legogame/startup.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic/numusic.h"
#include "nu2api/numusic/sfx.h"
#include <string.h>
#include <stdio.h>

typedef void (*LEVELSETDEFAULTS)(LEVELDATA *level);
extern "C" {
    void LevelsConfigDiagnostic(char *message);
}

extern "C" {
    void LevelObjectsArenaDiagnostic(i32 size);
}

typedef struct WORLD_OBJECT_SETUP_READER { // size 0x2968
    u8 unknown_0000[0x2964];               // 0x0000
    NUGSCN *icons_gscn;                    // 0x2964
} WORLD_OBJECT_SETUP_READER;

typedef struct menu_mission_reader_s { // size 0x13
    u8 unknown_00[0x12];               // 0x00
    i8 active_count;                   // 0x12
} MENU_MISSION_READER;

extern "C" {

    void fun_005788a0(void) {
    }

    void Level_RegisterGameConfigKeywords(NUFPCOMJMP *before_load, NUFPCOMJMP *after_load) {
        Level_ConfigBeforeLoad_GameKeywords = before_load;
        Level_ConfigAfterLoad_GameKeywords = after_load;
    }

    LEVELDATA *Levels_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end, i32 max_count, i32 *count_out,
                                    void *set_defaults_fn) {
        NUFPAR *parser;
        LEVELDATA *level_data;
        LEVELDATA *current_level;
        i32 in_level_config;
        i32 level_count;

        parser = NuFParCreate(path);
        if (parser == NULL) {
            LevelsConfigDiagnostic(path);
        }
        MAXLDATA = max_count;
        level_data = (LEVELDATA *)((buffer->addr + 3) & ~3u);
        buffer->void_ptr = level_data;
        current_level = level_data;
        level_count = 0;
        in_level_config = 0;

        while (NuFParGetLine(parser) != 0) {
            NuFParGetWord(parser);
            if (parser->word_buffer_ptr[0] == '\0') {
                continue;
            }

            if (in_level_config) {
                if (NuStrICmp(parser->word_buffer_ptr, "level_end") == 0) {
                    in_level_config = 0;
                    if (current_level->dir[0] == '\0' || current_level->name[0] == '\0' || (current_level->flags & LEVEL_TEST) != 0) {
                        continue;
                    }

                    if ((current_level->flags & LEVEL_NEWGAME) != 0) {
                        NEWGAME_LDATA = current_level;
                    }
                    if ((current_level->flags & LEVEL_LOADGAME) != 0) {
                        LOADGAME_LDATA = current_level;
                    }

                    level_count++;
                    current_level++;
                    continue;
                }

                if (NuStrICmp(parser->word_buffer_ptr, "dir") == 0) {
                    if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buffer_ptr) < 0x40) {
                        NuStrCpy(current_level->dir, parser->word_buffer_ptr);
                    }
                } else if (NuStrICmp(parser->word_buffer_ptr, "file") == 0) {
                    if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buffer_ptr) < 0x20) {
                        NuStrCpy(current_level->name, parser->word_buffer_ptr);
                    }
                }

                if (NuStrICmp(parser->word_buffer_ptr, "test_level") == 0) {
                    current_level->flags |= LEVEL_TEST;
                } else if (NuStrICmp(parser->word_buffer_ptr, "intro_level") == 0) {
                    current_level->flags |= LEVEL_INTRO;
                } else if (NuStrICmp(parser->word_buffer_ptr, "midtro_level") == 0 ||
                           NuStrICmp(parser->word_buffer_ptr, "cutscene_level") == 0) {
                    current_level->flags |= LEVEL_MIDTRO;
                } else if (NuStrICmp(parser->word_buffer_ptr, "outro_level") == 0) {
                    current_level->flags |= LEVEL_OUTRO;
                } else if (NuStrICmp(parser->word_buffer_ptr, "status_level") == 0) {
                    current_level->flags &= ~LEVEL_GAMEPLAY;
                    current_level->flags &= ~LEVEL_TERRAIN;
                    current_level->flags |= LEVEL_STATUS;
                } else if (NuStrICmp(parser->word_buffer_ptr, "newgame_level") == 0) {
                    if (NEWGAME_LDATA == NULL) {
                        current_level->flags |= LEVEL_NEWGAME;
                    }
                } else if (NuStrICmp(parser->word_buffer_ptr, "loadgame_level") == 0) {
                    if (LOADGAME_LDATA == NULL) {
                        current_level->flags |= LEVEL_LOADGAME;
                    }
                }
                continue;
            }

            if (NuStrICmp(parser->word_buffer_ptr, "level_start") == 0 && level_count < MAXLDATA) {
                i32 i;

                current_level->dir[0] = '\0';
                current_level->name[0] = '\0';
                current_level->override_index = -1;
                current_level->index = (i16)level_count;
                current_level->flags = 0xe;
                current_level->callbacks[0] = NULL;
                current_level->callbacks[1] = NULL;
                current_level->callbacks[2] = NULL;
                current_level->callbacks[3] = NULL;
                current_level->callbacks[4] = NULL;
                current_level->callbacks[5] = NULL;
                current_level->callbacks[6] = NULL;

                current_level->display.farclip_hack = PCKeyboardDefaultValue;
                current_level->display.near_clip = 0.15f;
                current_level->display.far_clip = 20000;
                current_level->display.bg_red_bottom = 0;
                current_level->display.bg_red_top = 0;
                current_level->display.bg_green_bottom = 0;
                current_level->display.bg_green_top = 0;
                current_level->display.bg_blue_bottom = 0;
                current_level->display.bg_blue_top = 0;

                current_level->music_index = -1;
                current_level->sfx_ambient = -1;
                current_level->max_ter_platforms = 0x80;
                current_level->max_ter_groups = 0x100;
                current_level->music_other_index = -1;
                current_level->secondary_index = -1;
                current_level->mipmap_mode = 3;
                current_level->blob_shadow_alpha = 0x7f;
                current_level->dfs_mode = -1;
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
                current_level->cam_pullback_dist = 0.0f;
                current_level->cam_lateral_dist = 0.0f;
                current_level->reflection_y = LevelConfigFarDistance;
                current_level->area_level_index = -1;
                current_level->blob_shadow_fade_near = 5;
                current_level->blob_shadow_fade_far = 10;
                current_level->cam_pos_seek = 5;
                current_level->cam_angle_seek = 5;
                current_level->reflection_range = 10;
                current_level->raycast_step = 5;
                current_level->platform_scan_distance = 5;
                current_level->conveyor_x_speed = 0.0f;
                current_level->conveyor_z_speed = 0.0f;

                for (i = 0; i < 6; i++) {
                    current_level->music_tracks[i] = -1;
                }

                in_level_config = 1;
                if (set_defaults_fn != NULL) {
                    ((LEVELSETDEFAULTS)set_defaults_fn)(current_level);
                }
            }
        }

        NuFParDestroy(parser);
        if (level_count > 0) {
            buffer->void_ptr = current_level;
            if (count_out != NULL) {
                *count_out = level_count;
            }
            return level_data;
        }
        LevelsConfigDiagnostic(path);
        return NULL;
    }

    LEVELDATA *Level_FindByName(char *name, i32 *index_out) {
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

    void Levels_FixUp(LEVELFIXUP *fixup) {
        LEVELDATA *level;
        if (fixup != NULL) {
            for (; fixup->name != NULL; ++fixup) {
                if (fixup->level != NULL) {
                    level = Level_FindByName(fixup->name, NULL);
                    *fixup->level = level;
                    if (level != NULL) {
                        if (fixup->load != NULL) {
                            level->callbacks[0] = fixup->load;
                        }
                        if (fixup->init != NULL) {
                            level->callbacks[1] = fixup->init;
                        }
                        if (fixup->reset != NULL) {
                            level->callbacks[2] = fixup->reset;
                        }
                        if (fixup->update != NULL) {
                            level->callbacks[3] = fixup->update;
                        }
                        if (fixup->always_update != NULL) {
                            level->callbacks[4] = fixup->always_update;
                        }
                        if (fixup->draw != NULL) {
                            level->callbacks[5] = fixup->draw;
                        }
                        if (fixup->draw_status != NULL) {
                            level->callbacks[6] = fixup->draw_status;
                        }
                    }
                }
            }
        }
    }

    void LC_BL_mipmapmode(NUFPAR *parser) {
        levelconfig_ldata->mipmap_mode = (u8)NuFParGetInt(parser);
    }

    void LC_BL_fix_strobing_anims(NUFPAR *parser) {
        levelconfig_ldata->flags |= LEVEL_FIX_STROBING_ANIMS;
    }

    void LC_BL_max_ter_groups(NUFPAR *parser) {
        levelconfig_ldata->max_ter_groups = (i16)NuFParGetInt(parser);
    }

    void LC_BL_max_ter_platforms(NUFPAR *parser) {
        levelconfig_ldata->max_ter_platforms = (i16)NuFParGetInt(parser);
    }

    void LevelConfig_BeforeLoad(LEVELDATA *level, char *buffer, NUFPCOMJMP *keywords) {
        NUFPAR *parser = NuFParCreateMem("levelbeforeload", buffer, 0xffff);

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

    void LC_AL_backr(NUFPAR *parser) {
        levelconfig_ldata->display.bg_red_bottom = (u8)NuFParGetInt(parser);
        levelconfig_ldata->display.bg_red_top = levelconfig_ldata->display.bg_red_bottom;
    }

    void LC_AL_backg(NUFPAR *parser) {
        levelconfig_ldata->display.bg_green_bottom = (u8)NuFParGetInt(parser);
        levelconfig_ldata->display.bg_green_top = levelconfig_ldata->display.bg_green_bottom;
    }

    void LC_AL_backb(NUFPAR *parser) {
        levelconfig_ldata->display.bg_blue_bottom = (u8)NuFParGetInt(parser);
        levelconfig_ldata->display.bg_blue_top = levelconfig_ldata->display.bg_blue_bottom;
    }

    void LC_AL_backr_top(NUFPAR *parser) {
        levelconfig_ldata->display.bg_red_top = (u8)NuFParGetInt(parser);
    }

    void LC_AL_backg_top(NUFPAR *parser) {
        levelconfig_ldata->display.bg_green_top = (u8)NuFParGetInt(parser);
    }

    void LC_AL_backb_top(NUFPAR *parser) {
        levelconfig_ldata->display.bg_blue_top = (u8)NuFParGetInt(parser);
    }

    void LC_AL_backr_bottom(NUFPAR *parser) {
        levelconfig_ldata->display.bg_red_bottom = (u8)NuFParGetInt(parser);
    }

    void LC_AL_backg_bottom(NUFPAR *parser) {
        levelconfig_ldata->display.bg_green_bottom = (u8)NuFParGetInt(parser);
    }

    void LC_AL_backb_bottom(NUFPAR *parser) {
        levelconfig_ldata->display.bg_blue_bottom = (u8)NuFParGetInt(parser);
    }

    void LC_AL_farclip_hack(NUFPAR *parser) {
        f32 value = NuFParGetFloat(parser);

        if (value < 0.1f) {
            value = 0.1f;
        } else if (value > 50.0) {
            value = 50.0;
        }
        levelconfig_ldata->display.farclip_hack = value;
    }

    void LC_AL_farclip(NUFPAR *parser) {
        i32 value = NuFParGetInt(parser);

        if (value < 10) {
            value = 10;
        } else if (value > 20000) {
            value = 20000;
        }
        levelconfig_ldata->display.far_clip = (i16)value;
    }

    void LC_AL_nearclip(NUFPAR *parser) {
        f32 value = NuFParGetFloat(parser);

        if (value < 0.001f) {
            value = 0.001f;
        } else if (value > 1.0f) {
            value = 1.0f;
        }
        levelconfig_ldata->display.near_clip = value;
    }

    void LC_AL_hover_height(NUFPAR *parser) {
        levelconfig_ldata->hover_height = NuFParGetFloat(parser);
    }

    void LC_AL_blobshadow_alpha(NUFPAR *parser) {
        levelconfig_ldata->blob_shadow_alpha = (u8)NuFParGetInt(parser);
    }

    void LC_AL_blobshadow_fadenear(NUFPAR *parser) {
        levelconfig_ldata->blob_shadow_fade_near = (u8)NuFParGetInt(parser);
    }

    void LC_AL_blobshadow_fadefar(NUFPAR *parser) {
        levelconfig_ldata->blob_shadow_fade_far = (u8)NuFParGetInt(parser);
    }

    void LC_AL_reflect_range(NUFPAR *parser) {
        levelconfig_ldata->reflection_range = (u8)NuFParGetInt(parser);
    }

    void LC_AL_reflect_y(NUFPAR *parser) {
        levelconfig_ldata->reflection_y = NuFParGetFloat(parser);
    }

    // Sets a level flag; a following "off" clears it again.
    static void LevelConfigSetFlag(NUFPAR *parser, u32 flag) {
        levelconfig_ldata->flags |= flag;
        if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
            levelconfig_ldata->flags &= ~flag;
        }
    }

    void LC_AL_metal(NUFPAR *parser) {
        LevelConfigSetFlag(parser, LEVEL_METAL);
    }

    void LC_AL_in_space(NUFPAR *parser) {
        LevelConfigSetFlag(parser, LEVEL_IN_SPACE);
    }

    void LC_AL_override_nopickupgravity(NUFPAR *parser) {
        LevelConfigSetFlag(parser, LEVEL_OVERRIDE_NO_PICKUP_GRAVITY);
    }

    void LC_AL_hidden_icons(NUFPAR *parser) {
        if (NuFParGetWord(parser) != 0) {
            if (NuStrICmp(parser->word_buffer_ptr, "on") == 0) {
                levelconfig_ldata->flags &= ~LEVEL_HIDE_ICONS;
                return;
            }
            if (NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
                levelconfig_ldata->flags |= LEVEL_HIDE_ICONS;
            }
        }
    }

    void LC_AL_pickups_to_panel(NUFPAR *parser) {
        LevelConfigSetFlag(parser, LEVEL_PICKUPS_TO_PANEL);
    }

    void LC_AL_forget_takeovers(NUFPAR *parser) {
        LevelConfigSetFlag(parser, LEVEL_FORGET_TAKEOVERS);
    }

    void LC_AL_narrow_socks(NUFPAR *parser) {
        LevelConfigSetFlag(parser, LEVEL_NARROW_SOCKS);
    }

    void LC_AL_camera_rain(NUFPAR *parser) {
        LevelConfigSetFlag(parser, LEVEL_CAMERA_RAIN);
    }

    void LC_AL_terrain_rain(NUFPAR *parser) {
        LevelConfigSetFlag(parser, LEVEL_TERRAIN_RAIN);
    }

    void LC_AL_double_score(NUFPAR *parser) {
        LevelConfigSetFlag(parser, LEVEL_DOUBLE_SCORE);
    }

    void LC_AL_flat_terrain(NUFPAR *parser) {
        LevelConfigSetFlag(parser, LEVEL_FLAT_TERRAIN);
    }

    void LC_AL_cam_tilt(NUFPAR *parser) {
        levelconfig_ldata->cam_tilt = NuFParGetFloat(parser);
    }

    void LC_AL_raycaststep(NUFPAR *parser) {
        levelconfig_ldata->raycast_step = (u8)NuFParGetInt(parser);
    }

    void LC_AL_plat_scan_dist(NUFPAR *parser) {
        levelconfig_ldata->platform_scan_distance = (u8)NuFParGetInt(parser);
    }

    void LC_AL_waterripple_startcol_r(NUFPAR *parser) {
        levelconfig_ldata->water_ripple_start_rgba[0] = (u8)NuFParGetInt(parser);
    }

    void LC_AL_waterripple_startcol_g(NUFPAR *parser) {
        levelconfig_ldata->water_ripple_start_rgba[1] = (u8)NuFParGetInt(parser);
    }

    void LC_AL_waterripple_startcol_b(NUFPAR *parser) {
        levelconfig_ldata->water_ripple_start_rgba[2] = (u8)NuFParGetInt(parser);
    }

    void LC_AL_waterripple_startcol_a(NUFPAR *parser) {
        levelconfig_ldata->water_ripple_start_rgba[3] = (u8)NuFParGetInt(parser);
    }

    void LC_AL_waterripple_endcol_r(NUFPAR *parser) {
        levelconfig_ldata->water_ripple_end_rgba[0] = (u8)NuFParGetInt(parser);
    }

    void LC_AL_waterripple_endcol_g(NUFPAR *parser) {
        levelconfig_ldata->water_ripple_end_rgba[1] = (u8)NuFParGetInt(parser);
    }

    void LC_AL_waterripple_endcol_b(NUFPAR *parser) {
        levelconfig_ldata->water_ripple_end_rgba[2] = (u8)NuFParGetInt(parser);
    }

    void LC_AL_waterripple_endcol_a(NUFPAR *parser) {
        levelconfig_ldata->water_ripple_end_rgba[3] = (u8)NuFParGetInt(parser);
    }

    void LC_AL_waterripple_life(NUFPAR *parser) {
        levelconfig_ldata->water_ripple_life = NuFParGetFloat(parser);
    }

    void LC_AL_cam_pullback_dist(NUFPAR *parser) {
        f32 value = NuFParGetFloat(parser);

        if (value < 0.0f) {
            value = 0.0f;
        }
        levelconfig_ldata->cam_pullback_dist = value;
    }

    void LC_AL_cam_lateral_dist(NUFPAR *parser) {
        f32 value = NuFParGetFloat(parser);

        if (value < 0.0f) {
            value = 0.0f;
        }
        levelconfig_ldata->cam_lateral_dist = value;
    }

    void LC_AL_campos_seek(NUFPAR *parser) {
        levelconfig_ldata->cam_pos_seek = (u8)NuFParGetInt(parser);
    }

    void LC_AL_camang_seek(NUFPAR *parser) {
        levelconfig_ldata->cam_angle_seek = (u8)NuFParGetInt(parser);
    }

    void LC_AL_shadowtype(NUFPAR *parser) {
        levelconfig_ldata->shadow_type = NuFParGetInt(parser);
    }
}

extern "C" void LC_AL_music(NUFPAR *parser) {
    if (NuFParGetWord(parser) == 0) {
        return;
    }

    levelconfig_ldata->music_index = GetMusicIndex(parser->word_buffer_ptr, MusicInfo, -1);
    levelconfig_ldata->music_tracks[0] = music_man.GetTrackHandle(1, parser->word_buffer_ptr);
    levelconfig_ldata->music_tracks[2] = music_man.GetTrackHandle(2, parser->word_buffer_ptr);
    levelconfig_ldata->music_tracks[4] = music_man.GetTrackHandle(0x20, parser->word_buffer_ptr);
}

extern "C" void LC_AL_music_other(NUFPAR *parser) {
    if (NuFParGetWord(parser) == 0) {
        return;
    }

    levelconfig_ldata->music_other_index = GetMusicIndex(parser->word_buffer_ptr, MusicInfo, -1);
    levelconfig_ldata->music_tracks[1] = music_man.GetTrackHandle(1, parser->word_buffer_ptr);
    levelconfig_ldata->music_tracks[3] = music_man.GetTrackHandle(2, parser->word_buffer_ptr);
    levelconfig_ldata->music_tracks[5] = music_man.GetTrackHandle(0x20, parser->word_buffer_ptr);
}

extern "C" {

    void LC_AL_sfx_ambient(NUFPAR *parser) {
        if (NuFParGetWord(parser) != 0) {
            levelconfig_ldata->sfx_ambient = (i16)GetSfxId(parser->word_buffer_ptr);
        }
    }

    void LC_AL_conveyor(NUFPAR *parser) {
        while (NuFParGetWord(parser) != 0) {
            if (NuStrICmp(parser->word_buffer_ptr, "xspeed") == 0) {
                levelconfig_ldata->conveyor_x_speed = NuFParGetFloat(parser);
            } else if (NuStrICmp(parser->word_buffer_ptr, "zspeed") == 0) {
                levelconfig_ldata->conveyor_z_speed = NuFParGetFloat(parser);
            }
        }
    }

    void LevelConfig_AfterLoad(LEVELDATA *level, char *buffer, NUFPCOMJMP *keywords) {
        NUFPAR *parser = NuFParCreateMem("levelafterload", buffer, 0xffff);

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
        level->flags |= LEVEL_CONFIG_LOADED;
    }

    void LevelSplines_InitForGame(LEVELSPLINE *table) {
        LEVELSPLINE *record;

        LevSplList = table;
        LEVELSPLINECOUNT = 0;
        if (table == NULL || table[0].name == NULL) {
            return;
        }
        for (record = &table[0]; record->name != NULL; record++) {
            if (levspl_i_start == -1 && NuStrICmp(record->name, "start") == 0) {
                levspl_i_start = LEVELSPLINECOUNT;
            } else if (levspl_i_startcam == -1 && NuStrICmp(record->name, "start_cam") == 0) {
                levspl_i_startcam = LEVELSPLINECOUNT;
            }
            LEVELSPLINECOUNT++;
        }
    }

    void LevelSplines_InitForLevel(WORLDINFO *world) {
        char name[64];
        i32 i;
        u32 size;
        world->giz_buffer.addr = (world->giz_buffer.addr + 3u) & ~3u;
        world->camera_splines = (NUGSPLINE **)world->giz_buffer.void_ptr;
        size = LEVELSPLINECOUNT * sizeof(*world->camera_splines);
        world->giz_buffer.addr += size;
        memset(world->camera_splines, 0, size);
        if (LevSplList == NULL) {
            return;
        }
        for (i = 0; i < LEVELSPLINECOUNT; ++i) {
            NUGSCN *scene;
            NUGSPLINE *spline;
            if ((LevSplList[i].area != -1 && world->area_index != LevSplList[i].area) ||
                (LevSplList[i].level != -1 && world->level_idx != LevSplList[i].level)) {
                continue;
            }
            scene = *(LevSplList[i].scene != NULL ? LevSplList[i].scene : &world->current_gscn);
            if (scene != NULL) {
                world->camera_splines[i] = NuSplineFind(scene, (char *)LevSplList[i].name);
                if (world->camera_splines[i] != NULL) {
                    if (LevSplList[i].min_points > 0 && world->camera_splines[i]->length < LevSplList[i].min_points) {
                        world->camera_splines[i] = NULL;
                    } else if (LevSplList[i].max_points > 0 && LevSplList[i].max_points >= LevSplList[i].min_points &&
                               world->camera_splines[i]->length > LevSplList[i].max_points) {
                        world->camera_splines[i] = NULL;
                    }
                }
            }
            if (levspl_i_start != -1) {
                name[0] = '\0';
                if (Mission_Active(NULL) != NULL) {
                    NuStrCpy(name, "bounty_start");
                } else if (world->area_index != -1 && (ADataList[world->area_index].flags & AREAFLAG_HUB_AREA) &&
                           RenderHubFromCutscenePlayer != 0) {
                    NuStrCpy(name, "shop_start");
                }
                if (name[0] != '\0') {
                    spline = NuSplineFind(scene, name);
                    if (spline != NULL && spline->length >= 2) {
                        world->camera_splines[levspl_i_start] = spline;
                        if (levspl_i_startcam != -1) {
                            world->camera_splines[levspl_i_startcam] = NULL;
                        }
                    }
                }
            }
        }
    }

    LEVELDATA *levelconfig_ldata = 0;
    LEVELDATA *LDataList = 0;
    LEVELDATA *LastLData = 0;
    LEVELDATA *NewLData = 0;
    i32 LEVELCOUNT = 0;
    LEVELDATA *TITLES_LDATA = 0;
    LEVELDATA *NEWGAME_LDATA = 0;
    LEVELDATA *LOADGAME_LDATA = 0;
    LEVELDATA *HUB_LDATA = 0;
    LEVELDATA *STATUS_LDATA = 0;
    LEVELDATA *CREDITS_LDATA = 0;
    i32 Level = 0;
    i32 LevelChange = 0;
    NUFPCOMJMP *Level_ConfigBeforeLoad_GameKeywords = 0;
    NUFPCOMJMP *Level_ConfigAfterLoad_GameKeywords = 0;
    LEVELSPLINE *LevSplList = 0;
    i32 LEVELSPLINECOUNT = 0;
    LEVEL_OBJECT_CONFIG *ObjTabList = 0;
    i32 LEVELOBJECTCOUNT = 0;
    static i32 LEVELOBJECTMAX = 0;
    static char *ExtraLevelObject_NameTable = 0;
    static i32 ExtraLevelObject_NameTableSize = 0;
    i32 MAXLDATA = 0x64;
    i32 levspl_i_start = -1;
    i32 levspl_i_startcam = -1;
    i32 LEGOSPL_SPLIT = -1;
    i32 PlayerStartSplineIndex = -1;
    i32 LEVOBJREF_FIRSTOBJ = -1;
    i32 LEVOBJREF_LASTOBJ = -1;
    i32 LEVOBJREF_FIRSTREFOBJ = -1;
    i32 LEVOBJREF_LASTREFOBJ = -1;
    i32 LEGOOBJ_ICON_FRAME_NEUTRAL = -1;
    i32 LEGOOBJ_ICON_FRAME_GREEN = -1;
    i32 LEGOOBJ_ICON_FRAME_BLUE = -1;
    i32 LegoObjectIconQuestion = -1;
    i32 PCArcadeUnpairedIconModel = -1;
    i32 CharKitModelId = -1;
    i32 MiniKitModelId = -1;
    i32 LEGOOBJ_FLOORTARGET = -1;
    i32 LEGOOBJ_GRAPPLE_HOOK = -1;
    LEGOOBJ_DEFAULT_LAST_COIN_STORAGE LEGOOBJ_DEFAULTLASTCOIN = {0xffffffffu};
    // Two-entry reconstruction access view; the original full array extent is unknown.
    // PC readers and reset producers access these two i32 entries; no original sizeof credit.
    i32 LevSafePlatID[2] = {-1, -1};
    NUFPCOMJMP LevelConfig_BeforeLoad_GenericKeywords[5] = {
        {const_cast<char *>("mipmapmode"), LC_BL_mipmapmode},
        {const_cast<char *>("fix_strobing_anims"), LC_BL_fix_strobing_anims},
        {const_cast<char *>("max_ter_groups"), LC_BL_max_ter_groups},
        {const_cast<char *>("max_ter_platforms"), LC_BL_max_ter_platforms},
        {NULL, NULL},
    };
    NUFPCOMJMP LevelConfig_AfterLoad_GenericKeywords[53] = {
        {const_cast<char *>("backr"), LC_AL_backr},
        {const_cast<char *>("backg"), LC_AL_backg},
        {const_cast<char *>("backb"), LC_AL_backb},
        {const_cast<char *>("backr_top"), LC_AL_backr_top},
        {const_cast<char *>("backg_top"), LC_AL_backg_top},
        {const_cast<char *>("backb_top"), LC_AL_backb_top},
        {const_cast<char *>("backr_bottom"), LC_AL_backr_bottom},
        {const_cast<char *>("backg_bottom"), LC_AL_backg_bottom},
        {const_cast<char *>("backb_bottom"), LC_AL_backb_bottom},
        {const_cast<char *>("farclip_hack"), LC_AL_farclip_hack},
        {const_cast<char *>("farclip"), LC_AL_farclip},
        {const_cast<char *>("nearclip"), LC_AL_nearclip},
        {const_cast<char *>("farclip_pc"), LC_AL_farclip},
        {const_cast<char *>("nearclip_pc"), LC_AL_nearclip},
        {const_cast<char *>("hover_height"), LC_AL_hover_height},
        {const_cast<char *>("blobshadow_alpha"), LC_AL_blobshadow_alpha},
        {const_cast<char *>("blobshadow_fadenear"), LC_AL_blobshadow_fadenear},
        {const_cast<char *>("blobshadow_fadefar"), LC_AL_blobshadow_fadefar},
        {const_cast<char *>("reflect_range"), LC_AL_reflect_range},
        {const_cast<char *>("reflect_y"), LC_AL_reflect_y},
        {const_cast<char *>("metal"), LC_AL_metal},
        {const_cast<char *>("in_space"), LC_AL_in_space},
        {const_cast<char *>("override_nopickupgravity"), LC_AL_override_nopickupgravity},
        {const_cast<char *>("hidden_icons"), LC_AL_hidden_icons},
        {const_cast<char *>("pickups_to_panel"), LC_AL_pickups_to_panel},
        {const_cast<char *>("forget_takeovers"), LC_AL_forget_takeovers},
        {const_cast<char *>("narrow_socks"), LC_AL_narrow_socks},
        {const_cast<char *>("camera_rain"), LC_AL_camera_rain},
        {const_cast<char *>("terrain_rain"), LC_AL_terrain_rain},
        {const_cast<char *>("double_score"), LC_AL_double_score},
        {const_cast<char *>("flat_terrain"), LC_AL_flat_terrain},
        {const_cast<char *>("cam_tilt"), LC_AL_cam_tilt},
        {const_cast<char *>("raycaststep"), LC_AL_raycaststep},
        {const_cast<char *>("plat_scan_dist"), LC_AL_plat_scan_dist},
        {const_cast<char *>("waterripple_startcol_r"), LC_AL_waterripple_startcol_r},
        {const_cast<char *>("waterripple_startcol_g"), LC_AL_waterripple_startcol_g},
        {const_cast<char *>("waterripple_startcol_b"), LC_AL_waterripple_startcol_b},
        {const_cast<char *>("waterripple_startcol_a"), LC_AL_waterripple_startcol_a},
        {const_cast<char *>("waterripple_endcol_r"), LC_AL_waterripple_endcol_r},
        {const_cast<char *>("waterripple_endcol_g"), LC_AL_waterripple_endcol_g},
        {const_cast<char *>("waterripple_endcol_b"), LC_AL_waterripple_endcol_b},
        {const_cast<char *>("waterripple_endcol_a"), LC_AL_waterripple_endcol_a},
        {const_cast<char *>("waterripple_life"), LC_AL_waterripple_life},
        {const_cast<char *>("cam_pullback_dist"), LC_AL_cam_pullback_dist},
        {const_cast<char *>("cam_lateral_dist"), LC_AL_cam_lateral_dist},
        {const_cast<char *>("campos_seek"), LC_AL_campos_seek},
        {const_cast<char *>("camang_seek"), LC_AL_camang_seek},
        {const_cast<char *>("shadowtype"), LC_AL_shadowtype},
        {const_cast<char *>("music"), LC_AL_music},
        {const_cast<char *>("music_other"), LC_AL_music_other},
        {const_cast<char *>("sfx_ambient"), LC_AL_sfx_ambient},
        {const_cast<char *>("conveyor"), LC_AL_conveyor},
        {NULL, NULL},
    };
    i32 KNOBS = -1;
    i32 newlevelfrommenu_newmenuid = -1;

    void LevelObjects_InitForGame(LEVEL_OBJECT_CONFIG *table, VARIPTR *buffer, VARIPTR *buffer_end, i32 maximum, i32 name_table_size) {
        i32 reflection_range;
        i32 object_range;
        LEVOBJREF_FIRSTOBJ = -1;
        LEVOBJREF_LASTOBJ = -1;
        LEVOBJREF_FIRSTREFOBJ = -1;
        LEVOBJREF_LASTREFOBJ = -1;
        ObjTabList = table;
        LEVELSPLINECOUNT = 0;
        LEVELOBJECTMAX = maximum;
        for (; table->kind != 0xff; ++table) {
            if (table->reflection == 1) {
                LEVOBJREF_LASTOBJ = LEVELOBJECTCOUNT;
                if (LEVOBJREF_FIRSTOBJ == -1) {
                    LEVOBJREF_FIRSTOBJ = LEVELOBJECTCOUNT;
                }
            }
            if (table->reflection == 2) {
                LEVOBJREF_LASTREFOBJ = LEVELOBJECTCOUNT;
                if (LEVOBJREF_FIRSTREFOBJ == -1) {
                    LEVOBJREF_FIRSTREFOBJ = LEVELOBJECTCOUNT;
                }
            }
            LEVELOBJECTCOUNT++;
        }
        object_range = LEVOBJREF_LASTOBJ - LEVOBJREF_FIRSTOBJ;
        reflection_range = LEVOBJREF_LASTREFOBJ - LEVOBJREF_FIRSTREFOBJ;
        if (reflection_range < object_range) {
            LEVOBJREF_LASTOBJ = LEVOBJREF_FIRSTOBJ + reflection_range;
        } else if (reflection_range > object_range) {
            LEVOBJREF_LASTREFOBJ = LEVOBJREF_FIRSTREFOBJ + object_range;
        }
        if (name_table_size > 0) {
            ExtraLevelObject_NameTable = (char *)buffer->void_ptr;
            ExtraLevelObject_NameTableSize = name_table_size;
            buffer->addr += name_table_size;
            if (buffer->addr >= buffer_end->addr) {
                LevelObjectsArenaDiagnostic(name_table_size);
            }
        }
    }

    i32 EXTRALEVELOBJECTCOUNT = 0;
    static i32 ExtraLevelObject_NameTableIndex = 0;

    i32 LevelObject_AddExtra(char *name, i32 kind) {
        i32 length;
        char *destination;
        if (LEVELOBJECTCOUNT >= LEVELOBJECTMAX) {
            return 0;
        }
        if (ExtraLevelObject_NameTable == NULL) {
            return 0;
        }
        length = NuStrLen(name);
        if (length + ExtraLevelObject_NameTableIndex + 1 >= ExtraLevelObject_NameTableSize) {
            return 0;
        }
        ObjTabList[LEVELOBJECTCOUNT].kind = (u8)kind;
        destination = ExtraLevelObject_NameTable + ExtraLevelObject_NameTableIndex;
        ObjTabList[LEVELOBJECTCOUNT].name = destination;
        ++LEVELOBJECTCOUNT;
        ++EXTRALEVELOBJECTCOUNT;
        NuStrCpy(destination, name);
        ExtraLevelObject_NameTableIndex += length + 1;
        return 1;
    }

    void LevelObjects_InitForLevel(WORLDINFO *world) {
        i32 index;
        u32 size;
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->level_objects = (LEVEL_OBJECT_RUNTIME *)world->giz_buffer.void_ptr;
        size = LEVELOBJECTCOUNT * sizeof(LEVEL_OBJECT_RUNTIME);
        world->giz_buffer.addr += size;
        memset(world->level_objects, 0, size);
        if (ObjTabList == NULL) {
            return;
        }
        for (index = 0; index < LEVELOBJECTCOUNT; ++index) {
            NUGSCN *scene;
            i32 found;
            i32 area_override;
            if (NuStrICmp(ObjTabList[index].name, "power_up") == 0) {
                KNOBS = index;
            }
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
                        if (scene != NULL && NuSpecialFind(scene, &world->level_objects[index].special, ObjTabList[index].name, 1) != 0) {
                            found = 1;
                        }
                    }
                    if (found) {
                        goto resolved;
                    }
                    scene = big_icon_scene != NULL ? big_icon_scene : ((WORLD_OBJECT_SETUP_READER *)world)->icons_gscn;
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
                    if (scene != NULL && world->area != NULL && (world->area->flags & AREAFLAG_OVERRIDE_THINGS_SCENE) != 0) {
                        area_override = 1;
                    } else {
                        scene = things_scene;
                    }
                    break;
            }
            if (scene == NULL) {
                continue;
            }
            if (NuSpecialFind(scene, &world->level_objects[index].special, ObjTabList[index].name, 1) == 0 && area_override != 0) {
                NuSpecialFind(things_scene, &world->level_objects[index].special, ObjTabList[index].name, 1);
            }
        resolved:
            world->level_objects[index].active = (u8)NuSpecialExistsFn(&world->level_objects[index].special);
            if (world->level_objects[index].active != 0) {
                NuSpecialSetVisibility(&world->level_objects[index].special, 0);
            }
        }
    }

    i32 LevelObject_FindIndexFromName(char *name) {
        i32 i;
        if (ObjTabList == NULL) {
            return -1;
        }
        for (i = 0; i < LEVELOBJECTCOUNT; ++i) {
            if (NuStrICmp(ObjTabList[i].name, name) == 0) {
                return i;
            }
        }
        return -1;
    }

    void LevObj_FixUpPlatIDs(WORLDINFO *world) {
        i32 index;
        if (ObjTabList == NULL) {
            return;
        }
        for (index = 0; index < LEVELOBJECTCOUNT; ++index) {
            world->level_objects[index].platform_id = -1;
            if (world->terrain != NULL && NuSpecialExistsFn(&world->level_objects[index].special) && ObjTabList[index].kind == 1) {
                world->level_objects[index].platform_id = (i16)FindPlatInst(NuSpecialGetInstanceix(&world->level_objects[index].special));
            }
        }
    }

    void *LevObj_FindByPlatID(WORLDINFO *world, i32 platform_id) {
        LEVEL_OBJECT_RUNTIME *object = world->level_objects;
        i32 i;
        for (i = 0; i < LEVELOBJECTCOUNT; ++i, ++object) {
            if (object->platform_id == platform_id) {
                return object;
            }
        }
        return NULL;
    }

    char *LevelObject_FindNameFromIndex(i32 index) {
        char *name = "";
        if (ObjTabList == NULL || index < 0 || index >= LEVELOBJECTCOUNT) {
            return name;
        }
        name = ObjTabList[index].name;
        return name;
    }

    i32 LevelObject_GetReflection(i32 index) {
        if (index != -1 && index >= LEVOBJREF_FIRSTOBJ && index <= LEVOBJREF_LASTOBJ) {
            return LEVOBJREF_FIRSTREFOBJ - LEVOBJREF_FIRSTOBJ + index;
        }
        return index;
    }

    i32 LevelObject_FindIndexFromName_RefOnly(char *name) {
        i32 end;
        i32 i;
        if (ObjTabList == NULL) {
            return -1;
        }
        if (LEVOBJREF_FIRSTOBJ == -1) {
            return -1;
        }
        end = LEVOBJREF_LASTOBJ - LEVOBJREF_FIRSTOBJ + 1;
        for (i = LEVOBJREF_FIRSTOBJ; i < end; ++i) {
            if (NuStrICmp(ObjTabList[i].name, name) == 0) {
                return i;
            }
        }
        return -1;
    }

    void Level_LoadConfigFile(WORLDINFO *world) {
        char name[128];
        i32 bytes_read;

        ConfigBuffer.bytes[0] = '\0';
        sprintf(name, "%s.txt", world->config_file);
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3U;
        bytes_read = NuFileLoadBuffer(name, world->giz_buffer.void_ptr, 0x10000);
        world->config_count = bytes_read;
        if (bytes_read > 0) {
            world->giz_buffer.char_ptr[bytes_read] = '\0';
            bytes_read = Text_StripComments(world->giz_buffer.char_ptr, ConfigBuffer.bytes, 1);
            world->config_count = bytes_read;
        }
    }

    void Level_Update(WORLDINFO *world) {
        void (*update)(WORLDINFO *) = (void (*)(WORLDINFO *))world->current_level->callbacks[3];
        if (update != NULL) {
            update(world);
        }
        if (DoubleScoreTime > 0.0f && (i32)((f64)GameTimer.time_elapsed * 2.0) != (i32)((f64)GameTimer.last_time_elapsed * 2.0)) {
            GameAudio_PlaySfxAndSetVolume(0x35, NULL, DoubleScoreTime);
        }
        if (AreaGlobals.player_deaths == 0 && Cheats_CheckFlags(0x2000) != 0) {
            AreaGlobals.player_deaths = 1;
        }
    }

    void Level_Draw(WORLDINFO *world) {
        void (*draw)(WORLDINFO *) = (void (*)(WORLDINFO *))world->current_level->callbacks[5];
        if (draw != NULL) {
            draw(world);
        }
    }
}

void GoToNewLevel(i32 level_index) {
    NewLData = &LDataList[level_index];
    if (waiting_for_level != -1) {
        waiting_for_new_level = 1;
    }
}

void CompleteLevel(WORLDINFO *world) {
    LEVELDATA *level;

    level = NewLData;
    LevelCompletionFallbackFlagPC = 1u;
    if (level == NULL || ((level->flags & (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO)) != 0 && FreePlay != 0)) {
        level = Area_FindStatusLevel(world->area, NULL);
        NewLData = level;
    }
    if (level == NULL) {
        NewLData = HUB_LDATA;
    }
    if (waiting_for_level != -1) {
        waiting_for_new_level = 1;
    }
}

extern "C" {

    void ResetLevel(WORLDINFO *world, char *cutscene, i32 unused) {
        CutDeadVehiclePlayer = NULL;
        if (world != NULL && NewCutScene(NULL, world->cutscene_sys, cutscene, 0) != NULL) {
            ResetBits |= 7;
            if (player != NULL && player->dead != 0) {
                CutDeadVehiclePlayer = player;
            } else if (player2 != NULL && player2->dead != 0) {
                CutDeadVehiclePlayer = player2;
            }
            reset_reimport = 1;
        } else {
            NewGameMode();
            ResetBits |= 0x17;
            reset_restart = 1;
            LevelCompletionFallbackFlagPC = 1;
            GizmoTypeStoreProgress(WORLD->gizmo_sys, WORLD, WORLD->current_level->area_level_index, gizmopickup_typeid, NULL);
        }
    }

    void NewLevelFromMenu(LEVELDATA *level, i32 menu_id, i32 menu_y, i32 ignored) {
        MISSIONDATA *mission;
        LOG_INFO("Menu level request: level=%p menu=%d row=%d", level, menu_id, menu_y);
        if (no_more_loads == 0) {
            abort_load = 1;
            no_more_loads = 1;
        }
        Door_Reset();
        NewLData = level;
        new_level_from_menu = 1;
        newlevelfrommenu_newmenuid = menu_id;
        newlevelfrommenu_newmenuy = menu_y;
        if (HUB_ADATA != NULL && WORLD->area == HUB_ADATA) {
            return;
        }
        if (SuperStory != 0) {
            hub_from_superstory = SuperStoryEpisode;
            return;
        }
        mission = Mission_Active(NULL);
        if (mission != NULL && NewLData != NULL && NewLData == HUB_LDATA) {
            hub_from_mission = ((MENU_MISSION_READER *)mission)->active_count;
        }
    }

    void LevelStreaming_DoorOverride(WORLDINFO *world, LEVELDATA *level, f32 radius, f32 *distance) {
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

    void fun_0057a810() {
    }
}
