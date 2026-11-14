#ifndef SAGA_LEGOAPI_ITEMS_FX_EXPLOSIONS_H
#define SAGA_LEGOAPI_ITEMS_FX_EXPLOSIONS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"
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
