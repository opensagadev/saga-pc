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

bool OccluderSet::IsOccludedOBB(NUVEC const *minimum, NUVEC const *maximum, NUMTX const *matrix) {
    if (!queries_prepared)
        return false;
    NUVEC4 corners[8] = {{minimum->x, maximum->y, minimum->z, 1.0f}, {minimum->x, maximum->y, maximum->z, 1.0f},
                         {maximum->x, maximum->y, maximum->z, 1.0f}, {maximum->x, maximum->y, minimum->z, 1.0f},
                         {minimum->x, minimum->y, minimum->z, 1.0f}, {minimum->x, minimum->y, maximum->z, 1.0f},
                         {maximum->x, minimum->y, maximum->z, 1.0f}, {maximum->x, minimum->y, minimum->z, 1.0f}};
    NUMTX transform;
    if (matrix)
        NuMtxMulH(&transform, const_cast<NUMTX *>(matrix), &projection_matrix);
    else
        OcclusionMtxCopyPrivate(&transform, &projection_matrix);
    float min_x = 1000000.0f, min_y = 1000000.0f, min_depth = 1000000.0f;
    float max_x = -1000000.0f, max_y = -1000000.0f;
    for (u32 i = 0; i < 8; ++i) {
        NUVEC4 &v = corners[i];
        NuVec4MtxTransform(&v, reinterpret_cast<NUVEC *>(&v), &transform);
        if (v.w <= 0.0f)
            return false;
        v.x /= v.w;
        v.y /= v.w;
        v.z /= v.w;
        min_x = v.x < min_x ? v.x : min_x;
        min_y = v.y < min_y ? v.y : min_y;
        max_x = v.x > max_x ? v.x : max_x;
        max_y = v.y > max_y ? v.y : max_y;
        min_depth = v.w < min_depth ? v.w : min_depth;
    }
    i32 limit = static_cast<i32>(count) < 100 ? static_cast<i32>(count) : 100;
    for (i32 i = 0; i != limit; ++i) {
        if (indices[i] == 0xffffffffu)
            continue;
        PC_OCCLUDER_RECORD &record = occluders[indices[i]];
        if (record.depth > min_depth - 2.0f)
            return false;
        if (min_x > record.max_x || min_y > record.max_y || record.min_x > max_x || record.min_y > max_y)
            continue;
        NUVEC4 *v = record.transformed;
        float winding = (v[2].x - v[0].x) * (v[1].y - v[0].y) - (v[1].x - v[0].x) * (v[2].y - v[0].y);
        bool inside = true;
        for (i32 edge = 0; edge < 4 && inside; ++edge) {
            i32 start = winding > 0.0f ? edge : 3 - edge;
            i32 end = winding > 0.0f ? (edge + 1) & 3 : (6 - edge) & 3;
            NUVEC4 normal = {v[end].y - v[start].y, -(v[end].x - v[start].x), 0.0f, 0.0f};
            NuVecNorm(reinterpret_cast<NUVEC *>(&normal), reinterpret_cast<NUVEC *>(&normal));
            normal.z = normal.w = 0.0f;
            for (u32 corner = 0; corner < 8; ++corner) {
                float distance =
                    (corners[corner].x - v[start].x) * normal.x + (corners[corner].y - v[start].y) * normal.y + 0.0f;
                if (distance < 0.01f) {
                    inside = false;
                    break;
                }
            }
        }
        if (inside)
            return true;
    }
    return false;
}

void OccluderSet::PrepareForQueries(NUMTX const *query, NUMTX const *projection) {
    projection_matrix = *projection;
    query_matrix = *query;
    for (u32 index = 0; index < count; ++index) {
        PC_OCCLUDER_RECORD &record = occluders[index];
        record.min_x = record.min_y = record.min_depth = 1000000.0f;
        record.max_x = record.max_y = record.depth = -1000000.0f;
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
            record.min_x = v.x < record.min_x ? v.x : record.min_x;
            record.max_x = v.x > record.max_x ? v.x : record.max_x;
            record.min_y = v.y < record.min_y ? v.y : record.min_y;
            record.max_y = v.y > record.max_y ? v.y : record.max_y;
            record.min_depth = v.w < record.min_depth ? v.w : record.min_depth;
            record.depth = v.w > record.depth ? v.w : record.depth;
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
    if (!current_set->queries_prepared) {
        NUMTX *projection = NuCameraGetVPMtx();
        NUMTX *view = NuCameraGetViewMtx();
        current_set->PrepareForQueries(view, projection);
    }
    ++query_count;
    bool occluded = current_set->IsOccludedOBB(minimum, maximum, matrix);
    if (occluded)
        ++occluded_count;
    return occluded;
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

/* Genuine original occlusion.cpp static members; outside extern C. */
NUMTL *OccluderSet::ms_pZOnlyMtl3D;
NUMTL *OccluderSet::ms_pZOnlyMtl2D;
NUMTL *OccluderSet::ms_pAlphaMtl2D;
