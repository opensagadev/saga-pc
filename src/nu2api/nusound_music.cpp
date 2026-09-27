#include "saga.h"

struct nusound_filename_info_s;

class NuMusic {
  public:
    u8 unknown_0000[0x19c];
    f32 fader_current;
    f32 fader_target;
    f32 fader_rate;
    u8 unknown_01a8[0x34];
    nusound_filename_info_s *fileinfo;
    i32 file_count;

    void SetFader(f32 target, f32 fade_seconds);
    void GetSoundFiles(nusound_filename_info_s **files, i32 *count);
};

STATIC_ASSERT(offsetof(NuMusic, fader_current) == 0x19c);
STATIC_ASSERT(offsetof(NuMusic, fader_target) == 0x1a0);
STATIC_ASSERT(offsetof(NuMusic, fader_rate) == 0x1a4);
STATIC_ASSERT(offsetof(NuMusic, fileinfo) == 0x1dc);
STATIC_ASSERT(offsetof(NuMusic, file_count) == 0x1e0);

extern NuMusic *the_music_player;

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
