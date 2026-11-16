#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct WORLDINFO_s;
struct gameobject_s;

typedef struct TUBE {             // size 0x40
    u8 unknown_00[0x10];          // 0x00
    NUVEC position;               // 0x10
    f32 height;                   // 0x1c
    f32 radius;                   // 0x20
    u8 unknown_24[0x4];           // 0x24
    f32 top;                      // 0x28
    f32 radius_squared;           // 0x2c
    u8 unknown_30[4];             // 0x30
    u8 flags;                     // 0x34
    u8 unknown_35_b[0x3];         // 0x35
    u32 occupied_object_masks[2]; // 0x38
} TUBE;

#ifdef __cplusplus
extern "C" {
#endif
    extern const char *TubeGizmoName;
    extern const char *Tube_OutputName;
    struct addgizmotype_s *Tubes_RegisterGizmo(i32);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void Tube_MoveCode(struct gameobject_s *, struct WORLDINFO_s *);
    TUBE *Tube_InAnyCylinder(struct WORLDINFO_s *, struct gameobject_s *, i32);
#ifdef __cplusplus
}
#endif
