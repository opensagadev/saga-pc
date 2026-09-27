#include "saga.h"

class SoundQueueVolumeSink {
  public:
    virtual void Slot00(void) = 0;
    virtual void Slot04(void) = 0;
    virtual void Slot08(void) = 0;
    virtual void Slot0c(void) = 0;
    virtual void Slot10(void) = 0;
    virtual void Slot14(void) = 0;
    virtual void Slot18(void) = 0;
    virtual void Slot1c(void) = 0;
    virtual void Slot20(void) = 0;
    virtual void Slot24(void) = 0;
    virtual void Slot28(void) = 0;
    virtual void Slot2c(void) = 0;
    virtual void Slot30(void) = 0;
    virtual void Slot34(void) = 0;
    virtual void Slot38(void) = 0;
    virtual i32 SetVolume(i32 volume) = 0;
};

struct SoundQueue {
    i32 active;
    u32 unknown_0004;
    i32 stream_state;
    i32 stream_flags;
    i32 requested_volume;
    i32 applied_volume;
    u8 unknown_0018[0x3c];
    SoundQueueVolumeSink *volume_sink;
    u8 unknown_0058[8];
    HANDLE event;
    u8 unknown_0064[0xc];
    CRITICAL_SECTION lock;
    u8 unknown_0088[0x18];
    CRITICAL_SECTION volume_lock;
    i32 command[32];
    i32 argument[32];
    i32 count;
    i32 read_index;
    i32 write_index;

    i32 SendCommand(i32 command, i32 argument);
    __declspec(noinline) i32 Stop();
    __declspec(noinline) i32 Pause();
    __declspec(noinline) i32 Resume();
    __declspec(noinline) i32 SetVolume(i32 volume);
    __declspec(noinline) i32 HasCommand(i32 command);
    __declspec(noinline) i32 IsStreamIdle();
    __declspec(noinline) i32 GetStatus();
};
STATIC_ASSERT(offsetof(SoundQueue, stream_state) == 0x08);
STATIC_ASSERT(offsetof(SoundQueue, stream_flags) == 0x0c);
STATIC_ASSERT(offsetof(SoundQueue, requested_volume) == 0x10);
STATIC_ASSERT(offsetof(SoundQueue, applied_volume) == 0x14);
STATIC_ASSERT(offsetof(SoundQueue, volume_sink) == 0x54);
STATIC_ASSERT(offsetof(SoundQueue, event) == 0x60);
STATIC_ASSERT(offsetof(SoundQueue, lock) == 0x70);
STATIC_ASSERT(offsetof(SoundQueue, volume_lock) == 0xa0);
STATIC_ASSERT(offsetof(SoundQueue, command) == 0xb8);
STATIC_ASSERT(offsetof(SoundQueue, argument) == 0x138);
STATIC_ASSERT(offsetof(SoundQueue, count) == 0x1b8);
STATIC_ASSERT(offsetof(SoundQueue, write_index) == 0x1c0);

extern SoundQueue *NuSound3StereoStreams[2];
extern "C" f32 NuSound3VolumeTable[0x4000];
extern "C" i32 NuSound3VolumeOffset;

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
