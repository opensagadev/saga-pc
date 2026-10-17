#include "diagnostics.h"
#include "pc_fmv_native.h"
#include <stdarg.h>
#include <string.h>
typedef i32(__cdecl *FMV_INPUT_CALLBACK_PC)(void);
typedef void(__cdecl *FMV_DRAW_CALLBACK_PC)(void);

extern "C" i32 __cdecl NuFmvPlayV(i32 option, ...) {
    i32 *args = &option;
    i32 count = 0, stop_sound = 1, result = 0, mode = 0;
    f32 volume = 1.0f;
    char *name = NULL;
    i32(__cdecl * callback)(void) = NULL;
    char path[256];
    while (option != 1) {
        ++count;
        switch (option) {
            case 0:
            case 3:
            case 9:
                break;
            case 2:
                name = (char *)*++args;
                break;
            case 4:
                mode = *++args == 3;
                break;
            case 5:
            case 6:
            case 10:
            case 11:
            case 12:
                (void)*++args;
                break;
            case 7:
                callback = (FMV_INPUT_CALLBACK_PC) * ++args;
                break;
            case 8:
                (void)*++args;
                (void)*++args;
                break;
            case 13:
                stop_sound = 0;
                break;
            case 14:
                volume = *(f32 *)*++args;
                break;
            case 15:
                FmvDrawCallbackPc = (FMV_DRAW_CALLBACK_PC) * ++args;
                break;
            default: {
                const char *suffixes[10] = {"th", "st", "nd", "rd", "th", "th", "th", "th", "th", "th"};
                FmvOptionErrorPc(count, suffixes[count % 10], option);
                break;
            }
        }
        option = *++args;
    }
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
