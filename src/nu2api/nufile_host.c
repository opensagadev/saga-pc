#include "saga.h"

#include <string.h>

__declspec(noinline) i32 __cdecl NuMcOpen(i32 port, i32 slot, char *filepath, i32 mode, i32 async) {
    (void)port;
    (void)slot;
    (void)filepath;
    (void)mode;
    (void)async;
    return 0;
}

__declspec(noinline) i32 __cdecl NuMcRead(i32 file, void *destination, i32 size, i32 async) {
    (void)file;
    (void)destination;
    (void)size;
    (void)async;
    return 0;
}

__declspec(noinline) i32 __cdecl NuMcClose(i32 file, i32 async) {
    (void)file;
    (void)async;
    return 0;
}

void __cdecl NuFileError(void) {
}

void __cdecl NuFileRetry(void) {
}

i32 __cdecl NuFileStatus(NUFILE file) {
    (void)file;
    /* The PC dispatch guards make its archive-status traversal unreachable. */
    return 0;
}

i32 __cdecl NuFileExists(char *filepath) {
    return NuFileExistQuiet(filepath);
}

static void AquireFileBuffer(FILEINFO *info) {
    i32 index;
    i32 selected = 0;
    i32 time = file_time_count;
    FILEBUFF *buffer;

    for (index = 0; index < 4; index++) {
        if (file_buff[index].time < time) {
            time = file_buff[index].time;
            selected = index;
        }
    }

    buffer = &file_buff[selected];
    if (buffer->file_info != NULL)
        buffer->file_info->buf = NULL;
    buffer->file_info = info;
    buffer->time = file_time_count++;
    info->buf = buffer;

    if (info->buf_len != 0) {
        NuPSFileLSeek(info->handle, -info->buf_len, 1);
        NuPSFileRead(info->handle, buffer->data, info->buf_len);
    }
}

NUFILE __cdecl NuFileOpenDF(const char *filepath, i32 mode, NUDATHDR *header, i32 quiet) {
    char path[256];
    NUFILE_DEVICE *device;
    i32 index;
    i64 length;
    FILEINFO *info;

    device = NuFileGetDeviceFromPath(filepath);
    if (device == NULL) {
        if (header != NULL && mode != 1 && mode != 2) {
            NUFILE file = NuDatFileOpen(header, (char *)filepath, mode);
            if (file > 0)
                return file;
        }
        device = default_device;
    }

    device->format_name_fn(device, path, (char *)filepath, sizeof(path));
    if (device->id == 2)
        NuStrCat(path, nufile_disc_suffix);
    if (device->id == 1) {
        i32 file = NuMcOpen(path[2] - '0', path[3] - '0', path + device->name_len, mode, 0);
        if (file >= 0)
            return file + 0x1000;
        return 0;
    }

    index = NuPSFileOpen(path, mode);
    if (index < 0)
        return 0;

    info = &file_info[index];
    memset(info, 0, sizeof(FILEINFO));
    info->handle = index;
    info->mode = mode;

    if (mode != 1) {
        if (index <= 16) {
            do {
                length = NuFileSeek(index + 1, 0, 2);
                if (length < 0 && quiet == 0)
                    NuFileRetry();
            } while (length < 0);
            info->file_len = length;

            if (mode == 3) {
                while (NuFileStatus(index + 1) != 0) {
                }
            }

            do {
                length = NuFileSeek(index + 1, 0, 0);
                if (length < 0 && quiet == 0)
                    NuFileRetry();
            } while (length < 0);

            if (mode == 3) {
                while (NuFileStatus(index + 1) != 0) {
                }
            }
        }
    } else {
        info->file_len = 0;
    }

    if (mode == 0)
        info->use_buf = nufile_buffering_enabled;
    return index + 1;
}

i32 __cdecl NuFileRead(NUFILE file, void *destination, i32 size) {
    FILEINFO *info;
    i32 bytes_read;
    i32 available;
    char *output;

    if (file >= 0x1000) {
        return NuMcRead(file - 0x1000, destination, size, 0);
    }
    if (file >= 0x400) {
        return NuMemFileRead(file, destination, size);
    }

    file--;
    info = &file_info[file];
    if (info->use_buf == 0)
        return NuPSFileRead(file, destination, size);

    if (info->buf == NULL)
        AquireFileBuffer(info);
    bytes_read = 0;
    output = (char *)destination;

    while (size > 0) {
        if (info->read_pos >= info->file_len)
            return bytes_read;

        if (info->read_pos < info->buf_start || info->read_pos >= info->buf_start + info->buf_len) {
            if (info->read_pos != info->file_pos) {
                NuPSFileLSeek(info->handle, info->read_pos, 0);
                info->file_pos = info->read_pos;
            }

            info->buf_len = NuPSFileRead(info->handle, info->buf->data, sizeof(info->buf->data));
            info->buf_start = info->file_pos;
            info->file_pos += info->buf_len;
        }

        available = info->buf_len + (i32)(info->buf_start - info->read_pos);
        if (available >= size)
            available = size;
        if (available != 0)
            memcpy(output, info->buf->data + (i32)(info->read_pos - info->buf_start), available);
        output += available;
        bytes_read += available;
        size -= available;
        info->read_pos += available;
    }

    return bytes_read;
}

void __cdecl NuFileClose(NUFILE file) {
    FILEINFO *info;

    if (file >= 0x1000) {
        NuMcClose(file - 0x1000, 0);
        return;
    }
    if (file >= 0x400) {
        NuMemFileClose(file);
        return;
    }

    file--;
    while (NuPSFileClose(file) < 0)
        NuFileError();
    info = &file_info[file];
    if (info->buf != NULL)
        info->buf->file_info = NULL;
    memset(info, 0, sizeof(FILEINFO));
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
    info->created_hour = (i8)time.wHour;
    info->created_day = (i8)time.wDay;
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

i32 __cdecl FileDirectoryMcOpenPc(i32 port, i32 slot, char *path) {
    (void)port;
    (void)slot;
    (void)path;
    return 0;
}
void __cdecl FileDirectoryMcClosePc(i32 file) {
    (void)file;
}
i32 __cdecl FileDirectoryMcReadPc(i32 file, FILE_DIRECTORY_ENTRY_PC *entry) {
    (void)file;
    (void)entry;
    return 0;
}
