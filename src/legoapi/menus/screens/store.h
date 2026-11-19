#pragma once

#include "nu2api/nucore/common.h"

typedef struct COLLECTID COLLECTID;

struct COLLECTID {  // size 0x1c
    i16 id;         // 0x00
    u8 type;        // 0x02
    u8 area_index;  // 0x03
    u32 price;      // 0x04
    u8 can_buy;     // 0x08
    u8 counts_towards_completion; // 0x09
    u16 gold_bricks; // 0x0a

    union { // size 0x10
        char cheat_code[16];

        struct {                 // size 0x10
            u8 grid_reserved[8]; // 0x0c
            f32 grid_x;          // 0x14
            f32 grid_y;          // 0x18
        };
    }; // 0x0c
};
