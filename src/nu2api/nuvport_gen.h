#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/render/fx/game_deb.h"

#ifdef __cplusplus
extern "C" {
#endif
    void NuRndrSetBurnoutParameters(i32, const BURN_PARAMETERS *);
    void NuRndrSetShadowCoefficient(i32, f32);
#ifdef __cplusplus
}
#endif
