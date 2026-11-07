#include "saga.h"
#include <stdlib.h>
#include <string.h>

static __declspec(noinline) void OcclusionMtxCopyPrivate(NUMTX *destination, const NUMTX *source) {
    destination->m00 = source->m00;
    destination->m01 = source->m01;
    destination->m02 = source->m02;
    destination->m03 = source->m03;
    destination->m10 = source->m10;
    destination->m11 = source->m11;
    destination->m12 = source->m12;
    destination->m13 = source->m13;
    destination->m20 = source->m20;
    destination->m21 = source->m21;
    destination->m22 = source->m22;
    destination->m23 = source->m23;
    destination->m30 = source->m30;
    destination->m31 = source->m31;
    destination->m32 = source->m32;
    destination->m33 = source->m33;
}

bool OccluderSet::IsOccludedSphere(NUVEC const *center, f32 radius) {
    if (!queries_prepared)
        return false;
    __declspec(align(16)) NUVEC4 projected;
    NuVec4MtxTransform(&projected, const_cast<NUVEC *>(center), &projection_matrix);
    if (projected.w < radius)
        return false;
    projected.x /= projected.w;
    i32 limit = static_cast<i32>(count) < 101 ? static_cast<i32>(count) : 100;
    projected.y /= projected.w;
    projected.z /= projected.w;
    f32 screen_radius = radius / projected.w;
    for (i32 i = 0; i != limit; ++i) {
        if (indices[i] == 0xffffffffu)
            continue;
        PC_OCCLUDER_RECORD &record = occluders[indices[i]];
        if (record.depth > projected.w - radius - 2.0)
            return false;
        if (projected.x - screen_radius > record.max_x || projected.y - screen_radius > record.max_y ||
            record.min_x > projected.x + screen_radius || record.min_y > projected.y + screen_radius)
            continue;
        NUVEC4 *v = record.transformed;
        NUVEC edge_a, edge_b, cross;
        edge_a.x = v[1].x - v[0].x;
        edge_a.y = v[1].y - v[0].y;
        edge_a.z = v[1].z - v[0].z;
        edge_b.x = v[2].x - v[0].x;
        edge_b.y = v[2].y - v[0].y;
        edge_b.z = v[2].z - v[0].z;
        NuVecCross(&cross, &edge_b, &edge_a);
        bool clockwise = cross.z > 0.0f;
        __declspec(align(16)) NUVEC4 normal;
        normal.z = normal.w = 0.0f;
        for (i32 edge = 0; edge < 4; ++edge) {
            i32 start = edge;
            i32 end = (edge + 1) % 4;
            if (!clockwise) {
                end = 3 - end;
                start = 3 - edge;
            }
            normal.x = v[end].y - v[start].y;
            normal.y = -(v[end].x - v[start].x);
            NuVecNorm(reinterpret_cast<NUVEC *>(&normal), reinterpret_cast<NUVEC *>(&normal));
            normal.z = normal.w = 0.0f;
            NUVEC delta;
            delta.x = projected.x - v[start].x;
            delta.y = projected.y - v[start].y;
            delta.z = 0.0f;
            f32 distance = normal.x * delta.x + delta.y * normal.y + delta.z * normal.z;
            if (distance - screen_radius < 0.0)
                goto next_occluder;
        }
        return true;
    next_occluder:;
    }
    return false;
}

bool OccluderSet::IsOccludedOBB(NUVEC const *minimum, NUVEC const *maximum, NUMTX const *matrix) {
    float min_x = 1000000.0f, min_y = 1000000.0f, min_depth = 1000000.0f;
    float max_x = -1000000.0f, max_y = -1000000.0f;
    if (!queries_prepared)
        return false;
    NUVEC4 corners[8] = {{minimum->x, maximum->y, minimum->z, 1.0f}, {minimum->x, maximum->y, maximum->z, 1.0f},
                         {maximum->x, maximum->y, maximum->z, 1.0f}, {maximum->x, maximum->y, minimum->z, 1.0f},
                         {minimum->x, minimum->y, minimum->z, 1.0f}, {minimum->x, minimum->y, maximum->z, 1.0f},
                         {maximum->x, minimum->y, maximum->z, 1.0f}, {maximum->x, minimum->y, minimum->z, 1.0f}};
    __declspec(align(16)) NUMTX transform;
    if (matrix)
        NuMtxMulH(&transform, const_cast<NUMTX *>(matrix), &projection_matrix);
    else
        OcclusionMtxCopyPrivate(&transform, &projection_matrix);
    for (i32 i = 0; i < 8; ++i) {
        NUVEC4 &v = corners[i];
        NuVec4MtxTransform(&v, reinterpret_cast<NUVEC *>(&v), &transform);
        if (!(0.0f < v.w))
            return false;
        float x = v.x / v.w;
        v.x = x;
        float y = v.y / v.w;
        v.y = y;
        v.z /= v.w;
        if (min_x > x)
            min_x = x;
        if (min_y > y)
            min_y = y;
        if (max_x < x)
            max_x = x;
        if (max_y < y)
            max_y = y;
        if (min_depth > v.w)
            min_depth = v.w;
    }
    i32 limit = 100;
    if (static_cast<i32>(count) <= 100)
        limit = static_cast<i32>(count);
    for (i32 i = 0; i != limit; ++i) {
        if (indices[i] == 0xffffffffu)
            continue;
        PC_OCCLUDER_RECORD &record = occluders[indices[i]];
        if (record.depth > min_depth - 2.0f)
            return false;
        if (min_x > record.max_x || min_y > record.max_y || record.min_x > max_x || record.min_y > max_y)
            continue;
        NUVEC4 *v = record.transformed;
        NUVEC edge_a, edge_b, cross;
        edge_a.x = v[1].x - v[0].x;
        edge_a.y = v[1].y - v[0].y;
        edge_a.z = v[1].z - v[0].z;
        edge_b.x = v[2].x - v[0].x;
        edge_b.y = v[2].y - v[0].y;
        edge_b.z = v[2].z - v[0].z;
        NuVecCross(&cross, &edge_b, &edge_a);
        bool clockwise = cross.z > 0.0f;
        for (i32 edge = 0; edge < 4; ++edge) {
            i32 start = edge;
            i32 end = (edge + 1) % 4;
            if (!clockwise) {
                end = 3 - end;
                start = 3 - edge;
            }
            NUVEC4 normal;
            normal.x = v[end].y - v[start].y;
            normal.y = -(v[end].x - v[start].x);
            NuVecNorm(reinterpret_cast<NUVEC *>(&normal), reinterpret_cast<NUVEC *>(&normal));
            normal.z = normal.w = 0.0f;
            for (u32 corner = 0; corner < 8; ++corner) {
                NUVEC delta;
                delta.x = corners[corner].x - v[start].x;
                delta.y = corners[corner].y - v[start].y;
                delta.z = 0.0f;
                f32 distance = delta.y * normal.y + normal.x * delta.x + delta.z * normal.z;
                if (distance < 0.01f)
                    goto next_occluder;
            }
        }
        return true;
    next_occluder:;
    }
    return false;
}

void OccluderSet::PrepareForQueries(NUMTX const *query, NUMTX const *projection) {
    projection_matrix = *projection;
    query_matrix = *query;
    for (u32 index = 0; index != count; ++index) {
        PC_OCCLUDER_RECORD &record = occluders[index];
        record.min_x = 1000000.0f;
        record.min_y = 1000000.0f;
        record.min_depth = 1000000.0f;
        record.max_x = -1000000.0f;
        record.max_y = -1000000.0f;
        record.depth = -1000000.0f;
        bool valid = true;
        for (u32 vertex = 0; vertex < 4; ++vertex) {
            NUVEC4 &v = record.transformed[vertex];
            NuVec4MtxTransform(&v, reinterpret_cast<NUVEC *>(&record.vertices[vertex]), &projection_matrix);
            if (!(v.w > 0.0f)) {
                valid = false;
                break;
            }
            v.x /= v.w;
            v.y /= v.w;
            v.z /= v.w;
            record.min_x = record.min_x > v.x ? v.x : record.min_x;
            record.min_y = record.min_y > v.y ? v.y : record.min_y;
            record.max_x = record.max_x < v.x ? v.x : record.max_x;
            record.max_y = record.max_y < v.y ? v.y : record.max_y;
            record.min_depth = record.min_depth > v.w ? v.w : record.min_depth;
            record.depth = record.depth < v.w ? v.w : record.depth;
        }
        if (valid && (record.depth < 0.0f || record.min_depth < 0.0f || record.max_x < -1.0f || record.min_x > 1.0f ||
                      record.max_y < -1.0f || record.min_y > 1.0f)) {
            valid = false;
        }
        indices[index] = valid ? index : 0xffffffffu;
    }
    qsort(indices, count, sizeof(*indices), SortByDepth);
    queries_prepared = true;
}

i32 OccluderSet::SortByDepth(void const *left, void const *right) {
    i32 a = *static_cast<const i32 *>(left);
    i32 b = *static_cast<const i32 *>(right);
    if (a == -1)
        return 1;
    if (b == -1)
        return -1;
    PC_OCCLUDER_RECORD *records = reinterpret_cast<OccluderSet *>(g_OcclusionManager.current_set)->occluders;
    return records[b].depth > records[a].depth ? -1 : 1;
}

bool OcclusionManager::IsOccludedOBB(NUVEC const *minimum, NUVEC const *maximum, NUMTX const *matrix) {
    if (!initialized || !enabled)
        return false;
    if (!current_set->queries_prepared)
        current_set->PrepareForQueries(&vmtx, &vpmtx);
    ++query_count;
    bool occluded = current_set->IsOccludedOBB(minimum, maximum, matrix);
    if (occluded)
        ++occluded_count;
    return occluded;
}
bool OcclusionManager::IsOccludedSphere(NUVEC const *center, f32 radius) {
    if (!initialized || !enabled)
        return false;
    if (!current_set->queries_prepared)
        current_set->PrepareForQueries(&vmtx, &vpmtx);
    ++query_count;
    bool occluded = current_set->IsOccludedSphere(center, radius);
    if (occluded)
        ++occluded_count;
    return occluded;
}

void OccluderSet::OnCameraSet() {
    queries_prepared = false;
}

extern "C" i32 __cdecl NuOcclusionManagerIsInitialised(void) {
    return g_OcclusionManager.initialized != 0;
}

extern "C" i32 __cdecl NuOcclusionManagerIsEnabled(void) {
    return g_OcclusionManager.initialized != 0 && g_OcclusionManager.enabled != 0;
}
/* Original legoapi/render/light/occlusion.cpp family; public material APIs
 * preserve the original inline operations without exposing private ABIs. */
void OccluderSet::Init(u32 max_occluders, VARIPTR *buffer, VARIPTR buffer_end) {
    (void)buffer_end;
    capacity = max_occluders;
    count = 0;
    occluders = reinterpret_cast<PC_OCCLUDER_RECORD *>((buffer->addr + 15u) & ~15u);
    buffer->addr = ((buffer->addr + 15u) & ~15u) + capacity * sizeof(PC_OCCLUDER_RECORD);
    indices = reinterpret_cast<u32 *>((buffer->addr + 3u) & ~3u);
    buffer->addr = ((buffer->addr + 3u) & ~3u) + capacity * sizeof(u32);
    if (ms_pZOnlyMtl3D == NULL) {
        ms_pZOnlyMtl3D = NuMtlCreate3D(1);
        ms_pZOnlyMtl2D = NuMtlCreate(1);
        ms_pAlphaMtl2D = NuMtlCreate(1);
        PC_MATERIAL_SHADER_DESC_VIEW desc2d;
        memset(&desc2d, 0, sizeof(desc2d));
        desc2d.flags = 0x1000;
        desc2d.diffuse_map_id = 0;
        desc2d.vertex_format |= 0x40001;
        NuMtlSetShaderDescPS(ms_pZOnlyMtl2D, &desc2d);
        ms_pZOnlyMtl2D->attribs.z_mode = 0;
        ms_pZOnlyMtl2D->attribs.alpha_mode = 13;
        ms_pZOnlyMtl2D->attribs.unknown_2_8 = 0;
        ms_pZOnlyMtl2D->attribs.cull_mode = 2;
        NuMtlUpdate(ms_pZOnlyMtl2D);
        PC_MATERIAL_SHADER_DESC_VIEW desc3d;
        memset(&desc3d, 0, sizeof(desc3d));
        desc3d.flags = 0x1000;
        desc3d.diffuse_map_id = 0;
        desc3d.vertex_format |= 1;
        NuMtlSetShaderDescPS(ms_pZOnlyMtl3D, &desc3d);
        ms_pZOnlyMtl3D->attribs.z_mode = 0;
        ms_pZOnlyMtl3D->attribs.alpha_mode = 13;
        ms_pZOnlyMtl3D->attribs.unknown_2_8 = 0;
        ms_pZOnlyMtl3D->attribs.cull_mode = 2;
        NuMtlUpdate(ms_pZOnlyMtl3D);
        ms_pAlphaMtl2D->attribs.z_mode = 1;
        ms_pAlphaMtl2D->attribs.alpha_mode = 1;
        ms_pAlphaMtl2D->attribs.cull_mode = 2;
        NuMtlUpdate(ms_pAlphaMtl2D);
    }
}

void OcclusionManager::Init(u32 capacity, VARIPTR *buffer, VARIPTR buffer_end) {
    if (capacity != 0) {
        sets[0].Init(capacity, buffer, buffer_end);
        sets[1].Init(capacity, buffer, buffer_end);
        unknown_158 = 0.3f;
        building_set = &sets[0];
        current_set = &sets[1];
        unknown_15c = -1.0f;
        initialized = true;
        enabled = true;
    }
}

void OccluderSet::AddOccluder(const NUVEC *a, const NUVEC *b, const NUVEC *c, const NUVEC *d) {
    if (count < capacity) {
        queries_prepared = false;
        NUVEC4 *vertex = &occluders[count].vertices[0];
        vertex->x = a->x;
        vertex->y = a->y;
        vertex->z = a->z;
        vertex->w = 1.0f;
        vertex = &occluders[count].vertices[1];
        vertex->x = b->x;
        vertex->y = b->y;
        vertex->z = b->z;
        vertex->w = 1.0f;
        vertex = &occluders[count].vertices[2];
        vertex->x = c->x;
        vertex->y = c->y;
        vertex->z = c->z;
        vertex->w = 1.0f;
        vertex = &occluders[count].vertices[3];
        vertex->x = d->x;
        vertex->y = d->y;
        vertex->z = d->z;
        vertex->w = 1.0f;
        ++count;
    }
}

typedef struct OCCLUDER_PRIM_VERTEX_PC {
    f32 x, y, z;
    u32 colour;
    f32 u, v;
} OCCLUDER_PRIM_VERTEX_PC;
STATIC_ASSERT(sizeof(OCCLUDER_PRIM_VERTEX_PC) == 0x18);
STATIC_ASSERT(offsetof(OCCLUDER_PRIM_VERTEX_PC, colour) == 0xc);

/* Same-TU copy of the nurndr.c colour helper (native EAX colour). */
extern "C" {
    static __declspec(noinline) void NuRndrPrimSetColourPrivate(i32 colour) {
        u32 converted;
        if (NuPrimNeedsOverbrightening != 0)
            converted = ((colour & 0xff) << 16) + ((colour >> 16) & 0xff) + (colour & 0xff00) + (colour & 0xff000000);
        else
            converted =
                ((colour & 0xfe) << 15) + ((colour >> 1) & 0x7f00) + ((colour >> 17) & 0x7f) + (colour & 0xff000000);
        ((OCCLUDER_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour = converted;
    }
}

#define OCCLUDER_PRIM_POSITION(source)                                                                                 \
    do {                                                                                                               \
        f32 position_x = (source).x, position_y = (source).y, position_z = (source).z;                                 \
        ((OCCLUDER_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->x = position_x;                              \
        ((OCCLUDER_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->y = position_y;                              \
        ((OCCLUDER_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->z = position_z;                              \
        NuPrimStreamBufferPointer->addr += sizeof(OCCLUDER_PRIM_VERTEX_PC);                                            \
        ++NuPrimVertexCount;                                                                                           \
    } while (0)

void OccluderSet::RenderOccluders(bool depth_only) const {
    i32 colour = 0;
    NUMTL *material = NULL;
    if (depth_only)
        material = queries_prepared ? ms_pZOnlyMtl2D : ms_pZOnlyMtl3D;
    if (queries_prepared) {
        ++NuPrimCSPos;
        NuPrimSetCoordinateSystem(1);
        NuPrim2DBegin(12, 5, material);
        for (u32 i = 0; i != count; ++i) {
            if (indices[i] == 0xffffffffu)
                continue;
            const PC_OCCLUDER_RECORD &record = occluders[indices[i]];
            if (record.min_depth < 0.0f || record.depth < 0.0f)
                continue;
            colour = (i32)((u32)(record.vertices[0].z * record.vertices[0].x) | 0xff000000u);
            NuRndrPrimSetColourPrivate(colour);
            NuPrim2DAddXYZ(record.transformed[0].x, (f32)(record.transformed[0].y * -1.0), record.transformed[0].z);
            NuRndrPrimSetColourPrivate(colour);
            NuPrim2DAddXYZ(record.transformed[1].x, (f32)(record.transformed[1].y * -1.0), record.transformed[1].z);
            NuRndrPrimSetColourPrivate(colour);
            NuPrim2DAddXYZ(record.transformed[2].x, (f32)(record.transformed[2].y * -1.0), record.transformed[2].z);
            NuRndrPrimSetColourPrivate(colour);
            NuPrim2DAddXYZ(record.transformed[2].x, (f32)(record.transformed[2].y * -1.0), record.transformed[2].z);
            NuRndrPrimSetColourPrivate(colour);
            NuPrim2DAddXYZ(record.transformed[3].x, (f32)(record.transformed[3].y * -1.0), record.transformed[3].z);
            NuRndrPrimSetColourPrivate(colour);
            NuPrim2DAddXYZ(record.transformed[0].x, (f32)(record.transformed[0].y * -1.0), record.transformed[0].z);
        }
        NuPrim2DEnd();
        --NuPrimCSPos;
        NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
    } else {
        NuPrim3DBegin(12, 5, material, NULL);
        for (u32 i = 0; i != count; ++i) {
            const PC_OCCLUDER_RECORD &record = occluders[i];
            colour = (i32)((u32)(record.vertices[0].z * record.vertices[0].x) | 0xff000000u);
            NuRndrPrimSetColourPrivate(colour);
            OCCLUDER_PRIM_POSITION(record.vertices[0]);
            NuRndrPrimSetColourPrivate(colour);
            OCCLUDER_PRIM_POSITION(record.vertices[1]);
            NuRndrPrimSetColourPrivate(colour);
            OCCLUDER_PRIM_POSITION(record.vertices[2]);
            NuRndrPrimSetColourPrivate(colour);
            OCCLUDER_PRIM_POSITION(record.vertices[2]);
            NuRndrPrimSetColourPrivate(colour);
            OCCLUDER_PRIM_POSITION(record.vertices[3]);
            NuRndrPrimSetColourPrivate(colour);
            OCCLUDER_PRIM_POSITION(record.vertices[0]);
        }
        NuPrim3DEnd();
    }
}
#undef OCCLUDER_PRIM_POSITION

void OcclusionManager::EndFrame() {
}

void OcclusionManager::BeginFrame() {
    if (initialized && enabled) {
        OccluderSet *completed = building_set;
        OccluderSet *building = current_set;
        current_set = completed;
        building_set = building;
        building->count = 0;
        building->queries_prepared = false;
        occluded_count = 0;
        query_count = 0;
    }
}

void OcclusionManager::OnCameraSet() {
    if (initialized && enabled)
        current_set->OnCameraSet();
}

void OcclusionManager::RenderZPass() const {
    if (initialized && enabled)
        current_set->RenderOccluders(true);
}

void OcclusionManager::AddOccluder(const NUVEC *a, const NUVEC *b, const NUVEC *c, const NUVEC *d) {
    if (!initialized || !enabled || building_set->count >= building_set->capacity)
        return;
    if (unknown_158 > 0.0f || unknown_15c > 0.0f) {
        __declspec(align(16)) NUVEC4 projected[4];
        NuVec4MtxTransform(&projected[0], const_cast<NUVEC *>(a), &vpmtx);
        NuVec4MtxTransform(&projected[1], const_cast<NUVEC *>(b), &vpmtx);
        NuVec4MtxTransform(&projected[2], const_cast<NUVEC *>(c), &vpmtx);
        if (unknown_158 > 0.0f) {
            NUVEC ac = {projected[2].x - projected[0].x, projected[2].y - projected[0].y,
                        projected[2].z - projected[0].z};
            NUVEC ab = {projected[1].x - projected[0].x, projected[1].y - projected[0].y,
                        projected[1].z - projected[0].z};
            NUVEC normal;
            NuVecCross(&normal, &ab, &ac);
            NuVecNorm(&normal, &normal);
            f32 z = normal.z;
            u32 bits = *reinterpret_cast<u32 *>(&z) & 0x7fffffff;
            if (*reinterpret_cast<f32 *>(&bits) < unknown_158)
                return;
        }
        NuVec4MtxTransform(&projected[3], const_cast<NUVEC *>(d), &vpmtx);
        f32 min_x = 1000000.0f, min_y = 1000000.0f, min_depth = 1000000.0f;
        f32 max_x = -1000000.0f, max_y = -1000000.0f;
        for (u32 i = 0; i < 4; ++i) {
            NUVEC4 &v = projected[i];
            if (v.w < 0.001f)
                return;
            f32 x = v.x / v.w;
            v.x = x;
            f32 y = v.y / v.w;
            v.y = y;
            v.z /= v.w;
            if (min_x > x)
                min_x = x;
            if (min_y > y)
                min_y = y;
            if (max_x < x)
                max_x = x;
            if (max_y < y)
                max_y = y;
            if (v.w < min_depth)
                min_depth = v.w;
        }
        if (unknown_15c > 0.0f) {
            f32 area = (max_y - min_y) * (max_x - min_x);
            if (area * 0.25 < unknown_15c)
                return;
        }
    }
    building_set->AddOccluder(a, b, c, d);
}

void OcclusionManager::AddOccluder(const NUVEC *center, f32 radius) {
    if (!initialized || !enabled)
        return;
    NUMTX view;
    OcclusionMtxCopyPrivate(&view, &vmtx);
    NUVEC right = {view.m00 * radius, view.m10 * radius, view.m20 * radius};
    NUVEC up = {view.m01 * radius, view.m11 * radius, view.m21 * radius};
    NUVEC forward = {view.m02 * radius, view.m12 * radius, view.m22 * radius};
    NUVEC4 a = {center->x, center->y, center->z, 1.0f};
    NUVEC4 b = {center->x, center->y, center->z, 1.0f};
    NUVEC4 c = {center->x, center->y, center->z, 1.0f};
    NUVEC4 d = {center->x, center->y, center->z, 1.0f};
    a.x += up.x;
    a.y += up.y;
    a.z += up.z;
    a.x -= right.x;
    a.y -= right.y;
    a.z -= right.z;
    a.x -= forward.x;
    a.y -= forward.y;
    a.z -= forward.z;
    b.x += up.x;
    b.y += up.y;
    b.z += up.z;
    b.x += right.x;
    b.y += right.y;
    b.z += right.z;
    b.x += forward.x;
    b.y += forward.y;
    b.z += forward.z;
    c.x -= up.x;
    c.y -= up.y;
    c.z -= up.z;
    c.x += right.x;
    c.y += right.y;
    c.z += right.z;
    c.x += forward.x;
    c.y += forward.y;
    c.z += forward.z;
    d.x -= up.x;
    d.y -= up.y;
    d.z -= up.z;
    d.x -= right.x;
    d.y -= right.y;
    d.z -= right.z;
    d.x -= forward.x;
    d.y -= forward.y;
    d.z -= forward.z;
    AddOccluder(reinterpret_cast<NUVEC *>(&a), reinterpret_cast<NUVEC *>(&b), reinterpret_cast<NUVEC *>(&c),
                reinterpret_cast<NUVEC *>(&d));
}

void OcclusionManager::AddOccluder(const NUVEC *minimum, const NUVEC *maximum, const NUMTX *matrix) {
    if (!initialized || !enabled)
        return;
    const f32 min_x = minimum->x;
    const f32 min_y = minimum->y;
    const f32 min_z = minimum->z;
    const f32 max_x = maximum->x;
    const f32 max_y = maximum->y;
    const f32 max_z = maximum->z;
    __declspec(align(16)) NUVEC4 corners[8] = {{min_x, max_y, min_z, 1.0f}, {max_x, max_y, max_z, 1.0f},
                                               {max_x, min_y, max_z, 1.0f}, {min_x, min_y, min_z, 1.0f},
                                               {min_x, max_y, max_z, 1.0f}, {max_x, max_y, min_z, 1.0f},
                                               {max_x, min_y, min_z, 1.0f}, {min_x, min_y, max_z, 1.0f}};
    for (i32 i = 0; i < 8; ++i) {
        NUVEC *vertex = reinterpret_cast<NUVEC *>(&corners[i]);
        NuVecMtxTransform(vertex, vertex, const_cast<NUMTX *>(matrix));
    }
    AddOccluder(reinterpret_cast<NUVEC *>(&corners[0]), reinterpret_cast<NUVEC *>(&corners[1]),
                reinterpret_cast<NUVEC *>(&corners[2]), reinterpret_cast<NUVEC *>(&corners[3]));
    AddOccluder(reinterpret_cast<NUVEC *>(&corners[4]), reinterpret_cast<NUVEC *>(&corners[5]),
                reinterpret_cast<NUVEC *>(&corners[6]), reinterpret_cast<NUVEC *>(&corners[7]));
}

extern "C" {
    void __cdecl NuOcclusionManagerOnCameraSet(void) {
        g_OcclusionManager.OnCameraSet();
    }

    void __cdecl NuOcclusionManagerRenderZPass(void) {
        g_OcclusionManager.RenderZPass();
    }

    void __cdecl NuOcclusionManagerAddOccluderQuad(const NUVEC *a, const NUVEC *b, const NUVEC *c, const NUVEC *d) {
        g_OcclusionManager.AddOccluder(a, b, c, d);
    }

    void __cdecl NuOcclusionManagerAddOccluderSphere(const NUVEC *center, f32 radius) {
        g_OcclusionManager.AddOccluder(center, radius);
    }

    void __cdecl NuOcclusionManagerAddOccluderOBB(const NUVEC *minimum, const NUVEC *maximum, const NUMTX *matrix) {
        g_OcclusionManager.AddOccluder(minimum, maximum, matrix);
    }
}

/* Genuine original occlusion.cpp static members; outside extern C. */
NUMTL *OccluderSet::ms_pZOnlyMtl3D = 0;
NUMTL *OccluderSet::ms_pZOnlyMtl2D = 0;
NUMTL *OccluderSet::ms_pAlphaMtl2D = 0;

/* Original same-TU register helper: EDI plane, ESI origin, ECX point_a, EAX point_b. */
static __declspec(noinline) void PostShadowMakePlanePrivatePc(NUVEC4 *plane, const NUVEC4 *origin,
                                                              const NUVEC4 *point_a, const NUVEC4 *point_b) {
    NUVEC a = {point_a->x - origin->x, point_a->y - origin->y, point_a->z - origin->z};
    NUVEC b = {point_b->x - origin->x, point_b->y - origin->y, point_b->z - origin->z};
    NuVecCross(reinterpret_cast<NUVEC *>(plane), &a, &b);
    NuVecNorm(reinterpret_cast<NUVEC *>(plane), reinterpret_cast<NUVEC *>(plane));
    plane->w = 1.0f;
    f32 dot = (plane->y * origin->y + origin->x * plane->x) + plane->z * origin->z;
    plane->w = -dot;
}
extern "C" void __cdecl PostShadowComputeFrustumCubePc(const NUCAMERA *camera, NUVEC4 *corners, NUVEC4 *plane) {
    f32 half_fov = (f32)((f64)camera->fov * 0.5);
    i32 angle = (i32)((f64)half_fov * 10430.3779296875);
    f32 height = NuTrigTable[(angle >> 1) & 0x7fff] * camera->near_clip /
                 NuTrigTable[(((i32)((u32)angle + 0x4000u)) >> 1) & 0x7fff];
    f32 width = height / camera->aspect;
    f32 far_scale = camera->far_clip / camera->near_clip;
    corners[0].x = -width;
    corners[0].y = -height;
    corners[0].z = camera->near_clip;
    corners[0].w = 1.0f;
    corners[1].x = -width;
    corners[1].y = height;
    corners[1].z = camera->near_clip;
    corners[1].w = 1.0f;
    corners[2].x = width;
    corners[2].y = height;
    corners[2].z = camera->near_clip;
    corners[2].w = 1.0f;
    corners[3].x = width;
    corners[3].y = -height;
    corners[3].z = camera->near_clip;
    corners[3].w = 1.0f;
    __declspec(align(16)) NUVEC4 scaled;
    for (i32 i = 0; i < 4; ++i) {
        scaled.x = corners[i].x * far_scale;
        scaled.y = corners[i].y * far_scale;
        scaled.z = corners[i].z * far_scale;
        /* Original copies the unwritten fourth word of this XYZ scratch. */
        corners[i + 4] = scaled;
    }
    NUMTX matrix;
    OcclusionMtxCopyPrivate(&matrix, &camera->mtx);
    for (i32 i = 0; i < 8; ++i)
        NuVecMtxTransform(reinterpret_cast<NUVEC *>(&corners[i]), reinterpret_cast<NUVEC *>(&corners[i]), &matrix);
    PostShadowMakePlanePrivatePc(plane, &corners[4], &corners[5], &corners[6]);
}
extern "C" i32 __cdecl PostShadowComputePlanesPc(const NUVEC4 *direction, const NUVEC4 *corners, NUVEC4 *planes) {
    f32 extrusion = -PostShadowPlaneExtrusionPc;
    NUVEC offset = {direction->x * extrusion, direction->y * extrusion, direction->z * extrusion};
    u8 positive[6];
    i32 count = 0;
    NUVEC4 *output = planes;
    for (i32 face = 0; face < 6; ++face) {
        const NUVEC4 *origin = &corners[PostShadowCubeFaceIndicesPc[face][0]];
        const NUVEC4 *point_a = &corners[PostShadowCubeFaceIndicesPc[face][1]];
        const NUVEC4 *point_b = &corners[PostShadowCubeFaceIndicesPc[face][2]];
        NUVEC b = {point_b->x - origin->x, point_b->y - origin->y, point_b->z - origin->z};
        NUVEC a = {point_a->x - origin->x, point_a->y - origin->y, point_a->z - origin->z};
        NUVEC normal;
        NuVecCross(&normal, &a, &b);
        f32 dot = (normal.y * offset.y + normal.x * offset.x) + normal.z * offset.z;
        positive[face] = dot >= 0.0f;
        if (positive[face] == 0) {
            output->x = -normal.x;
            output->y = -normal.y;
            output->z = -normal.z;
            output->w = 0.0f;
            NuVecNorm(reinterpret_cast<NUVEC *>(output), reinterpret_cast<NUVEC *>(output));
            ++count;
            output->w = 1.0f;
            f32 plane_dot = (output->y * origin->y + output->x * origin->x) + output->z * origin->z;
            output->w = -plane_dot;
            ++output;
        }
    }
    output = planes + count;
    for (i32 edge = 0; edge < 12 && count < 9; ++edge) {
        u8 first_positive = positive[cubeEdgeTopologies[edge][0]];
        if (first_positive != positive[cubeEdgeTopologies[edge][1]]) {
            const NUVEC4 *origin = &corners[cubeEdgeIndices[edge][0]];
            const NUVEC4 *other = &corners[cubeEdgeIndices[edge][1]];
            NUVEC4 extruded;
            extruded.x = origin->x + offset.x;
            extruded.y = origin->y + offset.y;
            extruded.z = origin->z + offset.z;
            if (first_positive != 0)
                PostShadowMakePlanePrivatePc(output, origin, other, &extruded);
            else
                PostShadowMakePlanePrivatePc(output, &extruded, other, origin);
            ++count;
            ++output;
        }
    }
    return count;
}
extern "C" void __cdecl PostShadowComputeCapsulePc(const NUVEC4 *direction, const NUVEC4 *corners, NUVEC4 *center,
                                                   NUVEC4 *endpoint, f32 *radius) {
    *center = corners[0];
    for (i32 i = 1; i < 8; ++i) {
        center->x += corners[i].x;
        center->y += corners[i].y;
        center->z += corners[i].z;
    }
    center->x = (f32)((f64)center->x * 0.125);
    center->y = (f32)((f64)center->y * 0.125);
    center->z = (f32)((f64)center->z * 0.125);
    *radius = 0.0f;
    f32 distance;
    NUVEC4 delta;
    for (i32 i = 0; i < 8; ++i) {
        delta.x = center->x - corners[i].x;
        delta.y = center->y - corners[i].y;
        delta.z = center->z - corners[i].z;
        distance = (delta.x * delta.x + delta.y * delta.y) + delta.z * delta.z;
        if (*radius < distance)
            *radius = distance;
    }
    /* Native passes the final vertex distance, after all maximum stores. */
    *radius = NuFsqrt(distance);
    NUVEC scaled = {(f32)((f64)direction->x * 100.0), (f32)((f64)direction->y * 100.0),
                    (f32)((f64)direction->z * 100.0)};
    delta.x = center->x - scaled.x;
    delta.y = center->y - scaled.y;
    delta.z = center->z - scaled.z;
    /* As in the retail stack scratch, the fourth component was not assigned. */
    *endpoint = delta;
}
