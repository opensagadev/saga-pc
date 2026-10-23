#include <stdio.h>
#include "saga.h"
typedef struct part_runtime_type_pc_s {
    char name[16];
    char object_names[8][16];
    i16 effect_ids[8];
    i8 effect_pages[8];
    i8 scene_indices[8];
    i8 variant_count;
    u8 variant_mode;
    i8 page;
    u8 field_b3;
    f32 particle_scale;
    f32 effect_scale;
    f32 lifetime;
    f32 lifetime_random;
    f32 speed;
    f32 gravity;
    f32 emission_rate;
    f32 bounce;
    NUVEC position_random;
    NUVEC velocity_random;
    f32 emission_period;
    f32 emission_period_random;
    f32 emission_pause;
    f32 emission_pause_random;
    i32 rotation[3];
    i32 rotation_random[3];
    u32 flags;
    i32 trail_effects[2];
    f32 trail_rates[2];
    i32 attached_effect;
    i32 impact_effect;
    i32 impact_part;
    char impact_part_name[16];
    i32 kill_effect;
    i32 sounds[4];
    u8 sound_modes[4];
    f32 maximum_distance;
    f32 field_160, field_164, field_168;
    f32 last_used_time;
    f32 scale;
    u32 field_174;
} PART_RUNTIME_TYPE_PC;
STATIC_ASSERT(sizeof(PART_RUNTIME_TYPE_PC) == 0x178);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, name) == 0x0);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, object_names) == 0x10);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, effect_ids) == 0x90);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, effect_pages) == 0xa0);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, scene_indices) == 0xa8);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, variant_count) == 0xb0);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, variant_mode) == 0xb1);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, page) == 0xb2);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, field_b3) == 0xb3);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, particle_scale) == 0xb4);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, effect_scale) == 0xb8);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, lifetime) == 0xbc);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, lifetime_random) == 0xc0);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, speed) == 0xc4);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, gravity) == 0xc8);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, emission_rate) == 0xcc);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, bounce) == 0xd0);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, position_random) == 0xd4);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, velocity_random) == 0xe0);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, emission_period) == 0xec);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, emission_period_random) == 0xf0);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, emission_pause) == 0xf4);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, emission_pause_random) == 0xf8);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, rotation) == 0xfc);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, rotation_random) == 0x108);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, flags) == 0x114);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, trail_effects) == 0x118);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, trail_rates) == 0x120);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, attached_effect) == 0x128);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, impact_effect) == 0x12c);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, impact_part) == 0x130);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, impact_part_name) == 0x134);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, kill_effect) == 0x144);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, sounds) == 0x148);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, sound_modes) == 0x158);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, maximum_distance) == 0x15c);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, field_160) == 0x160);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, field_164) == 0x164);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, field_168) == 0x168);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, last_used_time) == 0x16c);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, scale) == 0x170);
STATIC_ASSERT(offsetof(PART_RUNTIME_TYPE_PC, field_174) == 0x174);
void __cdecl edpartScaleType(i32 index, f32 scale) {
    PART_RUNTIME_TYPE_PC *type = &((PART_RUNTIME_TYPE_PC *)part_types)[index];
    type->particle_scale *= scale;
    type->effect_scale *= scale;
    type->speed *= scale;
    type->gravity *= scale;
    type->position_random.x *= scale;
    type->position_random.y *= scale;
    type->position_random.z *= scale;
    type->velocity_random.x *= scale;
    type->velocity_random.y *= scale;
    type->velocity_random.z *= scale;
    type->maximum_distance *= scale;
}
void edpartApplyScaleType(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    struct eduimenu_s *parent;
    parent = NULL;
    if (edpart_nearest_type)
        edpartScaleType(edpart_nearest_emit->effect_id, edpart_scale_factor);
    if (menu->parent) {
        parent = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->callback)
        menu->callback(menu, parent);
}

void edpartCancelScaleTypeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_scaletype_menu);
    edpart_scaletype_menu = NULL;
}

void edpartCancelTypeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_type_menu);
    edpart_type_menu = NULL;
}

static void edpartWorldInstanceMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    i32 group;
    i32 selected_first;
    i32 count;
    i32 index;
    selected_first = 0;
    edpart_which_scene = 0;
    edpart_worldinstance_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelWorldInstanceMenu,
                                               ((char *)(edpart_filter ? "World Scene (Filtered)" : "World Scene")));
    if (edpart_worldinstance_menu == NULL || edbits_base_scene == NULL || edpart_nearest_type == NULL)
        return;
    group = 1;
    count = NuGScnNumSpecials(edbits_base_scene);
    for (index = 0; index < count; ++index) {
        char *name;
        i32 selected;
        i32 included;
        i32 variant;
        nuhspecial_s special;
        selected = 0;
        included = 0;
        name = NULL;
        NuGScnGetSpecial(&special, edbits_base_scene, index);
        if (NuSpecialExistsFn(&special))
            name = NuSpecialGetName(&special);
        for (variant = 0; variant < edpart_nearest_type->variant_count; ++variant) {
            if (edpart_nearest_type->effect_ids[variant] == index && edpart_nearest_type->effect_pages[variant] == 0) {
                selected = 1;
                included = 1;
            }
        }
        if (!edpart_filter || NuStrNCmp(edpart_filter_string, name, NuStrLen(edpart_filter_string)) == 0 || included) {
            eduiMenuAddItem(edpart_worldinstance_menu,
                            eduiItemToggleCreate(index, edblack, selected, group, edpartSetInstanceType, name));
            group++;
            if (selected && !selected_first) {
                selected_first = 1;
                edpart_worldinstance_menu->selected = edui_last_item;
            }
        }
    }
    if (group == 1)
        eduiMenuAddItem(edpart_worldinstance_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "All Instances Filtered"));
    eduiMenuAttach(parent, edpart_worldinstance_menu);
    edpart_worldinstance_menu->x = parent->x + 10;
    edpart_worldinstance_menu->y = parent->y + 40;
}

void edpartCancelThingsInstanceMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_thingsinstance_menu);
    edpart_thingsinstance_menu = NULL;
}

void edpartCancelCutOffMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_cutoff_menu);
    edpart_cutoff_menu = NULL;
}

static void edpartChangeInstanceRot(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type != NULL) {
        if (item->data == 0)
            edpart_nearest_type->rotation[0] = ((i32)(((edui_slider_s *)(item))->value * (65536.0f / 360.0f)));
        else if (item->data == 1)
            edpart_nearest_type->rotation[1] = ((i32)(((edui_slider_s *)(item))->value * (65536.0f / 360.0f)));
        else if (item->data == 2)
            edpart_nearest_type->rotation[2] = ((i32)(((edui_slider_s *)(item))->value * (65536.0f / 360.0f)));
    }
}

static void edpartChangeInstanceVarRot(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type != NULL) {
        if (item->data == 0)
            edpart_nearest_type->rotation_random[0] = ((i32)(((edui_slider_s *)(item))->value * (65536.0f / 360.0f)));
        else if (item->data == 1)
            edpart_nearest_type->rotation_random[1] = ((i32)(((edui_slider_s *)(item))->value * (65536.0f / 360.0f)));
        else if (item->data == 2)
            edpart_nearest_type->rotation_random[2] = ((i32)(((edui_slider_s *)(item))->value * (65536.0f / 360.0f)));
    }
}

static void edpartCancelInstanceOrientMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_instorient_menu);
    edpart_instorient_menu = NULL;
}
void edpartInstanceOrientMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type == NULL)
        return;
    edpart_instorient_menu =
        eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelInstanceOrientMenu, "Instance Orientation");
    if (edpart_instorient_menu == NULL)
        return;
#define EDPART_ORIENT_SLIDER(axis, field, callback, minimum, maximum, label)                                           \
    do {                                                                                                               \
        eduiMenuAddItem(edpart_instorient_menu,                                                                        \
                        eduiItemSliderCreate(axis, edblack, 0, callback, minimum, maximum,                             \
                                             edpart_nearest_type->field[axis] * (360.0f / 65536.0f), label));          \
        eduiItemSliderSetFmt(((edui_slider_s *)(edui_last_item)), "(%1.01f)");                                         \
        eduiItemSliderSetGranularity(((edui_slider_s *)(edui_last_item)), 0.1f);                                       \
    } while (0)
    EDPART_ORIENT_SLIDER(0, rotation, edpartChangeInstanceRot, edpart_superscale * -180.0f, edpart_superscale * 360.0f,
                         "Base Rotation X");
    EDPART_ORIENT_SLIDER(1, rotation, edpartChangeInstanceRot, edpart_superscale * -180.0f, edpart_superscale * 360.0f,
                         "Base Rotation Y");
    EDPART_ORIENT_SLIDER(2, rotation, edpartChangeInstanceRot, edpart_superscale * -180.0f, edpart_superscale * 360.0f,
                         "Base Rotation Z");
    EDPART_ORIENT_SLIDER(0, rotation_random, edpartChangeInstanceVarRot, 0.0f, edpart_superscale * 180.0f,
                         "Rotation Variation X");
    EDPART_ORIENT_SLIDER(1, rotation_random, edpartChangeInstanceVarRot, 0.0f, edpart_superscale * 180.0f,
                         "Rotation Variation Y");
    EDPART_ORIENT_SLIDER(2, rotation_random, edpartChangeInstanceVarRot, 0.0f, edpart_superscale * 180.0f,
                         "Rotation Variation Z");
#undef EDPART_ORIENT_SLIDER
    eduiMenuAttach(menu, edpart_instorient_menu);
    edpart_instorient_menu->x = menu->x + 10;
    edpart_instorient_menu->y = menu->y + 40;
}
void edpartChangeInstanceFlag(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type != NULL) {
        if (item->highlighted)
            edpart_nearest_type->flags |= item->data;
        else
            edpart_nearest_type->flags &= ~item->data;
    }
}
void edpartChangeGrav(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type)
        edpart_nearest_type->gravity = ((edui_slider_s *)(item))->value * 2.0f;
}
void edpartChangeBounce(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type)
        edpart_nearest_type->bounce = ((edui_slider_s *)(item))->value;
}

void edpartCancelGravMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_grav_menu);
    edpart_grav_menu = NULL;
}
void edpartScaleTypeMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest != -1) {
        edpart_scaletype_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edpartCancelScaleTypeMenu, "Scale Type");
        if (edpart_scaletype_menu != NULL) {
            eduiMenuAddItem(edpart_scaletype_menu, eduiItemSliderCreate(0, edblack, 0, edpartSetScaleFactor, 0.1f, 9.9f,
                                                                        edpart_scale_factor, "Scale Factor"));
            eduiMenuAddItem(edpart_scaletype_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartApplyScaleType, "Apply Scale to Type"));
            eduiMenuAttach(menu, edpart_scaletype_menu);
            edpart_scaletype_menu->x = menu->x + 10;
            edpart_scaletype_menu->y = menu->y + 40;
        }
    }
}
extern i8 edpart_effect_list;

void __cdecl edpartInitType(i32 index) {
    i32 i;
    for (i = 0; i < 8; ++i) {
        ((PART_RUNTIME_TYPE_PC *)part_types)[index].effect_ids[i] = -1;
        ((PART_RUNTIME_TYPE_PC *)part_types)[index].effect_pages[i] = 1;
    }
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].variant_count = 0;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].lifetime = 0.0f;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].speed = 1.0f;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].gravity = 0.0f;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].emission_rate = 1.0f;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].emission_period = 1.0f;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].emission_period_random = 0.0f;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].emission_pause = 0.0f;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].emission_pause_random = 0.0f;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].rotation[0] = 0;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].rotation[1] = 0;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].rotation[2] = 0;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].flags = 0;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].trail_effects[0] = -1;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].trail_effects[1] = -1;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].attached_effect = -1;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].kill_effect = -1;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].impact_effect = -1;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].impact_part = -1;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].last_used_time = 0.0f;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].field_174 = -1;
    ((PART_RUNTIME_TYPE_PC *)part_types)[index].scale = 1.0f;
    for (i = 0; i < 4; ++i) {
        ((PART_RUNTIME_TYPE_PC *)part_types)[index].sounds[i] = -1;
        ((PART_RUNTIME_TYPE_PC *)part_types)[index].sound_modes[i] = 0;
    }
}

#define EDPART_ADD_TYPE(menu, list)                                                                                    \
    do {                                                                                                               \
        struct eduimenu_s *parent = NULL;                                                                              \
        i32 index, i;                                                                                                  \
        if (part_types_used < 128) {                                                                                   \
            for (index = 0; index < 128; ++index) {                                                                    \
                if (((PART_RUNTIME_TYPE_PC *)part_types)[index].name[0] == 0) {                                        \
                    sprintf(((PART_RUNTIME_TYPE_PC *)part_types)[index].name, "New%d", index);                         \
                    edpartInitType(index);                                                                             \
                    ++part_types_used;                                                                                 \
                    part_scene[(list)] = (list) == 0 ? edbits_things_scene : edbits_base_scene;                        \
                    ((PART_RUNTIME_TYPE_PC *)part_types)[index].field_b3 = (list);                                     \
                    ((PART_RUNTIME_TYPE_PC *)part_types)[index].page = (list);                                         \
                    part_page_on[(list)] = 1;                                                                          \
                    part_page_used[(list)] = 1;                                                                        \
                    part_scene_pageid[(list)] = (list);                                                                \
                    edpart_create_type = index;                                                                        \
                    for (i = 0; i < 8; ++i) {                                                                          \
                        ((PART_RUNTIME_TYPE_PC *)part_types)[index].effect_ids[i] = -1;                                \
                        ((PART_RUNTIME_TYPE_PC *)part_types)[index].effect_pages[i] = -1;                              \
                    }                                                                                                  \
                    ((PART_RUNTIME_TYPE_PC *)part_types)[index].field_160 = 1.0f;                                      \
                    ((PART_RUNTIME_TYPE_PC *)part_types)[index].field_164 = 1.0f;                                      \
                    ((PART_RUNTIME_TYPE_PC *)part_types)[index].field_168 = 1.0f;                                      \
                    break;                                                                                             \
                }                                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
        if ((menu)->parent) {                                                                                          \
            parent = (menu)->parent;                                                                                   \
            eduiMenuDetach(menu);                                                                                      \
        }                                                                                                              \
        if ((menu)->callback)                                                                                          \
            (menu)->callback(menu, parent);                                                                            \
    } while (0)

void __cdecl edpartAddGeneralType(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    EDPART_ADD_TYPE(menu, 0);
}

void __cdecl edpartCancelGeneralTypeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_generaltype_menu);
    edpart_generaltype_menu = NULL;
}

void __cdecl edpartSelType(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
    edpart_type_menu = NULL;
    edpart_active_menu = NULL;
    edpart_create_type = item->data;
    edpart_effect_list = ((PART_RUNTIME_TYPE_PC *)part_types)[edpart_create_type].field_b3;
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

void __cdecl edpartGeneralTypeMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    i32 index, selected;
    edpart_generaltype_menu =
        eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelGeneralTypeMenu, "Emitter Type (General)");
    if (edpart_generaltype_menu == NULL)
        return;
    if (part_types_used < 128)
        eduiMenuAddItem(edpart_generaltype_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartAddGeneralType, "Add Type"));
    else
        eduiMenuAddItem(edpart_generaltype_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Add Type"));
    for (index = 0; index < 128; ++index) {
        if (((PART_RUNTIME_TYPE_PC *)part_types)[index].name[0] != 0 &&
            ((PART_RUNTIME_TYPE_PC *)part_types)[index].field_b3 == 0) {
            selected = edpart_create_type == index;
            eduiMenuAddItem(edpart_generaltype_menu,
                            eduiItemCheckCreate(index, edblack, selected, 1, edpartSelType,
                                                ((PART_RUNTIME_TYPE_PC *)part_types)[index].name));
            if (selected == 1)
                edpart_generaltype_menu->selected = edui_last_item;
        }
    }
    eduiMenuAttach(menu, edpart_generaltype_menu);
    edpart_generaltype_menu->x = menu->x + 10;
    edpart_generaltype_menu->y = menu->y + 40;
}

void __cdecl edpartAddLevelType(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    EDPART_ADD_TYPE(menu, 1);
}

void __cdecl edpartCancelLevelTypeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_leveltype_menu);
    edpart_leveltype_menu = NULL;
}

void __cdecl edpartLevelTypeMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    i32 index, selected;
    edpart_leveltype_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelLevelTypeMenu, "Emitter Type (Level)");
    if (edpart_leveltype_menu == NULL)
        return;
    if (part_types_used < 128)
        eduiMenuAddItem(edpart_leveltype_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartAddLevelType, "Add Type"));
    else
        eduiMenuAddItem(edpart_leveltype_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Add Type"));
    for (index = 0; index < 128; ++index) {
        if (((PART_RUNTIME_TYPE_PC *)part_types)[index].name[0] != 0 &&
            ((PART_RUNTIME_TYPE_PC *)part_types)[index].field_b3 == 1) {
            selected = edpart_create_type == index;
            eduiMenuAddItem(edpart_leveltype_menu,
                            eduiItemCheckCreate(index, edblack, selected, 1, edpartSelType,
                                                ((PART_RUNTIME_TYPE_PC *)part_types)[index].name));
            if (selected == 1)
                edpart_leveltype_menu->selected = edui_last_item;
        }
    }
    eduiMenuAttach(menu, edpart_leveltype_menu);
    edpart_leveltype_menu->x = menu->x + 10;
    edpart_leveltype_menu->y = menu->y + 40;
}

extern struct eduimenu_s *edpart_changegenrate_menu;
extern struct eduimenu_s *edpart_maxlife_menu;

void __cdecl edpartChangeGenRate(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type)
        edpart_nearest_type->emission_rate = (f32)(i32)((edui_slider_s *)item)->value;
}

void __cdecl edpartCancelChangeGenRateMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_changegenrate_menu);
    edpart_changegenrate_menu = NULL;
}

void __cdecl edpartChangeGenRateMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type == NULL)
        return;
    edpart_changegenrate_menu =
        eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelChangeGenRateMenu, "Emits per Second");
    if (edpart_changegenrate_menu == NULL)
        return;
    eduiMenuAddItem(edpart_changegenrate_menu,
                    eduiItemSliderCreateInt(0, edblack, 0, edpartChangeGenRate, 0, (i32)(edpart_superscale * 1200.0),
                                            (i32)edpart_nearest_type->emission_rate, "Emits per Second"));
    eduiMenuAttach(menu, edpart_changegenrate_menu);
    edpart_changegenrate_menu->x = menu->x + 10;
    edpart_changegenrate_menu->y = menu->y + 40;
}

void __cdecl edpartChangeMaxLife(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type)
        edpart_nearest_type->lifetime = ((edui_slider_s *)item)->value;
}

void __cdecl edpartChangeRanMaxLife(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type)
        edpart_nearest_type->lifetime_random = ((edui_slider_s *)item)->value;
}

void __cdecl edpartCancelChangeMaxLifeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_maxlife_menu);
    edpart_maxlife_menu = NULL;
}

void edpartChangeMaxLifeMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type == NULL)
        return;
    edpart_maxlife_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelChangeMaxLifeMenu, "Max Instance Life");
    if (edpart_maxlife_menu == NULL)
        return;
    eduiMenuAddItem(edpart_maxlife_menu,
                    eduiItemSliderCreate(0, edblack, 0, edpartChangeMaxLife, 0.0f, edpart_superscale * 10.0,
                                         edpart_nearest_type->lifetime, "Base Max Life"));
    eduiMenuAddItem(edpart_maxlife_menu,
                    eduiItemSliderCreate(0, edblack, 0, edpartChangeRanMaxLife, 0.0f, edpart_superscale * 10.0,
                                         edpart_nearest_type->lifetime_random, "Random Max Life"));
    eduiMenuAttach(menu, edpart_maxlife_menu);
    edpart_maxlife_menu->x = menu->x + 10;
    edpart_maxlife_menu->y = menu->y + 40;
}

void edpartTypeMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    edpart_type_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelTypeMenu, "Emitter Type");
    if (edpart_type_menu != NULL) {
        eduiMenuAddItem(edpart_type_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralTypeMenu, "General List..."));
        eduiMenuAddItem(edpart_type_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartLevelTypeMenu, "Level List..."));
        eduiMenuAttach(menu, edpart_type_menu);
        edpart_type_menu->x = menu->x + 10;
        edpart_type_menu->y = menu->y + 40;
    }
}
static void edpartThingsInstanceMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    i32 group;
    i32 selected_first;
    i32 count;
    i32 index;
    selected_first = 0;
    edpart_which_scene = 1;
    edpart_thingsinstance_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelThingsInstanceMenu,
                                                (char *)(edpart_filter ? "Things Scene (Filtered)" : "Things Scene"));
    if (edpart_thingsinstance_menu == NULL || edbits_things_scene == NULL || edpart_nearest_type == NULL)
        return;
    group = 1;
    count = NuGScnNumSpecials(edbits_things_scene);
    for (index = 0; index < count; ++index) {
        char *name;
        i32 selected;
        i32 included;
        i32 variant;
        nuhspecial_s special;
        selected = 0;
        included = 0;
        name = NULL;
        NuGScnGetSpecial(&special, edbits_things_scene, index);
        if (NuSpecialExistsFn(&special))
            name = NuSpecialGetName(&special);
        for (variant = 0; variant < edpart_nearest_type->variant_count; ++variant) {
            if (edpart_nearest_type->effect_ids[variant] == index && edpart_nearest_type->effect_pages[variant] == 1) {
                selected = 1;
                included = 1;
            }
        }
        if (!edpart_filter || NuStrNCmp(edpart_filter_string, name, NuStrLen(edpart_filter_string)) == 0 || included) {
            eduiMenuAddItem(edpart_thingsinstance_menu,
                            eduiItemToggleCreate(index, edblack, selected, group, edpartSetInstanceType, name));
            group++;
            if (selected && !selected_first) {
                selected_first = 1;
                edpart_thingsinstance_menu->selected = edui_last_item;
            }
        }
    }
    if (group == 1)
        eduiMenuAddItem(edpart_thingsinstance_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "All Instances Filtered"));
    eduiMenuAttach(parent, edpart_thingsinstance_menu);
    edpart_thingsinstance_menu->x = parent->x + 10;
    edpart_thingsinstance_menu->y = parent->y + 40;
}
void edpartCutOffMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type != NULL) {
        edpart_cutoff_menu = eduiMenuCreate(70, 70, 250, 200, ed_fnt, edpartCancelCutOffMenu, "Radii");
        if (edpart_cutoff_menu != NULL) {
            eduiMenuAddItem(edpart_cutoff_menu,
                            eduiItemSliderCreate(0, edblack, 0, edpartChangeCutOff, 0.0f, edpart_superscale * 25.0f,
                                                 edpart_nearest_type->maximum_distance, "CutOff Rad"));
            eduiMenuAttach(menu, edpart_cutoff_menu);
            edpart_cutoff_menu->x = menu->x + 10;
            edpart_cutoff_menu->y = menu->y + 40;
        }
    }
}
void edpartGravMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type != NULL) {
        edpart_grav_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edpartCancelGravMenu, "Gravity");
        if (edpart_grav_menu != NULL) {
            eduiMenuAddItem(edpart_grav_menu,
                            eduiItemSliderCreate(0, edblack, 0, edpartChangeGrav, edpart_superscale * -10.0f,
                                                 edpart_superscale * 20.0f, edpart_nearest_type->gravity * 0.5f,
                                                 "Gravity"));
            eduiMenuAddItem(edpart_grav_menu,
                            eduiItemSliderCreate(0, edblack, 0, edpartChangeBounce, 0.0f, edpart_superscale,
                                                 edpart_nearest_type->bounce, "Bounce"));
            eduiMenuAttach(menu, edpart_grav_menu);
            edpart_grav_menu->x = menu->x + 10;
            edpart_grav_menu->y = menu->y + 40;
        }
    }
}

extern struct eduimenu_s *edpart_instanceorphans_menu;
extern i32 edpart_nearest_orphans;
extern i32 edpart_nearest_duplicates;

#define EDPART_RUNTIME_NEAREST ((PART_RUNTIME_TYPE_PC *)edpart_nearest_type)
#define EDPART_REMOVE_INSTANCE(first)                                                                                  \
    for (slot = (first); slot < 7; ++slot) {                                                                           \
        edpart_nearest_type->effect_ids[slot] = edpart_nearest_type->effect_ids[slot + 1];                             \
        edpart_nearest_type->effect_pages[slot] = edpart_nearest_type->effect_pages[slot + 1];                         \
    }                                                                                                                  \
    edpart_nearest_type->effect_ids[7] = -1;                                                                           \
    edpart_nearest_type->effect_pages[7] = -1;                                                                         \
    --edpart_nearest_type->variant_count

void __cdecl edpartSetInstanceType(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    i32 index;
    i32 slot;
    if (edpart_nearest_type == NULL)
        return;
    if (item->data == 9999) {
        for (index = 0; index < 8; ++index) {
            edpart_nearest_type->effect_ids[index] = -1;
            edpart_nearest_type->effect_pages[index] = 1;
        }
        if (item->highlighted)
            edpart_nearest_type->effect_ids[0] = 9999;
        edpart_nearest_type->flags |= 0x10;
        edpart_nearest_type->variant_count = 0;
        return;
    }
    edpart_nullobject_highlight->highlighted = 0;
    if (item->highlighted) {
        if (edpart_nearest_type->variant_count < 8) {
            edpart_nearest_type->effect_ids[edpart_nearest_type->variant_count] = item->data;
            edpart_nearest_type->effect_pages[edpart_nearest_type->variant_count] = edpart_which_scene;
            ++edpart_nearest_type->variant_count;
        } else {
            item->highlighted = 0;
        }
    } else {
        for (index = 0; index < 8; ++index) {
            if (edpart_nearest_type->effect_ids[index] == item->data) {
                EDPART_REMOVE_INSTANCE(index);
                return;
            }
        }
    }
}

void __cdecl edpartCancelWorldInstanceMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_worldinstance_menu);
    edpart_worldinstance_menu = NULL;
}

static void edpartDeleteInstanceOrphan(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
    struct eduimenu_s *parent;
    i32 slot;
    parent = NULL;
    EDPART_REMOVE_INSTANCE(item->data);
    --edpart_nearest_orphans;
    if (menu->parent) {
        parent = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->callback)
        menu->callback(menu, parent);
}

static void edpartDeleteAllInstanceOrphans(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    struct eduimenu_s *parent;
    i32 index;
    i32 slot;
    parent = NULL;
    for (index = 0; index < 8; ++index) {
        if (edpart_nearest_type->effect_ids[index] == 9998) {
            EDPART_REMOVE_INSTANCE(index);
            --index;
        }
    }
    edpart_nearest_orphans = 0;
    if (menu->parent) {
        parent = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->callback)
        menu->callback(menu, parent);
}

void __cdecl edpartDeleteAllInstanceDuplicates(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    struct eduimenu_s *parent;
    i32 index;
    i32 previous;
    i32 slot;
    parent = NULL;
    for (index = 0; index < 8; ++index) {
        i16 effect = edpart_nearest_type->effect_ids[index];
        if (effect == -1 || effect == 9999 || effect == 9998)
            continue;
        for (previous = 0; previous < index; ++previous) {
            if (effect == edpart_nearest_type->effect_ids[previous]) {
                EDPART_REMOVE_INSTANCE(index);
                --index;
                break;
            }
        }
    }
    edpart_nearest_duplicates = 0;
    if (menu->parent) {
        parent = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->callback)
        menu->callback(menu, parent);
}

static void edpartCancelInstanceOrphansMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    i32 index;
    if (edpart_nearest_type->variant_count == 0) {
        for (index = 0; index < 8; ++index) {
            edpart_nearest_type->effect_ids[index] = -1;
            edpart_nearest_type->effect_pages[index] = 1;
        }
        edpart_nearest_type->effect_ids[0] = 9999;
        edpart_nearest_type->flags |= 0x10;
    }
    eduiMenuDestroy(edpart_instanceorphans_menu);
    edpart_instanceorphans_menu = NULL;
}

void __cdecl edpartInstanceOrphansMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    i32 group;
    i32 index;
    char effect_name[20];
    char label[32];
    edpart_which_scene = 1;
    edpart_instanceorphans_menu =
        eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelInstanceOrphansMenu, "Instance Orphans/Dupes");
    if (edpart_instanceorphans_menu != NULL && edpart_nearest_type != NULL) {
        group = 1;
        for (index = 0; index < 8; ++index) {
            if (edpart_nearest_type->effect_ids[index] == 9998) {
                NuStrNCpy(effect_name, EDPART_RUNTIME_NEAREST->object_names[index], 17);
                sprintf(label, "Remove - %s", effect_name);
                eduiMenuAddItem(edpart_instanceorphans_menu,
                                eduiItemSelCreate(index, edblack, 0, group, edpartDeleteInstanceOrphan, label));
                ++group;
            }
        }
        if (edpart_nearest_orphans)
            eduiMenuAddItem(
                edpart_instanceorphans_menu,
                eduiItemSelCreate(index, edblack, 0, 0, edpartDeleteAllInstanceOrphans, "Remove All Orphans"));
        else
            eduiMenuAddItem(edpart_instanceorphans_menu,
                            eduiItemSelCreate(index, edgrey, 0, 0, NULL, "Remove All Orphans"));
        if (edpart_nearest_duplicates)
            eduiMenuAddItem(
                edpart_instanceorphans_menu,
                eduiItemSelCreate(index, edblack, 0, 0, edpartDeleteAllInstanceDuplicates, "Remove Duplicates"));
        else
            eduiMenuAddItem(edpart_instanceorphans_menu,
                            eduiItemSelCreate(index, edgrey, 0, 0, NULL, "Remove Duplicates"));
        eduiMenuAttach(parent, edpart_instanceorphans_menu);
        edpart_instanceorphans_menu->x = parent->x + 10;
        edpart_instanceorphans_menu->y = parent->y + 40;
    }
}

void __cdecl edpartSetDistribution(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type != NULL)
        EDPART_RUNTIME_NEAREST->variant_mode = item->data;
}

void __cdecl edpartCancelInstanceMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_instance_menu);
    edpart_instance_menu = NULL;
    edpart_nullobject_highlight = NULL;
}

void __cdecl edpartInstanceMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    i32 selected;
    selected = 0;
    edpart_instance_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelInstanceMenu, "Instance Select");
    if (edpart_instance_menu != NULL && edpart_nearest_type != NULL) {
        if (edpart_nearest_type->effect_ids[0] == 9999)
            selected = 1;
        eduiMenuAddItem(edpart_instance_menu, eduiItemToggleCreate(9999, edblack, selected, 2, edpartSetInstanceType,
                                                                   PartNullInstanceNamePc));
        edpart_nullobject_highlight = edui_last_item;
        if (edpart_nearest_type->field_b3 == 1)
            eduiMenuAddItem(edpart_instance_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartWorldInstanceMenu, "World Scene..."));
        else
            eduiMenuAddItem(edpart_instance_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "World Scene..."));
        eduiMenuAddItem(edpart_instance_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edpartThingsInstanceMenu, "Things Scene..."));
        if (edpart_nearest_orphans == 0 && edpart_nearest_duplicates == 0)
            eduiMenuAddItem(edpart_instance_menu,
                            eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Instance Orphans/Dupes..."));
        else
            eduiMenuAddItem(edpart_instance_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartInstanceOrphansMenu,
                                                                    "Instance Orphans/Dupes..."));
        eduiMenuAddItem(edpart_instance_menu, eduiItemCheckCreate(0, edblack, EDPART_RUNTIME_NEAREST->variant_mode == 0,
                                                                  1, edpartSetDistribution, "Random Distribution"));
        eduiMenuAddItem(edpart_instance_menu, eduiItemCheckCreate(1, edblack, EDPART_RUNTIME_NEAREST->variant_mode == 1,
                                                                  1, edpartSetDistribution, "Sequential Distribution"));
        eduiMenuAttach(menu, edpart_instance_menu);
        edpart_instance_menu->x = menu->x + 10;
        edpart_instance_menu->y = menu->y + 40;
    }
}
