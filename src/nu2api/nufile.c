#include "saga.h"

i32 __cdecl NuMcWrite(i32 file, const void *data, i32 size, i32 flags) {
    (void)file;
    (void)data;
    (void)size;
    (void)flags;
    return 0;
}

i32 __cdecl NuFileWrite(NUFILE file, const void *data, i32 size) {
    if (file >= 0x1000) {
        return NuMcWrite(file - 0x1000, data, size, 0);
    }

    if (file >= 0x400) {
        return NuMemFileWrite(file, data, size);
    }

    file -= 1;
    return NuPSFileWrite(file, data, size);
}

void __cdecl NuFileWriteString(NUFILE file, const char *text) {
    const char *end = text;
    i32 size;

    while (*end != '\0') {
        ++end;
    }
    size = (i32)(end - text);

    if (file >= 0x1000) {
        NuMcWrite(file - 0x1000, text, size, 0);
    } else if (file >= 0x400) {
        NuMemFileWrite(file, text, size);
    } else {
        NuPSFileWrite(file - 1, text, size);
    }
}

i32 __cdecl NuFileCopyEx(char *destination, char *source, void *buffer, i32 capacity) {
    i32 remaining;
    i32 copied = 0;
    NUFILE input = NuFileOpenDF(source, 0, curr_dat, 0);
    NUFILE output = NuFileOpenDF(destination, 1, curr_dat, 0);

    if (input != 0 && output != 0) {
        remaining = (i32)NuFileOpenSize(input);
        while (remaining != 0) {
            i32 count = NuFileRead(input, buffer, remaining < capacity ? remaining : capacity);
            NuFileWrite(output, buffer, count);
            copied += count;
            remaining -= count;
        }
    }

    if (input != 0) {
        NuFileClose(input);
    }
    if (output != 0) {
        NuFileClose(output);
    }
    return copied;
}

i32 __cdecl NuFileCopy(char *destination, char *source) {
    char buffer[1024];

    return NuFileCopyEx(destination, source, buffer, sizeof(buffer));
}

void __cdecl PCByteCopy_candidate(void *destination, const void *source, u32 count) {
    u8 *dst = (u8 *)destination;
    const u8 *src = (const u8 *)source;

    if (dst < src) {
        while (count != 0) {
            count--;
            *dst = *src;
            dst = dst + 1;
            src = src + 1;
        }
    } else {
        dst += count;
        src += count;
        while (count != 0) {
            count--;
            dst--;
            src--;
            *dst = *src;
        }
    }
}

u32 __fastcall PCFilePakChecksum_candidate(i32 count, const u8 *data) {
    u32 sum = 0x12345678;

    while (count != 0) {
        --count;
        sum += *data++;
    }

    return sum;
}

static PC_FILEPAKITEM *GetPakItems(PC_FILEPAKHDR_V0 *header) {
    switch (header->type) {
        case 0x12345678:
            return (PC_FILEPAKITEM *)(header + 1);
        case 0x1234567a:
            return (PC_FILEPAKITEM *)((PC_FILEPAKHDR *)header + 1);
        default:
            return (PC_FILEPAKITEM *)((PC_FILEPAKHDR_V1 *)header + 1);
    }
}

i32 __cdecl NuFilePakGetLastError(void) {
    return fpk_err;
}

u32 __cdecl NuFilePakGetSize(void *pak) {
    PC_FILEPAKHDR_V0 *header = (PC_FILEPAKHDR_V0 *)pak;
    u32 size = header->file_size;

    fpk_err = 0;
    return size;
}

i32 __cdecl NuFilePakGetItem(void *pak, char *name) {
    PC_FILEPAKHDR_V0 *header = (PC_FILEPAKHDR_V0 *)pak;
    PC_FILEPAKITEM *items;
    u32 index;

    fpk_err = 0;
    items = GetPakItems(header);
    for (index = 0; index < header->item_count; ++index) {
        if (NuStrICmp((char *)header + items[index].name_offset, name) == 0) {
            if ((items[index].flags & 2) != 0) {
                return 0;
            }
            return index + 1;
        }
    }
    return 0;
}

i32 __cdecl NuFilePakGetItemInfo(void *pak, i32 handle, void **address, i32 *size) {
    PC_FILEPAKHDR_V0 *header = (PC_FILEPAKHDR_V0 *)pak;
    PC_FILEPAKITEM *item;

    fpk_err = 0;
    --handle;
    if (handle >= 0 && (u32)handle < header->item_count) {
        item = GetPakItems(header) + handle;
        if ((item->flags & 2) == 0) {
            if (address != NULL) {
                *address = (char *)header + item->data_offset;
            }
            if (size != NULL) {
                *size = item->size;
            }
            return 1;
        }
    }
    return 0;
}

i32 __cdecl NuFilePakSetItemRequired(void *pak, i32 handle, i32 required) {
    PC_FILEPAKHDR_V0 *header = (PC_FILEPAKHDR_V0 *)pak;
    PC_FILEPAKITEM *item;

    fpk_err = 0;
    --handle;
    if (handle < 0 || (u32)handle >= header->item_count) {
        return 0;
    }
    item = GetPakItems(header) + handle;
    if ((item->flags & 2) == 0) {
        item->flags = (item->flags & ~1u) | (required & 1);
    }
    return 1;
}

void __cdecl NuFilePakRelocate(void *pak, void *destination) {
    PC_FILEPAKHDR_V0 *header = (PC_FILEPAKHDR_V0 *)pak;
    u32 size = header->file_size;

    fpk_err = 0;
    PCByteCopy_candidate(destination, header, size);
}

i32 __cdecl NuFilePakCondense(void *pak) {
    PC_FILEPAKHDR_V0 *header = (PC_FILEPAKHDR_V0 *)pak;
    PC_FILEPAKITEM *items;
    i32 removed_size;
    u32 index;

    fpk_err = 0;
    items = GetPakItems(header);
    removed_size = 0;
    for (index = 0; index < header->item_count; ++index) {
        PC_FILEPAKITEM *item = &items[index];

        if ((item->flags & 2) != 0) {
            continue;
        }
        if ((item->flags & 1) != 0) {
            if (removed_size != 0 && item->alignment != 0) {
                removed_size &= ~(item->alignment - 1);
            }
            if (removed_size != 0) {
                PCByteCopy_candidate((char *)header + item->data_offset - removed_size,
                                     (char *)header + item->data_offset, item->size);
                item->data_offset -= removed_size;
            }
        } else {
            item->flags |= 2;
            removed_size = (removed_size + item->size + item->alignment - 1) & ~(item->alignment - 1);
        }
    }
    header->file_size -= removed_size;
    return removed_size;
}

f32 __cdecl NuFileReadFloat(NUFILE file) {
    f32 value;

    NuFileRead(file, &value, sizeof(value));
    return value;
}

i32 __cdecl NuFileReadInt(NUFILE file) {
    i32 value;

    NuFileRead(file, &value, sizeof(value));
    return value;
}

i8 __cdecl NuFileReadChar(NUFILE file) {
    i8 value = 0;

    NuFileRead(file, &value, sizeof(value));
    return value;
}

u16 __cdecl NuFileReadWChar(NUFILE file) {
    u16 value = 0;

    NuFileRead(file, &value, sizeof(value));
    return value;
}

i32 __cdecl NuFileLoadBuffer(char *filepath, void *destination, i32 capacity) {
    NUFILE file;
    i32 loaded = 0;

    nufile_lasterror = 0;
    if (curr_dat != NULL) {
        loaded = NuDatFileLoadBuffer(curr_dat, filepath, destination, capacity);
        if (nufile_lasterror == -1) {
            return 0;
        }
    }

    if (loaded == 0) {
        if (NuFileExists(filepath)) {
            file = NuFileOpenDF(filepath, 0, curr_dat, 0);
            if (file != 0) {
                if (nufile_try_packed != 0) {
                    STUBBED_NAMED("NuFileLoadBuffer: PC NuPPLoadBuffer packed-file path");
                    nufile_lasterror = -1;
                    loaded = 0;
                } else {
                    loaded = (i32)NuFileOpenSize(file);
                    if (loaded <= capacity && loaded != 0) {
                        while (NuFileRead(file, destination, loaded) < 0) {
                            NuFileRetry();
                            while (NuFileSeek(file, 0, 0) < 0) {
                                NuFileRetry();
                            }
                        }
                    } else {
                        nufile_lasterror = -1;
                        loaded = 0;
                    }
                }
                NuFileClose(file);
            } else {
                nufile_lasterror = -3;
            }
        } else {
            nufile_lasterror = -2;
        }
    }
    return loaded;
}

i32 __cdecl NuFileLoadBufferVP(char *filepath, VARIPTR *buffer, VARIPTR *buffer_end) {
    i32 loaded = NuFileLoadBuffer(filepath, buffer->void_ptr, buffer_end->addr - buffer->addr);

    buffer->addr += loaded;
    return loaded;
}

NUDATHDR *__cdecl NuDatOpenEx(const char *filepath, VARIPTR *arena, i32 *size_out, i16 mode) {
    i32 saved_buffering = nufile_buffering_enabled;
    NUFILE file = NuFileOpenDF(filepath, mode, NULL, 0);
    i32 locator[2];
    i64 metadata_pos;
    i32 metadata_len;
    char formatted_name[256];
    i32 name_len;
    i32 name_storage;
    i32 allocation_size;
    NUDATHDR *header;
    u8 *metadata;
    u8 *cursor;
    u8 *hashes_start;
    i32 index;
    i32 read_size;

    if (file == 0)
        goto no_file;
    if (mode == 3) {
        while (NuFileStatus(file) != 0) {
        }
    }

    if (datsys_offset != 0) {
        do {
            metadata_pos = NuFileSeek(file, datsys_offset, 0);
            if (metadata_pos < 0)
                NuFileRetry();
        } while (metadata_pos < 0);
    }
    do {
        read_size = NuFileRead(file, locator, sizeof(locator));
        if (read_size < 0) {
            NuFileRetry();
            NuFileSeek(file, datsys_offset, 0);
        }
    } while (read_size < 0);
    if (read_size != sizeof(locator))
        goto close_file;
    if (mode == 3) {
        while (NuFileStatus(file) != 0) {
        }
    }

    metadata_pos = locator[0];
    if (metadata_pos < 0)
        metadata_pos *= -256;
    metadata_pos += datsys_offset;
    metadata_len = locator[1];
    do {
        if (NuFileSeek(file, metadata_pos, 0) >= 0)
            break;
        NuFileRetry();
    } while (1);
    if (mode == 3) {
        while (NuFileStatus(file) != 0) {
        }
    }

    default_device->format_name_fn(default_device, formatted_name, (char *)filepath, sizeof(formatted_name));
    name_len = NuStrLen(formatted_name);
    name_storage = (name_len + 16) & ~15;
    if (metadata_len < 16 || metadata_len > 0x7fffffff - 0xc0 - name_storage)
        goto close_file;
    allocation_size = sizeof(NUDATHDR) + name_storage + metadata_len;

    if (arena == NULL || arena->void_ptr == NULL) {
        NuFileError();
        STUBBED_NAMED("NuDatOpenEx: permanent arena not initialized");
        goto close_file;
    }

    header = (NUDATHDR *)arena->void_ptr;
    arena->char_ptr += allocation_size;
    if (size_out != NULL)
        *size_out = allocation_size;
    header->unknown_b0 = 0;
    header->name = (char *)(header + 1);
    NuStrCpy(header->name, formatted_name);
    metadata = (u8 *)header->name + name_storage;
    do {
        read_size = NuFileRead(file, metadata, metadata_len);
        if (read_size < 0) {
            NuFileRetry();
            NuFileSeek(file, metadata_pos, 0);
        }
    } while (read_size < 0);
    if (read_size != metadata_len)
        goto close_file;
    if (mode == 3) {
        while (NuFileStatus(file) != 0) {
        }
    }

    header->version = ((i32 *)metadata)[0];
    header->file_count = ((i32 *)metadata)[1];
    if (header->file_count < 0 || header->file_count > (metadata_len - 16) / (i32)sizeof(NUDATFINFO))
        goto close_file;
    header->file_info = (NUDATFINFO *)(metadata + 8);
    cursor = (u8 *)(header->file_info + header->file_count);
    header->tree_node_count = *(i32 *)cursor;
    cursor += sizeof(i32);
    if (header->tree_node_count < 0 ||
        header->tree_node_count > (metadata + metadata_len - cursor - 4) / (i32)sizeof(NUDFNODE))
        goto close_file;
    header->file_tree = (NUDFNODE *)cursor;
    cursor += header->tree_node_count * sizeof(NUDFNODE);
    header->leaf_names_len = *(i32 *)cursor;
    cursor += sizeof(i32);
    if (header->leaf_names_len < 0 || header->leaf_names_len > metadata + metadata_len - cursor)
        goto close_file;
    header->leaf_names = (char *)cursor;
    for (index = 0; index < header->tree_node_count; ++index) {
        header->file_tree[index].name = header->leaf_names + (u32)header->file_tree[index].name;
    }
    if (header->tree_node_count != 0)
        header->file_tree[0].name = NULL;
    cursor += header->leaf_names_len;

    header->hash_idxs = NULL;
    header->hash_count = 0;
    header->hashes_len = 0;
    header->hashes = NULL;
    if (header->version < -1) {
        if (metadata + metadata_len - cursor < header->file_count * (i32)sizeof(u32) + 8)
            goto close_file;
        header->hash_idxs = (u32 *)cursor;
        cursor += header->file_count * sizeof(u32);
        header->hash_count = *(i32 *)cursor;
        cursor += sizeof(i32);
        header->hashes_len = *(i32 *)cursor;
        cursor += sizeof(i32);
        if (header->hashes_len < 0 || header->hashes_len > metadata + metadata_len - cursor)
            goto close_file;
        header->hashes = (char *)cursor;
        hashes_start = (u8 *)header->file_tree;
        PCByteCopy_candidate(hashes_start, header->hash_idxs, header->file_count * sizeof(u32));
        header->hash_idxs = (u32 *)hashes_start;
        cursor = hashes_start + header->file_count * sizeof(u32);
        if (header->hashes_len != 0) {
            header->hashes = (char *)cursor;
            PCByteCopy_candidate(cursor, cursor, header->hashes_len);
            cursor += header->hashes_len;
        }
        arena->void_ptr = cursor;
        header->tree_node_count = 0;
        header->file_tree = NULL;
        header->leaf_names_len = 0;
        header->leaf_names = NULL;
    }

    for (index = 0; index < 8; ++index) {
        header->open_files[index].dat_file = 0;
        header->open_files[index].info_idx = -1;
        header->open_files[index].position = 0;
    }
    header->open_files[0].dat_file = file;
    header->mode = mode;
    header->unknown_b4 = 0;
    nufile_buffering_enabled = saved_buffering;
    return header;

close_file:
    NuFileClose(file);
no_file:
    nufile_buffering_enabled = saved_buffering;
    return NULL;
}

NUDATHDR *__cdecl NuDatOpen(const char *filepath, VARIPTR *buf, i32 *unused) {
    return NuDatOpenEx(filepath, buf, unused, 0);
}

NUDATHDR *__cdecl NuDatSet(NUDATHDR *header) {
    NUDATHDR *dat = curr_dat;
    curr_dat = header;
    return dat;
}

NUDATHDR *__cdecl NuDatGet(void) {
    return curr_dat;
}

NUFILE __cdecl NuFileOpen(const char *filepath, i32 mode) {
    return NuFileOpenDF(filepath, mode, curr_dat, 0);
}
