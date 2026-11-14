#ifndef SAGA_NU2API_NUMUSIC_NUMUSIC_H
#define SAGA_NU2API_NUMUSIC_NUMUSIC_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nusound/nusound.h (not all of it earlier in the shim). */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/nufile/nufpar.h"

#ifndef SAGA_SHIM
#include "nu2api/nusound/nusound.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    f32 numusicGetDuckVolume(void);
    extern i32 NOMUSIC;
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

class NuMusic {
  public:
    class Track {
      public:
        char *filenames[2];
        char *ident;
        i32 file_indexes[2];
        u32 clazz;
        f32 *entry_times;
        i32 entry_count;
        i32 entry_index;
        f32 start_offset;
        i32 pitch;
        f32 duck_volume;
        f32 duck_fade;
        f32 attenuation;
        u32 flags;
        void ManageEntryTime(void);
        void SetEntryTime(f32 time);
    };

    class Voice {
      public:
        i32 stream_index;
        Track *tracks[2];
        i32 track_sub[2];
        i32 track_index;
        i32 status;
        f32 gain;
        f32 volume;
        f32 last_volume;
        f32 fade_rate;
        f32 play_time;

        union {
            struct {
                u8 flags;
                u8 unknown_0031[3];
            };

            u32 flags_word;
        };

        void SetStatusFn(i32 new_status, i32 tag);
        i32 Load(Track *track, i32 subtrack);
        i32 Cue(void);
        i32 Play(void);
        i32 Unload(void);
    };

    struct Album {
        char *name;
        Track *tracks_source;
        i32 tracks_count;
        Track *tracks[6];
        Track *GetTrack(u32 class_mask);
        i32 GetTracks(u32 class_mask, Track **out_tracks);
        void Initialise(void);
    };

    Album *albums;
    i32 album_count;
    Track *extra_tracks;
    i32 extra_track_count;
    f32 *indexes;
    i32 index_count;
    i32 pitch_default;
    Album *current_album;
    Track *current_track;
    char current_path[256];
    u8 strict_mode;
    u8 unknown_125[3];
    char *string_pool_start;
    char *string_pool_end;
    Voice voices[2];
    f32 master_volume;
    f32 fader_current;
    f32 fader_target;
    f32 fader_rate;
    f32 duck_gain;
    f32 duck_current;
    f32 duck_rate;
    f32 class_volumes[6];
    f32 global_attenuation;
    i32 track_index;
    char *language;
    Album *album;
    nusound_filename_info_s *fileinfo;
    i32 file_count;

    void InitVoiceManager(void);
    char *RemovePath(char *);
    char *FindString(const char *);
    char *AllocString(const char *);
    void SubstituteString(char *src, char *dst, char *find, char *subst);
    i32 FindOrCreateSoundFile(nusound_filename_info_s *, i32 *, const char *, i32, i32);
    void BuildSoundTable(VARIPTR *, VARIPTR);
    void InitData(const char *, VARIPTR *, VARIPTR);
    i32 Initialise(const char *, char *, VARIPTR *, VARIPTR);
    void ParseTrack(u32, NUFPAR_PC *);
    void xAlbum(NUFPAR_PC *);
    static void xsAlbum(NUFPAR_PC *, void *);
    void xAction(NUFPAR_PC *);
    static void xsAction(NUFPAR_PC *, void *);
    void xQuiet(NUFPAR_PC *);
    static void xsQuiet(NUFPAR_PC *, void *);
    void xOverlay(NUFPAR_PC *);
    static void xsOverlay(NUFPAR_PC *, void *);
    void xSignature(NUFPAR_PC *);
    static void xsSignature(NUFPAR_PC *, void *);
    void xCutscene(NUFPAR_PC *);
    static void xsCutscene(NUFPAR_PC *, void *);
    void xNoMusicC(NUFPAR_PC *);
    static void xsNoMusicC(NUFPAR_PC *, void *);
    void xGlobalAttenuation(NUFPAR_PC *);
    static void xsGlobalAttenuation(NUFPAR_PC *, void *);
    void xPath(NUFPAR_PC *);
    static void xsPath(NUFPAR_PC *, void *);
    void xStrict(NUFPAR_PC *);
    static void xsStrict(NUFPAR_PC *, void *);
    void xIdent(NUFPAR_PC *);
    static void xsIdent(NUFPAR_PC *, void *);
    void xIndex(NUFPAR_PC *);
    static void xsIndex(NUFPAR_PC *, void *);
    void xNoMusic(NUFPAR_PC *);
    static void xsNoMusic(NUFPAR_PC *, void *);
    void xNoDuck(NUFPAR_PC *);
    static void xsNoDuck(NUFPAR_PC *, void *);
    void xDuck(NUFPAR_PC *);
    static void xsDuck(NUFPAR_PC *, void *);
    void xLooping(NUFPAR_PC *);
    static void xsLooping(NUFPAR_PC *, void *);
    void xNonLooping(NUFPAR_PC *);
    static void xsNonLooping(NUFPAR_PC *, void *);
    void xAttenuation(NUFPAR_PC *);
    static void xsAttenuation(NUFPAR_PC *, void *);
    static void GlobalParseErrorFn(NUFPAR_PC *);
    static void TrackParseErrorFn(NUFPAR_PC *);
    static NUFPCOMJMPCTX top_jmp_tab[11];
    static NUFPCOMJMPCTX track_jmp_tab[15];

    void SetFader(f32 target, f32 fade_seconds);
    void SetMasterVolume(f32 volume);
    i32 GetAlbumHandle(const char *name);
    i32 SetAlbum(i32 handle);
    i32 SetAlbum(const char *name);
    static i32 ClassToIX(u32 class_mask);
    const char *ClassToName(u32 class_mask);
    void SetClassVolume(u32 class_mask, f32 volume);
    void Debug(i32 unused, i32 y);
    void GetSoundFiles(nusound_filename_info_s **files, i32 *count);
    i32 PauseTrack(u32 class_mask);
    i32 ResumeTrack(u32 class_mask);
    i32 SelectTrackByHandle(u32 class_mask, i32 handle);
    i32 GetTrackHandle(u32 class_mask, const char *name);
    i32 GetStatus(u32 class_mask, i32 *per_class_status);
    i32 SelectTrack(u32 class_mask, const char *name);
    Voice *FindVoiceByTrack(Track *track);
    Voice *FindVoiceByClass(u32 class_mask);
    Voice *FindVoiceByClassAndStatus(u32 class_mask, u32 status);
    Voice *FindIdleVoice(void);
    i32 StopAll(i32 mode);
    i32 StopTrack(u32 class_mask, i32 fade);
    i32 SetTrackEntryTimeByClass(u32 class_mask, f32 time);
    i32 PlayTrack(u32 class_mask);
    i32 PlayTrack(u32 class_mask, u32 unused);
    i32 PlayTrackI(u32 class_mask, u32 prepare_only);
    i32 CueTrack(u32 class_mask);
    void Process(f32 delta);
    f32 fun_00693ba0();
    static NuMusic *GetPlayer();
    void NoMusic(i32 no_music);
};

STATIC_ASSERT(sizeof(NuMusic::Track) == 0x3c);
STATIC_ASSERT(offsetof(NuMusic::Track, attenuation) == 0x34);
STATIC_ASSERT(offsetof(NuMusic::Track, duck_fade) == 0x30);
STATIC_ASSERT(offsetof(NuMusic::Track, duck_volume) == 0x2c);
STATIC_ASSERT(offsetof(NuMusic, global_attenuation) == 0x1cc);
STATIC_ASSERT(offsetof(NuMusic, duck_rate) == 0x1b0);
STATIC_ASSERT(offsetof(NuMusic, duck_gain) == 0x1a8);
STATIC_ASSERT(offsetof(NuMusic::Track, filenames) == 0);
STATIC_ASSERT(offsetof(NuMusic::Track, ident) == 0x08);
STATIC_ASSERT(offsetof(NuMusic::Track, file_indexes) == 0x0c);
STATIC_ASSERT(offsetof(NuMusic::Track, clazz) == 0x14);
STATIC_ASSERT(offsetof(NuMusic::Track, start_offset) == 0x24);
STATIC_ASSERT(offsetof(NuMusic::Track, entry_times) == 0x18);
STATIC_ASSERT(offsetof(NuMusic::Track, entry_count) == 0x1c);
STATIC_ASSERT(offsetof(NuMusic::Track, entry_index) == 0x20);
STATIC_ASSERT(offsetof(NuMusic::Track, flags) == 0x38);
STATIC_ASSERT(offsetof(NuMusic::Voice, flags_word) == 0x30);
STATIC_ASSERT(offsetof(NuMusic::Voice, stream_index) == 0);
STATIC_ASSERT(offsetof(NuMusic::Voice, last_volume) == 0x24);
STATIC_ASSERT(offsetof(NuMusic::Voice, fade_rate) == 0x28);
STATIC_ASSERT(sizeof(NuMusic::Voice) == 0x34);
STATIC_ASSERT(offsetof(NuMusic::Voice, tracks) == 0x04);
STATIC_ASSERT(offsetof(NuMusic::Voice, track_index) == 0x14);
STATIC_ASSERT(offsetof(NuMusic::Voice, status) == 0x18);
STATIC_ASSERT(offsetof(NuMusic::Voice, track_sub) == 0x0c);
STATIC_ASSERT(offsetof(NuMusic::Voice, gain) == 0x1c);
STATIC_ASSERT(offsetof(NuMusic::Voice, play_time) == 0x2c);
STATIC_ASSERT(offsetof(NuMusic::Voice, flags) == 0x30);
STATIC_ASSERT(sizeof(NuMusic::Album) == 0x24);
STATIC_ASSERT(offsetof(NuMusic::Album, name) == 0);
STATIC_ASSERT(offsetof(NuMusic::Album, tracks) == 0x0c);
STATIC_ASSERT(offsetof(NuMusic, albums) == 0);
STATIC_ASSERT(offsetof(NuMusic, album_count) == 4);
STATIC_ASSERT(offsetof(NuMusic, extra_tracks) == 0x08);
STATIC_ASSERT(offsetof(NuMusic, extra_track_count) == 0x0c);
STATIC_ASSERT(offsetof(NuMusic, voices) == 0x130);
STATIC_ASSERT(offsetof(NuMusic, master_volume) == 0x198);
STATIC_ASSERT(offsetof(NuMusic, fader_current) == 0x19c);
STATIC_ASSERT(offsetof(NuMusic, fader_target) == 0x1a0);
STATIC_ASSERT(offsetof(NuMusic, fader_rate) == 0x1a4);
STATIC_ASSERT(offsetof(NuMusic, duck_current) == 0x1ac);
STATIC_ASSERT(offsetof(NuMusic, class_volumes) == 0x1b4);
STATIC_ASSERT(offsetof(NuMusic, track_index) == 0x1d0);
STATIC_ASSERT(offsetof(NuMusic, album) == 0x1d8);
STATIC_ASSERT(offsetof(NuMusic, fileinfo) == 0x1dc);
STATIC_ASSERT(offsetof(NuMusic, file_count) == 0x1e0);
STATIC_ASSERT(offsetof(NuMusic, indexes) == 0x10);
STATIC_ASSERT(offsetof(NuMusic, index_count) == 0x14);
STATIC_ASSERT(offsetof(NuMusic, pitch_default) == 0x18);
STATIC_ASSERT(offsetof(NuMusic, current_album) == 0x1c);
STATIC_ASSERT(offsetof(NuMusic, current_track) == 0x20);
STATIC_ASSERT(offsetof(NuMusic, current_path) == 0x24);
STATIC_ASSERT(offsetof(NuMusic, strict_mode) == 0x124);
STATIC_ASSERT(offsetof(NuMusic, string_pool_start) == 0x128);
STATIC_ASSERT(offsetof(NuMusic, string_pool_end) == 0x12c);
STATIC_ASSERT(offsetof(NuMusic, language) == 0x1d4);
STATIC_ASSERT(offsetof(NuMusic::Track, pitch) == 0x28);

extern NuMusic music_man;
#endif

#endif
