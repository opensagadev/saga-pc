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
PC_NUDATFILEINFO pc_dat_file_infos[20];
PC_NUMEMFILE pc_memfiles[20];
i32 file_criticalsection;
PC_NUDATFILEINFO *unpack_file_info;
NUDATOPENFILEINFO *unpack_file_odi;
i64 unpack_file_pos;
i32 decode_buffer_left;
i32 PCThreadCriticalSectionCount_candidate;
CRITICAL_SECTION PCThreadCriticalSections_candidate[13];
i32 PCFrameCriticalSectionIndex_candidate;

extern i32 __cdecl PCThreadCriticalSectionCreate_candidate(void);

#define REQUIRE(condition)                                                                                             \
    do {                                                                                                               \
        if (!(condition)) {                                                                                            \
            printf("FAIL line %d: %s\n", __LINE__, #condition);                                                        \
            return 1;                                                                                                  \
        }                                                                                                              \
    } while (0)

static int exercise_members(NUDATHDR *header, i32 base_offset) {
    NUFILE first, second, members[8];
    char payload[16];
    i32 i;

    first = NuFileOpenDF("D\\B.BIN", 0, header, 0);
    REQUIRE(first == 0x800);
    REQUIRE(NuFileOpenSize(first) == 4 && NuFilePos(first) == 0);
    REQUIRE(NuFileRead(first, payload, 2) == 2 && memcmp(payload, "wx", 2) == 0);
    REQUIRE(NuFilePos(first) == 2);
    REQUIRE(NuFileSeek(first, 1, 1) == base_offset + 515);
    REQUIRE(NuFileRead(first, payload, sizeof(payload)) == 1 && payload[0] == 'z');
    REQUIRE(NuFileRead(first, payload, 1) == 0 && NuFilePos(first) == 4);
    REQUIRE(NuFileSeek(first, 2, 2) == base_offset + 514);
    REQUIRE(NuFileRead(first, payload, 4) == 2 && memcmp(payload, "yz", 2) == 0);
    REQUIRE(NuFileSeek(first, 0, 0) == base_offset + 512);
    REQUIRE(NuFileRead(first, payload, 4) == 4 && memcmp(payload, "wxyz", 4) == 0);

    second = NuFileOpenDF("D\\A.BIN", 0, header, 0);
    REQUIRE(second == 0x801 && header->open_files[1].dat_file == 2);
    REQUIRE(NuFileRead(second, payload, 4) == 4 && memcmp(payload, "ABCD", 4) == 0);
    REQUIRE(NuFileSeek(second, 0, 0) == base_offset + 256);
    pc_dat_file_infos[1].compression_mode = 2;
    REQUIRE(NuFileRead(second, payload, 1) == -1 && NuFilePos(second) == 0);
    pc_dat_file_infos[1].compression_mode = 0;
    NuFileClose(first);
    NuFileClose(second);
    REQUIRE(header->open_files[0].info_idx == -1 && header->open_files[1].info_idx == -1);
    REQUIRE(pc_dat_file_infos[0].is_used == 0 && pc_dat_file_infos[1].is_used == 0);
    REQUIRE(pc_file_handles[0] != INVALID_HANDLE_VALUE && pc_file_handles[1] != INVALID_HANDLE_VALUE);

    for (i = 0; i < 8; ++i) {
        members[i] = NuDatFileOpen(header, "D\\B.BIN", 0);
        REQUIRE(members[i] == 0x800 + i && header->open_files[i].info_idx == i);
    }
    REQUIRE(NuDatFileOpen(header, "D\\B.BIN", 0) == 0);
    REQUIRE(pc_dat_file_infos[8].is_used == 0);
    for (i = 0; i < 8; ++i) {
        NuFileClose(members[i]);
        REQUIRE(header->open_files[i].info_idx == -1 && pc_dat_file_infos[i].is_used == 0);
    }
    return 0;
}

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
    REQUIRE(exercise_members(header, base_offset) == 0);
    for (i = 0; i < 8; ++i) {
        if (header->open_files[i].dat_file != 0)
            NuFileClose(header->open_files[i].dat_file);
    }
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

static int exercise_memory_file(void) {
    u8 input[4] = {'a', 'b', 'c', 'd'};
    char output[8];

    pc_memfiles[0].buffer = input;
    pc_memfiles[0].end = input + 3;
    pc_memfiles[0].position = input;
    pc_memfiles[0].used = 1;
    REQUIRE(NuFileRead(0x400, output, 8) == 4 && memcmp(output, "abcd", 4) == 0);
    REQUIRE(NuFileRead(0x400, output, 1) == 0);
    REQUIRE(NuFileSeek(0x400, 1, 0) == 1);
    REQUIRE(NuFileRead(0x400, output, 1) == 1 && output[0] == 'b');
    REQUIRE(NuFileSeek(0x400, 1, 1) == 3);
    REQUIRE(NuFileSeek(0x400, 1, 2) == 2);
    REQUIRE(NuFileRead(0x400, output, 2) == 2 && memcmp(output, "cd", 2) == 0);
    NuFileClose(0x400);
    REQUIRE(pc_memfiles[0].used == 0);
    puts("PASS memory-file read seek close");
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
    file_criticalsection = PCThreadCriticalSectionCreate_candidate();
    REQUIRE(file_criticalsection == 1);
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
    REQUIRE(exercise_memory_file() == 0);
    DeleteCriticalSection(&PCThreadCriticalSections_candidate[file_criticalsection]);
    puts("PASS synthetic version -3 DAT parser smoke");
    return 0;
}
