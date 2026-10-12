#include "saga.h"

extern NUVEC nusound_special_positions[5];
extern f32 nusound_fade_start;
extern f32 nusound_fade_end;
extern u32 nusound_seed;
extern NUMTX *NuSound3Listener;

NUMTX *__cdecl NuSound3GetListener(void) {
    return NuSound3Listener != NULL ? NuSound3Listener : &global_camera.mtx;
}

void __cdecl PlaySfx(char *name, NUVEC *position) {
    i32 sfx_id = GetSfxId(name);
    if (sfx_id != -1) {
        PlaySfxByIdEx(sfx_id, position, 1.0f, 1.0f);
    }
}

void __cdecl PlaySfxAndSetPitch(char *name, NUVEC *position, f32 pitch) {
    i32 sfx_id = GetSfxId(name);
    if (sfx_id != -1) {
        PlaySfxByIdEx(sfx_id, position, 1.0f, pitch);
    }
}

void PlaySfxAndSetVolumeAndPitch(char *name, NUVEC *position, f32 volume, f32 pitch) {
    i32 sfx_id = GetSfxId(name);
    if (sfx_id != -1) {
        PlaySfxByIdEx(sfx_id, position, volume, pitch);
    }
}

void __cdecl PlaySfxAndSetVolume(char *name, NUVEC *position, f32 volume) {
    i32 sfx_id = GetSfxId(name);
    if (sfx_id != -1) {
        PlaySfxByIdEx(sfx_id, position, volume, 1.0f);
    }
}

void __cdecl PlaySfxById(i32 sfx_id, NUVEC *position) {
    PlaySfxByIdEx(sfx_id, position, 1.0f, 1.0f);
}

void __cdecl PlaySfxByIdAndSetVolume(i32 sfx_id, NUVEC *position, f32 volume) {
    PlaySfxByIdEx(sfx_id, position, volume, 1.0f);
}

void __cdecl AddLevSfx(WORLDINFO *world, NUVEC *position, char *name, i32 sfx_id) {
    i32 index;

    if (sfx_id == -1 && name != NULL) {
        sfx_id = GetSfxId(name);
    }
    if (sfx_id == -1) {
        return;
    }

    index = 0;
    while (index < world->level_sfx_count && world->level_sfx[index].id != sfx_id) {
        index++;
    }
    if (index < world->level_sfx_count) {
        if (world->level_sfx[index].references != 0xff) {
            world->level_sfx[index].references++;
        }
        return;
    }
    if (index >= 64) {
        return;
    }

    if (position != NULL) {
        world->level_sfx[index].position = *position;
    } else {
        world->level_sfx[index].position.x = 0.0f;
        world->level_sfx[index].position.y = 0.0f;
        world->level_sfx[index].position.z = 0.0f;
    }
    world->level_sfx[index].id = (i16)sfx_id;
    world->level_sfx[index].references = 1;
    world->level_sfx_count = index + 1;
}

void __cdecl UpdateLevelSfx(WORLDINFO *world, i32 paused) {
    SfxBitsRestore(&CurrentSFXTAB);

    if (paused != 0) {
        NuSound3SetReverb(0);
        return;
    }
    if (GameAudio->check_reverb_fn != NULL && GameAudio->check_reverb_fn() != 0) {
        NuSound3SetReverb(1);
    } else {
        NuSound3SetReverb(0);
    }

    if (CUTSTOPGAME == 0 && world->current_level->sfx_ambient != -1) {
        GameAudio_PlaySfxById(world->current_level->sfx_ambient, NULL, 0, 0);
    }
}

void __cdecl GameAudio_PlaySfxById(i32 sfx_id, NUVEC *position, i32 flags, i32 volume) {
    (void)volume;
    if (flags == 0) {
        PlaySfxById(sfx_id, position);
        return;
    }
    if ((flags & ~2) == 1) {
        nusound_special_positions[1] = *position;
        PlaySfxById(sfx_id, &nusound_special_positions[1]);
    }
    flags -= 2;
    if ((u32)flags <= 1) {
        nusound_special_positions[2] = *position;
        PlaySfxById(sfx_id, &nusound_special_positions[2]);
    }
}

void __cdecl GameAudio_PlaySfx(i32 sfx, NUVEC *position, i32 flags, i32 volume) {
    if ((u32)sfx < 0x54) {
        GameAudio_PlaySfxById(GameAudio->sfx_ids[sfx], position, flags, volume);
    }
}

i32 __cdecl GroupBuffer_MakeGroup(i32 sample_id) {
    i32 group_id = NuSound3GroupCount;
    i32 sample_index;

    if (group_id == 0x80 || NuSound3GroupSampleCount == 0x200) {
        return -1;
    }

    sample_index = NuSound3GroupSampleCount;
    NuSound3Groups[group_id].first_sample = (i16)sample_index;
    NuSound3Groups[group_id].sample_count = 1;
    NuSound3Groups[group_id].cursor = 0;
    SfxInfo[sample_id].group = (i16)group_id;
    NuSound3GroupSampleIds[sample_index] = (i16)sample_id;
    NuSound3GroupSampleCount = sample_index + 1;
    NuSound3GroupCount = group_id + 1;
    return group_id;
}

i32 __cdecl GroupBuffer_GetSample(i32 group_id, i32 sequential) {
    SFX_GROUP_PC *group = &NuSound3Groups[group_id];
    i32 first_sample = group->first_sample;
    i32 sample_count = group->sample_count;

    if (sequential != 0) {
        i32 cursor = (i16)group->cursor;
        i32 next_cursor = (u16)group->cursor + 1;
        group->cursor = (i16)next_cursor;
        if ((i16)group->cursor >= sample_count) {
            group->cursor = 0;
        }
        return NuSound3GroupSampleIds[first_sample + cursor];
    }

    if (sample_count == 0) {
        sample_count = 1;
    }
    return NuSound3GroupSampleIds[first_sample + ((NuRandIntSeeded(&nusound_seed) >> 16) % sample_count)];
}

void __cdecl AudioGroup_Parse(NUFPAR_PC *parser) {
    i32 group_id = -1;
    i32 first = 1;

    while (NuFParGetWord(parser) != 0) {
        i32 sample_id = GetSfxId(parser->word_buffer_ptr);

        if (first != 0) {
            if (sample_id == -1) {
                return;
            }
            group_id = GroupBuffer_MakeGroup(sample_id);
            first = 0;
        } else {
            sample_id = GetSfxId(parser->word_buffer_ptr);
            if (sample_id != -1 && NuSound3GroupSampleCount != 0x200) {
                NuSound3GroupSampleIds[NuSound3GroupSampleCount] = (i16)sample_id;
                NuSound3GroupSampleCount++;
                NuSound3Groups[group_id].sample_count++;
                NuSound3Groups[group_id].cursor = 0;
            }
        }
    }
}

void __cdecl PlaySfxByIdEx(i32 sfx_id, NUVEC *position, f32 volume, f32 pitch) {
    SFX_RECORD_PC *record;
    const NUMTX *listener;
    i8 priority;
    i32 sample_id;
    i32 voice_volume;
    i32 volume_left;
    i32 volume_right;
    i32 loop;
    f32 pan;
    f32 falloff_near;
    f32 falloff_far;
    f32 saved_fade_start;
    f32 saved_fade_end;
    f32 volume_scale;
    i32 changed_fade;

    if (sfx_id == -1) {
        return;
    }

    record = &SfxInfo[sfx_id];
    if ((record->sample_id_flags & 0x000c0000u) != 0) {
        return;
    }

    priority = record->priority;
    if (record->group != -1) {
        sample_id = GroupBuffer_GetSample(record->group, (record->sample_id_flags >> 16) & 1);
        record = &SfxInfo[sample_id];
    }

    sample_id = (i32)(record->sample_id_flags & 0x7fffu);
    loop = (record->sample_id_flags & 0x8000u) != 0;
    pan = record->pan;
    falloff_near = record->falloff_near;
    falloff_far = record->falloff_far;
    saved_fade_start = 0.0f;
    saved_fade_end = 0.0f;
    changed_fade = 0;

    if (record->falloff_near != 0.0f || record->falloff_far != 0.0f) {
        saved_fade_start = nusound_fade_start;
        saved_fade_end = nusound_fade_end;
        nusound_fade_start = falloff_near * saved_fade_start * 0.5f;
        nusound_fade_end = falloff_far * saved_fade_end / 15.0f;
        changed_fade = 1;
    } else {
        falloff_near = 2.0f;
        falloff_far = 15.0f;
    }

    listener = NuSound3GetListener();
    if (listener == NULL) {
        goto restore_fade;
    }
    if (position != NULL) {
        f32 dx = position->x - listener->m30;
        f32 dy = position->y - listener->m31;
        f32 dz = position->z - listener->m32;
        f32 max_distance = nusound_fade_end;
        if (dx * dx + dy * dy + dz * dz > max_distance * max_distance) {
            goto restore_fade;
        }
    }

    if (volume != 1.0f) {
        if (volume > 1.0f) {
            volume = 1.0f;
        }
        volume_scale = (f32)record->volume * volume;
    } else {
        volume_scale = (f32)record->volume;
    }
    if ((record->sample_id_flags & 0x00400000u) == 0) {
        volume_scale *= AUDIOFADELEVEL;
        volume_scale *= numusicGetDuckVolume();
    }
    voice_volume = (i32)(volume_scale * MASTERVOLUME);

    if (record->pitch_rnd != 0.0f) {
        f32 pitch_variation = NuRandFloatSeeded(&nusound_seed) * record->pitch_rnd;
        if ((NuRandIntSeeded(&nusound_seed) & 1u) == 0) {
            pitch_variation *= 0.5f;
            pitch *= 1.0f - pitch_variation;
        } else {
            pitch *= 1.0f + pitch_variation;
        }
    }
    if (record->volume_rnd != 0.0f) {
        voice_volume = (i32)((f32)voice_volume * (1.0f + NuRandFloatSeeded(&nusound_seed) * record->volume_rnd));
    }

    if ((u32)sample_id <= 1599u) {
        if (position != NULL) {
            STUBBED_NAMED("dispatch positioned SFX to loop, standard, or priority backend");
        } else {
            volume_left = pan < 0.0f ? voice_volume : (i32)((1.0f - pan) * voice_volume);
            volume_right = pan > 0.0f ? voice_volume : (i32)((1.0f + pan) * voice_volume);
            if (loop != 0) {
                NUVEC listener_position;
                NUVEC forward;
                listener_position.x = listener->m30;
                listener_position.y = listener->m31;
                listener_position.z = listener->m32;
                forward.x = listener->m20;
                forward.y = listener->m21;
                forward.z = listener->m22;
                NuVecAdd(&listener_position, &listener_position, &forward);
                STUBBED_NAMED("dispatch listener-relative looped SFX to 3D backend");
            } else {
                STUBBED_NAMED("dispatch unpositioned SFX to standard or priority backend");
            }
        }
    }

restore_fade:
    if (changed_fade != 0) {
        nusound_fade_start = saved_fade_start;
        nusound_fade_end = saved_fade_end;
    }
}

void __cdecl SfxSetFileTable(SFX_FILE_INFO_PC *records) {
    i32 index = 0;

    SfxFileTable = records;
    SfxFileCount = 0;
    if (records == NULL) {
        return;
    }

    while (records[index].filename != NULL) {
        NuStrLen(records[index].filename);
        index++;
        SfxFileCount++;
    }
}

void __cdecl SfxBuildCRCTable(VARIPTR *buffer_cursor, VARIPTR end) {
    u32 *table;
    i32 index;

    if (SfxCRCTableInitialized != 0) {
        return;
    }

    buffer_cursor->addr = (buffer_cursor->addr + 3) & ~3u;
    table = (u32 *)buffer_cursor->void_ptr;
    SfxCRCTable = table;
    buffer_cursor->addr += 0x400;

    for (index = 0; index < 256; index++) {
        u32 value = (u32)index << 24;
        i32 bit;

        for (bit = 0; bit < 8; bit++) {
            if ((value & 0x80000000) != 0) {
                value = (value << 1) ^ 0x04c11db7;
            } else {
                value <<= 1;
            }
        }
        table[index] = value;
    }
    SfxCRCTableInitialized = 1;
}

u32 __cdecl SfxNameHash(const char *name) {
    u32 hash = 0;
    i32 index = 0;

    while (name[index] != '\0') {
        u8 character = (u8)name[index];
        i32 table_index;

        if (character >= 'a' && character <= 'z') {
            character = (u8)(character - ('a' - 'A'));
        }
        table_index = (i8)character ^ (i32)(hash >> 24);
        hash = (hash << 8) ^ SfxCRCTable[table_index];
        index++;
    }
    return hash;
}

u32 __cdecl SfxNameHashN(const char *name, u32 max_length) {
    u32 hash = 0;
    u32 index = 0;

    while (name[index] != '\0' && index < max_length) {
        u8 character = (u8)name[index];
        i32 table_index;

        if (character >= 'a' && character <= 'z') {
            character = (u8)(character - ('a' - 'A'));
        }
        table_index = (i8)character ^ (i32)(hash >> 24);
        hash = (hash << 8) ^ SfxCRCTable[table_index];
        index++;
    }
    return hash;
}

i32 __cdecl GetSfxId(char *name) {
    i32 index;

    if (name == NULL || SfxHashHeads == NULL) {
        return -1;
    }

    index = SfxHashHeads[SfxNameHash(name) & 0xff];
    while (index != -1) {
        SFX_RECORD_PC *record = &SfxInfo[index];
        if (NuStrNICmp(name, record->name, 0x20) == 0) {
            return index;
        }
        index = record->next;
    }
    return -1;
}

void __cdecl SetSfxBit_OnEx(i32 sfx_id) {
    u32 index = (u32)sfx_id;

    if (index > 0x63f) {
        return;
    }
    SfxBits[index >> 4] |= (u16)(1u << (index & 0xf));
}

void __cdecl SetSfxBit_OffEx(i32 sfx_id) {
    u32 index = (u32)sfx_id;

    if (index > 0x63f) {
        return;
    }
    SfxBits[index >> 4] &= (u16) ~(1u << (index & 0xf));
}

i32 __cdecl SfxBitEx(i32 sfx_id) {
    u32 index = (u32)sfx_id;

    if (index > 0x63f) {
        return -1;
    }
    return (SfxBits[index >> 4] & (u16)(1u << (index & 0xf))) != 0;
}

void __cdecl SetSfxBit_On(i32 sound_id) {
    i32 sample_id;

    if (sound_id < 0) {
        return;
    }
    sample_id = (i32)(SfxInfo[sound_id].sample_id_flags << 17) >> 17;
    if ((u32)sample_id <= 0x63f) {
        SetSfxBit_OnEx(sample_id);
    }
}

i32 __cdecl AmbientFromQuiet(i32 quiet_index) {
    i32 pair_index;

    if (quiet_index == -1) {
        return -1;
    }

    for (pair_index = 0; AmbientPairTab[pair_index * 2] != -1; pair_index++) {
        if (AmbientPairTab[pair_index * 2] == quiet_index) {
            return AmbientPairTab[pair_index * 2 + 1];
        }
    }
    return -1;
}

void __cdecl SfxBitsStore(SoundTable *sound_table) {
    PCByteCopy_candidate(sound_table, SfxBits, 0xc8);
}

void __cdecl SfxBitsRestore(SoundTable *sound_table) {
    PCByteCopy_candidate(SfxBits, sound_table, 0xc8);
}

void __cdecl SoundStopMusic(void) {
    if (NOSOUND == 0 && NOMUSIC == 0) {
        NuSound3StopStereoStream(0);
        NuSound3StopStereoStream(1);

        Music.transition = 0.0f;
        Music.state = 0;
        Music.requested_track = -1;
        Music.current_track = -1;
        Music.queued_track = -1;
        Music.primary_stream = 0;
        Music.transition_frames = 0;
        Music.pause_requested = 0;
        Music.field_0012 = 0;
        Music.field_0013 = 0;
        Music.update_delay = 0;
        Music.restore_requested = 0;
        Music.resume_track = -1;
    }
}

void __cdecl SoundKillAll(void) {
    SoundStopMusic();
    NuSound3FlushLoops();
    NuSound3KillAllAudio();
    NuSound3Update();
}

void __cdecl ResetLevSfx(WORLDINFO *world) {
    i32 index;
    for (index = 0; index < 64; ++index) {
        world->level_sfx[index].id = -1;
    }
    world->level_sfx_count = 0;
}

void __cdecl ClearLinkedCutSceneMusic(void *context) {
    if (context == NULL || Music.track_data == context) {
        Music.track_data = NULL;
    }
}

void __cdecl ResetSounds(void) {
    PCByteCopy_candidate(SfxBits, GlobalSfxBits, 0xc8);
}

i16 __cdecl GetMusicIndex(char *name, struct nusound_filename_info_s *table, i32 fallback) {
    (void)name;
    (void)table;
    (void)fallback;
    return -1;
}

void __cdecl UpdateBackgroundMusic(void) {
}

void SetSoundVolume(f32 volume) {
    MASTERVOLUME = volume;
}

f32 GetSoundVolume(void) {
    return MASTERVOLUME;
}

void __cdecl NuSound3AddRumble(void *pad, f32 strength, i32 duration, i32 priority, f32 buzz) {
    (void)pad;
    (void)strength;
    (void)duration;
    (void)priority;
    (void)buzz;
}

void __cdecl NuSound3SetRumblePads(void *pad0, void *pad1) {
    (void)pad0;
    (void)pad1;
}

void __cdecl NuSound3SetDat(NUDATHDR *header) {
    NuSoundDat = header;
}

i32 __cdecl NuSound3SetReverb(i32 enable) {
    i32 previous = NuSound3Reverb;
    NuSound3Reverb = enable != 0;
    return previous;
}

void NuSound3SetLoopHoldTime(f32 time) {
    NuSound3LoopHoldTime = (i32)(time * NuScale1000);
}

void __cdecl NuSound3StopRumble(void) {
}

f32 __cdecl PCSoundVolumeLookup(i32 index) {
    /* Retail checks only the upper bound; preserve its negative-index behavior. */
    if (index > 0x3fff) {
        index = 0;
    }

    return NuSound3VolumeTable[index];
}

HRESULT __cdecl PCSoundSetRawVolume(IDirectSoundBuffer *buffer, i32 volume) {
    volume += SoundVoiceVolumeOffset;
    if (volume > 0) {
        volume = 0;
    }
    if (volume < -10000) {
        volume = -10000;
    }

    return buffer->vtable->set_volume(buffer, volume);
}

HRESULT __cdecl PCSoundSetLinearVolume(IDirectSoundBuffer *buffer, f32 volume) {
    i32 raw_volume = (i32)volume + SoundVoiceVolumeOffset;

    if (raw_volume > 0) {
        raw_volume = 0;
    }
    if (raw_volume < -10000) {
        raw_volume = -10000;
    }

    return buffer->vtable->set_volume(buffer, raw_volume);
}

void __cdecl SetSoundBitsById(const i32 *ids, void(__cdecl *set_bit)(i32)) {
    i32 id;
    while ((id = *ids++) != -1) {
        i32 group, i, samples;
        if (id < 0)
            continue;
        set_bit(id);
        group = SfxInfo[id].group;
        if (group == -1)
            continue;
        samples = NuSound3Groups[group].sample_count;
        for (i = 0; i < samples; ++i)
            set_bit(NuSound3GroupSampleIds[NuSound3Groups[group].first_sample + i]);
    }
}

struct PC_MUSIC_ACTION_PAIR_PC {
    i16 quiet_id;
    i16 action_id;
    u8 unknown_004[0x18];
};
STATIC_ASSERT(offsetof(PC_MUSIC_ACTION_PAIR_PC, quiet_id) == 0);
STATIC_ASSERT(offsetof(PC_MUSIC_ACTION_PAIR_PC, action_id) == 2);
STATIC_ASSERT(sizeof(PC_MUSIC_ACTION_PAIR_PC) == 0x1c);
i32 __cdecl ActionFromQuiet(i32 quiet_index) {
    i32 index;
    if (quiet_index == -1)
        return -1;
    for (index = 0; ActionPairTab[index].quiet_id != -1; ++index) {
        if (ActionPairTab[index].quiet_id == quiet_index)
            return ActionPairTab[index].action_id;
    }
    return -1;
}

i32 __cdecl GetSfxIdN(char *name, i32 length) {
    i32 index;
    for (index = 0; index < 1600; ++index) {
        if (NuStrNICmp(name, SfxInfo[index].name, length) == 0)
            return index;
    }
    TerrainDiagnosticNoOpPC(name);
    return -1;
}

extern i32 NuSound3LoadingSfxPc;
i32 __cdecl NuSound3LoadingSfx(void) {
    return NuSound3LoadingSfxPc != 0;
}

void NuSound3CancelCheckStereo(void) {
}

STATIC_ASSERT(offsetof(GAMEAUDIO_PC, sfx_ids) == 0x158);
STATIC_ASSERT(sizeof(((GAMEAUDIO_PC *)0)->sfx_ids[0]) == 2);

i32 __cdecl GameAudio_GetSfxId(i32 sfx) {
    if ((u32)sfx <= 0x53)
        return GameAudio->sfx_ids[sfx];
    return -1;
}

/* Integration fragment: declarations below must precede callback definitions. */
void __cdecl InitSoundInfo(i32);
void __cdecl LoadSfx(const char *, VARIPTR *, VARIPTR);
static void fnAudioSample(NUFPAR_PC *fpar) {
    SFX_FILE_INFO_PC *finfo = NULL;
    char *str;
    u32 hash;
    i16 *id;
    i32 i;
    u32 filename_index;
    f32 pan, pitch_rnd, volume_rnd, falloff_near, falloff_far;
    f32 buzz_timer, rumble_sustain, rumble_release;
    i32 priority, volume, rumble_strength, category;
    InitSoundInfo(NumSfxInst);
    while (1) {
        if (NuFParGetWord(fpar) == 0) {
            SFX_RECORD_PC *sinfo = &SfxInfo[NumSfxInst];
            sinfo->sample_id_flags &= ~0x100000u;
            sinfo->sample_id_flags |= 0x200000u;
            PCByteCopy_candidate(&SfxRevertInfoPc[NumSfxInst], sinfo, sizeof(SFX_RECORD_PC));
            NumSfxInst++;
            return;
        }

        if (NuStrICmp(fpar->word_buffer_ptr, "disable") == 0) {
            SfxInfo[NumSfxInst].sample_id_flags |= 0x40000u;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "comment") == 0) {
            SfxInfo[NumSfxInst].sample_id_flags |= 0x80000u;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "name") == 0) {
            NuFParGetWord(fpar);
            NuStrNCpy(SfxNameSlotsPc[NumSfxInst], fpar->word_buffer_ptr, 0x20);

            str = SfxNameSlotsPc[NumSfxInst];
            SfxInfo[NumSfxInst].name = str;
            hash = SfxNameHash(str);

            id = &SfxHashHeads[hash & 0xff];
            if (*id == -1) {
                *id = NumSfxInst;
                SfxInfo[NumSfxInst].next = -1;
            } else {
                SfxInfo[NumSfxInst].next = *id;
                *id = NumSfxInst;
            }
        } else if (NuStrICmp(fpar->word_buffer_ptr, "fname") == 0) {
            NuFParGetWord(fpar);
            NuStrCat(fpar->word_buffer_ptr, NuFileExt_wav_PC);

            if ((SfxInfo[NumSfxInst].sample_id_flags & 0xc0000u) == 0) {
                i = 0;
                for (; i < NumSfx; i++) {
                    if (NuStrICmp(SfxFileInfoPc[i].filename, fpar->word_buffer_ptr) == 0) {
                        finfo = &SfxFileInfoPc[i];
                        SfxInfo[NumSfxInst].sample_id_flags =
                            (SfxInfo[NumSfxInst].sample_id_flags & ~0x7fffu) | ((i) & 0x7fff);
                        SfxInfo[NumSfxInst].filename = finfo->filename;
                        SfxRefCountsPc[i]++;
                        break;
                    }
                }
                if (i == NumSfx) {
                    finfo = &SfxFileInfoPc[i];
                    finfo->filename = SfxFilenameSlotsPc[NumSfxNames];
                    finfo->pitch = 0x75a;
                    finfo->static_sample = 0;
                    finfo->index = i + 0x1000 - SfxFileCount;
                    NuStrNCpy(SfxFilenameSlotsPc[NumSfxNames++], fpar->word_buffer_ptr, 0x40);
                    SfxRefCountsPc[NumSfx] = 1;
                    SfxInfo[NumSfxInst].filename = SfxFileInfoPc[NumSfx].filename;
                    SfxInfo[NumSfxInst].sample_id_flags =
                        (SfxInfo[NumSfxInst].sample_id_flags & ~0x7fffu) | ((NumSfx) & 0x7fff);
                    NumSfx++;
                }
            } else {
                SfxInfo[NumSfxInst].sample_id_flags =
                    (SfxInfo[NumSfxInst].sample_id_flags & ~0x7fffu) | ((-1) & 0x7fff);
                i = 0;
                for (; i < NumSfx; i++) {
                    if (NuStrICmp(SfxFileInfoPc[i].filename, fpar->word_buffer_ptr) == 0) {
                        SfxRefCountsPc[i]++;
                        SfxInfo[NumSfxInst].filename = SfxFileInfoPc[i].filename;
                        break;
                    }
                }
                if (i == NumSfx) {
                    filename_index = NumSfxNames++;
                    NuStrNCpy(SfxFilenameSlotsPc[filename_index], fpar->word_buffer_ptr, 0x40);
                    SfxInfo[NumSfxInst].filename = SfxFilenameSlotsPc[NumSfxNames - 1];
                }
            }
        } else if (NuStrICmp(fpar->word_buffer_ptr, "pitch") == 0) {
            SfxInfo[NumSfxInst].pitch = NuFParGetInt(fpar);
        } else if (NuStrICmp(fpar->word_buffer_ptr, "pan") == 0) {
            pan = NuFParGetFloat(fpar);
            SfxInfo[NumSfxInst].pan = pan < -1.0f ? -1.0f : pan > 1.0f ? 1.0f : pan;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "pri") == 0) {
            priority = NuFParGetInt(fpar);
            SfxInfo[NumSfxInst].priority = priority < -128 ? -128 : priority > 127 ? 127 : priority;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "loop") == 0) {
            SfxInfo[NumSfxInst].sample_id_flags |= 0x8000u;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "pitch_rnd") == 0) {
            pitch_rnd = NuFParGetFloat(fpar);
            SfxInfo[NumSfxInst].pitch_rnd = pitch_rnd < 0.0f ? 0.0f : pitch_rnd > 1.0f ? 1.0f : pitch_rnd;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "volume") == 0) {
            volume = NuFParGetInt(fpar);
            SfxInfo[NumSfxInst].volume = volume < 0 ? 0 : volume > 0x3fff ? 0x3fff : volume;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "nofade") == 0) {
            SfxInfo[NumSfxInst].sample_id_flags |= 0x400000u;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "volume_rnd") == 0) {
            volume_rnd = NuFParGetFloat(fpar);
            SfxInfo[NumSfxInst].volume_rnd = volume_rnd < -1.0f ? -1.0f : volume_rnd > 0.0f ? 0.0f : volume_rnd;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "near") == 0) {
            falloff_near = NuFParGetFloat(fpar);
            SfxInfo[NumSfxInst].falloff_near = falloff_near < 0.0f     ? 0.0f
                                               : falloff_near > 250.0f ? 250.0f
                                                                       : falloff_near;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "far") == 0) {
            falloff_far = NuFParGetFloat(fpar);
            SfxInfo[NumSfxInst].falloff_far = falloff_far < 0.0f ? 0.0f : falloff_far > 250.0f ? 250.0f : falloff_far;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "global") == 0) {
            SfxInfo[NumSfxInst].sample_id_flags |= 0x20000u;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "rumble") == 0) {
            buzz_timer = NuFParGetFloat(fpar);
            rumble_strength = NuFParGetInt(fpar);
            SfxInfo[NumSfxInst].buzz_timer = buzz_timer < 0.0f ? 0.0f : buzz_timer > 5.0f ? 5.0f : buzz_timer;
            SfxInfo[NumSfxInst].rumble_strength = rumble_strength < 0      ? 0
                                                  : rumble_strength > 0xff ? 0xff
                                                                           : rumble_strength;
            rumble_sustain = NuFParGetFloat(fpar);
            SfxInfo[NumSfxInst].rumble_sustain = rumble_sustain < 0.0f   ? 0.0f
                                                 : rumble_sustain > 5.0f ? 5.0f
                                                                         : rumble_sustain;
            rumble_release = NuFParGetFloat(fpar);
            SfxInfo[NumSfxInst].rumble_release = rumble_release < 0.0f   ? 0.0f
                                                 : rumble_release > 5.0f ? 5.0f
                                                                         : rumble_release;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "fcat") == 0) {
            category = NuFParGetInt(fpar);
            SfxInfo[NumSfxInst].category = category < 0 ? 0 : category > 0xffff ? 0xffff : category;
        }
    }
}

static void __cdecl fnAudioAudio(NUFPAR_PC *parser) {
    SfxAudioVersionPc = NuFParGetFloat(parser);
}
static NUFPCOMJMP audioComPc[] = {
    {"Audio", fnAudioAudio}, {"Sample", fnAudioSample}, {"Group", AudioGroup_Parse}, {NULL, NULL}};
void __cdecl InitSoundInfo(i32 index) {
    SFX_RECORD_PC *info = &SfxInfo[index];
    info->sample_id_flags |= 0x7fff;
    info->name = SfxNameSlotsPc[NumSfxInst];
    info->pitch = 0x5622;
    info->priority = 0;
    info->pan = 0.0f;
    info->sample_id_flags &= ~0x400000u;
    info->volume = 0x3fff;
    info->pitch_rnd = 0.0f;
    info->volume_rnd = 0.0f;
    info->group = -1;
    info->sample_id_flags &= ~0x8000u;
    info->sample_id_flags &= ~0x10000u;
    info->sample_id_flags &= ~0x20000u;
    info->sample_id_flags &= ~0x40000u;
    info->sample_id_flags &= ~0x100000u;
    info->falloff_near = 0.0f;
    info->falloff_far = 0.0f;
    info->buzz_timer = 0.0f;
    info->rumble_strength = 0;
    info->rumble_sustain = 0.0f;
    info->rumble_release = 0.0f;
    info->category = 0;
    info->sample_id_flags &= ~0x200000u;
    SfxRevertInfoPc[index] = *info;
}
void __cdecl LoadSfx(const char *file, VARIPTR *cursor, VARIPTR end) {
    NUFPAR_PC *parser = NuFParCreate((char *)file);
    SFX_FILE_INFO_PC *last;
    if (parser != NULL) {
        NuFParPushCom(parser, audioComPc);
        while (NuFParGetLine(parser)) {
            NuFParGetWord(parser);
            if (*parser->word_buffer_ptr != 0)
                NuFParInterpretWord(parser);
        }
        NuFParDestroy(parser);
    }
    last = &SfxFileInfoPc[NumSfx];
    last->filename = NULL;
    last->pitch = 0;
    last->static_sample = 0;
    last->index = -1;
    NuSound3SetSampleTable(SfxFileInfoPc, cursor, end);
    memset(SfxBits, 0, 200);
    if (NOSOUND == 0)
        NuSound3SetRequestTable(SfxBits, 100);
    NuSound3Update();
}
void __cdecl InitSfx(VARIPTR *cursor, VARIPTR end, const char *file) {
    i32 i;
    i32 sample;
    cursor->addr = (cursor->addr + 3) & ~3u;
    SfxHashHeads = (i16 *)cursor->void_ptr;
    cursor->addr += 0x200;
    cursor->addr = (cursor->addr + 3) & ~3u;
    SfxFileInfoPc = (SFX_FILE_INFO_PC *)cursor->void_ptr;
    cursor->addr += 1600 * sizeof(SFX_FILE_INFO_PC);
    memset(SfxFileInfoPc, 0, 1600 * sizeof(SFX_FILE_INFO_PC));
    cursor->addr = (cursor->addr + 3) & ~3u;
    SfxInfo = (SFX_RECORD_PC *)cursor->void_ptr;
    cursor->addr += 1600 * sizeof(SFX_RECORD_PC);
    memset(SfxInfo, 0, 1600 * sizeof(SFX_RECORD_PC));
    cursor->addr = (cursor->addr + 3) & ~3u;
    SfxRevertInfoPc = (SFX_RECORD_PC *)cursor->void_ptr;
    cursor->addr += 1600 * sizeof(SFX_RECORD_PC);
    memset(SfxRevertInfoPc, 0, 1600 * sizeof(SFX_RECORD_PC));
    SfxBuildCRCTable(cursor, end);
    for (i = 0; i < 256; ++i)
        SfxHashHeads[i] = -1;
    NumSfx = 0;
    NumSfxInst = 0;
    for (i = 0; i < SfxFileCount; ++i) {
        SfxFileInfoPc[i] = SfxFileTable[i];
        SfxFileInfoPc[i].index = i;
        NuStrCpy(SfxFilenameSlotsPc[i], SfxFileInfoPc[i].filename);
        SfxFileInfoPc[i].filename = SfxFilenameSlotsPc[i];
        ++NumSfx;
        ++NumSfxNames;
    }
    SfxFileInfoPc[i].filename = NULL;
    SfxFileInfoPc[i].pitch = 0;
    SfxFileInfoPc[i].static_sample = 0;
    SfxFileInfoPc[i].index = -1;
    NuStrCpy(SfxConfigFilenamePc, file);
    LoadSfx(file, cursor, end);
    memset(GlobalSfxBits, 0, 200);
    for (i = 0; i < NumSfxInst; ++i) {
        if (SfxInfo[i].sample_id_flags & 0x20000) {
            sample = ((i32)(SfxInfo[i].sample_id_flags << 17)) >> 17;
            GlobalSfxBits[sample >> 4] |= (u16)(1 << (sample & 15));
        }
    }
    PCByteCopy_candidate(SfxBits, GlobalSfxBits, 200);
}
