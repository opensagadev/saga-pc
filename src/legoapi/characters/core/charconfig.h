#ifndef SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARCONFIG_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

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
struct animpacket_pc_s;
struct apicharacterdata_s;
struct apicharactermodellist_s;
struct gameobject_s;
struct nufpcomjmp_s;
struct numtx_s;

typedef struct GAMECHARACTERLAYER_PC {
    char name[0x18];
    i16 mask_bit;
    i16 hierarchy_layer_index;
} GAMECHARACTERLAYER_PC;

STATIC_ASSERT(sizeof(GAMECHARACTERLAYER_PC) == 0x1c);
STATIC_ASSERT(offsetof(GAMECHARACTERLAYER_PC, mask_bit) == 0x18);
STATIC_ASSERT(offsetof(GAMECHARACTERLAYER_PC, hierarchy_layer_index) == 0x1a);

typedef struct CHARACTER_EFFECT_CONFIG_PC {
    i16 character_id;
    i16 debris_id;
    i16 action_id;
    u8 bits_on;
    u8 bits_off;
    f32 frame[2];
    f32 particle_rate;
    u32 flags;
    u16 locators;
    u8 random;
    u8 particle_count;
    f32 rumble;
    f32 shake;
    f32 judder;
    i16 sound_id;
    u16 surfaces;
    f32 minimum;
    f32 maximum;
} CHARACTER_EFFECT_CONFIG_PC;

STATIC_ASSERT(sizeof(CHARACTER_EFFECT_CONFIG_PC) == 0x34);
STATIC_ASSERT(offsetof(CHARACTER_EFFECT_CONFIG_PC, character_id) == 0);
STATIC_ASSERT(offsetof(CHARACTER_EFFECT_CONFIG_PC, debris_id) == 2);
STATIC_ASSERT(offsetof(CHARACTER_EFFECT_CONFIG_PC, action_id) == 4);
STATIC_ASSERT(offsetof(CHARACTER_EFFECT_CONFIG_PC, bits_on) == 6);
STATIC_ASSERT(offsetof(CHARACTER_EFFECT_CONFIG_PC, bits_off) == 7);
STATIC_ASSERT(offsetof(CHARACTER_EFFECT_CONFIG_PC, frame) == 8);
STATIC_ASSERT(offsetof(CHARACTER_EFFECT_CONFIG_PC, particle_rate) == 0x10);
STATIC_ASSERT(offsetof(CHARACTER_EFFECT_CONFIG_PC, flags) == 0x14);
STATIC_ASSERT(offsetof(CHARACTER_EFFECT_CONFIG_PC, locators) == 0x18);
STATIC_ASSERT(offsetof(CHARACTER_EFFECT_CONFIG_PC, random) == 0x1a);
STATIC_ASSERT(offsetof(CHARACTER_EFFECT_CONFIG_PC, particle_count) == 0x1b);
STATIC_ASSERT(offsetof(CHARACTER_EFFECT_CONFIG_PC, rumble) == 0x1c);
STATIC_ASSERT(offsetof(CHARACTER_EFFECT_CONFIG_PC, shake) == 0x20);
STATIC_ASSERT(offsetof(CHARACTER_EFFECT_CONFIG_PC, judder) == 0x24);
STATIC_ASSERT(offsetof(CHARACTER_EFFECT_CONFIG_PC, sound_id) == 0x28);
STATIC_ASSERT(offsetof(CHARACTER_EFFECT_CONFIG_PC, surfaces) == 0x2a);
STATIC_ASSERT(offsetof(CHARACTER_EFFECT_CONFIG_PC, minimum) == 0x2c);
STATIC_ASSERT(offsetof(CHARACTER_EFFECT_CONFIG_PC, maximum) == 0x30);
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
    void AddAnimEffects(struct CHARACTER_MODEL_LAYERS_PC *, struct apicharacterdata_s *, struct animpacket_pc_s *,
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
