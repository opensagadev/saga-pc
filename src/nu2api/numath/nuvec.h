#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuang.h"

struct numtx_s;

typedef struct nuvec_s { // size 0xc
    f32 x;               // 0x0
    f32 y;               // 0x4
    f32 z;               // 0x8
} NUVEC;

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuLineLineIntersect(NUVEC *, NUVEC *, NUVEC *, NUVEC *, f32 *, f32 *);
    extern NUVEC v000;
    extern NUVEC v001;
    f32 NuVecNorm(NUVEC *out, NUVEC *input);
    void NuVecSurfaceNormal(NUVEC *out, NUVEC *p0, NUVEC *p1, NUVEC *p2);
    f32 NuVecDistSqr(NUVEC *v0, NUVEC *v1, NUVEC *delta);
    f32 NuVecXZDistSqr(NUVEC *v0, NUVEC *v1, NUVEC *delta);
    void NuVecScaleAccum(NUVEC *out, NUVEC *input, f32 scale);
    void NuVecInvScale(NUVEC *out, NUVEC *input, f32 scale);
    void NuVecCrossRel(NUVEC *out, NUVEC *base, NUVEC *p0, NUVEC *p1);
    f32 NuVecMagXZ(NUVEC *input);
    void NuVecRotateX(NUVEC *out, NUVEC *input, NUANG angle);
    void NuVecRotateY(NUVEC *out, NUVEC *input, NUANG angle);
    void NuVecRotateZ(NUVEC *out, NUVEC *input, NUANG angle);
    void NuVecRotateYValZ(NUVEC *out, f32 magnitude, NUANG angle);
    void NuVecAdd(NUVEC *dst, const NUVEC *a, const NUVEC *b);
    void NuVecSub(NUVEC *dst, NUVEC *a, NUVEC *b);
    void NuVecScale(NUVEC *dst, NUVEC *src, f32 scale);
    void NuVecAddScale(NUVEC *dst, NUVEC *a, NUVEC *b, f32 scale);
    f32 NuVecDot(NUVEC *a, NUVEC *b);
    void NuVecNeg(NUVEC *dst, NUVEC *src);
    void NuVecCross(NUVEC *dst, const NUVEC *a, const NUVEC *b);
    void NuVecMax(NUVEC *dst, const NUVEC *a, const NUVEC *b);
    void NuVecMin(NUVEC *dst, const NUVEC *a, const NUVEC *b);
    f32 NuVecMagSqr(NUVEC *vec);
    f32 NuVecMag(NUVEC *vec);
    f32 NuVecDist(NUVEC *v0, NUVEC *v1, NUVEC *dist);
    void NuVecLerp(NUVEC *dst, NUVEC *a, NUVEC *b, f32 t);
    f32 NuVecXZDist(NUVEC *, NUVEC *, NUVEC *);

    extern NUVEC v100;
    extern NUVEC v010;
    i32 NuPointRelToBoundingBox(NUVEC *, NUVEC *, NUVEC *);
    void NuClipXPlane(NUVEC *, NUVEC *, NUVEC *, f32 *);
    void NuClipYPlane(NUVEC *, NUVEC *, NUVEC *, f32 *);
    void NuClipZPlane(NUVEC *, NUVEC *, NUVEC *, f32 *);
    void NuVecRotateYValX(NUVEC *out, f32 value, NUANG angle);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void NuVecMtxTransformVU0(NUVEC *, NUVEC *, struct numtx_s *);
    void NuVecMtxRotate(NUVEC *dst, NUVEC *src, struct numtx_s *matrix);
    void NuVecMtxTransform(NUVEC *dst, const NUVEC *src, const struct numtx_s *matrix);
    void NuVecInvMtxRotate(NUVEC *dst, NUVEC *src, struct numtx_s *matrix);
    void NuVecInvMtxTransform(NUVEC *dst, NUVEC *src, struct numtx_s *matrix);
    void NuVecMtxTransformBlock(NUVEC *, NUVEC *, struct numtx_s *, i32);
    f32 NuVecMtxTransformH(NUVEC *, NUVEC *, struct numtx_s *);
    i32 BoundingBoxToLine(NUVEC *, NUVEC *, struct numtx_s *, NUVEC *, NUVEC *, f32, NUVEC *);
    void NuVecMtxTranslate(NUVEC *out, NUVEC *v, struct numtx_s *m);
    void NuVecMtxRotateValX(NUVEC *out, f32 value, struct numtx_s *matrix);
    void NuVecMtxRotateValY(NUVEC *out, f32 value, struct numtx_s *matrix);
    void NuVecMtxRotateValZ(NUVEC *out, f32 value, struct numtx_s *matrix);
    void NuVecMtxRotateH(NUVEC *out, NUVEC *v, struct numtx_s *m);
    void NuVecMtxScale(NUVEC *out, NUVEC *v, struct numtx_s *m);
    void NuVecInvMtxTranslate(NUVEC *out, NUVEC *v, struct numtx_s *matrix);
    void NuVecInvMtxRotateValX(NUVEC *out, f32 unused, struct numtx_s *matrix);
    void NuVecInvMtxRotateValY(NUVEC *out, f32 unused, struct numtx_s *matrix);
    void NuVecInvMtxRotateValZ(NUVEC *out, f32 unused, struct numtx_s *matrix);
    void NuVecInvMtxScale(NUVEC *out, NUVEC *v, struct numtx_s *matrix);
#ifdef __cplusplus
}
#endif

typedef struct OBJECT_LIGHT_STATE_GAME {
    NUVEC ambient;
    NUVEC intensity[3];
    NUVEC direction[3];
} OBJECT_LIGHT_STATE_GAME;

typedef struct cut_load_bounds_s { // size 0x18
    NUVEC min;                     // 0x00
    NUVEC max;                     // 0x0c
} CUT_LOAD_BOUNDS;

typedef struct BURN_OUT { // size 0x24
    i32 active;           // 0x00
    NUVEC position;       // 0x04
    f32 threshold;        // 0x10
    f32 intensity;        // 0x14
    f32 flare;            // 0x18
    f32 radius;           // 0x1c
    f32 falloff;          // 0x20
} BURN_OUT;

typedef struct {
    u8 unknown_00[0x34];
    NUVEC position;
} FLOW_BUILDIT_POSITION;

typedef struct GAME_ANTI_NODE_DATA {
    NUVEC position;
    f32 radius;
    f32 min_y;
    f32 max_y;
    f32 extent_x;
    f32 extent_z;
    u16 angle;
    u8 shape;
    u8 mode;
} GAME_ANTI_NODE_DATA;

struct SHADOW_PLAYER_BOUNDS {
    u8 unknown_000[0x5fc];
    NUVEC center;
    f32 radius;
};

typedef struct grapple_rotation_s { // size 0x3c
    u8 unknown_00[0x30];            // 0x00
    NUVEC hook_position;            // 0x30
} GRAPPLE_ROTATION;

typedef struct player_position_record_s { // size 0x10
    NUVEC *position;                      // 0x0
    i32 sock_location;                    // 0x4
    f32 sock_ratio;                       // 0x8
    i16 angle;                            // 0xc
    u8 unknown_0e[2];                     // 0xe
} PLAYER_POSITION_RECORD;
