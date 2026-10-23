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
    VARIPTR *cursor;
    NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
    cursor = NuDisplayListStreamCursor;
    ((NUDISPLAYLISTITEM *)list->material_last)->next = cursor->void_ptr;
    list->items = cursor->void_ptr;
    cursor->addr += count * sizeof(NUDISPLAYLISTITEM);
    ((NUDISPLAYLISTITEM *)cursor->void_ptr)->type = 0x8d;
    ((NUDISPLAYLISTITEM *)cursor->void_ptr)->id = 1;
    ((NUDISPLAYLISTITEM *)cursor->void_ptr)->next = (NUDISPLAYLISTITEM *)list->dynamic_geometry + 1;
    list->material_last = cursor->void_ptr;
    cursor->addr += sizeof(NUDISPLAYLISTITEM);
    return cursor;
}
void *__cdecl DisplayListCreateGeomTransformPS(VARIPTR *buffer, NUMTX *matrix, NUMTL *material, void *geometry,
                                               void *existing) {
    NUVEC translation, scale;
    NUMTX *result;
    (void)material;
    (void)geometry;
    (void)existing;
    NuCameraGetProjectionMtx();
    NuCameraGetViewMtx();
    scale.z = 1.0f;
    translation.z = translation.y = translation.x = 0.0f;
    scale.x = PCSpecialProjectionAdjust.x_scale;
    scale.y = PCSpecialProjectionAdjust.y_scale;
    NuMtxSetScale(&PCSpecialProjectionScale, &scale);
    translation.x = PCSpecialProjectionAdjust.x_offset;
    translation.y = PCSpecialProjectionAdjust.y_offset;
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
    magnitude = NuFsqrt((f32)(((f64)NuVecMagSqr((NUVEC *)&((NUMTX *)buffer->void_ptr)->m10) +
                               NuVecMagSqr((NUVEC *)&((NUMTX *)buffer->void_ptr)->m00) +
                               NuVecMagSqr((NUVEC *)&((NUMTX *)buffer->void_ptr)->m20)) *
                              (f64)0.3333333432674408f));
    buffer->addr += sizeof(NUMTX);
    *(f32 *)buffer->void_ptr = magnitude;
    buffer->addr += sizeof(f32);
    orientation = (NUMTX *)buffer->void_ptr;
    if (((PC_SPECIAL_FACEON *)faceon)->kind == 0) {
        direction.x = direction.y = direction.z = 0.0f;
        buffer->addr += sizeof(NUMTX);
        NuMtxCalcCheapFaceOn(orientation, &direction);
    } else {
        direction.x = matrix->m10;
        direction.y = matrix->m11;
        direction.z = matrix->m12;
        buffer->addr += sizeof(NUMTX);
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
        *render_geometry = (void *)((buffer->addr + 31) & ~31u);
        buffer->addr = ((buffer->addr + 31) & ~31u) + sizeof(*copy);
        copy = (PC_SPECIAL_GEOMETRY *)*render_geometry;
        *copy = *geometry;
        locked = NULL;
        PCDeviceLockVertexBuffer(geometry->base_vertex * geometry->vertex_stride,
                                 geometry->vertex_count * geometry->vertex_stride, &locked, 0x10,
                                 geometry->vertex_buffer);
        copy->dynamic_vertices = (u8 *)((buffer->addr + 31) & ~31u);
        buffer->addr = (buffer->addr + 31) & ~31u;
        buffer->addr += geometry->vertex_count * geometry->vertex_stride;
        destination = (u8 *)copy->dynamic_vertices;
        PCByteCopy_candidate(destination, locked, geometry->vertex_count * geometry->vertex_stride);
        for (vertex = 0; vertex != geometry->vertex_count; ++vertex) {
            for (morph = 0; morph != geometry->morph_count; ++morph) {
                f32 weight = weight_view->weights[morph];
                if (geometry->morph_deltas[morph] != NULL && weight != 0.0f) {
                    ((NUVEC *)destination)->x += geometry->morph_deltas[morph][vertex].x * weight;
                    ((NUVEC *)destination)->y += geometry->morph_deltas[morph][vertex].y * weight;
                    ((NUVEC *)destination)->z += geometry->morph_deltas[morph][vertex].z * weight;
                }
            }
            locked = (u8 *)locked + geometry->vertex_stride;
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
    for (i = 0; geometry->joint_indices[i] != 255 && i < 8; ++i) {
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
        SAGA_SKIN_CALL(skin_trace_palette, (unsigned)arg, count);
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

typedef struct PC_DL_VERTEX_GROUPS_READER {
    i32 count;
    u32 flags;
    NUVEC4 values[1];
} PC_DL_VERTEX_GROUPS_READER;
STATIC_ASSERT(offsetof(PC_DL_VERTEX_GROUPS_READER, count) == 0);
STATIC_ASSERT(offsetof(PC_DL_VERTEX_GROUPS_READER, flags) == 4);
STATIC_ASSERT(offsetof(PC_DL_VERTEX_GROUPS_READER, values) == 8);

typedef struct PC_DL_VERTEX_OFFSETS_READER {
    i32 count;
    NUVEC4 values[1];
} PC_DL_VERTEX_OFFSETS_READER;
STATIC_ASSERT(offsetof(PC_DL_VERTEX_OFFSETS_READER, count) == 0);
STATIC_ASSERT(offsetof(PC_DL_VERTEX_OFFSETS_READER, values) == 4);

void __cdecl NuDisplayListVertexGroupsPc(void *arg) {
    PC_DL_VERTEX_GROUPS_READER *packet = (PC_DL_VERTEX_GROUPS_READER *)arg;
    i32 groups = (i32)((u32)packet->count + 3u) / 4;
    i32 i;
    for (i = 0; i < groups && i < 32; ++i)
        NuShaderManagerSetElementfv(0x4f, i, &packet->values[i].x);
}

void __cdecl NuDisplayListVertexOffsetsPc(void *arg) {
    PC_DL_VERTEX_OFFSETS_READER *packet = (PC_DL_VERTEX_OFFSETS_READER *)arg;
    i32 count = packet->count;
    i32 i;
    for (i = 0; i < count && i < 8; ++i)
        NuShaderManagerSetElementfv(0x4e, i, &packet->values[i].x);
}

/* PC-only table slot 0xad; no Android function identity is asserted. */
typedef struct PC_DL_AD_COLOURS {
    NUVEC colour[5];
} PC_DL_AD_COLOURS;
STATIC_ASSERT(sizeof(PC_DL_AD_COLOURS) == 0x3c);

typedef struct PC_DL_AD_RECORD {
    u8 unknown_0000[0x14];
    u8 enabled;
    u8 unknown_0015[3];
    i32 segment_count;
    f32 pixel_parameter, vertex_parameter_x, vertex_parameter_y, vertex_parameter_z;
    f32 pixel_parameter_x, pixel_parameter_y, pixel_parameter_z, pixel_scalar;
    PC_DL_AD_COLOURS colours;
} PC_DL_AD_RECORD;
STATIC_ASSERT(sizeof(PC_DL_AD_RECORD) == 0x78);
STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, enabled) == 0x14);
STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, segment_count) == 0x18);
STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, pixel_parameter) == 0x1c);
STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, vertex_parameter_x) == 0x20);
STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, vertex_parameter_y) == 0x24);
STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, vertex_parameter_z) == 0x28);
STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, pixel_parameter_x) == 0x2c);
STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, pixel_parameter_y) == 0x30);
STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, pixel_parameter_z) == 0x34);
STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, pixel_scalar) == 0x38);
STATIC_ASSERT(offsetof(PC_DL_AD_RECORD, colours) == 0x3c);

typedef struct PC_DL_AD_RECORD_LIST {
    u32 unknown_0000;
    i32 count;
    u32 unknown_0008;
    PC_DL_AD_RECORD *records;
} PC_DL_AD_RECORD_LIST;
STATIC_ASSERT(offsetof(PC_DL_AD_RECORD_LIST, count) == 4);
STATIC_ASSERT(offsetof(PC_DL_AD_RECORD_LIST, records) == 0xc);

typedef struct PC_DL_AD_PACKET {
    PC_DL_AD_RECORD_LIST *list;
    u32 instances;
    u8 unknown_0008[0xc];
    PC_BUFFER_RESOURCE_PC *instance_buffer, *vertex_buffer, *index_buffer;
} PC_DL_AD_PACKET;
STATIC_ASSERT(sizeof(PC_DL_AD_PACKET) == 0x20);
STATIC_ASSERT(offsetof(PC_DL_AD_PACKET, list) == 0);
STATIC_ASSERT(offsetof(PC_DL_AD_PACKET, instances) == 4);
STATIC_ASSERT(offsetof(PC_DL_AD_PACKET, instance_buffer) == 0x14);
STATIC_ASSERT(offsetof(PC_DL_AD_PACKET, vertex_buffer) == 0x18);
STATIC_ASSERT(offsetof(PC_DL_AD_PACKET, index_buffer) == 0x1c);

void __cdecl PCDeviceSetStreamSourceFreqPc(u32, u32);
void __cdecl PCDeviceSetStreamSourcePc(u32, PC_BUFFER_RESOURCE_PC *, u32, u32);
void __cdecl PCDeviceSetIndicesPc(PC_BUFFER_RESOURCE_PC *);
void __cdecl PCDeviceDrawIndexedPrimitivePc(i32, i32, u32, u32, u32, u32);

void __cdecl NuDisplayListPacketAdPc(void *arg) {
    PC_DL_AD_PACKET *packet = (PC_DL_AD_PACKET *)arg;
    PC_DL_AD_RECORD *record;
    PC_DL_AD_COLOURS colours;
    NUVEC4 values;
    i32 segments, i;
    u32 instances = packet->instances;
    if (packet->vertex_buffer == NULL || packet->instance_buffer == NULL || packet->list == NULL)
        return;
    record = &packet->list->records[packet->list->count - 1];
    segments = record->segment_count;
    values.x = (f32)segments;
    values.y = record->enabled != 0 ? 1.0f : 0.0f;
    values.z = record->vertex_parameter_x;
    values.w = record->vertex_parameter_y;
    PCDeviceSetVertexShaderConstantsF(0xc8, &values.x, 1);
    values.x = record->vertex_parameter_z;
    values.y = 0.0f;
    values.z = 0.0f;
    values.w = 0.0f;
    PCDeviceSetVertexShaderConstantsF(0xc9, &values.x, 1);
    colours = record->colours;
    for (i = 0; i < 5; ++i) {
        values.x = colours.colour[i].x;
        values.y = colours.colour[i].y;
        values.z = colours.colour[i].z;
        values.w = 1.0f;
        PCDeviceSetPixelShaderConstantsF(0xca + i, &values.x, 1);
    }
    values.x = record->pixel_parameter;
    values.y = record->pixel_parameter_x;
    values.z = record->pixel_parameter_y;
    values.w = record->pixel_parameter_z;
    PCDeviceSetPixelShaderConstantsF(0xd3, &values.x, 1);
    values.x = record->pixel_scalar;
    values.y = 0.0f;
    values.z = 0.0f;
    values.w = 0.0f;
    PCDeviceSetPixelShaderConstantsF(0xd4, &values.x, 1);
    PCDeviceSetStreamSourcePc(0, packet->vertex_buffer, 0, 0x10);
    PCDeviceSetStreamSourceFreqPc(0, instances | 0x40000000u);
    PCDeviceSetStreamSourcePc(1, packet->instance_buffer, 0, 0x50);
    PCDeviceSetStreamSourceFreqPc(1, 0x80000001u);
    PCDeviceSetIndicesPc(packet->index_buffer);
    PCDeviceDrawIndexedPrimitivePc(5, 0, 0, (u32)segments * 2u + 2u, 0, (u32)segments * 2u);
    PCDeviceSetStreamSourcePc(0, NULL, 0, 0);
    PCDeviceSetStreamSourceFreqPc(0, 1);
    PCDeviceSetStreamSourceFreqPc(1, 1);
}

typedef struct PC_DL_GEOMETRY_READER {
    u32 kind, primitive_count;
    u16 vertex_stride;
    u8 unknown_000a[0xa];
    i32 base_vertex;
    u32 vertex_count, start_index;
    PC_BUFFER_RESOURCE_PC *index_buffer, *vertex_buffer;
    u32 dynamic_count;
    void *morph_deltas;
    PC_BUFFER_RESOURCE_PC *dynamic_buffer;
    void *dynamic_vertices;
} PC_DL_GEOMETRY_READER;
STATIC_ASSERT(sizeof(PC_DL_GEOMETRY_READER) == 0x38);
STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, kind) == 0);
STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, primitive_count) == 4);
STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, vertex_stride) == 8);
STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, base_vertex) == 0x14);
STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, vertex_count) == 0x18);
STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, start_index) == 0x1c);
STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, index_buffer) == 0x20);
STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, vertex_buffer) == 0x24);
STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, dynamic_count) == 0x28);
STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, dynamic_buffer) == 0x30);
STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_READER, dynamic_vertices) == 0x34);

typedef struct PC_DL_GEOMETRY_MATERIAL_READER {
    u8 unknown_0000[0x26c];
    u32 lightmap_flags;
} PC_DL_GEOMETRY_MATERIAL_READER;
STATIC_ASSERT(offsetof(PC_DL_GEOMETRY_MATERIAL_READER, lightmap_flags) == 0x26c);

typedef struct PC_DL_FACEON_DRAW_READER {
    u32 unknown_0000, primitive_count;
    PC_BUFFER_RESOURCE_PC *vertex_buffer;
    u32 first_vertex;
} PC_DL_FACEON_DRAW_READER;
STATIC_ASSERT(sizeof(PC_DL_FACEON_DRAW_READER) == 0x10);
STATIC_ASSERT(offsetof(PC_DL_FACEON_DRAW_READER, primitive_count) == 4);
STATIC_ASSERT(offsetof(PC_DL_FACEON_DRAW_READER, vertex_buffer) == 8);
STATIC_ASSERT(offsetof(PC_DL_FACEON_DRAW_READER, first_vertex) == 0xc);

typedef struct PC_DL_DEBRIS_DRAW_READER {
    u8 buffer_index;
    u8 unknown_0001[3];
    u32 use_system_buffer, first_vertex, vertex_count;
} PC_DL_DEBRIS_DRAW_READER;
STATIC_ASSERT(sizeof(PC_DL_DEBRIS_DRAW_READER) == 0x10);
STATIC_ASSERT(offsetof(PC_DL_DEBRIS_DRAW_READER, buffer_index) == 0);
STATIC_ASSERT(offsetof(PC_DL_DEBRIS_DRAW_READER, use_system_buffer) == 4);
STATIC_ASSERT(offsetof(PC_DL_DEBRIS_DRAW_READER, first_vertex) == 8);
STATIC_ASSERT(offsetof(PC_DL_DEBRIS_DRAW_READER, vertex_count) == 0xc);

typedef struct PC_DL_LIGHTMAP_UPDATE_READER {
    i32 mode, texture_id;
    i32 texture_ids[3];
    f32 x_offset, y_offset;
} PC_DL_LIGHTMAP_UPDATE_READER;
STATIC_ASSERT(sizeof(PC_DL_LIGHTMAP_UPDATE_READER) == 0x1c);
STATIC_ASSERT(offsetof(PC_DL_LIGHTMAP_UPDATE_READER, mode) == 0);
STATIC_ASSERT(offsetof(PC_DL_LIGHTMAP_UPDATE_READER, texture_id) == 4);
STATIC_ASSERT(offsetof(PC_DL_LIGHTMAP_UPDATE_READER, texture_ids) == 8);
STATIC_ASSERT(offsetof(PC_DL_LIGHTMAP_UPDATE_READER, x_offset) == 0x14);
STATIC_ASSERT(offsetof(PC_DL_LIGHTMAP_UPDATE_READER, y_offset) == 0x18);

/* Existing canonical integer slot holds the native 32-bit packet identity. */
extern i32 PCGraphicsBoundShader;
void __cdecl PCDeviceSetStreamSourcePc(u32, PC_BUFFER_RESOURCE_PC *, u32, u32);
void __cdecl PCDeviceDrawPrimitivePc(i32, u32, u32);
void __cdecl PCDeviceSetIndicesPc(PC_BUFFER_RESOURCE_PC *);
void __cdecl PCDeviceDrawIndexedPrimitivePc(i32, i32, u32, u32, u32, u32);

void __cdecl NuDisplayListUpdateLightmapPc(void) {
    PC_DL_LIGHTMAP_UPDATE_READER *packet = (PC_DL_LIGHTMAP_UPDATE_READER *)PCGraphicsFrameScratchCount.lightmap_packet;
    i32 mode, stage;
    f32 offset[3];
    if (packet == NULL || (u32)packet == (u32)PCGraphicsBoundShader)
        return;
    PCGraphicsBoundShader = (i32)(u32)packet;
    mode = packet->mode;
    if (mode == 2 && PcWindowOptions.usehires == 0) {
        packet->mode = 1;
        mode = 1;
    }
    if (mode == 1) {
        NUTEXDIM_PC *texture;
        stage = PCDeviceHardwareVertexProcessingPC == 1 ? 1 : 0;
        texture = NuTextureGetDimensions(packet->texture_id == -1 ? 1 : packet->texture_id);
        PCDeviceSetTexture(stage, texture->resource);
        PCDeviceSetSamplerState(stage, 1, 1);
        PCDeviceSetSamplerState(stage, 2, 1);
        PCDeviceSetSamplerState(stage, 5, 2);
        PCDeviceSetSamplerState(stage, 6, 2);
        PCDeviceSetSamplerState(stage, 7, 0);
        offset[0] = packet->x_offset;
        offset[1] = -packet->y_offset;
        offset[2] = 0.0f;
        NuShaderManagerSetfv(0x60, offset);
    } else if (mode == 2) {
        for (stage = 0; stage < 3; ++stage) {
            NUTEXDIM_PC *texture =
                NuTextureGetDimensions(packet->texture_ids[stage] == -1 ? 1 : packet->texture_ids[stage]);
            PCDeviceSetTexture(stage, texture->resource);
            PCDeviceSetSamplerState(stage, 1, 1);
            PCDeviceSetSamplerState(stage, 2, 1);
            PCDeviceSetSamplerState(stage, 5, 2);
            PCDeviceSetSamplerState(stage, 6, 2);
            PCDeviceSetSamplerState(stage, 7, 0);
        }
        offset[0] = packet->x_offset;
        offset[1] = -packet->y_offset;
        offset[2] = 0.0f;
        NuShaderManagerSetfv(0x60, offset);
    }
}

void __cdecl NuDisplayListDrawGeometryPc(void *arg) {
    PC_DL_GEOMETRY_READER *packet = (PC_DL_GEOMETRY_READER *)arg;
    PC_DL_GEOMETRY_MATERIAL_READER *material = (PC_DL_GEOMETRY_MATERIAL_READER *)NuDisplayHandlerMaterialPc;
    u32 primitive_count;
    const void *vertices;
    void *locked;
    u32 point_size;
    f32 point_size_float;
    if (packet == NULL || PCShaderCurrentObjectPc() == NULL || packet->vertex_count == 0)
        return;
    primitive_count = packet->primitive_count;
    if (material->lightmap_flags & 0x4000000)
        NuDisplayListUpdateLightmapPc();
    vertices = packet + 1;
    switch (packet->kind) {
        case 6:
            if (packet->dynamic_count != 0 && packet->dynamic_vertices != NULL) {
                PCDeviceLockVertexBuffer(0, 0, &locked, 0x2000, packet->dynamic_buffer);
                PCByteCopy_candidate(locked, packet->dynamic_vertices, packet->vertex_stride * packet->vertex_count);
                PCDeviceUnlockVertexBuffer(packet->dynamic_buffer);
                PCDeviceSetStreamSourcePc(0, packet->dynamic_buffer, 0, packet->vertex_stride);
                PCDeviceSetIndicesPc(packet->index_buffer);
                SAGA_SKIN_CALL(skin_trace_capture_geometry, (unsigned)packet, (unsigned)NuDisplayHandlerMaterialPc,
                               PCShaderSlots, (unsigned)packet->dynamic_buffer, (unsigned)packet->index_buffer, 0,
                               packet->start_index);
                PCDeviceDrawIndexedPrimitivePc(5, 0, 0, packet->vertex_count, packet->start_index, primitive_count);
            } else {
                PCDeviceSetStreamSourcePc(0, packet->vertex_buffer, 0, packet->vertex_stride);
                PCDeviceSetIndicesPc(packet->index_buffer);
                SAGA_SKIN_CALL(skin_trace_capture_geometry, (unsigned)packet, (unsigned)NuDisplayHandlerMaterialPc,
                               PCShaderSlots, (unsigned)packet->vertex_buffer, (unsigned)packet->index_buffer,
                               packet->base_vertex, packet->start_index);
                PCDeviceDrawIndexedPrimitivePc(5, packet->base_vertex, 0, packet->vertex_count, packet->start_index,
                                               primitive_count);
            }
            break;
        case 12:
            PCDeviceDrawPrimitiveUPPc(4, packet->vertex_count / 3u, vertices, 24);
            break;
        case 13:
            if (packet->vertex_count >= 3u)
                PCDeviceDrawPrimitiveUPPc(5, packet->vertex_count - 2u, vertices, 24);
            break;
        case 14:
            PCDeviceDrawPrimitiveUPPc(2, packet->vertex_count >> 1, vertices, 24);
            break;
        case 15:
            PCDeviceDrawPrimitiveUPPc(3, packet->vertex_count - 1u, vertices, 24);
            break;
        case 17:
            if (packet->vertex_count >= 3u)
                PCDeviceDrawPrimitiveUPPc(6, packet->vertex_count - 2u, vertices, 24);
            break;
        case 18:
            point_size_float = 32.0f;
            memcpy(&point_size, &point_size_float, sizeof(point_size));
            PCDeviceSetRenderState(0x9a, point_size);
            PCDeviceDrawPrimitiveUPPc(1, packet->vertex_count, vertices, 24);
            point_size_float = 1.0f;
            memcpy(&point_size, &point_size_float, sizeof(point_size));
            PCDeviceSetRenderState(0x9a, point_size);
            break;
    }
}

void __cdecl NuDisplayListDrawFaceOnPc(void *arg) {
    PC_DL_FACEON_DRAW_READER *packet = (PC_DL_FACEON_DRAW_READER *)arg;
    if (packet == NULL)
        return;
    PCDeviceSetStreamSourcePc(0, packet->vertex_buffer, 0, 32);
    PCDeviceDrawPrimitivePc(4, packet->first_vertex, packet->primitive_count * 2u);
}

void __cdecl NuDisplayListDrawDebrisPc(void *arg) {
    PC_DL_DEBRIS_DRAW_READER *packet = (PC_DL_DEBRIS_DRAW_READER *)arg;
    if (packet->vertex_count == 0)
        return;
    if (packet->use_system_buffer != 0) {
        const void *vertices = (const u8 *)NuDebrisSystemBuffers[packet->buffer_index] + packet->first_vertex * 24u;
        PCDeviceDrawPrimitiveUPPc(4, packet->vertex_count / 3u, vertices, 24);
    } else {
        PCDeviceSetStreamSourcePc(0, NuDebrisVertexBuffers[packet->buffer_index], 0, 24);
        PCDeviceDrawPrimitivePc(4, packet->first_vertex, packet->vertex_count / 3u);
    }
}
