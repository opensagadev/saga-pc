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

extern "C" i32 CutScene_PlayingOrRequested(CUTINFO *cut) {
    if (cut == NULL) {
        if (CutStopInfo != NULL || NewCutInfoCount != 0)
            return 1;
        return 0;
    }
    if (cut == CutStopInfo)
        return 1;
    for (i32 i = 0; i < NewCutInfoCount; ++i) {
        if (NewCutInfo[i] == cut)
            return 1;
    }
    return 0;
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

#include "saga.h"

typedef struct CUT_DRAW_LOCATOR_PC CUT_DRAW_LOCATOR_PC;
typedef struct CUT_DRAW_CHARACTER_PC {
    NUMTX base_matrix;
    char *name;
    CUT_ANIMATION_PC *animation;
    ANI3_HEADER_PC *face_animation;
    NUANIMDATA2_PC *extra_animation;
    void *character_model;
    union {
        i32 index;
        CUT_DRAW_LOCATOR_PC *pointer;
    } locator;
    f32 animation_rate;
    u8 flags;
    u8 animation_index;
    u8 locator_count;
    u8 unknown_5f;
    u8 locator_index;
    u8 blend_time;
    u8 unknown_62;
    i8 start_frame;
} CUT_DRAW_CHARACTER_PC;
STATIC_ASSERT(sizeof(CUT_DRAW_CHARACTER_PC) == 0x64);
STATIC_ASSERT(offsetof(CUT_DRAW_CHARACTER_PC, name) == 0x40);
STATIC_ASSERT(offsetof(CUT_DRAW_CHARACTER_PC, animation) == 0x44);
STATIC_ASSERT(offsetof(CUT_DRAW_CHARACTER_PC, face_animation) == 0x48);
STATIC_ASSERT(offsetof(CUT_DRAW_CHARACTER_PC, extra_animation) == 0x4c);
STATIC_ASSERT(offsetof(CUT_DRAW_CHARACTER_PC, flags) == 0x5c);
STATIC_ASSERT(offsetof(CUT_DRAW_CHARACTER_PC, locator_count) == 0x5e);
STATIC_ASSERT(offsetof(CUT_DRAW_CHARACTER_PC, locator_index) == 0x60);
typedef struct CUT_DRAW_CHAR_INSTANCE_PC {
    void *character_model;
    f32 blend_progress;
    u8 unknown_08[4];
    f32 animation_frame_a;
    f32 animation_frame_b;
    u8 flags;
    u8 current_animation;
    u8 animation_a;
    u8 animation_b;
} CUT_DRAW_CHAR_INSTANCE_PC;
STATIC_ASSERT(sizeof(CUT_DRAW_CHAR_INSTANCE_PC) == 0x18);
STATIC_ASSERT(offsetof(CUT_DRAW_CHAR_INSTANCE_PC, animation_frame_a) == 0xc);
STATIC_ASSERT(offsetof(CUT_DRAW_CHAR_INSTANCE_PC, flags) == 0x14);
STATIC_ASSERT(offsetof(CUT_DRAW_CHAR_INSTANCE_PC, animation_b) == 0x17);
typedef struct CUT_DRAW_LOCATOR_ANIMATION_PC {
    NUMTX matrix;
    NUVEC pivot;
    f32 scale;
    NUANIMDATA2_PC *animation;
} CUT_DRAW_LOCATOR_ANIMATION_PC;
STATIC_ASSERT(offsetof(CUT_DRAW_LOCATOR_ANIMATION_PC, pivot) == 0x40);
STATIC_ASSERT(offsetof(CUT_DRAW_LOCATOR_ANIMATION_PC, animation) == 0x50);
struct CUT_DRAW_LOCATOR_PC {
    union {
        CUT_LOCATOR_GENERIC_PC prefix;
        CUT_DRAW_LOCATOR_ANIMATION_PC animated;
    };
    u8 unknown_54[4];
    u8 flags;
    u8 type_index;
    u8 joint_index;
    u8 unknown_5b[9];
};
STATIC_ASSERT(sizeof(CUT_DRAW_LOCATOR_PC) == 0x64);
STATIC_ASSERT(offsetof(CUT_DRAW_LOCATOR_PC, joint_index) == 0x5a);
typedef struct CUT_DRAW_LOCATOR_SYS_PC {
    CUT_DRAW_LOCATOR_PC *locators;
    void *types;
    i32 count;
} CUT_DRAW_LOCATOR_SYS_PC;
typedef struct CUT_DRAW_LOCATOR_INSTANCE_PC {
    f32 frame;
    i32 effect_handle;
} CUT_DRAW_LOCATOR_INSTANCE_PC;
STATIC_ASSERT(sizeof(CUT_DRAW_LOCATOR_INSTANCE_PC) == 8);
typedef struct CUT_DRAW_LOCATOR_INSTANCE_SYS_PC {
    CUT_DRAW_LOCATOR_INSTANCE_PC *locators;
} CUT_DRAW_LOCATOR_INSTANCE_SYS_PC;
typedef struct CUT_DRAW_SCENE_PC {
    u8 unknown_00[8];
    f32 duration;
    u8 unknown_0c[0x10];
    CUT_DRAW_LOCATOR_SYS_PC *locator_system;
} CUT_DRAW_SCENE_PC;
STATIC_ASSERT(offsetof(CUT_DRAW_SCENE_PC, locator_system) == 0x1c);
typedef struct CUT_DRAW_INSTANCE_PC {
    u8 unknown_00[0x18];
    NUMTX matrix;
    u8 unknown_58[0x30];
    u32 flags;
    u8 unknown_8c[0x1c];
    CUT_DRAW_LOCATOR_INSTANCE_SYS_PC *locator_instance;
} CUT_DRAW_INSTANCE_PC;
STATIC_ASSERT(offsetof(CUT_DRAW_INSTANCE_PC, matrix) == 0x18);
STATIC_ASSERT(offsetof(CUT_DRAW_INSTANCE_PC, flags) == 0x88);
STATIC_ASSERT(offsetof(CUT_DRAW_INSTANCE_PC, locator_instance) == 0xa8);
STATIC_ASSERT(sizeof(CUT_DRAW_SETTINGS_PC) == 8);
typedef struct CUT_DRAW_OBJECT_PC {
    u8 unknown_00[0xb8];
    NUMTX matrix;
} CUT_DRAW_OBJECT_PC;
STATIC_ASSERT(offsetof(CUT_DRAW_OBJECT_PC, matrix) == 0xb8);
typedef struct CUT_DRAW_HIERARCHY_PC {
    u8 unknown_00[0x1a0];
    u8 suppress_shadow_surface_points;
} CUT_DRAW_HIERARCHY_PC;
STATIC_ASSERT(offsetof(CUT_DRAW_HIERARCHY_PC, suppress_shadow_surface_points) == 0x1a0);
typedef struct CUT_DRAW_CAMERA_PC {
    u8 unknown_00[0xb8];
    NUMTX render_matrix;
} CUT_DRAW_CAMERA_PC;
STATIC_ASSERT(offsetof(CUT_DRAW_CAMERA_PC, render_matrix) == 0xb8);
extern "C" {
    extern CUT_DRAW_SETTINGS_PC *CutSceneSys;
    extern void *CutStopInfo;
    extern i32 CutSceneWaiting;
    extern u8 CutBlobShadowAlpha;
    extern f32 CutBlobShadowFadeNear;
    extern f32 CutBlobShadowFadeFar;
    extern f32 CutReflectRange2;
    extern i32 dco_id;
    extern PLAYERCHARACTERCONFIG_PC *dco_gcdata;
    extern CHARACTER_MODEL_LAYERS_PC *dco_cmodel;
    extern i32 dco_wearinghat;
    extern i32 dco_reflectaxis;
    extern f32 dco_reflectcoord;
    f32 __cdecl AnimEndFrame(CHARACTER_MODEL_LAYERS_PC *, i32);
    void __cdecl EnableShadowMapRendering(i32);
    void __cdecl ResetShadowMapRendering(void);
    void __cdecl SetZeroLights(void);
    void __cdecl FindAndSetLights(NUVEC *, f32, void *);
    void __cdecl APITransparentCharDraw(NUHGOBJ_EVAL_PC *, NUMTX *, i32, i16 *, NUMTX *, DWA_WEIGHT_BLOCK_PC **, i32);
    i32 __cdecl NuHGobjRndrMtxDwa(NUHGOBJ_EVAL_PC *, NUMTX *, i32, i16 *, NUMTX *, DWA_WEIGHT_BLOCK_PC **, i32);
    void __cdecl CharScene_Draw(WORLDINFO *, i32, NUMTX *, NUMTX *);
    f32 __cdecl BlobShadowFade(NUVEC *, f32, f32, f32);
    void __cdecl NuRndrAddShadow(NUVEC *, f32, i32, i32, i32, i32);
    i32 __cdecl MatrixReflection(NUMTX *, i32, f32, f32, NUMTX *);
    void __cdecl NuRndrStartReflectionRender(i32);
    void __cdecl NuRndrEndReflectionRender(void);
    void __cdecl DrawObjectOnCharacter(WORLDINFO *, GAMEOBJECT *, i32, NUSPECIALHANDLE *, i32, i32, NUMTX *, i32, u32,
                                       NUMTX *, NUVEC *, f32, f32);
}

extern "C" {
    static void __cdecl CutScene_DrawCharacter(INSTNUGCUTSCENE *cutscene_instance, CUT_DRAW_SCENE_PC *cutscene,
                                               CUT_DRAW_CHAR_INSTANCE_PC *instance, CUT_DRAW_CHARACTER_PC *character,
                                               f32 frame, i32 paused) {
        WORLDINFO *world = WorldInfo_CurrentlyActive();
        CUTSYS_PC *cutscene_system = world->cutscene_sys;
        if (cutscene_system == NULL) {
            return;
        }

        CUTINFO *cut = NULL;
        for (i32 i = 0; i < cutscene_system->count; ++i) {
            CUTINFO *candidate = cutscene_system->records[i];
            if (candidate->instance == cutscene_instance) {
                cut = candidate;
                break;
            }
        }

        GAMEOBJECT *scene_object = NULL;
        CHARACTER_MODEL_LAYERS_PC *model = NULL;
        if (cut != NULL && CutScene_ReplaceCharacterModelFn != NULL) {
            i32 replacement_id =
                CutScene_ReplaceCharacterModelFn(cut, reinterpret_cast<CUTSCENECHARACTER *>(character));
            if (replacement_id != -1 && apicharsys->playermodelids[replacement_id] != -1) {
                model = &apicharsys->models[apicharsys->playermodelids[replacement_id]];
            }
        }
        if (NULL == model) {
            if ((character->flags & 2) == 0) {
                scene_object = static_cast<GAMEOBJECT *>(instance->character_model);
                model = scene_object->character_model;
            } else {
                model = static_cast<CHARACTER_MODEL_LAYERS_PC *>(instance->character_model);
            }
        }
        if (model->hierarchy == NULL) {
            return;
        }

        const i32 character_id = model->model_id;
        CHARACTERDATA_PC *character_data = reinterpret_cast<CHARACTERDATA_PC *>(&apicharsys->char_data[character_id]);
        PLAYERCHARACTERCONFIG_PC *game_data = static_cast<PLAYERCHARACTERCONFIG_PC *>(character_data->game_character);
        NUMTX world_matrix;
        i32 visible;
        u32 animation_index;
        f32 animation_rate;
        f32 blend_time;
        f32 animation_start_frame;
        i32 layer_mask = -1;
        NuGCutCharAnimProcess(reinterpret_cast<CUT_CHARACTER_GENERIC_PC *>(character), frame, &world_matrix, &visible,
                              &animation_index, &animation_rate, &blend_time, &animation_start_frame, &layer_mask);
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
        if (static_cast<i8>(reinterpret_cast<CUT_DRAW_INSTANCE_PC *>(cutscene_instance)->flags) < 0) {
            NuMtxMul(&world_matrix, &world_matrix,
                     &reinterpret_cast<CUT_DRAW_INSTANCE_PC *>(cutscene_instance)->matrix);
        }
        if (scene_object != NULL) {
            reinterpret_cast<CUT_DRAW_OBJECT_PC *>(scene_object)->matrix = world_matrix;
            scene_object->position = *reinterpret_cast<NUVEC *>(&world_matrix.m30);
        }
        if (0 == visible) {
            return;
        }

        i16 render_indices[32];
        const i32 render_count = game_data->make_layer_list(model, render_indices, static_cast<u32>(layer_mask));
        if (render_count < 1) {
            return;
        }

        if (instance->current_animation != animation_index) {
            const u8 requested_animation = static_cast<u8>(animation_index);
            if (!(blend_time > 0.0f) || instance->current_animation == 0xff) {
                instance->animation_a = requested_animation;
                instance->animation_frame_a = !(animation_start_frame > 1.0f) ? 1.0f : animation_start_frame;
            } else {
                if ((instance->flags & 1) != 0) {
                    instance->animation_a = instance->animation_b;
                    instance->animation_frame_a = instance->animation_frame_b;
                    animation_index = instance->animation_b;
                } else {
                    if (instance->current_animation != 0) {
                        instance->animation_a = instance->current_animation;
                        animation_index = instance->current_animation;
                    } else {
                        instance->animation_frame_a = frame;
                        instance->animation_a = requested_animation;
                    }
                }
                if (!(1.0f < animation_start_frame)) {
                    animation_start_frame = 1.0f;
                }
                instance->animation_b = requested_animation;
                instance->animation_frame_b = animation_start_frame;
                if (requested_animation != static_cast<u8>(animation_index)) {
                    instance->blend_progress = 0;
                    instance->flags |= 1;
                }
            }
            instance->current_animation = requested_animation;
        }

        bool blending = (instance->flags & 1) != 0;
        u32 animation_a_index = instance->animation_a;
        if (blending) {
            if (!(blend_time <= 0.0f)) {
                instance->blend_progress += (1.0f / blend_time) * (FRAMETIME * 60.0f);
                if (instance->blend_progress >= 1.0f) {
                    instance->blend_progress = 0;
                    animation_a_index = instance->animation_b;
                    instance->flags ^= 1;
                    instance->animation_a = instance->animation_b;
                    instance->animation_frame_a = instance->animation_frame_b;
                    blending = false;
                }
            } else {
                instance->blend_progress = 0;
                animation_a_index = instance->animation_b;
                instance->flags ^= 1;
                instance->animation_a = instance->animation_b;
                instance->animation_frame_a = instance->animation_frame_b;
                blending = false;
            }
        }

        ANI3_HEADER_PC *animation_a;
        NUANIMDATA2_PC *dwa_animation_a;
        if (animation_a_index == 0) {
            instance->animation_frame_a = frame;
            animation_a = reinterpret_cast<ANI3_HEADER_PC *>(character->face_animation);
            dwa_animation_a = character->extra_animation;
        } else {
            animation_a = static_cast<ANI3_HEADER_PC *>(model->model_data_b[animation_a_index - 1]);
            dwa_animation_a = static_cast<NUANIMDATA2_PC *>(model->model_data_c[animation_a_index - 1]);
        }

        NUMTX joint_matrices[256];
        DWA_WEIGHT_BLOCK_PC **dwa = NULL;
        if (!blending) {
            if (animation_a == NULL) {
                NuHGobjEval(model->hierarchy, 0, NULL, joint_matrices);
            } else {
                NuHGobjEvalAnim2(model->hierarchy, animation_a, instance->animation_frame_a, 0, NULL, joint_matrices);
            }
            if (dwa_animation_a != NULL) {
                dwa = NuHGobjEvalDwa2(render_count, render_indices, dwa_animation_a, instance->animation_frame_a);
            }
        } else {
            ANI3_HEADER_PC *animation_b;
            NUANIMDATA2_PC *dwa_animation_b;
            if (instance->animation_b == 0) {
                instance->animation_frame_b = frame;
                animation_b = reinterpret_cast<ANI3_HEADER_PC *>(character->face_animation);
                dwa_animation_b = character->extra_animation;
            } else {
                animation_b = static_cast<ANI3_HEADER_PC *>(model->model_data_b[instance->animation_b - 1]);
                dwa_animation_b = static_cast<NUANIMDATA2_PC *>(model->model_data_c[instance->animation_b - 1]);
            }
            if (animation_a != NULL && animation_b != NULL) {
                NuHGobjEvalAnimBlend2(model->hierarchy, animation_a, instance->animation_frame_a, animation_b,
                                      instance->animation_frame_b, instance->blend_progress, 0, NULL, joint_matrices);
            } else if (animation_b != NULL) {
                NuHGobjEvalAnim2(model->hierarchy, animation_b, instance->animation_frame_b, 0, NULL, joint_matrices);
            } else if (animation_a != NULL) {
                NuHGobjEvalAnim2(model->hierarchy, animation_a, instance->animation_frame_a, 0, NULL, joint_matrices);
            } else {
                NuHGobjEval(model->hierarchy, 0, NULL, joint_matrices);
            }

            if (dwa_animation_a != NULL && dwa_animation_b != NULL) {
                dwa = NuHGobjEvalDwaBlend2(render_count, render_indices, dwa_animation_a, instance->animation_frame_a,
                                           dwa_animation_b, instance->animation_frame_b, instance->blend_progress);
            }

            if (animation_b != NULL && instance->animation_b != 0xff && instance->animation_b != 0 &&
                (cut == NULL || cut != CutStopInfo || CutSceneWaiting == 0)) {
                const f32 end_frame = AnimEndFrame(model, static_cast<u8>(instance->animation_b) - 1);
                instance->animation_frame_b += FRAMETIME * 60.0f * animation_rate;
                if (instance->animation_frame_b > end_frame) {
                    CHARACTERANIM_CONFIG_PC *animation_info = static_cast<CHARACTERANIM_CONFIG_PC *>(
                        model->animation_configs[static_cast<u8>(instance->animation_b) - 1]);
                    if ((animation_info->flags & 2) == 0) {
                        instance->animation_frame_b = end_frame;
                    } else {
                        instance->animation_frame_b = instance->animation_frame_b - end_frame + 1.0f;
                    }
                }
            }
        }

        NUVEC locator_positions[16];
        NUMTX locator_matrices[16];
        PCStoreModelLocatorCoordinates(model, &world_matrix, joint_matrices, locator_positions, locator_matrices);

        if (animation_a != NULL && animation_a_index != 0 && animation_a_index != 0xff &&
            (cut == NULL || cut != CutStopInfo || CutSceneWaiting == 0)) {
            const f32 end_frame = AnimEndFrame(model, animation_a_index - 1);
            instance->animation_frame_a += FRAMETIME * 60.0f * animation_rate;
            if (instance->animation_frame_a > end_frame) {
                CHARACTERANIM_CONFIG_PC *animation_info =
                    static_cast<CHARACTERANIM_CONFIG_PC *>(model->animation_configs[animation_a_index - 1]);
                if ((animation_info->flags & 2) == 0) {
                    instance->animation_frame_a = end_frame;
                } else {
                    instance->animation_frame_a = instance->animation_frame_a - end_frame + 1.0f;
                }
            }
        }

        EnableShadowMapRendering(0);
        if (Cheats_CheckFlags(1) != 0) {
            SetZeroLights();
        } else {
            FindAndSetLights(reinterpret_cast<NUVEC *>(&world_matrix.m30), 1.0f, world->rtl_set);
        }

        u8 render_character = 1;
        if ((character->flags & 0x20) == 0 && (character_data->model_flags & 0x10000) != 0) {
            render_character = static_cast<u8>(((character->flags >> 4) ^ 1) & 1);
        }
        reinterpret_cast<CUT_DRAW_HIERARCHY_PC *>(model->hierarchy)->suppress_shadow_surface_points = render_character;

        if ((game_data->flags_090 & 0x8000) != 0) {
            APITransparentCharDraw(model->hierarchy, &world_matrix, render_count, render_indices, joint_matrices, dwa,
                                   character->flags & 8);
        }
        NuHGobjRndrMtxDwa(model->hierarchy, &world_matrix, render_count, render_indices, joint_matrices, dwa,
                          character->flags & 8);

        NUMTX *attachment_matrix = NULL;
        if ((character_data->flags & 1) != 0) {
            attachment_matrix = &world_matrix;
            i32 locator = game_data->thingy_locator;
            if (locator != -1 && model->points_of_interest[locator] != NULL) {
                attachment_matrix = &locator_matrices[locator];
            }
            CharScene_Draw(world, character_id, attachment_matrix, NULL);
        }
        ResetShadowMapRendering();

        f32 ground_height = 2000000.0f;
        i32 platform = -1;
        i32 surface = -1;
        bool shadow_sampled = false;
        if ((character->flags & 0x10) != 0 && CutBlobShadowAlpha != 0 && (character_data->model_flags & 0x10000) == 0 &&
            PcWindowOptions.shadowmethod == 1 && PcWindowOptions.shadowsenabled != 0) {
            u8 shadow_alpha = CutBlobShadowAlpha;
            if (game_data->blob_shadow_alpha != 0xff) {
                shadow_alpha = game_data->blob_shadow_alpha;
            }
            if (shadow_alpha != 0) {
                f32 shadow_radius = game_data->field_08c;
                if (game_data->shadow_locators == 0) {
                    if (!(shadow_radius < 99.0f)) {
                        shadow_radius = character_data->model_scale * character_data->collision_radius * 2.0f;
                    }
                    if (shadow_radius > 0.0f) {
                        ground_height = GameShadow(NULL, reinterpret_cast<NUVEC *>(&world_matrix.m30), 5.0f, -1);
                        if (ground_height != 2000000.0f) {
                            platform = NewShadowOnPlatform();
                            surface = ShadowInfo();
                            if (ShadNorm.y > 0.0f) {
                                NUVEC shadow_position = *reinterpret_cast<NUVEC *>(&world_matrix.m30);
                                shadow_position.y = ground_height + 0.005f;
                                f32 alpha = BlobShadowFade(&shadow_position, CutBlobShadowFadeNear,
                                                           CutBlobShadowFadeFar, ShadNorm.y);
                                if (alpha > 0.0f) {
                                    u16 x_rotation;
                                    u16 z_rotation;
                                    FindAnglesZX(&ShadNorm, &x_rotation, &z_rotation);
                                    NuRndrAddShadow(
                                        &shadow_position, shadow_radius,
                                        static_cast<i32>(static_cast<f32>(shadow_alpha) * alpha * ShadNorm.y),
                                        x_rotation, 0, z_rotation);
                                }
                            }
                        }
                        shadow_sampled = true;
                    }
                } else {
                    if (!(shadow_radius < 99.0f)) {
                        shadow_radius = character_data->collision_radius * character_data->model_scale;
                    }
                    if (shadow_radius > 0.0f) {
                        f32 alpha = BlobShadowFade(reinterpret_cast<NUVEC *>(&world_matrix.m30), CutBlobShadowFadeNear,
                                                   CutBlobShadowFadeFar, 1.0f);
                        if (alpha > 0.0f) {
                            for (i32 i = 0; i < 16; ++i) {
                                if ((game_data->shadow_locators & (1 << i)) != 0 &&
                                    model->points_of_interest[i] != NULL) {
                                    NUVEC shadow_position = locator_positions[i];
                                    f32 locator_ground = GameShadow(NULL, &shadow_position, 5.0f, -1);
                                    if (locator_ground != 2000000.0f && 0.0f < ShadNorm.y) {
                                        shadow_position.y = locator_ground + 0.005f;
                                        u16 x_rotation;
                                        u16 z_rotation;
                                        FindAnglesZX(&ShadNorm, &x_rotation, &z_rotation);
                                        NuRndrAddShadow(
                                            &shadow_position, shadow_radius,
                                            static_cast<i32>(alpha * static_cast<f32>(shadow_alpha) * ShadNorm.y),
                                            x_rotation, 0, z_rotation);
                                    }
                                }
                            }
                        }
                    }
                    ground_height = 2000000.0f;
                    platform = -1;
                    shadow_sampled = false;
                }
            }
        }

        f32 reflection_height = 2000000.0f;
        if ((character->flags & 0x20) != 0 && (game_data->flags_090 & 0x8000) == 0 &&
            NuVecDistSqr(reinterpret_cast<NUVEC *>(&world_matrix.m30),
                         reinterpret_cast<NUVEC *>(&reinterpret_cast<CUT_DRAW_CAMERA_PC *>(GameCam)->render_matrix.m30),
                         NULL) < CutReflectRange2) {
            if (!shadow_sampled) {
                ground_height = GameShadow(NULL, reinterpret_cast<NUVEC *>(&world_matrix.m30), 5.0f, -1);
                if (ground_height != 2000000.0f) {
                    platform = NewShadowOnPlatform();
                    surface = ShadowInfo();
                }
            }
            if (surface >= 0 && surface <= 31 && (TerSurface[surface].flags & 2) != 0 && ground_height != 2000000.0f) {
                reflection_height = ground_height;
            } else if (platform != -1) {
                (void)FindReflectionNoPlatforms(reinterpret_cast<NUVEC *>(&world_matrix.m30));
            }
            if (reflection_height != 2000000.0f) {
                NUMTX reflection_matrix;
                if (MatrixReflection(&world_matrix, 2, reflection_height, WORLD->current_level->reflection_y,
                                     &reflection_matrix) != 0) {
                    NuRndrStartReflectionRender(0);
                    NuHGobjRndrMtxDwa(model->hierarchy, &reflection_matrix, render_count, render_indices,
                                      joint_matrices, dwa, character->flags & 8);
                    if (attachment_matrix != NULL) {
                        NUMTX attachment_reflection;
                        if (MatrixReflection(attachment_matrix, 2, reflection_height,
                                             WORLD->current_level->reflection_y, &attachment_reflection) != 0) {
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
            DrawObjectOnCharacter(NULL, NULL, CutSceneSys->object_id, NULL, game_data->head_locator, -1,
                                  locator_matrices, reflection_height != 2000000.0f, static_cast<u32>(layer_mask), NULL,
                                  NULL, 1.0f, 1.0f);
        }

        if (character->locator_index != 0xff) {
            CUT_DRAW_LOCATOR_SYS_PC *locator_system = cutscene->locator_system;
            for (i32 i = 0; i < character->locator_count; ++i) {
                i32 locator_index = character->locator_index + i;
                CUT_DRAW_LOCATOR_PC *locator = &locator_system->locators[locator_index];
                NUMTX *parent_matrix = &world_matrix;
                if (locator->joint_index != 0xff) {
                    NUMTX locator_parent;
                    NuMtxMulVU0(&locator_parent, &joint_matrices[locator->joint_index], &world_matrix);
                    instNuGCutLocatorUpdate(
                        cutscene_instance, reinterpret_cast<CUT_LOAD_LOCATOR_SYSTEM *>(locator_system),
                        reinterpret_cast<CUT_LOAD_LOCATOR_INSTANCE *>(
                            &reinterpret_cast<CUT_DRAW_INSTANCE_PC *>(cutscene_instance)
                                 ->locator_instance->locators[locator_index]),
                        reinterpret_cast<CUT_LOAD_LOCATOR *>(locator), frame, &locator_parent, paused);
                } else {
                    instNuGCutLocatorUpdate(
                        cutscene_instance, reinterpret_cast<CUT_LOAD_LOCATOR_SYSTEM *>(locator_system),
                        reinterpret_cast<CUT_LOAD_LOCATOR_INSTANCE *>(
                            &reinterpret_cast<CUT_DRAW_INSTANCE_PC *>(cutscene_instance)
                                 ->locator_instance->locators[locator_index]),
                        reinterpret_cast<CUT_LOAD_LOCATOR *>(locator), frame, parent_matrix, paused);
                }
            }
        }
    }
}

typedef struct CUT_DRAW_CHARACTER_SYSTEM_PC {
    CUT_DRAW_CHARACTER_PC *characters;
    u16 count;
} CUT_DRAW_CHARACTER_SYSTEM_PC;
typedef struct CUT_DRAW_CHAR_INSTANCE_SYSTEM_PC {
    CUT_DRAW_CHAR_INSTANCE_PC *characters;
} CUT_DRAW_CHAR_INSTANCE_SYSTEM_PC;
typedef struct CUT_DRAW_REGISTRATION_SCENE_PC {
    u8 unknown_00[0x18];
    CUT_DRAW_CHARACTER_SYSTEM_PC *characters;
    CUT_DRAW_LOCATOR_SYS_PC *locators;
} CUT_DRAW_REGISTRATION_SCENE_PC;
STATIC_ASSERT(offsetof(CUT_DRAW_REGISTRATION_SCENE_PC, characters) == 0x18);
typedef struct CUT_DRAW_REGISTRATION_INSTANCE_PC {
    u8 unknown_00[0x18];
    NUMTX matrix;
    CUT_DRAW_REGISTRATION_SCENE_PC *scene;
    u8 unknown_5c[0x2c];
    u32 flags;
    u8 unknown_8c[0x18];
    CUT_DRAW_CHAR_INSTANCE_SYSTEM_PC *characters;
} CUT_DRAW_REGISTRATION_INSTANCE_PC;
STATIC_ASSERT(offsetof(CUT_DRAW_REGISTRATION_INSTANCE_PC, characters) == 0xa4);
typedef struct CUT_DRAW_RIGID_PC {
    u8 unknown_00[0x58];
    u8 flags;
} CUT_DRAW_RIGID_PC;
typedef struct CUT_DRAW_LOCATOR_ENTRY_PC {
    char *name;
    u32 unknown_04, unknown_08, unknown_0c;
    void(__cdecl *update)(INSTNUGCUTSCENE *, CUT_DRAW_LOCATOR_SYS_PC *, CUT_DRAW_LOCATOR_INSTANCE_PC *,
                          CUT_DRAW_LOCATOR_PC *, f32, NUMTX *, i32);
} CUT_DRAW_LOCATOR_ENTRY_PC;
STATIC_ASSERT(sizeof(CUT_DRAW_LOCATOR_ENTRY_PC) == 20);
STATIC_ASSERT(offsetof(CUT_DRAW_LOCATOR_ENTRY_PC, update) == 0x10);
extern "C" {
    i32 __cdecl NuGCutLocatorIsVisble(CUT_LOCATOR_GENERIC_PC *, f32, NUANIM_TIME2_PC *, f32 *, f32 *);
    i32 __cdecl Draw3DObjectMtx(WORLDINFO *, i32, NUMTX *);
    static void *CutSceneFindModelPc(char *name, i32 model_only, i32 *character_id) {
        if (character_id != NULL)
            *character_id = -1;
        if (model_only != 0) {
            for (i32 i = 0; i < apicharsys->loaded_model_count; ++i) {
                i32 id = apicharsys->models[i].model_id;
                if (NuStrICmp(name, CDataList[id].name) == 0) {
                    if (character_id != NULL)
                        *character_id = apicharsys->models[i].model_id;
                    return &apicharsys->models[i];
                }
            }
        } else {
            for (i32 i = 0; i < 8; ++i) {
                GAMEOBJECT *object = static_cast<GAMEOBJECT *>(Player[i]);
                if (object != NULL && (object->flags_1fc & 1) != 0 &&
                    NuStrICmp(object->character_data->name, name) == 0)
                    return object;
            }
        }
        return NULL;
    }
    static void CutSceneFindCharacterPrivatePc(CUT_DRAW_SCENE_PC *scene, CUT_DRAW_CHARACTER_PC *character,
                                               CUT_LOAD_SYS_PC *system) {
        i32 id;
        CUT_DRAW_LOCATOR_SYS_PC *locators = reinterpret_cast<CUT_DRAW_REGISTRATION_SCENE_PC *>(scene)->locators;
        character->flags |= 2;
        character->character_model = CutSceneFindModelPc(character->name, character->flags & 2, &id);
        if (character->character_model == NULL)
            CutSceneConfigMissingPc(character->name);
        else if (id != -1)
            system->character_bits[id / 32] |= 1U << (id & 31);
        if (character->locator_count != 0 && character->locator.index < 0xff) {
            character->locator_index = static_cast<u8>(character->locator.index);
            character->locator.pointer = &locators->locators[character->locator.index];
        } else
            character->locator_index = 0xff;
    }
    static void CutScene_FindCharacters(CUT_DRAW_SCENE_PC *scene) {
        CUT_DRAW_CHARACTER_SYSTEM_PC *system = reinterpret_cast<CUT_DRAW_REGISTRATION_SCENE_PC *>(scene)->characters;
        for (i32 i = 0; i < system->count; ++i)
            CutSceneFindCharacterPrivatePc(scene, &system->characters[i], CS_cutsys);
    }
    static void CutScene_CreateCharacterInstance(CUT_DRAW_CHARACTER_PC *character, CUT_DRAW_CHAR_INSTANCE_PC *instance,
                                                 VARIPTR *) {
        if ((character->flags & 2) != 0)
            instance->character_model = character->character_model;
    }
    static void CutScene_ResetCharacters(INSTNUGCUTSCENE *owner) {
        CUT_DRAW_REGISTRATION_INSTANCE_PC *instance = reinterpret_cast<CUT_DRAW_REGISTRATION_INSTANCE_PC *>(owner);
        CUT_DRAW_CHARACTER_SYSTEM_PC *system = instance->scene->characters;
        for (i32 i = 0; i < system->count; ++i) {
            instance->characters->characters[i].blend_progress = 0;
            instance->characters->characters[i].flags = 0;
            instance->characters->characters[i].current_animation = 0xff;
        }
    }
    static void CutScene_EvalCharacter(INSTNUGCUTSCENE *owner, CUT_DRAW_SCENE_PC *, CUT_DRAW_CHAR_INSTANCE_PC *instance,
                                       CUT_DRAW_CHARACTER_PC *character, f32 frame) {
        GAMEOBJECT *object = (character->flags & 2) != 0 ? NULL : static_cast<GAMEOBJECT *>(instance->character_model);
        NUMTX matrix;
        i32 visible;
        NuGCutCharAnimProcess(reinterpret_cast<CUT_CHARACTER_GENERIC_PC *>(character), frame, &matrix, &visible, NULL,
                              NULL, NULL, NULL, NULL);
        CUT_DRAW_REGISTRATION_INSTANCE_PC *state = reinterpret_cast<CUT_DRAW_REGISTRATION_INSTANCE_PC *>(owner);
        if ((state->flags & 0x80) != 0)
            NuMtxMul(&matrix, &matrix, &state->matrix);
        if (object != NULL) {
            reinterpret_cast<CUT_DRAW_OBJECT_PC *>(object)->matrix = matrix;
            object->position = *reinterpret_cast<NUVEC *>(&matrix.m30);
        }
    }
    i32 __cdecl LevelObject_FindIndexFromName_RefOnly(char *);
    i32 __cdecl LevelObject_GetReflection(i32);
    static void CutScene_RigidPostRender(CUT_DRAW_RIGID_PC *rigid, NUSPECIALHANDLE *instance, NUMTX *matrix) {
        if ((rigid->flags & 0x10) == 0)
            return;
        NUVEC *position = reinterpret_cast<NUVEC *>(&matrix->m30);
        NUVEC *camera = reinterpret_cast<NUVEC *>(&reinterpret_cast<CUT_DRAW_CAMERA_PC *>(GameCam)->render_matrix.m30);
        if (!(CutReflectRange2 > NuVecDistSqr(position, camera, NULL)))
            return;
        f32 ground = GameShadow(NULL, position, 5.0f, -1);
        if ((f64)ground == 2000000.0)
            return;
        i32 surface = ShadowInfo();
        if (static_cast<u32>(surface) > 0x20 || (TerSurface[surface].flags & 2) == 0)
            return;
        WORLDINFO *world = WorldInfo_CurrentlyActive();
        NUMTX reflection = *matrix;
        reflection.m01 = -reflection.m01;
        reflection.m11 = -reflection.m11;
        reflection.m21 = -reflection.m21;
        reflection.m31 = ground - (reflection.m31 - ground);
        NUSPECIALHANDLE *special = instance;
        i32 index = LevelObject_FindIndexFromName_RefOnly(NuSpecialGetName(instance));
        if (index != -1) {
            index = LevelObject_GetReflection(index);
            if (index != -1) {
                NUSPECIALHANDLE *candidate = &world->level_objects[index].special;
                if (candidate != NULL)
                    special = candidate;
            }
        }
        NuRndrStartReflectionRender(0);
        NuSpecialDrawAt(special, &reflection);
        NuRndrEndReflectionRender();
    }
    static void LocatorFunction_Blaster(INSTNUGCUTSCENE *, CUT_DRAW_LOCATOR_SYS_PC *, CUT_DRAW_LOCATOR_INSTANCE_PC *,
                                        CUT_DRAW_LOCATOR_PC *locator, f32 frame, NUMTX *parent, i32) {
        NUANIM_TIME2_PC time;
        NuAnimData2CalcTime(locator->animated.animation, frame, &time);
        if (NuGCutLocatorIsVisble(&locator->prefix, frame, &time, NULL, NULL) == 0)
            return;
        NUMTX matrix;
        NuGCutLocatorCalcMtx(&locator->prefix, frame, &matrix, &time);
        if ((locator->flags & 4) != 0)
            NuMtxPreTranslate(&matrix, &locator->animated.pivot);
        if (parent != NULL)
            NuMtxMul(&matrix, &matrix, parent);
        Draw3DObjectMtx(NULL, CutSceneSys->blaster_object_0, &matrix);
        Draw3DObjectMtx(NULL, CutSceneSys->blaster_object_1, &matrix);
    }
    static CUT_DRAW_LOCATOR_ENTRY_PC cutscene_locatorfns[] = {
        {const_cast<char *>("blaster"), 0, 0, 0, LocatorFunction_Blaster}, {NULL, 0, 0, 0, NULL}};
    void __cdecl CutScenes_InitSystem(CUT_DRAW_SETTINGS_PC *system) {
        CutSceneSys = system;
        NuGCutSceneSysInit(reinterpret_cast<CUT_LOCATOR_FUNCTION_ENTRY_PC *>(cutscene_locatorfns));
        NuSetCutSceneCharacterRenderFn(reinterpret_cast<CUT_DRAW_RENDER_CALLBACK_PC>(CutScene_DrawCharacter));
        NuSetCutSceneFindCharactersFn(reinterpret_cast<void(__cdecl *)(CUT_SCENE_LOAD_PC *)>(CutScene_FindCharacters));
        NuSetCutSceneCharacterCreateDataFn(
            reinterpret_cast<void(__cdecl *)(CUT_LOAD_CHARACTER *, CUT_LOAD_CHARACTER_INSTANCE *, VARIPTR *)>(
                CutScene_CreateCharacterInstance));
        NuSetCutSceneResetCharactersFn(CutScene_ResetCharacters);
        NuSetCutSceneCharacterEvalFn(reinterpret_cast<CUT_DRAW_EVAL_CALLBACK_PC>(CutScene_EvalCharacter));
        NuSetCutSceneRigidPostRenderFn(reinterpret_cast<CUT_DRAW_RIGID_CALLBACK_PC>(CutScene_RigidPostRender));
    }
}

typedef struct CUT_CONTROLLER_INSTANCE_PC {
    u8 unknown_00[0x88];
    u32 flags_88, flags_8c;
    f32 current_frame, render_frame, rate;
} CUT_CONTROLLER_INSTANCE_PC;
STATIC_ASSERT(offsetof(CUT_CONTROLLER_INSTANCE_PC, flags_88) == 0x88);
STATIC_ASSERT(offsetof(CUT_CONTROLLER_INSTANCE_PC, current_frame) == 0x90);
STATIC_ASSERT(offsetof(CUT_CONTROLLER_INSTANCE_PC, rate) == 0x98);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, instance) == 4);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, unknown_58) == 0x58);
STATIC_ASSERT(offsetof(CUT_LOAD_TEXANIM_PC, frame) == 0);
STATIC_ASSERT(offsetof(CUT_LOAD_TEXANIM_PC, index) == 4);
STATIC_ASSERT(sizeof(CUT_LOAD_TEXANIM_PC) == 8);
STATIC_ASSERT(offsetof(CUT_DRAW_CHARACTER_SYSTEM_PC, count) == 4);
STATIC_ASSERT(offsetof(CUT_DRAW_CHARACTER_PC, character_model) == 0x50);
STATIC_ASSERT(offsetof(CUT_DRAW_CHAR_INSTANCE_PC, character_model) == 0);
STATIC_ASSERT(offsetof(WORLDINFO, cutscene_sys) == 0x2ab0);
STATIC_ASSERT(offsetof(WORLDINFO, level_progress) == 0x130);
typedef struct CUT_CONTROLLER_PLAYER_PC {
    void *clips;
    void *active;
} CUT_CONTROLLER_PLAYER_PC;
STATIC_ASSERT(offsetof(CUT_CONTROLLER_PLAYER_PC, active) == 4);
typedef struct CUT_CONTROLLER_CLIP_PC {
    u32 unknown_00;
    char name[1];
} CUT_CONTROLLER_CLIP_PC;
STATIC_ASSERT(offsetof(CUT_CONTROLLER_CLIP_PC, name) == 4);
typedef struct CUT_CONTROLLER_PROGRESS_PC {
    u8 unknown_00[0x2800];
    u8 scene_flags;
} CUT_CONTROLLER_PROGRESS_PC;
STATIC_ASSERT(offsetof(CUT_CONTROLLER_PROGRESS_PC, scene_flags) == 0x2800);
extern "C" {
    extern i32 CutSceneMusicStartFramePc;
    extern f32 LevelNameMul, LevelNameTime;
    extern u32 texanimbits;
    extern i32 CUTNOFOG, CUTDRAWWORLD, ACTIVECUTCOUNT, cut_waiting_for_new_level, CutInstEndCount;
    static INSTNUGCUTSCENE *CutInstEnd[4];
    static i32 CutInstEndStop;
    static i32 cutaudiopaused;
    static i32 CutFramePc;
    void __cdecl instNuGCutScenePause(INSTNUGCUTSCENE *, i32);
    void __cdecl instNuGCutSceneStop(INSTNUGCUTSCENE *);
    void __cdecl CutScenePlayer_SetObjects(CUTINFO *);
    void __cdecl PCFrameSetLoadingYieldEnabled(i32);

    static void CutSceneRebindPlayerCharactersPrivatePc(CUTSYS_PC *system) {
        if (system == NULL)
            return;
        for (i32 i = 0; i < system->count; ++i) {
            CUT_LOAD_RECORD_PC *cut = reinterpret_cast<CUT_LOAD_RECORD_PC *>(system->records[i]);
            if (cut->instance != NULL) {
                CUT_DRAW_CHARACTER_SYSTEM_PC *definitions =
                    reinterpret_cast<CUT_DRAW_REGISTRATION_SCENE_PC *>(cut->scene)->characters;
                CUT_DRAW_CHAR_INSTANCE_SYSTEM_PC *states =
                    reinterpret_cast<CUT_DRAW_REGISTRATION_INSTANCE_PC *>(cut->instance)->characters;
                if (definitions != NULL && states != NULL) {
                    for (i32 j = 0; j < definitions->count; ++j) {
                        CUT_DRAW_CHAR_INSTANCE_PC *state = &states->characters[j];
                        CUT_DRAW_CHARACTER_PC *definition = &definitions->characters[j];
                        if (state->character_model == reinterpret_cast<void *>(1)) {
                            if ((state->flags & 2) != 0)
                                state->character_model = NULL;
                            else
                                state->character_model = CutSceneFindModelPc(definition->name, 0, NULL);
                            definition->character_model = state->character_model;
                        }
                    }
                }
            }
        }
    }
    static void CutSceneResetTextureAnimationsPrivatePc(CUT_LOAD_RECORD_PC *cut) {
        for (i32 i = 0; i < 4; ++i) {
            i32 index = cut->texture_animations[i].index;
            if (index != -1)
                texanimbits &= ~(1U << (index & 31));
        }
    }
    static i32 CutScenePlayOncePrivatePc(WORLDINFO *world, CUT_LOAD_RECORD_PC *cut) {
        CUTSYS_PC *system = world->cutscene_sys;
        i32 index = -1;
        if (system != NULL) {
            for (i32 i = 0; i < system->count; ++i) {
                if (system->records[i] == reinterpret_cast<CUTINFO *>(cut))
                    index = static_cast<u16>(i);
            }
            if (world->level_progress != NULL) {
                u32 bit = 1U << (index & 31);
                if (static_cast<i16>(index) != -1 && (cut->end_flags & 1) != 0 &&
                    (world->level_progress->progress_state & bit) != 0)
                    return 1;
                world->level_progress->progress_state |= bit;
            }
        }
        return 0;
    }
    static void CutSceneSetStopFlagsPrivatePc(CUT_LOAD_RECORD_PC *cut) {
        CUTSTOPGAME = 1;
        CutStopInfo = cut;
        CUTDRAWWORLD = cut->flags & 2;
        CUTNOFOG = cut->flags & 4;
        DebrisSetRenderGroup(cut->debris_render_group);
        LevelNameMul = 0.0f;
        LevelNameTime = 0.0f;
        PCFrameSetLoadingYieldEnabled(1);
    }
    static i32 CutSceneStartPrivatePc(WORLDINFO *world, CUT_LOAD_RECORD_PC *cut) {
        if (cut == NULL || cut->instance == NULL)
            return 0;
        if (CutScenePlayOncePrivatePc(world, cut) != 0)
            return 0;
        instNuGCutSceneReset(cut->instance);
        if (CutScene_StartFn != NULL)
            CutScene_StartFn(reinterpret_cast<CUTINFO *>(cut));
        if (CutScenePlayerPc != NULL && reinterpret_cast<CUT_CONTROLLER_PLAYER_PC *>(CutScenePlayerPc)->active != NULL)
            CutScenePlayer_SetObjects(reinterpret_cast<CUTINFO *>(cut));
        f32 rate = cut->frames_per_second * FRAMETIME;
        reinterpret_cast<CUT_CONTROLLER_INSTANCE_PC *>(cut->instance)->rate = rate;
        instNuGCutSceneStart(cut->instance);
        if ((cut->flags & 0x200) != 0)
            reinterpret_cast<CUT_CONTROLLER_INSTANCE_PC *>(cut->instance)->flags_88 |= 8;
        else
            reinterpret_cast<CUT_CONTROLLER_INSTANCE_PC *>(cut->instance)->flags_88 &= ~8U;
        cut->unknown_58 = 0.0f;
        if (cut->music_handle != -1) {
            if (!music_man.SelectTrackByHandle(16, cut->music_handle))
                CutSceneConfigMissingPc(reinterpret_cast<char *>(cut->music_handle));
            i32 status = music_man.PlayTrack(16, 1);
            CutSceneMusicStartFramePc = nuapi.frame_count;
            if (status != 1)
                CutSceneConfigMissingPc(reinterpret_cast<char *>(status));
            reinterpret_cast<CUT_CONTROLLER_INSTANCE_PC *>(cut->instance)->rate = 0.0f;
            if ((cut->flags & 1) != 0 && status == 1) {
                CutSceneWaiting = 1;
                STUBBED_NAMED(0x00652400); /* Native PauseGameAudio(0). */
                cutaudiopaused = 1;
            } else
                cutaudiopaused = 0;
        } else {
            STUBBED_NAMED(0x00652400); /* Native PauseGameAudio(0). */
            STUBBED_NAMED(0x00652b60); /* Native SetLinkedCutSceneMusic(instance, linked_audio ? 13 : 11). */
            rate = cut->frames_per_second * FRAMETIME;
            reinterpret_cast<CUT_CONTROLLER_INSTANCE_PC *>(cut->instance)->rate = rate;
            if ((cut->flags & 1) == 0)
                return 1;
            CutSceneWaiting = 0;
            cutaudiopaused = 0;
        }
        if ((cut->flags & 1) == 0)
            return 1;
        STUBBED_NAMED(0x004ba190); /* Native Panel_Clear(). */
        CutFramePc = 0;
        for (i32 i = 0; i < world->cutscene_sys->count; ++i) {
            CUT_LOAD_RECORD_PC *other = reinterpret_cast<CUT_LOAD_RECORD_PC *>(world->cutscene_sys->records[i]);
            if ((reinterpret_cast<CUT_CONTROLLER_INSTANCE_PC *>(other->instance)->flags_88 & 2) != 0) {
                if (other != cut && other->instance != NULL && (other->flags & 1) != 0) {
                    reinterpret_cast<CUT_CONTROLLER_INSTANCE_PC *>(
                        reinterpret_cast<CUT_LOAD_RECORD_PC *>(world->cutscene_sys->records[i])->instance)
                        ->rate = 0.0f;
                    instNuGCutSceneStop(
                        reinterpret_cast<CUT_LOAD_RECORD_PC *>(world->cutscene_sys->records[i])->instance);
                } else {
                    reinterpret_cast<CUT_CONTROLLER_INSTANCE_PC *>(
                        reinterpret_cast<CUT_LOAD_RECORD_PC *>(world->cutscene_sys->records[i])->instance)
                        ->rate = 0.0f;
                    instNuGCutScenePause(
                        reinterpret_cast<CUT_LOAD_RECORD_PC *>(world->cutscene_sys->records[i])->instance, 1);
                }
            }
        }
        CutSceneSetStopFlagsPrivatePc(cut);
        CutBorderScale = 1.0f;
        ACTIVECUTCOUNT = 1;
        cut_waiting_for_new_level = 0;
        STUBBED_NAMED(0x00564ca0); /* Native GameFog_Reset(). */
        CutSceneResetTextureAnimationsPrivatePc(cut);
        return 1;
    }
    void __cdecl CutScenes_Start(WORLDINFO *world) {
        for (i32 i = 0; i < NewCutInfoCount; ++i)
            CutSceneStartPrivatePc(world, reinterpret_cast<CUT_LOAD_RECORD_PC *>(NewCutInfo[i]));
        NewCutInfoCount = 0;
    }
    void __cdecl CutScenes_Reset(WORLDINFO *world) {
        CutSceneRebindPlayerCharactersPrivatePc(world->cutscene_sys);
        CutBorderScale = 0.0f;
        CutStopInfo = NULL;
        CutSceneWaiting = 0;
        cutaudiopaused = 0;
        if (world->cutscene_sys != NULL && reset_restart != 0 &&
            ((world->current_level->flags & 0xe0) != 0 || world->level_progress == NULL ||
             (reinterpret_cast<CUT_CONTROLLER_PROGRESS_PC *>(world->level_progress)->scene_flags & 2) == 0)) {
            CUTINFO *cut = NULL;
            CUTSYS_PC *system = world->cutscene_sys;
            if (CutScenePlayerPc != NULL &&
                reinterpret_cast<CUT_CONTROLLER_PLAYER_PC *>(CutScenePlayerPc)->active != NULL && system->count > 0) {
                char *name = reinterpret_cast<CUT_CONTROLLER_CLIP_PC *>(
                                 reinterpret_cast<CUT_CONTROLLER_PLAYER_PC *>(CutScenePlayerPc)->active)
                                 ->name;
                if (name[0] != 0)
                    cut = CutScene_Find(system, name);
                if (cut == NULL) {
                    system = world->cutscene_sys;
                    cut = system->records[0];
                }
            }
            if (cut == NULL) {
                for (i32 i = 0; i < system->count && cut == NULL; ++i) {
                    CUT_LOAD_RECORD_PC *record = reinterpret_cast<CUT_LOAD_RECORD_PC *>(system->records[i]);
                    if ((record->flags & 0x1000) != 0)
                        cut = reinterpret_cast<CUTINFO *>(record);
                }
            }
            if (cut != NULL && CutSceneStartPrivatePc(world, reinterpret_cast<CUT_LOAD_RECORD_PC *>(cut)) != 0)
                CutBorderScale = 1.0f;
        }
        NewCutInfoCount = 0;
        cut_waiting_for_new_level = 0;
    }
    void __cdecl CutScenes_Stop(CUTSYS_PC *system) {
        if (system == NULL)
            return;
        for (i32 i = 0; i < system->count; ++i) {
            INSTNUGCUTSCENE *instance = reinterpret_cast<CUT_LOAD_RECORD_PC *>(system->records[i])->instance;
            if (instance != NULL)
                instNuGCutSceneStop(instance);
        }
    }
    void __cdecl CutScenes_End(void) {
        if (CutScenePlayerPc != NULL &&
            reinterpret_cast<CUT_CONTROLLER_PLAYER_PC *>(CutScenePlayerPc)->active != NULL && NewLData != NULL &&
            NewLData == HUB_LDATA) {
            PCFrameSetLoadingYieldEnabled(0);
            return;
        }
        for (i32 i = 0; i < CutInstEndCount; ++i) {
            instNuGCutSceneEnd(CutInstEnd[i]);
            if (i == CutInstEndStop) {
                CUTSTOPGAME = 0;
                CutStopInfo = NULL;
                STUBBED_NAMED(0x0054a2f0); /* Native GameCam_Reset(GameCam). */
                PCFrameSetLoadingYieldEnabled(0);
            }
        }
    }
}

STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, previous_frame) == 0x54);
STATIC_ASSERT(offsetof(CUT_LOAD_SFX_PC, id) == 0);
STATIC_ASSERT(offsetof(CUT_LOAD_SFX_PC, flags) == 2);
STATIC_ASSERT(offsetof(CUT_LOAD_SFX_PC, frame) == 4);
STATIC_ASSERT(offsetof(CUT_LOAD_SFX_PC, position) == 8);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, blob_shadow_fade_near) == 0xf0);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, blob_shadow_fade_far) == 0xf1);
STATIC_ASSERT(offsetof(CUT_LOAD_RECORD_PC, reflection_range) == 0xf2);
STATIC_ASSERT(offsetof(WORLDINFO, area) == 0x12c);
STATIC_ASSERT(offsetof(LEVELDATA_PC, flags) == 0x64);
STATIC_ASSERT(sizeof(LEVELDATA_PC) == 0x130);
enum FADETYPE_VALUE {
    FADE_TYPE_NONE = -1,
    FADE_TYPE_SCREEN = 0,
    FADE_TYPE_WIPE = 1,
    FADE_TYPE_STILL_WIPE = 2,
    FADE_TYPE_STILL = 3
};
struct FADEINFO_s;
struct FadeBase {
    virtual ~FadeBase();
    virtual void Init(FADEINFO_s *info) = 0;
    virtual void InitFade() = 0;
    virtual void UpdateFade() = 0;
    virtual void DrawFade() = 0;
    virtual FADETYPE_VALUE GetFadeType() const = 0;
};
struct FADEINFO_s {
    u32 direction;
    f32 fade, rate;
    i32 busy, stage;
    FadeBase *fades[4];
    FADETYPE_VALUE pending_type;
    i32 field_28;
};
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
};
STATIC_ASSERT(sizeof(FadeSystem) == 0x2c);
STATIC_ASSERT(sizeof(FADETYPE) == 4);
extern "C" {
    extern i32 CUTCAMONLY;
    static f32 CutReflectRange;
    i32 __cdecl instNuGCutSceneIsFinished(INSTNUGCUTSCENE *);
    void __cdecl GizmoSysDraw(void *, WORLDINFO *, f32);
    static void CutSceneUpdateTextureAnimationsPrivatePc(CUT_LOAD_RECORD_PC *cut) {
        u32 bits = texanimbits;
        for (i32 i = 0; i < 4; ++i) {
            i32 index = cut->texture_animations[i].index;
            if (index != -1) {
                u32 bit = 1U << (index & 31);
                if ((bits & bit) == 0 &&
                    cut->texture_animations[i].frame <=
                        reinterpret_cast<CUT_CONTROLLER_INSTANCE_PC *>(cut->instance)->current_frame) {
                    bits |= bit;
                    texanimbits = bits;
                }
            }
        }
    }
    void __cdecl CutScenes_Update(WORLDINFO *world, i32 paused) {
        i32 active_before[32];
        if (CutScenePlayerPc != NULL &&
            reinterpret_cast<CUT_CONTROLLER_PLAYER_PC *>(CutScenePlayerPc)->active != NULL && CutStopInfo != NULL)
            CutScenePlayer_SetObjects(reinterpret_cast<CUTINFO *>(CutStopInfo));
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
        if (world->cutscene_sys == NULL)
            return;
        i32 stop_index = -1;
        for (i32 i = 0; i < world->cutscene_sys->count; ++i) {
            CUT_LOAD_RECORD_PC *cut = reinterpret_cast<CUT_LOAD_RECORD_PC *>(world->cutscene_sys->records[i]);
            CUT_CONTROLLER_INSTANCE_PC *instance = reinterpret_cast<CUT_CONTROLLER_INSTANCE_PC *>(cut->instance);
            if (instance != NULL && (instance->flags_88 & 2) != 0) {
                if ((cut->flags & 1) != 0) {
                    stop_index = i;
                    CutSceneSetStopFlagsPrivatePc(cut);
                    CutFramePc = static_cast<i32>(static_cast<u32>(CutFramePc) + 1U);
                    break;
                }
                if ((cut->flags & 0x20) != 0)
                    CUTCAMONLY = 1;
            }
        }
        if (stop_index == -1)
            DebrisSetRenderGroup(1);
        ACTIVECUTCOUNT = 0;
        for (i32 i = 0; i < world->cutscene_sys->count; ++i) {
            CUT_LOAD_RECORD_PC *cut = reinterpret_cast<CUT_LOAD_RECORD_PC *>(world->cutscene_sys->records[i]);
            CUT_CONTROLLER_INSTANCE_PC *instance = reinterpret_cast<CUT_CONTROLLER_INSTANCE_PC *>(cut->instance);
            if (instance == NULL)
                continue;
            if ((instance->flags_88 & 2) == 0) {
                active_before[i] = 0;
                continue;
            }
            cut->previous_frame = instance->current_frame;
            active_before[i] = 1;
            f32 elapsed = cut->unknown_58 + FRAMETIME;
            cut->unknown_58 = elapsed;
            if (cutaudiopaused != 0 && elapsed >= 23.0f)
                cutaudiopaused = 0;
            if (stop_index != -1 && i != stop_index) {
                instance->rate = 0.0f;
                instNuGCutScenePause(cut->instance, 1);
                ++ACTIVECUTCOUNT;
                continue;
            }
            bool wait = false;
            if (NOSOUND == 0 && NOMUSIC == 0 && i == stop_index && cut->music_handle != -1 && instance->rate == 0.0f &&
                cutaudiopaused != 0 && music_man.GetStatus(16, NULL) != 4) {
                reinterpret_cast<CUT_CONTROLLER_INSTANCE_PC *>(cut->instance)->rate = 0.0f;
                CutSceneWaiting = 1;
                wait = true;
            }
            if (!wait) {
                if (CutSceneMusicStartFramePc != -1) {
                    if (static_cast<i32>(nuapi.frame_count) <
                        static_cast<i32>(static_cast<u32>(CutSceneMusicStartFramePc) + 5U)) {
                        reinterpret_cast<CUT_CONTROLLER_INSTANCE_PC *>(cut->instance)->rate = 0.0f;
                        CutSceneWaiting = 1;
                        wait = true;
                    } else {
                        CutSceneMusicStartFramePc = -1;
                        music_man.PlayTrack(16, 0);
                    }
                }
                if (!wait) {
                    f32 rate = cut->frames_per_second * FRAMETIME;
                    reinterpret_cast<CUT_CONTROLLER_INSTANCE_PC *>(cut->instance)->rate = rate;
                    if (i == stop_index && CutSceneWaiting != 0) {
                        CutSceneWaiting = 0;
                        cutaudiopaused = 0;
                    }
                }
            }
            instNuGCutScenePause(cut->instance, 0);
            if (CutScene_PreUpdateFn != NULL)
                CutScene_PreUpdateFn(reinterpret_cast<CUTINFO *>(cut));
            CutSceneUpdateTextureAnimationsPrivatePc(cut);
            ++ACTIVECUTCOUNT;
        }
        NuGCutSceneSysUpdate(paused, 0, 1.0f);
        if (CutScene_PostUpdateFn != NULL)
            CutScene_PostUpdateFn();
        if (paused == 0) {
            for (i32 i = 0; i < world->cutscene_sys->count; ++i) {
                CUT_LOAD_RECORD_PC *cut = reinterpret_cast<CUT_LOAD_RECORD_PC *>(world->cutscene_sys->records[i]);
                if (cut->instance == NULL)
                    continue;
                if (active_before[i] != 0 &&
                    reinterpret_cast<CUT_CONTROLLER_INSTANCE_PC *>(cut->instance)->rate > 0.0f) {
                    for (i32 j = 0; j < 6; ++j) {
                        CUT_LOAD_SFX_PC *sfx = &cut->sfx[j];
                        if (sfx->id != -1 && cut->previous_frame < sfx->frame &&
                            sfx->frame <= reinterpret_cast<CUT_CONTROLLER_INSTANCE_PC *>(cut->instance)->current_frame)
                            PlaySfxById(sfx->id, (sfx->flags & 1) != 0 ? &sfx->position : NULL);
                    }
                }
                if ((stop_index == -1 || i != stop_index) && active_before[i] != 0 &&
                    instNuGCutSceneIsFinished(cut->instance) != 0) {
                    if (CutScene_StoppedFn != NULL)
                        CutScene_StoppedFn(reinterpret_cast<CUTINFO *>(cut));
                    if (CutInstEndCount < 4) {
                        CutInstEnd[CutInstEndCount] = cut->instance;
                        reinterpret_cast<CUT_CONTROLLER_INSTANCE_PC *>(cut->instance)->flags_88 |= 2;
                        ++CutInstEndCount;
                    }
                }
            }
        }
        if (stop_index == -1)
            return;
        CUT_LOAD_RECORD_PC *stop_cut = reinterpret_cast<CUT_LOAD_RECORD_PC *>(world->cutscene_sys->records[stop_index]);
        STUBBED_NAMED(0x00564ca0); /* Native GameFog_Reset(). */
        if (instNuGCutSceneIsFinished(stop_cut->instance) == 0)
            return;
        if (NewLData == NULL || NewLData == WORLD->current_level || NewLData != HUB_LDATA) {
            FADETYPE type = {FADE_TYPE_STILL_WIPE};
            FadeSys.SetFade(type, 0);
        }
        if (CutScene_StoppedFn != NULL)
            CutScene_StoppedFn(reinterpret_cast<CUTINFO *>(stop_cut));
        CutSceneResetTextureAnimationsPrivatePc(stop_cut);
        if (stop_cut->skip_level != -1) {
            NewLData = &LDataList[stop_cut->skip_level];
            if (HUB_LDATA != NULL && NewLData == HUB_LDATA) {
                if (CutScenePlayerPc != NULL &&
                    reinterpret_cast<CUT_CONTROLLER_PLAYER_PC *>(CutScenePlayerPc)->active != NULL) {
                    NewLevelFromMenu(HUB_LDATA, -1, -1, 1);
                    RenderHubFromCutscenePlayerPc = 1;
                } else if (WORLD->area != NULL && (WORLD->area->flags & 2) != 0)
                    NewLData = SuperStory != 0 ? STATUS_LDATA : CREDITS_LDATA;
            } else if (CutScenePlayerPc != NULL &&
                       reinterpret_cast<CUT_CONTROLLER_PLAYER_PC *>(CutScenePlayerPc)->active != NULL &&
                       (NewLData->flags & 0xe0) == 0 && (stop_cut->end_flags & 2) == 0) {
                NewLevelFromMenu(HUB_LDATA, -1, -1, 1);
                RenderHubFromCutscenePlayerPc = 1;
            }
            if (waiting_for_level != -1) {
                cut_waiting_for_new_level = 1;
                STUBBED_NAMED(0x00652fb0); /* Native PauseGameCut(). */
                music_man.PauseTrack(16);
                NuSound3Update();
                newlevel_resumecutaudio = 1;
            }
        } else {
            CUTINFO *next = NewCutScene(NULL, world->cutscene_sys, stop_cut->next_cutscene, 0);
            if (CutScenePlayerPc != NULL &&
                reinterpret_cast<CUT_CONTROLLER_PLAYER_PC *>(CutScenePlayerPc)->active != NULL && next == NULL) {
                NewLevelFromMenu(HUB_LDATA, -1, -1, 1);
                RenderHubFromCutscenePlayerPc = 1;
            } else if ((stop_cut->flags & 0x400) != 0)
                NewMode = 1;
            else if (stop_cut->door_name[0] != 0) {
                void *door = Door_FindByName(world, stop_cut->door_name);
                if (door != NULL)
                    STUBBED_NAMED(0x00575c20); /* Native Door_GoThrough(world,door,1). */
            } else if ((stop_cut->flags & 0x100) != 0) {
                FADETYPE type = {FADE_TYPE_STILL_WIPE};
                FadeSys.SetFade(type, 0);
                STUBBED_NAMED(0x00565be0); /* Native NeedScreenGrab(1). */
                FadeSys.SetStage(1);
                GameAudio_PlaySfx(0x2d, NULL, 0, 0);
            }
        }
        CUTNOFOG = 0;
        if (CutInstEndCount < 4) {
            CutInstEnd[CutInstEndCount] = stop_cut->instance;
            reinterpret_cast<CUT_CONTROLLER_INSTANCE_PC *>(stop_cut->instance)->flags_88 |= 2;
            CutInstEndStop = CutInstEndCount;
            ++CutInstEndCount;
        }
    }
    void __cdecl CutScenes_Draw(WORLDINFO *world) {
        if (world->cutscene_sys == NULL || ACTIVECUTCOUNT <= 0)
            return;
        SetLevelLights(world->rtl_set, 1.0f);
        CUT_LOAD_RECORD_PC *cut = reinterpret_cast<CUT_LOAD_RECORD_PC *>(CutStopInfo);
        if (cut != NULL) {
            CutBlobShadowAlpha = cut->blob_shadow_alpha;
            CutBlobShadowFadeNear = static_cast<f32>(cut->blob_shadow_fade_near);
            CutBlobShadowFadeFar = static_cast<f32>(cut->blob_shadow_fade_far);
            CutReflectRange = static_cast<f32>(cut->reflection_range);
        } else {
            CutBlobShadowAlpha = world->current_level->blob_shadow_alpha;
            CutBlobShadowFadeNear = static_cast<f32>(world->current_level->blob_shadow_fade_near);
            CutBlobShadowFadeFar = static_cast<f32>(world->current_level->blob_shadow_fade_far);
            CutReflectRange = static_cast<f32>(world->current_level->reflection_range);
        }
        CutReflectRange2 = CutReflectRange * CutReflectRange;
        NuGCutSceneSysRender(Paused);
        if (CUTDRAWWORLD != 0 && world->current_gscn != NULL) {
            SetLevelLights(world->rtl_set, 1.0f);
            STUBBED_NAMED(0x00601df0); /* Native NuGScnRndr3(world->current_gscn). */
            if (world->gizmo_sys != NULL && CutStopInfo != NULL &&
                (reinterpret_cast<CUT_LOAD_RECORD_PC *>(CutStopInfo)->flags & 0x8000) != 0)
                GizmoSysDraw(world->gizmo_sys, world, FRAMETIME);
        }
    }
}

extern "C" void Bolts_Reset() {
    memset(Bolt, 0, sizeof(Bolt));
    i_bolt = 0;
    BOLT_OVERRIDE_PLAYERBOLTSPEED = 0.0f;
    BOLT_OVERRIDE_PLAYERBOLTDURATION = 0.0f;
}

extern "C" {
    static void bgAckStreamCutScene(BGPROCINFO *) {
        stream_cut_issued = 0;
    }
}

extern "C" {
    static void bgLoadStreamCutScene(BGPROCINFO *) {
        if (cutscene_load_instance != NULL) {
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
    i32 CutSceneMusicStartFramePc = -1;
}
