#ifndef SAGA_LEGOAPI_GIZMOS_TRANSPORT_TELEPORT_H
#define SAGA_LEGOAPI_GIZMOS_TRANSPORT_TELEPORT_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#include "legoapi/menus/core/hint.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 Teleport_UpdateHints(HINT_PC *);
    void Teleports_Configure(WORLDINFO *, char *);
    typedef struct TELEPORT_MOTION_PC TELEPORT_MOTION_PC;
    TELEPORT_MOTION_PC *Teleport_Find(GAMEOBJECT *, f32);
    void Teleport_MoveCode(GAMEOBJECT *, i32);
    void Teleports_Reset(WORLDINFO *);
    void Teleports_UpdateBeforeGameObjects(WORLDINFO *);
    void Teleports_UpdateAfterGameObjects(WORLDINFO *);
    void Teleport_NetMoveCode(GAMEOBJECT *);
#ifdef __cplusplus
}
#endif

#ifndef TYPES_OFF_19
struct TELEPORT_MOTION_PC {
    char name[0x40];                  // 0x00
    NUGSPLINE *path;                  // 0x40
    f32 duration;                     // 0x44
    f32 range_squared;                // 0x48
    u16 flags;                        // 0x4c
    u8 enabled;                       // 0x4e
    u8 active;                        // 0x4f
    NUSPECIALHANDLE blocking_special; // 0x50
    NUSPECIALHANDLE flap1_special;    // 0x5c
    NUSPECIALHANDLE flap2_special;    // 0x68
    u16 flap_angle[2];                // 0x74
    u16 flap_target[2];               // 0x78
    NUMTX flap1_matrix;               // 0x7c
    NUMTX flap2_matrix;               // 0xbc
}; // size 0xfc

#endif /* TYPES_OFF_19 */
#endif
