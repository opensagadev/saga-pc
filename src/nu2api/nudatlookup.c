#include "saga.h"

extern void __cdecl NuFileCorrectSlashes(NUFILE_DEVICE *device, char *path);
extern i32 nufile_lsn_allowed;

STATIC_ASSERT(offsetof(NUDATHDR, unknown_b4) == 0xb4);

i32 __cdecl NuDatGetFileInfo(NUDATHDR *header, char *name, i64 *position, i32 *length) {
    i32 index;
    const NUDATFINFO *file;
    i64 file_position;

    if (header == NULL) {
        return -1;
    }

    index = NuDatFileFindTree(header, name);
    if (index < 0) {
        return -1;
    }

    file = &header->file_info[index];
    file_position = file->file_offset;
    if (header->version <= -2) {
        file_position = (i64)((u64)file_position << 8);
    }
    file_position += datsys_offset;
    if (position != NULL) {
        if (nufile_lsn_allowed != 0 && header->unknown_b4 != 0) {
            *position = (i64)(i32)((u32)header->unknown_b4 + (u32)(file_position / 0x800));
        } else {
            *position = file_position;
        }
    }
    if (length != NULL) {
        *length = header->file_info[index].file_len;
    }
    return index;
}

i32 __cdecl ExplodeBufferSize(char *header) {
    char magic[] = "LZ2K";
    i32 i;

    for (i = 0; i < 4; ++i) {
        if (*header++ != magic[i]) {
            return 0;
        }
    }
    return (i32)(((u32)(u8)header[0] | ((u32)(u8)header[1] << 8) | ((u32)(u8)header[2] << 16) |
                  ((u32)(u8)header[3] << 24)) &
                 PCImplodeWordMask);
}

i32 __cdecl ExplodeCompressedSize(char *header) {
    char magic[] = "LZ2K";
    i32 i;
    u32 size;

    for (i = 0; i < 4; ++i) {
        if (*header++ != magic[i]) {
            return 0;
        }
    }
    size = ((u32)(u8)header[4] | ((u32)(u8)header[5] << 8) | ((u32)(u8)header[6] << 16) |
            ((u32)(u8)header[7] << 24)) &
           PCImplodeWordMask;
    if (size != 0) {
        size += 12;
    }
    return (i32)size;
}

i32 __cdecl NuDatFileFindHash(NUDATHDR *header, char *name) {
    u32 hash = 0x811c9dc5;
    const char *cursor = name;
    i32 index;

    while (*cursor != '\0') {
        hash = ((hash ^ (u32)(i32)(i8)*cursor) * 0x00199933);
        cursor++;
    }

    for (index = 0; index < header->file_count; index++) {
        if (header->hash_idxs[index] == hash) {
            return index;
        }
    }

    {
        const char *entry = header->hashes;
        for (index = 0; index < header->hash_count; index++) {
            const char *entry_cursor = entry;
            const char *name_cursor = name;

            while (*entry_cursor == *name_cursor && *entry_cursor != '\0') {
                entry_cursor++;
                name_cursor++;
            }
            if (*entry_cursor == *name_cursor) {
                while (*entry != '\0') {
                    entry++;
                }
                entry++;
                if (((u32)entry & 1) != 0) {
                    entry++;
                }
                return *(const i16 *)entry;
            }

            while (*entry != '\0') {
                entry++;
            }
            entry += 3;
            if (((u32)entry & 1) != 0) {
                entry++;
            }
        }
    }

    return -1;
}

i32 __cdecl NuDatFileFindTree(NUDATHDR *header, char *name) {
    char buffer[256];
    const char *source;
    char *component;
    char *segment;
    i32 node_index;

    source = name;
    if (*source == '@') {
        source += 4;
    }

    {
        char *destination = buffer;
        while ((*destination++ = *source++) != '\0') {
        }
    }

    NuFileCorrectSlashes(NULL, buffer);

    if (header->file_tree == NULL) {
        return NuDatFileFindHash(header, buffer);
    }

    node_index = header->file_tree->child_idx;
    component = buffer;
    segment = NuStrChr(component, '\\');
    if (segment != NULL) {
        *segment = '\0';
    }

    do {
        NUDFNODE *node = &header->file_tree[node_index];

        if (NuStrICmp(node->name, component) == 0) {
            if (segment == NULL) {
                if (node->child_idx < 1) {
                    return -(i32)node->child_idx;
                }
                return -1;
            }

            node_index = node->child_idx;
            component = segment + 1;
            segment = NuStrChr(component, '\\');
            if (segment != NULL) {
                *segment = '\0';
            }
        } else {
            node_index = node->sibling_idx;
        }
    } while (node_index != 0);

    return -1;
}

i32 __cdecl NuFileExistQuiet(char *filepath) {
    NUFILE file = 0;
    i32 saved_buffering;

    if (filepath == NULL || *filepath == '\0') {
        return 0;
    }

    saved_buffering = nufile_buffering_enabled;
    nufile_buffering_enabled = 0;

    if (curr_dat != NULL) {
        i32 index = NuDatFileFindTree(curr_dat, filepath);
        if (index >= 0) {
            nufile_buffering_enabled = saved_buffering;
            return curr_dat->file_info[index].decompressed_len;
        }
    }

    file = NuFileOpenDF(filepath, 0, curr_dat, 1);
    if (file != 0) {
        NuFileClose(file);
    }

    nufile_buffering_enabled = saved_buffering;
    return file != 0;
}
