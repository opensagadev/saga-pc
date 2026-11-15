#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    void DropTorpedoPickups(TORPEDOPACKET *, i32);
    void Torpedo_Shoot(GAMEOBJECT *);
    void HomeNearestTorpTarget(BOLT *, TORPEDOPACKET *);
    void Torpedo_Ricochet(BOLT *, TORPEDOPACKET *);
    void Torpedo_InitBolt(BOLT *);
    void Torpedo_UpdateBolt(BOLT *);
    i32 getMaxTorpedos(GAMEOBJECT *);
    void FreeTorpedoPacket(TORPEDOPACKET **);
    void *FindNearestTorpTarget(WORLDINFO *, NUVEC *, f32, u8 *);
    void TorpedoCode(GAMEOBJECT *, i32, f32);
    void Torpedo_UpdateJobbies(GAMEOBJECT *);
    extern u8 TorpedoFillEnabled;
    extern f32 TorpedoGrabRangeSquared;
    extern f32 TorpedoStealTimer;
    extern f32 TorpedoStealRange;
    extern char dat_00759770[2];
    TORPEDOPACKET *GetTorpedoPacket(void);
    void Torpedo_InitRicochet(BOLT *, NUVEC *);
    void Torpedo_EndBolt(BOLT *);
    f32 Torpedo_Scale(BOLT *);
    void TorpedoHitTarget(BOLT *);
    void Torpedo_EndBolt(BOLT *);
#ifdef __cplusplus
}
#endif
