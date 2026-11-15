#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"

#ifdef __cplusplus
extern "C" {
#endif
    void ReleasePush(GAMEOBJECT *);
    extern i32 PlayerPushSpinnerDegrees;
    void PushCode(GAMEOBJECT *, i32);
#ifdef __cplusplus
}
#endif
