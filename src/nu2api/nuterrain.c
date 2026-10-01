#include "saga.h"

struct TERRAIN_POLYGON_INFO_PC {
    f32 min_x, max_x, min_y, max_y, min_z, max_z;
    NUVEC vertices[4];
    NUVEC normals[2];
    union {
        u32 packed_metadata;
        struct {
            u8 material[2];
            u8 unknown_62[2];
        };
    };
};
STATIC_ASSERT(sizeof(TERRAIN_POLYGON_INFO_PC) == 0x64);
STATIC_ASSERT(offsetof(TERRAIN_POLYGON_INFO_PC, min_y) == 8);
STATIC_ASSERT(offsetof(TERRAIN_POLYGON_INFO_PC, max_y) == 0xc);
STATIC_ASSERT(offsetof(TERRAIN_POLYGON_INFO_PC, vertices) == 0x18);
STATIC_ASSERT(offsetof(TERRAIN_POLYGON_INFO_PC, normals) == 0x48);
STATIC_ASSERT(offsetof(TERRAIN_POLYGON_INFO_PC, material) == 0x60);

typedef struct TERRAIN_SKIN_STREAM_VIEW_PC TERRAIN_SKIN_STREAM_VIEW_PC;
typedef struct TERRAIN_XZ_BOUNDS_VIEW_PC {
    f32 x;
    u8 unknown_04[4];
    f32 z;
} TERRAIN_XZ_BOUNDS_VIEW_PC;

typedef struct TERRAIN_GROUP_PC {
    NUVEC origin;
    TERRAIN_SKIN_STREAM_VIEW_PC *data;
    TERRAIN_XZ_BOUNDS_VIEW_PC bounds_min;
    TERRAIN_XZ_BOUNDS_VIEW_PC bounds_max;
    u8 unknown_28[4];
    i32 chunk_type;
    i16 scene_index;
    u8 unknown_32[2];
    f32 radius;
} TERRAIN_GROUP_PC;

typedef struct TERRAIN_PLATFORM_PC {
    u8 unknown_00[0x40];
    void *scene_object;
    u32 unknown_44;
    i16 terrain_group_index;
    i16 scene_object_index;
    u32 flags;
    u8 unknown_50[4];
    f32 bounce_54;
    f32 bounce_58;
    f32 bounce_5c;
    f32 bounce_60;
    f32 bounce_64;
    void *scene_transform;
} TERRAIN_PLATFORM_PC;

typedef struct TERRAIN_TRACK_SLOT_PC {
    void *id;
    i16 platform_index;
    u8 unknown_06[0x12];
} TERRAIN_TRACK_SLOT_PC;

typedef struct TERRAIN_CELL_PC {
    i16 first_group;
    i16 group_count;
    u8 unknown_04[0x10];
} TERRAIN_CELL_PC;

typedef struct TERRAIN_SET_PC {
    TERRAIN_GROUP_PC *groups;
    u8 unknown_004[0x64];
    TERRAIN_PLATFORM_PC *platforms;
    u8 unknown_06c[0xe0];
    i16 group_count;
    u8 unknown_14e[2];
    i32 group_index_count;
    u8 unknown_154[0x44];
    TERRAIN_TRACK_SLOT_PC track_slots[64];
    TERRAIN_CELL_PC cells[256];
    TERRAIN_CELL_PC platform_cell;
    u8 unknown_1bac[8];
    i16 *group_indices;
    u8 unknown_1bb8[0x88cc];
    i32 max_group_indices;
    i32 max_groups;
    i32 max_platforms;
} TERRAIN_SET_PC;

STATIC_ASSERT(sizeof(TERRAIN_GROUP_PC) == 0x38);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, data) == 0xc);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, bounds_min) == 0x10);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, bounds_max) == 0x1c);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, chunk_type) == 0x2c);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, scene_index) == 0x30);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, radius) == 0x34);
STATIC_ASSERT(sizeof(TERRAIN_PLATFORM_PC) == 0x6c);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, scene_object) == 0x40);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, terrain_group_index) == 0x48);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, scene_object_index) == 0x4a);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, flags) == 0x4c);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, bounce_54) == 0x54);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, scene_transform) == 0x68);
STATIC_ASSERT(sizeof(TERRAIN_TRACK_SLOT_PC) == 0x18);
STATIC_ASSERT(offsetof(TERRAIN_TRACK_SLOT_PC, platform_index) == 0x4);
STATIC_ASSERT(sizeof(TERRAIN_CELL_PC) == 0x14);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, platforms) == 0x68);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, group_count) == 0x14c);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, group_index_count) == 0x150);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, track_slots) == 0x198);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, cells) == 0x798);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, platform_cell) == 0x1b98);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, group_indices) == 0x1bb4);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, max_group_indices) == 0xa484);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, max_groups) == 0xa488);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, max_platforms) == 0xa48c);
STATIC_ASSERT(sizeof(TERRAIN_SET_PC) == 0xa490);

void __cdecl TerrainSetCur(void *terrain) {
    CurrentTerrain = terrain;
}

void *__cdecl TerrainGetCur(void) {
    return CurrentTerrain;
}

i32 __cdecl PlatInstGetHit(i32 index) {
    TERRAIN_SET_PC *terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    if (terrain == NULL || index < 0 || index >= terrain->max_platforms)
        return 0;
    return (terrain->platforms[index].flags >> 1) & 1;
}

void __cdecl ScanTerrIDRemovePlat(i32 platform_index) {
    TERRAIN_SET_PC *terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    i32 i;

    for (i = 0; i < 64; i++) {
        if (terrain->track_slots[i].platform_index == platform_index)
            terrain->track_slots[i].id = NULL;
    }
}

i32 __cdecl NewPlatInst(void *object, i32 instance) {
    TERRAIN_SET_PC *terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    TERRAIN_PLATFORM_PC *source_platform;
    TERRAIN_PLATFORM_PC *new_platform;
    TERRAIN_GROUP_PC *new_group;
    i32 new_index;
    i32 source_index;
    i32 i;

    if (terrain == NULL)
        return -1;
    if (terrain->group_index_count >= terrain->max_group_indices)
        return -1;
    new_index = terrain->group_count;
    if (new_index >= terrain->max_groups || object == NULL)
        return -1;
    if (terrain->max_platforms <= 0)
        return -1;

    new_index = -1;
    for (i = 0; i < terrain->max_platforms; i++) {
        if (terrain->platforms[i].scene_object == NULL) {
            new_index = i;
            break;
        }
    }
    if (new_index < 0)
        return -1;

    source_index = -1;
    for (i = 0; i < terrain->max_platforms; i++) {
        if (terrain->platforms[i].scene_object != NULL && terrain->platforms[i].scene_object_index == instance) {
            source_index = i;
            break;
        }
    }
    if (source_index < 0)
        return -1;

    source_platform = &terrain->platforms[source_index];
    new_group = &terrain->groups[terrain->group_count];
    *new_group = terrain->groups[source_platform->terrain_group_index];
    new_group->scene_index = (i16)new_index;
    new_group->chunk_type = 1;

    new_platform = &terrain->platforms[new_index];
    new_platform->scene_object = object;
    new_platform->terrain_group_index = terrain->group_count;
    new_platform->scene_object_index = (i16)instance;
    new_platform->scene_transform = NULL;
    new_platform->flags = (new_platform->flags & ~1u) | (source_platform->flags & 1);
    new_platform->bounce_54 = 0.0f;
    new_platform->bounce_58 = 0.0f;
    new_platform->bounce_5c = 0.0f;
    new_platform->bounce_60 = 0.0f;
    new_platform->bounce_64 = 0.0f;

    terrain->group_indices[terrain->group_index_count] = terrain->group_count;
    terrain->group_index_count++;
    terrain->group_count++;
    terrain->platform_cell.group_count++;
    return new_index;
}

i32 __cdecl DeletePlatinst(i32 platform_index) {
    TERRAIN_SET_PC *terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    TERRAIN_PLATFORM_PC *platform;
    TERRAIN_GROUP_PC *last_group;
    i32 last_group_index;
    i32 group_index;
    i32 cell_count;
    i32 cell_group_index;
    i32 i;

    if (platform_index < 0 || platform_index >= terrain->max_platforms)
        return 0;
    platform = &terrain->platforms[platform_index];
    if (platform->scene_object == NULL)
        return 0;

    terrain->group_count--;
    terrain->group_index_count--;
    last_group_index = terrain->group_count;
    last_group = &terrain->groups[last_group_index];
    group_index = platform->terrain_group_index;
    terrain->platforms[last_group->scene_index].terrain_group_index = (i16)group_index;
    terrain->groups[group_index] = *last_group;
    last_group->chunk_type = -1;
    platform->scene_object = NULL;

    cell_count = terrain->platform_cell.group_count;
    cell_group_index = terrain->platform_cell.first_group;
    for (i = 0; i < cell_count; i++, cell_group_index++) {
        if (terrain->group_indices[cell_group_index] == last_group_index) {
            terrain->group_indices[cell_group_index] = terrain->group_indices[terrain->group_index_count];
            break;
        }
    }
    terrain->platform_cell.group_count--;
    ScanTerrIDRemovePlat(platform_index);
    return 1;
}

i32 __cdecl ShadowInfo(void) {
    return ShadPoly != NULL ? ShadPoly->material[0] : -1;
}

i32 __cdecl EShadowInfo(void) {
    return EShadowPolygonPc != NULL ? EShadowPolygonPc->material[1] : -1;
}

i32 __cdecl ShadowRoofInfo(void) {
    return ShadowRoofPolygonPc != NULL ? ShadowRoofPolygonPc->material[0] : -1;
}

i32 __cdecl NewShadowOnPlatform(void) {
    TERRAIN_SET_PC *terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    TERRAIN_GROUP_PC *group;
    if (terrain == NULL || TerrainShadowGroupIndexPc == -1)
        return -1;
    group = &terrain->groups[TerrainShadowGroupIndexPc];
    if (group->chunk_type != 1)
        return -1;
    return group->scene_index;
}

typedef struct TERRAIN_SKIN_BLEND_VIEW_PC {
    NUMTX end_matrix;    /* +00 */
    NUVEC row_amplitude; /* +40 */
    u8 opaque4c_50[4];
    NUVEC translation_amplitude; /* +50 */
} TERRAIN_SKIN_BLEND_VIEW_PC;

typedef struct TERRAIN_SKIN_INFO_VIEW_PC {
    NUMTX *matrix;                           /* +00 */
    const TERRAIN_SKIN_BLEND_VIEW_PC *blend; /* +04 */
    const NUVEC4 *sample_rows;               /* +08: sampled row banks */
    f32 scale;                               /* +0c */
    u8 opaque10_18[8];
    i16 mode;         /* +18 */
    i16 vertex_order; /* +1a */
} TERRAIN_SKIN_INFO_VIEW_PC;

typedef struct TERRAIN_SKIN_BATCH_VIEW_PC {
    i16 kind;
    i16 count;
    f32 min_x, max_x, min_z, max_z;
} TERRAIN_SKIN_BATCH_VIEW_PC;

typedef TERRAIN_POLYGON_INFO_PC TERRAIN_SKIN_POLYGON_VIEW_PC;

/* Raw records remain variable-length. These named views expose the observed
 * 20-byte header followed by count 100-byte polygons; they are not an owner. */
struct TERRAIN_SKIN_STREAM_VIEW_PC {
    TERRAIN_SKIN_BATCH_VIEW_PC header;
    TERRAIN_SKIN_POLYGON_VIEW_PC polygons[1];
};

extern const f64 TerrainSkinSampleCountPc;
extern const f64 TerrainSkinSampleIndexScalePc;
extern const f32 TerrainSkinBoundsMaxPc;
extern const f32 TerrainSkinBoundsMinPc;

/* Explicit output-pointer interface preserves the verified PC five-word ABI.
 * Android's aggregate return is a source candidate, not a historical PC type. */
NUVEC *TerrainSkin(NUVEC *result, const TERRAIN_SKIN_INFO_VIEW_PC *info, const NUVEC *position, f32 weight, i32 mode) {
    NUVEC4 point;
    NUMTX matrix;
    if (mode == 1) {
        point.x = position->x;
        point.y = position->y;
        point.z = position->z;
        point.w = 1.0f;
        NuVec4MtxTransformVU0(&point, &point, info->matrix);
    } else {
        f64 t = weight;
        if (t < 0.0)
            t = (f32)(position->z / info->scale);
        if (t > 1.0)
            t = 1.0;
        if (t < 0.0)
            t = 0.0;
        point.x = position->x;
        point.y = position->y;
        point.w = 1.0f;
        if (info->sample_rows != NULL) {
            i32 sample;
            t = (f32)((i32)(t * TerrainSkinSampleCountPc + 0.5) * 0.125);
            point.z = (f32)(position->z - info->scale * t);
            sample = (i32)(t * TerrainSkinSampleIndexScalePc);
            /* Original computes negative sample; -sample is0..8 for the
             * qualified quantized finite0..1 weights. Four banks stride11. */
            matrix.m00 = info->sample_rows[-sample].x;
            matrix.m01 = info->sample_rows[-sample].y;
            matrix.m02 = info->sample_rows[-sample].z;
            matrix.m03 = info->sample_rows[-sample].w;
            matrix.m10 = info->sample_rows[11 - sample].x;
            matrix.m11 = info->sample_rows[11 - sample].y;
            matrix.m12 = info->sample_rows[11 - sample].z;
            matrix.m13 = info->sample_rows[11 - sample].w;
            matrix.m20 = info->sample_rows[22 - sample].x;
            matrix.m21 = info->sample_rows[22 - sample].y;
            matrix.m22 = info->sample_rows[22 - sample].z;
            matrix.m23 = info->sample_rows[22 - sample].w;
            matrix.m30 = info->sample_rows[33 - sample].x;
            matrix.m31 = info->sample_rows[33 - sample].y;
            matrix.m32 = info->sample_rows[33 - sample].z;
            matrix.m33 = info->sample_rows[33 - sample].w;
        } else {
            const NUMTX *start = info->matrix;
            const TERRAIN_SKIN_BLEND_VIEW_PC *blend = info->blend;
            f64 inverse, wave, sx, sy, sz;
            t = (f32)((i32)(t * 10.0 + 0.5) / 10.0);
            point.z = (f32)(position->z - info->scale * t);
            inverse = 1.0 - t;
            wave = NuTrigTable[((i32)(t * 32768.0) >> 1) & 0x7fff];
            sx = 1.0 + blend->row_amplitude.x * wave;
            sy = 1.0 + blend->row_amplitude.y * wave;
            sz = 1.0 + blend->row_amplitude.z * wave;
#define SKIN_ROW(MEMBER, AMPLITUDE)                                                                                    \
    matrix.MEMBER = (f32)((start->MEMBER * inverse + blend->end_matrix.MEMBER * t) * (AMPLITUDE))
            SKIN_ROW(m00, sx);
            SKIN_ROW(m01, sx);
            SKIN_ROW(m02, sx);
            SKIN_ROW(m03, sx);
            SKIN_ROW(m10, sy);
            SKIN_ROW(m11, sy);
            SKIN_ROW(m12, sy);
            SKIN_ROW(m13, sy);
            SKIN_ROW(m20, sz);
            SKIN_ROW(m21, sz);
            SKIN_ROW(m22, sz);
            SKIN_ROW(m23, sz);
#undef SKIN_ROW
            matrix.m30 =
                (f32)(start->m30 * inverse + blend->end_matrix.m30 * t + blend->translation_amplitude.x * wave);
            matrix.m31 =
                (f32)(start->m31 * inverse + blend->end_matrix.m31 * t + blend->translation_amplitude.y * wave);
            matrix.m32 =
                (f32)(start->m32 * inverse + blend->end_matrix.m32 * t + blend->translation_amplitude.z * wave);
            matrix.m33 = (f32)(start->m33 * inverse + blend->end_matrix.m33 * t);
        }
        NuVec4MtxTransformVU0(&point, &point, &matrix);
    }
    result->x = point.x;
    result->y = point.y;
    result->z = point.z;
    return result;
}

#define TERRAIN_SKIN_NORMAL(normal, a, b)                                                                              \
    do {                                                                                                               \
        f32 length, inverse;                                                                                           \
        (normal)->x = (f32)((f64)(a)->y * (b)->z - (f64)(a)->z * (b)->y);                                              \
        (normal)->y = (f32)((f64)(a)->z * (b)->x - (f64)(a)->x * (b)->z);                                              \
        (normal)->z = (f32)((f64)(a)->x * (b)->y - (f64)(a)->y * (b)->x);                                              \
        length = NuFsqrt(                                                                                              \
            (f32)((f64)(normal)->x * (normal)->x + (f64)(normal)->y * (normal)->y + (f64)(normal)->z * (normal)->z));  \
        inverse = length != 0.0f ? (f32)(1.0 / length) : 0.0f;                                                         \
        (normal)->x = (f32)((f64)(normal)->x * inverse);                                                               \
        (normal)->y = (f32)((f64)(normal)->y * inverse);                                                               \
        (normal)->z = (f32)((f64)(normal)->z * inverse);                                                               \
    } while (0)

/* Valid initialized variable records, vertex_order 0 or 1, and enough output
 * storage for each complete 20-byte header and 100-byte polygon. */
void SkinPlatform(TERRAIN_GROUP_PC *group, TERRAIN_SKIN_STREAM_VIEW_PC *output, const TERRAIN_SKIN_INFO_VIEW_PC *info) {
    TERRAIN_SKIN_STREAM_VIEW_PC *input;
    TERRAIN_SKIN_STREAM_VIEW_PC *output_base = output;
    if (CurrentTerrain == NULL || (group->chunk_type != 0 && group->chunk_type != 1))
        return;
    input = group->data;
    while (input->header.kind >= 0) {
        TERRAIN_SKIN_BATCH_VIEW_PC *header = &output->header;
        TERRAIN_SKIN_POLYGON_VIEW_PC *destination = output->polygons;
        const TERRAIN_SKIN_POLYGON_VIEW_PC *source = input->polygons;
        f32 min_x = TerrainSkinBoundsMaxPc, max_x = TerrainSkinBoundsMinPc;
        f32 min_z = TerrainSkinBoundsMaxPc, max_z = TerrainSkinBoundsMinPc;
        i32 count = input->header.count;
        header->kind = input->header.kind;
        header->count = input->header.count;
        while (count > 0) {
            TERRAIN_SKIN_POLYGON_VIEW_PC *polygon = destination;
            f32 pmin_x = TerrainSkinBoundsMaxPc, pmax_x = TerrainSkinBoundsMinPc;
            f32 pmin_y = TerrainSkinBoundsMaxPc, pmax_y = TerrainSkinBoundsMinPc;
            f32 pmin_z = TerrainSkinBoundsMaxPc, pmax_z = TerrainSkinBoundsMinPc;
            i32 last = source->normals[1].y > BoltTargetRandomDenominator ? 2 : 3;
            i32 vertex;
            *polygon = *source; /* Original REP MOVSD copies25DWORDs. */
            for (vertex = last; vertex >= 0; --vertex) {
                NUVEC transformed;
                i32 selected = SkinFlipTab[vertex + info->vertex_order * 4];
                const NUVEC *position = &source->vertices[selected];
                NUVEC *p = &polygon->vertices[vertex];
                TerrainSkin(&transformed, info, position, -1.0f, info->mode);
                p->x = transformed.x;
                p->y = transformed.y;
                p->z = transformed.z;
                p->x = (f32)(p->x - info->matrix->m30);
                p->y = (f32)(p->y - info->matrix->m31);
                p->z = (f32)(p->z - info->matrix->m32);
#define SKIN_BOUNDS(VALUE, LO, HI)                                                                                     \
    if ((VALUE) < (LO))                                                                                                \
        (LO) = (VALUE);                                                                                                \
    if ((VALUE) > (HI))                                                                                                \
    (HI) = (VALUE)
                SKIN_BOUNDS(p->x, pmin_x, pmax_x);
                SKIN_BOUNDS(p->y, pmin_y, pmax_y);
                SKIN_BOUNDS(p->z, pmin_z, pmax_z);
                SKIN_BOUNDS(p->x, min_x, max_x);
                SKIN_BOUNDS(p->z, min_z, max_z);
#undef SKIN_BOUNDS
            }
            polygon->min_x = pmin_x - NuTimeBarTop;
            polygon->max_x = pmax_x + NuTimeBarTop;
            polygon->min_y = pmin_y - NuTimeBarTop;
            polygon->max_y = pmax_y + NuTimeBarTop;
            polygon->min_z = pmin_z - NuTimeBarTop;
            polygon->max_z = pmax_z + NuTimeBarTop;
            {
                NUVEC a, b;
                if (source->normals[1].y < BoltTargetRandomDenominator) {
                    a.x = polygon->vertices[1].x - polygon->vertices[3].x;
                    a.y = polygon->vertices[1].y - polygon->vertices[3].y;
                    a.z = polygon->vertices[1].z - polygon->vertices[3].z;
                    b.x = polygon->vertices[2].x - polygon->vertices[3].x;
                    b.y = polygon->vertices[2].y - polygon->vertices[3].y;
                    b.z = polygon->vertices[2].z - polygon->vertices[3].z;
                    TERRAIN_SKIN_NORMAL(&polygon->normals[1], &a, &b);
                }
                a.x = polygon->vertices[2].x - polygon->vertices[0].x;
                a.y = polygon->vertices[2].y - polygon->vertices[0].y;
                a.z = polygon->vertices[2].z - polygon->vertices[0].z;
                b.x = polygon->vertices[1].x - polygon->vertices[0].x;
                b.y = polygon->vertices[1].y - polygon->vertices[0].y;
                b.z = polygon->vertices[1].z - polygon->vertices[0].z;
                TERRAIN_SKIN_NORMAL(&polygon->normals[0], &a, &b);
            }
            ++source;
            ++destination;
            --count;
        }
        header->min_x = min_x - NuTimeBarTop;
        header->max_x = max_x + NuTimeBarTop;
        header->min_z = min_z - NuTimeBarTop;
        header->max_z = max_z + NuTimeBarTop;
        /* Advance to the next verified variable record after its polygons. */
        input = (TERRAIN_SKIN_STREAM_VIEW_PC *)source;
        output = (TERRAIN_SKIN_STREAM_VIEW_PC *)destination;
    }
    output->header.kind = -1;
    output->header.count = -1;
    group->data = output_base; /* original arg2, not terminal cursor */
}

#undef TERRAIN_SKIN_NORMAL

STATIC_ASSERT(sizeof(TERRAIN_SKIN_BATCH_VIEW_PC) == 0x14);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_STREAM_VIEW_PC, polygons) == 0x14);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_INFO_VIEW_PC, matrix) == 0);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_INFO_VIEW_PC, blend) == 4);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_INFO_VIEW_PC, sample_rows) == 8);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_INFO_VIEW_PC, scale) == 0xc);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_INFO_VIEW_PC, mode) == 0x18);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_INFO_VIEW_PC, vertex_order) == 0x1a);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_BLEND_VIEW_PC, row_amplitude) == 0x40);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_BLEND_VIEW_PC, translation_amplitude) == 0x50);
