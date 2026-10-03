#include "saga.h"

void __cdecl NuMemSet128(void *destination, u32 value, i32 blocks) {
    memset(destination, (u8)value, (u32)blocks << 4);
}
