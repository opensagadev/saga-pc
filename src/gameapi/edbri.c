#include "saga.h"

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
