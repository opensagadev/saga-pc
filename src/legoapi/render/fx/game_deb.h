#ifndef SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_PART
#define SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: gameapi/edptl.h, gameapi/edtools/edgra.h, legoapi/items/base/apiobject.h,
 * legoapi/legoapi_types.h, legoapi/render/core/gameliball.h, nu2api/nu3d/numtl.h, nu2api/numath/numtx.h,
 * nu2api/numath/nuvec.h, nu2api/nustencilshadows_PC.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "gameapi/edptl.h"
#include "gameapi/edtools/edgra.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/core/gameliball.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nustencilshadows_PC.h"
#endif
/* 4 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_PART == 1 && !defined(SAGA_LEGOAPI_RENDER_FX_GAME_DEB_H_STOP)

typedef struct BURN_PARAMETERS_PC {
    i32 field_00;
    f32 field_04;
    f32 field_08;
    f32 field_0c;
    f32 field_10;
    f32 field_14;
    i32 field_18;
    f32 field_1c;
    f32 field_20;
    f32 field_24;
    i32 field_28;
    f32 field_2c;
    f32 field_30;
    f32 field_34;
    f32 field_38;
    f32 field_3c;
    f32 field_40;
    f32 field_44;
    f32 field_48;
    f32 field_4c;
    f32 field_50;
} BURN_PARAMETERS_PC;

STATIC_ASSERT(sizeof(BURN_PARAMETERS_PC) == 0x54);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_14) == 0x14);
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct PC_GLOBAL_LIGHTING_STATE {
        NUVEC ambient;
        NUVEC direction[3];
        NUVEC intensity[3];
        NUVEC specular_direction;
        i32 specular_direction_missing;
        u8 unknown_64[0x10];
        i32 specular_intensity_present;
    } PC_GLOBAL_LIGHTING_STATE;

    STATIC_ASSERT(sizeof(PC_GLOBAL_LIGHTING_STATE) == 0x78);
    STATIC_ASSERT(offsetof(PC_GLOBAL_LIGHTING_STATE, specular_direction) == 0x54);
    STATIC_ASSERT(offsetof(PC_GLOBAL_LIGHTING_STATE, specular_direction_missing) == 0x60);
    STATIC_ASSERT(offsetof(PC_GLOBAL_LIGHTING_STATE, unknown_64) == 0x64);
    STATIC_ASSERT(offsetof(PC_GLOBAL_LIGHTING_STATE, specular_intensity_present) == 0x74);
    STATIC_ASSERT(offsetof(PC_GLOBAL_LIGHTING_STATE, direction) == 0xc);
    STATIC_ASSERT(offsetof(PC_GLOBAL_LIGHTING_STATE, intensity) == 0x30);
    extern const char TerrainWaterJumpSoundNamePc[13];
    void DebrisDraw(i32, i32);
    extern i32 g_renderingDebris;
    extern i32 DebrisSuspendDrawObjectSwitch;
    void DebrisFreeOldestDmaDebTypeTable(void);
    extern f32 debris_thinning_level;
    extern i32 forced_debris_thinning;
    extern i32 freeDmaDebType;
    extern i32 EDPP_MAX_DMADEBTYPES;
    extern DEBRIS_PART_HEADER_PC **DmaDebTypes;
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
    void BuildRippleMtx(NUMTX *, NUVEC *, NUVEC *, u16, u16);

    extern RIPPLE_SET_PC *RippleSetPc;
    extern f32 RipplePerturbationPc;
    extern f32 RippleSfxCooldownPc;
    extern f32 ForceFieldRippleRatePc;
    extern const char RippleForceFieldSoundNamePc[10];
    extern f32 GunshipDustEmissionRatePc;
    void InitRippleMtl(char *, NUMTL **, VARIPTR *, VARIPTR *);
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
    void AddWaterSplash(GAMEOBJECT *object, NUVEC *position);
    void AddSlamDebris(GAMEOBJECT *);
    void AddSurfaceRipples(GAMEOBJECT *);
    void AddSurfaceDebris(GAMEOBJECT *);
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
    extern DEBRIS_INFO_PC **debtab;
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
    void DebrisStartOffsetEx(DEBRISKEY_PC *, f32);
    void DebrisProcessSpheres(DEBRISPARTICLE_PC *, f32, DEBRIS_INFO_PC *, DEBRISKEY_PC *, i32);
    void DebReAlloc(DEBRISKEY_PC *, i32);
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
