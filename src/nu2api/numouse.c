#include "saga.h"

void __cdecl NuMouseRead(void) {
}

u32 NuMouseReadButtons(void) {
    return 0;
}

f32 __cdecl NuMouseReadX(void) {
    return PcMouseX;
}

f32 __cdecl NuMouseReadY(void) {
    return PcMouseY;
}
