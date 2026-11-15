#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 Lever_BeingPulled(void *);
    extern i32 lever_gizmotype_id;
#ifdef __cplusplus
}
#endif
/* GIZMO_LEVER.flags, named like the bitfields of LEVER_FLAG_BITS (lever.c); the level file sets AUTO_RESET,
 * HIDE_BASE and HIDE_TARGET_INDICATOR. */
#define LEVER_FLAG_INTERACTING 0x1
#define LEVER_FLAG_BEING_PULLED 0x2
#define LEVER_FLAG_GOODIE 0x4 /* pulled by a goodie (not a baddie, not neutral) */
#define LEVER_FLAG_BADDIE 0x8
#define LEVER_FLAG_VISIBLE 0x10
#define LEVER_FLAG_AUTO_RESET 0x20
#define LEVER_FLAG_RETURNING 0x40
#define LEVER_FLAG_ENABLED 0x80
#define LEVER_FLAG_HIDE_BASE 0x100
#define LEVER_FLAG_HIDE_TARGET_INDICATOR 0x200
#define LEVER_FLAG_INTERACTION_FINISHED 0x800
#define LEVER_FLAG_PULLED_DOWN 0x1000

typedef struct GIZMO_LEVER GIZMO_LEVER;

struct GIZMO_LEVER {            // size 0xa0
    NUMTX matrix;               // 0x00
    NUVEC floor_position;       // 0x40
    NUVEC target_offset;        // 0x4c
    char name[16];              // 0x58
    NUVEC position;             // 0x68
    f32 idle_animation_timer;   // 0x74
    f32 pull_progress;          // 0x78
    f32 auto_reset_timer;       // 0x7c
    f32 auto_reset_delay;       // 0x80
    f32 target_indicator_scale; // 0x84
    u16 yaw;                    // 0x88
    i16 model_special_index;    // 0x8a
    i16 platform_id;            // 0x8c
    u16 animation_frame;        // 0x8e
    u16 terrain_pitch;          // 0x90
    u16 terrain_roll;           // 0x92

    union { // size 0x2
        u16 flags;

        struct {                           // size 0x2
            u16 interacting : 1;           // 0x94:0
            u16 being_pulled : 1;          // 0x94:1
            u16 goodie : 1;                // 0x94:2
            u16 baddie : 1;                // 0x94:3
            u16 visible : 1;               // 0x94:4
            u16 auto_reset : 1;            // 0x94:5
            u16 returning : 1;             // 0x94:6
            u16 enabled : 1;               // 0x94:7
            u16 hide_base : 1;             // 0x94:8
            u16 hide_target_indicator : 1; // 0x94:9
            u16 unused_10 : 1;             // 0x94:10
            u16 interaction_finished : 1;  // 0x94:11
            u16 pulled_down : 1;           // 0x94:12
            u16 unused_13 : 3;             // 0x94:13
        };
    }; // 0x94

    u8 model_variant;  // 0x96
    u8 reserved_97[9]; // 0x97
};

#ifdef __cplusplus
extern "C" {
#endif
    void Levers_InitTerrain(WORLDINFO *);
    void ReleaseLever(GAMEOBJECT *);
    void Lever_GetAbsTargetPos(GIZMO_LEVER *, NUVEC *);
    GIZMO_LEVER *Lever_FindNearest(WORLDINFO *, NUVEC *, GAMEOBJECT *, f32 *);
    i32 Lever_FullyPulledDown(GIZMO_LEVER *);
    void Lever_MoveCode(WORLDINFO *, GAMEOBJECT *);
    struct addgizmotype_s *Levers_RegisterGizmo(i32 type_id);
    extern char *LeverTypeNamePtr;
    extern char *LeverOutputDownNamePtr;
    extern char *LeverTypeNamePtr;
    extern char *LeverOutputDownNamePtr;
    extern char dat_0075ac34[5];
#ifdef __cplusplus
}
#endif
