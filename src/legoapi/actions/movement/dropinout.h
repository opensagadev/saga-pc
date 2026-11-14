/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/menus/screens/movies.h"
#endif

struct gameobject_s;

typedef struct gamepad_init_record_s { // size 0x60
    NUPAD_INIT_STATE *pad;             // 0x00

    union { // size 0x5c

        struct {                 // size 0x5c
            u8 unknown_04[0x56]; // 0x04
            u16 allocated;       // 0x5a
            u8 unknown_5c[4];    // 0x5c
        };

        struct {                      // size 0x5c
            u32 menu_buttons_held;    // 0x04
            u32 menu_buttons_pressed; // 0x08
            u32 menu_directions_held; // 0x0c
            u32 menu_directions;      // 0x10
            u8 unknown_14[0x4c];      // 0x14
        };
    }; // 0x04
} GAMEPAD_INIT_RECORD;

#ifdef __cplusplus
extern "C" {
#endif

    void DropInOutCode(struct gameobject_s *);
    void DropOut(i32, i32, i32, i32);
#ifdef __cplusplus
}
#endif
