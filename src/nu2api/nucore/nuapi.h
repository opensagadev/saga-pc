/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gamelib/util/CRC16.h"
#include "nu2api/huffdecode.h"
#include "nu2api/nucore/nutime.h"
#endif

typedef struct nuapi_s {
    i32 language;
    i32 region;
    i32 video_mode;
    i32 video_swap_mode;
    i32 video_aspect;
    i32 screen_width;
    i32 screen_height;
    i32 video_is_pal;
    f32 fps;
    i32 frame_begin_count;
    char field12_0x24;
    char field13_0x25;
    char field14_0x26;
    char field15_0x27;
    NuTime time;
    NuTime time2;
    f32 frame_time;
    i32 frame_count;
    f32 forced_frame_time;

    union {
        struct {
            i32 max_fps;
            u8 unknown_004c[0x2c];
        };

        struct {
            FRAME_PAD_RECORD_PC pad_record;
            u8 reserved_pad_record_0070[8];
        };
    };

    void *wind;
    i32 reversible_characters;
    i32 loading_hgobj;
    i32 force_shadows_on_characters;
    i32 frame_end_state;
    f32 video_brightness;
} NUAPI;

STATIC_ASSERT(sizeof(NUAPI) == 0x90);
STATIC_ASSERT(offsetof(NUAPI, language) == 0);
STATIC_ASSERT(offsetof(NUAPI, region) == 4);
STATIC_ASSERT(offsetof(NUAPI, video_mode) == 8);
STATIC_ASSERT(offsetof(NUAPI, screen_width) == 0x14);
STATIC_ASSERT(offsetof(NUAPI, screen_height) == 0x18);
STATIC_ASSERT(offsetof(NUAPI, video_swap_mode) == 0xc);
STATIC_ASSERT(offsetof(NUAPI, video_aspect) == 0x10);
STATIC_ASSERT(offsetof(NUAPI, video_is_pal) == 0x1c);
STATIC_ASSERT(offsetof(NUAPI, fps) == 0x20);
STATIC_ASSERT(offsetof(NUAPI, frame_begin_count) == 0x24);
STATIC_ASSERT(offsetof(NUAPI, time) == 0x2c);
STATIC_ASSERT(offsetof(NUAPI, time2) == 0x34);
STATIC_ASSERT(offsetof(NUAPI, frame_time) == 0x3c);
STATIC_ASSERT(offsetof(NUAPI, frame_count) == 0x40);
STATIC_ASSERT(offsetof(NUAPI, forced_frame_time) == 0x44);
STATIC_ASSERT(offsetof(NUAPI, max_fps) == 0x48);
STATIC_ASSERT(offsetof(NUAPI, unknown_004c) == 0x4c);
STATIC_ASSERT(offsetof(NUAPI, pad_record) == 0x48);
STATIC_ASSERT(offsetof(NUAPI, pad_record.record) == 0x54);
STATIC_ASSERT(offsetof(NUAPI, pad_record.should_end) == 0x60);
STATIC_ASSERT(offsetof(NUAPI, pad_record.end_record_buttons) == 0x64);
STATIC_ASSERT(offsetof(NUAPI, pad_record.end_play_buttons) == 0x68);
STATIC_ASSERT(offsetof(NUAPI, wind) == 0x78);
STATIC_ASSERT(offsetof(NUAPI, frame_end_state) == 0x88);
STATIC_ASSERT(offsetof(NUAPI, reversible_characters) == 0x7c);
STATIC_ASSERT(offsetof(NUAPI, force_shadows_on_characters) == 0x84);
STATIC_ASSERT(offsetof(NUAPI, video_brightness) == 0x8c);
#ifdef __cplusplus

extern CRC16 CRC16InstancePC;
#endif
#ifdef __cplusplus
extern "C" {
#endif
    void NuCommandLine(i32 *__restrict argc, char ***__restrict argv);
    extern NUAPI nuapi;
    void NuFrameBegin(void);
    i32 NuLanguageGet(void);
    void NuLanguageSet(i32 language);
    f32 NuFrameEnd(void);
    void NudxFw_D3DBeginCriticalSection(void);
    void NudxFw_D3DEndCriticalSection(void);
    void NuAPIInit(void);
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(NUAPI, video_aspect) == 0x10);
