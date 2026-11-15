/* Animation data shared by the cutscene loader and the renderer: the DWA curve set header and the state
 * (step) animation track. */

#pragma once

#include "nu2api/nucore/common.h"

union DWA_CURVE_CELL;

typedef union DWA_FRAME_HEADER {
    f32 duration;
    u32 magic;
} DWA_FRAME_HEADER;

typedef union DWA_NODE_COUNT {
    i16 legacy;
    u16 modern;
} DWA_NODE_COUNT;

typedef struct NUANIMDATA2 {      // size 0x14
    DWA_FRAME_HEADER header;      // 0x00
    DWA_NODE_COUNT node_count;    // 0x04
    i16 curve_count;              // 0x06
    i16 chunk_count;              // 0x08
    u16 modern_frame_count;       // 0x0a
    union DWA_CURVE_CELL *curves; // 0x0c
    i8 *curve_types;              // 0x10
} NUANIMDATA2;

typedef struct state_anim_s { // size 0xc
    u16 count;                // 0x0
    u16 unknown_0002;         // 0x2
    f32 *times;               // 0x4
    u8 *values;               // 0x8
} STATE_ANIM;
