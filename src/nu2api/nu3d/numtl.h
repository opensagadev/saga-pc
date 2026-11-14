/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

struct PC_MATERIAL_CREATE_SHADER;
struct PC_MATERIAL_SHADER_DESC_VIEW;
struct nudisplaylist_pc_s;

typedef struct numtlattrib_s {
    u32 alpha_mode : 4;
    u32 filter_mode : 2;
    u32 unknown_0_64_128 : 2;
    u32 unknown_1_1_2 : 2;
    u32 unknown_1_4_8 : 2;
    u32 cull_mode : 2;
    u32 z_mode : 2;
    u32 unknown_2_1_2 : 2;
    u32 unknown_2_4 : 1;
    u32 unknown_2_8 : 1;
    u32 unknown_20_31 : 12;
} NUMTLATTRIB;

STATIC_ASSERT(sizeof(NUMTLATTRIB) == 4);

typedef struct numtl_s {
    u16 flags;
    u8 unknown_0002[0x2e];
    struct numtl_s *next;
    u8 unknown_0034[0x8];
    struct nudisplaylist_pc_s *display_list;
    NUMTLATTRIB attribs;
    u32 unknown_0044_0_4 : 5;
    u32 unknown_0044_5 : 1;
    u32 unknown_0044_6 : 1;
    u32 unknown_0044_7_31 : 25;
    u8 unknown_0048[0xc];
    f32 diffuse_r;
    f32 diffuse_g;
    f32 diffuse_b;
    u8 unknown_0060[0x10];
    f32 opacity;
    i16 texture_id;
    i16 unknown_076;
    u8 unknown_078[0x21];
    i8 particle_type_tag;
} NUMTL;

STATIC_ASSERT(offsetof(NUMTL, flags) == 0);
STATIC_ASSERT(offsetof(NUMTL, next) == 0x30);
STATIC_ASSERT(offsetof(NUMTL, display_list) == 0x3c);
STATIC_ASSERT(offsetof(NUMTL, attribs) == 0x40);
STATIC_ASSERT(offsetof(NUMTL, diffuse_r) == 0x54);
STATIC_ASSERT(offsetof(NUMTL, opacity) == 0x70);
STATIC_ASSERT(offsetof(NUMTL, texture_id) == 0x74);
STATIC_ASSERT(offsetof(NUMTL, particle_type_tag) == 0x99);
#ifdef __cplusplus
extern "C" {
#endif
    /* Add inside existing C declarations. Native register helpers stay local. */
    void NuShaderMtlDescInit(struct PC_MATERIAL_CREATE_SHADER *);
    void NuMtlCreatePS(NUMTL *, i32);
    NUMTL *NuMtlCreate(i32);
    NUMTL *NuMtlCreate3D(i32);
    NUMTL *NuMtlCreateEx(i32, u32);
    NUMTL *NuMtlCreateEx3D(i32, u32);
    void NuMtlRegisterForOverride(NUMTL *, u32 *, i32);
    NUMTL *NuMtlFindVariantMtl(NUMTL *, NUMTL *);

    void NuMtlSetRenderPlane(NUMTL *, i32);
    void NuMtlUpdatePS(NUMTL *);

    void NuMtlUpdate(NUMTL *);
    void NuMtlDestroy(NUMTL *);
    void NuDisplayListDestroyMtl(NUMTL *);
    extern NUMTL *numtl_defaultmtl2d;
    extern NUMTL *numtl_defaultmtl3d;
    void NuDisplayListDestroyMtl(NUMTL *);
    void NuMtlSetShaderDescPS(NUMTL *material, const struct PC_MATERIAL_SHADER_DESC_VIEW *description);
    void NuMtlInitOverride(i32, VARIPTR *, VARIPTR *);
    void NuMtlSetUVOffsetPS(NUMTL *material, u32 layer, f32 u, f32 v);
    void NuMtlAnimateShaderMtlTextures(f32 frame_time);

    void NuMtlInitEx(VARIPTR *buffer, i32 material_count);
#ifdef __cplusplus
}
#endif
