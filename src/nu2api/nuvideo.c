#include "saga.h"
#include <stdarg.h>
#include <stdio.h>

void __cdecl bgProcDataOversize(i32 size) {
}

/* Empty in retail, next to the other empty error hooks; NuFileReadDir in nufile_pc.c keeps its call. */
void __cdecl FileDirectoryErrorPc(i32 error) {
    (void)error;
}

void __cdecl PCNativeErrorPrint(PC_NATIVE_DEBUG_STATE_PC *state, const char *format, ...);
void __cdecl PCNativeWarningPrint(PC_NATIVE_DEBUG_STATE_PC *state, const char *format, ...);

void __cdecl NuErrorPrint(char *text) {
    PCNativeErrorPrint(&PCNativeWarningDebugState, PCWarningNativeFormat, text);
}

void __cdecl NuWarningPrint(char *text) {
    PCNativeWarningPrint(&PCNativeWarningDebugState, PCWarningNativeFormat, text);
}

void __cdecl NuDebugMsgPrint(char *text) {
    PCNativeDebugPrint(&PCNativeWarningDebugState, PCWarningNativeFormat, text);
}

typedef void(__cdecl *NUONLINEACHIEVEMENTCALLBACK_PC)(i32, i32);

/* The PC build has no online service: achievements report success and no
 * player is ever signed in. */
i32 __cdecl NuOnlineAchievementAchievedPS(i32 achievement, NUONLINEACHIEVEMENTCALLBACK_PC callback) {
    (void)achievement;
    (void)callback;
    return 1;
}

i32 __cdecl NuOnlineAchievementAchievedExPS(i32 player, i32 achievement, NUONLINEACHIEVEMENTCALLBACK_PC callback) {
    (void)player;
    (void)achievement;
    (void)callback;
    return 1;
}

void __cdecl NuOnlineInitPS(void) {
}

i32 __cdecl NuOnlineHasPlayerSignedInExPS(void) {
    return 0;
}

i32 __cdecl NuOnlineHasPlayerSignedInPS(void) {
    return 0;
}

i32 __cdecl NuOnlineHasPlayerDownloadedPS(u32 argument) {
    (void)argument;
    return 0;
}

void __cdecl NuSetErrorMsgHandler(void(__cdecl *handler)(char *)) {
    nuerror_handler = handler;
}

void __cdecl NuSetWarningMsgHandler(void(__cdecl *handler)(char *)) {
    nuwarning_handler = handler;
}

void __cdecl NuSetDebugMsgHandler(void(__cdecl *handler)(char *)) {
    PCWarningHandler = handler;
}

static void __cdecl NuErrorFunction(char *format, ...) {
    va_list args;
    sprintf(PCWarningOutputBuffer, "NuError - %s(%d) : ", nufile, nuline);
    va_start(args, format);
    vsprintf(PCWarningFormatBuffer, format, args);
    va_end(args);
    NuStrCat(PCWarningOutputBuffer, PCWarningFormatBuffer);
    NuStrCat(PCWarningOutputBuffer, PCWarningSuffix);
    if (nuerror_handler != NULL) {
        nuerror_handler(PCWarningOutputBuffer);
    }
    NuErrorPrint(PCWarningOutputBuffer);
}

typedef void(__cdecl *NUERRORFUNCTION_PC)(char *, ...);

NUERRORFUNCTION_PC __cdecl NuErrorProlog(char *file, i32 line) {
    nufile = file;
    nuline = line;
    return NuErrorFunction;
}

static void __cdecl NuWarningFunction(char *format, ...) {
    va_list args;
    if (!DisableDebugMsg) {
        sprintf(PCWarningOutputBuffer, "NuWarning - %s(%d) : ", nufile, nuline);
        va_start(args, format);
        vsprintf(PCWarningFormatBuffer, format, args);
        va_end(args);
        NuStrCat(PCWarningOutputBuffer, PCWarningFormatBuffer);
        NuStrCat(PCWarningOutputBuffer, PCWarningSuffix);
        if (nuwarning_handler != NULL) {
            nuwarning_handler(PCWarningOutputBuffer);
        }
        NuWarningPrint(PCWarningOutputBuffer);
    }
}

NUERRORFUNCTION_PC __cdecl NuWarningProlog(char *file, i32 line) {
    nufile = file;
    nuline = line;
    return NuWarningFunction;
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
            if (PCWarningHandler != NULL) {
                PCWarningHandler(captxt);
            }
            PCNativeDebugPrint(&PCNativeWarningDebugState, PCWarningNativeFormat, captxt);
        }
    }
}

static void NuDebugMsgFunctionTTY(i32 channel, char *format, ...) {
}

typedef void(__cdecl *NUDEBUGMSGFUNCTION_PC)(char *, ...);

NUDEBUGMSGFUNCTION_PC __cdecl NuDebugMsgProlog(char *file, i32 line) {
    nufile = file;
    nuline = line;
    return NuDebugMsgFunction;
}

typedef void (*NUDEBUGTTYFUNCTION_PC)(i32 channel, char *format, ...);

NUDEBUGTTYFUNCTION_PC NuDebugMsgPrologTTY(char *file, i32 line) {
    nufile = file;
    nuline = line;
    return NuDebugMsgFunctionTTY;
}

GAMECAMERA GameCamera = {0};
GAMECAMERA *GameCam = &GameCamera;
i32 part_debris_count = 0;

STATIC_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);
STATIC_ASSERT(offsetof(GAMEPAD_PC, bound_pad) == 0);

void __cdecl GameCam_NewShake(GAMECAMERA *camera, f32 intensity, f32 decay, f32 freq) {
    if (camera == NULL) {
        camera = GameCam;
    }
    camera->shake_intensity = intensity;
    camera->shake_decay = decay;
    camera->shake_freq = freq;
}
