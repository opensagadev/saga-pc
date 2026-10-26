#include "saga.h"

i32 __cdecl NuFileUseHiresPC(void) {
    return PcWindowOptions.usehires;
}

void __cdecl NuPSFileInitDevices(i32 device_id, i32 reboot_iop, i32 eject) {
    (void)device_id;
    (void)reboot_iop;
    (void)eject;
}

#include <windows.h>
typedef struct FILE_DIRECTORY_TIME_PC {
    u8 second, minute, hour, day, month, reserved;
    u16 year;
} FILE_DIRECTORY_TIME_PC;
typedef struct FILE_DIRECTORY_ENTRY_PC {
    u32 flags, size;
    FILE_DIRECTORY_TIME_PC created, modified;
    char name[256];
} FILE_DIRECTORY_ENTRY_PC;
STATIC_ASSERT(offsetof(FILE_DIRECTORY_ENTRY_PC, name) == 0x18);
typedef struct FILE_DIRECTORY_RAW_TIME_PC {
    u8 second, minute, hour, day, month, year_low, year_high;
} FILE_DIRECTORY_RAW_TIME_PC;
typedef struct FILE_DIRECTORY_HOST_ENTRY_PC {
    u32 flags;
    u8 reserved_04[4];
    u32 size;
    u8 reserved_0c;
    FILE_DIRECTORY_RAW_TIME_PC created;
    u8 reserved_14;
    FILE_DIRECTORY_RAW_TIME_PC accessed;
    u8 reserved_1c;
    FILE_DIRECTORY_RAW_TIME_PC modified;
    u32 unused_24;
    u8 reserved_28[0x18];
    char name[MAX_PATH];
} FILE_DIRECTORY_HOST_ENTRY_PC;
STATIC_ASSERT(offsetof(FILE_DIRECTORY_HOST_ENTRY_PC, created) == 0xd);
STATIC_ASSERT(offsetof(FILE_DIRECTORY_HOST_ENTRY_PC, modified) == 0x1d);
STATIC_ASSERT(offsetof(FILE_DIRECTORY_HOST_ENTRY_PC, name) == 0x40);

extern i32 __cdecl FileDirectoryHostOpenPc(char *path);
extern const char FileDirectorySourceNamePc[56];
char *FileDirectoryPathSlotsPc[16] = {0};
HANDLE FileDirectoryHostHandlesPc[16] = {0};
WIN32_FIND_DATAA FileDirectoryFindDataPc[16] = {0};
i32 FileDirectoryOpenCountPc = 0;
NUFILE_DEVICE *FileDirectoryDefaultDevicePc = &host_device;
const char FileDirectorySourceNamePc[56] = "d:\\projects\\game_saga\\nu2api.saga\\nucore\\pc\\nufile_pc.c";
const char FileDirectoryWildcardPc[4] = "*.*";
void __cdecl FileDirectoryErrorPc(i32 error) {
    (void)error;
}
extern WIN32_FIND_DATAA FileDirectoryFindDataPc[16];
extern HANDLE FileDirectoryHostHandlesPc[16];
i32 __cdecl FileDirectoryHostReadPc(i32 file, FILE_DIRECTORY_HOST_ENTRY_PC *entry) {
    WIN32_FIND_DATAA *data = &FileDirectoryFindDataPc[file];
    SYSTEMTIME time;
    u32 flags = (data->dwFileAttributes & 1) ? 0x180 : 0x100;
    if ((data->dwFileAttributes & 0x10) != 0)
        flags += 0x2000;
    entry->flags = flags;
    entry->size = data->nFileSizeLow;
    entry->unused_24 = 0;
    NuStrCpy(entry->name, data->cFileName);
    FileTimeToSystemTime(&data->ftCreationTime, &time);
    entry->created.second = (u8)time.wSecond;
    entry->created.minute = (u8)time.wMinute;
    entry->created.day = (u8)time.wDay;
    entry->created.hour = (u8)time.wHour;
    entry->created.month = (u8)time.wMonth;
    entry->created.year_low = (u8)time.wYear;
    entry->created.year_high = 0;
    FileTimeToSystemTime(&data->ftLastWriteTime, &time);
    entry->modified.second = (u8)time.wSecond;
    entry->modified.minute = (u8)time.wMinute;
    entry->modified.day = (u8)time.wDay;
    entry->modified.hour = (u8)time.wHour;
    entry->modified.month = (u8)time.wMonth;
    entry->modified.year_low = (u8)time.wYear;
    entry->modified.year_high = 0;
    FileTimeToSystemTime(&data->ftLastAccessTime, &time);
    entry->accessed.second = (u8)time.wSecond;
    entry->accessed.minute = (u8)time.wMinute;
    entry->accessed.hour = (u8)time.wHour;
    entry->accessed.day = (u8)time.wDay;
    entry->accessed.month = (u8)time.wMonth;
    entry->accessed.year_low = (u8)time.wYear;
    entry->accessed.year_high = 0;
    if (data->dwFileAttributes == 0xffffffff)
        return 0;
    if (FindNextFileA(FileDirectoryHostHandlesPc[file], data) != 1)
        data->dwFileAttributes = 0xffffffff;
    return NuStrLen(entry->name);
}

extern i32 FileDirectoryOpenCountPc;          /* 921268 */
extern char *FileDirectoryPathSlotsPc[16];    /* 921270 */
extern const char FileDirectoryWildcardPc[4]; /* 75ece0,*.* */
extern const char WorldPathSeparator[2];      /* 75a204 */

i32 __cdecl FileDirectoryHostOpenPc(char *path) {
    char search[200];
    i32 slot;
    if (FileDirectoryOpenCountPc == 16)
        return 0;
    for (slot = 0; slot < 16; ++slot)
        if (FileDirectoryPathSlotsPc[slot] == NULL)
            break;
    if (slot == 16)
        return 0;
    NuStrCpy(search, path);
    if (search[NuStrLen(search) - 1] != '\\')
        NuStrCat(search, WorldPathSeparator);
    NuStrCat(search, FileDirectoryWildcardPc);
    FileDirectoryPathSlotsPc[slot] = (char *)NuAlloc(NuStrLen(search) + 4, FileDirectorySourceNamePc, 0x21a);
    NuStrCpy(FileDirectoryPathSlotsPc[slot], search);
    ++FileDirectoryOpenCountPc;
    FileDirectoryHostHandlesPc[slot] = FindFirstFileA(FileDirectoryPathSlotsPc[slot], &FileDirectoryFindDataPc[slot]);
    if (FileDirectoryHostHandlesPc[slot] == INVALID_HANDLE_VALUE) {
        NuFree(FileDirectoryPathSlotsPc[slot], FileDirectorySourceNamePc, 0x222);
        --FileDirectoryOpenCountPc;
        if (slot == 0)
            PCThreadCriticalSectionCount_candidate = 0;
        else
            FileDirectoryPathSlotsPc[slot - 1] = NULL;
        return -1;
    }
    return slot;
}

i32 __cdecl FileDirectoryHostClosePc(i32 slot) {
    HANDLE handle;
    NuFree(FileDirectoryPathSlotsPc[slot], FileDirectorySourceNamePc, 0x22e);
    handle = FileDirectoryHostHandlesPc[slot];
    if (slot == 0)
        PCThreadCriticalSectionCount_candidate = 0;
    else
        FileDirectoryPathSlotsPc[slot - 1] = NULL;
    FindClose(handle);
    --FileDirectoryOpenCountPc;
    return 0;
}

i32 __cdecl FileHostDeletePathPc(const char *path) {
    return DeleteFileA(path);
}
