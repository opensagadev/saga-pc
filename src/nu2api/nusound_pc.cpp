#include "saga.h"
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdarg.h>

#include <string.h>

extern "C" void __cdecl PCSoundAlreadyPlayingHook(...) {
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

extern "C" {
    extern NUVEC nusound_special_positions[5];
    extern f32 nusound_fade_start;
    extern f32 nusound_fade_end;
    extern NUMTX *NuSound3Listener;

    static i32 __cdecl PCSoundPositionVolumePc(NUVEC *relative, NUVEC *position, i32 volume) {
        f32 distance_squared;
        f32 distance;
        f32 scale;

        if (NuSound3Listener == NULL)
            NuSound3Listener = &global_camera.mtx;
        if (nusound_fade_start >= nusound_fade_end)
            nusound_fade_end = nusound_fade_start;
        NuVecInvMtxTransform(relative, position, NuSound3Listener);
        distance_squared = NuVecMagSqr(relative);
        if (distance_squared > nusound_fade_end * nusound_fade_end)
            return 0;
        distance = NuFsqrt(distance_squared);
        if (distance > nusound_fade_start)
            scale = (nusound_fade_end - distance) / (nusound_fade_end - nusound_fade_start);
        else
            scale = 1.0f;
        return (i32)(volume * scale);
    }
}

extern "C" void __cdecl PCSoundPlayPositioned(NUVEC *position, i32 sample_id, i32 volume_left, i32 volume_right,
                                              i32 pitch, f32 buzz_timer, i32 rumble_strength, f32 rumble_sustain,
                                              f32 rumble_release, i32 priority) {
    NUVEC relative;
    i32 scaled_volume;
    i32 voice_id;

    if (SoundVoiceManager.samples == NULL)
        return;
    if (SoundVoiceManager.samples[sample_id].valid_byte == 0)
        return;
    if (position >= &nusound_special_positions[1] && position <= &nusound_special_positions[4] &&
        position->x == nusound_special_positions[0].x && position->y == nusound_special_positions[0].y &&
        position->z == nusound_special_positions[0].z)
        return;
    if (PCSoundVolumeMultiplier == 0x4000)
        scaled_volume = volume_left;
    else
        scaled_volume = (PCSoundVolumeMultiplier * volume_left) >> 15;
    scaled_volume = PCSoundPositionVolumePc(&relative, position, scaled_volume);
    if (scaled_volume == 0)
        return;
    voice_id = SoundVoiceManager.AllocateVoice(sample_id, 0);
    if (voice_id <= -1)
        return;
    SoundVoiceManager.Setup3DVoice(voice_id, &relative, 1);
    if (scaled_volume > 0x3fff)
        scaled_volume = 0;
    SoundVoiceManager.SetVoice(voice_id, 0, NuSound3VolumeTable[scaled_volume], pitch);
    ++PCSoundStartedOneShotCountPc;
    (void)volume_right;
    (void)buzz_timer;
    (void)rumble_strength;
    (void)rumble_sustain;
    (void)rumble_release;
    (void)priority;
}

extern "C" void __cdecl PCSoundPlayPositionedWrapper(NUVEC *position, i32 sample_id, i32 volume_left, i32 volume_right,
                                                     i32 pitch, f32 buzz_timer, i32 rumble_strength, f32 rumble_sustain,
                                                     f32 rumble_release) {
    PCSoundPlayPositioned(position, sample_id, volume_left, volume_right, pitch, buzz_timer, rumble_strength,
                          rumble_sustain, rumble_release, 0);
}

bool __thiscall SoundVoiceManagerPC::FindLoopVoicePc(NUVEC *position, i32 sample_id, i32 *voice_id) {
    i32 index;

    if (SoundVoiceEnabledGate == 0) {
        *voice_id = -1;
        return false;
    }
    for (index = 0; index < 64; ++index) {
        if (voices[index].sample_id == sample_id && voices[index].sound_key == (i32)position &&
            voices[index].field_19 == 0) {
            *voice_id = index;
            return true;
        }
    }
    *voice_id = AllocateVoice(sample_id, 1);
    return false;
}

extern "C" void __cdecl PCSoundPlayPositionedLoop(NUVEC *position, i32 sample_id, i32 volume_left, i32 volume_right,
                                                  i32 pitch, f32 buzz_timer, i32 rumble_strength, f32 rumble_sustain,
                                                  f32 rumble_release) {
    NUVEC relative;
    i32 scaled_volume;
    i32 attenuated_volume;
    i32 voice_id;
    bool reused;
    SoundVoiceSlotPC *slot;

    if (SoundVoiceManager.samples == NULL)
        return;
    if (SoundVoiceManager.samples[sample_id].valid_byte == 0)
        return;
    if (position >= &nusound_special_positions[1] && position <= &nusound_special_positions[4] &&
        position->x == nusound_special_positions[0].x && position->y == nusound_special_positions[0].y &&
        position->z == nusound_special_positions[0].z)
        return;
    if (PCSoundVolumeMultiplier == 0x4000)
        scaled_volume = volume_left;
    else
        scaled_volume = (PCSoundVolumeMultiplier * volume_left) >> 15;
    attenuated_volume = PCSoundPositionVolumePc(&relative, position, scaled_volume);
    if (attenuated_volume == 0)
        return;
    reused = SoundVoiceManager.FindLoopVoicePc(position, sample_id, &voice_id);
    if (voice_id <= -1)
        return;
    slot = &SoundVoiceManager.voices[voice_id];
    slot->field_28 = PCSoundUpdateClock;
    slot->field_1c = nusound_fade_start;
    slot->field_20 = nusound_fade_end;
    slot->sound_key = (i32)position;
    slot->field_18 = 1;
    slot->volume_index = scaled_volume;
    slot->field_2c = NuSound3LoopHoldTime;
    if (!reused) {
        SoundVoiceManager.Setup3DVoice(voice_id, &relative, 1);
        if (attenuated_volume > 0x3fff)
            attenuated_volume = 0;
        SoundVoiceManager.SetVoice(voice_id, 1, NuSound3VolumeTable[attenuated_volume], pitch);
        ++PCSoundStartedLoopCountPc;
    } else {
        SoundVoiceManager.Setup3DVoice(voice_id, &relative, 0);
        SoundVoiceManager.SetVolumeTableIndex(voice_id, attenuated_volume);
        SoundVoiceManager.SetFrequencyParameter(voice_id, pitch);
        ++PCSoundUpdatedLoopCountPc;
    }
    (void)volume_right;
    (void)buzz_timer;
    (void)rumble_strength;
    (void)rumble_sustain;
    (void)rumble_release;
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
    SAGA_SOUND_CHECK("NuSound3Update before request copy");
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

void SoundVoiceManagerPC::Init(i32 unused, u8 option5_value, u8 option6_value) {
    (void)unused;
    option5 = option5_value;
    option6 = option6_value;
    if (FAILED(DirectSoundCreate8(NULL, &factory, NULL)))
        return;
    factory->vtable->set_cooperative_level(factory, (HWND)PCInputInstance_candidate, 2);
    caps.size = sizeof(caps);
    factory->vtable->get_caps(factory, &caps);
    factory->vtable->get_speaker_config(factory, &speaker_config);
    memset(&primary_desc, 0, sizeof(primary_desc));
    primary_desc.size = sizeof(primary_desc);
    primary_desc.flags = 0x11;
    primary_desc.bytes = 0;
    primary_desc.format = NULL;
    factory->vtable->create_sound_buffer(factory, &primary_desc, &primary_buffer, NULL);
    if (primary_buffer != NULL) {
        WAVEFORMATEX format;
        format.wFormatTag = 1;
        format.nChannels = 2;
        format.nSamplesPerSec = 44100;
        format.nAvgBytesPerSec = 176400;
        format.nBlockAlign = 4;
        format.wBitsPerSample = 16;
        format.cbSize = 0;
        primary_buffer->vtable->set_format(primary_buffer, &format);
    }
    primary_buffer->vtable->query_interface(primary_buffer, PCSoundListenerIIDPc, (void **)&PCSoundCommitTarget);
    if (PCSoundCommitTarget != NULL) {
        PC_SOUND_COMMIT_PC *listener = PCSoundCommitTarget;
        listener->vtable->set_position(listener, 0, 0, 0, 1);
        listener = PCSoundCommitTarget;
        listener->vtable->set_orientation(listener, 0, 0, 1, 0, 1, 0, 1);
        listener = PCSoundCommitTarget;
        listener->vtable->set_rolloff_factor(listener, 0, 1);
        listener = PCSoundCommitTarget;
        listener->vtable->commit_deferred_settings(listener);
    }
    memset(voices, 0, sizeof(voices));
    NuSound3StereoStreams[0] = new SoundQueue(SoundVoiceManager.factory, 6);
    NuSound3StereoStreams[1] = new SoundQueue(SoundVoiceManager.factory, 6);
}

STATIC_ASSERT(offsetof(SoundSamplePC, failed_byte) == 0x405);
STATIC_ASSERT(offsetof(IDirectSound8Vtbl, create_sound_buffer) == 0xc);
STATIC_ASSERT(offsetof(IDirectSoundBufferVtbl, lock) == 0x2c);
STATIC_ASSERT(offsetof(IDirectSoundBufferVtbl, unlock) == 0x4c);
extern "C" i32 PCSampleLoadEnabledPc;
extern "C" i32 PCSoundSampleLimitPc;
extern "C" const GUID PCSampleBufferAlgorithmPc;
extern "C" char *PCSampleFilenameExtensionPc;

i32 SoundVoiceManagerPC::LoadWavSamplePc(const char *name, i32 index) {
    WavReader reader;
    WAVEFORMATEX format;
    PC_DS_BUFFER_DESC_VIEW descriptor;
    char path[0x104];
    char directory[0x104];
    void *first, *second;
    DWORD first_bytes, second_bytes;
    u32 read_bytes;
    if (PCSampleLoadEnabledPc == 0) {
        return -1;
    }
    if (index > PCSoundSampleLimitPc) {
        PCSoundAlreadyPlayingHook();
    }
    if (factory == NULL) {
        samples[index].failed_byte = 1;
        return 0;
    }
    if (NuSoundDat != NULL) {
        sprintf(path, "%s", name);
    } else {
        NuFileGetCurrentDirectory(directory);
        sprintf(path, "%s%s", directory, name);
    }
    if (reader.Open(NuSoundDat, path, &format) < 0) {
        samples[index].failed_byte = 1;
        return 0;
    }
    memset(&descriptor, 0, sizeof(descriptor));
    descriptor.size = sizeof(descriptor);
    descriptor.flags = 0xb8;
    descriptor.algorithm = PCSampleBufferAlgorithmPc;
    descriptor.bytes = reader.GetSize();
    descriptor.format = &format;
    if (descriptor.bytes > 0x0fffffff) {
        PCSoundAlreadyPlayingHook(path, 0x0fffffff);
    }
    if (descriptor.bytes < 4) {
        PCSoundAlreadyPlayingHook(path, 4);
    }
    samples[index].frequency = format.nSamplesPerSec;
    HRESULT result = factory->vtable->create_sound_buffer(factory, &descriptor, &samples[index].buffer, NULL);
    if (result < 0) {
        PCSoundFailure(path);
        reader.Close();
        samples[index].failed_byte = 1;
        return 0;
    }
    result = samples[index].buffer->vtable->lock(samples[index].buffer, 0, descriptor.bytes, &first, &first_bytes,
                                                 &second, &second_bytes, 0);
    if (result < 0) {
        PCSoundFailure(path);
        reader.Close();
        samples[index].failed_byte = 1;
        return 0;
    }
    result = reader.Read(first, first_bytes, &read_bytes);
    if (result < 0 || read_bytes != descriptor.bytes) {
        PCSoundFailure(path);
        samples[index].failed_byte = 1;
    }
    samples[index].buffer->vtable->unlock(samples[index].buffer, first, first_bytes, second, second_bytes);
    reader.Close();
    samples[index].valid_byte = 1;
    return 1;
}

void __thiscall SoundVoiceManagerPC::ClearSamplesPc(void) {
    i32 i;
    EnterCriticalSection(&SoundSampleCriticalSectionPc);
    for (i = 0; i < 64; ++i) {
        if (voices[i].voice != NULL) {
            voices[i].voice->vtable->release(voices[i].voice);
            voices[i].voice = NULL;
        }
    }
    for (i = 0; i < PCSoundSampleLimitPc; ++i) {
        if (samples[i].buffer != NULL) {
            samples[i].buffer->vtable->release(samples[i].buffer);
            samples[i].buffer = NULL;
        }
        samples[i].valid_byte = 0;
        samples[i].failed_byte = 0;
    }
    active_count = 0;
    state_000 = 0;
    LeaveCriticalSection(&SoundSampleCriticalSectionPc);
}
void __cdecl NuSound3SetSampleTable(SFX_FILE_INFO_PC *info, VARIPTR *cursor, VARIPTR end) {
    char directory[0x800];
    char filename[0x800];
    i32 count = 0;
    i32 i;
    f32 value;
    if (PCStereoSampleMetadata == NULL)
        SoundVoiceManager.Init(0, 0, 1);
    else
        SoundVoiceManager.ClearSamplesPc();
    PCStereoSampleMetadata = (PC_STEREO_SAMPLE_METADATA *)info;
    SoundSampleCountPc = 0;
    while (info[count].index != -1) {
        if (count >= PCSoundSampleLimitPc) {
            SoundSampleCountPc = count;
            PCSoundFailure();
            break;
        }
        ++count;
    }
    SoundSampleCountPc = count;
    for (i = 0; i <= 0x3fff; ++i) {
        if (i == 0)
            NuSound3VolumeTable[i] = -10000.0f;
        else {
            value = (f32)(i / 16383.0);
            value = (f32)log10((f64)value);
            NuSound3VolumeTable[i] = (f32)(value * 2000.0);
        }
    }
    for (i = 0; i < SoundSampleCountPc; ++i) {
        NuStrLen(((SFX_FILE_INFO_PC *)PCStereoSampleMetadata)[i].filename);
        NuFileGetCurrentDirectory(directory);
        sprintf(filename, "%s%s", directory, ((SFX_FILE_INFO_PC *)PCStereoSampleMetadata)[i].filename);
        if (((SFX_FILE_INFO_PC *)PCStereoSampleMetadata)[i].index < 0x1000)
            SoundVoiceManager.SetSampleName(filename, i);
    }
}
void __cdecl NuSound3SetRequestTable(u16 *request_bits, i32 short_count) {
    SAGA_SOUND_SETTER_BEGIN();
    if (PCSoundMixSampleCount != short_count) {
        if (PCSoundMixDestination != NULL)
            PCSoundAlreadyPlayingHook();
        PCSoundMixDestination = (u16 *)NuAlloc(short_count * 2, ".\\pc\\nusound.cpp", 0x8c4);
        memset(PCSoundMixDestination, 0, short_count * 2);
    }
    PCSoundMixSampleCount = short_count;
    PCSoundMixSource = request_bits;
    SAGA_SOUND_SETTER_END(request_bits, short_count, PCSoundMixDestination);
}

extern "C" {
    static DWORD WINAPI PCSoundSampleWorkerPc(void *unused) {
        char directory[0x104];
        char filename[0x104];
        i32 i;
        u8 requested, loaded;
        SFX_FILE_INFO_PC *info;
        for (;;) {
            WaitForSingleObject(PCSoundMixEvent, INFINITE);
            if (PCSoundSampleWorkerStopPc != 0)
                break;
            if (PCSoundMixDestination != NULL) {
                EnterCriticalSection(&SoundSampleCriticalSectionPc);
                for (i = 0; i < SoundSampleCountPc; ++i) {
                    if (PCSoundSampleWorkerStopPc != 0)
                        break;
                    requested = (((u16 *)PCSoundMixDestination)[i / 16] & (1 << (i & 15))) != 0;
                    loaded = SoundVoiceManager.samples != NULL ? SoundVoiceManager.samples[i].valid_byte : 0;
                    if (requested == loaded)
                        continue;
                    info = (SFX_FILE_INFO_PC *)PCStereoSampleMetadata;
                    if (requested != 0) {
                        if (SoundVoiceManager.samples != NULL && SoundVoiceManager.samples[i].failed_byte != 0)
                            continue;
                        NuFileGetCurrentDirectory(directory);
                        sprintf(filename, "%s%s", directory, info[i].filename);
                        if (info[i].index < 0x1000)
                            continue;
                        if (NuStrICmp(NuFileExt_wav_PC, PCSampleFilenameExtensionPc) == 0)
                            SoundVoiceManager.LoadWavSamplePc(info[i].filename, i);
                        else
                            PCSoundAlreadyPlayingHook(PCSampleFilenameExtensionPc);
                        if (SoundVoiceManager.samples != NULL && SoundVoiceManager.samples[i].failed_byte != 0)
                            PCSoundFailure(i);
                    } else if (info[i].index >= 0x1000) {
                        SoundVoiceManager.UnloadSample(i);
                    }
                }
                LeaveCriticalSection(&SoundSampleCriticalSectionPc);
            }
            if (PCSoundSampleWorkerStopPc != 0)
                break;
        }
        PCSoundSampleWorkerHandlePc = NULL;
        return 0;
    }
    i32 __cdecl PCSoundInitializePc(PC_SOUND_INIT_CONFIG *config) {
        VARIPTR zero;
        SoundVoiceManager.samples = (SoundSamplePC *)((config->cursor->addr + 3) & ~3u);
        config->cursor->addr = (config->cursor->addr + 3) & ~3u;
        config->cursor->addr += config->sample_capacity * sizeof(SoundSamplePC);
        if (config->cursor->addr > config->end.addr)
            PCSoundAlreadyPlayingHook();
        memset(SoundVoiceManager.samples, 0, config->sample_capacity * sizeof(SoundSamplePC));
        PCSoundSampleLimitPc = config->sample_capacity;
        zero.addr = 0;
        if (config->sample_table != NULL)
            NuSound3SetSampleTable(config->sample_table, NULL, zero);
        InitializeCriticalSection(&SoundVoiceCriticalSection);
        InitializeCriticalSection(&SoundSampleCriticalSectionPc);
        PCSoundMixEvent = CreateEventA(NULL, FALSE, FALSE, "SpotManager Event");
        if (PCSoundMixEvent != NULL) {
            PCSoundSampleWorkerHandlePc = CreateThread(NULL, 0, PCSoundSampleWorkerPc, NULL, CREATE_SUSPENDED, NULL);
            if (PCSoundSampleWorkerHandlePc == NULL) {
                PCSoundFailure();
                DeleteObject((HGDIOBJ)PCSoundMixEvent);
                PCSoundMixEvent = NULL;
            }
            SetThreadPriority(PCSoundSampleWorkerHandlePc, THREAD_PRIORITY_LOWEST);
            ResumeThread(PCSoundSampleWorkerHandlePc);
        } else {
            PCSoundFailure();
        }
        NuSound3Initialized = 1;
        return 1;
    }
}

extern "C" void __cdecl NuSound3StopSFX(void) {
    SoundVoiceManager.StopAllVoices();
}

extern "C" void __cdecl NuSound3SetSFXPitch(i32 pitch) {
    (void)pitch;
}

extern "C" void __cdecl NuSound3CancelCheckStereo(void) {
}
