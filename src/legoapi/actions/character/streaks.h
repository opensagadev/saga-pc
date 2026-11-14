/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct STREAK_NODE_PC {
        struct STREAK_NODE_PC *next;
        struct STREAK_NODE_PC *prev;
        NUVEC position;
        NUVEC previous_position;
        NUVEC start_tangent;
        NUVEC end_tangent;
        NUVEC positions[7];
        NUVEC tangents[7];
        i32 segment_count;
        f32 remaining_time;
    } STREAK_NODE_PC;

    typedef struct STREAK_HEADER_PC {
        struct STREAK_HEADER_PC *next;
        struct STREAK_HEADER_PC *prev;
        STREAK_NODE_PC *nodes;
        u16 index;
        u8 flags;
        u8 unknown_0f;
        i32 has_new_node;
        void **owner_slot;
        u32 colour;
        GAMEOBJECT *owner;
    } STREAK_HEADER_PC;

    STATIC_ASSERT(sizeof(STREAK_NODE_PC) == 0xe8);
    STATIC_ASSERT(offsetof(STREAK_NODE_PC, next) == 0x0);
    STATIC_ASSERT(offsetof(STREAK_NODE_PC, prev) == 0x4);
    STATIC_ASSERT(offsetof(STREAK_NODE_PC, position) == 0x8);
    STATIC_ASSERT(offsetof(STREAK_NODE_PC, previous_position) == 0x14);
    STATIC_ASSERT(offsetof(STREAK_NODE_PC, start_tangent) == 0x20);
    STATIC_ASSERT(offsetof(STREAK_NODE_PC, end_tangent) == 0x2c);
    STATIC_ASSERT(offsetof(STREAK_NODE_PC, positions) == 0x38);
    STATIC_ASSERT(offsetof(STREAK_NODE_PC, tangents) == 0x8c);
    STATIC_ASSERT(offsetof(STREAK_NODE_PC, segment_count) == 0xe0);
    STATIC_ASSERT(offsetof(STREAK_NODE_PC, remaining_time) == 0xe4);
    STATIC_ASSERT(sizeof(STREAK_HEADER_PC) == 0x20);
    STATIC_ASSERT(offsetof(STREAK_HEADER_PC, next) == 0x0);
    STATIC_ASSERT(offsetof(STREAK_HEADER_PC, prev) == 0x4);
    STATIC_ASSERT(offsetof(STREAK_HEADER_PC, nodes) == 0x8);
    STATIC_ASSERT(offsetof(STREAK_HEADER_PC, index) == 0xc);
    STATIC_ASSERT(offsetof(STREAK_HEADER_PC, flags) == 0xe);
    STATIC_ASSERT(offsetof(STREAK_HEADER_PC, has_new_node) == 0x10);
    STATIC_ASSERT(offsetof(STREAK_HEADER_PC, owner_slot) == 0x14);
    STATIC_ASSERT(offsetof(STREAK_HEADER_PC, colour) == 0x18);
    STATIC_ASSERT(offsetof(STREAK_HEADER_PC, owner) == 0x1c);
    extern STREAK_NODE_PC StreakNodesPc[128];
    extern STREAK_HEADER_PC StreakHeadersPc[32];
    extern STREAK_NODE_PC *streaks_free;
    extern STREAK_NODE_PC *StreakNodesUsedPc;
    extern STREAK_HEADER_PC *streakhdrs_free;
    extern NUMTL *streakmtl;
    extern NUMTL *streakmtl_ref;
    extern const char StreakTextureNamePc[17];
    void InitStreaks(VARIPTR *, VARIPTR, char *);
    void AddStreakPoints(NUVEC *, f32, u32, void **, i32, GAMEOBJECT *);
    void UpdateStreaks(f32);
    void DrawStreaks(void);
    void ResetStreaks(void);
#ifdef __cplusplus
}
#endif
