#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 i_explosion;

    EXPLOSION *AddExplosion(NUVEC *, f32, f32, GAMEOBJECT *, i32, u32);
    void ResetExplosions(void);
    void DrawExplosions(void);
    void UpdateExplosions(void);
    void UpdateExplosion_Generic(EXPLOSION *);
    extern f32 ExplosionKnockbackScale;
#ifdef __cplusplus
}
#endif
