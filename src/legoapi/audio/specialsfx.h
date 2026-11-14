#ifndef SAGA_LEGOAPI_AUDIO_SPECIALSFX_H
#define SAGA_LEGOAPI_AUDIO_SPECIALSFX_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "legoapi/world/world.h"
#include "nu2api/nucore/fixed_width.h"

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
