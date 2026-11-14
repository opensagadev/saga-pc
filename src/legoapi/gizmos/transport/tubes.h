#ifndef SAGA_LEGOAPI_GIZMOS_TRANSPORT_TUBES_H_PART
#define SAGA_LEGOAPI_GIZMOS_TRANSPORT_TUBES_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_GIZMOS_TRANSPORT_TUBES_H_PART == 1 && !defined(SAGA_LEGOAPI_GIZMOS_TRANSPORT_TUBES_H_STOP)

struct WORLDINFO_s;
struct gameobject_s;

typedef struct TUBE_PC {
    u8 unknown_00[0x10];
    NUVEC position;
    f32 height;
    f32 radius;
    u8 unknown_24[0x4];
    f32 top;
    f32 radius_squared;
    u8 unknown_30[4];
    u8 flags;
    u8 unknown_35_b[0x3];
    u32 occupied_object_masks[2];
} TUBE_PC;

STATIC_ASSERT(sizeof(TUBE_PC) == 0x40);
STATIC_ASSERT(offsetof(TUBE_PC, position) == 0x10);
STATIC_ASSERT(offsetof(TUBE_PC, top) == 0x28);
STATIC_ASSERT(offsetof(TUBE_PC, radius_squared) == 0x2c);
STATIC_ASSERT(offsetof(TUBE_PC, flags) == 0x34);
#ifdef __cplusplus
extern "C" {
#endif
    extern const char *TubeGizmoNamePc;
    extern const char *Tube_OutputName;
    struct addgizmotype_pc_s *Tubes_RegisterGizmo(i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMOS_TRANSPORT_TUBES_H_PART
#define SAGA_LEGOAPI_GIZMOS_TRANSPORT_TUBES_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMOS_TRANSPORT_TUBES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_GIZMOS_TRANSPORT_TUBES_H_PART == 2 && !defined(SAGA_LEGOAPI_GIZMOS_TRANSPORT_TUBES_H_STOP)

#ifdef __cplusplus
extern "C" {
#endif
    void Tube_MoveCode(struct gameobject_s *, struct WORLDINFO_s *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_GIZMOS_TRANSPORT_TUBES_H_PART
#define SAGA_LEGOAPI_GIZMOS_TRANSPORT_TUBES_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_GIZMOS_TRANSPORT_TUBES_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_GIZMOS_TRANSPORT_TUBES_H_STOP
