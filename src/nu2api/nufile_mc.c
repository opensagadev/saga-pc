#include "saga.h"

/* The memory-card device stubs form their own unit (0x6dee30-0x6def60); the nucore file
   functions call them out of line. */

/* Native 0x6dede0-0x6dee22: the card queries sit just before that range and are
   equally empty on PC. */
i32 NuMcCheckCardPresent(i32 port, i32 slot) {
    (void)port;
    (void)slot;
    return 0;
}

i32 NuMcCheckCardChangedPc(i32 port, i32 slot) {
    (void)port;
    (void)slot;
    return 0;
}

i32 NuMcCheckCardFormatted(i32 port, i32 slot) {
    (void)port;
    (void)slot;
    return 0;
}

i32 NuMcCheckCardFreeSpace(i32 port, i32 slot) {
    (void)port;
    (void)slot;
    return 0;
}

i32 NuMcFormat(i32 port, i32 slot) {
    (void)port;
    (void)slot;
    return 0;
}

i32 NuMcOpen(i32 port, i32 slot, char *filepath, i32 mode, i32 async) {
    (void)port;
    (void)slot;
    (void)filepath;
    (void)mode;
    (void)async;
    return 0;
}

i32 NuMcFileOpenSize(NUFILE file) {
    (void)file;
    return 0;
}

i32 NuMcRead(i32 file, void *destination, i32 size, i32 async) {
    (void)file;
    (void)destination;
    (void)size;
    (void)async;
    return 0;
}

i32 NuMcWrite(i32 file, const void *data, i32 size, i32 flags) {
    (void)file;
    (void)data;
    (void)size;
    (void)flags;
    return 0;
}

i32 NuMcClose(i32 file, i32 async) {
    (void)file;
    (void)async;
    return 0;
}

i32 fun_006dee80(void) {
    return 0;
}

i32 NuMcSeek(i32 file, i32 offset, i32 origin, i32 async) {
    (void)file;
    (void)offset;
    (void)origin;
    (void)async;
    return 0;
}

i32 fun_006deea0(void) {
    return 0;
}

i32 fun_006deeb0(void) {
    return 0;
}

i32 fun_006deec0(void) {
    return 0;
}

i32 fun_006deed0(void) {
    return 0;
}

i32 fun_006deee0(void) {
    return 0;
}

f32 fun_006deef0(void) {
    return 0.0f;
}

i32 fun_006def00(void) {
    return 0;
}

i32 fun_006def10(void) {
    return 0;
}

i16 fun_006def20(void) {
    return 0;
}

u8 fun_006def30(void) {
    return 0;
}

i32 FileDirectoryMcOpenPc(i32 port, i32 slot, char *path) {
    (void)port;
    (void)slot;
    (void)path;
    return 0;
}

void FileDirectoryMcClosePc(i32 file) {
    (void)file;
}

i32 FileDirectoryMcReadPc(i32 file, struct FILE_DIRECTORY_ENTRY_PC *entry) {
    (void)file;
    (void)entry;
    return 0;
}

i32 fun_006def70(void) {
    return 0;
}

i32 fun_006def80(void) {
    return 0;
}
