/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/menus/screens/movies.h"
#endif

struct gameobject_s;

typedef struct gamepad_init_record_s {
    NUPAD_INIT_STATE *pad;

    union {
        struct {
            u8 unknown_04[0x56];
            u16 allocated;
            u8 unknown_5c[4];
        };

        struct {
            u32 menu_buttons_held;
            u32 menu_buttons_pressed;
            u32 menu_directions_held;
            u32 menu_directions;
            u8 unknown_14[0x4c];
        };
    };
} GAMEPAD_INIT_RECORD;

STATIC_ASSERT(sizeof(GAMEPAD_INIT_RECORD) == 0x60);
STATIC_ASSERT(offsetof(GAMEPAD_INIT_RECORD, pad) == 0);
STATIC_ASSERT(offsetof(GAMEPAD_INIT_RECORD, allocated) == 0x5a);
STATIC_ASSERT(offsetof(GAMEPAD_INIT_RECORD, menu_buttons_held) == 4);
STATIC_ASSERT(offsetof(GAMEPAD_INIT_RECORD, menu_buttons_pressed) == 8);
STATIC_ASSERT(offsetof(GAMEPAD_INIT_RECORD, menu_directions_held) == 0xc);
STATIC_ASSERT(offsetof(GAMEPAD_INIT_RECORD, menu_directions) == 0x10);
STATIC_ASSERT(offsetof(GAMEPAD_INIT_RECORD, unknown_5c) == 0x5c);
STATIC_ASSERT(sizeof(GAMEPAD_INIT_RECORD) == 0x60);
#ifdef __cplusplus
extern "C" {
#endif

    void DropInOutCode(struct gameobject_s *);
    void DropOut(i32, i32, i32, i32);
#ifdef __cplusplus
}
#endif
