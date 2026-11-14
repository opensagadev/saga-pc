#include "saga.h"
#include <stdio.h>

extern "C" f32 AUDIOFADELEVEL;

void SetAudioFadeLevel(f32 level) {
    AUDIOFADELEVEL = level;
}

f32 GetAudioFadeLevel(void) {
    return AUDIOFADELEVEL;
}

void SfxCheckMusicOnOff(OPTIONSSAVE *options) {
    (void)options;
}

extern "C" void PlaySfxByIdAndSetVolumeAndPitch(i32 sfx_id, NUVEC *position, f32 volume, f32 pitch) {
    PlaySfxByIdEx(sfx_id, position, volume, pitch);
}

typedef struct level_sfx_gizmotype_s {
    u8 unknown_00[0x94];                                           // 0x00
    void (*add_level_sfx)(WORLDINFO *, void *, i32 *, i32 *, i32); // 0x94
    u8 unknown_98[4];                                              // 0x98
} LEVEL_SFX_GIZMOTYPE_PC;                                          // size 0x9c

extern "C" void PlaySfxByIdAndSetPitch(i32 sfx_id, NUVEC *position, f32 pitch) {
    PlaySfxByIdEx(sfx_id, position, 1.0f, pitch);
}

extern "C" {
    typedef struct footstep_audio_callbacks_pc_s {
        i32 (*override_footstep)(GAMEOBJECT *, i32); // 0x0
    } FOOTSTEP_AUDIO_CALLBACKS_PC;                   // size 0x4
}

extern "C" i32 IsSfxLooping(i32 id) {
    if (id != -1) {
        return (SfxInfo[id].sample_id_flags >> 15) & 1;
    }
    return -1;
}
