#include "saga.h"

extern void __cdecl NuFileCorrectSlashes(NUFILE_DEVICE *device, char *path);

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
