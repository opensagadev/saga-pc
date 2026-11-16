#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/gizmo/base/gizmo.h"

typedef struct gizrandom_s { // size 0x3c
    char name[16];           // 0x00
    i32 output_count;        // 0x10
    i32 output_weights[8];   // 0x14
    i32 selected_output;     // 0x34
    u8 flags;                // 0x38
    u8 unknown_039[3];       // 0x39
} GIZRANDOM;

typedef struct gizrandomsys_s { // size 0xc
    GIZRANDOM *randoms;         // 0x0
    i32 count;                  // 0x4
    i32 capacity;               // 0x8
} GIZRANDOMSYS;

#ifdef __cplusplus
extern "C" {
#endif
    GIZMO *createGizRandom(void *, i32, i32 *, char *);
    extern i32 gizrandom_gizmotype_id;
    void GizRandom_Activate(GIZMO *gizmo, i32 active);
    void GizRandom_AddGizmos(struct gizmosys_s *gizmo_sys, i32 type_id, void *world_ptr, void *arg3);
    char *GizRandom_GetGizmoName(GIZMO *gizmo);
    i32 GizRandom_GetMaxGizmos(void *world_ptr);
    i32 GizRandom_GetNumOutputs(void *gizmo);
    i32 GizRandom_GetOutput(GIZMO *gizmo, i32 output_index, i32 arg2);
    char *GizRandom_GetOutputName(GIZMO *arg0, i32 arg1);
    void *GizRandom_ReserveBufferSpace(void *world_ptr);
#ifdef __cplusplus
}
#endif
