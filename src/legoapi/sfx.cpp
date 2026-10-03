#include "saga.h"

extern "C" f32 AUDIOFADELEVEL;

f32 GameGetMusicVolume(OPTIONSSAVE *options) {
    f32 volume = (f32)options->music_volume / 10.0;
    return (f32)(volume * ((f64)options->master_volume / 10.0));
}

f32 GameSetMusicVolume(OPTIONSSAVE *options) {
    f32 volume = GameGetMusicVolume(options);
    if (options->music_enabled == 0 && WORLD->current_level == TITLES_LDATA) {
        volume = 0.0f;
    }
    music_man.SetClassVolume(0x23, volume);
    return volume;
}

f32 GameSetSoundVolume(OPTIONSSAVE *options) {
    f32 volume = (f32)options->sound_volume / 10.0;
    f32 cut_volume;
    volume = (f32)(volume * ((f64)options->master_volume / 10.0));
    SetSoundVolume(volume);
    cut_volume = (f32)((f64)0.85f * (f64)options->master_volume / 10.0);
    music_man.SetClassVolume(0x10, cut_volume);
    return volume;
}

void legoSetMusicVolume(f32 volume) {
    music_man.SetClassVolume(0x23, volume);
}

void legoSetCutVolume(f32 volume) {
    music_man.SetClassVolume(0x10, volume);
}

void SetAudioFadeLevel(f32 level) {
    AUDIOFADELEVEL = level;
}

f32 GetAudioFadeLevel(void) {
    return AUDIOFADELEVEL;
}

void SfxCheckMusicOnOff(OPTIONSSAVE *options) {
    (void)options;
}

extern "C" void __cdecl LC_AL_music(NUFPAR_PC *parser) {
    char *name;

    if (NuFParGetWord(parser) == 0) {
        return;
    }

    name = parser->word_buffer_ptr;
    levelconfig_ldata->music_index = GetMusicIndex(name, MusicInfo, -1);
    levelconfig_ldata->music_tracks[0] = music_man.GetTrackHandle(1, name);
    levelconfig_ldata->music_tracks[2] = music_man.GetTrackHandle(2, name);
    levelconfig_ldata->music_tracks[4] = music_man.GetTrackHandle(0x20, name);
}

extern "C" void __cdecl LC_AL_music_other(NUFPAR_PC *parser) {
    char *name;

    if (NuFParGetWord(parser) == 0) {
        return;
    }

    name = parser->word_buffer_ptr;
    levelconfig_ldata->music_other_index = GetMusicIndex(name, MusicInfo, -1);
    levelconfig_ldata->music_tracks[1] = music_man.GetTrackHandle(1, name);
    levelconfig_ldata->music_tracks[3] = music_man.GetTrackHandle(2, name);
    levelconfig_ldata->music_tracks[5] = music_man.GetTrackHandle(0x20, name);
}

void __cdecl PlayRepeatSfx(char *name, i32 sfx_id, f32 start_delay, i32 play_count_arg, f32 repeat_delay,
                           NUVEC *position) {
    REPEAT_SFX_ENTRY_PC *entry;
    i32 queue_index;
    u8 play_count;

    play_count = (u8)play_count_arg;
    if (play_count == 1 && start_delay == 0.0f) {
        if (sfx_id != -1) {
            PlaySfxById(sfx_id, position);
        } else {
            PlaySfx(name, position);
        }
        return;
    }

    queue_index = RepeatSfxQueueHead;
    entry = &RepeatSfxQueue[queue_index];
    if (start_delay > 0.0f) {
        entry->state = 1;
    } else {
        entry->state = 2;
    }

    if (sfx_id == -1) {
        sfx_id = GetSfxId(name);
        queue_index = RepeatSfxQueueHead;
        entry = &RepeatSfxQueue[queue_index];
    }
    entry->sfx_id = (i16)sfx_id;
    entry->start_delay = start_delay;
    entry->play_count = (i8)play_count;
    entry->repeat_delay = repeat_delay;
    entry->position = position;
    RepeatSfxQueueHead = (queue_index + 1) & 0x1f;
}

void __cdecl UpdateRepeatSfx(void) {
    i32 index;
    for (index = 0; index < 32; ++index) {
        REPEAT_SFX_ENTRY_PC *entry = &RepeatSfxQueue[index];
        switch (entry->state) {
            case 1:
            case 3:
                if (entry->start_delay > 0.0f) {
                    entry->start_delay -= FRAMETIME;
                } else {
                    entry->state = 2;
                }
                break;
            case 2:
                PlaySfxById(entry->sfx_id, entry->position);
                --entry->play_count;
                if (entry->play_count > 0) {
                    entry->start_delay = entry->repeat_delay;
                    entry->state = 3;
                } else {
                    entry->sfx_id = 0;
                    entry->state = 0;
                    entry->play_count = 0;
                    entry->start_delay = 0.0f;
                    entry->repeat_delay = 0.0f;
                    entry->position = NULL;
                }
                break;
        }
    }
}

void __cdecl PlayJumpSfx(GAMEOBJECT *object, i32 type) {
    i32 sfx;
    u32 flags = object->character_data->model_flags;
    if ((flags & 0x40) != 0)
        sfx = GameAudio->sfx_ids[1];
    else if ((flags & 8) == 0)
        sfx = GameAudio->sfx_ids[0];
    else {
        switch (type) {
            case 0:
                sfx = GameAudio->sfx_ids[2];
                break;
            case 1:
                sfx = GameAudio->sfx_ids[3];
                break;
            case 2:
                sfx = GameAudio->sfx_ids[6];
                break;
            case 3:
                sfx = GameAudio->sfx_ids[4];
                break;
            case 4:
                sfx = GameAudio->sfx_ids[5];
                break;
            default:
                return;
        }
    }
    if (sfx != -1) {
        if ((object->flags_1fc & 0x80) == 0 && (object->portal_flags & 0x08000000) == 0)
            PlaySfxByIdAndSetVolume(sfx, &object->lower_position, 0.5f);
        else
            PlaySfxById(sfx, &object->lower_position);
    }
}

extern "C" void(__cdecl *ExtraDieSfxFn)(GAMEOBJECT *);

extern "C" void __cdecl PlayDieSfx(GAMEOBJECT *object) {
    CHARACTERDATA_PC *character = object->character_data;
    PLAYERCHARACTERCONFIG_PC *config = character->game_character;
    i32 sfx = config->sfx_die;
    if (sfx == -1) {
        u32 flags = character->model_flags;
        if ((config->flags_090 & 0x800) != 0 || (flags & 0x2000) != 0)
            sfx = GameAudio->sfx_ids[0x18];
        else if ((flags & 0x04000000) != 0)
            sfx = GameAudio->sfx_ids[0x19];
        else if ((flags & 0x10) != 0)
            sfx = GameAudio->sfx_ids[0x1a];
        else if ((flags & 0x40000000) == 0)
            sfx = GameAudio->sfx_ids[(object->flags_ef4 & 0x800) != 0 ? 0x1b : 0x1c];
        if (sfx == -1)
            goto extra;
    }
    PlaySfxById(sfx, &object->collision_position);
extra:
    if (ExtraDieSfxFn != NULL)
        ExtraDieSfxFn(object);
}

extern "C" {
    void __cdecl PlayLandSfx(GAMEOBJECT *object, i32 type, i32 ignored) {
        i32 sfx;
        i32 alternate;
        (void)ignored;
        if (type == 1) {
            if ((object->character_data->model_flags & 8) == 0)
                return;
            sfx = GameAudio->sfx_ids[11];
        } else if (type == 2) {
            sfx = GameAudio->sfx_ids[12];
        } else if (type == 3) {
            sfx = GameAudio->sfx_ids[13];
        } else if (type == 4) {
            sfx = GameAudio->sfx_ids[14];
        } else {
            if (object->is_underwater != 0 || object->water_intersects != 0)
                return;
            alternate = (WorldInfo_CurrentlyActive()->current_level->flags & 0x1000) != 0;
            if ((object->character_data->model_flags & 0x10) != 0)
                sfx = GameAudio->sfx_ids[alternate ? 10 : 9];
            else {
                if (type != 0)
                    return;
                sfx = GameAudio->sfx_ids[alternate ? 8 : 7];
            }
        }
        if (sfx != -1) {
            if ((object->flags_1fc_word & 0x80) != 0 || (object->portal_flags & 0x8000000) != 0)
                PlaySfxById(sfx, &object->lower_position);
            else
                PlaySfxByIdAndSetVolume(sfx, &object->lower_position, 0.5f);
        }
    }
}

extern "C" void __cdecl PlaySfxByIdAndSetVolumeAndPitch(i32 sfx_id, NUVEC *position, f32 volume, f32 pitch) {
    PlaySfxByIdEx(sfx_id, position, volume, pitch);
}

typedef struct level_sfx_gizmotype_s {
    u8 unknown_00[0x94];
    void(__cdecl *add_level_sfx)(WORLDINFO *, void *, i32 *, i32 *, i32);
    u8 unknown_98[4];
} LEVEL_SFX_GIZMOTYPE_PC;
STATIC_ASSERT(sizeof(LEVEL_SFX_GIZMOTYPE_PC) == 0x9c);
STATIC_ASSERT(offsetof(LEVEL_SFX_GIZMOTYPE_PC, add_level_sfx) == 0x94);

extern "C" {
    void __cdecl AddLevelSfxFromName(char *name, i32 *ids, i32 *count, i32 capacity) {
        i32 id, i;
        if (ids == NULL || count == NULL || *count >= capacity)
            return;
        id = GetSfxId(name);
        if (id == -1)
            return;
        for (i = 0; i < *count; ++i)
            if (ids[i] == id)
                return;
        ids[*count] = id;
        ++*count;
    }

    void __cdecl AddLevelSfxFromId(i32 id, i32 *ids, i32 *count, i32 capacity) {
        i32 i;
        if (ids == NULL || count == NULL || *count >= capacity || id == -1)
            return;
        for (i = 0; i < *count; ++i)
            if (ids[i] == id)
                return;
        ids[*count] = id;
        ++*count;
    }

    void __cdecl AddLevelSfxGizmoSys(GIZMOSYS_PREFIX_PC *system, WORLDINFO *world, i32 *ids, i32 *count, i32 capacity) {
        LEVEL_SFX_GIZMOTYPE_PC *type;
        GIZMOSET_PC *set;
        i32 i;
        if (gizmotypes == NULL || system == NULL)
            return;
        set = system->sets;
        type = (LEVEL_SFX_GIZMOTYPE_PC *)((GIZMOTYPES_PC *)gizmotypes)->types;
        for (i = 0; i < ((GIZMOTYPES_PC *)gizmotypes)->count; ++i, ++type, ++set)
            if (type->add_level_sfx != NULL)
                type->add_level_sfx(world, set->data, ids, count, capacity);
    }
}

extern "C" void __cdecl PlaySabreSfx(char *name, GAMEOBJECT *object, NUVEC *position, i32 unused) {
    u8 player = object->field_280;
    i32 bits = 0;
    i32 sfx;
    (void)unused;
    if (player == 0xff && WORLD->rooms_visible_ptr[object->portal_room] == 0)
        return;
    if (object->flags_1fc & 0x80)
        bits = 1 << (player & 31);
    if (name != NULL) {
        sfx = GetSfxId(name);
        if (position == NULL)
            position = &object->collision_position;
        PlaySfxById(sfx, position);
    } else {
        sfx = object->character_data->game_character->sfx_sabre;
        if (sfx != -1)
            GameAudio_PlaySfxById(sfx, &object->collision_position, bits, 1);
        else
            PlaySfxById(GameAudio->sfx_ids[0x40], &object->collision_position);
    }
}
