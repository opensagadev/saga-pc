/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

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

    typedef struct PC_NUFADEGROUP {    // size 0x40
        i16 active;                    // 0x00
        i16 flags;                     // 0x02
        NUGSCN *scene;                 // 0x04
        NUDISPLAY_SPECIAL_PC *special; // 0x08
        void *terrain;                 // 0x0c
        i16 instance_index;            // 0x10
        u8 unknown_12[2];              // 0x12
        NUMTX *matrices;               // 0x14
        i16 *platforms;                // 0x18
        NUVEC centre;                  // 0x1c
        u8 drawn;                      // 0x28
        u8 in_range;                   // 0x29
        i16 count;                     // 0x2a
        f32 radius_squared;            // 0x2c
        f32 near_distance;             // 0x30
        f32 far_distance;              // 0x34
        f32 cull_distance_squared;     // 0x38
        f32 fade_range;                // 0x3c
    } PC_NUFADEGROUP;

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
