#ifndef SAGA_NU2API_NUCORE_NUDYNAMICLIGHT_H_PART
#define SAGA_NU2API_NUCORE_NUDYNAMICLIGHT_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "decomp.h"
#include "editor/edlevelall.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/numtx.h"
#endif

/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NUCORE_NUDYNAMICLIGHT_H_PART == 1 && !defined(SAGA_NU2API_NUCORE_NUDYNAMICLIGHT_H_STOP)

#ifdef __cplusplus
extern "C" {
#endif
    struct PC_SPECIAL_SHADOW_LIGHT *NuDynamicLightCreate(void);
    void NuDynamicLightAddRenderScene(NURNDRLIGHTRECORD *records, i32 index, i32 scene_id);
#ifdef __cplusplus
}
#endif
typedef struct NuDynamicLight NuDynamicLight;
typedef struct VuMtx VuMtx;

struct VuMtx {
    union {
        NUMTX matrix;
        VuVec rows[4];
    };
#ifdef __cplusplus
    void fun_006341a0(const VuVec &translation);
    VuVec &fun_006341d0(i32 row);
    void fun_006b8470(const VuVec &scale);
#endif
};

STATIC_ASSERT(sizeof(struct VuMtx) == 64);
#undef SAGA_NU2API_NUCORE_NUDYNAMICLIGHT_H_PART
#define SAGA_NU2API_NUCORE_NUDYNAMICLIGHT_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUCORE_NUDYNAMICLIGHT_H_STOP
#endif
#endif

#if SAGA_NU2API_NUCORE_NUDYNAMICLIGHT_H_PART == 2 && !defined(SAGA_NU2API_NUCORE_NUDYNAMICLIGHT_H_STOP)
#ifdef __cplusplus

struct NuDynamicLight {
    void resetGeometry();
    NURNDRLIGHTRECORD render_sets_pc[2];
    u8 unknown_700[0xc];
    i32 render_set_count_pc;
};

STATIC_ASSERT(offsetof(NuDynamicLight, render_sets_pc) == 0);
STATIC_ASSERT(offsetof(NuDynamicLight, render_set_count_pc) == 0x70c);
#endif
#undef SAGA_NU2API_NUCORE_NUDYNAMICLIGHT_H_PART
#define SAGA_NU2API_NUCORE_NUDYNAMICLIGHT_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUCORE_NUDYNAMICLIGHT_H_STOP
#endif
#endif

#undef SAGA_NU2API_NUCORE_NUDYNAMICLIGHT_H_STOP
