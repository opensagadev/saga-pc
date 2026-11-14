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

extern "C" void PlaySfxByIdAndSetPitch(i32 sfx_id, NUVEC *position, f32 pitch) {
    PlaySfxByIdEx(sfx_id, position, 1.0f, pitch);
}

extern "C" {}

extern "C" i32 IsSfxLooping(i32 id) {
    if (id != -1) {
        return (SfxInfo[id].sample_id_flags >> 15) & 1;
    }
    return -1;
}
