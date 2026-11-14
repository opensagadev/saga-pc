/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/world/world.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void Particles_Stop(WORLDINFO *);
    void Particles_Start(WORLDINFO *);
    void SetEffectVisibility(char *name, i32 visible);
    void Particles_DumpAreaPage(void);
#ifdef __cplusplus
}
#endif
