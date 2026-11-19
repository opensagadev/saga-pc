#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct PART;
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
    u8 reserve_ammo;           // 0x5
} WEAPON_AMMO;

#ifdef __cplusplus
extern "C" {
#endif
    extern i16 id_FOURLOM;
    extern i16 id_KAMINOANDROID;
    extern i16 id_NABOOSTARFIGHTER;
    extern i16 id_NABOOSTARFIGHTERLIME;
    extern i16 id_SATIPO;
    extern i16 id_MINITIEINTERCEPTOR;
    extern i16 id_MINIIMPERIALSHUTTLE;
    extern i16 id_MINIMILLENNIUMFALCON;
    extern i16 id_MINITIEFIGHTER;
    extern i16 id_MINITIEADVANCED;
    extern i16 id_MINISITHINFILTRATOR;
    extern i16 id_MINISOLARSAILOR;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 Bolt_HitPart_LSW(struct bolt_s *, struct PART *);
    void Bolt_Debris_LSW(struct bolt_s *, NUVEC *, i32, NUVEC *, i32);
    void UpdateBolt_Geonosian(struct bolt_s *);
    i32 Bolt_AlternateFire_LSW(struct gameobject_s *, i32);
    void EndBolt_EwokTorpedo(struct bolt_s *bolt);
#ifdef __cplusplus
}
#endif
