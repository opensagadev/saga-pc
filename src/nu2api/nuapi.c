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
/* Genuine native PC entry 0x006d6b40 is RET-only. Historical name/TU
 * visibility is unknown; its presentation caller consumes no return value. */
void __cdecl PCPresentationPlatformLeafPc(void) {
}
