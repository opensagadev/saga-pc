/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

typedef struct LEDGE_s LEDGE_s;

struct LEDGE_s {
    char name[8];
    NUVEC position;
    u16 y_rotation;
    i8 type_code;

    union {
        u8 state_flags;

        struct {
            u8 active : 1;
            u8 visible : 1;
            u8 : 6;
        };
    };

    u8 type_index;
    u8 flags;
    u8 pad_1a[2];
    i16 field_0x1c;
    i16 field_0x1e;
    NUVEC bounds_min;
    NUVEC bounds_max;
};

STATIC_ASSERT(sizeof(struct LEDGE_s) == 56);
