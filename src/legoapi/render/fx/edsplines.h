/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

struct NUGSCN_s;
struct NUVEC4;
struct flight_spline_resource_s;
struct nugspline_s;
struct nuhspecial_s;

typedef struct SPLINEPOS_s {    // size 0x20
    struct nugspline_s *spline; // 0x00
    i16 segment;                // 0x04
    i8 looping;                 // 0x06

    union { // size 0x1
        u8 reached_end;
        u8 finished;
    }; // 0x07

    union { // size 0x4
        f32 segment_distance;
        f32 distance;
    }; // 0x08

    f32 segment_length; // 0x0c
    NUVEC position;     // 0x10

    union { // size 0x4
        f32 along;
        f32 normalized_position;
    }; // 0x1c
} SPLINEPOS;

#ifdef __cplusplus
extern "C" {
#endif
    void CalcSplinePoint(struct flight_spline_resource_s *spline, struct NUVEC4 *result, f32 along);
    i32 OutSideSplineArea(NUVEC *, struct nugspline_s *, NUVEC *, NUVEC *, i32);
    void PointAlongSpline(struct nugspline_s *spline, f32 along, NUVEC *result, u16 *yaw, u16 *pitch, i32 looping);
    void MoveSplinePosition(SPLINEPOS *position, f32 distance);
    void GetNearestSplinePos(NUVEC *origin, SPLINEPOS *result, struct nugspline_s *spline, i32 looping, i16 first_point, i16 last_point);
    void InitSplinePosition(SPLINEPOS *position, struct nugspline_s *spline, f32 distance, i32 looping);
    extern struct nugspline_s *splineStore;
    extern i32 numSplinesLoaded;
    extern const char *EDSPLINE_FILECHECK;
    extern const char EditorSplineFileCheckText[13];
    void GameAntiNodeData_Init(void *, struct nuhspecial_s *);
    void GameAntiNodeData_Read(void *);
    struct nugspline_s *edSpline_SplineFind(struct NUGSCN_s *, char *);
#ifdef __cplusplus
}
#endif
