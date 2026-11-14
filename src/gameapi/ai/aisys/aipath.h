/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_GAMEAPI_AI_AISYS_AIPATH_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AIPATH_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_GAMEAPI_AI_AISYS_AIPATH_H_PART == 1 && !defined(SAGA_GAMEAPI_AI_AISYS_AIPATH_H_STOP)

typedef struct AIPATHINFO {       // size 0x18
    struct AIPATH *path;          // 0x00
    struct AIPATHCNX *connection; // 0x04
    u8 direction;                 // 0x08
    u8 path_index;                // 0x09
    u16 game_flags;               // 0x0a
    u16 next_check;               // 0x0c

    union { // size 0x2

        struct {        // size 0x2
            u8 flags;   // 0x0e
            u8 padding; // 0x0f
        };

        struct {                   // size 0x2
            u16 on_path : 1;       // 0x0e:0
            u16 was_on_path : 1;   // 0x0e:1
            u16 route_checked : 1; // 0x0e:2
            u16 narrow_path : 1;   // 0x0e:3
        };

        u16 flags_word;
    }; // 0x0e

    f32 distance; // 0x10
    f32 width;    // 0x14
} AIPATHINFO;

/* Not converted: these asserts balance VC8 symbol ids for fragile functions (docs/cleanup-audit.md, section 6). */

enum { AIPATHINFO_FLAG_ON_PATH = 1 };

#undef SAGA_GAMEAPI_AI_AISYS_AIPATH_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AIPATH_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_AI_AISYS_AIPATH_H_STOP
#endif
#endif

#if SAGA_GAMEAPI_AI_AISYS_AIPATH_H_PART == 2 && !defined(SAGA_GAMEAPI_AI_AISYS_AIPATH_H_STOP)

typedef struct AILOCATOR { // size 0x3c
    char name[0x10];       // 0x00
    NUVEC position;        // 0x10

    union { // size 0x4
        i32 rotation;
        u32 flags;
    }; // 0x1c

    AIPATHINFO path_info; // 0x20

    union { // size 0x4
        i32 rotation_offset;
        u32 locator_flags;
    }; // 0x38
} AILOCATOR;

#undef SAGA_GAMEAPI_AI_AISYS_AIPATH_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AIPATH_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_AI_AISYS_AIPATH_H_STOP
#endif
#endif

#undef SAGA_GAMEAPI_AI_AISYS_AIPATH_H_STOP
