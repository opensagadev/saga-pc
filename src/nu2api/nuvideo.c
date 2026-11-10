#include "saga.h"
#include <stdarg.h>
#include <stdio.h>

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

void __cdecl bgProcDataOversize(i32 size) {
}

/* Empty in retail, next to the other empty error hooks; NuFileReadDir in nufile_pc.c keeps its call. */
void __cdecl FileDirectoryErrorPc(i32 error) {
    (void)error;
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

void __cdecl NewRumbleAllPlayers(f32 strength, f32 duration, i32 frames, i32 player_index) {
    i32 index;
    GAMEOBJECT *object;
    (void)player_index;

    if (frames > 0) {
        f32 rounded_duration = (f64)frames / DEFAULTFPS;
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
    if (object != NULL && (object->flags_1fc & 0x80) != 0) {
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
