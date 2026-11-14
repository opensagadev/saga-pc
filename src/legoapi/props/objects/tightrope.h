/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"
#endif

/* Nine held scalar declarations are in the separately pinned state proposal. */

#ifdef __cplusplus
extern "C" {
#endif
    /* Inside the existing C public declaration section; definition remains owner-local. */
    typedef struct TIGHTROPE_MOTION TIGHTROPE_MOTION;
    TIGHTROPE_MOTION *TightRope_InRange(GAMEOBJECT *, WORLDINFO *, NUVEC *);
    void TightRope_MoveCode(GAMEOBJECT *, i32);
#ifdef __cplusplus
}
#endif
typedef struct TIGHTROPE_s TIGHTROPE_s;

struct TIGHTROPE_s { // size 0x48
    char name[16];   // 0x00

    union { // size 0xc
        NUVEC start_position;
        NUVEC start;
    }; // 0x10

    union { // size 0xc
        NUVEC end_position;
        NUVEC end;
    }; // 0x1c

    u16 unknown_28;  // 0x28
    u16 unknown_2a;  // 0x2a
    u16 unknown_2c;  // 0x2c
    u16 unknown_2e;  // 0x2e
    u8 unknown_30;   // 0x30
    u8 unknown_31;   // 0x31
    i8 unknown_32;   // 0x32
    u8 pad_33;       // 0x33
    NUVEC direction; // 0x34

    union { // size 0x4
        f32 length;
        f32 horizontal_length;
    }; // 0x40

    union { // size 0x2
        u16 rotation;
        u16 y_rotation;
        u16 angle;
    }; // 0x44

    union { // size 0x1
        u8 visible;
        u8 enabled;
    }; // 0x46

    union { // size 0x1
        u8 active;
        u8 available;
    }; // 0x47
};
