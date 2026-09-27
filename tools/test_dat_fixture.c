#include "saga.h"
#include <stdio.h>
#include <string.h>
/* Generated synthetic expectations; no game bytes. */
static const NUDATFINFO expected_files[2] = {
    {2, 4, 4, 0},
    {1, 4, 4, 0},
};
static const u32 expected_hashes[2] = {0x70235d42u, 0xcfb0c35bu};
static const char expected_payloads[2][4] = {
    {119, 120, 121, 122},
    {65, 66, 67, 68},
};

HANDLE pc_file_handles[32];
FILEINFO file_info[33];
FILEBUFF file_buff[4];
i32 file_time_count;
i32 nufile_buffering_enabled;
const char nufile_disc_suffix[3] = ";1";
NUFILE_DEVICE host_device;
NUFILE_DEVICE *default_device = &host_device;
NUFILE_DEVICE devices[16];
i32 numdevices;
i32 DontDoFileUpperCaseHack;
NUDATHDR *curr_dat;
i32 datsys_offset;

#define REQUIRE(condition)                                                                                             \
    do {                                                                                                               \
        if (!(condition)) {                                                                                            \
            printf("FAIL line %d: %s\n", __LINE__, #condition);                                                        \
            return 1;                                                                                                  \
        }                                                                                                              \
    } while (0)

static int exercise(const char *path, i32 base_offset, i32 buffered, int want_size) {
    __declspec(align(16)) unsigned char guarded[1024];
    NUDATHDR previous;
    VARIPTR arena;
    NUDATHDR *header;
    char *start;
    char *metadata;
    i32 name_storage, allocation, i, size_out;
    NUFILE file;
    char payload[4];

    memset(guarded, 0xa5, sizeof(guarded));
    start = (char *)guarded + 32;
    arena.char_ptr = start;
    name_storage = ((i32)strlen(path) + 16) & ~15;
    allocation = 192 + name_storage + 112;
    metadata = start + 192 + name_storage;
    size_out = -777;
    datsys_offset = base_offset;
    nufile_buffering_enabled = buffered;
    curr_dat = &previous;

    header = NuDatOpenEx(path, &arena, want_size ? &size_out : NULL, 0);
    REQUIRE(header == (NUDATHDR *)start);
    REQUIRE(curr_dat == &previous);
    REQUIRE(nufile_buffering_enabled == buffered);
    REQUIRE(size_out == (want_size ? allocation : -777));
    REQUIRE(header->version == -3 && header->file_count == 2);
    REQUIRE(header->file_info == (NUDATFINFO *)(metadata + 8));
    REQUIRE(memcmp(header->file_info, expected_files, sizeof(expected_files)) == 0);
    REQUIRE(header->hash_idxs == (u32 *)(metadata + 44));
    REQUIRE(memcmp(header->hash_idxs, expected_hashes, sizeof(expected_hashes)) == 0);
    REQUIRE(arena.char_ptr == metadata + 52);
    REQUIRE(header->hashes == metadata + 112);
    REQUIRE(header->hash_count == 0 && header->hashes_len == 0);
    REQUIRE(header->file_tree == NULL && header->tree_node_count == 0);
    REQUIRE(header->leaf_names == NULL && header->leaf_names_len == 0);
    REQUIRE(header->name == start + sizeof(NUDATHDR));
    REQUIRE(strcmp(header->name, path) == 0);
    REQUIRE(header->unknown_b0 == 0 && header->unknown_b4 == 0 && header->mode == 0);
    file = header->open_files[0].dat_file;
    REQUIRE(file == 1);
    REQUIRE(file_info[0].handle == 0 && file_info[0].use_buf == buffered);
    for (i = 0; i < 8; ++i) {
        REQUIRE(header->open_files[i].dat_file == (i == 0 ? 1 : 0));
        REQUIRE(header->open_files[i].info_idx == -1);
        REQUIRE(header->open_files[i].position == 0);
    }
    for (i = 0; i < 32; ++i)
        REQUIRE(guarded[i] == 0xa5);
    for (i = 32 + allocation; i < sizeof(guarded); ++i)
        REQUIRE(guarded[i] == 0xa5);

    /* Read synthetic payloads through the real host engine handle. */
    for (i = 0; i < 2; ++i) {
        i64 position = (i64)header->file_info[i].file_offset * 256 + base_offset;
        REQUIRE(NuFileSeek(file, position, 0) == position);
        REQUIRE(NuFileRead(file, payload, 4) == 4);
        REQUIRE(memcmp(payload, expected_payloads[i], 4) == 0);
    }
    NuFileClose(file);
    REQUIRE(pc_file_handles[0] == INVALID_HANDLE_VALUE);
    REQUIRE(file_info[0].buf == NULL);
    curr_dat = NULL;
    printf("PASS %s buffered=%d size_out=%d\n", path, buffered, want_size);
    return 0;
}

static int reject_metadata(const char *path, i32 metadata_size, int before_allocation, i32 buffered) {
    __declspec(align(16)) unsigned char guarded[1024];
    NUDATHDR previous;
    VARIPTR arena;
    char *start;
    i32 allocation, size_out, i;

    memset(guarded, 0xa5, sizeof(guarded));
    start = (char *)guarded + 32;
    arena.char_ptr = start;
    allocation = 192 + (((i32)strlen(path) + 16) & ~15) + metadata_size;
    size_out = -777;
    datsys_offset = 0;
    nufile_buffering_enabled = buffered;
    curr_dat = &previous;
    REQUIRE(NuDatOpenEx(path, &arena, &size_out, 0) == NULL);
    REQUIRE(nufile_buffering_enabled == buffered && curr_dat == &previous);
    if (before_allocation) {
        REQUIRE(arena.char_ptr == start && size_out == -777);
        for (i = 0; i < sizeof(guarded); ++i)
            REQUIRE(guarded[i] == 0xa5);
    } else {
        /* Current explicit failure path consumes its already reserved arena. */
        REQUIRE(arena.char_ptr == start + allocation && size_out == allocation);
        for (i = 0; i < 32; ++i)
            REQUIRE(guarded[i] == 0xa5);
        for (i = 32 + allocation; i < sizeof(guarded); ++i)
            REQUIRE(guarded[i] == 0xa5);
    }
    for (i = 0; i < 32; ++i)
        REQUIRE(pc_file_handles[i] == INVALID_HANDLE_VALUE);
    REQUIRE(file_info[0].buf == NULL);
    curr_dat = NULL;
    printf("PASS rejected %s buffered=%d\n", path, buffered);
    return 0;
}

int main(int argc, char **argv) {
    static const char *paths[] = {"TINY.DAT", "NEGPOS.DAT", "OFFSET.DAT", "LONG_NAMED_FIXTURE.DAT"};
    unsigned char untouched[512];
    VARIPTR arena;
    i32 i, buffered, size_out;

    REQUIRE(argc == 2 && SetCurrentDirectoryA(argv[1]));
    for (i = 0; i < 32; ++i)
        pc_file_handles[i] = INVALID_HANDLE_VALUE;
    host_device.dir_separator = '\\';
    host_device.format_name_fn = DEV_FormatName;
    host_device.interrogate_fn = DEVHOST_Interrogate;
    for (buffered = 0; buffered < 2; ++buffered) {
        for (i = 0; i < 4; ++i)
            REQUIRE(exercise(paths[i], i == 2 ? 64 : 0, buffered, 1) == 0);
    }
    REQUIRE(exercise("TINY.DAT", 0, 0, 0) == 0);
    for (buffered = 0; buffered < 2; ++buffered) {
        REQUIRE(reject_metadata("SHORT.DAT", 0, 1, buffered) == 0);
        REQUIRE(reject_metadata("META8.DAT", 8, 1, buffered) == 0);
        REQUIRE(reject_metadata("META11.DAT", 11, 1, buffered) == 0);
        REQUIRE(reject_metadata("CUTMETA.DAT", 112, 0, buffered) == 0);
        REQUIRE(reject_metadata("BADCOUNT.DAT", 16, 0, buffered) == 0);
        REQUIRE(reject_metadata("BADTREE.DAT", 16, 0, buffered) == 0);
        REQUIRE(reject_metadata("NOHASH.DAT", 16, 0, buffered) == 0);
    }
    datsys_offset = 0;
    size_out = -777;
    nufile_buffering_enabled = 1;
    REQUIRE(NuDatOpenEx("TINY.DAT", NULL, &size_out, 0) == NULL);
    REQUIRE(size_out == -777 && nufile_buffering_enabled == 1);
    REQUIRE(pc_file_handles[0] == INVALID_HANDLE_VALUE);
    arena.void_ptr = untouched;
    REQUIRE(GetFileAttributesA("ABSENT_SYNTHETIC_V3.DAT") == INVALID_FILE_ATTRIBUTES);
    REQUIRE(NuDatOpenEx("ABSENT_SYNTHETIC_V3.DAT", &arena, &size_out, 0) == NULL);
    REQUIRE(arena.void_ptr == untouched && size_out == -777);
    REQUIRE(nufile_buffering_enabled == 1);
    for (i = 0; i < 32; ++i)
        REQUIRE(pc_file_handles[i] == INVALID_HANDLE_VALUE);
    puts("PASS synthetic version -3 DAT parser smoke");
    return 0;
}
