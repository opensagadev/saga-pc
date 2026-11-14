#ifndef SAGA_LEGOAPI_CORE_CONFIG_CHEAT_H
#define SAGA_LEGOAPI_CORE_CONFIG_CHEAT_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

typedef struct cheat_pc_s {
    char *name;
    i16 *text_id;
    u8 enabled;
    u8 unknown_0009[2];
    i8 area_index;
    i32 field_0c;
    char *code;
    i32 extra_price;
    char *extra_name;
    u32 flags;
} CHEAT_PC;

STATIC_ASSERT(sizeof(CHEAT_PC) == 0x20);
STATIC_ASSERT(offsetof(CHEAT_PC, text_id) == 4);
STATIC_ASSERT(offsetof(CHEAT_PC, code) == 0x10);
STATIC_ASSERT(offsetof(CHEAT_PC, extra_price) == 0x14);
STATIC_ASSERT(offsetof(CHEAT_PC, extra_name) == 0x18);

STATIC_ASSERT(sizeof(CHEAT_PC) == 0x20);
STATIC_ASSERT(offsetof(CHEAT_PC, enabled) == 0x8);
STATIC_ASSERT(offsetof(CHEAT_PC, flags) == 0x1c);

STATIC_ASSERT(offsetof(CHEAT_PC, area_index) == 0x0b);
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

#endif
