#include "saga.h"

void NuMusic::SetFader(f32 target, f32 fade_seconds) {
    if (this == NULL || the_music_player == NULL) {
        return;
    }

    if (fade_seconds != 0.0f) {
        fader_rate = 1.0f / fade_seconds;
        fader_target = target;
        return;
    }

    fader_target = target;
    fader_current = target;
    fader_rate = 0.0f;
}

void NuMusic::SetClassVolume(u32 class_mask, f32 volume) {
    if (this == NULL || the_music_player == NULL) {
        return;
    }

    if (volume > 1.0f)
        volume = 1.0f;
    if (volume < 0.0)
        volume = 0.0f;
    if (volume > 1.0f)
        volume = 1.0f;

    if ((class_mask & 0x01) != 0)
        class_volumes[0] = volume;
    if ((class_mask & 0x02) != 0)
        class_volumes[1] = volume;
    if ((class_mask & 0x04) != 0)
        class_volumes[2] = volume;
    if ((class_mask & 0x08) != 0)
        class_volumes[3] = volume;
    if ((class_mask & 0x10) != 0)
        class_volumes[4] = volume;
    if ((class_mask & 0x20) != 0)
        class_volumes[5] = volume;
}

void NuMusic::GetSoundFiles(nusound_filename_info_s **files, i32 *count) {
    if (this != NULL) {
        if (files != NULL) {
            *files = fileinfo;
        }
    } else if (files != NULL) {
        *files = NULL;
    }

    if (count != NULL) {
        if (this != NULL) {
            *count = file_count;
        } else {
            *count = 0;
        }
    }
}

extern "C" f32 numusicGetDuckVolume(void) {
    f32 volume;

    if (the_music_player != NULL) {
        volume = the_music_player->duck_current;
    } else {
        volume = 1.0f;
    }
    return volume;
}
