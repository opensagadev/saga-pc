#include "legoapi/characters/motion.h"
#include "legoapi/gizmo/base/gizmessage.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/levels/levels.h"
#include "nu2api/PCInput.h"
#include "nu2api/nucore/nuerror.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore_pc_includes.h"
#include "nu2api/nuerror_gen.h"
#include <stdarg.h>
#include <stdio.h>

const char GizAIMessage_NameFormat[3] = "%s";

void bgProcDataOversize(i32 size) {
}

/* Empty, next to the other empty error hooks; NuFileReadDir in nufile_pc.c keeps its call. */
void FileDirectoryErrorPc(i32 error) {
    (void)error;
}

void NuErrorPrint(char *text) {
    PCNativeErrorPrint(&PCNativeWarningDebugState, PCWarningNativeFormat, text);
}

void NuWarningPrint(char *text) {
    PCNativeWarningPrint(&PCNativeWarningDebugState, PCWarningNativeFormat, text);
}

void NuDebugMsgPrint(char *text) {
    PCNativeDebugPrint(&PCNativeWarningDebugState, PCWarningNativeFormat, text);
}

typedef void (*NUONLINEACHIEVEMENTCALLBACK)(i32, i32);

/* The PC build has no online service: achievements report success and no
 * player is ever signed in. */
i32 NuOnlineAchievementAchievedPS(i32 achievement, NUONLINEACHIEVEMENTCALLBACK callback) {
    (void)achievement;
    (void)callback;
    return 1;
}

i32 NuOnlineAchievementAchievedExPS(i32 player, i32 achievement, NUONLINEACHIEVEMENTCALLBACK callback) {
    (void)player;
    (void)achievement;
    (void)callback;
    return 1;
}

void NuOnlineInitPS(void) {
}

i32 NuOnlineHasPlayerSignedInExPS(void) {
    return 0;
}

i32 NuOnlineHasPlayerSignedInPS(void) {
    return 0;
}

i32 NuOnlineHasPlayerDownloadedPS(u32 argument) {
    (void)argument;
    return 0;
}

void NuSetErrorMsgHandler(void (*handler)(char *)) {
    nuerror_handler = handler;
}

void NuSetWarningMsgHandler(void (*handler)(char *)) {
    nuwarning_handler = handler;
}

void NuSetDebugMsgHandler(void (*handler)(char *)) {
    PCWarningHandler = handler;
}

static void NuErrorFunction(char *format, ...) {
    va_list args;
    sprintf(PCWarningOutputBuffer, "NuError - %s(%d) : ", nufile, nuline);
    va_start(args, format);
    vsprintf(PCWarningFormatBuffer, format, args);
    va_end(args);
    NuStrCat(PCWarningOutputBuffer, PCWarningFormatBuffer);
    NuStrCat(PCWarningOutputBuffer, "\n");
    if (nuerror_handler != NULL) {
        nuerror_handler(PCWarningOutputBuffer);
    }
    NuErrorPrint(PCWarningOutputBuffer);
}

typedef void (*NUERRORFUNCTION)(char *, ...);

NUERRORFUNCTION NuErrorProlog(char *file, i32 line) {
    nufile = file;
    nuline = line;
    return NuErrorFunction;
}

static void NuWarningFunction(char *format, ...) {
    va_list args;
    if (!DisableDebugMsg) {
        sprintf(PCWarningOutputBuffer, "NuWarning - %s(%d) : ", nufile, nuline);
        va_start(args, format);
        vsprintf(PCWarningFormatBuffer, format, args);
        va_end(args);
        NuStrCat(PCWarningOutputBuffer, PCWarningFormatBuffer);
        NuStrCat(PCWarningOutputBuffer, "\n");
        if (nuwarning_handler != NULL) {
            nuwarning_handler(PCWarningOutputBuffer);
        }
        NuWarningPrint(PCWarningOutputBuffer);
    }
}

NUERRORFUNCTION NuWarningProlog(char *file, i32 line) {
    nufile = file;
    nuline = line;
    return NuWarningFunction;
}

void NuErrorSetFilter(char *filter) {
    if (filter == NULL) {
        NuStrCpy(err_msg_filter, "");
    } else {
        NuStrNCpy(err_msg_filter, filter, sizeof(err_msg_filter));
    }
}

static void NuDebugMsgFunction(char *format, ...) {
    char captxt[1024];
    va_list args;
    if (!DisableDebugMsg) {
        if (err_msg_filter[0] == 0 || strstr(nufile, err_msg_filter) != NULL) {
            sprintf(captxt, "NuDebugMsg - %s(%d) :", nufile, nuline);
            va_start(args, format);
            vsprintf(PCWarningFormatBuffer, format, args);
            va_end(args);
            NuStrCat(captxt, PCWarningFormatBuffer);
            NuStrCat(captxt, "\n");
            if (PCWarningHandler != NULL) {
                PCWarningHandler(captxt);
            }
            PCNativeDebugPrint(&PCNativeWarningDebugState, PCWarningNativeFormat, captxt);
        }
    }
}

static void NuDebugMsgFunctionTTY(i32 channel, char *format, ...) {
}

typedef void (*NUDEBUGMSGFUNCTION)(char *, ...);

NUDEBUGMSGFUNCTION NuDebugMsgProlog(char *file, i32 line) {
    nufile = file;
    nuline = line;
    return NuDebugMsgFunction;
}

typedef void (*NUDEBUGTTYFUNCTION)(i32 channel, char *format, ...);

NUDEBUGTTYFUNCTION NuDebugMsgPrologTTY(char *file, i32 line) {
    nufile = file;
    nuline = line;
    return NuDebugMsgFunctionTTY;
}

void (*PCWarningHandler)(char *) = 0;
GAMECAMERA GameCamera = {0};
GAMECAMERA *GameCam = &GameCamera;
i32 part_debris_count = 0;

void GameCam_NewShake(GAMECAMERA *camera, f32 intensity, f32 decay, f32 freq) {
    if (camera == NULL) {
        camera = GameCam;
    }
    camera->shake_intensity = intensity;
    camera->shake_decay = decay;
    camera->shake_freq = freq;
}
