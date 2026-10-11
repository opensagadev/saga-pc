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

static void edgracbInstanceMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
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
        nuhspecial_s special;
        NuGScnGetSpecial(&special, edbits_base_scene, i);
        if (NuSpecialExistsFn(&special))
            name = NuSpecialGetName(&special);
        selected = i == edgra_instance_type;
        if (edgra_filter && NuStrNCmp(edgra_filter_string, name, NuStrLen(edgra_filter_string))) {
            if (!selected)
                continue;
            eduiMenuAddItem(edgra_instance_menu,
                            eduiItemCheckCreate(i, edblack, selected, 1, edgracbSetInstanceType, name));
            ++added;
        } else {
            eduiMenuAddItem(edgra_instance_menu,
                            eduiItemCheckCreate(i, edblack, selected, 1, edgracbSetInstanceType, name));
            ++added;
        }
        if (selected != 0) {
            selected_found = 1;
            edgra_instance_menu->selected = edui_last_item;
        }
    }
    if (added == 1)
        eduiMenuAddItem(edgra_instance_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "All Instances Filtered"));
    eduiMenuAddItemFirst(edgra_instance_menu, eduiItemCheckCreate((size_t)(-1), edblack, edgra_instance_type == -1, 1,
                                                                  edgracbSetInstanceType, "None"));
    edgraAttachMenu(parent, edgra_instance_menu);
    if (selected_found)
        edgra_instance_menu->field_0c = edgra_instance_menu->selected;
}

static void edgracbDpadModeMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
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
