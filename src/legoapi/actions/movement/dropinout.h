#pragma once

#include "nu2api/nucore/common.h"


struct gameobject_s;

#ifdef __cplusplus
extern "C" {
#endif

    void DropInOutCode(struct gameobject_s *);
    void DropOut(i32, i32, i32, i32);
    extern i32 PlayerDevicePorts[2];
    f32 DropInOutScale(struct gameobject_s *object);
    extern i8 DropInConfigurationPlayer;
    extern i8 DropInPendingPlayer;
#ifdef __cplusplus
}
#endif
