#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 nuspecial_const_alpha;
    extern NUVEC nuspecial_const_tint;
    extern i32 nuspecial_const_alpha_enabled;
    extern i32 nuspecial_const_tint_enabled;
#ifdef __cplusplus
}
#endif
typedef struct NuSpecialLegacyInstanceBoundsLayout NuSpecialLegacyInstanceBoundsLayout;
typedef struct NuSpecialLegacyRuntimeLayout NuSpecialLegacyRuntimeLayout;

struct NuSpecialLegacyInstanceBoundsLayout { // size 0x42
    u8 pad_00[64];                           // 0x00
    i16 object_index;                        // 0x40
};

struct NuSpecialLegacyRuntimeLayout { // size 0x4c
    u8 pad_00[64];                    // 0x00
    u8 *instance;                     // 0x40
    char *name;                       // 0x44
    u32 flags;                        // 0x48
};
