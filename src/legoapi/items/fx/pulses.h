#ifndef SAGA_LEGOAPI_ITEMS_FX_PULSES_H
#define SAGA_LEGOAPI_ITEMS_FX_PULSES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/fixed_width.h"

typedef struct level_sfx_pulse_prefix_s LEVEL_SFX_PULSE_PC;
#ifdef __cplusplus
extern "C" {
#endif
    void Pulses_AddSfx(LEVEL_SFX_PULSE_PC *, i32 *, i32 *, i32);
    void Pulses_Reset(LEVEL_SFX_PULSE_PC *);
    void Pulses_Update(LEVEL_SFX_PULSE_PC *);
#ifdef __cplusplus
}
#endif

#endif
