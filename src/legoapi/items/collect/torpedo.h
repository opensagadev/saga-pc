/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void DropTorpedoPickups(TORPEDOPACKET_PC *, i32);
    void Torpedo_Shoot(GAMEOBJECT *);
    void HomeNearestTorpTarget(BOLT_PC *, TORPEDOPACKET_PC *);
    void Torpedo_Ricochet(BOLT_PC *, TORPEDOPACKET_PC *);
    void Torpedo_InitBolt(BOLT_PC *);
    void Torpedo_UpdateBolt(BOLT_PC *);
    i32 getMaxTorpedos(GAMEOBJECT *);
    void FreeTorpedoPacket(TORPEDOPACKET_PC **);
    void *FindNearestTorpTarget(WORLDINFO *, NUVEC *, f32, u8 *);
    void TorpedoCode(GAMEOBJECT *, i32, f32);
    void Torpedo_UpdateJobbies(GAMEOBJECT *);
    extern u8 TorpedoFillEnabledPc;
    extern f32 TorpedoGrabRangeSquaredPc;
    extern f32 TorpedoStealTimerPc;
    extern f32 TorpedoStealRangePc;
    extern char TextInputKeyNameTextPc759770[2];
    TORPEDOPACKET_PC *GetTorpedoPacket(void);
    void Torpedo_InitRicochet(BOLT_PC *, NUVEC *);
    void Torpedo_EndBolt(BOLT_PC *);
    f32 Torpedo_Scale(BOLT_PC *);
    void TorpedoHitTarget(BOLT_PC *);
    void Torpedo_EndBolt(BOLT_PC *);
#ifdef __cplusplus
}
#endif
