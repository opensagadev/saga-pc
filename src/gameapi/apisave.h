#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/menus/screens/gamemenuall.h"

#ifdef __cplusplus
extern "C" {
#endif
    void SaveSystemInitialiseEx(i32, SAVE_HASH_FN, void *, i32, void *, i32, i32, SAVE_VOID_FN);
    void SetSaveSuccessFn(SAVE_VOID_FN);
#ifdef __cplusplus
}
#endif

typedef struct AI_PATH_CONNECTION_TYPE {           // size 0x4c
    u32 connection_flag;                           // 0x00
    void (*draw)(NUVEC *, f32, NUVEC *, u32, i32); // 0x04
    char name[64];                                 // 0x08
    u32 flags;                                     // 0x48
} AI_PATH_CONNECTION_TYPE;

typedef struct FILE_DIRECTORY_RAW_TIME {
    u8 second;
    u8 minute;
    u8 hour;
    u8 day;
    u8 month;
    u8 year_low;
    u8 year_high;
} FILE_DIRECTORY_RAW_TIME;

typedef struct FILE_DIRECTORY_HOST_ENTRY { // size 0x144
    u32 flags;                             // 0x000
    u8 reserved_04[4];                     // 0x004
    u32 size;                              // 0x008
    u8 reserved_0c;                        // 0x00c
    FILE_DIRECTORY_RAW_TIME created;       // 0x00d
    u8 reserved_14;                        // 0x014
    FILE_DIRECTORY_RAW_TIME accessed;      // 0x015
    u8 reserved_1c;                        // 0x01c
    FILE_DIRECTORY_RAW_TIME modified;      // 0x01d
    u32 unused_24;                         // 0x024
    u8 reserved_28[0x18];                  // 0x028
    char name[MAX_PATH];                   // 0x040
} FILE_DIRECTORY_HOST_ENTRY;

typedef struct FILE_DIRECTORY_TIME { // size 0x8
    i8 second;                       // 0x0
    i8 minute;                       // 0x1
    i8 hour;                         // 0x2
    i8 day;                          // 0x3
    i8 month;                        // 0x4
    i8 reserved;                     // 0x5
    i16 year;                        // 0x6
} FILE_DIRECTORY_TIME;

typedef struct FILE_DIRECTORY_ENTRY { // size 0x118
    u32 flags;                        // 0x000
    u32 size;                         // 0x004
    FILE_DIRECTORY_TIME created;      // 0x008
    FILE_DIRECTORY_TIME modified;     // 0x010
    char name[256];                   // 0x018
} FILE_DIRECTORY_ENTRY;
