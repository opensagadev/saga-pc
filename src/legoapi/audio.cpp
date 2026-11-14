#include "saga.h"
#include <string.h>
#include <stdio.h>

/* Retail unit 0x5477e0-0x548f85: definitions in retail address order. */

struct PC_MUSIC_ACTION_PAIR_PC {
    i16 quiet_id;         // 0x00
    i16 action_id;        // 0x02
    u8 unknown_004[0x18]; // 0x04
}; // size 0x1c

extern "C" {
    extern void (*ExtraHurtSfxFn)(GAMEOBJECT *);
}
extern "C" void (*ExtraDieSfxFn)(GAMEOBJECT *);
extern "C" {
    typedef struct footstep_audio_callbacks_pc_s {
        i32 (*override_footstep)(GAMEOBJECT *, i32);
    } FOOTSTEP_AUDIO_CALLBACKS_PC;
}
// External declarations only; final shared integration must use canonical C linkage.
extern i32 (*CheckMusicOtherFn)(void);
extern i32 (*GameAudio_ActionMusicFn)(void);

struct MUSIC_PAIR_ARENA_PC {
    i32 prefix_word;
    u8 unknown_004[0x18];
    i16 action_first;
    u8 unknown_01e[0x1a];
    i16 ambient_first;
    u8 unknown_03a[2];
};

extern "C" {
    extern i32 last_currentmusicpair_quiet;
    extern i32 MusicOther;
    extern i32 sticky_attack;
}

extern "C" {

    i32 GameAudio_GetPlrSfxBits(void *object_ptr) {
        GAMEOBJECT *object = (GAMEOBJECT *)object_ptr;
        if (object != NULL && object->player_controlled) {
            return 1 << object->field_280;
        }
        return 0;
    }

    void GameAudio_Reset(void) {
        i32 i;

        GameAudio = &GameAudioStorage;
        memset(&GameAudioStorage, 0, sizeof(GameAudioStorage));
        for (i = 0; i < 84; i++) {
            GameAudioStorage.sfx_names[i] = NULL;
            GameAudioStorage.sfx_ids[i] = -1;
        }
    }

    void GameAudio_PlaySfxById(i32 sfx_id, NUVEC *position, i32 flags, i32 volume) {
        (void)volume;
        if (flags != 0) {
            if (flags == 1 || flags == 3) {
                nusound_special_positions[1] = *position;
                PlaySfxById(sfx_id, &nusound_special_positions[1]);
            }
            if (flags == 2 || flags == 3) {
                nusound_special_positions[2] = *position;
                PlaySfxById(sfx_id, &nusound_special_positions[2]);
            }
        } else {
            PlaySfxById(sfx_id, position);
        }
    }

    void GameAudio_PlaySfx(i32 sfx, NUVEC *position, i32 flags, i32 volume) {
        if ((u32)sfx <= 83) {
            GameAudio_PlaySfxById(GameAudio->sfx_ids[sfx], position, flags, volume);
        }
    }

    void GameAudio_PlaySfxAndSetVolume(i32 sfx, NUVEC *position, f32 volume) {
        if ((u32)sfx <= 0x53) {
            PlaySfxByIdAndSetVolume(GameAudio->sfx_ids[sfx], position, volume);
        }
    }

    i32 GameAudio_GetSfxId(i32 sfx) {
        if ((u32)sfx <= 0x53) {
            return GameAudio->sfx_ids[sfx];
        }
        return -1;
    }

    void GameAudio_AddSfx(i32 code, i32 *ids, i32 *count, i32 capacity) {
        i32 id;
        i32 i;
        if (ids == NULL || count == NULL || *count >= capacity) {
            return;
        }
        if ((u32)code > 83) {
            return;
        }
        id = GameAudio->sfx_ids[code];
        if (id == -1) {
            return;
        }
        for (i = 0; i < *count; ++i) {
            if (ids[i] == id) {
                return;
            }
        }
        ids[*count] = id;
        ++*count;
    }

    void GameAudio_SetActionMusicTimes(f32 initial_delay, f32 hold_time) {
        ActionMusicStickyTimeoutPc[0] = initial_delay;
        ActionMusicStickyTimeoutPc[1] = hold_time;
    }
}

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static f32 AudioFabs(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

void legoSetMusicVolume(f32 volume) {
    music_man.SetClassVolume(0x23, volume);
}

void legoSetCutVolume(f32 volume) {
    music_man.SetClassVolume(0x10, volume);
}

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

extern "C" {

    void ResetLevSfx(WORLDINFO *world) {
        i32 index;
        for (index = 0; index < 64; ++index) {
            world->level_sfx[index].id = -1;
        }
        world->level_sfx_count = 0;
    }

    void AddLevSfx(WORLDINFO *world, NUVEC *position, char *name, i32 sfx_id) {
        i32 index;

        if (sfx_id == -1 && name != NULL) {
            sfx_id = GetSfxId(name);
        }
        if (sfx_id == -1) {
            return;
        }

        for (index = 0; index < world->level_sfx_count; index++) {
            if (world->level_sfx[index].id == sfx_id) {
                break;
            }
        }
        if (index < world->level_sfx_count) {
            if (world->level_sfx[index].references < 0xff) {
                world->level_sfx[index].references++;
            }
            return;
        }
        if (world->level_sfx_count >= 64) {
            return;
        }
        if (position != NULL) {
            world->level_sfx[world->level_sfx_count].position = *position;
        } else {
            world->level_sfx[world->level_sfx_count].position = APIObjectZeroMomentumPC;
        }
        world->level_sfx[world->level_sfx_count].id = (i16)sfx_id;
        world->level_sfx[world->level_sfx_count].references = 1;
        world->level_sfx_count++;
    }

    void ChatterSfx(GAMEOBJECT *object, i32 sound, f32 interval) {
        if (TerrainChatterWaitPc <= 0.0f && ParticlesPerSecond(2.0f, FRAMETIME) > 0 && object->dead == 0 &&
            object->character_context == -1) {
            if (sound != TerrainLastChatterSfxPc || !(interval > 0.0f)) {
                PlaySfxById(sound, &object->collision_position);
                TerrainChatterWaitPc = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 2.0 + 3.0);
                TerrainLastChatterSfxPc = sound;
            } else {
                TerrainChatterWaitPc = interval;
                TerrainLastChatterSfxPc = -1;
            }
        }
    }

    /* PC 0x00547d70. */
    void LevChatterSfx(char *name, NUVEC *position) {
        if (TerrainChatterWaitPc <= 0.0f && qrand() < 0x400) {
            PlaySfx(name, position);
            TerrainChatterWaitPc = (f32)((f64)qrand() * PlayerRandomScalePc * 2.0 + NuTimeBarColumnStep);
        }
    }

    void EngineNoiseCode(GAMEOBJECT *object, i32 silent) {
        PLAYERCHARACTERCONFIG_PC *character = object->character_data->game_character;
        i32 sfx_id = character->sfx_engine;
        f32 target;
        f32 volume;
        f32 pitch;
        if (sfx_id == -1) {
            return;
        }
        target = 0.0f;
        if (silent == 0) {
            if (object->flags_1fc & 0x80) {
                target = AudioFabs(object->field_dc8);
            } else {
                target = object->velocity_magnitude / character->run_speed;
            }
            if (target > 1.0f) {
                target = 1.0f;
            }
        }
        object->engine_level = SeekLinearF(object->engine_level, target, (f32)((f64)FRAMETIME * .5));
        volume = (f32)((f64)object->engine_level * .5 + .5);
        if ((object->flags_1fc & 0x80) == 0) {
            volume = (f32)((f64)volume * (f64).6f);
        }
        pitch = (f32)((f64)object->engine_level * (f64).4f + (f64).6f);
        pitch += (f32)(((f64)object->hostility_group / 63.0) * (f64).05f - (f64).025f);
        pitch = (f32)(((f64)qrand() * (f64)1.5259022e-5f * (f64).03f + (f64).985f) * (f64)pitch);
        PlaySfxByIdAndSetVolumeAndPitch(sfx_id, &object->collision_position, volume, pitch);
    }
}

void PlayJumpSfx(GAMEOBJECT *object, i32 type) {
    i32 sfx;
    u32 flags = object->character_data->model_flags;
    if ((flags & 0x40) != 0) {
        sfx = GameAudio->sfx_ids[1];
    } else if ((flags & 8) != 0) {
        switch (type) {
            case 0:
                sfx = GameAudio->sfx_ids[2];
                break;
            case 1:
                sfx = GameAudio->sfx_ids[3];
                break;
            case 3:
                sfx = GameAudio->sfx_ids[4];
                break;
            case 4:
                sfx = GameAudio->sfx_ids[5];
                break;
            case 2:
                sfx = GameAudio->sfx_ids[6];
                break;
            default:
                return;
        }
    } else {
        sfx = GameAudio->sfx_ids[0];
    }
    if (sfx != -1) {
        if ((object->flags_1fc & 0x80) == 0 && (object->portal_flags & 0x08000000) == 0) {
            PlaySfxByIdAndSetVolume(sfx, &object->lower_position, 0.5f);
        } else {
            PlaySfxById(sfx, &object->lower_position);
        }
    }
}

extern "C" {

    void PlayLandSfx(GAMEOBJECT *object, i32 type, i32 ignored) {
        i32 sfx;
        i32 alternate;
        (void)ignored;
        if (type == 1) {
            if ((object->character_data->model_flags & 8) == 0) {
                return;
            }
            sfx = GameAudio->sfx_ids[11];
        } else if (type == 2) {
            sfx = GameAudio->sfx_ids[12];
        } else if (type == 3) {
            sfx = GameAudio->sfx_ids[13];
        } else if (type == 4) {
            sfx = GameAudio->sfx_ids[14];
        } else {
            if (object->is_underwater != 0 || object->water_intersects != 0) {
                return;
            }
            alternate = WorldInfo_CurrentlyActive()->current_level->flags & 0x1000;
            if ((object->character_data->model_flags & 0x10) != 0) {
                if (alternate) {
                    sfx = GameAudio->sfx_ids[10];
                } else {
                    sfx = GameAudio->sfx_ids[9];
                }
            } else {
                if (type != 0) {
                    return;
                }
                if (alternate) {
                    sfx = GameAudio->sfx_ids[8];
                } else {
                    sfx = GameAudio->sfx_ids[7];
                }
            }
        }
        if (sfx != -1) {
            if ((object->flags_1fc_word & 0x80) != 0 || (object->portal_flags & 0x8000000) != 0) {
                PlaySfxById(sfx, &object->lower_position);
            } else {
                PlaySfxByIdAndSetVolume(sfx, &object->lower_position, 0.5f);
            }
        }
    }

    void PlayFootStepSfx(GAMEOBJECT *object) {
        i32 on_platform;
        i8 surface;
        i32 sound;
        i32 alternate;
        CHARACTERANIM_CONFIG_PC *config;
        ANIMPACKET_PC *packet = &object->anim_packet;
        if (object->terrain_platform_id != -1 || object->supporting_platform_id != -1 ||
            ((surface = (i8)object->jump_animation_state) != -1 && (TerSurface[surface].flags & 2) != 0)) {
            on_platform = 1;
        } else {
            on_platform = 0;
        }
        if (object->character_data->game_character->sfx_footstep != -1) {
            sound = object->character_data->game_character->sfx_footstep;
        } else {
            i32 (*override_footstep)(GAMEOBJECT *, i32) = ((FOOTSTEP_AUDIO_CALLBACKS_PC *)GameAudio)->override_footstep;
            if (override_footstep == NULL || (sound = override_footstep(object, on_platform)) == -1) {
                if (object->is_underwater == 0 && object->water_intersects == 0) {
                    alternate = WorldInfo_CurrentlyActive()->current_level->flags & 0x1000;
                    if ((object->character_data->model_flags & 0x10) != 0) {
                        if (alternate) {
                            sound = GameAudio->sfx_ids[0x12];
                        } else {
                            sound = GameAudio->sfx_ids[0x11];
                        }
                    } else {
                        if (alternate) {
                            sound = GameAudio->sfx_ids[0x10];
                        } else {
                            sound = GameAudio->sfx_ids[0x0f];
                        }
                    }
                } else {
                    sound = GameAudio->sfx_ids[0x13];
                }
            }
        }
        if (sound != -1) {
            if ((object->flags_1fc_word & 0x80) != 0 || (object->portal_flags & 0x8000000) != 0) {
                PlaySfxById(sound, &object->lower_position);
            } else {
                PlaySfxByIdAndSetVolume(sound, &object->lower_position, 0.5f);
            }
        }
        if (packet->blending == 0 && packet->animation_index != -1) {
            config = object->character_model->animation_configs[packet->animation_index];
            if (config != NULL) {
                if ((config->flags & 0x20000) != 0) {
                    GameCam_Judder(GameCam, -0.25f, 0, &object->collision_position);
                    NewRumbleAllPlayers(0.6f, 0.0f, 0, 0);
                } else if ((config->flags & 0x40000) != 0 && VehicleArea == 0) {
                    GameCam_NewShake(GameCam, 0.5f, 0.5f, 1.0f);
                    NewRumbleAllPlayers(0.5f, 0.0f, 0, 0);
                }
            }
        }
    }

    void PlayGruntSfx(GAMEOBJECT *object) {
        CHARACTERDATA_PC *character = object->character_data;
        i32 sfx;
        if (character->game_character->sfx_grunt != -1) {
            sfx = character->game_character->sfx_grunt;
        } else {
            if (character->model_flags & 0x44002010) {
                return;
            }
            if (object->flags_ef4 & 0x800) {
                sfx = GameAudio->sfx_ids[0x14];
            } else {
                sfx = GameAudio->sfx_ids[0x15];
            }
        }
        if (sfx != -1) {
            PlaySfxById(sfx, &object->collision_position);
        }
    }

    void PlayHurtSfx(GAMEOBJECT *object) {
        CHARACTERDATA_PC *character = object->character_data;
        i32 sfx;
        if (character->game_character->sfx_hurt != -1) {
            sfx = character->game_character->sfx_hurt;
        } else {
            if (character->model_flags & 0x44002010) {
                goto extra;
            }
            if (object->flags_ef4 & 0x800) {
                sfx = GameAudio->sfx_ids[0x16];
            } else {
                sfx = GameAudio->sfx_ids[0x17];
            }
        }
        if (sfx != -1) {
            PlaySfxById(sfx, &object->collision_position);
        }
    extra:
        if (ExtraHurtSfxFn != NULL) {
            ExtraHurtSfxFn(object);
        }
    }
}

extern "C" void PlayDieSfx(GAMEOBJECT *object) {
    i32 sfx;
    if (object->character_data->game_character->sfx_die != -1) {
        sfx = object->character_data->game_character->sfx_die;
    } else if ((object->character_data->game_character->flags_090 & 0x800) != 0) {
        sfx = GameAudio->sfx_ids[0x18];
    } else if ((object->character_data->model_flags & 0x2000) != 0) {
        sfx = GameAudio->sfx_ids[0x18];
    } else if ((object->character_data->model_flags & 0x04000000) != 0) {
        sfx = GameAudio->sfx_ids[0x19];
    } else if ((object->character_data->model_flags & 0x10) != 0) {
        sfx = GameAudio->sfx_ids[0x1a];
    } else if ((object->character_data->model_flags & 0x40000000) != 0) {
        sfx = -1;
    } else if ((object->flags_ef4 & 0x800) != 0) {
        sfx = GameAudio->sfx_ids[0x1b];
    } else {
        sfx = GameAudio->sfx_ids[0x1c];
    }
    if (sfx != -1) {
        PlaySfxById(sfx, &object->collision_position);
    }
    if (ExtraDieSfxFn != NULL) {
        ExtraDieSfxFn(object);
    }
}

extern "C" void PlaySabreSfx(char *name, GAMEOBJECT *object, NUVEC *position, i32 unused) {
    u8 player = object->field_280;
    i32 bits = 0;
    (void)unused;
    if (player == 0xff && WORLD->rooms_visible_ptr[object->portal_room] == 0) {
        return;
    }
    if (object->flags_1fc & 0x80) {
        bits = 1 << player;
    }
    if (name != NULL) {
        i32 sfx = GetSfxId(name);
        if (position == NULL) {
            position = &object->collision_position;
        }
        PlaySfxById(sfx, position);
    } else {
        i16 sfx = object->character_data->game_character->sfx_sabre;
        if (sfx != -1) {
            GameAudio_PlaySfxById(sfx, &object->collision_position, bits, 1);
        } else {
            PlaySfxById(GameAudio->sfx_ids[0x40], &object->collision_position);
        }
    }
}

extern "C" {

    void AddLevelSfxFromName(char *name, i32 *ids, i32 *count, i32 capacity) {
        i32 id;
        i32 i;
        if (ids == NULL || count == NULL || *count >= capacity) {
            return;
        }
        id = GetSfxId(name);
        if (id == -1) {
            return;
        }
        for (i = 0; i < *count; ++i) {
            if (ids[i] == id) {
                return;
            }
        }
        ids[*count] = id;
        ++*count;
    }

    void AddLevelSfxFromId(i32 id, i32 *ids, i32 *count, i32 capacity) {
        i32 i;
        if (ids == NULL || count == NULL || *count >= capacity || id == -1) {
            return;
        }
        for (i = 0; i < *count; ++i) {
            if (ids[i] == id) {
                return;
            }
        }
        ids[*count] = id;
        ++*count;
    }

    void UpdateLevelSfx(WORLDINFO *world, i32 paused) {
        i32 (*check_reverb)(void);
        SfxBitsRestore(&CurrentSFXTAB);
        if (paused == 0 && (check_reverb = GameAudio->check_reverb_fn) != NULL && check_reverb() != 0) {
            NuSound3SetReverb(1);
        } else {
            NuSound3SetReverb(0);
        }
        if (paused == 0 && CUTSTOPGAME == 0 && world->current_level->sfx_ambient != -1) {
            PlaySfxById(world->current_level->sfx_ambient, NULL);
        }
    }
}
i32 ticktock = 0;

void TickTockSfx(void) {
    if (ticktock != 0) {
        PlaySfxById(GameAudio->sfx_ids[0x1d], NULL);
        ticktock = 0;
    } else {
        PlaySfxById(GameAudio->sfx_ids[0x1e], NULL);
        ticktock = 1;
    }
}

extern "C" void ResetRepeatSfx(void) {
    i32 i;
    memset(RepeatSfxQueue, 0, sizeof(RepeatSfxQueue));
    RepeatSfxQueueHead = 0;
    for (i = 0; i < 32; ++i) {
        RepeatSfxQueue[0].sfx_id = -1;
    }
}

void PlayRepeatSfx(char *name, i32 sfx_id, f32 start_delay, char play_count, f32 repeat_delay, NUVEC *position) {
    if (play_count == 1 && start_delay == 0.0f) {
        if (sfx_id != -1) {
            PlaySfxById(sfx_id, position);
        } else {
            PlaySfx(name, position);
        }
        return;
    }

    if (start_delay > 0.0f) {
        RepeatSfxQueue[RepeatSfxQueueHead].state = 1;
    } else {
        RepeatSfxQueue[RepeatSfxQueueHead].state = 2;
    }

    if (sfx_id == -1) {
        sfx_id = GetSfxId(name);
    }
    RepeatSfxQueue[RepeatSfxQueueHead].sfx_id = (i16)sfx_id;
    RepeatSfxQueue[RepeatSfxQueueHead].start_delay = start_delay;
    RepeatSfxQueue[RepeatSfxQueueHead].play_count = play_count;
    RepeatSfxQueue[RepeatSfxQueueHead].repeat_delay = repeat_delay;
    RepeatSfxQueue[RepeatSfxQueueHead].position = position;
    RepeatSfxQueueHead = (RepeatSfxQueueHead + 1) & 0x1f;
}

void UpdateRepeatSfx(void) {
    REPEAT_SFX_ENTRY_PC *entry = RepeatSfxQueue;
    i32 index;
    for (index = 0; index < 32; ++index, ++entry) {
        switch (entry->state) {
            case 1:
                if (entry->start_delay > 0.0f) {
                    entry->start_delay -= FRAMETIME;
                } else {
                    entry->state = 2;
                }
                break;
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
                    memset(entry, 0, sizeof(REPEAT_SFX_ENTRY_PC));
                    entry->state = 0;
                }
                break;
        }
    }
}

extern "C" void SetSoundFadeDist(WORLDINFO *world, OPTIONSSAVE *options) {
    options->sound_volume = 8;
    options->music_volume = 6;
    if (SetSoundFadeDistCallBackFn == NULL || SetSoundFadeDistCallBackFn(world) == 0) {
        if (VehicleArea != 0) {
            nusound_fade_start = 10.0f;
            nusound_fade_end = 80.0f;
        } else {
            nusound_fade_start = 2.0f;
            nusound_fade_end = 15.0f;
        }
    }
    GameSetSoundVolume(options);
    GameSetMusicVolume(options);
}

extern "C" {

    void NewAreaMusicChanges(void) {
        BeenAttacked = 0;
    }

    i32 ActionFromQuiet(i32 quiet_index) {
        i32 index;
        if (quiet_index == -1) {
            return -1;
        }
        for (index = 0; ActionPairTab[index].quiet_id != -1; ++index) {
            if (ActionPairTab[index].quiet_id == quiet_index) {
                return ActionPairTab[index].action_id;
            }
        }
        return -1;
    }

    i32 AmbientFromQuiet(i32 quiet_index) {
        i32 pair_index;
        i16 id;

        if (quiet_index == -1) {
            return -1;
        }

        for (pair_index = 0; (id = AmbientPairTab[pair_index * 2]) != -1; pair_index++) {
            if (id == quiet_index) {
                return AmbientPairTab[pair_index * 2 + 1];
            }
        }
        return -1;
    }

    i32 GetMusicIndex(char *name, struct nusound_filename_info_s *table, i32 fallback) {
        (void)name;
        (void)table;
        (void)fallback;
        return -1;
    }
}

extern "C" nusound_filename_info_s *ConfigureMusic(char *ignored_file, VARIPTR *cursor, VARIPTR *end) {
    nusound_filename_info_s *files;
    VARIPTR buffer;
    music_man.Initialise("audio\\music.cfg", NULL, cursor, *end);
    music_man.GetSoundFiles(&files, NULL);
    SfxMusicExtensionPc = ".mib";
    buffer.addr = (cursor->addr + 3) & ~3u;
    *(i32 *)buffer.void_ptr = 0;
    buffer.addr += offsetof(MUSIC_PAIR_ARENA_PC, action_first);
    *(i16 *)buffer.void_ptr = -1;
    ActionPairTab = (PC_MUSIC_ACTION_PAIR_PC *)buffer.void_ptr;
    buffer.addr += offsetof(MUSIC_PAIR_ARENA_PC, ambient_first) - offsetof(MUSIC_PAIR_ARENA_PC, action_first);
    *(i16 *)buffer.void_ptr = -1;
    AmbientPairTab = (i16 *)buffer.void_ptr;
    buffer.addr += sizeof(MUSIC_PAIR_ARENA_PC) - offsetof(MUSIC_PAIR_ARENA_PC, ambient_first);
    *cursor = buffer;
    return files;
}

extern "C" void GamePlayMusic(LEVELDATA_PC *level, i32 check, OPTIONSSAVE *options) {
    i32 other = MusicOther;
    i32 music_other;

    last_currentmusicpair_quiet = CurrentMusicPair_Quiet;
    MusicOther = 0;
    if (NOSOUND != 0 || NOMUSIC != 0) {
        return;
    }
    if (CheckMusicOtherFn != NULL) {
        music_other = CheckMusicOtherFn();
    } else {
        music_other = 0;
    }
    MusicOther = music_other;
    if (check != 0) {
        if (music_other == other) {
            return;
        }
    } else {
        sticky_attack_time = 0.0f;
        sticky_attack = MusicActiveReset;
    }

    music_man.SelectTrackByHandle(1, level->music_tracks[music_other]);
    music_man.SelectTrackByHandle(2, level->music_tracks[MusicOther + 2]);
    music_man.SelectTrackByHandle(0x20, level->music_tracks[MusicOther + 4]);

    if (options->music_enabled == 0) {
        music_man.PlayTrack(0x20);
        return;
    }
    if (sticky_attack != 0) {
        if (music_man.GetTrackHandle(2, NULL) != -1) {
            music_man.PlayTrack(2);
        } else if (music_man.GetTrackHandle(1, NULL) != -1) {
            music_man.PlayTrack(1);
        } else {
            music_man.PlayTrack(0x20);
        }
    } else {
        if (music_man.GetTrackHandle(1, NULL) != -1) {
            music_man.PlayTrack(1);
        } else if (music_man.GetTrackHandle(2, NULL) != -1) {
            music_man.PlayTrack(2);
        } else {
            music_man.PlayTrack(0x20);
        }
    }
}

extern "C" i32 fun_00548c30() {
    return 0;
}

extern "C" void ProcessMusicChanges(LEVELDATA_PC *level, OPTIONSSAVE *options) {
    if (GameAudio_ActionMusicFn != NULL) {
        MusicActiveReset = GameAudio_ActionMusicFn();
    } else {
        if (DoubleScore != 0) {
            MusicActiveReset = 1;
        } else if (Cheat_PowerUpActive(-1) != 0) {
            MusicActiveReset = 1;
        } else if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->ai_packet.opponent != NULL ||
                                         (((GAMEOBJECT *)Player[0])->ai_packet.nearest_opponent != NULL &&
                                          ((GAMEOBJECT *)Player[0])->ai_packet.nearest_opponent->dead == 0 &&
                                          ((GAMEOBJECT *)Player[0])->ai_packet.nearest_opponent_metric < 3.0f))) {
            MusicActiveReset = 1;
        } else if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->ai_packet.opponent != NULL ||
                                         (((GAMEOBJECT *)Player[1])->ai_packet.nearest_opponent != NULL &&
                                          ((GAMEOBJECT *)Player[1])->ai_packet.nearest_opponent->dead == 0 &&
                                          ((GAMEOBJECT *)Player[1])->ai_packet.nearest_opponent_metric < 3.0f))) {
            MusicActiveReset = 1;
        } else {
            MusicActiveReset = 0;
        }
    }
    if (CheckMusicOtherFn != NULL) {
        MusicOther = CheckMusicOtherFn();
    } else {
        MusicOther = 0;
    }
    if (Paused == 0 && sticky_attack_time > 0.0f) {
        sticky_attack_time -= FRAMETIME;
    }
    if (sticky_attack != MusicActiveReset && sticky_attack_time <= 0.0f) {
        sticky_attack = MusicActiveReset;
        sticky_attack_time = ActionMusicStickyTimeoutPc[MusicActiveReset];
    }
    if (CutStopInfo != NULL) {
        CUTINFO *cut = (CUTINFO *)CutStopInfo;
        if (cut->skip_level != -1 && (LDataList[cut->skip_level].flags & 1) != 0) {
            level = &LDataList[cut->skip_level];
        }
    }
    music_man.SelectTrackByHandle(1, level->music_tracks[MusicOther]);
    music_man.SelectTrackByHandle(2, level->music_tracks[MusicOther + 2]);
    music_man.SelectTrackByHandle(0x20, level->music_tracks[MusicOther + 4]);
    if (options->music_enabled == 0) {
        music_man.PlayTrack(0x20);
    } else if (sticky_attack != 0) {
        if (music_man.GetTrackHandle(2, NULL) != -1) {
            music_man.PlayTrack(2);
        } else if (music_man.GetTrackHandle(1, NULL) != -1) {
            music_man.PlayTrack(1);
        } else {
            music_man.PlayTrack(0x20);
        }
    } else {
        if (music_man.GetTrackHandle(1, NULL) != -1) {
            music_man.PlayTrack(1);
        } else if (music_man.GetTrackHandle(2, NULL) != -1) {
            music_man.PlayTrack(2);
        } else {
            music_man.PlayTrack(0x20);
        }
    }
    music_man.Process(FRAMETIME);
}

extern "C" void MusicClearAll() {
    MusicPlrsUnderAttack = 0;
    MusicActiveReset = 0;
    MusicActive = -1;
    sticky_attack_time = 0.0f;
    MusicSwapDelay = 0.0f;
    MusicSeekIndex = 0;
    PlayersUnderAttack = 0;
    MusicInside = 0;
    MusicFindCut = 0;
    MusicRestoreTrack = 0;
    MusicOnFlag = 1;
    MusicPlrsHoldAttack = 0;
    CurrentMusicPair_Quiet = -1;
}

extern "C" {

    void GameAudio_Init(GAMEAUDIO_PC *audio) {
        i32 i;
        GameAudio = audio;
        for (i = 0; i < 84; ++i) {
            GameAudio->sfx_ids[i] = (i16)GetSfxId(GameAudio->sfx_names[i]);
        }
        MenuRegisterSoundFX(GameAudio->sfx_ids[0x2f], GameAudio->sfx_ids[0x30], GameAudio->sfx_ids[0x31],
                            GameAudio->sfx_ids[0x32]);
    }
}
