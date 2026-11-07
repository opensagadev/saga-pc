#include "saga.h"

/* The memory-card device stubs form their own unit (0x6dee30-0x6def60); the nucore file
   functions call them out of line. */

i32 __cdecl NuMcOpen(i32 port, i32 slot, char *filepath, i32 mode, i32 async) {
    (void)port;
    (void)slot;
    (void)filepath;
    (void)mode;
    (void)async;
    return 0;
}

i32 __cdecl NuMcFileOpenSize(NUFILE file) {
    (void)file;
    return 0;
}

i32 __cdecl NuMcRead(i32 file, void *destination, i32 size, i32 async) {
    (void)file;
    (void)destination;
    (void)size;
    (void)async;
    return 0;
}

i32 __cdecl NuMcWrite(i32 file, const void *data, i32 size, i32 flags) {
    (void)file;
    (void)data;
    (void)size;
    (void)flags;
    return 0;
}

i32 __cdecl NuMcClose(i32 file, i32 async) {
    (void)file;
    (void)async;
    return 0;
}

i32 __cdecl NuMcSeek(i32 file, i32 offset, i32 origin, i32 async) {
    (void)file;
    (void)offset;
    (void)origin;
    (void)async;
    return 0;
}

i32 __cdecl FileDirectoryMcOpenPc(i32 port, i32 slot, char *path) {
    (void)port;
    (void)slot;
    (void)path;
    return 0;
}

void __cdecl FileDirectoryMcClosePc(i32 file) {
    (void)file;
}

i32 __cdecl FileDirectoryMcReadPc(i32 file, struct FILE_DIRECTORY_ENTRY_PC *entry) {
    (void)file;
    (void)entry;
    return 0;
}
