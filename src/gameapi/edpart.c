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
static void edpartScaleTypeMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
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
static void edpartTypeMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
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
    edpart_which_scene = 1;
    edpart_thingsinstance_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelThingsInstanceMenu,
                                                (char *)(edpart_filter ? "Things Scene (Filtered)" : "Things Scene"));
    if (edpart_thingsinstance_menu == NULL || edbits_things_scene == NULL || edpart_nearest_type == NULL)
        return;
    group = 1;
    selected_first = 0;
    count = NuGScnNumSpecials(edbits_things_scene);
    for (index = 0; index < count; ++index) {
        char *name;
        i32 selected;
        i32 included;
        i32 variant;
        struct eduiitem_s *item;
        nuhspecial_s special;
        NuGScnGetSpecial(&special, edbits_things_scene, index);
        name = NuSpecialExistsFn(&special) ? NuSpecialGetName(&special) : NULL;
        selected = 0;
        included = 0;
        for (variant = 0; variant < edpart_nearest_type->variant_count; ++variant) {
            if (edpart_nearest_type->effect_ids[variant] == index) {
                if (edpart_nearest_type->effect_pages[variant] == 1) {
                    selected = 1;
                    included = 1;
                }
            }
        }
        if (edpart_filter && NuStrNCmp(edpart_filter_string, name, NuStrLen(edpart_filter_string)) != 0 && !included)
            continue;
        item = eduiItemToggleCreate(index, edblack, selected, group++, edpartSetInstanceType, name);
        eduiMenuAddItem(edpart_thingsinstance_menu, item);
        if (selected & (selected_first ^ 1)) {
            selected_first = 1;
            edpart_thingsinstance_menu->selected = edui_last_item;
        }
    }
    if (group == 1)
        eduiMenuAddItem(edpart_thingsinstance_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "All Instances Filtered"));
    eduiMenuAttach(parent, edpart_thingsinstance_menu);
    edpart_thingsinstance_menu->x = parent->x + 10;
    edpart_thingsinstance_menu->y = parent->y + 40;
}
static void edpartCutOffMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
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
static void edpartGravMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
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
