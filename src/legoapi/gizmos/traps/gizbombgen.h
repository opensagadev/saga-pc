#ifndef SAGA_LEGOAPI_GIZMOS_TRAPS_GIZBOMBGEN_H
#define SAGA_LEGOAPI_GIZMOS_TRAPS_GIZBOMBGEN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h, legoapi/legoapi_types.h (not all of it earlier in
 * the shim). */

#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#endif

typedef struct GIZBOMBGENSYS_s GIZBOMBGENSYS_s;
typedef struct GIZBOMBGEN_s GIZBOMBGEN_s;

struct GIZBOMBGEN_s {
    char name[16];
    NUVEC position;
    struct gameanimset_s *anim_set;
    i32 interval;
    struct gameobject_s *generated_bomb;
    i16 field_0x28;

    union {
        u16 flags;

        struct {
            u16 active : 1;
            u16 visible : 1;
            u16 reserved_flags : 14;
        };
    };
};

STATIC_ASSERT(sizeof(struct GIZBOMBGEN_s) == 44);
STATIC_ASSERT(offsetof(struct GIZBOMBGEN_s, flags) == 0x2a);

struct GIZBOMBGENSYS_s {
    struct GIZBOMBGEN_s *bomb_generators;
    u16 count;
    u16 capacity;
    void *anim_object_pool;
    u8 flags;
    u8 reserved_0x0d[3];
    void *progress;
};

STATIC_ASSERT(sizeof(struct GIZBOMBGENSYS_s) == 20);
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 bombgen_gizmotype_id;
#ifdef __cplusplus
}
#endif

enum { GIZBOMBGEN_FLAG_ACTIVE = 1 };

enum { GIZBOMBGEN_FLAG_VISIBLE = 2 };

#endif
