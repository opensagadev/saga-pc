#ifndef SAGA_LEGOAPI_GIZMO_OBJECT_TAKEOVEROBJECTS_H
#define SAGA_LEGOAPI_GIZMO_OBJECT_TAKEOVEROBJECTS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct TAKEOVEROBJECT_PC {
        GAMEOBJECT *object;
        NUVEC last_safe_position;
        u32 heading;
        char script_name[16];
        i16 character_id;
        u8 source_creature;
        u8 registered_level;
        u8 current_level;
        u8 hitpoints;
        u8 contact_index;
        u8 unknown_2b;
    } TAKEOVEROBJECT_PC;

    STATIC_ASSERT(sizeof(TAKEOVEROBJECT_PC) == 0x2c);
    STATIC_ASSERT(offsetof(TAKEOVEROBJECT_PC, object) == 0);
    STATIC_ASSERT(offsetof(TAKEOVEROBJECT_PC, script_name) == 0x14);
    STATIC_ASSERT(offsetof(TAKEOVEROBJECT_PC, character_id) == 0x24);
    STATIC_ASSERT(offsetof(TAKEOVEROBJECT_PC, source_creature) == 0x26);
    STATIC_ASSERT(offsetof(TAKEOVEROBJECT_PC, registered_level) == 0x27);
    STATIC_ASSERT(offsetof(TAKEOVEROBJECT_PC, current_level) == 0x28);
    STATIC_ASSERT(offsetof(TAKEOVEROBJECT_PC, last_safe_position) == 4);
    STATIC_ASSERT(offsetof(TAKEOVEROBJECT_PC, heading) == 0x10);
    STATIC_ASSERT(offsetof(TAKEOVEROBJECT_PC, hitpoints) == 0x29);
    STATIC_ASSERT(offsetof(TAKEOVEROBJECT_PC, contact_index) == 0x2a);
    extern TAKEOVEROBJECT_PC takeoverobjects[8];
    void RegisterTakeOverObject(GAMEOBJECT *);

    extern i32 num_takeoverobjects;

    void ReStoreStatusTakeOverObjectSys(i32 restore_progress);
#ifdef __cplusplus
}
#endif

#endif
