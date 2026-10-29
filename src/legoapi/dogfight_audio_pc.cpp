#include "saga.h"

typedef struct DOGFIGHT_AUDIO_DOOR_PC {
    char name[12];
    f32 distance;
    f32 timer;
} DOGFIGHT_AUDIO_DOOR_PC;
STATIC_ASSERT(sizeof(DOGFIGHT_AUDIO_DOOR_PC) == 20);
STATIC_ASSERT(offsetof(DOGFIGHT_AUDIO_DOOR_PC, distance) == 12);
STATIC_ASSERT(offsetof(DOGFIGHT_AUDIO_DOOR_PC, timer) == 16);
typedef struct DOGFIGHT_AUDIO_SPACE_PC {
    u8 unknown_00[0x6371c];
    f32 door_time;
} DOGFIGHT_AUDIO_SPACE_PC;
STATIC_ASSERT(offsetof(DOGFIGHT_AUDIO_SPACE_PC, door_time) == 0x6371c);
STATIC_ASSERT(offsetof(WORLDINFO, level_specific_data) == 0x511c);
STATIC_ASSERT(offsetof(MUSICPLAYBACK_PC, seek_offset) == 0x14);

extern "C" {
    extern DOGFIGHT_AUDIO_DOOR_PC DogfightDoorsPc[7];
    void ResetSpaceFromWorldPc(WORLDINFO *world);
#pragma data_seg(".data$DogfightDebrisKeysPc")
    i32 DogfightDebrisKeysPc[4] = {-1, -1, -1, -1};
#pragma data_seg()

    void SetPreSeekStartPoint(f32 offset) {
        Music.seek_offset = offset;
    }

    void SpaceAudioPoint(void) {
        music_man.SetTrackEntryTimeByClass(2, ((DOGFIGHT_AUDIO_SPACE_PC *)WORLD->level_specific_data)->door_time);
        music_man.SetTrackEntryTimeByClass(0x20, ((DOGFIGHT_AUDIO_SPACE_PC *)WORLD->level_specific_data)->door_time);
    }

    void SpaceResetAudioPoint(void) {
        GAMEOBJECT *first = (GAMEOBJECT *)Player[0];
        GAMEOBJECT *second = (GAMEOBJECT *)Player[1];
        for (i32 i = 6; i >= 0; --i) {
            if (!(first != NULL && first->sock_position.distance > DogfightDoorsPc[i].distance) &&
                !(second != NULL && second->sock_position.distance > DogfightDoorsPc[i].distance))
                continue;
            music_man.SetTrackEntryTimeByClass(2, DogfightDoorsPc[i].timer);
            music_man.SetTrackEntryTimeByClass(0x20, DogfightDoorsPc[i].timer);
            return;
        }
        SetPreSeekStartPoint(0.0f);
    }

    void ChrisDogFightAReset(WORLDINFO *world) {
        SpaceResetAudioPoint();
        ResetSpaceFromWorldPc(world);
        BOLT_OVERRIDE_PLAYERBOLTSPEED = 150.0f;
        BOLT_OVERRIDE_PLAYERBOLTDURATION = 1.5f;
        DogfightDebrisKeysPc[0] = -1;
        DogfightDebrisKeysPc[1] = -1;
        DogfightDebrisKeysPc[2] = -1;
        DogfightDebrisKeysPc[3] = -1;
        music_man.StopTrack(2, 0);
        music_man.StopTrack(0x20, 0);
    }
}
