#include "saga.h"
#include <string.h>
typedef struct PC_SPECIAL_GEOMETRY {
    u8 unknown_0000[8];
    u16 vertex_stride;
    u8 joint_indices[8];
    u8 unknown_0012[2];
    u32 base_vertex, vertex_count;
    u8 unknown_001c[8];
    PC_BUFFER_RESOURCE_PC *vertex_buffer;
    u32 morph_count;
    NUVEC **morph_deltas;
    u32 unknown_0030;
    void *dynamic_vertices;
} PC_SPECIAL_GEOMETRY;
STATIC_ASSERT(sizeof(PC_SPECIAL_GEOMETRY) == 0x38);
STATIC_ASSERT(offsetof(PC_SPECIAL_GEOMETRY, vertex_stride) == 8);
STATIC_ASSERT(offsetof(PC_SPECIAL_GEOMETRY, joint_indices) == 0xa);
STATIC_ASSERT(offsetof(PC_SPECIAL_GEOMETRY, base_vertex) == 0x14);
STATIC_ASSERT(offsetof(PC_SPECIAL_GEOMETRY, vertex_count) == 0x18);
STATIC_ASSERT(offsetof(PC_SPECIAL_GEOMETRY, vertex_buffer) == 0x24);
STATIC_ASSERT(offsetof(PC_SPECIAL_GEOMETRY, morph_count) == 0x28);
STATIC_ASSERT(offsetof(PC_SPECIAL_GEOMETRY, morph_deltas) == 0x2c);
STATIC_ASSERT(offsetof(PC_SPECIAL_GEOMETRY, dynamic_vertices) == 0x34);
typedef struct PC_SPECIAL_DEFORMER_WEIGHTS {
    u32 unknown_0000[2];
    f32 weights[1];
} PC_SPECIAL_DEFORMER_WEIGHTS;
STATIC_ASSERT(offsetof(PC_SPECIAL_DEFORMER_WEIGHTS, weights) == 8);
typedef struct PC_SPECIAL_FACEON {
    i32 kind;
} PC_SPECIAL_FACEON;
typedef struct PC_SPECIAL_TRANSFORM_PACKET {
    NUMTX matrix;
} PC_SPECIAL_TRANSFORM_PACKET;
STATIC_ASSERT(offsetof(PC_SPECIAL_TRANSFORM_PACKET, matrix.m32) == 0x38);
STATIC_ASSERT(offsetof(PC_SPECIAL_TRANSFORM_PACKET, matrix.m33) == 0x3c);

static __declspec(noinline) void PCSpecialTransposeMatrix(NUMTX *out, NUMTX *matrix) {
    f32 saved;
    out->m00 = matrix->m00;
    saved = matrix->m01;
    out->m01 = matrix->m10;
    out->m10 = saved;
    saved = matrix->m02;
    out->m02 = matrix->m20;
    out->m20 = saved;
    saved = matrix->m03;
    out->m03 = matrix->m30;
    out->m30 = saved;
    out->m11 = matrix->m11;
    saved = matrix->m12;
    out->m12 = matrix->m21;
    out->m21 = saved;
    saved = matrix->m13;
    out->m13 = matrix->m31;
    out->m31 = saved;
    out->m22 = matrix->m22;
    saved = matrix->m23;
    out->m23 = matrix->m32;
    out->m32 = saved;
    out->m33 = matrix->m33;
}
VARIPTR *__cdecl NuDisplayListLinkItems(NUDISPLAYLIST_PC *list, i32 count) {
    NUDISPLAYLISTITEM *item;
    NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
    ((NUDISPLAYLISTITEM *)list->material_last)->next = NuDisplayListStreamCursor->void_ptr;
    list->items = NuDisplayListStreamCursor->void_ptr;
    NuDisplayListStreamCursor->addr += count * sizeof(NUDISPLAYLISTITEM);
    item = (NUDISPLAYLISTITEM *)NuDisplayListStreamCursor->void_ptr;
    item->type = 0x8d;
    item->id = 1;
    item->next = (NUDISPLAYLISTITEM *)list->dynamic_geometry + 1;
    list->material_last = item;
    NuDisplayListStreamCursor->addr += sizeof(NUDISPLAYLISTITEM);
    return NuDisplayListStreamCursor;
}
void *__cdecl DisplayListCreateGeomTransformPS(VARIPTR *buffer, NUMTX *matrix, NUMTL *material, void *geometry,
                                               void *existing) {
    NUVEC scale, translation;
    NUMTX *result;
    (void)material;
    (void)geometry;
    (void)existing;
    NuCameraGetProjectionMtx();
    NuCameraGetViewMtx();
    scale.x = PCSpecialProjectionAdjust.x_scale;
    scale.y = PCSpecialProjectionAdjust.y_scale;
    scale.z = 1.0f;
    translation.x = PCSpecialProjectionAdjust.x_offset;
    translation.y = PCSpecialProjectionAdjust.y_offset;
    translation.z = 0.0f;
    NuMtxSetScale(&PCSpecialProjectionScale, &scale);
    NuMtxSetTranslation(&PCSpecialProjectionTranslate, &translation);
    buffer->addr = (buffer->addr + 3) & ~3u;
    result = (NUMTX *)buffer->void_ptr;
    PCSpecialTransposeMatrix(result, matrix);
    buffer->addr += sizeof(NUMTX);
    return result;
}
void *__cdecl DisplayListCreateFaceonTransformPS(VARIPTR *buffer, NUMTX *matrix, NUMTL *material, void *faceon) {
    NUMTX *result, *orientation;
    NUVEC direction;
    f32 magnitude;
    (void)material;
    buffer->addr = (buffer->addr + 3) & ~3u;
    result = (NUMTX *)buffer->void_ptr;
    *result = *matrix;
    magnitude = NuFsqrt((f32)(((f64)NuVecMagSqr((NUVEC *)&result->m10) + NuVecMagSqr((NUVEC *)&result->m00) +
                               NuVecMagSqr((NUVEC *)&result->m20)) *
                              (f64)0.3333333432674408f));
    buffer->addr += sizeof(NUMTX);
    *(f32 *)buffer->void_ptr = magnitude;
    buffer->addr += sizeof(f32);
    orientation = (NUMTX *)buffer->void_ptr;
    buffer->addr += sizeof(NUMTX);
    if (((PC_SPECIAL_FACEON *)faceon)->kind == 0) {
        direction.x = direction.y = direction.z = 0.0f;
        NuMtxCalcCheapFaceOn(orientation, &direction);
    } else {
        direction.x = matrix->m10;
        direction.y = matrix->m11;
        direction.z = matrix->m12;
        NuMtxCalcCheapFaceY_v2(orientation, &direction);
    }
    return result;
}
void *__cdecl DisplayListCreateSkinTransformPS(VARIPTR *buffer, NUMTX *skin, void *weights, void *geometry_ptr,
                                               void **render_geometry, void *unused) {
    PC_SPECIAL_GEOMETRY *geometry = (PC_SPECIAL_GEOMETRY *)geometry_ptr;
    PC_SPECIAL_DEFORMER_WEIGHTS *weight_view = (PC_SPECIAL_DEFORMER_WEIGHTS *)weights;
    i32 *matrix_count;
    u32 vertex, morph;
    u8 *destination;
    void *locked;
    PC_SPECIAL_GEOMETRY *copy;
    i32 i;
    (void)unused;
    if (weights != NULL && geometry->morph_count != 0) {
        buffer->addr = (buffer->addr + 31) & ~31u;
        copy = (PC_SPECIAL_GEOMETRY *)buffer->void_ptr;
        *render_geometry = copy;
        buffer->addr += sizeof(*copy);
        *copy = *geometry;
        locked = NULL;
        PCDeviceLockVertexBuffer(geometry->base_vertex * geometry->vertex_stride,
                                 geometry->vertex_count * geometry->vertex_stride, &locked, 0x10,
                                 geometry->vertex_buffer);
        buffer->addr = (buffer->addr + 31) & ~31u;
        destination = (u8 *)buffer->void_ptr;
        copy->dynamic_vertices = destination;
        buffer->addr += geometry->vertex_count * geometry->vertex_stride;
        PCByteCopy_candidate(destination, locked, geometry->vertex_count * geometry->vertex_stride);
        for (vertex = 0; vertex < geometry->vertex_count; ++vertex) {
            NUVEC *position = (NUVEC *)destination;
            for (morph = 0; morph < geometry->morph_count; ++morph) {
                NUVEC *delta = geometry->morph_deltas[morph];
                f32 weight = weight_view->weights[morph];
                if (delta != NULL && weight != 0.0f) {
                    position->x += delta[vertex].x * weight;
                    position->y += delta[vertex].y * weight;
                    position->z += delta[vertex].z * weight;
                }
            }
            destination += geometry->vertex_stride;
        }
        PCDeviceUnlockVertexBuffer(geometry->vertex_buffer);
    } else {
        geometry->dynamic_vertices = NULL;
        *render_geometry = geometry;
    }
    matrix_count = (i32 *)buffer->void_ptr;
    buffer->addr += sizeof(*matrix_count);
    *matrix_count = 0;
    for (i = 0; i < 8 && geometry->joint_indices[i] != 255; ++i) {
        *(NUMTX *)buffer->void_ptr = skin[geometry->joint_indices[i]];
        buffer->addr += sizeof(NUMTX);
        ++*matrix_count;
    }
    return matrix_count;
}
void *__cdecl NuDisplayListPrepareFaceonPS(VARIPTR *buffer, void *faceon, NUMTX *matrix) {
    (void)buffer;
    (void)matrix;
    return faceon;
}
void __cdecl DisplayListSetShadowCasterFlagPS(NUDISPLAYLISTITEM *first, NUDISPLAYLISTITEM *last, i32 shadow) {
    PC_SPECIAL_TRANSFORM_PACKET *packet = (PC_SPECIAL_TRANSFORM_PACKET *)first->next;
    (void)last;
    packet->matrix.m32 = (f32)shadow;
}
void __cdecl DisplayListUpdateRenderStateShadow(NUDISPLAYLIST_PC *list, NUGLOBALRNDRSTATE_PC *state) {
    NUGLOBALRNDRSTATE_PC *previous = (NUGLOBALRNDRSTATE_PC *)list->render_state;
    if (state != NULL && previous->counter_0020 != state->counter_0020) {
        if (previous->vertex_groups_id != state->vertex_groups_id) {
            void *packet = DisplayListBuildVertexGroupPacket();
            if (packet != NULL)
                NuDisplayListLinkItem(list, 0xa9, packet);
            previous = (NUGLOBALRNDRSTATE_PC *)list->render_state;
            previous->vertex_groups_id = state->vertex_groups_id;
        }
        previous->counter_0020 = state->counter_0020;
    }
}

void __cdecl DisplayListCreatePS(NUDISPLAY_LIST_PC *scene, VARIPTR *buffer, VARIPTR end) {
    (void)scene;
    (void)buffer;
    (void)end;
}

typedef struct display_primitive_packet_pc_s {
    u32 type;
    u8 unknown_0004[0x14];
    u32 vertex_count;
    u8 unknown_001c[0x1c];
} DISPLAY_PRIMITIVE_PACKET_PC;
STATIC_ASSERT(sizeof(DISPLAY_PRIMITIVE_PACKET_PC) == 0x38);
STATIC_ASSERT(offsetof(DISPLAY_PRIMITIVE_PACKET_PC, vertex_count) == 0x18);
void __cdecl NuDisplayListDrawPrimitive2DPc(void *data) {
    DISPLAY_PRIMITIVE_PACKET_PC *packet = (DISPLAY_PRIMITIVE_PACKET_PC *)data;
    u32 count;
    const void *vertices;
    u32 point_size;
    f32 point_size_float;
    if (packet == NULL || PCShaderCurrentObjectPc() == NULL || packet->vertex_count == 0)
        return;
    count = packet->vertex_count;
    vertices = packet + 1;
    switch (packet->type) {
        case 12:
        case 16:
            if (count > 2)
                PCDeviceDrawPrimitiveUPPc(4, count / 3, vertices, 24);
            break;
        case 15:
            if (count > 1)
                PCDeviceDrawPrimitiveUPPc(3, count - 1, vertices, 24);
            break;
        case 13:
            if (count > 2)
                PCDeviceDrawPrimitiveUPPc(5, count - 2, vertices, 24);
            break;
        case 14:
            if (count > 1)
                PCDeviceDrawPrimitiveUPPc(2, count / 2, vertices, 24);
            break;
        case 18:
            point_size_float = 32.0f;
            memcpy(&point_size, &point_size_float, sizeof(point_size));
            PCDeviceSetRenderState(154, point_size);
            PCDeviceDrawPrimitiveUPPc(1, packet->vertex_count, vertices, 24);
            point_size_float = 1.0f;
            memcpy(&point_size, &point_size_float, sizeof(point_size));
            PCDeviceSetRenderState(154, point_size);
            break;
    }
}

/* Native603960 is an actual empty cross-TU operation, also empty in source. */
void __cdecl DisplayGenerateTransformsNoOpPc(NUDISPLAY_LIST_PC *scene) {
    (void)scene;
}

typedef struct PC_DL_SKIN_PALETTE_READER {
    i32 count;
    f32 elements[1];
} PC_DL_SKIN_PALETTE_READER;
STATIC_ASSERT(offsetof(PC_DL_SKIN_PALETTE_READER, count) == 0);
STATIC_ASSERT(offsetof(PC_DL_SKIN_PALETTE_READER, elements) == 4);

typedef struct PC_DL_LIGHTS_READER {
    NUVEC4 ambient;
    NUVEC4 intensity[3];
    NUVEC4 direction[3];
    NUMTX shine_matrix;
} PC_DL_LIGHTS_READER;
STATIC_ASSERT(sizeof(PC_DL_LIGHTS_READER) == 0xb0);
STATIC_ASSERT(offsetof(PC_DL_LIGHTS_READER, intensity) == 0x10);
STATIC_ASSERT(offsetof(PC_DL_LIGHTS_READER, direction) == 0x40);
STATIC_ASSERT(offsetof(PC_DL_LIGHTS_READER, shine_matrix) == 0x70);

typedef union PC_DL_NORMALIZED_DIRECTION {
    NUVEC4 four;
    struct {
        NUVEC three;
        f32 w;
    } normalized;
} PC_DL_NORMALIZED_DIRECTION;
STATIC_ASSERT(sizeof(PC_DL_NORMALIZED_DIRECTION) == 16);
STATIC_ASSERT(offsetof(PC_DL_NORMALIZED_DIRECTION, normalized.w) == 12);

typedef struct PC_DL_FOG_READER {
    i32 enabled;
    u32 colour;
    f32 near_distance, far_distance, density;
} PC_DL_FOG_READER;
STATIC_ASSERT(sizeof(PC_DL_FOG_READER) == 0x14);
STATIC_ASSERT(offsetof(PC_DL_FOG_READER, colour) == 4);
STATIC_ASSERT(offsetof(PC_DL_FOG_READER, near_distance) == 8);
STATIC_ASSERT(offsetof(PC_DL_FOG_READER, far_distance) == 0xc);
STATIC_ASSERT(offsetof(PC_DL_FOG_READER, density) == 0x10);

typedef struct PC_DL_LEGACY_LIGHTMAP_READER {
    i32 texture_id[3];
    f32 x_offset, y_offset;
} PC_DL_LEGACY_LIGHTMAP_READER;
STATIC_ASSERT(offsetof(PC_DL_LEGACY_LIGHTMAP_READER, x_offset) == 0xc);
STATIC_ASSERT(offsetof(PC_DL_LEGACY_LIGHTMAP_READER, y_offset) == 0x10);

typedef struct PC_DL_CAMERA_READER {
    i32 id;
    NUMTX view, projection;
    f32 viewport[4];
} PC_DL_CAMERA_READER;
STATIC_ASSERT(sizeof(PC_DL_CAMERA_READER) == 0x94);
STATIC_ASSERT(offsetof(PC_DL_CAMERA_READER, view) == 4);
STATIC_ASSERT(offsetof(PC_DL_CAMERA_READER, projection) == 0x44);
STATIC_ASSERT(offsetof(PC_DL_CAMERA_READER, viewport) == 0x84);

/* Source-local working declarations; backing and canonical union remain separate gates. */
typedef struct PC_DL_MATERIAL_READER {
    u8 unknown_0000[0x40];
    u32 reflection_flags;
} PC_DL_MATERIAL_READER;
STATIC_ASSERT(offsetof(PC_DL_MATERIAL_READER, reflection_flags) == 0x40);
extern NUVEC4 NuDisplayHandlerTintPc;
extern void *NuDisplayHandlerMaterialPc;
extern i32 NuDisplayHandlerReflectionPc;
extern i32 NuDisplayHandlerCameraIdPc;
extern PC_GRAPHICS_FRAME_SCRATCH_VIEW_PC PCGraphicsFrameScratchCount;
void __cdecl NuVec4Add(NUVEC4 *, NUVEC4 *, NUVEC4 *);

void __cdecl NuDisplayListSkinPalettePc(void *arg) {
    PC_DL_SKIN_PALETTE_READER *packet = (PC_DL_SKIN_PALETTE_READER *)arg;
    i32 count = packet->count;
    PC_SHADER_OBJECT_FULL_VIEW *shader = (PC_SHADER_OBJECT_FULL_VIEW *)PCShaderCurrentObjectPc();
    if (shader != NULL) {
        if (count > 8)
            NuPtlDmaSlotsExhaustedPc();
        NuShaderObjectSetElementsfv(shader, 0x18, 0, count, packet->elements);
    }
}

void __cdecl NuDisplayListLightsPc(void *arg) {
    PC_DL_LIGHTS_READER *packet = (PC_DL_LIGHTS_READER *)arg;
    if (PCDeviceHardwareVertexProcessingPC != 4) {
        NuShaderManagerSetfv(1, &packet->ambient.x);
        NuShaderManagerSetfv(6, &packet->direction[0].x);
        NuShaderManagerSetfv(7, &packet->direction[1].x);
        NuShaderManagerSetfv(8, &packet->direction[2].x);
        NuShaderManagerSetfv(3, &packet->intensity[0].x);
        NuShaderManagerSetfv(4, &packet->intensity[1].x);
        NuShaderManagerSetfv(5, &packet->intensity[2].x);
        NuShaderManagerSetfv(0x49, &packet->shine_matrix.m00);
    } else {
        NuShaderManagerSetfv(0x23, &packet->ambient.x);
    }
    if (PCDeviceHardwareVertexProcessingPC != 4) {
        PC_DL_NORMALIZED_DIRECTION direction;
        NUVEC4 colour;
        NuVec4Add(&direction.four, &packet->direction[0], &packet->direction[1]);
        NuVec4Add(&direction.four, &direction.four, &packet->direction[2]);
        NuVecNorm(&direction.normalized.three, &direction.normalized.three);
        NuShaderManagerSetfv(0x4c, &direction.four.x);
        colour.x =
            (packet->intensity[2].x < packet->intensity[1].x ? packet->intensity[1].x : packet->intensity[2].x) <
                    packet->intensity[0].x
                ? packet->intensity[0].x
                : (packet->intensity[2].x < packet->intensity[1].x ? packet->intensity[1].x : packet->intensity[2].x);
        colour.y =
            (packet->intensity[2].y < packet->intensity[1].y ? packet->intensity[1].y : packet->intensity[2].y) <
                    packet->intensity[0].y
                ? packet->intensity[0].y
                : (packet->intensity[2].y < packet->intensity[1].y ? packet->intensity[1].y : packet->intensity[2].y);
        colour.z =
            (packet->intensity[2].z < packet->intensity[1].z ? packet->intensity[1].z : packet->intensity[2].z) <
                    packet->intensity[0].z
                ? packet->intensity[0].z
                : (packet->intensity[2].z < packet->intensity[1].z ? packet->intensity[1].z : packet->intensity[2].z);
        colour.w = 1.0f;
        NuShaderManagerSetfv(0x4b, &colour.x);
    }
}

void __cdecl NuDisplayListTintPc(void *arg) {
    NUVEC4 tint = {1.0f, 1.0f, 1.0f, *(f32 *)arg};
    NuDisplayHandlerTintPc = tint;
    NuShaderManagerSetfv(0x38, &tint.x);
}

void __cdecl NuDisplayListFogPc(void *arg) {
    PC_DL_FOG_READER *packet = (PC_DL_FOG_READER *)arg;
    if (packet->enabled != 0) {
        u32 packed = packet->colour;
        NUVEC4 colour;
        NUVEC4 params;
        colour.x = (f32)((i32)(packed & 255) / 255.0);
        colour.y = (f32)((i32)((packed >> 8) & 255) / 255.0);
        colour.z = (f32)((i32)((packed >> 16) & 255) / 255.0);
        colour.w = (f32)((i32)((packed >> 24) & 255) / 255.0);
        params.x = packet->near_distance;
        params.y = packet->far_distance;
        params.z = packet->far_distance - packet->near_distance;
        params.w = packet->density;
        NuShaderManagerSetfv(0x44, &colour.x);
        NuShaderManagerSetfv(0x45, &params.x);
    } else {
        NUVEC4 params = {0.0f, 0.0f, 0.0f, 0.0f};
        NuShaderManagerSetfv(0x45, &params.x);
    }
}

void __cdecl NuDisplayListLightmapPacketPc(void *arg) {
    PCGraphicsFrameScratchCount.lightmap_packet = arg;
}

void __cdecl NuDisplayListLegacyLightmapPc(void *arg) {
    i32 *texture_ids = (i32 *)arg;
    f32 offset[3] = {0.0f, 0.0f, 0.0f};
    i32 i;
    for (i = 0; i < 3; ++i) {
        NUTEXDIM_PC *texture = NuTextureGetDimensions(texture_ids[i] == -1 ? 1 : texture_ids[i]);
        PCDeviceSetTexture(i, texture->resource);
        PCDeviceSetSamplerState(i, 1, 1);
        PCDeviceSetSamplerState(i, 2, 1);
    }
    NuShaderManagerSetfv(0x60, offset);
}

void __cdecl NuDisplayListLegacyLightmapOffsetPc(void *arg) {
    PC_DL_LEGACY_LIGHTMAP_READER *packet = (PC_DL_LEGACY_LIGHTMAP_READER *)arg;
    f32 offset[3];
    i32 i;
    for (i = 0; i < 3; ++i) {
        NUTEXDIM_PC *texture = NuTextureGetDimensions(packet->texture_id[i] == -1 ? 1 : packet->texture_id[i]);
        PCDeviceSetTexture(i, texture->resource);
        PCDeviceSetSamplerState(i, 1, 1);
        PCDeviceSetSamplerState(i, 2, 1);
    }
    offset[0] = packet->x_offset;
    offset[1] = -packet->y_offset;
    offset[2] = 0.0f;
    NuShaderManagerSetfv(0x60, offset);
}

void __cdecl NuDisplayListReflectionPc(void *arg) {
    i32 reflection = *(i32 *)arg;
    PC_DL_MATERIAL_READER *material;
    NuDisplayHandlerReflectionPc = reflection;
    material = (PC_DL_MATERIAL_READER *)NuDisplayHandlerMaterialPc;
    if (material != NULL) {
        u32 mode = (material->reflection_flags >> 12) & 3;
        if (mode == 0)
            PCDeviceSetRenderState(0x16, reflection != 0 ? 2 : 3);
        else if (mode == 1)
            PCDeviceSetRenderState(0x16, reflection != 0 ? 3 : 2);
    }
}

void __cdecl NuDisplayListCameraPc(void *arg) {
    PC_DL_CAMERA_READER *packet = (PC_DL_CAMERA_READER *)arg;
    if (packet->id != NuDisplayHandlerCameraIdPc) {
        PC_VIEWPORT_PC viewport;
        i32 viewport_x;
        NuDisplayHandlerCameraIdPc = packet->id;
        PcUpdateGeometryCameraOriginPc(&packet->view, &packet->projection);
        viewport_x = (i32)packet->viewport[0];
        viewport.x = (u32)viewport_x;
        viewport.y = (u32)(i64)packet->viewport[1];
        viewport.width = (u32)(i64)packet->viewport[2];
        viewport.height = (u32)(i64)packet->viewport[3];
        viewport.min_z = 0.0f;
        viewport.max_z = 1.0f;
        NuVpSubmitPc(&viewport);
    }
}
