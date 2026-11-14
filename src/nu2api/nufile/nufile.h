/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

struct AI_EDITOR_FILE_INFO_PC;

typedef struct nudathdr_s NUDATHDR;

typedef i32 NUFILE;

typedef struct nudatfinfo_s {
    i32 file_offset;
    i32 file_len;
    i32 decompressed_len;
    i32 compression_mode;
} NUDATFINFO;

STATIC_ASSERT(sizeof(NUDATFINFO) == 0x10);

typedef struct nudfnode_s {
    i16 child_idx;
    i16 sibling_idx;
    char *name;
} NUDFNODE;

STATIC_ASSERT(sizeof(NUDFNODE) == 8);

typedef struct nudatopenfileinfo_s {
    NUFILE dat_file;
    i32 info_idx;
    i64 position;
} NUDATOPENFILEINFO;

STATIC_ASSERT(sizeof(NUDATOPENFILEINFO) == 0x10);
STATIC_ASSERT(offsetof(NUDATOPENFILEINFO, position) == 8);
STATIC_ASSERT(offsetof(NUDATOPENFILEINFO, dat_file) == 0);
STATIC_ASSERT(offsetof(NUDATOPENFILEINFO, info_idx) == 4);

struct nudathdr_s {
    i32 version;
    i32 file_count;
    NUDATFINFO *file_info;
    i32 tree_node_count;
    NUDFNODE *file_tree;
    i32 leaf_names_len;
    char *leaf_names;
    u32 *hash_idxs;
    i32 hash_count;
    i32 hashes_len;
    char *hashes;
    NUDATOPENFILEINFO open_files[8];
    i16 unknown_b0;
    i16 mode;
    i32 unknown_b4;
    char *name;
};

STATIC_ASSERT(sizeof(NUDATHDR) == 0xc0);
STATIC_ASSERT(offsetof(NUDATHDR, open_files) == 0x30);
STATIC_ASSERT(offsetof(NUDATHDR, unknown_b0) == 0xb0);
STATIC_ASSERT(offsetof(NUDATHDR, mode) == 0xb2);
STATIC_ASSERT(offsetof(NUDATHDR, name) == 0xb8);

typedef struct fileinfo_s FILEINFO;

typedef struct filebuff_s {
    FILEINFO *file_info;
    i32 time;
    char data[1024];
} FILEBUFF;

STATIC_ASSERT(sizeof(FILEBUFF) == 0x408);

struct fileinfo_s {
    i32 handle;
    i64 read_pos;
    i64 file_pos;
    i64 file_len;
    i64 buf_start;
    i32 buf_len;
    i32 use_buf;
    i32 unused;
    i32 mode;
    FILEBUFF *buf;
};

STATIC_ASSERT(sizeof(FILEINFO) == 0x40);
STATIC_ASSERT(offsetof(FILEINFO, read_pos) == 8);
STATIC_ASSERT(offsetof(FILEINFO, file_pos) == 0x10);
STATIC_ASSERT(offsetof(FILEINFO, file_len) == 0x18);
STATIC_ASSERT(offsetof(FILEINFO, buf_start) == 0x20);
STATIC_ASSERT(offsetof(FILEINFO, buf_len) == 0x28);
STATIC_ASSERT(offsetof(FILEINFO, use_buf) == 0x2c);
STATIC_ASSERT(offsetof(FILEINFO, mode) == 0x34);
STATIC_ASSERT(offsetof(FILEINFO, buf) == 0x38);

typedef struct nufile_device_s NUFILE_DEVICE;

struct nufile_device_s {
    i32 id;
    i32 unit;
    i32 attr;
    i32 status;
    i32 dir_separator;
    i32 max_name_size;
    i32 max_extension_size;
    i32 max_directory_size;
    i32 free_space;
    i32 params[4];
    char name[0x10];
    i32 match_len;
    i32 name_len;
    char root[0x20];
    char description[0x40];
    char cur_dir[0x80];
    char sys_dir[0x80];
    char dll_dir[0x80];
    i32 (*format_name_fn)(NUFILE_DEVICE *, char *, char *, i32);
    i32 (*interrogate_fn)(NUFILE_DEVICE *);
};

STATIC_ASSERT(sizeof(NUFILE_DEVICE) == 0x234);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, id) == 0);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, unit) == 4);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, dir_separator) == 0x10);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, name) == 0x34);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, match_len) == 0x44);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, name_len) == 0x48);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, root) == 0x4c);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, description) == 0x6c);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, cur_dir) == 0xac);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, sys_dir) == 0x12c);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, dll_dir) == 0x1ac);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, format_name_fn) == 0x22c);
STATIC_ASSERT(offsetof(NUFILE_DEVICE, interrogate_fn) == 0x230);
#ifdef __cplusplus
extern "C" {
#endif
    NUDATHDR *NuDatSet(NUDATHDR *header);
    NUDATHDR *NuDatGet(void);
    i32 NuDatFileFindHash(NUDATHDR *header, char *name);
    i32 NuDatFileFindTree(NUDATHDR *header, char *name);
    i32 NuFileExistQuiet(char *filepath);
    i32 NuFileExists(char *filepath);
    i32 NuFileGetInfo(char *path, struct AI_EDITOR_FILE_INFO_PC *info);
    f32 NuFileReadFloat(NUFILE file);
    i32 NuFileReadInt(NUFILE file);
    i8 NuFileReadChar(NUFILE file);
    u16 NuFileReadWChar(NUFILE file);
    NUDATHDR *NuDatOpenEx(const char *filepath, VARIPTR *buf, i32 *size_out, i16 mode);
    NUDATHDR *NuDatOpen(const char *filepath, VARIPTR *buf, i32 *size_out);
    i64 NuDatCalcPos(NUDATHDR *header, i32 offset);
    i32 NuDatFileGetFreeInfo(void);
    i32 NuDatFileGetFreeHandleIX(NUDATHDR *header, i32 info_index);
    NUFILE NuDatFileOpen(NUDATHDR *header, char *filepath, i32 mode);
    i32 NuDatFileRead(NUFILE file, void *buffer, i32 size);
    i32 NuDatFileLoadBuffer(NUDATHDR *header, char *filepath, void *buffer, i32 capacity);
    NUFILE NuMemFileOpen(void *buffer, i32 buffer_size, i32 mode);
    void *NuMemFileAddr(NUFILE file);
    i32 NuMemFileWrite(NUFILE file, const void *data, i32 size);
    i32 NuMemFileRead(NUFILE file, void *buffer, i32 size);
    i64 NuMemFileSeek(NUFILE file, i64 offset, i32 origin);
    void NuMemFileClose(NUFILE file);
    i32 NuPSFileOpen(const char *filepath, i32 mode);
    i32 NuPSFileClose(i32 handle);
    i32 NuPSFileRead(i32 handle, void *buffer, i32 size);
    i32 NuPSFileWrite(i32 handle, const void *buffer, i32 size);
    i64 NuPSFileLSeek(i32 handle, i64 position, i32 origin);
    extern NUFILE_DEVICE *default_device;
    NUFILE_DEVICE *NuFileGetDeviceFromPath(const char *path);
    void NuFileGetCurrentDirectory(char *destination);
    void NuFileSetCurrentDirectory(const char *path);
    void NuFileSetCurrentSysDirectory(char *path);
    void NuFileGetCurrentDllPath(char *destination);
    void NuFileSetCurrentDllDirectory(const char *path);
    i32 DEV_FormatName(NUFILE_DEVICE *device, char *destination, char *source, i32 capacity);
    i32 DEVHOST_Interrogate(NUFILE_DEVICE *device);
    NUFILE NuFileOpen(const char *, i32);
    NUFILE NuFileOpenDF(const char *filepath, i32 mode, NUDATHDR *header, i32 flags);
    void NuFileClose(NUFILE file);
    i64 NuFileSeek(NUFILE file, i64 offset, i32 origin);
    i64 NuMemFilePos(NUFILE file);
    i64 NuFilePos(NUFILE file);
    i64 NuFileSize(char *filepath);
    i64 NuFileOpenSize(NUFILE file);
    i32 NuFileBeginBlkRead(NUFILE file, i32 expected_tag);
    i32 NuFileAlignRead(NUFILE file, i32 mask);
    void NuFileEndBlkRead(NUFILE file);
    i32 NuFileGetBlkSize(void);
    i64 NuDatFileSeek(NUFILE file, i64 offset, i32 origin);
    void NuDatFileClose(NUFILE file);
    i32 NuDatFileOpenSize(NUFILE file);
    i32 NuMcFileOpenSize(NUFILE file);
    i32 NuMcOpen(i32 port, i32 slot, char *filepath, i32 mode, i32 async);
    i32 NuMcRead(i32 file, void *destination, i32 size, i32 async);
    i32 NuMcClose(i32 file, i32 async);
    i32 NuMcSeek(i32 file, i32 offset, i32 origin, i32 async);
    i32 NuFileRead(NUFILE file, void *buffer, i32 size);
    i32 NuFileLoadBuffer(char *filepath, void *buffer, i32 capacity);
    i32 NuFileLoadBufferVP(char *filepath, VARIPTR *current, VARIPTR *end);
    i32 NuFileStatus(NUFILE file);
    i32 NuMcWrite(i32 file, const void *buffer, i32 size, i32 async);
    i32 NuFileWrite(NUFILE file, const void *data, i32 size);
    i32 NuDatGetFileInfo(NUDATHDR *, char *, i64 *, i32 *);
    i32 NuFileInitEx(i32 device_id, i32 reboot_iop, i32 eject);
    void NuFileInit(i32 device_id);
    void NuPSFileInitDevices(i32 device_id, i32 reboot_iop, i32 eject);
    i32 NuFileExtGetType(char *path, i32 path_len);
    i32 NuFileExtRemove(char *destination, char *path);
    i32 NuFileExtGetExt(char *destination, i32 capacity, i32 type);
    i32 NuFileExtConvert(char *destination, char *path);
    void NuPPUnpack(void *, void *);
    i32 NuPPLoadBuffer(NUFILE file, void *buffer, i32 capacity);
    extern const char AITransferLiteral757a94[9];
    void *NuFileLoad(char *);
    i32 NuFileGetLastError(void);
    NUFILE_DEVICE *NuFileFindDevice(i32, i32);
    i32 NuFileWriteInt(NUFILE file, i32 value);
    u32 NuFileWriteUnsignedInt(NUFILE file, u32 value);
    extern i32 NuFile_SwapEndianOnWrite;
#ifdef __cplusplus
}
#endif
typedef struct nufile_info_s nufile_info_s;

struct nufile_info_s {
    u8 unknown_00[16];
    i8 second;
    i8 minute;
    i8 hour;
    i8 day;
    i8 month;
    u8 unknown_15;
    i16 year;
};

STATIC_ASSERT(sizeof(struct nufile_info_s) == 24);
#ifdef __cplusplus
extern "C" {
#endif
    void NuFileUpCase(struct nufile_device_s *device, char *filepath);
    void NuFileCloseDir(i32 file);
    i32 NuFileOpenDir(char *path);
#ifdef __cplusplus
}
#endif

enum { NUFILE_READ = 0 };

enum { NUFILE_WRITE = 1 };
