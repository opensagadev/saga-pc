#include "saga.h"

/* Retail places StatusIconsOnOff (0x4850e0) with the shared status-screen code next to
   Status_DrawPlayerIconsPc, not with the LSW stage callbacks, so the stage callbacks call
   it instead of inlining it. */
f32 __cdecl StatusIconsOnOff(f32 blend) {
    i32 angle = (i32)((f64)blend * 16384.0);
    f32 position = (STATSPOSY - STATSPOS2YPc) * NuTrigTable[(angle >> 1) & 0x7fff] + STATSPOS2YPc;
    return position;
}
