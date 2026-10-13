#include "saga.h"

typedef struct material_cleanup_flags_pc_s {
    u16 flags;
} MATERIAL_CLEANUP_FLAGS_PC;
STATIC_ASSERT(offsetof(MATERIAL_CLEANUP_FLAGS_PC, flags) == 0);

typedef struct material_override_cleanup_pc_s {
    NUMTL *material;
    NUMTL *original;
} MATERIAL_OVERRIDE_CLEANUP_PC;
STATIC_ASSERT(offsetof(MATERIAL_OVERRIDE_CLEANUP_PC, material) == 0);
STATIC_ASSERT(offsetof(MATERIAL_OVERRIDE_CLEANUP_PC, original) == 4);

void __cdecl NuMtlDestroy(NUMTL *material) {
    NULNKHDR *node;
    ((MATERIAL_CLEANUP_FLAGS_PC *)material)->flags &= ~1U;
    NuDisplayListDestroyMtl(material);
    node = NuLstGetNext(g_overrideList, NULL);
    while (node != NULL) {
        MATERIAL_OVERRIDE_CLEANUP_PC *entry = (MATERIAL_OVERRIDE_CLEANUP_PC *)node;
        node = NuLstGetNext(g_overrideList, node);
        if (entry->original == material) {
            ((MATERIAL_CLEANUP_FLAGS_PC *)entry->material)->flags &= ~1U;
            NuDisplayListDestroyMtl(entry->material);
            NuLstFree((NULNKHDR *)entry);
        }
    }
}

void __cdecl NuMtlUpdate(NUMTL *material) {
    NuMtlUpdatePS(material);
    ++((PC_SCENE_MTL_VIEW *)material)->version;
}

struct PC_MATERIAL_CREATE_SHADER {
    u32 flags;
    i32 diffuse_map_id;
    u8 unknown_08[0xc];
    u32 packed_diffuse_colour;
    u8 unknown_18[0xc];
    f32 default_scale;
    u8 unknown_28[0x80];
    u8 texture_unit_count;
    u8 unknown_a9[0x63];
    i32 animation_ids[4];
    u8 unknown_11c[0x20];
    u32 vertex_format;
    i16 shader_handle, shader_variant_handle;
    u8 unknown_144[0x74];
    u32 material_flags;
    u8 unknown_1bc[0x4c];
};
STATIC_ASSERT(sizeof(PC_MATERIAL_CREATE_SHADER) == 0x208);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, packed_diffuse_colour) == 0x14);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, default_scale) == 0x24);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, texture_unit_count) == 0xa8);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, animation_ids) == 0x10c);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, vertex_format) == 0x13c);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, shader_handle) == 0x140);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, material_flags) == 0x1b8);
typedef struct PC_MATERIAL_CREATE_VIEW PC_MATERIAL_CREATE_VIEW;
struct PC_MATERIAL_CREATE_VIEW {
    u16 flags;
    u8 unknown_002[0x26];
    u32 retained_28, retained_2c;
    NUMTL *next;
    u8 unknown_034[8];
    NUDISPLAYLIST_PC *display_list;
    u32 attributes, attributes_high;
    u8 unknown_048[0xc];
    NUVEC diffuse;
    u8 unknown_060[0x10];
    f32 opacity;
    i16 texture_id;
    u8 unknown_076[0x3e];
    PC_MATERIAL_CREATE_SHADER shader;
    PC_TEXTURE_RESOURCE_PC *vertex_declaration;
    u16 version;
    u8 unknown_2c2[2];
};
STATIC_ASSERT(sizeof(PC_MATERIAL_CREATE_VIEW) == 0x2c4);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, retained_28) == 0x28);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, retained_2c) == 0x2c);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, next) == 0x30);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, display_list) == 0x3c);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, attributes) == 0x40);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, attributes_high) == 0x44);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, diffuse) == 0x54);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, opacity) == 0x70);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, texture_id) == 0x74);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, shader) == 0xb4);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, vertex_declaration) == 0x2bc);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, version) == 0x2c0);
typedef struct PC_MATERIAL_VARIANT_RECORD PC_MATERIAL_VARIANT_RECORD;
struct PC_MATERIAL_VARIANT_RECORD {
    NUMTL *material;
    NUMTL *original;
    u32 vertex_descriptor;
};
STATIC_ASSERT(sizeof(PC_MATERIAL_VARIANT_RECORD) == 12);
STATIC_ASSERT(offsetof(PC_MATERIAL_VARIANT_RECORD, original) == 4);
STATIC_ASSERT(offsetof(PC_MATERIAL_VARIANT_RECORD, vertex_descriptor) == 8);

extern NUMTL *PCMaterialPoolPc;
extern i32 PCMaterialPoolCountPc;
void __cdecl NuMtlCreatePS(NUMTL *, i32);
void __cdecl NuShaderMtlDescInit(PC_MATERIAL_CREATE_SHADER *);
void __cdecl PCThreadCriticalSectionEnter_candidate(i32);
void __cdecl PCThreadCriticalSectionLeave_candidate(i32);
NULNKHDR *__cdecl NuLstAlloc(NULSTHDR *);

void __cdecl NuShaderMtlDescInit(PC_MATERIAL_CREATE_SHADER *desc) {
    i32 i;
    if (desc == NULL)
        return;
    memset(desc, 0, sizeof(*desc));
    desc->default_scale = 1.0f;
    desc->material_flags |= 0x1000;
    desc->flags = 0x1000;
    desc->packed_diffuse_colour = 0xff8080ff;
    for (i = 0; i < 4; ++i)
        desc->animation_ids[i] = -1;
    desc->vertex_format |= 0x101;
}

static NUMTL *__cdecl NuMtlCreatePoolPC(u32 render_plane, i32 count, i32 is_3d) {
    NUMTL *next = NULL;
    PC_MATERIAL_CREATE_VIEW *mtl;
    i32 i;
    for (i = 0; i < count; ++i) {
        PC_MATERIAL_CREATE_VIEW *pool;
        i32 j;
        mtl = NULL;
        pool = ((PC_MATERIAL_CREATE_VIEW *)(PCMaterialPoolPc));
        for (j = 0; j < PCMaterialPoolCountPc; ++j) {
            if ((pool[j].flags & 1) == 0 && pool[j].display_list == NULL) {
                mtl = &pool[j];
                break;
            }
        }
        if (mtl == NULL)
            AnimationDiagnosticNoOpPc();
        memset(mtl, 0, sizeof(*mtl));
        mtl->diffuse.x = mtl->diffuse.y = mtl->diffuse.z = mtl->opacity = 1.0f;
        mtl->attributes = (mtl->attributes & 0xbf9e2fd0) | 0x3f962010;
        mtl->attributes_high |= 0x10;
        mtl->flags = (mtl->flags & 0xf00f) | ((render_plane & 0xff) << 4) | 5;
        if (is_3d != 0) {
            PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
            if (PCDisplayMaterialAddCount == 600) {
                PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
                AnimationDiagnosticNoOpPc();
            } else {
                PCDisplayMaterialAddQueue[PCDisplayMaterialAddCount++] = ((NUMTL *)(mtl));
                PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
            }
        } else {
            mtl->attributes_high |= 0x800000;
        }
        mtl->next = next;
        next = ((NUMTL *)(mtl));
    }
    NuMtlCreatePS(((NUMTL *)(mtl)), is_3d);
    return ((NUMTL *)(mtl));
}

NUMTL *__cdecl NuMtlCreate(i32 count) {
    return NuMtlCreatePoolPC(numtl_renderplane, count, 0);
}
NUMTL *__cdecl NuMtlCreateEx(i32 count, u32 plane) {
    return NuMtlCreatePoolPC(plane, count, 0);
}
NUMTL *__cdecl NuMtlCreate3D(i32 count) {
    return NuMtlCreatePoolPC(numtl_renderplane, count, 1);
}
NUMTL *__cdecl NuMtlCreateEx3D(i32 count, u32 plane) {
    return NuMtlCreatePoolPC(plane, count, 1);
}

extern i32 NuMtlAnimateMaskPc;

void NuMtlAnimateSetSpeedScale(f32 scale) {
    FrameMaterialAnimationSpeedPc = scale;
}

i32 NuMtlSetCurrentRenderPlane(i32 render_plane) {
    i32 previous_render_plane = numtl_renderplane;
    numtl_renderplane = render_plane;
    return previous_render_plane;
}

void NuMtlAnimateSetMask(i32 mask) {
    NuMtlAnimateMaskPc = mask;
}

static NUMTL *__cdecl NuMtlLookupVariantPC(NUMTL *original, u32 descriptor) {
    NULNKHDR *node;
    for (node = NuLstGetNext(g_overrideList, NULL); node != NULL; node = NuLstGetNext(g_overrideList, node)) {
        PC_MATERIAL_VARIANT_RECORD *entry = ((PC_MATERIAL_VARIANT_RECORD *)(node));
        if (entry->original == original &&
            ((PC_MATERIAL_CREATE_VIEW *)(entry->material))->shader.vertex_format == descriptor)
            return entry->material;
    }
    return NULL;
}

void __cdecl NuMtlRegisterForOverride(NUMTL *original, u32 *descriptors, i32 count) {
    i32 i;
    for (i = 0; i < count; ++i) {
        if (NuMtlLookupVariantPC(original, descriptors[i]) == NULL) {
            PC_MATERIAL_VARIANT_RECORD *entry = ((PC_MATERIAL_VARIANT_RECORD *)(NuLstAlloc(g_overrideList)));
            PC_MATERIAL_CREATE_VIEW *destination;
            u32 retained_28, retained_2c;
            entry->material = NuMtlCreatePoolPC(numtl_renderplane, 1, 1);
            entry->original = original;
            entry->vertex_descriptor = descriptors[i];
            destination = ((PC_MATERIAL_CREATE_VIEW *)(entry->material));
            retained_28 = destination->retained_28;
            retained_2c = destination->retained_2c;
            *destination = *((PC_MATERIAL_CREATE_VIEW *)(original));
            destination->shader.shader_handle = destination->shader.shader_variant_handle = 0;
            destination->retained_28 = retained_28;
            destination->retained_2c = retained_2c;
            destination->flags |= 1;
            destination->shader.vertex_format = descriptors[i];
            destination->vertex_declaration = NuGetVertexDeclaration(destination->shader.vertex_format);
            NuMtlUpdate(entry->material);
        }
    }
}

NUMTL *__cdecl NuMtlFindVariantMtl(NUMTL *source, NUMTL *original) {
    u32 *descriptor = &((PC_MATERIAL_CREATE_VIEW *)(source))->shader.vertex_format;
    NUMTL *material = NuMtlLookupVariantPC(original, *descriptor);
    if (material != NULL)
        return material;
    NuMtlRegisterForOverride(original, descriptor, 1);
    return NuMtlLookupVariantPC(original, *descriptor);
}

typedef struct FRAME_MTL_ANIM_DESC_PC {
    u8 u_mode, v_mode, reserved02[2];
    f32 u_amplitude, v_amplitude, u_frequency, v_frequency;
} FRAME_MTL_ANIM_DESC_PC;
STATIC_ASSERT(sizeof(FRAME_MTL_ANIM_DESC_PC) == 0x14);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_frequency) == 0xc);
typedef struct FRAME_MTL_ANIM_PC {
    u8 reserved00[0x7b];
    u8 disable_u_animation, disable_v_animation;
    u8 reserved7d[0x143];
    i32 texture_animation_data[4];
    f32 texture_animation_offsets[4][2];
    u8 reserved1f0[8];
    FRAME_MTL_ANIM_DESC_PC animation[4];
} FRAME_MTL_ANIM_PC;
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, disable_u_animation) == 0x7b);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, disable_v_animation) == 0x7c);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, texture_animation_data) == 0x1c0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, texture_animation_offsets) == 0x1d0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, animation) == 0x1f8);
typedef struct FRAME_MTL_ANIM_SET_PC {
    NUDISPLAY_LIST_PC *scene;
    i32 material_count;
    i32 *material_indices;
    struct FRAME_MTL_ANIM_SET_PC *next;
} FRAME_MTL_ANIM_SET_PC;
STATIC_ASSERT(sizeof(FRAME_MTL_ANIM_SET_PC) == 0x10);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, next) == 0xc);
typedef struct FRAME_MTL_SCENE_PC {
    u8 reserved00[0x158];
    i32 shader_texture_animation_enabled;
} FRAME_MTL_SCENE_PC;
STATIC_ASSERT(offsetof(FRAME_MTL_SCENE_PC, shader_texture_animation_enabled) == 0x158);

extern void NuDisplayListAnimateMtls(f32);

void NuMtlSetUVOffsetPS(NUMTL *mtl, u32 layer, f32 u, f32 v) {
    FRAME_MTL_ANIM_PC *view = (FRAME_MTL_ANIM_PC *)mtl;
    view->texture_animation_offsets[layer][0] = u;
    view->texture_animation_offsets[layer][1] = v;
}
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_mode) == 0x0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_mode) == 0x1);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_amplitude) == 0x4);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_amplitude) == 0x8);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_frequency) == 0x10);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, scene) == 0x0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, material_count) == 0x4);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, material_indices) == 0x8);

static NUMTL *RendererZClearMaterialPc;
static NUMTL *RendererAlphaMaskMaterialPc;
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, version) == 0x2c0);

void __cdecl NuRndrInitGeneric(void) {
    i32 saved_plane = (i32)numtl_renderplane;
    numtl_renderplane = 22;
    if (RendererZClearMaterialPc == NULL) {
        PC_MATERIAL_CREATE_VIEW *material;
        RendererZClearMaterialPc = NuMtlCreatePoolPC(numtl_renderplane, 1, 0);
        material = (PC_MATERIAL_CREATE_VIEW *)RendererZClearMaterialPc;
        material->attributes = (material->attributes & 0xff9fbff2u) | 0x00108002;
        NuMtlUpdatePS(RendererZClearMaterialPc);
        ++material->version;
    }
    if (RendererAlphaMaskMaterialPc == NULL) {
        PC_MATERIAL_CREATE_VIEW *material;
        RendererAlphaMaskMaterialPc = NuMtlCreatePoolPC(numtl_renderplane, 1, 0);
        material = (PC_MATERIAL_CREATE_VIEW *)RendererAlphaMaskMaterialPc;
        material->attributes_high &= ~3u;
        material->attributes = (material->attributes & 0x806fbff2u) | 0x00608002;
    }
    if (saved_plane >= 24 || saved_plane < 0)
        AnimationDiagnosticNoOpPc();
    numtl_renderplane = saved_plane;
    RenderPostViewportResetPc();
}

STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, flags) == 0);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, diffuse_map_id) == 4);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, default_scale) == 0x24);
/* Native void platform initializer60ded0 tails60ddc0. Its shader-cache
   value-producing closure is genuinely absent; no replacement result exists. */
void __cdecl NuMtlInitEx(VARIPTR *buffer, i32 material_count) {
    PC_MATERIAL_CREATE_SHADER desc2d, desc3d;
    PC_MATERIAL_CREATE_VIEW *material;
    STUBBED_NAMED(0x0060ded0); /* NuMtlInitExPS(buffer): nativevoid; shader-factory closure held. */
    PCMaterialPoolCountPc = material_count;
    PCMaterialPoolPc = (NUMTL *)((buffer->addr + 15u) & ~15u);
    buffer->addr = (u32)PCMaterialPoolPc + material_count * sizeof(PC_MATERIAL_CREATE_VIEW);
    memset(PCMaterialPoolPc, 0, material_count * sizeof(PC_MATERIAL_CREATE_VIEW));
    numtl_defaultmtl3d = NuMtlCreatePoolPC(numtl_renderplane, 1, 1);
    numtl_defaultmtl2d = NuMtlCreatePoolPC(numtl_renderplane, 1, 0);
    memset(&desc2d, 0, sizeof(desc2d));
    desc2d.default_scale = 1.0f;
    desc2d.vertex_format |= 0x40101;
    desc2d.material_flags |= 0x1000;
    desc2d.flags = 0x1000;
    desc2d.diffuse_map_id = 0;
    desc2d.packed_diffuse_colour = 0xffffffff;
    memset(&desc3d, 0, sizeof(desc3d));
    desc3d.material_flags |= 0x1000;
    desc3d.default_scale = 1.0f;
    desc3d.vertex_format |= 0x101;
    desc3d.flags = 0x1000;
    desc3d.diffuse_map_id = 0;
    desc3d.packed_diffuse_colour = 0xffffffff;
    NuMtlSetShaderDescPS(numtl_defaultmtl2d, (const struct PC_MATERIAL_SHADER_DESC_VIEW *)&desc2d);
    NuMtlSetShaderDescPS(numtl_defaultmtl3d, (const struct PC_MATERIAL_SHADER_DESC_VIEW *)&desc3d);
    material = (PC_MATERIAL_CREATE_VIEW *)numtl_defaultmtl2d;
    NuMtlUpdatePS(numtl_defaultmtl2d);
    ++material->version;
    material = (PC_MATERIAL_CREATE_VIEW *)numtl_defaultmtl3d;
    NuMtlUpdatePS(numtl_defaultmtl3d);
    ++material->version;
}

void __cdecl NuMtlInitOverride(i32 count, VARIPTR *buffer, VARIPTR *end) {
    g_overrideList = NuLstCreateBuff(count, sizeof(PC_MATERIAL_VARIANT_RECORD), buffer, *end, 0x10);
}
