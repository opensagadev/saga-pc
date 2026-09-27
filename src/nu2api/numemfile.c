#include "saga.h"

#include <string.h>

NUFILE __cdecl NuMemFileOpen(void *buffer, i32 buffer_size, i32 mode) {
    i32 index;

    if (buffer_size > 0 && (mode == 0 || mode == 1)) {
        for (index = 0; index < 20; index++) {
            if (pc_memfiles[index].used == 0) {
                pc_memfiles[index].buffer = (u8 *)buffer;
                pc_memfiles[index].end = (u8 *)buffer + buffer_size - 1;
                pc_memfiles[index].position = pc_memfiles[index].buffer;
                pc_memfiles[index].mode = mode;
                pc_memfiles[index].used = 1;
                return index + 0x400;
            }
        }
    }

    return 0;
}

void *__cdecl NuMemFileAddr(NUFILE file) {
    file -= 0x400;
    return pc_memfiles[file].position;
}

i32 __cdecl NuMemFileWrite(NUFILE file, const void *data, i32 size) {
    i32 remaining;

    file -= 0x400;
    remaining = pc_memfiles[file].end - pc_memfiles[file].position + 1;
    if (size > remaining) {
        size = remaining;
    }
    if (size != 0) {
        memcpy(pc_memfiles[file].position, data, size);
        pc_memfiles[file].position += size;
    }
    return size;
}
