#ifndef SAGA_LEGOAPI_CHARACTERS_MOTION_CHRIS_H_PART
#define SAGA_LEGOAPI_CHARACTERS_MOTION_CHRIS_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/collect/spacelevel.h, nu2api/nucore/common.h,
 * nu2api/nucore/fixed_width.h, nu2api/numath/nuvec4.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "legoapi/items/collect/spacelevel.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec4.h"
#endif
/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_CHARACTERS_MOTION_CHRIS_H_PART == 1 && !defined(SAGA_LEGOAPI_CHARACTERS_MOTION_CHRIS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct flight_spline_resource_pc_s {
        NUVEC4 points[64];
        i32 point_count;
        u8 unknown_0404[12];
        f32 length;
        f32 cumulative_distances[64];
        u8 unknown_0514[16];
        i32 id;
        u8 unknown_0528[8];
    } FLIGHT_SPLINE_RESOURCE_PC;

    STATIC_ASSERT(sizeof(FLIGHT_SPLINE_RESOURCE_PC) == 0x530);
    STATIC_ASSERT(offsetof(FLIGHT_SPLINE_RESOURCE_PC, point_count) == 0x400);
    STATIC_ASSERT(offsetof(FLIGHT_SPLINE_RESOURCE_PC, length) == 0x410);
    STATIC_ASSERT(offsetof(FLIGHT_SPLINE_RESOURCE_PC, cumulative_distances) == 0x414);
    STATIC_ASSERT(offsetof(FLIGHT_SPLINE_RESOURCE_PC, id) == 0x524);
    void CalcSplinePointFromDist(FLIGHT_SPLINE_RESOURCE_PC *spline, NUVEC4 *result, f32 distance);
    extern f32 MulAdd;
    extern f32 MulDist;
    extern f32 SpeedAdd;
    extern f32 SpeedDist;
    void ChrisAnakinCReset(void);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CHARACTERS_MOTION_CHRIS_H_PART
#define SAGA_LEGOAPI_CHARACTERS_MOTION_CHRIS_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_MOTION_CHRIS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_CHARACTERS_MOTION_CHRIS_H_PART == 2 && !defined(SAGA_LEGOAPI_CHARACTERS_MOTION_CHRIS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    i32 ShipDropCoins(struct starfighter_s *fighter);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CHARACTERS_MOTION_CHRIS_H_PART
#define SAGA_LEGOAPI_CHARACTERS_MOTION_CHRIS_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_MOTION_CHRIS_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_CHARACTERS_MOTION_CHRIS_H_STOP
