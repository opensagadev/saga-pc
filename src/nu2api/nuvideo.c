#include "saga.h"

i32 NuVideoGetMode(void) {
    return nuapi.video_mode;
}

void NuVideoSetMode(i32 mode) {
    nuapi.video_mode = mode;
    switch (mode) {
        case 3:
        case 4:
            nuapi.video_is_pal = 1;
            break;
        case 2:
        case 5:
        case 6:
            nuapi.video_is_pal = 0;
            break;
        default:
            break;
    }
}

i32 NuVideoGetAspect(void) {
    return nuapi.video_aspect;
}

void NuVideoSetResolution(i32 width, i32 height) {
    nuapi.screen_width = width;
    nuapi.screen_height = height;
}

i32 NuVideoGetSwapMode(void) {
    return nuapi.video_swap_mode;
}

i32 NuVideoSetSwapMode(i32 mode) {
    i32 previous_mode = nuapi.video_swap_mode;

    nuapi.video_swap_mode = mode;
    switch (mode) {
        case 1:
        case 2:
        case 5:
            if (nuapi.video_is_pal != 0) {
                NuVideoFrameRate = 50.0f;
            } else {
                NuVideoFrameRate = 60.0f;
            }
            break;
        case 3:
        case 4:
        case 6:
            if (nuapi.video_is_pal != 0) {
                NuVideoFrameRate = 25.0f;
            } else {
                NuVideoFrameRate = 30.0f;
            }
            break;
        default:
            break;
    }

    NuVideoSetSwapModePS(mode);
    return previous_mode;
}

void NuVideoRollingFrameRateReset(void) {
}

void NuVideoSetBrightness(f32 brightness) {
    nuapi.video_brightness = brightness;
    NuVideoSetBrightnessPS();
}

i32 NuLanguageGet(void) {
    return nuapi.language;
}

void NuLanguageSet(i32 language) {
    nuapi.language = language;
}

i32 NuRegionGet(void) {
    return nuapi.region;
}

void NuRegionSet(i32 region) {
    nuapi.region = region;
}

void __cdecl bgProcDataOversize(i32 size) {
}

GAMECAMERA *GameCam;
i32 part_debris_count;

void __cdecl NewRumbleAllPlayers(f32 strength, f32 duration, i32 frames, f32 priority) {
    i32 index;
    GAMEOBJECT *object;

    if (frames > 0) {
        if ((f32)frames / DEFAULTFPS > duration) {
            duration = (f32)frames / DEFAULTFPS;
        }
    }
    for (index = 0; index < 8; index++) {
        object = (GAMEOBJECT *)Player[index];
        if (object != NULL && (object->flags_1fc & 0x80) != 0 && object->pad != NULL) {
            NuSound3AddRumble(object->pad, duration, (i32)(strength * 255.0f), 0, strength);
        }
    }
}

void __cdecl GameCam_NewShake(GAMECAMERA *camera, f32 intensity, f32 decay, f32 freq) {
    if (camera == NULL) {
        camera = GameCam;
    }
    camera->shake_intensity = intensity;
    camera->shake_decay = decay;
    camera->shake_freq = freq;
}
