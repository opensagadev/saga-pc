#include "debug/diagnostics.h"
#include "pc_fmv_native.h"
#include <stdarg.h>
#include <string.h>
typedef i32(__cdecl *FMV_INPUT_CALLBACK_PC)(void);
typedef void(__cdecl *FMV_DRAW_CALLBACK_PC)(void);

extern "C" i32 __cdecl NuFmvPlayV(i32 option, ...) {
    va_list args;
    i32 count = 0, stop_sound = 1, result = 0, mode = 0;
    f32 volume = 1.0f;
    char *name = NULL;
    i32(__cdecl * callback)(void) = NULL;
    char path[256];
    va_start(args, option);
    while (option != 1) {
        ++count;
        switch (option) {
            case 0:
            case 3:
            case 9:
                break;
            case 2:
                name = va_arg(args, char *);
                break;
            case 4:
                mode = va_arg(args, i32) == 3;
                break;
            case 5:
            case 6:
            case 10:
            case 11:
            case 12:
                va_arg(args, i32);
                break;
            case 14:
                volume = *va_arg(args, f32 *);
                break;
            case 7:
                callback = va_arg(args, FMV_INPUT_CALLBACK_PC);
                break;
            case 15:
                FmvDrawCallbackPc = va_arg(args, FMV_DRAW_CALLBACK_PC);
                break;
            case 8:
                va_arg(args, i32);
                va_arg(args, i32);
                break;
            case 13:
                stop_sound = 0;
                break;
            default: {
                const char *suffixes[10] = {"th", "st", "nd", "rd", "th", "th", "th", "th", "th", "th"};
                FmvOptionErrorPc(count, suffixes[count % 10], option);
                break;
            }
        }
        option = va_arg(args, i32);
    }
    va_end(args);
    if (name != NULL) {
        NuFmvStopSoundPc(stop_sound);
        NuFileGetCurrentDirectory(path);
        NuStrCat(path, name);
        char *extension = NuStrIStr(path + 1, ".");
        if (extension != NULL)
            *extension = '\0';
        NuStrCat(path, ".bik");
        LOG_INFO("FMV requested: %s -> %s", name, path);
        if (FmvStartPc(path, volume, mode) != 0) {
            result = 1;
            NuPadUpdatePads();
            NuWindowsProcessMessages();
            FmvPlatformUpdatePc();
            while (FmvStepPc() != 0) {
                if (callback != NULL) {
                    i32 callback_result = callback();
                    if (callback_result == 1)
                        break;
                    if (callback_result == 2)
                        FmvSkipPc();
                }
                PcInputEndFramePc();
                NuPadUpdatePads();
                NuWindowsProcessMessages();
                FmvPlatformUpdatePc();
            }
        }
        FmvStopPc();
        LOG_INFO("FMV finished: %s (opened=%d)", path, result);
    }
    FmvPlatformFinishPc();
    NuPadUpdatePads();
    return result;
}
