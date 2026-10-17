#include "saga.h"
#include "../audio/vorbis/vorbis/vorbisfile.h"
#include <stdio.h>
struct PC_OGG_CACHE_RECORD_PC {
    char path[0x104];
    u8 *data;
    i32 bytes;
    u32 last_used;
    i32 references;
};
STATIC_ASSERT(sizeof(PC_OGG_CACHE_RECORD_PC) == 0x114);
STATIC_ASSERT(offsetof(PC_OGG_CACHE_RECORD_PC, data) == 0x104);
STATIC_ASSERT(offsetof(PC_OGG_CACHE_RECORD_PC, bytes) == 0x108);
STATIC_ASSERT(offsetof(PC_OGG_CACHE_RECORD_PC, last_used) == 0x10c);
STATIC_ASSERT(offsetof(PC_OGG_CACHE_RECORD_PC, references) == 0x110);
#pragma pack(push, 4)
class OggReader : public WavReader {
  public:
    OggReader(void);
    virtual ~OggReader(void);
    virtual HRESULT Open(NUDATHDR *, char *, WAVEFORMATEX *);
    virtual HRESULT Unknown08Pc(u32, u32);
    virtual HRESULT Close(void);
    virtual WAVEFORMATEX *GetFormat(void);
    virtual i32 GetSize(void);
    virtual HRESULT Reset(void);
    virtual HRESULT Read(void *, u32, u32 *);
    virtual i32 SeekTime(f32);
    static u32 __cdecl ReadCallbackPc(void *, u32, u32, void *);
    static i32 __cdecl SeekCallbackPc(void *, i64, i32);
    static i32 __cdecl CloseCallbackPc(void *);
    // NOLINTNEXTLINE(google-runtime-int): Original upstream long type preserves the native C ABI.
    static long __cdecl TellCallbackPc(void *);
    u8 compressed;
    u8 unknown_041[7];
    OggVorbis_File vorbis_state;
    PC_OGG_CACHE_RECORD_PC *cache;
    i32 position;
    WAVEFORMATEX compressed_format;
    u8 unknown_332[6];
};
#pragma pack(pop)
STATIC_ASSERT(sizeof(OggReader) == 0x338);
STATIC_ASSERT(offsetof(OggReader, compressed) == 0x40);
STATIC_ASSERT(offsetof(OggReader, vorbis_state) == 0x48);
STATIC_ASSERT(offsetof(OggReader, cache) == 0x318);
STATIC_ASSERT(offsetof(OggReader, position) == 0x31c);
STATIC_ASSERT(offsetof(OggReader, compressed_format) == 0x320);
STATIC_ASSERT(offsetof(PC_OGG_CACHE_RECORD_PC, path) == 0);
STATIC_ASSERT(sizeof(OggVorbis_File) == 0x2d0);
STATIC_ASSERT(offsetof(OggVorbis_File, callbacks) == 0x2c0);

extern "C" void __cdecl NuSound3GetStreamInfo(i32 stream_index, void *info) {
    (void)stream_index;
    (void)info;
}

extern SoundQueue *NuSound3StereoStreams[2];
extern "C" f32 NuSound3VolumeTable[0x4000];
extern "C" i32 NuSound3VolumeOffset;

extern "C" void __cdecl NuSound3KillAllAudio(void) {
    NuSound3StereoStreams[0]->Stop();
    NuSound3StereoStreams[1]->Stop();
    SoundVoiceManager.StopAllVoices();
}

i32 SoundQueue::SendCommand(i32 command_value, i32 argument_value) {
    i32 success = 0;
    EnterCriticalSection(&lock);
    if (count < 32) {
        command[write_index] = command_value;
        argument[write_index] = argument_value;
        write_index = (write_index + 1) & 31;
        ++count;
        success = 1;
    }
    LeaveCriticalSection(&lock);
    if (success != 0) {
        SetEvent(event);
    }
    return success;
}

i32 SoundQueue::Stop() {
    if (this == NULL) {
        return 0;
    }
    if (active == 0) {
        return 0;
    }
    return SendCommand(3, 0);
}

i32 SoundQueue::Pause() {
    if (this == NULL) {
        return 0;
    }
    if (active == 0) {
        return 0;
    }
    return SendCommand(5, 0);
}

i32 SoundQueue::Resume() {
    if (this == NULL) {
        return 0;
    }
    if (active == 0) {
        return 0;
    }
    return SendCommand(4, 0);
}

i32 SoundQueue::SetVolume(i32 volume) {
    if (this == NULL) {
        return 0;
    }
    EnterCriticalSection(&volume_lock);
    requested_volume = volume;
    if (volume_sink != NULL) {
        volume_sink->SetVolume(requested_volume);
        applied_volume = requested_volume;
    }
    LeaveCriticalSection(&volume_lock);
    return 1;
}

i32 SoundQueue::HasCommand(i32 command_value) {
    if (this == NULL) {
        return 0;
    }
    i32 found = 0;
    EnterCriticalSection(&lock);
    for (i32 index = 0; index < count; ++index) {
        if (command[index] == command_value) {
            found = 1;
            break;
        }
    }
    LeaveCriticalSection(&lock);
    return found;
}

i32 SoundQueue::IsStreamIdle() {
    if (this == NULL) {
        return 1;
    }
    if (count != 0) {
        return 0;
    }
    if (stream_state != 0 && (stream_state != 3 || stream_flags == 0)) {
        return 0;
    }
    return 1;
}

i32 SoundQueue::GetStatus() {
    if (this == NULL) {
        return 0;
    }
    if (HasCommand(3) != 0) {
        return 2;
    }
    if (stream_state == 2 || (stream_state == 3 && stream_flags != 0)) {
        return 1;
    }
    return IsStreamIdle() != 0 ? 0 : 2;
}

void SoundQueue::StartTimer() {
    if (timer_id != 0) {
        timeKillEvent(timer_id);
    }
    timer_id = timeSetEvent(500, 100, timer_callback, 0, 0x21);
}

void SoundQueue::StopTimer() {
    if (timer_id != 0) {
        timeKillEvent(timer_id);
    }
    timer_id = 0;
}

i32 SoundQueue::SetStreamFile(const char *path, f32 offset) {
    if (this == NULL || active == 0) {
        return 0;
    }
    NuStrNCpy(stream_path, path, sizeof(stream_path));
    start_offset = offset;
    return SendCommand(1, 0);
}

i32 SoundQueue::SetStreamFlags(i32 flags) {
    if (this == NULL || active == 0) {
        return 0;
    }
    return SendCommand(2, flags);
}

extern "C" void __cdecl NuSound3StopStereoStream(i32 index) {
    if (index != 0) {
        index = 1;
    }
    NuSound3StereoStreams[index]->Stop();
}

extern "C" void __cdecl NuSound3PauseStereoStream(i32 index) {
    if (index != 0) {
        index = 1;
    }
    NuSound3StereoStreams[index]->Pause();
}

extern "C" void __cdecl NuSound3ResumeStereoStream(i32 index) {
    if (index != 0) {
        index = 1;
    }
    NuSound3StereoStreams[index]->Resume();
}

extern "C" i32 __cdecl NuSound3SetStreamVolume(i32 index, i32 volume) {
    if (index != 0) {
        index = 1;
    }
    if (volume > 0x3fff) {
        volume = 0;
    }
    i32 level = (i32)NuSound3VolumeTable[volume] + NuSound3VolumeOffset;
    if (level < -10000) {
        level = -10000;
    }
    return NuSound3StereoStreams[index]->SetVolume(level);
}

extern "C" i32 __cdecl NuSound3GetStereoStreamStatus(i32 index) {
    if (index != 0) {
        index = 1;
    }
    return NuSound3StereoStreams[index]->GetStatus();
}

void SoundQueue::CancelPausePc(void) {
    if (this == NULL || unknown_03f4 == 0)
        return;
    EnterCriticalSection(&volume_lock);
    IDirectSoundBuffer *buffer = (IDirectSoundBuffer *)volume_sink;
    if (stream_state == 2 && buffer != NULL)
        buffer->vtable->play(buffer, 0, 0, 1);
    LeaveCriticalSection(&volume_lock);
    unknown_03f4 = 0;
    LeaveCriticalSection(&operation_lock);
}
SoundQueue::~SoundQueue(void) {
    if (this == NULL)
        return;
    if (active != 0) {
        if (unknown_03f4 != 0)
            CancelPausePc();
        if (active != 0)
            SendCommand(3, 0);
        SendCommand(6, 0);
        if (thread != NULL) {
            do {
                Sleep(10);
            } while (thread != NULL);
        }
    }
    if (decoder != NULL)
        decoder->vtable->destroy(decoder, 1);
    decoder = NULL;
    DeleteCriticalSection(&volume_lock);
    DeleteCriticalSection(&operation_lock);
    DeleteCriticalSection(&lock);
    if (event != INVALID_HANDLE_VALUE)
        CloseHandle(event);
    if ((HANDLE)timer_callback != INVALID_HANDLE_VALUE)
        CloseHandle((HANDLE)timer_callback);
    event = INVALID_HANDLE_VALUE;
    timer_callback = (LPTIMECALLBACK)INVALID_HANDLE_VALUE;
    if (timer_id != 0)
        timeKillEvent(timer_id);
    timer_id = 0;
    IDirectSoundBuffer *buffer = (IDirectSoundBuffer *)volume_sink;
    if (buffer != NULL)
        buffer->vtable->release(buffer);
    volume_sink = NULL;
    active = 0;
}
i32 SoundQueue::SetFormatPc(const WAVEFORMATEX *format) {
    if (volume_sink != NULL) {
        EnterCriticalSection(&volume_lock);
        IDirectSoundBuffer *buffer = (IDirectSoundBuffer *)volume_sink;
        buffer->vtable->release(buffer);
        volume_sink = NULL;
        wave_format.nSamplesPerSec = format->nSamplesPerSec;
        wave_format.nChannels = format->nChannels;
        wave_format.nBlockAlign = format->nBlockAlign;
        wave_format.wBitsPerSample = format->wBitsPerSample;
        wave_format.nAvgBytesPerSec = format->nAvgBytesPerSec;
        buffer_seconds = 4;
        memset(&descriptor, 0, sizeof(descriptor));
        descriptor.size = sizeof(descriptor);
        descriptor.flags = 0x10080;
        descriptor.bytes = buffer_seconds * wave_format.nAvgBytesPerSec;
        descriptor.format = &wave_format;
        IDirectSoundBuffer *temporary = NULL;
        volume_sink = NULL;
        if (SUCCEEDED(factory->vtable->create_sound_buffer(factory, &descriptor, &temporary, NULL))) {
            if (SUCCEEDED(temporary->vtable->query_interface(temporary, PCSoundBuffer8IIDPc, (void **)&volume_sink))) {
                buffer = (IDirectSoundBuffer *)volume_sink;
                buffer->vtable->set_volume(buffer, requested_volume);
                applied_volume = requested_volume;
            }
            temporary->vtable->release(temporary);
        }
        LeaveCriticalSection(&volume_lock);
    }
    return 0;
}
i32 SoundQueue::FillBufferPc(i32 bytes, u32 flags) {
    if (stream_state != 1 && stream_state != 2)
        return 0;
    IDirectSoundBuffer *buffer = (IDirectSoundBuffer *)volume_sink;
    if (buffer == NULL)
        return 0;
    DWORD play_cursor;
    HRESULT result = buffer->vtable->get_current_position(buffer, &play_cursor, NULL);
    i32 requested = bytes;
    i32 total = buffer_seconds * wave_format.nAvgBytesPerSec;
    if (SUCCEEDED(result)) {
        i32 available = (i32)play_cursor - buffer_write_position;
        if (available < 0)
            available += total;
        if ((flags & 2) == 0) {
            requested = 2 * wave_format.nAvgBytesPerSec;
            if (available < requested)
                requested = available;
            if (buffer_write_position + requested > total)
                requested = total - buffer_write_position;
        }
    }
    if ((flags & 2) == 0 && (i32)play_cursor >= buffer_write_position &&
        (i32)play_cursor < buffer_write_position + requested)
        return 0;
    i32 amount = decoder->vtable->get_size(decoder) - decoded_bytes;
    i32 decoded;
    if (requested < amount)
        decoded = requested;
    else
        decoded = decoder->vtable->get_size(decoder) - decoded_bytes;
    void *data, *second;
    DWORD data_bytes, second_bytes;
    buffer = (IDirectSoundBuffer *)volume_sink;
    result =
        buffer->vtable->lock(buffer, buffer_write_position, requested, &data, &data_bytes, &second, &second_bytes, 0);
    if (result == (HRESULT)0x88780096) {
        buffer = (IDirectSoundBuffer *)volume_sink;
        if (FAILED(buffer->vtable->restore(buffer)))
            return 0;
        buffer = (IDirectSoundBuffer *)volume_sink;
        result = buffer->vtable->lock(buffer, buffer_write_position, requested, &data, &data_bytes, &second,
                                      &second_bytes, 0);
    }
    if (FAILED(result))
        return 0;
    if (data_bytes < (u32)requested) {
        buffer = (IDirectSoundBuffer *)volume_sink;
        buffer->vtable->unlock(buffer, data, 0, second, 0);
    } else {
        if ((flags & 1) != 0)
            decoded = 0;
        else if (decoded != 0) {
            u32 read_count;
            if (FAILED(decoder->vtable->read(decoder, data, decoded, &read_count)) || read_count != (u32)decoded) {
                decoded_bytes = decoder->vtable->get_size(decoder);
                decoded = 0;
                buffer = (IDirectSoundBuffer *)volume_sink;
                if (SUCCEEDED(buffer->vtable->stop(buffer)))
                    stream_state = 0;
            }
        }
        if ((loop_flags & 1) != 0) {
            if (decoded < requested) {
                do {
                    decoder->vtable->reset(decoder);
                    decoded_bytes = 0;
                    i32 remaining = requested - decoded;
                    i32 available = decoder->vtable->get_size(decoder) - decoded_bytes;
                    i32 block = remaining;
                    if (available < remaining)
                        block = decoder->vtable->get_size(decoder) - decoded_bytes;
                    u32 read_count;
                    if (FAILED(decoder->vtable->read(decoder, (u8 *)data + decoded, block, &read_count)) ||
                        read_count != (u32)block) {
                        decoded_bytes = decoder->vtable->get_size(decoder);
                        memset(data, 0, requested);
                        buffer = (IDirectSoundBuffer *)volume_sink;
                        if (SUCCEEDED(buffer->vtable->stop(buffer)))
                            stream_state = 0;
                        decoded = block;
                        break;
                    }
                    decoded += block;
                    if (decoded >= requested) {
                        decoded = block;
                        break;
                    }
                } while (1);
            }
        } else if (decoded < requested) {
            memset((u8 *)data + decoded, 0, requested - decoded);
            total = buffer_seconds * wave_format.nAvgBytesPerSec;
            if (decoded != 0) {
                PCSoundDrainRemainingPc = total;
                PCSoundDrainStartPc = play_cursor;
            } else {
                i32 elapsed;
                if ((i32)play_cursor >= PCSoundDrainStartPc)
                    elapsed = (i32)play_cursor - PCSoundDrainStartPc;
                else
                    elapsed = total - PCSoundDrainStartPc + play_cursor;
                PCSoundDrainRemainingPc -= elapsed;
                PCSoundDrainStartPc = play_cursor;
                if (PCSoundDrainRemainingPc <= 0)
                    ++pending_bytes;
            }
        }
        buffer = (IDirectSoundBuffer *)volume_sink;
        buffer->vtable->unlock(buffer, data, requested, second, 0);
        decoded_bytes += decoded;
        buffer_write_position =
            (u32)(buffer_write_position + requested) % (u32)(buffer_seconds * wave_format.nAvgBytesPerSec);
    }
    return 1;
}
DWORD SoundQueue::Run(void) {
    i32 quitting = 0;
    i32 command_arg;
    do {
        DWORD wait = WaitForMultipleObjects(2, wait_events, FALSE, INFINITE);
        if (wait == WAIT_OBJECT_0 + 1) {
            ResetEvent(event);
            i32 cmd = 0;
            EnterCriticalSection(&lock);
            if (count > 0) {
                command_arg = argument[read_index];
                cmd = command[read_index];
                read_index = (read_index + 1) & 31;
                --count;
            }
            LeaveCriticalSection(&lock);
            while (cmd != 0) {
                EnterCriticalSection(&operation_lock);
                IDirectSoundBuffer *buffer = (IDirectSoundBuffer *)volume_sink;
                switch (cmd) {
                    case 1: {
                        NuStrNCpy(current_path, stream_path, 0x104);
                        current_offset = start_offset;
                        if (stream_state == 2) {
                            if (timer_id != 0)
                                timeKillEvent(timer_id);
                            timer_id = 0;
                            buffer = (IDirectSoundBuffer *)volume_sink;
                            if (buffer != NULL)
                                buffer->vtable->stop(buffer);
                        }
                        stream_flags = 0;
                        stream_state = 0;
                        pending_bytes = 0;
                        decoded_bytes = 0;
                        buffer_write_position = 0;
                        unknown_0004 = -1;
                        decoder->vtable->close(decoder);
                        WAVEFORMATEX format;
                        if (SUCCEEDED(decoder->vtable->open(decoder, NULL, current_path, &format))) {
                            if (wave_format.nSamplesPerSec != format.nSamplesPerSec ||
                                wave_format.nChannels != format.nChannels)
                                SetFormatPc(&format);
                            unknown_0004 = 1;
                        }
                        break;
                    }
                    case 2:
                        if ((i32)unknown_0004 < 0) {
                            if (timer_id != 0)
                                timeKillEvent(timer_id);
                            timer_id = 0;
                        } else if (stream_state != 2) {
                            pending_bytes = 0;
                            stream_flags = 0;
                            decoded_bytes = 0;
                            buffer_write_position = 0;
                            loop_flags = 0;
                            decoder->vtable->reset(decoder);
                            i32 duration = buffer_seconds * 1000;
                            if (duration < 2000)
                                duration = 2000;
                            u32 preload = (u32)(wave_format.nAvgBytesPerSec * duration) / 1000;
                            stream_state = 1;
                            decoded_bytes = decoder->vtable->seek_time(decoder, current_offset);
                            buffer = (IDirectSoundBuffer *)volume_sink;
                            if (buffer != NULL)
                                buffer->vtable->set_current_position(buffer, 0);
                            FillBufferPc(preload, 2);
                            buffer = (IDirectSoundBuffer *)volume_sink;
                            if (buffer != NULL && SUCCEEDED(buffer->vtable->set_current_position(buffer, 0))) {
                                if ((command_arg & 2) != 0) {
                                    stream_state = 3;
                                    loop_flags = command_arg;
                                    StartTimer();
                                    break;
                                }
                                buffer = (IDirectSoundBuffer *)volume_sink;
                                if (SUCCEEDED(buffer->vtable->play(buffer, 0, 0, 1))) {
                                    stream_flags = 1;
                                    stream_state = 2;
                                    loop_flags = command_arg;
                                    StartTimer();
                                    break;
                                }
                            }
                            if (timer_id != 0)
                                timeKillEvent(timer_id);
                            timer_id = 0;
                        }
                        break;
                    case 3:
                        if (timer_id != 0)
                            timeKillEvent(timer_id);
                        timer_id = 0;
                        buffer = (IDirectSoundBuffer *)volume_sink;
                        if (buffer != NULL && SUCCEEDED(buffer->vtable->stop(buffer))) {
                            buffer_write_position = 0;
                            FillBufferPc(buffer_seconds * wave_format.nAvgBytesPerSec, 3);
                        }
                        stream_state = 0;
                        break;
                    case 4:
                        if (stream_state == 2) {
                            if (timer_id != 0)
                                timeKillEvent(timer_id);
                            timer_id = 0;
                            buffer = (IDirectSoundBuffer *)volume_sink;
                            if (buffer != NULL && SUCCEEDED(buffer->vtable->stop(buffer)))
                                stream_state = 3;
                        }
                        break;
                    case 5:
                        if (stream_state == 3 && buffer != NULL && SUCCEEDED(buffer->vtable->play(buffer, 0, 0, 1))) {
                            stream_state = 2;
                            stream_flags = 1;
                            if (timer_id != 0)
                                timeKillEvent(timer_id);
                            timer_id = timeSetEvent(500, 100, timer_callback, 0, 0x21);
                        }
                        break;
                    case 6:
                        thread = NULL;
                        quitting = 1;
                        break;
                }
                LeaveCriticalSection(&operation_lock);
                cmd = 0;
                EnterCriticalSection(&lock);
                if (count > 0) {
                    command_arg = argument[read_index];
                    cmd = command[read_index];
                    read_index = (read_index + 1) & 31;
                    --count;
                }
                LeaveCriticalSection(&lock);
                if (applied_volume != requested_volume || applied_volume == 1) {
                    EnterCriticalSection(&volume_lock);
                    buffer = (IDirectSoundBuffer *)volume_sink;
                    if (buffer != NULL) {
                        buffer->vtable->set_volume(buffer, requested_volume);
                        applied_volume = requested_volume;
                    }
                    LeaveCriticalSection(&volume_lock);
                }
            }
        } else if (wait == WAIT_OBJECT_0 && (i32)unknown_0004 >= 0) {
            FillBufferPc((u32)(buffer_seconds * wave_format.nAvgBytesPerSec) >> 2, 0);
            if (pending_bytes > 0 && stream_state == 2) {
                IDirectSoundBuffer *buffer = (IDirectSoundBuffer *)volume_sink;
                if (buffer != NULL)
                    buffer->vtable->stop(buffer);
                stream_state = 0;
                if (timer_id != 0)
                    timeKillEvent(timer_id);
                timer_id = 0;
            }
        }
        if (applied_volume != requested_volume || applied_volume == 1) {
            EnterCriticalSection(&volume_lock);
            IDirectSoundBuffer *buffer = (IDirectSoundBuffer *)volume_sink;
            if (buffer != NULL) {
                buffer->vtable->set_volume(buffer, requested_volume);
                applied_volume = requested_volume;
            }
            LeaveCriticalSection(&volume_lock);
        }
    } while (quitting == 0);
    return 0;
}
extern "C" DWORD __stdcall PCSoundStreamWorkerPc(void *context) {
    return ((SoundQueue *)context)->Run();
}

SoundQueue::SoundQueue(IDirectSound8 *factory_value, i32 mode) {
    serial = PCSoundStreamSerialPc++;
    factory = factory_value;
    active = 0;
    format_mode = mode;
    memset(&wave_format, 0, sizeof(wave_format));
    memset(pending_path, 0, sizeof(pending_path));
    memset(current_path, 0, sizeof(current_path));
    wave_format.wFormatTag = 1;
    switch (mode) {
        case 1:
            wave_format.nChannels = 1;
            wave_format.nSamplesPerSec = 22050;
            wave_format.nBlockAlign = 1;
            wave_format.wBitsPerSample = 8;
            buffer_seconds = 4;
            break;
        case 2:
            wave_format.nChannels = 1;
            wave_format.nSamplesPerSec = 22050;
            wave_format.nBlockAlign = 2;
            wave_format.wBitsPerSample = 16;
            buffer_seconds = 3;
            break;
        case 3:
            wave_format.nChannels = 2;
            wave_format.nSamplesPerSec = 22050;
            wave_format.nBlockAlign = 2;
            wave_format.wBitsPerSample = 8;
            buffer_seconds = 3;
            break;
        case 4:
            wave_format.nChannels = 2;
            wave_format.nSamplesPerSec = 22050;
            wave_format.nBlockAlign = 4;
            wave_format.wBitsPerSample = 16;
            buffer_seconds = 3;
            break;
        case 5:
            wave_format.nChannels = 1;
            wave_format.nSamplesPerSec = 44100;
            wave_format.nBlockAlign = 2;
            wave_format.wBitsPerSample = 16;
            buffer_seconds = 3;
            break;
        case 6:
            wave_format.nChannels = 2;
            wave_format.nSamplesPerSec = 44100;
            wave_format.nBlockAlign = 4;
            wave_format.wBitsPerSample = 16;
            buffer_seconds = 4;
            break;
        case 7:
            wave_format.nChannels = 1;
            wave_format.nSamplesPerSec = 48000;
            wave_format.nBlockAlign = 2;
            wave_format.wBitsPerSample = 16;
            buffer_seconds = 3;
            break;
        case 8:
            wave_format.nChannels = 2;
            wave_format.nSamplesPerSec = 48000;
            wave_format.nBlockAlign = 4;
            wave_format.wBitsPerSample = 16;
            buffer_seconds = 3;
            break;
    }
    wave_format.nAvgBytesPerSec = wave_format.nBlockAlign * wave_format.nSamplesPerSec;
    memset(&descriptor, 0, sizeof(descriptor));
    descriptor.size = sizeof(descriptor);
    descriptor.flags = 0x10080;
    descriptor.bytes = buffer_seconds * wave_format.nAvgBytesPerSec;
    descriptor.format = &wave_format;
    IDirectSoundBuffer *temporary = NULL;
    volume_sink = NULL;
    timer_callback = (LPTIMECALLBACK)INVALID_HANDLE_VALUE;
    event = INVALID_HANDLE_VALUE;
    count = 0;
    read_index = 0;
    write_index = 0;
    unknown_0004 = -1;
    loop_flags = 0;
    unknown_03f4 = 0;
    requested_volume = 0;
    applied_volume = 1;
    decoder = (PC_AUDIO_DECODER_VIEW *)new OggReader;
    stream_state = 0;
    InitializeCriticalSection(&lock);
    InitializeCriticalSection(&operation_lock);
    InitializeCriticalSection(&volume_lock);
    if (SUCCEEDED(factory_value->vtable->create_sound_buffer(factory_value, &descriptor, &temporary, NULL))) {
        if (SUCCEEDED(temporary->vtable->query_interface(temporary, PCSoundBuffer8IIDPc, (void **)&volume_sink))) {
            timer_callback = (LPTIMECALLBACK)CreateEvent(NULL, FALSE, FALSE, NULL);
            event = CreateEvent(NULL, TRUE, FALSE, NULL);
            if ((HANDLE)timer_callback != INVALID_HANDLE_VALUE && event != INVALID_HANDLE_VALUE) {
                thread = CreateThread(NULL, 0, PCSoundStreamWorkerPc, this, CREATE_SUSPENDED, &thread_id);
                if (thread != NULL) {
                    ResumeThread(thread);
                    active = 1;
                }
            }
        }
        temporary->vtable->release(temporary);
    }
}

void SoundQueue::SuspendForFocusPc(void) {
    if (this == NULL)
        return;
    EnterCriticalSection(&operation_lock);
    EnterCriticalSection(&volume_lock);
    IDirectSoundBuffer *buffer = (IDirectSoundBuffer *)volume_sink;
    if (stream_state == 2 && buffer != NULL)
        buffer->vtable->stop(buffer);
    LeaveCriticalSection(&volume_lock);
    unknown_03f4 = 1;
}
extern "C" void __cdecl NuSound3SuspendForFocusPc(void) {
    if (NuSound3Initialized != 0) {
        SoundVoiceManager.StopAllVoices();
        NuSound3StereoStreams[0]->SuspendForFocusPc();
        NuSound3StereoStreams[1]->SuspendForFocusPc();
    }
}
extern "C" void __cdecl NuSound3ResumeAfterFocusPc(void) {
    if (NuSound3Initialized != 0) {
        NuSound3StereoStreams[0]->CancelPausePc();
        NuSound3StereoStreams[1]->CancelPausePc();
    }
}

extern "C" void __cdecl PCSoundStopStereoChannelPc(i32 channel) {
    char message[64];
    sprintf(message, ">>> NuSoundStopStereo %d", channel);
    SoundQueue *stream = NuSound3StereoStreams[0];
    if (channel != PCStereoChannel0A)
        stream = NuSound3StereoStreams[1];
    stream->Stop();
}

extern "C" void __cdecl NuFmvStopSoundPc(i32 wait) {
    SoundVoiceManager.StopAllVoices();
    if (wait != 0) {
        PCSoundStopStereoChannelPc(PCStereoChannel0A);
        PCSoundStopStereoChannelPc(PCStereoChannel1A);
        while (NuSound3GetStereoStreamStatus(PCStereoChannel0A) != 0 ||
               NuSound3GetStereoStreamStatus(PCStereoChannel1A) != 0)
            Sleep(10);
    }
}

extern "C" IDirectSound8 *__cdecl NuFmvGetSoundFactoryPc(void) {
    return SoundVoiceManager.factory;
}
