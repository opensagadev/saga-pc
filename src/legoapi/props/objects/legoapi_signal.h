/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_PROPS_OBJECTS_LEGOAPI_SIGNAL_H_PART
#define SAGA_LEGOAPI_PROPS_OBJECTS_LEGOAPI_SIGNAL_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_PROPS_OBJECTS_LEGOAPI_SIGNAL_H_PART == 1 && !defined(SAGA_LEGOAPI_PROPS_OBJECTS_LEGOAPI_SIGNAL_H_STOP)

struct WORLDINFO_s;
struct gameobject_s;

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct SUIT_INITIALIZER_RECORD_PC {
        char *base_character_name;
        char *suit_character_name;
        i16 *text_id;
        u16 flags;
        u8 group;
        char letter;
        u32 character_flags;
        i16 character_id;
        u8 index;
        u8 unknown_17;
    } SUIT_INITIALIZER_RECORD_PC;

    STATIC_ASSERT(sizeof(SUIT_INITIALIZER_RECORD_PC) == 24);
    STATIC_ASSERT(offsetof(SUIT_INITIALIZER_RECORD_PC, text_id) == 8);
    STATIC_ASSERT(offsetof(SUIT_INITIALIZER_RECORD_PC, flags) == 0xc);
    STATIC_ASSERT(offsetof(SUIT_INITIALIZER_RECORD_PC, character_flags) == 0x10);
    STATIC_ASSERT(offsetof(SUIT_INITIALIZER_RECORD_PC, character_id) == 0x14);
    STATIC_ASSERT(offsetof(SUIT_INITIALIZER_RECORD_PC, base_character_name) == 0);
    STATIC_ASSERT(offsetof(SUIT_INITIALIZER_RECORD_PC, suit_character_name) == 4);
    STATIC_ASSERT(offsetof(SUIT_INITIALIZER_RECORD_PC, group) == 0xe);
    STATIC_ASSERT(offsetof(SUIT_INITIALIZER_RECORD_PC, letter) == 0xf);
    STATIC_ASSERT(offsetof(SUIT_INITIALIZER_RECORD_PC, index) == 0x16);
#ifdef __cplusplus
}
#endif
typedef struct SIGNALPROGRESS_s SIGNALPROGRESS_s;

struct SIGNALPROGRESS_s {
    char suit_letters[32];
    u32 active_mask[1];
    u32 visible_mask[1];
};

STATIC_ASSERT(sizeof(struct SIGNALPROGRESS_s) == 40);
#undef SAGA_LEGOAPI_PROPS_OBJECTS_LEGOAPI_SIGNAL_H_PART
#define SAGA_LEGOAPI_PROPS_OBJECTS_LEGOAPI_SIGNAL_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_PROPS_OBJECTS_LEGOAPI_SIGNAL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_PROPS_OBJECTS_LEGOAPI_SIGNAL_H_PART == 2 && !defined(SAGA_LEGOAPI_PROPS_OBJECTS_LEGOAPI_SIGNAL_H_STOP)
typedef struct SIGNAL_s SIGNAL_s;

struct SIGNAL_s {
    char name[16];
    NUMTX matrix;
    NUVEC position;
    u16 yaw;
    u16 terrain_pitch;
    u16 terrain_roll;
    i16 animation_0;
    i16 animation_1;
    i16 animation_2;
    char character_letter;
    char suit_letter;

    union {
        u8 flags;

        struct {
            u8 visible : 1;
            u8 active : 1;
            u8 in_use : 1;
            u8 reserved_flags : 5;
        };
    };

    u8 pad_6b;
    f32 radius;
    f32 scale;
    NUVEC normal;
    struct SUIT_INITIALIZER_RECORD_PC *suit;
    NUVEC target_position;
    i16 platform_id;
    u16 pad_92;
};

STATIC_ASSERT(sizeof(struct SIGNAL_s) == 148);
#ifdef __cplusplus
extern "C" {
#endif
    struct SIGNAL_s *Signal_FindNearest(struct WORLDINFO_s *world, struct nuvec_s *position,
                                        struct gameobject_s *object, f32 *distance);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_PROPS_OBJECTS_LEGOAPI_SIGNAL_H_PART
#define SAGA_LEGOAPI_PROPS_OBJECTS_LEGOAPI_SIGNAL_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_PROPS_OBJECTS_LEGOAPI_SIGNAL_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_PROPS_OBJECTS_LEGOAPI_SIGNAL_H_STOP
