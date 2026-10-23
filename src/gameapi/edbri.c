#include <stdio.h>
#include <string.h>
#include "saga.h"

extern i32 edbri_page_on[8];
extern struct eduimenu_s *edbri_plankinstance_menu;
extern struct eduimenu_s *edbri_postinstance_menu;
void EdFileWrite(void *data, i32 len);
void EdFileWriteInt(i32 value);
void EdFileWriteShort(i16 value);
void EdFileWriteChar(char value);
void EdFileWriteNuVec(NUVEC *value);
void __cdecl edcamMove(FRAME_PAD_PC *pad);
void __cdecl edcamGetPosAng(NUVEC *position, i32 *angle_x, i32 *angle_y);
void edbricbFileLoad(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2);

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

void edbricbSetPlankCount(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edbri_planks = ((i32)(((edui_slider_s *)(item))->value));
}

void edbricbSetPostInterval(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edbri_post_interval = ((i32)(((edui_slider_s *)(item))->value));
}

void edbricbCancelPlankCountMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edbri_plankcount_menu);
    edbri_plankcount_menu = NULL;
}

void edbricbCancelBridgePropertiesMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edbri_bridgeproperties_menu);
    edbri_bridgeproperties_menu = NULL;
}

void edbricbSetDpadMode(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edbri_mode = item->data;
}

void edbricbCancelDpadModeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edbri_dpadmode_menu);
    edbri_dpadmode_menu = NULL;
}

static void edbricbCancelOptMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    edbri_active_menu = NULL;
    if (edbri_options_menu) {
        eduiMenuDestroy(edbri_options_menu);
        edbri_options_menu = NULL;
    }
}
static void edbricbPlankCountMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    edbri_plankcount_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edbricbCancelPlankCountMenu, "Plank Count");
    if (!edbri_plankcount_menu)
        return;
    eduiMenuAddItem(edbri_plankcount_menu,
                    eduiItemSliderCreateInt(0, edblack, 0, edbricbSetPlankCount, 1, 23, edbri_planks, "Plank Count"));
    eduiMenuAddItem(edbri_plankcount_menu, eduiItemSliderCreateInt(0, edblack, 0, edbricbSetPostInterval, 1, 23,
                                                                   edbri_post_interval, "Post Interval"));
    eduiMenuAttach(parent, edbri_plankcount_menu);
    edbri_plankcount_menu->x = parent->x + 10;
    edbri_plankcount_menu->y = parent->y + 40;
}
static void edbricbDpadModeMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    edbri_dpadmode_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edbricbCancelDpadModeMenu, "Dpad Mode");
    if (!edbri_dpadmode_menu)
        return;
    eduiMenuAddItem(edbri_dpadmode_menu,
                    eduiItemCheckCreate(0, edblack, edbri_mode == 0, 1, edbricbSetDpadMode, "Bridge Orient"));
    eduiMenuAddItem(edbri_dpadmode_menu,
                    eduiItemCheckCreate(1, edblack, edbri_mode == 1, 1, edbricbSetDpadMode, "Bridge Size"));
    eduiMenuAttach(parent, edbri_dpadmode_menu);
    edbri_dpadmode_menu->x = parent->x + 10;
    edbri_dpadmode_menu->y = parent->y + 40;
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

void edbriBridgePlace(i32 index, NUVEC *position) {
    PC_EDBRIDGE *bridge = &edBridges[index];
    bridge->position = *position;
    bridge->rotation_z = edbri_rotz;
    bridge->rotation_y = edbri_roty;
    bridge->length = edbri_length;
    bridge->field_14 = edbri_width;
    bridge->field_1d = edbri_planks;
    bridge->field_1e = edbri_post_interval;
    bridge->special_20 = edbri_plank_instance_type;
    bridge->special_24 = edbri_post_instance_type;
    edbriBridgeUpdate(index, edbits_base_scene);
}

void edbriRender() {
    edcamSet();
    edbriDrawCursor();
    if (edbri_nearest != -1) {
        const NUVEC *position = &(edBridges[edbri_nearest].position);
        edbitsDrawDiagonalCross((*position).x, (*position).y, (*position).z, 0.125f, 0xff80ffff, edbri_mtl_zoff);
    }
    if (edbri_active_menu)
        eduiMenuRender(edbri_active_menu);
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

static void edbricbRopeColourMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    PC_EDBRIDGE *bridge;
    edbri_ropecolour_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edbricbCancelRopeColourMenu, "Rope Colour");
    if (!edbri_ropecolour_menu)
        return;
    eduiMenuAddItem(edbri_ropecolour_menu, eduiItemColourPickCreate(0, edblack, edbricbSetRopeColour, "Rope Colour"));
    bridge = &(edBridges[edbri_nearest]);
    eduiItemColourPickSetRGB(((edui_colour_pick_s *)(edui_last_item)), (*bridge).red / 255.0f, (*bridge).green / 255.0f,
                             (*bridge).blue / 255.0f);
    eduiMenuAttach(parent, edbri_ropecolour_menu);
    edbri_ropecolour_menu->x = parent->x + 10;
    edbri_ropecolour_menu->y = parent->y + 40;
}

void edbricbSetBridgeTension(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edBridges[edbri_nearest].field_28 = ((edui_slider_s *)(item))->value;
    edbriBridgeUpdate(edbri_nearest, edbits_base_scene);
}

void edbricbSetBridgeDamp(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edBridges[edbri_nearest].field_2c = ((edui_slider_s *)(item))->value;
    edbriBridgeUpdate(edbri_nearest, edbits_base_scene);
}

void edbricbSetBridgeGravity(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edBridges[edbri_nearest].field_30 = ((edui_slider_s *)(item))->value;
    edbriBridgeUpdate(edbri_nearest, edbits_base_scene);
}

void edbricbSetBridgePlrweight(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edBridges[edbri_nearest].field_34 = ((edui_slider_s *)(item))->value;
    edbriBridgeUpdate(edbri_nearest, edbits_base_scene);
}

void edbricbSetBridgeRopeheight(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edBridges[edbri_nearest].field_38 = ((edui_slider_s *)(item))->value;
    edbriBridgeUpdate(edbri_nearest, edbits_base_scene);
}

void edbricbSetBridgeStability(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edBridges[edbri_nearest].field_3c = ((edui_slider_s *)(item))->value;
    edbriBridgeUpdate(edbri_nearest, edbits_base_scene);
}
static void edbricbBridgePropertiesMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    if (edbri_nearest == -1)
        return;
    edbri_bridgeproperties_menu =
        eduiMenuCreate(70, 70, 180, 250, ed_fnt, edbricbCancelBridgePropertiesMenu, "Bridge Properties");
    if (!edbri_bridgeproperties_menu)
        return;
    eduiMenuAddItem(edbri_bridgeproperties_menu,
                    eduiItemSliderCreate(0, edblack, 0, edbricbSetBridgeTension, 0.0f, 1.0f,
                                         edBridges[edbri_nearest].field_28, "Tension"));
    eduiMenuAddItem(edbri_bridgeproperties_menu, eduiItemSliderCreate(0, edblack, 0, edbricbSetBridgeDamp, 0.0f, 1.0f,
                                                                      edBridges[edbri_nearest].field_2c, "Damp"));
    eduiMenuAddItem(edbri_bridgeproperties_menu,
                    eduiItemSliderCreate(0, edblack, 0, edbricbSetBridgeGravity, -0.1f, 0.2f,
                                         edBridges[edbri_nearest].field_30, "Gravity"));
    eduiMenuAddItem(edbri_bridgeproperties_menu,
                    eduiItemSliderCreate(0, edblack, 0, edbricbSetBridgePlrweight, 0.0f, 10.0f,
                                         edBridges[edbri_nearest].field_34, "Plrweight"));
    eduiMenuAddItem(edbri_bridgeproperties_menu,
                    eduiItemSliderCreate(0, edblack, 0, edbricbSetBridgeRopeheight, 0.0f, 2.0f,
                                         edBridges[edbri_nearest].field_38, "Rope Height"));
    eduiMenuAddItem(edbri_bridgeproperties_menu,
                    eduiItemSliderCreate(0, edblack, 0, edbricbSetBridgeStability, 0.0f, 10.0f,
                                         edBridges[edbri_nearest].field_3c, "Stability"));
    eduiMenuAddItem(edbri_bridgeproperties_menu,
                    eduiItemSelCreate(1, edblack, 0, 0, edbricbRopeColourMenu, "Rope Colour..."));
    eduiMenuAttach(parent, edbri_bridgeproperties_menu);
    edbri_bridgeproperties_menu->x = parent->x + 10;
    edbri_bridgeproperties_menu->y = parent->y + 40;
}

i32 __cdecl edbriFileSave(char *path) {
    NUSPECIALHANDLE special;
    char name[20];
    i32 count = 0;
    i32 i;
    for (i = 0; i < 64; ++i) {
        if ((i8)edBridges[i].connection_index != -1)
            ++count;
    }
    EdFileSetMedia(1);
    if (EdFileOpen(path, 1)) {
        EdFileWriteInt(1);
        EdFileWriteInt(count);
        for (i = 0; i < 64; ++i) {
            if ((i8)edBridges[i].connection_index == -1)
                continue;
            EdFileWriteNuVec(&edBridges[i].position);
            EdFileWriteFloat(edBridges[i].length);
            EdFileWriteFloat(edBridges[i].field_14);
            EdFileWriteShort(edBridges[i].rotation_z);
            EdFileWriteShort(edBridges[i].rotation_y);
            EdFileWriteChar(edBridges[i].field_1d);
            EdFileWriteChar(edBridges[i].field_1e);
            if (edBridges[i].special_20 == -1) {
                name[0] = 0;
            } else {
                NuGScnGetSpecial(&special, edbits_base_scene, edBridges[i].special_20);
                strncpy(name, NuSpecialGetName(&special), 20);
            }
            name[19] = 0;
            EdFileWrite(name, 20);
            if (edBridges[i].special_24 == -1) {
                name[0] = 0;
            } else {
                NuGScnGetSpecial(&special, edbits_base_scene, edBridges[i].special_24);
                strncpy(name, NuSpecialGetName(&special), 20);
            }
            name[19] = 0;
            EdFileWrite(name, 20);
            EdFileWriteFloat(edBridges[i].field_28);
            EdFileWriteFloat(edBridges[i].field_2c);
            EdFileWriteFloat(edBridges[i].field_30);
            EdFileWriteFloat(edBridges[i].field_34);
            EdFileWriteFloat(edBridges[i].field_38);
            EdFileWriteFloat(edBridges[i].field_3c);
            EdFileWriteChar(edBridges[i].red);
            EdFileWriteChar(edBridges[i].green);
            EdFileWriteChar(edBridges[i].blue);
            EdFileWriteChar(edBridges[i].field_43);
        }
        EdFileClose();
        return 1;
    }
    return 0;
}

void __cdecl edbricbFileSave(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    char name[256];
    char directory[256];
    char extension[256];
    char path[256];
    if (edbits_level_save_directory[0] == 0)
        sprintf(directory, ".");
    else
        strcpy(directory, edbits_level_save_directory);
    if (edbits_level_save_name[0] == 0)
        sprintf(name, "bridge");
    else
        strcpy(name, edbits_level_save_name);
    if (edbits_level_save_extension[0] == 0)
        sprintf(extension, "bri");
    else
        strcpy(extension, edbits_level_save_extension);
    sprintf(path, "%s\\%s.%s", directory, name, extension);
    if (edbriFileSave(path))
        eduiCreateMessageMenu(menu, "Saved OK", 1);
    else
        eduiCreateMessageMenu(menu, "File Save Error", 0);
}

static void edbricbSetPostInstanceType(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edbri_post_instance_type = item->data;
}

static void edbricbCancelPostInstanceMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edbri_postinstance_menu);
    edbri_postinstance_menu = NULL;
}

void __cdecl edbricbPostInstanceMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    i32 count, i, selected;
    edbri_postinstance_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edbricbCancelPostInstanceMenu, "Post Select");
    if (!edbri_postinstance_menu || !edbits_base_scene)
        return;
    eduiMenuAddItem(edbri_postinstance_menu, eduiItemCheckCreate((size_t)(-1), edblack, edbri_post_instance_type == -1,
                                                                 1, edbricbSetPostInstanceType, "None"));
    count = NuSpecialGetNumSpecials(edbits_base_scene);
    for (i = 0; i < count; ++i) {
        NUSPECIALHANDLE special;
        selected = edbri_post_instance_type == i;
        NuGScnGetSpecial(&special, edbits_base_scene, i);
        eduiMenuAddItem(
            edbri_postinstance_menu,
            eduiItemCheckCreate(i, edblack, selected, 1, edbricbSetPostInstanceType, NuSpecialGetName(&special)));
        if (selected == 1)
            edbri_postinstance_menu->selected = edui_last_item;
    }
    eduiMenuAttach(parent, edbri_postinstance_menu);
    edbri_postinstance_menu->x = parent->x + 10;
    edbri_postinstance_menu->y = parent->y + 40;
}

static void edbricbSetPlankInstanceType(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edbri_plank_instance_type = item->data;
}

static void edbricbCancelPlankInstanceMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edbri_plankinstance_menu);
    edbri_plankinstance_menu = NULL;
}

void __cdecl edbricbPlankInstanceMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    i32 count, i, selected;
    edbri_plankinstance_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edbricbCancelPlankInstanceMenu, "Plank Select");
    if (!edbri_plankinstance_menu || !edbits_base_scene)
        return;
    eduiMenuAddItem(edbri_plankinstance_menu,
                    eduiItemCheckCreate((size_t)(-1), edblack, edbri_plank_instance_type == -1, 1,
                                        edbricbSetPlankInstanceType, "None"));
    count = NuSpecialGetNumSpecials(edbits_base_scene);
    for (i = 0; i < count; ++i) {
        NUSPECIALHANDLE special;
        selected = edbri_plank_instance_type == i;
        NuGScnGetSpecial(&special, edbits_base_scene, i);
        eduiMenuAddItem(
            edbri_plankinstance_menu,
            eduiItemCheckCreate(i, edblack, selected, 1, edbricbSetPlankInstanceType, NuSpecialGetName(&special)));
        if (selected == 1)
            edbri_plankinstance_menu->selected = edui_last_item;
    }
    eduiMenuAttach(parent, edbri_plankinstance_menu);
    edbri_plankinstance_menu->x = parent->x + 10;
    edbri_plankinstance_menu->y = parent->y + 40;
}

i32 __cdecl edbriBridgeCreate(NUVEC *position) {
    i32 index = 0;
    if (edbri_bridges_used == 64)
        return -1;
    while (edBridges[index].connection_index != 0xff)
        ++index;
    edBridges[index].connection_index = edbri_pageid;
    ++edbri_bridges_used;
    edBridges[index].field_28 = 0.18f;
    edBridges[index].field_2c = 0.1f;
    edBridges[index].field_30 = -0.01f;
    edBridges[index].field_34 = 4.0f;
    edBridges[index].field_38 = 0.5f;
    edBridges[index].field_3c = 0.5f;
    edBridges[index].red = 0x50;
    edBridges[index].green = 0x50;
    edBridges[index].blue = 0x30;
    edBridges[index].field_43 = 0x80;
    edbri_page_used[edbri_pageid] = 1;
    edbri_page_scene[edbri_pageid] = edbits_base_scene;
    edbriBridgePlace(index, position);
    return index;
}

void __cdecl edbriBridgeDestroy(i32 index) {
    NuBridgeRemove(edBridges[index].instance_id);
    --edbri_bridges_used;
    edBridges[index].instance_id = -1;
    edBridges[index].connection_index = -1;
}

void __cdecl edbriCreateOptionsMenu(void) {
    edbri_options_menu = eduiMenuCreate(70, 70, 220, 300, ed_fnt, edbricbCancelOptMenu, "Options");
    if (!edbri_options_menu)
        return;
    eduiMenuAddItem(edbri_options_menu,
                    eduiItemSelCreate(1, edblack, 0, 0, edbricbPlankInstanceMenu, "Plank Instance..."));
    eduiMenuAddItem(edbri_options_menu,
                    eduiItemSelCreate(1, edblack, 0, 0, edbricbPostInstanceMenu, "Post Instance..."));
    eduiMenuAddItem(edbri_options_menu, eduiItemSelCreate(1, edblack, 0, 0, edbricbPlankCountMenu, "Plank Counts..."));
    eduiMenuAddItem(edbri_options_menu, eduiItemSelCreate(1, edblack, 0, 0, edbricbDpadModeMenu, "Dpad Mode..."));
    if (edbri_nearest == -1)
        eduiMenuAddItem(edbri_options_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Bridge Properties..."));
    else
        eduiMenuAddItem(edbri_options_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edbricbBridgePropertiesMenu, "Bridge Properties..."));
    eduiMenuAddItem(edbri_options_menu, eduiItemSelCreate(1, edblack, 0, 0, edbricbFileSave, "Save Bridges"));
    eduiMenuAddItem(edbri_options_menu, eduiItemSelCreate(1, edblack, 0, 0, edbricbFileLoad, "Load Bridges"));
}

void __cdecl edbriDoInput(FRAME_PAD_PC *pad) {
    i32 index;
    if ((pad->digital_buttons & 0x100) == 0)
        edcamMove(pad);
    if (pad->digital_buttons & 0x100) {
        if (edbri_nearest != -1) {
            index = edbri_nearest;
            if (pad->digital_buttons_pressed & 8) {
                do {
                    ++index;
                    if (index == 64)
                        index = 0;
                } while (edBridges[index].instance_id == -1);
                edbri_nearest = index;
            }
            if (pad->digital_buttons_pressed & 2) {
                do {
                    --index;
                    if (index == -1)
                        index = 63;
                } while (edBridges[index].instance_id == -1);
                edbri_nearest = index;
            }
        } else {
            edbriDetermineNearest(-1.0f);
        }
        if (edbri_nearest != -1) {
            edcamSetPos(&edBridges[edbri_nearest].position);
            edbri_rotz = edBridges[edbri_nearest].rotation_z;
            edbri_roty = edBridges[edbri_nearest].rotation_y;
            edbri_length = edBridges[edbri_nearest].length;
            edbri_width = edBridges[edbri_nearest].field_14;
            edbri_planks = edBridges[edbri_nearest].field_1d;
            edbri_post_interval = edBridges[edbri_nearest].field_1e;
            edbri_plank_instance_type = edBridges[edbri_nearest].special_20;
            edbri_post_instance_type = edBridges[edbri_nearest].special_24;
        }
    }
    edcamGetPosAng(&edbri_cam_pos, &edbri_cam_ax, &edbri_cam_ay);
    if ((pad->digital_buttons & 0x100) == 0) {
        if (pad->digital_buttons_pressed & 0x80) {
            edbriCreateOptionsMenu();
            edbri_active_menu = edbri_options_menu;
        }
        if ((pad->digital_buttons_pressed & 0x40) && edbri_plank_instance_type != -1)
            edbriBridgeCreate(&edbri_cam_pos);
        if ((pad->digital_buttons & 0x20) && edbri_nearest != -1)
            edbriBridgePlace(edbri_nearest, &edbri_cam_pos);
        if (pad->digital_buttons_pressed & 0x10) {
            if (edbri_nearest != -1)
                edbriBridgeDestroy(edbri_nearest);
            edbri_nearest = -1;
        }
    }
    if (edbri_mode == 0) {
        edbri_roty += pad->analog_left_pad_right;
        edbri_roty -= pad->analog_left_pad_left;
        edbri_rotz += pad->analog_left_pad_up;
        if (edbri_rotz > 0x4000)
            edbri_rotz = 0x2000;
        edbri_rotz -= pad->analog_left_pad_down;
        if (edbri_rotz < -0x4000)
            edbri_rotz = -0x2000;
    } else if (edbri_mode == 1) {
        edbri_length += pad->analog_left_pad_up / 2500.0;
        edbri_length -= pad->analog_left_pad_down / 2500.0;
        if (edbri_length < 0.1f)
            edbri_length = 0.1f;
        if (edbri_length > 20.0f)
            edbri_length = 20.0f;
        edbri_width += pad->analog_left_pad_right / 5000.0;
        edbri_width -= pad->analog_left_pad_left / 5000.0;
        if (edbri_width < 0.1f)
            edbri_width = 0.1f;
        if (edbri_width > 20.0f)
            edbri_width = 5.0f;
    }
}

void __cdecl edbricbCancelRopeColourMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edbri_ropecolour_menu);
    edbri_ropecolour_menu = NULL;
}

void __cdecl edbricbSetRopeColour(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
    edui_colour_pick_s *colour = (edui_colour_pick_s *)item;
    eduimenu_s *parent = NULL;
    edBridges[edbri_nearest].red = (u8)(colour->red * 255.0);
    edBridges[edbri_nearest].green = (u8)(colour->green * 255.0);
    edBridges[edbri_nearest].blue = (u8)(colour->blue * 255.0);
    edbriBridgeUpdate(edbri_nearest, edbits_base_scene);
    if (menu->parent) {
        parent = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->callback)
        menu->callback(menu, parent);
}
