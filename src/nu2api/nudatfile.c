#include "saga.h"

#include <string.h>

static i32 decode_buffer_left;
static i32 decode_buffer_pos;
static u8 decode_buffer[262144];
/* Preserve the existing canonical working view; this is not a PC capacity claim. */
static u8 read_buffer[0x40000];
static i32 read_buffer_size;
static i32 read_buffer_decoded_size;

extern i32 file_criticalsection;
extern PC_NUDATFILEINFO *unpack_file_info;
extern NUDATOPENFILEINFO *unpack_file_odi;
extern i64 unpack_file_pos;

extern void __cdecl PCThreadCriticalSectionEnter_candidate(i32 section_index);
extern void __cdecl PCThreadCriticalSectionLeave_candidate(i32 section_index);

void __cdecl NuDatFileDecodeNext(void) {
    char block_header[12];
    i32 file;
    i64 position;

    file = unpack_file_odi->dat_file;
    NuFileRead(file, block_header, 12);

    read_buffer_decoded_size = ExplodeBufferSize(block_header);
    read_buffer_size = ExplodeCompressedSize(block_header) - 12;
    NuFileRead(file, read_buffer, read_buffer_size);

    position = unpack_file_info->position + read_buffer_size + 12;
    unpack_file_info->position = position;
    unpack_file_odi->position = position;
}

i64 __cdecl NuDatCalcPos(NUDATHDR *header, i32 offset) {
    i64 position = offset;

    if (header->version <= -2) {
        position = (i64)((u64)position << 8);
    }
    return position + datsys_offset;
}

i32 __cdecl NuDatFileGetFreeInfo(void) {
    i32 found = -1;
    i32 index;

    PCThreadCriticalSectionEnter_candidate(file_criticalsection);
    for (index = 0; index < 20; index++) {
        if (pc_dat_file_infos[index].is_used == 0) {
            pc_dat_file_infos[index].is_used = 1;
            found = index;
            break;
        }
    }
    PCThreadCriticalSectionLeave_candidate(file_criticalsection);
    return found;
}

i32 __cdecl NuDatFileGetFreeHandleIX(NUDATHDR *header, i32 info_index) {
    i32 found = -1;
    i32 index;

    PCThreadCriticalSectionEnter_candidate(file_criticalsection);
    for (index = 0; index < 8; index++) {
        if (header->open_files[index].info_idx == -1) {
            header->open_files[index].info_idx = info_index;
            found = index;
            break;
        }
    }
    PCThreadCriticalSectionLeave_candidate(file_criticalsection);
    return found;
}

static NUFILE OpenDatFileBase(NUDATHDR *header, i32 file_index) {
    NUDATOPENFILEINFO *open_file = &header->open_files[file_index];

    if (open_file->dat_file == 0) {
        open_file->dat_file = NuFileOpenDF(header->name, header->mode, NULL, 0);
        open_file->position = 0;
    }
    return open_file->dat_file;
}

NUFILE __cdecl NuDatFileOpen(NUDATHDR *header, char *filepath, i32 mode) {
    i32 node_index;
    i32 info_index;
    i32 file_index;
    NUDATOPENFILEINFO *open_file;
    PC_NUDATFILEINFO *info;

    if (mode != 0) {
        return 0;
    }

    node_index = NuDatFileFindTree(header, filepath);
    if (node_index < 0 || header->file_info[node_index].file_len == 0) {
        NuFileRetry();
        return 0;
    }

    info_index = NuDatFileGetFreeInfo();
    if (info_index == -1) {
        NuFileRetry();
        return 0;
    }

    file_index = NuDatFileGetFreeHandleIX(header, info_index);
    if (file_index == -1) {
        pc_dat_file_infos[info_index].is_used = 0;
        return 0;
    }

    open_file = &header->open_files[file_index];
    OpenDatFileBase(header, file_index);

    info = &pc_dat_file_infos[info_index];
    info->header = header;
    info->start = NuDatCalcPos(header, header->file_info[node_index].file_offset);
    info->position = info->start;
    info->file_length = header->file_info[node_index].file_len;
    info->decompressed_length = header->file_info[node_index].decompressed_len;
    info->compression_mode = header->file_info[node_index].compression_mode;
    info->open_file_index = file_index;
    open_file->position = info->position;

    while (NuFileSeek(open_file->dat_file, info->start, 0) < 0) {
        NuFileRetry();
    }

    if (info->compression_mode == 2) {
        unpack_file_info = info;
        unpack_file_odi = open_file;
        unpack_file_pos = info->start;
        decode_buffer_left = 0;
    }
    return info_index + 0x800;
}

i32 __cdecl NuDatFileRead(NUFILE file, void *destination, i32 size) {
    PC_NUDATFILEINFO *info = &pc_dat_file_infos[file - 0x800];
    NUDATOPENFILEINFO *open_file = &info->header->open_files[info->open_file_index];
    i64 remaining;
    i32 to_read;
    i32 bytes_read;

    if (info->compression_mode == 2) {
        i32 total_read = 0;
        u8 *output = (u8 *)destination;
        while (size != 0) {
            if (decode_buffer_left == 0) {
                NuDatFileDecodeNext();
                if (read_buffer_size == read_buffer_decoded_size) {
                    memcpy(decode_buffer, read_buffer, read_buffer_size);
                } else {
                    ExplodeBufferNoHeader((char *)read_buffer, (char *)decode_buffer, read_buffer_size,
                                          read_buffer_decoded_size);
                }
                decode_buffer_pos = 0;
                decode_buffer_left = read_buffer_decoded_size;
            }
            bytes_read = size < decode_buffer_left ? size : decode_buffer_left;
            memcpy(output, decode_buffer + decode_buffer_pos, bytes_read);
            output += bytes_read;
            decode_buffer_pos += bytes_read;
            decode_buffer_left -= bytes_read;
            size -= bytes_read;
            total_read += bytes_read;
        }
        return total_read;
    }

    if (info->position != open_file->position) {
        NuFileSeek(open_file->dat_file, info->position, 0);
        open_file->position = info->position;
    }

    remaining = info->file_length + info->start - info->position;
    if (remaining < 0) {
        remaining = 0;
    }
    to_read = size;
    if ((i64)to_read >= remaining) {
        to_read = (i32)remaining;
    }
    if (to_read != 0) {
        bytes_read = NuFileRead(open_file->dat_file, destination, to_read);
        if (bytes_read >= 0) {
            info->position += bytes_read;
            open_file->position = info->position;
        }
        return bytes_read;
    }
    return 0;
}

i32 __cdecl NuDatFileLoadBuffer(NUDATHDR *header, char *filepath, void *destination, i32 capacity) {
    NUFILE file;
    PC_NUDATFILEINFO *info;
    i32 size;

    nufile_lasterror = 0;
    file = NuDatFileOpen(header, filepath, 0);
    if (file == 0) {
        return 0;
    }

    if (header->mode == 3) {
        while (NuFileStatus(file) != 0) {
        }
    }

    info = &pc_dat_file_infos[file - 0x800];
    if (info->compression_mode != 0) {
        size = info->decompressed_length;
    } else {
        size = info->file_length;
    }

    if (size <= capacity && size != 0) {
        while (NuDatFileRead(file, destination, size) < 0) {
            do {
                NuFileRetry();
            } while (NuDatFileSeek(file, 0, 0) < 0);
        }

        if (header->mode == 3) {
            while (NuFileStatus(file) != 0) {
            }
        }
        NuFileClose(file);
        return size;
    }

    if (size != 0) {
        nufile_lasterror = -1;
    }
    NuFileClose(file);
    return 0;
}

i32 __cdecl NuMemFileRead(NUFILE file, void *destination, i32 size) {
    PC_NUMEMFILE *memory;
    i32 available;

    if (file >= 0x800) {
        return NuDatFileRead(file, destination, size);
    }

    memory = &pc_memfiles[file - 0x400];
    available = (i32)(memory->end - memory->position) + 1;
    if (size > available) {
        size = available;
    }
    if (size != 0) {
        memcpy(destination, memory->position, size);
        memory->position += size;
    }
    return size;
}

i64 __cdecl NuMemFileSeek(NUFILE file, i64 offset, i32 origin) {
    PC_NUMEMFILE *memory;

    if (file >= 0x800) {
        return NuDatFileSeek(file, offset, origin);
    }

    memory = &pc_memfiles[file - 0x400];
    switch (origin) {
        case 1:
            memory->position += (i32)offset;
            break;
        case 2:
            memory->position = memory->end - (i32)offset;
            break;
        default:
            memory->position = memory->buffer + (i32)offset;
            break;
    }
    return memory->position - memory->buffer;
}

void __cdecl NuMemFileClose(NUFILE file) {
    if (file >= 0x800) {
        PC_NUDATFILEINFO *info = &pc_dat_file_infos[file - 0x800];

        if (info->open_file_index >= 0) {
            info->header->open_files[info->open_file_index].info_idx = -1;
        } else {
            NuFileError();
        }
        info->is_used = 0;
    } else {
        pc_memfiles[file - 0x400].used = 0;
    }
}
