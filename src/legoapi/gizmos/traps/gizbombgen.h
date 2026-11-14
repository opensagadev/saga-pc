/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

typedef struct GIZBOMBGENSYS_s GIZBOMBGENSYS_s;
typedef struct GIZBOMBGEN_s GIZBOMBGEN_s;

struct GIZBOMBGEN_s {                    // size 0x2c
    char name[16];                       // 0x00
    NUVEC position;                      // 0x10
    struct gameanimset_s *anim_set;      // 0x1c
    i32 interval;                        // 0x20
    struct gameobject_s *generated_bomb; // 0x24
    i16 field_0x28;                      // 0x28

    union { // size 0x2
        u16 flags;

        struct {                     // size 0x2
            u16 active : 1;          // 0x2a:0
            u16 visible : 1;         // 0x2a:1
            u16 reserved_flags : 14; // 0x2a:2
        };
    }; // 0x2a
};

struct GIZBOMBGENSYS_s {                  // size 0x14
    struct GIZBOMBGEN_s *bomb_generators; // 0x00
    u16 count;                            // 0x04
    u16 capacity;                         // 0x06
    void *anim_object_pool;               // 0x08
    u8 flags;                             // 0x0c
    u8 reserved_0x0d[3];                  // 0x0d
    void *progress;                       // 0x10
};

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 bombgen_gizmotype_id;
#ifdef __cplusplus
}
#endif

enum { GIZBOMBGEN_FLAG_ACTIVE = 1 };

enum { GIZBOMBGEN_FLAG_VISIBLE = 2 };

typedef struct {                 // size 0x14
    GIZBOMBGEN_s *items;         // 0x00
    u16 count;                   // 0x04
    u16 capacity;                // 0x06
    void *pool;                  // 0x08
    i16 reset : 1;               // 0x0c:0
    i16 reset_saved : 1;         // 0x0c:1
    i16 reset_flags_unused : 14; // 0x0c:2
    u16 opaque_0e;               // 0x0e
    void *progress;              // 0x10
} ADD_BOMB_RESET_SYS;
