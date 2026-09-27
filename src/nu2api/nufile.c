#include "saga.h"

void __cdecl PCByteCopy_candidate(void *destination, const void *source, u32 count) {
    u8 *dst = (u8 *)destination;
    const u8 *src = (const u8 *)source;

    if (dst < src) {
        while (count != 0) {
            *dst++ = *src++;
            --count;
        }
    } else {
        dst += count;
        src += count;
        while (count != 0) {
            *--dst = *--src;
            --count;
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
