/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/nuvec.h"
#endif

typedef struct GRAPPLE_DYNAMIC { // size 0xf8
    u8 unknown_00[0x1d];         // 0x00
    u8 retain_attachment;        // 0x1d
    u8 flags;                    // 0x1e
    u8 unknown_1f;               // 0x1f
    u16 y_rotation;              // 0x20
    u16 x_rotation;              // 0x22
    NUVEC ground_position;       // 0x24
    NUVEC hook_position;         // 0x30
    NUVEC shadow_probe_position; // 0x3c
    u16 ground_x_rotation;       // 0x48
    u16 ground_z_rotation;       // 0x4a
    f32 unknown_4c;              // 0x4c
    f32 rope_length;             // 0x50
    GAMEOBJECT *attached_object; // 0x54
    u8 unknown_58[0xa0];         // 0x58
} GRAPPLE_DYNAMIC;

#ifdef __cplusplus
extern "C" {
#endif
    /* Existing extern C shared block; no new shared layout fields. */

    GRAPPLE_DYNAMIC *Grapple_AddDynamic(void *, i32);
    GRAPPLE_DYNAMIC *Grapple_RemoveDynamic(void *);
    extern GRAPPLE_DYNAMIC DynamicGrapplePc[4];
    void Grapple_SetRotOrder(GAMEOBJECT *);

    extern i32 PlayerGrappleSwingModePc;
    void Grapple_SetPlayerTargetPoint(GAMEOBJECT *object, NUVEC *target);
    i32 Grapple_SetTargetMom(GAMEOBJECT *object);
    extern LANGUAGEDATA Text_LanguageList_Default[7];
    extern NUMTL *ZipUpLineMaterial;
    GAMEOBJECT *Grapple_Occupied(void *, GAMEOBJECT *, AIPATHCNX *);
    extern i32 grapple_gizmotype_id;
    extern i16 Grapple_RopeSwingRotate;
    extern i32 LEGOHINT_GRAPPLE;
#ifdef __cplusplus
}
#endif

/* Public declarations only; private original grapple views/helpers remain in their owner. */
typedef struct GRAPPLE_MOTION { // size 0xf8
    char name[16];              // 0x00
    NUVEC position;             // 0x10
    u8 has_terrain_platform;    // 0x1c
    u8 retain_attachment;       // 0x1d

    union { // size 0x1
        u8 flags;

        struct {                   // size 0x1
            u8 active : 1;         // 0x1e:0
            u8 visible : 1;        // 0x1e:1
            u8 disabled : 1;       // 0x1e:2
            u8 reversed : 1;       // 0x1e:3
            u8 reserved_flags : 4; // 0x1e:4
        };
    }; // 0x1e

    u8 unknown_1f;               // 0x1f
    u16 y_rotation;              // 0x20
    u16 x_rotation;              // 0x22
    NUVEC ground_position;       // 0x24
    NUVEC hook_position;         // 0x30
    NUVEC shadow_probe_position; // 0x3c
    u16 ground_x_rotation;       // 0x48
    u16 ground_z_rotation;       // 0x4a
    f32 swing_scale;             // 0x4c
    f32 rope_length;             // 0x50
    GAMEOBJECT *attached_object; // 0x54
    f32 activation_progress;     // 0x58
    NUVEC rope_points[6];        // 0x5c
    NUVEC target_rope_points[6]; // 0xa4
    u8 display_special[12];      // 0xec
} GRAPPLE_MOTION;

#ifdef __cplusplus
extern "C" {
#endif
    GRAPPLE_MOTION *Grapple_FindNearest(WORLDINFO *, NUVEC *, GAMEOBJECT *, f32 *);
    void Grapple_MoveCode(GAMEOBJECT *);
    GRAPPLE_DYNAMIC *Grapple_FindNearestToPos(WORLDINFO *world, NUVEC *position);
    i32 IndyGrappleCanJump(GAMEOBJECT *object);
#ifdef __cplusplus
}
#endif
