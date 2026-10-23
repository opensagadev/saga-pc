#include <stdio.h>
#include <string.h>
#include "saga.h"

extern struct eduimenu_s *edgra_sscale_menu;
void EdFileWrite(void *data, i32 len);
void EdFileWriteInt(i32 value);
void EdFileWriteShort(i16 value);
void EdFileWriteChar(char value);
void EdFileWriteNuVec(NUVEC *value);
extern struct PC_SCENE_MTL_VIEW *edgra_mtl_zoff;
void __cdecl edbitsDrawCube(f32 x, f32 y, f32 z, f32 half_x, f32 half_y, f32 half_z, i32 rotation_z, i32 rotation_y,
                            i32 rotation_x, i32 outer_z, i32 outer_y, i32 colour, NUMTL *material);
void __cdecl edbitsDrawOvalTilted(NUVEC *centre, f32 radius_x, f32 radius_z, i32 colour,
                                  struct PC_SCENE_MTL_VIEW *material, i32 rotation_z, i32 rotation_y);

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

static __inline void edgraSliderSetValue(edui_slider_s *slider, f32 value) {
    slider->value = value;
    slider->normalized_value = (value - slider->minimum) / slider->range;
}

void edgracbSetGlobalFadeIn(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edgra_global_fadein = ((edui_slider_s *)(item))->value;
    if (((edui_slider_s *)(item))->value > edgra_global_fadeout) {
        edui_slider_s *slider = globalfadeoutitem;
        edgra_global_fadeout = ((edui_slider_s *)(item))->value;
        edgraSliderSetValue(slider, ((edui_slider_s *)(item))->value);
    }
}

void edgracbSetGlobalFadeOut(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edgra_global_fadeout = ((edui_slider_s *)(item))->value;
    if (((edui_slider_s *)(item))->value < edgra_global_fadein) {
        edui_slider_s *slider = globalfadeinitem;
        edgra_global_fadein = ((edui_slider_s *)(item))->value;
        edgraSliderSetValue(slider, ((edui_slider_s *)(item))->value);
    }
}

void edgracbCancelGlobalsMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edgra_globals_menu);
    edgra_globals_menu = NULL;
}

void __cdecl edgraDrawCursor(void) {
    NURND_VERTEX3D_PC line[2];
    NUVEC arrow;
    NUSPECIALHANDLE special;
    line[0].colour = 0xffffffff;
    line[1].colour = 0xffffffff;
    line[0].position.x = edgra_cam_pos.x - 0.5f;
    line[0].position.y = edgra_cam_pos.y;
    line[0].position.z = edgra_cam_pos.z;
    line[1].position.x = edgra_cam_pos.x + 0.5f;
    line[1].position.y = edgra_cam_pos.y;
    line[1].position.z = edgra_cam_pos.z;
    NuRndrLine3d(line, (NUMTL *)edgra_mtl, NULL);
    line[0].colour = 0xffffffff;
    line[1].colour = 0xffffffff;
    line[0].position.x = edgra_cam_pos.x;
    line[0].position.y = edgra_cam_pos.y - 0.5f;
    line[0].position.z = edgra_cam_pos.z;
    line[1].position.x = edgra_cam_pos.x;
    line[1].position.y = edgra_cam_pos.y + 0.5f;
    line[1].position.z = edgra_cam_pos.z;
    NuRndrLine3d(line, (NUMTL *)edgra_mtl, NULL);
    line[0].colour = 0xffffffff;
    line[1].colour = 0xffffffff;
    line[0].position.x = edgra_cam_pos.x;
    line[0].position.y = edgra_cam_pos.y;
    line[0].position.z = edgra_cam_pos.z - 0.5f;
    line[1].position.x = edgra_cam_pos.x;
    line[1].position.y = edgra_cam_pos.y;
    line[1].position.z = edgra_cam_pos.z + 0.5f;
    NuRndrLine3d(line, (NUMTL *)edgra_mtl, NULL);
    arrow.x = 0.0f;
    arrow.y = 0.5f;
    arrow.z = 0.0f;
    NuVecRotateZ(&arrow, &arrow, edgra_rotz);
    NuVecRotateY(&arrow, &arrow, edgra_roty);
    line[0].colour = 0xff0000ff;
    line[1].colour = 0xff0000ff;
    line[0].position.x = edgra_cam_pos.x;
    line[0].position.y = edgra_cam_pos.y;
    line[0].position.z = edgra_cam_pos.z;
    line[1].position.x = edgra_cam_pos.x + arrow.x;
    line[1].position.y = edgra_cam_pos.y + arrow.y;
    line[1].position.z = edgra_cam_pos.z + arrow.z;
    NuRndrLine3d(line, (NUMTL *)edgra_mtl, NULL);
    if (edgra_nearest == -1)
        edbitsDrawOvalTilted(&edgra_cam_pos, edgra_size, edgra_size, 0xff0000ff, edgra_mtl, edgra_rotz, edgra_roty);
    else if (GrassClumps[edgra_nearest].kind == 3)
        edbitsDrawOvalTilted(&edgra_cam_pos, 0.5f, 0.5f, 0xff0000ff, edgra_mtl, edgra_rotz, edgra_roty);
    else if (GrassClumps[edgra_nearest].unknown_25 == 3 || GrassClumps[edgra_nearest].unknown_25 == 4)
        edbitsDrawCube(edgra_cam_pos.x, edgra_cam_pos.y, edgra_cam_pos.z, edgra_size, 0.0f, edgra_size, 0, 0, 0,
                       edgra_rotz, edgra_roty, 0xff0000ff, (NUMTL *)edgra_mtl);
    else
        edbitsDrawCircleTilted(&edgra_cam_pos, edgra_size, 0xff0000ff, edgra_mtl, edgra_rotz, edgra_roty);
    NuRndrRect2di(0x1720, 0x9b0, 0xdc0, 0x410, 0x80808080, (NUMTL *)edgra_mtl_zoff);
    NuRndrRect2di(0x1710, 0x8f0, 0xde0, 0xc0, 0x80000000, (NUMTL *)edgra_mtl_zoff);
    NuRndrLine2di(0x1710, 0x9b0, 0x1710, 0xdc8, 0x80000000, (NUMTL *)edgra_mtl_zoff);
    NuRndrLine2di(0x24f0, 0x9b0, 0x24f0, 0xdc8, 0x80000000, (NUMTL *)edgra_mtl_zoff);
    NuRndrLine2di(0x1710, 0xdc8, 0x24f0, 0xdc8, 0x80000000, (NUMTL *)edgra_mtl_zoff);
    NuQFntPushPrintMode(2);
    NuQFntPushCoordinateSystem(NUQFNT_CSMODE_PS2);
    NuQFntSet(system_qfont);
    NuQFntSetColour(system_qfont, 0xe0e0e0e0);
    NuQFntPrintEx(system_qfont, 0x17c0, 0x988, 0x10, "Info Box");
    NuQFntSetColour(system_qfont, 0x80000000);
    if (edgra_copy_source != -1) {
        NuQFntPrintEx(system_qfont, 0x17c0, 0xa50, 0x10, "Copy Clump Mode");
    } else if (edgra_instance_type == -1) {
        NuQFntPrintEx(system_qfont, 0x17c0, 0xa50, 0x10, "Curr Inst: <none>");
    } else {
        NuGScnGetSpecial(&special, edbits_base_scene, edgra_instance_type);
        NuQFntPrintEx(system_qfont, 0x17c0, 0xa50, 0x10, "Curr Inst: %s", NuSpecialGetName(&special));
    }
    if (edgra_mode == 3) {
        if (edgra_nearest != -1)
            NuQFntPrintEx(system_qfont, 0x17c0, 0xb90, 0x10, "Clump Size: %d/%d",
                          GrassClumps[edgra_nearest].element_count, EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP);
        else
            NuQFntPrintEx(system_qfont, 0x17c0, 0xb90, 0x10, "Clump Size: -");
    } else {
        NuQFntPrintEx(system_qfont, 0x17c0, 0xaf0, 0x10, "Base Size: %0.2f", edgra_size);
        NuQFntPrintEx(system_qfont, 0x17c0, 0xb90, 0x10, "Clump Size: %d", edgra_clump_size);
    }
    NuQFntPrintEx(system_qfont, 0x17c0, 0xc30, 0x10, "Used: %d/%d,%d/%d", edgra_units_used, 0x3000, edgra_clumps_used,
                  EDGRA_MAX_CLUMPS);
    if (edgra_dpadmode == 0)
        NuQFntPrintEx(system_qfont, 0x17c0, 0xcd0, 0x10, "Dpad Mode: Size");
    else if (edgra_dpadmode == 1)
        NuQFntPrintEx(system_qfont, 0x17c0, 0xcd0, 0x10, "Dpad Mode: Tilt");
    NuQFntPrintEx(system_qfont, 0x17c0, 0xd70, 0x10, "%5.2f", edgra_cam_pos.x);
    NuQFntPrintEx(system_qfont, 0x1c20, 0xd70, 0x10, "%5.2f", edgra_cam_pos.y);
    NuQFntPrintEx(system_qfont, 0x2080, 0xd70, 0x10, "%5.2f", edgra_cam_pos.z);
    NuQFntPopPrintMode();
    NuQFntPopCoordinateSystem();
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

void __cdecl edgraRender(void) {
    edcamSet();
    edgraDrawCursor();
    if (edgra_nearest != -1) {
        if (GrassClumps[edgra_nearest].kind == 3) {
            if (edgra_editormode == 1) {
                edbitsDrawDiagonalCross(GrassClumps[edgra_nearest].position.x, GrassClumps[edgra_nearest].position.y,
                                        GrassClumps[edgra_nearest].position.z, 0.125f, 0xffff0000, edgra_mtl);
                if (edgra_nearest_instance != -1) {
                    edbitsDrawCross(
                        GrassClumps[edgra_nearest].position.x +
                            EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, edgra_nearest_instance)
                                ->position.x,
                        GrassClumps[edgra_nearest].position.y +
                            EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, edgra_nearest_instance)
                                ->position.y,
                        GrassClumps[edgra_nearest].position.z +
                            EDGRA_INDIVIDUAL(GrassClumps[edgra_nearest].individual_index, edgra_nearest_instance)
                                ->position.z,
                        0.125f, 0xff80ffff, edgra_mtl);
                }
            } else {
                edbitsDrawDiagonalCross(GrassClumps[edgra_nearest].position.x, GrassClumps[edgra_nearest].position.y,
                                        GrassClumps[edgra_nearest].position.z, 0.125f, 0xff80ffff, edgra_mtl);
            }
        } else {
            edbitsDrawDiagonalCross(GrassClumps[edgra_nearest].position.x, GrassClumps[edgra_nearest].position.y,
                                    GrassClumps[edgra_nearest].position.z, 0.125f, 0xff80ffff, edgra_mtl);
            if (GrassClumps[edgra_nearest].unknown_25 == 3 || GrassClumps[edgra_nearest].unknown_25 == 4) {
                edbitsDrawCube(GrassClumps[edgra_nearest].position.x, GrassClumps[edgra_nearest].position.y,
                               GrassClumps[edgra_nearest].position.z, GrassClumps[edgra_nearest].size, 0.0f,
                               GrassClumps[edgra_nearest].size, 0, 0, 0, GrassClumps[edgra_nearest].rotation_z,
                               GrassClumps[edgra_nearest].rotation_y, 0xff80ffff, (NUMTL *)edgra_mtl);
            } else {
                edbitsDrawCircleTilted(&GrassClumps[edgra_nearest].position, GrassClumps[edgra_nearest].size,
                                       0xff80ffff, edgra_mtl, GrassClumps[edgra_nearest].rotation_z,
                                       GrassClumps[edgra_nearest].rotation_y);
            }
        }
    }
    if (edgra_active_menu)
        eduiMenuRender(edgra_active_menu);
}
