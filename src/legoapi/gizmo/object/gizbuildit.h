#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct WORLDINFO_s;
struct gameanimobj_s;
struct gameanimset_s;
struct gameobject_s;

/* GIZBUILDIT.availability_flags */
#define GIZBUILDIT_AVAILABLE_ACTIVE 0x1

/* A build-it: a pile of bricks the players build into the anim_objects, one batch per step. Indexed by the
 * GIZBUILDITSYS_s; the gizmo system's buildit gizmos point at these records. */
typedef struct GIZBUILDIT {              // size 0x7c
    char name[16];                       // 0x00
    struct gameanimset_s *anim_set;      // 0x10
    struct gameanimobj_s **anim_objects; // 0x14
    struct GIZBUILDIT *linked_buildit;   // 0x18
    NUVEC file_position;                 // 0x1c
    NUVEC start_position;                // 0x28
    NUVEC position;                      // 0x34
    f32 bounds_radius;                   // 0x40
    f32 step_timer;                      // 0x44
    f32 step_duration;                   // 0x48
    f32 interaction_radius;              // 0x4c
    f32 unknown_50;                      // 0x50
    i16 wobble_parameter;                // 0x54
    i16 completion_score;                // 0x56
    i16 unknown_58;                      // 0x58
    i16 unknown_5a;                      // 0x5a
    NUVEC effect_position;               // 0x5c
    f32 activation_radius;               // 0x68
    u8 anim_object_count;                // 0x6c
    u8 build_state;                      // 0x6d
    u8 builders_active;                  // 0x6e
    u8 built_object_count;               // 0x6f
    u8 batch_size;                       // 0x70
    u8 progress;                         // 0x71
    u16 state_flags;                     // 0x72
    u16 linked_rotation;                 // 0x74
    i16 room_index;                      // 0x76
    i16 blowup_type;                     // 0x78

    union { // size 0x2
        u16 availability_flags;

        struct {                     // size 0x2
            u16 active : 1;          // 0x7a:0
            u16 visible : 1;         // 0x7a:1
            u16 flag_04 : 1;         // 0x7a:2
            u16 unknown_bits_3 : 4;  // 0x7a:3
            u16 flag_80 : 1;         // 0x7a:7
            u16 unknown_bit_8 : 1;   // 0x7a:8
            u16 reward_released : 1; // 0x7a:9
        } availability;
    }; // 0x7a
} GIZBUILDIT;

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 BuilditHopSfxWait;
    extern f32 GIZBUILDITWOBBLEJUMPHEIGHT;
    extern i32 gizbuildit_gizmotype_id;
    i32 GizBuildIt_AtEnd(void *);
    void GizBuildIt_KillParts(struct GIZBUILDIT *);
    void GizBuildit_SetVisibility(struct GIZBUILDIT *, i32);
    u32 GizBuildIts_TotalScore(struct WORLDINFO_s *world);
    void GizDrawBuildItPiece(struct gameobject_s *, i32);
    void Hub_LockUnlockDoors(struct WORLDINFO_s *world);
    void Indy_GameAISysInit(void);
    extern i32 gizbuildit_gizmotype_id;
    void GizBuildIt_KillParts(GIZBUILDIT_s *);
#ifdef __cplusplus
}
#endif
