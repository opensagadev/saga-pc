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
