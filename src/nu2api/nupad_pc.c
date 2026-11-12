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

void __cdecl NuPadSetMotorsPc(NUPAD_INIT_STATE *pad, i32 motor0, i32 motor1) {
    (void)pad;
    (void)motor0;
    (void)motor1;
}

i32 __cdecl NuPs2ApplyDeadZone(i32 v, i32 deadrng) {
    i32 dx;

    v -= 128;
    if (v > deadrng) {
        dx = (v - deadrng) * 127 / (127 - deadrng);
    } else if (v < -deadrng) {
        dx = (v + deadrng) * 127 / (127 - deadrng);
    } else {
        dx = 0;
    }
    return dx;
}
