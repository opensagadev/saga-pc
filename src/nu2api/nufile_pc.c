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
#include <string.h>
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
void __cdecl FileDirectoryErrorPc(i32 error);

i32 __cdecl NuFileExists(char *filepath) {
    return NuFileExistQuiet(filepath);
}

extern void __cdecl NuFileReldirFix(NUFILE_DEVICE *device, char *path);

i32 __cdecl DEV_FormatName(NUFILE_DEVICE *device, char *formatted_name, char *path, i32 buf_size) {
    char buffer[512];
    i32 device_name_len;
    char *subpath;
    i32 path_len;
    i32 path_pos;

    device_name_len = NuStrLen(device->name);

    /* PC accepts an already-qualified path unchanged. */
    if (NuStrStr(path, ":") != NULL) {
        NuStrCpy(buffer, path);
    } else if (NuStrNICmp(path, device->name, device_name_len) == 0) {
        subpath = path + NuStrLen(device->name);
        if (*subpath == '\\' || *subpath == '/') {
            NuStrCpy(buffer, path);
        } else {
            NuStrCpy(buffer, device->root);
            NuStrCat(buffer, device->cur_dir);
            NuStrCat(buffer, subpath);
        }
    } else {
        NuStrCpy(buffer, device->root);
        NuStrCat(buffer, device->cur_dir);
        NuStrCat(buffer, path);
    }

    path_pos = NuStrLen(device->root);
    NuFileUpCase(device, buffer + path_pos);
    NuFileReldirFix(device, buffer);

    path_len = NuStrLen(buffer);
    if (path_len < buf_size) {
        NuStrCpy(formatted_name, buffer);
        return 1;
    }

    return 0;
}

i32 __cdecl DEVHOST_Interrogate(NUFILE_DEVICE *device) {
    device->status = 1;
    return 1;
}

extern i32 __cdecl FileDirectoryMcOpenPc(i32 port, i32 slot, char *path);
extern void __cdecl FileDirectoryMcClosePc(i32 file);
extern i32 __cdecl FileDirectoryMcReadPc(i32 file, FILE_DIRECTORY_ENTRY_PC *entry);

NUFILE __cdecl NuFileOpenDir(char *path) {
    char formatted[256];
    NUFILE_DEVICE *device = NuFileGetDeviceFromPath(path);
    i32 result;
    if (device == NULL)
        device = FileDirectoryDefaultDevicePc;
    device->format_name_fn(device, formatted, path, 256);
    if (device->id == 2)
        strcat(formatted, ";1");
    if (device->id == 1) {
        result = FileDirectoryMcOpenPc(formatted[2] - '0', formatted[3] - '0', formatted + device->name_len);
        if (result >= 0)
            return result + 0x1000;
        return 0;
    }
    result = FileDirectoryHostOpenPc(formatted);
    if (result < 0)
        return 0;
    return result + 1;
}

void __cdecl NuFileCloseDir(NUFILE file) {
    if (file >= 0x1000) {
        FileDirectoryMcClosePc(file - 0x1000);
        return;
    }
    FileDirectoryHostClosePc(file - 1);
}
i32 __cdecl NuFileReadDir(NUFILE file, FILE_DIRECTORY_ENTRY_PC *entry) {
    FILE_DIRECTORY_HOST_ENTRY_PC host;
    i32 result;
    u32 flags;
    i32 length = 0;
    if (file >= 0x1000)
        return FileDirectoryMcReadPc(file - 0x1000, entry);
    result = FileDirectoryHostReadPc(file - 1, &host);
    if (result > 0) {
        flags = (((host.flags >> 2) & 0x800) | (host.flags & 0x100));
        flags = ((flags >> 2) | (host.flags & 0x80));
        entry->flags = (((flags >> 2) | (host.flags & 0x40)) >> 4);
        entry->size = host.size;
        entry->created.second = host.created.second;
        entry->created.hour = host.created.hour;
        entry->created.minute = host.created.minute;
        entry->created.day = host.created.day;
        entry->created.month = host.created.month;
        entry->modified.second = host.modified.second;
        entry->created.year = (u16)(((u16)host.created.year_high << 8) + host.created.year_low);
        entry->modified.hour = host.modified.hour;
        entry->modified.day = host.modified.day;
        entry->modified.minute = host.modified.minute;
        entry->modified.month = host.modified.month;
        entry->modified.year = (u16)(((u16)host.modified.year_high << 8) + host.modified.year_low);
        strcpy(entry->name, host.name);
        length = strlen(entry->name);
    } else if (result < 0)
        FileDirectoryErrorPc(result);
    return length;
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
    strcpy(search, path);
    if (search[strlen(search) - 1] != '\\')
        strcat(search, WorldPathSeparator);
    strcat(search, FileDirectoryWildcardPc);
    FileDirectoryPathSlotsPc[slot] = (char *)NuAlloc(strlen(search) + 4, FileDirectorySourceNamePc, 0x21a);
    strcpy(FileDirectoryPathSlotsPc[slot], search);
    ++FileDirectoryOpenCountPc;
    FileDirectoryHostHandlesPc[slot] = FindFirstFileA(FileDirectoryPathSlotsPc[slot], &FileDirectoryFindDataPc[slot]);
    if (FileDirectoryHostHandlesPc[slot] == INVALID_HANDLE_VALUE) {
        NuFree(FileDirectoryPathSlotsPc[slot], FileDirectorySourceNamePc, 0x222);
        --FileDirectoryOpenCountPc;
        /* Retail clears the slot before this one (slot 0 hits the word in front of the table). */
        FileDirectoryPathSlotsPc[slot - 1] = NULL;
        return -1;
    }
    return slot;
}

i32 __cdecl FileDirectoryHostClosePc(i32 slot) {
    HANDLE handle;
    NuFree(FileDirectoryPathSlotsPc[slot], FileDirectorySourceNamePc, 0x22e);
    handle = FileDirectoryHostHandlesPc[slot];
    /* Retail clears the slot before this one, as in FileDirectoryHostOpenPc. */
    FileDirectoryPathSlotsPc[slot - 1] = NULL;
    FindClose(handle);
    --FileDirectoryOpenCountPc;
    return 0;
}

i32 __cdecl FileHostDeletePathPc(const char *path) {
    return DeleteFileA(path);
}

extern i32 __cdecl FileDeviceFormatDefaultNamePc(char *destination, char *path, i32 maximum_size);

i32 __cdecl FileHostMovePathPc(char *destination, char *source) {
    char formatted_destination[256];
    char formatted_source[256];
    FileDeviceFormatDefaultNamePc(formatted_source, source, 256);
    FileDeviceFormatDefaultNamePc(formatted_destination, destination, 256);
    DeleteFileA(formatted_destination);
    return MoveFileA(formatted_source, formatted_destination);
}

typedef struct AI_EDITOR_FILE_INFO_PC {
    u32 flags;
    u32 size;
    i8 created_second;
    i8 created_minute;
    i8 created_hour;
    i8 created_day;
    i8 created_month;
    u8 unknown_0d;
    i16 created_year;
    i8 second;
    i8 minute;
    i8 hour;
    i8 day;
    i8 month;
    u8 unknown_15;
    i16 year;
    u8 unknown_18;
    u8 unknown_19[3];
} AI_EDITOR_FILE_INFO_PC;
STATIC_ASSERT(sizeof(AI_EDITOR_FILE_INFO_PC) == 0x1c);
STATIC_ASSERT(offsetof(AI_EDITOR_FILE_INFO_PC, second) == 0x10);
STATIC_ASSERT(offsetof(AI_EDITOR_FILE_INFO_PC, year) == 0x16);
STATIC_ASSERT(offsetof(AI_EDITOR_FILE_INFO_PC, unknown_18) == 0x18);

i32 __cdecl SaveCreateDirectoryPc(char *path) {
    SECURITY_ATTRIBUTES security;
    security.bInheritHandle = 1;
    security.nLength = sizeof(security);
    security.lpSecurityDescriptor = NULL;
    return CreateDirectoryA(path, &security);
}

i32 __cdecl NuFileGetInfo(char *path, AI_EDITOR_FILE_INFO_PC *info) {
    WIN32_FILE_ATTRIBUTE_DATA attributes;
    SYSTEMTIME time;
    u32 flags;

    GetFileAttributesExA(path, GetFileExInfoStandard, &attributes);
    flags = (attributes.dwFileAttributes & 0x4000) | 0x400;
    flags >>= 9;
    flags |= attributes.dwFileAttributes & 0x10;
    flags >>= 1;
    flags |= ~(attributes.dwFileAttributes << 1) & 2;
    info->flags = flags;
    info->unknown_18 = 0;
    info->size = attributes.nFileSizeLow;
    FileTimeToSystemTime(&attributes.ftCreationTime, &time);
    info->created_year = (i16)time.wYear;
    info->created_month = (i8)time.wMonth;
    info->created_day = (i8)time.wDay;
    info->created_hour = (i8)time.wHour;
    info->created_minute = (i8)time.wMinute;
    info->created_second = (i8)time.wSecond;
    FileTimeToSystemTime(&attributes.ftLastWriteTime, &time);
    info->year = (i16)time.wYear;
    info->month = (i8)time.wMonth;
    info->day = (i8)time.wDay;
    info->hour = (i8)time.wHour;
    info->minute = (i8)time.wMinute;
    info->second = (i8)time.wSecond;
    return 1;
}
