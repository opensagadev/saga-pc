#pragma once

#include "nu2api/nucore/common.h"

#include "editor/edlevelall.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/numtx.h"

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

struct VuMtx { // size 0x40

    union { // size 0x40
        NUMTX matrix;
        VuVec rows[4];
    }; // 0x00
#ifdef __cplusplus
    void SetTranslation(const VuVec &translation);
    VuVec &GetRow(i32 row);
    void SetScale(const VuVec &scale);
#endif
};

#ifdef __cplusplus

struct NuDynamicLight { // size 0x710
    void resetGeometry();
    NURNDRLIGHTRECORD render_sets[2]; // 0x000
    u8 unknown_700[0xc];              // 0x700
    i32 render_set_count;             // 0x70c
};

#endif
