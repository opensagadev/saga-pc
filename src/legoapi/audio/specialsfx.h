#ifndef SAGA_LEGOAPI_AUDIO_SPECIALSFX_H
#define SAGA_LEGOAPI_AUDIO_SPECIALSFX_H

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
    void SetSpecialSfxBits(i32 *, i32 *, WORLDINFO *);
    i32 SpecialSfxLoad(char *, void *);
    void UpdateSpecialSfx(WORLDINFO *);
#ifdef __cplusplus
}
#endif

#endif
