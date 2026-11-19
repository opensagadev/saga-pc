#pragma once

#include "nu2api/nucore/common.h"

struct AI_EDITOR_FILE_INFO;
struct FILE_DIRECTORY_ENTRY;

typedef struct nudathdr_s NUDATHDR;

typedef i32 NUFILE;

typedef struct nudatfinfo_s { // size 0x10
    i32 file_offset;          // 0x0
    i32 file_len;             // 0x4
    i32 decompressed_len;     // 0x8
    i32 compression_mode;     // 0xc
} NUDATFINFO;

typedef struct nudfnode_s { // size 0x8
    i16 child_idx;          // 0x0
    i16 sibling_idx;        // 0x2
    char *name;             // 0x4
} NUDFNODE;

typedef struct nudatopenfileinfo_s { // size 0x10
    NUFILE dat_file;                 // 0x0
    i32 info_idx;                    // 0x4
    i64 position;                    // 0x8
} NUDATOPENFILEINFO;

struct nudathdr_s {                  // size 0xc0
    i32 version;                     // 0x00
    i32 file_count;                  // 0x04
    NUDATFINFO *file_info;           // 0x08
    i32 tree_node_count;             // 0x0c
    NUDFNODE *file_tree;             // 0x10
    i32 leaf_names_len;              // 0x14
    char *leaf_names;                // 0x18
    u32 *hash_idxs;                  // 0x1c
    i32 hash_count;                  // 0x20
    i32 hashes_len;                  // 0x24
    char *hashes;                    // 0x28
    NUDATOPENFILEINFO open_files[8]; // 0x30
    i16 unknown_b0;                  // 0xb0
    i16 mode;                        // 0xb2
    i32 start_lsn;                   // 0xb4
    char *name;                      // 0xb8
};

typedef struct fileinfo_s FILEINFO;

typedef struct filebuff_s { // size 0x408
    FILEINFO *file_info;    // 0x000
    i32 time;               // 0x004
    char data[1024];        // 0x008
} FILEBUFF;

struct fileinfo_s { // size 0x40
    i32 handle;     // 0x00
    i64 read_pos;   // 0x08
    i64 file_pos;   // 0x10
    i64 file_len;   // 0x18
    i64 buf_start;  // 0x20
    i32 buf_len;    // 0x28
    i32 use_buf;    // 0x2c
    i32 unused;     // 0x30
    i32 mode;       // 0x34
    FILEBUFF *buf;  // 0x38
};

typedef struct nufile_device_s NUFILE_DEVICE;

struct nufile_device_s {                                         // size 0x234
    i32 id;                                                      // 0x000
    i32 unit;                                                    // 0x004
    i32 attr;                                                    // 0x008
    i32 status;                                                  // 0x00c
    i32 dir_separator;                                           // 0x010
    i32 max_name_size;                                           // 0x014
    i32 max_extension_size;                                      // 0x018
    i32 max_directory_size;                                      // 0x01c
    i32 free_space;                                              // 0x020
    i32 params[4];                                               // 0x024
    char name[0x10];                                             // 0x034
    i32 match_len;                                               // 0x044
    i32 name_len;                                                // 0x048
    char root[0x20];                                             // 0x04c
    char description[0x40];                                      // 0x06c
    char cur_dir[0x80];                                          // 0x0ac
    char sys_dir[0x80];                                          // 0x12c
    char dll_dir[0x80];                                          // 0x1ac
    i32 (*format_name_fn)(NUFILE_DEVICE *, char *, char *, i32); // 0x22c
    i32 (*interrogate_fn)(NUFILE_DEVICE *);                      // 0x230
};

/* PC_FILEEXTINFO.type: the kind of file, by the PC extension NuFileExtInit registers for it; .tex, .pnt and .dds
 * are the platforms' textures, .wav, .vag, .msf their sounds, .bik, .pss, .wmv their videos. */
#define NUFILEEXT_AN3 0
#define NUFILEEXT_BSA 1
#define NUFILEEXT_GHG 2
#define NUFILEEXT_GSC 3
#define NUFILEEXT_TEXTURE 4
#define NUFILEEXT_CU2 5
#define NUFILEEXT_SOUND 6
#define NUFILEEXT_MIB 7
#define NUFILEEXT_VIDEO 8

/* PC_FILEEXTINFO.platform: the platform of each extension set (_ps3, _360 and _pc in the names; .vag/.pss, .dsp/.h4m,
 * .at3 and .wavx/.wmv in the others). NuFileExtInit converts any other platform's extension to the PC one. */
#define NUFILEEXT_PLATFORM_PS2 0
#define NUFILEEXT_PLATFORM_XBOX 1
#define NUFILEEXT_PLATFORM_GAMECUBE 2
#define NUFILEEXT_PLATFORM_PSP 3
#define NUFILEEXT_PLATFORM_WINDOWS 4
#define NUFILEEXT_PLATFORM_PS3 5
#define NUFILEEXT_PLATFORM_XBOX360 6

#ifdef __cplusplus
extern "C" {
#endif
    NUDATHDR *NuDatSet(NUDATHDR *header);
    NUDATHDR *NuDatGet(void);
    i32 NuDatFileFindHash(NUDATHDR *header, char *name);
    i32 NuDatFileFindTree(NUDATHDR *header, char *name);
    i32 NuFileExistQuiet(char *filepath);
    i32 NuFileExists(char *filepath);
    i32 NuFileGetInfo(char *path, struct AI_EDITOR_FILE_INFO *info);
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

struct nufile_info_s { // size 0x18
    u8 unknown_00[16]; // 0x00
    i8 second;         // 0x10
    i8 minute;         // 0x11
    i8 hour;           // 0x12
    i8 day;            // 0x13
    i8 month;          // 0x14
    u8 unknown_15;     // 0x15
    i16 year;          // 0x16
};

#ifdef __cplusplus
extern "C" {
#endif
    void NuFileUpCase(struct nufile_device_s *device, char *filepath);
    void NuFileCloseDir(i32 file);
    i32 NuFileOpenDir(char *path);
    i32 NuFileAlign(NUFILE file, i32 mask);
    extern void NuFileReldirFix(NUFILE_DEVICE *device, char *path);
    i32 NuFileCopy(char *destination, char *source);
    i32 NuMcCheckCardFormatted(i32 port, i32 slot);
    i32 NuMcCheckCardFreeSpace(i32 port, i32 slot);
    i32 NuMcCheckCardPresent(i32 port, i32 slot);
    i32 NuMcFormat(i32 port, i32 slot);
    void NuFileWriteString(NUFILE file, const char *text);
    extern i32 FileDirectoryMcOpen(i32 port, i32 slot, char *path);
    extern i32 FileDirectoryMcRead(i32 file, struct FILE_DIRECTORY_ENTRY *entry);
    extern void FileDirectoryMcClose(i32 file);
#ifdef __cplusplus
}
#endif

enum { NUFILE_READ = 0 };

enum { NUFILE_WRITE = 1 };

/* NuPSFileOpen opens it for writing without truncating it (OPEN_ALWAYS) */
#define NUFILE_APPEND 2

/* a read whose seeks and reads complete in the background: the dat file code waits on NuFileStatus after each */
#define NUFILE_READ_ASYNC 3
