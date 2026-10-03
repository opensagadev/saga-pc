#include "saga.h"

CUTINFO *__cdecl CutScene_Find(CUTSYS_PC *system, char *name) {
    i32 index;

    if (system == NULL || name == NULL)
        return NULL;
    for (index = 0; index < system->count; ++index) {
        if (NuStrICmp(system->records[index]->name, name) == 0)
            return system->records[index];
    }
    return NULL;
}

CUTINFO *__cdecl NewCutScene(CUTINFO *cutscene, CUTSYS_PC *system, char *name, i32 unused_4) {
    i32 count;

    if (cutscene == NULL) {
        if (system == NULL || name == NULL)
            return NULL;
        cutscene = CutScene_Find(system, name);
        if (cutscene == NULL)
            return NULL;
    }
    count = NewCutInfoCount;
    if (count >= 8)
        return NULL;
    NewCutInfo[count] = cutscene;
    NewCutInfoCount = count + 1;
    return cutscene;
}

typedef struct cut_record_cleanup_pc_s {
    void *scene;
    INSTNUGCUTSCENE *instance;
} CUT_RECORD_CLEANUP_PC;
STATIC_ASSERT(offsetof(CUT_RECORD_CLEANUP_PC, scene) == 0);
STATIC_ASSERT(offsetof(CUT_RECORD_CLEANUP_PC, instance) == 4);

void __cdecl CutScenes_Destroy(CUTSYS_PC *system) {
    i32 i;
    if (system == NULL) {
        return;
    }
    for (i = 0; i < system->count; ++i) {
        CUT_RECORD_CLEANUP_PC *record = (CUT_RECORD_CLEANUP_PC *)system->records[i];
        if (record->instance != NULL) {
            instNuGCutSceneDestroy(record->instance);
            ((CUT_RECORD_CLEANUP_PC *)system->records[i])->instance = NULL;
        }
    }
    for (i = 0; i < system->count; ++i) {
        CUT_RECORD_CLEANUP_PC *record = (CUT_RECORD_CLEANUP_PC *)system->records[i];
        if (record->scene != NULL) {
            NuGCutSceneDestroy(record->scene);
            ((CUT_RECORD_CLEANUP_PC *)system->records[i])->scene = NULL;
        }
    }
}

typedef struct CUT_LOAD_STATE_PC {
    NUSPECIALHANDLE special;
    u8 flags;
    u8 unknown_0d, unknown_0e, unknown_0f;
} CUT_LOAD_STATE_PC;
STATIC_ASSERT(sizeof(CUT_LOAD_STATE_PC) == 0x10);
typedef struct CUT_LOAD_SFX_PC {
    i16 id;
    u8 flags;
    u8 unknown_03;
    f32 frame;
    NUVEC position;
} CUT_LOAD_SFX_PC;
STATIC_ASSERT(sizeof(CUT_LOAD_SFX_PC) == 0x14);
typedef struct CUT_LOAD_TEXANIM_PC {
    f32 frame;
    i32 index;
} CUT_LOAD_TEXANIM_PC;
typedef struct CUT_LOAD_FADE_FOG_PC {
    f32 frame, near_distance, far_distance, value;
} CUT_LOAD_FADE_FOG_PC;
typedef struct CUT_LOAD_SUBTITLE_PC {
    i16 text_id;
    u8 red, green, blue, alpha, alignment, unknown_07;
    f32 start_frame, end_frame, x, y, x_scale, y_scale, max_width, fade_time;
} CUT_LOAD_SUBTITLE_PC;
STATIC_ASSERT(sizeof(CUT_LOAD_SUBTITLE_PC) == 0x28);
STATIC_ASSERT(offsetof(CUT_LOAD_SUBTITLE_PC, start_frame) == 8);
STATIC_ASSERT(offsetof(CUT_LOAD_SUBTITLE_PC, x) == 0x10);
STATIC_ASSERT(offsetof(CUT_LOAD_SUBTITLE_PC, fade_time) == 0x24);
typedef struct CUT_LOAD_RECORD_PC {
    void *scene;
    INSTNUGCUTSCENE *instance;
    char name[64];
    CUT_LOAD_STATE_PC *state_entries;
    u8 state_count;
    u8 unknown_4d[3];
    u32 flags;
    f32 previous_frame;
    f32 unknown_58;
    f32 frames_per_second;
    f32 burnout_threshold;
    f32 burnout_intensity;
    f32 burnout_flare;
    f32 camera_near_clip;
    CUT_LOAD_SFX_PC sfx[6];
    u16 camera_far_clip;
    i16 legacy_music_index;
    i16 skip_level;
    u8 linked_audio;
    u8 blob_shadow_alpha;
    u8 blob_shadow_fade_near;
    u8 blob_shadow_fade_far;
    u8 reflection_range;
    i8 debris_render_group;
    char door_name[16];
    char next_cutscene[64];
    CUT_LOAD_TEXANIM_PC texture_animations[4];
    CUT_LOAD_FADE_FOG_PC fade_fog[2];
    CUT_LOAD_SUBTITLE_PC *subtitle_data;
    u16 subtitle_count;
    u8 end_flags;
    u8 unknown_18b;
    i32 music_handle;
} CUT_LOAD_RECORD_PC;
STATIC_ASSERT(sizeof(CUT_LOAD_RECORD_PC) == 0x190);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, state_entries) == 0x48);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, state_count) == 0x4c);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, flags) == 0x50);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, frames_per_second) == 0x5c);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, sfx) == 0x70);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, camera_far_clip) == 0xe8);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, legacy_music_index) == 0xea);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, skip_level) == 0xec);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, linked_audio) == 0xee);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, blob_shadow_alpha) == 0xef);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, debris_render_group) == 0xf3);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, door_name) == 0xf4);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, next_cutscene) == 0x104);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, texture_animations) == 0x144);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, fade_fog) == 0x164);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, subtitle_data) == 0x184);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, subtitle_count) == 0x188);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, end_flags) == 0x18a);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, music_handle) == 0x18c);

extern "C" {
    static void CS_fpsec(NUFPAR_PC *fp) {
        CS_CutInfo->frames_per_second = NuFParGetFloat(fp);
    }

    static void CS_no_fog(NUFPAR_PC *) {
        CS_CutInfo->flags |= 4;
    }

    static void CS_draw_world(NUFPAR_PC *fp) {
        if (NuFParGetWord(fp) == 0) {
            return;
        }
        if (NuStrICmp(fp->word_buffer_ptr, "on") == 0) {
            CS_CutInfo->flags |= 2;
        } else if (NuStrICmp(fp->word_buffer_ptr, "off") == 0) {
            CS_CutInfo->flags &= ~2U;
        }
    }

    static void CS_in_game(NUFPAR_PC *) {
        CS_CutInfo->flags &= ~3U;
        CS_CutInfo->flags |= 0x800;
    }

    static void CS_blobshadow_alpha(NUFPAR_PC *fp) {
        i32 alpha = NuFParGetInt(fp);
        if (alpha < 0) {
            alpha = 0;
        } else if (alpha > 0xfe) {
            alpha = 0xfe;
        }
        CS_CutInfo->blob_shadow_alpha = static_cast<u8>(alpha);
    }

    static void CS_blobshadow_fadenear(NUFPAR_PC *fp) {
        CS_CutInfo->blob_shadow_fade_near = static_cast<u8>(NuFParGetInt(fp));
    }

    static void CS_blobshadow_fadefar(NUFPAR_PC *fp) {
        CS_CutInfo->blob_shadow_fade_far = static_cast<u8>(NuFParGetInt(fp));
    }

    static void CS_reflect_range(NUFPAR_PC *fp) {
        CS_CutInfo->reflection_range = static_cast<u8>(NuFParGetInt(fp));
    }

    static void CS_render_group(NUFPAR_PC *fp) {
        CS_CutInfo->debris_render_group = static_cast<i8>(NuFParGetInt(fp));
    }

    static void CS_deb_page(NUFPAR_PC *fp) {
        if (NuFParGetWord(fp) == 0) {
            return;
        }
        if (NuStrICmp(fp->word_buffer_ptr, "level") == 0) {
            CS_CutInfo->flags |= 8;
        } else {
            CS_CutInfo->flags &= ~8U;
        }
    }

    static void CS_snap_out(NUFPAR_PC *) {
        CS_CutInfo->flags |= 0x10;
    }

    static void CS_wipe_out(NUFPAR_PC *) {
        CS_CutInfo->flags |= 0x100;
    }

    static void CS_new_mode(NUFPAR_PC *) {
        CS_CutInfo->flags |= 0x400;
    }

    static void CS_cam_only(NUFPAR_PC *) {
        CS_CutInfo->flags |= 0x20;
    }

    static void CS_replace_players(NUFPAR_PC *) {
        CS_CutInfo->flags |= 0x40;
    }

    static void CS_burnout_threshold(NUFPAR_PC *fp) {
        CS_CutInfo->burnout_threshold = NuFParGetFloat(fp);
        CS_CutInfo->flags |= 0x80;
    }

    static void CS_burnout_intensity(NUFPAR_PC *fp) {
        CS_CutInfo->burnout_intensity = NuFParGetFloat(fp);
        CS_CutInfo->flags |= 0x80;
    }

    static void CS_burnout_flare(NUFPAR_PC *fp) {
        CS_CutInfo->burnout_flare = NuFParGetFloat(fp);
        CS_CutInfo->flags |= 0x80;
    }

    static void CS_looping(NUFPAR_PC *) {
        CS_CutInfo->flags |= 0x200;
    }

    static void CS_start_cam(NUFPAR_PC *) {
        CS_CutInfo->flags |= 0x2000;
    }

    static void CS_super_widescreen(NUFPAR_PC *) {
        CS_CutInfo->flags |= 0x4000;
    }

    static void CS_go_through_door(NUFPAR_PC *fp) {
        if (NuFParGetWord(fp) != 0 && NuStrLen(fp->word_buffer_ptr) <= 0xf) {
            NuStrCpy(CS_CutInfo->door_name, fp->word_buffer_ptr);
        }
    }

    static void CS_sfx(NUFPAR_PC *fp) {
        if (NuFParGetWord(fp) != 0) {
            CS_CutInfo->legacy_music_index = GetMusicIndex(fp->word_buffer_ptr, MusicInfo, -1);
            CS_CutInfo->music_handle = music_man.GetTrackHandle(0x10, fp->word_buffer_ptr);
        }
    }

    static void CS_play_sfx(NUFPAR_PC *fp) {
        if (NuFParGetWord(fp) == 0) {
            return;
        }

        i32 sfx_id = GetSfxId(fp->word_buffer_ptr);
        if (sfx_id == -1) {
            return;
        }

        i32 slot;
        for (slot = 0; slot < 6 && CS_CutInfo->sfx[slot].id != -1; ++slot) {
        }
        if (slot == 6) {
            return;
        }

        CUT_LOAD_SFX_PC *sfx = &CS_CutInfo->sfx[slot];
        sfx->id = static_cast<i16>(sfx_id);
        sfx->flags &= ~1U;
        while (NuFParGetWord(fp) != 0) {
            if (NuStrICmp(fp->word_buffer_ptr, "frame") == 0) {
                sfx->frame = NuFParGetFloat(fp);
                if (sfx->frame < 1.0f) {
                    sfx->frame = 1.0f;
                }
            } else if (NuStrICmp(fp->word_buffer_ptr, "pos") == 0) {
                f32 value = NuFParGetFloat(fp);
                if (value == 0.0f) {
                    continue;
                }
                sfx->position.x = NuAToF(fp->word_buffer_ptr);
                value = NuFParGetFloat(fp);
                if (value == 0.0f) {
                    continue;
                }
                sfx->position.y = NuAToF(fp->word_buffer_ptr);
                value = NuFParGetFloat(fp);
                if (value == 0.0f) {
                    continue;
                }
                sfx->position.z = NuAToF(fp->word_buffer_ptr);
                sfx->flags |= 1;
            }
        }
    }

    static void CS_goto_level(NUFPAR_PC *fp) {
        if (NuFParGetWord(fp) == 0) {
            return;
        }
        i32 level_index;
        LEVELDATA_PC *level = Level_FindByName(fp->word_buffer_ptr, &level_index);
        if (level_index != -1 && &LDataList[level_index] == HUB_LDATA && CS_area != -1) {
            i32 status_index;
            Area_FindStatusLevel(&ADataList[CS_area], &status_index);
            if (status_index != -1) {
                level_index = status_index;
            }
        }
        CS_CutInfo->skip_level = static_cast<i16>(level_index);
    }

    static void CS_next_cut_scene(NUFPAR_PC *fp) {
        if (NuFParGetWord(fp) != 0 && NuStrLen(fp->word_buffer_ptr) <= 0x3f) {
            NuStrCpy(CS_CutInfo->next_cutscene, fp->word_buffer_ptr);
            CS_CutInfo->linked_audio = 1;
        }
    }

    static void CS_hold_audio(NUFPAR_PC *) {
        CS_CutInfo->linked_audio = 1;
    }

    static void CS_level_intro(NUFPAR_PC *) {
        CS_CutInfo->flags |= 0x1000;
    }

    static void CS_tex_anim(NUFPAR_PC *fp) {
        if (CS_texanimcount > 3) {
            return;
        }
        CUT_LOAD_TEXANIM_PC *animation = &CS_CutInfo->texture_animations[CS_texanimcount];
        animation->index = NuFParGetInt(fp);
        if (animation->index == -1) {
            return;
        }
        animation->frame = static_cast<f32>(NuFParGetInt(fp));
        if (animation->frame >= 1.0f) {
            ++CS_texanimcount;
        }
    }

    static void CS_fadefog(NUFPAR_PC *fp) {
        if (CS_fadefogcount > 1) {
            return;
        }
        CUT_LOAD_FADE_FOG_PC *fade = &CS_CutInfo->fade_fog[CS_fadefogcount];
        fade->frame = NuFParGetFloat(fp);
        if (!(fade->frame >= 0.0f)) {
            return;
        }
        fade->near_distance = NuFParGetFloat(fp);
        if (!(fade->near_distance >= 0.0f)) {
            return;
        }
        fade->far_distance = NuFParGetFloat(fp);
        if (!(fade->far_distance >= 0.0f)) {
            return;
        }
        fade->value = NuFParGetFloat(fp);
        if (fade->value >= 0.0f) {
            ++CS_fadefogcount;
        }
    }

    static void CS_subtitle(NUFPAR_PC *fp) {
        if (CS_CutInfo->subtitle_data == NULL) {
            CS_CutInfo->subtitle_data = reinterpret_cast<CUT_LOAD_SUBTITLE_PC *>(CS_buffptr->addr);
        }

        CUT_LOAD_SUBTITLE_PC *subtitle = &CS_CutInfo->subtitle_data[CS_CutInfo->subtitle_count];
        subtitle->text_id = -1;
        subtitle->start_frame = 0.0f;
        subtitle->end_frame = 0.0f;
        subtitle->red = CUTSUBTITLEDEFAULT_R;
        subtitle->green = CUTSUBTITLEDEFAULT_G;
        subtitle->blue = CUTSUBTITLEDEFAULT_B;
        subtitle->alpha = CUTSUBTITLEDEFAULT_A;
        subtitle->alignment = CUTSUBTITLEDEFAULT_ALIGN;
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
                    subtitle->alignment = 1;
                } else if (NuStrICmp(fp->word_buffer_ptr, "top_right") == 0) {
                    subtitle->alignment = 9;
                } else if (NuStrICmp(fp->word_buffer_ptr, "right") == 0) {
                    subtitle->alignment = 8;
                } else if (NuStrICmp(fp->word_buffer_ptr, "bottom_right") == 0) {
                    subtitle->alignment = 12;
                } else if (NuStrICmp(fp->word_buffer_ptr, "bottom") == 0) {
                    subtitle->alignment = 4;
                } else if (NuStrICmp(fp->word_buffer_ptr, "bottom_left") == 0) {
                    subtitle->alignment = 6;
                } else if (NuStrICmp(fp->word_buffer_ptr, "left") == 0) {
                    subtitle->alignment = 2;
                } else if (NuStrICmp(fp->word_buffer_ptr, "top_left") == 0) {
                    subtitle->alignment = 3;
                }
            } else if (NuStrICmp(fp->word_buffer_ptr, "start_time") == 0) {
                subtitle->start_frame = static_cast<f32>(fabs(static_cast<f64>(NuFParGetFloat(fp))));
            } else if (NuStrICmp(fp->word_buffer_ptr, "end_time") == 0) {
                subtitle->end_frame = static_cast<f32>(fabs(static_cast<f64>(NuFParGetFloat(fp))));
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
                subtitle->max_width = static_cast<f32>(fabs(static_cast<f64>(NuFParGetFloat(fp))));
            } else if (NuStrICmp(fp->word_buffer_ptr, "fade_time") == 0) {
                subtitle->fade_time = static_cast<f32>(fabs(static_cast<f64>(NuFParGetFloat(fp))));
            }
        }

        i16 text_id = subtitle->text_id;
        if (text_id >= 0 && text_id < Text_GetMaxOverallStrings() && TTab[text_id] != NULL &&
            subtitle->start_frame < subtitle->end_frame) {
            if (subtitle->fade_time > 0.0f &&
                subtitle->end_frame - subtitle->start_frame < subtitle->fade_time * 2.0f) {
                subtitle->fade_time = 0.0f;
            }
            CS_buffptr->addr += sizeof(CUT_LOAD_SUBTITLE_PC);
            ++CS_CutInfo->subtitle_count;
        }
    }

    static void CS_playonce(NUFPAR_PC *) {
        CS_CutInfo->end_flags |= 1;
    }

    static void CS_nextcutscene_inplayablelevel(NUFPAR_PC *) {
        CS_CutInfo->end_flags |= 2;
    }

    static void CS_cutsceneplayerobj(NUFPAR_PC *fp) {
        if (CS_CutInfo->state_count > 0x1f || NuFParGetWord(fp) == 0) {
            return;
        }

        CUT_LOAD_STATE_PC *object = &CS_CutInfo->state_entries[CS_CutInfo->state_count];
        if (NuSpecialFind(CS_worldinfo->current_gscn, &object->special, fp->word_buffer_ptr, 1) == 0) {
            return;
        }
        object->flags &= ~(1 | 2);
        while (NuFParGetWord(fp) != 0) {
            if (NuStrICmp(fp->word_buffer_ptr, "on") == 0) {
                object->flags = (object->flags | 1) & ~2;
            } else if (NuStrICmp(fp->word_buffer_ptr, "off") == 0) {
                object->flags = (object->flags & ~1) | 2;
            } else if (NuStrICmp(fp->word_buffer_ptr, "end_anim") == 0 ||
                       NuStrICmp(fp->word_buffer_ptr, "endanim") == 0 ||
                       NuStrICmp(fp->word_buffer_ptr, "anim_end") == 0 ||
                       NuStrICmp(fp->word_buffer_ptr, "animend") == 0) {
                object->flags = (object->flags & ~(1 | 2)) | 4;
            }
        }
        ++CS_CutInfo->state_count;
    }

    static void CS_nearclip(NUFPAR_PC *fp) {
        CS_CutInfo->camera_near_clip = NuFParGetFloat(fp);
    }

    static void CS_farclip(NUFPAR_PC *fp) {
        CS_CutInfo->camera_far_clip = static_cast<u16>(NuFParGetInt(fp));
    }

    static void CS_draw_gizmo_sys(NUFPAR_PC *fp) {
        if (NuFParGetWord(fp) == 0) {
            return;
        }
        if (NuStrICmp(fp->word_buffer_ptr, "on") == 0) {
            CS_CutInfo->flags |= 0x8000;
        } else if (NuStrICmp(fp->word_buffer_ptr, "off") == 0) {
            CS_CutInfo->flags &= ~0x8000U;
        }
    }

    NUFPCOMJMP CutScene_ConfigKeywords[] = {
        {const_cast<char *>("fpsec"), CS_fpsec},
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
}

typedef struct CUT_LOAD_SYS_PC {
    CUT_LOAD_RECORD_PC **records;
    i32 count;
    u32 *character_bits;
} CUT_LOAD_SYS_PC;
STATIC_ASSERT(sizeof(CUT_LOAD_SYS_PC) == 12);
STATIC_ASSERT(offsetof(CUT_LOAD_SYS_PC, character_bits) == 8);
typedef struct CUT_LOAD_LIST_ENTRY_PC {
    char *directory;
    char *name;
    i16 level;
    u8 unknown_0a[2];
} CUT_LOAD_LIST_ENTRY_PC;
STATIC_ASSERT(sizeof(CUT_LOAD_LIST_ENTRY_PC) == 12);
STATIC_ASSERT(offsetof(CUT_LOAD_LIST_ENTRY_PC, level) == 8);
typedef struct CUT_LOAD_INSTANCE_RATE_PC {
    u8 unknown_00[0x98];
    f32 rate;
} CUT_LOAD_INSTANCE_RATE_PC;
STATIC_ASSERT(offsetof(CUT_LOAD_INSTANCE_RATE_PC, rate) == 0x98);
extern "C" {
    extern CUT_LOAD_LIST_ENTRY_PC *CutList;
    extern i32 CUTCOUNT;
    extern CUT_LOAD_SYS_PC *CS_cutsys;
    extern VARIPTR *CS_buffend;
    extern void(__cdecl *CutScene_OverrideConfigFileNameFn)(char *, i32, i32);
    void __cdecl CutSceneConfigMissingPc(char *);
    void *__cdecl NuGCutSceneLoad(char *, VARIPTR *, VARIPTR *, i32);
    void __cdecl NuGCutSceneFixUp(void *, NUGSCN *, void *, i32);
    void __cdecl NuGCutSceneFixUpExtra(void *, NUGSCN *);
    INSTNUGCUTSCENE *__cdecl instNuGCutSceneCreate(void *, NUGSCN *, void *, char *, VARIPTR *, i32);
}

extern "C" {
    static __declspec(noinline) void CutSceneConfigurePc(CUT_LOAD_RECORD_PC *cut, char *name, VARIPTR *buf,
                                                         VARIPTR *buf_end) {
        CUT_LOAD_STATE_PC states[32];
        CS_buffend = buf_end;
        CS_CutInfo = cut;
        CS_buffptr = buf;
        for (i32 i = 0; i < 6; ++i)
            cut->sfx[i].id = -1;
        cut->end_flags &= 0xfc;
        cut->frames_per_second = 30.0f;
        cut->legacy_music_index = -1;
        cut->music_handle = -1;
        cut->burnout_threshold = 1.0f;
        cut->debris_render_group = 2;
        cut->skip_level = -1;
        cut->burnout_intensity = 0.0f;
        cut->flags = 3;
        cut->burnout_flare = 0.0f;
        cut->camera_near_clip = 0.0f;
        cut->state_count = 0;
        cut->linked_audio = 0;
        cut->door_name[0] = 0;
        cut->next_cutscene[0] = 0;
        cut->camera_far_clip = 0;
        cut->state_entries = states;
        cut->blob_shadow_alpha = 0xff;
        cut->blob_shadow_fade_near = 0xff;
        cut->blob_shadow_fade_far = 0xff;
        cut->reflection_range = 0xff;
        for (i32 i = 0; i < 4; ++i)
            cut->texture_animations[i].index = -1;
        cut->fade_fog[0].value = 0.0f;
        cut->fade_fog[0].far_distance = 0.0f;
        cut->fade_fog[0].near_distance = 0.0f;
        cut->fade_fog[0].frame = 0.0f;
        cut->fade_fog[1].value = 0.0f;
        cut->fade_fog[1].far_distance = 0.0f;
        cut->fade_fog[1].near_distance = 0.0f;
        cut->fade_fog[1].frame = 0.0f;
        cut->subtitle_data = NULL;
        cut->subtitle_count = 0;
        cut->unknown_18b = 0;
        NUFPAR_PC *fp = NuFParCreate(name);
        if (fp == NULL) {
            CutSceneConfigMissingPc(name);
            return;
        }
        CS_texanimcount = 0;
        CS_fadefogcount = 0;
        NuFParPushCom(fp, CutScene_ConfigKeywords);
        while (NuFParGetLine(fp) != 0) {
            if (NuFParGetWord(fp) != 0)
                NuFParInterpretWord(fp);
        }
        NuFParDestroy(fp);
        CUT_LOAD_RECORD_PC *current = CS_CutInfo;
        if (current->blob_shadow_fade_near > current->blob_shadow_fade_far)
            current->blob_shadow_fade_near = current->blob_shadow_fade_far;
        if (current->subtitle_data != NULL && current->subtitle_count == 0)
            current->subtitle_data = NULL;
        if (current->state_count != 0) {
            buf->addr = (buf->addr + 3) & ~3U;
            current->state_entries = reinterpret_cast<CUT_LOAD_STATE_PC *>(buf->void_ptr);
            u32 bytes = current->state_count * sizeof(CUT_LOAD_STATE_PC);
            PCByteCopy_candidate(current->state_entries, states, bytes);
            buf->addr += bytes;
        } else
            current->state_entries = NULL;
    }

    static __declspec(noinline) CUT_LOAD_SYS_PC *CutScenesLoadListedPc(NUGSCN *gscn1, NUGSCN *gscn2, i32 fixup_mode,
                                                                       VARIPTR *buf, VARIPTR *buf_end, i32 area,
                                                                       i32 level, WORLDINFO *world) {
        CUT_LOAD_RECORD_PC *entries[32];
        char path[128], config_path[128];
        void *initial = buf->void_ptr;
        CUT_LOAD_SYS_PC *sys = reinterpret_cast<CUT_LOAD_SYS_PC *>((buf->addr + 3) & ~3U);
        CS_area = area;
        buf->addr = (reinterpret_cast<u32>(sys) + 15) & ~3U;
        sys->character_bits = reinterpret_cast<u32 *>(buf->void_ptr);
        buf->addr = (buf->addr + ((CHARCOUNT + 31) / 32) * 4 + 3) & ~3U;
        CS_cutsys = sys;
        CS_worldinfo = world;
        for (i32 i = 0; i < CUTCOUNT; ++i) {
            if (CutList[i].level != level)
                continue;
            CUT_LOAD_RECORD_PC *cut = reinterpret_cast<CUT_LOAD_RECORD_PC *>(buf->void_ptr);
            entries[sys->count] = cut;
            buf->addr = (buf->addr + sizeof(CUT_LOAD_RECORD_PC) + 3) & ~3U;
            NuStrCpy(path, "cut\\");
            NuStrCat(path, CutList[i].directory);
            NuStrCat(path, WorldPathSeparator);
            NuStrCat(path, CutList[i].name);
            NuStrCpy(config_path, path);
            NuStrCat(config_path, AreaResidentsExtension);
            CutSceneConfigurePc(cut, config_path, buf, buf_end);
            if (!(cut->flags & 0x800) && InStory() == 0 && !(LDataList[level].flags & 0xe0)) {
                buf->void_ptr = cut;
                continue;
            }
            NuStrCpy(config_path, path);
            NuStrCat(config_path, NuFileExt_cu2_PC);
            cut->scene = NuGCutSceneLoad(config_path, buf, buf_end, 0);
            if (cut->scene == NULL) {
                buf->void_ptr = cut;
                continue;
            }
            NuStrCpy(cut->name, CutList[i].name);
            NuGCutSceneFixUp(cut->scene, gscn1, 0, fixup_mode);
            NuGCutSceneFixUpExtra(cut->scene, gscn2);
            cut->instance = instNuGCutSceneCreate(cut->scene, NULL, NULL, CutList[i].name, buf, 0);
            if (cut->instance != NULL) {
                reinterpret_cast<CUT_LOAD_INSTANCE_RATE_PC *>(cut->instance)->rate =
                    cut->frames_per_second * DEFAULTFRAMETIME;
                buf->addr = (buf->addr + 15) & ~15U;
            }
            ++sys->count;
        }
        if (sys->count <= 0) {
            buf->void_ptr = initial;
            return NULL;
        }
        sys->records = reinterpret_cast<CUT_LOAD_RECORD_PC **>(buf->void_ptr);
        PCByteCopy_candidate(sys->records, entries, sys->count * 4);
        buf->addr = (buf->addr + sys->count * 4 * sys->count + 3) & ~3U;
        return sys;
    }

    extern "C" CUT_LOAD_SYS_PC *__cdecl CutScenes_Load(char *config, NUGSCN *gscn1, NUGSCN *gscn2, i32 param1,
                                                       VARIPTR *buf, VARIPTR *buf_end, i32 param2, i32 param3,
                                                       WORLDINFO *world) {
        NUFPAR_PC *fp;
        CUT_LOAD_SYS_PC *sys;
        void *initial;
        CUT_LOAD_RECORD_PC *cut;
        char name[128];
        char full_path[128];
        char config_name[128];
        CUT_LOAD_RECORD_PC *entries[32];

        if (CutList != NULL && CUTCOUNT > 0)
            return CutScenesLoadListedPc(gscn1, gscn2, param1, buf, buf_end, param2, param3, world);
        if (config == NULL) {
            return NULL;
        }

        fp = NuFParCreateMem("cutscenes", config, 0xffff);
        if (fp == NULL) {
            return NULL;
        }

        initial = buf->void_ptr;
        sys = (CUT_LOAD_SYS_PC *)((reinterpret_cast<u32>(initial) + 3) & ~3U);
        sys->character_bits = reinterpret_cast<u32 *>((reinterpret_cast<u32>(sys) + 15) & ~3U);
        CS_area = param2;
        CS_worldinfo = world;
        CS_cutsys = sys;
        buf->addr = (reinterpret_cast<u32>(sys->character_bits) + ((CHARCOUNT + 31) / 32) * 4 + 3) & ~3U;

        while (NuFParGetLine(fp) != 0) {
            if (NuFParGetWord(fp) == 0 || NuStrICmp(fp->word_buffer_ptr, "cutscene") != 0 || sys->count > 0x1f ||
                NuFParGetWord(fp) == 0) {
                continue;
            }

            cut = reinterpret_cast<CUT_LOAD_RECORD_PC *>(buf->void_ptr);
            entries[sys->count] = cut;
            buf->addr = (buf->addr + sizeof(CUT_LOAD_RECORD_PC) + 3) & ~3U;
            NuStrLwr(name, fp->word_buffer_ptr);
            i32 len = NuStrLen(name);
            while (len > 0 && name[len - 1] != '.') {
                --len;
            }
            if (len > 0) {
                name[len - 1] = '\0';
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
            NuStrCat(full_path, AreaResidentsExtension);
            CutSceneConfigurePc(cut, full_path, buf, buf_end);
            NuStrCpy(full_path, "cut\\");
            NuStrCat(full_path, name);
            NuStrCat(full_path, AreaResidentsExtension);

            if ((cut->flags & 0x800) == 0 && InStory() == 0 && (LDataList[param3].flags & 0xe0) == 0) {
                buf->void_ptr = cut;
                continue;
            }
            buf->void_ptr = (char *)((buf->addr + 63) & ~63U);
            NuStrCpy(full_path, "cut\\");
            NuStrCat(full_path, name);
            NuStrCat(full_path, NuFileExt_cu2_PC);
            cut->scene = NuGCutSceneLoad(full_path, buf, buf_end, 0);
            if (cut->scene == NULL) {
                NuStrCpy(full_path, "cut\\");
                NuStrCat(full_path, name);
                NuStrCat(full_path, ".cut");
                cut->scene = NuGCutSceneLoad(full_path, buf, buf_end, 0);
                if (cut->scene == NULL) {
                    buf->void_ptr = cut;
                    continue;
                }
            }
            char *base_name = name;
            for (char *cursor = name; *cursor != '\0'; ++cursor) {
                if (*cursor == '\\') {
                    base_name = cursor + 1;
                }
            }
            NuStrCpy(cut->name, base_name);
            NuGCutSceneFixUp(cut->scene, gscn1, 0, param1);
            NuGCutSceneFixUpExtra(cut->scene, gscn2);
            cut->instance = instNuGCutSceneCreate(cut->scene, NULL, NULL, name, buf, 0);
            if (cut->instance != NULL) {
                reinterpret_cast<CUT_LOAD_INSTANCE_RATE_PC *>(cut->instance)->rate =
                    cut->frames_per_second * DEFAULTFRAMETIME;
                buf->void_ptr = (char *)((buf->addr + 15) & ~15U);
            }
            sys->count++;
        }
        NuFParDestroy(fp);
        if (sys->count <= 0) {
            buf->void_ptr = initial;
            return NULL;
        }
        sys->records = reinterpret_cast<CUT_LOAD_RECORD_PC **>(buf->void_ptr);
        PCByteCopy_candidate(sys->records, entries, sys->count * sizeof(CUT_LOAD_RECORD_PC *));
        // Retail reserves count pointer-table rows after copying the count entries.
        buf->addr = ((buf->addr + sys->count * sizeof(CUT_LOAD_RECORD_PC *) * sys->count + 3) & ~3U);
        return sys;
    }
}
