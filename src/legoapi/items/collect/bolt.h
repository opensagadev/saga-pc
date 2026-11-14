/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_ITEMS_COLLECT_BOLT_H_PART
#define SAGA_LEGOAPI_ITEMS_COLLECT_BOLT_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_ITEMS_COLLECT_BOLT_H_PART == 1 && !defined(SAGA_LEGOAPI_ITEMS_COLLECT_BOLT_H_STOP)

struct PART_PC;
struct bolt_s;
struct gameobject_s;

typedef struct GAMEDEBRISENTRY { // size 0x14
    i32 effect;                  // 0x00
    char name[16];               // 0x04
} GAMEDEBRISENTRY;

/* Minimum shared consumer records; offsets verified, no item allocation extent claimed. */
typedef struct vehicle_weapon_item_s { // size 0xa
    u8 unknown_00[2];                  // 0x0
    u8 start_ammo;                     // 0x2
    u8 start_field_03;                 // 0x3
    u8 unknown_04[4];                  // 0x4
    u8 type;                           // 0x8
    i8 bolt_override;                  // 0x9
} VEHICLE_WEAPON_ITEM;

typedef struct weapon_ammo_s { // size 0x8
    VEHICLE_WEAPON_ITEM *type; // 0x0
    u8 ammo;                   // 0x4
    u8 unknown_05;             // 0x5
} WEAPON_AMMO_PC;

#ifdef __cplusplus
extern "C" {
#endif
    extern i16 dat_007f190c;
    extern i16 dat_007f1a38;
    extern i16 dat_007f1a9c;
    extern i16 dat_007f1aa0;
    extern i16 dat_007f1aac;
    extern i16 dat_007f1ad8;
    extern i16 dat_007f1aec;
    extern i16 dat_007f1af0;
    extern i16 dat_007f1b00;
    extern i16 dat_007f1b04;
    extern i16 dat_007f1b08;
    extern i16 dat_007f1b0c;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_COLLECT_BOLT_H_PART
#define SAGA_LEGOAPI_ITEMS_COLLECT_BOLT_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_COLLECT_BOLT_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_COLLECT_BOLT_H_PART == 2 && !defined(SAGA_LEGOAPI_ITEMS_COLLECT_BOLT_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    i32 Bolt_HitPart_LSW(struct bolt_s *, struct PART_PC *);
    void Bolt_Debris_LSW(struct bolt_s *, NUVEC *, i32, NUVEC *, i32);
    void UpdateBolt_Geonosian(struct bolt_s *);
    i32 Bolt_AlternateFire_LSW(struct gameobject_s *, i32);
    void EndBolt_EwokTorpedo(struct bolt_s *bolt);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_COLLECT_BOLT_H_PART
#define SAGA_LEGOAPI_ITEMS_COLLECT_BOLT_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_COLLECT_BOLT_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_ITEMS_COLLECT_BOLT_H_STOP
