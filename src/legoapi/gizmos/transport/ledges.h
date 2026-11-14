/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

typedef struct LEDGE_s LEDGE_s;

struct LEDGE_s {    // size 0x38
    char name[8];   // 0x00
    NUVEC position; // 0x08
    u16 y_rotation; // 0x14
    i8 type_code;   // 0x16

    union { // size 0x1
        u8 state_flags;

        struct {            // size 0x1
            u8 active : 1;  // 0x17:0
            u8 visible : 1; // 0x17:1
            u8 : 6;         // 0x17:2
        };
    }; // 0x17

    u8 type_index;    // 0x18
    u8 flags;         // 0x19
    u8 pad_1a[2];     // 0x1a
    i16 field_0x1c;   // 0x1c
    i16 field_0x1e;   // 0x1e
    NUVEC bounds_min; // 0x20
    NUVEC bounds_max; // 0x2c
};
