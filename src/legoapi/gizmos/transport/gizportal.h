#ifndef SAGA_LEGOAPI_GIZMOS_TRANSPORT_GIZPORTAL_H
#define SAGA_LEGOAPI_GIZMOS_TRANSPORT_GIZPORTAL_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nugscn.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void PortalGameObject(GAMEOBJECT *, i32, i32, i16, NUGSCN *);
    extern const char *PortalRegistrationPrefixPc;
    extern i32 portal_gizmotype_id;
    PORTAL_REGISTRATION_PC *Portal_RegisterGizmo(i32 type_id);
    void PortalDoors_Reset(WORLDINFO *world);
    extern const char *PortalRegistrationPrefixPc;
    extern i32 portal_gizmotype_id;
#ifdef __cplusplus
}
#endif

enum { PORTALDOOR_CLOSED = 4 };

enum { PORTALDOOR_OPENED = 2 };

enum { PORTALDOOR_TRIGGER_AT_END = 1 };

#endif
