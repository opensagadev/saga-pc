#include "saga.h"

void __cdecl NuCommandLine(i32 *__restrict count, char ***__restrict arguments) {
    // NuMain passes separate stack arguments, not these global cursors.
    argc = *count;
    argv = *arguments;
}

void NuVideoSetBrightnessPS(void) {
}

void NuVideoSetSwapModePS(i32 mode) {
    (void)mode;
}

void MenuSetPlatformState(i32 state) {
    (void)state;
}
