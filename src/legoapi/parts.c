#include <stdio.h>
#include "saga.h"

extern const char PowerUpLoopSfx_PC[18];

void __cdecl InitParts(i32 count, VARIPTR *buffer, VARIPTR end) {
    i32 i, j;
    MAXPARTS = count;
    buffer->addr = (buffer->addr + 15) & ~(u32)15;
    Part = (PART_PC *)buffer->void_ptr;
    buffer->addr += MAXPARTS * sizeof(PART_PC);
    if (Part != NULL)
        memset(Part, 0, MAXPARTS * sizeof(PART_PC));
    part_slot_cursor = 0;
    memset(part_types, 0, sizeof(part_types));
    for (i = 0; i < 128; ++i) {
        for (j = 0; j < 8; ++j) {
            part_types[i].effect_ids[j] = -1;
            part_types[i].effect_pages[j] = -1;
        }
    }
    part_types_used = 0;
    memset(part_emits, 0, sizeof(part_emits));
    for (i = 0; i < 40; ++i)
        part_emits[i].effect_id = -1;
    for (i = 0; i < 32; ++i)
        part_scene_pageid[i] = -1;
    part_emits_used = 0;
    for (i = 0; i < 8; ++i) {
        part_page_used[i] = 0;
        part_page_on[i] = 0;
    }
    for (i = 0; i < 32; ++i)
        part_scene[i] = NULL;
    part_platimpactcnt = 0;
}

__declspec(noinline) void __cdecl KillPartsByScene(NUGSCN *scene) {
    PART_PC *part = Part;
    i32 index;
    for (index = 0; index < MAXPARTS; ++index, ++part) {
        if ((part->active & 1) != 0 && part->source_special != NULL && part->special.scene == scene) {
            part->active &= ~1;
            memset(&part->special, 0, sizeof(part->special));
            part->source_special = NULL;
        }
    }
}
void edpartDeleteType(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    struct eduimenu_s *parent;
    i32 emitter;
    parent = NULL;
    if (edpart_nearest_type == &part_types[edpart_create_type]) {
        edpart_nearest = -1;
        edpart_nearest_emit = NULL;
        edpart_nearest_type = NULL;
    }
    for (emitter = 0; emitter < 40; ++emitter) {
        if (part_emits[emitter].effect_id == edpart_create_type && part_emits[emitter].effect_id != -1) {
            part_emits[emitter].effect_id = -1;
            --part_emits_used;
        }
    }
    edpartDetermineNearest(1.0f);
    part_types[edpart_create_type].name[0] = '\0';
    part_types[edpart_create_type].effect_ids[0] = -1;
    --part_types_used;
    if (menu->parent) {
        parent = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->callback)
        menu->callback(menu, parent);
    edpart_create_type = -1;
}

void __cdecl edpartClearPage(i8 page) {
    i32 index;
    NuThreadDisableThreadSwap();
    if (part_page_on[page] != 0)
        part_page_on[page] = 0;
    for (index = 0; index < 128; ++index) {
        if (part_types[index].page == page && part_types[index].name[0] != 0) {
            part_types[index].name[0] = 0;
            part_types[index].effect_ids[0] = -1;
            --part_types_used;
        }
    }
    for (index = 0; index < 40; ++index) {
        if (part_emits[index].page == page && part_emits[index].effect_id != -1) {
            part_emits[index].effect_id = -1;
            --part_emits_used;
        }
    }
    for (index = 0; index < 32; ++index) {
        if (part_scene_pageid[index] == page) {
            KillPartsByScene(part_scene[index]);
            part_scene[index] = NULL;
            part_scene_pageid[index] = -1;
        }
    }
    NuThreadEnableThreadSwap();
    part_page_used[page] = 0;
}

__declspec(noinline) i32 __cdecl PARTLookupTypePageOnly(char *name, i32 page) {
    i32 i;
    if (name == NULL || name[0] == 0)
        return -1;
    if ((u32)(page - 1) <= 6) {
        for (i = 0; i < 128; ++i) {
            if (part_types[i].page == page && NuStrNICmp(name, part_types[i].name, 16) == 0)
                return i;
        }
    }
    for (i = 0; i < 128; ++i) {
        if (part_types[i].page == 0 && NuStrNCmp(name, part_types[i].name, 16) == 0)
            return i;
    }
    return -1;
}

#include <string.h>

void __cdecl KillPart(PART_PC *part, i32 reason) {
    if ((part->active & 1) == 0) {
        return;
    }
    part->active &= ~1u;
    if ((part->flags & 0x20000) != 0 && part->debris_handle != NULL) {
        DebFreeInstantly(part->debris_handle);
    }
    if (part->lighting_handle != NULL) {
        rtlDynamicFree(*part->lighting_handle);
        *part->lighting_handle = -1;
    }
    if (part->kill_fn != NULL) {
        part->kill_fn(part, reason);
    }
    if (reason != 6 && part->effect_id_1f4 != -1) {
        i32 handle = -1;
        AddScaledFiniteShotDebrisEffect(&handle, part->effect_id_1f4, &part->position, NULL, NULL, 1,
                                        part->effect_scale);
    }
}

__declspec(noinline) void __cdecl NewPartRotation(PART_PC *part) {
    i32 index;
    for (index = 0; index < 2; ++index) {
        do {
            part->rotation_axes[index] = (u8)((NuRandIntSeeded(&partseed) % 65535u) / 21845u + 1);
        } while (index == 1 && part->rotation_axes[1] == part->rotation_axes[0]);
        part->field_124[index + 3] = (i32)(((f64)NuRandFloatSeeded(&partseed) * 2.0 - 1.0) * 65536.0);
    }
}

__declspec(noinline) void __cdecl NewPartOrderedRotation(PART_PC *part) {
    part->field_124[0] = part->rotation_parameters[0] +
                         (i32)(((f64)NuRandFloatSeeded(&partseed) * 2.0 - 1.0) * part->rotation_parameters[3]);
    part->field_124[1] = part->rotation_parameters[1] +
                         (i32)(((f64)NuRandFloatSeeded(&partseed) * 2.0 - 1.0) * part->rotation_parameters[4]);
    part->field_124[2] = part->rotation_parameters[2] +
                         (i32)(((f64)NuRandFloatSeeded(&partseed) * 2.0 - 1.0) * part->rotation_parameters[5]);
}

PART_PC *__cdecl AddPart(ADDPART_PC *parameters) {
    NUVEC forward = {0.0f, 0.0f, -1.0f};
    PART_PC *part;
    i32 scanned;
    i32 index;
    u8 hostility_group;

    if (parameters->special == NULL && parameters->part_id != -1 && parameters->part_id != 9999)
        return NULL;
    if (parameters->special != NULL && NuSpecialExistsFn(parameters->special) == 0)
        return NULL;
    part = &Part[part_slot_cursor];
    scanned = 0;
    while ((part->active & 1) != 0 && (part->flags & 0x200) != 0 && scanned < MAXPARTS) {
        if (++part_slot_cursor == MAXPARTS) {
            part_slot_cursor = 0;
        }
        part = &Part[part_slot_cursor];
        ++scanned;
    }
    if (scanned == MAXPARTS) {
        return NULL;
    }
    if ((part->active & 1) != 0) {
        if (part->destroy_fn != NULL) {
            part->destroy_fn(part);
        }
        KillPart(part, 0);
    }
    if (parameters->matrix != NULL) {
        NUMTX matrix;
        part->transform = *parameters->matrix;
        matrix = *parameters->matrix;
        NuVecMtxRotate(&forward, &forward, &matrix);
        part->rotation_y = (u16)NuAtan2D(forward.x, forward.z);
        NuVecRotateY(&forward, &forward, -(i32)part->rotation_y);
        part->rotation_x = (u16)-NuAtan2D(forward.y, forward.z);
    } else {
        if (parameters->position == NULL) {
            return NULL;
        }
        NuMtxSetTranslation(&part->transform, parameters->position);
    }
    part->previous_position = part->position;
    part->owner = parameters->owner;
    part->recipient = parameters->recipient;
    part->velocity = *parameters->velocity;
    part->field_206 = -1;
    part->field_20b = -1;
    part->time_remaining = part->initial_time = parameters->field_a4;
    part->active = (part->active & ~0x60u) | 0x21;
    part->time_step = parameters->time_step;
    part->scale_time = 0.0f;
    part->radius = parameters->field_14;
    part->field_e0 = (parameters->time_step / PARTSCALEUPTIME) * parameters->field_14;
    part->field_e4 = parameters->field_18;
    part->field_214 = parameters->field_c0;
    part->gravity = parameters->gravity;
    part->field_ec = parameters->field_20;
    part->active &= ~2u;
    part->part_id = (i16)parameters->part_id;
    part->field_f0 = parameters->field_88;
    part->flags = parameters->flags;
    part->flicker_flags = parameters->field_c4;
    part->active &= ~0x4000u;
    if (parameters->lighting_data != NULL) {
        for (index = 0; index < 7; ++index) {
            part->lighting[index] = parameters->lighting_data->values[index];
        }
        part->active |= 8;
    } else {
        part->active &= ~8u;
        if (parameters->lighting_scale_x != 1.0f || parameters->lighting_scale_y != 1.0f ||
            parameters->lighting_scale_z != 1.0f) {
            part->lighting[6].x = parameters->lighting_scale_x;
            part->lighting[6].y = parameters->lighting_scale_y;
            part->lighting[6].z = parameters->lighting_scale_z;
            part->active |= 0x2000;
        } else {
            part->active &= ~0x2000u;
        }
    }
    part->source_special = parameters->special;
    if (parameters->special != NULL) {
        part->special = *parameters->special;
    } else {
        memset(&part->special, 0, sizeof(part->special));
    }
    part->active &= ~0x14u;
    part->field_f4 = 0.0f;
    part->callback_1ac = parameters->callback_30;
    part->move_fn = parameters->move_fn;
    part->deflect_fn = parameters->deflect_fn;
    part->impact_fn = parameters->impact_fn;
    part->collision_fn = parameters->collision_fn;
    part->kill_fn = parameters->kill_fn;
    part->update_fn = parameters->update_fn;
    part->stop_fn = parameters->stop_fn;
    part->destroy_fn = parameters->destroy_fn;
    part->draw_fn = parameters->draw_fn;
    part->collision_min.x = part->position.x - part->radius;
    part->collision_min.y = part->position.y - part->radius;
    part->collision_min.z = part->position.z - part->radius;
    part->collision_max.x = part->position.x + part->radius;
    part->collision_max.y = part->position.y + part->radius;
    part->collision_max.z = part->position.z + part->radius;
    for (index = 0; index < 6; ++index) {
        part->field_10c[index] = parameters->field_a8[index];
    }
    for (index = 0; index < 5; ++index) {
        part->field_124[index] = 0;
    }
    if ((part->flags & 0x80) != 0) {
        NewPartRotation(part);
    }
    if ((part->flags & 0x100000) != 0) {
        NewPartOrderedRotation(part);
    }
    if (parameters->effect_id_58 != -1) {
        i32 effect;
        part->inline_debris_handle = -1;
        part->debris_handle = &part->inline_debris_handle;
        if (parameters->effect_scale != 1.0f)
            effect = CreateScaledEffect(parameters->effect_id_58, parameters->effect_scale);
        else
            effect = parameters->effect_id_58;
        AddDebrisEffect(part->debris_handle, effect, part->position.x, part->position.y, part->position.z);
        DebrisPopulateInstance(*part->debris_handle, 0.0f);
    } else {
        part->debris_handle = parameters->debris_handle;
    }
    part->effect_id_1e0 = parameters->effect_id_60;
    part->field_1e8 = parameters->field_68;
    part->effect_id_1e4 = parameters->effect_id_64;
    part->field_1ec = parameters->field_6c;
    part->effect_id_1f0 = parameters->effect_id_70;
    part->effect_id_1f8 = parameters->effect_id_74;
    part->effect_id_1f4 = parameters->effect_id_78;
    part->effect_scale = parameters->effect_scale;
    part->previous_transform = part->transform;
    part->field_210 = 0;
    part->field_208 = 1;
    part->force_flags = 0;
    part->field_21c = 0;
    hostility_group = (u8)parameters->hostility_group;
    part->active |= 0x1800;
    part->hostility_group = hostility_group;
    part->field_204 = 1;
    if ((part->flags & 0x2000) != 0 && parameters->lighting_handle != NULL && PartRTL != NULL) {
        i32 light_id;
        part->lighting_handle = parameters->lighting_handle;
        light_id = rtlDynamicAllocTemplate((RTL_SET_PC *)PartRTL, parameters->lighting_id);
        *parameters->lighting_handle = light_id;
    }
    if (++part_slot_cursor == MAXPARTS) {
        part_slot_cursor = 0;
    }
    return part;
}

PART_PC *__cdecl HitParts(GAMEOBJECT *excluded_owner, NUVEC *points, i32 point_count, f32 radius, NUVEC *minimum,
                          NUVEC *maximum, u32 flags) {
    PART_PC *part = Part;
    i32 index;

    for (index = 0; index < MAXPARTS; ++index, ++part) {
        i32 point;
        if ((part->active & 1) == 0 || part->owner == excluded_owner || (part->flags & flags) == 0) {
            continue;
        }
        if (part->collision_min.x > maximum->x || minimum->x > part->collision_max.x ||
            part->collision_min.z > maximum->z || minimum->z > part->collision_max.z ||
            part->collision_min.y > maximum->y || minimum->y > part->collision_max.y) {
            continue;
        }
        for (point = point_count - 1; point >= 0; --point) {
            NUVEC difference;
            f32 distance;
            f32 combined_radius;
            NuVecSub(&difference, &points[point], &part->position);
            distance = difference.x * difference.x + difference.y * difference.y + difference.z * difference.z;
            combined_radius = radius + part->radius;
            if (distance <= combined_radius * combined_radius) {
                return part;
            }
        }
    }
    return NULL;
}

__declspec(noinline) void edpartStartPage(u8 page) {
    i32 i;
    i8 signed_page = (i8)page;

    for (i = 0; i < 40; ++i) {
        if (part_emits[i].effect_id != -1 && part_emits[i].page == signed_page) {
            part_emits[i].camera_distance = 9999.9f;
        }
    }
    part_page_on[signed_page] = 1;
}

__declspec(noinline) void edpartStopPage(u8 page) {
    part_page_on[(i8)page] = 0;
}

void edpartCancelInstanceFlagsMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_instanceflags_menu);
    edpart_instanceflags_menu = NULL;
}

extern i32 edpart_particle_list;
extern i32 edpart_part_list;
extern i32 edpart_set_part;
extern struct eduimenu_s *edpart_debrisindex_menu;
extern struct eduimenu_s *edpart_partindex_menu;

void edpartChangeDebrisIndex(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type != NULL) {
        if (edpart_set_debris == 0)
            edpart_nearest_type->trail_effects[0] = item->data;
        else if (edpart_set_debris == 1)
            edpart_nearest_type->trail_effects[1] = item->data;
        else if (edpart_set_debris == 2)
            edpart_nearest_type->attached_effect = item->data;
        else if (edpart_set_debris == 3)
            edpart_nearest_type->impact_effect = item->data;
        else if (edpart_set_debris == 4)
            edpart_nearest_type->kill_effect = item->data;
    }
}

void edpartCancelDebrisIndexMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_debrisindex_menu);
    edpart_debrisindex_menu = NULL;
}

void edpartDebrisIndexMenu(eduimenu_s *menu) {
    i32 chosen;
    i32 found;
    i32 index;
    edpart_debrisindex_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelDebrisIndexMenu, "Debris Type");
    if (edpart_debrisindex_menu == NULL || edpart_nearest_type == NULL)
        return;
    chosen = -1;
    if (edpart_set_debris == 0)
        chosen = edpart_nearest_type->trail_effects[0];
    else if (edpart_set_debris == 1)
        chosen = edpart_nearest_type->trail_effects[1];
    else if (edpart_set_debris == 2)
        chosen = edpart_nearest_type->attached_effect;
    else if (edpart_set_debris == 3)
        chosen = edpart_nearest_type->impact_effect;
    else if (edpart_set_debris == 4)
        chosen = edpart_nearest_type->kill_effect;
    found = 0;
    for (index = 1; index < debinf_count; ++index) {
        if (debtab[index] != NULL && (i8)debtab[index]->category == edpart_particle_list) {
            i32 selected = chosen == index;
            eduiMenuAddItem(edpart_debrisindex_menu, eduiItemCheckCreate(index, edblack, selected, 1,
                                                                         edpartChangeDebrisIndex, debtab[index]->name));
            if (selected == 1) {
                edpart_debrisindex_menu->selected = edui_last_item;
                found = selected;
            }
        }
    }
    eduiMenuSortItemsByTxt(edpart_debrisindex_menu);
    eduiMenuAddItemFirst(edpart_debrisindex_menu,
                         eduiItemCheckCreate(-1, edblack, chosen == -1, 1, edpartChangeDebrisIndex, "None"));
    eduiMenuAttach(menu, edpart_debrisindex_menu);
    edpart_debrisindex_menu->x = menu->x + 10;
    edpart_debrisindex_menu->y = menu->y + 40;
    if (found)
        edpart_debrisindex_menu->field_0c = edpart_debrisindex_menu->selected;
}

void edpartChangePartIndex(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type != NULL && edpart_set_part == 5)
        edpart_nearest_type->impact_part = item->data;
}

void edpartCancelPartIndexMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_partindex_menu);
    edpart_partindex_menu = NULL;
}

void edpartPartIndexMenu(eduimenu_s *menu) {
    i32 chosen;
    i32 index;
    edpart_partindex_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelPartIndexMenu, "Part Type");
    if (edpart_partindex_menu == NULL || edpart_nearest_type == NULL)
        return;
    chosen = -1;
    if (edpart_set_part == 3)
        chosen = edpart_nearest_type->impact_part;
    eduiMenuAddItem(edpart_partindex_menu,
                    eduiItemCheckCreate(-1, edblack, chosen == -1, 1, edpartChangePartIndex, "None"));
    for (index = 0; index < 128; ++index) {
        if (part_types[index].name[0] != '\0' && (i8)part_types[index].field_b3 == edpart_part_list) {
            i32 selected = chosen == index;
            if (index == edpart_nearest_emit->effect_id)
                eduiMenuAddItem(edpart_partindex_menu,
                                eduiItemCheckCreate(index, edgrey, 0, 0, NULL, part_types[index].name));
            else
                eduiMenuAddItem(
                    edpart_partindex_menu,
                    eduiItemCheckCreate(index, edblack, selected, 1, edpartChangePartIndex, part_types[index].name));
            if (selected == 1)
                edpart_partindex_menu->selected = edui_last_item;
        }
    }
    eduiMenuAttach(menu, edpart_partindex_menu);
    edpart_partindex_menu->x = menu->x + 10;
    edpart_partindex_menu->y = menu->y + 40;
}

void edpartGeneralDebrisIndexMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    edpart_particle_list = 0;
    edpartDebrisIndexMenu(menu);
}

void edpartLevelDebrisIndexMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    edpart_particle_list = 1;
    edpartDebrisIndexMenu(menu);
}

void edpartGeneralPartIndexMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    edpart_part_list = 0;
    edpartPartIndexMenu(menu);
}

void edpartLevelPartIndexMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    edpart_part_list = 1;
    edpartPartIndexMenu(menu);
}

void edpartChangeDebrisPerSec(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type != NULL) {
        if (edpart_set_debris == 0)
            edpart_nearest_type->trail_rates[0] = (f32)(i32)((edui_slider_s *)item)->value;
        else if (edpart_set_debris == 1)
            edpart_nearest_type->trail_rates[1] = (f32)(i32)((edui_slider_s *)item)->value;
    }
}

void edpartCancelTrail1DebrisMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_trail1debris_menu);
    edpart_trail1debris_menu = NULL;
}

void edpartCancelTrail2DebrisMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_trail2debris_menu);
    edpart_trail2debris_menu = NULL;
}

void edpartCancelEmitterDebrisMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_emitterdebris_menu);
    edpart_emitterdebris_menu = NULL;
}

void edpartCancelImpactDebrisMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_impactdebris_menu);
    edpart_impactdebris_menu = NULL;
}

void edpartCancelImpactPartMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_impactpart_menu);
    edpart_impactpart_menu = NULL;
}

void edpartCancelDieDebrisMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_diedebris_menu);
    edpart_diedebris_menu = NULL;
}

void edpartTrail1DebrisMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type == NULL)
        return;
    edpart_set_debris = 0;
    edpart_trail1debris_menu = eduiMenuCreate(70, 70, 300, 300, ed_fnt, edpartCancelTrail1DebrisMenu, "Trail 1 Debris");
    if (edpart_trail1debris_menu != NULL) {
        eduiMenuAddItem(edpart_trail1debris_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralDebrisIndexMenu, "General List..."));
        if (edpart_nearest_type->field_b3 == 1)
            eduiMenuAddItem(edpart_trail1debris_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartLevelDebrisIndexMenu, "Level List..."));
        else
            eduiMenuAddItem(edpart_trail1debris_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Level List..."));
        eduiMenuAddItem(edpart_trail1debris_menu, eduiItemSliderCreateInt(0, edblack, 0, edpartChangeDebrisPerSec, 0,
                                                                          ((i32)(edpart_superscale * 1200.0f)),
                                                                          ((i32)(edpart_nearest_type->trail_rates[0])),
                                                                          "Trail 1 particles per second"));
    }
    eduiMenuAttach(menu, edpart_trail1debris_menu);
    edpart_trail1debris_menu->x = menu->x + 10;
    edpart_trail1debris_menu->y = menu->y + 40;
}

void edpartTrail2DebrisMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type == NULL)
        return;
    edpart_set_debris = 1;
    edpart_trail2debris_menu = eduiMenuCreate(70, 70, 300, 300, ed_fnt, edpartCancelTrail2DebrisMenu, "Trail 2 Debris");
    if (edpart_trail2debris_menu != NULL) {
        eduiMenuAddItem(edpart_trail2debris_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralDebrisIndexMenu, "General List..."));
        if (edpart_nearest_type->field_b3 == 1)
            eduiMenuAddItem(edpart_trail2debris_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartLevelDebrisIndexMenu, "Level List..."));
        else
            eduiMenuAddItem(edpart_trail2debris_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Level List..."));
        eduiMenuAddItem(edpart_trail2debris_menu, eduiItemSliderCreateInt(0, edblack, 0, edpartChangeDebrisPerSec, 0,
                                                                          ((i32)(edpart_superscale * 1200.0f)),
                                                                          ((i32)(edpart_nearest_type->trail_rates[1])),
                                                                          "Trail 2 particles per second"));
    }
    eduiMenuAttach(menu, edpart_trail2debris_menu);
    edpart_trail2debris_menu->x = menu->x + 10;
    edpart_trail2debris_menu->y = menu->y + 40;
}

void edpartEmitterDebrisMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type == NULL)
        return;
    edpart_set_debris = 2;
    edpart_emitterdebris_menu =
        eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelEmitterDebrisMenu, "Emitter Debris");
    if (edpart_emitterdebris_menu != NULL) {
        eduiMenuAddItem(edpart_emitterdebris_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralDebrisIndexMenu, "General List..."));
        if (edpart_nearest_type->field_b3 == 1)
            eduiMenuAddItem(edpart_emitterdebris_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartLevelDebrisIndexMenu, "Level List..."));
        else
            eduiMenuAddItem(edpart_emitterdebris_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Level List..."));
    }
    eduiMenuAttach(menu, edpart_emitterdebris_menu);
    edpart_emitterdebris_menu->x = menu->x + 10;
    edpart_emitterdebris_menu->y = menu->y + 40;
}

void edpartImpactDebrisMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type == NULL)
        return;
    edpart_set_debris = 3;
    edpart_impactdebris_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelImpactDebrisMenu, "Impact Debris");
    if (edpart_impactdebris_menu != NULL) {
        eduiMenuAddItem(edpart_impactdebris_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralDebrisIndexMenu, "General List..."));
        if (edpart_nearest_type->field_b3 == 1)
            eduiMenuAddItem(edpart_impactdebris_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartLevelDebrisIndexMenu, "Level List..."));
        else
            eduiMenuAddItem(edpart_impactdebris_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Level List..."));
    }
    eduiMenuAttach(menu, edpart_impactdebris_menu);
    edpart_impactdebris_menu->x = menu->x + 10;
    edpart_impactdebris_menu->y = menu->y + 40;
}

void edpartImpactPartMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type == NULL)
        return;
    edpart_set_debris = 5;
    edpart_impactpart_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelImpactPartMenu, "Impact Part");
    if (edpart_impactpart_menu != NULL) {
        eduiMenuAddItem(edpart_impactpart_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralPartIndexMenu, "General List..."));
        if (edpart_nearest_type->field_b3 == 1)
            eduiMenuAddItem(edpart_impactpart_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartLevelPartIndexMenu, "Level List..."));
        else
            eduiMenuAddItem(edpart_impactpart_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Level List..."));
    }
    eduiMenuAttach(menu, edpart_impactpart_menu);
    edpart_impactpart_menu->x = menu->x + 10;
    edpart_impactpart_menu->y = menu->y + 40;
}

void edpartDieDebrisMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type == NULL)
        return;
    edpart_set_debris = 4;
    edpart_diedebris_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelDieDebrisMenu, "Die Debris");
    if (edpart_diedebris_menu != NULL) {
        eduiMenuAddItem(edpart_diedebris_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralDebrisIndexMenu, "General List..."));
        if (edpart_nearest_type->field_b3 == 1)
            eduiMenuAddItem(edpart_diedebris_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartLevelDebrisIndexMenu, "Level List..."));
        else
            eduiMenuAddItem(edpart_diedebris_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Level List..."));
    }
    eduiMenuAttach(menu, edpart_diedebris_menu);
    edpart_diedebris_menu->x = menu->x + 10;
    edpart_diedebris_menu->y = menu->y + 40;
}

void edpartChangeDebrisScale(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type)
        edpart_nearest_type->effect_scale = ((edui_slider_s *)(item))->value;
}

void edpartCancelDebrisScaleMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_debrisscale_menu);
    edpart_debrisscale_menu = NULL;
}

void edpartCancelDebrisSettingsMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_debrissettings_menu);
    edpart_debrissettings_menu = NULL;
}

void edpartInstanceFlagsMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    i32 group = 1;
    if (edpart_nearest_type == NULL)
        return;
    edpart_instanceflags_menu =
        eduiMenuCreate(70, 70, 300, 250, ed_fnt, edpartCancelInstanceFlagsMenu, "Instance Flags");
    if (edpart_instanceflags_menu != NULL) {
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(1, edblack, (edpart_nearest_type->flags & 1) != 0, group++,
                                             edpartChangeInstanceFlag, "Die when stopped"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(2, edblack, (edpart_nearest_type->flags & 2) != 0, group++,
                                             edpartChangeInstanceFlag, "Collide with Characters"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(4, edblack, (edpart_nearest_type->flags & 4) != 0, group++,
                                             edpartChangeInstanceFlag, "Interact only with Active Chars"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(8, edblack, (edpart_nearest_type->flags & 8) != 0, group++,
                                             edpartChangeInstanceFlag, "Damage Characters"));
        if (edpart_nearest_type->effect_ids[0] != 9999)
            eduiMenuAddItem(edpart_instanceflags_menu,
                            eduiItemToggleCreate(0x10, edblack, (edpart_nearest_type->flags & 0x10) != 0, group++,
                                                 edpartChangeInstanceFlag, "Don't die when off-screen"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(0x40, edblack, (edpart_nearest_type->flags & 0x40) != 0, group++,
                                             edpartChangeInstanceFlag, "Is a Collectible"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(0x80, edblack, (edpart_nearest_type->flags & 0x80) != 0, group++,
                                             edpartChangeInstanceFlag, "Rotate Randomly"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(0x100000, edblack, (edpart_nearest_type->flags & 0x100000) != 0, group++,
                                             edpartChangeInstanceFlag, "Ordered Rotate Randomly"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(0x100, edblack, (edpart_nearest_type->flags & 0x100) != 0, group++,
                                             edpartChangeInstanceFlag, "No Bounce"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(0x200, edblack, (edpart_nearest_type->flags & 0x200) != 0, group++,
                                             edpartChangeInstanceFlag, "Slot Cannot be Stolen"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(0x400, edblack, (edpart_nearest_type->flags & 0x400) != 0, group++,
                                             edpartChangeInstanceFlag, "Ignore Terrain"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(0x800, edblack, (edpart_nearest_type->flags & 0x800) != 0, group++,
                                             edpartChangeInstanceFlag, "Face Direction of Movement"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(0x1000, edblack, (edpart_nearest_type->flags & 0x1000) != 0, group++,
                                             edpartChangeInstanceFlag, "Disable Draw"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(0x2000, edblack, (edpart_nearest_type->flags & 0x2000) != 0, group++,
                                             edpartChangeInstanceFlag, "Real Time Lighting"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(0x4000, edblack, (edpart_nearest_type->flags & 0x4000) != 0, group++,
                                             edpartChangeInstanceFlag, "Ignore Creature"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(0x8000, edblack, (edpart_nearest_type->flags & 0x8000) != 0, group++,
                                             edpartChangeInstanceFlag, "Thrown"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(0x10000, edblack, (edpart_nearest_type->flags & 0x10000) != 0, group++,
                                             edpartChangeInstanceFlag, "Can Damage Owner"));
    }
    eduiMenuAttach(menu, edpart_instanceflags_menu);
    edpart_instanceflags_menu->x = menu->x + 10;
    edpart_instanceflags_menu->y = menu->y + 40;
}
void edpartDebrisScaleMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    edpart_debrisscale_menu = eduiMenuCreate(70, 70, 180, 300, ed_fnt, edpartCancelDebrisScaleMenu, "Debris Scale");
    if (edpart_debrisscale_menu != NULL && edpart_nearest_type != NULL) {
        eduiMenuAddItem(edpart_debrisscale_menu,
                        eduiItemSliderCreate(0, edblack, 0, edpartChangeDebrisScale, 0.0f, edpart_superscale * 10.0f,
                                             edpart_nearest_type->effect_scale, "Debris Scale"));
        eduiMenuAttach(menu, edpart_debrisscale_menu);
        edpart_debrisscale_menu->x = menu->x + 10;
        edpart_debrisscale_menu->y = menu->y + 40;
    }
}
void edpartDebrisSettingsMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type != NULL) {
        edpart_debrissettings_menu =
            eduiMenuCreate(70, 70, 300, 300, ed_fnt, edpartCancelDebrisSettingsMenu, "Debris Settings");
        if (edpart_debrissettings_menu != NULL) {
            eduiMenuAddItem(edpart_debrissettings_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartTrail1DebrisMenu, "Trail 1 Debris..."));
            eduiMenuAddItem(edpart_debrissettings_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartTrail2DebrisMenu, "Trail 2 Debris..."));
            eduiMenuAddItem(edpart_debrissettings_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartEmitterDebrisMenu, "Emitter Debris..."));
            eduiMenuAddItem(edpart_debrissettings_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartImpactDebrisMenu, "Impact Debris..."));
            eduiMenuAddItem(edpart_debrissettings_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartDieDebrisMenu, "Die Debris..."));
            eduiMenuAddItem(edpart_debrissettings_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartImpactPartMenu, "Impact Part..."));
            eduiMenuAddItem(edpart_debrissettings_menu,
                            eduiItemToggleCreate(0x200000, edblack, (edpart_nearest_type->flags >> 21) & 1, 1,
                                                 edpartChangeInstanceFlag, "Debris stops when Part stops"));
            eduiMenuAddItem(edpart_debrissettings_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edpartDebrisScaleMenu, "Debris Scale..."));
        }
        eduiMenuAttach(menu, edpart_debrissettings_menu);
        edpart_debrissettings_menu->x = menu->x + 10;
        edpart_debrissettings_menu->y = menu->y + 40;
    }
}

void Parts_Start(WORLDINFO *world) {
    if (world->page_part != -1) {
        edpartStartPage((u8)world->page_part);
    }
}

void Parts_Stop(WORLDINFO *world) {
    if (world->page_part != -1) {
        edpartStopPage((u8)world->page_part);
    }
}

void SetPartRTLSet(RTLSET_PC *rtl_set) {
    PartRTL = rtl_set;
}

void NuPartSetSeed(i32 seed) {
    partseed = (u32)seed;
}

u32 NuPartGetSeed(void) {
    return partseed;
}

void __cdecl ResetParts(void) {
    if (Part != NULL)
        memset(Part, 0, MAXPARTS * sizeof(PART_PC));
    part_slot_cursor = 0;
}

__declspec(noinline) void __cdecl AddPartDebris(PARTDEBRISSYS *sys, i32 index, NUVEC *position) {
    i32 effect;

    if (sys == NULL || index < 0 || index >= part_debris_count) {
        return;
    }
    effect = sys->entries[index].id;
    if (effect == -1) {
        return;
    }
    AddFiniteShotPART(effect, position, 1);
}

void __cdecl PartStop_Flickerer(PART_PC *part) {
    part->time_remaining = 5.0f;
    if ((part->flicker_flags & 1) != 0)
        part->time_remaining = 2.5f;
}
i32 __cdecl PartDraw_Flickerer(PART_PC *part) {
    i32 draw = 1;
    if (part->time_remaining > 0.0f && (u32)GameTimer.update_count % 6 >= 3) {
        f32 threshold = 2.0f;
        if ((part->flicker_flags & 1) != 0)
            threshold = 1.0f;
        if (part->time_remaining < threshold)
            draw = 0;
    }
    return draw;
}
void __cdecl PartImpact_Basketball(PART_PC *part) {
    f32 volume = NuVecMag(&part->velocity);
    if (volume >= 2.5f)
        volume = 1.0f;
    else if (volume >= (f64)0.1f)
        volume = (f32)(((f64)volume - (f64)0.1f) / (f64)2.4f);
    else
        return;
    if (volume > 0.0f)
        PlaySfxAndSetVolume("BBounce", &part->position, volume);
}

void __cdecl PartUpdate_Basketball(PART_PC *part) {
    GIZMOBLOWUP_PC *blowup;
    if (LevGizmo[0] == NULL)
        return;
    blowup = (GIZMOBLOWUP_PC *)LevGizmo[0]->object;
    if ((blowup->status_flags & 0x800001) == 0 &&
        NuVecDistSqr(&blowup->position, &part->position, NULL) < 0.1f * 0.1f) {
        GizmoActivate(WORLD->gizmo_sys, LevGizmo[0], 1, 0);
        STUBBED_NAMED("PartUpdate_Basketball: GizmoBlowupBlowup PC567800(blowup,1,-1,-1,NULL,1)");
    }
}
void __cdecl DeflectPart(PART_PC *, GAMEOBJECT *, f32, f32, i32, i32);
i32 __cdecl getMaxTorpedos(GAMEOBJECT *);
void __cdecl ReleaseBuildIt(GAMEOBJECT *, i32);
void __cdecl ReleasePush(GAMEOBJECT *);
void __cdecl ObjHitShield(GAMEOBJECT *, GAMEOBJECT *, i32, void *);
i32 __cdecl ObjHitObj(GAMEOBJECT *, GAMEOBJECT *, i32, u16, i32, i32);
__declspec(noinline) static void PartCollidePC(PART_PC *part, i32 three_dimensional) {
    i32 i;
    i32 slot;
    i32 count;
    NUVEC minimum;
    NUVEC maximum;
    NUVEC direction;
    GIZMOPICKUP_PC pickup;
    GAMEOBJECT *object;
    GAMEOBJECT *api;
    TORPEDOPACKET_PC *packet;
    PLAYERCHARACTERCONFIG_PC *character;
    i8 context;
    u64 flags;
    i32 effect;
    f32 speed;
    u16 angle;
    minimum.x = part->position.x - part->field_e4;
    maximum.x = part->field_e4 + part->position.x;
    minimum.y = part->position.y - part->field_e4;
    maximum.y = part->field_e4 + part->position.y;
    minimum.z = part->position.z - part->field_e4;
    maximum.z = part->field_e4 + part->position.z;
    object = Obj;
    api = Obj;
    slot = (i32)(((u8 *)part - (u8 *)Part) / sizeof(PART_PC));
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object, ++api) {
        if ((api->flags_1fc_word & 1) == 0 || (api->flags_1fc_word & 0x1000) == 0 || api->dead != 0)
            continue;
        context = (i8)(object->character_context);
        if ((U64_PAIR(&CInfo[context].flags) & 0x8000) != 0 || (object->flags_e20 & 0x20) != 0)
            continue;
        if (part->field_206 == 0) {
            if (object == part->owner) {
                if (part->scale_time < 0.5f)
                    continue;
            } else if (api->field_280 != 0xff && part->scale_time < 0.25f)
                continue;
        } else if ((part->flags & 0x8000) == 0 && part->owner == object)
            continue;
        if (context == 0x3b || context == 0x39 || context == 0x3c)
            continue;
        if ((part->flags & 0x4000) != 0 && (i8)part->hostility_group == api->hostility_group)
            continue;
        if ((part->flags & 4) != 0 && (api->flags_1fc_word & 0x80) == 0)
            continue;
        if (minimum.x > api->collision_max.x || maximum.x < api->collision_min.x || minimum.z > api->collision_max.z ||
            maximum.z < api->collision_min.z)
            continue;
        if (three_dimensional != 0) {
            if ((api->character_data->model_flags & 0x2000) != 0 && (part->flags & 0x40) != 0)
                goto collect_pickup;
            if (minimum.y > api->collision_max.y || maximum.y < api->collision_min.y)
                continue;
        }
        if ((part->flags & 0x40) == 0) {
            character = (PLAYERCHARACTERCONFIG_PC *)(api->character_data->game_character);
            if ((character->flags_090 & 0x8000) != 0)
                continue;
            if (!SphereSphereOverlapScaleY(&part->position, part->field_e4, part->field_e4, &api->collision_position,
                                           api->collision_radius, api->bounds_half_height))
                continue;
            if (!((part->active & 2) != 0 && (part->flags & 0x1000008) == 0x1000008))
                KillPart(part, 1);
            if ((part->flags & 8) != 0 && !((part->flags & 0x1000000) != 0 && (part->active & 2) != 0) &&
                ((api->flags_1fc_word & 0x80) != 0 || (part->flags & 0x800000) == 0)) {
                flags = U64_PAIR(&CInfo[(i8)(object->character_context)].flags);
                if ((flags & 0x4000000) != 0 ||
                    ((flags & 0x8000000) != 0 && (object->context_variant_flags & 0x20000) != 0)) {
                    if (object->id == (u16)id_BOB)
                        effect = (object->flags_ef0 & 0x200) != 0 ? 2 : 3;
                    else if (object->blade_index != -1)
                        effect = BladeTab[object->blade_index].hit_effect;
                    else
                        effect = -1;
                    DeflectPart(part, object, GameDeflectPartSpeed, GameDeflectPartGravity, 0, effect);
                } else {
                    GameCam_Judder(GameCam, 0.2f, 0, NULL);
                    ReleaseBuildIt(object, 0);
                    ReleasePush(object);
                    if (object->origin_scale_override >= 1.0f)
                        ObjHitShield(part->owner, object, object->shield_hitpoints, NULL);
                    else if (CannotKill(object)) {
                        NewRumble(part->owner->pad->bound_pad, 0.5f, 0);
                        NewRumble(object->pad->bound_pad, 0.5f, 0);
                    } else {
                        ObjHitObj((part->flags & 0x10000) != 0 ? NULL : part->owner, object, part->field_204,
                                  (u16)(part->force_flags), 0, 1);
                    }
                }
            }
            if (api->dead != 0 || (api->flags_1fc_word & 2) != 0)
                continue;
            if ((part->active & 1) == 0) {
                speed = api->character_data->game_character->run_speed;
                if (api->scaled_radius > 1.0f)
                    speed /= api->scaled_radius;
                NuVecNorm(&direction, &part->velocity);
                api->velocity.x += direction.x * speed;
                api->velocity.z += direction.z * speed;
            }
            if ((part->flags & 0x2000000) != 0 && ((part->flags & 0x4000000) == 0 || (part->active & 2) != 0)) {
                angle = NuAtan2D(api->position.x - part->position.x, api->position.z - part->position.z);
                api->movement_direction_x =
                    NuTrigTable[(u16)angle >> 1] * api->character_data->game_character->run_speed;
                api->movement_direction_z =
                    NuTrigTable[(((u16)angle + 0x4000) >> 1) & 0x7fff] * api->character_data->game_character->run_speed;
            }
        } else {
        collect_pickup:
            if ((part->active & 2) == 0) {
                if (part->part_id != 0xcb || !(part->scale_time >= 0.5f) ||
                    (VehicleArea == 0 &&
                     ((i8)(object->current_hp) <= 0 || (i8)(object->current_hp) >= object->field_107e))) {
                    if (part->part_id == 0xcb)
                        continue;
                    if (!(part->scale_time >= (f64)(slot % 11) * (f64)0.05f + 0.75))
                        continue;
                }
            }
            if ((api->flags_1f8 & 0x40000) != 0) {
                KillPart(part, 2);
                continue;
            }
            if (part->part_id == 0xcb) {
                CollectHitPoint(object, &part->position, 1);
                KillPart(part, 2);
            } else if (part->part_id == 0xd0) {
                CollectPowerUp(object, &part->position, part->rotation_y, 1);
                KillPart(part, 2);
            } else if (part->field_206 == 3) {
                if ((api->character_data->model_flags & 0x2000) == 0)
                    continue;
                if (object->torpedo == NULL)
                    continue;
                count = object->torpedo->count;
                packet = object->torpedo;
                if (count < getMaxTorpedos(object)) {
                    packet->pickup_positions[count] = part->position;
                    object->torpedo->pickup_data[object->torpedo->count] = part->field_10c[0];
                    object->torpedo->pickup_flags[object->torpedo->count] = part->field_10c[1];
                    ++object->torpedo->count;
                    object->torpedo->steal_timer = 0.0f;
                    object->torpedo->pickup_state = 0;
                    KillPart(part, 2);
                }
            } else {
                SetCoinType(part->part_id, &pickup);
                GizmoPickup_CollectCoin(WORLD, &part->position, pickup.type_index, pickup.model_variant, object, 1);
                KillPart(part, 2);
            }
        }
    }
}

void __cdecl PartCollide_2D(PART_PC *part) {
    PartCollidePC(part, 0);
}
void __cdecl PartCollide_3D(PART_PC *part) {
    PartCollidePC(part, 1);
}

void __cdecl DebrisEmitterPos(i32 handle, f32 x, f32 y, f32 z) {
    if (handle != -1) {
        debkeydata[handle].position.x = x;
        debkeydata[handle].position.y = y;
        debkeydata[handle].position.z = z;
        debkeydata[handle].orientation_dirty = 0.0f;
    }
}

__declspec(noinline) void __cdecl DebrisEmitterMomentum(i32 handle, f32 x, f32 y, f32 z) {
    if (handle != -1) {
        debkeydata[handle].emitter_momentum.x = x;
        debkeydata[handle].emitter_momentum.y = y;
        debkeydata[handle].emitter_momentum.z = z;
    }
}

__declspec(noinline) void __cdecl DebrisParticleMomentum(i32 handle, f32 x, f32 y, f32 z) {
    if (handle != -1) {
        debkeydata[handle].momentum.x = x;
        debkeydata[handle].momentum.y = y;
        debkeydata[handle].momentum.z = z;
    }
}

i32 __cdecl PARTLookupType(char *name) {
    i32 index;
    if (name != NULL && name[0] != 0) {
        for (index = 0; index < 128; ++index) {
            if (NuStrNICmp(name, part_types[index].name, 16) == 0)
                return index;
        }
    }
    return -1;
}

#include "saga.h"
#include <string.h>
static __declspec(noinline) void DebrisMtxCopyPrivate(NUMTX *destination, const NUMTX *source) {
    destination->m00 = source->m00;
    destination->m01 = source->m01;
    destination->m02 = source->m02;
    destination->m03 = source->m03;
    destination->m10 = source->m10;
    destination->m11 = source->m11;
    destination->m12 = source->m12;
    destination->m13 = source->m13;
    destination->m20 = source->m20;
    destination->m21 = source->m21;
    destination->m22 = source->m22;
    destination->m23 = source->m23;
    destination->m30 = source->m30;
    destination->m31 = source->m31;
    destination->m32 = source->m32;
    destination->m33 = source->m33;
}

static __declspec(noinline) void DebrisMtxRotateXPrivate(NUMTX *m, i32 a) {
    f32 cosx;
    f32 sinx;
    f32 m01;
    f32 m11;
    f32 m21;
    f32 m31;
    cosx = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
    sinx = NuTrigTable[(a >> 1) & 0x7fff];
    m01 = m->m01;
    m11 = m->m11;
    m21 = m->m21;
    m31 = m->m31;

    m->m01 = (m01 * cosx) - (m->m02 * sinx);
    m->m02 = (m01 * sinx) + (m->m02 * cosx);
    m->m11 = (m11 * cosx) - (m->m12 * sinx);
    m->m12 = (m11 * sinx) + (m->m12 * cosx);
    m->m21 = (m21 * cosx) - (m->m22 * sinx);
    m->m22 = (m21 * sinx) + (m->m22 * cosx);
    m->m31 = (m31 * cosx) - (m->m32 * sinx);
    m->m32 = (m31 * sinx) + (m->m32 * cosx);
}

static __declspec(noinline) void DebrisMtxRotateYPrivate(NUMTX *m, i32 a) {
    f32 cosx;
    f32 sinx;
    f32 m00;
    f32 m10;
    f32 m20;
    f32 m30;
    cosx = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
    sinx = NuTrigTable[(a >> 1) & 0x7fff];
    m00 = m->m00;
    m10 = m->m10;
    m20 = m->m20;
    m30 = m->m30;

    m->m00 = m00 * cosx + m->m02 * sinx;
    m->m02 = m->m02 * cosx - m00 * sinx;
    m->m10 = m10 * cosx + m->m12 * sinx;
    m->m12 = m->m12 * cosx - m10 * sinx;
    m->m20 = m20 * cosx + m->m22 * sinx;
    m->m22 = m->m22 * cosx - m20 * sinx;
    m->m30 = m30 * cosx + m->m32 * sinx;
    m->m32 = m->m32 * cosx - m30 * sinx;
}

static __declspec(noinline) void DebrisMtxRotateZPrivate(NUMTX *m, i32 a) {
    f32 cosx;
    f32 sinx;
    f32 m00;
    f32 m10;
    f32 m20;
    f32 m30;
    cosx = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
    sinx = NuTrigTable[(a >> 1) & 0x7fff];
    m00 = m->m00;
    m10 = m->m10;
    m20 = m->m20;
    m30 = m->m30;

    m->m00 = (m00 * cosx) - (m->m01 * sinx);
    m->m01 = (m00 * sinx) + (m->m01 * cosx);
    m->m10 = (m10 * cosx) - (m->m11 * sinx);
    m->m11 = (m10 * sinx) + (m->m11 * cosx);
    m->m20 = (m20 * cosx) - (m->m21 * sinx);
    m->m21 = (m20 * sinx) + (m->m21 * cosx);
    m->m30 = (m30 * cosx) - (m->m31 * sinx);
    m->m31 = (m30 * sinx) + (m->m31 * cosx);
}

void __cdecl DebrisOrientation(i32 handle, i16 z, i16 y) {
    if (handle == -1)
        return;
    DebrisMtxCopyPrivate(&debkeydata[handle].effect_orientation, &ANI3_EVAL_IDENTITY_MATRIX);
    DebrisMtxRotateZPrivate(&debkeydata[handle].effect_orientation, z);
    DebrisMtxRotateYPrivate(&debkeydata[handle].effect_orientation, y);
    debkeydata[handle].orientation_dirty = 0.0f;
}
void __cdecl DebrisEmitterOrientation(i32 handle, i16 z, i16 y, i16 x) {
    if (handle == -1)
        return;
    DebrisMtxCopyPrivate(&debkeydata[handle].emitter_orientation, &ANI3_EVAL_IDENTITY_MATRIX);
    DebrisMtxRotateZPrivate(&debkeydata[handle].emitter_orientation, z);
    DebrisMtxRotateYPrivate(&debkeydata[handle].emitter_orientation, y);
    DebrisMtxRotateXPrivate(&debkeydata[handle].emitter_orientation, x);
    debkeydata[handle].orientation_dirty = 0.0f;
}

__declspec(noinline) void DebrisReflectionOrientation(i32 handle, i16 x, i16 y, f32 plane, f32 scale) {
    if (handle != -1) {
        debkeydata[handle].reflection_x = x;
        debkeydata[handle].reflection_y = y;
        debkeydata[handle].collision_plane = plane;
        debkeydata[handle].reflection_scale = scale;
    }
}

void DebrisSetTrigger(i32 handle, i32 first, i32 second, f32 third) {
    if (handle != -1) {
        debkeydata[handle].trigger_first = first;
        debkeydata[handle].trigger_second = second;
        debkeydata[handle].switch_variable = third;
    }
}

void DebrisStartOffset(i32 handle, f32 offset) {

    DebrisStartOffsetEx(debkeydata + handle, offset);
}

void AddDebrisEffect(i32 *handle, i32 effect_index, f32 x, f32 y, f32 z) {
    i32 i;
    DEBRIS_INFO_PC *effect;
    i32 newly_allocated;
    i32 key_index;
    f32 now;
    DEBRISKEY_PC *key;
    if (effect_index == -1 || effect_index == 0 || effect_index >= debinf_count) {
        return;
    }
    effect = debtab[effect_index];
    if (effect == NULL)
        return;
    if (effect->disabled != 0) {
        return;
    }

    newly_allocated = 0;
    key_index = *handle;
    if (key_index == -1) {
        key_index = DebAlloc();
        *handle = key_index;
        if (key_index == -1) {
            return;
        }
        newly_allocated = 1;
    }

    now = effect->time_group == 4 ? panelglobaltime : globaltime;
    debkeydata[key_index].effect_index = (i16)(effect_index);
    debkeydata[key_index].finite = 0;
    DebrisStartOffset(key_index, effect->emission_start_offset);
    debkeydata[key_index].generator = gensorttab[(i8)(effect->generator_type)];
    debkeydata[key_index].momentum_adjuster = gencodetab[(i8)(effect->momentum_adjustment_type)];
    debkeydata[key_index].finite_count = 0;
    debkeydata[key_index].emitter_rotation_x = 0;
    debkeydata[key_index].emitter_rotation_y = 0;
    debkeydata[key_index].sphere_index = 0;
    debkeydata[key_index].sphere_next_time = 0.0f;
    for (i = 0; i < debtab[effect_index]->process_spheres; ++i) {
        debkeydata[key_index].process_spheres[i].time = -1.0f;
    }
    debkeydata[key_index].emission_position.x = 0.0f;
    debkeydata[key_index].emission_position.y = 0.0f;
    debkeydata[key_index].emission_position.z = 0.0f;
    debkeydata[key_index].momentum.x = 0.0f;
    debkeydata[key_index].momentum.y = 0.0f;
    debkeydata[key_index].momentum.z = 0.0f;
    debkeydata[key_index].emitter_momentum.x = 0.0f;
    debkeydata[key_index].emitter_momentum.y = 0.0f;
    debkeydata[key_index].emitter_momentum.z = 0.0f;
    debkeydata[key_index].field_2f2 = -1;
    debkeydata[key_index].orientation_flags = 7;
    debkeydata[key_index].gscene = NULL;
    debkeydata[key_index].orientation_dirty = 0.0f;
    debkeydata[key_index].cutoff_distance = 999999.0f;
    debkeydata[key_index].process_collision_sound = 0;
    debkeydata[key_index].last_update_time = now;
    debkeydata[key_index].field_2f9 = 1;
    debkeydata[key_index].user_data = NULL;
    debkeydata[key_index].field_2fa = 0;
    debkeydata[key_index].emission_epoch =
        debkeydata[key_index].emission_end_time < now ? debkeydata[key_index].emission_end_time : now;
    debkeydata[key_index].emission_disabled = 0;

    switch (effect->particle_type) {
        case 3:
            debkeydata[key_index].render_priority = (i16)(40000);
            break;
        case 7:
            debkeydata[key_index].render_priority = (i16)(20000);
            break;
        case 2:
            debkeydata[key_index].render_priority = (i16)(50000);
            break;
        default:
            debkeydata[key_index].render_priority = (i16)(30000);
            break;
    }
    for (i = 0; i < 4; ++i) {
        debkeydata[key_index].collision_timers[i] = 9999;
        if (effect->sound_data[i * 3] != -1) {
            if (effect->sound_data[i * 3 + 1] == 3 || effect->sound_data[i * 3 + 1] == 4)
                debkeydata[key_index].collision_timers[i] = 1;
            debkeydata[key_index].process_collision_sound = 1;
        }
    }
    DebrisEmitterPos(key_index, x, y, z);
    DebrisEmitterOrientation(key_index, 0, 0, 0);
    DebrisOrientation(key_index, 0, 0);
    DebrisReflectionOrientation(key_index, 0, 0, 0, 0.9f);
    DebrisSetTrigger(key_index, 0, -1, 0);
    if (newly_allocated) {
        key = debkeydata + key_index;
        if (debris_keystack != NULL)
            debris_keystack->next = key;
        key->previous = debris_keystack;
        debris_keystack = key;
    }
}

void AddFiniteShotDebrisEffect2(i32 *handle, i32 effect, NUVEC *position, NUVEC *emitter_momentum,
                                NUVEC *particle_momentum, i32 count) {
    i32 sound;
    f32 now;
    f32 offset;
    f32 range;
    f32 volume;
    if (debris_suspended != 0 || update_debris_enabled == 0)
        return;
    AddDebrisEffect(handle, effect, position->x, position->y, position->z);
    if (*handle == -1)
        return;
    debkeydata[*handle].finite_count = count;
    now = debtab[effect]->time_group == 4 ? panelglobaltime : globaltime;
    debkeydata[*handle].emission_disabled = 0;
    offset = debkeydata[*handle].emission_time - now;
    debkeydata[*handle].emission_time -= offset;
    debkeydata[*handle].emission_end_time -= offset;
    debkeydata[*handle].emission_epoch = debkeydata[*handle].last_update_time;
    debkeydata[*handle].finite = 1;
    debkeydata[*handle].cutoff_distance =
        (debris_cutoff_camera_vec != NULL ? NuVecDist(position, (NUVEC *)debris_cutoff_camera_vec, NULL) : 0.0f);
    if (debkeydata[*handle].process_collision_sound != 0) {
        DEBRIS_INFO_PC **effects = debtab;
        volume = 0.0f;
        if (effects[effect]->sound_range_override == 0.0f) {
            if (effects[effect]->sound_range == 0.0f)
                range = effects[effect]->clip_extent;
            else
                range = effects[effect]->sound_range;
        } else {
            range = effects[effect]->sound_range_override;
        }
        if (debkeydata[*handle].cutoff_distance < range) {
            for (sound = 0; sound < 4; ++sound) {
                if (effects[effect]->sound_data[sound * 3] != -1)
                    SetSfxBit_On(effects[effect]->sound_data[sound * 3]);
            }
            volume = (range - debkeydata[*handle].cutoff_distance) / range;
        }
        if (debkeydata[*handle].cutoff_distance < range) {
            for (sound = 0; sound < 4; ++sound) {
                if (effects[effect]->sound_data[sound * 3] != -1 && effects[effect]->sound_data[sound * 3 + 1] == 1) {
                    PlaySfxByIdEx(effects[effect]->sound_data[sound * 3], position, volume, 1.0f);
                    effects = debtab;
                }
            }
        }
    }
    if (emitter_momentum != NULL) {
        DebrisEmitterMomentum(*handle, emitter_momentum->x, emitter_momentum->y, emitter_momentum->z);
    }
    if (particle_momentum != NULL) {
        DebrisParticleMomentum(*handle, particle_momentum->x, particle_momentum->y, particle_momentum->z);
    }
}

void AddFiniteShotDebrisEffect(i32 *handle, i32 effect, NUVEC *position, i32 count) {
    AddFiniteShotDebrisEffect2(handle, effect, position, NULL, NULL, count);
}

i32 __cdecl DebrisNuASin(f32);
DEBRISPARTICLE_PC *GenDebIndexSpheroid(DEBRISKEY_PC *key, DEBRIS_INFO_PC *effect, float time) {
    i32 trail;
    i16 particle_index;
    DEBRISPARTICLE_PC *particle;
    NUVEC vector;
    i32 angle_y;
    f64 random;
    f32 sine;
    i16 angle_z;
    NUMTX scale;
    i16 trail_index;
    DEBRISPARTICLE_PC *trail_particle;
    if (key->particle_cursor >= key->particle_count)
        key->particle_cursor = 0;
    particle_index = key->particle_cursor;

    if (effect->particle_type == 7)
        particle = &key->particle_chunks[particle_index / 12]->particles[particle_index % 12];
    else
        particle = &key->particle_chunks[particle_index / 32]->particles[particle_index % 32];
    ++key->particle_cursor;
    particle->start_time = time;
    key->emission_epoch = time;
    particle->inverse_lifetime = 64.0f / effect->particle_lifetime;

    vector.x = NuFsqrt(NuRandFloatSeeded(&debrisseed));
    vector.y = 0.0f;
    vector.z = 0.0f;
    angle_y = (i32)(NuRandFloatSeeded(&debrisseed) * 65536.0f);
    random = NuRandFloatSeeded(&debrisseed);
    sine = random + random - 1.0f;
    angle_z = (i16)(DebrisNuASin(sine));
    NuVecRotateZ(&vector, &vector, angle_z);
    NuVecRotateY(&vector, &vector, angle_y);

    DebrisMtxCopyPrivate(&scale, &ANI3_EVAL_IDENTITY_MATRIX);
    scale.m00 = effect->emitter_random_extent.x;
    scale.m11 = effect->emitter_random_extent.y;
    scale.m22 = effect->emitter_random_extent.z;
    NuVecMtxTransformVU0(&vector, &vector, &scale);
    NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
    particle->position.x = vector.x;
    particle->position.y = vector.y;
    particle->position.z = vector.z;
    random = NuRandFloatSeeded(&debrisseed);
    vector.x = (random + random) * effect->momentum_random_x - effect->momentum_random_x;
    random = NuRandFloatSeeded(&debrisseed);
    vector.y = (random + random) * effect->rotor_speed_x - effect->rotor_speed_x + effect->momentum_base;
    random = NuRandFloatSeeded(&debrisseed);
    vector.z = (random + random) * effect->rotor_speed_y - effect->rotor_speed_y;
    NuVecMtxTransformVU0(&vector, &vector, &key->emitter_orientation);
    particle->momentum.x = vector.x;
    particle->momentum.y = vector.y;
    particle->momentum.z = vector.z;
    if (key->momentum_adjuster != NULL) {
        key->momentum_adjuster(key, effect, particle);
    }
    particle->position.x = key->emission_position.x + particle->position.x;
    particle->position.y = key->emission_position.y + particle->position.y;
    particle->position.z = key->emission_position.z + particle->position.z;
    particle->momentum.x = key->momentum.x + particle->momentum.x;
    particle->momentum.y = key->momentum.y + particle->momentum.y;
    particle->momentum.z = key->momentum.z + particle->momentum.z;

    for (trail = 0; trail < (i8)(effect->trail_count); ++trail) {
        if (key->particle_cursor >= key->particle_count)
            key->particle_cursor = 0;
        trail_index = key->particle_cursor;
        if (effect->particle_type == 7)
            trail_particle = &key->particle_chunks[trail_index / 12]->particles[trail_index % 12];
        else
            trail_particle = &key->particle_chunks[trail_index / 32]->particles[trail_index % 32];
        ++key->particle_cursor;
        *trail_particle = *particle;
        trail_particle->start_time += (f32)(trail + 1) * effect->trail_time;
    }

    if (effect->native_data == NULL) {
        GenericDebinfoDmaTypeUpdate(effect);
    }
    return particle;
}

extern LEVELDATA_PC *CLOUDCITYTRAPB_LDATA;
extern LEVELDATA_PC *MAULA_LDATA;
extern LEVELDATA_PC *MAULF_LDATA;
extern LEVELDATA_PC *DOOKUC_LDATA;
extern const char ForceThrowExplodeNamePc[9];
i32 __cdecl AddGameDebris(APIDEBRISSYS_PC *, i32, NUVEC *);
void PartKill_ForceThrow(PART_PC *part, i32 reason) {
    LEVELDATA_PC *level;
    i32 debris = -1;
    i32 part_debris = -1;
    i32 type;
    level = WorldInfo_CurrentlyActive()->current_level;
    if (level == CLOUDCITYTRAPB_LDATA) {
        debris = 0x75;
        part_debris = 8;
    } else if (level == MAULA_LDATA) {
        debris = 0x2d;
        part_debris = 9;
    } else if (level == MAULF_LDATA) {
        for (type = 0; part_debris == -1 && type < 3; ++type) {
            if (NuSpecialExistsFn(&LevHSpecial[type]) && NuSpecialCompare(&LevHSpecial[type], &part->special))
                part_debris = type == 0 ? 11 : type == 1 ? 12 : 13;
        }
        if (part_debris == -1)
            part_debris = 11;
    } else if (level == DOOKUC_LDATA) {
        debris = 0x34;
        part_debris = 10;
    }
    if (debris != -1)
        AddGameDebris(WORLD->game_debris_sys, debris, &part->position);
    if (part_debris != -1)
        AddPartDebris(WORLD->part_debris_sys, part_debris, &part->position);
    NewRumbleAllPlayers(0.7f, 0.0f, 0, 0);
    GameCam_Judder(GameCam, qrand() < 0x8000 ? 0.4f : -0.4f, 2, NULL);
    GameCam_NewShake(GameCam, 0.6f, 0.6f, 1.0f);
    PlaySfx((char *)ForceThrowExplodeNamePc, &part->position);
}

void __cdecl Hub_LockUnlockDoors(WORLDINFO *world);

void __cdecl Areas_OpenAll(i32 mode) {
    i32 i;

    for (i = 0; i < AREACOUNT; i++) {
        Game.area_save[i].complete = 1;
        Game.area_save[i].area_complete = 1;
        if (ADataList[i].challenge_trial_time > 0) {
            Game.area_save[i].challenge_trial_time = (f32)(ADataList[i].challenge_trial_time - 0.5);
        }
        if (mode != 0) {
            Game.area_save[i].challenge_unlocked = 1;
        }
    }
    Game.unknown_game_02[1] = 1;
    if (WORLD != NULL && WORLD->current_level == HUB_LDATA) {
        Hub_LockUnlockDoors(WORLD);
    }
    ReCalculateCompletionPoints();
}

extern void *game_cutscenes_droid_factory_conveyor;
extern void *game_cutscenes_podrace_avalanche;

void __cdecl FindGameCutScenes(void) {
    game_cutscenes_podrace_pod_explode = NULL;
    game_cutscenes_podrace_out_of_time = NULL;
    game_cutscenes_bonus_gunship_cavalry_explode = NULL;
    game_cutscenes_droid_factory_conveyor = NULL;
    game_cutscenes_podrace_avalanche = NULL;
    game_cutscenes_dogfight_die = NULL;
    game_cutscenes_podrace_sebulba = NULL;
    game_cutscenes_cutscene = NULL;
    game_cutscenes_podsprint_out_of_time = NULL;
    game_cutscenes_podsprint_sebulba = NULL;

    game_cutscenes_podrace_pod_explode = CutScene_Find(WORLD->cutscene_sys, "ep1_podrace_podexplode");
    game_cutscenes_podrace_out_of_time = CutScene_Find(WORLD->cutscene_sys, "ep1_podrace_outoftime");
    game_cutscenes_bonus_gunship_cavalry_explode =
        CutScene_Find(WORLD->cutscene_sys, "ep2_bonus_gunshipcavalry_explode");
    game_cutscenes_droid_factory_conveyor = CutScene_Find(WORLD->cutscene_sys, "ep2_droidfactory_conveyor");
    game_cutscenes_podrace_avalanche = CutScene_Find(WORLD->cutscene_sys, "ep1_podrace_avalanche");
    game_cutscenes_dogfight_die = CutScene_Find(WORLD->cutscene_sys, "ep3_dogfight_die");
    game_cutscenes_podrace_sebulba = CutScene_Find(WORLD->cutscene_sys, "ep1_podrace_sebulba");
    game_cutscenes_cutscene = CutScene_Find(WORLD->cutscene_sys, "ep1_podsprint_avalanche");
    game_cutscenes_podsprint_out_of_time = CutScene_Find(WORLD->cutscene_sys, "ep1_podsprint_outoftime");
    game_cutscenes_podsprint_sebulba = CutScene_Find(WORLD->cutscene_sys, "ep1_podsprint_sebulba");
}

void GizmoBlowupCreateStuff(GIZMOBLOWUP_PC *blowup) {
    i32 coins;
    NUVEC direction;
    char script[32] = "GoonBox";
    i32 model = GetGenericGoon(0);
    f32 choice = qrand() * (1.0f / 65535.0f) * 100.0f;
    if (!(choice < 50.0f)) {
        choice -= 50.0f;
        if (choice < 50.0f) {
            i32 i;
            i32 count = (i32)(NuFloatRand((NURAND *)(&GAMERAND)) * 3.0f);
            for (i = 0; i <= count; ++i) {
                i32 angle = (u16)(NuFloatRand((NURAND *)(&GAMERAND)) * 65536.0f);
                GAMEOBJECT *object =
                    AddDynamicCreature(model, &blowup->mid_position, angle, script, NULL, NULL, 1, NULL, NULL, 0, -1);
                if (object != NULL) {
                    object->collision_scale_override = 0.0f;
                    object->collision_scale = 0.0f;
                    object->origin_scale = 0.0f * object->character_data->model_scale;
                } else {
                    NUVEC direction;
                    i32 coins = ((i32)(NuFloatRand((NURAND *)(&GAMERAND)) * 1000.0f) / 100) * 100;
                    if (coins < 100)
                        coins = 100;
                    direction = v010;
                    NuVecRotateX(&direction, &direction, (u16)(blowup->pickup_x_rotation));
                    NuVecRotateY(&direction, &direction, (u16)(blowup->pickup_y_rotation));
                    AddPickups(coins, ReleaseHearts(), 0, 0, &blowup->mid_position, &direction, 0.0f, -1, 1.5f,
                               2000000.0f, NULL, 1, 1);
                }
            }
            return;
        }
        choice -= 50.0f;
        if (choice < 0.0f)
            return;
    }
    coins = ((i32)(NuFloatRand((NURAND *)(&GAMERAND)) * 1000.0f) / 100) * 100;
    if (coins < 100)
        coins = 100;
    direction = v010;
    NuVecRotateX(&direction, &direction, (u16)(blowup->pickup_x_rotation));
    NuVecRotateY(&direction, &direction, (u16)(blowup->pickup_y_rotation));
    AddPickups(coins, ReleaseHearts(), 0, 0, &blowup->mid_position, &direction, 0.0f, -1, 1.5f, 2000000.0f, NULL, 1, 1);
}

static __declspec(noinline) f32 DebrisDivide(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f)
        return 0.0f;
    return a / b;
}

void __cdecl DebrisEmitterOrientationMtx(i32 handle, NUMTX *source) {
    if (handle != -1) {
        debkeydata[handle].emitter_orientation = *source;
        debkeydata[handle].emitter_orientation.m30 = 0.0f;
        debkeydata[handle].emitter_orientation.m31 = 0.0f;
        debkeydata[handle].emitter_orientation.m32 = 0.0f;
        debkeydata[handle].orientation_dirty = 0.0f;
    }
}

void __cdecl AddVariableShotDebrisEffectTimed5(i32 effect_index, NUVEC *position, NUVEC *momentum,
                                               NUVEC *position_delta, i32 count, f32 duration,
                                               NUMTX *emitter_orientation, NUMTX *particle_orientation,
                                               u16 render_priority, i8 timed_flags) {
    DEBRIS_INFO_PC *effect;
    NUVEC clipped_position;

    f32 thinning;
    f32 emission_interval;
    f32 rate;
    f32 now;
    f32 end_time;
    f32 elapsed_intervals;
    f32 emission_time;
    f32 emission_epoch;
    f32 next_emission_time;
    i32 panel_time;
    i32 emission_count;
    i32 i;
    i32 j;
    i32 particle;
    i32 particle_count;
    i32 maximum_particles;
    i32 particle_key_slot;
    i32 new_key_index;
    DEBRISKEY_PC *key;
    i32 required_particles;
    i32 required_chunks;
    i32 allocated_chunks;
    i32 new_chunk_count;
    i32 total_chunk_count;
    DEBRISPARTICLE_PC *generated;
    i32 chunk_index;
    DEBRIS_CHUNK_CONTROL_PC *control;
    DEBRIS_CHUNK_CONTROL_PC **stack;
    DEBRIS_PARTICLE_CHUNK_PC *last_chunk;

    if (debris_suspended != 0 || update_debris_enabled == 0 || effect_index == -1 || effect_index == 0 ||
        debinf_count <= effect_index)
        return;
    effect = debtab[effect_index];
    if (effect == NULL || effect->disabled != 0 || count < 1)
        return;
    if (4 != effect->time_group) {
        NUVEC extent;
        clipped_position.x = position->x;
        clipped_position.y = position->y;
        clipped_position.z = position->z;
        extent.x = 1.0f;
        extent.y = 1.0f;
        extent.z = 1.0f;
        if (NuCameraClipTestExtentsAxisAligned(&clipped_position, &extent, effect->clip_extent) == 0)
            return;
    }
    thinning = 0 != forced_debris_thinning
                   ? debris_thinning_level
                   : (effect->thinning < debris_thinning_level ? effect->thinning : debris_thinning_level);
    rate = DebrisDivide((f32)count, thinning);
    emission_interval = DebrisDivide(1.0f, rate);
    if (emitter_orientation == NULL)
        emitter_orientation = &NuMtxSetIdentityTemplate;
    if (particle_orientation == NULL)
        particle_orientation = &NuMtxSetIdentityTemplate;
    panel_time = effect->time_group == 4;
    now = panel_time ? panelglobaltime : globaltime;
    end_time = now + duration;
    emission_epoch = ((f32)(i32)DebrisDivide(now, emission_interval)) * emission_interval;
    emission_time = emission_interval + emission_epoch;
    if (!(emission_time <= end_time))
        return;
    emission_count = 0;
    next_emission_time = emission_time;
    i = 0;
    while (next_emission_time <= end_time) {
        if (++i >= 100)
            break;
        ++emission_count;
        next_emission_time += emission_interval;
    }
    if (emission_count == 0)
        return;
    particle_count = emission_count * ((i32)effect->trail_count + 1);
    maximum_particles = effect->particle_type == 7 ? 0x180 : 0x400;
    particle_key_slot = -1;
    key = NULL;
    for (i = 0; i < 8; ++i) {
        if (effect->particle_keys[i] != -1) {
            key = &debkeydata[effect->particle_keys[i]];
            if (key->particle_count + particle_count <= maximum_particles) {
                particle_key_slot = i;
                break;
            }
        } else if (freedebkeyptr < maxdebkeys) {
            new_key_index = DebAlloc();
            effect->particle_keys[i] = (i16)new_key_index;
            key = &debkeydata[(i16)new_key_index];
            key->effect_index = (i16)effect_index;
            key->finite_count = 0;
            key->generator = gensorttab[effect->generator_type];
            key->momentum_adjuster = gencodetab[effect->momentum_adjustment_type];
            key->effect_orientation = *particle_orientation;
            key->effect_orientation.m30 = 0.0f;
            key->effect_orientation.m31 = 0.0f;
            key->effect_orientation.m32 = 0.0f;
            DebrisEmitterPos(effect->particle_keys[i], 0.0f, 0.0f, 0.0f);
            key->render_priority = render_priority;
            key->timed_flags = timed_flags;
            particle_key_slot = i;
            break;
        }
    }
    if (key == NULL)
        return;
    required_particles = key->particle_count + particle_count;
    if (effect->particle_type == 7) {
        allocated_chunks = key->allocated_chunk_count;
        if (allocated_chunks * 12 < required_particles) {
            if (allocated_chunks <= -1) {
                key->allocated_chunk_count = 0;
            }
            required_chunks = (required_particles + 12 - 1) / 12;
            new_chunk_count = required_chunks - key->allocated_chunk_count;
            if (debrischunksglass <= freedebchkptrg + new_chunk_count || required_chunks > 32)
                return;
            for (i = 0; i < new_chunk_count; ++i) {
                key->particle_chunks[key->allocated_chunk_count] = freedebchunksglass[freedebchkptrg++];
                for (particle = 0; particle < 12; ++particle) {
                    key->particle_chunks[key->allocated_chunk_count]->particles[particle].start_time = 0.0f;
                    key->particle_chunks[key->allocated_chunk_count]->particles[particle].inverse_lifetime = 32768.0f;
                }
                ++key->allocated_chunk_count;
            }
            key->allocated_chunk_count = (i16)required_chunks;
            LinkDmaParticalSets(key->particle_chunks, required_chunks);
            if (required_chunks == new_chunk_count) {
                total_chunk_count = debrischunks + debrischunksglass;
                for (j = 0; j < total_chunk_count; ++j) {
                    if (NULL == ParticleChunkToRender[j].particle_chunk) {
                        ParticleChunkToRender[j].particle_chunk = key->particle_chunks[0];
                        ParticleChunkToRender[j].effect = effect;
                        ParticleChunkToRender[j].key = key;
                        ParticleChunkToRender[j].render_priority = render_priority;
                        AddChunkToRenderStack(&ParticleChunkToRender[j], &ParticleChunkRenderStack[effect->time_group]);
                        break;
                    }
                }
            }
        }
    } else {
        allocated_chunks = key->allocated_chunk_count;
        if (allocated_chunks * 32 < required_particles) {
            if (allocated_chunks <= -1) {
                key->allocated_chunk_count = 0;
            }
            required_chunks = (required_particles + 32 - 1) / 32;
            new_chunk_count = required_chunks - key->allocated_chunk_count;
            if (debrischunks <= freedebchkptr + new_chunk_count || required_chunks > 32)
                return;
            for (i = 0; i < new_chunk_count; ++i) {
                key->particle_chunks[key->allocated_chunk_count] = freedebchunks[freedebchkptr++];
                for (particle = 0; particle < 32; ++particle) {
                    key->particle_chunks[key->allocated_chunk_count]->particles[particle].start_time = 0.0f;
                    key->particle_chunks[key->allocated_chunk_count]->particles[particle].inverse_lifetime = 32768.0f;
                }
                ++key->allocated_chunk_count;
            }
            key->allocated_chunk_count = (i16)required_chunks;
            LinkDmaParticalSets(key->particle_chunks, required_chunks);
            if (required_chunks == new_chunk_count) {
                total_chunk_count = debrischunks + debrischunksglass;
                for (j = 0; j < total_chunk_count; ++j) {
                    if (NULL == ParticleChunkToRender[j].particle_chunk) {
                        ParticleChunkToRender[j].particle_chunk = key->particle_chunks[0];
                        ParticleChunkToRender[j].effect = effect;
                        ParticleChunkToRender[j].key = key;
                        ParticleChunkToRender[j].render_priority = render_priority;
                        AddChunkToRenderStack(&ParticleChunkToRender[j], &ParticleChunkRenderStack[effect->time_group]);
                        break;
                    }
                }
            }
        }
    }
    key->particle_count += (i16)particle_count;
    if (NULL == momentum) {
        key->momentum.x = 0.0f;
        key->momentum.y = 0.0f;
        key->momentum.z = 0.0f;
    } else {
        key->momentum.x = momentum->x;
        key->momentum.y = momentum->y;
        key->momentum.z = momentum->z;
    }
    DebrisEmitterOrientationMtx(effect->particle_keys[particle_key_slot], emitter_orientation);
    key->emission_epoch = emission_epoch;
    for (i = 1; i < 100 && emission_time <= end_time; ++i) {
        if (position_delta == NULL) {
            key->emission_position.x = position->x;
            key->emission_position.y = position->y;
            key->emission_position.z = position->z;
        } else
            NuVecAddScale(&key->emission_position, position, position_delta, emission_time - end_time);
        key->emission_time = emission_time;
        generated = key->generator(key, effect, emission_time);
        if (effect->process_spheres != 0 && generated != NULL && i == 1)
            DebrisProcessSpheres(generated, emission_time, effect, key, 1);
        emission_time = key->emission_epoch + emission_interval;
    }
    DebrisGetControlStackLock();
    for (chunk_index = key->controlled_chunk_count; chunk_index < key->allocated_chunk_count; ++chunk_index) {
        if (freechunkcontrolsptr < (debrischunks + debrischunksglass) * 2) {
            control = freechunkcontrols[freechunkcontrolsptr];
            control->particle_chunk = key->particle_chunks[chunk_index];
            control->active = 1;
            control->owner = key;
            control->expiry_time = now + effect->particle_lifetime + effect->trail_time * ((f32)effect->trail_count);
            AddChunkControlToStack(control, &debris_chunk_control_stack[panel_time ? 1 : 0]);
            ++freechunkcontrolsptr;
            ++key->controlled_chunk_count;
        }
    }
    if (key->allocated_chunk_count == key->controlled_chunk_count) {
        last_chunk = key->particle_chunks[key->controlled_chunk_count - 1];
        stack = &debris_chunk_control_stack[panel_time ? 1 : 0];
        for (control = *stack; control != NULL; control = control->next) {
            if (control->particle_chunk == last_chunk) {
                RemoveChunkControlFromStack(control, stack);
                control->expiry_time = now + effect->particle_lifetime + (f32)effect->trail_count * effect->trail_time;
                AddChunkControlToStack(control, stack);
                break;
            }
        }
    }
    key->previous_particle_count = key->particle_count;
    key->previous_allocated_chunk_count = key->allocated_chunk_count;
    debris_control_stack_lock = 0;
}

void __cdecl AddVariableShotDebrisEffectMtx3(i32 effect, NUVEC *position, NUVEC *momentum, i32 count,
                                             NUMTX *emitter_orientation, NUMTX *particle_orientation) {
    u16 priority = 20000;
    if (effect < 0 || debtab[effect] == NULL)
        return;
    switch (debtab[effect]->particle_type) {
        case 2:
            priority = 40000;
            break;
        case 3:
            priority = 30000;
            break;
        case 7:
            priority = 10000;
            break;
    }
    AddVariableShotDebrisEffectTimed5(effect, position, momentum, NULL, count * 30, debris_time_increment,
                                      emitter_orientation, particle_orientation, priority, 0);
}

void __cdecl AddVariableShotDebrisEffect(i32 effect, NUVEC *position, i32 count, i16 z_rotation, i16 y_rotation) {
    NUMTX orientation;
    DebrisMtxCopyPrivate(&orientation, &ANI3_EVAL_IDENTITY_MATRIX);
    DebrisMtxRotateZPrivate(&orientation, z_rotation);
    DebrisMtxRotateYPrivate(&orientation, y_rotation);
    AddVariableShotDebrisEffectMtx3(effect, position, &APIObjectZeroMomentumPC, count, &orientation, NULL);
}

i32 AddScaledVariableShotDebrisEffect(i32 effect, NUVEC *position, i32 count, f32 time, i16 z_rotation, i16 y_rotation,
                                      f32 scale) {
    i32 scaled = CreateScaledEffect(effect, scale);
    if (scaled != -1)
        AddVariableShotDebrisEffectTimed1(scaled, position, count, time, z_rotation, y_rotation, NULL);
    return scaled;
}

void __cdecl DebrisDrawCalculateClipBoxes(DEBRIS_INFO_PC *effect, DEBRISKEY_PC *key) {
    NUMTX matrix;
    f64 emission_time;
    f32 padding, radius;
    NUVEC extent, minimum;
    union {
        f32 value;
        u32 bits;
    } velocity_x, velocity_y, velocity_z, momentum;
    DebrisMtxCopyPrivate(&matrix, &key->effect_orientation);
    NuMtxTranslate(&matrix, &key->position);
    velocity_x.value = effect->emitter_velocity.x;
    /* Keep the period sum unrounded until each extent is stored. */
    emission_time = (f64)effect->emission_period_random + effect->emission_period;
    velocity_x.bits &= 0x7fffffff;
    extent.x = (velocity_x.value * emission_time + effect->momentum_random_x * effect->particle_lifetime) +
               effect->emitter_random_extent.x;
    velocity_y.value = effect->emitter_velocity.y;
    velocity_y.bits &= 0x7fffffff;
    momentum.value = effect->momentum_base;
    momentum.bits &= 0x7fffffff;
    extent.y =
        (velocity_y.value * emission_time + (momentum.value + effect->rotor_speed_x) * effect->particle_lifetime) +
        effect->emitter_random_extent.y;
    velocity_z.value = effect->emitter_velocity.z;
    velocity_z.bits &= 0x7fffffff;
    extent.z = (velocity_z.value * emission_time + effect->rotor_speed_y * effect->particle_lifetime) +
               effect->emitter_random_extent.z;
    NuVecMtxRotate(&extent, &extent, &key->emitter_orientation);
    if (extent.x < 0.0f)
        extent.x = -extent.x;
    if (extent.y < 0.0f)
        extent.y = -extent.y;
    if (extent.z < 0.0f)
        extent.z = -extent.z;
    padding = (f32)((f64)0.0001f * effect->max_size);
    /* Retail rounds after each padding pass. */
    extent.x += padding;
    extent.y += padding;
    extent.z += padding;
    extent.x = (f32)(extent.x + (f64)0.2f);
    extent.y = (f32)(extent.y + (f64)0.2f);
    extent.z = (f32)(extent.z + (f64)0.2f);
    minimum.x = -extent.x;
    minimum.y = -extent.y;
    minimum.z = -extent.z;
    if (effect->gravity > 0.0f)
        extent.y += (effect->gravity * effect->particle_lifetime) * effect->particle_lifetime;
    else if (effect->gravity < 0.0f)
        minimum.y += (effect->gravity * effect->particle_lifetime) * effect->particle_lifetime;
    radius = MAX(MAX(extent.x, extent.z), MAX(extent.y, -minimum.y));
    key->clip_radius = radius;
    key->clip_min = minimum;
    key->clip_max = extent;
    DebrisMtxCopyPrivate(&key->clip_matrix, &matrix);
}

void __cdecl DebrisDraw(i32 unused, i32 pass) {
    DEBRIS_RENDER_CHUNK_PC *chunk;
    i32 drawn = 0;
    (void)unused;
    if (debris_suspended != 0)
        return;
    _NuTimeBarSlotBegin(0, 12, DebrisTimeBarName);
    DebMat[7]->texture_id = (i16)debris_rt;
    for (chunk = ParticleChunkRenderStack[pass]; chunk != NULL; chunk = chunk->next) {
        DEBRIS_INFO_PC *effect;
        DEBRISKEY_PC *key;
        NUMTX matrix;
        NUVEC position;
        f32 render_time;
        if (chunk->particle_chunk == NULL)
            continue;
        effect = chunk->effect;
        key = chunk->key;
        if (DebrisCutSceneMode != 0 && effect->cutscene_only != 0)
            continue;
        if (DebrisSuspendDrawObjectSwitch != -1 && key != NULL && key->trigger_second == DebrisSuspendDrawObjectSwitch)
            continue;
        if (effect->native_data == NULL) {
            if (freeDmaDebType == EDPP_MAX_DMADEBTYPES)
                DebrisFreeOldestDmaDebTypeTable();
            GenericDebinfoDmaTypeUpdate(effect);
            if (effect->native_data == NULL)
                continue;
        }
        if (key != NULL) {
            i32 assigned = 0;
            i32 visible = 1;
            i32 i;
            for (i = 0; i < 8; ++i) {
                if (effect->particle_keys[i] != -1 && &debkeydata[effect->particle_keys[i]] == key) {
                    assigned = 1;
                    break;
                }
            }
            if (!assigned && effect->sound_range > 0.0f && effect->use_explicit_clip_box == 0 &&
                key->cutoff_distance > effect->sound_range)
                visible = 0;
            if (key->field_2f7 == 0)
                visible = 0;
            if (pass != 4 && visible && effect->generator_type == 0 && effect->use_explicit_clip_box == 0 &&
                !assigned) {
                NUVEC minimum, maximum;
                if (key->clip_radius == 0.0f || *edbits_editor_enabled != 0)
                    DebrisDrawCalculateClipBoxes(effect, key);
                if (edbits_editmode == 0 && key->gscene != NULL && key->field_2f2 != -1)
                    visible = NuPortalClipTest(key->gscene, &key->position, key->clip_radius, key->field_2f2) != 0;
                minimum = key->clip_min;
                maximum = key->clip_max;
                if (NuCameraClipTestExtents(&minimum, &maximum, &key->clip_matrix, 0.0f, 0) == 0)
                    continue;
            }
            if (!visible)
                continue;
            DebrisMtxCopyPrivate(&matrix, &key->effect_orientation);
            position.x = key->position.x;
            position.y = key->position.y;
            position.z = key->position.z;
        } else {
            if (effect->sound_range > 0.0f && effect->use_explicit_clip_box == 0 &&
                CameraEmitterDistance(&chunk->position) > effect->sound_range)
                continue;
            matrix = chunk->effect_orientation;
            position.x = chunk->position.x;
            position.y = chunk->position.y;
            position.z = chunk->position.z;
        }
        drawn = 1;
        NuMtxTranslate(&matrix, &position);
        if (pass == 4) {
            NuMtxMulVU0(&matrix, &matrix, NuCameraGetMtx());
            render_time = debris_render_panel_time;
            effect->last_render_time = panelglobaltime;
        } else {
            render_time = debris_render_global_time;
            effect->last_render_time = globaltime;
        }
        if (key != NULL && key->field_2fa != 0)
            NuRndrSetParticleRotation(&key->particle_orientation);
        else if (effect->camera_facing != 0)
            NuRndrSetParticleRotation(&xzfacingmtx);
        else
            NuRndrSetParticleRotation(NULL);
        NuMtxPreScaleX(&matrix, debris_render_scale);
        if (effect->particle_type == 7) {
            NuRndrParticleGroup(chunk->particle_chunk, chunk->effect->native_data,
                                DebMat[(i8)chunk->effect->particle_type], render_time, &matrix, 4,
                                effect->particle_parameter_a, effect->particle_parameter_b, effect->clip_extent,
                                effect->field_044);
        } else if (effect->use_explicit_clip_box != 0) {
            NuRndrSetDebBox(&effect->particle_box);
            NuRndrParticleGroup(chunk->particle_chunk, chunk->effect->native_data,
                                DebMat[(i8)chunk->effect->particle_type], render_time, &matrix, 6,
                                effect->particle_parameter_a, effect->particle_parameter_b, effect->clip_extent,
                                effect->field_044);
        } else {
            NuRndrParticleGroup(chunk->particle_chunk, chunk->effect->native_data,
                                DebMat[(i8)chunk->effect->particle_type], render_time, &matrix, 0,
                                effect->particle_parameter_a, effect->particle_parameter_b, effect->clip_extent,
                                effect->field_044);
        }
    }
    if (drawn != 0)
        g_renderingDebris = g_renderingDebris == 0;
    _NuTimeBarSlotEnd(0, 12);
}

void __cdecl MakePartVector(NUVEC *velocity, NUVEC *direction, f32 scale) {
    if (direction != NULL) {
        i32 divisor;
        u16 z_random, x_random;
        FindAnglesZX(direction, NULL, NULL);
        *velocity = v010;
        divisor = 65535 / (((PartVectorSpreadDegrees_PC * 2 + 1) << 16) / 360) + 1;
        z_random = (u16)(qrand() / divisor - ((PartVectorSpreadDegrees_PC << 16) / 360));
        x_random = (u16)(qrand() / divisor - ((PartVectorSpreadDegrees_PC << 16) / 360));
        NuVecRotateZ(velocity, velocity, (u16)(z_random + temp_xrot));
        NuVecRotateX(velocity, velocity, (u16)(x_random + temp_zrot));
        NuVecScale(velocity, velocity, scale);
    } else {
        f64 random = (f64)qrand() * (f64)(f32)(1.0f / 65535.0f);
        velocity->x = (f32)(random + random - 1.0);
        velocity->y = AreaPickupGravity == 0.0f ? 0.0f : (f32)((f64)qrand() * (f64)(f32)(1.0f / 65535.0f) + 1.0);
        random = (f64)qrand() * (f64)(f32)(1.0f / 65535.0f);
        velocity->z = (f32)(random + random - 1.0);
        NuVecScale(velocity, velocity, AreaPickupScale);
    }
}

static void __cdecl PartMove_VehiclePickup(PART_PC *part, f32 timestep) {
    f32 gain;
    GAMEOBJECT *object;
    (void)timestep;
    if (part->initial_time == 0.0f)
        gain = 0.0f;
    else
        gain = 1.0f - (part->scale_time != 0.0f ? part->scale_time / part->initial_time : 0.0f);
    if (gain <= 0.0f) {
        part->active |= 2;
        if (part->stop_fn != NULL)
            part->stop_fn(part);
        return;
    }
    object = FindNearestGameObject(&part->position, NULL, 0, 0.0f, 0.0f, -1, -1, 99, NULL, 0, NULL);
    if (object != NULL)
        part->velocity.y =
            SeekValF(part->velocity.y, (f32)((f64)(object->collision_position.y - part->position.y) * 3.0), 3.0f);
    part->position.x += part->velocity.x * gain * FRAMETIME;
    part->position.y += part->velocity.y * gain * FRAMETIME;
    part->position.z += part->velocity.z * gain * FRAMETIME;
}

static void __cdecl PartMove_VehicleHeart(PART_PC *part, f32 timestep) {
    GAMEOBJECT *recipient = part->recipient;
    f32 progress = 1.0f - part->time_remaining / PartVehicleHeartLifetime_PC;
    NUVEC start;
    f32 height;
    (void)timestep;
    if (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA) {
        NuVecAdd(&start, &recipient->collision_position, &part->previous_position);
        height = PartVehicleHeartPodraceHeight_PC;
    } else {
        start = part->previous_position;
        height = PartVehicleHeartHeight_PC;
    }
    part->position.x = start.x + (recipient->collision_position.x - start.x) * progress;
    part->position.y = start.y + (recipient->collision_position.y - start.y) * progress +
                       NuTrigTable[((i32)((f64)progress * 32768.0) >> 1) & 0x7fff] * height;
    part->position.z = start.z + (recipient->collision_position.z - start.z) * progress;
}

static i32 __cdecl PartDraw_VehicleHeart(PART_PC *part) {
    NUVEC scaling;
    NUMTX matrix;
    if (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA)
        scaling.x = PartVehicleHeartPodraceScale_PC;
    else if ((BONUS_GUNSHIP_ADATA != NULL && WORLD->area == BONUS_GUNSHIP_ADATA) ||
             (GUNSHIP_ADATA != NULL && WORLD->area == GUNSHIP_ADATA))
        scaling.x = PartVehicleHeartGunshipScale_PC;
    else
        scaling.x = PartVehicleHeartScale_PC;
    scaling.x = (1.0f - part->time_remaining / PartVehicleHeartLifetime_PC) * scaling.x;
    if (scaling.x > 0.0f) {
        scaling.y = scaling.z = scaling.x;
        matrix = part->transform;
        NuMtxPreScale(&matrix, &scaling);
        if (NuSpecialDrawAt(&part->special, &matrix) != 0)
            part->active |= 0x600;
    }
    return 0;
}

void __cdecl PartUpdate_Heart(PART_PC *part) {
    NUVEC position = part->position;
    part->rotation_y = (u16)(i32)((f64)part->rotation_y + (f64)FRAMETIME * 16384.0);
    NuMtxSetRotationY(&part->transform, part->rotation_y);
    NuMtxTranslate(&part->transform, &position);
}

static void __cdecl PowerUp_ImpactPart(PART_PC *part) {
    f32 minimum = (f32)((f64)AreaPickupScale * (f64)0.1f);
    f32 maximum = (f32)((f64)AreaPickupScale * 2.5);
    f32 speed = NuVecMag(&part->velocity);
    f32 volume;
    if (speed >= maximum)
        volume = 1.0f;
    else {
        if (!(speed >= minimum))
            return;
        volume = (speed - minimum) / (maximum - minimum);
    }
    if (volume > 0.0f)
        PlaySfxAndSetVolume((char *)PowerUpBounceSfx_PC, &part->position, volume);
}

static void __cdecl PowerUp_UpdatePart(PART_PC *part) {
    NUVEC position = part->position;
    part->rotation_y = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
    NuMtxSetRotationY(&part->transform, part->rotation_y);
    NuMtxTranslate(&part->transform, &position);
    PowerUp_Particles(WorldInfo_CurrentlyActive(), &position);
    PlaySfx((char *)PowerUpLoopSfx_PC, &position);
}

static i32 __cdecl PowerUp_DrawPart(PART_PC *part) {
    i32 draw = PartDraw_Flickerer(part);
    if (draw && WORLD->level_objects[0xd1].active != 0) {
        NUMTX matrix = part->transform;
        NuSpecialDrawAt(&WORLD->level_objects[0xd1].special, &matrix);
    }
    return draw;
}

void __cdecl AddHeartAsPart(GAMEOBJECT *recipient, NUVEC *position, NUVEC *velocity, f32 scale, f32 lifetime) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    ADDPART_PC params;
    PART_PC *part;
    if (recipient == NULL)
        return;
    params = Default_ADDPART;
    params.position = position;
    params.velocity = velocity;
    params.part_id = 0xcb;
    params.flags = 0x256;
    params.field_18 = params.field_14 = scale * GizmoPickupType[5].shadow_radius_x;
    params.gravity = AreaPickupGravity;
    params.special = &world->level_objects[0xcb].special;
    params.field_88 = lifetime;
    params.collision_fn = PartCollide_3D;
    params.update_fn = PartUpdate_Heart;
    if ((world->current_level->flags & 0x80000) != 0) {
        params.field_a4 = PartVehicleHeartLifetime_PC;
        params.move_fn = PartMove_VehicleHeart;
        params.flags |= 0x400;
        params.recipient = recipient;
        params.draw_fn = PartDraw_VehicleHeart;
    } else {
        params.stop_fn = PartStop_Flickerer;
        params.draw_fn = PartDraw_Flickerer;
        if (params.gravity == 0.0f)
            params.move_fn = PartMove_VehiclePickup;
    }
    params.time_step = FRAMETIME;
    params.field_c0 = scale;
    part = AddPart(&params);
    if (part != NULL)
        part->rotation_y = (u16)qrand();
}

void __cdecl AddTorpedoAsPart(NUVEC *position, NUVEC *velocity, f32 scale, f32 lifetime) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    ADDPART_PC params = Default_ADDPART;
    PART_PC *part;
    params.position = position;
    params.velocity = velocity;
    params.field_14 = (f32)((f64)WORLD->torpedo_machine_sys->scale * 4.0 * (f64)GizmoPickupType[8].shadow_radius_x);
    params.field_18 = (f32)((f64)WORLD->torpedo_machine_sys->scale * 4.0 * (f64)GizmoPickupType[8].shadow_extent_x);
    params.gravity = AreaPickupGravity;
    params.special = &world->level_objects[0x79].special;
    params.part_id = 0x79;
    params.flags = 0x256;
    if (params.gravity == 0.0f) {
        params.field_a4 = 5.0f;
        params.move_fn = PartMove_VehiclePickup;
    } else {
        params.stop_fn = PartStop_Flickerer;
    }
    params.collision_fn = PartCollide_3D;
    params.update_fn = NULL;
    params.draw_fn = PartDraw_Torp;
    params.field_88 = lifetime;
    params.time_step = FRAMETIME;
    params.field_c0 = scale;
    part = AddPart(&params);
    if (part != NULL) {
        part->rotation_y = (u16)qrand();
        part->field_206 = 3;
        part->field_214 = (f32)((f64)WORLD->torpedo_machine_sys->scale * 4.0 * (f64)NuTrigTable[0x2000]);
    }
}

void __cdecl PowerUp_AddPart(NUVEC *position, NUVEC *velocity, f32 scale, f32 lifetime) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    ADDPART_PC params = Default_ADDPART;
    PART_PC *part;
    params.position = position;
    params.velocity = velocity;
    params.special = &world->level_objects[0xd0].special;
    params.field_18 = params.field_14 = scale * GizmoPickupType[9].shadow_radius_x;
    params.gravity = AreaPickupGravity;
    params.part_id = 0xd0;
    params.flags = 0x256;
    params.impact_fn = PowerUp_ImpactPart;
    params.collision_fn = PartCollide_3D;
    params.update_fn = PowerUp_UpdatePart;
    params.stop_fn = PartStop_Flickerer;
    params.draw_fn = PowerUp_DrawPart;
    if (params.gravity == 0.0f)
        params.move_fn = PartMove_VehiclePickup;
    params.field_88 = lifetime;
    params.time_step = FRAMETIME;
    params.field_c0 = scale;
    part = AddPart(&params);
    if (part != NULL)
        part->rotation_y = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
}

static __declspec(noinline) void PartExtraCoinsPrivate(NUVEC *position, i32 index) {
    i32 effect = WORLD->game_debris_sys->entries[index].effect;
    f32 rate;
    i32 count;
    if (effect == -1)
        return;
    rate = PartExtraCoinRate_PC;
    if (WORLD->area != NULL && (WORLD->area->flags & 0x104) == 4)
        rate = (f32)((f64)rate * 0.5);
    count = ParticlesPerSecond(rate, FRAMETIME);
    if (count > 0)
        AddVariableShotDebrisEffect(effect, position, count, 0, 0);
}

static void __cdecl PartExtra_BlueCoin(PART_PC *part) {
    if ((part->active & 0x200) != 0)
        PartExtraCoinsPrivate(&part->position, 56);
}

void __cdecl PartExtra_PurpleCoin(PART_PC *part) {
    if ((part->active & 0x200) != 0)
        PartExtraCoinsPrivate(&part->position, 57);
}

static void __cdecl PartImpact_Coin(PART_PC *part) {
    f32 speed, volume;
    i32 angle;
    if (!(coinimpactwait <= 0.0f))
        return;
    speed = NuVecMag(&part->velocity);
    if (!(speed > 0.0f))
        return;
    if (speed > 1.5f)
        speed = 1.5f;
    angle = (i32)(((1.5 - (f64)speed) / 1.5) * 16384.0 + 16384.0);
    volume = NuTrigTable[(angle >> 1) & 0x7fff];
    if (GAMEDEMO != 0) {
        PlaySfxAndSetVolume((char *)PickupCoinSfx_PC, &part->position, volume);
        coinimpactwait = 0.1f;
    } else if (part->part_id >= 0xb7 && part->part_id <= 0xba) {
        PlaySfxAndSetVolume((char *)CoinDropSilverSfx_PC, &part->position, volume);
    } else {
        PlaySfxAndSetVolume((char *)CoinDropGoldSfx_PC, &part->position, volume);
    }
}

static void __cdecl PartStolen_Coin(PART_PC *part) {
    GAMEOBJECT *recipient;
    i32 first_active, second_active;
    GIZMOPICKUP_PC pickup;
    if (netclient != 0 || (part->active & 0x200) == 0)
        return;
    recipient = Player[0];
    first_active = recipient != NULL && (recipient->flags_1fc & 0x80) != 0;
    second_active = Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0;
    if (first_active) {
        if (second_active)
            recipient = Player[qrand() / 32768];
    } else if (second_active) {
        recipient = Player[1];
    } else {
        return;
    }
    if (recipient != NULL) {
        SetCoinType(part->part_id, &pickup);
        GizmoPickup_CollectCoin(WORLD, &part->position, pickup.type_index, pickup.model_variant, recipient, 1);
    }
}

static void __cdecl PartStop_Coin(PART_PC *part) {
    if (netclient == 0) {
        PICKUP_RUNTIME_SYS_PC *system = WORLD->pickup_runtime_sys;
        GIZMOPICKUP_PC *pickup = &system->pickups[system->cursor];
        ++system->cursor;
        if (WORLD->pickup_runtime_sys->cursor >= 64)
            WORLD->pickup_runtime_sys->cursor = 0;
        if ((pickup->flags & 1) != 0)
            GizmoPickup_CollectCoin(WORLD, &pickup->position, pickup->type_index, pickup->model_variant, NULL, 1);
        SetCoinType(part->part_id, pickup);
        pickup->flags = (pickup->flags & 0xf7) | 0x17;
        pickup->position = part->position;
        pickup->rotation = 0;
        pickup->room = WORLD->current_gscn != NULL ? (i8)NuPortalWhichRoom(WORLD->current_gscn, &pickup->position) : -1;
        pickup->floor = 2000000.0f;
        pickup->timer = 5.0f;
        pickup->shadow_x = 0;
        pickup->shadow_z = 0;
        pickup->flags = (pickup->flags | 0x20) & ~0x40;
    }
    part->active &= ~1;
}

void __cdecl AddCoinsAsParts(i32 type_id, NUVEC *position, NUVEC *velocity, f32 lifetime, f32 scale) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    GIZMO_PICKUP_TYPE_PC *type = &GizmoPickupType[type_id];
    i32 model = type->first_model_id;
    LEVEL_OBJECT_RUNTIME_PC *object;
    ADDPART_PC params;
    PART_PC *part;
    if (type->random_model_count != 0)
        model += qrand() / (65535 / type->random_model_count + 1);
    object = &world->level_objects[model];
    if (object->active == 0)
        return;
    params = Default_ADDPART;
    params.special = &object->special;
    params.part_id = model;
    params.position = position;
    params.velocity = velocity;
    params.field_14 = params.field_18 = scale * type->shadow_radius_x;
    params.flags = 0x56;
    params.gravity = AreaPickupGravity;
    if (model >= 0xd5 && model <= 0xd8) {
        params.flags |= 0x200;
    } else if (model >= 0xc7 && model <= 0xca) {
        params.flags |= 0x200;
        params.callback_30 = PartExtra_BlueCoin;
    } else if (model >= 0xd5 && model <= 0xd8) {
        params.flags |= 0x200;
        params.callback_30 = PartExtra_PurpleCoin;
    } else {
        params.destroy_fn = PartStolen_Coin;
    }
    params.field_88 = lifetime;
    params.collision_fn = PartCollide_3D;
    params.stop_fn = PartStop_Coin;
    params.impact_fn = PartImpact_Coin;
    params.time_step = FRAMETIME;
    params.field_c0 = scale;
    part = AddPart(&params);
    if (part != NULL) {
        part->time_remaining = 30.0f;
        if (params.gravity == 0.0f) {
            part->move_fn = PartMove_VehiclePickup;
            part->initial_time = (f32)((f64)qrand() * (f64)(f32)(1.0f / 65535.0f) + 4.0);
        }
    }
}

void __cdecl Pup_CollectCoin(WORLDINFO *world, GIZMOPICKUP_PC *pickup, i32 type, GAMEOBJECT *object, i32 flags) {
    GizmoPickup_CollectCoin(world, &pickup->position, type, pickup->model_variant, object, flags);
}

void __cdecl Pup_UpdateBlueCoin(WORLDINFO *world, GIZMOPICKUP_PC *pickup) {
    (void)world;
    if ((pickup->flags & 0x10) != 0 && (pickup->flags & 0x20) != 0)
        PartExtraCoinsPrivate(&pickup->position, 56);
}

void __cdecl Pup_UpdatePurpleCoin(WORLDINFO *world, GIZMOPICKUP_PC *pickup) {
    (void)world;
    if ((pickup->flags & 0x10) != 0 && (pickup->flags & 0x20) != 0)
        PartExtraCoinsPrivate(&pickup->position, 57);
}

void __cdecl Pup_UpdatePowerUp(WORLDINFO *world, GIZMOPICKUP_PC *pickup) {
    PowerUp_Particles(world, &pickup->position);
    PlaySfx((char *)PowerUpLoopSfx_PC, &pickup->position);
}

void __cdecl Pup_CollectPowerUp(WORLDINFO *world, GIZMOPICKUP_PC *pickup, i32 type, GAMEOBJECT *object, i32 flags) {
    (void)world;
    (void)type;
    CollectPowerUp(object, &pickup->position, pickup->rotation, flags);
}

void __cdecl Pup_CollectMinikit(WORLDINFO *world, GIZMOPICKUP_PC *pickup, i32 type, GAMEOBJECT *object, i32 flags) {
    (void)world;
    (void)type;
    CollectMinikit(&pickup->position, pickup->name, flags);
    NewBuzz(object->pad->bound_pad, 0.2f, 0);
}

void __cdecl Pup_CollectHeart(WORLDINFO *world, GIZMOPICKUP_PC *pickup, i32 type, GAMEOBJECT *object, i32 flags) {
    (void)world;
    (void)type;
    CollectHitPoint(object, &pickup->position, flags);
}

void __cdecl Pup_CollectRedBrick(WORLDINFO *world, GIZMOPICKUP_PC *pickup, i32 type, GAMEOBJECT *object, i32 flags) {
    (void)world;
    (void)type;
    (void)flags;
    CollectRedBrick(&pickup->position);
    NewBuzz(object->pad->bound_pad, 0.2f, 0);
}

void __cdecl Pup_CollectCharKit(WORLDINFO *world, GIZMOPICKUP_PC *pickup, i32 type, GAMEOBJECT *object, i32 flags) {
    NUVEC target;
    ADD_GAME_MESSAGE_PC message;
    GAME_MESSAGE_PC *queued;
    (void)world;
    (void)type;
    (void)flags;
    NewBuzz(object->pad->bound_pad, 0.2f, 0);
    AddGameDebris(WORLD->game_debris_sys, 0x13, &pickup->position);
    if (WORLD->level_objects[0xcf].active == 0)
        return;
    target.x = KITPOSX;
    target.y = KITPOSY + PANEL_MINIKITY;
    target.z = 1.0f;
    message = AddGameMsg_Default;
    message.position = &pickup->position;
    message.target_position = &target;
    message.scale = AreaPickupScale;
    message.target_scale = PANEL_MINIKITSCALE;
    message.flags = 0x2112d;
    message.duration = 1.0f;
    message.icon = 0xcf;
    message.special = &WORLD->level_objects[0xcf].special;
    message.tick_fn = MiniKit_GameMsg_Update;
    message.end_fn = MiniKit_GameMsg_End;
    message.allow_temporary = 1;
    queued = AddGameMsg(&message);
    if (queued != NULL)
        queued->target_is_coin_total = 6;
    DrawMiniKitTime = 2.0f;
    if (AreaGlobals_PC.field_1c < 10)
        ++AreaGlobals_PC.field_1c;
}

static void __cdecl EndRedBrickMessage(GAME_MESSAGE_PC *message) {
    (void)message;
    if (AreaGlobals_PC.field_08 != 0)
        AreaGlobals_PC.field_08 = 2;
    RedBrickScale = 2.0f;
    GameAudio_PlaySfx(0x26, NULL, 0, 0);
    NewRumbleAllPlayers(0.6f, 0.0f, 0, 0.0f);
    GameCam_Judder(GameCam, -0.2f, 0, NULL);
}

void __cdecl CollectRedBrick(NUVEC *position) {
    ADD_GAME_MESSAGE_PC message;
    NUVEC target;
    AddGameDebris(WORLD->game_debris_sys, 0x62, position);
    GameAudio_PlaySfxById(GetSfxId(PickupTypeLiteral758c14), position, 3, 0);
    if (WORLD->area_index == -1 || Game.area_save[WORLD->area_index].red_brick_collected == 0) {
        target.x = REDBRICKPOSX;
        target.y = REDBRICKPOSY;
        target.z = 1.0f;
        message = AddGameMsg_Default;
        message.position = position;
        message.target_position = &target;
        message.scale = AreaPickupScale;
        message.target_scale = PANEL_REDBRICKSCALE;
        message.flags = 0x2112d;
        message.duration = 1.0f;
        message.icon = 0xd2;
        message.special = &WORLD->level_objects[0xd2].special;
        message.tick_fn = MiniKit_GameMsg_Update;
        message.end_fn = EndRedBrickMessage;
        message.allow_temporary = 1;
        AddGameMsg(&message);
        DrawRedBrickTime = 2.0f;
        if (WORLD->area_index != -1)
            AreaGlobals_PC.field_08 = 1;
    }
}

void __cdecl CollectHitPoint(GAMEOBJECT *object, NUVEC *position, i32 flags) {
    NUVEC momentum;
    (void)flags;
    if (object->current_hp > 0 && object->current_hp < object->field_107e)
        ++object->current_hp;
    if (object->saved_hitpoints < 4)
        ++object->saved_hitpoints;
    if (object->character_data->model_flags & 0x2000) {
        NuVecScale(&momentum, &object->velocity, 0.5f);
        AddGameDebrisMom(WORLD->game_debris_sys, 0x10, position, 15, &momentum);
    } else
        AddGameDebris(WORLD->game_debris_sys, 15, position);
    GameAudio_PlaySfx(0x27, position, 0, 0);
    NewBuzz(object->pad->bound_pad, 0.1f, 0);
}

static __declspec(noinline) void PowerUp_ExistingIconPrivate(i32 player_index) {
    NUVEC position;
    ADD_GAME_MESSAGE_PC message;
    position.x = player_index == 0 ? -ICONX : ICONX;
    position.y = PowerUp_GetPanelY(player_index) + STATSPOSY;
    position.z = 1.0f;
    message = AddGameMsg_Default;
    message.icon = 0x35;
    message.special = &WORLD->level_objects[0x35].special;
    message.position = &position;
    message.flags = 0x20;
    message.scale = 0.6f;
    message.target_scale = 0.0f;
    message.duration = 0.3f;
    AddGameMsg(&message);
}

static void __cdecl PowerUp_UpdateMsg(GAME_MESSAGE_PC *message) {
    message->parameter_e2 = (u16)(i32)(message->parameter_e2 + 16384.0 * FRAMETIME);
    if (message->player_index == 0 || message->player_index == 1)
        message->target_position.y = PowerUp_GetPanelY(message->player_index) + STATSPOSY;
}

static void __cdecl PowerUp_EndMsg(GAME_MESSAGE_PC *message) {
    GameAudio_PlaySfx(0x26, NULL, 0, 0);
    if (FindGameMsgsWithID(7, 0, message->player_index, message))
        PowerUp_ExistingIconPrivate(message->player_index);
}

void __cdecl CollectPowerUp(GAMEOBJECT *object, NUVEC *position, u16 rotation, i32 flags) {
    NUVEC target;
    ADD_GAME_MESSAGE_PC message;
    GAME_MESSAGE_PC *queued;
    u16 panel_rotation;
    (void)flags;
    if (object == NULL || (u8)(i8)object->field_280 >= 2)
        return;
    if (ONEPLAYERPOWERUPS != 0) {
        panel_rotation = (u16)(rotation - RotDiff(0, GameCam_GetAdjustedYRot(NULL)));
        target.x = (i8)object->field_280 == 0 ? -ICONX : ICONX;
        target.y = PowerUp_GetPanelY((i8)object->field_280) + STATSPOSY;
        target.z = 1.0f;
        if (object->powerup_time > 0.0f)
            PowerUp_ExistingIconPrivate((i8)object->field_280);
        message = AddGameMsg_Default;
        message.position = position;
        message.target_position = &target;
        message.scale = AreaPickupScale;
        message.target_scale = POWERUPOBJSIZE;
        message.flags = 0x2112d;
        message.duration = 1.0f;
        message.icon = 0xd0;
        message.special = &WORLD->level_objects[0xd0].special;
        message.update_fn = PowerUp_UpdateMsg;
        message.end_fn = PowerUp_EndMsg;
        message.player_index = (i8)object->field_280;
        message.allow_temporary = 1;
        message.target_is_coin_total = 7;
        queued = AddGameMsg(&message);
        if (queued != NULL) {
            queued->parameter_e2 = panel_rotation;
            PowerUp_PanelYRot[(i8)object->field_280] = panel_rotation;
        }
    }
    Cheat_StartPowerUp(position, object);
    NewRumble(object->pad->bound_pad, 1.0f, 0);
    NewBuzz(object->pad->bound_pad, 0.2f, 0);
}

void __cdecl AddVariableShotDebrisEffectTimed3(i32 effect, NUVEC *position, NUVEC *momentum, i32 count, f32 time,
                                               NUMTX *emitter_orientation, NUMTX *particle_orientation) {
    u16 priority = 20000;
    if (effect < 0 || debtab[effect] == NULL)
        return;
    switch (debtab[effect]->particle_type) {
        case 2:
            priority = 40000;
            break;
        case 3:
            priority = 30000;
            break;
        case 7:
            priority = 10000;
            break;
    }
    AddVariableShotDebrisEffectTimed5(effect, position, momentum, NULL, count, time, emitter_orientation,
                                      particle_orientation, priority, 0);
}
void __cdecl AddVariableShotDebrisEffectTimed1(i32 effect, NUVEC *position, i32 count, f32 time, i16 z_rotation,
                                               i16 y_rotation, NUMTX *particle_orientation) {
    NUMTX orientation;
    DebrisMtxCopyPrivate(&orientation, &ANI3_EVAL_IDENTITY_MATRIX);
    DebrisMtxRotateZPrivate(&orientation, z_rotation);
    DebrisMtxRotateYPrivate(&orientation, y_rotation);
    AddVariableShotDebrisEffectTimed3(effect, position, &APIObjectZeroMomentumPC, count, time, &orientation,
                                      particle_orientation);
}

void __cdecl SetCoinType(i32 model, GIZMOPICKUP_PC *pickup) {
    u8 count;
    if ((u32)(model - 0xb7) <= 3) {
        pickup->type_index = 0;
        pickup->model_variant = (u8)(model - 0xb7);
    } else if ((u32)(model - 0xbf) <= 3) {
        pickup->type_index = 1;
        pickup->model_variant = (u8)(model - 0xbf);
    } else if ((u32)(model - 0xc7) <= 3) {
        pickup->type_index = 2;
        pickup->model_variant = (u8)(model - 0xc7);
    } else {
        pickup->type_index = 3;
        pickup->model_variant = (u8)(model - 0xd5);
    }
    count = GizmoPickupType[pickup->type_index].random_model_count;
    pickup->model_variant = count ? pickup->model_variant % count : 0;
}

i32 GetRandomCoinType() {
    return CoinTab[qrand() / 16384];
}
void __cdecl PowerUp_Particles(WORLDINFO *world, NUVEC *position) {
    if (VehicleArea == 0 && ParticlesPerSecond(10.0f, FRAMETIME) > 0)
        AddGameDebris(world->game_debris_sys, GizBuilditGDeb[qrand() / 10923], position);
}

typedef struct coinpacket_pickup_pc_s {
    u8 unknown0[0xa];
    u8 multiplier, unknown_b;
    f32 timer;
} COINPACKET_PICKUP_PC;
STATIC_ASSERT(offsetof(COINPACKET_PICKUP_PC, multiplier) == 0xa);
STATIC_ASSERT(offsetof(COINPACKET_PICKUP_PC, timer) == 0xc);
void __cdecl GizmoPickup_CollectCoin(WORLDINFO *world, NUVEC *position, i32 type_index, i32 model_variant,
                                     GAMEOBJECT *object, i32 ignored) {
    i32 player_index, player0_active, main_total, model_id, multiplier = 0;
    NUVEC target;
    f32 target_scale;
    u32 score;
    ADD_GAME_MESSAGE_PC message;
    COINPACKET_PICKUP_PC *packet;
    char text[8];
    (void)ignored;
    if (object != NULL)
        player_index = (i8)object->field_280;
    else {
        player0_active = Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0;
        if (Player[1] == NULL || (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) == 0) {
            if (!player0_active)
                return;
            player_index = 0;
        } else if (!player0_active)
            player_index = 1;
        else
            player_index = qrand() / 32768;
    }
    if ((u32)player_index > 1)
        return;
    if (type_index >= 2 && type_index <= 3 && object != NULL) {
        if (world->area != NULL && (world->area->flags & 0x104) == 4)
            NewBuzzFrames(object->pad->bound_pad, 1, 0);
        else
            NewBuzz(object->pad->bound_pad, 0.1f, 0);
    }
    PlaySfx((char *)(DoubleScoreTime > 0.0f || type_index == 2 || type_index == 3 ? PickupTypeLiteral758c5c
                                                                                  : PickupCoinSfx_PC),
            position);
    main_total = CoinsGoToMainTotal() != 0;
    if (main_total) {
        target.x = cointotal_x[player_index];
        target.y = STATSPOSY;
        DrawCoinTotalTime = (f32)(COINMSGTIME + 1.0);
        target_scale = COINTOTAL_COINSIZE;
    } else {
        target.x = player_index == 1 ? PANEL_COINX : -PANEL_COINX;
        target.y = STATSPOSY + PANEL_COINY;
        DrawBuildUpTime = (f32)(COINMSGTIME + 1.0);
        target_scale = PANEL_COINSCALE_END;
    }
    target.z = 1.0f;
    model_id = GizmoPickupType[type_index].first_model_id + model_variant;
    if (model_id == -1 || world->level_objects[model_id].active == 0)
        return;
    message = AddGameMsg_Default;
    message.icon = (i16)model_id;
    message.special = &world->level_objects[message.icon];
    message.player_index = (i8)player_index;
    score = GizmoPickupType[type_index].score;
    if (object != NULL && object->coin_packet != NULL) {
        if (DoubleScore & (1 << player_index)) {
            if (StartupGameFlag1)
                score += score;
            else {
                packet = (COINPACKET_PICKUP_PC *)object->coin_packet;
                multiplier = packet->multiplier;
                score *= multiplier;
                if (packet->multiplier < 10)
                    ++packet->multiplier;
                ((COINPACKET_PICKUP_PC *)object->coin_packet)->timer = 0.5f;
            }
        }
        if (object->powerup_time > 0.0f)
            score += score;
    }
    message.score = score;
    if (main_total)
        message.update_fn = GameMsg_DrawAdjustNewPos_CoinToTotal;
    message.scale = AreaPickupScale * PANEL_COINSCALE_START;
    message.position = position;
    message.target_position = &target;
    message.target_scale = target_scale;
    message.flags = 0x2012d;
    message.duration = COINMSGTIME;
    message.end_fn = EndScoreMessage;
    AddGameMsg(&message);
    if (multiplier > 0) {
        sprintf(text, PickupMultiplierFormat, multiplier);
        message = AddGameMsg_Default;
        message.scale = message.target_scale = 2.0f;
        message.duration = 0.25f;
        message.text = text;
        message.position = position;
        message.target_position = position;
        message.flags = 0x8007;
        AddGameMsg(&message);
    }
}

void __cdecl InstantKillParts(GAMEOBJECT *object, i32 mode, f32 vertical_scale) {
    i32 excluded_layer;
    excluded_layer = object->id == (u16)id_BODYGUARD && object->current_hp <= 1 ? 4 : -1;
    KillParts(object, -1, excluded_layer, mode, vertical_scale, 0, NULL);
}

STATIC_ASSERT(offsetof(GAMEOBJECT, field_c8c) == 0xc8c);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_c90) == offsetof(GAMEOBJECT, field_c8c) + sizeof(f32));
STATIC_ASSERT(offsetof(GAMEOBJECT, field_c94) == offsetof(GAMEOBJECT, field_c8c) + 2 * sizeof(f32));
i32 __cdecl SetPartTarget(GAMEOBJECT *object, PART_PC *target) {
    object->attack_target_position = target->position;
    *(NUVEC *)&object->field_c8c = target->velocity;
    object->flags_e20 |= 0x800;
    object->collision_target = VehicleArea != 0 ? NULL : (GAMEOBJECT *)target;
    return 1;
}

PART_PC *__cdecl TargetPart(GAMEOBJECT *object, NUVEC *position, NUVEC *direction, f32 radius, f32 range_squared,
                            i32 directional, i32 bolt_id) {
    NUVEC aim = *direction, delta;
    BOLTTYPE_PC *bolt = BoltType_FindByID(bolt_id, WORLD);
    PART_PC *part = Part, *best = NULL, *previous = NULL;
    f32 min_x = position->x - radius, max_x = position->x + radius;
    f32 min_z = position->z - radius, max_z = position->z + radius;
    f32 nearest_distance = range_squared, distance, dot;
    u16 angle;
    i32 i;
    if (directional != 0) {
        aim = *direction;
        if ((bolt->flags_60 & 0x20000) != 0) {
            aim.y = 0.0f;
            NuVecNorm(&aim, &aim);
        }
    }
    for (i = 0; i < MAXPARTS; ++i, ++part) {
        if ((part->active & 1) == 0 || ((part->flags & 0x8000000) == 0 && (part->flags & 0xa) != 0xa))
            continue;
        if (part->position.x < min_x || part->position.x > max_x || part->position.z < min_z ||
            part->position.z > max_z)
            continue;
        distance = NuVecDistSqr(&part->position, position, &delta);
        if (!(range_squared > distance))
            continue;
        if (directional == 0)
            NuVecRotateY(&aim, &v001, NuAtan2D(part->position.x - position->x, part->position.z - position->z));
        if ((bolt->flags_60 & 0x20000) != 0)
            delta.y = 0.0f;
        NuVecNorm(&delta, &delta);
        dot = NuVecDot(&delta, &aim);
        if (TargetDist_Near2 > distance && directional != 0)
            angle = TargetDeg_Near;
        else if (TargetDist_Mid2 > distance)
            angle = TargetDeg_Mid;
        else
            angle = TargetDeg_Far;
        if (!(dot > NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff]) || !(nearest_distance > distance))
            continue;
        if ((PART_PC *)object->collision_target == part)
            previous = part;
        else {
            nearest_distance = distance;
            best = part;
        }
    }
    return best != NULL ? best : previous;
}

typedef struct part_emitter_type_view_pc_s {
    char name[16];
    u8 unknown_010[0x80];
    i16 effect_ids[8];
    i8 effect_pages[8];
    i8 scene_indices[8];
    i8 variant_count;
    u8 variant_mode;
    i8 page;
    u8 unknown_0b3[9];
    f32 lifetime, lifetime_random, speed, gravity, emission_rate, bounce;
    NUVEC position_random, velocity_random;
    f32 emission_period, emission_period_random, emission_pause, emission_pause_random;
    i32 rotation[3], rotation_random[3];
    u32 flags;
    i32 trail_effects[2];
    f32 trail_rates[2];
    i32 attached_effect, impact_effect, impact_part;
    char impact_part_name[16];
    i32 kill_effect;
    i32 sounds[4];
    u8 sound_modes[4];
    u8 unknown_15c[4];
    NUVEC lighting_scale;
    f32 last_used_time;
    u8 unknown_170[8];
} PART_EMITTER_TYPE_VIEW_PC;
STATIC_ASSERT(sizeof(PART_EMITTER_TYPE_VIEW_PC) == 0x178);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, name) == 0);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, effect_ids) == 0x90);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, effect_pages) == 0xa0);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, scene_indices) == 0xa8);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, variant_count) == 0xb0);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, variant_mode) == 0xb1);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, lifetime) == 0xbc);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, lifetime_random) == 0xc0);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, speed) == 0xc4);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, gravity) == 0xc8);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, emission_rate) == 0xcc);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, bounce) == 0xd0);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, position_random) == 0xd4);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, velocity_random) == 0xe0);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, rotation) == 0xfc);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, rotation_random) == 0x108);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, flags) == 0x114);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, trail_effects) == 0x118);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, trail_rates) == 0x120);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, attached_effect) == 0x128);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, impact_effect) == 0x12c);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, impact_part) == 0x130);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, impact_part_name) == 0x134);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, kill_effect) == 0x144);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, sounds) == 0x148);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, sound_modes) == 0x158);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, lighting_scale) == 0x160);
STATIC_ASSERT(offsetof(PART_EMITTER_TYPE_VIEW_PC, last_used_time) == 0x16c);

void __cdecl AddVariableShotPARTEffect(i32 effect_index, NUVEC *position, f32 rate, f32 duration, NUMTX *orientation) {
    PART_EMITTER_TYPE_VIEW_PC *type = &((PART_EMITTER_TYPE_VIEW_PC *)part_types)[effect_index];
    f32 interval, quotient, aligned_time, emission_time, end_time;
    i32 remaining = 99;
    if (type->name[0] == 0 || type->emission_rate <= 0.0f)
        return;
    if (rate <= 0.0f)
        rate = type->emission_rate;
    if (orientation == NULL)
        orientation = &NuMtxSetIdentityTemplate;
    interval = rate == 0.0f ? 0.0f : 1.0f / rate;
    end_time = partglobaltime + duration;
    quotient = interval == 0.0f || partglobaltime == 0.0f ? 0.0f : partglobaltime / interval;
    aligned_time = (i32)quotient * interval;
    emission_time = aligned_time + interval;
    if (!(emission_time <= end_time))
        return;
    do {
        ADDPART_PC params = Default_ADDPART;
        NUVEC part_position, velocity;
        NUSPECIALHANDLE special;
        NUMTX matrix;
        f32 random, range;
        i16 angle_x, angle_y, angle_z;
        i32 variant = 0, axis, sound;
        random = NuRandFloatSeeded(&partseed);
        part_position.x = (random + random) * type->position_random.x - type->position_random.x;
        random = NuRandFloatSeeded(&partseed);
        part_position.y = (random + random) * type->position_random.y - type->position_random.y;
        random = NuRandFloatSeeded(&partseed);
        part_position.z = (random + random) * type->position_random.z - type->position_random.z;
        NuVecMtxTransformVU0(&part_position, &part_position, orientation);
        NuVecAdd(&part_position, &part_position, position);
        velocity.x = velocity.z = 0.0f;
        velocity.y = type->speed;
        random = NuRandFloatSeeded(&partseed);
        velocity.x += (random + random) * type->velocity_random.x - type->velocity_random.x;
        random = NuRandFloatSeeded(&partseed);
        velocity.y += (random + random) * type->velocity_random.y - type->velocity_random.y;
        random = NuRandFloatSeeded(&partseed);
        velocity.z += (random + random) * type->velocity_random.z - type->velocity_random.z;
        NuVecMtxTransformVU0(&velocity, &velocity, orientation);
        params.position = &part_position;
        params.velocity = &velocity;
        params.gravity = type->gravity;
        params.field_20 = type->bounce;
        if (type->variant_count != 0 && (type->variant_mode == 0 || type->variant_mode == 1))
            variant = (i32)((f64)NuRandFloatSeeded(&partseed) * 65535.0) % type->variant_count;
        params.part_id = type->effect_ids[variant];
        if (params.part_id == 9999)
            params.special = NULL;
        else if (params.part_id != -1) {
            params.special = &special;
            NuGScnGetSpecial(&special, part_scene[type->scene_indices[variant]], params.part_id);
        }
        params.field_a4 = type->lifetime + NuRandFloatSeeded(&partseed) * type->lifetime_random;
        NuMtxSetIdentity(&matrix);
        range = (f32)type->rotation_random[0];
        random = NuRandFloatSeeded(&partseed);
        angle_x = (i16)(i32)((f64)(random + random) * range + (f64)type->rotation[0] - range);
        range = (f32)type->rotation_random[1];
        random = NuRandFloatSeeded(&partseed);
        angle_y = (i16)(i32)((f64)(random + random) * range + (f64)type->rotation[1] - range);
        range = (f32)type->rotation_random[2];
        random = NuRandFloatSeeded(&partseed);
        angle_z = (i16)(i32)((f64)(random + random) * range + (f64)type->rotation[2] - range);
        for (axis = 0; axis < 3; ++axis) {
            params.field_a8[axis] = type->rotation[axis];
            params.field_a8[axis + 3] = type->rotation_random[axis];
        }
        NuMtxRotateX(&matrix, angle_x);
        NuMtxRotateY(&matrix, angle_y);
        NuMtxRotateZ(&matrix, angle_z);
        matrix.m30 = part_position.x;
        matrix.m31 = part_position.y;
        matrix.m32 = part_position.z;
        params.matrix = &matrix;
        params.flags = type->flags & ~0x60000;
        if (type->trail_effects[0] != -1) {
            params.effect_id_60 = type->trail_effects[0];
            params.flags |= 0x40000;
        }
        if (type->trail_effects[1] != -1) {
            params.effect_id_64 = type->trail_effects[1];
            params.flags |= 0x40000;
        }
        if (type->attached_effect != -1) {
            params.effect_id_58 = type->attached_effect;
            params.flags |= 0x20000;
        }
        params.field_68 = type->trail_rates[0];
        params.field_6c = type->trail_rates[1];
        params.effect_id_70 = type->impact_effect;
        if (type->impact_part == -1 && type->impact_part_name[0] != 0)
            type->impact_part = PARTLookupType(type->impact_part_name);
        params.effect_id_74 = type->impact_part;
        params.effect_id_78 = type->kill_effect;
        params.lighting_scale_x = type->lighting_scale.x;
        params.lighting_scale_y = type->lighting_scale.y;
        params.lighting_scale_z = type->lighting_scale.z;
        AddPart(&params);
        for (sound = 0; sound < 4; ++sound) {
            if (type->sounds[sound] != -1 && type->sound_modes[sound] == 3)
                PlaySfxById(type->sounds[sound], &part_position);
        }
        type->last_used_time = emission_time;
        emission_time += interval;
    } while (emission_time <= end_time && --remaining != 0);
}

STATIC_ASSERT(offsetof(PART_PC, field_206) == 0x206);
PART_PC *__cdecl FindPart(NUVEC *position, i32 player, GAMEOBJECT *owner) {
    PART_PC *part = Part, *result = NULL;
    f32 nearest = 1000000.0f;
    i32 i;
    if (part == NULL)
        return NULL;
    for (i = 0; i < MAXPARTS; ++i, ++part) {
        f32 distance;
        if ((part->active & 1) == 0 || (player != -1 && part->field_206 != player) ||
            (owner != NULL && part->owner != owner))
            continue;
        if (position == NULL)
            return part;
        distance = NuVecDistSqr(position, &part->position, NULL);
        if (distance < nearest) {
            nearest = distance;
            result = part;
        }
    }
    return result;
}

void __cdecl DebrisSetFacing(i32 handle, u8 enabled, i16 x_angle, i16 y_angle) {
    if (handle != -1) {
        debkeydata[handle].field_2fa = enabled;
        DebrisMtxCopyPrivate(&debkeydata[handle].particle_orientation, &ANI3_EVAL_IDENTITY_MATRIX);
        DebrisMtxRotateXPrivate(&debkeydata[handle].particle_orientation, x_angle);
        DebrisMtxRotateYPrivate(&debkeydata[handle].particle_orientation, y_angle);
    }
}
void __cdecl DebrisSetRoomID(i32 handle, NUGSCN *scene) {
    if (scene != NULL) {
        debkeydata[handle].field_2f2 = NuPortalWhichRoom(scene, &debkeydata[handle].position);
        debkeydata[handle].gscene = debkeydata[handle].field_2f2 != -1 ? scene : NULL;
    } else {
        debkeydata[handle].field_2f2 = -1;
        debkeydata[handle].gscene = NULL;
    }
}

void __cdecl DebrisOrientationMtx(i32 handle, NUMTX *source) {
    if (handle != -1) {
        debkeydata[handle].effect_orientation = *source;
        debkeydata[handle].effect_orientation.m30 = 0.0f;
        debkeydata[handle].effect_orientation.m31 = 0.0f;
        debkeydata[handle].effect_orientation.m32 = 0.0f;
        debkeydata[handle].orientation_dirty = 0.0f;
    }
}
void __cdecl DebrisPosOrientationMtx(i32 handle, NUMTX *source) {
    if (handle != -1) {
        debkeydata[handle].effect_orientation = *source;
        debkeydata[handle].position.x = source->m30;
        debkeydata[handle].position.y = source->m31;
        debkeydata[handle].position.z = source->m32;
        debkeydata[handle].effect_orientation.m30 = 0.0f;
        debkeydata[handle].effect_orientation.m31 = 0.0f;
        debkeydata[handle].effect_orientation.m32 = 0.0f;
        debkeydata[handle].orientation_dirty = 0.0f;
    }
}

void __cdecl AddScaledFiniteShotDebrisEffect(i32 *key, i32 effect, NUVEC *position, NUVEC *orientation, NUVEC *momentum,
                                             i32 count, f32 scale) {
    i32 scaled = CreateScaledEffect(effect, scale);
    if (scaled != -1)
        AddFiniteShotDebrisEffect2(key, scaled, position, orientation, momentum, count);
}
i32 __cdecl AddScaledVariableShotDebrisEffect2(i32 effect, NUVEC *position, i32 count, f32 time, NUMTX *emitter,
                                               NUMTX *particle, f32 scale) {
    i32 scaled = CreateScaledEffect(effect, scale);
    if (scaled != -1)
        AddVariableShotDebrisEffectTimed3(scaled, position, &APIObjectZeroMomentumPC, count, time, emitter, particle);
    return scaled;
}

i32 AddScaledVariableShotDebrisEffect3(i32 effect, NUVEC *position, NUVEC *momentum, i32 count, f32 time,
                                       NUMTX *emitter_orientation, NUMTX *particle_orientation, f32 scale) {
    i32 scaled = CreateScaledEffect(effect, scale);
    if (scaled != -1)
        AddVariableShotDebrisEffectTimed3(scaled, position, momentum, count, time, emitter_orientation,
                                          particle_orientation);
    return scaled;
}

extern const f32 DebrisGlassMaterialOpacity;

typedef struct debris_collision_effect_pc_s {
    u8 unknown_000[0x2b0];
    DEBRIS_FLOAT_KEY_PC collision_keys[8];
    i8 process_spheres, time_group;
    u8 collision_flags, use_explicit_clip_box;
    f32 thinning;
    NUVEC particle_box;
    f32 torus_radius1, torus_radius2, torus_lifetime;
    DEBRIS_FLOAT_KEY_PC torus_keys1[8], torus_keys2[8], torus_keys3[8];
    u8 unknown_3d0[0x58];
} DEBRIS_COLLISION_EFFECT_PC;
STATIC_ASSERT(sizeof(DEBRIS_COLLISION_EFFECT_PC) == 0x428);
STATIC_ASSERT(offsetof(DEBRIS_COLLISION_EFFECT_PC, collision_keys) == 0x2b0);
STATIC_ASSERT(offsetof(DEBRIS_COLLISION_EFFECT_PC, torus_radius1) == 0x304);
STATIC_ASSERT(offsetof(DEBRIS_COLLISION_EFFECT_PC, torus_radius2) == 0x308);
STATIC_ASSERT(offsetof(DEBRIS_COLLISION_EFFECT_PC, torus_lifetime) == 0x30c);
STATIC_ASSERT(offsetof(DEBRIS_COLLISION_EFFECT_PC, torus_keys1) == 0x310);
STATIC_ASSERT(offsetof(DEBRIS_COLLISION_EFFECT_PC, torus_keys2) == 0x350);
STATIC_ASSERT(offsetof(DEBRIS_COLLISION_EFFECT_PC, torus_keys3) == 0x390);
extern DEBRIS_COLLISION_ENTRY_PC *spherecolldata, *toruscolldata;
extern i32 numspherecolldata, numtoruscolldata;
extern NUVEC debris_collide_pt;
typedef struct debris_material_flags_pc_s {
    u32 alpha_mode : 4, filter_mode : 2, unknown_0_64_128 : 2, unknown_1_1_2 : 2, unknown_1_4_8 : 2, cull_mode : 2,
        z_mode : 2, unknown_2_1_2 : 2, unknown_2_4 : 1, unknown_19 : 1, alpha_test : 3, unknown_23_31 : 9;
} DEBRIS_MATERIAL_FLAGS_PC;
typedef struct debris_material_secondary_pc_s {
    u32 alpha_fail : 2, disable_depth_write : 1, unknown_03_31 : 29;
} DEBRIS_MATERIAL_SECONDARY_PC;
typedef struct debris_material_pc_s {
    u8 unknown_000[0x40];
    DEBRIS_MATERIAL_FLAGS_PC attribs;
    DEBRIS_MATERIAL_SECONDARY_PC secondary;
    u8 unknown_048[0xc];
    NUVEC diffuse;
    f32 glass_parameter;
    u8 unknown_064[0xc];
    f32 opacity;
    i16 texture_id, sort_priority;
    u8 unknown_078[0x21];
    i8 particle_type_tag;
    u8 unknown_09a[0x156];
    u32 vertex_flags;
} DEBRIS_MATERIAL_PC;
STATIC_ASSERT(sizeof(DEBRIS_MATERIAL_FLAGS_PC) == 4);
STATIC_ASSERT(sizeof(DEBRIS_MATERIAL_SECONDARY_PC) == 4);
STATIC_ASSERT(offsetof(DEBRIS_MATERIAL_PC, attribs) == 0x40);
STATIC_ASSERT(offsetof(DEBRIS_MATERIAL_PC, secondary) == 0x44);
STATIC_ASSERT(offsetof(DEBRIS_MATERIAL_PC, diffuse) == 0x54);
STATIC_ASSERT(offsetof(DEBRIS_MATERIAL_PC, glass_parameter) == 0x60);
STATIC_ASSERT(offsetof(DEBRIS_MATERIAL_PC, opacity) == 0x70);
STATIC_ASSERT(offsetof(DEBRIS_MATERIAL_PC, texture_id) == 0x74);
STATIC_ASSERT(offsetof(DEBRIS_MATERIAL_PC, sort_priority) == 0x76);
STATIC_ASSERT(offsetof(DEBRIS_MATERIAL_PC, particle_type_tag) == 0x99);
STATIC_ASSERT(offsetof(DEBRIS_MATERIAL_PC, vertex_flags) == 0x1f0);
i32 __cdecl NuTexReadPc(char *, VARIPTR *, VARIPTR);
NUMTL *__cdecl CreateAlphaBlendTexture(VARIPTR *, VARIPTR, char *, i32, i32, i32, i32);
NUMTL *__cdecl CreateCopyMat(NUMTL *, i32, i32, i32, i32);
extern i32 DebrisArenaInitializedPc;
extern DEBRIS_INFO_PC DebrisNullEffectPc;
extern NUMTX xzfacingmtx;
extern const NUMTX ANI3_EVAL_IDENTITY_MATRIX;
void __cdecl NuApiSetEndFrameCallbackPc(void(__cdecl *)(void));
typedef struct debris_animation_effect_pc_s {
    u8 unknown_000[0xa8];
    i16 texture_frames;
    i8 texture_rows, frame_divisor;
    union {
        f32 rate;
        u32 packed_rate;
    } animation_rate;
} DEBRIS_ANIMATION_EFFECT_PC;
STATIC_ASSERT(offsetof(DEBRIS_ANIMATION_EFFECT_PC, texture_frames) == 0xa8);
STATIC_ASSERT(offsetof(DEBRIS_ANIMATION_EFFECT_PC, texture_rows) == 0xaa);
STATIC_ASSERT(offsetof(DEBRIS_ANIMATION_EFFECT_PC, frame_divisor) == 0xab);
STATIC_ASSERT(offsetof(DEBRIS_ANIMATION_EFFECT_PC, animation_rate) == 0xac);
NUMTL *__cdecl CreateCopyMat(NUMTL *source, i32 enable_uv_mode, i32 alpha_mode, i32 depth_mode, i32 filter_mode) {
    DEBRIS_MATERIAL_PC *material, *original = (DEBRIS_MATERIAL_PC *)source;
    if (source == NULL)
        return NULL;
    material = (DEBRIS_MATERIAL_PC *)NuMtlCreate3D(1);
    material->diffuse.x = 1.0f;
    material->diffuse.y = 1.0f;
    material->diffuse.z = 1.0f;
    material->opacity = DebrisGlassMaterialOpacity;
    material->attribs.cull_mode = 2;
    material->attribs.z_mode = (u32)depth_mode & 3;
    material->attribs.filter_mode = (u32)filter_mode & 3;
    material->attribs.alpha_mode = (u32)alpha_mode & 15;
    if (enable_uv_mode != 0)
        material->secondary.disable_depth_write = 1;
    material->attribs.unknown_1_1_2 = 1;
    material->attribs.unknown_1_4_8 = 1;
    material->texture_id = original->texture_id;
    material->sort_priority = original->sort_priority;
    material->secondary.alpha_fail = 1;
    NuMtlUpdate((NUMTL *)material);
    return (NUMTL *)material;
}
void __cdecl DebrisSetup2(VARIPTR *buffer, VARIPTR buffer_end, VARIPTR *trash_buffer, VARIPTR trash_end,
                          char *texture_name, i32 chunk_count, i32 glass_chunk_count, i32 key_count, i32 effect_count) {
    i32 i;
    i16 texture_id;
    f32 opacity;
    if (debris_trash_space == 0) {
        debrischunks = chunk_count;
        debrischunksglass = glass_chunk_count;
    }
    debris_setup_called = 1;
    maxdebkeys = key_count;
    debinf_count = effect_count;
    DebrisArenaInitializedPc = 1;
    NuApiSetEndFrameCallbackPc(DebrisProcessTimeSlip);
    buffer->addr = (buffer->addr + 15) & ~15u;
    debris_chunk_controls = (DEBRIS_CHUNK_CONTROL_PC *)buffer->addr;
    buffer->addr += (debrischunks + debrischunksglass) * 2 * sizeof(DEBRIS_CHUNK_CONTROL_PC);
    freechunkcontrols = (DEBRIS_CHUNK_CONTROL_PC **)buffer->addr;
    buffer->addr += (debrischunks + debrischunksglass) * 2 * sizeof(DEBRIS_CHUNK_CONTROL_PC *);
    freedebchunks = (DEBRIS_PARTICLE_CHUNK_PC **)buffer->addr;
    buffer->addr += debrischunks * sizeof(DEBRIS_PARTICLE_CHUNK_PC *);
    freedebchunksglass = (DEBRIS_PARTICLE_CHUNK_PC **)buffer->addr;
    buffer->addr += debrischunksglass * sizeof(DEBRIS_PARTICLE_CHUNK_PC *);
    ParticleChunkToRender = (DEBRIS_RENDER_CHUNK_PC *)buffer->addr;
    buffer->addr += (debrischunks + debrischunksglass) * sizeof(DEBRIS_RENDER_CHUNK_PC);
    buffer->addr = (buffer->addr + 15) & ~15u;
    debkeydata = (DEBRISKEY_PC *)buffer->addr;
    buffer->addr += maxdebkeys * sizeof(DEBRISKEY_PC);
    freedebkeys = (i16 *)buffer->addr;
    buffer->addr += maxdebkeys * sizeof(i16);
    effecttypes = (DEBRIS_INFO_PC *)buffer->addr;
    buffer->addr += debinf_count * sizeof(DEBRIS_INFO_PC);
    debtab = (DEBRIS_INFO_PC **)buffer->addr;
    buffer->addr += debinf_count * sizeof(DEBRIS_INFO_PC *);
    DmaDebTypes = (DEBRIS_PART_HEADER_PC **)buffer->addr;
    buffer->addr += EDPP_MAX_DMADEBTYPES * sizeof(DEBRIS_PART_HEADER_PC *);
    spherecolldata = (DEBRIS_COLLISION_ENTRY_PC *)buffer->addr;
    buffer->addr += maxdebkeys * sizeof(DEBRIS_COLLISION_ENTRY_PC);
    toruscolldata = (DEBRIS_COLLISION_ENTRY_PC *)buffer->addr;
    buffer->addr += maxdebkeys * sizeof(DEBRIS_COLLISION_ENTRY_PC);
    debscale = (DEBRIS_SCALE_CACHE_PC *)buffer->addr;
    buffer->addr += EDPP_SCALE_TYPES * sizeof(DEBRIS_SCALE_CACHE_PC);
    memset(freedebkeys, 0, maxdebkeys * sizeof(i16));
    memset(debkeydata, 0, maxdebkeys * sizeof(DEBRISKEY_PC));
    memset(effecttypes, 0, debinf_count * sizeof(DEBRIS_INFO_PC));
    memset(debtab, 0, debinf_count * sizeof(DEBRIS_INFO_PC *));
    memset(debscale, 0, EDPP_SCALE_TYPES * sizeof(u32));
    for (i = 0; i < maxdebkeys; ++i) {
        spherecolldata[i].key_index = i;
        spherecolldata[i].flags = 3;
        toruscolldata[i].key_index = i;
        toruscolldata[i].flags = 3;
    }
    effecttypes[0] = DebrisNullEffectPc;
    debtab[0] = effecttypes;
    edpp_types_used = 1;
    numspherecolldata = maxdebkeys;
    numtoruscolldata = maxdebkeys;
    DebMat[0] = CreateAlphaBlendTexture(buffer, buffer_end, texture_name, 1, 2, 256, 0);
    texture_id = DebMat[0]->texture_id;
    ((DEBRIS_MATERIAL_PC *)DebMat[0])->attribs.z_mode = 1;
    ((DEBRIS_MATERIAL_PC *)DebMat[0])->attribs.alpha_test = 1;
    NuMtlUpdate(DebMat[0]);
    DebMat[1] = CreateCopyMat(DebMat[0], 0, 0, 3, 0);
    DebMat[2] = CreateCopyMat(DebMat[0], 1, 1, 1, 1);
    DebMat[3] = CreateCopyMat(DebMat[0], 1, 3, 1, 1);
    DebMat[4] = CreateCopyMat(DebMat[0], 0, 1, 1, 1);
    DebMat[5] = CreateCopyMat(DebMat[0], 0, 2, 1, 1);
    DebMat[6] = CreateCopyMat(DebMat[0], 0, 0, 0, 1);
    ((DEBRIS_MATERIAL_PC *)DebMat[0])->vertex_flags |= 0x100000;
    ((DEBRIS_MATERIAL_PC *)DebMat[1])->vertex_flags |= 0x100000;
    ((DEBRIS_MATERIAL_PC *)DebMat[2])->vertex_flags |= 0x100000;
    ((DEBRIS_MATERIAL_PC *)DebMat[3])->vertex_flags |= 0x100000;
    ((DEBRIS_MATERIAL_PC *)DebMat[4])->vertex_flags |= 0x100000;
    ((DEBRIS_MATERIAL_PC *)DebMat[5])->vertex_flags |= 0x100000;
    ((DEBRIS_MATERIAL_PC *)DebMat[6])->vertex_flags |= 0x100000;
    DebMat[6]->texture_id = texture_id;
    DebMat[5]->texture_id = texture_id;
    DebMat[4]->texture_id = texture_id;
    DebMat[3]->texture_id = texture_id;
    DebMat[2]->texture_id = texture_id;
    DebMat[1]->texture_id = texture_id;
    DebMat[0]->texture_id = texture_id;
    DebMat[7] = NuMtlCreateEx3D(1, 2);
    ((DEBRIS_MATERIAL_PC *)DebMat[7])->diffuse.x = 1.0f;
    ((DEBRIS_MATERIAL_PC *)DebMat[7])->diffuse.y = 1.0f;
    ((DEBRIS_MATERIAL_PC *)DebMat[7])->diffuse.z = 1.0f;
    opacity = DebrisGlassMaterialOpacity;
    ((DEBRIS_MATERIAL_PC *)DebMat[7])->attribs.cull_mode = 2;
    ((DEBRIS_MATERIAL_PC *)DebMat[7])->attribs.z_mode = 1;
    ((DEBRIS_MATERIAL_PC *)DebMat[7])->attribs.filter_mode = 0;
    ((DEBRIS_MATERIAL_PC *)DebMat[7])->attribs.unknown_2_1_2 = 2;
    ((DEBRIS_MATERIAL_PC *)DebMat[7])->attribs.unknown_2_4 = 1;
    ((DEBRIS_MATERIAL_PC *)DebMat[7])->attribs.unknown_1_1_2 = 1;
    ((DEBRIS_MATERIAL_PC *)DebMat[7])->attribs.unknown_1_4_8 = 1;
    ((DEBRIS_MATERIAL_PC *)DebMat[7])->opacity = opacity;
    ((DEBRIS_MATERIAL_PC *)DebMat[7])->attribs.alpha_mode = 1;
    ((DEBRIS_MATERIAL_PC *)DebMat[7])->secondary.alpha_fail = 1;
    ((DEBRIS_MATERIAL_PC *)DebMat[7])->texture_id = 1;
    ((DEBRIS_MATERIAL_PC *)DebMat[7])->sort_priority = 128;
    ((DEBRIS_MATERIAL_PC *)DebMat[7])->vertex_flags |= 0x100000;
    ((DEBRIS_MATERIAL_PC *)DebMat[7])->glass_parameter = 0.25f;
    ((DEBRIS_MATERIAL_PC *)DebMat[7])->particle_type_tag = -105;
    NuMtlUpdate(DebMat[7]);
    DebMat[8] = CreateCopyMat(DebMat[0], 0, 6, 0, 1);
    for (i = 0; debtab[i] != NULL; ++i) {
        if (((DEBRIS_ANIMATION_EFFECT_PC *)debtab[i])->frame_divisor == 0)
            ((DEBRIS_ANIMATION_EFFECT_PC *)debtab[i])->frame_divisor = 1;
        ((DEBRIS_ANIMATION_EFFECT_PC *)debtab[i])->animation_rate.rate =
            (f32)((((DEBRIS_ANIMATION_EFFECT_PC *)debtab[i])->texture_frames * 60) /
                  ((DEBRIS_ANIMATION_EFFECT_PC *)debtab[i])->frame_divisor);
        ((DEBRIS_ANIMATION_EFFECT_PC *)debtab[i])->animation_rate.packed_rate =
            (((DEBRIS_ANIMATION_EFFECT_PC *)debtab[i])->animation_rate.packed_rate & 0xfffff800) +
            (((u16)((DEBRIS_ANIMATION_EFFECT_PC *)debtab[i])->texture_frames *
              (((DEBRIS_ANIMATION_EFFECT_PC *)debtab[i])->texture_rows - 1)) &
             0x3ff);
        debtab[i]->alpha_keys[0].value /= debtab[i]->particle_lifetime;
        debtab[i]->texture_u0 += 524288.0;
        debtab[i]->texture_u1 += 524288.0;
        debtab[i]->texture_v0 += 524288.0;
        debtab[i]->texture_v1 += 524288.0;
    }
    for (i = 0; i < debinf_count; ++i)
        effecttypes[i].native_data = NULL;
    freeDmaDebType = 0;
    if (debris_suspended == 0) {
        if (trash_buffer != NULL)
            DebrisTrashableSetup(trash_buffer, &trash_end);
        else
            DebrisTrashableSetup(buffer, &buffer_end);
    }
    for (i = 0; i < maxdebkeys; ++i)
        freedebkeys[i] = (i16)i;
    for (i = 0; i < maxdebkeys; ++i) {
        debkeydata[i].finite = 0;
        debkeydata[i].particle_cursor = 0;
        debkeydata[i].emitter_rotation_x = 0;
        debkeydata[i].emitter_rotation_y = 0;
        debkeydata[i].finite_count = 0;
        debkeydata[i].effect_index = 0;
        debkeydata[i].allocation_index = -1;
    }
    xzfacingmtx.m00 = ANI3_EVAL_IDENTITY_MATRIX.m00;
    xzfacingmtx.m01 = ANI3_EVAL_IDENTITY_MATRIX.m01;
    xzfacingmtx.m02 = ANI3_EVAL_IDENTITY_MATRIX.m02;
    xzfacingmtx.m03 = ANI3_EVAL_IDENTITY_MATRIX.m03;
    xzfacingmtx.m10 = ANI3_EVAL_IDENTITY_MATRIX.m10;
    xzfacingmtx.m11 = ANI3_EVAL_IDENTITY_MATRIX.m11;
    xzfacingmtx.m12 = ANI3_EVAL_IDENTITY_MATRIX.m12;
    xzfacingmtx.m13 = ANI3_EVAL_IDENTITY_MATRIX.m13;
    xzfacingmtx.m20 = ANI3_EVAL_IDENTITY_MATRIX.m20;
    xzfacingmtx.m21 = ANI3_EVAL_IDENTITY_MATRIX.m21;
    xzfacingmtx.m22 = ANI3_EVAL_IDENTITY_MATRIX.m22;
    xzfacingmtx.m23 = ANI3_EVAL_IDENTITY_MATRIX.m23;
    xzfacingmtx.m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
    xzfacingmtx.m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
    xzfacingmtx.m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
    xzfacingmtx.m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
    DebrisMtxRotateXPrivate(&xzfacingmtx, -0x4000);
}

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

void __cdecl edpartScaleType(i32, f32);

void __cdecl PartTimeSlip(void) {
    i32 i;
    PART_RUNTIME_TYPE_PC *types = (PART_RUNTIME_TYPE_PC *)part_types;
    for (i = 0; i < 40; ++i) {
        if (part_emits[i].effect_id != -1) {
            part_emits[i].time_20 -= 800.0f;
            part_emits[i].time_24 -= 800.0f;
            part_emits[i].time_28 -= 800.0f;
        }
    }
    for (i = 0; i < 128; ++i) {
        if (types[i].effect_ids[0] != -1)
            types[i].last_used_time -= 800.0f;
    }
    partglobaltime -= 800.0f;
}

static i32 PartCleanupCursorPrivate = 0;
static i32 PartCleanupFramesPrivate = 0;
void __cdecl PartCleanupTypes(void) {
    i32 i;
    if (++PartCleanupFramesPrivate > 5) {
        PartCleanupFramesPrivate = 0;
        if (part_types[PartCleanupCursorPrivate].effect_ids[0] != -1 &&
            partglobaltime > (f64)part_types[PartCleanupCursorPrivate].last_used_time + 5.0 &&
            part_types[PartCleanupCursorPrivate].scale != 1.0f) {
            for (i = 0; i < 40; ++i) {
                if (part_emits[i].effect_id == PartCleanupCursorPrivate) {
                    if (part_emits[i].effect_id != -1) {
                        part_emits[i].effect_id = -1;
                        --part_emits_used;
                    }
                }
            }
            part_types[PartCleanupCursorPrivate].name[0] = 0;
            part_types[PartCleanupCursorPrivate].effect_ids[0] = -1;
            --part_types_used;
        }
        if (++PartCleanupCursorPrivate >= 128)
            PartCleanupCursorPrivate = 0;
    }
}
i32 __cdecl CreateScaledPARTEffect(i32 effect_index, f32 requested_scale) {
    i32 closest_index, i;
    i32 result = 0;
    f32 ratio, closest_distance;
    // Retail uses this inclusive upper check; loaded type IDs are 0..127.
    if (effect_index <= 0 || effect_index > 128 ||
        ((PART_RUNTIME_TYPE_PC *)part_types)[effect_index].effect_ids[0] == -1) {
        return -1;
    }
    if (((PART_RUNTIME_TYPE_PC *)part_types)[effect_index].scale != 1.0f) {
        // The final word identifies the original, unscaled type.
        effect_index = ((PART_RUNTIME_TYPE_PC *)part_types)[effect_index].field_174;
        if (((PART_RUNTIME_TYPE_PC *)part_types)[effect_index].effect_ids[0] == -1) {
            return -1;
        }
    }
    if (requested_scale == 1.0f) {
        result = effect_index;
    }
    if (requested_scale < 0.01f) {
        requested_scale = 0.01f;
    }

    if (result != 0)
        return result;
    closest_index = effect_index;
    ratio = requested_scale / ((PART_RUNTIME_TYPE_PC *)part_types)[effect_index].scale;
    closest_distance = ratio <= 1.0f ? 1.0f - ratio : ratio - 1.0f;
    for (i = 0; i < 128; ++i) {
        f32 distance;
        if (((PART_RUNTIME_TYPE_PC *)part_types)[i].effect_ids[0] != -1 &&
            ((PART_RUNTIME_TYPE_PC *)part_types)[i].field_174 == (u32)effect_index) {
            ratio = requested_scale / ((PART_RUNTIME_TYPE_PC *)part_types)[i].scale;
            distance = ratio <= 1.0f ? 1.0f - ratio : ratio - 1.0f;
            if (distance < closest_distance) {
                closest_index = i;
                closest_distance = distance;
            }
        }
    }
    if (closest_index != 0 && closest_distance < 1.1f) {
        return closest_index;
    }

    for (i = 0; i < 128; ++i) {
        PART_RUNTIME_TYPE_PC *scaled;
        PART_RUNTIME_TYPE_PC *source;
        if (((PART_RUNTIME_TYPE_PC *)part_types)[i].effect_ids[0] != -1) {
            continue;
        }
        scaled = &((PART_RUNTIME_TYPE_PC *)part_types)[i];
        source = &((PART_RUNTIME_TYPE_PC *)part_types)[effect_index];
        *scaled = *source;
        edpartScaleType(i, requested_scale);
        scaled->last_used_time = partglobaltime;
        scaled->scale = requested_scale;
        scaled->field_174 = effect_index;
        if (strlen(source->name) > 12) {
            char shortened[16];
            sprintf(shortened, "%s", source->name);
            shortened[12] = '\0';
            sprintf(scaled->name, "%s%03d", shortened, i);
        } else {
            sprintf(scaled->name, "%s%03d", source->name, i);
        }
        ++part_types_used;
        return i != 0 ? i : closest_index;
    }
    return closest_index;
}

extern char PartNullInstanceNamePc[14];
extern i32 PartLoadParticlePagePc;
u32 __cdecl EdFileReadUnsignedInt(void);
i32 __cdecl GetSfxIdN(char *, i32);
i32 __cdecl edpartLookupObjectInScene(char *name, NUGSCN *scene) {
    NUSPECIALHANDLE special;
    i32 count, i;
    if (name[0] == 0)
        return -1;
    if (NuStrNCmp(name, PartNullInstanceNamePc, 16) == 0)
        return 9999;
    if (scene != NULL) {
        count = NuGScnNumSpecials(scene);
        for (i = 0; i < count; ++i) {
            NuGScnGetSpecial(&special, scene, i);
            if (NuSpecialExistsFn(&special) && NuStrNCmp(name, NuSpecialGetName(&special), 16) == 0)
                return i;
        }
    }
    return -1;
}

void __cdecl edpartLoadSingleType(PARTTYPE_LOOKUP_PC *record, i32 version, i32 page) {
    PART_RUNTIME_TYPE_PC *type = (PART_RUNTIME_TYPE_PC *)record;
    char name[16];
    i32 i, scene;
    char mode;
    EdFileRead(type->name, 16);
    if (version > 5) {
        for (i = 0; i < 8; ++i)
            type->effect_pages[i] = EdFileReadChar();
    } else {
        type->effect_pages[0] = version > 1 ? EdFileReadChar() : 0;
        for (i = 1; i < 8; ++i)
            type->effect_pages[i] = -1;
    }
    if (version > 5) {
        for (i = 0; i < 8; ++i) {
            EdFileRead(type->object_names[i], 16);
            type->effect_ids[i] = -1;
            if (type->effect_pages[i] == 0) {
                for (scene = 0; scene < 32; ++scene) {
                    if (part_scene_pageid[scene] == page)
                        type->effect_ids[i] = edpartLookupObjectInScene(type->object_names[i], part_scene[scene]);
                    if (type->effect_ids[i] != -1) {
                        type->scene_indices[i] = scene;
                        break;
                    }
                }
            } else if (type->effect_pages[i] == 1) {
                for (scene = 0; scene < 32; ++scene) {
                    if (part_scene_pageid[scene] == 0)
                        type->effect_ids[i] = edpartLookupObjectInScene(type->object_names[i], part_scene[scene]);
                    if (type->effect_ids[i] != -1) {
                        type->scene_indices[i] = scene;
                        break;
                    }
                }
            }
            if (type->effect_ids[i] == -1 && type->object_names[i][0] != 0)
                type->effect_ids[i] = 9998;
        }
    } else {
        EdFileRead(type->object_names[0], 16);
        type->effect_ids[0] = -1;
        if (type->effect_pages[0] == 0) {
            for (scene = 0; scene < 32; ++scene) {
                if (part_scene_pageid[scene] == page)
                    type->effect_ids[0] = edpartLookupObjectInScene(type->object_names[0], part_scene[scene]);
                if (type->effect_ids[0] != -1) {
                    type->scene_indices[0] = scene;
                    break;
                }
            }
        } else if (type->effect_pages[0] == 1) {
            for (scene = 0; scene < 32; ++scene) {
                if (part_scene_pageid[scene] == 0)
                    type->effect_ids[0] = edpartLookupObjectInScene(type->object_names[0], part_scene[scene]);
                if (type->effect_ids[0] != -1) {
                    type->scene_indices[0] = scene;
                    break;
                }
            }
        }
        if (type->effect_ids[0] == -1 && type->object_names[0][0] != 0)
            type->effect_ids[0] = 9998;
    }
    type->variant_count = 0;
    for (i = 0; i < 8; ++i) {
        if (type->effect_ids[i] != 9999 && type->effect_ids[i] != -1)
            ++type->variant_count;
    }
    if (version > 5)
        type->variant_mode = EdFileReadChar();
    else
        type->variant_mode = 0;
    if (version > 15) {
        type->particle_scale = EdFileReadFloat();
        type->effect_scale = EdFileReadFloat();
    } else {
        type->particle_scale = 1.0f;
        type->effect_scale = 1.0f;
    }
    type->lifetime = EdFileReadFloat();
    if (version > 13)
        type->lifetime_random = EdFileReadFloat();
    else
        type->lifetime_random = 0.0f;
    type->speed = EdFileReadFloat();
    type->gravity = EdFileReadFloat();
    type->emission_rate = EdFileReadFloat();
    if (version > 7)
        type->bounce = EdFileReadFloat();
    else
        type->bounce = 0.75f;
    type->position_random.x = EdFileReadFloat();
    type->position_random.y = EdFileReadFloat();
    type->position_random.z = EdFileReadFloat();
    type->velocity_random.x = EdFileReadFloat();
    type->velocity_random.y = EdFileReadFloat();
    type->velocity_random.z = EdFileReadFloat();
    type->emission_period = EdFileReadFloat();
    type->emission_period_random = EdFileReadFloat();
    type->emission_pause = EdFileReadFloat();
    type->emission_pause_random = EdFileReadFloat();
    if (version > 12) {
        type->rotation[0] = EdFileReadInt();
        type->rotation[1] = EdFileReadInt();
        type->rotation[2] = EdFileReadInt();
        type->rotation_random[0] = EdFileReadInt();
        type->rotation_random[1] = EdFileReadInt();
        type->rotation_random[2] = EdFileReadInt();
    } else {
        type->rotation[0] = EdFileReadShort();
        type->rotation[1] = EdFileReadShort();
        type->rotation[2] = EdFileReadShort();
        type->rotation_random[0] = EdFileReadShort();
        type->rotation_random[1] = EdFileReadShort();
        type->rotation_random[2] = EdFileReadShort();
    }
    type->flags = EdFileReadUnsignedInt();
    if (type->effect_ids[0] == 9999)
        type->flags |= 0x10;
    if (version > 8) {
        EdFileRead(name, 16);
        type->trail_effects[0] =
            (PartLoadParticlePagePc != -1 ? LookupDebrisEffectPageOnly(name, (i8)PartLoadParticlePagePc)
                                          : LookupDebrisEffect(name));
        EdFileRead(name, 16);
        type->trail_effects[1] =
            (PartLoadParticlePagePc != -1 ? LookupDebrisEffectPageOnly(name, (i8)PartLoadParticlePagePc)
                                          : LookupDebrisEffect(name));
        EdFileRead(name, 16);
        type->attached_effect =
            (PartLoadParticlePagePc != -1 ? LookupDebrisEffectPageOnly(name, (i8)PartLoadParticlePagePc)
                                          : LookupDebrisEffect(name));
        type->trail_rates[0] = EdFileReadFloat();
        type->trail_rates[1] = EdFileReadFloat();
    } else if (version > 4) {
        EdFileRead(name, 16);
        type->trail_effects[0] =
            (PartLoadParticlePagePc != -1 ? LookupDebrisEffectPageOnly(name, (i8)PartLoadParticlePagePc)
                                          : LookupDebrisEffect(name));
        type->trail_effects[1] = -1;
        EdFileRead(name, 16);
        type->attached_effect =
            (PartLoadParticlePagePc != -1 ? LookupDebrisEffectPageOnly(name, (i8)PartLoadParticlePagePc)
                                          : LookupDebrisEffect(name));
        type->trail_rates[0] = EdFileReadFloat();
        type->trail_rates[1] = 60.0f;
    } else {
        type->trail_effects[0] = -1;
        type->trail_effects[1] = -1;
        type->attached_effect = -1;
        if (version > 2) {
            mode = EdFileReadChar();
            EdFileRead(name, 16);
            if (mode == 0)
                type->trail_effects[0] =
                    (PartLoadParticlePagePc != -1 ? LookupDebrisEffectPageOnly(name, (i8)PartLoadParticlePagePc)
                                                  : LookupDebrisEffect(name));
            else if (mode == 1)
                type->attached_effect =
                    (PartLoadParticlePagePc != -1 ? LookupDebrisEffectPageOnly(name, (i8)PartLoadParticlePagePc)
                                                  : LookupDebrisEffect(name));
            type->trail_rates[0] = EdFileReadFloat();
        } else
            type->trail_rates[0] = 60.0f;
        type->trail_rates[1] = 60.0f;
    }
    if (version > 3) {
        EdFileRead(name, 16);
        type->kill_effect = (PartLoadParticlePagePc != -1 ? LookupDebrisEffectPageOnly(name, (i8)PartLoadParticlePagePc)
                                                          : LookupDebrisEffect(name));
        EdFileRead(name, 16);
        type->impact_effect =
            (PartLoadParticlePagePc != -1 ? LookupDebrisEffectPageOnly(name, (i8)PartLoadParticlePagePc)
                                          : LookupDebrisEffect(name));
    } else {
        type->impact_effect = -1;
        type->kill_effect = -1;
    }
    if (version > 11) {
        EdFileRead(type->impact_part_name, 16);
        type->impact_part = PARTLookupType(type->impact_part_name);
    } else {
        type->impact_part_name[0] = 0;
        type->impact_part = -1;
    }
    if (version > 6) {
        for (i = 0; i < 4; ++i) {
            EdFileRead(name, 16);
            type->sounds[i] = GetSfxIdN(name, 16);
        }
        for (i = 0; i < 4; ++i)
            type->sound_modes[i] = EdFileReadChar();
    } else {
        for (i = 0; i < 4; ++i) {
            type->sounds[i] = -1;
            type->sound_modes[i] = 0;
        }
    }
    if (version > 9)
        type->maximum_distance = EdFileReadFloat();
    else
        type->maximum_distance = 25.0f;
    if (version > 14) {
        type->field_160 = EdFileReadFloat();
        type->field_164 = EdFileReadFloat();
        type->field_168 = EdFileReadFloat();
    } else {
        type->field_160 = 1.0f;
        type->field_164 = 1.0f;
        type->field_168 = 1.0f;
    }
    type->scale = 1.0f;
    type->field_174 = 0;
}

extern NUVEC PartEditorCameraPosPc;
extern PARTTYPE_LOOKUP_PC *edpart_nearest_type;
extern i32 PartGeneralPagePc;
extern i32 PartLevelPagePc;
void __cdecl edpartDetermineNearest(f32 distance) {
    NUVEC delta;
    f32 squared_distance;
    i32 i;
    if (edpart_nearest != -1 && part_emits[edpart_nearest].effect_id != -1) {
        NuVecSub(&delta, &PartEditorCameraPosPc, &part_emits[edpart_nearest].position);
        squared_distance = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        if (squared_distance == 0.0f)
            return;
    }
    edpart_nearest = -1;
    for (i = 0; i < 40; ++i) {
        if (part_emits[i].effect_id != -1) {
            NuVecSub(&delta, &PartEditorCameraPosPc, &part_emits[i].position);
            squared_distance = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
            if (distance < 0.0f || squared_distance < distance) {
                distance = squared_distance;
                edpart_nearest = i;
            }
        }
    }
    edpart_nearest_emit = NULL;
    edpart_nearest_type = NULL;
    if (edpart_nearest != -1) {
        edpart_nearest_emit = &part_emits[edpart_nearest];
        if (edpart_nearest_emit->effect_id != -1)
            edpart_nearest_type = &part_types[edpart_nearest_emit->effect_id];
    }
}

void edpartChangeNameMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_create_type != -1) {
        PARTTYPE_LOOKUP_PC *type = &part_types[edpart_create_type];
        edpart_name_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edpartCancelChangeNameMenu, "Type Name");
        if (edpart_name_menu != NULL) {
            eduiMenuAddItem(edpart_name_menu, eduiItemTextPickCreate(0, edblack, edpartChangeName, "Name: "));
            strcpy(((edui_textpicker_s *)(edui_last_item))->value, type->name);
            ((edui_textpicker_s *)(edui_last_item))->max_length = 15;
            eduiMenuAttach(menu, edpart_name_menu);
            edpart_name_menu->x = menu->x + 10;
            edpart_name_menu->y = menu->y + 40;
        }
    }
}
void edpartChangeCutOff(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type)
        edpart_nearest_type->maximum_distance = ((edui_slider_s *)(item))->value;
}

void edpartSetScaleFactor(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edpart_scale_factor = ((edui_slider_s *)(item))->value;
}
void edpartCopyType(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    struct eduimenu_s *parent;
    i32 index;
    parent = NULL;
    index = 0;
    if (part_types_used < 128 && edpart_create_type != -1) {
        for (; index < 128; ++index) {
            if (part_types[index].name[0] == '\0') {
                u32 name_length;
                PARTTYPE_LOOKUP_PC *source = &part_types[edpart_create_type];
                PARTTYPE_LOOKUP_PC *copy = &part_types[index];
                *copy = *source;
                name_length = strlen(source->name);
                if (name_length <= 12) {
                    sprintf(copy->name, "%s%03d", source->name, index);
                } else {
                    char name[16];
                    sprintf(name, "%s", source->name);
                    name[12] = '\0';
                    sprintf(copy->name, "%s%03d", name, index);
                }
                ++part_types_used;
                edpart_create_type = index;
                break;
            }
        }
    }
    if (menu->parent) {
        parent = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->callback)
        menu->callback(menu, parent);
}

void __cdecl edpartChangeTintR(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type)
        edpart_nearest_type->field_160 = ((edui_slider_s *)item)->value;
}

void __cdecl edpartChangeTintG(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type)
        edpart_nearest_type->field_164 = ((edui_slider_s *)item)->value;
}

void __cdecl edpartChangeTintB(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type)
        edpart_nearest_type->field_168 = ((edui_slider_s *)item)->value;
}

void __cdecl edpartCancelTintMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_tint_menu);
    edpart_tint_menu = NULL;
}

void __cdecl edpartChangeInstanceScale(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type)
        edpart_nearest_type->particle_scale = ((edui_slider_s *)item)->value;
}

void __cdecl edpartCancelInstanceScaleMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_instancescale_menu);
    edpart_instancescale_menu = NULL;
}

void edpartTintMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type != NULL) {
        edpart_tint_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelTintMenu, "Instance Tint");
        if (edpart_tint_menu != NULL) {
            eduiMenuAddItem(edpart_tint_menu, eduiItemSliderCreate(0, edblack, 0, edpartChangeTintR, 0.0f, 2.0f,
                                                                   edpart_nearest_type->field_160, "Red Tint"));
            eduiMenuAddItem(edpart_tint_menu, eduiItemSliderCreate(0, edblack, 0, edpartChangeTintG, 0.0f, 2.0f,
                                                                   edpart_nearest_type->field_164, "Green Tint"));
            eduiMenuAddItem(edpart_tint_menu, eduiItemSliderCreate(0, edblack, 0, edpartChangeTintB, 0.0f, 2.0f,
                                                                   edpart_nearest_type->field_168, "Blue Tint"));
            eduiMenuAttach(menu, edpart_tint_menu);
            edpart_tint_menu->x = menu->x + 10;
            edpart_tint_menu->y = menu->y + 40;
        }
    }
}

void edpartInstanceScaleMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    edpart_instancescale_menu =
        eduiMenuCreate(70, 70, 180, 300, ed_fnt, edpartCancelInstanceScaleMenu, "Instance Scale");
    if (edpart_instancescale_menu != NULL && edpart_nearest_type != NULL) {
        eduiMenuAddItem(edpart_instancescale_menu,
                        eduiItemSliderCreate(0, edblack, 0, edpartChangeInstanceScale, 0.0f, edpart_superscale * 10.0f,
                                             edpart_nearest_type->particle_scale, "Instance Scale"));
        eduiMenuAttach(menu, edpart_instancescale_menu);
        edpart_instancescale_menu->x = menu->x + 10;
        edpart_instancescale_menu->y = menu->y + 40;
    }
}

void edpartChangeMaxLifeMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2);
void edpartInstanceOrientMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2);

void __cdecl edpartCancelInstanceSettingsMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_instancesettings_menu);
    edpart_instancesettings_menu = NULL;
}

void __cdecl edpartInstanceSettingsMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type == NULL)
        return;
    edpart_instancesettings_menu =
        eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelInstanceSettingsMenu, "Instance Settings");
    if (edpart_instancesettings_menu != NULL) {
        eduiMenuAddItem(edpart_instancesettings_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edpartChangeMaxLifeMenu, "Max Instance Life..."));
        eduiMenuAddItem(edpart_instancesettings_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edpartInstanceOrientMenu, "Instance Orientation..."));
        eduiMenuAddItem(edpart_instancesettings_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edpartInstanceFlagsMenu, "Instance Flags..."));
        eduiMenuAddItem(edpart_instancesettings_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edpartTintMenu, "Instance Tint..."));
        eduiMenuAddItem(edpart_instancesettings_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edpartInstanceScaleMenu, "Instance Scale..."));
    }
    eduiMenuAttach(menu, edpart_instancesettings_menu);
    edpart_instancesettings_menu->x = menu->x + 10;
    edpart_instancesettings_menu->y = menu->y + 40;
}

extern NUMTL *edpart_mtl;
extern i32 edpart_copy_source[8];
extern NUVEC edpart_copy_source_vec;
extern i32 edpart_copy_source_count;
void edbitsDrawCube(f32 x, f32 y, f32 z, f32 half_x, f32 half_y, f32 half_z, i32 rotation_z, i32 rotation_y,
                    i32 rotation_x, i32 outer_z, i32 outer_y, i32 colour, struct PC_SCENE_MTL_VIEW *material);

void edpartMultipleCopyCopy(void) {
    extern i32 edpart_copyroty;
    extern f32 edpart_copy_size;
    i32 index;
    NUVEC delta;
    union {
        f32 value;
        u32 bits;
    } temp;
    union {
        f32 value[3];
        u32 bits[3];
    } abs_delta;
    edpart_copy_source_count = 0;
    for (index = 0; index < 512; ++index) {
        if (part_emits[index].instance_id == -1 || part_emits[index].instance_id == 99999)
            continue;
        NuVecSub(&delta, &PartEditorCameraPosPc, &part_emits[index].position);
        temp.value = delta.x;
        abs_delta.bits[0] = temp.bits & 0x7fffffff;
        if (!(abs_delta.value[0] <= edpart_copy_size))
            continue;
        temp.value = delta.y;
        abs_delta.bits[1] = temp.bits & 0x7fffffff;
        if (!(abs_delta.value[1] <= edpart_copy_size))
            continue;
        temp.value = delta.z;
        abs_delta.bits[2] = temp.bits & 0x7fffffff;
        if (!(abs_delta.value[2] <= edpart_copy_size))
            continue;
        edpart_copy_source[edpart_copy_source_count++] = index;
        if (edpart_copy_source_count == 8)
            break;
    }
    edpart_copy_source_vec = PartEditorCameraPosPc;
    edpart_copyroty = 0;
}

void edpartHighlightNearest(void) {
    extern i32 edpart_copy_mode, edpart_copyroty, edpart_copy_enclosed;
    extern f32 edpart_copy_size;
    i32 index;
    NUVEC delta;
    NUVEC offset;
    NUVEC destination;
    union {
        f32 value;
        u32 bits;
    } temp;
    union {
        f32 value[3];
        u32 bits[3];
    } abs_delta;
    if (edpart_copy_mode != 0) {
        edpart_copy_enclosed = 0;
        for (index = 0; index < 512; ++index) {
            if (part_emits[index].instance_id == -1 || part_emits[index].instance_id == 99999)
                continue;
            NuVecSub(&delta, &PartEditorCameraPosPc, &part_emits[index].position);
            temp.value = delta.x;
            abs_delta.bits[0] = temp.bits & 0x7fffffff;
            if (!(abs_delta.value[0] <= edpart_copy_size))
                continue;
            temp.value = delta.y;
            abs_delta.bits[1] = temp.bits & 0x7fffffff;
            if (!(abs_delta.value[1] <= edpart_copy_size))
                continue;
            temp.value = delta.z;
            abs_delta.bits[2] = temp.bits & 0x7fffffff;
            if (!(abs_delta.value[2] <= edpart_copy_size))
                continue;
            ++edpart_copy_enclosed;
            edbitsDrawDiagonalCross(part_emits[index].position.x, part_emits[index].position.y,
                                    part_emits[index].position.z, 0.05f, -1, (struct PC_SCENE_MTL_VIEW *)edpart_mtl);
        }
        if (edpart_copy_source_count > 0) {
            for (index = 0; index < edpart_copy_source_count; ++index) {
                NuVecSub(&offset, &part_emits[edpart_copy_source[index]].position, &edpart_copy_source_vec);
                NuVecRotateY(&offset, &offset, edpart_copyroty);
                NuVecAdd(&destination, &PartEditorCameraPosPc, &offset);
                edbitsDrawDiagonalCross(destination.x, destination.y, destination.z, 0.05f, 0xff808080,
                                        (struct PC_SCENE_MTL_VIEW *)edpart_mtl);
            }
        }
    } else if (edpart_nearest != -1) {
        edbitsDrawCube(part_emits[edpart_nearest].position.x, part_emits[edpart_nearest].position.y,
                       part_emits[edpart_nearest].position.z, 0.5f, 0.5f, 0.5f, 0, 0, 0, 0, 0, -1,
                       (struct PC_SCENE_MTL_VIEW *)edpart_mtl);
    }
}

void edpartRender() {
    edcamSet();
    edpartDrawCursor();
    edpartHighlightNearest();
    if (edpart_active_menu)
        eduiMenuRender(edpart_active_menu);
}

void edpartChangeName(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_create_type != -1) {
        i32 index;
        char *name = ((edui_textpicker_s *)(item))->value;
        NuStrNCpy(part_types[edpart_create_type].name, name, 16);
        for (index = 0; index < 40; ++index) {
            if (part_emits[index].effect_id == edpart_create_type)
                NuStrNCpy(part_emits[index].name, name, 16);
        }
    }
}

void edpartCancelChangeNameMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_name_menu);
    edpart_name_menu = NULL;
}

i32 __cdecl edpartLoadPageEx(char *path, i8 mode, NUGSCN **scenes, i32 scene_count) {
    i32 page = 0;
    i32 scene_slot, i, version, type_count, skipped_types, type_slot, emitter_count, emitter_slot, sound;
    PART_RUNTIME_TYPE_PC *type;
    PART_EMIT_PC *emitter;
    PART_RUNTIME_TYPE_PC discarded_type;
    if (mode != 0) {
        for (page = 1; page < 8; ++page) {
            if (part_page_used[page] == 0)
                break;
        }
        if (page == 8)
            return -1;
    }
    scene_slot = 0;
    for (i = 0; i < scene_count; ++i) {
        if (scenes[i] != NULL) {
            while (part_scene[scene_slot] != NULL && scene_slot < 32)
                ++scene_slot;
            if (scene_slot < 32) {
                part_scene[scene_slot] = scenes[i];
                part_scene_pageid[scene_slot] = page;
            }
        }
    }
    EdFileSetMedia(1);
    if (EdFileOpen(path, 0) != 0) {
        EdFileSetReadWrongEndianess(1);
        version = EdFileReadInt();
        if (version >= 1 && version <= 16) {
            type_count = EdFileReadInt();
            skipped_types = 0;
            if (type_count + part_types_used > 128) {
                skipped_types = type_count + part_types_used - 128;
                type_count -= skipped_types;
            }
            type_slot = 0;
            while (type_count > 0) {
                while (part_types[type_slot].name[0] != 0)
                    ++type_slot;
                type = &((PART_RUNTIME_TYPE_PC *)part_types)[type_slot];
                edpartLoadSingleType((PARTTYPE_LOOKUP_PC *)type, version, page);
                if (type->name[0] != 0) {
                    type->page = page;
                    type->field_b3 = mode;
                    ++part_types_used;
                }
                --type_count;
            }
            while (skipped_types > 0) {
                edpartLoadSingleType((PARTTYPE_LOOKUP_PC *)&discarded_type, version, page);
                --skipped_types;
            }
            emitter_count = EdFileReadInt();
            if (emitter_count + part_emits_used > 40)
                emitter_count = 40 - part_emits_used;
            emitter_slot = 0;
            while (emitter_count > 0) {
                while (part_emits[emitter_slot].effect_id != -1)
                    ++emitter_slot;
                emitter = &part_emits[emitter_slot];
                emitter->position.x = EdFileReadFloat();
                emitter->position.y = EdFileReadFloat();
                emitter->position.z = EdFileReadFloat();
                EdFileRead(emitter->name, 16);
                emitter->effect_id = PARTLookupTypePageOnly(emitter->name, page);
                emitter->rotation_30 = EdFileReadShort();
                emitter->rotation_2e = EdFileReadShort();
                emitter->rotation_2c = EdFileReadShort();
                if (version > 10) {
                    emitter->field_44 = EdFileReadShort();
                    emitter->field_46 = EdFileReadShort();
                } else {
                    emitter->field_44 = 0;
                    emitter->field_46 = 0;
                }
                emitter->page = page;
                emitter->field_3c = 0;
                type = &((PART_RUNTIME_TYPE_PC *)part_types)[emitter->effect_id];
                for (sound = 0; sound < 4; ++sound) {
                    if (type->sounds[sound] != -1 && type->sound_modes[sound] != 0) {
                        emitter->field_3c = 1;
                        break;
                    }
                }
                if (emitter->effect_id != -1) {
                    ++part_emits_used;
                    PARTStartOffset(emitter_slot, 0.0f);
                    emitter->field_3d = 1;
                }
                --emitter_count;
            }
            EdFileSetReadWrongEndianess(0);
            EdFileClose();
            edpartDetermineNearest(1.0f);
            part_page_used[page] = 1;
            if (page == 0)
                part_page_on[0] = 1;
            if (mode == 0)
                PartGeneralPagePc = page;
            else if (mode == 1)
                PartLevelPagePc = page;
            return page;
        }
        EdFileClose();
    }
    return -1;
}

void __cdecl UpdatePartEmits(f32 time) {
    i32 switch_changes[32][2];
    i32 i, sound, axis, state, remaining, variant;
    i8 next;
    PART_EMIT_PC *emitter;
    PART_RUNTIME_TYPE_PC *type;
    ADDPART_PC params;
    NUVEC position, velocity;
    NUMTX matrix;
    NUSPECIALHANDLE special;
    f32 interval, shift, previous_distance, emission_time, pause, random, lifetime;
    f32 random_x, random_y, random_z, range_x, range_y, range_z;
    i32 last_switch_change = -1;
    for (i = 0; i < 40; ++i) {
        emitter = &part_emits[i];
        if (part_page_on[emitter->page] == 0 || emitter->effect_id == -1)
            continue;
        type = &((PART_RUNTIME_TYPE_PC *)part_types)[emitter->effect_id];
        if (type->effect_ids[0] == -1)
            continue;
        interval = 1.0f / type->emission_rate;
        if (emitter->field_44 == 1 && emitter->field_3d != 2 && emitter->field_46 != -1) {
            state = debris_object_switches[emitter->field_46];
            switch (state) {
                case 0:
                    emitter->field_3d = 0;
                    break;
                case 1:
                    emitter->field_3d = 1;
                    break;
                case 2:
                case 3: {
                    last_switch_change = (last_switch_change + 1) & 31;
                    emitter->field_3d = 2;
                    switch_changes[last_switch_change][0] = emitter->field_46;
                    switch_changes[last_switch_change][1] = state == 2 ? 4 : 0;
                    PARTStartOffset(i, 0.0f);
                    shift = emitter->time_24 - partglobaltime;
                    emitter->time_20 = partglobaltime - interval;
                    emitter->time_24 -= shift;
                    emitter->time_28 -= shift;
                    break;
                }
                case 4:
                    last_switch_change = (last_switch_change + 1) & 31;
                    switch_changes[last_switch_change][0] = emitter->field_46;
                    switch_changes[last_switch_change][1] = 0;
                    break;
            }
        }
        previous_distance = emitter->camera_distance;
        emitter->camera_distance = CameraEmitterDistance(&emitter->position);
        if (emitter->field_3c != 0) {
            for (sound = 0; sound < 4; ++sound) {
                if (type->sounds[sound] != -1) {
                    SetSfxBit_On(type->sounds[sound]);
                    if (type->sound_modes[sound] == 4)
                        PlaySfxById(type->sounds[sound], &emitter->position);
                }
            }
        }
        if ((type->flags & 0x400000) != 0) {
            if (type->maximum_distance > emitter->camera_distance && previous_distance >= type->maximum_distance) {
                emitter->time_24 = partglobaltime;
                emitter->time_28 = partglobaltime - 1.0f;
                emitter->time_20 = partglobaltime - interval + 0.001f;
            } else if (emitter->time_24 > emitter->time_28) {
                emitter->time_28 += time;
                emitter->time_24 += time;
                emitter->time_20 += time;
            }
        }
        emission_time = emitter->time_20 + interval;
        remaining = 101;
        while (partglobaltime + time > emission_time && --remaining != 0) {
            while (emission_time >= emitter->time_28 && emission_time > emitter->time_24 &&
                   partglobaltime + time > emission_time) {
                if (emitter->time_24 >= emitter->time_28) {
                    emitter->time_28 = NuRandFloatSeeded(&partseed) * type->emission_period_random +
                                       (type->emission_period + emitter->time_24);
                    if (emitter->field_3c != 0 && emitter->field_3d != 0) {
                        for (sound = 0; sound < 4; ++sound) {
                            if (type->sounds[sound] != -1 && type->sound_modes[sound] == 1)
                                PlaySfxById(type->sounds[sound], &emitter->position);
                        }
                    }
                    if (emitter->shots_remaining != 0 && --emitter->shots_remaining == 0) {
                        if (part_emits[i].effect_id != -1) {
                            part_emits[i].effect_id = -1;
                            --part_emits_used;
                        }
                        emission_time = partglobaltime + 99999.9f;
                    }
                } else {
                    pause = NuRandFloatSeeded(&partseed) * type->emission_pause_random + type->emission_pause;
                    emitter->time_24 = emitter->time_28 + pause;
                    emission_time = emitter->time_24 + 0.001f;
                    emitter->time_20 = emission_time - interval + 0.001f;
                    if (emitter->field_3d == 2)
                        emitter->field_3d = 0;
                    if (emitter->field_3c != 0 && emitter->field_3d != 0) {
                        for (sound = 0; sound < 4; ++sound) {
                            if (type->sounds[sound] != -1 && type->sound_modes[sound] == 2)
                                PlaySfxById(type->sounds[sound], &emitter->position);
                        }
                    }
                }
            }
            if (partglobaltime + time <= emission_time)
                continue;
            if (emitter->field_3d != 0 &&
                (type->maximum_distance == 0.0f || type->maximum_distance > emitter->camera_distance)) {
                params = Default_ADDPART;
                params.field_c0 = type->particle_scale;
                random = NuRandFloatSeeded(&partseed);
                position.x = (random + random) * type->position_random.x - type->position_random.x;
                random = NuRandFloatSeeded(&partseed);
                position.y = (random + random) * type->position_random.y - type->position_random.y;
                random = NuRandFloatSeeded(&partseed);
                position.z = (random + random) * type->position_random.z - type->position_random.z;
                NuVecRotateZ(&position, &position, emitter->rotation_2c);
                NuVecRotateY(&position, &position, emitter->rotation_2e);
                NuVecRotateX(&position, &position, emitter->rotation_30);
                NuVecAdd(&position, &position, &emitter->position);
                params.position = &position;
                velocity.x = 0.0f;
                velocity.y = type->speed;
                velocity.z = 0.0f;
                random = NuRandFloatSeeded(&partseed);
                velocity.x += (random + random) * type->velocity_random.x - type->velocity_random.x;
                random = NuRandFloatSeeded(&partseed);
                velocity.y += (random + random) * type->velocity_random.y - type->velocity_random.y;
                random = NuRandFloatSeeded(&partseed);
                velocity.z += (random + random) * type->velocity_random.z - type->velocity_random.z;
                NuVecRotateZ(&velocity, &velocity, emitter->rotation_2c);
                NuVecRotateY(&velocity, &velocity, emitter->rotation_2e);
                NuVecRotateX(&velocity, &velocity, emitter->rotation_30);
                params.velocity = &velocity;
                params.gravity = type->gravity;
                params.field_20 = type->bounce;
                variant = 0;
                if (type->variant_count != 0) {
                    if (type->variant_mode == 0)
                        variant = (i32)(NuRandFloatSeeded(&partseed) * 65535.0f) % type->variant_count;
                    else if (type->variant_mode == 1) {
                        next = (i8)(emitter->field_33 + 1);
                        if (type->variant_count > next)
                            variant = next;
                        emitter->field_33 = variant;
                    }
                }
                params.part_id = type->effect_ids[variant];
                if ((u16)(params.part_id - 9998) <= 1)
                    params.special = NULL;
                else if (params.part_id != -1) {
                    params.special = &special;
                    NuGScnGetSpecial(&special, part_scene[type->scene_indices[variant]], params.part_id);
                }
                lifetime = type->lifetime;
                params.field_a4 = lifetime + NuRandFloatSeeded(&partseed) * type->lifetime_random;
                NuMtxSetIdentity(&matrix);
                random_x = NuRandFloatSeeded(&partseed);
                range_x = (f32)(type->rotation_random[0]);
                random_y = NuRandFloatSeeded(&partseed);
                range_y = (f32)(type->rotation_random[1]);
                random_z = NuRandFloatSeeded(&partseed);
                range_z = (f32)(type->rotation_random[2]);
                for (axis = 0; axis < 3; ++axis) {
                    params.field_a8[axis] = type->rotation[axis];
                    params.field_a8[axis + 3] = type->rotation_random[axis];
                }
                NuMtxRotateX(&matrix, (i16)((i32)((random_x + random_x) * range_x + (f64)type->rotation[0] - range_x)));
                NuMtxRotateY(&matrix, (i16)((i32)((random_y + random_y) * range_y + (f64)type->rotation[1] - range_y)));
                NuMtxRotateZ(&matrix, (i16)((i32)((random_z + random_z) * range_z + (f64)type->rotation[2] - range_z)));
                matrix.m30 = position.x;
                matrix.m31 = position.y;
                matrix.m32 = position.z;
                params.matrix = &matrix;
                params.flags = type->flags & ~0x60000;
                if (type->trail_effects[0] != -1) {
                    params.effect_id_60 = type->trail_effects[0];
                    params.flags |= 0x40000;
                }
                if (type->trail_effects[1] != -1) {
                    params.effect_id_64 = type->trail_effects[1];
                    params.flags |= 0x40000;
                }
                if (type->attached_effect != -1) {
                    params.effect_id_58 = type->attached_effect;
                    params.flags |= 0x20000;
                }
                params.field_68 = type->trail_rates[0];
                params.field_6c = type->trail_rates[1];
                params.effect_id_70 = type->impact_effect;
                if (type->impact_part == -1 && type->impact_part_name[0] != 0)
                    type->impact_part = PARTLookupType(type->impact_part_name);
                params.effect_id_74 = type->impact_part;
                params.effect_scale = type->effect_scale;
                params.lighting_scale_x = type->field_160;
                params.lighting_scale_y = type->field_164;
                params.lighting_scale_z = type->field_168;
                params.effect_id_78 = type->kill_effect;
                AddPart(&params);
                if (emitter->field_3c != 0) {
                    for (sound = 0; sound < 4; ++sound) {
                        if (type->sounds[sound] != -1 && type->sound_modes[sound] == 3)
                            PlaySfxById(type->sounds[sound], &emitter->position);
                    }
                }
                type->last_used_time = emission_time;
            }
            emitter->time_20 = emission_time;
            emission_time += interval;
        }
    }
    for (i = 0; i <= last_switch_change; ++i)
        debris_object_switches[switch_changes[i][0]] = switch_changes[i][1];
}

#include <float.h>
typedef struct PART_TERRAIN_STATE_PC {
    u8 unknown_000[0x200];
    u32 platform_id;
    u8 unknown_204[5];
    i8 impact_material;
} PART_TERRAIN_STATE_PC;
STATIC_ASSERT(offsetof(PART_TERRAIN_STATE_PC, platform_id) == 0x200);
STATIC_ASSERT(offsetof(PART_TERRAIN_STATE_PC, impact_material) == 0x209);
static NUVEC PartImpactNormalPrivate = {0};
#define PART_NORM PartImpactNormalPrivate
extern i32 __cdecl NewRayCast(NUVEC *, NUVEC *, f32, i32);
extern f32 __cdecl NewRayCastGetEmbedDist(void);
extern i32 __cdecl NewRayCastHitWallSpline(void);
i32 __cdecl PartRayCast(NUVEC *position, NUVEC *movement, f32 radius, i32 flags) {
    i32 hit = NewRayCast(position, movement, radius, flags);
    NewRayCastGetImpactNormal(&PART_NORM);
    return hit;
}

void __cdecl CastPart(PART_PC *part, f32 time) {
    PART_TERRAIN_STATE_PC *state = (PART_TERRAIN_STATE_PC *)part;
    i32 hit;
    f32 distance, radius;
    NUVEC movement;
    NUVEC cast_movement;
    part->active &= ~4;
    part->field_f4 = time;
    movement.x = time * part->velocity.x;
    movement.y = time * part->velocity.y + time * time * (0.5f * part->gravity);
    movement.z = time * part->velocity.z;
    if (part->velocity.y > 0.0f && movement.y <= 0.0f) {
        time = -part->velocity.y / part->gravity;
        part->field_f4 = time;
        movement.x = time * part->velocity.x;
        movement.y = time * part->velocity.y + time * time * (0.5f * part->gravity);
        movement.z = time * part->velocity.z;
    }
    distance = NuVecMagSqr(&movement);
    if (distance < 0.001f || distance > 10000.0f || !_finite(distance) || _isnan(distance)) {
        state->impact_material = -1;
        return;
    }
    if ((part->active & 0x60) == 0x20) {
        radius = (part->time_step + time) / PARTSCALEUPTIME * part->radius;
        if (radius > part->radius)
            radius = part->radius;
        cast_movement = movement;
        hit = PartRayCast(&part->position, &cast_movement, radius, 0);
        if (hit == 0) {
            state->impact_material = -1;
            return;
        }
        state->impact_material = NewRayCastGetImpactTerrainType();
        if ((NewRayCastGetTOFI() > 0.0f || PART_NORM.y > 0.5f) &&
            ((part->flags & 0x10000000) == 0 || NewRayCastHitWallSpline() != 4))
            goto impact;
        if (hit > 0x10) {
            part->position.x -= PART_NORM.x * NewRayCastGetEmbedDist() * 1.05f;
            part->position.y -= PART_NORM.y * NewRayCastGetEmbedDist() * 1.05f;
            part->position.z -= PART_NORM.z * NewRayCastGetEmbedDist() * 1.05f;
        }
    }
    part->active &= ~0x60;
    cast_movement = movement;
    hit = PartRayCast(&part->position, &cast_movement, part->field_e0, 0);
    if (hit == 0) {
        state->impact_material = -1;
        return;
    }
    state->impact_material = NewRayCastGetImpactTerrainType();
    if ((NewRayCastGetTOFI() > 0.0f || PART_NORM.y > 0.5f) &&
        ((part->flags & 0x10000000) == 0 || NewRayCastHitWallSpline() != 4))
        goto impact;
    if (hit > 0x10) {
        part->position.x -= PART_NORM.x * NewRayCastGetEmbedDist() * 1.05f;
        part->position.y -= PART_NORM.y * NewRayCastGetEmbedDist() * 1.05f;
        part->position.z -= PART_NORM.z * NewRayCastGetEmbedDist() * 1.05f;
    }
    {
        radius = (part->time_step - time) / PARTSCALEUPTIME * part->radius;
        if (!(radius > 0.0f))
            return;
        part->active = (part->active & ~0x60) | 0x40;
        cast_movement = movement;
        if (PartRayCast(&part->position, &cast_movement, radius, 0) == 0) {
            state->impact_material = -1;
            return;
        }
        state->impact_material = NewRayCastGetImpactTerrainType();
        if ((part->flags & 0x10000000) != 0 && NewRayCastHitWallSpline() == 4)
            return;
    }
impact:
    part->active |= 4;
    state->platform_id = TerrainPlatId();
    part->active = (part->active & ~0x10) | (state->platform_id != (u32)-1 ? 0x10 : 0);
    part->field_f4 *= NewRayCastGetTOFI();
    part->impact_normal = PART_NORM;
    part->impact_position.x = part->position.x + cast_movement.x;
    part->impact_position.y = part->position.y + cast_movement.y;
    part->impact_position.z = part->position.z + cast_movement.z;
}

void edpartPlace(i32 index, NUVEC *position) {
    PART_EMIT_PC *emitter = &part_emits[index];
    emitter->position = *position;
    emitter->rotation_2c = ((i16)(edpart_emitrotz));
    emitter->rotation_30 = ((i16)(edpart_emitrotx));
    emitter->rotation_2e = ((i16)(edpart_emitroty));
}

void edpartDestroy(i32 index) {
    if (part_emits[index].effect_id != -1) {
        --part_emits_used;
        part_emits[index].effect_id = -1;
    }
}

void edcamMove(FRAME_PAD_PC *pad);
void edcamSetPos(NUVEC *position);
void edcamGetPosAng(NUVEC *position, i32 *pitch, i32 *yaw);
void edcamGetPosAngSnap(NUVEC *position, i32 *pitch, i32 *yaw);
void edpartOptMenu(void);
extern struct eduimenu_s *edpart_opt_menu;

void edpartDoInput(FRAME_PAD_PC *pad) {
    extern i32 edpart_copy_mode, edpart_copyroty, edpart_rotz, edpart_roty;
    extern i32 edpart_emitrotz, edpart_emitroty, edpart_emitrotx, edpart_refrotz, edpart_refroty;
    extern i32 edpart_dpad_mode, edpart_create_type, edpart_nearest_orphans, edpart_nearest_duplicates;
    extern i32 edpart_snap_enabled, edpart_cam_ax, edpart_cam_ay;
    extern i8 edpart_effect_list;
    extern f32 edpart_copy_size, edpart_offset;
    i32 nearest;
    i32 variant;
    i32 previous;
    if ((pad->digital_buttons & 0x100) == 0)
        edcamMove(pad);
    if (pad->digital_buttons & 0x100) {
        nearest = edpart_nearest;
        if (nearest != -1) {
            if (pad->digital_buttons_pressed & 8) {
                do {
                    ++nearest;
                    if (nearest == 40)
                        nearest = 0;
                } while (part_emits[nearest].effect_id == -1);
                edpart_nearest = nearest;
            }
            if (pad->digital_buttons_pressed & 2) {
                do {
                    --nearest;
                    if (nearest == -1)
                        nearest = 39;
                } while (part_emits[nearest].effect_id == -1);
                edpart_nearest = nearest;
            }
        } else {
            edpartDetermineNearest(-1.0f);
            nearest = edpart_nearest;
        }
        if (nearest != -1) {
            edcamSetPos(&part_emits[nearest].position);
            edpart_offset = *(f32 *)&part_emits[edpart_nearest].trailing_state_words[1];
            edpart_rotz = ((i16 *)&part_emits[edpart_nearest].trailing_state_words[0])[0];
            edpart_roty = ((i16 *)&part_emits[edpart_nearest].trailing_state_words[0])[1];
            edpart_emitrotz = part_emits[edpart_nearest].rotation_2c;
            edpart_emitroty = part_emits[edpart_nearest].rotation_2e;
            edpart_emitrotx = part_emits[edpart_nearest].rotation_30;
            edpart_create_type = part_emits[edpart_nearest].effect_id;
            edpart_effect_list = part_types[edpart_create_type].field_b3;
        }
    }
    if (edpart_snap_enabled)
        edcamGetPosAngSnap(&PartEditorCameraPosPc, &edpart_cam_ax, &edpart_cam_ay);
    else
        edcamGetPosAng(&PartEditorCameraPosPc, &edpart_cam_ax, &edpart_cam_ay);
    if ((pad->digital_buttons & 0x100) == 0) {
        if (pad->digital_buttons_pressed & 0x80) {
            edpartOptMenu();
            edpart_active_menu = edpart_opt_menu;
        }
        if ((pad->digital_buttons_pressed & 0x40) && edpart_copy_mode == 0 && edpart_create_type != -1)
            AddPARTEffect(edpart_create_type, &PartEditorCameraPosPc);
        if (pad->digital_buttons & 0x20) {
            if (edpart_copy_mode != 0)
                edpartMultipleCopyCopy();
            else if (edpart_nearest != -1)
                edpartPlace(edpart_nearest, &PartEditorCameraPosPc);
        }
        if ((pad->digital_buttons & 0x400) && edpart_copy_mode == 0 && edpart_nearest != -1)
            edpartPlace(edpart_nearest, &PartEditorCameraPosPc);
        if (pad->digital_buttons_pressed & 0x10) {
            if (edpart_copy_mode != 0) {
                edpart_copy_source_count = 0;
                edpart_copyroty = 0;
            } else {
                if (edpart_nearest != -1)
                    edpartDestroy(edpart_nearest);
                edpart_nearest = -1;
            }
        }
    }
    if (edpart_copy_mode != 0) {
        edpart_copy_size += pad->analog_left_pad_up / 5000.0f;
        edpart_copy_size -= pad->analog_left_pad_down / 5000.0f;
        if (edpart_copy_size < 0.05f)
            edpart_copy_size = 0.05f;
        else if (edpart_copy_size > 2.0f)
            edpart_copy_size = 2.0f;
        edpart_copyroty += pad->analog_left_pad_right;
        edpart_copyroty -= pad->analog_left_pad_left;
    } else {
        if (edpart_dpad_mode == 1) {
            if (pad->digital_buttons & 0x200) {
                edpart_rotz = 0;
                edpart_roty = 0;
            }
            edpart_roty += pad->analog_left_pad_right;
            edpart_roty -= pad->analog_left_pad_left;
            edpart_rotz += pad->analog_left_pad_up;
            if (edpart_rotz > 0)
                edpart_rotz = 0;
            edpart_rotz -= pad->analog_left_pad_down;
            if (edpart_rotz < -0x8000)
                edpart_rotz = -0x8000;
        } else if (edpart_dpad_mode == 0) {
            if (pad->digital_buttons & 0x200) {
                edpart_emitrotz = 0;
                edpart_emitroty = 0;
                edpart_emitrotx = 0;
            }
            if (pad->digital_buttons & 0x400) {
                edpart_emitrotx += pad->analog_left_pad_right;
                edpart_emitrotx -= pad->analog_left_pad_left;
            } else {
                edpart_emitroty += pad->analog_left_pad_right;
                edpart_emitroty -= pad->analog_left_pad_left;
                edpart_emitrotz += pad->analog_left_pad_up;
                if (edpart_emitrotz > 0x8000)
                    edpart_emitrotz = 0x8000;
                edpart_emitrotz -= pad->analog_left_pad_down;
                if (edpart_emitrotz < -0x8000)
                    edpart_emitrotz = -0x8000;
            }
        } else if (edpart_dpad_mode == 3) {
            edpart_refroty += pad->analog_left_pad_right;
            edpart_refroty -= pad->analog_left_pad_left;
            edpart_refrotz += pad->analog_left_pad_up;
            if (edpart_refrotz > 0)
                edpart_refrotz = 0;
            edpart_refrotz -= pad->analog_left_pad_down;
            if (edpart_refrotz < -0x8000)
                edpart_refrotz = -0x8000;
        } else if (edpart_dpad_mode == 2) {
            if (pad->analog_left_pad_up == 255 || (pad->digital_buttons_pressed & 0x1000))
                edpart_offset += 0.01f;
            if (pad->analog_left_pad_down == 255 || (pad->digital_buttons_pressed & 0x4000))
                edpart_offset -= 0.01f;
            if (edpart_offset < 0.0f)
                edpart_offset = 0.0f;
        }
    }
    if (edpart_nearest == -1)
        return;
    edpart_nearest_emit = &part_emits[edpart_nearest];
    if (edpart_nearest_emit->effect_id != -1)
        edpart_nearest_type = &part_types[edpart_nearest_emit->effect_id];
    edpart_nearest_duplicates = 0;
    edpart_nearest_orphans = 0;
    for (variant = 0; variant < 8; ++variant) {
        i16 effect = edpart_nearest_type->effect_ids[variant];
        if (effect == -1 || effect == 9999)
            continue;
        if (effect == 9998) {
            ++edpart_nearest_orphans;
            continue;
        }
        for (previous = 0; previous < variant; ++previous) {
            if (edpart_nearest_type->effect_ids[previous] == effect) {
                ++edpart_nearest_duplicates;
                break;
            }
        }
    }
}

void edpartDrawCursor(void) {
    extern i32 edpart_copy_mode, edpart_copyrotz, edpart_copyroty, edpart_rotz, edpart_roty;
    extern i32 edpart_emitrotz, edpart_emitroty, edpart_emitrotx, edpart_refrotz, edpart_refroty;
    extern i32 edpart_dpad_mode, edpart_readout, edpart_num_orphans;
    extern i32 edpart_create_type, edpart_nearest_orphans, edpart_nearest_duplicates, edpart_copy_enclosed;
    extern i32 part_types_used, part_emits_used;
    extern i8 edpart_effect_list;
    extern f32 edpart_copy_size;
    extern NUMTL *edpart_mtl, *edpart_boxmtl;
    i32 rotation_z, rotation_y;
    NUVEC vector;
    NURND_VERTEX3D_PC line[2];

    if (edpart_copy_mode != 0) {
        rotation_z = edpart_copyrotz;
        rotation_y = edpart_copyroty;
    } else {
        rotation_z = edpart_rotz;
        rotation_y = edpart_roty;
    }
    vector.x = 0.5f;
    vector.y = 0.0f;
    vector.z = 0.0f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPosPc.x - vector.x;
    line[0].position.y = PartEditorCameraPosPc.y - vector.y;
    line[0].position.z = PartEditorCameraPosPc.z - vector.z;
    line[0].colour = 0xffffffff;
    line[1].position.x = PartEditorCameraPosPc.x;
    line[1].position.y = PartEditorCameraPosPc.y;
    line[1].position.z = PartEditorCameraPosPc.z;
    line[1].colour = 0xffffffff;
    NuRndrLine3d(line, edpart_mtl, NULL);
    line[0].position.x = PartEditorCameraPosPc.x;
    line[0].position.y = PartEditorCameraPosPc.y;
    line[0].position.z = PartEditorCameraPosPc.z;
    line[0].colour = 0xff00ff00;
    line[1].position.x = PartEditorCameraPosPc.x + vector.x;
    line[1].position.y = PartEditorCameraPosPc.y + vector.y;
    line[1].position.z = PartEditorCameraPosPc.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.0f;
    vector.y = 0.5f;
    vector.z = 0.0f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPosPc.x - vector.x;
    line[0].position.y = PartEditorCameraPosPc.y - vector.y;
    line[0].position.z = PartEditorCameraPosPc.z - vector.z;
    line[0].colour = 0xffffffff;
    line[1].position.x = PartEditorCameraPosPc.x;
    line[1].position.y = PartEditorCameraPosPc.y;
    line[1].position.z = PartEditorCameraPosPc.z;
    line[1].colour = 0xffffffff;
    NuRndrLine3d(line, edpart_mtl, NULL);
    line[0].position.x = PartEditorCameraPosPc.x;
    line[0].position.y = PartEditorCameraPosPc.y;
    line[0].position.z = PartEditorCameraPosPc.z;
    line[0].colour = 0xff00ff00;
    line[1].position.x = PartEditorCameraPosPc.x + vector.x;
    line[1].position.y = PartEditorCameraPosPc.y + vector.y;
    line[1].position.z = PartEditorCameraPosPc.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.0f;
    vector.y = 0.0f;
    vector.z = 0.5f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPosPc.x - vector.x;
    line[0].position.y = PartEditorCameraPosPc.y - vector.y;
    line[0].position.z = PartEditorCameraPosPc.z - vector.z;
    line[0].colour = 0xffffffff;
    line[1].position.x = PartEditorCameraPosPc.x;
    line[1].position.y = PartEditorCameraPosPc.y;
    line[1].position.z = PartEditorCameraPosPc.z;
    line[1].colour = 0xffffffff;
    NuRndrLine3d(line, edpart_mtl, NULL);
    line[0].position.x = PartEditorCameraPosPc.x;
    line[0].position.y = PartEditorCameraPosPc.y;
    line[0].position.z = PartEditorCameraPosPc.z;
    line[0].colour = 0xff00ff00;
    line[1].position.x = PartEditorCameraPosPc.x + vector.x;
    line[1].position.y = PartEditorCameraPosPc.y + vector.y;
    line[1].position.z = PartEditorCameraPosPc.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.55f;
    vector.y = 0.05f;
    vector.z = 0.0f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPosPc.x + vector.x;
    line[0].position.y = PartEditorCameraPosPc.y + vector.y;
    line[0].position.z = PartEditorCameraPosPc.z + vector.z;
    line[0].colour = 0xff00ff00;
    vector.x = 0.6f;
    vector.y = -0.05f;
    vector.z = 0.0f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[1].position.x = PartEditorCameraPosPc.x + vector.x;
    line[1].position.y = PartEditorCameraPosPc.y + vector.y;
    line[1].position.z = PartEditorCameraPosPc.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.6f;
    vector.y = 0.05f;
    vector.z = 0.0f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPosPc.x + vector.x;
    line[0].position.y = PartEditorCameraPosPc.y + vector.y;
    line[0].position.z = PartEditorCameraPosPc.z + vector.z;
    line[0].colour = 0xff00ff00;
    vector.x = 0.55f;
    vector.y = -0.05f;
    vector.z = 0.0f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[1].position.x = PartEditorCameraPosPc.x + vector.x;
    line[1].position.y = PartEditorCameraPosPc.y + vector.y;
    line[1].position.z = PartEditorCameraPosPc.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.0f;
    vector.y = 0.65f;
    vector.z = -0.025f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPosPc.x + vector.x;
    line[0].position.y = PartEditorCameraPosPc.y + vector.y;
    line[0].position.z = PartEditorCameraPosPc.z + vector.z;
    line[0].colour = 0xff00ff00;
    vector.x = 0.0f;
    vector.y = 0.6f;
    vector.z = 0.0f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[1].position.x = PartEditorCameraPosPc.x + vector.x;
    line[1].position.y = PartEditorCameraPosPc.y + vector.y;
    line[1].position.z = PartEditorCameraPosPc.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.0f;
    vector.y = 0.65f;
    vector.z = 0.025f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPosPc.x + vector.x;
    line[0].position.y = PartEditorCameraPosPc.y + vector.y;
    line[0].position.z = PartEditorCameraPosPc.z + vector.z;
    line[0].colour = 0xff00ff00;
    vector.x = 0.0f;
    vector.y = 0.6f;
    vector.z = 0.0f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[1].position.x = PartEditorCameraPosPc.x + vector.x;
    line[1].position.y = PartEditorCameraPosPc.y + vector.y;
    line[1].position.z = PartEditorCameraPosPc.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.0f;
    vector.y = 0.6f;
    vector.z = 0.0f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPosPc.x + vector.x;
    line[0].position.y = PartEditorCameraPosPc.y + vector.y;
    line[0].position.z = PartEditorCameraPosPc.z + vector.z;
    line[0].colour = 0xff00ff00;
    vector.x = 0.0f;
    vector.y = 0.55f;
    vector.z = 0.0f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[1].position.x = PartEditorCameraPosPc.x + vector.x;
    line[1].position.y = PartEditorCameraPosPc.y + vector.y;
    line[1].position.z = PartEditorCameraPosPc.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.0f;
    vector.y = 0.05f;
    vector.z = 0.55f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPosPc.x + vector.x;
    line[0].position.y = PartEditorCameraPosPc.y + vector.y;
    line[0].position.z = PartEditorCameraPosPc.z + vector.z;
    line[0].colour = 0xff00ff00;
    vector.x = 0.0f;
    vector.y = 0.05f;
    vector.z = 0.6f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[1].position.x = PartEditorCameraPosPc.x + vector.x;
    line[1].position.y = PartEditorCameraPosPc.y + vector.y;
    line[1].position.z = PartEditorCameraPosPc.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.0f;
    vector.y = 0.05f;
    vector.z = 0.6f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPosPc.x + vector.x;
    line[0].position.y = PartEditorCameraPosPc.y + vector.y;
    line[0].position.z = PartEditorCameraPosPc.z + vector.z;
    line[0].colour = 0xff00ff00;
    vector.x = 0.0f;
    vector.y = -0.05f;
    vector.z = 0.55f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[1].position.x = PartEditorCameraPosPc.x + vector.x;
    line[1].position.y = PartEditorCameraPosPc.y + vector.y;
    line[1].position.z = PartEditorCameraPosPc.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.0f;
    vector.y = -0.05f;
    vector.z = 0.55f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPosPc.x + vector.x;
    line[0].position.y = PartEditorCameraPosPc.y + vector.y;
    line[0].position.z = PartEditorCameraPosPc.z + vector.z;
    line[0].colour = 0xff00ff00;
    vector.x = 0.0f;
    vector.y = -0.05f;
    vector.z = 0.6f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[1].position.x = PartEditorCameraPosPc.x + vector.x;
    line[1].position.y = PartEditorCameraPosPc.y + vector.y;
    line[1].position.z = PartEditorCameraPosPc.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);

    if (edpart_copy_mode != 0) {
        edbitsDrawCube(PartEditorCameraPosPc.x, PartEditorCameraPosPc.y, PartEditorCameraPosPc.z, edpart_copy_size,
                       edpart_copy_size, edpart_copy_size, 0, 0, 0, 0, 0, 0xffffffff, edpart_mtl);
    } else {
        vector.x = 0.0f;
        vector.y = 0.375f;
        vector.z = 0.0f;
        NuVecRotateZ(&vector, &vector, edpart_emitrotz);
        NuVecRotateY(&vector, &vector, edpart_emitroty);
        NuVecRotateX(&vector, &vector, edpart_emitrotx);
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[0].position.x = PartEditorCameraPosPc.x;
        line[0].position.y = PartEditorCameraPosPc.y;
        line[0].position.z = PartEditorCameraPosPc.z;
        line[1].position.x = PartEditorCameraPosPc.x + vector.x;
        line[1].position.y = PartEditorCameraPosPc.y + vector.y;
        line[1].position.z = PartEditorCameraPosPc.z + vector.z;
        line[0].colour = 0xff0000ff;
        line[1].colour = 0xff0000ff;
        NuRndrLine3d(line, edpart_mtl, NULL);
        vector.x = 0.0f;
        vector.y = 0.375f;
        vector.z = 0.125f;
        NuVecRotateZ(&vector, &vector, edpart_emitrotz);
        NuVecRotateY(&vector, &vector, edpart_emitroty);
        NuVecRotateX(&vector, &vector, edpart_emitrotx);
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[0].position.x = PartEditorCameraPosPc.x + vector.x;
        line[0].position.y = PartEditorCameraPosPc.y + vector.y;
        line[0].position.z = PartEditorCameraPosPc.z + vector.z;
        line[0].colour = 0xff0000ff;
        NuRndrLine3d(line, edpart_mtl, NULL);
        if (edpart_dpad_mode == 3) {
            vector.x = 0.0f;
            vector.y = 0.25f;
            vector.z = 0.0f;
            NuVecRotateZ(&vector, &vector, edpart_refrotz);
            NuVecRotateY(&vector, &vector, edpart_refroty);
            NuVecRotateZ(&vector, &vector, rotation_z);
            NuVecRotateY(&vector, &vector, rotation_y);
            line[0].position.x = PartEditorCameraPosPc.x;
            line[0].position.y = PartEditorCameraPosPc.y;
            line[0].position.z = PartEditorCameraPosPc.z;
            line[1].position.x = PartEditorCameraPosPc.x + vector.x;
            line[1].position.y = PartEditorCameraPosPc.y + vector.y;
            line[1].position.z = PartEditorCameraPosPc.z + vector.z;
            line[0].colour = 0xffff0000;
            line[1].colour = 0xffff0000;
            NuRndrLine3d(line, edpart_mtl, NULL);
            if (edpart_nearest != -1) {
                vector.x = 0.0f;
                vector.y = *(f32 *)(&part_emits[edpart_nearest].trailing_state_words[4]);
                vector.z = 0.0f;
                NuVecRotateZ(&vector, &vector, edpart_refrotz);
                NuVecRotateY(&vector, &vector, edpart_refroty);
                NuVecRotateZ(&vector, &vector, rotation_z);
                NuVecRotateY(&vector, &vector, rotation_y);
                edbitsDrawCube(PartEditorCameraPosPc.x + vector.x, PartEditorCameraPosPc.y + vector.y,
                               PartEditorCameraPosPc.z + vector.z, 0.25f, 0.0f, 0.25f, edpart_refrotz, edpart_refroty,
                               0, rotation_z, rotation_y, 0xffff0000, edpart_mtl);
            }
        }
    }

    NuRndrRect2di(0x1720, 0x9b0, 0xf00, 0x410, 0x80808080, edpart_boxmtl);
    NuRndrRect2di(0x1710, 0x8f0, 0xf20, 0xc0, 0x80000000, edpart_boxmtl);
    NuRndrLine2di(0x1710, 0x9b0, 0x1710, 0xdc8, 0x80000000, edpart_boxmtl);
    NuRndrLine2di(0x2630, 0x9b0, 0x2630, 0xdc8, 0x80000000, edpart_boxmtl);
    NuRndrLine2di(0x1710, 0xdc8, 0x2630, 0xdc8, 0x80000000, edpart_boxmtl);
    NuQFntPushPrintMode(2);
    NuQFntPushCoordinateSystem(NUQFNT_CSMODE_PS2);
    NuQFntSet(system_qfont);
    NuQFntSetColour(system_qfont, 0x80808080);
    NuQFntPrintEx(system_qfont, 0x17c0, 0x988, 0x10, "Info Box");
    if (edpart_readout == 0) {
        if (edpart_num_orphans > 0) {
            NuQFntSetColour(system_qfont, 0x80202080);
            NuQFntPrintEx(system_qfont, 0x17c0, 0x988, 0x10, "WARNING: %d Orphans", edpart_num_orphans);
        } else {
            NuQFntSetColour(system_qfont, 0x80808080);
            NuQFntPrintEx(system_qfont, 0x17c0, 0x988, 0x10, "Info Box");
        }
    } else if (edpart_readout == 1) {
        NuQFntSetColour(system_qfont, 0x80808080);
        NuQFntPrintEx(system_qfont, 0x17c0, 0x988, 0x10, "Co-ordinates");
    }
    NuQFntSetColour(system_qfont, 0x80000000);
    if (edpart_readout == 0) {
        if (edpart_copy_mode == 0) {
            if (edpart_create_type == -1)
                NuQFntPrintEx(system_qfont, 0x17c0, 0xa50, 0x10, "Current Type: <none>");
            else
                NuQFntPrintEx(system_qfont, 0x17c0, 0xa50, 0x10, "Current Type: %s",
                              part_types[edpart_create_type].name);
            if (edpart_create_type == -1)
                NuQFntPrintEx(system_qfont, 0x17c0, 0xaf0, 0x10, "Current List: <none>");
            else if (edpart_effect_list == 1)
                NuQFntPrintEx(system_qfont, 0x17c0, 0xaf0, 0x10, "Current List: Level");
            else if (edpart_effect_list == 0)
                NuQFntPrintEx(system_qfont, 0x17c0, 0xaf0, 0x10, "Current List: General");
        }
        if (edpart_copy_mode != 0) {
            if (edpart_copy_enclosed > 8)
                NuQFntSetColour(system_qfont, 0x80000080);
            NuQFntPrintEx(system_qfont, 0x17c0, 0xc30, 0x10, "Enclosed: %d", edpart_copy_enclosed);
            NuQFntSetColour(system_qfont, 0x80000000);
        } else {
            if (edpart_nearest == -1) {
                NuQFntPrintEx(system_qfont, 0x17c0, 0xc30, 0x10, "Highlight: <none>");
            } else {
                edbitsDrawCube(PartEditorCameraPosPc.x, PartEditorCameraPosPc.y, PartEditorCameraPosPc.z,
                               edpart_nearest_type->position_random.x, edpart_nearest_type->position_random.y,
                               edpart_nearest_type->position_random.z, edpart_emitrotz, edpart_emitroty,
                               edpart_emitrotx, rotation_z, rotation_y, 0xff0000ff, edpart_mtl);
                NuQFntSet(system_qfont);
                NuQFntSetColour(system_qfont, 0x80000000);
                NuQFntPrintEx(system_qfont, 0x17c0, 0xc30, 0x10, "Highlight: %s", edpart_nearest_type->name);
                if (edpart_nearest_orphans == 0 && edpart_nearest_duplicates == 0) {
                    NuQFntPrintEx(system_qfont, 0x1860, 0xcd0, 0x10, "Instances: %d of %d",
                                  edpart_nearest_type->variant_count, 8);
                } else {
                    NuQFntSetColour(system_qfont, 0x80000080);
                    NuQFntPrintEx(system_qfont, 0x1860, 0xcd0, 0x10, "Instances: %d of %d (Or = %d, Du = %d)",
                                  edpart_nearest_type->variant_count, 8, edpart_nearest_orphans,
                                  edpart_nearest_duplicates);
                    NuQFntSetColour(system_qfont, 0x80000000);
                }
            }
        }
        NuQFntPrintEx(system_qfont, 0x17c0, 0xd70, 0x10, "Types: %d/%d; Emits: %d/%d", part_types_used, 128,
                      part_emits_used, 40);
    } else if (edpart_readout == 1) {
        if (edpart_nearest == -1) {
            NuQFntPrintEx(system_qfont, 0x17c0, 0xa50, 0x10, "Highlight: <none>");
        } else {
            NuQFntPrintEx(system_qfont, 0x17c0, 0xa50, 0x10, "Highlight: %s", edpart_nearest_type->name);
            NuQFntPrintEx(system_qfont, 0x17c0, 0xaf0, 0x10, "XYZ: %0.2f %0.2f %0.2f", edpart_nearest_emit->position.x,
                          edpart_nearest_emit->position.y, edpart_nearest_emit->position.z);
            NuQFntPrintEx(system_qfont, 0x17c0, 0xb90, 0x10, "RotZ: %d",
                          ((const i16 *)&edpart_nearest_emit->trailing_state_words[0])[0]);
            NuQFntPrintEx(system_qfont, 0x17c0, 0xc30, 0x10, "RotY: %d",
                          ((const i16 *)&edpart_nearest_emit->trailing_state_words[0])[1]);
            NuQFntPrintEx(system_qfont, 0x17c0, 0xcd0, 0x10, "EmitRotZ: %d", edpart_nearest_emit->rotation_2c);
            NuQFntPrintEx(system_qfont, 0x17c0, 0xd70, 0x10, "EmitRotY: %d", edpart_nearest_emit->rotation_2e);
        }
    }
    NuQFntPopPrintMode();
    NuQFntPopCoordinateSystem();
}

STATIC_ASSERT(offsetof(PART_PC, field_e0) == 0xe0);
STATIC_ASSERT(offsetof(PART_PC, gravity) == 0xe8);
STATIC_ASSERT(offsetof(PART_PC, field_f4) == 0xf4);
STATIC_ASSERT(offsetof(PART_PC, time_step) == 0xf8);

static i32 PartRaycastsEnabledPrivate = 1;
extern i16 PartPlatformImpactListPc[4];
extern f32 PartCollisionIntervalPc, PartStopSpeedSquaredPc;
extern void __cdecl DebrisStatusAlwaysOff(i32 *handle);
extern i32 __cdecl AddScaledVariableShotDebrisEffect1(i32 effect, NUVEC *position, i32 count, f32 time, i16 z, i16 y,
                                                      NUMTX *orientation, f32 scale);

static void PartRuntimeMatrixCopyPrivate(NUMTX *destination, NUMTX *source) {
    destination->m00 = source->m00;
    destination->m01 = source->m01;
    destination->m02 = source->m02;
    destination->m03 = source->m03;
    destination->m10 = source->m10;
    destination->m11 = source->m11;
    destination->m12 = source->m12;
    destination->m13 = source->m13;
    destination->m20 = source->m20;
    destination->m21 = source->m21;
    destination->m22 = source->m22;
    destination->m23 = source->m23;
    destination->m30 = source->m30;
    destination->m31 = source->m31;
    destination->m32 = source->m32;
    destination->m33 = source->m33;
}
static void PartRuntimeDirectionalLightsPrivate(NUVEC *direction0, NUVEC *colour0, NUVEC *direction1, NUVEC *colour1,
                                                NUVEC *direction2, NUVEC *colour2) {
    PCGlobalLightingState.direction[0] = *direction0;
    PCGlobalLightingState.direction[1] = *direction1;
    PCGlobalLightingState.direction[2] = *direction2;
    PCGlobalLightingState.intensity[0] = *colour0;
    PCGlobalLightingState.intensity[1] = *colour1;
    PCGlobalLightingState.intensity[2] = *colour2;
    NuRndrSetDirectionalLightsPS(direction0, colour0, direction1, colour1, direction2, colour2);
}
void __cdecl DrawParts(i32 keep_offscreen) {
    PART_PC *part = Part;
    i32 i, drawn;
    NUMTX scaled, *matrix;
    NUVEC scale;
    for (i = 0; i < MAXPARTS; ++i, ++part) {
        if ((part->active & 1) == 0 || (part->flags & 0x1000) != 0)
            continue;
        if (part->draw_fn != NULL && part->draw_fn(part) == 0)
            continue;
        if ((part->active & 8) != 0) {
            PartRuntimeDirectionalLightsPrivate(&part->lighting[3], &part->lighting[0], &part->lighting[4],
                                                &part->lighting[1], &part->lighting[5], &part->lighting[2]);
            PCGlobalLightingState.ambient = part->lighting[6];
            NuRndrSetAmbientLightPS(&part->lighting[6]);
        } else if ((part->active & 0x2000) != 0) {
            PCGlobalLightingState.ambient = part->lighting[6];
            NuRndrSetAmbientLightPS(&part->lighting[6]);
        }
        part->active &= ~0x200;
        matrix = &part->transform;
        if (part->field_214 != 1.0f) {
            PartRuntimeMatrixCopyPrivate(&scaled, matrix);
            scale.x = part->field_214;
            scale.y = part->field_214;
            scale.z = part->field_214;
            NuMtxPreScale(&scaled, &scale);
            matrix = &scaled;
        }
        drawn = 0;
        if (part->source_special != NULL && NuSpecialDrawAt(&part->special, matrix) != 0) {
            part->active |= 0x600;
            drawn = 1;
        }
        if ((drawn | keep_offscreen) == 0 && ((part->flags & 0x10) == 0 || (part->active & 0x100) != 0))
            KillPart(part, 0);
    }
}

void AddScaledVariableShotPARTEffect(i32 effect, NUVEC *position, f32 rate, f32 time, NUMTX *orientation, f32 scale) {
    i32 scaled = CreateScaledPARTEffect(effect, scale);
    if (scaled != -1)
        AddVariableShotPARTEffect(scaled, position, rate, time, orientation);
}

i32 edpartProc(float delta_time, FRAME_PAD_PC *pad) {
    edpart_count += 5;
    if (edpart_active_menu) {
        eduiMenuProcess(edpart_active_menu, delta_time, pad);
        return 0;
    }
    edpartDoInput(pad);
    edpartDetermineNearest(1.0f);
    return (pad->digital_buttons_pressed >> 11) & 1;
}

void UpdateParts(f32 time) {
    i32 key, i, axis, angle;
    PART_PC *parts, *part;
    PART_TERRAIN_STATE_PC *state;
    NUVEC direction;
    NUMTX impact_orientation;
    NUMTX movement_orientation;
    NUMTX emitter_orientation;
    partglobaltime += time;
    if (partglobaltime > 900.0f)
        PartTimeSlip();
    PartCleanupTypes();
    part_platimpactcnt = 0;
    UpdatePartEmits(time);
    parts = Part;
    for (i = 0; i < MAXPARTS; ++i) {
        part = &parts[i];
        state = (PART_TERRAIN_STATE_PC *)part;
        if ((part->active & 1) == 0)
            continue;
        part->previous_transform.m00 = part->transform.m00;
        part->previous_transform.m01 = part->transform.m01;
        part->previous_transform.m02 = part->transform.m02;
        part->previous_transform.m03 = part->transform.m03;
        part->previous_transform.m10 = part->transform.m10;
        part->previous_transform.m11 = part->transform.m11;
        part->previous_transform.m12 = part->transform.m12;
        part->previous_transform.m13 = part->transform.m13;
        part->previous_transform.m20 = part->transform.m20;
        part->previous_transform.m21 = part->transform.m21;
        part->previous_transform.m22 = part->transform.m22;
        part->previous_transform.m23 = part->transform.m23;
        part->previous_transform.m30 = part->transform.m30;
        part->previous_transform.m31 = part->transform.m31;
        part->previous_transform.m32 = part->transform.m32;
        part->previous_transform.m33 = part->transform.m33;
        part->scale_time += time;
        if ((part->active & 2) == 0) {
            f32 movement_time = time;
            if ((part->flags & 0x400) == 0) {
                if (time >= part->field_f4) {
                    part->active = (part->active & ~0x60) | (part->radius > part->field_e0 ? 0x20 : 0);
                    if ((part->active & 4) != 0) {
                        if ((part->flags & 0x80000) != 0 && (part->active & 0x10) != 0 && part_platimpactcnt < 4) {
                            PartPlatformImpactListPc[part_platimpactcnt++] = state->platform_id;
                        }
                        if ((part->flags & 0x100) != 0) {
                            part->active |= 2;
                            if (part->stop_fn != NULL)
                                part->stop_fn(part);
                            if ((part->flags & 1) != 0 || (part->active & 0x10) != 0)
                                KillPart(part, 0);
                        } else {
                            movement_time -= part->field_f4;
                            part->position = part->impact_position;
                            FullReflect(&part->impact_normal, &part->velocity, &part->velocity);
                            if ((part->flags & 0x80) != 0)
                                NewPartRotation(part);
                            if ((part->flags & 0x100000) != 0)
                                NewPartOrderedRotation(part);
                            part->velocity.x *= part->field_ec;
                            part->velocity.z *= part->field_ec;
                            if (part->impact_normal.y > 0.5f) {
                                part->velocity.y *= part->field_ec;
                                if (part->velocity.x * part->velocity.x + part->velocity.y * part->velocity.y +
                                        part->velocity.z * part->velocity.z <
                                    PartStopSpeedSquaredPc) {
                                    part->active |= 2;
                                    if (part->stop_fn != NULL)
                                        part->stop_fn(part);
                                    if ((part->flags & 1) != 0 || (part->active & 0x10) != 0 ||
                                        (part->active & 0x100) != 0)
                                        KillPart(part, 0);
                                } else if ((part->active & 0x100) != 0) {
                                    KillPart(part, 0);
                                }
                            }
                        }
                        if (part->effect_id_1f0 != -1) {
                            key = -1;
                            PartRuntimeMatrixCopyPrivate(&impact_orientation, (NUMTX *)&ANI3_EVAL_IDENTITY_MATRIX);
                            NuMtxAlignY(&impact_orientation, &part->impact_normal);
                            AddScaledFiniteShotDebrisEffect(&key, part->effect_id_1f0, &part->impact_position, NULL,
                                                            NULL, 1, part->effect_scale);
                            DebrisOrientationMtx(key, &impact_orientation);
                        }
                        if (part->effect_id_1f8 != -1)
                            AddFiniteShotPART(part->effect_id_1f8, &part->impact_position, 1);
                        if (part->impact_fn != NULL)
                            part->impact_fn(part);
                    }
                    if (PartRaycastsEnabledPrivate != 0)
                        CastPart(part, PartCollisionIntervalPc);
                    if ((part->active & 4) != 0 && movement_time >= part->field_f4)
                        movement_time = 0.0f;
                }
                part->field_f4 -= movement_time;
            }
            if (movement_time > 0.0f) {
                if (part->move_fn != NULL) {
                    part->move_fn(part, movement_time);
                } else {
                    part->position.x += part->velocity.x * movement_time;
                    part->position.y += part->velocity.y * movement_time;
                    part->position.z += part->velocity.z * movement_time;
                    part->velocity.y += part->gravity * movement_time;
                }
            }
            if ((part->active & 0x100) == 0 && part->field_f0 != 2000000.0f && part->position.y < part->field_f0)
                part->active |= 0x100;
            part->collision_min.x = part->position.x - part->radius;
            part->collision_max.x = part->position.x + part->radius;
            part->collision_min.y = part->position.y - part->radius;
            part->collision_max.y = part->position.y + part->radius;
            part->collision_min.z = part->position.z - part->radius;
            part->collision_max.z = part->position.z + part->radius;
            if ((part->active & 0x60) == 0x20) {
                part->time_step += time;
                part->field_e0 = part->time_step / PARTSCALEUPTIME * part->radius;
                if (part->field_e0 > part->radius) {
                    part->field_e0 = part->radius;
                    part->active &= ~0x60;
                }
            } else if ((part->active & 0x60) == 0x40) {
                part->time_step -= time;
                if (part->time_step <= 0.0f)
                    KillPart(part, 0);
                else
                    part->field_e0 = part->time_step / PARTSCALEUPTIME * part->radius;
            }
            if ((part->flags & 0x80) != 0) {
                angle = (i32)((f64)part->field_124[3] * time);
                switch (part->rotation_axes[0]) {
                    case 1:
                        NuMtxPreRotateX(&part->transform, angle);
                        break;
                    case 2:
                        NuMtxPreRotateY(&part->transform, angle);
                        break;
                    case 3:
                        NuMtxPreRotateZ(&part->transform, angle);
                        break;
                }
                angle = (i32)((f64)part->field_124[4] * time);
                switch (part->rotation_axes[1]) {
                    case 1:
                        NuMtxPreRotateX(&part->transform, angle);
                        break;
                    case 2:
                        NuMtxPreRotateY(&part->transform, angle);
                        break;
                    case 3:
                        NuMtxPreRotateZ(&part->transform, angle);
                        break;
                }
            }
            if ((part->flags & 0x100000) != 0) {
                if (part->field_124[0] != 0)
                    NuMtxPreRotateX(&part->transform, (i32)((f64)part->field_124[0] * time));
                if (part->field_124[1] != 0)
                    NuMtxPreRotateY(&part->transform, (i32)((f64)part->field_124[1] * time));
                if (part->field_124[2] != 0)
                    NuMtxPreRotateZ(&part->transform, (i32)((f64)part->field_124[2] * time));
            }
            if ((part->flags & 0x800) != 0) {
                direction.x = -part->velocity.x;
                direction.y = -part->velocity.y;
                direction.z = -part->velocity.z;
                movement_orientation = ANI3_EVAL_IDENTITY_MATRIX;
                NuMtxLookAtZ(&movement_orientation, &direction);
                memmove(&part->transform, &movement_orientation, 0x30);
            }
        }
        if ((part->active & 1) != 0) {
            if ((part->flags & 2) != 0 && part->collision_fn != NULL)
                part->collision_fn(part);
            if ((part->active & 1) != 0) {
                if (part->update_fn != NULL)
                    part->update_fn(part);
                if (part->time_remaining > 0.0f) {
                    part->time_remaining -= time;
                    if (part->time_remaining <= 0.0f)
                        KillPart(part, 0);
                }
            }
        }
        if ((part->flags & 0x20000) != 0 && part->debris_handle != NULL) {
            emitter_orientation = ANI3_EVAL_IDENTITY_MATRIX;
            direction = part->velocity;
            direction.x = -direction.x;
            direction.y = -direction.y;
            direction.z = -direction.z;
            NuMtxLookAtY(&emitter_orientation, &direction);
            DebrisOrientationMtx(*part->debris_handle, &emitter_orientation);
            DebrisEmitterPos(*part->debris_handle, part->position.x, part->position.y, part->position.z);
            if (((part->flags & 0x200000) != 0 && (part->active & 2) != 0) || (part->flags & 0x1000) != 0)
                DebrisStatusAlwaysOff(part->debris_handle);
        }
        if (time != 0.0f) {
            if ((part->flags & 0x40000) != 0 && part->effect_id_1e0 != -1 &&
                !((part->flags & 0x200000) != 0 && (part->active & 2) != 0) && (part->flags & 0x1000) == 0)
                AddScaledVariableShotDebrisEffect1(part->effect_id_1e0, &part->position, (i32)(part->field_1e8), time,
                                                   0, 0, NULL, part->effect_scale);
            if ((part->flags & 0x40000) != 0 && part->effect_id_1e4 != -1 &&
                !((part->flags & 0x200000) != 0 && (part->active & 2) != 0) && (part->flags & 0x1000) == 0)
                AddScaledVariableShotDebrisEffect1(part->effect_id_1e4, &part->position, (i32)(part->field_1ec), time,
                                                   0, 0, NULL, part->effect_scale);
        }
        if (part->lighting_handle != NULL) {
            if ((part->flags & 0x1000) != 0) {
                direction.x = direction.y = direction.z = 10000.0f;
                rtlDynamicSetPos(*part->lighting_handle, &direction);
            } else {
                rtlDynamicSetPos(*part->lighting_handle, &part->position);
            }
        }
        if (part->callback_1ac != NULL)
            part->callback_1ac(part);
        part->field_208 = 0;
        if ((part->active & 1) == 0) {
            KillPart(part, 0);
            memset(part, 0, sizeof(PART_PC));
        }
    }
}

i32 __cdecl AddScaledVariableShotDebrisEffect1(i32 effect, NUVEC *position, i32 count, f32 time, i16 z_rotation,
                                               i16 y_rotation, NUMTX *orientation, f32 scale) {
    i32 scaled = CreateScaledEffect(effect, scale);
    if (scaled != -1)
        AddVariableShotDebrisEffectTimed1(scaled, position, count, time, z_rotation, y_rotation, orientation);
    return scaled;
}

void __cdecl NuPartEnableRayCasts(i32 enabled) {
    PartRaycastsEnabledPrivate = enabled;
}

STATIC_ASSERT(offsetof(PART_PC, field_ec) == 0xec);
STATIC_ASSERT(offsetof(PART_PC, field_f0) == 0xf0);
STATIC_ASSERT(offsetof(PART_PC, field_f4) == 0xf4);
STATIC_ASSERT(offsetof(PART_PC, time_step) == 0xf8);
STATIC_ASSERT(offsetof(PART_PC, scale_time) == 0xfc);
STATIC_ASSERT(offsetof(PART_PC, field_124) == 0x124);
STATIC_ASSERT(offsetof(PART_PC, callback_1ac) == 0x1ac);
STATIC_ASSERT(offsetof(PART_PC, impact_fn) == 0x1b8);
STATIC_ASSERT(offsetof(PART_PC, collision_fn) == 0x1bc);
STATIC_ASSERT(offsetof(PART_PC, update_fn) == 0x1c4);
STATIC_ASSERT(offsetof(PART_PC, stop_fn) == 0x1c8);
STATIC_ASSERT(offsetof(PART_PC, lighting_handle) == 0x1dc);
STATIC_ASSERT(offsetof(PART_PC, effect_id_1e0) == 0x1e0);
STATIC_ASSERT(offsetof(PART_PC, effect_id_1e4) == 0x1e4);
STATIC_ASSERT(offsetof(PART_PC, field_1e8) == 0x1e8);
STATIC_ASSERT(offsetof(PART_PC, field_1ec) == 0x1ec);
STATIC_ASSERT(offsetof(PART_PC, effect_id_1f0) == 0x1f0);
STATIC_ASSERT(offsetof(PART_PC, effect_id_1f8) == 0x1f8);
STATIC_ASSERT(offsetof(PART_PC, field_208) == 0x208);
STATIC_ASSERT(offsetof(PART_PC, field_214) == 0x214);

void __cdecl edpartSetParticlePage(i32 page) {
    PartLoadParticlePagePc = page;
}

i32 edpartLookupObject(char *name) {
    if (edbits_base_scene == NULL)
        return -1;
    return edpartLookupObjectInScene(name, edbits_base_scene);
}
i32 __cdecl edpartLoadPage(char *path, i8 mode, NUGSCN *scene) {
    return edpartLoadPageEx(path, mode, &scene, 1);
}

i32 __cdecl FindPartDebris(PARTDEBRISSYS *system, char *name) {
    i32 index;
    if (system != NULL) {
        for (index = system->named_count; index < system->capacity; ++index) {
            if (NuStrICmp(name, system->entries[index].name) == 0)
                return system->entries[index].id;
        }
    }
    return -1;
}

void __cdecl DebrisSetup(VARIPTR *buffer, VARIPTR buffer_end, char *texture_name, i32 chunk_count, i32 key_count,
                         i32 effect_count) {
    DebrisSetup2(buffer, buffer_end, buffer, buffer_end, texture_name, chunk_count, 0x20, key_count, effect_count);
}

PART_PC *__cdecl FindIncomingPart(void *owner, NUVEC *position, f32 radius, u32 flags, f32 range) {
    f32 radius_squared = radius * radius;
    f32 nearest_distance = range > 0.0f ? range : 100.0f;
    PART_PC *nearest = NULL;
    PART_PC *part = Part;
    i32 i;
    for (i = 0; i < MAXPARTS; ++i, ++part) {
        if ((part->active & 1) == 0 || part->owner == owner || (part->flags & flags) != flags)
            continue;
        if (range > 0.0f) {
            f32 distance = NuVecDistSqr(&part->position, position, NULL);
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest = part;
            }
        } else {
            f32 speed = NuVecMag(&part->velocity);
            if (speed != 0.0f) {
                NUVEC direction;
                f32 reciprocal = 1.0f / speed;
                f32 half_speed;
                f32 distance;
                f32 limit;
                NuVecScale(&direction, &part->velocity, reciprocal);
                half_speed = (f32)((f64)speed * 0.5);
                distance = NuVecDistSqr(&part->position, position, NULL);
                limit = half_speed * half_speed;
                if (distance < limit &&
                    LineIntersectSphere(&part->position, &direction, position, radius_squared, NULL)) {
                    nearest_distance = distance;
                    nearest = part;
                }
            }
        }
    }
    return nearest;
}

PART_PC *__cdecl Part_FindFromHSpecial(NUSPECIALHANDLE *special) {
    PART_PC *part;
    i32 index;
    if (special == NULL)
        return NULL;
    part = Part;
    for (index = 0; index < MAXPARTS; index++, part++) {
        if ((part->active & 1) != 0 && NuSpecialCompare(&part->special, special))
            return part;
    }
    return NULL;
}
