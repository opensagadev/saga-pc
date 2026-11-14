#ifndef SAGA_LEGOAPI_ITEMS_COLLECT_BOLT_H_PART
#define SAGA_LEGOAPI_ITEMS_COLLECT_BOLT_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/items/base/apiobject.h, legoapi/items/collect/bolts.h,
 * legoapi/items/objects/objectsall.h, nu2api/numath/nuvec.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/objects/objectsall.h"
#include "nu2api/numath/nuvec.h"
#endif
/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_ITEMS_COLLECT_BOLT_H_PART == 1 && !defined(SAGA_LEGOAPI_ITEMS_COLLECT_BOLT_H_STOP)

typedef struct GAMEDEBRISENTRY_PC {
    i32 effect;
    char name[16];
} GAMEDEBRISENTRY_PC;

STATIC_ASSERT(sizeof(GAMEDEBRISENTRY_PC) == 0x14);
STATIC_ASSERT(offsetof(GAMEDEBRISENTRY_PC, name) == 4);

/* Minimum shared consumer records; offsets verified, no item allocation extent claimed. */
typedef struct vehicle_weapon_item_pc_s {
    u8 unknown_00[2];
    u8 start_ammo;
    u8 start_field_03;
    u8 unknown_04[4];
    u8 type;
    i8 bolt_override;
} VEHICLE_WEAPON_ITEM_PC;

STATIC_ASSERT(offsetof(VEHICLE_WEAPON_ITEM_PC, type) == 8);
STATIC_ASSERT(offsetof(VEHICLE_WEAPON_ITEM_PC, bolt_override) == 9);

typedef struct weapon_ammo_pc_s {
    VEHICLE_WEAPON_ITEM_PC *type;
    u8 ammo;
    u8 field_05;
} WEAPON_AMMO_PC;

STATIC_ASSERT(offsetof(WEAPON_AMMO_PC, ammo) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    extern i16 VehicleBoltId7f190cPc;
    extern i16 VehicleBoltId7f1a38Pc;
    extern i16 VehicleBoltId7f1a9cPc;
    extern i16 VehicleBoltId7f1aa0Pc;
    extern i16 VehicleBoltId7f1aacPc;
    extern i16 VehicleBoltId7f1ad8Pc;
    extern i16 VehicleBoltId7f1aecPc;
    extern i16 VehicleBoltId7f1af0Pc;
    extern i16 VehicleBoltId7f1b00Pc;
    extern i16 VehicleBoltId7f1b04Pc;
    extern i16 VehicleBoltId7f1b08Pc;
    extern i16 VehicleBoltId7f1b0cPc;
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
    i32 Bolt_HitPart_LSW(BOLT_PC *, PART_PC *);
    void Bolt_Debris_LSW(BOLT_PC *, NUVEC *, i32, NUVEC *, i32);
    void UpdateBolt_Geonosian(BOLT_PC *);
    i32 Bolt_AlternateFire_LSW(GAMEOBJECT *, i32);
    void EndBolt_EwokTorpedo(BOLT_PC *bolt);
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
