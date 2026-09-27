#include "saga.h"

i32 NuKeyToAscii(u32 key, i32 shifted) {
    NUKEYCHAR *entry;

    for (entry = NuKeyChar; entry->key != 0; ++entry) {
        if (entry->key == key) {
            return shifted ? entry->shifted : entry->normal;
        }
    }
    return 0;
}
