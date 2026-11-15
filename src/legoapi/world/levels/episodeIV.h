#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 MosEisleyC_PastBarrier(GAMEOBJECT *);
    extern f32 VehicleLoopTime;
    extern i8 *DeathStarEscapeBNetPacket;
    extern const char DeathStarEscapeBSponge[7];
    extern const char DeathStarEscapeBLiftDown[10];
    extern const char DeathStarEscapeBLiftUp[8];
    i32 DeathStarShieldDown(void);
#ifdef __cplusplus
}
#endif
