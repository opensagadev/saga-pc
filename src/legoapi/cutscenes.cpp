#include "batman.h"
#include "globals.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/characters.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/timing.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/cutscenes/gcutscn.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/gamestructure.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/render.h"
#include "legoapi/render/core/screen.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/light/lighting.h"
#include "legoapi/render/light/shadow.h"
#include "legoapi/render/light/surfaces.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episode.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/nuanimdata.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nugcutscene.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic/numusic.h"
#include "nu2api/numusic/sfx.h"
#include "nu2api/nusound/nusound.h"
#include "nu2api/nuwin32/d3dApiCalls.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include <stdio.h>
#include "legoapi/menus/screens/gamestatus_lsw.h"

static i32 CS_area = 0;
static i32 CS_fadefogcount = 0;
extern "C" {
    i32 CUTNOFOG = 0;
}
static i32 CS_texanimcount = 0;
extern "C" {
    CUTINFO *NewCutInfo[8] = {0};
}
static VARIPTR *CS_buffptr = 0;
static CUTINFO *CS_CutInfo = 0;
static WORLDINFO *CS_worldinfo = 0;
static CUTSYS *CS_cutsys = 0;
extern "C" {
    i32 CUTDRAWWORLD = 0;
}
extern "C" {
    i32 CUTSTOPGAME = 0;
}
static VARIPTR *CS_buffend = 0;
extern "C" {
    CUTINFO *CutStopInfo = 0;
}
static u8 CutBlobShadowAlpha = 0;
static f32 CutBlobShadowFadeFar = 0;
extern "C" {
    i32 cut_waiting_for_new_level = 0;
    void *newmode_cutinfo = 0;
    i32 ACTIVECUTCOUNT = 0;
    i32 NewCutInfoCount = 0;
    i32 CutInstEndCount = 0;
    u8 CUTSUBTITLEDEFAULT_ALIGN = 0;
    f32 CUTSUBTITLEDEFAULT_X = 0;
    i32 (*CutScene_StartFn)(CUTINFO *cut) = 0;
    void (*CutScene_PreUpdateFn)(CUTINFO *cut) = 0;
    void (*CutScene_PostUpdateFn)(void) = 0;
    void (*CutScene_StoppedFn)(CUTINFO *cut) = 0;
    i32 (*CutScene_ReplaceCharacterModelFn)(CUTINFO *cut, CUTSCENECHARACTER *character) = 0;
    void (*CutScene_OverrideConfigFileNameFn)(char *, i32, i32) = 0;
    CUT_LOAD_LIST_ENTRY *CutList = 0;
    i32 CUTCOUNT = 0;
    i32 stream_cut_issued = 0;
    CUT_PLAYER_ACTIVE *CutScenePlayer = NULL;
    i32 (*CutScenePlayer_AcceptFn)(struct cut_player_clip_config_s *) = NULL;
}
static f32 CutBlobShadowFadeNear = 0;
static f32 CutReflectRange2 = 0;

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static f32 CutscenesFabs(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

typedef struct CUT_DRAW_RIGID {
    u8 unknown_00[0x58];
    u8 flags;
} CUT_DRAW_RIGID;

struct FADEINFO_s;

extern "C" {
    static void LocatorFunction_Blaster(INSTNUGCUTSCENE *, CUT_LOAD_LOCATOR_SYSTEM *, CUT_LOAD_LOCATOR_INSTANCE *,
                                        CUT_LOAD_LOCATOR *locator, f32 frame, NUMTX *parent, i32);
}
extern "C" {

    void CutScenes_ConfigureList(char *filename, VARIPTR *buffer, VARIPTR end) {
        CUT_LOAD_LIST_ENTRY entries[1000];
        char file[128];
        char path[128];
        NUFPAR *fp;
        char *cursor;
        u32 size;
        i32 duplicate;
        i32 level;
        i32 i;
        i32 done;
        CutList = NULL;
        CUTCOUNT = 0;
        fp = NuFParCreate(filename);
        if (fp == NULL) {
            return;
        }
        buffer->addr = (buffer->addr + 3) & ~3U;
        cursor = buffer->char_ptr;
        while (CUTCOUNT < 1000 && NuFParGetLine(fp) != 0) {
            if (NuFParGetWord(fp) == 0 || NuStrICmp(fp->word_buffer_ptr, "cutscene_start") != 0) {
                continue;
            }
            path[0] = '\0';
            file[0] = '\0';
            entries[CUTCOUNT].level = -1;
            done = 0;
            while (done == 0 && NuFParGetLine(fp) != 0 && NuFParGetWord(fp) != 0) {
                if (NuStrICmp(fp->word_buffer_ptr, "cutscene_end") == 0) {
                    done = 1;
                    if (path[0] == '\0' || file[0] == '\0' || entries[CUTCOUNT].level == -1) {
                        continue;
                    }
                    duplicate = 0;
                    i = 0;
                    while (i < CUTCOUNT && duplicate == 0) {
                        if (NuStrICmp(path, entries[i].directory) == 0 && NuStrICmp(file, entries[i].name) == 0 &&
                            entries[CUTCOUNT].level == entries[i].level) {
                            duplicate = 1;
                        } else {
                            ++i;
                        }
                    }
                    if (duplicate != 0) {
                        reinterpret_cast<void (*)(char *, char *, char *)>(CutSceneConfigMissing)(path, file,
                                                                                                  LDataList[entries[CUTCOUNT].level].name);
                    } else {
                        entries[CUTCOUNT].directory = cursor;
                        NuStrCpy(cursor, path);
                        cursor += NuStrLen(path);
                        *cursor++ = '\0';
                        entries[CUTCOUNT].name = cursor;
                        NuStrCpy(cursor, file);
                        cursor += NuStrLen(file);
                        *cursor++ = '\0';
                        ++CUTCOUNT;
                    }
                } else if (NuStrICmp(fp->word_buffer_ptr, "dir") == 0 || NuStrICmp(fp->word_buffer_ptr, "path") == 0) {
                    if (NuFParGetWord(fp) != 0 && NuStrLen(fp->word_buffer_ptr) < 0x80) {
                        NuStrCpy(path, fp->word_buffer_ptr);
                    }
                } else if (NuStrICmp(fp->word_buffer_ptr, "file") == 0 || NuStrICmp(fp->word_buffer_ptr, "filename") == 0) {
                    if (NuFParGetWord(fp) != 0 && NuStrLen(fp->word_buffer_ptr) < 0x40) {
                        NuStrCpy(file, fp->word_buffer_ptr);
                    }
                } else if (NuStrICmp(fp->word_buffer_ptr, "level") == 0 || NuStrICmp(fp->word_buffer_ptr, "in_level") == 0) {
                    if (NuFParGetWord(fp) != 0) {
                        Level_FindByName(fp->word_buffer_ptr, &level);
                        entries[CUTCOUNT].level = (i16)level;
                    }
                }
            }
        }
        NuFParDestroy(fp);
        if (CUTCOUNT > 0) {
            buffer->addr = ((u32)cursor + 3) & ~3U;
            CutList = static_cast<CUT_LOAD_LIST_ENTRY *>(buffer->void_ptr);
            size = CUTCOUNT * sizeof(CUT_LOAD_LIST_ENTRY);
            PCByteCopy(CutList, entries, size);
            buffer->addr += size;
        }
    }

    static void CS_fpsec(NUFPAR *fp) {
        CS_CutInfo->frames_per_second = NuFParGetFloat(fp);
    }

    static void CS_no_fog(NUFPAR *) {
        CS_CutInfo->flags |= CUTINFO_NO_FOG;
    }

    static void CS_draw_world(NUFPAR *fp) {
        if (NuFParGetWord(fp) == 0) {
            return;
        }
        if (NuStrICmp(fp->word_buffer_ptr, "on") == 0) {
            CS_CutInfo->flags |= CUTINFO_DRAW_WORLD;
        } else if (NuStrICmp(fp->word_buffer_ptr, "off") == 0) {
            CS_CutInfo->flags &= ~CUTINFO_DRAW_WORLD;
        }
    }

    static void CS_draw_gizmo_sys(NUFPAR *fp) {
        if (NuFParGetWord(fp) == 0) {
            return;
        }
        if (NuStrICmp(fp->word_buffer_ptr, "on") == 0) {
            CS_CutInfo->flags |= CUTINFO_DRAW_GIZMO_SYS;
        } else if (NuStrICmp(fp->word_buffer_ptr, "off") == 0) {
            CS_CutInfo->flags &= ~CUTINFO_DRAW_GIZMO_SYS;
        }
    }

    static void CS_in_game(NUFPAR *) {
        CUTINFO *info = CS_CutInfo;
        info->flags = (info->flags & ~3U) | CUTINFO_IN_GAME;
    }

    static void CS_blobshadow_alpha(NUFPAR *fp) {
        i32 alpha = NuFParGetInt(fp);
        if (alpha > 0xfe) {
            alpha = 0xfe;
        } else if (alpha < 0) {
            alpha = 0;
        }
        CS_CutInfo->blob_shadow_alpha = static_cast<u8>(alpha);
    }

    static void CS_blobshadow_fadenear(NUFPAR *fp) {
        CS_CutInfo->blob_shadow_fade_near = static_cast<u8>(NuFParGetInt(fp));
    }

    static void CS_blobshadow_fadefar(NUFPAR *fp) {
        CS_CutInfo->blob_shadow_fade_far = static_cast<u8>(NuFParGetInt(fp));
    }

    static void CS_reflect_range(NUFPAR *fp) {
        CS_CutInfo->reflection_range = static_cast<u8>(NuFParGetInt(fp));
    }

    static void CS_render_group(NUFPAR *fp) {
        CS_CutInfo->debris_render_group = static_cast<i8>(NuFParGetInt(fp));
    }

    static void CS_deb_page(NUFPAR *fp) {
        if (NuFParGetWord(fp) == 0) {
            return;
        }
        if (NuStrICmp(fp->word_buffer_ptr, "level") == 0) {
            CS_CutInfo->flags |= CUTINFO_LEVEL_DEBRIS_PAGE;
        } else {
            CS_CutInfo->flags &= ~CUTINFO_LEVEL_DEBRIS_PAGE;
        }
    }

    static void CS_snap_out(NUFPAR *) {
        CS_CutInfo->flags |= CUTINFO_SNAP_OUT;
    }

    static void CS_cam_only(NUFPAR *) {
        CS_CutInfo->flags |= CUTINFO_CAM_ONLY;
    }

    static void CS_wipe_out(NUFPAR *) {
        CS_CutInfo->flags |= CUTINFO_WIPE_OUT;
    }

    static void CS_new_mode(NUFPAR *) {
        CS_CutInfo->flags |= CUTINFO_NEW_MODE;
    }

    static void CS_replace_players(NUFPAR *) {
        CS_CutInfo->flags |= CUTINFO_REPLACE_PLAYERS;
    }

    static void CS_looping(NUFPAR *) {
        CS_CutInfo->flags |= CUTINFO_LOOPING;
    }

    static void CS_start_cam(NUFPAR *) {
        CS_CutInfo->flags |= CUTINFO_START_CAM;
    }

    static void CS_super_widescreen(NUFPAR *) {
        CS_CutInfo->flags |= CUTINFO_SUPER_WIDESCREEN;
    }

    static void CS_burnout_threshold(NUFPAR *fp) {
        CS_CutInfo->burnout_threshold = NuFParGetFloat(fp);
        CS_CutInfo->flags |= CUTINFO_BURNOUT;
    }

    static void CS_burnout_intensity(NUFPAR *fp) {
        CS_CutInfo->burnout_intensity = NuFParGetFloat(fp);
        CS_CutInfo->flags |= CUTINFO_BURNOUT;
    }

    static void CS_burnout_flare(NUFPAR *fp) {
        CS_CutInfo->burnout_flare = NuFParGetFloat(fp);
        CS_CutInfo->flags |= CUTINFO_BURNOUT;
    }

    static void CS_go_through_door(NUFPAR *fp) {
        if (NuFParGetWord(fp) != 0 && NuStrLen(fp->word_buffer_ptr) < 16) {
            NuStrCpy(CS_CutInfo->door_name, fp->word_buffer_ptr);
        }
    }

    static void CS_sfx(NUFPAR *fp) {
        if (NuFParGetWord(fp) != 0) {
            CS_CutInfo->legacy_music_index = GetMusicIndex(fp->word_buffer_ptr, MusicInfo, -1);
            CS_CutInfo->music_handle = music_man.GetTrackHandle(0x10, fp->word_buffer_ptr);
        }
    }

    static void CS_play_sfx(NUFPAR *fp) {
        if (NuFParGetWord(fp) == 0) {
            return;
        }

        i32 sfx_id = GetSfxId(fp->word_buffer_ptr);
        if (sfx_id == -1) {
            return;
        }

        i32 slot;
        for (slot = 0; slot < 6; ++slot) {
            if (CS_CutInfo->sfx[slot].id == -1) {
                break;
            }
        }
        if (slot >= 6) {
            return;
        }

        CS_CutInfo->sfx[slot].flags &= ~1U;
        CS_CutInfo->sfx[slot].id = static_cast<i16>(sfx_id);
        while (NuFParGetWord(fp) != 0) {
            if (NuStrICmp(fp->word_buffer_ptr, "frame") == 0) {
                f32 frame = NuFParGetFloat(fp);
                CS_CutInfo->sfx[slot].frame = frame;
                if (frame < 1.0f) {
                    CS_CutInfo->sfx[slot].frame = 1.0f;
                }
            } else if (NuStrICmp(fp->word_buffer_ptr, "pos") == 0) {
                if (NuFParGetFloat(fp) != 0.0f) {
                    CS_CutInfo->sfx[slot].position.x = NuAToF(fp->word_buffer_ptr);
                    if (NuFParGetFloat(fp) != 0.0f) {
                        CS_CutInfo->sfx[slot].position.y = NuAToF(fp->word_buffer_ptr);
                        if (NuFParGetFloat(fp) != 0.0f) {
                            CS_CutInfo->sfx[slot].position.z = NuAToF(fp->word_buffer_ptr);
                            CS_CutInfo->sfx[slot].flags |= 1;
                        }
                    }
                }
            }
        }
    }

    static void CS_goto_level(NUFPAR *fp) {
        if (NuFParGetWord(fp) == 0) {
            return;
        }
        i32 level_index;
        LEVELDATA *level = Level_FindByName(fp->word_buffer_ptr, &level_index);
        if (level_index != -1 && &LDataList[level_index] == HUB_LDATA && CS_area != -1) {
            i32 status_index;
            Area_FindStatusLevel(&ADataList[CS_area], &status_index);
            if (status_index != -1) {
                level_index = status_index;
            }
        }
        CS_CutInfo->skip_level = static_cast<i16>(level_index);
    }

    static void CS_next_cut_scene(NUFPAR *fp) {
        if (NuFParGetWord(fp) != 0 && NuStrLen(fp->word_buffer_ptr) < 64) {
            NuStrCpy(CS_CutInfo->next_cutscene, fp->word_buffer_ptr);
            CS_CutInfo->linked_audio = 1;
        }
    }

    static void CS_level_intro(NUFPAR *) {
        CS_CutInfo->flags |= CUTINFO_LEVEL_INTRO;
    }

    static void CS_nearclip(NUFPAR *fp) {
        CS_CutInfo->camera_near_clip = NuFParGetFloat(fp);
    }

    static void CS_farclip(NUFPAR *fp) {
        CS_CutInfo->camera_far_clip = static_cast<u16>(NuFParGetInt(fp));
    }

    static void CS_hold_audio(NUFPAR *) {
        CS_CutInfo->linked_audio = 1;
    }

    static void CS_tex_anim(NUFPAR *fp) {
        if (CS_texanimcount < 4) {
            i32 index = NuFParGetInt(fp);
            CS_CutInfo->texture_animations[CS_texanimcount].index = index;
            if (index != -1) {
                CUT_LOAD_TEXANIM *animation = &CS_CutInfo->texture_animations[CS_texanimcount];
                animation->frame = static_cast<f32>(NuFParGetInt(fp));
                if (1.0f <= animation->frame) {
                    ++CS_texanimcount;
                }
            }
        }
    }

    static void CS_fadefog(NUFPAR *fp) {
        if (CS_fadefogcount < 2) {
            f32 value = NuFParGetFloat(fp);
            CS_CutInfo->fade_fog[CS_fadefogcount].frame = value;
            if (value >= 0.0f) {
                value = NuFParGetFloat(fp);
                CS_CutInfo->fade_fog[CS_fadefogcount].near_distance = value;
                if (value >= 0.0f) {
                    value = NuFParGetFloat(fp);
                    CS_CutInfo->fade_fog[CS_fadefogcount].far_distance = value;
                    if (value >= 0.0f) {
                        value = NuFParGetFloat(fp);
                        CS_CutInfo->fade_fog[CS_fadefogcount].value = value;
                        if (value >= 0.0f) {
                            ++CS_fadefogcount;
                        }
                    }
                }
            }
        }
    }

    static void CS_subtitle(NUFPAR *fp) {
        if (CS_CutInfo->subtitle_data == NULL) {
            CS_CutInfo->subtitle_data = reinterpret_cast<CUT_LOAD_SUBTITLE *>(CS_buffptr->addr);
        }

        CUT_LOAD_SUBTITLE *subtitle = &CS_CutInfo->subtitle_data[CS_CutInfo->subtitle_count];
        subtitle->text_id = -1;
        subtitle->red = CUTSUBTITLEDEFAULT_R;
        subtitle->green = CUTSUBTITLEDEFAULT_G;
        subtitle->blue = CUTSUBTITLEDEFAULT_B;
        subtitle->alpha = CUTSUBTITLEDEFAULT_A;
        subtitle->alignment = CUTSUBTITLEDEFAULT_ALIGN;
        subtitle->start_frame = 0.0f;
        subtitle->end_frame = 0.0f;
        subtitle->x = CUTSUBTITLEDEFAULT_X;
        subtitle->y = CUTSUBTITLEDEFAULT_Y;
        subtitle->x_scale = CUTSUBTITLEDEFAULT_SCALEX;
        subtitle->y_scale = CUTSUBTITLEDEFAULT_SCALEY;
        subtitle->max_width = CUTSUBTITLEDEFAULT_FITWIDTH;
        subtitle->fade_time = 0.0f;

        while (NuFParGetWord(fp) != 0) {
            if (NuStrICmp(fp->word_buffer_ptr, "text_id") == 0) {
                subtitle->text_id = static_cast<i16>(NuFParGetInt(fp));
            } else if (NuStrICmp(fp->word_buffer_ptr, "colour") == 0) {
                subtitle->red = static_cast<u8>(NuFParGetFloat(fp) * 255.0f);
                subtitle->green = static_cast<u8>(NuFParGetFloat(fp) * 255.0f);
                subtitle->blue = static_cast<u8>(NuFParGetFloat(fp) * 255.0f);
                subtitle->alpha = static_cast<u8>(NuFParGetFloat(fp) * 128.0f);
            } else if (NuStrICmp(fp->word_buffer_ptr, "r") == 0) {
                subtitle->red = static_cast<u8>(NuFParGetFloat(fp) * 255.0f);
            } else if (NuStrICmp(fp->word_buffer_ptr, "g") == 0) {
                subtitle->green = static_cast<u8>(NuFParGetFloat(fp) * 255.0f);
            } else if (NuStrICmp(fp->word_buffer_ptr, "b") == 0) {
                subtitle->blue = static_cast<u8>(NuFParGetFloat(fp) * 255.0f);
            } else if (NuStrICmp(fp->word_buffer_ptr, "a") == 0) {
                subtitle->alpha = static_cast<u8>(NuFParGetFloat(fp) * 128.0f);
            } else if (NuStrICmp(fp->word_buffer_ptr, "align") == 0) {
                if (NuStrICmp(fp->word_buffer_ptr, "centre") == 0) {
                    subtitle->alignment = 0;
                } else if (NuStrICmp(fp->word_buffer_ptr, "top") == 0) {
                    subtitle->alignment = TEXT_ALIGN_TOP;
                } else if (NuStrICmp(fp->word_buffer_ptr, "top_right") == 0) {
                    subtitle->alignment = TEXT_ALIGN_TOP | TEXT_ALIGN_RIGHT;
                } else if (NuStrICmp(fp->word_buffer_ptr, "right") == 0) {
                    subtitle->alignment = TEXT_ALIGN_RIGHT;
                } else if (NuStrICmp(fp->word_buffer_ptr, "bottom_right") == 0) {
                    subtitle->alignment = TEXT_ALIGN_BOTTOM | TEXT_ALIGN_RIGHT;
                } else if (NuStrICmp(fp->word_buffer_ptr, "bottom") == 0) {
                    subtitle->alignment = TEXT_ALIGN_BOTTOM;
                } else if (NuStrICmp(fp->word_buffer_ptr, "bottom_left") == 0) {
                    subtitle->alignment = TEXT_ALIGN_BOTTOM | TEXT_ALIGN_LEFT;
                } else if (NuStrICmp(fp->word_buffer_ptr, "left") == 0) {
                    subtitle->alignment = TEXT_ALIGN_LEFT;
                } else if (NuStrICmp(fp->word_buffer_ptr, "top_left") == 0) {
                    subtitle->alignment = TEXT_ALIGN_TOP | TEXT_ALIGN_LEFT;
                }
            } else if (NuStrICmp(fp->word_buffer_ptr, "start_time") == 0) {
                subtitle->start_frame = CutscenesFabs(NuFParGetFloat(fp));
            } else if (NuStrICmp(fp->word_buffer_ptr, "end_time") == 0) {
                {
                    f32 fabs_value = (NuFParGetFloat(fp));
                    *(i32 *)&subtitle->end_frame = *(i32 *)&fabs_value & 0x7fffffff;
                }
            } else if (NuStrICmp(fp->word_buffer_ptr, "pos") == 0) {
                subtitle->x = NuFParGetFloat(fp);
                subtitle->y = NuFParGetFloat(fp);
            } else if (NuStrICmp(fp->word_buffer_ptr, "x") == 0) {
                subtitle->x = NuFParGetFloat(fp);
            } else if (NuStrICmp(fp->word_buffer_ptr, "y") == 0) {
                subtitle->y = NuFParGetFloat(fp);
            }

            if (NuStrICmp(fp->word_buffer_ptr, "scale") == 0) {
                subtitle->x_scale = NuFParGetFloat(fp);
                subtitle->y_scale = subtitle->x_scale;
            } else if (NuStrICmp(fp->word_buffer_ptr, "x_scale") == 0) {
                subtitle->x_scale = NuFParGetFloat(fp);
            } else if (NuStrICmp(fp->word_buffer_ptr, "y_scale") == 0) {
                subtitle->y_scale = NuFParGetFloat(fp);
            } else if (NuStrICmp(fp->word_buffer_ptr, "fit_width") == 0) {
                {
                    f32 fabs_value = (NuFParGetFloat(fp));
                    *(i32 *)&subtitle->max_width = *(i32 *)&fabs_value & 0x7fffffff;
                }
            } else if (NuStrICmp(fp->word_buffer_ptr, "fade_time") == 0) {
                {
                    f32 fabs_value = (NuFParGetFloat(fp));
                    *(i32 *)&subtitle->fade_time = *(i32 *)&fabs_value & 0x7fffffff;
                }
            }
        }

        if (subtitle->text_id >= 0 && subtitle->text_id < Text_GetMaxOverallStrings() && TTab[subtitle->text_id] != NULL &&
            subtitle->end_frame > subtitle->start_frame) {
            if (subtitle->fade_time > 0.0f && subtitle->fade_time * 2.0f > subtitle->end_frame - subtitle->start_frame) {
                subtitle->fade_time = 0.0f;
            }
            CS_buffptr->addr += sizeof(CUT_LOAD_SUBTITLE);
            ++CS_CutInfo->subtitle_count;
        }
    }

    static void CS_playonce(NUFPAR *) {
        CS_CutInfo->end_flags |= 1;
    }

    static void CS_nextcutscene_inplayablelevel(NUFPAR *) {
        CS_CutInfo->end_flags |= 2;
    }

    static void CS_cutsceneplayerobj(NUFPAR *fp) {
        if (CS_CutInfo->player_state_count >= 0x20 || NuFParGetWord(fp) == 0) {
            return;
        }
        if (NuSpecialFind(CS_worldinfo->current_gscn, &CS_CutInfo->player_states[CS_CutInfo->player_state_count].special,
                          fp->word_buffer_ptr, 1) == 0) {
            return;
        }
        CS_CutInfo->player_states[CS_CutInfo->player_state_count].on = 0;
        CS_CutInfo->player_states[CS_CutInfo->player_state_count].off = 0;
        while (NuFParGetWord(fp) != 0) {
            if (NuStrICmp(fp->word_buffer_ptr, "on") == 0) {
                CS_CutInfo->player_states[CS_CutInfo->player_state_count].on = 1;
                CS_CutInfo->player_states[CS_CutInfo->player_state_count].off = 0;
            } else if (NuStrICmp(fp->word_buffer_ptr, "off") == 0) {
                CS_CutInfo->player_states[CS_CutInfo->player_state_count].on = 0;
                CS_CutInfo->player_states[CS_CutInfo->player_state_count].off = 1;
            } else if (NuStrICmp(fp->word_buffer_ptr, "end_anim") == 0 || NuStrICmp(fp->word_buffer_ptr, "endanim") == 0 ||
                       NuStrICmp(fp->word_buffer_ptr, "anim_end") == 0 || NuStrICmp(fp->word_buffer_ptr, "animend") == 0) {
                CS_CutInfo->player_states[CS_CutInfo->player_state_count].on = 0;
                CS_CutInfo->player_states[CS_CutInfo->player_state_count].off = 0;
                CS_CutInfo->player_states[CS_CutInfo->player_state_count].end_anim = 1;
            }
        }
        ++CS_CutInfo->player_state_count;
    }

    i32 newlevel_resumecutaudio = 1;
    CUT_DRAW_SETTINGS CutSceneDrawSettings = {-1, -1, -1, -1, 0};
    CUT_DRAW_SETTINGS *CutSceneSys = &CutSceneDrawSettings;
    u8 CUTSUBTITLEDEFAULT_R = 255;
    u8 CUTSUBTITLEDEFAULT_G = 255;
    u8 CUTSUBTITLEDEFAULT_B = 255;
    u8 CUTSUBTITLEDEFAULT_A = 128;
    f32 CUTSUBTITLEDEFAULT_Y = -0.5f;
    f32 CUTSUBTITLEDEFAULT_SCALEX = 1.0f;
    f32 CUTSUBTITLEDEFAULT_SCALEY = 1.0f;
    f32 CUTSUBTITLEDEFAULT_FITWIDTH = 1.7f;
    NUFPCOMJMP CutScene_ConfigKeywords[] = {{const_cast<char *>("fpsec"), CS_fpsec},
                                            {const_cast<char *>("no_fog"), CS_no_fog},
                                            {const_cast<char *>("draw_world"), CS_draw_world},
                                            {const_cast<char *>("in_game"), CS_in_game},
                                            {const_cast<char *>("blobshadow_alpha"), CS_blobshadow_alpha},
                                            {const_cast<char *>("blobshadow_fadenear"), CS_blobshadow_fadenear},
                                            {const_cast<char *>("blobshadow_fadefar"), CS_blobshadow_fadefar},
                                            {const_cast<char *>("reflect_range"), CS_reflect_range},
                                            {const_cast<char *>("render_group"), CS_render_group},
                                            {const_cast<char *>("deb_page"), CS_deb_page},
                                            {const_cast<char *>("snap_out"), CS_snap_out},
                                            {const_cast<char *>("wipe_out"), CS_wipe_out},
                                            {const_cast<char *>("new_mode"), CS_new_mode},
                                            {const_cast<char *>("cam_only"), CS_cam_only},
                                            {const_cast<char *>("replace_players"), CS_replace_players},
                                            {const_cast<char *>("burnout_threshold"), CS_burnout_threshold},
                                            {const_cast<char *>("burnout_intensity"), CS_burnout_intensity},
                                            {const_cast<char *>("burnout_flare"), CS_burnout_flare},
                                            {const_cast<char *>("looping"), CS_looping},
                                            {const_cast<char *>("start_cam"), CS_start_cam},
                                            {const_cast<char *>("super_widescreen"), CS_super_widescreen},
                                            {const_cast<char *>("go_through_door"), CS_go_through_door},
                                            {const_cast<char *>("sfx"), CS_sfx},
                                            {const_cast<char *>("play_sfx"), CS_play_sfx},
                                            {const_cast<char *>("goto_level"), CS_goto_level},
                                            {const_cast<char *>("next_cut_scene"), CS_next_cut_scene},
                                            {const_cast<char *>("holdaudio"), CS_hold_audio},
                                            {const_cast<char *>("hold_audio"), CS_hold_audio},
                                            {const_cast<char *>("level_intro"), CS_level_intro},
                                            {const_cast<char *>("tex_anim"), CS_tex_anim},
                                            {const_cast<char *>("fadefog"), CS_fadefog},
                                            {const_cast<char *>("subtitle"), CS_subtitle},
                                            {const_cast<char *>("play_once"), CS_playonce},
                                            {const_cast<char *>("nextcutscene_inplayablelevel"), CS_nextcutscene_inplayablelevel},
                                            {const_cast<char *>("cutsceneplayerobj"), CS_cutsceneplayerobj},
                                            {const_cast<char *>("cutsceneplayer_obj"), CS_cutsceneplayerobj},
                                            {const_cast<char *>("nearclip_360"), CS_nearclip},
                                            {const_cast<char *>("farclip_360"), CS_farclip},
                                            {const_cast<char *>("nearclip_pc"), CS_nearclip},
                                            {const_cast<char *>("farclip_pc"), CS_farclip},
                                            {const_cast<char *>("nearclip"), CS_nearclip},
                                            {const_cast<char *>("farclip"), CS_farclip},
                                            {const_cast<char *>("drawgizmosys"), CS_draw_gizmo_sys},
                                            {NULL, NULL}};

    static void CutScene_Configure(CUTINFO *cut, char *name, VARIPTR *buf, VARIPTR *buf_end) {
        CUT_PLAYER_STATE states[32];
        CS_buffend = buf_end;
        CS_CutInfo = cut;
        CS_buffptr = buf;
        for (i32 i = 0; i < 6; ++i) {
            cut->sfx[i].id = -1;
        }
        cut->frames_per_second = 30.0f;
        cut->end_flags &= 0xfc;
        cut->legacy_music_index = -1;
        cut->music_handle = -1;
        cut->burnout_threshold = 1.0f;
        cut->debris_render_group = 2;
        cut->skip_level = -1;
        cut->burnout_intensity = 0.0f;
        cut->flags = 3;
        cut->burnout_flare = 0.0f;
        cut->camera_near_clip = 0.0f;
        cut->player_state_count = 0;
        cut->linked_audio = 0;
        cut->door_name[0] = 0;
        cut->next_cutscene[0] = 0;
        cut->camera_far_clip = 0;
        cut->player_states = states;
        cut->blob_shadow_alpha = 0xff;
        cut->blob_shadow_fade_near = 0xff;
        cut->blob_shadow_fade_far = 0xff;
        cut->reflection_range = 0xff;
        for (i32 i = 0; i < 4; ++i) {
            cut->texture_animations[i].index = -1;
        }
        for (i32 i = 0; i < 2; ++i) {
            cut->fade_fog[i].value = 0.0f;
            cut->fade_fog[i].far_distance = 0.0f;
            cut->fade_fog[i].near_distance = 0.0f;
            cut->fade_fog[i].frame = 0.0f;
        }
        cut->subtitle_data = NULL;
        cut->subtitle_count = 0;
        cut->subtitle_shown = 0;
        NUFPAR *fp = NuFParCreate(name);
        if (fp == NULL) {
            CutSceneConfigMissing(name);
            return;
        }
        CS_texanimcount = 0;
        CS_fadefogcount = 0;
        NuFParPushCom(fp, CutScene_ConfigKeywords);
        while (NuFParGetLine(fp) != 0) {
            if (NuFParGetWord(fp) != 0) {
                NuFParInterpretWord(fp);
            }
        }
        NuFParDestroy(fp);
        CUTINFO *current = CS_CutInfo;
        if (current->blob_shadow_fade_near > current->blob_shadow_fade_far) {
            current->blob_shadow_fade_near = current->blob_shadow_fade_far;
        }
        if (current->subtitle_data != NULL && current->subtitle_count <= 0) {
            current->subtitle_data = NULL;
        }
        if (current->player_state_count > 0) {
            buf->addr = (buf->addr + 3) & ~3U;
            current->player_states = reinterpret_cast<CUT_PLAYER_STATE *>(buf->void_ptr);
            u32 bytes = current->player_state_count * sizeof(CUT_PLAYER_STATE);
            PCByteCopy(current->player_states, states, bytes);
            buf->addr += bytes;
        } else {
            current->player_states = NULL;
        }
    }

    void *CutSceneFindModel(char *name, i32 model_only, i32 *character_id) {
        if (character_id != NULL) {
            *character_id = -1;
        }
        if (model_only != 0) {
            for (i32 i = 0; i < apicharsys->loaded_model_count; ++i) {
                if (NuStrICmp(name, CDataList[apicharsys->models[i].model_id].name) == 0) {
                    if (character_id != NULL) {
                        *character_id = apicharsys->models[i].model_id;
                    }
                    return &apicharsys->models[i];
                }
            }
        } else {
            for (i32 i = 0; i < 8; ++i) {
                GAMEOBJECT *object = Player[i];
                if (object != NULL && (object->flags_1fc & APIOBJECT_FLAG_IN_USE) != 0 &&
                    NuStrICmp(object->character_data->name, name) == 0) {
                    return object;
                }
            }
        }
        return NULL;
    }

    static void CutSceneFindCharacterPrivate(CUT_SCENE_LOAD *scene, CUT_LOAD_CHARACTER *character, CUTSYS *system) {
        i32 id;
        CUT_LOAD_LOCATOR_SYSTEM *locators = scene->locator_system;
        character->flags |= CUT_CHARACTER_MODEL_ONLY;
        character->model = CutSceneFindModel(character->name, character->flags & CUT_CHARACTER_MODEL_ONLY, &id);
        if (character->model == NULL) {
            CutSceneConfigMissing(character->name);
        } else if (id != -1) {
            system->character_bits[id / 32] |= 1U << (id & 31);
        }
        if (character->locator_count != 0 && character->locator.index < 0xff) {
            character->locator_index = static_cast<u8>(character->locator.index);
            character->locator.pointer = &locators->locators[character->locator.index];
        } else {
            character->locator_index = 0xff;
        }
    }

    static void CutScene_FindCharacters(CUT_SCENE_LOAD *scene) {
        CUT_LOAD_CHARACTER_SYSTEM *system = scene->character_system;
        for (i32 i = 0; i < system->count; ++i) {
            CutSceneFindCharacterPrivate(scene, &system->characters[i], CS_cutsys);
        }
    }

    f32 DEFAULTFRAMETIME = 0;

    static CUTSYS *CutScenesLoadListed(NUGSCN *gscn1, NUGSCN *gscn2, i32 fixup_mode, VARIPTR *buf, VARIPTR *buf_end, i32 area,
                                             i32 level, WORLDINFO *world) {
        CUTSYS *sys;
        void *initial;
        CUTINFO *cut;
        CUTINFO *entries[32];
        char path[128];
        char config_path[128];
        initial = buf->void_ptr;
        CS_area = area;
        sys = reinterpret_cast<CUTSYS *>((reinterpret_cast<u32>(initial) + 3) & ~3U);
        buf->addr = (reinterpret_cast<u32>(sys) + 15) & ~3U;
        sys->character_bits = buf->u32_ptr;
        CS_cutsys = sys;
        CS_worldinfo = world;
        buf->addr = (buf->addr + ((CHARCOUNT + 31) / 32) * 4 + 3) & ~3U;
        for (i32 i = 0; i < CUTCOUNT; ++i) {
            if (CutList[i].level != level) {
                continue;
            }
            void *mark = buf->void_ptr;
            entries[sys->count] = reinterpret_cast<CUTINFO *>(mark);
            buf->addr = (buf->addr + sizeof(CUTINFO) + 3) & ~3U;
            cut = entries[sys->count];
            NuStrCpy(path, "cut\\");
            NuStrCat(path, CutList[i].directory);
            NuStrCat(path, "\\");
            NuStrCat(path, CutList[i].name);
            NuStrCpy(config_path, path);
            NuStrCat(config_path, ".txt");
            CutScene_Configure(cut, config_path, buf, buf_end);
            if ((cut->flags & CUTINFO_IN_GAME) != 0 || InStory() != 0 ||
                (LDataList[level].flags & (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO)) != 0) {
                NuStrCpy(config_path, path);
                NuStrCat(config_path, ".cu2");
                cut->scene = NuGCutSceneLoad(config_path, buf, buf_end, 0);
                if (cut->scene != NULL) {
                    NuStrCpy(cut->name, CutList[i].name);
                    NuGCutSceneFixUp(cut->scene, gscn1, 0, fixup_mode);
                    NuGCutSceneFixUpExtra(cut->scene, gscn2);
                    cut->instance = instNuGCutSceneCreate(cut->scene, NULL, NULL, CutList[i].name, buf, 0);
                    if (cut->instance != NULL) {
                        cut->instance->rate = cut->frames_per_second * DEFAULTFRAMETIME;
                        buf->addr = (buf->addr + 15) & ~15U;
                    }
                    ++sys->count;
                    continue;
                }
            }
            buf->void_ptr = mark;
        }
        if (sys->count <= 0) {
            buf->void_ptr = initial;
            return NULL;
        }
        sys->records = reinterpret_cast<CUTINFO **>(buf->void_ptr);
        PCByteCopy(sys->records, entries, sys->count * 4);
        buf->addr = (buf->addr + sys->count * 4 * sys->count + 3) & ~3U;
        return sys;
    }
}

extern "C" CUTSYS *CutScenes_Load(char *config, NUGSCN *gscn1, NUGSCN *gscn2, i32 param1, VARIPTR *buf, VARIPTR *buf_end, i32 param2,
                                        i32 param3, WORLDINFO *world) {
    NUFPAR *fp;
    CUTSYS *sys;
    void *initial;
    CUTINFO *cut;
    char name[128];
    char full_path[128];
    char config_name[128];
    CUTINFO *entries[32];

    if (CutList != NULL && CUTCOUNT > 0) {
        return CutScenesLoadListed(gscn1, gscn2, param1, buf, buf_end, param2, param3, world);
    }
    if (config == NULL) {
        return NULL;
    }

    fp = NuFParCreateMem("cutscenes", config, 0xffff);
    if (fp == NULL) {
        return NULL;
    }

    initial = buf->void_ptr;
    sys = (CUTSYS *)((reinterpret_cast<u32>(initial) + 3) & ~3U);
    buf->addr = (reinterpret_cast<u32>(sys) + 15) & ~3U;
    sys->character_bits = buf->u32_ptr;
    CS_worldinfo = world;
    CS_area = param2;
    CS_cutsys = sys;
    buf->addr = (buf->addr + ((CHARCOUNT + 31) / 32) * 4 + 3) & ~3U;

    while (NuFParGetLine(fp) != 0) {
        if (NuFParGetWord(fp) == 0 || NuStrICmp(fp->word_buffer_ptr, "cutscene") != 0 || sys->count >= 32 || NuFParGetWord(fp) == 0) {
            continue;
        }

        void *mark = buf->void_ptr;
        entries[sys->count] = reinterpret_cast<CUTINFO *>(mark);
        buf->addr = (buf->addr + sizeof(CUTINFO) + 3) & ~3U;
        cut = entries[sys->count];
        NuStrLwr(name, fp->word_buffer_ptr);
        i32 dot = NuStrLen(name) - 1;
        while (dot > 0 && name[dot] != '.') {
            --dot;
        }
        if (name[dot] == '.') {
            name[dot] = '\0';
        }

        if (name[0] == 'c' && name[1] == 'u' && name[2] == 't' && name[3] == '\\') {
            NuStrCpy(name, name + 4);
        }
        NuStrCpy(config_name, name);
        if (CutScene_OverrideConfigFileNameFn != NULL) {
            CutScene_OverrideConfigFileNameFn(config_name, param2, param3);
        }
        NuStrCpy(full_path, "cut\\");
        NuStrCat(full_path, config_name);
        NuStrCat(full_path, ".txt");
        CutScene_Configure(cut, full_path, buf, buf_end);
        NuStrCpy(full_path, "cut\\");
        NuStrCat(full_path, name);
        NuStrCat(full_path, ".txt");

        if ((cut->flags & CUTINFO_IN_GAME) != 0 || InStory() != 0 ||
            (LDataList[param3].flags & (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO)) != 0) {
            buf->void_ptr = (char *)((buf->addr + 63) & ~63U);
            NuStrCpy(full_path, "cut\\");
            NuStrCat(full_path, name);
            NuStrCat(full_path, ".cu2");
            cut->scene = NuGCutSceneLoad(full_path, buf, buf_end, 0);
            if (cut->scene == NULL) {
                NuStrCpy(full_path, "cut\\");
                NuStrCat(full_path, name);
                NuStrCat(full_path, ".cut");
                cut->scene = NuGCutSceneLoad(full_path, buf, buf_end, 0);
            }
            if (cut->scene != NULL) {
                i32 separator = NuStrLen(name) - 1;
                while (separator >= 0 && name[separator] != '\\') {
                    --separator;
                }
                if (separator < 0) {
                    NuStrCpy(cut->name, name);
                } else {
                    NuStrCpy(cut->name, name + separator + 1);
                }
                NuGCutSceneFixUp(cut->scene, gscn1, 0, param1);
                NuGCutSceneFixUpExtra(cut->scene, gscn2);
                cut->instance = instNuGCutSceneCreate(cut->scene, NULL, NULL, name, buf, 0);
                if (cut->instance != NULL) {
                    cut->instance->rate = cut->frames_per_second * DEFAULTFRAMETIME;
                    buf->void_ptr = (char *)((buf->addr + 15) & ~15U);
                }
                sys->count++;
                continue;
            }
        }
        buf->void_ptr = mark;
    }
    NuFParDestroy(fp);
    if (sys->count > 0) {
        sys->records = reinterpret_cast<CUTINFO **>(buf->void_ptr);
        u32 size = sys->count * sizeof(CUTINFO *);
        PCByteCopy(sys->records, entries, size);
        // Reserves count pointer-table rows after copying the count entries.
        buf->addr = (buf->addr + sys->count * size + 3) & ~3U;
        return sys;
    }
    buf->void_ptr = initial;
    return NULL;
}

extern "C" {

    void *CutScene_FindInst(CUTSYS *system, char *name) {
        if (system == NULL) {
            return NULL;
        }
        for (i32 index = 0; index < system->count; ++index) {
            if (NuStrICmp(system->records[index]->name, name) == 0) {
                return system->records[index]->instance;
            }
        }
        return NULL;
    }
}

CUTINFO *CutScene_Find(CUTSYS *system, char *name) {
    i32 index;

    if (system != NULL && name != NULL) {
        for (index = 0; index < system->count; ++index) {
            if (NuStrICmp(system->records[index]->name, name) == 0) {
                return system->records[index];
            }
        }
    }
    return NULL;
}

extern "C" {

    static void CutSceneRebindPlayerCharactersPrivate(CUTSYS *system) {
        if (system == NULL) {
            return;
        }
        for (i32 i = 0; i < system->count; ++i) {
            CUTINFO *cut = system->records[i];
            if (cut->instance != NULL) {
                CUT_LOAD_CHARACTER_SYSTEM *definitions = reinterpret_cast<CUT_SCENE_LOAD *>(cut->scene)->character_system;
                CUT_LOAD_CHARACTER_INSTANCE_SYSTEM *states = cut->instance->character_instance;
                if (definitions != NULL && states != NULL) {
                    for (i32 j = 0; j < definitions->count; ++j) {
                        CUT_LOAD_CHARACTER *definition = &definitions->characters[j];
                        CUT_LOAD_CHARACTER_INSTANCE *state = &states->characters[j];
                        if (state->model == reinterpret_cast<void *>(1)) {
                            if ((state->flags & 2) != 0) {
                                state->model = NULL;
                            } else {
                                state->model = CutSceneFindModel(definition->name, state->flags & 2, NULL);
                            }
                            definition->model = state->model;
                        }
                    }
                }
            }
        }
    }

    static void CutSceneResetTextureAnimationsPrivate(CUTINFO *cut) {
        for (i32 i = 0; i < 4; ++i) {
            i32 index = cut->texture_animations[i].index;
            if (index != -1) {
                texanimbits &= ~(1U << index);
            }
        }
    }

    static void CutSceneUpdateTextureAnimationsPrivate(CUTINFO *cut) {
        u32 bits = texanimbits;
        for (i32 i = 0; i < 4; ++i) {
            i32 index = cut->texture_animations[i].index;
            if (index != -1) {
                u32 bit = 1U << index;
                if ((bits & bit) == 0 && cut->instance->current_frame >= cut->texture_animations[i].frame) {
                    bits |= bit;
                    texanimbits = bits;
                }
            }
        }
    }

    static void CutScene_EvalCharacter(INSTNUGCUTSCENE *owner, CUT_SCENE_LOAD *, CUT_LOAD_CHARACTER_INSTANCE *instance,
                                       CUT_LOAD_CHARACTER *character, f32 frame) {
        GAMEOBJECT *object = NULL;
        NUMTX matrix;
        i32 visible;
        if ((character->flags & CUT_CHARACTER_MODEL_ONLY) == 0) {
            object = static_cast<GAMEOBJECT *>(instance->model);
        }
        NuGCutCharAnimProcess(character, frame, &matrix, &visible, NULL, NULL, NULL, NULL, NULL);
        INSTNUGCUTSCENE *state = owner;
        if ((state->flags_88 & 0x80) != 0) {
            NuMtxMul(&matrix, &matrix, &state->matrix);
        }
        if (object != NULL) {
            object->transform = matrix;
            object->position = matrix.translation;
        }
    }

    static void CutScene_CreateCharacterInstance(CUT_LOAD_CHARACTER *character, CUT_LOAD_CHARACTER_INSTANCE *instance, VARIPTR *) {
        if ((character->flags & CUT_CHARACTER_MODEL_ONLY) != 0) {
            instance->model = character->model;
        }
    }

    static i32 CutScenePlayOncePrivate(WORLDINFO *world, CUTINFO *cut) {
        CUTSYS *system = world->cutscene_sys;
        i16 index = -1;
        if (system != NULL) {
            for (i32 i = 0; i < system->count; ++i) {
                if (system->records[i] == cut) {
                    index = i;
                }
            }
            if (world->level_progress != NULL) {
                u32 bit = 1 << index;
                if (index != -1 && (cut->end_flags & 1) != 0 && (world->level_progress->progress_state & bit) != 0) {
                    return 1;
                }
                world->level_progress->progress_state |= bit;
            }
        }
        return 0;
    }
}

CUTINFO *NewCutScene(CUTINFO *cutscene, CUTSYS *system, char *name, i32 unused_4) {
    i32 count;

    if (cutscene == NULL) {
        if (system == NULL || name == NULL) {
            return NULL;
        }
        cutscene = CutScene_Find(system, name);
        if (cutscene == NULL) {
            return NULL;
        }
    }
    count = NewCutInfoCount;
    if (count < 8) {
        NewCutInfo[count] = cutscene;
        NewCutInfoCount = count + 1;
        return cutscene;
    }
    return NULL;
}

extern "C" i32 CutScene_PlayingOrRequested(CUTINFO *cut) {
    if (cut != NULL) {
        if (cut == CutStopInfo) {
            return 1;
        }
        for (i32 i = 0; i < NewCutInfoCount; ++i) {
            if (cut == NewCutInfo[i]) {
                return 1;
            }
        }
    } else if (CutStopInfo != NULL || NewCutInfoCount != 0) {
        return 1;
    }
    return 0;
}

extern "C" {

    i32 CutScene_HasPlayed(CUTINFO *cutscene) {
        CUTSYS *system = WorldInfo_CurrentlyActive()->cutscene_sys;
        i32 found;
        i32 index;
        if (system != NULL && cutscene != NULL) {
            found = -1;
            for (index = 0; index < system->count; ++index) {
                if (system->records[index] == cutscene) {
                    found = index;
                }
            }
            if (found != -1) {
                return (WorldInfo_CurrentlyActive()->level_progress->progress_state & (1 << found)) != 0;
            }
        }
        return 0;
    }

    static void SetStopCutFlags(CUTINFO *cut) {
        CUTSTOPGAME = 1;
        CutStopInfo = cut;
        CUTDRAWWORLD = cut->flags & CUTINFO_DRAW_WORLD;
        CUTNOFOG = cut->flags & CUTINFO_NO_FOG;
        DebrisSetRenderGroup(cut->debris_render_group);
        LevelNameMul = 0.0f;
        LevelNameTime = 0.0f;
        PCFrameSetLoadingYieldEnabled(1);
    }
}

void CutScene_SnapToEnd(void *cut) {
    if (cut != NULL) {
        instNuGCutSceneEnd(((CUTINFO *)cut)->instance);
    }
}

extern "C" {

    void CutScenes_Stop(CUTSYS *system) {
        if (system == NULL) {
            return;
        }
        for (i32 i = 0; i < system->count; ++i) {
            INSTNUGCUTSCENE *instance = system->records[i]->instance;
            if (instance != NULL) {
                instNuGCutSceneStop(instance);
            }
        }
    }
}

void CutScenes_Destroy(CUTSYS *system) {
    i32 i;
    if (system == NULL) {
        return;
    }
    for (i = 0; i < system->count; ++i) {
        if (system->records[i]->instance != NULL) {
            instNuGCutSceneDestroy(system->records[i]->instance);
            system->records[i]->instance = NULL;
        }
    }
    for (i = 0; i < system->count; ++i) {
        if (system->records[i]->scene != NULL) {
            NuGCutSceneDestroy(system->records[i]->scene);
            system->records[i]->scene = NULL;
        }
    }
}

/* PC never lets a cutscene be skipped here: both outcomes of the test return 0. */
i32 CutScene_IsSkippable(CUTINFO *cut) {
    if (FadeSys.fade == 0.0f && cut != NULL && cut != CutStopInfo) {
        return 0;
    }
    return 0;
}

extern "C" {

    static void LocatorFunction_Blaster(INSTNUGCUTSCENE *, CUT_LOAD_LOCATOR_SYSTEM *, CUT_LOAD_LOCATOR_INSTANCE *,
                                        CUT_LOAD_LOCATOR *locator, f32 frame, NUMTX *parent, i32) {
        NUANIM_TIME2 time;
        NuAnimData2CalcTime(&locator->animation->prefix, frame, &time);
        if (NuGCutLocatorIsVisble(locator, frame, &time, NULL, NULL) == 0) {
            return;
        }
        NUMTX matrix;
        NuGCutLocatorCalcMtx(locator, frame, &matrix, &time);
        if ((locator->flags & 4) != 0) {
            NuMtxPreTranslate(&matrix, &locator->pivot);
        }
        if (parent != NULL) {
            NuMtxMul(&matrix, &matrix, parent);
        }
        Draw3DObjectMtx(NULL, CutSceneSys->blaster_object_0, &matrix);
        Draw3DObjectMtx(NULL, CutSceneSys->blaster_object_1, &matrix);
    }

    static void CutScene_ResetCharacters(INSTNUGCUTSCENE *owner) {
        INSTNUGCUTSCENE *instance = owner;
        CUT_LOAD_CHARACTER_SYSTEM *system = instance->scene->character_system;
        CUT_LOAD_CHARACTER_INSTANCE_SYSTEM *states = instance->character_instance;
        for (i32 i = 0; i < system->count; ++i) {
            CUT_LOAD_CHARACTER_INSTANCE *state = &states->characters[i];
            state->blend_progress = 0;
            state->flags = 0;
            state->current_animation = 0xff;
        }
    }

    static f32 CutReflectRange = 0;

    void CutScenes_Draw(WORLDINFO *world) {
        if (world->cutscene_sys == NULL || ACTIVECUTCOUNT <= 0) {
            return;
        }
        SetLevelLights(world->rtl_set, 1.0f);
        CUTINFO *cut = CutStopInfo;
        if (cut != NULL) {
            const i32 fade_near = cut->blob_shadow_fade_near;
            const u8 alpha = cut->blob_shadow_alpha;
            const i32 reflection = cut->reflection_range;
            CutBlobShadowAlpha = alpha;
            const i32 fade_far = cut->blob_shadow_fade_far;
            CutBlobShadowFadeNear = static_cast<f32>(fade_near);
            CutBlobShadowFadeFar = static_cast<f32>(fade_far);
            CutReflectRange = static_cast<f32>(reflection);
        } else {
            LEVELDATA *level = world->current_level;
            const i32 fade_near = level->blob_shadow_fade_near;
            const u8 alpha = level->blob_shadow_alpha;
            const i32 reflection = level->reflection_range;
            CutBlobShadowAlpha = alpha;
            const i32 fade_far = level->blob_shadow_fade_far;
            CutBlobShadowFadeNear = static_cast<f32>(fade_near);
            CutBlobShadowFadeFar = static_cast<f32>(fade_far);
            CutReflectRange = static_cast<f32>(reflection);
        }
        CutReflectRange2 = CutReflectRange * CutReflectRange;
        NuGCutSceneSysRender(Paused);
        if (CUTDRAWWORLD != 0 && world->current_gscn != NULL) {
            SetLevelLights(world->rtl_set, 1.0f);
            NuGScnRndr3(world->current_gscn);
            if (world->gizmo_sys != NULL && CutStopInfo != NULL && (CutStopInfo->flags & CUTINFO_DRAW_GIZMO_SYS) != 0) {
                GizmoSysDraw(world->gizmo_sys, world, FRAMETIME);
            }
        }
    }

    static void bgAckStreamCutScene(BGPROCINFO *) {
        stream_cut_issued = 0;
    }

    static void bgLoadStreamCutScene(BGPROCINFO *) {
        if (cutscene_load_instance != NULL) {
            /* Debug trace of the frame counter; the print is compiled empty. */
            CutSceneConfigMissing(reinterpret_cast<char *>(nuvideo_global_vbcnt));
            instNuGCutSceneServiceLoad();
        }
    }
}

extern "C" void CutScenes_BGLoadManager() {
    if (cutscene_load_instance != NULL && stream_cut_issued == 0) {
        stream_cut_issued = 1;
        bgPostRequest(bgLoadStreamCutScene, bgAckStreamCutScene, NULL, 0);
    }
    NuGCutSceneSysPostBackgroundLoad();
}

extern "C" {

    static void CutScene_RigidPostRender(CUT_DRAW_RIGID *rigid, nuhspecial_s *instance, NUMTX *matrix) {
        if ((rigid->flags & 0x10) == 0) {
            return;
        }
        NUVEC *position = &matrix->translation;
        if (!(NuVecDistSqr(position, &GameCam->render_mtx.translation, NULL) < CutReflectRange2)) {
            return;
        }
        f32 ground = GameShadow(NULL, position, 5.0f, -1);
        if ((f64)ground == 2000000.0) {
            return;
        }
        i32 surface = ShadowInfo();
        if (static_cast<u32>(surface) > 0x20 || (TerSurface[surface].flags & TERSURFACE_REFLECTIVE) == 0) {
            return;
        }
        WORLDINFO *world = WorldInfo_CurrentlyActive();
        NUMTX reflection = *matrix;
        reflection.m01 = -reflection.m01;
        reflection.m11 = -reflection.m11;
        reflection.m21 = -reflection.m21;
        reflection.m31 = ground - (reflection.m31 - ground);
        nuhspecial_s *special = NULL;
        i32 index = LevelObject_FindIndexFromName_RefOnly(NuSpecialGetName(instance));
        if (index != -1) {
            index = LevelObject_GetReflection(index);
            if (index != -1) {
                special = &world->level_objects[index].special;
            }
        }
        if (special == NULL) {
            special = instance;
        }
        NuRndrStartReflectionRender(0);
        NuSpecialDrawAt(special, &reflection);
        NuRndrEndReflectionRender();
    }

    static void CutScene_DrawCharacter(INSTNUGCUTSCENE *cutscene_instance, CUT_SCENE_LOAD *cutscene, CUT_LOAD_CHARACTER_INSTANCE *instance,
                                       CUT_LOAD_CHARACTER *character, f32 frame, i32 paused) {
        f32 reflection_height = 2000000.0f;
        i32 visible;
        i32 layer_mask = -1;
        GAMEOBJECT *scene_object = NULL;
        WORLDINFO *world = WorldInfo_CurrentlyActive();
        CUTSYS *cutscene_system = world->cutscene_sys;
        if (cutscene_system == NULL) {
            return;
        }

        CUTINFO *cut = NULL;
        for (i32 i = 0; i < cutscene_system->count && cut == NULL; ++i) {
            CUTINFO *candidate = cutscene_system->records[i];
            if (candidate->instance == cutscene_instance) {
                cut = candidate;
            }
        }

        CHARACTER_MODEL_LAYERS *model;
        i32 replacement_id;
        if (cut != NULL && CutScene_ReplaceCharacterModelFn != NULL &&
            (replacement_id = CutScene_ReplaceCharacterModelFn(cut, reinterpret_cast<CUTSCENECHARACTER *>(character))) != -1 &&
            apicharsys->playermodelids[replacement_id] != -1) {
            model = &apicharsys->models[apicharsys->playermodelids[replacement_id]];
        } else if ((character->flags & CUT_CHARACTER_MODEL_ONLY) != 0) {
            model = static_cast<CHARACTER_MODEL_LAYERS *>(instance->model);
        } else {
            scene_object = static_cast<GAMEOBJECT *>(instance->model);
            model = scene_object->character_model;
        }
        NUHGOBJ_EVAL *hierarchy = model->hierarchy;
        if (hierarchy == NULL) {
            return;
        }

        const i32 character_id = model->model_id;
        CHARACTERDATA *character_data = reinterpret_cast<CHARACTERDATA *>(&apicharsys->char_data[character_id]);
        PLAYERCHARACTERCONFIG *game_data = character_data->game_character;
        NUMTX world_matrix;
        u32 animation_index;
        f32 animation_rate;
        f32 blend_time;
        f32 animation_start_frame;
        NuGCutCharAnimProcess(character, frame, &world_matrix, &visible, &animation_index,
                              &animation_rate, &blend_time, &animation_start_frame, &layer_mask);
        if (paused != 0) {
            animation_rate = 0.0f;
        }
        if (layer_mask == -1) {
            if (CutSceneSys->normal_layers != 0) {
                layer_mask = static_cast<i32>(game_data->layer_mask);
            } else {
                layer_mask = static_cast<i32>(game_data->layer_mask_special);
            }
        }
        if ((cutscene_instance->flags_88 & 0x80) != 0) {
            NuMtxMul(&world_matrix, &world_matrix, &cutscene_instance->matrix);
        }
        if (scene_object != NULL) {
            scene_object->transform = world_matrix;
            scene_object->position = world_matrix.translation;
        }
        if (visible == 0) {
            return;
        }

        i16 render_indices[32];
        const i32 render_count = game_data->make_layer_list(model, render_indices, static_cast<u32>(layer_mask));
        if (render_count < 1) {
            return;
        }

        if (animation_index != instance->current_animation) {
            const u8 requested_animation = static_cast<u8>(animation_index);
            if (!(blend_time > 0.0f) || instance->current_animation == 0xff) {
                instance->animation_a = requested_animation;
                if (animation_start_frame > 1.0f) {
                    instance->frame_a = animation_start_frame;
                } else {
                    instance->frame_a = 1.0f;
                }
            } else {
                if ((instance->flags & 1) != 0) {
                    instance->animation_a = instance->animation_b;
                    instance->frame_a = instance->frame_b;
                } else if (instance->current_animation > 0) {
                    instance->animation_a = instance->current_animation;
                } else {
                    instance->animation_a = requested_animation;
                    instance->frame_a = frame;
                }
                instance->animation_b = requested_animation;
                if (animation_start_frame > 1.0f) {
                    instance->frame_b = animation_start_frame;
                } else {
                    instance->frame_b = 1.0f;
                }
                if (requested_animation != instance->animation_a) {
                    instance->blend_progress = 0.0f;
                    instance->flags |= 1;
                }
            }
            instance->current_animation = requested_animation;
        }

        if ((instance->flags & 1) != 0) {
            if (blend_time <= 0.0f) {
                instance->blend_progress = 1.0f;
            } else {
                instance->blend_progress += (1.0f / blend_time) * (FRAMETIME * 60.0f);
            }
            if (instance->blend_progress >= 1.0f) {
                instance->blend_progress = 0.0f;
                instance->frame_a = instance->frame_b;
                instance->animation_a = instance->animation_b;
                instance->flags ^= 1;
            }
        }

        ANI3_HEADER *animation_a;
        NUANIMDATA2 *dwa_animation_a;
        if (instance->animation_a == 0) {
            instance->frame_a = frame;
            animation_a = character->face_animation;
            dwa_animation_a = character->extra_animation;
        } else {
            animation_a = static_cast<ANI3_HEADER *>(model->model_data_b[instance->animation_a - 1]);
            dwa_animation_a = model->model_data_c[instance->animation_a - 1];
        }

        NUMTX joint_matrices[256];
        DWA_WEIGHT_BLOCK **dwa;
        if ((instance->flags & 1) != 0) {
            ANI3_HEADER *animation_b;
            NUANIMDATA2 *dwa_animation_b;
            if (instance->animation_b == 0) {
                instance->frame_b = frame;
                animation_b = character->face_animation;
                dwa_animation_b = character->extra_animation;
            } else {
                animation_b = static_cast<ANI3_HEADER *>(model->model_data_b[instance->animation_b - 1]);
                dwa_animation_b = model->model_data_c[instance->animation_b - 1];
            }
            if (animation_a != NULL) {
                if (animation_b != NULL) {
                    NuHGobjEvalAnimBlend2(hierarchy, animation_a, instance->frame_a, animation_b, instance->frame_b,
                                          instance->blend_progress, 0, NULL, joint_matrices);
                } else {
                    NuHGobjEvalAnim2(hierarchy, animation_a, instance->frame_a, 0, NULL, joint_matrices);
                }
            } else if (animation_b != NULL) {
                NuHGobjEvalAnim2(hierarchy, animation_b, instance->frame_b, 0, NULL, joint_matrices);
            } else {
                NuHGobjEval(hierarchy, 0, NULL, joint_matrices);
            }

            if (dwa_animation_a != NULL && dwa_animation_b != NULL) {
                dwa = NuHGobjEvalDwaBlend2(render_count, render_indices, dwa_animation_a, instance->frame_a, dwa_animation_b,
                                           instance->frame_b, instance->blend_progress);
            } else {
                dwa = NULL;
            }

            if (animation_b != NULL && instance->animation_b != 0xff && instance->animation_b > 0 &&
                (cut == NULL || cut != CutStopInfo || CutSceneWaiting == 0)) {
                const i32 current_animation = instance->animation_b - 1;
                const f32 end_frame = AnimEndFrame(model, current_animation);
                instance->frame_b += FRAMETIME * 60.0f * animation_rate;
                if (instance->frame_b > end_frame) {
                    CHARACTERANIM_CONFIG *animation_info = model->animation_configs[current_animation];
                    if ((animation_info->flags & CHARANIM_CYCLE) != 0) {
                        instance->frame_b = instance->frame_b - end_frame + 1.0f;
                    } else {
                        instance->frame_b = end_frame;
                    }
                }
            }
        } else {
            if (animation_a != NULL) {
                NuHGobjEvalAnim2(hierarchy, animation_a, instance->frame_a, 0, NULL, joint_matrices);
            } else {
                NuHGobjEval(hierarchy, 0, NULL, joint_matrices);
            }
            if (dwa_animation_a != NULL) {
                dwa = NuHGobjEvalDwa2(render_count, render_indices, dwa_animation_a, instance->frame_a);
            } else {
                dwa = NULL;
            }
        }

        NUVEC locator_positions[16];
        NUMTX locator_matrices[16];
        NUMTX locator_parent;
        PCStoreModelLocatorCoordinates(model, &world_matrix, joint_matrices, locator_positions, locator_matrices);

        if (animation_a != NULL && instance->animation_a != 0xff && instance->animation_a > 0 &&
            (cut == NULL || cut != CutStopInfo || CutSceneWaiting == 0)) {
            const i32 current_animation = instance->animation_a - 1;
            const f32 end_frame = AnimEndFrame(model, current_animation);
            instance->frame_a += FRAMETIME * 60.0f * animation_rate;
            if (instance->frame_a > end_frame) {
                CHARACTERANIM_CONFIG *animation_info = model->animation_configs[current_animation];
                if ((animation_info->flags & CHARANIM_CYCLE) != 0) {
                    instance->frame_a = instance->frame_a - end_frame + 1.0f;
                } else {
                    instance->frame_a = end_frame;
                }
            }
        }

        EnableShadowMapRendering(0);
        if (Cheats_CheckFlags(CHEAT_ZERO_LIGHTING) != 0) {
            SetZeroLights();
        } else {
            FindAndSetLights(&world_matrix.translation, 1.0f, world->rtl_set);
        }

        if ((character->flags & CUT_CHARACTER_REFLECTION) == 0 && (character_data->model_flags & CHARACTER_MODEL_COMPLEX_SHADOW) != 0 &&
            (character->flags & CUT_CHARACTER_SHADOW) != 0) {
            hierarchy->suppress_shadow_points = 0;
        } else {
            hierarchy->suppress_shadow_points = 1;
        }

        if ((game_data->flags_090 & CHARACTER_RUNTIME_GHOST) != 0) {
            APITransparentCharDraw(hierarchy, &world_matrix, render_count, render_indices, joint_matrices, dwa, character->flags & 8);
        }
        NuHGobjRndrMtxDwa(hierarchy, &world_matrix, render_count, render_indices, joint_matrices, dwa, character->flags & 8);

        NUMTX *attachment_matrix = NULL;
        if ((character_data->flags & 1) != 0) {
            i32 locator = game_data->thingy_locator;
            if (locator != -1 && model->points_of_interest[locator] != NULL) {
                attachment_matrix = &locator_matrices[locator];
            } else {
                attachment_matrix = &world_matrix;
            }
            CharScene_Draw(world, character_id, attachment_matrix, NULL);
        }
        ResetShadowMapRendering();

        f32 ground_height = 2000000.0f;
        i32 surface = -1;
        i32 platform = -1;
        i32 shadow_sampled = 0;
        if ((character->flags & CUT_CHARACTER_SHADOW) != 0 && CutBlobShadowAlpha > 0 &&
            (character_data->model_flags & CHARACTER_MODEL_COMPLEX_SHADOW) == 0 && PcWindowOptions.shadowmethod == 1 &&
            PcWindowOptions.shadowsenabled != 0) {
            i32 shadow_alpha;
            if (game_data->blob_shadow_alpha < 0xff) {
                shadow_alpha = game_data->blob_shadow_alpha;
            } else {
                shadow_alpha = CutBlobShadowAlpha;
            }
            if (shadow_alpha > 0) {
                f32 shadow_radius;
                if (game_data->shadow_locators != 0) {
                    shadow_radius = game_data->shadow_radius < 99.0f ? game_data->shadow_radius
                                                                     : character_data->model_scale * character_data->collision_radius;
                    if (shadow_radius > 0.0f) {
                        f32 alpha = BlobShadowFade(&world_matrix.translation, CutBlobShadowFadeNear, CutBlobShadowFadeFar, 1.0f);
                        if (alpha > 0.0f) {
                            alpha *= shadow_alpha;
                            for (i32 i = 0; i < 16; ++i) {
                                if ((game_data->shadow_locators & (1 << i)) != 0 && model->points_of_interest[i] != NULL) {
                                    NUVEC shadow_position;
                                    shadow_position.x = locator_positions[i].x;
                                    f32 locator_ground = GameShadow(NULL, &locator_positions[i], 5.0f, -1);
                                    shadow_position.z = locator_positions[i].z;
                                    if (locator_ground != 2000000.0f && ShadNorm.y > 0.0f) {
                                        shadow_position.y = locator_ground + 0.005f;
                                        u16 x_rotation;
                                        u16 z_rotation;
                                        FindAnglesZX(&ShadNorm, &x_rotation, &z_rotation);
                                        NuRndrAddShadow(&shadow_position, shadow_radius, static_cast<i32>(alpha * ShadNorm.y), x_rotation,
                                                        0, z_rotation);
                                    }
                                }
                            }
                        }
                    }
                } else {
                    shadow_radius = game_data->shadow_radius < 99.0f
                                        ? game_data->shadow_radius
                                        : character_data->model_scale * character_data->collision_radius * 2.0f;
                    if (shadow_radius > 0.0f) {
                        ground_height = GameShadow(NULL, &world_matrix.translation, 5.0f, -1);
                        shadow_sampled = 1;
                        if (ground_height != 2000000.0f) {
                            platform = NewShadowOnPlatform();
                            surface = ShadowInfo();
                            if (ShadNorm.y > 0.0f) {
                                NUVEC shadow_position;
                                shadow_position.x = world_matrix.m30;
                                shadow_position.y = ground_height + 0.005f;
                                shadow_position.z = world_matrix.m32;
                                f32 alpha = BlobShadowFade(&shadow_position, CutBlobShadowFadeNear, CutBlobShadowFadeFar, ShadNorm.y);
                                if (alpha > 0.0f) {
                                    u16 x_rotation;
                                    u16 z_rotation;
                                    FindAnglesZX(&ShadNorm, &x_rotation, &z_rotation);
                                    NuRndrAddShadow(&shadow_position, shadow_radius, static_cast<i32>(shadow_alpha * alpha * ShadNorm.y),
                                                    x_rotation, 0, z_rotation);
                                }
                            }
                        }
                    }
                }
            }
        }

        if ((character->flags & CUT_CHARACTER_REFLECTION) != 0 && (game_data->flags_090 & CHARACTER_RUNTIME_GHOST) == 0 &&
            NuVecDistSqr(&world_matrix.translation, &GameCam->render_mtx.translation, NULL) < CutReflectRange2) {
            if (shadow_sampled == 0) {
                ground_height = GameShadow(NULL, &world_matrix.translation, 5.0f, -1);
                if (ground_height != 2000000.0f) {
                    platform = NewShadowOnPlatform();
                    surface = ShadowInfo();
                }
            }
            if (surface < 0 || surface > 31 || (TerSurface[surface].flags & TERSURFACE_REFLECTIVE) == 0) {
                ground_height = 2000000.0f;
            }
            if (ground_height == 2000000.0f && platform != -1) {
                ground_height = FindReflectionNoPlatforms(&world_matrix.translation);
            }
            if (ground_height != 2000000.0f) {
                NUMTX reflection_matrix;
                reflection_height = ground_height;
                if (MatrixReflection(&world_matrix, 2, ground_height, WORLD->current_level->reflection_y, &reflection_matrix) != 0) {
                    NuRndrStartReflectionRender(0);
                    NuHGobjRndrMtxDwa(hierarchy, &reflection_matrix, render_count, render_indices, joint_matrices, dwa,
                                      character->flags & 8);
                    if (attachment_matrix != NULL) {
                        NUMTX attachment_reflection;
                        if (MatrixReflection(attachment_matrix, 2, ground_height, WORLD->current_level->reflection_y,
                                             &attachment_reflection) != 0) {
                            CharScene_Draw(world, character_id, NULL, &attachment_reflection);
                        }
                    }
                    NuRndrEndReflectionRender();
                }
            }
        }

        if (CutSceneSys->object_id != -1 && CutSceneSys->cheat_id != -1 && Cheat_IsOn(CutSceneSys->cheat_id) != 0) {
            dco_id = character_id;
            dco_gcdata = game_data;
            dco_cmodel = model;
            dco_wearinghat = 0;
            dco_reflectaxis = reflection_height != 2000000.0f ? 2 : 0;
            dco_reflectcoord = reflection_height;
            DrawObjectOnCharacter(NULL, NULL, CutSceneSys->object_id, NULL, game_data->head_locator, -1, locator_matrices,
                                  reflection_height != 2000000.0f, static_cast<u32>(layer_mask), NULL, NULL, 1.0f, 1.0f);
        }

        if (character->locator_index != 0xff) {
            CUT_LOAD_LOCATOR_SYSTEM *locator_system = cutscene->locator_system;
            CUT_LOAD_LOCATOR_INSTANCE_SYSTEM *locator_instance = cutscene_instance->locator_instance;
            for (i32 i = 0; i < character->locator_count; ++i) {
                i32 locator_index = character->locator_index + i;
                if (locator_system->locators[locator_index].joint_index == 0xff) {
                    instNuGCutLocatorUpdate(cutscene_instance, locator_system, &locator_instance->locators[locator_index],
                                            &locator_system->locators[locator_index], frame, &world_matrix, paused);
                } else {
                    NuMtxMulVU0(&locator_parent, &joint_matrices[locator_system->locators[locator_index].joint_index], &world_matrix);
                    instNuGCutLocatorUpdate(cutscene_instance, locator_system, &locator_instance->locators[locator_index],
                                            &locator_system->locators[locator_index], frame, &locator_parent, paused);
                }
            }
        }
    }

    static CUT_LOCATOR_FUNCTION_ENTRY cutscene_locatorfns[] = {{const_cast<char *>("blaster"), 0, NULL, 0, LocatorFunction_Blaster},
                                                           {NULL, 0, NULL, 0, NULL}};

    void CutScenes_InitSystem(CUT_DRAW_SETTINGS *system) {
        CutSceneSys = system;
        NuGCutSceneSysInit(cutscene_locatorfns);
        NuSetCutSceneCharacterRenderFn(reinterpret_cast<CUT_DRAW_RENDER_CALLBACK>(CutScene_DrawCharacter));
        NuSetCutSceneFindCharactersFn(CutScene_FindCharacters);
        NuSetCutSceneCharacterCreateDataFn(CutScene_CreateCharacterInstance);
        NuSetCutSceneResetCharactersFn(CutScene_ResetCharacters);
        NuSetCutSceneCharacterEvalFn(reinterpret_cast<CUT_DRAW_EVAL_CALLBACK>(CutScene_EvalCharacter));
        NuSetCutSceneRigidPostRenderFn(reinterpret_cast<CUT_DRAW_RIGID_CALLBACK>(CutScene_RigidPostRender));
    }
}

extern "C" void CutScene_DrawSubtitles(void) {
    CUTINFO *cut = CutStopInfo;
    if (cut == NULL) {
        return;
    }
    CUT_LOAD_SUBTITLE *subtitle = cut->subtitle_data;
    if (subtitle == NULL) {
        return;
    }
    for (i32 i = 0; i < cut->subtitle_count; ++i, ++subtitle) {
        if (cut->current_frame >= subtitle->start_frame && cut->current_frame <= subtitle->end_frame) {
            f32 alpha = 1.0f;
            if (subtitle->fade_time > 0.0f) {
                f32 fade_end = subtitle->start_frame + subtitle->fade_time;
                if (cut->current_frame < fade_end) {
                    alpha = (cut->current_frame - subtitle->start_frame) / (fade_end - subtitle->start_frame);
                } else {
                    f32 fade_start = subtitle->end_frame - subtitle->fade_time;
                    if (cut->current_frame > fade_start) {
                        alpha = 1.0f - (cut->current_frame - fade_start) / (subtitle->end_frame - fade_start);
                    }
                }
            }
            SmartTextEx(TTab[subtitle->text_id], subtitle->x, subtitle->y, 1.0f, subtitle->x_scale, subtitle->y_scale, 1.0f,
                        subtitle->alignment, subtitle->red, subtitle->green, subtitle->blue, subtitle->max_width, 1, NULL, 0,
                        (i32)((f32)subtitle->alpha * alpha));
            cut = CutStopInfo;
        }
    }
}

extern "C" {

    void CutScenePlayer_Configure(char *file, VARIPTR *buffer, VARIPTR *end, i16 *clip_text, i16 *intro, i16 *midtro, i16 *outro,
                                  i16 *ending) {
        NUFPAR *fp;
        CUT_PLAYER_CONFIG player;
        CUT_PLAYER_CLIP_CONFIG *clip;
        i32 level;

        fp = NuFParCreate(file);
        if (fp == NULL) {
            CutSceneConfigMissing(file);
            return;
        }
        buffer->addr = (buffer->addr + 3) & ~3U;
        player.clips = (CUT_PLAYER_CLIP_CONFIG *)buffer->void_ptr;
        player.active = NULL;
        player.clip_count = 0;
        player.clip_text = clip_text;
        player.intro_text = intro;
        player.midtro_text = midtro;
        player.outro_text = outro;
        player.ending_text = ending;
        while (NuFParGetLine(fp)) {
            if (!NuFParGetWord(fp) || NuStrICmp(fp->word_buffer_ptr, "clip") != 0) {
                continue;
            }
            clip = &player.clips[player.clip_count];
            clip->level_id = -1;
            clip->type = 0;
            clip->name[0] = 0;
            while (NuFParGetWord(fp)) {
                if (NuStrICmp(fp->word_buffer_ptr, "in_level") == 0) {
                    if (NuFParGetWord(fp)) {
                        if (Level_FindByName(fp->word_buffer_ptr, &level) != NULL) {
                            clip->level_id = (i16)level;
                        } else {
                            CutSceneConfigMissing(fp->word_buffer_ptr);
                        }
                    }
                } else if (NuStrICmp(fp->word_buffer_ptr, "name") == 0) {
                    if (NuFParGetWord(fp) && NuStrLen(fp->word_buffer_ptr) < 64) {
                        NuStrCpy(clip->name, fp->word_buffer_ptr);
                    }
                } else if (NuStrICmp(fp->word_buffer_ptr, "intro") == 0) {
                    clip->type = 0;
                } else if (NuStrICmp(fp->word_buffer_ptr, "midtro") == 0) {
                    clip->type = 1;
                } else if (NuStrICmp(fp->word_buffer_ptr, "outro") == 0) {
                    clip->type = 2;
                } else if (NuStrICmp(fp->word_buffer_ptr, "ending") == 0) {
                    clip->type = 3;
                }
            }
            if (clip->level_id != -1 && (CutScenePlayer_AcceptFn == NULL || CutScenePlayer_AcceptFn(clip) != 0)) {
                ++player.clip_count;
            }
        }
        NuFParDestroy(fp);
        if (player.clip_count > 0) {
            buffer->void_ptr = player.clips + player.clip_count;
            PCByteCopy(buffer->void_ptr, &player, sizeof(player));
            CutScenePlayer = (CUT_PLAYER_ACTIVE *)buffer->void_ptr;
            buffer->addr += sizeof(player);
        } else {
            CutSceneConfigMissing(file);
        }
    }

    void CutScenePlayer_Reset(void) {
        if (CutScenePlayer != NULL) {
            CutScenePlayer->active = NULL;
        }
    }

    CUT_PLAYER_ACTIVE *CutScenePlayer_Available(void) {
        return CutScenePlayer;
    }

    void *CutScenePlayer_Active(void) {
        return CutScenePlayer != NULL ? CutScenePlayer->active : NULL;
    }

    i32 CutScenePlayer_CanStart(i32 clip_id) {
        CUT_PLAYER_CLIP_CONFIG *clips;
        i32 area_id;
        i32 episode;
        if (CutScenePlayer != NULL && clip_id >= 0 && clip_id < CutScenePlayer->clip_count) {
            clips = CutScenePlayer->clips;
            area_id = LDataList[clips[clip_id].level_id].area_index;
            if (area_id != -1) {
                if ((ADataList[area_id].flags & AREAFLAG_ENDING_AREA) != 0) {
                    episode = ADataList[area_id].episode_index;
                    if (episode != -1 && Episode_IsComplete(&EDataList[episode], NULL) != 0) {
                        return 2;
                    }
                } else if (Game_AreaSave != NULL && Game_AreaSave[area_id].area_complete != 0) {
                    return 1;
                }
            }
        }
        return 0;
    }

    /* Accepts one argument and does not write return_door. */
    void CutScenePlayer_Start(i32 clip_id) {
        CUT_PLAYER_CLIP_CONFIG *clips;
        if (CutScenePlayer != NULL && clip_id >= 0 && clip_id < CutScenePlayer->clip_count) {
            clips = CutScenePlayer->clips;
            NewLevelFromMenu(&LDataList[clips[clip_id].level_id], -1, -1, 1);
            CutScenePlayer->active = &CutScenePlayer->clips[clip_id];
            NextArea_FreePlay = 0;
            FreePlay = 0;
        }
    }

    /* Mode is only a boolean controlling the type suffix. */
    void CutScenePlayer_GetText(i32 id, char *title, char *subtitle, i32 mode) {
        CUT_PLAYER_CLIP_CONFIG *clips;
        LEVELDATA *level;
        i32 area_id;
        i32 episode;
        if (CutScenePlayer == NULL) {
            if (title != NULL) {
                NuStrCpy(title, "?");
            }
            if (subtitle != NULL) {
                NuStrCpy(subtitle, "?");
            }
            return;
        }
        if (title != NULL) {
            if (CutScenePlayer->clip_text != NULL) {
                sprintf(title, TTab[*CutScenePlayer->clip_text], id + 1);
            } else {
                sprintf(title, "Clip %i", id + 1);
            }
            if (mode != 0) {
                clips = CutScenePlayer->clips;
                switch (clips[id].type) {
                    case 0:
                        if (CutScenePlayer->intro_text != NULL) {
                            NuStrCat(title, " - ");
                            NuStrCat(title, TTab[*CutScenePlayer->intro_text]);
                        }
                        break;
                    case 1:
                        if (CutScenePlayer->midtro_text != NULL) {
                            NuStrCat(title, " - ");
                            NuStrCat(title, TTab[*CutScenePlayer->midtro_text]);
                        }
                        break;
                    case 2:
                        if (CutScenePlayer->outro_text != NULL) {
                            NuStrCat(title, " - ");
                            NuStrCat(title, TTab[*CutScenePlayer->outro_text]);
                        }
                        break;
                    case 3:
                        if (CutScenePlayer->ending_text != NULL) {
                            NuStrCat(title, " - ");
                            NuStrCat(title, TTab[*CutScenePlayer->ending_text]);
                        }
                        break;
                }
            }
        }
        if (subtitle != NULL) {
            clips = CutScenePlayer->clips;
            level = &LDataList[clips[id].level_id];
            area_id = level->area_index;
            if (area_id == -1) {
                NuStrCpy(subtitle, "?");
                return;
            }
            if ((ADataList[area_id].flags & AREAFLAG_ENDING_AREA) != 0) {
                episode = (i8)level->episode;
                if (episode == -1) {
                    NuStrCpy(subtitle, "?");
                    return;
                }
                NuStrCpy(subtitle, TTab[EDataList[episode].text_id]);
            } else {
                NuStrCpy(subtitle, TTab[ADataList[area_id].name_id]);
            }
        }
    }

    void CutScenePlayer_SetObjects(CUTINFO *owner) {
        // MATCH-EXCEPTION: owner->player_states at each use; a local copy lets VC8 pack the end frame into the dead
        // parameter slot.
        i32 i;
        if (CutScenePlayer == NULL || CutScenePlayer->active == NULL) {
            return;
        }
        if (owner->player_states == NULL) {
            return;
        }
        for (i = 0; i < owner->player_state_count; ++i) {
            u8 flags = owner->player_states[i].flags;
            if ((flags & 1) != 0) {
                NuSpecialSetVisibility(&owner->player_states[i].special, 1);
            } else if ((flags & 2) != 0) {
                NuSpecialSetVisibility(&owner->player_states[i].special, 0);
            } else if ((flags & 4) != 0) {
                f32 time = NuSpecialGetAnimEndFrame(&owner->player_states[i].special);
                CUT_PLAYER_INST_ANIM *animation = (CUT_PLAYER_INST_ANIM *)NuSpecialGetInstAnim(&owner->player_states[i].special);
                if (animation != NULL) {
                    animation->flags |= 1;
                    animation->time = time;
                }
            }
        }
    }

    static i32 cutaudiopaused = 0;
    i32 CUTCAMONLY = 0;
    i32 CUTCAM = 0;
    static i32 CutFrame = 0;
    i32 CutSceneWaiting = 0;
    i32 CUTSKIPLOCK = 0;
    i32 CutSceneMusicStartFrame = -1;

    static i32 CutScene_Start(WORLDINFO *world, CUTINFO *cut) {
        if (cut == NULL || cut->instance == NULL || CutScenePlayOncePrivate(world, cut) != 0) {
            return 0;
        }
        instNuGCutSceneReset(cut->instance);
        if (CutScene_StartFn != NULL) {
            CutScene_StartFn(cut);
        }
        if (CutScenePlayer != NULL && CutScenePlayer->active != NULL) {
            CutScenePlayer_SetObjects(cut);
        }
        f32 rate = cut->frames_per_second * FRAMETIME;
        cut->instance->rate = rate;
        instNuGCutSceneStart(cut->instance);
        if ((cut->flags & CUTINFO_LOOPING) != 0) {
            cut->instance->flags_88 |= NUGCUTSCENE_INST_LOOP;
        } else {
            cut->instance->flags_88 &= ~NUGCUTSCENE_INST_LOOP;
        }
        cut->current_frame = 0.0f;
        if (cut->music_handle != -1) {
            if (!music_man.SelectTrackByHandle(16, cut->music_handle)) {
                CutSceneConfigMissing(reinterpret_cast<char *>(cut->music_handle));
            }
            i32 status = music_man.PlayTrack(16, 1);
            CutSceneMusicStartFrame = nuapi.frame_count;
            if (status != 1) {
                // MATCH-EXCEPTION: the original tests status == 1 below on the eax this diagnostic stub returns (the
                // empty stub leaves the status there), so the status is read back from the call.
                status = reinterpret_cast<i32 (*)(char *)>(CutSceneConfigMissing)(reinterpret_cast<char *>(status));
            }
            cut->instance->rate = 0.0f;
            if ((cut->flags & 1) != 0 && status == 1) {
                CutSceneWaiting = 1;
                PauseGameAudio(0);
                cutaudiopaused = 1;
            } else {
                cutaudiopaused = 0;
            }
        } else {
            PauseGameAudio(0);
            SetLinkedCutSceneMusic(cut->instance, cut->linked_audio != 0 ? MUSIC_PLAYBACK_DUAL_STREAM_PENDING : MUSIC_PLAYBACK_DUAL_STREAM);
            rate = cut->frames_per_second * FRAMETIME;
            cut->instance->rate = rate;
            if ((cut->flags & 1) == 0) {
                return 1;
            }
            CutSceneWaiting = 0;
            cutaudiopaused = 0;
        }
        if ((cut->flags & 1) == 0) {
            return 1;
        }
        Panel_Clear();
        CutFrame = 0;
        for (i32 i = 0; i < world->cutscene_sys->count; ++i) {
            if ((world->cutscene_sys->records[i]->instance->flags_88 & NUGCUTSCENE_INST_PLAYING) != 0) {
                if (world->cutscene_sys->records[i] != cut && world->cutscene_sys->records[i]->instance != NULL &&
                    (world->cutscene_sys->records[i]->flags & 1) != 0) {
                    world->cutscene_sys->records[i]->instance->rate = 0.0f;
                    instNuGCutSceneStop(world->cutscene_sys->records[i]->instance);
                } else {
                    world->cutscene_sys->records[i]->instance->rate = 0.0f;
                    instNuGCutScenePause(world->cutscene_sys->records[i]->instance, 1);
                }
            }
        }
        SetStopCutFlags(cut);
        CutBorderScale = 1.0f;
        ACTIVECUTCOUNT = 1;
        cut_waiting_for_new_level = 0;
        GameFog_Reset();
        CutSceneResetTextureAnimationsPrivate(cut);
        return 1;
    }

    void CutScenes_Start(WORLDINFO *world) {
        for (i32 i = 0; i < NewCutInfoCount; ++i) {
            CutScene_Start(world, NewCutInfo[i]);
        }
        NewCutInfoCount = 0;
    }

    void CutScenes_Reset(WORLDINFO *world) {
        CutSceneRebindPlayerCharactersPrivate(world->cutscene_sys);
        CutBorderScale = 0.0f;
        CutStopInfo = NULL;
        CutSceneWaiting = 0;
        cutaudiopaused = 0;
        if (world->cutscene_sys != NULL && reset_restart != 0 &&
            ((world->current_level->flags & (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO)) != 0 || world->level_progress == NULL ||
             (world->level_progress->flags & 2) == 0)) {
            CUTINFO *cut = NULL;
            CUTSYS *system = world->cutscene_sys;
            if (CutScenePlayer != NULL) {
                CUT_PLAYER_CLIP_CONFIG *clip = CutScenePlayer->active;
                if (clip != NULL && system->count > 0) {
                    if (clip->name[0] != 0) {
                        cut = CutScene_Find(system, clip->name);
                    }
                    if (cut == NULL) {
                        system = world->cutscene_sys;
                        cut = system->records[0];
                    }
                }
            }
            if (cut == NULL) {
                for (i32 i = 0; cut == NULL && i < system->count;) {
                    CUTINFO *record = system->records[i];
                    if ((record->flags & CUTINFO_LEVEL_INTRO) != 0) {
                        cut = record;
                    } else {
                        ++i;
                    }
                }
            }
            if (cut != NULL && CutScene_Start(world, cut) != 0) {
                CutBorderScale = 1.0f;
            }
        }
        NewCutInfoCount = 0;
        cut_waiting_for_new_level = 0;
    }

    static INSTNUGCUTSCENE *CutInstEnd[4] = {0};
    static i32 CutInstEndStop = 0;

    void CutScenes_End(void) {
        if (CutScenePlayer != NULL && CutScenePlayer->active != NULL && NewLData != NULL && NewLData == HUB_LDATA) {
            PCFrameSetLoadingYieldEnabled(0);
            return;
        }
        if (CutInstEndCount <= 0) {
            return;
        }
        for (i32 i = 0; i < CutInstEndCount; ++i) {
            instNuGCutSceneEnd(CutInstEnd[i]);
            if (i == CutInstEndStop) {
                CUTSTOPGAME = 0;
                CutStopInfo = NULL;
                GameCam_Reset(GameCam);
                PCFrameSetLoadingYieldEnabled(0);
            }
        }
    }

    void CutScenes_Update(WORLDINFO *world, i32 paused) {
        i32 active_before[32];
        if (CutScenePlayer != NULL && CutScenePlayer->active != NULL && CutStopInfo != NULL) {
            CutScenePlayer_SetObjects(CutStopInfo);
        }
        CutInstEnd[0] = NULL;
        CutInstEndCount = 0;
        CutInstEndStop = -1;
        CUTSTOPGAME = 0;
        CUTCAMONLY = 0;
        CutStopInfo = NULL;
        CUTDRAWWORLD = 0;
        CUTCAM = 0;
        CUTNOFOG = 0;
        CUTSKIPLOCK = 0;
        PCFrameSetLoadingYieldEnabled(0);
        if (world->cutscene_sys == NULL) {
            return;
        }
        // MATCH-EXCEPTION: the search runs on stop_index itself and jumps past the not-found reset; the original
        // keeps the loop counter as the stop index and skips the DebrisSetRenderGroup(1) test on the found path.
        i32 stop_index;
        for (stop_index = 0; stop_index < world->cutscene_sys->count; ++stop_index) {
            CUTINFO *cut = world->cutscene_sys->records[stop_index];
            INSTNUGCUTSCENE *instance = cut->instance;
            if (instance != NULL && (instance->flags_88 & NUGCUTSCENE_INST_PLAYING) != 0) {
                if ((cut->flags & 1) != 0) {
                    SetStopCutFlags(cut);
                    CutFrame = static_cast<i32>(static_cast<u32>(CutFrame) + 1U);
                    goto stopped;
                }
                if ((cut->flags & CUTINFO_CAM_ONLY) != 0) {
                    CUTCAMONLY = 1;
                }
            }
        }
        stop_index = -1;
        DebrisSetRenderGroup(1);
    stopped:
        ACTIVECUTCOUNT = 0;
        for (i32 i = 0; i < world->cutscene_sys->count; ++i) {
            CUTINFO *cut = world->cutscene_sys->records[i];
            INSTNUGCUTSCENE *instance = cut->instance;
            if (instance == NULL) {
                continue;
            }
            if ((instance->flags_88 & NUGCUTSCENE_INST_PLAYING) != 0) {
                cut->previous_frame = instance->current_frame;
                active_before[i] = 1;
                f32 elapsed = cut->current_frame + FRAMETIME;
                cut->current_frame = elapsed;
                if (cutaudiopaused != 0 && elapsed >= 23.0f) {
                    cutaudiopaused = 0;
                }
                if (stop_index != -1 && i != stop_index) {
                    instance->rate = 0.0f;
                    instNuGCutScenePause(cut->instance, 1);
                    ++ACTIVECUTCOUNT;
                    continue;
                }
                bool wait = false;
                if (NOSOUND == 0 && NOMUSIC == 0 && i == stop_index && cut->music_handle != -1 && instance->rate == 0.0f &&
                    cutaudiopaused != 0 && music_man.GetStatus(16, NULL) != 4) {
                    cut->instance->rate = 0.0f;
                    CutSceneWaiting = 1;
                    wait = true;
                }
                if (!wait) {
                    if (CutSceneMusicStartFrame != -1) {
                        if (nuapi.frame_count < static_cast<i32>(static_cast<u32>(CutSceneMusicStartFrame) + 5U)) {
                            cut->instance->rate = 0.0f;
                            CutSceneWaiting = 1;
                            wait = true;
                        } else {
                            CutSceneMusicStartFrame = -1;
                            music_man.PlayTrack(16, 0);
                        }
                    }
                    if (!wait) {
                        f32 rate = cut->frames_per_second * FRAMETIME;
                        cut->instance->rate = rate;
                        if (i == stop_index && CutSceneWaiting != 0) {
                            CutSceneWaiting = 0;
                            cutaudiopaused = 0;
                        }
                    }
                }
                instNuGCutScenePause(cut->instance, 0);
                if (CutScene_PreUpdateFn != NULL) {
                    CutScene_PreUpdateFn(cut);
                }
                CutSceneUpdateTextureAnimationsPrivate(cut);
                ++ACTIVECUTCOUNT;
            } else {
                active_before[i] = 0;
            }
        }
        NuGCutSceneSysUpdate(paused, 0, 1.0f);
        if (CutScene_PostUpdateFn != NULL) {
            CutScene_PostUpdateFn();
        }
        if (paused == 0) {
            for (i32 i = 0; i < world->cutscene_sys->count; ++i) {
                CUTINFO *cut = world->cutscene_sys->records[i];
                if (cut->instance == NULL) {
                    continue;
                }
                if (active_before[i] != 0 && cut->instance->rate > 0.0f) {
                    for (i32 j = 0; j < 6; ++j) {
                        CUT_LOAD_SFX *sfx = &cut->sfx[j];
                        if (sfx->id != -1 && cut->previous_frame < sfx->frame && cut->instance->current_frame >= sfx->frame) {
                            PlaySfxById(sfx->id, (sfx->flags & 1) != 0 ? &sfx->position : NULL);
                        }
                    }
                }
                if ((stop_index == -1 || i != stop_index) && active_before[i] != 0 && instNuGCutSceneIsFinished(cut->instance) != 0) {
                    if (CutScene_StoppedFn != NULL) {
                        CutScene_StoppedFn(cut);
                    }
                    if (CutInstEndCount < 4) {
                        INSTNUGCUTSCENE *ended_instance = cut->instance;
                        CutInstEnd[CutInstEndCount] = ended_instance;
                        ended_instance->flags_88 |= NUGCUTSCENE_INST_PLAYING;
                        ++CutInstEndCount;
                    }
                }
            }
        }
        if (stop_index == -1) {
            return;
        }
        CUTINFO *stop_cut = world->cutscene_sys->records[stop_index];
        GameFog_Reset();
        if (instNuGCutSceneIsFinished(stop_cut->instance) == 0) {
            return;
        }
        if (NewLData == NULL || WORLD->current_level == NewLData || NewLData != HUB_LDATA) {
            FADETYPE type = {FADE_TYPE_STILL_WIPE};
            FadeSys.SetFade(type, 0);
        }
        if (CutScene_StoppedFn != NULL) {
            CutScene_StoppedFn(stop_cut);
        }
        CutSceneResetTextureAnimationsPrivate(stop_cut);
        i32 skip_level = stop_cut->skip_level;
        if (skip_level != -1) {
            NewLData = &LDataList[skip_level];
            if (HUB_LDATA != NULL && NewLData == HUB_LDATA) {
                if (CutScenePlayer != NULL && CutScenePlayer->active != NULL) {
                    NewLevelFromMenu(HUB_LDATA, -1, -1, 1);
                    RenderHubFromCutscenePlayer = 1;
                } else if (WORLD->area != NULL && (WORLD->area->flags & AREAFLAG_ENDING_AREA) != 0) {
                    NewLData = SuperStory != 0 ? STATUS_LDATA : CREDITS_LDATA;
                }
            } else if (CutScenePlayer != NULL && CutScenePlayer->active != NULL &&
                       (NewLData->flags & (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO)) == 0 && (stop_cut->end_flags & 2) == 0) {
                NewLevelFromMenu(HUB_LDATA, -1, -1, 1);
                RenderHubFromCutscenePlayer = 1;
            }
            if (waiting_for_level != -1) {
                cut_waiting_for_new_level = 1;
                PauseGameCut();
                music_man.PauseTrack(16);
                NuSound3Update();
                newlevel_resumecutaudio = 1;
            }
        } else {
            CUTINFO *next = NewCutScene(NULL, world->cutscene_sys, stop_cut->next_cutscene, 0);
            if (CutScenePlayer != NULL && CutScenePlayer->active != NULL && next == NULL) {
                NewLevelFromMenu(HUB_LDATA, -1, -1, 1);
                RenderHubFromCutscenePlayer = 1;
            } else if ((stop_cut->flags & CUTINFO_NEW_MODE) != 0) {
                NewMode = 1;
            } else if (stop_cut->door_name[0] != 0) {
                void *door = Door_FindByName(world, stop_cut->door_name);
                if (door != NULL) {
                    Door_GoThrough(world, static_cast<DOOR_s *>(door), 1);
                }
            } else if ((stop_cut->flags & CUTINFO_WIPE_OUT) != 0) {
                FADETYPE type = {FADE_TYPE_STILL_WIPE};
                FadeSys.SetFade(type, 0);
                NeedScreenGrab(1);
                FadeSys.SetStage(1);
                GameAudio_PlaySfx(0x2d, NULL, 0, 0);
            }
        }
        CUTNOFOG = 0;
        if (CutInstEndCount < 4) {
            INSTNUGCUTSCENE *ended_instance = stop_cut->instance;
            CutInstEnd[CutInstEndCount] = ended_instance;
            ended_instance->flags_88 |= NUGCUTSCENE_INST_PLAYING;
            CutInstEndStop = CutInstEndCount;
            ++CutInstEndCount;
        }
    }
}

extern "C" void CutScene_StartAudio(void) {
}

extern "C" {

    /* NuFabs: the SDK macro copies the value with the sign bit cleared into the destination. */
}
