#ifndef SAGA_NU2API_NUOBJECT_H
#define SAGA_NU2API_NUOBJECT_H

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

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct PC_NUFADEGROUP {
        i16 active;
        i16 flags;
        NUGSCN *scene;
        NUDISPLAY_SPECIAL_PC *special;
        void *terrain;
        i16 instance_index;
        u8 unknown_12[2];
        NUMTX *matrices;
        i16 *platforms;
        NUVEC centre;
        u8 drawn;
        u8 in_range;
        i16 count;
        f32 radius_squared;
        f32 near_distance;
        f32 far_distance;
        f32 cull_distance_squared;
        f32 fade_range;
    } PC_NUFADEGROUP;

    STATIC_ASSERT(sizeof(PC_NUFADEGROUP) == 64);
    STATIC_ASSERT(offsetof(PC_NUFADEGROUP, active) == 0);
    STATIC_ASSERT(offsetof(PC_NUFADEGROUP, flags) == 2);
    STATIC_ASSERT(offsetof(PC_NUFADEGROUP, scene) == 4);
    STATIC_ASSERT(offsetof(PC_NUFADEGROUP, special) == 8);
    STATIC_ASSERT(offsetof(PC_NUFADEGROUP, terrain) == 12);
    STATIC_ASSERT(offsetof(PC_NUFADEGROUP, instance_index) == 16);
    STATIC_ASSERT(offsetof(PC_NUFADEGROUP, unknown_12) == 18);
    STATIC_ASSERT(offsetof(PC_NUFADEGROUP, matrices) == 20);
    STATIC_ASSERT(offsetof(PC_NUFADEGROUP, platforms) == 24);
    STATIC_ASSERT(offsetof(PC_NUFADEGROUP, centre) == 28);
    STATIC_ASSERT(offsetof(PC_NUFADEGROUP, drawn) == 40);
    STATIC_ASSERT(offsetof(PC_NUFADEGROUP, in_range) == 41);
    STATIC_ASSERT(offsetof(PC_NUFADEGROUP, count) == 42);
    STATIC_ASSERT(offsetof(PC_NUFADEGROUP, radius_squared) == 44);
    STATIC_ASSERT(offsetof(PC_NUFADEGROUP, near_distance) == 48);
    STATIC_ASSERT(offsetof(PC_NUFADEGROUP, far_distance) == 52);
    STATIC_ASSERT(offsetof(PC_NUFADEGROUP, cull_distance_squared) == 56);
    STATIC_ASSERT(offsetof(PC_NUFADEGROUP, fade_range) == 60);
    extern PC_NUFADEGROUP *NuFadeObjGroup;
    extern PC_NUFADEGROUP *NuFadeObjCurGrp;
    extern i32 NuFadeObjEnabledPc;
    extern i16 *NuFadeData;
    extern i32 NuFadeObjDataIndex;
    extern i32 NuFadeObjWave;
    extern i32 NuFadeObjDir;
    extern i32 NuFadeObjDir2;
    PC_NUFADEGROUP *NuFadeObjAllocateGrp(void);
#ifdef __cplusplus
}
#endif

#endif
