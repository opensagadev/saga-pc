#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "nu2api/numath/nuvec.h"

/* cheat_s.flags and cheatsystem_s.flags (the flags of the enabled cheats, Cheats_SetFlags): what each cheat does,
 * from the code that tests it with Cheats_CheckFlags and the Player_Has* functions. 0x2000, 0x20000-0x100000 and
 * 0x400000 are listed in docs/magic-values.md. */
#define CHEAT_ZERO_LIGHTING 0x1        /* characters drawn with zero lights (SetZeroLights, KillPartsZeroLighting) */
#define CHEAT_DOUBLE_BOLT_DAMAGE 0x2   /* Player_HasDoubleBoltDamage_FromBolt */
#define CHEAT_SCORE_X2 0x4             /* Cheat_MultiplyScore */
#define CHEAT_SCORE_X4 0x8
#define CHEAT_SCORE_X6 0x10
#define CHEAT_SCORE_X8 0x20
#define CHEAT_SCORE_X10 0x40
#define CHEAT_INVINCIBILITY 0x80          /* Player_HasInvincibility */
#define CHEAT_EXTRA_TOGGLE 0x100          /* the characters with CHARACTER_RUNTIME_EXTRA_TOGGLE */
#define CHEAT_DOUBLE_WEAPON_DAMAGE 0x400  /* Player_HasDoubleWeaponDamage */
#define CHEAT_DOUBLE_PUNCH_DAMAGE 0x800   /* Punch_Hit doubles the damage */
#define CHEAT_REGENERATE_HEARTS 0x1000    /* RegenerateHearts for active players */
#define CHEAT_FAST_BUILD 0x4000           /* Player_HasFastBuild */
#define CHEAT_STUD_MAGNET 0x8000          /* the pickups' coin magnet */
#define CHEAT_POWERUP_ON_FOOT 0x10000     /* a power-up cheat that counts while Cheat_PowerUpTime runs, on foot */
#define CHEAT_POWERUP_IN_VEHICLE 0x20000  /* the same in vehicle areas */
#define CHEAT_DEFLECT_BOLTS 0x80000       /* Player_HasDeflectBolts */
#define CHEAT_POWERUP 0x200000            /* turned off by Cheats_TurnOff(only_powerups) */

typedef struct cheat_s { // size 0x20
    char *name;          // 0x00
    i16 *text_id;        // 0x04
    u8 enabled;          // 0x08
    u8 unknown_0009[2];  // 0x09
    i8 area_index;       // 0x0b
    i32 unknown_0c;      // 0x0c
    char *code;          // 0x10
    i32 extra_price;     // 0x14
    char *extra_name;    // 0x18
    u32 flags;           // 0x1c
} CHEAT;

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 CHEAT_POWERUPTIME;
    extern i32 POWERUP_TEXTID;
    void Cheat_StartPowerUp(NUVEC *, GAMEOBJECT *);
    extern f32 CHEAT_POWERUPTIME;
    void Cheats_Init(CHEAT *cheats);
    i32 Cheat_FindByName(char *name);
    void Cheat_SetArea(i32 cheat, i32 area_index);
    i32 Cheat_PowerUpActive(i32);
    extern i32 ONEPLAYERPOWERUPS;
    i32 Cheats_CheckFlags(u32);
    i32 Cheat_IsOn(i32);
#ifdef __cplusplus
    u32 Cheat_CheckFlags(i32, u32);
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    void Cheats_SetFlags(void);
    void Cheat_SetOn(i32 cheat, i32 on, i32 update_save);
    void Cheats_TurnOff(i32 only_powerups);
    void Cheats_Reset(void);
    u32 Cheat_MultiplyScore(u32);
#ifdef __cplusplus
}
#endif
