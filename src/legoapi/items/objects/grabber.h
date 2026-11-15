#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/world/level.h"
#include "legoapi/world/world.h"

#ifdef __cplusplus
extern "C" {
#endif
    void Grabber_StoreProgress(WORLDINFO *world, LEVEL_PROGRESS *progress);

    extern i16 GrabMachineId;
    extern i16 GrabMagnetId;
    extern i16 RobotBaseId;
    void Grabber_Configure(WORLDINFO *, char *);
#ifdef __cplusplus
}
#endif
