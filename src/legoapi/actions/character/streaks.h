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

    typedef struct STREAK_NODE {  // size 0xe8
        struct STREAK_NODE *next; // 0x00
        struct STREAK_NODE *prev; // 0x04
        NUVEC position;           // 0x08
        NUVEC previous_position;  // 0x14
        NUVEC start_tangent;      // 0x20
        NUVEC end_tangent;        // 0x2c
        NUVEC positions[7];       // 0x38
        NUVEC tangents[7];        // 0x8c
        i32 segment_count;        // 0xe0
        f32 remaining_time;       // 0xe4
    } STREAK_NODE;

    typedef struct STREAK_HEADER {  // size 0x20
        struct STREAK_HEADER *next; // 0x00
        struct STREAK_HEADER *prev; // 0x04
        STREAK_NODE *nodes;         // 0x08
        u16 index;                  // 0x0c
        u8 flags;                   // 0x0e
        u8 unknown_0f;              // 0x0f
        i32 has_new_node;           // 0x10
        void **owner_slot;          // 0x14
        u32 colour;                 // 0x18
        GAMEOBJECT *owner;          // 0x1c
    } STREAK_HEADER;

    extern STREAK_NODE StreakNodes[128];
    extern STREAK_HEADER StreakHeaders[32];
    extern STREAK_NODE *streaks_free;
    extern STREAK_NODE *StreakNodesUsed;
    extern STREAK_HEADER *streakhdrs_free;
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
