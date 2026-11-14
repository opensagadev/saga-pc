/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct COLLECTID_PC {
        union {
            struct {
                i16 id;
                u8 unknown_02[0x1a];
            };

            struct {
                u8 unknown_price_prefix[2];
                u8 selection_type;
                u8 unknown_price_03;
                u32 price;
                u8 shop_enabled;
                u8 unknown_price_tail[0x13];
            };

            struct {
                u8 unknown_grid_prefix[0x14];
                f32 grid_x;
                f32 grid_y;
            };
        };
    } COLLECTID_PC;

    STATIC_ASSERT(sizeof(COLLECTID_PC) == 0x1c);
    STATIC_ASSERT(offsetof(COLLECTID_PC, id) == 0);
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(sizeof(COLLECTID_PC) == 0x1c);
STATIC_ASSERT(offsetof(COLLECTID_PC, id) == 0);
STATIC_ASSERT(offsetof(COLLECTID_PC, unknown_02) == 2);
STATIC_ASSERT(offsetof(COLLECTID_PC, selection_type) == 2);
STATIC_ASSERT(offsetof(COLLECTID_PC, price) == 4);
STATIC_ASSERT(offsetof(COLLECTID_PC, shop_enabled) == 8);
typedef struct COLLECTID COLLECTID;

struct COLLECTID {
    i16 id;
    u8 type;
    u8 field2_0x3;
    i32 field3_0x4;
    u8 can_buy;
    u8 field5_0x9;
    u16 field6_0xa;

    union {
        char cheat_code[16];

        struct {
            u8 grid_reserved[8];
            f32 grid_x;
            f32 grid_y;
        };
    };
};

STATIC_ASSERT(sizeof(struct COLLECTID) == 28);
STATIC_ASSERT(offsetof(COLLECTID, grid_x) == 20);
STATIC_ASSERT(offsetof(COLLECTID, grid_y) == 24);
STATIC_ASSERT(offsetof(COLLECTID_PC, grid_x) == 20);
STATIC_ASSERT(offsetof(COLLECTID_PC, grid_y) == 24);
STATIC_ASSERT(offsetof(COLLECTID_PC, unknown_price_tail) == 9);
