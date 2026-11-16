#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nufile/nufile.h"

struct FILE_DIRECTORY_ENTRY;

#ifdef __cplusplus
extern "C" {
#endif

    /* NuFileGetInfo result; the AI editor's file browser reads it. */
    typedef struct AI_EDITOR_FILE_INFO {                             // size 0x118
        u32 flags;                                                   // 0x000
        u32 size;                                                    // 0x004
        i8 created_second;                                           // 0x008
        i8 created_minute;                                           // 0x009
        i8 created_hour;                                             // 0x00a
        i8 created_day;                                              // 0x00b
        i8 created_month;                                            // 0x00c
        u8 unknown_0d;                                               // 0x00d
        i16 created_year;                                            // 0x00e
        i8 second;                                                   // 0x010
        i8 minute;                                                   // 0x011
        i8 hour;                                                     // 0x012
        i8 day;                                                      // 0x013
        i8 month;                                                    // 0x014
        u8 unknown_15;                                               // 0x015
        i16 year;                                                    // 0x016
        char name[256]; /* NuFileGetInfo (PC) only clears name[0] */ // 0x018
    } AI_EDITOR_FILE_INFO;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern struct nufile_device_s host_device;
    extern i32 FileDirectoryHostOpen(char *path);
    i32 NuFileReadDir(i32 file, struct FILE_DIRECTORY_ENTRY *entry);
    i32 SaveCreateDirectory(char *path);
#ifdef __cplusplus
}
#endif
