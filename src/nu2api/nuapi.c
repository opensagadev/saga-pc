#include "saga.h"
#include <string.h>

i32 __cdecl StartupParseArgument(i32 count, char **arguments) {
    (void)count;
    (void)arguments;
    return 0;
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

void __cdecl NuMainTrailerLeafPc(void) {
}

i32 __cdecl NuKeyboard(i32 key) {
    return (i8)nu_keyboard_pages[(i8)nu_keyboard_page_index * 0x100 + key];
}
