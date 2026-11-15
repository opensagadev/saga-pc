#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 ObjHitObj(GAMEOBJECT *, GAMEOBJECT *, i32, u16, i32, i32);
    i32 NoLayerKill(GAMEOBJECT *);
    u16 ObjHitObj_Flags(GAMEOBJECT *);
    i32 CannotKill(GAMEOBJECT *);
    void RegenerateHearts(GAMEOBJECT *);
#ifdef __cplusplus
}
#endif
