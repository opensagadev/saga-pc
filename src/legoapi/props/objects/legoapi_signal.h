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

    typedef struct SUIT_INITIALIZER_RECORD_PC { // size 0x18
        char *base_character_name;              // 0x00
        char *suit_character_name;              // 0x04
        i16 *text_id;                           // 0x08
        u16 flags;                              // 0x0c
        u8 group;                               // 0x0e
        char letter;                            // 0x0f
        u32 character_flags;                    // 0x10
        i16 character_id;                       // 0x14
        u8 index;                               // 0x16
        u8 unknown_17;                          // 0x17
    } SUIT_INITIALIZER_RECORD_PC;

#ifdef __cplusplus
}
#endif
typedef struct SIGNALPROGRESS_s SIGNALPROGRESS_s;

struct SIGNALPROGRESS_s {  // size 0x28
    char suit_letters[32]; // 0x00
    u32 active_mask[1];    // 0x20
    u32 visible_mask[1];   // 0x24
};

#undef SAGA_LEGOAPI_PROPS_OBJECTS_LEGOAPI_SIGNAL_H_PART
#define SAGA_LEGOAPI_PROPS_OBJECTS_LEGOAPI_SIGNAL_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_PROPS_OBJECTS_LEGOAPI_SIGNAL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_PROPS_OBJECTS_LEGOAPI_SIGNAL_H_PART == 2 && !defined(SAGA_LEGOAPI_PROPS_OBJECTS_LEGOAPI_SIGNAL_H_STOP)
typedef struct SIGNAL_s SIGNAL_s;

struct SIGNAL_s {          // size 0x94
    char name[16];         // 0x00
    NUMTX matrix;          // 0x10
    NUVEC position;        // 0x50
    u16 yaw;               // 0x5c
    u16 terrain_pitch;     // 0x5e
    u16 terrain_roll;      // 0x60
    i16 animation_0;       // 0x62
    i16 animation_1;       // 0x64
    i16 animation_2;       // 0x66
    char character_letter; // 0x68
    char suit_letter;      // 0x69

    union { // size 0x1
        u8 flags;

        struct {                   // size 0x1
            u8 visible : 1;        // 0x6a:0
            u8 active : 1;         // 0x6a:1
            u8 in_use : 1;         // 0x6a:2
            u8 reserved_flags : 5; // 0x6a:3
        };
    }; // 0x6a

    u8 pad_6b;                               // 0x6b
    f32 radius;                              // 0x6c
    f32 scale;                               // 0x70
    NUVEC normal;                            // 0x74
    struct SUIT_INITIALIZER_RECORD_PC *suit; // 0x80
    NUVEC target_position;                   // 0x84
    i16 platform_id;                         // 0x90
    u16 pad_92;                              // 0x92
};

#ifdef __cplusplus
extern "C" {
#endif
    struct SIGNAL_s *Signal_FindNearest(struct WORLDINFO_s *world, struct nuvec_s *position, struct gameobject_s *object, f32 *distance);
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
