#include "saga.h"

#include <string.h>

void __cdecl NuFileError(void) {
}

void __cdecl NuFileRetry(void) {
}

i32 __cdecl NuFileStatus(NUFILE file) {
    (void)file;
    /* The PC dispatch guards make its archive-status traversal unreachable. */
    return 0;
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
            STUBBED_NAMED("NuFileOpenDF: try NuDatFileOpen before the host path");
        }
        device = default_device;
    }

    device->format_name_fn(device, path, filepath, sizeof(path));
    if (device->id == 2)
        NuStrCat(path, nufile_disc_suffix);
    if (device->id == 1) {
        STUBBED_NAMED("NuFileOpenDF: NuMcOpen using the formatted port and slot");
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

i64 __cdecl NuFileSeek(NUFILE file, i64 offset, i32 origin) {
    FILEINFO *info;

    if (file >= 0x1000) {
        STUBBED_NAMED("NuFileSeek: NuMcSeek");
        return -1;
    }
    if (file >= 0x400) {
        STUBBED_NAMED("NuFileSeek: memory-file and archive-member seek");
        return -1;
    }

    file--;
    info = &file_info[file];
    if (info->use_buf == 0)
        return NuPSFileLSeek(file, offset, origin);

    switch (origin) {
        case 1:
            info->read_pos += offset;
            break;
        case 2:
            info->read_pos = info->file_len - offset;
            break;
        default:
            info->read_pos = offset;
            break;
    }
    return info->read_pos;
}

i32 __cdecl NuFileRead(NUFILE file, void *destination, i32 size) {
    FILEINFO *info;
    i32 bytes_read;
    i32 available;
    char *output;

    if (file >= 0x1000) {
        STUBBED_NAMED("NuFileRead: NuMcRead");
        return -1;
    }
    if (file >= 0x400) {
        STUBBED_NAMED("NuFileRead: memory-file and archive-member read");
        return -1;
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
        STUBBED_NAMED("NuFileClose: NuMcClose");
        return;
    }
    if (file >= 0x400) {
        STUBBED_NAMED("NuFileClose: memory-file and archive-member close");
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
