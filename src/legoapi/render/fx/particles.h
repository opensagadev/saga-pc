#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/world/world.h"

#ifdef __cplusplus
extern "C" {
#endif
    void Particles_Stop(WORLDINFO *);
    void Particles_Start(WORLDINFO *);
    void SetEffectVisibility(char *name, i32 visible);
    void Particles_DumpAreaPage(void);
    void Particles_LoadAreaPage(char *);
#ifdef __cplusplus
}
#endif
