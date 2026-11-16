#include "legogame/pcbatman.h"
#include "nu2api/PCInput.h"
#include "nu2api/nucore/nukeyboard.h"
#include "nu2api/nucore/nuvideo.h"
#include <string.h>

i32 StartupParseArgument(i32 count, char **arguments) {
    return 0;
}

void NuVideoSetBrightnessPS(void) {
}

void NuVideoSetSwapModePS(i32 mode) {
}

void MenuSetPlatformState(i32 state) {
}

/* Empty; its presentation caller consumes no return value. */
void PCPresentationPlatformLeaf(void) {
}

void NuMainTrailerLeaf(void) {
}

i32 NuKeyboard(i32 key) {
    return (i8)nu_keyboard_pages[nu_keyboard_page_index * 0x100 + key];
}
