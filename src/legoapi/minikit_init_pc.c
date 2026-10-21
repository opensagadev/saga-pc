#include "saga.h"

STATIC_ASSERT(sizeof(VARIPTR) == 4);
STATIC_ASSERT(sizeof(*Char_MiniKit) == 4);

void __cdecl MiniKits_Init(VARIPTR *buffer, VARIPTR *end) {
    (void)end;
    buffer->addr = (buffer->addr + 3) & ~3u;
    Char_MiniKit = (i32 *)buffer->void_ptr;
    buffer->addr += (u32)CHARCOUNT * 4u;
}
