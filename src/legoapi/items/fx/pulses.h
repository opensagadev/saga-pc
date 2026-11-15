#pragma once

#include "nu2api/nucore/common.h"

typedef struct level_sfx_pulse_prefix_s LEVEL_SFX_PULSE;
#ifdef __cplusplus
extern "C" {
#endif
    void Pulses_AddSfx(LEVEL_SFX_PULSE *, i32 *, i32 *, i32);
    void Pulses_Reset(LEVEL_SFX_PULSE *);
    void Pulses_Update(LEVEL_SFX_PULSE *);
#ifdef __cplusplus
}
#endif
