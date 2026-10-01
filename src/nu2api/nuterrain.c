#include "saga.h"

struct TERRAIN_POLYGON_INFO_PC {
    u8 unknown_00[0x60];
    u8 material[2];
};
STATIC_ASSERT(offsetof(TERRAIN_POLYGON_INFO_PC, material) == 0x60);

typedef struct TERRAIN_GROUP_PC {
    u8 unknown_00[0x2c];
    i32 chunk_type;
    i16 scene_index;
    u8 unknown_32[6];
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
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, chunk_type) == 0x2c);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, scene_index) == 0x30);
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
