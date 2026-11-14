#ifndef SAGA_LEGOAPI_CHARACTERS_CORE_PLAYERITEMS_H
#define SAGA_LEGOAPI_CHARACTERS_CORE_PLAYERITEMS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/collect/bolt.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 PlayerItem_GotAmmo(WEAPON_AMMO_PC *);
    i32 ReleaseHearts(void);
#ifdef __cplusplus
}
#endif

#endif
