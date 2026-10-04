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
