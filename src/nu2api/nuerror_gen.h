#pragma once

#include "nu2api/nucore/common.h"

typedef struct PC_NATIVE_DEBUG_HEADER { // size 0xa4
    i32 first;                          // 0x00
    i32 second;                         // 0x04
    i32 third;                          // 0x08
    i32 font_size;                      // 0x0c
    char label[128];                    // 0x10
    u32 unknown_colour;                 // 0x90
    u32 face_colour;                    // 0x94
    u32 high_colour;                    // 0x98
    u32 low_colour;                     // 0x9c
    u32 inner_colour;                   // 0xa0
} PC_NATIVE_DEBUG_HEADER;

typedef struct PC_NATIVE_DEBUG_VERTEX { // size 0x14
    f32 x;                              // 0x00
    f32 y;                              // 0x04
    f32 z;                              // 0x08
    f32 w;                              // 0x0c
    u32 colour;                         // 0x10
} PC_NATIVE_DEBUG_VERTEX;

typedef struct PC_NATIVE_DEBUG_STATE {      // size 0x508
    u8 initialised;                         // 0x000
    u8 enabled;                             // 0x001
    u8 unknown_002[2];                      // 0x002
    i32 unknown_004;                        // 0x004
    i32 unknown_008;                        // 0x008
    i32 unknown_00c;                        // 0x00c
    PC_NATIVE_DEBUG_HEADER header;          // 0x010
    PC_NATIVE_DEBUG_HEADER *current_header; // 0x0b4
    f32 top;                                // 0x0b8
    f32 left;                               // 0x0bc
    f32 right;                              // 0x0c0
    f32 bottom;                             // 0x0c4
    PC_NATIVE_DEBUG_VERTEX vertices[54];    // 0x0c8
    i32 log_file;                           // 0x500
    u8 write_log;                           // 0x504
    u8 unknown_505[3];                      // 0x505
#ifdef __cplusplus
    PC_NATIVE_DEBUG_STATE();
    ~PC_NATIVE_DEBUG_STATE();
    void InitializeHeader();
    void UpdateGeometry();
    void Initialize();
    void Shutdown();
    PC_NATIVE_DEBUG_STATE *fun_006dff70();
    void fun_006dffa0(u8 value);
#endif
} PC_NATIVE_DEBUG_STATE;

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 bHaveErr;
    void PCNativeDebugPrint(PC_NATIVE_DEBUG_STATE *state, const char *format, ...);
    extern char PCWarningFormatBuffer[];
    extern char PCWarningOutputBuffer[];
    extern char PCWarningMessages[];
    extern void (*PCWarningHandler)(char *);
    extern i32 part_debris_count;
    extern i32 DisableDebugMsg;
    extern char err_msg_filter[32];
    extern char *nufile;
    extern i32 nuline;
    extern void (*nuerror_handler)(char *arg0);
    extern void (*nuwarning_handler)(char *arg0);
    void PCNativeErrorPrint(PC_NATIVE_DEBUG_STATE *state, const char *format, ...);
    void PCNativeWarningPrint(PC_NATIVE_DEBUG_STATE *state, const char *format, ...);
#ifdef __cplusplus
}
#endif
