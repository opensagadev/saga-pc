#ifndef SAGA_LEGOAPI_WORLD_LEVELS_EPISODEV_H
#define SAGA_LEGOAPI_WORLD_LEVELS_EPISODEV_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/items/base/apiobject.h, legoapi/items/objects/objectsall.h,
 * legoapi/legoapi_types.h (not all of it earlier in the shim). */

#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/traps/gizbombgen.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#endif

typedef void (*PART_KILL_PC)(PART_PC *, i32);
#ifdef __cplusplus
extern "C" {
#endif
    extern i16 VehicleBoltId7f18b4Pc;
    i32 TakeOverYodaSeekDistanceHack(GAMEOBJECT *, GAMEOBJECT *, NUVEC *);
    extern i32 dagobah_training;
    extern u8 minikitCounter_C;
    extern u8 minikitCounter_A;
    extern const char AITransferLiteral758430[5];
    extern LEVELDATA_PC *LevelFixupSlotPc0087b250;
#ifdef __cplusplus
}
#endif

typedef struct HOTHBATTLEE_NETPACKET_PC {
    i16 targets[12];
    i8 defeated[12];
    f32 alpha[12];
    i32 count;
} HOTHBATTLEE_NETPACKET_PC;

STATIC_ASSERT(sizeof(HOTHBATTLEE_NETPACKET_PC) == 0x58);
STATIC_ASSERT(offsetof(HOTHBATTLEE_NETPACKET_PC, targets) == 0);
STATIC_ASSERT(offsetof(HOTHBATTLEE_NETPACKET_PC, defeated) == 0x18);
STATIC_ASSERT(offsetof(HOTHBATTLEE_NETPACKET_PC, alpha) == 0x24);
STATIC_ASSERT(offsetof(HOTHBATTLEE_NETPACKET_PC, count) == 0x54);
#ifdef __cplusplus
extern "C" {
#endif
    extern i16 gunship_bolts[2];
    void InitTrooperCannons(struct WORLDINFO_s *arg0);
    extern f32 spotLightA_yrot[2];
    extern f32 spotLightA_zrot[2];
    extern f32 spotLightB_yrot[2];
    extern f32 spotLightB_zrot[2];
    extern HOTHBATTLE_MELEE_s melee;
    extern u8 drawLights;
    extern struct GIZBOMBGEN_s *HothBattleC_BombGenerator;
    void HothBattleE_UpdateWave(void);
    void AsteroidChaseD_SetTurretTarget(i32 index, struct nuvec_s *position);
    extern struct GIZTURRET_s *StarDestroyerTurrets[16];
    extern nuhspecial_s escape[8];
    extern f32 melee_waveDelay;
    extern i8 melee_wavePhase;
    extern u8 turretAliveCount;
    void ResetTrooperCannons(struct WORLDINFO_s *arg0, i32 arg1);
    void UpdateTrooperCannons(struct WORLDINFO_s *arg0);
    extern struct gameobject_s *Vader_obj;
#ifdef __cplusplus
}
#endif

typedef struct TROOPERCANNON_PC {
    GIZMO_PC *base;
    GIZBUILDIT_s *buildit;
    GAMEOBJECT *object;
    char character_name[32];
    u8 rebuilding;
    u8 padding[3];
} TROOPERCANNON_PC;

STATIC_ASSERT(sizeof(TROOPERCANNON_PC) == 0x30);
STATIC_ASSERT(offsetof(TROOPERCANNON_PC, base) == 0);
STATIC_ASSERT(offsetof(TROOPERCANNON_PC, buildit) == 4);
STATIC_ASSERT(offsetof(TROOPERCANNON_PC, object) == 8);
STATIC_ASSERT(offsetof(TROOPERCANNON_PC, character_name) == 0xc);
STATIC_ASSERT(offsetof(TROOPERCANNON_PC, rebuilding) == 0x2c);
#ifdef __cplusplus
extern "C" {
#endif
    extern TROOPERCANNON_PC troopercannons[4];
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(sizeof(troopercannons) == 0xc0);

#endif
