#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

/* SECURITY_DOOR_RECORD.flags, the bitfields */
#define SECURITYDOOR_ACTIVE 0x1
#define SECURITYDOOR_VISIBLE 0x2
#define SECURITYDOOR_OPENED 0x4 /* fully open: the gizmo output */
#define SECURITYDOOR_DRAWN 0x8  /* a leaf was drawn this frame */

/* SECURITY_DOOR_RECORD.state */
#define SECURITYDOOR_STATE_CLOSED 0
#define SECURITYDOOR_STATE_IN_USE 1 /* a character works the panel (CHARACTER_CONTEXT_SECURITY) */
#define SECURITYDOOR_STATE_OPEN 2   /* opening, then open */

typedef struct security_door_record_s { // size 0xc0
    NUMTX leaf_matrix[2];               // 0x00
    char name[16];                      // 0x80
    NUVEC position;                     // 0x90
    i16 platform_id[2];                 // 0x9c
    u16 yaw;                            // 0xa0

    union { // size 0x1
        u8 flags;

        struct {            // size 0x1
            u8 active : 1;  // 0xa2:0
            u8 visible : 1; // 0xa2:1
            u8 opened : 1;  // 0xa2:2
            u8 drawn : 1;   // 0xa2:3
            u8 unused : 4;  // 0xa2:4
        };
    }; // 0xa2

    u8 state;              // 0xa3
    NUVEC player_position; // 0xa4
    u16 terrain_angle_z;   // 0xb0
    u16 terrain_angle_x;   // 0xb2
    f32 opening;           // 0xb4
    u8 reserved_b8[8];     // 0xb8
} SECURITY_DOOR_RECORD;

#ifdef __cplusplus
extern "C" {
#endif
    ADDGIZMOTYPE *SecurityDoors_RegisterGizmo(i32);
    void SecurityDoor_MoveCode(WORLDINFO *, GAMEOBJECT *);
    void SecurityDoors_InitTerrain(WORLDINFO *);
#ifdef __cplusplus
}
#endif
