#pragma once

#include "nu2api/nucore/common.h"

struct numtl_s;

#ifdef __cplusplus
extern "C" {
#endif
    void edbitsDrawDiagonalCross(f32 x, f32 y, f32 z, f32 radius, i32 colour, struct numtl_s *material);
    void edbitsDrawCross(f32 x, f32 y, f32 z, f32 radius, i32 colour, struct numtl_s *material);
#ifdef __cplusplus
}
#endif
