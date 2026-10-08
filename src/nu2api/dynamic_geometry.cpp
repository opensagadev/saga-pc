#include "saga.h"

typedef struct PC_GEOMETRY_FORMAT_RECORD_PC {
    u8 unknown_00[0x18];
    i32 strip_segments;
    u8 unknown_1c[0x5c];
} PC_GEOMETRY_FORMAT_RECORD_PC;
typedef struct PC_GEOMETRY_FORMAT_PC {
    u32 unknown_00;
    u32 record_count;
    u32 unknown_08;
    PC_GEOMETRY_FORMAT_RECORD_PC *records;
} PC_GEOMETRY_FORMAT_PC;
typedef struct PC_DYNAMIC_GEOMETRY_PC {
    PC_GEOMETRY_FORMAT_PC *format;
    u8 unknown_04[0x10];
    PC_BUFFER_RESOURCE_PC *vertex_buffer;
    PC_BUFFER_RESOURCE_PC *strip_vertex_buffer;
    PC_BUFFER_RESOURCE_PC *strip_index_buffer;
} PC_DYNAMIC_GEOMETRY_PC;
typedef struct PC_GEOMETRY_STRIP_VERTEX_PC {
    f32 fraction, side;
    u8 unknown_08[8];
} PC_GEOMETRY_STRIP_VERTEX_PC;
typedef struct PC_GEOMETRY_CORNER_PC {
    NUVEC position;
    f32 w;
} PC_GEOMETRY_CORNER_PC;
typedef struct PC_GEOMETRY_OUTPUT_PC {
    PC_GEOMETRY_CORNER_PC corners[4];
    NUVEC basis_axis;
    f32 distance_squared;
} PC_GEOMETRY_OUTPUT_PC;
typedef struct PC_GEOMETRY_SORT_ITEM_PC {
    u32 output_index;
    f32 distance_squared;
    u32 unknown_08;
} PC_GEOMETRY_SORT_ITEM_PC;
STATIC_ASSERT(sizeof(PC_GEOMETRY_FORMAT_RECORD_PC) == 0x78);
STATIC_ASSERT(offsetof(PC_GEOMETRY_FORMAT_RECORD_PC, strip_segments) == 0x18);
STATIC_ASSERT(offsetof(PC_GEOMETRY_FORMAT_PC, record_count) == 4);
STATIC_ASSERT(offsetof(PC_GEOMETRY_FORMAT_PC, records) == 0xc);
STATIC_ASSERT(offsetof(PC_DYNAMIC_GEOMETRY_PC, format) == 0);
STATIC_ASSERT(offsetof(PC_DYNAMIC_GEOMETRY_PC, vertex_buffer) == 0x14);
STATIC_ASSERT(offsetof(PC_DYNAMIC_GEOMETRY_PC, strip_vertex_buffer) == 0x18);
STATIC_ASSERT(offsetof(PC_DYNAMIC_GEOMETRY_PC, strip_index_buffer) == 0x1c);
STATIC_ASSERT(sizeof(PC_GEOMETRY_STRIP_VERTEX_PC) == 0x10);
STATIC_ASSERT(offsetof(PC_GEOMETRY_STRIP_VERTEX_PC, fraction) == 0);
STATIC_ASSERT(offsetof(PC_GEOMETRY_STRIP_VERTEX_PC, side) == 4);
STATIC_ASSERT(sizeof(PC_GEOMETRY_OUTPUT_PC) == 0x50);
STATIC_ASSERT(offsetof(PC_GEOMETRY_OUTPUT_PC, corners) + 2 * sizeof(PC_GEOMETRY_CORNER_PC) == 0x20);
STATIC_ASSERT(offsetof(PC_GEOMETRY_OUTPUT_PC, distance_squared) == 0x4c);
STATIC_ASSERT(sizeof(PC_GEOMETRY_SORT_ITEM_PC) == 0xc);
STATIC_ASSERT(offsetof(PC_GEOMETRY_SORT_ITEM_PC, output_index) == 0);
STATIC_ASSERT(offsetof(PC_GEOMETRY_SORT_ITEM_PC, distance_squared) == 4);
extern "C" NUVEC4 PcGeometryDistanceOriginPc;

typedef struct PC_GEOMETRY_INPUT_PC {
    PC_GEOMETRY_CORNER_PC corners[4];
    NUVEC basis_weights;
    u32 unknown_4c;
    f32 output_w[4];
    i32 skin_indices[3];
    u8 skin_weights[3];
    u8 unknown_6f;
} PC_GEOMETRY_INPUT_PC;
typedef struct PC_GEOMETRY_GROUP_PC {
    u32 element_count;
    u32 first_element;
    u32 transform_index;
} PC_GEOMETRY_GROUP_PC;
typedef struct PC_GEOMETRY_MESH_PC {
    u8 unknown_00[8];
    u32 group_count;
    PC_GEOMETRY_INPUT_PC *elements;
    PC_GEOMETRY_GROUP_PC *groups;
} PC_GEOMETRY_MESH_PC;
typedef struct PC_GEOMETRY_SKIN_PC {
    u8 unknown_00[8];
    u32 matrix_group_count;
    u32 unknown_0c;
    NUMTX *matrices;
} PC_GEOMETRY_SKIN_PC;
typedef struct PC_GEOMETRY_BASIS_PC {
    NUMTX matrices[3];
} PC_GEOMETRY_BASIS_PC;
STATIC_ASSERT(sizeof(PC_GEOMETRY_INPUT_PC) == 0x70);
STATIC_ASSERT(offsetof(PC_GEOMETRY_INPUT_PC, corners) == 0);
STATIC_ASSERT(offsetof(PC_GEOMETRY_INPUT_PC, basis_weights) == 0x40);
STATIC_ASSERT(offsetof(PC_GEOMETRY_INPUT_PC, output_w) == 0x50);
STATIC_ASSERT(offsetof(PC_GEOMETRY_INPUT_PC, skin_indices) == 0x60);
STATIC_ASSERT(offsetof(PC_GEOMETRY_INPUT_PC, skin_weights) == 0x6c);
STATIC_ASSERT(sizeof(PC_GEOMETRY_GROUP_PC) == 0xc);
STATIC_ASSERT(offsetof(PC_GEOMETRY_GROUP_PC, element_count) == 0);
STATIC_ASSERT(offsetof(PC_GEOMETRY_GROUP_PC, first_element) == 4);
STATIC_ASSERT(offsetof(PC_GEOMETRY_GROUP_PC, transform_index) == 8);
STATIC_ASSERT(offsetof(PC_GEOMETRY_MESH_PC, group_count) == 8);
STATIC_ASSERT(offsetof(PC_GEOMETRY_MESH_PC, elements) == 0xc);
STATIC_ASSERT(offsetof(PC_GEOMETRY_MESH_PC, groups) == 0x10);
STATIC_ASSERT(offsetof(PC_GEOMETRY_SKIN_PC, matrix_group_count) == 8);
STATIC_ASSERT(offsetof(PC_GEOMETRY_SKIN_PC, matrices) == 0x10);
STATIC_ASSERT(sizeof(PC_GEOMETRY_BASIS_PC) == 0xc0);
extern "C" PC_GEOMETRY_BASIS_PC *PcGeometryTransformsPc;
extern "C" PC_GEOMETRY_OUTPUT_PC *PcGeometryOutputPc;

STATIC_ASSERT(sizeof(PC_GEOMETRY_CORNER_PC) == 0x10);
STATIC_ASSERT(offsetof(PC_GEOMETRY_CORNER_PC, position) == 0);
STATIC_ASSERT(offsetof(PC_GEOMETRY_CORNER_PC, w) == 0xc);

extern "C" i32 __cdecl PcInitializeDynamicGeometryBuffersPc(PC_DYNAMIC_GEOMETRY_PC *geometry, i32 count) {
    geometry->vertex_buffer = NULL;
    PCDeviceCreateVertexBuffer((u32)count * 0x50, 0x208, 0, 0, &geometry->vertex_buffer);
    i32 segments = geometry->format ? geometry->format->records[geometry->format->record_count - 1].strip_segments : 5;
    i32 vertices = (i32)((u32)segments * 2 + 2);
    PCDeviceCreateVertexBuffer((u32)vertices * sizeof(PC_GEOMETRY_STRIP_VERTEX_PC), 0, 0, 0,
                               &geometry->strip_vertex_buffer);
    PC_GEOMETRY_STRIP_VERTEX_PC *strip = NULL;
    PCDeviceLockVertexBuffer(0, 0, (void **)&strip, 0, geometry->strip_vertex_buffer);
    f32 denominator = (f32)(vertices - 3);
    for (i32 i = 0; i < vertices; ++i) {
        strip[i].fraction = (f32)(i - 1) / denominator;
        if (0.0f > strip[i].fraction)
            strip[i].fraction = 0.0f;
        if (1.0f < strip[i].fraction)
            strip[i].fraction = 1.0f;
        strip[i].side = (i & 1) ? -0.5f : 0.5f;
    }
    PCDeviceUnlockVertexBuffer(geometry->strip_vertex_buffer);
    PCDeviceCreateIndexBuffer((u32)vertices * sizeof(u16), 0, 0x65, 0, &geometry->strip_index_buffer, NULL);
    u16 *indices;
    geometry->strip_index_buffer->methods->lock(geometry->strip_index_buffer, 0, 0, (void **)&indices, 0);
    for (i32 i = 0; i < vertices; ++i)
        indices[i] = (u16)i;
    geometry->strip_index_buffer->methods->unlock(geometry->strip_index_buffer);
    return 1;
}

extern "C" i32 __cdecl PcUpdateGeometryDistancesPc(PC_GEOMETRY_OUTPUT_PC *output, PC_GEOMETRY_SORT_ITEM_PC *items,
                                                   i32 count) {
    NUVEC4 origin = PcGeometryDistanceOriginPc;
    for (i32 i = 0; i < count; ++i) {
        f32 dx = origin.x - output[i].corners[2].position.x;
        f32 dy = origin.y - output[i].corners[2].position.y;
        f32 dz = origin.z - output[i].corners[2].position.z;
        output[i].distance_squared = (f32)(((f64)dx * dx + (f64)dy * dy) + (f64)dz * dz);
    }
    for (i32 i = 0; i < count; ++i)
        items[i].distance_squared = output[items[i].output_index].distance_squared;
    return 1;
}

extern "C" u8 __cdecl PcTransformDynamicGeometryPc(PC_GEOMETRY_SKIN_PC *skin, PC_GEOMETRY_MESH_PC *mesh,
                                                   u32 *output_index) {
    for (u32 group_index = 0; group_index < mesh->group_count; ++group_index) {
        PC_GEOMETRY_GROUP_PC *group = &mesh->groups[group_index];
        PC_GEOMETRY_BASIS_PC *basis = &PcGeometryTransformsPc[group->transform_index];
        for (u32 element = 0; element < group->element_count; ++element) {
            PC_GEOMETRY_INPUT_PC *input = &mesh->elements[group->first_element + element];
            PC_GEOMETRY_OUTPUT_PC *output = &PcGeometryOutputPc[*output_index];
            NUVEC products[3][3], combined[3], axes[3];
            for (i32 axis = 0; axis < 3; ++axis) {
                NUMTX *m = &basis->matrices[axis];
                f32 weight = axis == 0   ? input->basis_weights.x
                             : axis == 1 ? input->basis_weights.y
                                         : input->basis_weights.z;
                products[axis][0].x = weight * m->m00;
                products[axis][0].y = weight * m->m01;
                products[axis][0].z = weight * m->m02;
                products[axis][1].x = weight * m->m10;
                products[axis][1].y = weight * m->m11;
                products[axis][1].z = weight * m->m12;
                products[axis][2].x = weight * m->m20;
                products[axis][2].y = weight * m->m21;
                products[axis][2].z = weight * m->m22;
            }
            for (i32 row = 0; row < 3; ++row) {
                combined[row].x = products[0][row].x + products[1][row].x;
                combined[row].y = products[0][row].y + products[1][row].y;
                combined[row].z = products[0][row].z + products[1][row].z;
                axes[row].x = combined[row].x + products[2][row].x;
                axes[row].y = combined[row].y + products[2][row].y;
                axes[row].z = combined[row].z + products[2][row].z;
            }
            NUVEC cross;
            NuVecCross(&cross, &axes[1], &axes[2]);
            PC_GEOMETRY_CORNER_PC *corners = output->corners;
            if (input->skin_indices[0] >= 0 && skin->matrix_group_count > 1) {
                corners[0] = input->corners[0];
                for (i32 corner = 1; corner < 4; ++corner) {
                    NUVEC delta;
                    NuVecSub(&delta, &input->corners[corner].position, &input->corners[corner - 1].position);
                    corners[corner] = corners[corner - 1];
                    for (i32 j = 0; j < 3; ++j) {
                        i32 index = input->skin_indices[j];
                        if (index >= 0 && index < (i32)skin->matrix_group_count) {
                            NUVEC transformed;
                            f32 weight = (f32)((f64)input->skin_weights[j] * (f64)(1.0f / 255.0f));
                            NuVecMtxTransform(&transformed, &delta, &skin->matrices[index * 3 + corner - 1]);
                            NuVecScale(&transformed, &transformed, weight);
                            NuVecAdd(&corners[corner].position, &corners[corner].position, &transformed);
                        }
                    }
                }
            } else {
                for (i32 corner = 0; corner < 4; ++corner)
                    corners[corner] = input->corners[corner];
            }
            NUMTX matrix;
            matrix.m00 = axes[2].x;
            matrix.m01 = axes[2].y;
            matrix.m02 = axes[2].z;
            matrix.m03 = 0;
            matrix.m10 = cross.x;
            matrix.m11 = cross.y;
            matrix.m12 = cross.z;
            matrix.m13 = 0;
            matrix.m20 = axes[1].x;
            matrix.m21 = axes[1].y;
            matrix.m22 = axes[1].z;
            matrix.m23 = 0;
            matrix.m30 = axes[0].x;
            matrix.m31 = axes[0].y;
            matrix.m32 = axes[0].z;
            matrix.m33 = 1;
            for (i32 corner = 0; corner < 4; ++corner)
                NuVecMtxTransform(&corners[corner].position, &corners[corner].position, &matrix);
            output->basis_axis = axes[1];
            output->distance_squared = 0;
            for (i32 corner = 0; corner < 4; ++corner)
                corners[corner].w = input->output_w[corner];
            ++*output_index;
        }
    }
    return 1;
}
