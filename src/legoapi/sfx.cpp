#include "legoapi/audio/audio.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/render/core/gameliball.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic/sfx.h"
#include <stdio.h>

extern "C" f32 AUDIOFADELEVEL;

void SetAudioFadeLevel(f32 level) {
    AUDIOFADELEVEL = level;
}

f32 GetAudioFadeLevel(void) {
    return AUDIOFADELEVEL;
}

void SfxCheckMusicOnOff(OPTIONSSAVE *options) {
}

extern "C" void PlaySfxByIdAndSetVolumeAndPitch(i32 sfx_id, NUVEC *position, f32 volume, f32 pitch) {
    PlaySfxByIdEx(sfx_id, position, volume, pitch);
}

extern "C" void PlaySfxByIdAndSetPitch(i32 sfx_id, NUVEC *position, f32 pitch) {
    PlaySfxByIdEx(sfx_id, position, 1.0f, pitch);
}

extern "C" {
    typedef struct footstep_audio_callbacks_s {      // size 0x4
        i32 (*override_footstep)(GAMEOBJECT *, i32); // 0x0
    } FOOTSTEP_AUDIO_CALLBACKS;
}

extern "C" i32 IsSfxLooping(i32 id) {
    if (id != -1) {
        return (SfxInfo[id].sample_id_flags >> 15) & 1;
    }
    return -1;
}
