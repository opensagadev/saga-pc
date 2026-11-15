#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec4.h"

struct starfighter_s;

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct flight_spline_resource_s { // size 0x530
        NUVEC4 points[64];                    // 0x000
        i32 point_count;                      // 0x400
        u8 unknown_0404[12];                  // 0x404
        f32 length;                           // 0x410
        f32 cumulative_distances[64];         // 0x414
        u8 unknown_0514[16];                  // 0x514
        i32 id;                               // 0x524
        u8 unknown_0528[8];                   // 0x528
    } FLIGHT_SPLINE_RESOURCE;

    void CalcSplinePointFromDist(FLIGHT_SPLINE_RESOURCE *spline, NUVEC4 *result, f32 distance);
    extern f32 MulAdd;
    extern f32 MulDist;
    extern f32 SpeedAdd;
    extern f32 SpeedDist;
    void ChrisAnakinCReset(void);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 ShipDropCoins(struct starfighter_s *fighter);
#ifdef __cplusplus
}
#endif
