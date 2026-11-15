#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct debris_info_s;
struct debris_key_s;
struct debris_part_header_s;
struct debris_particle_s;
struct gameobject_s;
struct numtl_s;
struct numtx_s;
struct ripple_set_s;

typedef struct BURN_PARAMETERS { // size 0x54
    i32 unknown_00;              // 0x00
    f32 unknown_04;              // 0x04
    f32 unknown_08;              // 0x08
    f32 unknown_0c;              // 0x0c
    f32 unknown_10;              // 0x10
    f32 unknown_14;              // 0x14
    i32 unknown_18;              // 0x18
    f32 unknown_1c;              // 0x1c
    f32 unknown_20;              // 0x20
    f32 unknown_24;              // 0x24
    i32 unknown_28;              // 0x28
    f32 unknown_2c;              // 0x2c
    f32 unknown_30;              // 0x30
    f32 unknown_34;              // 0x34
    f32 unknown_38;              // 0x38
    f32 unknown_3c;              // 0x3c
    f32 unknown_40;              // 0x40
    f32 unknown_44;              // 0x44
    f32 unknown_48;              // 0x48
    f32 unknown_4c;              // 0x4c
    f32 unknown_50;              // 0x50
} BURN_PARAMETERS;

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct PC_GLOBAL_LIGHTING_STATE { // size 0x78
        NUVEC ambient;                        // 0x00
        NUVEC direction[3];                   // 0x0c
        NUVEC intensity[3];                   // 0x30
        NUVEC specular_direction;             // 0x54
        i32 specular_direction_missing;       // 0x60
        u8 unknown_64[0x10];                  // 0x64
        i32 specular_intensity_present;       // 0x74
    } PC_GLOBAL_LIGHTING_STATE;

    void DebrisDraw(i32, i32);
    extern i32 g_renderingDebris;
    extern i32 DebrisSuspendDrawObjectSwitch;
    void DebrisFreeOldestDmaDebTypeTable(void);
    extern f32 debris_thinning_level;
    extern i32 forced_debris_thinning;
    extern i32 freeDmaDebType;
    extern i32 EDPP_MAX_DMADEBTYPES;
    extern struct debris_part_header_s **DmaDebTypes;
    extern i32 debris_render_group;
    void DebrisProcessTriggers(void);
    void DebrisProcessGeneration(void);
    extern i32 debris_detail_level;
    extern u32 debrisseed;
    void DebrisSetCutSceneMode(i32 enabled);
    void DebrisSetSeed(i32 seed);
    u32 DebrisGetSeed(void);
    void DebrisSetTimeIncrement(f32 increment);
    void Debris(i32);
    void BuildRippleMtx(struct numtx_s *, NUVEC *, NUVEC *, u16, u16);

    extern struct ripple_set_s *RippleSet;
    extern f32 RipplePerturbation;
    extern f32 RippleSfxCooldown;
    extern f32 ForceFieldRippleRate;
    extern const char RippleForceFieldSoundNamePc[10];
    extern f32 GunshipDustEmissionRate;
    void InitRippleMtl(char *, struct numtl_s **, VARIPTR *, VARIPTR *);
    void BurnoutApply(i32);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void AddWaterSplash(struct gameobject_s *object, NUVEC *position);
    void AddSlamDebris(struct gameobject_s *);
    void AddSurfaceRipples(struct gameobject_s *);
    void AddSurfaceDebris(struct gameobject_s *);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern struct debris_info_s **debtab;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void DebrisStartOffsetEx(struct debris_key_s *, f32);
    void DebrisProcessSpheres(struct debris_particle_s *, f32, struct debris_info_s *, struct debris_key_s *, i32);
    void DebReAlloc(struct debris_key_s *, i32);
#ifdef __cplusplus
}
#endif
