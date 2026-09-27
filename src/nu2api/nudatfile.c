#include "saga.h"

extern i32 file_criticalsection;
extern PC_NUDATFILEINFO *unpack_file_info;
extern NUDATOPENFILEINFO *unpack_file_odi;
extern i64 unpack_file_pos;
extern i32 decode_buffer_left;
extern void __cdecl PCThreadCriticalSectionEnter_candidate(i32 section_index);
extern void __cdecl PCThreadCriticalSectionLeave_candidate(i32 section_index);

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
