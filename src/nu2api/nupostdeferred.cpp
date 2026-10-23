#include "saga.h"

/* Native private register parameters are expressed by typed source arguments. */
extern "C" {
    extern NUMTX PostShadowViewPc, PostShadowProjectionPc;
    extern POST_SHADER_PROGRAM_PC *PostBlur5x5ProgramPc;
    void __cdecl PCGraphicsClearFramebuffer(i32, u32);
    void __cdecl PCGraphicsEffectTextureSetFilterFields(GRAPHICS_RENDER_TEXTURE_PC *, i32, i32, i32);
    void __cdecl PCGraphicsEffectTextureSetLowNibbles(GRAPHICS_RENDER_TEXTURE_PC *, i32, i32);
    void __cdecl PCGraphicsAttachFramebuffer(PC_FRAMEBUFFER_PC *, i32, GRAPHICS_RENDER_TEXTURE_PC *, i32);
    void __cdecl PostBlur7x7DispatchPc(GRAPHICS_RENDER_TEXTURE_PC *, i32, GRAPHICS_RENDER_TEXTURE_PC *, i32, i32, i32,
                                       bool, f32, POST_SHADER_PROGRAM_PC *);
    extern NUVEC4 PostShadowWhitePc;
    extern const WCHAR PostDeferredLabelPc[17], PostShadowRenderLabelPc[14];
    extern PC_DISPLAY_ITEM_HANDLER_PC PCDisplayShadowItemHandlersPc[49];
    void __cdecl PCGraphicsResolveFramebufferPc(i32);
    void __cdecl PCGraphicsRenderTextureAllocationError(void);
    void __cdecl PcGetGeometryPerspectivePc(NUMTX *, f32 *, f32 *, f32 *, f32 *);
    void __cdecl PcUpdateGeometryCameraOriginPc(NUMTX *, NUMTX *);
}

extern "C" void __cdecl PCGraphicsResolveFramebufferPc(i32) {
}
void NuPostFilterGen::reset() {
    enabled = false;
}
extern "C" void __cdecl PostShadowSelectHandlersPc(i32 mode) {
    switch (mode) {
        case 0:
            PCDisplayItemHandlersPc = PCDisplayItemHandlerPrefixPc;
            break;
        case 1:
            PCDisplayItemHandlersPc = PCDisplayShadowItemHandlersPc;
            break;
        default:
            PCGraphicsRenderTextureAllocationError();
            break;
    }
}
extern "C" void __cdecl PostShadowCopyVectorsPc(NUVEC4 *out, const NUVEC4 *in, i32 count) {
    const NUVEC4 *end = in + count;
    while (in < end) {
        out->x = in->x;
        out->y = in->y;
        out->z = in->z;
        out->w = in->w;
        ++out;
        ++in;
    }
}
/* ECX=input, stack=output; EAX returns output. */
extern "C" NUVEC *__cdecl PostShadowNormalizePc(const NUVEC *input, NUVEC *out) {
    volatile f32 squared = input->y * input->y + input->x * input->x + input->z * input->z;
    volatile f32 length = NuFsqrt(squared);
    out->x = input->x / length;
    out->y = input->y / length;
    out->z = input->z / length;
    return out;
}
extern "C" void __cdecl PostShadowBuildBasisPc(NUVEC *direction, NUVEC *up, NUMTX *basis, NUMTX *inverse) {
    NUVEC cross, projected, normalized;
    NuMtxSetIdentity(inverse);
    inverse->m10 = -direction->x;
    inverse->m11 = -direction->y;
    inverse->m12 = -direction->z;
    cross.x = inverse->m12 * up->y - inverse->m11 * up->z;
    cross.y = up->z * inverse->m10 - inverse->m12 * up->x;
    cross.z = inverse->m11 * up->x - inverse->m10 * up->y;
    projected.x = inverse->m11 * cross.z - inverse->m12 * cross.y;
    projected.y = inverse->m12 * cross.x - inverse->m10 * cross.z;
    projected.z = inverse->m10 * cross.y - inverse->m11 * cross.x;
    NUVEC *result = PostShadowNormalizePc(&projected, &normalized);
    inverse->m20 = result->x;
    inverse->m21 = result->y;
    inverse->m22 = result->z;
    cross.x = inverse->m11 * inverse->m22 - inverse->m21 * inverse->m12;
    cross.y = inverse->m20 * inverse->m12 - inverse->m10 * inverse->m22;
    cross.z = inverse->m10 * inverse->m21 - inverse->m20 * inverse->m11;
    inverse->m00 = cross.x;
    inverse->m01 = cross.y;
    inverse->m02 = cross.z;
    NuMtxInv(basis, inverse);
}
/* Native EAX=count, ECX=vertices, stack offset/scale/four outputs. */
extern "C" void __cdecl PostShadowProjectBoundsPc(i32 count, const NUVEC4 *vertices, NUVEC *offset, f32 scale,
                                                  f32 *xmin_out, f32 *xmax_out, f32 *ymin_out, f32 *ymax_out) {
    f32 xmin = 3.4028234663852886e+38f, xmax = -3.4028234663852886e+38f, ymin = 3.4028234663852886e+38f,
        ymax = -3.4028234663852886e+38f;
    for (i32 i = 0; i < count; ++i) {
        NUVEC result;
        NuVecAdd(&result, (NUVEC *)&vertices[i], offset);
        volatile f32 x = result.x * scale / result.z;
        volatile f32 y = result.y * scale / result.z;
        if (xmin > x)
            xmin = x;
        else if (xmax < x)
            xmax = x;
        if (ymin > y)
            ymin = y;
        else if (ymax < y)
            ymax = y;
    }
    *xmin_out = xmin;
    *xmax_out = xmax;
    *ymin_out = ymin;
    *ymax_out = ymax;
}

#define SHADOW_COPY_VEC4(dst, src)                                                                                     \
    do {                                                                                                               \
        (dst).x = (src).x;                                                                                             \
        (dst).y = (src).y;                                                                                             \
        (dst).z = (src).z;                                                                                             \
        (dst).w = (src).w;                                                                                             \
    } while (0)
#define SHADOW_COPY_MTX(dst, src)                                                                                      \
    do {                                                                                                               \
        (dst).m00 = (src).m00;                                                                                         \
        (dst).m01 = (src).m01;                                                                                         \
        (dst).m02 = (src).m02;                                                                                         \
        (dst).m03 = (src).m03;                                                                                         \
        (dst).m10 = (src).m10;                                                                                         \
        (dst).m11 = (src).m11;                                                                                         \
        (dst).m12 = (src).m12;                                                                                         \
        (dst).m13 = (src).m13;                                                                                         \
        (dst).m20 = (src).m20;                                                                                         \
        (dst).m21 = (src).m21;                                                                                         \
        (dst).m22 = (src).m22;                                                                                         \
        (dst).m23 = (src).m23;                                                                                         \
        (dst).m30 = (src).m30;                                                                                         \
        (dst).m31 = (src).m31;                                                                                         \
        (dst).m32 = (src).m32;                                                                                         \
        (dst).m33 = (src).m33;                                                                                         \
    } while (0)
void PC_SPECIAL_SHADOW_LIGHT::ComputeShadowMatrixPc(NURNDRLIGHTRECORD *set) {
    NUMTX original = set->view_pc;
    NUMTX captured = PostShadowViewPc;
    NUMTX combined, combined_inverse, original_inverse, captured_inverse;
    NUMTX basis, basis_inverse, basis_copy;
    NUMTX translation, frustum, translated_basis, transformed, axis;
    NUVEC4 corners[10], projected[10];
    NUVEC light_direction, camera_direction;
    NUVEC offset, camera_position;
    NuMtxMulH(&combined, &original, &set->projection_pc);
    NuMtxInvH(&combined_inverse, &combined);
    NuMtxInv(&original_inverse, &original);
    NuMtxInv(&captured_inverse, &captured);
    light_direction = *(NUVEC *)&original_inverse.m20;
    camera_direction = *(NUVEC *)&captured_inverse.m20;
    PostShadowBuildBasisPc(&light_direction, &camera_direction, &basis, &basis_inverse);
    PostShadowCopyVectorsPc(corners, set->corners_pc, 8);
    {
        NUVEC temporary;
        NUVEC4 sum;
        temporary.x = shadow_direction_pc.x * 200.0;
        temporary.y = shadow_direction_pc.y * 200.0;
        temporary.z = shadow_direction_pc.z * 200.0;
        sum.x = captured_inverse.m30 + temporary.x;
        sum.y = captured_inverse.m31 + temporary.y;
        sum.z = captured_inverse.m32 + temporary.z;
        SHADOW_COPY_VEC4(corners[8], sum);
        temporary.x = shadow_direction_pc.x * 200.0;
        temporary.y = shadow_direction_pc.y * 200.0;
        temporary.z = shadow_direction_pc.z * 200.0;
        sum.x = captured_inverse.m30 - temporary.x;
        sum.y = captured_inverse.m31 - temporary.y;
        sum.z = captured_inverse.m32 - temporary.z;
        SHADOW_COPY_VEC4(corners[9], sum);
    }
    NUVEC minimum, maximum;
    minimum.x = 3.4028234663852886e+38f;
    minimum.y = 3.4028234663852886e+38f;
    minimum.z = 3.4028234663852886e+38f;
    maximum.x = -3.4028234663852886e+38f;
    maximum.y = -3.4028234663852886e+38f;
    maximum.z = -3.4028234663852886e+38f;
    for (i32 i = 0; i < 10; ++i) {
        __declspec(align(16)) NUVEC4 result;
        NUVEC bound;
        f32 bound_x, bound_y, bound_z;
        SHADOW_COPY_MTX(basis_copy, basis);
        NuVecMtxTransformH((NUVEC *)&result, (NUVEC *)&corners[i], &basis_copy);
        SHADOW_COPY_VEC4(projected[i], result);
        bound_z = minimum.z > result.z ? result.z : minimum.z;
        bound_y = minimum.y > result.y ? result.y : minimum.y;
        bound_x = minimum.x > result.x ? result.x : minimum.x;
        bound.x = bound_x;
        bound.y = bound_y;
        bound.z = bound_z;
        minimum = bound;
        bound_z = maximum.z < result.z ? result.z : maximum.z;
        bound_y = maximum.y < result.y ? result.y : maximum.y;
        bound_x = maximum.x < result.x ? result.x : maximum.x;
        bound.x = bound_x;
        bound.y = bound_y;
        bound.z = bound_z;
        maximum = bound;
    }
    f32 fovy, aspect, near_plane, far_plane;
    PcGetGeometryPerspectivePc(&PostShadowProjectionPc, &fovy, &aspect, &near_plane, &far_plane);
    f64 depth = (f64)maximum.z - (f64)minimum.z;
    volatile f32 depth32 = depth;
    f32 dot = camera_direction.y * light_direction.y + light_direction.x * camera_direction.x +
              camera_direction.z * light_direction.z;
    union {
        f32 f;
        u32 u;
    } absolute;
    absolute.u = *(u32 *)&dot & 0x7fffffff;
    f32 product = near_plane * depth32;
    f32 distance = NuFsqrt(product) + near_plane;
    distance = set->parameter_110 * distance / (f32)(1.0f - absolute.f);
    camera_position.x = captured_inverse.m30;
    camera_position.y = captured_inverse.m31;
    camera_position.z = captured_inverse.m32;
    NUVEC transformed_position;
    NuVecMtxTransformH(&transformed_position, &camera_position, &basis);
    volatile f32 midpoint_y = (minimum.y + maximum.y) * 0.5;
    volatile f32 lower_z = minimum.z - distance;
    offset.x = -transformed_position.x;
    offset.y = -midpoint_y;
    offset.z = -lower_z;
    f32 xmin, xmax, ymin, ymax;
    PostShadowProjectBoundsPc(10, projected, &offset, distance, &xmin, &xmax, &ymin, &ymax);
    NuMtxSetTranslation(&translation, &offset);
    f32 far_distance = depth + distance;
    NuMtxSetFrustumD3D(&frustum, xmin, xmax, ymin, ymax, distance, far_distance);
    axis.m00 = 1.0f;
    axis.m01 = 0.0f;
    axis.m02 = 0.0f;
    axis.m03 = 0.0f;
    axis.m10 = 0.0f;
    axis.m11 = 0.0f;
    axis.m12 = -1.0f;
    axis.m13 = 0.0f;
    axis.m20 = 0.0f;
    axis.m21 = 1.0f;
    axis.m22 = 0.0f;
    axis.m23 = 0.0f;
    axis.m30 = 0.0f;
    axis.m31 = 0.0f;
    axis.m32 = 0.0f;
    axis.m33 = 1.0f;
    NuMtxMulH(&transformed, &basis, &translation);
    NuMtxMulH(&translated_basis, &transformed, &frustum);
    NuMtxMulH(&combined_inverse, &translated_basis, &axis);
    set->shadow_matrix_pc = combined_inverse;
}
void PC_SPECIAL_SHADOW_LIGHT::SetupShadowPc(NURNDRLIGHTRECORD *set) {
    NUMTX inverse, bias;
    NuMtxInvH(&inverse, &PostShadowViewPc);
    ComputeShadowMatrixPc(set);
    bias = NuMtxSetIdentityTemplate;
    bias.m00 = 0.5f;
    bias.m11 = -0.5f;
    bias.m22 = 0.5f;
    bias.m32 = 0.5f;
    bias.m30 = 0.5f;
    bias.m31 = 0.5f;
    NuMtxMulH(&set->texture_matrix_pc, &inverse, &set->shadow_matrix_pc);
    NuMtxMulH(&set->texture_matrix_pc, &set->texture_matrix_pc, &bias);
}
void PC_SPECIAL_SHADOW_LIGHT::RenderShadowPc(i32 index, PC_FRAMEBUFFER_PC *) {
    NURNDRLIGHTRECORD &set = render_sets[index];
    if (set.shadow_lists_pc[0].material_last == set.shadow_lists_pc[0].first &&
        set.shadow_lists_pc[1].material_last == set.shadow_lists_pc[1].first && set.count == 0)
        return;
    SetupShadowPc(&set);
    PostShadowSelectHandlersPc(1);
    NUMTX axis = NuMtxSetIdentityTemplate, view_projection;
    axis.m22 = 0.5f;
    axis.m32 = 0.5f;
    NuMtxMulH(&view_projection, &set.shadow_matrix_pc, &axis);
    PcUpdateGeometryCameraOriginPc(&view_projection, &NuMtxSetIdentityTemplate);
    for (i32 i = 0; i < 2; ++i) {
        NU_LIGHT_SHADOW_LIST_VIEW_PC &list = set.shadow_lists_pc[i];
        if (list.material_last != list.first) {
            list.material_last->type = 0x84;
            list.material_last->next = NULL;
            list.material_last->id = 4;
            NuDisplayHandlerZModePc = 0;
            PCDeviceSetRenderState(7, 1);
            PCDeviceSetRenderState(14, 1);
            PCDeviceSetRenderState(23, 4);
            NuDisplayHandlerTintPc = PostShadowWhitePc;
            NuShaderManagerSetfv(0x38, &PostShadowWhitePc.x);
            NuDisplayListDrawItemsPc(list.first);
            list.material_last = list.first;
        }
    }
    for (i32 i = 0; i < set.count; ++i)
        NuDisplayListDrawRenderScene_candidate(set.pending[i]);
    PostShadowSelectHandlersPc(0);
}

void PC_SPECIAL_SHADOW_LIGHT::RenderConstantsPc(void *) {
    NUMTX inverse;
    NuMtxInv(&inverse, &view);
    f32 vector[4];
    if (parameter_4 == 0) {
        vector[0] = vector[1] = 0.0f;
        vector[2] = -1.0f;
        NuVecMtxRotate((NUVEC *)vector, (NUVEC *)vector, &inverse);
        NuVecMtxRotate((NUVEC *)vector, (NUVEC *)vector, &PcGeometryViewMatrixPc);
        NuVecNorm((NUVEC *)vector, (NUVEC *)vector);
    } else {
        vector[0] = inverse.m30;
        vector[1] = inverse.m31;
        vector[2] = inverse.m32;
        NuVecMtxTransformH((NUVEC *)vector, (NUVEC *)vector, &PcGeometryViewMatrixPc);
    }
    PCDeviceSetPixelShaderConstantsF(0x85, vector, 1);
    f32 parameters[4];
    parameters[0] = parameter_0;
    parameters[1] = parameter_1 > 0.0f ? 1.0f / parameter_1 : 0.0f;
    parameters[2] = 1.0f - parameter_2;
    parameters[3] = parameter_3;
    PCDeviceSetPixelShaderConstantsF(0x86, parameters, 1);
    f32 distance[4] = {active_render_set_count == 1 ? 1000.0f : shadow_distance_pc, 0.0f, 0.0f, 0.0f};
    PCDeviceSetPixelShaderConstantsF(0x88, distance, 1);
    NUMTX bias = NuMtxSetIdentityTemplate;
    bias.m00 = 0.5f;
    bias.m11 = -0.5f;
    bias.m22 = 0.5f;
    bias.m30 = bias.m31 = bias.m32 = 0.5f;
    for (i32 i = 0, reg = 0x8d; i < active_render_set_count; ++i, reg += 7) {
        NURNDRLIGHTRECORD &set = render_sets[i];
        NUMTX transposed, matrix;
        NuMtxTranspose(&transposed, &set.texture_matrix_pc);
        PCDeviceSetPixelShaderConstantsF(reg - 4, &transposed.m00, 4);
        NuMtxMulH(&matrix, &inverse, &set.shadow_matrix_pc);
        NuMtxMulH(&matrix, &matrix, &bias);
        NuMtxInvH(&matrix, &matrix);
        f32 column2[4] = {matrix.m02, matrix.m12, matrix.m22, matrix.m32};
        f32 column3[4] = {matrix.m03, matrix.m13, matrix.m23, matrix.m33};
        PCDeviceSetPixelShaderConstantsF(reg, column2, 1);
        PCDeviceSetPixelShaderConstantsF(reg + 1, column3, 1);
        f32 shadow[4] = {set.parameter_100, 1.0f - set.parameter_104, parameter_19, parameter_20 - parameter_19};
        PCDeviceSetPixelShaderConstantsF(reg + 2, shadow, 1);
    }
}

extern "C" void __cdecl NuDisplayListShadowTransformPc(void *data) {
    NUMTX *matrix = (NUMTX *)data;
    f32 saved33 = matrix->m33, saved23 = matrix->m23;
    matrix->m33 = 1.0f;
    matrix->m23 = 0.0f;
    NuShaderManagerSetfv(0x11, &matrix->m00);
    matrix->m33 = saved33;
    matrix->m23 = saved23;
}
extern "C" void __cdecl NuDisplayListShadowTransformTransposePc(void *data) {
    NUMTX *matrix = (NUMTX *)data;
    f32 saved33 = matrix->m33, saved32 = matrix->m32;
    matrix->m33 = 1.0f;
    matrix->m32 = 0.0f;
    NuShaderManagerSetElementsfv_transpose(0x11, 0, 1, &matrix->m00);
    matrix->m33 = saved33;
    matrix->m32 = saved32;
}
typedef struct PC_SHADOW_MATERIAL_READER_PC {
    union {
        NUMTL common;
        struct {
            u8 unknown_000[0x40];
            u32 attributes_word;
        };
    };
    u8 unknown_09c[0x154];
    u32 vertex_format;
    i16 shader_id, shadow_shader_id;
    u8 unknown_1f8[0xc4];
    PC_TEXTURE_RESOURCE_PC *vertex_declaration;
} PC_SHADOW_MATERIAL_READER_PC;
STATIC_ASSERT(offsetof(PC_SHADOW_MATERIAL_READER_PC, attributes_word) == 0x40);
STATIC_ASSERT(offsetof(PC_SHADOW_MATERIAL_READER_PC, vertex_format) == 0x1f0);
STATIC_ASSERT(offsetof(PC_SHADOW_MATERIAL_READER_PC, shadow_shader_id) == 0x1f6);
STATIC_ASSERT(offsetof(PC_SHADOW_MATERIAL_READER_PC, vertex_declaration) == 0x2bc);
extern "C" void __cdecl NuDisplayListShadowMaterialPc(void *data) {
    PC_SHADOW_MATERIAL_READER_PC *material = (PC_SHADOW_MATERIAL_READER_PC *)data;
    PC_SHADER_OBJECT_FULL_VIEW *shader =
        (PC_SHADER_OBJECT_FULL_VIEW *)PCShaderRecordFromHandle((u32)(i32)material->shadow_shader_id);
    if (shader != NULL) {
        NuDisplayHandlerMaterialPc = material;
        if (!(material->vertex_format & 0x300000) && material->common.display_list != NULL) {
            PCShaderSetMaterialParametersPc(shader, &material->common);
            NuShaderManagerBindShaderPc(shader);
            PCDeviceSetVertexDeclarationPc(material->vertex_declaration);
        }
        u32 flags = material->attributes_word;
        u32 z_mode = flags & 0xc000;
        if (z_mode == 0x4000)
            material->attributes_word = (flags & 0xffff7fff) | z_mode;
        NuMtlSetRenderStatesPS(&material->common);
        PCDeviceSetRenderState(27, 0);
    }
}

void NuDeferredFilterGen::render() {
    D3DPERF_BeginEvent(0xffffffff, PostDeferredLabelPc);
    PC_FRAMEBUFFER_PC *output =
        (PC_FRAMEBUFFER_PC *)PostResourcePortsPc[0].manager->entries[PostResourcePortsPc[0].index].data;
    PC_POST_BUFFER_PROXY_PC *color =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data;
    PC_POST_BUFFER_PROXY_PC *normal =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[2].manager->entries[PostResourcePortsPc[2].index].data;
    PC_POST_BUFFER_PROXY_PC *depth_rt =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[4].manager->entries[PostResourcePortsPc[4].index].data;
    PC_POST_BUFFER_PROXY_PC *depth =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[5].manager->entries[PostResourcePortsPc[5].index].data;
    PC_POST_BUFFER_PROXY_PC *resolve[4] = {color, normal, depth_rt, depth};
    for (i32 i = 0; i < 4; ++i) {
        if (!resolve[i]->resolved) {
            PCGraphicsFramebufferBarrierPc(resolve[i]->kind, resolve[i]->enabled);
            resolve[i]->resolved = true;
        }
    }
    GRAPHICS_RENDER_TEXTURE_PC *normal_texture = normal->texture;
    GRAPHICS_RENDER_TEXTURE_PC *color_texture = color->texture;
    GRAPHICS_RENDER_TEXTURE_PC *depth_texture = depth_rt->texture;
    GRAPHICS_RENDER_TEXTURE_PC *destination = PCGraphicsGetFramebufferAttachmentPc(output, 0, NULL, NULL);
    volatile f32 last_sample = sample_count - 1.0;
    bool first = true;
    for (i32 i = 0; i < dynamic_light_count; ++i) {
        PC_SPECIAL_SHADOW_LIGHT *light = (PC_SPECIAL_SHADOW_LIGHT *)dynamic_lights[i];
        if (light->enabled == 0)
            continue;
        NUMTX saved_view = PcGeometryViewMatrixPc, saved_projection = PcGeometryProjectionMatrixPc;
        PCDeviceSetRenderState(22, 1);
        PCDeviceSetRenderState(15, 1);
        PCDeviceSetRenderState(27, 1);
        PCDeviceSetRenderState(7, 1);
        PCDeviceSetRenderState(23, 4);
        D3DPERF_BeginEvent(0xffffffff, PostShadowRenderLabelPc);
        for (i32 shadow = 0; shadow < light->active_render_set_count; ++shadow) {
            PCGraphicsAttachFramebuffer(shadow_fbos[shadow], 0, textures[4 + shadow], 0);
            PCGraphicsAttachFramebuffer(shadow_fbos[shadow], 4, textures[2], 0);
            PCGraphicsBindFramebuffer(shadow_fbos[shadow]);
            i32 width, height;
            PCGraphicsEffectTextureDimensions(textures[2], 0, &width, &height);
            PCGraphicsSubmitViewportPc(0, 0, width, height);
            PCGraphicsClearFramebuffer(0x300, 0xffffffff);
            light->RenderShadowPc(shadow, shadow_fbos[shadow]);
            PCGraphicsResolveFramebufferPc(1);
        }
        D3DPERF_EndEvent();
        PcUpdateGeometryCameraOriginPc(&saved_view, &saved_projection);
        if (PCGraphicsGetFramebufferAttachmentPc(light_fbo, 0, NULL, NULL) == NULL)
            PCGraphicsAttachFramebuffer(light_fbo, 0, textures[0], 0);
        PCGraphicsBindFramebuffer(light_fbo);
        if (first) {
            PCGraphicsClearFramebuffer(0x100, 0xff0000ff);
            PCDeviceSetRenderState(22, 1);
            PCDeviceSetRenderState(15, 0);
            PCDeviceSetRenderState(27, 0);
            PCDeviceSetRenderState(7, 0);
            PCDeviceSetRenderState(168, 15);
        }
        POST_SHADER_PROGRAM_PC *program =
            (POST_SHADER_PROGRAM_PC *)programs[light->parameter_5 == 0 || light->parameter_5 == 1 ? 0 : 1];
        if (!first) {
            NuDisplayHandlerVertexShaderCachePc = NULL;
            NuDisplayHandlerPixelShaderCachePc = NULL;
        }
        PostShaderProgramBindPc(program);
        light->RenderConstantsPc(program);
        PCGraphicsEffectTextureSetFilterFields(depth_texture, 2, 2, 0);
        {
            const u32 stage_pc = 0;
            GRAPHICS_RENDER_TEXTURE_PC *texture_pc = depth_texture;
            if (texture_pc != NULL) {
                u32 *tuple_pc = PCGraphicsEffectTextureSamplerPc(texture_pc);
                if ((u32)PCDeviceSamplerInvalidation[stage_pc][0] != tuple_pc[0] ||
                    (u32)PCDeviceSamplerInvalidation[stage_pc][1] != tuple_pc[1])
                    PCGraphicsSetSamplerTuplePc(stage_pc, PCGraphicsEffectTextureSamplerPc(texture_pc));
            }
            PCGraphicsBindEffectTexture(texture_pc, stage_pc);
        }
        {
            const u32 stage_pc = 1;
            GRAPHICS_RENDER_TEXTURE_PC *texture_pc = textures[4];
            if (texture_pc != NULL) {
                u32 *tuple_pc = PCGraphicsEffectTextureSamplerPc(texture_pc);
                if ((u32)PCDeviceSamplerInvalidation[stage_pc][0] != tuple_pc[0] ||
                    (u32)PCDeviceSamplerInvalidation[stage_pc][1] != tuple_pc[1])
                    PCGraphicsSetSamplerTuplePc(stage_pc, PCGraphicsEffectTextureSamplerPc(texture_pc));
            }
            PCGraphicsBindEffectTexture(texture_pc, stage_pc);
        }
        {
            const u32 stage_pc = 4;
            GRAPHICS_RENDER_TEXTURE_PC *texture_pc = textures[5];
            if (texture_pc != NULL) {
                u32 *tuple_pc = PCGraphicsEffectTextureSamplerPc(texture_pc);
                if ((u32)PCDeviceSamplerInvalidation[stage_pc][0] != tuple_pc[0] ||
                    (u32)PCDeviceSamplerInvalidation[stage_pc][1] != tuple_pc[1])
                    PCGraphicsSetSamplerTuplePc(stage_pc, PCGraphicsEffectTextureSamplerPc(texture_pc));
            }
            PCGraphicsBindEffectTexture(texture_pc, stage_pc);
        }
        if (normal_texture != NULL) {
            PCGraphicsEffectTextureSetFilterFields(normal_texture, 1, 1, 0);
            {
                const u32 stage_pc = 2;
                GRAPHICS_RENDER_TEXTURE_PC *texture_pc = normal_texture;
                if (texture_pc != NULL) {
                    u32 *tuple_pc = PCGraphicsEffectTextureSamplerPc(texture_pc);
                    if ((u32)PCDeviceSamplerInvalidation[stage_pc][0] != tuple_pc[0] ||
                        (u32)PCDeviceSamplerInvalidation[stage_pc][1] != tuple_pc[1])
                        PCGraphicsSetSamplerTuplePc(stage_pc, PCGraphicsEffectTextureSamplerPc(texture_pc));
                }
                PCGraphicsBindEffectTexture(texture_pc, stage_pc);
            }
        }
        PCDeviceSetRenderState(22, 1);
        PCDeviceSetRenderState(15, 0);
        PCDeviceSetRenderState(27, 0);
        PCDeviceSetRenderState(7, 0);
        PCDeviceSetRenderState(168, 15);
        PCGraphicsDrawPostEffectQuadPc();
        PCGraphicsFramebufferBarrierPc(0, 0);
        for (i32 stage = 0; stage < 6; ++stage)
            PCGraphicsBindEffectTexture(NULL, stage);
        first = false;
    }
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(27, 0);
    PCDeviceSetRenderState(7, 0);
    PCDeviceSetRenderState(168, 15);
    PostBlur7x7DispatchPc(textures[0], 0, textures[0], 1, 1, sample_count - 1, true, 1.0f, PostBlur5x5ProgramPc);
    PCGraphicsBindFramebuffer(output);
    i32 width = NuFramebufferGetWidth(output), height = NuFramebufferGetHeight(output);
    PCGraphicsSubmitViewportPc(0, 0, width, height);
    bool special = output == NuFramebufferGetObject(1) && PCDeviceBackend.textures[0] != NULL;
    PostShaderProgramBindPc((POST_SHADER_PROGRAM_PC *)programs[special ? 2 : 3]);
    f32 parameters_pc[4] = {parameters[1], last_sample, parameters[2], parameters[3]};
    PCDeviceSetPixelShaderConstantsF(0xa0, parameters_pc, 1);
    if (output == NuFramebufferGetObject(1) && PCDeviceBackend.textures[0] != NULL) {
        PCGraphicsEffectTextureSetFilterFields(color_texture, 2, 2, 0);
        {
            const u32 stage_pc = 4;
            GRAPHICS_RENDER_TEXTURE_PC *texture_pc = color_texture;
            if (texture_pc != NULL) {
                u32 *tuple_pc = PCGraphicsEffectTextureSamplerPc(texture_pc);
                if ((u32)PCDeviceSamplerInvalidation[stage_pc][0] != tuple_pc[0] ||
                    (u32)PCDeviceSamplerInvalidation[stage_pc][1] != tuple_pc[1])
                    PCGraphicsSetSamplerTuplePc(stage_pc, PCGraphicsEffectTextureSamplerPc(texture_pc));
            }
            PCGraphicsBindEffectTexture(texture_pc, stage_pc);
        }
        PCDeviceSetRenderState(27, 0);
        PCDeviceSetRenderState(15, 0);
    } else {
        PCDeviceSetRenderState(27, 1);
        PCDeviceSetRenderState(15, 0);
        PCDeviceSetRenderState(19, 5);
        PCDeviceSetRenderState(20, 6);
        PCGraphicsBindEffectTexture(NULL, 4);
    }
    PCDeviceSetRenderState(190, 0);
    PCDeviceSetRenderState(191, 0);
    PCDeviceSetRenderState(192, 0);
    PCGraphicsEffectTextureSetFilterFields(textures[0], 2, 2, 2);
    PCGraphicsEffectTextureSetLowNibbles(textures[0], 0, (i32)last_sample + 1);
    {
        const u32 stage_pc = 5;
        GRAPHICS_RENDER_TEXTURE_PC *texture_pc = textures[0];
        if (texture_pc != NULL) {
            u32 *tuple_pc = PCGraphicsEffectTextureSamplerPc(texture_pc);
            if ((u32)PCDeviceSamplerInvalidation[stage_pc][0] != tuple_pc[0] ||
                (u32)PCDeviceSamplerInvalidation[stage_pc][1] != tuple_pc[1])
                PCGraphicsSetSamplerTuplePc(stage_pc, PCGraphicsEffectTextureSamplerPc(texture_pc));
        }
        PCGraphicsBindEffectTexture(texture_pc, stage_pc);
    }
    PCGraphicsDrawPostEffectQuadPc();
    color->texture = destination;
    color->kind = 0;
    color->enabled = true;
    color->resolved = false;
    PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data = color;
    PCDeviceSetRenderState(190, 0xffffffff);
    PCDeviceSetRenderState(191, 0xffffffff);
    PCDeviceSetRenderState(192, 0xffffffff);
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(27, 0);
    PCDeviceSetRenderState(7, 0);
    PCDeviceSetRenderState(168, 15);
    D3DPERF_EndEvent();
}

#pragma bss_seg(".bss$PostShadowViewPc")
extern "C" NUMTX PostShadowViewPc = {0};
#pragma bss_seg(".bss$PostShadowProjectionPc")
extern "C" NUMTX PostShadowProjectionPc = {0};
#pragma bss_seg()
#pragma data_seg(".data$PostShadowWhitePc")
extern "C" NUVEC4 PostShadowWhitePc = {1.0f, 1.0f, 1.0f, 1.0f};
#pragma data_seg()
extern "C" {
    extern const WCHAR PostDeferredLabelPc[17] = L"Deferred shading";
    extern const WCHAR PostShadowRenderLabelPc[14] = L"Shadow render";
}
#pragma data_seg(".data$PCDisplayShadowItemHandlersPc")
extern "C" PC_DISPLAY_ITEM_HANDLER_PC PCDisplayShadowItemHandlersPc[49] = {NuDisplayListShadowMaterialPc,
                                                                           NULL,
                                                                           NuDisplayListDrawGeometryPc,
                                                                           NuDisplayListShadowTransformPc,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NuDisplayListDrawGeometryPc,
                                                                           NuDisplayListShadowTransformTransposePc,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NuDisplayListDrawGeometryPc,
                                                                           NuDisplayListSkinPalettePc,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NuDisplayListVertexGroupsPc,
                                                                           NuDisplayListVertexOffsetsPc,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL,
                                                                           NULL};
#pragma data_seg()
