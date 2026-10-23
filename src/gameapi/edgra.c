#include <stdio.h>
#include <string.h>
#include "saga.h"

extern struct eduimenu_s *edgra_sscale_menu;
void EdFileWrite(void *data, i32 len);
void EdFileWriteInt(i32 value);
void EdFileWriteShort(i16 value);
void EdFileWriteChar(char value);
void EdFileWriteNuVec(NUVEC *value);

#define EDGRA_INDIVIDUAL(clump, element) (&IndGrassClumps[EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP * (clump) + (element)])

i32 __cdecl edgraFileSave(char *path) {
    NUSPECIALHANDLE special;
    char name[20];
    i32 clump_count = 0;
    i32 element_count = 0;
    i32 i, j;
    for (i = 0; i < EDGRA_MAX_CLUMPS; ++i) {
        if (GrassClumps[i].element_count) {
            ++clump_count;
            element_count += GrassClumps[i].element_count;
        }
    }
    EdFileSetMedia(1);
    if (EdFileOpen(path, 1)) {
        EdFileSetReadWrongEndianess(1);
        EdFileWriteInt(9);
        EdFileWriteFloat(edgra_global_fadein);
        EdFileWriteFloat(edgra_global_fadeout);
        EdFileWriteInt(clump_count);
        EdFileWriteInt(element_count);
        for (i = 0; i < EDGRA_MAX_CLUMPS; ++i) {
            if (!GrassClumps[i].element_count)
                continue;
            NuGScnGetSpecial(&special, edbits_base_scene, GrassClumps[i].special_index);
            strncpy(name, NuSpecialGetName(&special), 20);
            name[19] = 0;
            EdFileWrite(name, 20);
            EdFileWriteInt(GrassClumps[i].element_count);
            EdFileWriteNuVec(&GrassClumps[i].position);
            EdFileWriteFloat(GrassClumps[i].size);
            EdFileWriteFloat(GrassClumps[i].field_18);
            EdFileWriteInt(GrassClumps[i].flags);
            EdFileWriteFloat(GrassClumps[i].field_20);
            EdFileWriteChar(GrassClumps[i].unknown_25);
            EdFileWriteChar(GrassClumps[i].unknown_26);
            EdFileWriteChar(GrassClumps[i].kind);
            EdFileWriteInt(GrassClumps[i].seed);
            EdFileWriteFloat(GrassClumps[i].field_2c);
            EdFileWriteFloat(GrassClumps[i].field_30);
            EdFileWriteShort(GrassClumps[i].rotation_z);
            EdFileWriteShort(GrassClumps[i].rotation_y);
            EdFileWriteFloat(GrassClumps[i].near_distance);
            EdFileWriteFloat(GrassClumps[i].far_distance);
            EdFileWriteChar(GrassClumps[i].field_42);
            EdFileWriteChar(GrassClumps[i].field_43);
            EdFileWriteFloat(GrassClumps[i].field_44);
            if (GrassClumps[i].kind == 3) {
                for (j = 0; j < GrassClumps[i].element_count; ++j) {
                    EdFileWriteNuVec(&EDGRA_INDIVIDUAL(GrassClumps[i].individual_index, j)->position);
                    EdFileWriteFloat(EDGRA_INDIVIDUAL(GrassClumps[i].individual_index, j)->field_0c);
                    EdFileWriteShort(EDGRA_INDIVIDUAL(GrassClumps[i].individual_index, j)->field_10);
                    EdFileWriteShort(EDGRA_INDIVIDUAL(GrassClumps[i].individual_index, j)->field_12);
                }
            }
            for (j = 0; j < GrassClumps[i].element_count; ++j)
                EdFileWriteNuVec(&((NUVEC *)GrassClumps[i].vector_buffer)[j]);
        }
        EdFileSetReadWrongEndianess(0);
        EdFileClose();
        return 1;
    }
    return 0;
}

void edGraEnableTerrainSwap(void) {
}

void edGraDisableTerrainSwap(void) {
}

void edGraInitTerrainSwapProtection(void) {
}

void edgracbSetGlobalFadeIn(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edui_slider_s *slider = globalfadeoutitem;
    edgra_global_fadein = ((edui_slider_s *)(item))->value;
    if (edgra_global_fadein > edgra_global_fadeout) {
        edgra_global_fadeout = edgra_global_fadein;
        slider->value = edgra_global_fadeout;
        slider->normalized_value = (slider->value - slider->minimum) / slider->range;
    }
}

void edgracbSetGlobalFadeOut(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edui_slider_s *slider = globalfadeinitem;
    edgra_global_fadeout = ((edui_slider_s *)(item))->value;
    if (edgra_global_fadein > edgra_global_fadeout) {
        edgra_global_fadein = edgra_global_fadeout;
        slider->value = edgra_global_fadein;
        slider->normalized_value = (slider->value - slider->minimum) / slider->range;
    }
}

void edgracbCancelGlobalsMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edgra_globals_menu);
    edgra_globals_menu = NULL;
}

void __cdecl edgracbFileSave(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    char name[256];
    char directory[256];
    char extension[256];
    char path[256];
    if (edbits_level_save_directory[0] == 0)
        sprintf(directory, ".");
    else
        strcpy(directory, edbits_level_save_directory);
    if (edbits_level_save_name[0] == 0)
        sprintf(name, "grass");
    else
        strcpy(name, edbits_level_save_name);
    if (edbits_level_save_extension[0] == 0)
        sprintf(extension, "gra");
    else
        strcpy(extension, edbits_level_save_extension);
    sprintf(path, "%s\\%s.%s", directory, name, extension);
    if (edgraFileSave(path))
        eduiCreateMessageMenu(menu, "Saved OK", 1);
    else
        eduiCreateMessageMenu(menu, "File Save Error", 0);
}

void __cdecl edgracbCopyClump(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    struct eduimenu_s *parent = NULL;
    edgra_copy_source = edgra_nearest;
    if (menu->parent != NULL) {
        parent = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->callback != NULL)
        menu->callback(menu, parent);
}

void __cdecl edgracbDiscardCopy(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    struct eduimenu_s *parent = NULL;
    edgra_copy_source = -1;
    if (menu->parent != NULL) {
        parent = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->callback != NULL)
        menu->callback(menu, parent);
}

static void edgracbChangeSScale(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edgra_superscale = (i32)((edui_slider_s *)item)->value;
}

static void edgracbCancelSScaleMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edgra_sscale_menu);
    edgra_sscale_menu = NULL;
}

void __cdecl edgracbSScaleMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    edgra_sscale_menu = eduiMenuCreate(70, 70, 180, 300, ed_fnt, edgracbCancelSScaleMenu, "Super Scale");
    if (edgra_sscale_menu == NULL)
        return;
    eduiMenuAddItem(edgra_sscale_menu, eduiItemSliderCreateInt(0, edblack, 0, edgracbChangeSScale, 1, 99,
                                                               edgra_superscale, "Super Scale"));
    eduiMenuAttach(parent, edgra_sscale_menu);
    edgra_sscale_menu->x = parent->x + 10;
    edgra_sscale_menu->y = parent->y + 40;
}

void __cdecl edgraToggleFilter(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edgra_filter = item->highlighted;
}

void __cdecl edgraChangeFilterName(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    NuStrNCpy(edgra_filter_string, ((edui_textpicker_s *)item)->value, 16);
}

void __cdecl edgracbCancelOptMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    edgra_active_menu = NULL;
    if (edgra_options_menu != NULL) {
        eduiMenuDestroy(edgra_options_menu);
        edgra_options_menu = NULL;
    }
}

static void edgraRender() {
    edcamSet();
    edgraDrawCursor();
    if (edgra_nearest != -1) {
        PC_EDGRACLUMP *clump = &(GrassClumps[edgra_nearest]);
        if ((*clump).kind == 3) {
            if (edgra_editormode == 1) {
                edbitsDrawDiagonalCross((*clump).position.x, (*clump).position.y, (*clump).position.z, 0.125f,
                                        0xffff0000, edgra_mtl);
                if (edgra_nearest_instance != -1) {
                    PC_EDGRAINDIVIDUAL *individual =
                        GetIndGrassClump((*clump).individual_index, edgra_nearest_instance);
                    edbitsDrawCross((*clump).position.x + individual->position.x,
                                    (*clump).position.y + individual->position.y,
                                    (*clump).position.z + individual->position.z, 0.125f, 0xff80ffff, edgra_mtl);
                }
            } else {
                edbitsDrawDiagonalCross((*clump).position.x, (*clump).position.y, (*clump).position.z, 0.125f,
                                        0xff80ffff, edgra_mtl);
            }
        } else {
            edbitsDrawDiagonalCross((*clump).position.x, (*clump).position.y, (*clump).position.z, 0.125f, 0xff80ffff,
                                    edgra_mtl);
            if (((u8)((*clump).unknown_25 - 3)) < 2) {
                edbitsDrawCube((*clump).position.x, (*clump).position.y, (*clump).position.z, (*clump).size, 0.0f,
                               (*clump).size, 0, 0, 0, (*clump).rotation_z, (*clump).rotation_y, 0xff80ffff, edgra_mtl);
            } else {
                edbitsDrawCircleTilted(&(*clump).position, (*clump).size, 0xff80ffff, edgra_mtl, (*clump).rotation_z,
                                       (*clump).rotation_y);
            }
        }
    }
    if (edgra_active_menu)
        eduiMenuRender(edgra_active_menu);
}
