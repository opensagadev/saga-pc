#include "gameapi/apisave.h"
#include "gameapi/edtools/edui.h"
#include "nu2api/bgproc_pc.h"
#include "nu2api/huffdecode.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nudir_gen.h"
#include "nu2api/nuerror_gen.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile_pc.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include <windows.h>
#include <string.h>

i32 NuFileUseHiresPC(void) {
    return PcWindowOptions.usehires;
}

void NuPSFileInitDevices(i32 device_id, i32 reboot_iop, i32 eject) {
    (void)device_id;
    (void)reboot_iop;
    (void)eject;
}

char *FileDirectoryPathSlots[16] = {0};
i32 bgproc_initialized = 0;
HANDLE FileDirectoryHostHandles[16] = {0};
HANDLE bgproc_work_event = 0;
HANDLE bgproc_resume_event = 0;
WIN32_FIND_DATAA FileDirectoryFindData[16] = {0};
void (*nuwarning_handler)(char *) = 0;
char *nufile = 0;
char PCWarningFormatBuffer[1024] = {0};
i32 nuline = 0;
CRITICAL_SECTION PcStartupCriticalSection = {0};
void (*nuerror_handler)(char *) = 0;
char PCWarningMessages[1024] = {0};
char PCWarningOutputBuffer[1024] = {0};
CRITICAL_SECTION bgproc_critical_section = {0};
char err_msg_filter[32] = {0};
i32 DisableDebugMsg = 0;
i32 bHaveErr = 0;
BGPROCINFO *bgproc_active = 0;
HANDLE bgproc_thread_handle = NULL;
DWORD bgproc_thread_id = 0;
i32 bgproc_frozen = 0;
i32 bgproc_freeze = 0;
i32 PcStartupCriticalSectionInitialized = 0;
i32 FileDirectoryOpenCount = 0;
i32 PCThreadCriticalSectionCount = 0;
NUFILE_DEVICE *FileDirectoryDefaultDevice = &host_device;

i32 NuFileExists(char *filepath) {
    return NuFileExistQuiet(filepath);
}

i32 DEV_FormatName(NUFILE_DEVICE *device, char *formatted_name, char *path, i32 buf_size) {
    char buffer[512];
    i32 device_name_len;
    char *subpath;

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

    NuFileUpCase(device, buffer + NuStrLen(device->root));
    NuFileReldirFix(device, buffer);

    if (NuStrLen(buffer) < buf_size) {
        NuStrCpy(formatted_name, buffer);
        return 1;
    }

    return 0;
}

i32 fun_004ff280(NUFILE_DEVICE *device) {
    device->status = 1;
    return 1;
}

i32 DEVHOST_Interrogate(NUFILE_DEVICE *device) {
    device->status = 1;
    return 1;
}

NUFILE_DEVICE host_device = {
    0, 0, 0x0e, 0, '\\', 0x400, -1, -1, -1, {0}, {0}, 4, 6, {0}, "Host pc filesystem", {0}, {0}, {0}, DEV_FormatName, DEVHOST_Interrogate,
};

i32 fun_004ff2a0(NUFILE_DEVICE *device) {
    device->status = 1;
    return 1;
}

void fun_004ff2b0(void) {
}

i32 fun_004ff2c0(void) {
    return 0;
}

extern i32 FileDirectoryMcOpen(i32 port, i32 slot, char *path);
extern void FileDirectoryMcClose(i32 file);
extern i32 FileDirectoryMcRead(i32 file, FILE_DIRECTORY_ENTRY *entry);

NUFILE NuFileOpenDir(char *path) {
    char formatted[256];
    NUFILE_DEVICE *device = NuFileGetDeviceFromPath(path);
    i32 result;
    if (device == NULL) {
        device = FileDirectoryDefaultDevice;
    }
    device->format_name_fn(device, formatted, path, 256);
    if (device->id == 2) {
        strcat(formatted, ";1");
    }
    if (device->id == 1) {
        i32 port = formatted[2] - '0';
        i32 slot = formatted[3] - '0';
        result = FileDirectoryMcOpen(port, slot, formatted + device->name_len);
        if (result >= 0) {
            return result + 0x1000;
        }
        return 0;
    }
    result = FileDirectoryHostOpen(formatted);
    if (result < 0) {
        return 0;
    }
    return result + 1;
}

void NuFileCloseDir(NUFILE file) {
    if (file >= 0x1000) {
        FileDirectoryMcClose(file - 0x1000);
        return;
    }
    FileDirectoryHostClose(file - 1);
}

i32 NuFileReadDir(NUFILE file, FILE_DIRECTORY_ENTRY *entry) {
    FILE_DIRECTORY_HOST_ENTRY host;
    i32 result;
    u32 flags;
    i32 length = 0;
    if (file >= 0x1000) {
        return FileDirectoryMcRead(file - 0x1000, entry);
    }
    result = FileDirectoryHostRead(file - 1, &host);
    if (result > 0) {
        flags = (((host.flags >> 2) & 0x800) | (host.flags & 0x100));
        flags = ((flags >> 2) | (host.flags & 0x80));
        entry->flags = (((flags >> 2) | (host.flags & 0x40)) >> 4);
        entry->size = host.size;
        entry->created.second = host.created.second;
        entry->created.minute = host.created.minute;
        entry->created.hour = host.created.hour;
        entry->created.day = host.created.day;
        entry->created.month = host.created.month;
        entry->created.year = (u16)(((u16)host.created.year_high << 8) + host.created.year_low);
        entry->modified.second = host.modified.second;
        entry->modified.minute = host.modified.minute;
        entry->modified.hour = host.modified.hour;
        entry->modified.day = host.modified.day;
        entry->modified.month = host.modified.month;
        entry->modified.year = (u16)(((u16)host.modified.year_high << 8) + host.modified.year_low);
        strcpy(entry->name, host.name);
        length = strlen(entry->name);
    } else if (result < 0) {
        FileDirectoryError(result);
    }
    return length;
}

i32 FileDirectoryHostRead(i32 file, FILE_DIRECTORY_HOST_ENTRY *entry) {
    SYSTEMTIME time;
    u32 flags = 0x100;
    if ((FileDirectoryFindData[file].dwFileAttributes & 1) != 0) {
        flags = 0x180;
    }
    if ((FileDirectoryFindData[file].dwFileAttributes & 0x10) != 0) {
        flags += 0x2000;
    }
    entry->flags = flags;
    entry->size = FileDirectoryFindData[file].nFileSizeLow;
    entry->unused_24 = 0;
    strcpy(entry->name, FileDirectoryFindData[file].cFileName);
    FileTimeToSystemTime(&FileDirectoryFindData[file].ftCreationTime, &time);
    entry->created.second = (u8)time.wSecond;
    entry->created.minute = (u8)time.wMinute;
    entry->created.hour = (u8)time.wHour;
    entry->created.day = (u8)time.wDay;
    entry->created.month = (u8)time.wMonth;
    entry->created.year_low = (u8)time.wYear;
    entry->created.year_high = 0;
    FileTimeToSystemTime(&FileDirectoryFindData[file].ftLastWriteTime, &time);
    entry->modified.second = (u8)time.wSecond;
    entry->modified.minute = (u8)time.wMinute;
    entry->modified.hour = (u8)time.wHour;
    entry->modified.day = (u8)time.wDay;
    entry->modified.month = (u8)time.wMonth;
    entry->modified.year_low = (u8)time.wYear;
    entry->modified.year_high = 0;
    FileTimeToSystemTime(&FileDirectoryFindData[file].ftLastAccessTime, &time);
    entry->accessed.second = (u8)time.wSecond;
    entry->accessed.minute = (u8)time.wMinute;
    entry->accessed.hour = (u8)time.wHour;
    entry->accessed.day = (u8)time.wDay;
    entry->accessed.month = (u8)time.wMonth;
    entry->accessed.year_low = (u8)time.wYear;
    entry->accessed.year_high = 0;
    if (FileDirectoryFindData[file].dwFileAttributes == 0xffffffff) {
        return 0;
    }
    if (FindNextFileA(FileDirectoryHostHandles[file], &FileDirectoryFindData[file]) != 1) {
        FileDirectoryFindData[file].dwFileAttributes = 0xffffffff;
    }
    return strlen(entry->name);
}

i32 FileDirectoryHostOpen(char *path) {
    char search[200];
    i32 slot;
    if (FileDirectoryOpenCount == 16) {
        return 0;
    }
    for (slot = 0; slot < 16; ++slot) {
        if (FileDirectoryPathSlots[slot] == NULL) {
            break;
        }
    }
    if (slot == 16) {
        return 0;
    }
    strcpy(search, path);
    if (search[strlen(search) - 1] != '\\') {
        strcat(search, "\\");
    }
    strcat(search, "*.*");
    FileDirectoryPathSlots[slot] = NuAlloc(strlen(search) + 4, "d:\\projects\\game_saga\\nu2api.saga\\nucore\\pc\\nufile_pc.c", 0x21a);
    strcpy(FileDirectoryPathSlots[slot], search);
    ++FileDirectoryOpenCount;
    FileDirectoryHostHandles[slot] = FindFirstFileA(FileDirectoryPathSlots[slot], &FileDirectoryFindData[slot]);
    if (FileDirectoryHostHandles[slot] == INVALID_HANDLE_VALUE) {
        NuFree(FileDirectoryPathSlots[slot], "d:\\projects\\game_saga\\nu2api.saga\\nucore\\pc\\nufile_pc.c", 0x222);
        --FileDirectoryOpenCount;
        /* Clears the slot before this one (slot 0 hits the word in front of the table). */
        FileDirectoryPathSlots[slot - 1] = NULL;
        return -1;
    }
    return slot;
}

i32 FileDirectoryHostClose(i32 slot) {
    HANDLE handle;
    NuFree(FileDirectoryPathSlots[slot], "d:\\projects\\game_saga\\nu2api.saga\\nucore\\pc\\nufile_pc.c", 0x22e);
    handle = FileDirectoryHostHandles[slot];
    /* Clears the slot before this one, as in FileDirectoryHostOpen. */
    FileDirectoryPathSlots[slot - 1] = NULL;
    FindClose(handle);
    --FileDirectoryOpenCount;
    return 0;
}

i32 FileHostDeletePath(const char *path) {
    return DeleteFileA(path);
}

i32 FileHostMovePath(char *destination, char *source) {
    char formatted_destination[256];
    char formatted_source[256];
    FileDeviceFormatDefaultName(formatted_source, source, 256);
    FileDeviceFormatDefaultName(formatted_destination, destination, 256);
    DeleteFileA(formatted_destination);
    return MoveFileA(formatted_source, formatted_destination);
}

i32 SaveCreateDirectory(char *path) {
    SECURITY_ATTRIBUTES security;
    security.bInheritHandle = 1;
    security.nLength = sizeof(security);
    security.lpSecurityDescriptor = NULL;
    return CreateDirectoryA(path, &security);
}

i32 fun_004ff960(void) {
    return 0;
}

i32 NuFileGetInfo(char *path, AI_EDITOR_FILE_INFO *info) {
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
    info->name[0] = 0;
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
