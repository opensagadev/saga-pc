#include <stdio.h>
#include "saga.h"

void __cdecl NuWindSetup(VARIPTR *buffer, VARIPTR end, i32 matrix_count, i32 group_count);
void __cdecl NuFadeObjSetup(VARIPTR *buffer, VARIPTR end, i32 matrix_count, i32 group_count);

void __cdecl edgraClumpsReset(void) {
    i32 i;
    for (i = 0; i < EDGRA_MAX_CLUMPS; ++i)
        GrassClumps[i].element_count = 0;
    memset(edgra_page_used, 0, sizeof(edgra_page_used));
    memset(edgra_page_scene, 0, sizeof(edgra_page_scene));
    memset(edgra_page_on, 0, sizeof(edgra_page_on));
    edgra_clumps_used = 0;
    for (i = 0; i < EDGRA_MAX_INDIVIDUAL_CLUMPS; ++i)
        IndGrassClumpsUsed[i] = 0;
    edgra_ind_clumps_used = 0;
}

void __cdecl edgraSetup(VARIPTR *buffer, VARIPTR end, i32 clumps, i32 individual_clumps, i32 units_per_clump) {
    EDGRA_MAX_CLUMPS = clumps;
    EDGRA_MAX_INDIVIDUAL_CLUMPS = individual_clumps;
    EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP = units_per_clump;
    if (units_per_clump == 0)
        EDGRA_MAX_INDIVIDUAL_CLUMPS = 0;
    buffer->addr = (buffer->addr + 15) & ~15u;
    GrassClumps = (PC_EDGRACLUMP *)buffer->void_ptr;
    buffer->addr += EDGRA_MAX_CLUMPS * sizeof(PC_EDGRACLUMP);
    IndGrassClumps = (PC_EDGRAINDIVIDUAL *)buffer->void_ptr;
    buffer->addr += EDGRA_MAX_INDIVIDUAL_CLUMPS * EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP * sizeof(PC_EDGRAINDIVIDUAL);
    IndGrassClumpsUsed = (i32 *)buffer->void_ptr;
    buffer->addr += EDGRA_MAX_INDIVIDUAL_CLUMPS * sizeof(i32);
    NuWindSetup(buffer, end, 0x3000, clumps);
    NuFadeObjSetup(buffer, end, 0x3000, clumps);
}

void __cdecl edgraStartPage(i8 page) {
    i32 page_index = page;
    i32 i;
    void *saved_terrain;
    if (!edgra_page_used[page_index] || !edgra_page_scene[page_index] || edgra_page_on[page_index])
        return;
    saved_terrain = PCGrassTerrainScene;
    PCGrassTerrainScene = edgra_page_terrain[page_index];
    if (!edgra_page_calculate_done[page_index]) {
        if (edgra_page_vectors_valid[page_index] == 0) {
            edgraCalculatePage(page, 1);
            edgra_page_vectors_valid[page_index] = 1;
        } else {
            edgraCalculatePage(page, 0);
        }
        edgra_page_calculate_done[page_index] = 1;
    }
    for (i = 0; i < EDGRA_MAX_CLUMPS; ++i) {
        PC_EDGRACLUMP *clump = &GrassClumps[i];
        if (clump->element_count && clump->page == (u8)page) {
            i32 count = clump->element_count;
            if (edgra_units_used + count > 0x3000)
                count = 0x3000 - edgra_units_used;
            if (count > 0) {
                NUGSCN *scene = edgra_page_scene[page_index];
                NUSPECIALHANDLE special;
                special.scene = scene;
                if (scene->display_list != NULL) {
                    special.special = NULL;
                    special.display_special = (NUMTX *)&scene->display_list->specials[clump->special_index];
                } else {
                    special.special = (NUMTX *)&scene->specials[clump->special_index];
                    special.display_special = NULL;
                }
                if (clump->kind == 2 || clump->kind == 3) {
                    NuFadeObjCreateMtx(&special, clump->matrices, (i16)count, clump->near_distance, clump->far_distance,
                                       clump->flags);
                } else {
                    NuWindCreateMtx(&special, clump->matrices, (i16)count, clump->field_18, clump->field_20,
                                    clump->flags, clump->near_distance, clump->far_distance);
                }
                edgra_units_used += count;
            }
        }
    }
    edgra_page_on[page_index] = 1;
    PCGrassTerrainScene = saved_terrain;
}

void __cdecl edgraInitAllClumps(void) {
    i32 page;
    edgra_units_used = 0;
    NuWindInit();
    NuFadeObjInit();
    for (page = 0; page < 8; ++page)
        edgra_page_on[page] = 0;
    for (page = 0; page < 8; ++page) {
        if (edgra_page_used[page]) {
            edgra_page_calculate_done[page] = 0;
            edgra_page_vectors_valid[page] = 0;
            edgraStartPage((i8)page);
        }
    }
}

void edgraClumpReseed(i32 index) {
    PC_EDGRACLUMP *clump = &GrassClumps[index];
    clump->seed = NuRand(NULL);
    edgraInitAllClumps();
}
static void edgracbClumpAreaMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    if (edgra_nearest == -1 || !GrassClumps[edgra_nearest].element_count)
        return;
    edgra_clumparea_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edgracbCancelClumpAreaMenu, "Clump Area Type");
    if (!edgra_clumparea_menu)
        return;
    eduiMenuAddItem(edgra_clumparea_menu, eduiItemCheckCreate(1, colours, GrassClumps[edgra_nearest].unknown_25 == 1, 1,
                                                              edgracbSetClumpArea, "Legacy"));
    eduiMenuAddItem(edgra_clumparea_menu, eduiItemCheckCreate(2, colours, GrassClumps[edgra_nearest].unknown_25 == 2, 1,
                                                              edgracbSetClumpArea, "Circle"));
    eduiMenuAddItem(edgra_clumparea_menu, eduiItemCheckCreate(3, colours, GrassClumps[edgra_nearest].unknown_25 == 3, 1,
                                                              edgracbSetClumpArea, "Square"));
    eduiMenuAddItem(edgra_clumparea_menu, eduiItemCheckCreate(4, colours, GrassClumps[edgra_nearest].unknown_25 == 4, 1,
                                                              edgracbSetClumpArea, "Grid"));
    edgraAttachMenu(parent, edgra_clumparea_menu);
}
static void edgracbClumpFadeMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    if (!(edgra_nearest == -1 || !GrassClumps[edgra_nearest].element_count)) {
        edgra_clumpfade_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edgracbCancelClumpFadeMenu, "Clump Fading");
        if (edgra_clumpfade_menu) {
            eduiMenuAddItem(edgra_clumpfade_menu,
                            eduiItemSliderCreate(0, edblack, 0, edgracbSetClumpFadeIn, 0.0f, edgra_superscale * 2.0f,
                                                 GrassClumps[edgra_nearest].near_distance, "Start of Fade"));
            fadeinitem = (edui_slider_s *)(edui_last_item);
            eduiItemSliderSetFmt((edui_slider_s *)(edui_last_item), "(%1.01f)");
            eduiItemSliderSetGranularity((edui_slider_s *)(edui_last_item), 0.1f);
            eduiMenuAddItem(edgra_clumpfade_menu,
                            eduiItemSliderCreate(0, edblack, 0, edgracbSetClumpFadeOut, 0.0f, edgra_superscale * 2.0f,
                                                 GrassClumps[edgra_nearest].far_distance, "End of Fade"));
            fadeoutitem = (edui_slider_s *)(edui_last_item);
            eduiItemSliderSetFmt((edui_slider_s *)(edui_last_item), "(%1.01f)");
            eduiItemSliderSetGranularity((edui_slider_s *)(edui_last_item), 0.1f);
            edgraAttachMenu(parent, edgra_clumpfade_menu);
        }
    }
}
static void edgracbClumpPropertiesMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    if (!(edgra_nearest == -1 || !GrassClumps[edgra_nearest].element_count)) {
        edgra_clumpproperties_menu =
            eduiMenuCreate(70, 70, 220, 250, ed_fnt, edgracbCancelClumpPropertiesMenu, "Clump Properties");
        if (!edgra_clumpproperties_menu)
            return;
        eduiMenuAddItem(edgra_clumpproperties_menu,
                        eduiItemSelCreate(1, colours, 0, 0, edgracbClumpSizesMenu, "Clump Sizes..."));
        if (GrassClumps[edgra_nearest].kind == 3) {
            eduiMenuAddItem(edgra_clumpproperties_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Clump Area Type..."));
            eduiMenuAddItem(edgra_clumpproperties_menu,
                            eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Clump Distribution..."));
        } else {
            eduiMenuAddItem(edgra_clumpproperties_menu,
                            eduiItemSelCreate(1, colours, 0, 0, edgracbClumpAreaMenu, "Clump Area Type..."));
            eduiMenuAddItem(edgra_clumpproperties_menu,
                            eduiItemSelCreate(1, colours, 0, 0, edgracbClumpDistMenu, "Clump Distribution..."));
        }
        eduiMenuAddItem(edgra_clumpproperties_menu,
                        eduiItemSelCreate(1, colours, 0, 0, edgracbClumpFadeMenu, "Clump Fading..."));
        eduiMenuAddItem(edgra_clumpproperties_menu,
                        eduiItemSelCreate(1, colours, 0, 0, edgracbClumpTerrainMenu, "Clump Terraining..."));
        if (GrassClumps[edgra_nearest].kind == 1)
            eduiMenuAddItem(edgra_clumpproperties_menu,
                            eduiItemSliderCreate(0, colours, 0, edgracbSetClumpWind, 0.01f, 1.99f,
                                                 GrassClumps[edgra_nearest].field_18, "Wind"));
        eduiMenuAddItem(edgra_clumpproperties_menu,
                        eduiItemToggleCreate(0, colours, GrassClumps[edgra_nearest].flags, 1,
                                             edgracbToggleClumpReactive, "Collide with Player"));
        edgraAttachMenu(parent, edgra_clumpproperties_menu);
    }
}
static void edgracbClumpModeMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    edgra_clumpmode_menu = eduiMenuCreate(70, 70, 200, 250, ed_fnt, edgracbCancelClumpModeMenu, "Clump Mode");
    if (!edgra_clumpmode_menu)
        return;
    if (edgra_nearest != -1 && GrassClumps[edgra_nearest].kind == 3) {
        eduiMenuAddItem(edgra_clumpmode_menu, eduiItemSelCreate(1, edgrey, edgra_mode == 1, 0, NULL, "Wind Mode"));
        eduiMenuAddItem(edgra_clumpmode_menu,
                        eduiItemSelCreate(2, edgrey, edgra_mode == 2, 0, NULL, "Faded Static Mode"));
    } else {
        eduiMenuAddItem(edgra_clumpmode_menu,
                        eduiItemCheckCreate(1, edblack, edgra_mode == 1, 1, edgracbSetClumpMode, "Wind Mode"));
        eduiMenuAddItem(edgra_clumpmode_menu,
                        eduiItemCheckCreate(2, edblack, edgra_mode == 2, 1, edgracbSetClumpMode, "Faded Static Mode"));
    }
    if (edgra_nearest != -1 && GrassClumps[edgra_nearest].kind != 3)
        eduiMenuAddItem(edgra_clumpmode_menu,
                        eduiItemSelCreate(3, edgrey, edgra_mode == 3, 0, NULL, "Individual FS Mode"));
    else
        eduiMenuAddItem(edgra_clumpmode_menu,
                        eduiItemCheckCreate(3, edblack, edgra_mode == 3, 1, edgracbSetClumpMode, "Individual FS Mode"));
    edgraAttachMenu(parent, edgra_clumpmode_menu);
}
static void edgracbGlobalsMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    edgra_globals_menu =
        eduiMenuCreate(70, 70, 220, 300, ed_fnt, edgracbCancelGlobalsMenu, "Global Options (Take Care!)");
    if (!edgra_globals_menu)
        return;
    eduiMenuAddItem(edgra_globals_menu,
                    eduiItemSliderCreate(0, edblack, 0, edgracbSetGlobalFadeIn, 0.0f, edgra_superscale * 2.0f,
                                         edgra_global_fadein, "Global Start of Fade"));
    globalfadeinitem = (edui_slider_s *)(edui_last_item);
    eduiItemSliderSetFmt((edui_slider_s *)(edui_last_item), "(%1.01f)");
    eduiItemSliderSetGranularity((edui_slider_s *)(edui_last_item), 0.1f);
    eduiMenuAddItem(edgra_globals_menu,
                    eduiItemSliderCreate(0, edblack, 0, edgracbSetGlobalFadeOut, 0.0f, edgra_superscale * 2.0f,
                                         edgra_global_fadeout, "Global End of Fade"));
    globalfadeoutitem = (edui_slider_s *)(edui_last_item);
    eduiItemSliderSetFmt((edui_slider_s *)(edui_last_item), "(%1.01f)");
    eduiItemSliderSetGranularity((edui_slider_s *)(edui_last_item), 0.1f);
    eduiMenuAddItem(edgra_globals_menu,
                    eduiItemSelCreate(1, edblack, 0, 0, edgracbApplyGlobalFade, "Apply Globally (Careful!)"));
    edgraAttachMenu(parent, edgra_globals_menu);
}

void __cdecl edgraStopPage(i8 page) {
    i32 i;
    if (edgra_page_on[page]) {
        edgra_page_on[page] = 0;
        edgra_units_used = 0;
        NuWindInit();
        NuFadeObjInit();
        for (i = 0; i < 8; ++i) {
            if (edgra_page_on[i]) {
                edgra_page_on[i] = 0;
                edgraStartPage((i8)i);
            }
        }
    }
}

void __cdecl edgraClearPage(i8 page) {
    i32 i;
    edgraStopPage(page);
    for (i = 0; i < EDGRA_MAX_CLUMPS; ++i) {
        PC_EDGRACLUMP *clump = &GrassClumps[i];
        if (clump->element_count && clump->page == (u8)page) {
            if (clump->kind == 3) {
                IndGrassClumpsUsed[clump->individual_index] = 0;
                --edgra_ind_clumps_used;
            }
            clump->element_count = 0;
            --edgra_clumps_used;
        }
    }
    edgra_page_used[page] = 0;
    edgra_page_scene[page] = NULL;
    edgra_page_matrix_stack[page] = NULL;
    edgra_page_vectors_valid[page] = 0;
    edgra_page_calculate_done[page] = 0;
}

void __cdecl edgraSortVectorBuffer(i32 index) {
    NUVEC temporary[256];
    if (index != -1 && index != edgra_last_clump_in_buffer && GrassClumps[index].vector_buffer != NULL) {
        u8 *buffer = (u8 *)GrassClumps[index].vector_buffer;
        u32 bytes = GrassClumps[index].element_count * sizeof(NUVEC);
        i32 i;
        PCByteCopy_candidate(temporary, buffer, bytes);
        PCByteCopy_candidate(buffer, buffer + bytes, (u32)((u8 *)edgra_free_vecbuffer - (buffer + bytes)));
        PCByteCopy_candidate((u8 *)edgra_free_vecbuffer - bytes, temporary, bytes);
        edgra_last_clump_in_buffer = index;
        for (i = 0; i < EDGRA_MAX_CLUMPS; ++i) {
            if (i != index && GrassClumps[i].element_count &&
                (u8 *)GrassClumps[i].vector_buffer > (u8 *)GrassClumps[index].vector_buffer) {
                GrassClumps[i].vector_buffer = (u8 *)GrassClumps[i].vector_buffer - bytes;
            }
        }
        GrassClumps[index].vector_buffer = (u8 *)edgra_free_vecbuffer - bytes;
        edgraInitAllClumps();
    }
}

void __cdecl edgraDetermineNearestClump(f32 distance) {
    NUVEC delta;
    i32 i;
    if (edgra_nearest != -1) {
        NuVecSub(&delta, &edgra_cam_pos, &GrassClumps[edgra_nearest].position);
        if (delta.x * delta.x + delta.y * delta.y + delta.z * delta.z == 0.0f)
            return;
    }
    edgra_nearest = -1;
    for (i = 0; i < EDGRA_MAX_CLUMPS; ++i) {
        if (GrassClumps[i].element_count) {
            f32 candidate;
            NuVecSub(&delta, &edgra_cam_pos, &GrassClumps[i].position);
            candidate = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
            if (distance < 0.0f || candidate < distance) {
                distance = candidate;
                edgra_nearest = i;
            }
        }
    }
    if (edgra_nearest != -1)
        edgraSortVectorBuffer(edgra_nearest);
}

i32 __cdecl edgraLoadPage(char *path, void *scene, i32 terrain, VARIPTR *buffer, VARIPTR *buffer_end) {
    i32 page, version, clump_count, matrix_count, slot, file_index;
    u32 matrix_base, vector_cursor;
    page = 0;
    while (page < 8 && edgra_page_used[page])
        ++page;
    if (page == 8)
        return -1;
    matrix_base = edbits_editmode == 1 && edgra_mtxbuffer != NULL ? (u32)edgra_mtxbuffer : buffer->addr;
    matrix_base = (matrix_base + 15) & ~15u;
    edgra_page_matrix_stack[page] = (NUMTX *)matrix_base;
    EdFileSetMedia(1);
    if (!EdFileOpen(path, 0))
        return -1;
    EdFileSetReadWrongEndianess(1);
    version = EdFileReadInt();
    if (version < 1 || version > 9) {
        EdFileSetReadWrongEndianess(0);
        EdFileClose();
        return -1;
    }
    if (version == 9) {
        edgra_global_fadein = EdFileReadFloat();
        edgra_global_fadeout = EdFileReadFloat();
    } else {
        edgra_global_fadein = 15.0f;
        edgra_global_fadeout = 25.0f;
    }
    clump_count = EdFileReadInt();
    if (clump_count + edgra_clumps_used > EDGRA_MAX_CLUMPS)
        clump_count = EDGRA_MAX_CLUMPS - edgra_clumps_used;
    matrix_count = version >= 8 ? EdFileReadInt() : 0x3000;
    vector_cursor = edbits_editmode == 1 && edgra_vecbuffer != NULL ? (u32)edgra_vecbuffer
                                                                    : matrix_base + matrix_count * sizeof(NUMTX);
    slot = 0;
    for (file_index = 0; file_index < clump_count; ++file_index) {
        PC_EDGRACLUMP *clump;
        char name[20];
        i32 skipped_individuals, individual, element;
        /* Retail proceeds with the limit record if no free slot was found. */
        while (GrassClumps[slot].element_count != 0 && slot < EDGRA_MAX_CLUMPS)
            ++slot;
        clump = &GrassClumps[slot];
        EdFileRead(name, sizeof(name));
        clump->special_index = edbitsLookupInstance(name, (NUGSCN *)scene);
        clump->element_count = EdFileReadInt();
        clump->position.x = EdFileReadFloat();
        clump->position.y = EdFileReadFloat();
        clump->position.z = EdFileReadFloat();
        clump->size = EdFileReadFloat();
        clump->field_18 = EdFileReadFloat();
        clump->flags = version < 2 ? 1 : EdFileReadInt();
        clump->field_20 = version < 3 ? 1.0f : EdFileReadFloat();
        clump->page = (u8)page;
        if (version < 5) {
            clump->unknown_25 = 1;
            clump->unknown_26 = 1;
        } else {
            clump->unknown_25 = (u8)EdFileReadChar();
            clump->unknown_26 = (u8)EdFileReadChar();
        }
        clump->kind = version < 6 ? 1 : (u8)EdFileReadChar();
        if (version < 4) {
            clump->seed = 0;
            clump->field_2c = 0.2f;
            clump->field_30 = 1.7625f;
        } else {
            clump->seed = EdFileReadInt();
            clump->field_2c = EdFileReadFloat();
            clump->field_30 = EdFileReadFloat();
        }
        if (version < 5) {
            clump->rotation_z = 0;
            clump->rotation_y = 0;
        } else {
            clump->rotation_z = EdFileReadShort();
            clump->rotation_y = EdFileReadShort();
        }
        if (version < 6) {
            clump->near_distance = 15.0f;
            clump->far_distance = 25.0f;
        } else {
            clump->near_distance = EdFileReadFloat();
            clump->far_distance = EdFileReadFloat();
        }
        if (version >= 7) {
            clump->field_42 = (u8)EdFileReadChar();
            clump->field_43 = (u8)EdFileReadChar();
            clump->field_44 = EdFileReadFloat();
        } else if (clump->kind == 1) {
            clump->field_42 = 0;
            clump->field_43 = 0;
            clump->field_44 = 0.0f;
            clump->near_distance = 20.0f;
            clump->far_distance = 20.0f;
        } else if (clump->kind == 2) {
            clump->field_42 = 1;
            clump->field_43 = 1;
            clump->field_44 = 0.05f;
        } else if (clump->kind == 3) {
            clump->field_42 = 0;
            clump->field_43 = 0;
            clump->field_44 = 0.0f;
        }
        skipped_individuals = 0;
        if (version >= 6 && clump->kind == 3) {
            individual = 0;
            if (edgra_ind_clumps_used == EDGRA_MAX_INDIVIDUAL_CLUMPS) {
                skipped_individuals = clump->element_count;
                clump->element_count = 0;
            } else {
                while (IndGrassClumpsUsed[individual] != 0)
                    ++individual;
                clump->individual_index = (i16)individual;
                IndGrassClumpsUsed[individual] = 1;
                if (clump->element_count > EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP) {
                    skipped_individuals = clump->element_count - EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP;
                    clump->element_count = EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP;
                }
                for (element = 0; element < clump->element_count; ++element) {
                    IndGrassClumps[individual * EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP + element].position.x =
                        EdFileReadFloat();
                    IndGrassClumps[individual * EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP + element].position.y =
                        EdFileReadFloat();
                    IndGrassClumps[individual * EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP + element].position.z =
                        EdFileReadFloat();
                    IndGrassClumps[individual * EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP + element].field_0c =
                        EdFileReadFloat();
                    IndGrassClumps[individual * EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP + element].field_10 =
                        EdFileReadShort();
                    IndGrassClumps[individual * EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP + element].field_12 =
                        EdFileReadShort();
                }
            }
            for (element = 0; element < skipped_individuals; ++element) {
                EdFileReadFloat();
                EdFileReadFloat();
                EdFileReadFloat();
                EdFileReadFloat();
                EdFileReadShort();
                EdFileReadShort();
            }
        }
        clump->vector_buffer = (void *)vector_cursor;
        edgra_page_vectors_valid[page] = version >= 8;
        if (version >= 8) {
            for (element = 0; element < clump->element_count; ++element) {
                ((NUVEC *)clump->vector_buffer)[element].x = EdFileReadFloat();
                ((NUVEC *)clump->vector_buffer)[element].y = EdFileReadFloat();
                ((NUVEC *)clump->vector_buffer)[element].z = EdFileReadFloat();
            }
        }
        if (clump->element_count < 4)
            clump->element_count = 4;
        if (clump->special_index == -1)
            clump->element_count = 0;
        if (file_index % edgra_clumpthin != 0 && edbits_editmode == 0) {
            clump->element_count = 0;
        } else if (edgra_elementthin != 1 && edbits_editmode == 0) {
            clump->element_count = (clump->element_count + edgra_elementthin - 1) / edgra_elementthin;
        }
        if (clump->element_count > 0) {
            vector_cursor += clump->element_count * sizeof(NUVEC);
            ++edgra_clumps_used;
            edgra_last_clump_in_buffer = slot;
            edgra_free_vecbuffer = (void *)vector_cursor;
        }
    }
    EdFileSetReadWrongEndianess(0);
    EdFileClose();
    edgra_nearest = -1;
    edgraDetermineNearestClump(1.0f);
    edgra_page_scene[page] = (NUGSCN *)scene;
    edgra_page_terrain[page] = (void *)terrain;
    edgra_page_used[page] = 1;
    if (edgra_page_vectors_valid[page]) {
        void *saved_scene = PCGrassTerrainScene;
        PCGrassTerrainScene = scene;
        edgraCalculatePage((i8)page, 0);
        PCGrassTerrainScene = saved_scene;
    }
    if (edbits_editmode != 1 || edgra_vecbuffer == NULL)
        buffer->addr = vector_cursor;
    (void)buffer_end;
    return page;
}
static void edgracbFileLoad(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    char path[256];
    char directory[256];
    char name[256];
    char extension[256];
    if (edbits_level_save_directory[0] == 0)
        strcpy(directory, ".");
    else
        strcpy(directory, edbits_level_save_directory);
    if (edbits_level_save_name[0] == 0)
        strcpy(name, "grass");
    else
        strcpy(name, edbits_level_save_name);
    if (edbits_level_save_extension[0] == 0)
        strcpy(extension, "gra");
    else
        strcpy(extension, edbits_level_save_extension);
    sprintf(path, "%s\\%s.%s", directory, name, extension);
    edgraClumpsReset();
    if (NuFileExists(path))
        edgraLoadPage(path, edbits_base_scene, (size_t)(edbits_base_terrain), NULL, NULL);
    edgraInitAllClumps();
    eduiCreateMessageMenu(menu, "Loaded OK", 1);
}
static i32 edgraProc(f32 delta_time, FRAME_PAD_PC *pad) {
    if (edgra_active_menu) {
        eduiMenuProcess(edgra_active_menu, delta_time, pad);
        return 0;
    }
    edgraDoInput(pad);
    if (edgra_editormode == 1)
        edgraDetermineNearestInstance(1.0f);
    else
        edgraDetermineNearestClump(1.0f);
    return (pad->digital_buttons_pressed >> 11) & 1;
}
