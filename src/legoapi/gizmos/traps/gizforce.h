#ifndef SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART
#define SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/gizmo/base/gizmo.h, legoapi/items/base/apiobject.h,
 * legoapi/items/objects/objectsall.h, legoapi/legoapi_types.h, legoapi/menus/core/hint.h, nu2api/nucore/common.h,
 * nu2api/nucore/fixed_width.h, nu2api/numath/nuvec.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/core/hint.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"
#endif
/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART == 1 && !defined(SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_STOP)
typedef ADDGIZMOTYPE_PC *(*REGISTER_GIZMO_CONFIG_TYPE_FN)(i32 type_id);
#ifdef __cplusplus
extern "C" {
#endif

    extern const char ForcePushSoundNamePc[11];
    extern const char VehicleSmartBombChargeNamePc[10];
    extern const char VehicleShieldOnNamePc[10];
    i32 GizForce_UpdateHint(HINT_PC *);
    extern const char EpisodeVI_electric_off[13];
    extern const char EpisodeVI_elecpad_zero_format[15];
    extern const char EpisodeVI_elecpad_format[14];
    extern const char EpisodeVI_eFloor_AlwaysOn[16];
    extern const char EpisodeVI_eFloor_AlwaysOf[16];
    extern const char EpisodeVI_eFloor_OnOff[13];
    extern i32 force_gizmotype_id;
    extern NUVEC ForceThrowMaulPositionPc;
    extern u32 lightning_col[2];
    extern f32 lightning_endw[2];
    extern f32 lightning_sizel[2];
    extern f32 lightning_sizew[2];
    extern f32 lightning_sizewab[2];
    extern const char GameAudioName00758be8Pc[9];
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART
#define SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART == 2 && !defined(SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_STOP)

typedef struct FORCE_RESET_GROUP_PC {
    union {
        struct {
            GIZFORCE_PC *forces[8];
            f32 combined_height;
        };

        u32 slot_words[9];
    };

    u16 runtime_flags;
    u8 member_count;
    u8 configured_count;
} FORCE_RESET_GROUP_PC;

#undef SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART
#define SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART == 3 && !defined(SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_STOP)

typedef struct gizforce_target_record_pc_s {
    GIZFORCE_PC *force;
    GAMEANIMOBJ *animation;
    f32 distance;
    i32 index;
} GIZFORCE_TARGET_RECORD_PC;

STATIC_ASSERT(sizeof(GIZFORCE_TARGET_RECORD_PC) == 16);
STATIC_ASSERT(offsetof(GIZFORCE_TARGET_RECORD_PC, animation) == 4);
STATIC_ASSERT(offsetof(GIZFORCE_TARGET_RECORD_PC, distance) == 8);
STATIC_ASSERT(offsetof(GIZFORCE_TARGET_RECORD_PC, index) == 12);
#ifdef __cplusplus
extern "C" {
#endif
    extern GIZFORCE_TARGET_RECORD_PC PossibleForceTargetsPc[];
    i32 GizForce_FindBestForceTarget(void *, GAMEOBJECT *);

    i32 GizForce_StoodOnForce(GIZFORCE_PC *, GAMEOBJECT *);
    u16 GizForces_AngleToForce(NUVEC *, GIZFORCE_PC *);
    i32 GizForce_Complete(GIZFORCE_PC *);
    void ForceLightning_Origin(GAMEOBJECT *, NUVEC *, NUVEC *);
    PART_PC *GizForce_Throw(GAMEOBJECT *, GIZFORCE_PC *, f32, f32, i32);
    void GizForce_ResetLOS(GAMEOBJECT *);
    i32 GizForce_GameObjUsingForce(GAMEOBJECT *, GIZFORCE_PC *);
#ifdef __cplusplus
}
#endif
#ifndef TYPES_OFF_21

typedef struct force_glow_reader_pc_s {
    u8 unknown_0000[0xd14];           // 0x000
    GAMEOBJECT *force_glow_object;    // 0xd14
    GAMEOBJECT *force_glow_candidate; // 0xd18
} FORCE_GLOW_READER_PC;               // size 0xd1c

typedef struct force_glow_packet_pc_s {
    u8 unknown000[0x650];         // 0x000
    void *object;                 // 0x650
    void *candidate;              // 0x654
    u8 unknown658[0x6bc - 0x658]; // 0x658
    f32 alpha;                    // 0x6bc
    f32 target;                   // 0x6c0
    f32 recovery;                 // 0x6c4
    f32 unknown6c8;               // 0x6c8
    f32 hold;                     // 0x6cc
} FORCE_GLOW_PACKET_PC;           // size 0x6d0

typedef struct GIZFORCE_ANIMATION_DATA_PC {
    u8 unknown_00[2];         // 0x0
    i16 platform_id;          // 0x2
} GIZFORCE_ANIMATION_DATA_PC; // size 0x4

typedef struct GIZFORCE_GROUP_BEST_PC {
    GIZFORCE_PC *forces[9]; // 0x00
    u8 flags;               // 0x24
    u8 unknown_25;          // 0x25
    u8 count;               // 0x26
} GIZFORCE_GROUP_BEST_PC;   // size 0x28

typedef struct GIZFORCE_BEST_PC {
    u8 unknown_000[0x1c];          // 0x00
    NUVEC position;                // 0x1c
    GAMEANIMSET *anim_set;         // 0x28
    u8 unknown_02c[0xc];           // 0x2c
    GAMEOBJECT *using_object;      // 0x38
    u32 unavailable_flags;         // 0x3c
    GIZFORCE_GROUP_BEST_PC *group; // 0x40
    u8 unknown_044[0x10];          // 0x44
    f32 interaction_radius;        // 0x54
    u8 unknown_058[0x1c];          // 0x58
    u32 config_flags;              // 0x74
    u8 unknown_078[0x28];          // 0x78
    u32 runtime_flags;             // 0xa0
} GIZFORCE_BEST_PC;                // size 0xa4

typedef struct GIZFORCE_SYS_BEST_PC {
    u8 unknown_000[4];            // 0x00
    GIZFORCE_PC **visible_forces; // 0x04
    u8 unknown_008[8];            // 0x08
    u16 visible_count;            // 0x10
    u8 unknown_012[2];            // 0x12
    GAMEANIMOBJPOOL *anim_pool;   // 0x14
} GIZFORCE_SYS_BEST_PC;           // size 0x18

typedef struct GIZFORCE_LOS_BEST_PC {
    u32 clear_bits[12]; // 0x00
    u32 queried_at[1];  // 0x30
} GIZFORCE_LOS_BEST_PC; // size 0x34

typedef struct GIZFORCE_OBJECT_BEST_PC {
    u8 unknown_000[0x10cc]; // 0x0000
    GIZFORCE_PC *target;    // 0x10cc
    GAMEANIMOBJ *animation; // 0x10d0
} GIZFORCE_OBJECT_BEST_PC;  // size 0x10d4

typedef struct GIZFORCE_PLATFORM_BEST_PC {
    u8 unknown_000[2];       // 0x0
    i16 platform_id;         // 0x2
} GIZFORCE_PLATFORM_BEST_PC; // size 0x4

typedef struct force_sfx_config_pc_s {
    u8 unknown_00[0x28];        // 0x00
    GAMEANIMSET *anim_set;      // 0x28
    u8 unknown_2c[0x82 - 0x2c]; // 0x2c
    i16 process_sfx;            // 0x82
    i16 complete_sfx;           // 0x84
    i16 return_sfx;             // 0x86
    u8 unknown_88[0xa4 - 0x88]; // 0x88
} FORCE_SFX_CONFIG_PC;          // size 0xa4

typedef struct force_sfx_system_pc_s {
    FORCE_SFX_CONFIG_PC *forces; // 0x0
    u8 unknown_04[0xe - 4];      // 0x4
    u16 count;                   // 0xe
} FORCE_SFX_SYSTEM_PC;           // size 0x10

typedef struct world_force_sfx_pc_s {
    u8 unknown_0000[0x46ac];           // 0x0000
    FORCE_SFX_SYSTEM_PC *force_system; // 0x46ac
} WORLD_FORCE_SFX_PC;                  // size 0x46b0

typedef struct trigger_force_object_pc_s {
    u8 unknown000[0x10cc]; // 0x0000
    GIZFORCE_PC *force;    // 0x10cc
} TRIGGER_FORCE_OBJECT_PC; // size 0x10d0

typedef struct FORCE_RESET_PROGRESS_PC {
    u32 enabled[4];             // 0x00
    u32 visible[4];             // 0x10
    u32 reward_released[4];     // 0x20
    u32 socket_hidden[4];       // 0x30
    u32 completion_released[4]; // 0x40
    u32 completion_pending[4];  // 0x50
    u32 destroyed[4];           // 0x60
    u8 group_members[8][8];     // 0x70
} FORCE_RESET_PROGRESS_PC;      // size 0xb0

typedef struct registry_force_record_pc_s {
    u8 unknown_00[0x28];    // 0x00
    GAMEANIMSET *anim_set;  // 0x28
    u8 unknown_2c[0x5c];    // 0x2c
    i16 blowup_type;        // 0x88
    u8 unknown_8a[0x16];    // 0x8a
    u32 runtime_flags;      // 0xa0
} REGISTRY_FORCE_RECORD_PC; // size 0xa4

typedef struct registry_force_system_pc_s {
    REGISTRY_FORCE_RECORD_PC *forces;          // 0x000
    REGISTRY_FORCE_RECORD_PC **visible_forces; // 0x004
    GIZMO_PC **hit_test_gizmos;                // 0x008
    u16 capacity;                              // 0x00c
    u16 count;                                 // 0x00e
    u16 visible_force_count;                   // 0x010
    u16 hit_test_gizmo_count;                  // 0x012
    GAMEANIMOBJPOOL *anim_pool;                // 0x014
    GIZFORCEGROUP_s groups[8];                 // 0x018
} REGISTRY_FORCE_SYSTEM_PC;                    // size 0x158

#endif /* TYPES_OFF_21 */
#undef SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART
#define SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_GIZMOS_TRAPS_GIZFORCE_H_STOP
