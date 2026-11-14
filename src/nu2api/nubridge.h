#ifndef SAGA_NU2API_NUBRIDGE_H
#define SAGA_NU2API_NUBRIDGE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h (not all of it earlier in the shim). */

#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#endif

typedef struct nubridge_pc_s {
    u8 active;
    u8 unknown_01;
    i16 idle_frames;
    NUGSCN *scene;
    void *terrain;
    NUVEC edge_positions[24][2];
    NUVEC edge_motion[24][2];
    NUMTX platform_matrices[24];
    NUSPECIALHANDLE special;
    NUSPECIALHANDLE second_special;
    NUVEC centre;
    f32 cull_radius_squared;
    i16 platforms[24];
    u8 in_range;
    u8 was_drawn;
    i8 platform_count;
    u8 field_ae7;
    u32 colour;
    u16 contact_frames;
    i16 rotation_y;
    f32 width;
    f32 spring_strength;
    f32 gravity;
    f32 damping;
    f32 load_multiplier;
    f32 field_b04;
    f32 field_b08;
} NUBRIDGE_PC;

STATIC_ASSERT(sizeof(NUBRIDGE_PC) == 0xb0c);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, active) == 0x0);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, idle_frames) == 0x2);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, scene) == 0x4);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, terrain) == 0x8);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, edge_positions) == 0xc);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, edge_motion) == 0x24c);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, platform_matrices) == 0x48c);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, special) == 0xa8c);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, second_special) == 0xa98);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, centre) == 0xaa4);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, cull_radius_squared) == 0xab0);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, platforms) == 0xab4);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, in_range) == 0xae4);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, was_drawn) == 0xae5);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, platform_count) == 0xae6);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, field_ae7) == 0xae7);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, colour) == 0xae8);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, contact_frames) == 0xaec);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, rotation_y) == 0xaee);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, width) == 0xaf0);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, spring_strength) == 0xaf4);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, gravity) == 0xaf8);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, damping) == 0xafc);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, load_multiplier) == 0xb00);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, field_b04) == 0xb04);
STATIC_ASSERT(offsetof(NUBRIDGE_PC, field_b08) == 0xb08);
#ifdef __cplusplus
extern "C" {
#endif
    extern NUBRIDGE_PC Bridges[8];
    extern i32 NuBridgeProc;
    void NuBridgeUpdate(NUVEC *);
    void NuBridgeDraw(NUMTL *);
#ifdef __cplusplus
}
#endif

#endif
