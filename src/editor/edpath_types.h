/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_EDITOR_EDPATH_TYPES_H_PART
#define SAGA_EDITOR_EDPATH_TYPES_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_EDITOR_EDPATH_TYPES_H_PART == 1 && !defined(SAGA_EDITOR_EDPATH_TYPES_H_STOP)
typedef struct EDAIPATHCNX_s EDAIPATHCNX_s;
typedef struct EDAIPATHNODE_s EDAIPATHNODE_s;
typedef struct EDAIPATHROUTE_s EDAIPATHROUTE_s;
typedef struct EDAIPATHWALL_s EDAIPATHWALL_s;
typedef struct EDAIPATH_s EDAIPATH_s;
typedef struct EDAISHAREDPATHNODE_s EDAISHAREDPATHNODE_s;

struct EDAIPATHROUTE_s { // size 0x20
    char name[16];       // 0x00
    u64 user_mask;       // 0x10
    u32 flags;           // 0x18
    /* 0x18 is a full dword: retail sets and clears bit 0 with dword OR/AND. */
};

struct EDAIPATHCNX_s {           // size 0xc
    struct EDAIPATHNODE_s *node; // 0x0
    u32 flags;                   // 0x4
    u16 route_mask;              // 0x8
    u16 unknown_0a;              // 0xa
};

struct EDAIPATHWALL_s { // size 0xc
    NULISTLNK link;     // 0x0
    u8 unknown_08[2];   // 0x8
    u8 flags;           // 0xa
};

#undef SAGA_EDITOR_EDPATH_TYPES_H_PART
#define SAGA_EDITOR_EDPATH_TYPES_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_EDITOR_EDPATH_TYPES_H_STOP
#endif
#endif

#if SAGA_EDITOR_EDPATH_TYPES_H_PART == 2 && !defined(SAGA_EDITOR_EDPATH_TYPES_H_STOP)

struct EDAIPATH_s {                      // size 0x240
    NULISTLNK link;                      // 0x000
    char name[16];                       // 0x008
    NULISTHDR nodes;                     // 0x018
    struct EDAIPATHNODE_s *current_node; // 0x020

    union { // size 0x4
        struct EDAIPATHNODE_s *other_node;
        struct EDAIPATHNODE_s *nearest_node;
    }; // 0x024

    union { // size 0x4
        struct EDAIPATHROUTE_s *current_route;
        u32 unknown_28;
    }; // 0x028

    u32 flags; // 0x02c
    /* 0x2c is a full dword: retail clears bit 0 with a dword AND. */
    i16 runtime_start;                                    // 0x030
    i16 runtime_end;                                      // 0x032
    i16 runtime_nearest;                                  // 0x034
    i16 node_count;                                       // 0x036
    i8 draw_index; /* PC keeps the draw index at 0x38. */ // 0x038
    u8 unknown_039[7];                                    // 0x039
    struct EDAIPATHROUTE_s routes[16];                    // 0x040
};

struct EDAIPATHNODE_s {                  // size 0xb4
    NULISTLNK link;                      // 0x00
    char name[16];                       // 0x08
    NUVEC position;                      // 0x18
    f32 radius;                          // 0x24
    f32 lower_height;                    // 0x28
    f32 upper_height;                    // 0x2c
    struct EDAIPATHCNX_s connections[8]; // 0x30
    i16 index;                           // 0x90
    u8 flags;                            // 0x92
    u8 unknown_093[3];                   // 0x93
    u16 route_mask;                      // 0x96
    struct nuhspecial_s special;         // 0x98

    union { // size 0xc
        NUVEC special_position;
        NUVEC platform_position;
    }; // 0xa4
    struct EDAISHAREDPATHNODE_s *shared_node; // 0xb0
};

struct EDAISHAREDPATHNODE_s { // size 0xc
    NULISTLNK link;           // 0x0
    i8 reference_count;       // 0x8
    i8 runtime_index;         // 0x9
    u8 draw_flags;            // 0xa
};

#undef SAGA_EDITOR_EDPATH_TYPES_H_PART
#define SAGA_EDITOR_EDPATH_TYPES_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_EDITOR_EDPATH_TYPES_H_STOP
#endif
#endif

#undef SAGA_EDITOR_EDPATH_TYPES_H_STOP
