#ifndef SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_PART
#define SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_PART == 1 && !defined(SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_STOP)

struct WORLDINFO_s;
struct bolt_s;
struct gameobject_s;

typedef struct game_antinode_data_pc_s {
    NUVEC position;
    f32 radius;
    f32 min_y;
    f32 max_y;
    f32 extent_x;
    f32 extent_z;
    u16 angle;
    u8 shape;
    u8 mode;
} GAMEANTINODEDATA_PC;
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 MAXBLOWUPRANDSPANG;
    extern void (*GameBlowUpBlownUpFn)(void *blowup);
    void GizBlowup_InitTerrain(void);
    void GizBlowup_DeleteTerrain(void);
    void SetLevelExBlowupFlags(u32);
    extern i32 GizmoBlowupNameCountPc;
    extern char GizmoBlowupNameTablePc[32][32];
    void GizmoBlowupResetNameTable(void);
    i32 GizmoBlowupGetNameTableId(char *);
    void GizmoBlowupLateUpdate(void *, void *, f32);
    void GizmoBlowupEarlyUpdate(void *, void *, f32);

    STATIC_ASSERT(sizeof(GAMEANTINODEDATA_PC) == 0x24);
    i32 gizmoblowup_Load(void *, void *);
    STATIC_ASSERT(offsetof(GAMEANTINODEDATA_PC, position) == 0x0);
    STATIC_ASSERT(offsetof(GAMEANTINODEDATA_PC, radius) == 0xc);
    STATIC_ASSERT(offsetof(GAMEANTINODEDATA_PC, min_y) == 0x10);
    STATIC_ASSERT(offsetof(GAMEANTINODEDATA_PC, max_y) == 0x14);
    STATIC_ASSERT(offsetof(GAMEANTINODEDATA_PC, extent_x) == 0x18);
    STATIC_ASSERT(offsetof(GAMEANTINODEDATA_PC, extent_z) == 0x1c);
    STATIC_ASSERT(offsetof(GAMEANTINODEDATA_PC, angle) == 0x20);
    STATIC_ASSERT(offsetof(GAMEANTINODEDATA_PC, shape) == 0x22);
    STATIC_ASSERT(offsetof(GAMEANTINODEDATA_PC, mode) == 0x23);
    u32 GetLevelExBlowupFlags(void);
    extern u32 EXBLOWUPFLAGS;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_PART
#define SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_PART == 2 && !defined(SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_STOP)

typedef struct GIZMOBLOWUPTYPE_PC {
    NUSPECIALHANDLE alternate_specials[4];
    NUSPECIALHANDLE animated_special;
    NUSPECIALHANDLE decal_special;
    NUSPECIALHANDLE shadow_special;
    NUSPECIALHANDLE burst_special;
    u8 unknown_60[0x18];
    f32 animation_time_scale;
    u32 animation_flags;
    u32 effect_flags;
    f32 field_084;
    f32 animation_start_frame;
    f32 animation_end_frame;
    f32 animation_base_frame;
    f32 field_094;
    f32 field_098;
    f32 field_09c;
    i16 particle_types[9];
    u16 instance_count;
    GAMEANTINODEDATA_PC anti_node_data;
    char name[32];
    u8 field_0f8;
    u8 field_0f9;
    u8 field_0fa;
    u8 field_0fb;
    u8 animation_runtime_flags;
    u8 unknown_fd[3];
} GIZMOBLOWUPTYPE_PC;

STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, animated_special) == 0x30);

typedef struct GIZMOBLOWUP_PC {
    union {
        NUMTX transform;

        struct {
            u8 unknown_00[0x30];
            NUVEC position;
            u32 field_3c;
        };
    };

    union {
        u8 unknown_40[0xc];
        NUVEC screen_position;
    };

    NUVEC mid_position;
    NUVEC bounds_min;
    NUVEC bounds_max;
    NUVEC decal_offset;
    NUVEC field_07c;
    NUVEC shadow_offset;

    union {
        u32 status_flags;

        struct {
            u32 blown_up : 1;
            u32 unknown_status_1 : 13;
            u32 visible : 1;
            u32 unknown_status_15 : 8;
            u32 active : 1;
            u32 unknown_status_24 : 4;
            u32 secondary_output : 1;
            u32 unknown_status_29 : 3;
        };
    };

    u32 draw_flags;
    u32 secondary_flags;
    u32 score;
    GIZMOBLOWUPTYPE_PC *type;
    f32 target_scale;
    f32 proximity_radius;
    f32 effect_delay;
    f32 activation_delay;
    f32 proximity_delay;
    f32 animation_time;
    f32 decal_scale;
    f32 field_0c4;
    f32 animation_offset;
    f32 respawn_timer;
    f32 respawn_period;
    f32 reflection_height;
    void *anti_node;
    u16 decal_rotation_x;
    u16 decal_rotation_y;
    u16 decal_rotation_z;
    u16 shadow_rotation_x;
    u16 shadow_rotation_y;
    u16 shadow_rotation_z;
    u16 model_rotation_x;
    u16 model_rotation_y;
    u16 model_rotation_z;
    u16 pickup_x_rotation;
    u16 pickup_y_rotation;
    char name[16];

    union {
        i16 platform_id;

        struct {
            u8 name_terminator;
            u8 platform_high_byte;
        };
    };

    i16 burst_platform_id;
    u8 unknown_106[6];
    u8 field_10c;
    u8 score_multiplier;
    u8 damage;
    u8 initial_state;
    u8 remaining_health;
    u8 initial_health;
    u8 unknown_112[2];
} GIZMOBLOWUP_PC;

STATIC_ASSERT(sizeof(GIZMOBLOWUP_PC) == 0x114);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, screen_position) == 0x40);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, mid_position) == 0x4c);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, status_flags) == 0x94);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, draw_flags) == 0x98);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, type) == 0xa4);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, proximity_radius) == 0xac);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, activation_delay) == 0xb4);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, proximity_delay) == 0xb8);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, animation_time) == 0xbc);
#ifdef __cplusplus
extern "C" {
#endif
    extern void (*GizmoBlowUp_SfxFn)(GIZMOBLOWUP_PC *, NUVEC *);
    void GizBlowup_Respawn(GIZMOBLOWUP_PC *);
    void UpdateMidPos(GIZMOBLOWUP_PC *);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, position) == 0x30);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, decal_special) == 0x3c);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, burst_special) == 0x54);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, animation_flags) == 0x7c);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, animation_start_frame) == 0x88);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, animation_end_frame) == 0x8c);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, effect_delay) == 0xb0);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, respawn_timer) == 0xcc);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, respawn_period) == 0xd0);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, anti_node) == 0xd8);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, platform_id) == 0x102);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, burst_platform_id) == 0x104);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, damage) == 0x10e);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, remaining_health) == 0x110);

    i32 GizBlowup_InitSingleTerrain(GIZMOBLOWUP_PC *);
    void GizBlowup_DeleteSingleTerrain(GIZMOBLOWUP_PC *);
    void GizmoSwapBlowups(GIZMOBLOWUP_PC *, GIZMOBLOWUP_PC *);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, platform_id) == 0x102);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, burst_platform_id) == 0x104);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, initial_health) == 0x111);
    STATIC_ASSERT(sizeof(GIZMOBLOWUPTYPE_PC) == 0x100);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, name) == 0xd8);

    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, transform) == 0);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, bounds_min) == 0x58);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, bounds_max) == 0x64);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, decal_offset) == 0x70);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, shadow_offset) == 0x88);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, decal_scale) == 0xc0);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, decal_rotation_x) == 0xdc);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, decal_rotation_y) == 0xde);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, decal_rotation_z) == 0xe0);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, shadow_rotation_x) == 0xe2);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, shadow_rotation_y) == 0xe4);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, shadow_rotation_z) == 0xe6);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, model_rotation_x) == 0xe8);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, model_rotation_y) == 0xea);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, model_rotation_z) == 0xec);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, animation_base_frame) == 0x90);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, particle_types) == 0xa0);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, instance_count) == 0xb2);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, animation_runtime_flags) == 0xfc);
    void GizmoBlowupUpdateMatrix(GIZMOBLOWUP_PC *);
    void GizmoBlowupGenDecalMatrix(GIZMOBLOWUP_PC *, NUMTX *, i32);
    void GizmoBlowupGenShadowMatrix(GIZMOBLOWUP_PC *, NUMTX *);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, alternate_specials) == 0);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, shadow_special) == 0x48);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, animation_time_scale) == 0x78);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, effect_flags) == 0x80);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, field_084) == 0x84);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, field_094) == 0x94);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, field_098) == 0x98);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, field_09c) == 0x9c);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, anti_node_data) == 0xb4);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, field_0f8) == 0xf8);
    STATIC_ASSERT(offsetof(GIZMOBLOWUPTYPE_PC, field_0fb) == 0xfb);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, field_07c) == 0x7c);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, secondary_flags) == 0x9c);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, score) == 0xa0);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, target_scale) == 0xa8);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, animation_offset) == 0xc8);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, reflection_height) == 0xd4);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, pickup_x_rotation) == 0xee);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, pickup_y_rotation) == 0xf0);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, name) == 0xf2);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, name_terminator) == 0x102);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, field_10c) == 0x10c);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, score_multiplier) == 0x10d);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, initial_state) == 0x10f);
    STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, initial_health) == 0x111);
    extern i32 (*BlowupExFunc)(GIZMOBLOWUP_PC *, i32);
    extern void (*GizmoBlowup_TransformDrawFn)(GIZMOBLOWUP_PC *target);
#ifdef __cplusplus
}
#endif
typedef i32 (*LEVEL_BLOWUP_CALLBACK_PC)(GIZMOBLOWUP_PC *, i32);
#ifdef __cplusplus
extern "C" {
#endif
    void SetLevelBlowupCallbackPc(LEVEL_BLOWUP_CALLBACK_PC callback);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_PART
#define SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_PART == 3 && !defined(SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 (*GizmoBlowUp_NoTargetFn)(struct WORLDINFO_s *, struct gameobject_s *);
    void GizmoBlowUp_AddEffects(NUVEC *position, GIZMOBLOWUP_PC *blowup, i32 add_offset, i32 flags,
                                struct gameobject_s *object);
    i32 GizmoBlowupCheckProximity(struct WORLDINFO_s *, struct gameobject_s *);
    i32 GizmoBlowupBlowup(GIZMOBLOWUP_PC *, i32, i32, i32, struct gameobject_s *, i32);
    GIZMOBLOWUPTYPE_PC *GizmoBlowupTypeAdd(struct WORLDINFO_s *, NUSPECIALHANDLE *, i32, i32 *);
    void GizmoBlowupTypeRemove(GIZMOBLOWUPTYPE_PC *, struct WORLDINFO_s *);
    void *gizmoblowup_reservebuffers(struct WORLDINFO_s *);
    void GizmoBlowupsFinalSetup(struct WORLDINFO_s *);
    void GizmoBlowupVisibilityOverrides(struct WORLDINFO_s *);
    GIZMOBLOWUP_PC *FindNearestGizmoBlowUp(struct WORLDINFO_s *, NUVEC *, f32);
    void GizmoSortBlowups(struct WORLDINFO_s *);
    i32 GizmoBlowupGetTypeFromNameTableId(struct WORLDINFO_s *, i32);
    GIZMOBLOWUPTYPE_PC *GizmoBlowup_FindType(char *, struct WORLDINFO_s *);
    void GizmoBlowUpTypeBlowUp(struct WORLDINFO_s *, i32, NUVEC *);
    i32 InitGizmoBlowupTypes(struct WORLDINFO_s *);
    i32 InitGizmoBlowups(struct WORLDINFO_s *);
    i32 InitGizmoBlowupsMtxBuffer(struct WORLDINFO_s *);
    GIZMOBLOWUP_PC *GizmoBlowUp_Hit(struct gameobject_s *, NUVEC *, i32, f32, NUVEC *, NUVEC *, struct bolt_s *, u32,
                                    u8 *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_PART
#define SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_STOP
