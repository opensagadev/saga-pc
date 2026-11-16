#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/render/core/gameliball.h"
#include "nu2api/numath/nuvec.h"

struct PC_SOUND_INIT_CONFIG;
struct PC_STEREO_PLAY_PACKET;
struct nudathdr_s;
struct numtx_s;

typedef struct SoundSamplePC {  // size 0x40c
    IDirectSoundBuffer *buffer; // 0x000
    char file_name[0x400];      // 0x004
    u8 valid_byte;              // 0x404
    u8 failed_byte;             // 0x405
    u8 unknown_406[2];          // 0x406
    u32 frequency;              // 0x408
} SoundSamplePC;

typedef struct SoundVoiceSlotPC { // size 0x34
    IDirectSoundBuffer *voice;    // 0x00
    IDirectSound3DBuffer *aux;    // 0x04
    SoundSamplePC *sample;        // 0x08
    u32 is_spatial;               // 0x0c
    i32 sample_id;                // 0x10
    i32 sound_key;                // 0x14
    u8 unknown_18;                // 0x18
    u8 unknown_19;                // 0x19
    u8 unknown_1a[2];             // 0x1a
    f32 unknown_1c;               // 0x1c
    f32 unknown_20;               // 0x20
    i32 volume_index;             // 0x24
    i32 unknown_28;               // 0x28
    i32 unknown_2c;               // 0x2c
    i32 parameter;                // 0x30
} SoundVoiceSlotPC;

typedef struct SoundVoiceManagerPC { // size 0xda8
    u32 state_000;                   // 0x000
    u8 unknown_004;                  // 0x004
    u8 option5;                      // 0x005
    u8 option6;                      // 0x006
    u8 unknown_007[5];               // 0x007
    SoundVoiceSlotPC voices[64];     // 0x00c
    SoundSamplePC *samples;          // 0xd0c
    i32 active_count;                // 0xd10

    union { // size 0x4
        u32 unknown_d14;

        struct {               // size 0x4
            u8 option_d14;     // 0xd14
            u8 unknown_d15[3]; // 0xd15
        };
    }; // 0xd14

    IDirectSound8 *factory;              // 0xd18
    PC_DS_CAPS_VIEW caps;                // 0xd1c
    DWORD speaker_config;                // 0xd7c
    IDirectSoundBuffer *primary_buffer;  // 0xd80
    PC_DS_BUFFER_DESC_VIEW primary_desc; // 0xd84
#ifdef __cplusplus
    SoundVoiceManagerPC();
    ~SoundVoiceManagerPC();
    void Init(i32, u8, u8);
    i32 LoadWavSample(const char *, i32);
    void UnloadSample(i32);
    void ClearSamples(void);
    void SetOptionD14(u8);
    IDirectSound8 *GetFactory(void);
    void SetSampleName(const char *name, i32 sample_index);
    i32 AllocateVoice(i32, i32);
    bool FindLoopVoice(NUVEC *, i32, i32 *);
    void SetVoice(i32, i32, f32, i32);
    void SetVolumeTableIndex(i32, i32);
    void SetFrequencyParameter(i32, i32);
    void StopVoice(i32);
    void StopAllVoices(void);
    void UpdateVoices(void);
    void Setup3DVoice(i32, const NUVEC *, i32);
    u8 fun_00697360(i32 sample_index);
    u8 fun_00697390(i32 sample_index);
    void fun_00697700(i32 voice_id, i32 sound_key, i32, f32 unknown_1c, f32 unknown_20, i32 volume_index);
#endif
} SoundVoiceManagerPC;

typedef struct PC_SOUND_COMMIT PC_SOUND_COMMIT;

typedef struct PC_SOUND_COMMIT_METHODS {                                                                     // size 0x48
    void *unknown_00[2];                                                                                     // 0x00
    ULONG(__stdcall *release)(PC_SOUND_COMMIT *);                                                            // 0x08
    void *unknown_0c[10];                                                                                    // 0x0c
    HRESULT(__stdcall *set_orientation)(PC_SOUND_COMMIT *, float, float, float, float, float, float, DWORD); // 0x34
    HRESULT(__stdcall *set_position)(PC_SOUND_COMMIT *, float, float, float, DWORD);                         // 0x38
    HRESULT(__stdcall *set_rolloff_factor)(PC_SOUND_COMMIT *, float, DWORD);                                 // 0x3c
    void *unknown_40;                                                                                        // 0x40
    HRESULT(__stdcall *commit_deferred_settings)(PC_SOUND_COMMIT *);                                         // 0x44
} PC_SOUND_COMMIT_METHODS;

struct PC_SOUND_COMMIT {
    const PC_SOUND_COMMIT_METHODS *vtable;
};

struct nusound_filename_info_s;
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 PCSoundStorageLockReady;
    extern CRITICAL_SECTION PCSoundStorageLock;
    extern i32 PCSoundDrainStart;
    extern i32 PCSoundDrainRemaining;
/* NuSound3PlayStereoV's tags: each but the end tag is followed by its value. Tags 9 and 11 set fields whose
 * meaning is not settled; 12 takes no value and clears tag 9's. */
#define NUSOUND3_TAG_NONE 0   /* no value */
#define NUSOUND3_TAG_END 1    /* ends the list */
#define NUSOUND3_TAG_STREAM 2 /* the stream index */
#define NUSOUND3_TAG_SAMPLE 3 /* a single sample (first, and last = first + 1) */
#define NUSOUND3_TAG_FIRST_SAMPLE 4
#define NUSOUND3_TAG_LAST_SAMPLE 5
#define NUSOUND3_TAG_VOLUME 6 /* both channels */
#define NUSOUND3_TAG_VOLUME_LEFT 7
#define NUSOUND3_TAG_VOLUME_RIGHT 8
#define NUSOUND3_TAG_START_OFFSET 10 /* a double */
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
    void NuSound3SetRumblePads(void *pad_0, void *pad1);
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
    extern const IID PCSoundListenerIID;
    extern const GUID PCSampleBufferAlgorithm;
    extern i32 PCSampleLoadEnabled;
    extern i32 PCSoundSampleLimit;
    extern i32 PCSoundStreamSerial;
    void PCSoundPlayUnpositioned(i32, i32, i32, i32, f32, i32, f32, f32, i32);
    void PCSoundPlayUnpositionedWrapper(i32, i32, i32, i32, f32, i32, f32, f32);
    extern i32 PCSoundStartedOneShotCount;
    extern i32 PCSoundStartedLoopCount;
    extern i32 PCSoundUpdatedLoopCount;
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
    extern PC_SOUND_COMMIT *PCSoundCommitTarget;
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

extern "C" const IID PCSoundBuffer8IID;
extern "C" DWORD __stdcall PCSoundStreamWorker(void *);
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 NuSound3SfxLoading;
    i32 NuSound3LoadingSfx(void);
    extern CRITICAL_SECTION SoundSampleCriticalSection;
    extern HANDLE PCSoundSampleWorkerHandle;
    extern volatile u8 PCSoundSampleWorkerStop;
    extern char *PCSampleFilenameExtension;
    void NuSound3SetRequestTable(u16 *, i32);
    void NuSound3CancelCheckStereo(void);
#ifdef __cplusplus
}
#endif

/* music_playback_s.state, named after what SoundUpdate, PlayMusic, SwapMusic and PlayCutMusic do in each:
 * the SWAP fades pause the old stream when done, the CROSSFADEs stop it; a _START state waits for the new
 * stream to begin; the FAST ones (mode 3) fade ten times as fast. The DUAL_STREAM states play cutscene
 * music on the second stream: DUAL_STREAM resumes the game music after it, DUAL_STREAM_THEN_STOP (nothing
 * played before) stops, DUAL_STREAM_PENDING (linked cutscene audio) waits to be ended. */
#define MUSIC_PLAYBACK_STOPPED 0
#define MUSIC_PLAYBACK_PLAYING 4
#define MUSIC_PLAYBACK_SWAP_FADE 5
#define MUSIC_PLAYBACK_SWAP_FADE_FAST 6
#define MUSIC_PLAYBACK_CROSSFADE 7
#define MUSIC_PLAYBACK_CROSSFADE_START 8
#define MUSIC_PLAYBACK_CROSSFADE_FAST 9
#define MUSIC_PLAYBACK_CROSSFADE_FAST_START 10
#define MUSIC_PLAYBACK_DUAL_STREAM 11
#define MUSIC_PLAYBACK_DUAL_STREAM_THEN_STOP 12
#define MUSIC_PLAYBACK_DUAL_STREAM_PENDING 13

#ifdef __cplusplus
extern "C" {
#endif
    void NuSound3SuspendForFocus(void);
    void NuSound3ResumeAfterFocus(void);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void PCStereoStartPacket(struct PC_STEREO_PLAY_PACKET *packet);
    extern struct nudathdr_s *NuSoundDat;
    void NuSound3SetDat(struct nudathdr_s *header);
    extern MUSICPLAYBACK Music;
    struct numtx_s *NuSound3GetListener(void);
    extern struct numtx_s *NuSound3Listener;
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

struct nusound_filename_info_s { // size 0x1c
    char *filename;              // 0x00
    i32 pitch;                   // 0x04
    i32 static_sample;           // 0x08
    i32 index;                   // 0x0c
    i32 unknown_10;              // 0x10
    i32 unknown_14;              // 0x14
    i32 unknown_18;              // 0x18
};

#endif
#ifdef __cplusplus
extern "C" {
#endif
    void NuSound3SetSampleTable(SFX_FILE_INFO *, VARIPTR *, VARIPTR);
    i32 NuSound3InitV(VARIPTR *, VARIPTR, i32, i32, ...);
    i32 PCSoundInitialize(struct PC_SOUND_INIT_CONFIG *);
    struct nusound_filename_info_s *ConfigureMusic(char *, VARIPTR *, VARIPTR *);
    i32 NuSoundGetStereoStreamStatus(i32 stream);
    void fun_00697eb0(void);
    void PauseGameCut(void);
    void ResetSounds(void);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

struct SoundQueue {                    // size 0x400
    i32 active;                        // 0x000
    u32 unknown_0004;                  // 0x004
    i32 stream_state;                  // 0x008
    i32 stream_flags;                  // 0x00c
    i32 requested_volume;              // 0x010
    i32 applied_volume;                // 0x014
    i32 format_mode;                   // 0x018
    WAVEFORMATEX wave_format;          // 0x01c
    u8 unknown_02e[2];                 // 0x02e
    PC_DS_BUFFER_DESC_VIEW descriptor; // 0x030
    SoundQueueVolumeSink *volume_sink; // 0x054
    i32 buffer_seconds;                // 0x058

    union { // size 0x8

        struct {                           // size 0x8
            LPTIMECALLBACK timer_callback; // 0x05c
            HANDLE event;                  // 0x060
        };

        HANDLE wait_events[2];
    }; // 0x05c

    UINT timer_id;                   // 0x064
    HANDLE thread;                   // 0x068
    DWORD thread_id;                 // 0x06c
    CRITICAL_SECTION lock;           // 0x070
    CRITICAL_SECTION operation_lock; // 0x088
    CRITICAL_SECTION volume_lock;    // 0x0a0
    i32 command[32];                 // 0x0b8
    i32 argument[32];                // 0x138
    i32 count;                       // 0x1b8
    i32 read_index;                  // 0x1bc
    i32 write_index;                 // 0x1c0
    i32 unknown_1c4;                 // 0x1c4
    i32 decoded_bytes;               // 0x1c8
    i32 buffer_write_position;       // 0x1cc
    i32 loop_flags;                  // 0x1d0
    i32 unknown_1d4;                 // 0x1d4
    i32 pending_bytes;               // 0x1d8
    class WavReader *decoder;        // 0x1dc

    union { // size 0x105

        struct {                     // size 0x105
            char stream_path[0x104]; // 0x1e0
            u8 unknown_2e4;          // 0x2e4
        };

        char pending_path[0x105];
    }; // 0x1e0

    char current_path[0x105]; // 0x2e5
    u8 unknown_3ea[2];        // 0x3ea
    f32 start_offset;         // 0x3ec
    f32 current_offset;       // 0x3f0
    i32 unknown_03f4;         // 0x3f4
    i32 serial;               // 0x3f8
    IDirectSound8 *factory;   // 0x3fc

    SoundQueue(IDirectSound8 *, i32);
    ~SoundQueue(void);
    void SuspendForFocus(void);
    void CancelPause(void);
    DWORD Run(void);
    i32 SetFormat(const WAVEFORMATEX *);
    i32 FillBuffer(i32, u32);
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

#endif
