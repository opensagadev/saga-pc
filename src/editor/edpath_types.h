#ifndef SAGA_EDITOR_EDPATH_TYPES_H_PART
#define SAGA_EDITOR_EDPATH_TYPES_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

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

struct EDAIPATHROUTE_s {
    char name[16];
    u64 user_mask;
    u32 flags;
    /* 0x18 is a full dword: retail sets and clears bit 0 with dword OR/AND. */
};

STATIC_ASSERT(sizeof(struct EDAIPATHROUTE_s) == 32);

struct EDAIPATHCNX_s {
    struct EDAIPATHNODE_s *node;
    u32 flags;
    u16 route_mask;
    u16 unknown_0a;
};

STATIC_ASSERT(sizeof(struct EDAIPATHCNX_s) == 12);

struct EDAIPATHWALL_s {
    NULISTLNK link;
    u8 unknown_08[2];
    u8 flags;
};

STATIC_ASSERT(sizeof(struct EDAIPATHWALL_s) == 12);
#undef SAGA_EDITOR_EDPATH_TYPES_H_PART
#define SAGA_EDITOR_EDPATH_TYPES_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_EDITOR_EDPATH_TYPES_H_STOP
#endif
#endif

#if SAGA_EDITOR_EDPATH_TYPES_H_PART == 2 && !defined(SAGA_EDITOR_EDPATH_TYPES_H_STOP)

struct EDAIPATH_s {
    NULISTLNK link;
    char name[16];
    NULISTHDR nodes;
    struct EDAIPATHNODE_s *current_node;

    union {
        struct EDAIPATHNODE_s *other_node;
        struct EDAIPATHNODE_s *nearest_node;
    };

    union {
        struct EDAIPATHROUTE_s *current_route;
        u32 unknown_28;
    };

    u32 flags;
    /* 0x2c is a full dword: retail clears bit 0 with a dword AND. */
    i16 runtime_start;
    i16 runtime_end;
    i16 runtime_nearest;
    i16 node_count;
    i8 draw_index; /* PC keeps the draw index at 0x38 (Android: 0x36). */
    u8 unknown_039[7];
    struct EDAIPATHROUTE_s routes[16];
};

STATIC_ASSERT(sizeof(struct EDAIPATH_s) == 576);
STATIC_ASSERT(offsetof(struct EDAIPATH_s, draw_index) == 0x38);
STATIC_ASSERT(offsetof(struct EDAIPATH_s, node_count) == 0x36);
STATIC_ASSERT(offsetof(struct EDAIPATH_s, routes) == 0x40);

struct EDAIPATHNODE_s {
    NULISTLNK link;
    char name[16];
    NUVEC position;
    f32 radius;
    f32 lower_height;
    f32 upper_height;
    struct EDAIPATHCNX_s connections[8];
    i16 index;
    u8 flags;
    u8 unknown_093[3];
    u16 route_mask;
    struct nuhspecial_s special;

    union {
        NUVEC special_position;
        NUVEC platform_position;
    };
    struct EDAISHAREDPATHNODE_s *shared_node;
};

STATIC_ASSERT(sizeof(struct EDAIPATHNODE_s) == 180);

struct EDAISHAREDPATHNODE_s {
    NULISTLNK link;
    i8 reference_count;
    i8 runtime_index;
    u8 draw_flags;
};

STATIC_ASSERT(sizeof(struct EDAISHAREDPATHNODE_s) == 12);
#undef SAGA_EDITOR_EDPATH_TYPES_H_PART
#define SAGA_EDITOR_EDPATH_TYPES_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_EDITOR_EDPATH_TYPES_H_STOP
#endif
#endif

#undef SAGA_EDITOR_EDPATH_TYPES_H_STOP
