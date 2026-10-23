#include "saga.h"

i32 __cdecl edbitsLookupInstance(char *name, NUGSCN *scene) {
    i32 index;
    NUSPECIALHANDLE special;
    if (scene != NULL) {
        for (index = 0; index < NuGScnNumSpecials(scene); ++index) {
            NuGScnGetSpecial(&special, scene, index);
            if (NuStrNICmp(NuSpecialGetName(&special), name, 19) == 0)
                return index;
        }
    }
    return -1;
}

void edbitsDoSingleDump(i32 face) {
    char filename[32];
    i32 index;
    for (index = 0; index < 1000; ++index) {
        sprintf(filename, "pictures\\cub%03d_%d.bmp", index, face);
        if (NuFileSize(filename) <= 0)
            break;
        if (index == 999)
            break;
    }
    sprintf(filename, "pictures\\cub%03d_", index);
    NuPs2VideoScreenDump(filename, 1, 1.0f, 1.0f, face, 0, 0);
}

extern i32 edbits_cubecount;
extern i32 NuRndrDoingScreenGrab;

i32 __cdecl edbitsStartCubemapDump(void) {
    if (edbits_cubecount != 0)
        return 0;
    edbits_cubecount = 65;
    NuRndrDoingScreenGrab = 1;
    return 1;
}

void __cdecl edbitsDrawCross(f32 x, f32 y, f32 z, f32 radius, i32 colour, struct PC_SCENE_MTL_VIEW *material) {
    f32 axes[9] = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    NURND_VERTEX3D_PC vertices[2];
    i32 i;
    for (i = 0; i < 9; i += 3) {
        vertices[0].colour = colour;
        vertices[1].colour = colour;
        vertices[0].position.x = x + radius * axes[i];
        vertices[0].position.y = y + radius * axes[i + 1];
        vertices[0].position.z = z + radius * axes[i + 2];
        vertices[1].position.x = x - radius * axes[i];
        vertices[1].position.y = y - radius * axes[i + 1];
        vertices[1].position.z = z - radius * axes[i + 2];
        NuRndrLine3d(vertices, (NUMTL *)material, NULL);
    }
}

void __cdecl edbitsDrawDiagonalCross(f32 x, f32 y, f32 z, f32 radius, i32 colour, struct PC_SCENE_MTL_VIEW *material) {
    f32 axes[12] = {1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f};
    NURND_VERTEX3D_PC vertices[2];
    i32 i;
    for (i = 0; i < 12; i += 3) {
        vertices[0].colour = colour;
        vertices[1].colour = colour;
        vertices[0].position.x = x + radius * axes[i];
        vertices[0].position.y = y + radius * axes[i + 1];
        vertices[0].position.z = z + radius * axes[i + 2];
        vertices[1].position.x = x - radius * axes[i];
        vertices[1].position.y = y - radius * axes[i + 1];
        vertices[1].position.z = z - radius * axes[i + 2];
        NuRndrLine3d(vertices, (NUMTL *)material, NULL);
    }
}

void __cdecl edbitsDrawOvalTilted(NUVEC *centre, f32 radius_x, f32 radius_z, i32 colour,
                                  struct PC_SCENE_MTL_VIEW *material, i32 rotation_z, i32 rotation_y) {
    NUVEC previous;
    NUVEC point;
    i32 i, angle;
    point.x = 0.0f;
    point.y = 0.0f;
    point.z = radius_z;
    if (rotation_z)
        NuVecRotateZ(&point, &point, rotation_z);
    if (rotation_y)
        NuVecRotateY(&point, &point, rotation_y);
    point.x += centre->x;
    point.y += centre->y;
    point.z += centre->z;
    for (i = 1; i <= 10; ++i) {
        previous = point;
        angle = (i << 16) / 10;
        point.x = radius_x * NU_SIN_LUT(angle);
        point.y = 0.0f;
        point.z = radius_z * NU_COS_LUT(angle);
        if (rotation_z)
            NuVecRotateZ(&point, &point, rotation_z);
        if (rotation_y)
            NuVecRotateY(&point, &point, rotation_y);
        point.x += centre->x;
        point.y += centre->y;
        point.z += centre->z;
        NuRndrLine3dDbg(previous.x, previous.y, previous.z, point.x, point.y, point.z, colour);
    }
}

void __cdecl edbitsDrawCircleTilted(NUVEC *centre, f32 radius, i32 colour, struct PC_SCENE_MTL_VIEW *material,
                                    i32 rotation_z, i32 rotation_y) {
    edbitsDrawOvalTilted(centre, radius, radius, colour, material, rotation_z, rotation_y);
}

void __cdecl edbitsDrawOvalXY(NUVEC *centre, f32 radius_x, f32 radius_z, i32 colour,
                              struct PC_SCENE_MTL_VIEW *material) {
    edbitsDrawOvalTilted(centre, radius_x, radius_z, colour, material, 0, 0);
}

void __cdecl edbitsDrawCircleXY(NUVEC *centre, f32 radius, i32 colour, struct PC_SCENE_MTL_VIEW *material) {
    edbitsDrawOvalXY(centre, radius, radius, colour, material);
}

void __cdecl edbitsDrawTorus(NUVEC *centre, f32 radius, f32 radial_extent, f32 vertical_extent, i32 colour,
                             NUMTL *material) {
    NUVEC offset;
    NUVEC point;
    i32 i, angle;
    edbitsDrawCircleXY(centre, radius - radial_extent, colour, (struct PC_SCENE_MTL_VIEW *)material);
    edbitsDrawCircleXY(centre, radius + radial_extent, colour, (struct PC_SCENE_MTL_VIEW *)material);
    offset = *centre;
    offset.y -= vertical_extent;
    edbitsDrawCircleXY(&offset, radius, colour, (struct PC_SCENE_MTL_VIEW *)material);
    offset = *centre;
    offset.y += vertical_extent;
    edbitsDrawCircleXY(&offset, radius, colour, (struct PC_SCENE_MTL_VIEW *)material);
    for (i = 0; i <= 10; ++i) {
        point.x = centre->x;
        point.y = centre->y;
        point.z = centre->z;
        angle = (i << 16) / 10;
        point.x += radius * NU_SIN_LUT(angle);
        point.z += radius * NU_COS_LUT(angle);
        edbitsDrawOvalTilted(&point, vertical_extent, radial_extent, colour, (struct PC_SCENE_MTL_VIEW *)material,
                             0x4000, angle);
    }
}

void __cdecl edbitsDrawSphere(NUVEC *centre, f32 radius, i32 colour, NUMTL *material) {
    struct PC_SCENE_MTL_VIEW *view = (struct PC_SCENE_MTL_VIEW *)material;
    edbitsDrawCircleXY(centre, radius, colour, view);
    edbitsDrawCircleTilted(centre, radius, colour, view, 0x2000, 0);
    edbitsDrawCircleTilted(centre, radius, colour, view, 0x4000, 0);
    edbitsDrawCircleTilted(centre, radius, colour, view, 0x6000, 0);
    edbitsDrawCircleTilted(centre, radius, colour, view, 0x4000, 0x2000);
    edbitsDrawCircleTilted(centre, radius, colour, view, 0x4000, 0x4000);
    edbitsDrawCircleTilted(centre, radius, colour, view, 0x4000, 0x6000);
}

void edgraCalculatePage(i8 page, i32 calculate_vectors) {
    typedef struct Sample {
        NUVEC position;
        f32 scale;
    } Sample;
    Sample samples[256];
    Sample *sample;
    NUMTX *matrix, *matrix_start;
    PC_EDGRACLUMP *clump;
    i32 max_clumps, count, rows, columns, terrain_enabled, slope_enabled;
    u32 seed, angle;
    f32 distance, radius, random, computed_scale, limited, height_offset;
    NUVEC offset, scale, position;
    NUVEC *terrain_values;
    const i32 page_index = page;
    i32 clump_index;
    i32 element;
    if (!edgra_page_used[page_index] || !edgra_page_scene[page_index] || !edgra_page_matrix_stack[page_index] ||
        edgra_page_on[page_index])
        return;
    matrix = edgra_page_matrix_stack[page_index];
    max_clumps = EDGRA_MAX_CLUMPS;
    for (clump_index = 0; clump_index < max_clumps; ++clump_index) {
        clump = &GrassClumps[clump_index];
        if (clump->element_count <= 0 || (i8)clump->page != page)
            continue;
        seed = clump->seed;
        count = clump->element_count;
        height_offset = clump->field_44;
        terrain_enabled = (i8)clump->field_42;
        slope_enabled = (i8)clump->field_43;
        if (clump->kind == 1 || !terrain_enabled)
            slope_enabled = 0;
        for (element = 0; element < count; ++element) {
            sample = &samples[element];
            if (clump->kind == 3) {
                sample->position.x =
                    IndGrassClumps[clump->individual_index * EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP + element].position.x;
                sample->position.y =
                    IndGrassClumps[clump->individual_index * EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP + element].position.y;
                sample->position.z =
                    IndGrassClumps[clump->individual_index * EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP + element].position.z;
            } else {
                switch ((i8)clump->unknown_25) {
                    case 1: {
                        offset.y = 0.0f;
                        angle = NuRandIntSeeded(&seed) & 0xffff;
                        radius = NuRandFloatSeeded(&seed) * clump->size;
                        offset.x = NuTrigTable[(angle >> 1) & 0x7fff] * radius;
                        offset.z = NuTrigTable[((0x4000 + angle) >> 1) & 0x7fff] * radius;
                        distance = NuFsqrt(offset.x * offset.x + offset.z * offset.z);
                        sample->scale = distance;
                        offset.x *= distance + 1.5;
                        offset.z *= distance + 1.5;
                        break;
                    }
                    case 2: {
                        offset.y = 0.0f;
                        angle = NuRandIntSeeded(&seed) & 0xffff;
                        radius = NuRandFloatSeeded(&seed) * clump->size;
                        offset.x = radius * NuTrigTable[(angle >> 1) & 0x7fff];
                        offset.z = radius * NuTrigTable[((0x4000 + angle) >> 1) & 0x7fff];
                        distance = NuFsqrt(offset.x * offset.x + offset.z * offset.z);
                        sample->scale = distance;
                        break;
                    }
                    case 3:
                        offset.y = 0.0f;
                        offset.x = (NuRandFloatSeeded(&seed) * 2.0f - 1.0) * clump->size;
                        offset.z = (NuRandFloatSeeded(&seed) * 2.0f - 1.0) * clump->size;
                        distance = NuFsqrt(offset.x * offset.x + offset.z * offset.z);
                        sample->scale = distance;
                        break;
                    case 4: {
                        offset.y = 0.0f;
                        rows = (i32)NuFsqrt((f32)count);
                        columns = (count - 1 + rows) / rows;
                        offset.x = (f32)(element / columns) * (2.0f * clump->size / (f32)(rows - 1)) - clump->size;
                        offset.z = (f32)(element % columns) * (2.0f * clump->size / (f32)(columns - 1)) - clump->size;
                        distance = NuFsqrt(offset.x * offset.x + offset.z * offset.z);
                        sample->scale = distance;
                        break;
                    }
                    default:
                        goto translate_sample;
                }
                NuVecRotateZ(&offset, &offset, clump->rotation_z);
                NuVecRotateY(&offset, &offset, clump->rotation_y);
                sample->position = offset;
            }
        translate_sample:
            sample->position.x += clump->position.x;
            sample->position.y += clump->position.y;
            sample->position.z += clump->position.z;
            if (clump->kind == 3) {
                sample->scale =
                    (clump->field_30 - clump->field_2c) *
                        IndGrassClumps[clump->individual_index * EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP + element]
                            .field_0c +
                    clump->field_2c;
            } else {
                switch ((i8)clump->unknown_26) {
                    case 1: {
                        random = (f32)(NuRandFloatSeeded(&seed) - 0.25);
                        computed_scale = (f32)(1.25 * sample->scale);
                        computed_scale = (f32)(((f64)1.2f - computed_scale) + random * random);
                        computed_scale = (f32)(computed_scale / (f64)1.7625f * clump->field_30);
                        if (computed_scale < clump->field_2c)
                            computed_scale = clump->field_2c;
                        sample->scale = computed_scale;
                        break;
                    }
                    case 2:
                        sample->scale =
                            NuRandFloatSeeded(&seed) * (clump->field_30 - clump->field_2c) + clump->field_2c;
                        break;
                    case 3: {
                        limited = sample->scale < clump->size ? sample->scale : clump->size;
                        sample->scale = clump->field_30 - (limited / clump->size) * (clump->field_30 - clump->field_2c);
                        break;
                    }
                    case 4: {
                        limited = sample->scale < clump->size ? sample->scale : clump->size;
                        angle = (i32)((limited / clump->size) * 16384.0);
                        sample->scale =
                            (clump->field_30 - clump->field_2c) * NuTrigTable[((0x4000 - angle) >> 1) & 0x7fff] +
                            clump->field_2c;
                        break;
                    }
                }
            }
        }
        seed = clump->seed;
        matrix_start = matrix;
        terrain_values = (NUVEC *)(clump->vector_buffer);
        for (element = 0; element < count; ++element) {
            sample = &samples[element];
            if (clump->kind == 3) {
                if (terrain_enabled && calculate_vectors)
                    terrain_values[element] = NuFadeObjGetAngleTerrainValues(&sample->position);
                NuMtxSetIdentity(matrix);
                NuMtxRotateZ(
                    matrix,
                    IndGrassClumps[clump->individual_index * EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP + element].field_10);
                NuMtxRotateY(
                    matrix,
                    IndGrassClumps[clump->individual_index * EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP + element].field_12);
                if (slope_enabled) {
                    NuMtxRotateZ(matrix, (i32)(terrain_values[element].z));
                    NuMtxRotateX(matrix, (i32)(terrain_values[element].x));
                }
                scale.x = scale.y = scale.z = sample->scale;
                NuMtxScale(matrix, &scale);
                position = sample->position;
                if (terrain_enabled)
                    position.y = height_offset + terrain_values[element].y;
                NuMtxTranslate(matrix, &position);
            } else {
                if (terrain_enabled && calculate_vectors)
                    terrain_values[element] = NuFadeObjGetAngleTerrainValues(&sample->position);
                NuMtxSetIdentity(matrix);
                NuMtxPreRotateY(matrix, (u16)(NuRandIntSeeded(&seed)));
                if (slope_enabled) {
                    NuMtxRotateZ(matrix, (i32)(terrain_values[element].z));
                    NuMtxRotateX(matrix, (i32)(terrain_values[element].x));
                }
                scale.x = scale.y = scale.z = sample->scale;
                NuMtxScale(matrix, &scale);
                position = sample->position;
                if (terrain_enabled)
                    position.y = height_offset + terrain_values[element].y;
                NuMtxTranslate(matrix, &position);
                if (clump->kind == 1)
                    matrix->m33 = clump->field_18 * sample->scale;
            }
            ++matrix;
        }
        clump->matrices = matrix_start;
        max_clumps = EDGRA_MAX_CLUMPS;
    }
}

void edgracbSetInstanceType(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edgra_instance_type = item->data;
}

void edgracbToggleClumpReactive(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edgra_nearest != -1)
        GrassClumps[edgra_nearest].flags = item->flags & EDUI_ITEM_HIGHLIGHTED;
}

void edgracbCancelInstanceMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edgra_instance_menu);
    edgra_instance_menu = NULL;
}

void edgracbCancelClumpAreaMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edgra_clumparea_menu);
    edgra_clumparea_menu = NULL;
}

static void edgracbCancelClumpDistMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edgra_clumpdist_menu);
    edgra_clumpdist_menu = NULL;
}

void edgracbCancelClumpFadeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edgra_clumpfade_menu);
    edgra_clumpfade_menu = NULL;
    fadeoutitem = NULL;
    fadeinitem = NULL;
}

static void edgracbCancelClumpTerrainMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edgra_clumpterrain_menu);
    edgra_clumpterrain_menu = NULL;
}

void edgracbCancelClumpPropertiesMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edgra_clumpproperties_menu);
    edgra_clumpproperties_menu = NULL;
}

void edgracbCancelClumpModeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edgra_clumpmode_menu);
    edgra_clumpmode_menu = NULL;
}

void edgracbSetDpadMode(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edgra_dpadmode = item->data;
}

void edgracbCancelDpadModeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edgra_dpadmode_menu);
    edgra_dpadmode_menu = NULL;
}
void __cdecl edgracbInstanceMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    i32 selected_found;
    i32 added;
    i32 count;
    char *name;
    i32 i;
    edgra_instance_menu =
        eduiMenuCreate(70, 70, 220, 250, ed_fnt, edgracbCancelInstanceMenu,
                       (char *)(edgra_filter ? "Base Instance Select (Filtered)" : "Base Instance Select"));
    if (!edgra_instance_menu || !edbits_base_scene)
        return;
    selected_found = 0;
    added = 1;
    count = NuSpecialGetNumSpecials(edbits_base_scene);
    name = NULL;
    for (i = 0; i < count; ++i) {
        i32 selected;
        i32 keep = 0;
        nuhspecial_s special;
        NuGScnGetSpecial(&special, edbits_base_scene, i);
        if (NuSpecialExistsFn(&special))
            name = NuSpecialGetName(&special);
        if (edgra_instance_type == i) {
            selected = 1;
            keep = 1;
        } else {
            selected = 0;
        }
        if (edgra_filter && NuStrNCmp(edgra_filter_string, name, NuStrLen(edgra_filter_string)) && !keep)
            continue;
        eduiMenuAddItem(edgra_instance_menu,
                        eduiItemCheckCreate(i, edblack, selected, 1, edgracbSetInstanceType, name));
        ++added;
        if (selected == 1) {
            edgra_instance_menu->selected = edui_last_item;
            selected_found = 1;
        }
    }
    if (added == 1)
        eduiMenuAddItem(edgra_instance_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "All Instances Filtered"));
    eduiMenuAddItemFirst(edgra_instance_menu, eduiItemCheckCreate((size_t)(-1), edblack, edgra_instance_type == -1, 1,
                                                                  edgracbSetInstanceType, "None"));
    eduiMenuAttach(parent, edgra_instance_menu);
    edgra_instance_menu->x = parent->x + 10;
    edgra_instance_menu->y = parent->y + 40;
    if (selected_found)
        edgra_instance_menu->field_0c = edgra_instance_menu->selected;
}
void edgracbDpadModeMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    edgra_dpadmode_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edgracbCancelDpadModeMenu, "Dpad Mode");
    if (edgra_dpadmode_menu) {
        eduiMenuAddItem(edgra_dpadmode_menu,
                        eduiItemCheckCreate(0, colours, edgra_dpadmode == 0, 1, edgracbSetDpadMode, "Clump Size"));
        eduiMenuAddItem(edgra_dpadmode_menu,
                        eduiItemCheckCreate(1, colours, edgra_dpadmode == 1, 1, edgracbSetDpadMode, "Clump Tilt"));
        eduiMenuAttach(parent, edgra_dpadmode_menu);
        edgra_dpadmode_menu->x = parent->x + 10;
        edgra_dpadmode_menu->y = parent->y + 40;
    }
}
