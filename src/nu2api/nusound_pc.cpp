#include "saga.h"

#include <string.h>

extern "C" void __cdecl PCSoundAlreadyPlayingHook(void) {
}

extern "C" void __cdecl NuSound3FlushLoops(void) {
}

extern "C" void __cdecl PCSoundFailure(...) {
}

extern "C" void __cdecl PCSoundPlayUnpositioned(i32 sample_id, i32 volume_index, i32 arg3, i32 arg4, f32 arg5, i32 arg6,
                                                f32 arg7, f32 arg8, i32 arg9) {
    SoundSamplePC *sample;
    i32 scaled_volume;
    i32 voice_id;
    i32 table_index;

    if (SoundVoiceManager.samples == NULL) {
        return;
    }

    sample = &SoundVoiceManager.samples[sample_id];
    if (sample->valid_byte == 0) {
        return;
    }

    if (PCSoundVolumeMultiplier == 0x4000) {
        scaled_volume = volume_index;
    } else {
        scaled_volume = (PCSoundVolumeMultiplier * volume_index) >> 15;
    }

    voice_id = SoundVoiceManager.AllocateVoice(sample_id, 0);
    if (voice_id <= -1) {
        return;
    }

    table_index = scaled_volume;
    if (table_index > 0x3fff) {
        table_index = 0;
    }

    SoundVoiceManager.SetVoice(voice_id, 0, NuSound3VolumeTable[table_index], 0);

    (void)arg3;
    (void)arg4;
    (void)arg5;
    (void)arg6;
    (void)arg7;
    (void)arg8;
    (void)arg9;
}

extern "C" void __cdecl PCSoundPlayUnpositionedWrapper(i32 arg1, i32 arg2, i32 arg3, i32 arg4, f32 arg5, i32 arg6,
                                                       f32 arg7, f32 arg8) {
    PCSoundPlayUnpositioned(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, 0);
}

i32 __thiscall SoundVoiceManagerPC::AllocateVoice(i32 sample_id, i32 bank) {
    i32 first_voice;
    i32 last_voice;
    i32 voice_id;
    SoundSamplePC *sample;
    SoundVoiceSlotPC *slot;
    HRESULT result;

    if (factory == NULL || SoundVoiceEnabledGate == 0 || samples == NULL) {
        return -1;
    }

    sample = &samples[sample_id];
    if (sample->valid_byte == 0) {
        return -1;
    }

    first_voice = bank ? 32 : 0;
    last_voice = first_voice + 32;
    for (voice_id = first_voice; voice_id < last_voice; ++voice_id) {
        slot = &voices[voice_id];
        if (slot->voice == NULL) {
            break;
        }
    }
    if (voice_id == last_voice) {
        return -1;
    }

    EnterCriticalSection(&SoundVoiceCriticalSection);
    result = factory->vtable->duplicate_sound_buffer(factory, sample->buffer, &slot->voice);
    LeaveCriticalSection(&SoundVoiceCriticalSection);
    if (result < 0) {
        PCSoundFailure();
        return -1;
    }

    /* PC stores the sample-table index here; the second argument selects a voice bank. */
    slot->sample_id = sample_id;
    slot->sample = sample;
    ++active_count;
    return voice_id;
}

void __thiscall SoundVoiceManagerPC::SetVoice(i32 voice_id, i32 spatial, f32 volume, i32 parameter) {
    SoundVoiceSlotPC *slot = &voices[voice_id];
    IDirectSoundBuffer *voice = slot->voice;
    i32 volume_level;
    i32 status;
    i32 sample_frequency;
    i32 absolute_parameter;
    i32 pitch_product;
    i32 frequency_divisor;
    i32 frequency;
    HRESULT result;

    if (voice == NULL) {
        return;
    }

    slot->is_spatial = (u8)spatial != 0;
    slot->parameter = parameter;

    volume_level = (i32)volume + SoundVoiceVolumeOffset;
    if (volume_level > 0) {
        volume_level = 0;
    }
    if (volume_level < -10000) {
        volume_level = -10000;
    }
    result = voice->vtable->set_volume(voice, volume_level);
    if (result < 0) {
        PCSoundFailure(result);
    }

    sample_frequency = samples[slot->sample_id].frequency;
    absolute_parameter = parameter;
    if (absolute_parameter < 0) {
        absolute_parameter = -absolute_parameter;
    }
    pitch_product = absolute_parameter * sample_frequency;
    if (pitch_product < 0) {
        pitch_product += 0xfff;
    }
    pitch_product >>= 12;
    pitch_product *= 48;
    frequency_divisor = sample_frequency / 1000;
    frequency = pitch_product / frequency_divisor;
    voice->vtable->set_frequency(voice, (DWORD)frequency);

    voice->vtable->get_status(voice, (DWORD *)&status);
    if ((status & 1) == 0) {
        voice->vtable->play(voice, 0, 0, slot->is_spatial);
    } else {
        PCSoundAlreadyPlayingHook();
    }
}

void __thiscall SoundVoiceManagerPC::SetVolumeTableIndex(i32 voice_id, i32 volume_index) {
    SoundVoiceSlotPC *slot = &voices[voice_id];
    IDirectSoundBuffer *voice = slot->voice;
    i32 table_index;
    i32 volume_level;
    HRESULT result;

    slot->volume_index = volume_index;
    if (voice == NULL) {
        return;
    }
    table_index = volume_index;
    if (table_index > 0x3fff) {
        table_index = 0;
    }
    volume_level = (i32)NuSound3VolumeTable[table_index] + SoundVoiceVolumeOffset;
    if (volume_level > 0) {
        volume_level = 0;
    }
    if (volume_level < -10000) {
        volume_level = -10000;
    }

    result = voice->vtable->set_volume(voice, volume_level);
    if (result < 0) {
        PCSoundFailure(result);
    }
}

void __thiscall SoundVoiceManagerPC::SetFrequencyParameter(i32 voice_id, i32 parameter) {
    SoundVoiceSlotPC *slot = &voices[voice_id];
    IDirectSoundBuffer *voice = slot->voice;
    i32 sample_frequency;
    i32 absolute_parameter;
    i32 frequency_product;
    i32 frequency_divisor;
    i32 frequency;

    if (voice == NULL) {
        return;
    }
    if (parameter < 0 && slot->parameter < 0) {
        return;
    }
    if (slot->parameter == parameter) {
        return;
    }

    slot->parameter = parameter;
    sample_frequency = samples[slot->sample_id].frequency;
    absolute_parameter = parameter;
    if (absolute_parameter < 0) {
        absolute_parameter = -absolute_parameter;
    }
    frequency_product = absolute_parameter * sample_frequency;
    if (frequency_product < 0) {
        frequency_product += 0xfff;
    }
    frequency_product >>= 12;
    frequency_product *= 48;
    frequency_divisor = sample_frequency / 1000;
    frequency = frequency_product / frequency_divisor;
    voice->vtable->set_frequency(voice, (DWORD)frequency);
}

void __thiscall SoundVoiceManagerPC::StopVoice(i32 voice_id) {
    SoundVoiceSlotPC *slot = &voices[voice_id];
    if (slot->voice != NULL) {
        if (slot->aux != NULL) {
            slot->aux->vtable->release(slot->aux);
        }
        slot->voice->vtable->stop(slot->voice);
        slot->voice->vtable->release(slot->voice);
        slot->voice = NULL;
        slot->aux = NULL;
        --active_count;
    }
    memset(slot, 0, sizeof(*slot));
}

void __thiscall SoundVoiceManagerPC::StopAllVoices(void) {
    SoundVoiceSlotPC *slot = voices;
    i32 count = 64;

    do {
        if (slot->voice != NULL) {
            if (slot->aux != NULL) {
                slot->aux->vtable->release(slot->aux);
            }
            slot->voice->vtable->stop(slot->voice);
            slot->voice->vtable->release(slot->voice);
            slot->voice = NULL;
            slot->aux = NULL;
            --active_count;
        }
        memset(slot, 0, sizeof(*slot));
        ++slot;
        --count;
    } while (count != 0);
}

void __thiscall SoundVoiceManagerPC::UpdateVoices(void) {
    SoundVoiceSlotPC *slot;
    i32 voice_id;
    DWORD status;

    PCSoundLoopVoiceCount = 0;
    PCSoundOneShotVoiceCount = 0;
    PCSoundUpdateClock = (i32)(timeGetTime() & 0x7fffffff);

    slot = &voices[32];
    for (voice_id = 32; voice_id < 64; ++voice_id, ++slot) {
        if (slot->voice != NULL) {
            ++PCSoundLoopVoiceCount;
            slot->voice->vtable->get_status(slot->voice, &status);
            if (slot->sound_key != 0 && (status & 4) != 0) {
                i32 elapsed = (i32)((u32)PCSoundUpdateClock - (u32)slot->field_28);
                if (elapsed < 0) {
                    elapsed = (i32)((u32)elapsed - 0x80000000u);
                }
                slot->field_19 = elapsed > slot->field_2c;
                if (slot->field_19 != 0) {
                    LONG volume = 0;
                    slot->voice->vtable->get_volume(slot->voice, &volume);
                    volume += (i32)(nuapi.frame_time * -3500.0);
                    if (volume < -7000) {
                        StopVoice(voice_id);
                    } else {
                        PCSoundSetRawVolume(slot->voice, volume);
                    }
                }
                slot->field_18 = 0;
            }
        }
    }

    slot = voices;
    for (voice_id = 0; voice_id < 32; ++voice_id, ++slot) {
        if (slot->voice != NULL) {
            ++PCSoundOneShotVoiceCount;
            slot->voice->vtable->get_status(slot->voice, &status);
            if ((status & 1) == 0) {
                StopVoice(voice_id);
            }
        }
    }
}

extern "C" void __cdecl NuSound3Update(void) {
    if (PCSoundMixDestination != NULL) {
        if (PCSoundMixSource != NULL && PCSoundMixSampleCount != 0) {
            memcpy(PCSoundMixDestination, PCSoundMixSource, (u32)PCSoundMixSampleCount * 2u);
        }
        if (PCSoundMixEvent != NULL) {
            SetEvent(PCSoundMixEvent);
        }
        if (PCSoundCommitTarget != NULL) {
            PCSoundCommitTarget->vtable->commit_deferred_settings(PCSoundCommitTarget);
        }
        SoundVoiceManager.UpdateVoices();
    }
}

void __thiscall SoundVoiceManagerPC::Setup3DVoice(i32 voice_id, const NUVEC *position, i32 update_mode) {
    SoundVoiceSlotPC *slot = &voices[voice_id];
    DWORD update_flag = update_mode == 0;
    HRESULT result;

    if (slot->aux == NULL) {
        result = slot->voice->vtable->query_interface(slot->voice, SoundVoice3DBufferIID, (void **)&slot->aux);
        if (result < 0) {
            return;
        }
        slot->aux->vtable->set_max_distance(slot->aux, 1.0e9f, update_flag);
        slot->aux->vtable->set_min_distance(slot->aux, 1.0e9f, update_flag);
        slot->aux->vtable->set_mode(slot->aux, 1, update_flag);
    }

    slot->aux->vtable->set_position(slot->aux, position->x, position->y, position->z, update_flag);
}

void __thiscall SoundVoiceManagerPC::SetSampleName(const char *name, i32 sample_index) {
    i32 name_length;

    this->samples[sample_index].valid_byte = 1;
    NuStrCpy(this->samples[sample_index].file_name, name);
    NuStrLwr(this->samples[sample_index].file_name, this->samples[sample_index].file_name);
    name_length = NuStrLen(name);
    if (name_length > 2 && this->samples[sample_index].file_name[name_length - 1] == 'l' &&
        this->samples[sample_index].file_name[name_length - 2] == '_') {
        this->samples[sample_index].file_name[name_length - 2] = '\0';
    }
    NuStrCat(this->samples[sample_index].file_name, PCSoundOggExtension);
}

extern SoundQueue *NuSound3StereoStreams[2];

struct PC_STEREO_PLAY_PACKET {
    i32 option11;
    i32 reserved;
    i32 stream_index;
    i32 first_sample;
    i32 last_sample;
    i32 volume_left;
    i32 volume_right;
    i32 option9_mode;
    f32 start_offset;
};
STATIC_ASSERT(sizeof(PC_STEREO_PLAY_PACKET) == 36);
STATIC_ASSERT(offsetof(PC_STEREO_PLAY_PACKET, stream_index) == 8);
STATIC_ASSERT(offsetof(PC_STEREO_PLAY_PACKET, first_sample) == 12);
STATIC_ASSERT(offsetof(PC_STEREO_PLAY_PACKET, last_sample) == 16);
STATIC_ASSERT(offsetof(PC_STEREO_PLAY_PACKET, option9_mode) == 28);
STATIC_ASSERT(offsetof(PC_STEREO_PLAY_PACKET, start_offset) == 32);
struct PC_STEREO_SAMPLE_METADATA {
    u8 unknown_00[8];
    i32 flags;
    u8 unknown_0c[16];
};
STATIC_ASSERT(sizeof(PC_STEREO_SAMPLE_METADATA) == 28);
STATIC_ASSERT(offsetof(PC_STEREO_SAMPLE_METADATA, flags) == 8);
extern "C" {
    extern i32 PCStereoChannel0A, PCStereoChannel0B, PCStereoChannel1A, PCStereoChannel1B;
    extern PC_STEREO_SAMPLE_METADATA *PCStereoSampleMetadata;
}
extern "C" void PCStereoStartPacket(PC_STEREO_PLAY_PACKET *packet) {
    i32 channel_a;
    i32 channel_b;
    if (packet->stream_index != 0) {
        channel_a = PCStereoChannel1A;
        channel_b = PCStereoChannel1B;
    } else {
        channel_a = PCStereoChannel0A;
        channel_b = PCStereoChannel0B;
    }
    if (packet->last_sample == -1)
        packet->last_sample = packet->first_sample + 1;
    if (channel_a != -1 && channel_b != -1) {
        i32 flags = 0;
        if (packet->option9_mode == 0)
            flags = 2;
        if (PCStereoSampleMetadata != NULL && PCStereoSampleMetadata[packet->first_sample].flags == 0)
            flags |= 1;
        NuSound3StereoStreams[packet->stream_index]->Stop();
        NuSound3StereoStreams[packet->stream_index]->SetStreamFile(
            SoundVoiceManager.samples[packet->first_sample].file_name, packet->start_offset);
        NuSound3StereoStreams[packet->stream_index]->SetStreamFlags(flags);
    }
}

SoundVoiceManagerPC *SoundVoiceManagerPC::ResetStoragePc(void) {
    memset(this, 0, sizeof(*this));
    if (PCSoundStorageLockReadyPc == 0) {
        InitializeCriticalSection(&PCSoundStorageLockPc);
        PCSoundStorageLockReadyPc = 1;
    }
    return this;
}
void SoundVoiceManagerPC::UnloadSample(i32 index) {
    EnterCriticalSection(&SoundVoiceCriticalSection);
    SoundSamplePC *sample = &samples[index];
    if (sample->buffer != NULL) {
        sample->buffer->vtable->release(sample->buffer);
        samples[index].buffer = NULL;
    }
    samples[index].buffer = NULL;
    samples[index].valid_byte = 0;
    LeaveCriticalSection(&SoundVoiceCriticalSection);
}
void SoundVoiceManagerPC::SetOptionD14Pc(u8 mode) {
    option_d14 = mode;
}
IDirectSound8 *SoundVoiceManagerPC::GetFactory(void) {
    return factory;
}
