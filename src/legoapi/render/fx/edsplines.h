#ifndef SAGA_LEGOAPI_RENDER_FX_EDSPLINES_H
#define SAGA_LEGOAPI_RENDER_FX_EDSPLINES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h (not all of it earlier in the shim). */

#include "legoapi/characters/motion/chris.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#endif

typedef struct SPLINEPOS_s {
    NUGSPLINE *spline;
    i16 segment;
    i8 looping;

    union {
        u8 reached_end;
        u8 finished;
    };

    union {
        f32 segment_distance;
        f32 distance;
    };

    f32 segment_length;
    NUVEC position;

    union {
        f32 along;
        f32 normalized_position;
    };
} SPLINEPOS;

STATIC_ASSERT(sizeof(SPLINEPOS) == 0x20);
STATIC_ASSERT(offsetof(SPLINEPOS, spline) == 0);
STATIC_ASSERT(offsetof(SPLINEPOS, segment) == 4);
STATIC_ASSERT(offsetof(SPLINEPOS, looping) == 6);
STATIC_ASSERT(offsetof(SPLINEPOS, finished) == 7);
STATIC_ASSERT(offsetof(SPLINEPOS, segment_distance) == 8);
STATIC_ASSERT(offsetof(SPLINEPOS, segment_length) == 0xc);
STATIC_ASSERT(offsetof(SPLINEPOS, position) == 0x10);
STATIC_ASSERT(offsetof(SPLINEPOS, along) == 0x1c);
#ifdef __cplusplus
extern "C" {
#endif
    void CalcSplinePoint(FLIGHT_SPLINE_RESOURCE_PC *spline, NUVEC4 *result, f32 along);
    i32 OutSideSplineArea(NUVEC *, NUGSPLINE *, NUVEC *, NUVEC *, i32);
    void PointAlongSpline(NUGSPLINE *spline, f32 along, NUVEC *result, u16 *yaw, u16 *pitch, i32 looping);
    void MoveSplinePosition(SPLINEPOS *position, f32 distance);
    void GetNearestSplinePos(NUVEC *origin, SPLINEPOS *result, NUGSPLINE *spline, i32 looping, i16 first_point,
                             i16 last_point);
    void InitSplinePosition(SPLINEPOS *position, NUGSPLINE *spline, f32 distance, i32 looping);
    extern NUGSPLINE *splineStore;
    extern i32 numSplinesLoaded;
    extern const char *EDSPLINE_FILECHECK;
    extern const char EditorSplineFileCheckTextPc[13];
    void GameAntiNodeData_Init(void *, NUSPECIALHANDLE *);
    void GameAntiNodeData_Read(void *);
    NUGSPLINE *edSpline_SplineFind(NUGSCN *, char *);
#ifdef __cplusplus
}
#endif

#endif
