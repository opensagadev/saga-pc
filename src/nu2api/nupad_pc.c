#include "saga.h"
void NuPadInitPS(FRAME_SCANNED_PAD_PC *pad) {
    (void)pad;
}
i32 NuPad_Interface_GetMaxDevices(void) {
    return 14;
}

void NuPadOpenPS(FRAME_PAD_PC *pad) {
    pad->flags |= 4;
}
