#ifndef SAGA_LEGOAPI_AUDIO_AUDIO_H
#define SAGA_LEGOAPI_AUDIO_AUDIO_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h, legoapi/legoapi_types.h,
 * legoapi/render/core/gameliball.h, legoapi/world/level.h (not all of it earlier in the shim). */

#include "legoapi/world/world.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nusound/nusound.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/world/level.h"
#endif

typedef struct repeat_sfx_entry_pc_s {
    i16 sfx_id;
    i8 state;
    i8 play_count;
    f32 start_delay;
    f32 repeat_delay;
    NUVEC *position;
} REPEAT_SFX_ENTRY_PC;

STATIC_ASSERT(sizeof(REPEAT_SFX_ENTRY_PC) == 0x10);
STATIC_ASSERT(offsetof(REPEAT_SFX_ENTRY_PC, sfx_id) == 0);
STATIC_ASSERT(offsetof(REPEAT_SFX_ENTRY_PC, state) == 2);
STATIC_ASSERT(offsetof(REPEAT_SFX_ENTRY_PC, play_count) == 3);
STATIC_ASSERT(offsetof(REPEAT_SFX_ENTRY_PC, start_delay) == 4);
STATIC_ASSERT(offsetof(REPEAT_SFX_ENTRY_PC, repeat_delay) == 8);
STATIC_ASSERT(offsetof(REPEAT_SFX_ENTRY_PC, position) == 0xc);

typedef struct optionssave_s {
    union {
        u8 unknown_0000[3];

        struct {
            u8 option_0000;
            u8 option_0001;
            u8 option_0002;
        };
    };

    u8 sound_volume;
    u8 music_volume;
    u8 master_volume;
    u8 music_enabled;
    u8 option_0007;
    u8 option_0008;
    u8 option_0009;
    u8 option_000a;
    u8 widescreen;
    u8 brightness;
} OPTIONSSAVE;

STATIC_ASSERT(sizeof(OPTIONSSAVE) == 0x0d);
STATIC_ASSERT(offsetof(OPTIONSSAVE, sound_volume) == 3);
STATIC_ASSERT(offsetof(OPTIONSSAVE, music_volume) == 4);
STATIC_ASSERT(offsetof(OPTIONSSAVE, master_volume) == 5);
STATIC_ASSERT(offsetof(OPTIONSSAVE, widescreen) == 0x0b);
#ifdef __cplusplus
extern "C" {
#endif
    /* ObjHitObj/Shield prototypes belong to the independently reviewed combat SCC packet. */

    void EngineNoiseCode(GAMEOBJECT *, i32);
    void AddLevelSfxFromName(char *, i32 *, i32 *, i32);
    void AddLevelSfxFromId(i32, i32 *, i32 *, i32);
    void GameAudio_AddSfx(i32, i32 *, i32 *, i32);
    void PlaySabreSfx(char *, GAMEOBJECT *, NUVEC *, i32);
    void PlayGruntSfx(GAMEOBJECT *);
    void PlayHurtSfx(GAMEOBJECT *object);
    extern void (*ExtraHurtSfxFn)(GAMEOBJECT *object);
    void PlayLandSfx(GAMEOBJECT *object, i32 variant, i32 ignored);
    extern i32 TerrainLastChatterSfxPc;
    extern f32 TerrainChatterWaitPc;
    f32 GameGetMusicVolume(OPTIONSSAVE *options);
    f32 GameSetMusicVolume(OPTIONSSAVE *options);
    f32 GameSetSoundVolume(OPTIONSSAVE *options);
    void SfxCheckMusicOnOff(OPTIONSSAVE *options);
    void SetBackgroundMusic(i32 track);
    i32 GameAudio_GetPlrSfxBits(void *);
    void PlayDieSfx(GAMEOBJECT *);
    extern void (*ExtraDieSfxFn)(GAMEOBJECT *);
    void UpdateBackgroundMusic(void);
    void ClearLinkedCutSceneMusic(void *context);
    void ProcessMusicChanges(LEVELDATA_PC *level, OPTIONSSAVE *options);
    void PauseGameAudio(i32 ignored);
    void SetLinkedCutSceneMusic(void *context, i32 state);
    void PlayJumpSfx(GAMEOBJECT *, i32);
    i32 GetMusicIndex(char *name, struct nusound_filename_info_s *table, i32 fallback);
    void GameAudio_PlaySfxById(i32, NUVEC *, i32, i32);
    void GameAudio_PlaySfx(i32, NUVEC *, i32, i32);
    void AddLevSfx(WORLDINFO *, NUVEC *, char *, i32);
    void UpdateLevelSfx(WORLDINFO *, i32);
    extern const char AITransferLiteral757a38[12];
    extern const char AITransferLiteral757a70[11];
    extern i32 BeenAttacked;
    extern REPEAT_SFX_ENTRY_PC RepeatSfxQueue[32];
    extern i32 RepeatSfxQueueHead;
    void PlayRepeatSfx(char *, i32, f32, char, f32, NUVEC *);
    void UpdateRepeatSfx(void);
    extern GAMEAUDIO_PC GameAudioStorage;
    extern GAMEAUDIO_PC *GameAudio;
    void SfxSetFileTable(SFX_FILE_INFO_PC *records);
    extern i16 *AmbientPairTab;
    void PlayFootStepSfx(GAMEOBJECT *);
    typedef struct PC_MUSIC_ACTION_PAIR_PC PC_MUSIC_ACTION_PAIR_PC;
    extern PC_MUSIC_ACTION_PAIR_PC *ActionPairTab;
    extern i32 (*CheckMusicOtherFn)(void);
    extern i32 (*GameAudio_ActionMusicFn)(void);
    void GameAudio_PlaySfxAndSetVolume(i32 sfx, NUVEC *position, f32 volume);
    i32 GameAudio_GetSfxId(i32);
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(OPTIONSSAVE, option_0000) == 0);
#ifdef __cplusplus
extern "C" {
#endif
    extern f32 ActionMusicStickyTimeoutPc[2];
    void GameAudio_SetActionMusicTimes(f32 initial_delay, f32 hold_time);
    extern const char *SfxMusicExtensionPc;

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

    void GameAudio_Init(GAMEAUDIO_PC *audio);
    extern i32 ticktock;
    void TickTockSfx(void);
#ifdef __cplusplus
}
#endif

#endif
