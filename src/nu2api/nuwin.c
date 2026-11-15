#include "nu2api/huffdecode.h"
#include "nu2api/nuwin32/nu2api_windows.h"

i32 NuWindowsProcessMessages(void) {
    while (PeekMessageA(&win_message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&win_message);
        DispatchMessageA(&win_message);
        if (win_message.message == WM_QUIT) {
            return 0;
        }
    }
    return 1;
}

static i32 NuWindowsConfigValues[19] = {0};

i32 *fun_006dd360(void) {
    return NuWindowsConfigValues;
}

void fun_006dd370(i32 index, i32 value) {
    NuWindowsConfigValues[index] = value;
}

i32 NuWindowsGetConfigValue(i32 index) {
    return NuWindowsConfigValues[index];
}
