/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_PART
#define SAGA_LEGOAPI_GIZMO_OBJECT_GIZMOBLOWUPS_H_PART 1
#endif

#include "nu2api/nucore/common.h"

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

typedef struct game_antinode_data_s { // size 0x24
    NUVEC position;                   // 0x00
    f32 radius;                       // 0x0c
    f32 min_y;                        // 0x10
    f32 max_y;                        // 0x14
    f32 extent_x;                     // 0x18
    f32 extent_z;                     // 0x1c
    u16 angle;                        // 0x20
    u8 shape;                         // 0x22
    u8 mode;                          // 0x23
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

    i32 gizmoblowup_Load(void *, void *);
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

typedef struct GIZMOBLOWUPTYPE_PC {        // size 0x100
    NUSPECIALHANDLE alternate_specials[4]; // 0x00
    NUSPECIALHANDLE animated_special;      // 0x30
    NUSPECIALHANDLE decal_special;         // 0x3c
    NUSPECIALHANDLE shadow_special;        // 0x48
    NUSPECIALHANDLE burst_special;         // 0x54
    u8 unknown_60[0x18];                   // 0x60
    f32 animation_time_scale;              // 0x78
    u32 animation_flags;                   // 0x7c
    u32 effect_flags;                      // 0x80
    f32 field_084;                         // 0x84
    f32 animation_start_frame;             // 0x88
    f32 animation_end_frame;               // 0x8c
    f32 animation_base_frame;              // 0x90
    f32 field_094;                         // 0x94
    f32 field_098;                         // 0x98
    f32 field_09c;                         // 0x9c
    i16 particle_types[9];                 // 0xa0
    u16 instance_count;                    // 0xb2
    GAMEANTINODEDATA_PC anti_node_data;    // 0xb4
    char name[32];                         // 0xd8
    u8 field_0f8;                          // 0xf8
    u8 field_0f9;                          // 0xf9
    u8 field_0fa;                          // 0xfa
    u8 field_0fb;                          // 0xfb
    u8 animation_runtime_flags;            // 0xfc
    u8 unknown_fd[3];                      // 0xfd
} GIZMOBLOWUPTYPE_PC;

typedef struct GIZMOBLOWUP_PC { // size 0x114

    union { // size 0x40
        NUMTX transform;

        struct {                 // size 0x40
            u8 unknown_00[0x30]; // 0x000
            NUVEC position;      // 0x030
            u32 field_3c;        // 0x03c
        };
    }; // 0x000

    union { // size 0xc
        u8 unknown_40[0xc];
        NUVEC screen_position;
    }; // 0x040

    NUVEC mid_position;  // 0x04c
    NUVEC bounds_min;    // 0x058
    NUVEC bounds_max;    // 0x064
    NUVEC decal_offset;  // 0x070
    NUVEC field_07c;     // 0x07c
    NUVEC shadow_offset; // 0x088

    union { // size 0x4
        u32 status_flags;

        struct {                       // size 0x4
            u32 blown_up : 1;          // 0x094:0
            u32 unknown_status_1 : 13; // 0x094:1
            u32 visible : 1;           // 0x094:14
            u32 unknown_status_15 : 8; // 0x094:15
            u32 active : 1;            // 0x094:23
            u32 unknown_status_24 : 4; // 0x094:24
            u32 secondary_output : 1;  // 0x094:28
            u32 unknown_status_29 : 3; // 0x094:29
        };
    }; // 0x094

    u32 draw_flags;           // 0x098
    u32 secondary_flags;      // 0x09c
    u32 score;                // 0x0a0
    GIZMOBLOWUPTYPE_PC *type; // 0x0a4
    f32 target_scale;         // 0x0a8
    f32 proximity_radius;     // 0x0ac
    f32 effect_delay;         // 0x0b0
    f32 activation_delay;     // 0x0b4
    f32 proximity_delay;      // 0x0b8
    f32 animation_time;       // 0x0bc
    f32 decal_scale;          // 0x0c0
    f32 field_0c4;            // 0x0c4
    f32 animation_offset;     // 0x0c8
    f32 respawn_timer;        // 0x0cc
    f32 respawn_period;       // 0x0d0
    f32 reflection_height;    // 0x0d4
    void *anti_node;          // 0x0d8
    u16 decal_rotation_x;     // 0x0dc
    u16 decal_rotation_y;     // 0x0de
    u16 decal_rotation_z;     // 0x0e0
    u16 shadow_rotation_x;    // 0x0e2
    u16 shadow_rotation_y;    // 0x0e4
    u16 shadow_rotation_z;    // 0x0e6
    u16 model_rotation_x;     // 0x0e8
    u16 model_rotation_y;     // 0x0ea
    u16 model_rotation_z;     // 0x0ec
    u16 pickup_x_rotation;    // 0x0ee
    u16 pickup_y_rotation;    // 0x0f0
    char name[16];            // 0x0f2

    union { // size 0x2
        i16 platform_id;

        struct {                   // size 0x2
            u8 name_terminator;    // 0x102
            u8 platform_high_byte; // 0x103
        };
    }; // 0x102

    i16 burst_platform_id; // 0x104
    u8 unknown_106[6];     // 0x106
    u8 field_10c;          // 0x10c
    u8 score_multiplier;   // 0x10d
    u8 damage;             // 0x10e
    u8 initial_state;      // 0x10f
    u8 remaining_health;   // 0x110
    u8 initial_health;     // 0x111
    u8 unknown_112[2];     // 0x112
} GIZMOBLOWUP_PC;

#ifdef __cplusplus
extern "C" {
#endif
    extern void (*GizmoBlowUp_SfxFn)(GIZMOBLOWUP_PC *, NUVEC *);
    void GizBlowup_Respawn(GIZMOBLOWUP_PC *);
    void UpdateMidPos(GIZMOBLOWUP_PC *);

    i32 GizBlowup_InitSingleTerrain(GIZMOBLOWUP_PC *);
    void GizBlowup_DeleteSingleTerrain(GIZMOBLOWUP_PC *);
    void GizmoSwapBlowups(GIZMOBLOWUP_PC *, GIZMOBLOWUP_PC *);

    void GizmoBlowupUpdateMatrix(GIZMOBLOWUP_PC *);
    void GizmoBlowupGenDecalMatrix(GIZMOBLOWUP_PC *, NUMTX *, i32);
    void GizmoBlowupGenShadowMatrix(GIZMOBLOWUP_PC *, NUMTX *);
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
