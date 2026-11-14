#ifndef SAGA_LEGOAPI_RENDER_FX_PARTICLES_H
#define SAGA_LEGOAPI_RENDER_FX_PARTICLES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

#include "legoapi/world/world.h"

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
