#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct WORLDINFO_s;
struct gameobject_s;

typedef struct TUBE {   // size 0x40
    char name[16];      // 0x00
    NUVEC position;     // 0x10
    f32 height;         // 0x1c
    f32 radius;         // 0x20
    f32 momentum_y;     // 0x24
    f32 top;            // 0x28
    f32 radius_squared; // 0x2c
    f32 audio_cooldown; // 0x30

    union { // size 0x1
        u8 flags;

        struct {             // size 0x1
            u8 active : 1;   // 0x34:0
            u8 visible : 1;  // 0x34:1
            u8 flag_04 : 1;  // 0x34:2
            u8 reversed : 1; // 0x34:3
            u8 unused : 4;   // 0x34:4
        } bits;
    }; // 0x34

    u8 padding[3];                // 0x35
    u32 occupied_object_masks[2]; // 0x38
} TUBE;

/* TUBE.flags (the bitfields of TUBE_CALLBACKS) */
#define TUBE_ACTIVE 0x1
#define TUBE_VISIBLE 0x2
#define TUBE_MAGNET 0x4   /* pulls droids that are not immune to magnets in, cannot be entered */
#define TUBE_REVERSED 0x8 /* Tube_ActivateRev */

#ifdef __cplusplus
extern "C" {
#endif
    extern char *TubeGizmoName;
    extern const char *Tube_OutputName;
    struct addgizmotype_s *Tubes_RegisterGizmo(i32);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void Tube_MoveCode(struct gameobject_s *, struct WORLDINFO_s *);
    i32 Tube_InCylinder(struct gameobject_s *object, TUBE *tube, f32 *horizontal_distance_squared, i32 ignore_height);
    TUBE *Tube_InAnyCylinder(struct WORLDINFO_s *, struct gameobject_s *, i32);
#ifdef __cplusplus
}
#endif
