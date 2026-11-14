#ifndef SAGA_GAMEAPI_AI_AISYS_AIPATH_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AIPATH_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nucore/common.h, nu2api/nucore/fixed_width.h, nu2api/numath/nuvec.h (not all
 * of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"
#endif
/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_GAMEAPI_AI_AISYS_AIPATH_H_PART == 1 && !defined(SAGA_GAMEAPI_AI_AISYS_AIPATH_H_STOP)

typedef struct AIPATHINFO_PC {
    struct AIPATH_PC *path;
    struct AIPATHCNX_PC *connection;
    u8 direction;
    u8 path_index;
    u16 game_flags;
    u16 next_check;

    union {
        struct {
            u8 flags;
            u8 padding;
        };

        struct {
            u16 on_path : 1;
            u16 was_on_path : 1;
            u16 route_checked : 1;
            u16 narrow_path : 1;
        };

        u16 flags_word;
    };

    f32 distance;
    f32 width;
} AIPATHINFO_PC;

/* Not converted: these asserts balance VC8 symbol ids for fragile functions (docs/cleanup-audit.md, section 6). */
STATIC_ASSERT(sizeof(AIPATHINFO_PC) == 0x18);
STATIC_ASSERT(offsetof(AIPATHINFO_PC, path) == 0);
STATIC_ASSERT(offsetof(AIPATHINFO_PC, connection) == 4);
STATIC_ASSERT(offsetof(AIPATHINFO_PC, direction) == 8);
STATIC_ASSERT(offsetof(AIPATHINFO_PC, path_index) == 9);
STATIC_ASSERT(offsetof(AIPATHINFO_PC, game_flags) == 0xa);
STATIC_ASSERT(offsetof(AIPATHINFO_PC, next_check) == 0xc);
STATIC_ASSERT(offsetof(AIPATHINFO_PC, width) == 0x14);
STATIC_ASSERT(offsetof(AIPATHINFO_PC, flags) == 0x0e);
STATIC_ASSERT(offsetof(AIPATHINFO_PC, distance) == 0x10);

STATIC_ASSERT(offsetof(AIPATHINFO_PC, flags_word) == 0xe);

enum { AIPATHINFO_FLAG_ON_PATH = 1 };

#undef SAGA_GAMEAPI_AI_AISYS_AIPATH_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AIPATH_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_AI_AISYS_AIPATH_H_STOP
#endif
#endif

#if SAGA_GAMEAPI_AI_AISYS_AIPATH_H_PART == 2 && !defined(SAGA_GAMEAPI_AI_AISYS_AIPATH_H_STOP)

typedef struct AILOCATOR_PC {
    char name[0x10]; // 0x00
    NUVEC position;  // 0x10

    union {           // 0x1c
        i32 rotation; // 0x1c
        u32 flags;    // 0x1c
    };

    AIPATHINFO_PC path_info; // 0x20

    union {                  // 0x38
        i32 rotation_offset; // 0x38
        u32 locator_flags;   // 0x38
    };
} AILOCATOR_PC; // size 0x3c

#undef SAGA_GAMEAPI_AI_AISYS_AIPATH_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AIPATH_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_AI_AISYS_AIPATH_H_STOP
#endif
#endif

#undef SAGA_GAMEAPI_AI_AISYS_AIPATH_H_STOP
