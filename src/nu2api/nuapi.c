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
