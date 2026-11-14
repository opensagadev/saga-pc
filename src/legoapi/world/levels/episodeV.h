#ifndef SAGA_LEGOAPI_WORLD_LEVELS_EPISODEV_H
#define SAGA_LEGOAPI_WORLD_LEVELS_EPISODEV_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/numath/nuvec.h"
#endif

struct PART_PC;
struct WORLDINFO_s;
struct gameobject_s;
struct gizmo_pc_s;
struct level_data_pc_s;

typedef void (*PART_KILL_PC)(struct PART_PC *, i32);
#ifdef __cplusplus
extern "C" {
#endif
    extern i16 VehicleBoltId7f18b4Pc;
    i32 TakeOverYodaSeekDistanceHack(struct gameobject_s *, struct gameobject_s *, NUVEC *);
    extern i32 dagobah_training;
    extern u8 minikitCounter_C;
    extern u8 minikitCounter_A;
    extern const char AITransferLiteral758430[5];
    extern struct level_data_pc_s *LevelFixupSlotPc0087b250;
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
    struct gizmo_pc_s *base;
    GIZBUILDIT_s *buildit;
    struct gameobject_s *object;
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
