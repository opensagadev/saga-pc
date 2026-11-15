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
    void HomeNearestTorpTarget(BOLT_PC *, TORPEDOPACKET *);
    void Torpedo_Ricochet(BOLT_PC *, TORPEDOPACKET *);
    void Torpedo_InitBolt(BOLT_PC *);
    void Torpedo_UpdateBolt(BOLT_PC *);
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
    void Torpedo_InitRicochet(BOLT_PC *, NUVEC *);
    void Torpedo_EndBolt(BOLT_PC *);
    f32 Torpedo_Scale(BOLT_PC *);
    void TorpedoHitTarget(BOLT_PC *);
    void Torpedo_EndBolt(BOLT_PC *);
#ifdef __cplusplus
}
#endif
