#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct addgizmotype_s;

typedef struct edgizshadow_s { // size 0x38
    NUVEC direction;           // 0x00
    f32 secondary_strength;    // 0x0c
    f32 secondary_spread;      // 0x10
    f32 secondary_falloff;     // 0x14
    f32 shadow_bias;           // 0x18
    f32 shadow_softness;       // 0x1c
    f32 shadow_distance;       // 0x20
    f32 coefficient_0;         // 0x24
    f32 coefficient_1;         // 0x28
    f32 coefficient_2;         // 0x2c
    f32 height_offset;         // 0x30
    i32 preset;                // 0x34
} EDGIZSHADOW;

#ifdef __cplusplus
extern "C" {
#endif
    struct addgizmotype_s *EdGizShadowMachine_RegisterGizmo(i32 type_id);
#ifdef __cplusplus
}
#endif
