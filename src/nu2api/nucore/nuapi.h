#pragma once

#include "nu2api/nucore/common.h"

#include "gamelib/util/CRC16.h"
#include "nu2api/huffdecode.h"
#include "nu2api/nucore/nutime.h"

typedef struct nuapi_s {   // size 0x90
    i32 language;          // 0x00
    i32 region;            // 0x04
    i32 video_mode;        // 0x08
    i32 video_swap_mode;   // 0x0c
    i32 video_aspect;      // 0x10
    i32 screen_width;      // 0x14
    i32 screen_height;     // 0x18
    i32 video_is_pal;      // 0x1c
    f32 fps;               // 0x20
    i32 frame_begin_count; // 0x24
    char field12_0x24;     // 0x28
    char field13_0x25;     // 0x29
    char field14_0x26;     // 0x2a
    char field15_0x27;     // 0x2b
    NuTime time;           // 0x2c
    NuTime time2;          // 0x34
    f32 frame_time;        // 0x3c
    i32 frame_count;       // 0x40
    f32 forced_frame_time; // 0x44

    union { // size 0x30

        struct {                   // size 0x30
            i32 max_fps;           // 0x48
            u8 unknown_004c[0x2c]; // 0x4c
        };

        struct {                               // size 0x30
            FRAME_PAD_RECORD frame_pad_record; // 0x48
            u8 reserved_pad_record_0070[8];    // 0x70
        };
    }; // 0x48

    struct nuwind_s *wind;           // 0x78
    i32 reversible_characters;       // 0x7c
    i32 loading_hgobj;               // 0x80
    i32 force_shadows_on_characters; // 0x84
    i32 frame_end_state;             // 0x88
    f32 video_brightness;            // 0x8c
} NUAPI;

#ifdef __cplusplus

extern CRC16 CRC16InstancePC;
#endif
/* NuLanguageGet/NuLanguageSet values. PcNativeSelectLanguage sets them from the Windows language ID
 * (MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK) and the other English variants give NULANGUAGE_ENGLISH,
 * SUBLANG_ENGLISH_US NULANGUAGE_US_ENGLISH); Text_GetLanguagePath picks the text files by them. 19 is not set. */
#define NULANGUAGE_JAPANESE 0
#define NULANGUAGE_ENGLISH 1
#define NULANGUAGE_FRENCH 2
#define NULANGUAGE_SPANISH 3
#define NULANGUAGE_GERMAN 4
#define NULANGUAGE_ITALIAN 5
#define NULANGUAGE_DUTCH 6
#define NULANGUAGE_PORTUGUESE 7
#define NULANGUAGE_DANISH 8
#define NULANGUAGE_GREEK 9
#define NULANGUAGE_CZECH 10
#define NULANGUAGE_POLISH 11
#define NULANGUAGE_RUSSIAN 12
#define NULANGUAGE_KOREAN 13
#define NULANGUAGE_SWEDISH 14
#define NULANGUAGE_NORWEGIAN 15
#define NULANGUAGE_BRAZILIAN_PORTUGUESE 16
#define NULANGUAGE_FINNISH 17
#define NULANGUAGE_US_ENGLISH 18
#define NULANGUAGE_HUNGARIAN 20

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
