#include "saga.h"

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

static i32 NuWindowsConfigValuesPc[19] = {0};

i32 *fun_006dd360(void) {
    return NuWindowsConfigValuesPc;
}

void fun_006dd370(i32 index, i32 value) {
    NuWindowsConfigValuesPc[index] = value;
}

i32 NuWindowsGetConfigValuePc(i32 index) {
    return NuWindowsConfigValuesPc[index];
}
