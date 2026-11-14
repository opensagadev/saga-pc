/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART == 1 && !defined(SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_STOP)

struct APIDEBRISSYS_PC;
struct CHARACTER_MODEL_LAYERS_PC;
struct PLAYERCHARACTERCONFIG_PC;
struct animpacket_s;
struct apicharacterdata_s;
struct apicharactermodellist_s;
struct gameobject_s;
struct nufpcomjmp_s;
struct numtx_s;

typedef struct GAMECHARACTERLAYER { // size 0x1c
    char name[0x18];                // 0x00
    i16 mask_bit;                   // 0x18
    i16 hierarchy_layer_index;      // 0x1a
} GAMECHARACTERLAYER;

typedef struct CHARACTER_EFFECT_CONFIG_PC { // size 0x34
    i16 character_id;                       // 0x00
    i16 debris_id;                          // 0x02
    i16 action_id;                          // 0x04
    u8 bits_on;                             // 0x06
    u8 bits_off;                            // 0x07
    f32 frame[2];                           // 0x08
    f32 particle_rate;                      // 0x10
    u32 flags;                              // 0x14
    u16 locators;                           // 0x18
    u8 random;                              // 0x1a
    u8 particle_count;                      // 0x1b
    f32 rumble;                             // 0x1c
    f32 shake;                              // 0x20
    f32 judder;                             // 0x24
    i16 sound_id;                           // 0x28
    u16 surfaces;                           // 0x2a
    f32 minimum;                            // 0x2c
    f32 maximum;                            // 0x30
} CHARACTER_EFFECT_CONFIG_PC;

#ifdef __cplusplus
extern "C" {
#endif
    void CharConfig_CalculateJumpStats(f32, f32, f32 *, f32 *);
    extern f32 vehicle_timebase_dist[4];
    extern i32 vehicle_timebase_nframes[4];
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART == 2 && !defined(SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void CharConfig_ConfigureAll(i32, struct nufpcomjmp_s *);
    i32 LayerFromName(struct PLAYERCHARACTERCONFIG_PC *, const char *);
    i32 MakeLayerList_Name(struct CHARACTER_MODEL_LAYERS_PC *, i16 *, u32);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART == 3 && !defined(SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern BOLTSYS_PC GlobalBoltSystem_Default;
    void AddAnimEffects(struct CHARACTER_MODEL_LAYERS_PC *, struct apicharacterdata_s *, struct animpacket_s *,
                        struct numtx_s *, struct gameobject_s *, CHARACTER_EFFECT_CONFIG_PC *, i32, f32,
                        void (*)(void *, struct gameobject_s *, i32, i32), struct APIDEBRISSYS_PC *);
    i32 CharCategory_IsCategory(struct gameobject_s *, i32);
    void GameLoadCharacterModels(struct apicharactermodellist_s *, i32, VARIPTR *, VARIPTR *, i32, i32);
    i32 InModelListDataFlags(struct apicharactermodellist_s *, u32, u32, i32, i32);
    i32 RandomIDFromFlags(u32, u32, i32, struct apicharactermodellist_s *, i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_STOP
