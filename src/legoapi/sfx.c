#include "saga.h"

f32 GameGetMusicVolume(OPTIONSSAVE *options) {
    f32 volume = (f32)options->music_volume / 10.0;
    return (f32)(volume * ((f64)options->master_volume / 10.0));
}

f32 GameSetMusicVolume(OPTIONSSAVE *options) {
    f32 volume = GameGetMusicVolume(options);
    if (options->music_enabled == 0 && WORLD->current_level == TITLES_LDATA) {
        volume = 0.0f;
    }
    STUBBED_NAMED(0x00694db0); /* music_man.SetClassVolume(0x23, volume), inlined legoSetMusicVolume */
    return volume;
}

f32 GameSetSoundVolume(OPTIONSSAVE *options) {
    f32 volume = (f32)options->sound_volume / 10.0;
    f32 cut_volume;
    volume = (f32)(volume * ((f64)options->master_volume / 10.0));
    SetSoundVolume(volume);
    cut_volume = (f32)((f64)0.85f * (f64)options->master_volume / 10.0);
    STUBBED_NAMED(0x00694db0); /* music_man.SetClassVolume(0x10, cut_volume), inlined legoSetCutVolume */
    (void)cut_volume;
    return volume;
}

void SfxCheckMusicOnOff(OPTIONSSAVE *options) {
    (void)options;
}
