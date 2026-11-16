#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/menus/core/hint.h"
#include "nu2api/numath/nuvec.h"

struct PART;
struct WORLDINFO_s;
struct addgizmotype_s;
struct gameanimobj_s;
struct gameanimobjpool_s;
struct gameobject_s;
struct gizforce_s;
struct gizmo_s;

typedef struct addgizmotype_s *(*REGISTER_GIZMO_CONFIG_TYPE_FN)(i32 type_id);
#ifdef __cplusplus
extern "C" {
#endif

    extern const char VehicleSmartBombChargeName[10];
    extern const char VehicleShieldOnName[10];
    i32 GizForce_UpdateHint(HINT *);
    extern const char EpisodeVI_electric_off[13];
    extern const char EpisodeVI_elecpad_zero_format[15];
    extern const char EpisodeVI_elecpad_format[14];
    extern const char EpisodeVI_eFloor_AlwaysOn[16];
    extern const char EpisodeVI_eFloor_AlwaysOf[16];
    extern const char EpisodeVI_eFloor_OnOff[13];
    extern i32 force_gizmotype_id;
    extern NUVEC ForceThrowMaulPosition;
    extern u32 lightning_col[2];
    extern f32 lightning_endw[2];
    extern f32 lightning_sizel[2];
    extern f32 lightning_sizew[2];
    extern f32 lightning_sizewab[2];
#ifdef __cplusplus
}
#endif

/* A stack of up to eight forces that rise on top of each other (group_index); taking the top one off clears the slot
 * above it, which for a full stack is combined_height. */
typedef struct gizforcegroup_s { // size 0x28

    union { // size 0x24

        struct {                          // size 0x24
            struct gizforce_s *forces[8]; // 0x00
            f32 combined_height;          // 0x20
        };

        u32 slot_words[9];
    }; // 0x00

    u16 runtime_flags;   // 0x24
    u8 member_count;     // 0x26
    u8 configured_count; // 0x27
} GIZFORCEGROUP;

typedef struct gizforcesys_s {           // size 0x158
    struct gizforce_s *forces;           // 0x000
    struct gizforce_s **visible_forces;  // 0x004
    struct gizmo_s **hit_test_gizmos;    // 0x008
    u16 capacity;                        // 0x00c
    u16 count;                           // 0x00e
    u16 visible_force_count;             // 0x010
    u16 hit_test_gizmo_count;            // 0x012
    struct gameanimobjpool_s *anim_pool; // 0x014
    GIZFORCEGROUP groups[8];             // 0x018
} GIZFORCESYS;

/* The saved state of up to 128 forces: one bit per force in each word set, the forces of each group by index. */
typedef struct gizforceprogress_s { // size 0xb0
    u32 enabled[4];                 // 0x00
    u32 visible[4];                 // 0x10
    u32 reward_released[4];         // 0x20
    u32 socket_hidden[4];           // 0x30
    u32 completion_released[4];     // 0x40
    u32 completion_pending[4];      // 0x50
    u32 destroyed[4];               // 0x60
    u8 group_members[8][8];         // 0x70
} GIZFORCEPROGRESS;

typedef struct gizforce_target_record_s { // size 0x10
    struct gizforce_s *force;             // 0x0
    struct gameanimobj_s *animation;      // 0x4
    f32 distance;                         // 0x8
    i32 index;                            // 0xc
} GIZFORCE_TARGET_RECORD;

#ifdef __cplusplus
extern "C" {
#endif
    extern GIZFORCE_TARGET_RECORD PossibleForceTargets[];
    i32 GizForce_FindBestForceTarget(void *, struct gameobject_s *);

    i32 GizForce_StoodOnForce(struct gizforce_s *, struct gameobject_s *);
    u16 GizForces_AngleToForce(NUVEC *, struct gizforce_s *);
    i32 GizForce_Complete(struct gizforce_s *);
    void ForceLightning_Origin(struct gameobject_s *, NUVEC *, NUVEC *);
    struct PART *GizForce_Throw(struct gameobject_s *, struct gizforce_s *, f32, f32, i32);
    void GizForce_ResetLOS(struct gameobject_s *);
    i32 GizForce_GameObjUsingForce(struct gameobject_s *, struct gizforce_s *);
    struct gizforce_s *GizForces_FindForce(struct WORLDINFO_s *world, char *name);
    i32 ForceAnimationDone(struct gizforce_s *force);
    void GizForce_PlayBackwards(struct gizforce_s *force);
    void GizForce_PlayForwards(struct gizforce_s *force);
#ifdef __cplusplus
}
#endif
