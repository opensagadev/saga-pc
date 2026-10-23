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

void edgracbSetClumpWind(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edgra_nearest != -1)
        GrassClumps[edgra_nearest].field_18 = ((edui_slider_s *)(item))->value;
    edgraInitAllClumps();
}

static void edgracbChangeInstanceMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    i32 selected_found;
    i32 added;
    i32 count;
    char *name;
    i32 i;
    edgra_changeinstance_menu =
        eduiMenuCreate(70, 70, 220, 250, ed_fnt, edgracbCancelChangeInstanceMenu,
                       ((char *)(edgra_filter ? "Change Instance (Filtered)" : "Change Instance")));
    if (!edgra_changeinstance_menu || !edbits_base_scene)
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
        if (GrassClumps[edgra_nearest].special_index == i) {
            selected = 1;
            keep = 1;
        } else {
            selected = 0;
        }
        NuGScnGetSpecial(&special, edbits_base_scene, i);
        if (NuSpecialExistsFn(&special))
            name = NuSpecialGetName(&special);
        if (edgra_filter && NuStrNCmp(edgra_filter_string, name, NuStrLen(edgra_filter_string)) && !keep)
            continue;
        eduiMenuAddItem(edgra_changeinstance_menu,
                        eduiItemCheckCreate(i, edblack, selected, 1, edgracbChangeInstanceType, name));
        ++added;
        if (selected == 1) {
            edgra_changeinstance_menu->selected = edui_last_item;
            selected_found = 1;
        }
    }
    if (added == 1)
        eduiMenuAddItem(edgra_changeinstance_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "All Instances Filtered"));
    eduiMenuAttach(parent, edgra_changeinstance_menu);
    edgra_changeinstance_menu->x = parent->x + 10;
    edgra_changeinstance_menu->y = parent->y + 40;
    if (selected_found)
        edgra_changeinstance_menu->field_0c = edgra_changeinstance_menu->selected;
}

static __inline void edgracbClumpSizesMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    if (edgra_nearest == -1 || !GrassClumps[edgra_nearest].element_count)
        return;
    edgra_clumpsizes_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edgracbCancelClumpSizesMenu, "Clump Sizes");
    if (!edgra_clumpsizes_menu)
        return;
    eduiMenuAddItem(edgra_clumpsizes_menu, eduiItemSliderCreate(0, colours, 0, edgracbSetClumpMinHeight, 0.0f, 5.0f,
                                                                GrassClumps[edgra_nearest].field_2c, "Min Model Size"));
    eduiMenuAddItem(edgra_clumpsizes_menu, eduiItemSliderCreate(0, colours, 0, edgracbSetClumpMaxHeight, 0.0f, 5.0f,
                                                                GrassClumps[edgra_nearest].field_30, "Max Model Size"));
    if (GrassClumps[edgra_nearest].kind == 1)
        eduiMenuAddItem(edgra_clumpsizes_menu,
                        eduiItemSliderCreate(0, colours, 0, edgracbSetClumpHeight, 0.1f, 9.9f,
                                             GrassClumps[edgra_nearest].field_20, "Bend Height"));
    eduiMenuAttach(parent, edgra_clumpsizes_menu);
    edgra_clumpsizes_menu->x = parent->x + 10;
    edgra_clumpsizes_menu->y = parent->y + 40;
}

void edgracbSetClumpArea(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edgra_nearest != -1)
        GrassClumps[edgra_nearest].unknown_25 = item->data;
    edgraInitAllClumps();
}
static void edgracbClumpDistMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    if (edgra_nearest == -1 || !GrassClumps[edgra_nearest].element_count)
        return;
    edgra_clumpdist_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edgracbCancelClumpDistMenu, "Clump Distribution");
    if (!edgra_clumpdist_menu)
        return;
    eduiMenuAddItem(edgra_clumpdist_menu, eduiItemCheckCreate(1, colours, GrassClumps[edgra_nearest].unknown_26 == 1, 1,
                                                              edgracbSetClumpDist, "Legacy"));
    eduiMenuAddItem(edgra_clumpdist_menu, eduiItemCheckCreate(2, colours, GrassClumps[edgra_nearest].unknown_26 == 2, 1,
                                                              edgracbSetClumpDist, (char *)AIActionRandomName));
    eduiMenuAddItem(edgra_clumpdist_menu, eduiItemCheckCreate(3, colours, GrassClumps[edgra_nearest].unknown_26 == 3, 1,
                                                              edgracbSetClumpDist, "Linear"));
    eduiMenuAddItem(edgra_clumpdist_menu, eduiItemCheckCreate(4, colours, GrassClumps[edgra_nearest].unknown_26 == 4, 1,
                                                              edgracbSetClumpDist, "Bell Curve"));
    eduiMenuAttach(parent, edgra_clumpdist_menu);
    edgra_clumpdist_menu->x = parent->x + 10;
    edgra_clumpdist_menu->y = parent->y + 40;
}
static void edgracbClumpTerrainMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    if (edgra_nearest == -1 || !GrassClumps[edgra_nearest].element_count)
        return;
    edgra_clumpterrain_menu =
        eduiMenuCreate(70, 70, 250, 250, ed_fnt, edgracbCancelClumpTerrainMenu, "Clump Terraining");
    if (!edgra_clumpterrain_menu)
        return;
    eduiMenuAddItem(edgra_clumpterrain_menu,
                    eduiItemToggleCreate(0, edblack, ((i8)(GrassClumps[edgra_nearest].field_42)), 1,
                                         edgracbToggleClumpTerrain, "Clump Terraining"));
    if (GrassClumps[edgra_nearest].kind != 1)
        eduiMenuAddItem(edgra_clumpterrain_menu,
                        eduiItemToggleCreate(0, edblack, ((i8)(GrassClumps[edgra_nearest].field_43)), 2,
                                             edgracbToggleClumpTilt, "Clump Tilting"));
    eduiMenuAddItem(edgra_clumpterrain_menu,
                    eduiItemSliderCreate(0, edblack, 0, edgracbSetClumpTerrainOffset, -1.0f, 2.0f,
                                         GrassClumps[edgra_nearest].field_44, "Terraining Offset"));
    eduiMenuAttach(parent, edgra_clumpterrain_menu);
    edgra_clumpterrain_menu->x = parent->x + 10;
    edgra_clumpterrain_menu->y = parent->y + 40;
}

void edgracbSetClumpMode(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edgra_mode = item->data;
    if (edgra_nearest != -1)
        GrassClumps[edgra_nearest].kind = item->data;
    edgraInitAllClumps();
}

void edgraClumpReseed(i32 index) {
    GrassClumps[index].seed = NuRand(NULL);
    edgraInitAllClumps();
}

void edgracbApplyGlobalFade(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    i32 i;
    struct eduimenu_s *parent;
    for (i = 0; i < EDGRA_MAX_CLUMPS; ++i) {
        if (GrassClumps[i].element_count) {
            GrassClumps[i].near_distance = edgra_global_fadein;
            GrassClumps[i].far_distance = edgra_global_fadeout;
        }
    }
    edgraInitAllClumps();
    parent = menu->parent;
    if (parent)
        eduiMenuDetach(menu);
    if (menu->callback)
        menu->callback(menu, parent);
}
void edgracbClumpAreaMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
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
    eduiMenuAttach(parent, edgra_clumparea_menu);
    edgra_clumparea_menu->x = parent->x + 10;
    edgra_clumparea_menu->y = parent->y + 40;
}

static void edgracbSetClumpFadeIn(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edui_slider_s *slider = fadeoutitem;
    if (edgra_nearest != -1) {
        PC_EDGRACLUMP *clump = &(GrassClumps[edgra_nearest]);
        (*clump).near_distance = ((edui_slider_s *)(item))->value;
        if ((*clump).near_distance > (*clump).far_distance) {
            (*clump).far_distance = (*clump).near_distance;
            slider->value = (*clump).far_distance;
            slider->normalized_value = (slider->value - slider->minimum) / slider->range;
        }
    }
    edgraInitAllClumps();
}

static void edgracbSetClumpFadeOut(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edui_slider_s *slider = fadeinitem;
    if (edgra_nearest != -1) {
        PC_EDGRACLUMP *clump = &(GrassClumps[edgra_nearest]);
        (*clump).far_distance = ((edui_slider_s *)(item))->value;
        if ((*clump).near_distance > (*clump).far_distance) {
            (*clump).near_distance = (*clump).far_distance;
            slider->value = (*clump).near_distance;
            slider->normalized_value = (slider->value - slider->minimum) / slider->range;
        }
    }
    edgraInitAllClumps();
}

static void edgracbSetClumpDist(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edgra_nearest != -1)
        GrassClumps[edgra_nearest].unknown_26 = item->data;
    edgraInitAllClumps();
}

static void edgracbToggleClumpTerrain(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edgra_nearest != -1)
        GrassClumps[edgra_nearest].field_42 = item->flags & EDUI_ITEM_HIGHLIGHTED;
    edgraInitAllClumps();
}

static void edgracbToggleClumpTilt(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edgra_nearest != -1)
        GrassClumps[edgra_nearest].field_43 = item->flags & EDUI_ITEM_HIGHLIGHTED;
    edgraInitAllClumps();
}

static void edgracbSetClumpTerrainOffset(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edgra_nearest != -1)
        GrassClumps[edgra_nearest].field_44 = ((edui_slider_s *)(item))->value;
    edgraInitAllClumps();
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
            eduiMenuAttach(parent, edgra_clumpfade_menu);
            edgra_clumpfade_menu->x = parent->x + 10;
            edgra_clumpfade_menu->y = parent->y + 40;
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
        eduiMenuAttach(parent, edgra_clumpproperties_menu);
        edgra_clumpproperties_menu->x = parent->x + 10;
        edgra_clumpproperties_menu->y = parent->y + 40;
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
    eduiMenuAttach(parent, edgra_clumpmode_menu);
    edgra_clumpmode_menu->x = parent->x + 10;
    edgra_clumpmode_menu->y = parent->y + 40;
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
    eduiMenuAttach(parent, edgra_globals_menu);
    edgra_globals_menu->x = parent->x + 10;
    edgra_globals_menu->y = parent->y + 40;
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

void edgracbFileLoad(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
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

void __cdecl edgracbInstanceMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2);
void edgracbDpadModeMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2);

#define EDGRA_INDIVIDUAL(clump, element) (&IndGrassClumps[EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP * (clump) + (element)])

extern i32 edgra_pageid;

void __cdecl edgraClumpPlace(i32 index, NUVEC *position) {
    GrassClumps[index].position = *position;
    GrassClumps[index].size = edgra_size;
    GrassClumps[index].rotation_z = (i16)edgra_rotz;
    GrassClumps[index].rotation_y = (i16)edgra_roty;
    if (edgra_mode != 3) {
        if (edgra_units_used + edgra_clump_size - GrassClumps[index].element_count <= 0x3000)
            GrassClumps[index].element_count = edgra_clump_size;
    }
    edgra_free_vecbuffer = (NUVEC *)GrassClumps[index].vector_buffer + GrassClumps[index].element_count;
    edgraInitAllClumps();
}

void __cdecl edgraInstancePlace(i32 index, NUVEC *position) {
    EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, index)->position.x =
        position->x - GrassClumps[edgra_nearest].position.x;
    EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, index)->position.y =
        position->y - GrassClumps[edgra_nearest].position.y;
    EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, index)->position.z =
        position->z - GrassClumps[edgra_nearest].position.z;
    EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, index)->field_0c = 1.0f;
    EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, index)->field_10 = (i16)edgra_rotz;
    EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, index)->field_12 = (i16)edgra_roty;
    edgraInitAllClumps();
}

i32 __cdecl edgraClumpCreate(NUVEC *position) {
    i32 index = 0;
    i32 individual = 0;
    if (edgra_clumps_used == EDGRA_MAX_CLUMPS)
        return -1;
    if (edgra_copy_source == -1 && edgra_mode == 3) {
        if (edgra_units_used == 0x3000 || edgra_ind_clumps_used == EDGRA_MAX_INDIVIDUAL_CLUMPS)
            return -1;
    } else if (edgra_units_used + edgra_clump_size > 0x3000) {
        return -1;
    }
    while (GrassClumps[index].element_count != 0)
        ++index;
    if (edgra_copy_source != -1) {
        GrassClumps[index].special_index = GrassClumps[edgra_copy_source].special_index;
        GrassClumps[index].element_count = edgra_clump_size;
        GrassClumps[index].field_18 = GrassClumps[edgra_copy_source].field_18;
        GrassClumps[index].flags = GrassClumps[edgra_copy_source].flags;
        GrassClumps[index].field_20 = GrassClumps[edgra_copy_source].field_20;
        GrassClumps[index].page = (u8)edgra_pageid;
        GrassClumps[index].seed = NuRand(NULL);
        GrassClumps[index].unknown_25 = GrassClumps[edgra_copy_source].unknown_25;
        GrassClumps[index].unknown_26 = GrassClumps[edgra_copy_source].unknown_26;
        GrassClumps[index].kind = GrassClumps[edgra_copy_source].kind;
        GrassClumps[index].field_2c = GrassClumps[edgra_copy_source].field_2c;
        GrassClumps[index].field_30 = GrassClumps[edgra_copy_source].field_30;
        GrassClumps[index].near_distance = GrassClumps[edgra_copy_source].near_distance;
        GrassClumps[index].far_distance = GrassClumps[edgra_copy_source].far_distance;
        GrassClumps[index].field_42 = GrassClumps[edgra_copy_source].field_42;
        GrassClumps[index].field_44 = GrassClumps[edgra_copy_source].field_44;
        GrassClumps[index].field_43 = GrassClumps[edgra_copy_source].field_43;
    } else {
        GrassClumps[index].special_index = edgra_instance_type;
        if (edgra_mode == 3)
            GrassClumps[index].element_count = 1;
        else
            GrassClumps[index].element_count = edgra_clump_size;
        GrassClumps[index].field_18 = 0.2f;
        GrassClumps[index].flags = 1;
        GrassClumps[index].field_20 = 1.0f;
        GrassClumps[index].page = (u8)edgra_pageid;
        GrassClumps[index].seed = NuRand(NULL);
        GrassClumps[index].unknown_25 = 1;
        GrassClumps[index].unknown_26 = 1;
        GrassClumps[index].kind = (u8)edgra_mode;
        GrassClumps[index].field_2c = 0.0f;
        GrassClumps[index].field_30 = 1.0f;
        GrassClumps[index].near_distance = edgra_global_fadein;
        GrassClumps[index].far_distance = edgra_global_fadeout;
        GrassClumps[index].field_42 = 1;
        GrassClumps[index].field_44 = 0.0f;
        GrassClumps[index].field_43 = 1;
    }
    if (GrassClumps[index].kind == 3) {
        while (IndGrassClumpsUsed[individual])
            ++individual;
        GrassClumps[index].individual_index = (i16)individual;
        IndGrassClumpsUsed[individual] = 1;
        EDGRA_INDIVIDUAL(individual, 0)->position.x = 0.0f;
        EDGRA_INDIVIDUAL(individual, 0)->position.y = 0.0f;
        EDGRA_INDIVIDUAL(individual, 0)->position.z = 0.0f;
        EDGRA_INDIVIDUAL(individual, 0)->field_0c = 1.0f;
        EDGRA_INDIVIDUAL(individual, 0)->field_10 = (i16)edgra_rotz;
        EDGRA_INDIVIDUAL(individual, 0)->field_12 = (i16)edgra_roty;
        ++edgra_ind_clumps_used;
    } else {
        GrassClumps[index].individual_index = -1;
    }
    ++edgra_clumps_used;
    if (!edgra_page_used[edgra_pageid]) {
        edgra_page_used[edgra_pageid] = 1;
        edgra_page_scene[edgra_pageid] = edbits_base_scene;
        edgra_page_terrain[edgra_pageid] = edbits_base_terrain;
        edgra_page_matrix_stack[edgra_pageid] = edgra_mtxbuffer;
    }
    GrassClumps[index].vector_buffer = edgra_free_vecbuffer;
    edgra_last_clump_in_buffer = index;
    edgraClumpPlace(index, position);
    return index;
}

void __cdecl edgraInstanceCreate(NUVEC *position) {
    i32 index;
    if (edgra_nearest != -1 && GrassClumps[edgra_nearest].element_count != EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP) {
        index = GrassClumps[edgra_nearest].element_count++;
        edgraInstancePlace(index, position);
    }
}

void __cdecl edgraClumpDestroy(i32 index) {
    GrassClumps[index].element_count = 0;
    edgra_free_vecbuffer = GrassClumps[index].vector_buffer;
    GrassClumps[index].vector_buffer = NULL;
    --edgra_clumps_used;
    if (GrassClumps[index].kind == 3) {
        IndGrassClumpsUsed[GrassClumps[index].individual_index] = 0;
        --edgra_ind_clumps_used;
    }
    edgraInitAllClumps();
}

void __cdecl edgraInstanceDestroy(i32 index) {
    i32 i;
    if (edgra_nearest != -1 && GrassClumps[edgra_nearest].element_count != 1) {
        if (index != GrassClumps[edgra_nearest].element_count - 1) {
            for (i = index; i < GrassClumps[edgra_nearest].element_count - 1; ++i) {
                EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, i)->position =
                    EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, i + 1)->position;
                EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, i)->field_0c =
                    EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, i + 1)->field_0c;
                EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, i)->field_10 =
                    EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, i + 1)->field_10;
                EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, i)->field_12 =
                    EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, i + 1)->field_12;
            }
        }
        --GrassClumps[edgra_nearest].element_count;
        edgraInitAllClumps();
    }
}

void __cdecl edgraCreateOptionsMenu(void) {
    edgra_options_menu = eduiMenuCreate(70, 70, 220, 300, ed_fnt, edgracbCancelOptMenu, "Options");
    if (edgra_options_menu == NULL)
        return;
    eduiMenuAddItem(edgra_options_menu,
                    eduiItemSelCreate(1, edblack, 0, 0, edgracbInstanceMenu, "Base Instance Select..."));
    if (edgra_nearest == -1)
        eduiMenuAddItem(edgra_options_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Change Instance..."));
    else
        eduiMenuAddItem(edgra_options_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edgracbChangeInstanceMenu, "Change Instance..."));
    eduiMenuAddItem(edgra_options_menu, eduiItemSelCreate(1, edblack, 0, 0, edgracbClumpModeMenu, "Clump Mode..."));
    if (edgra_nearest == -1)
        eduiMenuAddItem(edgra_options_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Clump Properties..."));
    else
        eduiMenuAddItem(edgra_options_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edgracbClumpPropertiesMenu, "Clump Properties..."));
    eduiMenuAddItem(edgra_options_menu, eduiItemSelCreate(1, edblack, 0, 0, edgracbDpadModeMenu, "Dpad Mode..."));
    if (edgra_nearest == -1)
        eduiMenuAddItem(edgra_options_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Copy Clump"));
    else
        eduiMenuAddItem(edgra_options_menu, eduiItemSelCreate(1, edblack, 0, 0, edgracbCopyClump, "Copy Clump"));
    if (edgra_copy_source == -1)
        eduiMenuAddItem(edgra_options_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Discard Copy"));
    else
        eduiMenuAddItem(edgra_options_menu, eduiItemSelCreate(1, edblack, 0, 0, edgracbDiscardCopy, "Discard Copy"));
    eduiMenuAddItem(edgra_options_menu, eduiItemSelCreate(1, edblack, 0, 0, edgracbFileSave, "Save Grass"));
    eduiMenuAddItem(edgra_options_menu, eduiItemSelCreate(1, edblack, 0, 0, edgracbFileLoad, "Load Grass"));
    eduiMenuAddItem(edgra_options_menu, eduiItemSelCreate(1, edblack, 0, 0, edgracbSScaleMenu, "Super Scale..."));
    eduiMenuAddItem(edgra_options_menu,
                    eduiItemToggleCreate(1, edblack, edgra_filter, 1, edgraToggleFilter, "Instance Filter"));
    eduiMenuAddItem(edgra_options_menu, eduiItemTextPickCreate(0, edblack, edgraChangeFilterName, "Filter String: "));
    strcpy(((edui_textpicker_s *)edui_last_item)->value, edgra_filter_string);
    ((edui_textpicker_s *)edui_last_item)->max_length = 15;
    eduiMenuAddItem(edgra_options_menu, eduiItemSelCreate(1, edblack, 0, 0, edgracbGlobalsMenu, "Global Options..."));
}

void __cdecl edgraDoInput(FRAME_PAD_PC *pad) {
    NUVEC position;
    i32 individual;
    if ((pad->digital_buttons & 0x100) == 0)
        edcamMove(pad);
    if (pad->digital_buttons & 0x100) {
        if ((pad->digital_buttons_pressed & 0x20) && edgra_nearest != -1)
            edgraClumpReseed(edgra_nearest);
        if ((pad->digital_buttons_pressed & 0x40) && edgra_nearest != -1) {
            if (edgra_editormode == 1)
                edgra_editormode = 0;
            else if (edgra_mode == 3)
                edgra_editormode = 1;
        }
        if (edgra_editormode != 1) {
            if (edgra_nearest != -1) {
                if (pad->digital_buttons_pressed & 8) {
                    do {
                        ++edgra_nearest;
                        if (edgra_nearest == EDGRA_MAX_CLUMPS)
                            edgra_nearest = 0;
                    } while (!GrassClumps[edgra_nearest].element_count);
                    edgraSortVectorBuffer(edgra_nearest);
                }
                if (pad->digital_buttons_pressed & 2) {
                    do {
                        --edgra_nearest;
                        if (edgra_nearest == -1)
                            edgra_nearest = EDGRA_MAX_CLUMPS - 1;
                    } while (!GrassClumps[edgra_nearest].element_count);
                    edgraSortVectorBuffer(edgra_nearest);
                }
            } else {
                edgraDetermineNearestClump(-1.0f);
            }
        } else if (edgra_nearest_instance == -1) {
            edgraDetermineNearestInstance(-1.0f);
        } else if (edgra_nearest != -1) {
            if (pad->digital_buttons_pressed & 8) {
                ++edgra_nearest_instance;
                if (edgra_nearest_instance == GrassClumps[edgra_nearest].element_count)
                    edgra_nearest_instance = 0;
            }
            if (pad->digital_buttons_pressed & 2) {
                --edgra_nearest_instance;
                if (edgra_nearest_instance == -1)
                    edgra_nearest_instance = GrassClumps[edgra_nearest].element_count - 1;
            }
        }
        if (edgra_editormode == 1) {
            if (edgra_nearest != -1 && edgra_nearest_instance != -1) {
                individual = GrassClumps[edgra_nearest].individual_index;
                NuVecAdd(&position, &GrassClumps[edgra_nearest].position,
                         &EDGRA_INDIVIDUAL(individual, edgra_nearest_instance)->position);
                edcamSetPos(&position);
                edgra_rotz = EDGRA_INDIVIDUAL(individual, edgra_nearest_instance)->field_10;
                edgra_roty = EDGRA_INDIVIDUAL(individual, edgra_nearest_instance)->field_12;
            }
        } else {
            if (edgra_nearest != -1) {
                edcamSetPos(&GrassClumps[edgra_nearest].position);
                edgra_mode = (i8)GrassClumps[edgra_nearest].kind;
                edgra_instance_type = GrassClumps[edgra_nearest].special_index;
                if (edgra_mode != 3) {
                    edgra_size = GrassClumps[edgra_nearest].size;
                    edgra_clump_size = GrassClumps[edgra_nearest].element_count;
                    edgra_rotz = GrassClumps[edgra_nearest].rotation_z;
                    edgra_roty = GrassClumps[edgra_nearest].rotation_y;
                }
            }
        }
    }
    edcamGetPosAng(&edgra_cam_pos, &edgra_cam_ax, &edgra_cam_ay);
    if ((pad->digital_buttons & 0x100) == 0) {
        if (pad->digital_buttons_pressed & 0x80) {
            edgraCreateOptionsMenu();
            edgra_active_menu = edgra_options_menu;
        }
        if (pad->digital_buttons_pressed & 0x40) {
            if (edgra_editormode == 1)
                edgraInstanceCreate(&edgra_cam_pos);
            else if (edgra_instance_type != -1)
                edgraClumpCreate(&edgra_cam_pos);
        }
        if (pad->digital_buttons & 0x20) {
            if (edgra_editormode == 1) {
                if (edgra_nearest_instance != -1)
                    edgraInstancePlace(edgra_nearest_instance, &edgra_cam_pos);
            } else if (edgra_nearest != -1) {
                edgraClumpPlace(edgra_nearest, &edgra_cam_pos);
            }
        }
        if (pad->digital_buttons_pressed & 0x10) {
            if (edgra_editormode != 1) {
                if (edgra_nearest != -1)
                    edgraClumpDestroy(edgra_nearest);
                edgra_nearest = -1;
            } else {
                if (edgra_nearest_instance != -1)
                    edgraInstanceDestroy(edgra_nearest_instance);
                edgra_nearest_instance = -1;
            }
        }
    }
    if (edgra_dpadmode == 0) {
        if (edgra_mode != 3) {
            edgra_size += (f32)pad->analog_left_pad_up / 5000.0f;
            edgra_size -= (f32)pad->analog_left_pad_down / 5000.0f;
            if (edgra_size < 0.1f)
                edgra_size = 0.1f;
            if (edgra_size > 10.0f)
                edgra_size = 10.0f;
            if (pad->analog_left_pad_left == 255 || (pad->digital_buttons_pressed & 0x8000))
                --edgra_clump_size;
            if (pad->analog_left_pad_right == 255 || (pad->digital_buttons_pressed & 0x2000))
                ++edgra_clump_size;
            if (edgra_clump_size < 4)
                edgra_clump_size = 4;
            if (edgra_clump_size > 256)
                edgra_clump_size = 256;
        } else {
            if (edgra_nearest != -1 && edgra_nearest_instance != -1) {
                EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, edgra_nearest_instance)->field_0c +=
                    (f32)pad->analog_left_pad_up / 5000.0f;
                EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, edgra_nearest_instance)->field_0c -=
                    (f32)pad->analog_left_pad_down / 5000.0f;
                if (EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, edgra_nearest_instance)->field_0c >
                    1.0f)
                    EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, edgra_nearest_instance)->field_0c =
                        1.0f;
                if (EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, edgra_nearest_instance)->field_0c <
                    0.1f)
                    EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, edgra_nearest_instance)->field_0c =
                        0.1f;
            }
            if (pad->analog_left_pad_up || pad->analog_left_pad_down)
                edgraInitAllClumps();
        }
    } else if (edgra_dpadmode == 1) {
        edgra_roty += pad->analog_left_pad_right - pad->analog_left_pad_left;
        edgra_rotz += pad->analog_left_pad_up;
        if (edgra_rotz > 32768)
            edgra_rotz = 32768;
        edgra_rotz -= pad->analog_left_pad_down;
        if (edgra_rotz < -32768)
            edgra_rotz = -32768;
    }
}

i32 edgraProc(f32 delta_time, FRAME_PAD_PC *pad) {
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
