#ifndef SAGA_LEGOAPI_CHARACTERS_MOTION_CHARPLATFORMS_H
#define SAGA_LEGOAPI_CHARACTERS_MOTION_CHARPLATFORMS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    GAMEOBJECT *CharPlatform_FindObjFromPlatID(void *system, i32 platform_id);
#ifdef __cplusplus
}
#endif

#endif
