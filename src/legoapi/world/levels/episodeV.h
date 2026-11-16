#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/numath/nuvec.h"

struct GIZBUILDIT_s;
struct PART;
struct WORLDINFO_s;
struct gameobject_s;
struct gizmo_s;
struct level_data_s;

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

typedef struct HOTHBATTLE_MELEE_s HOTHBATTLE_MELEE_s;

typedef struct HOTHBATTLE_MELEE_WAVE_s HOTHBATTLE_MELEE_WAVE_s;

struct HOTHBATTLE_MELEE_WAVE_s {       // size 0x28
    i16 character_id;                  // 0x00
    u8 reserved_02[2];                 // 0x02
    struct gameobject_s *creatures[4]; // 0x04

    union { // size 0x1
        u8 remaining_count;
        u8 unknown_18;
    }; // 0x14

    union { // size 0x1
        u8 initial_count;
        u8 unknown_19;
    }; // 0x15

    union { // size 0x1
        u8 active_count;
        u8 reserved_1a;
    }; // 0x16

    char name[13];     // 0x17
    u8 reserved_24[4]; // 0x24
};

struct HOTHBATTLE_MELEE_s { // size 0xc8

    union { // size 0x1
        u8 current_wave;
        u8 unknown_0;
    }; // 0x00

    union { // size 0x1
        u8 next_wave;
        u8 unknown_1;
    }; // 0x01

    union { // size 0x1
        u8 initialize_wave;
        u8 unknown_2;
    }; // 0x02

    u8 unknown_3; // 0x03

    union { // size 0x1
        i8 transition_phase;
        i8 unknown_4;
    }; // 0x04

    u8 unknown_5[3];                              // 0x05
    f32 wave_delay;                               // 0x08
    struct HOTHBATTLE_MELEE_WAVE_s waves[4];      // 0x0c
    struct gameobject_s *background_creatures[6]; // 0xac
    u8 creature_count;                            // 0xc4
    u8 reserved_0xc5[3];                          // 0xc5
};

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

typedef struct TROOPERCANNON {    // size 0x30
    struct gizmo_s *base;         // 0x00
    struct GIZBUILDIT_s *buildit; // 0x04
    struct gameobject_s *object;  // 0x08
    char character_name[32];      // 0x0c
    u8 rebuilding;                // 0x2c
    u8 padding[3];                // 0x2d
} TROOPERCANNON;

#ifdef __cplusplus
extern "C" {
#endif
    extern TROOPERCANNON troopercannons[4];
    extern u32 LEGO_AIPATHCNX_BIGJUMP;
#ifdef __cplusplus
}
#endif
