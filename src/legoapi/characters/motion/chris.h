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
        /* the dogfight time of the next fighter, -1 when done */
        f32 next_spawn;  // 0x404
        f32 reset_spawn; // 0x408
        /* 0 for one fighter */
        f32 spawn_interval;           // 0x40c
        f32 length;                   // 0x410
        f32 cumulative_distances[64]; // 0x414
        /* 0 for no limit */
        i32 repeat_limit; // 0x514
        i32 remaining;    // 0x518
        /* the spline the fighter chases, -1 for none */
        i32 target_id; // 0x51c
        i32 object_id; // 0x520
        /* also the fighter's model for playback splines */
        i32 id;          // 0x524
        i32 enabled;     // 0x528
        u32 unknown_52c; // 0x52c
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
    extern const f32 DogfightOriginEndXPc;
    extern const f32 DogfightOriginEndY;
    extern const f32 DogfightOriginEndZ;
    extern const f32 DogfightOriginStartX;
    i32 ChrisExtraBoltCollision(struct bolt_s *bolt, struct nuvec_s *points);
    void ChrisAllocLevelStuff(struct WORLDINFO_s *world);
    void ChrisDogFightADraw(struct WORLDINFO_s *);
    void ChrisDogFightAInit(struct WORLDINFO_s *);
    void ChrisDogFightAReset(struct WORLDINFO_s *);
    void ChrisDogFightAUpdate(struct WORLDINFO_s *);
    void FlightSpline_Init(struct WORLDINFO_s *world, FLIGHT_SPLINE_RESOURCE *splines, i32 capacity);
#ifdef __cplusplus
}
#endif
