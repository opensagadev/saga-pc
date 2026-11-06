#include "saga.h"

i32 __cdecl NuMcWrite(i32 file, const void *data, i32 size, i32 flags) {
    (void)file;
    (void)data;
    (void)size;
    (void)flags;
    return 0;
}

i32 __cdecl NuMcSeek(i32 file, i32 offset, i32 origin, i32 async) {
    (void)file;
    (void)offset;
    (void)origin;
    (void)async;
    return 0;
}
