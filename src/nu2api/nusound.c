#include "saga.h"
static i32 NumSfxInst = 0;

extern NUVEC nusound_special_positions[5];
extern f32 nusound_fade_start;
extern f32 nusound_fade_end;
extern u32 nusound_seed;

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

__declspec(noinline) void __cdecl PlaySfxById(i32 sfx_id, NUVEC *position) {
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

    for (index = 0; index < world->level_sfx_count; index++) {
        if (world->level_sfx[index].id == sfx_id)
            break;
    }
    if (index < world->level_sfx_count) {
        if (world->level_sfx[index].references < 0xff)
            world->level_sfx[index].references++;
        return;
    }
    if (world->level_sfx_count >= 64)
        return;
    if (position != NULL)
        world->level_sfx[world->level_sfx_count].position = *position;
    else
        world->level_sfx[world->level_sfx_count].position = APIObjectZeroMomentumPC;
    world->level_sfx[world->level_sfx_count].id = (i16)sfx_id;
    world->level_sfx[world->level_sfx_count].references = 1;
    world->level_sfx_count++;
}

void __cdecl UpdateLevelSfx(WORLDINFO *world, i32 paused) {
    i32(__cdecl * check_reverb)(void);
    SfxBitsRestore(&CurrentSFXTAB);
    if (paused == 0 && (check_reverb = GameAudio->check_reverb_fn) != NULL && check_reverb() != 0)
        NuSound3SetReverb(1);
    else
        NuSound3SetReverb(0);
    if (paused == 0 && CUTSTOPGAME == 0 && world->current_level->sfx_ambient != -1)
        PlaySfxById(world->current_level->sfx_ambient, NULL);
}

void __cdecl GroupBuffer_MoveToEnd(i32 group_id) {
    i32 buffer_length = NuSound3GroupSampleCount;
    i32 first_sample = NuSound3Groups[group_id].first_sample;
    i32 sample_count = NuSound3Groups[group_id].sample_count;
    i32 group_end = sample_count + first_sample;
    i32 index;

    if (group_end == buffer_length) {
        return;
    }
    NuSound3Groups[group_id].first_sample = (i16)buffer_length;
    PCByteCopy_candidate(&NuSound3GroupSampleIds[buffer_length], &NuSound3GroupSampleIds[first_sample],
                         sample_count * 2);
    PCByteCopy_candidate(&NuSound3GroupSampleIds[first_sample], &NuSound3GroupSampleIds[group_end],
                         (NuSound3GroupSampleCount - first_sample) * 2);
    for (index = 0; index < NuSound3GroupCount; index++) {
        if (NuSound3Groups[index].first_sample > first_sample) {
            NuSound3Groups[index].first_sample -= (i16)sample_count;
        }
    }
}

void __cdecl GroupBuffer_AddToGroup(i32 group_id, i32 sample_id) {
    if (NuSound3GroupSampleCount == 0x200) {
        return;
    }
    NuSound3GroupSampleIds[NuSound3GroupSampleCount++] = (i16)sample_id;
    NuSound3Groups[group_id].sample_count++;
    NuSound3Groups[group_id].cursor = 0;
}

void __cdecl GroupBuffer_RemoveFromGroup(i32 group_id, i32 sample_id) {
    i32 first_sample = NuSound3Groups[group_id].first_sample;
    i32 group_end = first_sample + NuSound3Groups[group_id].sample_count;
    i32 found = -1;
    i32 index;

    for (index = first_sample; index < group_end; index++) {
        if (NuSound3GroupSampleIds[index] == sample_id) {
            found = index;
            break;
        }
    }
    if (found == -1) {
        return;
    }
    PCByteCopy_candidate(&NuSound3GroupSampleIds[found], &NuSound3GroupSampleIds[found + 1],
                         (NuSound3GroupSampleCount - found - 1) * 2);
    NuSound3GroupSampleCount--;
    NuSound3Groups[group_id].sample_count--;
    NuSound3Groups[group_id].cursor = 0;
}

void __cdecl GroupBuffer_RemoveGroup(i32 group_id) {
    i32 first_sample;
    i32 sample_count;
    i32 index;

    if (group_id == -1) {
        return;
    }
    first_sample = NuSound3Groups[group_id].first_sample;
    sample_count = NuSound3Groups[group_id].sample_count;
    PCByteCopy_candidate(&NuSound3GroupSampleIds[first_sample], &NuSound3GroupSampleIds[first_sample + sample_count],
                         (NuSound3GroupSampleCount - sample_count - first_sample) * 2);
    NuSound3GroupSampleCount -= sample_count;
    for (index = 0; index < NuSound3GroupCount; index++) {
        if (NuSound3Groups[index].first_sample > first_sample) {
            NuSound3Groups[index].first_sample -= (i16)sample_count;
        }
    }
    PCByteCopy_candidate(&NuSound3Groups[group_id], &NuSound3Groups[group_id + 1],
                         (NuSound3GroupCount - 1) * sizeof(SFX_GROUP_PC));
    NuSound3GroupCount--;
    for (index = SfxFileCount; index < NumSfxInst; index++) {
        if (SfxInfo[index].group > group_id) {
            SfxInfo[index].group--;
        } else if (SfxInfo[index].group == group_id) {
            SfxInfo[index].group = -1;
        }
    }
}

i32 __cdecl GroupBuffer_MakeGroup(i32 sample_id) {

    if (NuSound3GroupCount == 0x80)
        return -1;
    if (NuSound3GroupSampleCount == 0x200)
        return -1;

    NuSound3Groups[NuSound3GroupCount].first_sample = (i16)NuSound3GroupSampleCount;
    NuSound3Groups[NuSound3GroupCount].sample_count = 1;
    NuSound3Groups[NuSound3GroupCount].cursor = 0;
    SfxInfo[sample_id].group = (i16)NuSound3GroupCount;
    NuSound3GroupSampleIds[NuSound3GroupSampleCount] = (i16)sample_id;
    NuSound3GroupSampleCount++;
    return NuSound3GroupCount++;
}

i32 __cdecl GroupBuffer_InGroup(i32 group_id, i32 sample_id) {
    i32 index = NuSound3Groups[group_id].first_sample;
    i32 group_end = index + NuSound3Groups[group_id].sample_count;

    for (; index < group_end; index++) {
        if (NuSound3GroupSampleIds[index] == sample_id) {
            return 1;
        }
    }
    return 0;
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

    nusound_seed = NuRandIntSeeded(&nusound_seed);
    if (sample_count == 0) {
        sample_count = 1;
    }
    return NuSound3GroupSampleIds[first_sample + ((nusound_seed >> 16) % sample_count)];
}

i32 __cdecl GroupBuffer_GetNumInGroup(i32 group_id) {
    return NuSound3Groups[group_id].sample_count;
}

i32 __cdecl GroupBuffer_GetSampleByIndex(i32 group_id, i32 sample_index) {
    return NuSound3GroupSampleIds[NuSound3Groups[group_id].first_sample + sample_index];
}

void __cdecl AudioGroup_Parse(NUFPAR_PC *parser) {
    i32 first = 1;
    i32 group_id = -1;

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
    const NUMTX *listener;
    i32 priority;
    i32 sample_id;
    i32 voice_volume;
    i32 pitch_parameter;
    i32 volume_left;
    i32 volume_right;
    i32 loop;
    i32 rumble_strength;
    f32 pan;
    f32 buzz_timer;
    f32 rumble_sustain;
    f32 rumble_release;
    f32 saved_fade_start = 0.0f;
    f32 saved_fade_end = 0.0f;
    f32 volume_scale;

    if (sfx_id == -1) {
        return;
    }

    if ((SfxInfo[sfx_id].sample_id_flags & 0x000c0000u) != 0) {
        return;
    }

    priority = SfxInfo[sfx_id].priority;
    if (SfxInfo[sfx_id].group != -1) {
        sfx_id = GroupBuffer_GetSample(SfxInfo[sfx_id].group, (SfxInfo[sfx_id].sample_id_flags >> 16) & 1);
    }

    sample_id = (i32)(SfxInfo[sfx_id].sample_id_flags << 17) >> 17;
    pitch_parameter = (i32)((f64)SfxInfo[sfx_id].pitch * 3763.0 / 44100.0);
    pan = SfxInfo[sfx_id].pan;
    buzz_timer = SfxInfo[sfx_id].buzz_timer;
    rumble_strength = SfxInfo[sfx_id].rumble_strength;
    rumble_sustain = SfxInfo[sfx_id].rumble_sustain;
    loop = (SfxInfo[sfx_id].sample_id_flags & 0x8000u) != 0;
    rumble_release = SfxInfo[sfx_id].rumble_release;

    if (SfxInfo[sfx_id].falloff_near != 0.0f || SfxInfo[sfx_id].falloff_far != 0.0f) {
        saved_fade_start = nusound_fade_start;
        saved_fade_end = nusound_fade_end;
        nusound_fade_start = saved_fade_start * SfxInfo[sfx_id].falloff_near * 0.5;
        nusound_fade_end = SfxInfo[sfx_id].falloff_far * saved_fade_end / 15.0;
    }

    listener = NuSound3GetListener();
    if (position != NULL) {
        if ((position->x - listener->m30) * (position->x - listener->m30) +
                (position->y - listener->m31) * (position->y - listener->m31) +
                (position->z - listener->m32) * (position->z - listener->m32) >
            nusound_fade_end * nusound_fade_end) {
            if (SfxInfo[sfx_id].falloff_near != 0.0f || SfxInfo[sfx_id].falloff_far != 0.0f) {
                nusound_fade_start = saved_fade_start;
                nusound_fade_end = saved_fade_end;
            }
            return;
        }
    }

    if (pitch != 1.0f) {
        pitch_parameter = (i32)((f64)pitch * SfxInfo[sfx_id].pitch * 3763.0 / 44100.0);
    }
    if (volume != 1.0f) {
        if (volume > 1.0f) {
            volume = 1.0f;
        }
        volume_scale = (f32)SfxInfo[sfx_id].volume * volume;
    } else {
        volume_scale = (f32)SfxInfo[sfx_id].volume;
    }
    if ((SfxInfo[sfx_id].sample_id_flags & 0x00400000u) == 0) {
        volume_scale *= AUDIOFADELEVEL;
        volume_scale *= numusicGetDuckVolume();
    }
    voice_volume = (i32)(volume_scale * MASTERVOLUME);

    if (SfxInfo[sfx_id].pitch_rnd != 0.0f) {
        f32 pitch_variation = NuRandFloatSeeded(&nusound_seed) * SfxInfo[sfx_id].pitch_rnd;
        f32 pitch_scale;
        if ((NuRandIntSeeded(&nusound_seed) & 1u) != 0) {
            pitch_scale = 1.0f + pitch_variation;
        } else {
            pitch_scale = (f32)(1.0 - (f64)pitch_variation * 0.5);
        }
        pitch_parameter = (i32)(pitch_parameter * pitch_scale);
        if ((SfxInfo[sfx_id].sample_id_flags & 0x8000u) != 0)
            pitch_parameter = -pitch_parameter;
    }
    if (SfxInfo[sfx_id].volume_rnd != 0.0f) {
        f32 volume_factor = 1.0f + NuRandFloatSeeded(&nusound_seed) * SfxInfo[sfx_id].volume_rnd;
        voice_volume = (i32)((f64)volume_factor * (f64)voice_volume);
    }

    if ((u32)sample_id <= 1599u) {
        if (position != NULL) {
            if (loop != 0) {
                PCSoundPlayPositionedLoop(position, sample_id, voice_volume, voice_volume, pitch_parameter, buzz_timer,
                                          rumble_strength, rumble_sustain, rumble_release);
            } else if (priority != 0) {
                PCSoundPlayPositioned(position, sample_id, voice_volume, voice_volume, pitch_parameter, buzz_timer,
                                      rumble_strength, rumble_sustain, rumble_release, priority);
            } else {
                PCSoundPlayPositionedWrapper(position, sample_id, voice_volume, voice_volume, pitch_parameter,
                                             buzz_timer, rumble_strength, rumble_sustain, rumble_release);
            }
        } else {
            volume_left = pan < 0.0f ? voice_volume : (i32)((1.0f - pan) * voice_volume);
            volume_right = pan > 0.0f ? voice_volume : (i32)((1.0f + pan) * voice_volume);
            if (loop != 0) {
                listener = NuSound3GetListener();
                PCSoundListenerLoopPositionPc = *(NUVEC *)&listener->m30;
                NuVecAdd(&PCSoundListenerLoopPositionPc, &PCSoundListenerLoopPositionPc, (NUVEC *)&listener->m20);
                PCSoundPlayPositionedLoop(&PCSoundListenerLoopPositionPc, sample_id, volume_left, volume_right,
                                          pitch_parameter, buzz_timer, rumble_strength, rumble_sustain, rumble_release);
            } else {
                if (priority != 0) {
                    PCSoundPlayUnpositioned(sample_id, volume_left, volume_right, pitch_parameter, buzz_timer,
                                            rumble_strength, rumble_sustain, rumble_release, priority);
                } else {
                    PCSoundPlayUnpositionedWrapper(sample_id, volume_left, volume_right, pitch_parameter, buzz_timer,
                                                   rumble_strength, rumble_sustain, rumble_release);
                }
            }
        }
    }

    if (SfxInfo[sfx_id].falloff_near != 0.0f || SfxInfo[sfx_id].falloff_far != 0.0f) {
        nusound_fade_start = saved_fade_start;
        nusound_fade_end = saved_fade_end;
    }
}

void __cdecl SfxSetFileTable(SFX_FILE_INFO_PC *records) {
    i32 index;

    SfxFileTable = records;
    SfxFileCount = 0;
    if (records == NULL) {
        return;
    }

    index = 0;
    while (records[index].filename != NULL) {
        NuStrLen(records[index].filename);
        index++;
        SfxFileCount++;
    }
}

void __cdecl SfxBuildCRCTable(VARIPTR *buffer_cursor, VARIPTR end) {
    u32 index;
    u32 value;
    u32 top_bit;
    i32 bit;

    if (SfxCRCTableInitialized != 0) {
        return;
    }

    SfxCRCTable = (u32 *)((buffer_cursor->addr + 3) & ~3u);
    buffer_cursor->addr = ((buffer_cursor->addr + 3) & ~3u) + 0x400;

    for (index = 0; index < 256; index++) {
        value = index << 24;
        for (bit = 0; bit < 8; bit++) {
            top_bit = value >> 31;
            value <<= 1;
            if (top_bit != 0) {
                value ^= 0x04c11db7;
            }
        }
        SfxCRCTable[index] = value;
    }
    SfxCRCTableInitialized = 1;
}

u32 __cdecl CRC_Process(const void *data, u32 size) {
    const u8 *bytes = (const u8 *)data;
    u32 hash = 0;
    u32 index;

    for (index = 0; index < size; index++) {
        hash = SfxCRCTable[bytes[index] ^ (hash >> 24)] ^ (hash << 8);
    }
    return hash;
}

u32 __cdecl CRC_ProcessString(const char *str) {
    u32 reg = 0;
    u32 top;

    while (*str != '\0') {
        top = (reg >> 24) ^ *str++;
        reg = (reg << 8) ^ SfxCRCTable[top];
    }
    return reg;
}

u32 __cdecl CRC_ProcessStringN(const char *str, u32 size) {
    u32 reg = 0;
    u32 i;
    u32 top;

    i = 0;
    while (str[i] != '\0' && i < size) {
        top = (reg >> 24) ^ str[i++];
        reg = (reg << 8) ^ SfxCRCTable[top];
    }
    return reg;
}

u32 __cdecl SfxNameHash(const char *name) {
    u32 reg = 0;
    u32 i = 0;
    u32 top;
    char c;

    while (name[i] != '\0') {
        top = reg >> 24;
        c = name[i++];
        if (c >= 'a' && c <= 'z')
            c -= 'a' - 'A';
        top ^= c;
        reg = (reg << 8) ^ SfxCRCTable[top];
    }
    return reg;
}

u32 __cdecl SfxNameHashN(const char *name, u32 max_length) {
    u32 reg = 0;
    u32 i = 0;
    u32 top;
    char c;

    while (name[i] != '\0' && i < max_length) {
        top = reg >> 24;
        c = name[i++];
        if (c >= 'a' && c <= 'z')
            c -= 'a' - 'A';
        top ^= c;
        reg = (reg << 8) ^ SfxCRCTable[top];
    }
    return reg;
}

i32 __cdecl GetSfxId(char *name) {
    i32 index;

    if (name == NULL)
        return -1;
    if (SfxHashHeads == NULL)
        return -1;

    index = SfxHashHeads[SfxNameHash(name) & 0xff];
    while (index != -1) {
        if (NuStrNICmp(name, SfxInfo[index].name, 0x20) == 0) {
            return index;
        }
        index = SfxInfo[index].next;
    }
    return -1;
}

void __cdecl SetSfxBit_OnEx(i32 sfx_id) {
    i32 word;
    if ((u32)sfx_id <= 0x63f) {
        word = sfx_id / 16;
        if (word < 100)
            SfxBits[word] |= (u16)(1u << (sfx_id & 0xf));
    }
}

void __cdecl SetSfxBit_OffEx(i32 sfx_id) {
    i32 word;
    if ((u32)sfx_id <= 0x63f) {
        word = sfx_id / 16;
        if (word < 100)
            SfxBits[word] &= (u16)(0xffff - (1u << (sfx_id & 0xf)));
    }
}

i32 __cdecl SfxBitEx(i32 sfx_id) {
    i32 word;
    if ((u32)sfx_id <= 0x63f) {
        word = sfx_id / 16;
        if (word < 100) {
            if (SfxBits[word] & (u16)(1 << (sfx_id & 0xf)))
                return 1;
            return 0;
        }
    }
    return -1;
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

void __cdecl SetSfxBit_Off(i32 sound_id) {
    if (sound_id >= 0) {
        SetSfxBit_OffEx((i32)(SfxInfo[sound_id].sample_id_flags << 17) >> 17);
    }
}

i32 __cdecl SfxBit(i32 sound_id) {
    if (sound_id >= 0) {
        return SfxBitEx((i32)(SfxInfo[sound_id].sample_id_flags << 17) >> 17);
    }
    return 0;
}

void __cdecl SetSfxBitTab_OnEx(SoundTable *table, i32 sfx_id) {
    i32 word;
    if ((u32)sfx_id <= 0x63f) {
        word = sfx_id / 16;
        if (word < 100)
            table->bits[word] |= (u16)(1u << (sfx_id & 0xf));
    }
}

void __cdecl SetSfxBitTab_OffEx(SoundTable *table, i32 sfx_id) {
    i32 word;
    if ((u32)sfx_id <= 0x63f) {
        word = sfx_id / 16;
        if (word < 100)
            table->bits[word] &= (u16)(0xffff - (1u << (sfx_id & 0xf)));
    }
}

i32 __cdecl SfxBitTabEx(const SoundTable *table, i32 sfx_id) {
    i32 word;
    if ((u32)sfx_id <= 0x63f) {
        word = sfx_id / 16;
        if (word < 100) {
            if (table->bits[word] & (u16)(1 << (sfx_id & 0xf)))
                return 1;
            return 0;
        }
    }
    return -1;
}

void __cdecl SetSfxBitTab_On(SoundTable *table, i32 sound_id) {
    if (sound_id >= 0) {
        SetSfxBitTab_OnEx(table, (i32)(SfxInfo[sound_id].sample_id_flags << 17) >> 17);
    }
}

void __cdecl SetSfxBitTab_Off(SoundTable *table, i32 sound_id) {
    if (sound_id >= 0) {
        SetSfxBitTab_OffEx(table, (i32)(SfxInfo[sound_id].sample_id_flags << 17) >> 17);
    }
}

i32 __cdecl SfxBitTab(const SoundTable *table, i32 sound_id) {
    if (sound_id >= 0) {
        return SfxBitTabEx(table, (i32)(SfxInfo[sound_id].sample_id_flags << 17) >> 17);
    }
    return 0;
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

void __cdecl SfxBitsSetAll(u16 *bits) {
    memset(bits, -1, 200);
}

void __cdecl SfxBitsStore(SoundTable *sound_table) {
    PCByteCopy_candidate(sound_table, SfxBits, 0xc8);
}

__declspec(noinline) void __cdecl SfxBitsRestore(SoundTable *sound_table) {
    PCByteCopy_candidate(SfxBits, sound_table, 0xc8);
}

void __cdecl SfxBitMaskTable(u16 *bits, const u16 *mask) {
    i32 index;
    for (index = 0; index < 100; index++) {
        bits[index] &= mask[index];
    }
}

void __cdecl PrepareSounds(const u16 *sounds) {
    i32 index;
    for (index = 0; index < 100; index++) {
        SfxBits[index] |= sounds[index];
    }
}

void __cdecl PrepareAllSounds(void) {
    i32 index;
    for (index = 0; index < 100; index++) {
        SfxBits[index] = 0xffff;
    }
}

void __cdecl MaskSounds(const u16 *mask) {
    i32 index;
    for (index = 0; index < 100; index++) {
        SfxBits[index] &= mask[index];
    }
}

f32 MusicVolume = 1.0f;
f32 CutVolume = 0.8f;
i32 __cdecl NuSound3GetMusicStreamStatusPc(i32 stream);

static void MusicPlayStream(i32 stream, i32 track, i32 volume, f32 seek_offset, i32 one_shot) {
    if (NOSOUND == 0 && NOMUSIC == 0) {
        NuSound3StopStereoStream(stream);
        NuSound3Update();
        NuSound3PlayStereoV(2, stream, 3, track, 6, volume, 10, (f64)seek_offset, one_shot != 0 ? 12 : 0, 1);
        NuSound3Update();
        Music.secondary_stream = (i16)stream;
        Music.transition_frames = 0;
    }
}

void SetPreSeekStartPoint(f32 offset) {
    Music.seek_offset = offset;
}

void __cdecl MusicPreSeek(i32 track) {
    if (NOSOUND != 0 || NOMUSIC != 0 || track < 0) {
        return;
    }
    if (track <= SfxFileCount - 1 && (Music.state == 4 || Music.state == 0) &&
        (Music.requested_track != track || Music.pause_requested != 0)) {
        Music.resume_track = (i16)track;
        Music.queued_track = (i16)track;
        Music.requested_track = (i16)track;
        (&Music.field_0012)[Music.primary_stream] = 0;
        if (Music.transition_frames < 0x40 && Music.state != 0) {
            Music.pause_requested = 1;
        } else {
            Music.pause_requested = 0;
            MusicPlayStream(1 - Music.primary_stream, track, 0, Music.seek_offset, 1);
            Music.seek_offset = 0.0f;
        }
    }
    if (track <= SfxFileCount - 1 && (Music.state == 11 || Music.state == 12 || Music.state == 13) &&
        (Music.current_track != track || Music.pause_requested != 0)) {
        Music.current_track = (i16)track;
        Music.queued_track = (i16)track;
        Music.requested_track = (i16)track;
        Music.state = 11;
        (&Music.field_0012)[Music.primary_stream] = 0;
        if (Music.transition_frames >= 0x40) {
            Music.pause_requested = 0;
            MusicPlayStream(Music.primary_stream, track, 0, Music.seek_offset, 1);
            Music.seek_offset = 0.0f;
            Music.transition_frames = 0;
        } else {
            Music.pause_requested = 1;
        }
    }
}

void __cdecl RestoreGameMusic(void) {
    if ((NOMUSIC == 0 || NOSOUND == 0) && PCStereoChannel1B != -1) {
        if (Music.transition_frames <= 0x18) {
            Music.restore_requested = 1;
            return;
        }
        if (Music.state != 11 && Music.state != 12 && Music.state != 13) {
            NuSound3ResumeStereoStream(Music.primary_stream);
            switch (Music.state) {
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                    NuSound3ResumeStereoStream(1 - Music.primary_stream);
                    break;
            }
        }
    }
}

void __cdecl SoundUpdate(f32 frame_time) {
    if (Music.update_delay != 0 || frame_time == 0.0f) {
        return;
    }
    switch (Music.state) {
        case 8:
        case 10:
            if (NuSound3GetMusicStreamStatusPc(Music.primary_stream) != 2) {
                Music.state = 7;
            }
            break;
        case 5:
        case 6:
        case 7:
        case 9:
            if (Music.state == 9 || Music.state == 6) {
                Music.transition = frame_time + frame_time + Music.transition;
            } else {
                Music.transition = frame_time * 0.2f + Music.transition;
            }
            if (Music.transition >= 1.0f) {
                Music.transition = 1.0f;
                if (Music.state == 5 || Music.state == 6) {
                    NuSound3PauseStereoStream(1 - Music.primary_stream);
                } else {
                    NuSound3StopStereoStream(1 - Music.primary_stream);
                }
                Music.state = 4;
                Music.transition_frames = 0;
            }
            if (Music.current_track != -1) {
                NuSound3SetStreamVolume(Music.primary_stream, (i32)((f32)SfxFileTable[Music.current_track].index *
                                                                    Music.transition * MusicVolume));
            }
            if (Music.queued_track != -1) {
                NuSound3SetStreamVolume(1 - Music.primary_stream, (i32)((f32)SfxFileTable[Music.queued_track].index *
                                                                        (1.0f - Music.transition) * MusicVolume));
            }
            break;
        case 11:
        case 12:
        case 13:
            if ((Music.track_data != NULL && (((INSTNUGCUTSCENE *)Music.track_data)->flags_88 & 2) != 0) ||
                NuSound3GetMusicStreamStatusPc(1 - Music.primary_stream) == 2) {
                Music.resume_frames = 0;
            } else {
                if (Music.resume_frames < 0x40) {
                    Music.resume_frames++;
                }
                if (Music.resume_frames > 8 && Music.current_track != -1 && Music.state != 13 &&
                    NuSound3GetMusicStreamStatusPc(Music.primary_stream) != 2 && Music.pause_requested == 0) {
                    if (Music.state == 12) {
                        Music.state = 0;
                        NuSound3StopStereoStream(1 - Music.primary_stream);
                        NuSound3StopStereoStream(Music.primary_stream);
                    } else {
                        Music.state = 4;
                        NuSound3StopStereoStream(1 - Music.primary_stream);
                        NuSound3ResumeStereoStream(Music.primary_stream);
                    }
                    Music.transition = 1.0f;
                    NuSound3SetStreamVolume(Music.primary_stream,
                                            (i32)((f32)SfxFileTable[Music.current_track].index * MusicVolume));
                    NuSound3Update();
                    Music.transition_frames = 0;
                    Music.requested_track = -1;
                }
            }
            if (NuSound3GetMusicStreamStatusPc(Music.secondary_stream) != 2) {
                if (Music.transition_frames < 0x80) {
                    Music.transition_frames++;
                }
                if (Music.transition_frames > 0x40 && Music.pause_requested != 0) {
                    MusicPreSeek(Music.requested_track);
                }
            }
            break;
        case 4:
            if (NuSound3GetMusicStreamStatusPc(Music.secondary_stream) != 2) {
                if (Music.transition_frames < 0x80) {
                    Music.transition_frames++;
                }
                if (Music.transition_frames > 0x40 && Music.pause_requested != 0) {
                    MusicPreSeek(Music.requested_track);
                }
                if (Music.transition_frames > 0x18 && Music.restore_requested != 0) {
                    RestoreGameMusic();
                    Music.restore_requested = 0;
                }
            }
            break;
    }
}

i32 MusicState(void) {
    return Music.state;
}

i32 MusicSeeking(void) {
    return 0;
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
        Music.resume_frames = 0;
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
    if (context == NULL || context == Music.track_data) {
        Music.track_data = NULL;
    }
}

void PauseGameCut(void) {
    struct music_playback_pc_s *music;
    if (NOMUSIC != 0 || NOSOUND != 0 || PCStereoChannel1B == -1) {
        return;
    }

    music = &Music;
    if (music->state != MUSIC_PLAYBACK_DUAL_STREAM && music->state != MUSIC_PLAYBACK_DUAL_STREAM_PENDING) {
        return;
    }
    NuSound3CancelCheckStereo();
    NuSound3PauseStereoStream(1 - music->primary_stream);
}

void RestoreGameCut(void) {
    MUSICPLAYBACK_PC *music = &Music;

    if (NOMUSIC != 0 && NOSOUND != 0) {
        return;
    }
    if (PCStereoChannel1B == -1) {
        return;
    }
    if (music->state != MUSIC_PLAYBACK_DUAL_STREAM && music->state != MUSIC_PLAYBACK_DUAL_STREAM_PENDING) {
        return;
    }
    NuSound3ResumeStereoStream(1 - music->primary_stream);
}

void __cdecl ResetSounds(void) {
    PCByteCopy_candidate(SfxBits, GlobalSfxBits, 0xc8);
}

i32 __cdecl GetMusicIndex(char *name, struct nusound_filename_info_s *table, i32 fallback) {
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

void __cdecl NuSound3AddRumble(void *pad, f32 strength, i32 duration, f32 priority, f32 buzz) {
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

__declspec(noinline) i32 __cdecl NuSound3SetReverb(i32 enable) {
    i32 previous = SoundVoiceManager.option_d14;
    SoundVoiceManager.option_d14 = enable != 0;
    return previous;
}

void NuSound3SetLoopHoldTime(f32 time) {
    NuSound3LoopHoldTime = (i32)(time * NuScale1000);
}

void __cdecl NuSound3StopRumble(void) {
}

void __cdecl NuSound3UpdateRumble(f32 frame_time) {
    (void)frame_time;
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
    volume = volume > 0 ? 0 : volume;
    if (volume < -10000) {
        volume = -10000;
    }

    return buffer->vtable->set_volume(buffer, volume);
}

HRESULT __cdecl PCSoundSetLinearVolume(IDirectSoundBuffer *buffer, f32 volume) {
    i32 raw_volume = (i32)volume + SoundVoiceVolumeOffset;

    raw_volume = raw_volume > 0 ? 0 : raw_volume;
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

char *__cdecl GetSfxName(i32 sound_id) {
    if (sound_id >= 0) {
        return SfxInfo[sound_id].name;
    }
    return NULL;
}

extern i32 NuSound3LoadingSfxPc;
i32 __cdecl NuSound3LoadingSfx(void) {
    return NuSound3LoadingSfxPc != 0;
}

STATIC_ASSERT(offsetof(GAMEAUDIO_PC, sfx_ids) == 0x158);
STATIC_ASSERT(sizeof(((GAMEAUDIO_PC *)0)->sfx_ids[0]) == 2);

/* Integration fragment: declarations below must precede callback definitions. */
void __cdecl InitSoundInfo(i32);
void __cdecl LoadSfx(const char *, VARIPTR *, VARIPTR);
static void fnAudioSample(NUFPAR_PC *fpar) {
    char *str;
    u32 hash;
    i32 i;
    u32 filename_index;
    f32 pan, pitch_rnd, volume_rnd, falloff_near, falloff_far;
    f32 buzz_timer, rumble_sustain, rumble_release;
    i32 priority, volume, rumble_strength, category;
    InitSoundInfo(NumSfxInst);
    while (NuFParGetWord(fpar) != 0) {
        if (NuStrICmp(fpar->word_buffer_ptr, "disable") == 0) {
            SfxInfo[NumSfxInst].sample_id_flags |= 0x40000u;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "comment") == 0) {
            SfxInfo[NumSfxInst].sample_id_flags |= 0x80000u;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "name") == 0) {
            NuFParGetWord(fpar);
            NuStrNCpy(SfxNameSlotsPc[NumSfxInst], fpar->word_buffer_ptr, 0x20);

            str = SfxNameSlotsPc[NumSfxInst];
            SfxInfo[NumSfxInst].name = str;
            hash = SfxNameHash(str) & 0xff;

            if (SfxHashHeads[hash] != -1) {
                SfxInfo[NumSfxInst].next = SfxHashHeads[hash];
                SfxHashHeads[hash] = NumSfxInst;
            } else {
                SfxHashHeads[hash] = NumSfxInst;
                SfxInfo[NumSfxInst].next = -1;
            }
        } else if (NuStrICmp(fpar->word_buffer_ptr, "fname") == 0) {
            NuFParGetWord(fpar);
            NuStrCat(fpar->word_buffer_ptr, NuFileExt_wav_PC);

            if ((SfxInfo[NumSfxInst].sample_id_flags & 0xc0000u) == 0) {
                i = 0;
                for (; i < NumSfx; i++) {
                    if (NuStrICmp(SfxFileInfoPc[i].filename, fpar->word_buffer_ptr) == 0) {
                        SfxInfo[NumSfxInst].sample_id_flags =
                            (SfxInfo[NumSfxInst].sample_id_flags & ~0x7fffu) | ((i) & 0x7fff);
                        SfxInfo[NumSfxInst].filename = SfxFileInfoPc[i].filename;
                        SfxRefCountsPc[i]++;
                        break;
                    }
                }
                if (i == NumSfx) {
                    str = SfxFilenameSlotsPc[NumSfxNames++];
                    SfxFileInfoPc[i].filename = str;
                    SfxFileInfoPc[i].pitch = 0x75a;
                    SfxFileInfoPc[i].index = i - SfxFileCount + 0x1000;
                    SfxFileInfoPc[i].static_sample = 0;
                    NuStrNCpy(str, fpar->word_buffer_ptr, 0x40);
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
                        SfxInfo[NumSfxInst].filename = SfxFileInfoPc[i].filename;
                        SfxRefCountsPc[i]++;
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
            if (pan < -1.0f)
                pan = -1.0f;
            else if (pan > 1.0f)
                pan = 1.0f;
            SfxInfo[NumSfxInst].pan = pan;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "pri") == 0) {
            priority = NuFParGetInt(fpar);
            SfxInfo[NumSfxInst].priority = priority < -128 ? -128 : priority > 127 ? 127 : priority;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "loop") == 0) {
            SfxInfo[NumSfxInst].sample_id_flags |= 0x8000u;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "pitch_rnd") == 0) {
            pitch_rnd = NuFParGetFloat(fpar);
            if (pitch_rnd < 0.0f)
                pitch_rnd = 0.0f;
            else if (pitch_rnd > 1.0f)
                pitch_rnd = 1.0f;
            SfxInfo[NumSfxInst].pitch_rnd = pitch_rnd;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "volume") == 0) {
            volume = NuFParGetInt(fpar);
            SfxInfo[NumSfxInst].volume = volume < 0 ? 0 : volume > 0x3fff ? 0x3fff : volume;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "nofade") == 0) {
            SfxInfo[NumSfxInst].sample_id_flags |= 0x400000u;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "volume_rnd") == 0) {
            volume_rnd = NuFParGetFloat(fpar);
            if (volume_rnd < -1.0f)
                volume_rnd = -1.0f;
            else if (volume_rnd > 0.0f)
                volume_rnd = 0.0f;
            SfxInfo[NumSfxInst].volume_rnd = volume_rnd;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "near") == 0) {
            falloff_near = NuFParGetFloat(fpar);
            if (falloff_near < 0.0f)
                falloff_near = 0.0f;
            if (falloff_near > 250.0)
                falloff_near = 250.0f;
            SfxInfo[NumSfxInst].falloff_near = falloff_near;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "far") == 0) {
            falloff_far = NuFParGetFloat(fpar);
            if (falloff_far < 0.0f)
                falloff_far = 0.0f;
            if (falloff_far > 250.0)
                falloff_far = 250.0f;
            SfxInfo[NumSfxInst].falloff_far = falloff_far;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "global") == 0) {
            SfxInfo[NumSfxInst].sample_id_flags |= 0x20000u;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "rumble") == 0) {
            buzz_timer = NuFParGetFloat(fpar);
            rumble_strength = NuFParGetInt(fpar);
            if (buzz_timer < 0.0f)
                buzz_timer = 0.0f;
            else if (buzz_timer > 5.0f)
                buzz_timer = 5.0f;
            SfxInfo[NumSfxInst].buzz_timer = buzz_timer;
            SfxInfo[NumSfxInst].rumble_strength = rumble_strength < 0      ? 0
                                                  : rumble_strength > 0xff ? 0xff
                                                                           : rumble_strength;
            rumble_sustain = NuFParGetFloat(fpar);
            if (rumble_sustain < 0.0f)
                rumble_sustain = 0.0f;
            else if (rumble_sustain > 5.0f)
                rumble_sustain = 5.0f;
            SfxInfo[NumSfxInst].rumble_sustain = rumble_sustain;
            rumble_release = NuFParGetFloat(fpar);
            if (rumble_release < 0.0f)
                rumble_release = 0.0f;
            else if (rumble_release > 5.0f)
                rumble_release = 5.0f;
            SfxInfo[NumSfxInst].rumble_release = rumble_release;
        } else if (NuStrICmp(fpar->word_buffer_ptr, "fcat") == 0) {
            category = NuFParGetInt(fpar);
            SfxInfo[NumSfxInst].category = category < 0 ? 0 : category > 0xffff ? 0xffff : category;
        }
    }
    SfxInfo[NumSfxInst].sample_id_flags &= ~0x100000u;
    SfxInfo[NumSfxInst].sample_id_flags |= 0x200000u;
    PCByteCopy_candidate(&SfxRevertInfoPc[NumSfxInst], &SfxInfo[NumSfxInst], sizeof(SFX_RECORD_PC));
    NumSfxInst++;
}

static void __cdecl fnAudioAudio(NUFPAR_PC *parser) {
    SfxAudioVersionPc = NuFParGetFloat(parser);
}
static NUFPCOMJMP audioComPc[] = {
    {"Audio", fnAudioAudio}, {"Sample", fnAudioSample}, {"Group", AudioGroup_Parse}, {NULL, NULL}};
void __cdecl InitSoundInfo(i32 index) {
    SfxInfo[index].sample_id_flags |= 0x7fff;
    SfxInfo[index].name = SfxNameSlotsPc[NumSfxInst];
    SfxInfo[index].pitch = 0x5622;
    SfxInfo[index].priority = 0;
    SfxInfo[index].pan = 0.0f;
    SfxInfo[index].sample_id_flags &= ~0x400000u;
    SfxInfo[index].volume = 0x3fff;
    SfxInfo[index].pitch_rnd = 0.0f;
    SfxInfo[index].volume_rnd = 0.0f;
    SfxInfo[index].group = -1;
    SfxInfo[index].sample_id_flags &= ~0x8000u;
    SfxInfo[index].sample_id_flags &= ~0x10000u;
    SfxInfo[index].sample_id_flags &= ~0x20000u;
    SfxInfo[index].sample_id_flags &= ~0x40000u;
    SfxInfo[index].sample_id_flags &= ~0x100000u;
    SfxInfo[index].falloff_near = 0.0f;
    SfxInfo[index].falloff_far = 0.0f;
    SfxInfo[index].buzz_timer = 0.0f;
    SfxInfo[index].rumble_strength = 0;
    SfxInfo[index].rumble_sustain = 0.0f;
    SfxInfo[index].rumble_release = 0.0f;
    SfxInfo[index].category = 0;
    SfxInfo[index].sample_id_flags &= ~0x200000u;
    SfxRevertInfoPc[index] = SfxInfo[index];
}
void __cdecl LoadSfx(const char *file, VARIPTR *cursor, VARIPTR end) {
    NUFPAR_PC *parser = NuFParCreate((char *)file);
    i32 count;
    if (parser != NULL) {
        NuFParPushCom(parser, audioComPc);
        while (NuFParGetLine(parser)) {
            NuFParGetWord(parser);
            if (*parser->word_buffer_ptr != 0)
                NuFParInterpretWord(parser);
        }
        NuFParDestroy(parser);
    }
    count = NumSfx;
    SfxFileInfoPc[count].filename = NULL;
    SfxFileInfoPc[count].pitch = 0;
    SfxFileInfoPc[count].static_sample = 0;
    SfxFileInfoPc[count].index = -1;
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
    SfxFileInfoPc = (SFX_FILE_INFO_PC *)((cursor->addr + 3) & ~3u);
    cursor->addr = ((cursor->addr + 3) & ~3u) + 1600 * sizeof(SFX_FILE_INFO_PC);
    memset(SfxFileInfoPc, 0, 1600 * sizeof(SFX_FILE_INFO_PC));
    SfxInfo = (SFX_RECORD_PC *)((cursor->addr + 3) & ~3u);
    cursor->addr = ((cursor->addr + 3) & ~3u) + 1600 * sizeof(SFX_RECORD_PC);
    memset(SfxInfo, 0, 1600 * sizeof(SFX_RECORD_PC));
    SfxRevertInfoPc = (SFX_RECORD_PC *)((cursor->addr + 3) & ~3u);
    cursor->addr = ((cursor->addr + 3) & ~3u) + 1600 * sizeof(SFX_RECORD_PC);
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
        ++NumSfx;
        ++NumSfxNames;
        SfxFileInfoPc[i].filename = SfxFilenameSlotsPc[i];
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

void __cdecl PauseGameAudio(i32 ignored) {
    (void)ignored;
    if (NOSOUND == 0) {
        NuSound3StopSFX();
        NuSound3SetSFXPitch(0);
    }
}

void ResumeGameAudio(void) {
}

i32 GetSfxCount(void) {
    return NumSfx;
}

i32 GetLogicalSfxCount(void) {
    return NumSfxInst;
}

void PlayAMusic(i32 stream, i32 track, i32 volume, i32 one_shot) {
    if (NOSOUND == 0 && NOMUSIC == 0) {
        NuSound3StopStereoStream(stream);
        NuSound3Update();
        NuSound3PlayStereoV(2, stream, 3, track, 6, volume, one_shot != 0 ? 12 : 0, 1);
        NuSound3Update();
        Music.secondary_stream = (i16)stream;
        Music.transition_frames = 0;
    }
}

i32 GetCurrentMusicId(void) {
    return Music.current_track;
}

i32 GetOppMusicId(void) {
    return Music.queued_track;
}

void SetMusicVolume(f32 volume) {
    if (NOSOUND == 0 && NOMUSIC == 0) {
        MusicVolume = volume;
        if (Music.state == 4) {
            NuSound3SetStreamVolume(Music.primary_stream, (i32)((f32)SfxFileTable[Music.current_track].index * volume));
        }
    }
}

void SetCutVolume(f32 volume) {
    CutVolume = volume;
}

i32 GetCurPreSeek(void) {
    return Music.requested_track;
}

void ResetPreSeek(void) {
    if (Music.state == 11) {
        Music.current_track = -1;
    } else {
        Music.queued_track = -1;
        Music.requested_track = -1;
        Music.pause_requested = 0;
    }
    Music.seek_offset = 0.0f;
}

void MusicPreSeekNow(i32 track) {
    if (track < 0 || track > SfxFileCount - 1) {
        return;
    }
    Music.queued_track = (i16)track;
    Music.requested_track = (i16)track;
    (&Music.field_0012)[Music.primary_stream] = 0;
    Music.pause_requested = 0;
    MusicPlayStream(1 - Music.primary_stream, track, 0, Music.seek_offset, 1);
    Music.seek_offset = 0.0f;
}

void MusicSeekOffset(i32 track, f32 seek_offset) {
    if (seek_offset < 0.0f) {
        seek_offset = Music.seek_offset;
    }
    if (track < 0) {
        return;
    }
    if (track <= SfxFileCount - 1 && (Music.state == 11 || Music.state == 12 || Music.state == 13)) {
        if (Music.current_track != track || Music.pause_requested != 0) {
            Music.current_track = (i16)track;
            Music.queued_track = (i16)track;
            Music.requested_track = -1;
            Music.state = 11;
            (&Music.field_0012)[Music.primary_stream] = 0;
            if (Music.transition_frames >= 0x40) {
                Music.pause_requested = 0;
                MusicPlayStream(Music.primary_stream, track, (i32)((f32)SfxFileTable[track].index * MusicVolume),
                                seek_offset, 1);
                Music.seek_offset = 0.0f;
                Music.transition_frames = 0;
            } else {
                Music.pause_requested = 1;
            }
        }
        return;
    }
    if (track <= SfxFileCount - 1) {
        Music.current_track = (i16)track;
        Music.queued_track = (i16)track;
        Music.requested_track = (i16)track;
        Music.state = 4;
        Music.pause_requested = 0;
        Music.transition_frames = 0;
        (&Music.field_0012)[Music.primary_stream] = 0;
        MusicPlayStream(Music.primary_stream, track, (i32)((f32)SfxFileTable[track].index * MusicVolume), seek_offset,
                        1);
    }
}

i32 PlayingCutMusic(void) {
    u8 *delay = &(&Music.field_0012)[1 - Music.primary_stream];
    if (*delay != 0) {
        --*delay;
        return 0;
    }
    if (NOSOUND != 0 || NOMUSIC != 0) {
        return 0;
    }
    return NuSound3GetMusicStreamStatusPc(1 - Music.primary_stream) != 2;
}

i32 PlayCutMusic(i32 track, i32 state, void *context) {
    i32 started = 0;
    if (NOSOUND != 0 || NOMUSIC != 0) {
        return 0;
    }
    if (track >= 0 && track <= SfxFileCount - 1) {
        Music.transition_frames = 0;
        Music.queued_track = (i16)track;
        (&Music.field_0012)[1 - Music.primary_stream] = 6;
        NuSound3CancelCheckStereo();
        NuSound3PauseStereoStream(Music.primary_stream);
        NuSound3SetStreamVolume(Music.primary_stream, (i32)((f32)SfxFileTable[track].index * CutVolume));
        if (Music.requested_track == track && Music.pause_requested == 0 &&
            NuSound3GetMusicStreamStatusPc(1 - Music.primary_stream) != 2) {
            NuSound3ResumeStereoStream(1 - Music.primary_stream);
            NuSound3SetStreamVolume(1 - Music.primary_stream, (i32)((f32)SfxFileTable[track].index * CutVolume));
            NuSound3Update();
        } else {
            started = 1;
            PlayAMusic(1 - Music.primary_stream, track, (i32)((f32)SfxFileTable[track].index * CutVolume), 0);
        }
        Music.transition = 1.0f;
        Music.requested_track = -1;
        Music.pause_requested = 0;
        if (state == 13) {
            Music.state = 13;
        } else if (state == 12) {
            Music.state = 12;
        } else {
            Music.state = Music.state != 0 ? 11 : 12;
        }
        Music.track_data = context;
    }
    return started;
}

void PauseGameMusic(void) {
    if (NOMUSIC == 0 && NOSOUND == 0 && PCStereoChannel1B != -1 && Music.state != 11 && Music.state != 12 &&
        Music.state != 13) {
        NuSound3CancelCheckStereo();
        NuSound3PauseStereoStream(Music.primary_stream);
        switch (Music.state) {
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
                NuSound3PauseStereoStream(1 - Music.primary_stream);
                break;
        }
    }
    Music.restore_requested = 0;
}

void PlayAltGameMusic(i32 track) {
    Music.update_delay = 1;
    if (Music.requested_track == track && Music.pause_requested == 0) {
        NuSound3ResumeStereoStream(1 - Music.primary_stream);
        NuSound3SetStreamVolume(1 - Music.primary_stream, (i32)((f32)SfxFileTable[track].index * MusicVolume));
        NuSound3Update();
        Music.resume_track = -1;
    } else {
        PlayAMusic(1 - Music.primary_stream, track, (i32)((f32)SfxFileTable[track].index * MusicVolume), 0);
    }
}

void StopAltGameMusic(void) {
    NuSound3CancelCheckStereo();
    NuSound3PauseStereoStream(1 - Music.primary_stream);
    Music.update_delay = 0;
    Music.queued_track = -1;
    Music.requested_track = -1;
    if (Music.resume_track >= 0) {
        MusicPreSeek(Music.resume_track);
    }
}

void PauseGameSfx(void) {
    NuSound3StopSFX();
    NuSound3FlushLoops();
    NuSound3SetSFXPitch(0);
}

void PlayMusic(i32 track, i32 mode) {
    if (NOSOUND != 0 || NOMUSIC != 0) {
        return;
    }
    if (track < 0 || track > SfxFileCount - 1) {
        Music.transition = 0.0f;
        Music.primary_stream = (i16)(1 - Music.primary_stream);
        Music.queued_track = Music.current_track;
        Music.current_track = (i16)track;
        Music.restore_requested = 0;
        Music.state = mode == 3 ? 9 : 7;
        return;
    }
    Music.queued_track = Music.current_track;
    Music.current_track = (i16)track;
    Music.transition_frames = 0;
    if (Music.state != 0 && mode != 1 && PCStereoChannel1B != -1) {
        Music.primary_stream = (i16)(1 - Music.primary_stream);
        (&Music.field_0012)[Music.primary_stream] = 0;
        if (Music.requested_track == track && Music.pause_requested == 0) {
            Music.state = mode == 3 ? 9 : 7;
            NuSound3ResumeStereoStream(Music.primary_stream);
            NuSound3SetStreamVolume(Music.primary_stream, 0);
            NuSound3Update();
        } else {
            Music.state = mode == 3 ? 10 : 8;
            PlayAMusic(Music.primary_stream, track, 0, 0);
        }
        Music.pause_requested = 0;
        Music.transition = 0.0f;
        Music.restore_requested = 0;
        Music.requested_track = -1;
        return;
    }
    (&Music.field_0012)[Music.primary_stream] = 0;
    if (Music.requested_track == track && Music.state != 11) {
        Music.primary_stream = (i16)(1 - Music.primary_stream);
    }
    NuSound3StopStereoStream(1 - Music.primary_stream);
    if (Music.requested_track == track && Music.pause_requested == 0 &&
        NuSound3GetMusicStreamStatusPc(Music.primary_stream) != 2) {
        NuSound3ResumeStereoStream(Music.primary_stream);
        NuSound3SetStreamVolume(Music.primary_stream, (i32)((f32)SfxFileTable[track].index * MusicVolume));
        NuSound3Update();
    } else {
        PlayAMusic(Music.primary_stream, track, (i32)((f32)SfxFileTable[track].index * MusicVolume), 0);
    }
    Music.pause_requested = 0;
    Music.transition = 1.0f;
    Music.restore_requested = 0;
    Music.state = 4;
    Music.requested_track = -1;
}

void SwapMusic(i32 mode) {
    i32 previous_track;
    i32 track;
    if (NOSOUND != 0 || NOMUSIC != 0) {
        return;
    }
    previous_track = Music.current_track;
    Music.primary_stream = (i16)(1 - Music.primary_stream);
    Music.current_track = Music.queued_track;
    Music.queued_track = (i16)previous_track;
    track = Music.current_track;
    NuSound3ResumeStereoStream(Music.primary_stream);
    if (mode == 1) {
        NuSound3SetStreamVolume(Music.primary_stream, (i32)((f32)SfxFileTable[track].index * MusicVolume));
        NuSound3PauseStereoStream(1 - Music.primary_stream);
        NuSound3SetStreamVolume(1 - Music.primary_stream, 0);
        Music.transition = 1.0f;
        Music.state = 4;
    } else {
        if (Music.state == 5 || Music.state == 6) {
            Music.transition = 1.0f - Music.transition;
        } else {
            Music.transition = 0.0f;
        }
        NuSound3SetStreamVolume(Music.primary_stream, (i32)((f32)SfxFileTable[track].index * Music.transition));
        Music.state = mode == 3 ? 6 : 5;
    }
    NuSound3Update();
}

void __cdecl SetLinkedCutSceneMusic(void *context, i32 state) {
    Music.track_data = context;
    if (Music.state == 11 || Music.state == 12 || Music.state == 13)
        Music.state = state == 13 ? 13 : 11;
}

/* Retail 0x00698a20 is a one-byte RET and reads neither argument. */
void __cdecl NuSound3ClearLoopHold(NUVEC *position, i32 mode) {
}

/* Retail's sound error report, compiled empty. It lives outside the sound voice code so its callers keep their
 * failure tests and calls; they pass a path, an index, an HRESULT or nothing. */
void __cdecl PCSoundFailure(const char *format, ...) {
    (void)format;
}
