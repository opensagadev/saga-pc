#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct GAMEAUDIO;
struct WORLDINFO_s;
struct gameobject_s;
struct level_data_s;
struct nusound_filename_info_s;
struct sfx_file_info_s;

typedef struct repeat_sfx_entry_s { // size 0x10
    i16 sfx_id;                     // 0x0
    i8 state;                       // 0x2
    i8 play_count;                  // 0x3
    f32 start_delay;                // 0x4
    f32 repeat_delay;               // 0x8
    NUVEC *position;                // 0xc
} REPEAT_SFX_ENTRY;

typedef struct optionssave_s { // size 0xd

    union {           // size 0x3
        u8 rumble[2]; /* per player */

        struct {               // size 0x3
            u8 rumble_player1; // 0x0
            u8 rumble_player2; // 0x1
            u8 dolby_surround; // 0x2: NuSound3SetDPL
        };
    }; // 0x0

    u8 sound_volume;  // 0x3
    u8 music_volume;  // 0x4
    u8 master_volume; // 0x5
    u8 music_enabled; // 0x6
    u8 option_0007;   // 0x7
    u8 option_0008;   // 0x8
    u8 option_0009;   // 0x9
    u8 option_000a;   // 0xa
    u8 widescreen;    // 0xb
    u8 brightness;    // 0xc
} OPTIONSSAVE;

#ifdef __cplusplus
extern "C" {
#endif
    extern OPTIONSSAVE TempOptions;

    /* ObjHitObj/Shield prototypes belong to the independently reviewed combat SCC packet. */

    void EngineNoiseCode(struct gameobject_s *, i32);
    void AddLevelSfxFromName(char *, i32 *, i32 *, i32);
    void AddLevelSfxFromId(i32, i32 *, i32 *, i32);
    void GameAudio_AddSfx(i32, i32 *, i32 *, i32);
    void PlaySabreSfx(char *, struct gameobject_s *, NUVEC *, i32);
    void PlayGruntSfx(struct gameobject_s *);
    void PlayHurtSfx(struct gameobject_s *object);
    extern void (*ExtraHurtSfxFn)(struct gameobject_s *object);
    void PlayLandSfx(struct gameobject_s *object, i32 variant, i32 ignored);
    extern i32 TerrainLastChatterSfx;
    extern f32 TerrainChatterWait;
    f32 GameGetMusicVolume(OPTIONSSAVE *options);
    f32 GameSetMusicVolume(OPTIONSSAVE *options);
    f32 GameSetSoundVolume(OPTIONSSAVE *options);
    void SfxCheckMusicOnOff(OPTIONSSAVE *options);
    void SetBackgroundMusic(i32 track);
    i32 GameAudio_GetPlrSfxBits(void *);
    void PlayDieSfx(struct gameobject_s *);
    extern void (*ExtraDieSfxFn)(struct gameobject_s *);
    void UpdateBackgroundMusic(void);
    void ClearLinkedCutSceneMusic(void *context);
    void ProcessMusicChanges(struct level_data_s *level, OPTIONSSAVE *options);
    void PauseGameAudio(i32 ignored);
    void SetLinkedCutSceneMusic(void *context, i32 state);
    void PlayJumpSfx(struct gameobject_s *, i32);
    i32 GetMusicIndex(char *name, struct nusound_filename_info_s *table, i32 fallback);
    void GameAudio_PlaySfxById(i32, NUVEC *, i32, i32);
    void GameAudio_PlaySfx(i32, NUVEC *, i32, i32);
    void AddLevSfx(struct WORLDINFO_s *, NUVEC *, char *, i32);
    void UpdateLevelSfx(struct WORLDINFO_s *, i32);
    extern i32 BeenAttacked;
    extern REPEAT_SFX_ENTRY RepeatSfxQueue[32];
    extern i32 RepeatSfxQueueHead;
    void PlayRepeatSfx(char *, i32, f32, char, f32, NUVEC *);
    void UpdateRepeatSfx(void);
    extern struct GAMEAUDIO GameAudioStorage;
    extern struct GAMEAUDIO *GameAudio;
    void SfxSetFileTable(struct sfx_file_info_s *records);
    extern i16 *AmbientPairTab;
    void PlayFootStepSfx(struct gameobject_s *);
    typedef struct PC_MUSIC_ACTION_PAIR PC_MUSIC_ACTION_PAIR;
    extern PC_MUSIC_ACTION_PAIR *ActionPairTab;
    extern i32 (*CheckMusicOtherFn)(void);
    extern i32 (*GameAudio_ActionMusicFn)(void);
    void GameAudio_PlaySfxAndSetVolume(i32 sfx, NUVEC *position, f32 volume);
    i32 GameAudio_GetSfxId(i32);

    extern f32 ActionMusicStickyTimeout[2];
    void GameAudio_SetActionMusicTimes(f32 initial_delay, f32 hold_time);
    extern const char *SfxMusicExtension;

    extern i32 CurrentMusicPair_Quiet;
    extern i32 MusicActive;
    extern i32 MusicActiveReset;
    extern i32 MusicFindCut;
    extern i32 MusicInside;
    extern i32 MusicOnFlag;
    extern i32 MusicPlrsHoldAttack;
    extern i32 MusicPlrsUnderAttack;
    extern i32 MusicRestoreTrack;
    extern i32 MusicSeekIndex;
    extern f32 MusicSwapDelay;
    extern i32 PlayersUnderAttack;
    extern f32 sticky_attack_time;

    // Project diagnostics are selected at the end of this header; see debug/diagnostics.h.
    // Kept at the end so Matching line numbers and native declarations stay unchanged.

    void GameAudio_Init(struct GAMEAUDIO *audio);
    extern i32 ticktock;
    void TickTockSfx(void);
    extern i32 MusicOther;
    extern i32 last_currentmusicpair_quiet;
    extern i32 sticky_attack;
    i32 AmbientFromQuiet(i32);
    void MusicClearAll(void);
    void GameAudio_Reset(void);
    void GamePlayMusic(struct level_data_s *, i32, OPTIONSSAVE *);
    void NewAreaMusicChanges(void);
    void ResetRepeatSfx(void);
    void SetSoundFadeDist(struct WORLDINFO_s *world, OPTIONSSAVE *options);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
void legoSetMusicVolume(f32);
#endif
