#include "saga.h"

extern i32 __cdecl NuMcSeek(i32 file, i32 offset, i32 origin, i32 async);

__declspec(noinline) i32 __cdecl NuMcFileOpenSize(NUFILE file) {
    (void)file;
    return 0;
}

i64 __cdecl NuMemFilePos(NUFILE file) {
    if (file >= 0x800) {
        PC_NUDATFILEINFO *info = &pc_dat_file_infos[file - 0x800];
        return info->position - info->start;
    }

    file -= 0x400;
    return pc_memfiles[file].position - pc_memfiles[file].buffer;
}

i64 __cdecl NuFilePos(NUFILE file) {
    i64 position;

    if (file >= 0x1000) {
        return NuMcSeek(file - 0x1000, 0, 2, 0);
    }

    if (file >= 0x400) {
        return NuMemFilePos(file);
    }

    file--;
    if (file_info[file].use_buf != 0) {
        return file_info[file].read_pos;
    }

    do {
        position = NuPSFileLSeek(file, 0, 1);
    } while (position < 0);

    return position;
}

i64 __cdecl NuFileOpenSize(NUFILE file) {
    if (file >= 0x1000) {
        return NuMcFileOpenSize(file - 0x1000);
    }

    if (file >= 0x800) {
        PC_NUDATFILEINFO *info = &pc_dat_file_infos[file - 0x800];
        if (info->compression_mode != 0) {
            return info->decompressed_length;
        }
        return info->file_length;
    }

    if (file >= 0x400) {
        return pc_memfiles[file - 0x400].end - pc_memfiles[file - 0x400].buffer;
    }

    return file_info[file - 1].file_len;
}

i64 __cdecl NuDatFileSeek(NUFILE file, i64 offset, i32 whence) {
    PC_NUDATFILEINFO *info = &pc_dat_file_infos[file - 0x800];
    NUDATOPENFILEINFO *open_file = &info->header->open_files[info->open_file_index];
    i64 target_position;

    switch (whence) {
        case 1:
            target_position = info->position + offset;
            break;
        case 2:
            target_position = info->start + info->file_length - offset;
            break;
        default:
            target_position = info->start + offset;
            break;
    }

    info->position = NuFileSeek(open_file->dat_file, target_position, 0);
    open_file->position = info->position;
    return info->position;
}

i64 __cdecl NuFileSize(char *filepath) {
    i32 saved_buffering = nufile_buffering_enabled;
    i64 position = -1;

    nufile_buffering_enabled = 0;
    if (curr_dat != NULL) {
        i32 index = NuDatFileFindTree(curr_dat, filepath);
        if (index >= 0) {
            /* The PC early return leaves file buffering disabled. */
            return curr_dat->file_info[index].decompressed_len;
        }
    }

    if (filepath != NULL && *filepath != '\0') {
        NUFILE file = NuFileOpenDF(filepath, 0, curr_dat, 0);
        if (file != 0) {
            do {
                position = NuFileSeek(file, 0, 2);
            } while (position < 0);

            position = NuFilePos(file);
            NuFileClose(file);
        }
    }

    nufile_buffering_enabled = saved_buffering;
    return position;
}
