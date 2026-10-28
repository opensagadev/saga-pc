#include "saga.h"
#include <stdarg.h>
#include <stdio.h>

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

void __cdecl PCDeviceResetState(void) {
    i32 stage;

    PCThreadCriticalSectionEnter_candidate(PCFrameCriticalSectionIndex_candidate);
    PCDeviceSetRenderState(137, 0);
    PCDeviceSetRenderState(7, 1);
    PCDeviceSetRenderState(14, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(19, 2);
    PCDeviceSetRenderState(20, 1);
    PCDeviceSetRenderState(24, 0);
    PCDeviceSetRenderState(25, 8);
    PCDeviceSetRenderState(27, 0);
    PCDeviceSetRenderState(52, 0);
    for (stage = 0; stage < 8; ++stage) {
        PCDeviceSetSamplerState(stage, 5, 2);
        PCDeviceSetSamplerState(stage, 6, 2);
        PCDeviceSetSamplerState(stage, 7, 2);
    }
    PCDeviceSetTexture(0, NULL);
    PCDeviceSetTexture(1, NULL);
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetTextureStageState(1, 1, 1);
    PCDeviceSetTextureStageState(1, 4, 1);
    PCThreadCriticalSectionLeave_candidate(PCFrameCriticalSectionIndex_candidate);
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

void NuErrorSetFilter(char *filter) {
    if (filter == NULL) {
        NuStrCpy(err_msg_filter, "");
    } else {
        NuStrNCpy(err_msg_filter, filter, 32);
    }
}
static void __cdecl NuDebugMsgFunction(char *format, ...) {
    char captxt[1024];
    va_list args;
    if (!DisableDebugMsg) {
        if (err_msg_filter[0] == 0 || strstr(nufile, err_msg_filter) != NULL) {
            sprintf(captxt, "NuDebugMsg - %s(%d) :", nufile, nuline);
            va_start(args, format);
            vsprintf(PCWarningFormatBuffer, format, args);
            va_end(args);
            NuStrCat(captxt, PCWarningFormatBuffer);
            NuStrCat(captxt, PCWarningSuffix);
            if (PCWarningHandler != NULL)
                PCWarningHandler(captxt);
            PCNativeDebugPrint(&PCNativeWarningDebugState, PCWarningNativeFormat, captxt);
        }
    }
}

typedef void(__cdecl *NUDEBUGMSGFUNCTION_PC)(char *, ...);
NUDEBUGMSGFUNCTION_PC __cdecl NuDebugMsgProlog(char *file, i32 line) {
    nufile = file;
    nuline = line;
    return NuDebugMsgFunction;
}

GAMECAMERA GameCamera = {0};
GAMECAMERA *GameCam = &GameCamera;
i32 part_debris_count = 0;

STATIC_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);
STATIC_ASSERT(offsetof(GAMEPAD_PC, bound_pad) == 0);

void __cdecl NewRumbleAllPlayers(f32 strength, f32 duration, i32 frames, f32 priority) {
    i32 index;
    GAMEOBJECT *object;
    (void)priority;

    if (frames > 0) {
        volatile f32 rounded_duration = (f64)frames / DEFAULTFPS;
        if (rounded_duration > duration) {
            duration = rounded_duration;
        }
    }
    for (index = 0; index < 8; index++) {
        object = (GAMEOBJECT *)Player[index];
        if (object != NULL && (object->flags_1fc & 0x80) != 0 && object->pad->bound_pad != NULL) {
            NuSound3AddRumble(object->pad->bound_pad, duration, (i32)((f64)strength * 255.0), 0, strength);
        }
    }
}

void DieRumble(GAMEOBJECT *object) {
    if (object != NULL && object->flags_1fc) {
        NewRumble(object->pad->bound_pad, 1.0f, 0);
        NewBuzz(object->pad->bound_pad, 0.3f, 0);
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
