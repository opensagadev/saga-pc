#ifndef SAGA_NU2API_NUSOUND_NUSOUND_H_PART
#define SAGA_NU2API_NUSOUND_NUSOUND_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: decomp.h, legoapi/render/core/gameliball.h, nu2api/nucore/common.h,
 * nu2api/nucore/fixed_width.h, nu2api/nufile/nufile.h, nu2api/numath/numtx.h, nu2api/numath/nuvec.h,
 * nu2api/numusic_gen.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "decomp.h"
#include "legoapi/render/core/gameliball.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic_gen.h"
#endif
/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NUSOUND_NUSOUND_H_PART == 1 && !defined(SAGA_NU2API_NUSOUND_NUSOUND_H_STOP)

typedef struct SoundSamplePC {
    IDirectSoundBuffer *buffer;
    char file_name[0x400];
    u8 valid_byte;
    u8 failed_byte;
    u8 unknown_406[2];
    u32 frequency;
} SoundSamplePC;

STATIC_ASSERT(sizeof(SoundSamplePC) == 0x40c);
STATIC_ASSERT(offsetof(SoundSamplePC, buffer) == 0);
STATIC_ASSERT(offsetof(SoundSamplePC, file_name) == 4);
STATIC_ASSERT(offsetof(SoundSamplePC, valid_byte) == 0x404);
STATIC_ASSERT(offsetof(SoundSamplePC, frequency) == 0x408);

typedef struct SoundVoiceSlotPC {
    IDirectSoundBuffer *voice;
    IDirectSound3DBuffer *aux;
    SoundSamplePC *sample;
    u32 is_spatial;
    i32 sample_id;
    i32 sound_key;
    u8 field_18;
    u8 field_19;
    u8 unknown_1a[2];
    f32 field_1c;
    f32 field_20;
    i32 volume_index;
    i32 field_28;
    i32 field_2c;
    i32 parameter;
} SoundVoiceSlotPC;

STATIC_ASSERT(sizeof(SoundVoiceSlotPC) == 0x34);
STATIC_ASSERT(offsetof(SoundVoiceSlotPC, voice) == 0);
STATIC_ASSERT(offsetof(SoundVoiceSlotPC, aux) == 4);
STATIC_ASSERT(offsetof(SoundVoiceSlotPC, sample) == 8);
STATIC_ASSERT(offsetof(SoundVoiceSlotPC, is_spatial) == 0xc);
STATIC_ASSERT(offsetof(SoundVoiceSlotPC, sample_id) == 0x10);
STATIC_ASSERT(offsetof(SoundVoiceSlotPC, sound_key) == 0x14);
STATIC_ASSERT(offsetof(SoundVoiceSlotPC, field_18) == 0x18);
STATIC_ASSERT(offsetof(SoundVoiceSlotPC, field_19) == 0x19);
STATIC_ASSERT(offsetof(SoundVoiceSlotPC, field_1c) == 0x1c);
STATIC_ASSERT(offsetof(SoundVoiceSlotPC, field_20) == 0x20);
STATIC_ASSERT(offsetof(SoundVoiceSlotPC, field_28) == 0x28);
STATIC_ASSERT(offsetof(SoundVoiceSlotPC, field_2c) == 0x2c);
STATIC_ASSERT(offsetof(SoundVoiceSlotPC, volume_index) == 0x24);
STATIC_ASSERT(offsetof(SoundVoiceSlotPC, parameter) == 0x30);

typedef struct SoundVoiceManagerPC {
    u32 state_000;
    u8 unknown_004;
    u8 option5;
    u8 option6;
    u8 unknown_007[5];
    SoundVoiceSlotPC voices[64];
    SoundSamplePC *samples;
    i32 active_count;

    union {
        u32 unknown_d14;

        struct {
            u8 option_d14;
            u8 unknown_d15[3];
        };
    };

    IDirectSound8 *factory;
    PC_DS_CAPS_VIEW caps;
    DWORD speaker_config;
    IDirectSoundBuffer *primary_buffer;
    PC_DS_BUFFER_DESC_VIEW primary_desc;
#ifdef __cplusplus
    SoundVoiceManagerPC();
    ~SoundVoiceManagerPC();
    void Init(i32, u8, u8);
    i32 LoadWavSamplePc(const char *, i32);
    void UnloadSample(i32);
    void ClearSamplesPc(void);
    void SetOptionD14Pc(u8);
    IDirectSound8 *GetFactory(void);
    void SetSampleName(const char *name, i32 sample_index);
    i32 AllocateVoice(i32, i32);
    bool FindLoopVoicePc(NUVEC *, i32, i32 *);
    void SetVoice(i32, i32, f32, i32);
    void SetVolumeTableIndex(i32, i32);
    void SetFrequencyParameter(i32, i32);
    void StopVoice(i32);
    void StopAllVoices(void);
    void UpdateVoices(void);
    void Setup3DVoice(i32, const NUVEC *, i32);
    u8 fun_00697360(i32 sample_index);
    u8 fun_00697390(i32 sample_index);
    void fun_00697700(i32 voice_id, i32 sound_key, i32, f32 field_1c, f32 field_20, i32 volume_index);
#endif
} SoundVoiceManagerPC;

STATIC_ASSERT(offsetof(SoundVoiceManagerPC, state_000) == 0);
STATIC_ASSERT(offsetof(SoundSamplePC, failed_byte) == 0x405);
STATIC_ASSERT(offsetof(SoundVoiceManagerPC, voices) == 0xc);
STATIC_ASSERT(offsetof(SoundVoiceManagerPC, samples) == 0xd0c);
STATIC_ASSERT(offsetof(SoundVoiceManagerPC, active_count) == 0xd10);
STATIC_ASSERT(offsetof(SoundVoiceManagerPC, factory) == 0xd18);
STATIC_ASSERT(sizeof(SoundVoiceManagerPC) == 0xda8);
STATIC_ASSERT(offsetof(SoundVoiceManagerPC, option_d14) == 0xd14);

typedef struct PC_SOUND_COMMIT_PC PC_SOUND_COMMIT_PC;

typedef struct PC_SOUND_COMMIT_METHODS_PC {
    void *unknown_00[2];
    ULONG(__stdcall *release)(PC_SOUND_COMMIT_PC *);
    void *unknown_0c[10];
    HRESULT(__stdcall *set_orientation)(PC_SOUND_COMMIT_PC *, float, float, float, float, float, float, DWORD);
    HRESULT(__stdcall *set_position)(PC_SOUND_COMMIT_PC *, float, float, float, DWORD);
    HRESULT(__stdcall *set_rolloff_factor)(PC_SOUND_COMMIT_PC *, float, DWORD);
    void *unknown_40;
    HRESULT(__stdcall *commit_deferred_settings)(PC_SOUND_COMMIT_PC *);
} PC_SOUND_COMMIT_METHODS_PC;

struct PC_SOUND_COMMIT_PC {
    const PC_SOUND_COMMIT_METHODS_PC *vtable;
};

STATIC_ASSERT(offsetof(PC_SOUND_COMMIT_METHODS_PC, commit_deferred_settings) == 0x44);

struct nusound_filename_info_s;
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 PCSoundStorageLockReadyPc;
    extern CRITICAL_SECTION PCSoundStorageLockPc;
    extern i32 PCSoundDrainStartPc;
    extern i32 PCSoundDrainRemainingPc;
    i32 NuSound3PlayStereoV(i32 token, ...);
    extern i32 PCStereoChannel0A;
    extern i32 PCStereoChannel0B;
    extern i32 PCStereoChannel1A;
    extern i32 PCStereoChannel1B;
    extern i32 NuSound3LoopHoldTime;
    i32 NuSound3SetReverb(i32 enable);
    void NuSound3SetLoopHoldTime(f32 time);
    void NuSound3FlushLoops(void);
    void NuSound3StopRumble(void);
    void NuSound3SetRumblePads(void *pad0, void *pad1);
    void NuSound3StopStereoStream(i32 index);
    void NuSound3PauseStereoStream(i32 index);
    void NuSound3ResumeStereoStream(i32 index);
    i32 NuSound3SetStreamVolume(i32 index, i32 volume);
    i32 NuSound3GetStereoStreamStatus(i32 index);
    extern f32 NuSound3VolumeTable[0x4000];
    extern i32 NuSound3VolumeOffset;
    void NuSound3StopSFX(void);
    void NuSound3SetSFXPitch(i32 pitch);
    extern i32 NuSound3Initialized;
    f32 PCSoundVolumeLookup(i32);
    HRESULT PCSoundSetRawVolume(IDirectSoundBuffer *, i32);
    HRESULT PCSoundSetLinearVolume(IDirectSoundBuffer *, f32);
    extern SoundVoiceManagerPC SoundVoiceManager;
    extern CRITICAL_SECTION SoundVoiceCriticalSection;
    extern i32 SoundVoiceVolumeOffset;
    extern const char PCSoundOggExtensionText[5];
    extern const char *PCSoundOggExtension;
    extern i32 PCSoundVolumeMultiplier;
    extern const IID PCSoundListenerIIDPc;
    extern const GUID PCSampleBufferAlgorithmPc;
    extern i32 PCSampleLoadEnabledPc;
    extern i32 PCSoundSampleLimitPc;
    extern i32 PCSoundStreamSerialPc;
    STATIC_ASSERT(offsetof(SoundVoiceManagerPC, option5) == 5);
    STATIC_ASSERT(offsetof(SoundVoiceManagerPC, option6) == 6);
    STATIC_ASSERT(offsetof(SoundVoiceManagerPC, caps) == 0xd1c);
    STATIC_ASSERT(offsetof(SoundVoiceManagerPC, speaker_config) == 0xd7c);
    STATIC_ASSERT(offsetof(SoundVoiceManagerPC, primary_buffer) == 0xd80);
    STATIC_ASSERT(offsetof(SoundVoiceManagerPC, primary_desc) == 0xd84);
    STATIC_ASSERT(offsetof(SoundSamplePC, failed_byte) == 0x405);
    STATIC_ASSERT(offsetof(PC_SOUND_COMMIT_METHODS_PC, set_orientation) == 0x34);
    STATIC_ASSERT(offsetof(PC_SOUND_COMMIT_METHODS_PC, set_position) == 0x38);
    STATIC_ASSERT(offsetof(PC_SOUND_COMMIT_METHODS_PC, set_rolloff_factor) == 0x3c);
    void PCSoundPlayUnpositioned(i32, i32, i32, i32, f32, i32, f32, f32, i32);
    void PCSoundPlayUnpositionedWrapper(i32, i32, i32, i32, f32, i32, f32, f32);
    extern i32 PCSoundStartedOneShotCountPc;
    extern i32 PCSoundStartedLoopCountPc;
    extern i32 PCSoundUpdatedLoopCountPc;
    void PCSoundPlayPositionedLoop(NUVEC *, i32, i32, i32, i32, f32, i32, f32, f32);
    void PCSoundPlayPositioned(NUVEC *, i32, i32, i32, i32, f32, i32, f32, f32, i32);
    void PCSoundPlayPositionedWrapper(NUVEC *, i32, i32, i32, i32, f32, i32, f32, f32);
    extern const GUID SoundVoice3DBufferIID;
#ifdef __cplusplus
    void PCSoundAlreadyPlayingHook(...);
    void PCSoundFailure(...);
#ifdef __cplusplus
}
#endif
#else
#ifdef __cplusplus
extern "C" {
#endif
    void PCSoundAlreadyPlayingHook(void);
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif

    void NuSound3GetStreamInfo(i32 stream_index, void *info);
    void NuSound3KillAllAudio(void);
    void NuSound3Update(void);
    extern void *PCSoundMixSource;
    extern i32 PCSoundMixSampleCount;
    extern void *PCSoundMixDestination;
    extern HANDLE PCSoundMixEvent;
    extern PC_SOUND_COMMIT_PC *PCSoundCommitTarget;
    extern f32 nusound_fade_start;
    extern f32 nusound_fade_end;
    i32 ActionFromQuiet(i32);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

struct nusound_filename_info_s;
#endif
#ifdef __cplusplus

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
    virtual i32 __stdcall SetVolume(i32 volume) = 0;
};

extern "C" const IID PCSoundBuffer8IIDPc;
extern "C" DWORD __stdcall PCSoundStreamWorkerPc(void *);
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 NuSound3LoadingSfxPc;
    i32 NuSound3LoadingSfx(void);
    extern CRITICAL_SECTION SoundSampleCriticalSectionPc;
    extern HANDLE PCSoundSampleWorkerHandlePc;
    extern volatile u8 PCSoundSampleWorkerStopPc;
    extern char *PCSampleFilenameExtensionPc;
    void NuSound3SetRequestTable(u16 *, i32);
    void NuSound3CancelCheckStereo(void);
#ifdef __cplusplus
}
#endif

enum { MUSIC_PLAYBACK_DUAL_STREAM = 11 };

enum { MUSIC_PLAYBACK_DUAL_STREAM_PENDING = 13 };

#ifdef __cplusplus
extern "C" {
#endif
    void NuSound3SuspendForFocusPc(void);
    void NuSound3ResumeAfterFocusPc(void);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUSOUND_NUSOUND_H_PART
#define SAGA_NU2API_NUSOUND_NUSOUND_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUSOUND_NUSOUND_H_STOP
#endif
#endif

#if SAGA_NU2API_NUSOUND_NUSOUND_H_PART == 2 && !defined(SAGA_NU2API_NUSOUND_NUSOUND_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void PCStereoStartPacket(struct PC_STEREO_PLAY_PACKET *packet);
    extern NUDATHDR *NuSoundDat;
    void NuSound3SetDat(NUDATHDR *header);
    extern MUSICPLAYBACK_PC Music;
    NUMTX *NuSound3GetListener(void);
    extern NUMTX *NuSound3Listener;
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

struct nusound_filename_info_s {
    char *filename;
    i32 pitch;
    i32 static_sample;
    i32 index;
    i32 unknown_10;
    i32 unknown_14;
    i32 unknown_18;
};

STATIC_ASSERT(sizeof(nusound_filename_info_s) == 0x1c);
STATIC_ASSERT(offsetof(nusound_filename_info_s, filename) == 0);
STATIC_ASSERT(offsetof(nusound_filename_info_s, pitch) == 4);
STATIC_ASSERT(offsetof(nusound_filename_info_s, static_sample) == 8);
STATIC_ASSERT(offsetof(nusound_filename_info_s, index) == 0xc);
STATIC_ASSERT(offsetof(nusound_filename_info_s, unknown_10) == 0x10);
#endif
#ifdef __cplusplus
extern "C" {
#endif
    void NuSound3SetSampleTable(SFX_FILE_INFO_PC *, VARIPTR *, VARIPTR);
    i32 NuSound3InitV(VARIPTR *, VARIPTR, i32, i32, ...);
    i32 PCSoundInitializePc(PC_SOUND_INIT_CONFIG *);
    struct nusound_filename_info_s *ConfigureMusic(char *, VARIPTR *, VARIPTR *);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUSOUND_NUSOUND_H_PART
#define SAGA_NU2API_NUSOUND_NUSOUND_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUSOUND_NUSOUND_H_STOP
#endif
#endif

#if SAGA_NU2API_NUSOUND_NUSOUND_H_PART == 3 && !defined(SAGA_NU2API_NUSOUND_NUSOUND_H_STOP)
#ifdef __cplusplus

struct SoundQueue {
    i32 active;
    u32 unknown_0004;
    i32 stream_state;
    i32 stream_flags;
    i32 requested_volume;
    i32 applied_volume;
    i32 format_mode;
    WAVEFORMATEX wave_format;
    u8 unknown_02e[2];
    PC_DS_BUFFER_DESC_VIEW descriptor;
    SoundQueueVolumeSink *volume_sink;
    i32 buffer_seconds;

    union {
        struct {
            LPTIMECALLBACK timer_callback;
            HANDLE event;
        };

        HANDLE wait_events[2];
    };

    UINT timer_id;
    HANDLE thread;
    DWORD thread_id;
    CRITICAL_SECTION lock;
    CRITICAL_SECTION operation_lock;
    CRITICAL_SECTION volume_lock;
    i32 command[32];
    i32 argument[32];
    i32 count;
    i32 read_index;
    i32 write_index;
    i32 unknown_1c4;
    i32 decoded_bytes;
    i32 buffer_write_position;
    i32 loop_flags;
    i32 unknown_1d4;
    i32 pending_bytes;
    PC_AUDIO_DECODER_VIEW *decoder;

    union {
        struct {
            char stream_path[0x104];
            u8 unknown_2e4;
        };

        char pending_path[0x105];
    };

    char current_path[0x105];
    u8 unknown_3ea[2];
    f32 start_offset;
    f32 current_offset;
    i32 unknown_03f4;
    i32 serial;
    IDirectSound8 *factory;

    SoundQueue(IDirectSound8 *, i32);
    ~SoundQueue(void);
    void SuspendForFocusPc(void);
    void CancelPausePc(void);
    DWORD Run(void);
    i32 SetFormatPc(const WAVEFORMATEX *);
    i32 FillBufferPc(i32, u32);
    i32 SendCommand(i32 command, i32 argument);
    // noinline: the original calls these out of line from the NuSound3*StereoStream wrappers; VC8 would inline them.
    __declspec(noinline) i32 Stop();
    __declspec(noinline) i32 Pause();
    __declspec(noinline) i32 Resume();
    __declspec(noinline) i32 SetVolume(i32 volume);
    bool HasCommand(i32 command);
    i32 IsStreamIdle();
    __declspec(noinline) i32 GetStatus();
    void StartTimer();
    void StopTimer();
    i32 SetStreamFile(const char *path, f32 offset);
    i32 SetStreamFlags(i32 flags);
    i32 fun_00699040(const WAVEFORMATEX *format);
    i32 fun_00699200(i32 *argument_value);
};

STATIC_ASSERT(offsetof(SoundQueue, stream_state) == 0x08);
STATIC_ASSERT(offsetof(SoundQueue, stream_flags) == 0x0c);
STATIC_ASSERT(offsetof(SoundQueue, requested_volume) == 0x10);
STATIC_ASSERT(offsetof(SoundQueue, applied_volume) == 0x14);
STATIC_ASSERT(offsetof(SoundQueue, volume_sink) == 0x54);
STATIC_ASSERT(offsetof(SoundQueue, timer_callback) == 0x5c);
STATIC_ASSERT(offsetof(SoundQueue, event) == 0x60);
STATIC_ASSERT(offsetof(SoundQueue, timer_id) == 0x64);
STATIC_ASSERT(offsetof(SoundQueue, lock) == 0x70);
STATIC_ASSERT(offsetof(SoundQueue, volume_lock) == 0xa0);
STATIC_ASSERT(offsetof(SoundQueue, command) == 0xb8);
STATIC_ASSERT(offsetof(SoundQueue, argument) == 0x138);
STATIC_ASSERT(offsetof(SoundQueue, count) == 0x1b8);
STATIC_ASSERT(offsetof(SoundQueue, write_index) == 0x1c0);
STATIC_ASSERT(offsetof(SoundQueue, stream_path) == 0x1e0);
STATIC_ASSERT(offsetof(SoundQueue, start_offset) == 0x3ec);
STATIC_ASSERT(offsetof(SoundQueue, unknown_03f4) == 0x3f4);
STATIC_ASSERT(offsetof(SoundQueue, active) == 0);
STATIC_ASSERT(offsetof(SoundQueue, unknown_0004) == 4);
STATIC_ASSERT(offsetof(SoundQueue, read_index) == 0x1bc);
STATIC_ASSERT(offsetof(SoundQueue, pending_path) == 0x1e0);
STATIC_ASSERT(offsetof(SoundQueue, format_mode) == 0x18);
STATIC_ASSERT(offsetof(SoundQueue, wave_format) == 0x1c);
STATIC_ASSERT(offsetof(SoundQueue, descriptor) == 0x30);
STATIC_ASSERT(offsetof(SoundQueue, buffer_seconds) == 0x58);
STATIC_ASSERT(offsetof(SoundQueue, thread) == 0x68);
STATIC_ASSERT(offsetof(SoundQueue, thread_id) == 0x6c);
STATIC_ASSERT(offsetof(SoundQueue, operation_lock) == 0x88);
STATIC_ASSERT(offsetof(SoundQueue, decoded_bytes) == 0x1c8);
STATIC_ASSERT(offsetof(SoundQueue, buffer_write_position) == 0x1cc);
STATIC_ASSERT(offsetof(SoundQueue, loop_flags) == 0x1d0);
STATIC_ASSERT(offsetof(SoundQueue, pending_bytes) == 0x1d8);
STATIC_ASSERT(offsetof(SoundQueue, decoder) == 0x1dc);
STATIC_ASSERT(offsetof(SoundQueue, current_path) == 0x2e5);
STATIC_ASSERT(offsetof(SoundQueue, current_offset) == 0x3f0);
STATIC_ASSERT(offsetof(SoundQueue, serial) == 0x3f8);
STATIC_ASSERT(offsetof(SoundQueue, factory) == 0x3fc);
STATIC_ASSERT(sizeof(SoundQueue) == 0x400);
#endif
#undef SAGA_NU2API_NUSOUND_NUSOUND_H_PART
#define SAGA_NU2API_NUSOUND_NUSOUND_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUSOUND_NUSOUND_H_STOP
#endif
#endif

#undef SAGA_NU2API_NUSOUND_NUSOUND_H_STOP
