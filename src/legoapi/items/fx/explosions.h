#ifndef SAGA_LEGOAPI_ITEMS_FX_EXPLOSIONS_H
#define SAGA_LEGOAPI_ITEMS_FX_EXPLOSIONS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h, legoapi/legoapi_types.h (not all of it earlier in
 * the shim). */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 i_explosion;

    EXPLOSION_PC *AddExplosion(NUVEC *, f32, f32, GAMEOBJECT *, i32, u32);
    void ResetExplosions(void);
    void DrawExplosions(void);
    void UpdateExplosions(void);
    void UpdateExplosion_Generic(EXPLOSION_PC *);
    extern f32 ExplosionKnockbackScalePc;
#ifdef __cplusplus
}
#endif

#endif
