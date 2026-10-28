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

i32 __cdecl NuPs2ApplyDeadZone(i32 raw_value, i32 dead_zone) {
    i32 value = (i32)((u32)raw_value - 128U);
    if (value > dead_zone)
        return (i32)(((u32)value - (u32)dead_zone) * 127U) / (i32)(127U - (u32)dead_zone);
    if (value < (i32)(0U - (u32)dead_zone))
        return (i32)(((u32)value + (u32)dead_zone) * 127U) / (i32)(127U - (u32)dead_zone);
    return 0;
}
