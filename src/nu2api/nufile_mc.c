#include "gameapi/apisave.h"
#include "nu2api/nufile/nufile.h"

/* The memory-card device stubs form their own unit; the nucore file
   functions call them out of line. */

i32 NuMcOpen(i32 port, i32 slot, char *filepath, i32 mode, i32 async) {
    return 0;
}

i32 NuMcFileOpenSize(NUFILE file) {
    return 0;
}

i32 NuMcRead(i32 file, void *destination, i32 size, i32 async) {
    return 0;
}

i32 NuMcWrite(i32 file, const void *data, i32 size, i32 flags) {
    return 0;
}

i32 NuMcClose(i32 file, i32 async) {
    return 0;
}

i32 fun_006dee80(void) {
    return 0;
}

i32 NuMcSeek(i32 file, i32 offset, i32 origin, i32 async) {
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

i32 FileDirectoryMcOpen(i32 port, i32 slot, char *path) {
    return 0;
}

void FileDirectoryMcClose(i32 file) {
}

i32 FileDirectoryMcRead(i32 file, struct FILE_DIRECTORY_ENTRY *entry) {
    return 0;
}

i32 fun_006def70(void) {
    return 0;
}

i32 fun_006def80(void) {
    return 0;
}
