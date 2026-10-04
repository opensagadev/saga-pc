#include "saga.h"
#include <stdio.h>
#include <stdlib.h>
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
i32 nufile_blkcnt = 1;
PC_NUFILE_READBLOCK nufile_blkinfo[1024];
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
u8 read_buffer[0x40000];
i32 read_buffer_size;
i32 read_buffer_decoded_size;
u32 PCImplodeWordMask = 0xffffffff;
i32 nufile_lasterror;
i32 nufile_try_packed;
i32 nufile_lsn_allowed = 1;
PC_FILEEXTINFO extensions[64];
i32 num_extensions;
u8 PCUseHiresFlag;
i32 fpk_err;
i32 PCThreadCriticalSectionCount_candidate;
CRITICAL_SECTION PCThreadCriticalSections_candidate[13];
i32 PCFrameCriticalSectionIndex_candidate;
u8 PCFrameLoadingYieldEnabledPC;

/* Independent zeroed parser/string state for this synthetic file-I/O executable. */
NUSTRINGBANK StringBank[3];
i16 CurrentStringBank;
u32 nustring_format = 1;
char *NuBlankString = " ";
u16 **BadWords;
i32 NumBadWords;
const u16 bad_fluff[26] = {0};
u16 bad_equiv[8][2];
NUFPAR_PC NuFParRecords[4];
char NuFParBuffers[4][0x1000];
NUFPAR_PC *NuFParRecordBase;
char *NuFParBufferBase;
i32 old_line_pos;
void(__cdecl *fnInterpreterError)(NUFPAR_PC *);

extern i32 __cdecl PCThreadCriticalSectionCreate_candidate(void);

/* Fixture-only allocation provider; production keeps its real memory manager. */
void *__cdecl NuAlloc(i32 size, const char *file, i32 line) {
    (void)file;
    (void)line;
    if (size <= 0 || size > 0x10000) {
        return NULL;
    }
    return malloc((size_t)size);
}

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
    REQUIRE(NuFileRead(second, payload, 1) == 1 && payload[0] == 'A' && NuFilePos(second) == 1);
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

static int exercise_loaders(NUDATHDR *header) {
    static const char host_payload[8] = {'h', 'o', 's', 't', 0, 'i', 'o', '!'};
    NUDATHDR *previous = curr_dat;
    char output[16];
    VARIPTR buffer, end;
    i32 index;
    void *allocated;

    memset(output, 0xa5, sizeof(output));
    nufile_lasterror = 777;
    REQUIRE(NuDatFileLoadBuffer(header, "D\\A.BIN", output, sizeof(output)) == 4);
    REQUIRE(nufile_lasterror == 0 && memcmp(output, "ABCD", 4) == 0);
    REQUIRE((u8)output[4] == 0xa5);
    REQUIRE(NuDatFileLoadBuffer(header, "D\\A.BIN", output, 3) == 0);
    REQUIRE(nufile_lasterror == -1);
    REQUIRE(NuDatFileLoadBuffer(header, "ABSENT.BIN", output, sizeof(output)) == 0);
    REQUIRE(nufile_lasterror == 0);

    curr_dat = header;
    header->mode = 3;
    REQUIRE(NuFileLoadBuffer("D\\B.BIN", output, sizeof(output)) == 4);
    REQUIRE(nufile_lasterror == 0 && memcmp(output, "wxyz", 4) == 0);
    header->mode = 0;
    buffer.char_ptr = output;
    end.char_ptr = output + sizeof(output);
    REQUIRE(NuFileLoadBufferVP("D\\A.BIN", &buffer, &end) == 4);
    REQUIRE(buffer.char_ptr == output + 4 && memcmp(output, "ABCD", 4) == 0);
    REQUIRE(NuFileLoadBufferVP("D\\B.BIN", &buffer, &end) == 4);
    REQUIRE(buffer.char_ptr == output + 8 && memcmp(output, "ABCDwxyz", 8) == 0);
    end.char_ptr = buffer.char_ptr + 3;
    REQUIRE(NuFileLoadBufferVP("D\\B.BIN", &buffer, &end) == 0);
    REQUIRE(buffer.char_ptr == output + 8 && nufile_lasterror == -1);

    /* The mode-2 decoder is tested separately with a genuine serialized stream. */
    header->file_info[0].compression_mode = 3;
    REQUIRE(NuFileLoadBuffer("D\\B.BIN", output, sizeof(output)) == 4);
    REQUIRE(nufile_lasterror == 0 && memcmp(output, "wxyz", 4) == 0);
    header->file_info[0].compression_mode = 0;

    REQUIRE(NuFileLoadBuffer("HOST.BIN", output, sizeof(output)) == sizeof(host_payload));
    REQUIRE(nufile_lasterror == 0 && memcmp(output, host_payload, sizeof(host_payload)) == 0);
    REQUIRE(NuFileLoadBuffer("HOST.BIN", output, sizeof(host_payload) - 1) == 0);
    REQUIRE(nufile_lasterror == -1);
    REQUIRE(NuFileLoadBuffer("EMPTY.BIN", output, sizeof(output)) == 0);
    REQUIRE(nufile_lasterror == -1);
    REQUIRE(NuFileLoadBuffer("ABSENT.BIN", output, sizeof(output)) == 0);
    REQUIRE(nufile_lasterror == -2);
    nufile_try_packed = 1;
    REQUIRE(NuFileLoadBuffer("HOST.BIN", output, sizeof(output)) == sizeof(host_payload));
    REQUIRE(nufile_lasterror == 0 && memcmp(output, host_payload, sizeof(host_payload)) == 0);
    {
        u8 packed_output[0x120];
        memset(packed_output, 0xa5, sizeof(packed_output));
        REQUIRE(NuFileLoadBuffer("PACKED.BIN", packed_output, sizeof(packed_output)) == 3);
        REQUIRE(nufile_lasterror == 0 && memcmp(packed_output, "ABC", 3) == 0);
        for (index = 3; index < 3 + 256 - 24; ++index) {
            REQUIRE(packed_output[index] == 0xa5);
        }
        for (index = 3 + 256 - 4; index < sizeof(packed_output); ++index) {
            REQUIRE(packed_output[index] == 0xa5);
        }
    }
    nufile_try_packed = 0;
    curr_dat = NULL;
    REQUIRE(NuFileLoadBuffer("HOST.BIN", output, sizeof(output)) == sizeof(host_payload));
    REQUIRE(nufile_lasterror == 0 && memcmp(output, host_payload, sizeof(host_payload)) == 0);
    allocated = NuFileLoad("HOST.BIN");
    REQUIRE(allocated != NULL && memcmp(allocated, host_payload, sizeof(host_payload)) == 0);
    free(allocated);
    curr_dat = previous;

    for (index = 0; index < 20; ++index)
        REQUIRE(pc_dat_file_infos[index].is_used == 0);
    for (index = 0; index < 8; ++index)
        REQUIRE(header->open_files[index].info_idx == -1);
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
    REQUIRE(exercise_loaders(header) == 0);
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

static int exercise_compressed(i32 buffered) {
    static const char expected[16] = "AAAAAAAAAAAABCDE";
    __declspec(align(16)) u8 storage[1024];
    VARIPTR arena;
    NUDATHDR *header;
    NUFILE member;
    i32 size_out, i;
    char output[20];
    datsys_offset = 0;
    nufile_buffering_enabled = buffered;
    arena.void_ptr = storage;
    header = NuDatOpenEx("MODE2.DAT", &arena, &size_out, 0);
    REQUIRE(header != NULL && header->file_info[0].compression_mode == 2);
    member = NuDatFileOpen(header, "D\\B.BIN", 0);
    REQUIRE(member == 0x800 && NuFileOpenSize(member) == 16);
    REQUIRE(unpack_file_info == &pc_dat_file_infos[0] && unpack_file_odi == &header->open_files[0]);
    REQUIRE(unpack_file_pos == 512 && NuFilePos(member) == 0);
    memset(output, 0xa5, sizeof(output));
    REQUIRE(NuFileRead(member, output, 3) == 3 && memcmp(output, expected, 3) == 0);
    REQUIRE((u8)output[3] == 0xa5 && NuFilePos(member) == 19);
    REQUIRE(NuFileRead(member, output + 3, 10) == 10 && memcmp(output, expected, 13) == 0);
    REQUIRE((u8)output[13] == 0xa5 && NuFilePos(member) == 35);
    REQUIRE(NuFileRead(member, output + 13, 3) == 3 && memcmp(output, expected, 16) == 0);
    REQUIRE((u8)output[16] == 0xa5 && NuFilePos(member) == 35);
    NuFileClose(member);
    memset(output, 0xa5, sizeof(output));
    REQUIRE(NuDatFileLoadBuffer(header, "D\\B.BIN", output, 15) == 0 && nufile_lasterror == -1);
    REQUIRE((u8)output[0] == 0xa5 && pc_dat_file_infos[0].is_used == 0);
    REQUIRE(NuDatFileLoadBuffer(header, "D\\B.BIN", output, sizeof(output)) == 16);
    REQUIRE(nufile_lasterror == 0 && memcmp(output, expected, 16) == 0 && (u8)output[16] == 0xa5);
    REQUIRE(pc_dat_file_infos[0].is_used == 0 && header->open_files[0].info_idx == -1);
    for (i = 0; i < 8; ++i) {
        if (header->open_files[i].dat_file != 0)
            NuFileClose(header->open_files[i].dat_file);
    }
    curr_dat = NULL;
    printf("PASS mode-2 block stream buffered=%d\n", buffered);
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

static int exercise_file_pak(i32 version) {
    union {
        u32 alignment;
        u8 bytes[512];
    } storage;
    u8 original[256];
    char *paths[3] = {"PAK0.BIN", "PAK1.BIN", "PAK2.BIN"};
    PC_FILEPAKHDR_V0 *header = (PC_FILEPAKHDR_V0 *)storage.bytes;
    PC_FILEPAKITEM *items;
    void *address;
    i32 size, reclaimed;

    REQUIRE(NuFileLoadBuffer(paths[version], storage.bytes, sizeof(storage.bytes)) == sizeof(original));
    memcpy(original, storage.bytes, sizeof(original));
    REQUIRE(header->type == 0x12345678u + version && header->item_count == 3);
    switch (version) {
        case 0:
            items = (PC_FILEPAKITEM *)(header + 1);
            break;
        case 1:
            items = (PC_FILEPAKITEM *)((PC_FILEPAKHDR_V1 *)header + 1);
            break;
        default:
            items = (PC_FILEPAKITEM *)((PC_FILEPAKHDR *)header + 1);
            break;
    }
    fpk_err = 5;
    REQUIRE(NuFilePakGetLastError() == 5 && fpk_err == 5);
    REQUIRE(NuFilePakGetSize(header) == sizeof(original) && NuFilePakGetLastError() == 0);
    REQUIRE(NuFilePakGetItem(header, "drop") == 1);
    REQUIRE(NuFilePakGetItem(header, "KEEP.BIN") == 2);
    REQUIRE(NuFilePakGetItem(header, "old.bin") == 0);
    REQUIRE(NuFilePakGetItem(header, "ABSENT") == 0 && fpk_err == 0);
    address = storage.bytes;
    size = -777;
    REQUIRE(NuFilePakGetItemInfo(header, 0, &address, &size) == 0);
    REQUIRE(NuFilePakGetItemInfo(header, -1, &address, &size) == 0);
    REQUIRE(NuFilePakGetItemInfo(header, 4, &address, &size) == 0);
    REQUIRE(NuFilePakGetItemInfo(header, 3, &address, &size) == 0);
    REQUIRE(address == storage.bytes && size == -777);
    REQUIRE(NuFilePakGetItemInfo(header, 2, NULL, NULL) == 1);
    REQUIRE(NuFilePakGetItemInfo(header, 2, &address, &size) == 1);
    REQUIRE(address == storage.bytes + 208 && size == 16);
    REQUIRE(memcmp(address, "keep-payload-123", size) == 0);
    REQUIRE(NuFilePakSetItemRequired(header, 0, 1) == 0);
    REQUIRE(NuFilePakSetItemRequired(header, 4, 1) == 0);
    REQUIRE(NuFilePakSetItemRequired(header, 3, 0) == 1 && items[2].flags == 0x83);
    REQUIRE(NuFilePakSetItemRequired(header, 1, 3) == 1 && items[0].flags == 0x81);
    REQUIRE(NuFilePakSetItemRequired(header, 1, 2) == 1 && items[0].flags == 0x80);
    if (version == 1) {
        header->type = 0xdeadbeef;
        REQUIRE(NuFilePakGetItem(header, "KEEP.BIN") == 2);
        header->type = 0x12345679;
    }

    NuFilePakRelocate(storage.bytes, storage.bytes + 4);
    REQUIRE(memcmp(storage.bytes + 4, original, sizeof(original)) == 0);
    REQUIRE(NuFilePakGetItem(storage.bytes + 4, "KEEP.BIN") == 2);
    NuFilePakRelocate(storage.bytes + 4, storage.bytes);
    REQUIRE(memcmp(storage.bytes, original, sizeof(original)) == 0);
    reclaimed = version == 1 ? 0 : 8;
    REQUIRE(NuFilePakCondense(header) == reclaimed);
    REQUIRE(NuFilePakGetSize(header) == sizeof(original) - reclaimed);
    REQUIRE(items[0].flags == 0x82 && items[1].flags == 0x101 && items[2].flags == 0x83);
    REQUIRE(items[1].data_offset == 208 - reclaimed);
    REQUIRE(NuFilePakGetItem(header, "drop") == 0);
    REQUIRE(NuFilePakGetItemInfo(header, 2, &address, &size) == 1 && size == 16);
    REQUIRE(memcmp(address, "keep-payload-123", size) == 0);
    REQUIRE(NuFilePakCondense(header) == 0);
    REQUIRE(NuFilePakGetSize(header) == sizeof(original) - reclaimed);
    header->item_count = 0;
    REQUIRE(NuFilePakGetItem(header, "keep.bin") == 0 && NuFilePakCondense(header) == 0);
    printf("PASS file-pak version=%d\n", version);
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
    memset(pc_memfiles, 0xa5, sizeof(pc_memfiles));
    memset(pc_dat_file_infos, 0xa5, sizeof(pc_dat_file_infos));
    unpack_file_info = pc_dat_file_infos;
    unpack_file_odi = (NUDATOPENFILEINFO *)untouched;
    unpack_file_pos = 123;
    REQUIRE(NuFileInitEx(7, 1, 0) == 7);
    REQUIRE(file_criticalsection == 1);
    REQUIRE(unpack_file_info == NULL && unpack_file_odi == NULL && unpack_file_pos == -1);
    REQUIRE(num_extensions == 63);
    for (i = 0; i < sizeof(pc_memfiles); ++i)
        REQUIRE(((const u8 *)pc_memfiles)[i] == 0);
    for (i = 0; i < sizeof(pc_dat_file_infos); ++i)
        REQUIRE(((const u8 *)pc_dat_file_infos)[i] == 0);
    DeleteCriticalSection(&PCThreadCriticalSections_candidate[file_criticalsection]);
    PCUseHiresFlag = 1;
    NuFileInit(0);
    REQUIRE(file_criticalsection == 2 && num_extensions == 63);
    REQUIRE(NuFileUseHiresPC() == 1);
    for (buffered = 0; buffered < 2; ++buffered) {
        for (i = 0; i < 4; ++i)
            REQUIRE(exercise(paths[i], i == 2 ? 64 : 0, buffered, 1) == 0);
    }
    REQUIRE(exercise("TINY.DAT", 0, 0, 0) == 0);
    for (buffered = 0; buffered < 2; ++buffered)
        REQUIRE(exercise_compressed(buffered) == 0);
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
    for (i = 0; i < 3; ++i)
        REQUIRE(exercise_file_pak(i) == 0);
    DeleteCriticalSection(&PCThreadCriticalSections_candidate[file_criticalsection]);
    puts("PASS synthetic version -3 DAT parser smoke");
    return 0;
}
