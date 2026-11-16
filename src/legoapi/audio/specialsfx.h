#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/world/world.h"

#ifdef __cplusplus
extern "C" {
#endif
    void SetSpecialSfxBits(i32 *, i32 *, WORLDINFO *);
    i32 SpecialSfxLoad(char *, void *);
    void UpdateSpecialSfx(WORLDINFO *);
    void ParticleReset(void);
#ifdef __cplusplus
}
#endif
