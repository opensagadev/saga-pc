/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

typedef struct PC_NATIVE_DEBUG_HEADER_PC {
    i32 first;
    i32 second;
    i32 third;
    i32 font_size;
    char label[128];
    u32 unknown_colour;
    u32 face_colour;
    u32 high_colour;
    u32 low_colour;
    u32 inner_colour;
} PC_NATIVE_DEBUG_HEADER_PC;

typedef struct PC_NATIVE_DEBUG_VERTEX_PC {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
    u32 colour;
} PC_NATIVE_DEBUG_VERTEX_PC;

typedef struct PC_NATIVE_DEBUG_STATE_PC {
    u8 unknown_0;
    u8 enabled;
    u8 unknown_002[2];
    i32 unknown_004;
    i32 unknown_008;
    i32 unknown_00c;
    PC_NATIVE_DEBUG_HEADER_PC header;
    PC_NATIVE_DEBUG_HEADER_PC *current_header;
    f32 top;
    f32 left;
    f32 right;
    f32 bottom;
    PC_NATIVE_DEBUG_VERTEX_PC vertices[54];
    i32 log_file;
    u8 write_log;
    u8 unknown_505[3];
#ifdef __cplusplus
    PC_NATIVE_DEBUG_STATE_PC();
    ~PC_NATIVE_DEBUG_STATE_PC();
    void InitializeHeader();
    void UpdateGeometry();
    void Initialize();
    void Shutdown();
    PC_NATIVE_DEBUG_STATE_PC *fun_006dff70();
    void fun_006dffa0(u8 value);
#endif
} PC_NATIVE_DEBUG_STATE_PC;

STATIC_ASSERT(sizeof(PC_NATIVE_DEBUG_HEADER_PC) == 0xa4);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_HEADER_PC, first) == 0);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_HEADER_PC, second) == 4);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_HEADER_PC, third) == 8);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_HEADER_PC, font_size) == 0xc);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_HEADER_PC, label) == 0x10);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_HEADER_PC, unknown_colour) == 0x90);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_HEADER_PC, face_colour) == 0x94);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_HEADER_PC, high_colour) == 0x98);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_HEADER_PC, low_colour) == 0x9c);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_HEADER_PC, inner_colour) == 0xa0);
STATIC_ASSERT(sizeof(PC_NATIVE_DEBUG_VERTEX_PC) == 20);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_VERTEX_PC, x) == 0);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_VERTEX_PC, y) == 4);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_VERTEX_PC, z) == 8);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_VERTEX_PC, w) == 12);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_VERTEX_PC, colour) == 16);
STATIC_ASSERT(sizeof(PC_NATIVE_DEBUG_STATE_PC) == 0x508);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_STATE_PC, unknown_0) == 0);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_STATE_PC, enabled) == 1);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_STATE_PC, header) == 0x10);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_STATE_PC, current_header) == 0xb4);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_STATE_PC, top) == 0xb8);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_STATE_PC, left) == 0xbc);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_STATE_PC, right) == 0xc0);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_STATE_PC, bottom) == 0xc4);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_STATE_PC, vertices) == 0xc8);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_STATE_PC, log_file) == 0x500);
STATIC_ASSERT(offsetof(PC_NATIVE_DEBUG_STATE_PC, write_log) == 0x504);

#define PlayerCarryRotationScalePc TerrainSkinSampleCountPc // NOLINT(readability-identifier-naming)

#define PCWarningPrefix NuBlankStringValue            // NOLINT(readability-identifier-naming)
#define PCWarningNativeFormat GizAIMessage_NameFormat // NOLINT(readability-identifier-naming)
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 bHaveErr;
    void PCNativeDebugPrint(PC_NATIVE_DEBUG_STATE_PC *state, const char *format, ...);
    extern char PCWarningFormatBuffer[];
    extern char PCWarningOutputBuffer[];
    extern char PCWarningMessages[];
    extern void (*PCWarningHandler)(char *);
    extern const char PCWarningSuffix[];
    extern i32 part_debris_count;
    extern i32 DisableDebugMsg;
    extern char err_msg_filter[32];
    extern char *nufile;
    extern i32 nuline;
    extern void (*nuerror_handler)(char *arg0);
    extern void (*nuwarning_handler)(char *arg0);
#ifdef __cplusplus
}
#endif
