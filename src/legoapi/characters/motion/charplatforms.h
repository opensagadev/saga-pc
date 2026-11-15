#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"

#ifdef __cplusplus
extern "C" {
#endif
    GAMEOBJECT *CharPlatform_FindObjFromPlatID(void *system, i32 platform_id);
#ifdef __cplusplus
}
#endif
