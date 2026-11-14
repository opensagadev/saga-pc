#ifndef SAGA_GAMELIB_NUWIND_NUWIND_H
#define SAGA_GAMELIB_NUWIND_NUWIND_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

typedef struct nuwind_s {
    i32 texture_ids[8];
    i32 current;
    f32 parameter_24;
    f32 phase_speed_scale;
    f32 phase;
    f32 accumulated_time;
    f32 scaled_accumulated_time;
} NUWIND;

STATIC_ASSERT(sizeof(NUWIND) == 0x38);
STATIC_ASSERT(offsetof(NUWIND, phase_speed_scale) == 0x28);
STATIC_ASSERT(offsetof(NUWIND, phase) == 0x2c);
STATIC_ASSERT(offsetof(NUWIND, accumulated_time) == 0x30);
STATIC_ASSERT(offsetof(NUWIND, scaled_accumulated_time) == 0x34);
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct PC_NUWINDGROUP {
        u16 in_use;
        u16 flags;
        NUGSCN *scene;
        NUDISPLAY_SPECIAL_PC *special;
        NUMTX *matrices;
        NUVEC center;
        u8 drawn;
        u8 visible;
        i16 matrix_count;
        f32 value20;
        f32 value24;
        f32 radius_squared;
        f32 radius;
        f32 near_distance;
        f32 far_distance;
        f32 extended_radius_squared;
        f32 distance_range;
        f32 field_40;
        f32 field_44;
        f32 field_48;
    } PC_NUWINDGROUP;

    STATIC_ASSERT(sizeof(PC_NUWINDGROUP) == 76);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, in_use) == 0);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, flags) == 2);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, scene) == 4);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, special) == 8);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, matrices) == 12);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, center) == 16);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, drawn) == 28);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, visible) == 29);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, matrix_count) == 30);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, value20) == 32);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, value24) == 36);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, radius_squared) == 40);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, radius) == 44);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, near_distance) == 48);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, far_distance) == 52);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, extended_radius_squared) == 56);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, distance_range) == 60);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, field_40) == 64);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, field_44) == 68);
    STATIC_ASSERT(offsetof(PC_NUWINDGROUP, field_48) == 72);
    extern PC_NUWINDGROUP *NuWindGroup;
    extern PC_NUWINDGROUP *NuWindCurGrp;
    extern i32 NuWindMaxGroupsPC;
    extern i32 NuWindWave;
    extern i32 NuWindDir;
    extern i32 NuWindDir2;
    void NuWindInit(void);
    PC_NUWINDGROUP *NuWindAllocateGrp(void);
    PC_NUWINDGROUP *NuWindCreateMtx(NUSPECIALHANDLE *, NUMTX *, i16, f32, f32, i32, f32, f32);
    void NuWindUpdateArray(NUVEC **positions);
    void NuWindUpdate(NUVEC *wind);
    void NuWindDraw(void);
    void NuWindInit(void);
    void NuWindAnimate(NUWIND *wind, f32 frame_time);

    extern i32 NuWindMaxMatsPC;
    void NuWindSetup(VARIPTR *, VARIPTR, i32, i32);
    void NuWindInitialise(NUWIND *);
#ifdef __cplusplus
}
#endif

STATIC_ASSERT(offsetof(NUWIND, texture_ids) == 0);
STATIC_ASSERT(offsetof(NUWIND, current) == 0x20);
STATIC_ASSERT(offsetof(NUWIND, parameter_24) == 0x24);
#ifdef __cplusplus
extern "C" {
#endif
    i32 NuWindCurrent(NUWIND *);
#ifdef __cplusplus
}
#endif

#endif
