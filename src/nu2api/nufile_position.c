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

i32 NuFileEOF(NUFILE file) {
    i64 position = NuFilePos(file);
    i64 size = NuFileOpenSize(file);
    return position >= size;
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

i32 __cdecl NuFileBeginBlkRead(NUFILE file, i32 expected_tag) {
    i32 index = nufile_blkcnt++;
    i64 position = NuFilePos(file);
    PC_NUFILE_READBLOCK *block;

    if (index > 1 && nufile_blkinfo[index - 1].position + nufile_blkinfo[index - 1].size <= position) {
        --nufile_blkcnt;
        return 0;
    }
    block = &nufile_blkinfo[index];
    block->position = NuFilePos(file);
    NuFileRead(file, block, 8);
    if (block->size < 0) {
        block->size = -block->size;
    }
    if (expected_tag != 0 && expected_tag != block->tag) {
        NuFileError();
    }
    return block->tag;
}

i32 __cdecl NuFileAlignRead(NUFILE file, i32 mask) {
    i32 count = 0;
    i64 position = NuFilePos(file);
    while ((position & (i64)mask) != 0) {
        i8 byte = 0;
        ++position;
        ++count;
        NuFileRead(file, &byte, 1);
    }
    return count;
}

void __cdecl NuFileEndBlkRead(NUFILE file) {
    i32 index = --nufile_blkcnt;
    i64 position = NuFilePos(file);
    i64 end = nufile_blkinfo[index].position + nufile_blkinfo[index].size;
    if (position != end) {
        NuFileSeek(file, end, 0);
    }
}

i32 __cdecl NuFileGetBlkSize(void) {
    return nufile_blkinfo[nufile_blkcnt - 1].size;
}

i32 NuFileExtractPath(char *dest, char *path) {
    i32 length;
    char *end = strrchr(path, '\\');
    if (end != NULL) {
        ++end;
    } else {
        end = path;
    }
    length = end - path;
    strncpy(dest, path, length);
    dest[length] = '\0';
    return length;
}

i32 NuFileExtractFile(char *dest, char *path) {
    char *end;
    i32 length;
    char *start = strrchr(path, '\\');
    if (start != NULL) {
        ++start;
    } else {
        start = path;
    }
    end = strrchr(path, '.');
    if (end == NULL) {
        end = path + strlen(path);
    }
    length = end - start;
    strncpy(dest, start, length);
    dest[length] = '\0';
    return length;
}
