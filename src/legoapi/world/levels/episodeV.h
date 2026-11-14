/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/numath/nuvec.h"
#endif

struct PART_PC;
struct WORLDINFO_s;
struct gameobject_s;
struct gizmo_s;
struct level_data_s;

typedef void (*PART_KILL_PC)(struct PART_PC *, i32);
#ifdef __cplusplus
extern "C" {
#endif
    extern i16 dat_007f18b4;
    i32 TakeOverYodaSeekDistanceHack(struct gameobject_s *, struct gameobject_s *, NUVEC *);
    extern i32 dagobah_training;
    extern u8 minikitCounter_C;
    extern u8 minikitCounter_A;
    extern const char AITransferLiteral758430[5];
    extern struct level_data_s *dat_0087b250;
#ifdef __cplusplus
}
#endif

typedef struct HOTHBATTLEE_NETPACKET { // size 0x58
    i16 targets[12];                   // 0x00
    i8 defeated[12];                   // 0x18
    f32 alpha[12];                     // 0x24
    i32 count;                         // 0x54
} HOTHBATTLEE_NETPACKET;

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

typedef struct TROOPERCANNON {   // size 0x30
    struct gizmo_s *base;        // 0x00
    GIZBUILDIT_s *buildit;       // 0x04
    struct gameobject_s *object; // 0x08
    char character_name[32];     // 0x0c
    u8 rebuilding;               // 0x2c
    u8 padding[3];               // 0x2d
} TROOPERCANNON;

#ifdef __cplusplus
extern "C" {
#endif
    extern TROOPERCANNON troopercannons[4];
#ifdef __cplusplus
}
#endif
