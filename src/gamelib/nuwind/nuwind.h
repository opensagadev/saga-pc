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

typedef struct nuwind_s {        // size 0x38
    i32 texture_ids[8];          // 0x00
    i32 current;                 // 0x20
    f32 parameter_24;            // 0x24
    f32 phase_speed_scale;       // 0x28
    f32 phase;                   // 0x2c
    f32 accumulated_time;        // 0x30
    f32 scaled_accumulated_time; // 0x34
} NUWIND;

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct PC_NUWINDGROUP {    // size 0x4c
        u16 in_use;                    // 0x00
        u16 flags;                     // 0x02
        NUGSCN *scene;                 // 0x04
        NUDISPLAY_SPECIAL_PC *special; // 0x08
        NUMTX *matrices;               // 0x0c
        NUVEC center;                  // 0x10
        u8 drawn;                      // 0x1c
        u8 visible;                    // 0x1d
        i16 matrix_count;              // 0x1e
        f32 value20;                   // 0x20
        f32 value24;                   // 0x24
        f32 radius_squared;            // 0x28
        f32 radius;                    // 0x2c
        f32 near_distance;             // 0x30
        f32 far_distance;              // 0x34
        f32 extended_radius_squared;   // 0x38
        f32 distance_range;            // 0x3c
        f32 field_40;                  // 0x40
        f32 field_44;                  // 0x44
        f32 field_48;                  // 0x48
    } PC_NUWINDGROUP;

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

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuWindCurrent(NUWIND *);
#ifdef __cplusplus
}
#endif
