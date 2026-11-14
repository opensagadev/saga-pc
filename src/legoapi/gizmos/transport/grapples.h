#ifndef SAGA_LEGOAPI_GIZMOS_TRANSPORT_GRAPPLES_H
#define SAGA_LEGOAPI_GIZMOS_TRANSPORT_GRAPPLES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: gameapi/ai/aisys/aisys.h, legoapi/items/base/apiobject.h, legoapi/legoapi_types.h
 * (not all of it earlier in the shim). */

#include "legoapi/world/world.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#endif

typedef struct GRAPPLE_DYNAMIC_PC {
    u8 unknown_00[0x1d];
    u8 retain_attachment;
    u8 flags;
    u8 field_1f;
    u16 y_rotation;
    u16 x_rotation;
    NUVEC ground_position;
    NUVEC hook_position;
    NUVEC shadow_probe_position;
    u16 ground_x_rotation;
    u16 ground_z_rotation;
    f32 field_4c;
    f32 rope_length;
    GAMEOBJECT *attached_object;
    u8 unknown_58[0xa0];
} GRAPPLE_DYNAMIC_PC;

STATIC_ASSERT(sizeof(GRAPPLE_DYNAMIC_PC) == 0xf8);
STATIC_ASSERT(offsetof(GRAPPLE_DYNAMIC_PC, retain_attachment) == 0x1d);
STATIC_ASSERT(offsetof(GRAPPLE_DYNAMIC_PC, flags) == 0x1e);
STATIC_ASSERT(offsetof(GRAPPLE_DYNAMIC_PC, attached_object) == 0x54);
#ifdef __cplusplus
extern "C" {
#endif
    /* Existing extern C shared block; no new shared layout fields. */

    GRAPPLE_DYNAMIC_PC *Grapple_AddDynamic(void *, i32);
    GRAPPLE_DYNAMIC_PC *Grapple_RemoveDynamic(void *);
    extern GRAPPLE_DYNAMIC_PC DynamicGrapplePc[4];
    void Grapple_SetRotOrder(GAMEOBJECT *);

    extern i32 PlayerGrappleSwingModePc;
    void Grapple_SetPlayerTargetPoint(GAMEOBJECT *object, NUVEC *target);
    i32 Grapple_SetTargetMom(GAMEOBJECT *object);
    extern LANGUAGEDATA Text_LanguageList_Default[7];
    extern NUMTL *ZipUpLineMaterialPc;
    GAMEOBJECT *Grapple_Occupied(void *, GAMEOBJECT *, AIPATHCNX_PC *);
    extern i32 grapple_gizmotype_id;
    extern i16 Grapple_RopeSwingRotate;
    extern i32 LEGOHINT_GRAPPLE;
#ifdef __cplusplus
}
#endif

/* Public declarations only; private original grapple views/helpers remain in their owner. */
typedef struct GRAPPLE_MOTION_PC {
    char name[16];
    NUVEC position;
    u8 has_terrain_platform;
    u8 retain_attachment;

    union {
        u8 flags;

        struct {
            u8 active : 1;
            u8 visible : 1;
            u8 disabled : 1;
            u8 reversed : 1;
            u8 reserved_flags : 4;
        };
    };

    u8 unknown_1f;
    u16 y_rotation;
    u16 x_rotation;
    NUVEC ground_position;
    NUVEC hook_position;
    NUVEC shadow_probe_position;
    u16 ground_x_rotation;
    u16 ground_z_rotation;
    f32 swing_scale;
    f32 rope_length;
    GAMEOBJECT *attached_object;
    f32 activation_progress;
    NUVEC rope_points[6];
    NUVEC target_rope_points[6];
    u8 display_special[12];
} GRAPPLE_MOTION_PC;

STATIC_ASSERT(sizeof(GRAPPLE_MOTION_PC) == 0xf8);
STATIC_ASSERT(offsetof(GRAPPLE_MOTION_PC, has_terrain_platform) == 0x1c);
#ifdef __cplusplus
extern "C" {
#endif
    GRAPPLE_MOTION_PC *Grapple_FindNearest(WORLDINFO *, NUVEC *, GAMEOBJECT *, f32 *);
    void Grapple_MoveCode(GAMEOBJECT *);
    GRAPPLE_DYNAMIC_PC *Grapple_FindNearestToPos(WORLDINFO *world, NUVEC *position);
    i32 IndyGrappleCanJumpPc(GAMEOBJECT *object);
#ifdef __cplusplus
}
#endif

#endif
