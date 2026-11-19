#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct TAKEOVEROBJECT { // size 0x2c
        GAMEOBJECT *object;         // 0x00
        NUVEC last_safe_position;   // 0x04
        u32 heading;                // 0x10
        char script_name[16];       // 0x14
        i16 character_id;           // 0x24
        u8 source_creature;         // 0x26
        u8 registered_level;        // 0x27
        u8 current_level;           // 0x28
        u8 hitpoints;               // 0x29
        u8 contact_index;           // 0x2a
        u8 unknown_2b;              // 0x2b
    } TAKEOVEROBJECT;

    extern TAKEOVEROBJECT takeoverobjects[8];
    void RegisterTakeOverObject(GAMEOBJECT *);

    extern i32 num_takeoverobjects;

    void ReStoreStatusTakeOverObjectSys(i32 restore_progress);
#ifdef __cplusplus
}
#endif
