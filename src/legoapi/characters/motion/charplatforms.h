#ifndef SAGA_LEGOAPI_CHARACTERS_MOTION_CHARPLATFORMS_H
#define SAGA_LEGOAPI_CHARACTERS_MOTION_CHARPLATFORMS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    GAMEOBJECT *CharPlatform_FindObjFromPlatID(void *system, i32 platform_id);
#ifdef __cplusplus
}
#endif

#ifndef TYPES_OFF_9
typedef struct terrain_player_platform_record_pc_s {
    u8 unknown_0[0xe];               // 0x00
    i16 platform_id;                 // 0x0e
    GAMEOBJECT *object;              // 0x10
} TERRAIN_PLAYER_PLATFORM_RECORD_PC; // size 0x14

typedef struct terrain_player_platform_sys_pc_s {
    u8 unknown_0[4];                              // 0x00
    i32 count;                                    // 0x04
    TERRAIN_PLAYER_PLATFORM_RECORD_PC entries[1]; // 0x08
} TERRAIN_PLAYER_PLATFORM_SYS_PC;                 // size 0x1c

#endif /* TYPES_OFF_9 */
#endif
