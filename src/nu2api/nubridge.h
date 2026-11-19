#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

typedef struct nubridge_s {      // size 0xb0c
    u8 active;                   // 0x000
    u8 unknown_01;               // 0x001
    i16 idle_frames;             // 0x002
    NUGSCN *scene;               // 0x004
    void *terrain;               // 0x008
    NUVEC edge_positions[24][2]; // 0x00c
    NUVEC edge_motion[24][2];    // 0x24c
    NUMTX platform_matrices[24]; // 0x48c
    nuhspecial_s special;        // 0xa8c
    nuhspecial_s second_special; // 0xa98
    NUVEC centre;                // 0xaa4
    f32 cull_radius_squared;     // 0xab0
    i16 platforms[24];           // 0xab4
    u8 in_range;                 // 0xae4
    u8 was_drawn;                // 0xae5
    i8 platform_count;           // 0xae6
    u8 special_spacing;          // 0xae7
    u32 colour;                  // 0xae8
    u16 contact_frames;          // 0xaec
    i16 rotation_y;              // 0xaee
    f32 width;                   // 0xaf0
    f32 spring_strength;         // 0xaf4
    f32 gravity;                 // 0xaf8
    f32 damping;                 // 0xafc
    f32 load_multiplier;         // 0xb00
    f32 half_width;              // 0xb04
    f32 height_offset;           // 0xb08
} NUBRIDGE;

#ifdef __cplusplus
extern "C" {
#endif
    extern NUBRIDGE Bridges[8];
    extern i32 NuBridgeProc;
    void NuBridgeUpdate(NUVEC *);
    void NuBridgeDraw(NUMTL *);
#ifdef __cplusplus
}
#endif
