#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct numtx_s;

typedef struct NUVEC4 { // size 0x10

    union { // size 0xc

        struct {   // size 0xc
            f32 x; // 0x0
            f32 y; // 0x4
            f32 z; // 0x8
        };

        NUVEC xyz;
    }; // 0x0

    f32 w; // 0xc
#ifdef __cplusplus
    NUVEC operator-(const NUVEC4 &other) const {
        __declspec(align(16)) NUVEC result;
        result.x = x - other.x;
        result.y = y - other.y;
        result.z = z - other.z;
        return result;
    }

    void fun_00595250(f32 new_x, f32 new_y, f32 new_z, f32 new_w);
    void fun_00595270();
    NUVEC4 &fun_00595290(const NUVEC &v);
#endif
} NUVEC4;

typedef struct NUHALFVEC4 { // size 0x8
    i16 x;                  // 0x0
    i16 y;                  // 0x2
    i16 z;                  // 0x4
    i16 w;                  // 0x6
} NUHALFVEC4;

#ifdef __cplusplus
extern "C" {
#endif
    void NuVec4MtxRotateVU0(NUVEC4 *, NUVEC4 *, struct numtx_s *);
    void NuVec4MtxTransformVU0x3(NUVEC4 *out, NUVEC4 *in, struct numtx_s *matrix);
    void NuVec4MtxTransform(NUVEC4 *, const NUVEC *, const struct numtx_s *);
    void NuVec4MtxTransformVU0(NUVEC4 *, NUVEC4 *, struct numtx_s *);
    void NuVec4MtxTransformH(NUVEC4 *, const NUVEC4 *, const struct numtx_s *);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
    void NuVec4Add(NUVEC4 *, NUVEC4 *, NUVEC4 *);
    f32 NuVec4Dot(NUVEC4 *v0, NUVEC4 *v1);
    void NuVec4ScaleAccum(NUVEC4 *out, NUVEC4 *v, f32 scale);
    void NuVec4Sub(NUVEC4 *out, NUVEC4 *a, NUVEC4 *b);
    void NuVec4Max(NUVEC4 *out, NUVEC4 *a, NUVEC4 *b);
    void NuVec4Min(NUVEC4 *out, NUVEC4 *a, NUVEC4 *b);
    f32 NuVec4Mag(NUVEC4 *v);
    f32 NuVec4MagSqr(NUVEC4 *v);
    void NuVec4Lerp(NUVEC4 *out, NUVEC4 *a, NUVEC4 *b, f32 t);
    void NuVec4ToNuHalfVec4(NUVEC4 *v, NUHALFVEC4 *out);
    void NuHalfVec4ToNuVec4(NUHALFVEC4 *v, NUVEC4 *out);
    void NuVec4MtxTransformHVU0(struct NUVEC4 *out, struct NUVEC4 *v, struct numtx_s *matrix);
    void NuVec4Scale(NUVEC4 *, NUVEC4 *, f32);
    void NuVec4ScaleXYZVU0(NUVEC4 *, NUVEC4 *, f32);
#ifdef __cplusplus
}
#endif
