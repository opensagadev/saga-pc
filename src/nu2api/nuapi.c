#include "saga.h"
#include <string.h>

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

/* Original source: nu2api/nucore/nuapi.cpp; PC initializes only the fields
 * below after clearing the real 0x90-byte API record. */
static i32 NUAPI_PADREC_DEFAULT_BUFFERSIZE = 0x500000;

void __cdecl NuAPIInit(void) {
    memset(&nuapi, 0, sizeof(NUAPI));
    nuapi.fps = 60.0f;
    nuapi.video_mode = 2;
    nuapi.video_swap_mode = 5;
    nuapi.language = 1;
    nuapi.video_aspect = 0;
    nuapi.frame_end_state = 0;
    nuapi.pad_record.buf_size = NUAPI_PADREC_DEFAULT_BUFFERSIZE;
    nuapi.pad_record.end_record_buttons = 0;
    nuapi.pad_record.end_play_buttons = 0;
    NuWindInitialise((NUWIND *)nuapi.wind);
}
