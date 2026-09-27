#include "saga.h"

i32 __cdecl NuPSFileOpen(const char *path, i32 mode) {
    DWORD access = 0;
    DWORD creation = 0;
    i32 index;

    switch (mode) {
        case 0:
            access = GENERIC_READ;
            creation = OPEN_EXISTING;
            break;
        case 1:
            access = GENERIC_WRITE;
            creation = CREATE_ALWAYS;
            break;
        case 2:
            access = GENERIC_WRITE;
            creation = OPEN_ALWAYS;
            break;
    }

    /* Original bugs: scan 64 entries in a 32-entry table, then use -1 if none is free. */
    for (index = 0; index < 64; index++) {
        if (pc_file_handles[index] == INVALID_HANDLE_VALUE)
            break;
    }
    if (index == 64)
        index = -1;

    pc_file_handles[index] = CreateFileA(path, access, FILE_SHARE_READ, NULL, creation, 0, NULL);
    if (pc_file_handles[index] == INVALID_HANDLE_VALUE)
        return -1;
    return index;
}

i32 __cdecl NuPSFileClose(i32 index) {
    i32 result = !CloseHandle(pc_file_handles[index]);
    pc_file_handles[index] = INVALID_HANDLE_VALUE;
    return result;
}

i32 __cdecl NuPSFileRead(i32 index, void *destination, i32 size) {
    DWORD count = 0;
    ReadFile(pc_file_handles[index], destination, size, &count, NULL);
    return count;
}

i32 __cdecl NuPSFileWrite(i32 index, const void *source, i32 size) {
    DWORD count = 0;
    if (WriteFile(pc_file_handles[index], source, size, &count, NULL))
        return count;
    return 0;
}

i64 __cdecl NuPSFileLSeek(i32 index, i64 offset, i32 origin) {
    DWORD method;
    LARGE_INTEGER distance;
    LARGE_INTEGER position;

    switch (origin) {
        case 0:
            method = FILE_BEGIN;
            break;
        case 1:
            method = FILE_CURRENT;
            break;
        default:
            method = FILE_END;
            break;
    }

    distance.QuadPart = offset;
    position.QuadPart = 0;
    if (SetFilePointerEx(pc_file_handles[index], distance, &position, method))
        return position.QuadPart;
    return -1;
}
