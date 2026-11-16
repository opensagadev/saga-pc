#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"

struct CHARPLATFORMSYS_s;

#ifdef __cplusplus
extern "C" {
#endif
    GAMEOBJECT *CharPlatform_FindObjFromPlatID(void *system, i32 platform_id);
    void CharPlatforms_Reset(struct CHARPLATFORMSYS_s *system);
    void CharPlatforms_Update(struct CHARPLATFORMSYS_s *system);
#ifdef __cplusplus
}
#endif
