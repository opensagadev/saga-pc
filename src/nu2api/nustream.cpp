#include "nu2api/nualloc.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nusound/nusound.h"
#include "nu2api/wavreader.h"
/* vorbisfile.h's static seek callback (the OV_CALLBACKS tables use it): each unit including the header carries
 * its own copy; this one is named apart from the other units' copies. */
// NOLINTNEXTLINE(readability-identifier-naming): vorbisfile.h names the static
#define _ov_header_fseek_wrap NustreamOvHeaderFseekWrap
#include "vorbis/vorbisfile.h"
#undef _ov_header_fseek_wrap
#include "nu2api/oggreader.h"
#include <stdio.h>
#include "nu2api/nu3d/nushaderobject.h"

extern "C" void NuSound3GetStreamInfo(i32 stream_index, void *info) {
}

SoundVoiceManagerPC::~SoundVoiceManagerPC() {
    i32 i;
    NuSound3ShutdownStreams();
    for (i = 0; i < 64; i++) {
        if (voices[i].voice != NULL) {
            voices[i].voice->vtable->release(voices[i].voice);
            voices[i].voice = NULL;
        }
    }
    for (i = 0; i < PCSoundSampleLimit; i++) {
        if (samples[i].buffer != NULL) {
            samples[i].buffer->vtable->release(samples[i].buffer);
            samples[i].buffer = NULL;
        }
    }
    if (PCSoundCommitTarget != NULL) {
        PCSoundCommitTarget->vtable->release(PCSoundCommitTarget);
        PCSoundCommitTarget = NULL;
    }
    if (primary_buffer != NULL) {
        primary_buffer->vtable->release(primary_buffer);
        primary_buffer = NULL;
    }
    if (factory != NULL) {
        factory->vtable->release(factory);
        factory = NULL;
    }
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

i32 SoundQueue::PopCommand(i32 *argument_value) {
    i32 command_value = 0;
    EnterCriticalSection(&lock);
    if (count > 0) {
        command_value = command[read_index];
        if (argument_value != NULL) {
            *argument_value = argument[read_index];
        }
        read_index = (read_index + 1) & 31;
        --count;
    }
    LeaveCriticalSection(&lock);
    return command_value;
}

i32 SoundQueue::Stop() {
    if (this == NULL) {
        return 0;
    }
    if (active == 0) {
        return 0;
    }
    return SendCommand(SOUNDQUEUE_CMD_STOP, 0);
}

/* Command 4 stops a playing buffer and keeps its position (pause). */
i32 SoundQueue::Pause() {
    if (this == NULL) {
        return 0;
    }
    if (active == 0) {
        return 0;
    }
    return SendCommand(SOUNDQUEUE_CMD_PAUSE, 0);
}

/* Command 5 restarts a paused buffer (resume). */
i32 SoundQueue::Resume() {
    if (this == NULL) {
        return 0;
    }
    if (active == 0) {
        return 0;
    }
    return SendCommand(SOUNDQUEUE_CMD_RESUME, 0);
}

i32 SoundQueue::SetVolume(i32 volume) {
    if (this == NULL) {
        return 0;
    }
    EnterCriticalSection(&volume_lock);
    requested_volume = volume;
    if (volume_sink != NULL) {
        volume_sink->vtable->set_volume(volume_sink, requested_volume);
        applied_volume = requested_volume;
    }
    LeaveCriticalSection(&volume_lock);
    return 1;
}

bool SoundQueue::HasCommand(i32 command_value) {
    bool found = false;
    EnterCriticalSection(&lock);
    for (i32 index = 0; index != count; ++index) {
        if (command[index] == command_value) {
            found = true;
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
    if (stream_state != SOUNDQUEUE_STATE_STOPPED && (stream_state != SOUNDQUEUE_STATE_PAUSED || stream_flags == 0)) {
        return 0;
    }
    return 1;
}

i32 SoundQueue::GetStatus() {
    if (this == NULL) {
        return 0;
    }
    if (HasCommand(SOUNDQUEUE_CMD_STOP) != 0) {
        return 2;
    }
    if (stream_state == SOUNDQUEUE_STATE_PLAYING || (stream_state == SOUNDQUEUE_STATE_PAUSED && stream_flags != 0)) {
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
    if (this == NULL) {
        return 0;
    }
    if (active == 0) {
        return 0;
    }
    NuStrNCpy(stream_path, path, sizeof(stream_path));
    start_offset = offset;
    return SendCommand(SOUNDQUEUE_CMD_SET_FILE, 0);
}

i32 SoundQueue::SetStreamFlags(i32 flags) {
    if (this == NULL) {
        return 0;
    }
    if (active == 0) {
        return 0;
    }
    return SendCommand(SOUNDQUEUE_CMD_PLAY, flags);
}

extern "C" void fun_006984b0(void) {
}

void SoundQueue::CancelPause(void) {
    if (this == NULL || stream_running == 0) {
        return;
    }
    EnterCriticalSection(&volume_lock);
    if (stream_state == SOUNDQUEUE_STATE_PLAYING && volume_sink != NULL) {
        volume_sink->vtable->play(volume_sink, 0, 0, 1);
    }
    LeaveCriticalSection(&volume_lock);
    stream_running = 0;
    LeaveCriticalSection(&operation_lock);
}

SoundQueue::~SoundQueue(void) {
    if (this == NULL) {
        return;
    }
    if (active != 0) {
        if (stream_running != 0) {
            CancelPause();
        }
        if (active != 0) {
            SendCommand(SOUNDQUEUE_CMD_STOP, 0);
        }
        SendCommand(SOUNDQUEUE_CMD_QUIT, 0);
        if (thread != NULL) {
            do {
                Sleep(10);
            } while (thread != NULL);
        }
    }
    delete decoder;
    decoder = NULL;
    DeleteCriticalSection(&volume_lock);
    DeleteCriticalSection(&operation_lock);
    DeleteCriticalSection(&lock);
    if (event != INVALID_HANDLE_VALUE) {
        CloseHandle(event);
    }
    if ((HANDLE)timer_callback != INVALID_HANDLE_VALUE) {
        CloseHandle((HANDLE)timer_callback);
    }
    event = INVALID_HANDLE_VALUE;
    timer_callback = (LPTIMECALLBACK)INVALID_HANDLE_VALUE;
    if (timer_id != 0) {
        timeKillEvent(timer_id);
    }
    timer_id = 0;
    if (volume_sink != NULL) {
        volume_sink->vtable->release(volume_sink);
    }
    volume_sink = NULL;
    active = 0;
}

extern "C" {
    extern const IID PCSoundBuffer8IID = {0x6825a449, 0x7524, 0x4d82, {0x92, 0xf, 0x50, 0xe3, 0x6a, 0xb3, 0xab, 0x1e}};
}

i32 SoundQueue::SetFormat(const WAVEFORMATEX *format) {
    if (volume_sink != NULL) {
        EnterCriticalSection(&volume_lock);
        volume_sink->vtable->release(volume_sink);
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
            if (SUCCEEDED(temporary->vtable->query_interface(temporary, PCSoundBuffer8IID, (void **)&volume_sink))) {
                volume_sink->vtable->set_volume(volume_sink, requested_volume);
                applied_volume = requested_volume;
            }
            temporary->vtable->release(temporary);
        }
        LeaveCriticalSection(&volume_lock);
    }
    return 0;
}

extern "C" {
    i32 PCSoundDrainStart = 0;
    i32 PCSoundDrainRemaining = 0;
}

i32 SoundQueue::FillBuffer(i32 bytes, u32 flags) {
    if (stream_state != SOUNDQUEUE_STATE_PRELOADING && stream_state != SOUNDQUEUE_STATE_PLAYING) {
        return 0;
    }
    if (volume_sink == NULL) {
        return 0;
    }
    DWORD play_cursor;
    HRESULT result = volume_sink->vtable->get_current_position(volume_sink, &play_cursor, NULL);
    i32 requested = bytes;
    i32 total = buffer_seconds * wave_format.nAvgBytesPerSec;
    if (SUCCEEDED(result)) {
        i32 available = (i32)play_cursor - buffer_write_position;
        if (available < 0) {
            available += total;
        }
        if ((flags & 2) == 0) {
            requested = 2 * wave_format.nAvgBytesPerSec;
            if (available < requested) {
                requested = available;
            }
            if (buffer_write_position + requested > total) {
                requested = total - buffer_write_position;
            }
        }
    }
    if ((flags & 2) == 0 && (i32)play_cursor >= buffer_write_position && (i32)play_cursor < buffer_write_position + requested) {
        return 0;
    }
    i32 amount = decoder->GetSize() - decoded_bytes;
    i32 decoded;
    if (requested < amount) {
        decoded = requested;
    } else {
        decoded = decoder->GetSize() - decoded_bytes;
    }
    void *data;
    void *second;
    DWORD data_bytes;
    DWORD second_bytes;
    result = volume_sink->vtable->lock(volume_sink, buffer_write_position, requested, &data, &data_bytes, &second, &second_bytes, 0);
    if (result == (HRESULT)0x88780096) {
        if (FAILED(volume_sink->vtable->restore(volume_sink))) {
            return 0;
        }
        result = volume_sink->vtable->lock(volume_sink, buffer_write_position, requested, &data, &data_bytes, &second, &second_bytes, 0);
    }
    if (FAILED(result)) {
        return 0;
    }
    if (data_bytes < (u32)requested) {
        volume_sink->vtable->unlock(volume_sink, data, 0, second, 0);
    } else {
        if ((flags & 1) != 0) {
            decoded = 0;
        } else if (decoded != 0) {
            u32 read_count;
            if (FAILED(decoder->Read(data, decoded, &read_count)) || read_count != (u32)decoded) {
                decoded_bytes = decoder->GetSize();
                decoded = 0;
                if (SUCCEEDED(volume_sink->vtable->stop(volume_sink))) {
                    stream_state = SOUNDQUEUE_STATE_STOPPED;
                }
            }
        }
        if ((loop_flags & 1) != 0) {
            if (decoded < requested) {
                do {
                    decoder->Reset();
                    decoded_bytes = 0;
                    i32 remaining = requested - decoded;
                    i32 available = decoder->GetSize() - decoded_bytes;
                    i32 block = remaining;
                    if (available < remaining) {
                        block = decoder->GetSize() - decoded_bytes;
                    }
                    u32 read_count;
                    if (FAILED(decoder->Read((u8 *)data + decoded, block, &read_count)) || read_count != (u32)block) {
                        decoded_bytes = decoder->GetSize();
                        memset(data, 0, requested);
                        if (SUCCEEDED(volume_sink->vtable->stop(volume_sink))) {
                            stream_state = SOUNDQUEUE_STATE_STOPPED;
                        }
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
                PCSoundDrainRemaining = total;
                PCSoundDrainStart = play_cursor;
            } else {
                i32 elapsed;
                if ((i32)play_cursor >= PCSoundDrainStart) {
                    elapsed = (i32)play_cursor - PCSoundDrainStart;
                } else {
                    elapsed = total - PCSoundDrainStart + play_cursor;
                }
                PCSoundDrainRemaining -= elapsed;
                PCSoundDrainStart = play_cursor;
                if (PCSoundDrainRemaining <= 0) {
                    ++pending_bytes;
                }
            }
        }
        volume_sink->vtable->unlock(volume_sink, data, requested, second, 0);
        decoded_bytes += decoded;
        buffer_write_position = (u32)(buffer_write_position + requested) % (u32)(buffer_seconds * wave_format.nAvgBytesPerSec);
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
                IDirectSoundBuffer *buffer = volume_sink;
                switch (cmd) {
                    case SOUNDQUEUE_CMD_SET_FILE: {
                        NuStrNCpy(current_path, stream_path, 0x104);
                        current_offset = start_offset;
                        if (stream_state == SOUNDQUEUE_STATE_PLAYING) {
                            if (timer_id != 0) {
                                timeKillEvent(timer_id);
                            }
                            timer_id = 0;
                            buffer = volume_sink;
                            if (buffer != NULL) {
                                buffer->vtable->stop(buffer);
                            }
                        }
                        stream_flags = 0;
                        stream_state = SOUNDQUEUE_STATE_STOPPED;
                        pending_bytes = 0;
                        decoded_bytes = 0;
                        buffer_write_position = 0;
                        decoder_state = -1;
                        decoder->Close();
                        WAVEFORMATEX format;
                        if (SUCCEEDED(decoder->Open(NULL, current_path, &format))) {
                            if (wave_format.nSamplesPerSec != format.nSamplesPerSec || wave_format.nChannels != format.nChannels) {
                                SetFormat(&format);
                            }
                            decoder_state = 1;
                        }
                        break;
                    }
                    case SOUNDQUEUE_CMD_PLAY:
                        if ((i32)decoder_state < 0) {
                            if (timer_id != 0) {
                                timeKillEvent(timer_id);
                            }
                            timer_id = 0;
                        } else if (stream_state != SOUNDQUEUE_STATE_PLAYING) {
                            pending_bytes = 0;
                            stream_flags = 0;
                            decoded_bytes = 0;
                            buffer_write_position = 0;
                            loop_flags = 0;
                            decoder->Reset();
                            i32 duration = buffer_seconds * 1000;
                            if (duration < 2000) {
                                duration = 2000;
                            }
                            u32 preload = (u32)(wave_format.nAvgBytesPerSec * duration) / 1000;
                            stream_state = SOUNDQUEUE_STATE_PRELOADING;
                            decoded_bytes = decoder->SeekTime(current_offset);
                            buffer = volume_sink;
                            if (buffer != NULL) {
                                buffer->vtable->set_current_position(buffer, 0);
                            }
                            FillBuffer(preload, 2);
                            buffer = volume_sink;
                            if (buffer != NULL && SUCCEEDED(buffer->vtable->set_current_position(buffer, 0))) {
                                if ((command_arg & 2) != 0) {
                                    stream_state = SOUNDQUEUE_STATE_PAUSED;
                                    loop_flags = command_arg;
                                    StartTimer();
                                    break;
                                }
                                buffer = volume_sink;
                                if (SUCCEEDED(buffer->vtable->play(buffer, 0, 0, 1))) {
                                    stream_flags = 1;
                                    stream_state = SOUNDQUEUE_STATE_PLAYING;
                                    loop_flags = command_arg;
                                    StartTimer();
                                    break;
                                }
                            }
                            if (timer_id != 0) {
                                timeKillEvent(timer_id);
                            }
                            timer_id = 0;
                        }
                        break;
                    case SOUNDQUEUE_CMD_STOP:
                        if (timer_id != 0) {
                            timeKillEvent(timer_id);
                        }
                        timer_id = 0;
                        buffer = volume_sink;
                        if (buffer != NULL && SUCCEEDED(buffer->vtable->stop(buffer))) {
                            buffer_write_position = 0;
                            FillBuffer(buffer_seconds * wave_format.nAvgBytesPerSec, 3);
                        }
                        stream_state = SOUNDQUEUE_STATE_STOPPED;
                        break;
                    case SOUNDQUEUE_CMD_PAUSE:
                        if (stream_state == SOUNDQUEUE_STATE_PLAYING) {
                            if (timer_id != 0) {
                                timeKillEvent(timer_id);
                            }
                            timer_id = 0;
                            buffer = volume_sink;
                            if (buffer != NULL && SUCCEEDED(buffer->vtable->stop(buffer))) {
                                stream_state = SOUNDQUEUE_STATE_PAUSED;
                            }
                        }
                        break;
                    case SOUNDQUEUE_CMD_RESUME:
                        if (stream_state == SOUNDQUEUE_STATE_PAUSED && buffer != NULL && SUCCEEDED(buffer->vtable->play(buffer, 0, 0, 1))) {
                            stream_state = SOUNDQUEUE_STATE_PLAYING;
                            stream_flags = 1;
                            if (timer_id != 0) {
                                timeKillEvent(timer_id);
                            }
                            timer_id = timeSetEvent(500, 100, timer_callback, 0, 0x21);
                        }
                        break;
                    case SOUNDQUEUE_CMD_QUIT:
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
                    buffer = volume_sink;
                    if (buffer != NULL) {
                        buffer->vtable->set_volume(buffer, requested_volume);
                        applied_volume = requested_volume;
                    }
                    LeaveCriticalSection(&volume_lock);
                }
            }
        } else if (wait == WAIT_OBJECT_0 && (i32)decoder_state >= 0) {
            FillBuffer((u32)(buffer_seconds * wave_format.nAvgBytesPerSec) >> 2, 0);
            if (pending_bytes > 0 && stream_state == SOUNDQUEUE_STATE_PLAYING) {
                if (volume_sink != NULL) {
                    volume_sink->vtable->stop(volume_sink);
                }
                stream_state = SOUNDQUEUE_STATE_STOPPED;
                if (timer_id != 0) {
                    timeKillEvent(timer_id);
                }
                timer_id = 0;
            }
        }
        if (applied_volume != requested_volume || applied_volume == 1) {
            EnterCriticalSection(&volume_lock);
            if (volume_sink != NULL) {
                volume_sink->vtable->set_volume(volume_sink, requested_volume);
                applied_volume = requested_volume;
            }
            LeaveCriticalSection(&volume_lock);
        }
    } while (quitting == 0);
    return 0;
}

extern "C" DWORD __stdcall PCSoundStreamWorker(void *context) {
    return ((SoundQueue *)context)->Run();
}

extern "C" {
    i32 PCSoundStreamSerial = 0;
}

SoundQueue::SoundQueue(IDirectSound8 *factory_value, i32 mode) {
    serial = PCSoundStreamSerial++;
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
    decoder_state = -1;
    loop_flags = 0;
    stream_running = 0;
    requested_volume = 0;
    applied_volume = 1;
    decoder = new OggReader;
    stream_state = SOUNDQUEUE_STATE_STOPPED;
    InitializeCriticalSection(&lock);
    InitializeCriticalSection(&operation_lock);
    InitializeCriticalSection(&volume_lock);
    if (SUCCEEDED(factory_value->vtable->create_sound_buffer(factory_value, &descriptor, &temporary, NULL))) {
        if (SUCCEEDED(temporary->vtable->query_interface(temporary, PCSoundBuffer8IID, (void **)&volume_sink))) {
            timer_callback = (LPTIMECALLBACK)CreateEvent(NULL, FALSE, FALSE, NULL);
            event = CreateEvent(NULL, TRUE, FALSE, NULL);
            if ((HANDLE)timer_callback != INVALID_HANDLE_VALUE && event != INVALID_HANDLE_VALUE) {
                thread = CreateThread(NULL, 0, PCSoundStreamWorker, this, CREATE_SUSPENDED, &thread_id);
                if (thread != NULL) {
                    ResumeThread(thread);
                    active = 1;
                }
            }
        }
        temporary->vtable->release(temporary);
    }
}

void SoundQueue::SuspendForFocus(void) {
    if (this == NULL) {
        return;
    }
    EnterCriticalSection(&operation_lock);
    EnterCriticalSection(&volume_lock);
    if (stream_state == SOUNDQUEUE_STATE_PLAYING && volume_sink != NULL) {
        volume_sink->vtable->stop(volume_sink);
    }
    LeaveCriticalSection(&volume_lock);
    stream_running = 1;
}

extern "C" void fun_00698fe0(void) {
}

extern "C" void NuStreamNuError(void) {
}

i32 SoundQueue::FormatDiffers(const WAVEFORMATEX *format) {
    if (wave_format.nSamplesPerSec != format->nSamplesPerSec) {
        return 1;
    }
    return wave_format.nChannels != format->nChannels;
}
