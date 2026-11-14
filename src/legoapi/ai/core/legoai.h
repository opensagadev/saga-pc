/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/core/config/cheats.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 LEGO_AIPATHCNX_BLOCKAGE;
    extern i32 LEGO_AIPATHCNX_WALLSHUFFLE;
    extern i32 LEGO_AIPATHCNX_FULLTERRAIN;
    extern i32 LEGO_AIPATHCNX_BLOCKAGE;
    extern i32 LEGO_AIPATHCNX_WALLSHUFFLE;
    extern i32 LEGO_AIPATHCNX_FULLTERRAIN;
    extern CHEATSYSTEM_PC CheatSystem;
    extern u32 LEGO_AIPATHCNX_FORGOODIES;
    extern u32 LEGO_AIPATHCNX_FORBADDIES;
    extern i32 LEGO_AIPATHCNX_R2D2GLIDE;
    extern i32 LEGO_AIPATHCNX_JUMP;
    extern i32 LEGO_AIPATHCNX_DONTTOGGLE;
#ifdef __cplusplus
}
#endif
