#include "saga.h"

i32 __cdecl StartupParseArgument(i32 count, char **arguments) {
    (void)count;
    (void)arguments;
    return 0;
}

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
