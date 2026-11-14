#ifndef SAGA_LEGOAPI_ACTIONS_COMBAT_SHOVESYS_H
#define SAGA_LEGOAPI_ACTIONS_COMBAT_SHOVESYS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/items/base/apiobject.h, nu2api/nu3d/nu3d_includes.h (not all of it earlier
 * in the shim). */

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "nu2api/nu3d/nu3d_includes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void AddShoveObject(NUSPECIALHANDLE *, i16);
    void ShoveObjectSysReset(void);
    void ShoveSystemCheckGameObject(GAMEOBJECT *);
    extern f32 ShoveMaximumSpeedPc;
    extern f32 ShoveMaximumPenetrationPc;
#ifdef __cplusplus
}
#endif

#endif
