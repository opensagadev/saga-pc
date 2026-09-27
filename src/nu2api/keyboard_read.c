#include "saga.h"

void __cdecl NuKeyboardRead(void) {
    /*
     * PC extent: 0x006d9340..0x006d9369 (42 bytes). It checks the keyboard
     * state at 0x02975164+0x4e10, sets that byte once, calls 0x006d5990 with
     * 0x029765a0, and stores the result at state+0x68b0. Android's body is
     * empty; the PC state layout and Windows helper remain unresolved.
     */
    STUBBED_NAMED("PC Windows keyboard initialization");
}

i32 NuKeyToAscii(u32 key, i32 shifted) {
    NUKEYCHAR *entry;

    for (entry = NuKeyChar; entry->key != 0; ++entry) {
        if (entry->key == key) {
            return shifted ? entry->shifted : entry->normal;
        }
    }
    return 0;
}
