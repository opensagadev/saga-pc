#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nugscn.h"

#ifdef __cplusplus
extern "C" {
#endif
    void PortalGameObject(GAMEOBJECT *, i32, i32, i16, NUGSCN *);
    extern const char *PortalRegistrationPrefix;
    extern i32 portal_gizmotype_id;
    PORTAL_REGISTRATION *Portal_RegisterGizmo(i32 type_id);
    void PortalDoors_Reset(WORLDINFO *world);
    extern const char *PortalRegistrationPrefix;
    extern i32 portal_gizmotype_id;
#ifdef __cplusplus
}
#endif

enum { PORTALDOOR_CLOSED = 4 };

enum { PORTALDOOR_OPENED = 2 };

enum { PORTALDOOR_TRIGGER_AT_END = 1 };
