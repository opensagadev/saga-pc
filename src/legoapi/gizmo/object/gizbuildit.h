#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct GIZBUILDIT_s;
struct WORLDINFO_s;
struct gameanimobj_s;
struct gameanimset_s;
struct gameobject_s;

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
    u8 unknown_4c[0x20];                 // 0x4c
    u8 anim_object_count;                // 0x6c
    u8 build_state;                      // 0x6d
    u8 builders_active;                  // 0x6e
    u8 built_object_count;               // 0x6f
    u8 unknown_70;                       // 0x70
    u8 progress;                         // 0x71
    u16 state_flags;                     // 0x72
    u8 unknown_74[6];                    // 0x74
    u16 availability_flags;              // 0x7a
} GIZBUILDIT;

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 BuilditHopSfxWait;
    extern f32 GIZBUILDITWOBBLEJUMPHEIGHT;
    extern i32 gizbuildit_gizmotype_id;
    i32 GizBuildIt_AtEnd(void *);
    void GizBuildIt_KillParts(struct GIZBUILDIT_s *);
    void GizBuildit_SetVisibility(struct GIZBUILDIT_s *, i32);
    u32 GizBuildIts_TotalScore(struct WORLDINFO_s *world);
    void GizDrawBuildItPiece(struct gameobject_s *, i32);
    void Hub_LockUnlockDoors(struct WORLDINFO_s *world);
    void Indy_GameAISysInit(void);
#ifdef __cplusplus
}
#endif
