#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"

#ifdef __cplusplus
extern "C" {
#endif
    void AddShoveObject(nuhspecial_s *, i16);
    void ShoveObjectSysReset(void);
    void ShoveSystemCheckGameObject(GAMEOBJECT *);
    extern f32 ShoveMaximumSpeed;
    extern f32 ShoveMaximumPenetration;
#ifdef __cplusplus
}
#endif
