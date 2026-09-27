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
