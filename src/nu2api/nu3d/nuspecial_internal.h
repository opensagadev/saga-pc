/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

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

struct NuSpecialLegacyInstanceBoundsLayout {
    u8 pad_00[64];
    i16 object_index;
};

STATIC_ASSERT(sizeof(struct NuSpecialLegacyInstanceBoundsLayout) == 66);

struct NuSpecialLegacyRuntimeLayout {
    u8 pad_00[64];
    u8 *instance;
    char *name;
    u32 flags;
};

STATIC_ASSERT(sizeof(struct NuSpecialLegacyRuntimeLayout) == 76);
