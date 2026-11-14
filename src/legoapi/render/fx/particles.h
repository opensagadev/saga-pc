#ifndef SAGA_LEGOAPI_RENDER_FX_PARTICLES_H
#define SAGA_LEGOAPI_RENDER_FX_PARTICLES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/world/world.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/world/world.h"
#endif

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

#endif
