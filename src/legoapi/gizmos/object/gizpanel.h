#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

/* PANEL_MOVEMENT.model_variant: who can use the panel (GizPanel_CanUsePanel) */
#define PANEL_VARIANT_ASTROMECH 0
#define PANEL_VARIANT_PROTOCOL_DROID 1
#define PANEL_VARIANT_BOUNTY_HUNTER 2 /* "Hunter_Granted" */
#define PANEL_VARIANT_STORMTROOPER 3  /* "Trooper_Granted" */

/* PANEL_MOVEMENT.flags runtime bits, PANEL_MOVEMENT.draw_flags */
#define PANEL_IN_USE 0x1            /* a character works the panel */
#define PANEL_USED 0x2              /* worked to the end: the panel shows its on state */
#define PANEL_VISIBLE 0x4           /* GizPanel_SetVisibility */
#define PANEL_ACTIVE 0x8            /* GizPanel_Activate */
#define PANEL_ARM_OUT 0x10          /* the arm reached out for the character */
#define PANEL_NO_MODELS 0x20        /* editor option (file version 3): no base, arm or state models */
#define PANEL_NO_TARGET 0x40        /* editor option (file version 5): no target indicator */
#define PANEL_ALT_ANIMATED 0x80     /* editor option (file version 6): the alternative animated model */
#define PANEL_ALT_BASE 0x1          /* draw_flags, editor option (file version 6): the alternative base model */
#define PANEL_CASTS_SHADOW 0x2      /* draw_flags, editor option (file version 7): drawn into the shadow map */
#define PANEL_NO_VOICE_IN_STORY 0x4 /* draw_flags, an editor option (file version 8): no droid interface line in story mode */

/* A wall panel that a character of the right kind uses (protocol droid, bounty hunter, storm trooper). */
typedef struct panel_movement_s { // size 0x90
    NUMTX matrix;                 // 0x00
    char name[16];                // 0x40
    NUVEC position;               // 0x50
    u16 y_rotation;               // 0x5c
    u8 unknown_5e[3];             // 0x5e
    u8 model_variant;             // 0x61

    union { // size 0x1
        u8 flags;

        struct {                 // size 0x1
            u8 in_use : 1;       // 0x62:0
            u8 used : 1;         // 0x62:1
            u8 visible : 1;      // 0x62:2
            u8 active : 1;       // 0x62:3
            u8 arm_out : 1;      // 0x62:4
            u8 no_models : 1;    // 0x62:5
            u8 no_target : 1;    // 0x62:6
            u8 alt_animated : 1; // 0x62:7
        };
    }; // 0x62

    union { // size 0x1
        u8 draw_flags;

        struct {                      // size 0x1
            u8 alt_base : 1;          // 0x63:0
            u8 casts_shadow : 1;      // 0x63:1
            u8 no_voice_in_story : 1; // 0x63:2
            u8 : 5;                   // 0x63:3
        };
    }; // 0x63

    NUVEC floor_position; // 0x64
    NUVEC target_offset;  // 0x70
    u16 arm_pitch;        // 0x7c
    u16 arm_roll;         // 0x7e
    u16 arm_x_rotation;   // 0x80
    u16 target_pitch;     // 0x82
    u16 target_roll;      // 0x84
    i16 platform_id;      // 0x86
    f32 activation_time;  // 0x88
    f32 target_scale;     // 0x8c
} PANEL_MOVEMENT;

typedef struct panel_load_system_s { // size 0x8
    i32 count;                       // 0x0
    PANEL_MOVEMENT *panels;          // 0x4
} PANEL_SYSTEM;

#ifdef __cplusplus
extern "C" {
#endif
    extern u32 LswForceUsedHintFlags;
    i32 GizPanel_UpdateHint(HINT *);
    extern const char EpisodeVI_shot_format[9];
    extern const char EpisodeVI_shot_a_format[10];
    extern const char EpisodeVI_panel2[7];
    extern i32 gizpanel_gizmotype_id;
    void GizPanel_GetAbsTargetPos(void *, NUVEC *, i32);
    void GizPanel_Activate(void *gizmo, i32 active);
    void GizPanel_AddGizmos(struct gizmosys_s *gizmo_sys, i32 type_id, void *world_ptr, void *arg3);
    void GizPanel_Draw(void *world_ptr, void *arg1, f32 arg2);
    char *GizPanel_GetGizmoName(void *gizmo);
    i32 GizPanel_GetMaxGizmos(void *world_ptr);
    i32 GizPanel_GetNumOutputs(void *arg0);
    i32 GizPanel_GetOutput(void *gizmo, i32 arg1, i32 arg2);
    char *GizPanel_GetOutputName(void *arg0, i32 output_index);
    i32 GizPanel_Load(void *world_ptr, void *arg1);
    void GizPanel_InitTerrain(WORLDINFO *world);
    void GizPanel_SetVisibility(GIZMO *gizmo, i32 visible);
    void GizPanel_Update(void *world_ptr, void *arg1, f32 delta_time);
    void *GizPanels_AllocateProgressData(void *buffer, void *buffer_end);
    void GizPanels_ClearProgress(void *arg0, void *progress_data);
    void *GizPanels_ReserveBufferSpace(void *world_ptr);
    void GizPanels_Reset(void *world_ptr, void *arg1, void *progress_ptr);
    void GizPanels_StoreProgress(void *world_ptr, void *arg1, void *progress_ptr);

    void GizPanel_MoveCode(WORLDINFO *, GAMEOBJECT *, i32);
    void GizPanel_Reset(void *panel);
    ADDGIZMOTYPE *GizPanel_RegisterGizmo(i32);
#ifdef __cplusplus
}
#endif
