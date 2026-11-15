#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/collect/bolts.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 GizmoSys_BoltHit(void *, WORLDINFO *, BOLT *, NUVEC *, NUVEC *, NUVEC *, f32, u8 *);
#ifdef __cplusplus
}
#endif
