/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_PART
#define SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

/* 4 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_PART == 1 && !defined(SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_STOP)

struct debris_info_s;
struct debris_key_s;
struct debris_part_header_s;
struct debris_particle_s;
struct gameobject_s;
struct numtl_s;
struct numtx_s;
struct ripple_set_s;

typedef struct BURN_PARAMETERS_PC { // size 0x54
    i32 field_00;                   // 0x00
    f32 field_04;                   // 0x04
    f32 field_08;                   // 0x08
    f32 field_0c;                   // 0x0c
    f32 field_10;                   // 0x10
    f32 field_14;                   // 0x14
    i32 field_18;                   // 0x18
    f32 field_1c;                   // 0x1c
    f32 field_20;                   // 0x20
    f32 field_24;                   // 0x24
    i32 field_28;                   // 0x28
    f32 field_2c;                   // 0x2c
    f32 field_30;                   // 0x30
    f32 field_34;                   // 0x34
    f32 field_38;                   // 0x38
    f32 field_3c;                   // 0x3c
    f32 field_40;                   // 0x40
    f32 field_44;                   // 0x44
    f32 field_48;                   // 0x48
    f32 field_4c;                   // 0x4c
    f32 field_50;                   // 0x50
} BURN_PARAMETERS_PC;

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

    extern struct ripple_set_s *RippleSetPc;
    extern f32 RipplePerturbationPc;
    extern f32 RippleSfxCooldownPc;
    extern f32 ForceFieldRippleRatePc;
    extern const char RippleForceFieldSoundNamePc[10];
    extern f32 GunshipDustEmissionRatePc;
    void InitRippleMtl(char *, struct numtl_s **, VARIPTR *, VARIPTR *);
    void BurnoutApply(i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_PART
#define SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_PART == 2 && !defined(SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_STOP)
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
#undef SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_PART
#define SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_PART == 3 && !defined(SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern struct debris_info_s **debtab;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_PART
#define SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_PART == 4 && !defined(SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void DebrisStartOffsetEx(struct debris_key_s *, f32);
    void DebrisProcessSpheres(struct debris_particle_s *, f32, struct debris_info_s *, struct debris_key_s *, i32);
    void DebReAlloc(struct debris_key_s *, i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_PART
#define SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_STOP
