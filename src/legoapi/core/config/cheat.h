/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "nu2api/numath/nuvec.h"
#endif

typedef struct cheat_s { // size 0x20
    char *name;          // 0x00
    i16 *text_id;        // 0x04
    u8 enabled;          // 0x08
    u8 unknown_0009[2];  // 0x09
    i8 area_index;       // 0x0b
    i32 field_0c;        // 0x0c
    char *code;          // 0x10
    i32 extra_price;     // 0x14
    char *extra_name;    // 0x18
    u32 flags;           // 0x1c
} CHEAT_PC;

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 CHEAT_POWERUPTIME;
    extern i32 POWERUP_TEXTID;
    void Cheat_StartPowerUp(NUVEC *, GAMEOBJECT *);
    extern f32 CHEAT_POWERUPTIME;
    void Cheats_Init(CHEAT_PC *cheats);
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
