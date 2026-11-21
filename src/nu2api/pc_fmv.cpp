#include "debug/diagnostics.h"
#include "decomp.h"
#include "nu2api/PCInput.h"
#include "nu2api/huffdecode.h"
#include "nu2api/nucore/nupad.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "pc_fmv_native.h"
#include <stdarg.h>
#include <string.h>
typedef i32 (*FMV_INPUT_CALLBACK)(void);
typedef void (*FMV_DRAW_CALLBACK)(void);

/* Polls the movie's input callback: 1 stops the movie, 2 skips to the next one. */
static i32 FmvPollCallback(FMV_INPUT_CALLBACK callback) {
    if (callback != NULL) {
        switch (callback()) {
            case 1:
                return 1;
            case 2:
                FmvSkip();
                break;
        }
    }
    return 0;
}

extern "C" i32 NuFmvPlayV(i32 option, ...) {
    va_list args;
    i32 count = 0;
    i32 stop_sound = 1;
    i32 result = 0;
    i32 mode = 0;
    f32 volume = 1.0f;
    char *name = NULL;
    i32 (*callback)(void) = NULL;
    char path[256];
    va_start(args, option);
    while (option != NUFMV_END) {
        ++count;
        switch (option) {
            case NUFMV_NONE:
            case 3:
            case 9:
                break;
            case NUFMV_NAME:
                name = va_arg(args, char *);
                break;
            case NUFMV_MODE:
                mode = va_arg(args, i32) == 3;
                break;
            case NUFMV_SUBTITLES:
            case 6:
            case 10:
            case 11:
            case 12:
                va_arg(args, i32);
                break;
            case NUFMV_VOLUME:
                volume = *va_arg(args, f32 *);
                break;
            case NUFMV_INPUT_CALLBACK:
                callback = va_arg(args, FMV_INPUT_CALLBACK);
                break;
            case NUFMV_DRAW_CALLBACK:
                FmvDrawCallback = va_arg(args, FMV_DRAW_CALLBACK);
                break;
            case NUFMV_BUFFER:
                va_arg(args, i32);
                va_arg(args, i32);
                break;
            case NUFMV_KEEP_SOUND:
                stop_sound = 0;
                break;
            default: {
                const char *suffixes[10] = {"th", "st", "nd", "rd", "th", "th", "th", "th", "th", "th"};
                FmvOptionError(count, suffixes[count % 10], option);
                break;
            }
        }
        option = va_arg(args, i32);
    }
    va_end(args);
    if (name != NULL) {
        NuFmvStopSound(stop_sound);
        NuFileGetCurrentDirectory(path);
        NuStrCat(path, name);
        char *extension = NuStrIStr(path + 1, ".");
        if (extension != NULL) {
            *extension = '\0';
        }
        NuStrCat(path, ".bik");
        LOG_INFO("FMV requested: %s -> %s", name, path);
        if (FmvStart(path, volume, mode) != 0) {
            result = 1;
            NuPadUpdatePads();
            NuWindowsProcessMessages();
            FmvPlatformUpdate();
            while (FmvStep() != 0) {
                if (FmvPollCallback(callback)) {
                    break;
                }
                PcInputEndFrame();
                NuPadUpdatePads();
                NuWindowsProcessMessages();
                FmvPlatformUpdate();
            }
        }
        FmvStop();
        LOG_INFO("FMV finished: %s (opened=%d)", path, result);
    }
    FmvPlatformFinish();
    NuPadUpdatePads();
    return result;
}

extern "C" i32 NuFmvPlay(u32 argument0, i32 enabled, u32 argument2, u32 argument3, u32 argument4, u32 argument5, u32 argument6,
                         u32 argument7) {
    return NuFmvPlayV(NUFMV_NAME, argument0, enabled != 0 ? 3 : NUFMV_NONE, NUFMV_MODE, argument2, NUFMV_SUBTITLES, argument3, 6, argument4,
                      NUFMV_INPUT_CALLBACK, argument5, NUFMV_BUFFER, argument6, argument7, NUFMV_END);
}
