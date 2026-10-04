#include "saga.h"

extern i32 edbri_page_on[8];

void __cdecl edbriBridgesReset(void) {
    i32 i;
    NuBridgeInit();
    for (i = 0; i < 64; ++i) {
        edBridges[i].instance_id = -1;
        edBridges[i].connection_index = 0xff;
    }
    memset(edbri_page_used, 0, sizeof(edbri_page_used));
    memset(edbri_page_scene, 0, sizeof(edbri_page_scene));
    memset(edbri_page_on, 0, sizeof(edbri_page_on));
    edbri_bridges_used = 0;
}

void __cdecl edbriStopPage(i8 page) {
    i32 index;
    if (edbri_page_used[page]) {
        for (index = 0; index < 64; ++index) {
            PC_EDBRIDGE *bridge = &edBridges[index];
            if (bridge->connection_index == (u8)page && bridge->instance_id != -1) {
                NuBridgeRemove(bridge->instance_id);
                bridge->instance_id = -1;
            }
        }
    }
}

void __cdecl edbriClearPage(i8 page) {
    i32 index;
    edbriStopPage(page);
    for (index = 0; index < 64; ++index) {
        if (edBridges[index].connection_index == (u8)page) {
            edBridges[index].connection_index = 0xff;
            --edbri_bridges_used;
        }
    }
    edbri_page_used[page] = 0;
    edbri_page_scene[page] = NULL;
}

void __cdecl edbriDetermineNearest(f32 distance) {
    NUVEC delta;
    i32 index;
    if (edbri_nearest != -1) {
        NuVecSub(&delta, &edbri_cam_pos, &edBridges[edbri_nearest].position);
        if (delta.x * delta.x + delta.y * delta.y + delta.z * delta.z == 0.0f)
            return;
    }
    edbri_nearest = -1;
    for (index = 0; index < 64; ++index) {
        f32 candidate;
        if (edBridges[index].instance_id == -1)
            continue;
        NuVecSub(&delta, &edbri_cam_pos, &edBridges[index].position);
        candidate = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        if (distance < 0.0f || candidate < distance) {
            edbri_nearest = index;
            distance = candidate;
        }
    }
}

i32 __cdecl edbriLoadPage(char *path, void *scene) {
    i32 page = 0;
    i32 version, count, index, i;
    while (page < 8 && edbri_page_used[page])
        ++page;
    if (page == 8)
        return -1;
    EdFileSetMedia(1);
    if (!EdFileOpen(path, 0))
        return -1;
    version = EdFileReadInt();
    if (version >= 2) {
        EdFileClose();
        return -1;
    }
    count = EdFileReadInt();
    if (count + edbri_bridges_used > 64)
        count = 64 - edbri_bridges_used;
    index = 0;
    i = 0;
    while (i < count) {
        PC_EDBRIDGE *bridge;
        char name[20];
        /* Retail tests the record before checking the index limit. */
        while (edBridges[index].connection_index != 0xff && index < 64)
            ++index;
        if (index >= 64) {
            ++i;
            continue;
        }
        bridge = &edBridges[index];
        bridge->instance_id = -1;
        EdFileReadNuVec(&bridge->position);
        bridge->length = EdFileReadFloat();
        bridge->field_14 = EdFileReadFloat();
        bridge->rotation_z = EdFileReadShort();
        bridge->rotation_y = EdFileReadShort();
        bridge->connection_index = (u8)page;
        bridge->field_1d = EdFileReadChar();
        bridge->field_1e = EdFileReadChar();
        EdFileRead(name, sizeof(name));
        bridge->special_20 = name[0] ? edbitsLookupInstance(name, (NUGSCN *)scene) : -1;
        EdFileRead(name, sizeof(name));
        bridge->special_24 = name[0] ? edbitsLookupInstance(name, (NUGSCN *)scene) : -1;
        bridge->field_28 = EdFileReadFloat();
        bridge->field_2c = EdFileReadFloat();
        bridge->field_30 = EdFileReadFloat();
        bridge->field_34 = EdFileReadFloat();
        bridge->field_38 = EdFileReadFloat();
        bridge->field_3c = EdFileReadFloat();
        bridge->red = (u8)EdFileReadChar();
        bridge->green = (u8)EdFileReadChar();
        bridge->blue = (u8)EdFileReadChar();
        bridge->field_43 = (u8)EdFileReadChar();
        ++edbri_bridges_used;
        ++i;
    }
    EdFileClose();
    edbri_nearest = -1;
    edbriDetermineNearest(1.0f);
    edbri_page_used[page] = 1;
    edbri_page_scene[page] = (NUGSCN *)scene;
    return page;
}

STATIC_ASSERT(offsetof(PC_EDBRIDGE, instance_id) == 0);
STATIC_ASSERT(offsetof(PC_EDBRIDGE, position) == 4);
STATIC_ASSERT(offsetof(PC_EDBRIDGE, field_1d) == 0x1d);
STATIC_ASSERT(offsetof(PC_EDBRIDGE, field_1e) == 0x1e);
STATIC_ASSERT(offsetof(PC_EDBRIDGE, red) == 0x40);
STATIC_ASSERT(offsetof(PC_EDBRIDGE, green) == 0x41);
STATIC_ASSERT(offsetof(PC_EDBRIDGE, blue) == 0x42);

void __cdecl edbriBridgeUpdate(i32 index, NUGSCN *scene) {
    NUVEC end, delta;
    NUSPECIALHANDLE first, second;
    u32 colour;
    if (edBridges[index].instance_id != -1) {
        NuBridgeRemove(edBridges[index].instance_id);
        edBridges[index].instance_id = -1;
    }
    delta.x = edBridges[index].length;
    delta.y = 0.0f;
    delta.z = 0.0f;
    NuVecRotateZ(&delta, &delta, edBridges[index].rotation_z);
    NuVecRotateY(&delta, &delta, edBridges[index].rotation_y);
    NuVecAdd(&end, &edBridges[index].position, &delta);
    first.scene = NULL;
    first.special = NULL;
    first.display_special = NULL;
    colour = edBridges[index].red + ((edBridges[index].green << 8) + (edBridges[index].blue << 16) + 0x80000000u);
    if (edBridges[index].special_20 != -1)
        NuGScnGetSpecial(&first, edbits_base_scene, edBridges[index].special_20);
    second.scene = NULL;
    second.special = NULL;
    second.display_special = NULL;
    if (edBridges[index].special_24 != -1)
        NuGScnGetSpecial(&second, edbits_base_scene, edBridges[index].special_24);
    edBridges[index].instance_id =
        NuBridgeCreate(scene, &first, &second, &edBridges[index].position, &end, edBridges[index].field_3c,
                       edBridges[index].rotation_y, edBridges[index].field_28, edBridges[index].field_2c,
                       edBridges[index].field_30, edBridges[index].field_34, edBridges[index].field_1d,
                       edBridges[index].field_14, edBridges[index].field_38, edBridges[index].field_1e, colour);
}

void __cdecl edbriStartPage(i32 page_number) {
    i8 page = (i8)page_number;
    i32 index;
    if (edbri_page_used[page] && edbri_page_scene[page]) {
        for (index = 0; index < 64; ++index) {
            if (edBridges[index].connection_index == (u8)page)
                edbriBridgeUpdate(index, edbri_page_scene[page]);
        }
    }
}
