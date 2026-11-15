#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct COLLECTID_PC { // size 0x1c

        union { // size 0x1c

            struct {                 // size 0x1c
                i16 id;              // 0x00
                u8 unknown_02[0x1a]; // 0x02
            };

            struct {                         // size 0x1c
                u8 unknown_price_prefix[2];  // 0x00
                u8 selection_type;           // 0x02
                u8 unknown_price_03;         // 0x03
                u32 price;                   // 0x04
                u8 shop_enabled;             // 0x08
                u8 unknown_price_tail[0x13]; // 0x09
            };

            struct {                          // size 0x1c
                u8 unknown_grid_prefix[0x14]; // 0x00
                f32 grid_x;                   // 0x14
                f32 grid_y;                   // 0x18
            };
        }; // 0x00
    } COLLECTID_PC;

#ifdef __cplusplus
}
#endif
typedef struct COLLECTID COLLECTID;

struct COLLECTID {  // size 0x1c
    i16 id;         // 0x00
    u8 type;        // 0x02
    u8 field2_0x3;  // 0x03
    i32 field3_0x4; // 0x04
    u8 can_buy;     // 0x08
    u8 field5_0x9;  // 0x09
    u16 field6_0xa; // 0x0a

    union { // size 0x10
        char cheat_code[16];

        struct {                 // size 0x10
            u8 grid_reserved[8]; // 0x0c
            f32 grid_x;          // 0x14
            f32 grid_y;          // 0x18
        };
    }; // 0x0c
};
