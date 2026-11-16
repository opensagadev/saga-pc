#include <stdio.h>
#include "batman.h"
#include "gameapi/apisave.h"
#include "gameapi/edpart.h"
#include "gameapi/edptl.h"
#include "gameapi/edtools/edanim_internal.h"
#include "gameapi/edtools/edcam.h"
#include "gameapi/edtools/edfile.h"
#include "gameapi/edtools/edgra.h"
#include "gameapi/edtools/edpart_internal.h"
#include "gameapi/edtools/edpp_internal.h"
#include "gameapi/edtools/edrender.h"
#include "gameapi/edtools/edstubs.h"
#include "gameapi/edtools/edui.h"
#include "gameapi/part.h"
#include "globals.h"
#include "legoapi/characters/motion.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/rtl.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/render/light/lighting.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episodeIII.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "legogame/game.h"
#include "nu2api/nu3d/glutils.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuportal.h"
#include "nu2api/nu3d/nuptl.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nutimebar_plain.h"
#include "nu2api/nualloc.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nudynamiclight_gen.h"
#include "nu2api/nuerror_gen.h"
#include "nu2api/nugscn_gen.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic/sfx.h"
#include "nu2api/nustencilshadows_PC.h"
#include <string.h>
#include <float.h>

static i32 PartVectorSpreadDegrees = 30;
i32 edpart_set_part = 5;
i32 edpart_create_type = -1;
f32 edpart_copy_size = 0.2f;
f32 edpart_scale_factor = 1.0f;
i32 edpart_load_particle_page = -1;
i32 edpart_superscale = 1;
char PartNullInstanceName[14] = "NULL instance";
i32 MAXPARTS = 32;
ADDPART Default_ADDPART = {
    0,                                    // matrix
    0,                                    // position
    0,                                    // velocity
    0,                                    // owner
    0,                                    // recipient
    0.1f,                                 // unknown_14
    0.1f,                                 // unknown_18
    -5.0f,                                // gravity
    0.75f,                                // unknown_20
    0,                                    // special
    -1,                                   // part_id
    0x00000000u,                          // flags
    0,                                    // callback_30
    0,                                    // callback_34
    0,                                    // callback_38
    0,                                    // callback_3c
    0,                                    // callback_40
    0,                                    // callback_44
    0,                                    // callback_48
    0,                                    // callback_4c
    0,                                    // callback_50
    0,                                    // callback_54
    -1,                                   // effect_id_58
    0,                                    // debris_handle
    -1,                                   // effect_id_60
    -1,                                   // effect_id_64
    60.0f,                                // unknown_68
    60.0f,                                // unknown_6c
    -1,                                   // effect_id_70
    -1,                                   // effect_id_74
    -1,                                   // effect_id_78
    1.0f,                                 // effect_scale
    0,                                    // lighting_handle
    -1,                                   // lighting_id
    2000000.0f,                           // unknown_88
    0.016666668f,                         // time_step
    0xffffffffu,                          // hostility_group
    0,                                    // lighting_data
    1.0f,                                 // lighting_scale_x
    1.0f,                                 // lighting_scale_y
    1.0f,                                 // lighting_scale_z
    0.0f,                                 // unknown_a4
    {0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u}, // unknown_a8
    1.0f,                                 // unknown_c0
    0,                                    // unknown_c4
};
i32 part_slot_cursor = -1;
f32 PartCollisionInterval = 0.1f;
f32 PartStopSpeedSquared = 0.1f;
f32 PARTSCALEUPTIME = 0.5f;
u32 partseed = 0x5c0999;

void InitParts(i32 count, VARIPTR *buffer, VARIPTR end) {
    i32 i;
    i32 j;
    MAXPARTS = count;
    buffer->addr = (buffer->addr + 15) & ~(u32)15;
    Part = buffer->void_ptr;
    buffer->addr += MAXPARTS * sizeof(PART);
    if (Part != NULL) {
        memset(Part, 0, MAXPARTS * sizeof(PART));
    }
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
    for (i = 0; i < 40; ++i) {
        part_emits[i].effect_id = -1;
    }
    for (i = 0; i < 32; ++i) {
        part_scene_pageid[i] = -1;
    }
    part_emits_used = 0;
    for (i = 0; i < 8; ++i) {
        part_page_used[i] = 0;
        part_page_on[i] = 0;
    }
    for (i = 0; i < 32; ++i) {
        part_scene[i] = NULL;
    }
    part_platimpactcnt = 0;
}

void KillPartsByScene(NUGSCN *scene) {
    PART *part = Part;
    i32 index;
    for (index = 0; index < MAXPARTS; ++index, ++part) {
        if ((part->active & PART_IN_USE) != 0 && part->source_special != NULL && part->special.scene == scene) {
            nuhspecial_s *special = &part->special;
            special->scene = NULL;
            special->special = NULL;
            special->display_special = NULL;
            part->active &= ~PART_IN_USE;
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
    if (menu->callback) {
        menu->callback(menu, parent);
    }
    edpart_create_type = -1;
}

void edpartClearPage(i8 page) {
    i32 index;
    NuThreadDisableThreadSwap();
    if (part_page_on[page] != 0) {
        part_page_on[page] = 0;
    }
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

i32 PARTLookupTypePageOnly(char *name, i32 page) {
    i32 i;
    if (name == NULL || name[0] == 0) {
        return -1;
    }
    if ((u32)(page - 1) <= 6) {
        for (i = 0; i < 128; ++i) {
            if (part_types[i].page == page && NuStrNICmp(name, part_types[i].name, 16) == 0) {
                return i;
            }
        }
    }
    for (i = 0; i < 128; ++i) {
        if (part_types[i].page == 0 && NuStrNCmp(name, part_types[i].name, 16) == 0) {
            return i;
        }
    }
    return -1;
}

i32 GetPartCount(void) {
    i32 n;
    i32 used = 0;
    for (n = 0; n < 40; ++n) {
        if (part_emits[n].effect_id != -1) {
            ++used;
        }
    }
    return used;
}

i32 GetMaxPartTypes(void) {
    return 128;
}

char *GetPartName(i32 type) {
    if (part_types[type].name[0] == 0) {
        return NULL;
    }
    return part_types[type].name;
}

void fun_0064b150(void) {
}

f32 PARTGetTotalOnTime(i32 type) {
    if (type >= 0) {
        return part_types[type].emission_period + part_types[type].emission_period_random;
    }
    return 0.0f;
}

f32 PARTGetTotalOffTime(i32 type) {
    if (type >= 0) {
        return part_types[type].emission_pause + part_types[type].emission_pause_random;
    }
    return 0.0f;
}

void KillPart(PART *part, i32 reason) {
    if ((part->active & PART_IN_USE) == 0) {
        return;
    }
    part->active &= ~PART_IN_USE;
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
        AddScaledFiniteShotDebrisEffect(&handle, part->effect_id_1f4, &part->position, NULL, NULL, 1, part->effect_scale);
    }
}

void NewPartRotation(PART *part) {
    i32 index;
    for (index = 0; index < 2; ++index) {
        do {
            part->rotation_axes[index] = (u8)((NuRandIntSeeded(&partseed) % 65535u) / 21845u + 1);
        } while (index == 1 && part->rotation_axes[1] == part->rotation_axes[0]);
        part->unknown_124[index + 3] = (i32)(((f64)NuRandFloatSeeded(&partseed) * 2.0 - 1.0) * 65536.0);
    }
}

void NewPartOrderedRotation(PART *part) {
    part->unknown_124[0] =
        part->rotation_parameters[0] + (i32)(((f64)NuRandFloatSeeded(&partseed) * 2.0 - 1.0) * part->rotation_parameters[3]);
    part->unknown_124[1] =
        part->rotation_parameters[1] + (i32)(((f64)NuRandFloatSeeded(&partseed) * 2.0 - 1.0) * part->rotation_parameters[4]);
    part->unknown_124[2] =
        part->rotation_parameters[2] + (i32)(((f64)NuRandFloatSeeded(&partseed) * 2.0 - 1.0) * part->rotation_parameters[5]);
}

PART *AddPart(ADDPART *parameters) {
    NUVEC forward = {0.0f, 0.0f, -1.0f};
    PART *part;
    i32 scanned;
    i32 index;
    u8 hostility_group;

    if (parameters->special == NULL && parameters->part_id != -1 && parameters->part_id != 9999) {
        return NULL;
    }
    if (parameters->special != NULL && NuSpecialExistsFn(parameters->special) == 0) {
        return NULL;
    }
    scanned = 0;
    while ((Part[part_slot_cursor].active & PART_IN_USE) != 0 && (Part[part_slot_cursor].flags & PART_FLAG_SLOT_CANNOT_BE_STOLEN) != 0 &&
           scanned < MAXPARTS) {
        if (++part_slot_cursor == MAXPARTS) {
            part_slot_cursor = 0;
        }
        ++scanned;
    }
    if (scanned == MAXPARTS) {
        return NULL;
    }
    part = &Part[part_slot_cursor];
    if ((part->active & PART_IN_USE) != 0) {
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
        NuVecRotateY(&forward, &forward, -part->rotation_y);
        part->rotation_x = (u16)-NuAtan2D(forward.y, forward.z);
    } else if (parameters->position != NULL) {
        NuMtxSetTranslation(&part->transform, parameters->position);
    } else {
        return NULL;
    }
    part->previous_position = part->position;
    part->owner = parameters->owner;
    part->recipient = parameters->recipient;
    part->velocity = *parameters->velocity;
    part->time_remaining = part->initial_time = parameters->unknown_a4;
    part->unknown_206 = -1;
    part->unknown_20b = -1;
    part->scale_time = 0.0f;
    part->time_step = parameters->time_step;
    part->active = (part->active & ~PART_STATE_SCALING_DOWN) | PART_STATE_SCALING_UP;
    part->radius = parameters->unknown_14;
    part->unknown_e0 = (part->time_step / PARTSCALEUPTIME) * part->radius;
    part->unknown_e4 = parameters->unknown_18;
    part->unknown_214 = parameters->unknown_c0;
    part->gravity = parameters->gravity;
    part->field_ec = parameters->unknown_20;
    part->unknown_f0 = parameters->unknown_88;
    part->part_id = (i16)parameters->part_id;
    part->active = (part->active & ~PART_STATE_STOPPED) | PART_IN_USE;
    part->flags = parameters->flags;
    part->flicker_flags = parameters->unknown_c4;
    part->active &= ~PART_STATE_BEEPED;
    if (parameters->lighting_data != NULL) {
        part->lighting[0] = parameters->lighting_data->values[0];
        part->lighting[3] = parameters->lighting_data->values[3];
        part->lighting[1] = parameters->lighting_data->values[1];
        part->lighting[4] = parameters->lighting_data->values[4];
        part->lighting[2] = parameters->lighting_data->values[2];
        part->lighting[5] = parameters->lighting_data->values[5];
        part->lighting[6] = parameters->lighting_data->values[6];
        part->active |= PART_STATE_LIT;
    } else {
        part->active &= ~PART_STATE_LIT;
        if (parameters->lighting_scale_x != 1.0f || parameters->lighting_scale_y != 1.0f || parameters->lighting_scale_z != 1.0f) {
            // lighting_scale_x/y/z are one NUVEC in the original (struct copy).
            part->lighting[6] = *(NUVEC *)&parameters->lighting_scale_x;
            part->active |= PART_STATE_LIGHT_SCALE;
        } else {
            part->active &= ~PART_STATE_LIGHT_SCALE;
        }
    }
    part->source_special = parameters->special;
    if (part->source_special != NULL) {
        part->special = *part->source_special;
    } else {
        memset(&part->special, 0, sizeof(part->special));
    }
    part->active &= ~0x14u;
    part->unknown_f4 = 0.0f;
    part->collision_min.x = part->position.x - part->radius;
    part->collision_max.x = part->position.x + part->radius;
    part->collision_min.y = part->position.y - part->radius;
    part->collision_max.y = part->position.y + part->radius;
    part->collision_min.z = part->position.z - part->radius;
    part->collision_max.z = part->position.z + part->radius;
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
    memcpy(part->unknown_10c, parameters->unknown_a8, sizeof(part->unknown_10c));
    for (index = 0; index < 5; ++index) {
        part->unknown_124[index] = 0;
    }
    if ((part->flags & PART_FLAG_ROTATE_RANDOMLY) != 0) {
        NewPartRotation(part);
    }
    if ((part->flags & PART_FLAG_ORDERED_ROTATE_RANDOMLY) != 0) {
        NewPartOrderedRotation(part);
    }
    if (parameters->effect_id_58 != -1) {
        i32 effect;
        part->inline_debris_handle = -1;
        part->debris_handle = &part->inline_debris_handle;
        if (parameters->effect_scale != 1.0f) {
            effect = CreateScaledEffect(parameters->effect_id_58, parameters->effect_scale);
        } else {
            effect = parameters->effect_id_58;
        }
        AddDebrisEffect(part->debris_handle, effect, part->position.x, part->position.y, part->position.z);
        DebrisPopulateInstance(*part->debris_handle, 0.0f);
    } else {
        part->debris_handle = parameters->debris_handle;
    }
    part->effect_id_1e0 = parameters->effect_id_60;
    part->unknown_1e8 = parameters->unknown_68;
    part->effect_id_1e4 = parameters->effect_id_64;
    part->unknown_1ec = parameters->unknown_6c;
    part->effect_id_1f0 = parameters->effect_id_70;
    part->effect_id_1f8 = parameters->effect_id_74;
    part->effect_id_1f4 = parameters->effect_id_78;
    part->effect_scale = parameters->effect_scale;
    part->previous_transform = part->transform;
    part->unknown_210 = 0;
    part->unknown_208 = 1;
    part->force_flags = 0;
    part->unknown_21c = 0;
    hostility_group = (u8)parameters->hostility_group;
    part->active |= 0x1800;
    part->hostility_group = hostility_group;
    part->unknown_204 = 1;
    if ((part->flags & PART_FLAG_REAL_TIME_LIGHTING) != 0 && parameters->lighting_handle != NULL && PartRTL != NULL) {
        part->lighting_handle = parameters->lighting_handle;
        *parameters->lighting_handle = rtlDynamicAllocTemplate((RTL_SET *)PartRTL, parameters->lighting_id);
    }
    if (++part_slot_cursor == MAXPARTS) {
        part_slot_cursor = 0;
    }
    return part;
}

PART *HitParts(GAMEOBJECT *excluded_owner, NUVEC *points, i32 point_count, f32 radius, NUVEC *minimum, NUVEC *maximum, u32 flags) {
    PART *part = Part;
    i32 index;

    for (index = 0; index < MAXPARTS; ++index, ++part) {
        i32 point;
        if ((part->active & PART_IN_USE) == 0 || part->owner == excluded_owner || (part->flags & flags) == 0) {
            continue;
        }
        if (maximum->x < part->collision_min.x || minimum->x > part->collision_max.x || maximum->z < part->collision_min.z ||
            minimum->z > part->collision_max.z || maximum->y < part->collision_min.y || minimum->y > part->collision_max.y) {
            continue;
        }
        for (point = point_count - 1; point >= 0; --point) {
            NUVEC difference;
            f32 combined_radius;
            NuVecSub(&difference, &points[point], &part->position);
            combined_radius = radius + part->radius;
            if (difference.x * difference.x + difference.y * difference.y + difference.z * difference.z <=
                combined_radius * combined_radius) {
                return part;
            }
        }
    }
    return NULL;
}

void edpartStartPage(u8 page) {
    i32 i;
    i8 signed_page = (i8)page;

    for (i = 0; i < 40; ++i) {
        if (part_emits[i].effect_id != -1 && part_emits[i].page == signed_page) {
            part_emits[i].camera_distance = 9999.9f;
        }
    }
    part_page_on[signed_page] = 1;
}

// noinline: the original calls edpartStopPage out of line from Parts_Stop; VC8 would inline it there.
__declspec(noinline) void edpartStopPage(u8 page) {
    part_page_on[(i8)page] = 0;
}

void edpartCancelInstanceFlagsMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_instanceflags_menu);
    edpart_instanceflags_menu = NULL;
}

extern struct eduimenu_s *edpart_debrisindex_menu;
extern struct eduimenu_s *edpart_partindex_menu;

void edpartChangeDebrisIndex(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type != NULL) {
        if (edpart_set_debris == 0) {
            edpart_nearest_type->trail_effects[0] = item->data;
        } else if (edpart_set_debris == 1) {
            edpart_nearest_type->trail_effects[1] = item->data;
        } else if (edpart_set_debris == 2) {
            edpart_nearest_type->attached_effect = item->data;
        } else if (edpart_set_debris == 3) {
            edpart_nearest_type->impact_effect = item->data;
        } else if (edpart_set_debris == 4) {
            edpart_nearest_type->kill_effect = item->data;
        }
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
    if (edpart_debrisindex_menu == NULL || edpart_nearest_type == NULL) {
        return;
    }
    chosen = -1;
    if (edpart_set_debris == 0) {
        chosen = edpart_nearest_type->trail_effects[0];
    } else if (edpart_set_debris == 1) {
        chosen = edpart_nearest_type->trail_effects[1];
    } else if (edpart_set_debris == 2) {
        chosen = edpart_nearest_type->attached_effect;
    } else if (edpart_set_debris == 3) {
        chosen = edpart_nearest_type->impact_effect;
    } else if (edpart_set_debris == 4) {
        chosen = edpart_nearest_type->kill_effect;
    }
    found = 0;
    for (index = 1; index < debinf_count; ++index) {
        if (debtab[index] != NULL && (i8)debtab[index]->category == edpart_particle_list) {
            i32 selected = chosen == index;
            eduiMenuAddItem(edpart_debrisindex_menu,
                            eduiItemCheckCreate(index, edblack, selected, 1, edpartChangeDebrisIndex, debtab[index]->name));
            if (selected == 1) {
                edpart_debrisindex_menu->selected = edui_last_item;
                found = selected;
            }
        }
    }
    eduiMenuSortItemsByTxt(edpart_debrisindex_menu);
    eduiMenuAddItemFirst(edpart_debrisindex_menu, eduiItemCheckCreate(-1, edblack, chosen == -1, 1, edpartChangeDebrisIndex, "None"));
    eduiMenuAttach(menu, edpart_debrisindex_menu);
    edpart_debrisindex_menu->x = menu->x + 10;
    edpart_debrisindex_menu->y = menu->y + 40;
    if (found) {
        edpart_debrisindex_menu->first_visible = edpart_debrisindex_menu->selected;
    }
}

void edpartChangePartIndex(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type != NULL && edpart_set_part == 5) {
        edpart_nearest_type->impact_part = item->data;
    }
}

void edpartCancelPartIndexMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_partindex_menu);
    edpart_partindex_menu = NULL;
}

void edpartPartIndexMenu(eduimenu_s *menu) {
    i32 chosen;
    i32 index;
    edpart_partindex_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelPartIndexMenu, "Part Type");
    if (edpart_partindex_menu == NULL || edpart_nearest_type == NULL) {
        return;
    }
    chosen = -1;
    if (edpart_set_part == 3) {
        chosen = edpart_nearest_type->impact_part;
    }
    eduiMenuAddItem(edpart_partindex_menu, eduiItemCheckCreate(-1, edblack, chosen == -1, 1, edpartChangePartIndex, "None"));
    for (index = 0; index < 128; ++index) {
        if (part_types[index].name[0] != '\0' && (i8)part_types[index].effect_list == edpart_part_list) {
            i32 selected = chosen == index;
            if (index == edpart_nearest_emit->effect_id) {
                eduiMenuAddItem(edpart_partindex_menu, eduiItemCheckCreate(index, edgrey, 0, 0, NULL, part_types[index].name));
            } else {
                eduiMenuAddItem(edpart_partindex_menu,
                                eduiItemCheckCreate(index, edblack, selected, 1, edpartChangePartIndex, part_types[index].name));
            }
            if (selected == 1) {
                edpart_partindex_menu->selected = edui_last_item;
            }
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
        if (edpart_set_debris == 0) {
            edpart_nearest_type->trail_rates[0] = (f32)(i32)((edui_slider_s *)item)->value;
        } else if (edpart_set_debris == 1) {
            edpart_nearest_type->trail_rates[1] = (f32)(i32)((edui_slider_s *)item)->value;
        }
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
    if (edpart_nearest_type == NULL) {
        return;
    }
    edpart_set_debris = 0;
    edpart_trail1debris_menu = eduiMenuCreate(70, 70, 300, 300, ed_fnt, edpartCancelTrail1DebrisMenu, "Trail 1 Debris");
    if (edpart_trail1debris_menu != NULL) {
        eduiMenuAddItem(edpart_trail1debris_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralDebrisIndexMenu, "General List..."));
        if (edpart_nearest_type->effect_list == 1) {
            eduiMenuAddItem(edpart_trail1debris_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartLevelDebrisIndexMenu, "Level List..."));
        } else {
            eduiMenuAddItem(edpart_trail1debris_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Level List..."));
        }
        eduiMenuAddItem(edpart_trail1debris_menu,
                        eduiItemSliderCreateInt(0, edblack, 0, edpartChangeDebrisPerSec, 0, ((i32)(edpart_superscale * 1200.0f)),
                                                ((i32)(edpart_nearest_type->trail_rates[0])), "Trail 1 particles per second"));
    }
    eduiMenuAttach(menu, edpart_trail1debris_menu);
    edpart_trail1debris_menu->x = menu->x + 10;
    edpart_trail1debris_menu->y = menu->y + 40;
}

void edpartTrail2DebrisMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type == NULL) {
        return;
    }
    edpart_set_debris = 1;
    edpart_trail2debris_menu = eduiMenuCreate(70, 70, 300, 300, ed_fnt, edpartCancelTrail2DebrisMenu, "Trail 2 Debris");
    if (edpart_trail2debris_menu != NULL) {
        eduiMenuAddItem(edpart_trail2debris_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralDebrisIndexMenu, "General List..."));
        if (edpart_nearest_type->effect_list == 1) {
            eduiMenuAddItem(edpart_trail2debris_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartLevelDebrisIndexMenu, "Level List..."));
        } else {
            eduiMenuAddItem(edpart_trail2debris_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Level List..."));
        }
        eduiMenuAddItem(edpart_trail2debris_menu,
                        eduiItemSliderCreateInt(0, edblack, 0, edpartChangeDebrisPerSec, 0, ((i32)(edpart_superscale * 1200.0f)),
                                                ((i32)(edpart_nearest_type->trail_rates[1])), "Trail 2 particles per second"));
    }
    eduiMenuAttach(menu, edpart_trail2debris_menu);
    edpart_trail2debris_menu->x = menu->x + 10;
    edpart_trail2debris_menu->y = menu->y + 40;
}

void edpartEmitterDebrisMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type == NULL) {
        return;
    }
    edpart_set_debris = 2;
    edpart_emitterdebris_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelEmitterDebrisMenu, "Emitter Debris");
    if (edpart_emitterdebris_menu != NULL) {
        eduiMenuAddItem(edpart_emitterdebris_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralDebrisIndexMenu, "General List..."));
        if (edpart_nearest_type->effect_list == 1) {
            eduiMenuAddItem(edpart_emitterdebris_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartLevelDebrisIndexMenu, "Level List..."));
        } else {
            eduiMenuAddItem(edpart_emitterdebris_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Level List..."));
        }
    }
    eduiMenuAttach(menu, edpart_emitterdebris_menu);
    edpart_emitterdebris_menu->x = menu->x + 10;
    edpart_emitterdebris_menu->y = menu->y + 40;
}

void edpartImpactDebrisMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type == NULL) {
        return;
    }
    edpart_set_debris = 3;
    edpart_impactdebris_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelImpactDebrisMenu, "Impact Debris");
    if (edpart_impactdebris_menu != NULL) {
        eduiMenuAddItem(edpart_impactdebris_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralDebrisIndexMenu, "General List..."));
        if (edpart_nearest_type->effect_list == 1) {
            eduiMenuAddItem(edpart_impactdebris_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartLevelDebrisIndexMenu, "Level List..."));
        } else {
            eduiMenuAddItem(edpart_impactdebris_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Level List..."));
        }
    }
    eduiMenuAttach(menu, edpart_impactdebris_menu);
    edpart_impactdebris_menu->x = menu->x + 10;
    edpart_impactdebris_menu->y = menu->y + 40;
}

void edpartImpactPartMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type == NULL) {
        return;
    }
    edpart_set_debris = 5;
    edpart_impactpart_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelImpactPartMenu, "Impact Part");
    if (edpart_impactpart_menu != NULL) {
        eduiMenuAddItem(edpart_impactpart_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralPartIndexMenu, "General List..."));
        if (edpart_nearest_type->effect_list == 1) {
            eduiMenuAddItem(edpart_impactpart_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartLevelPartIndexMenu, "Level List..."));
        } else {
            eduiMenuAddItem(edpart_impactpart_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Level List..."));
        }
    }
    eduiMenuAttach(menu, edpart_impactpart_menu);
    edpart_impactpart_menu->x = menu->x + 10;
    edpart_impactpart_menu->y = menu->y + 40;
}

void edpartDieDebrisMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type == NULL) {
        return;
    }
    edpart_set_debris = 4;
    edpart_diedebris_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelDieDebrisMenu, "Die Debris");
    if (edpart_diedebris_menu != NULL) {
        eduiMenuAddItem(edpart_diedebris_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartGeneralDebrisIndexMenu, "General List..."));
        if (edpart_nearest_type->effect_list == 1) {
            eduiMenuAddItem(edpart_diedebris_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartLevelDebrisIndexMenu, "Level List..."));
        } else {
            eduiMenuAddItem(edpart_diedebris_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Level List..."));
        }
    }
    eduiMenuAttach(menu, edpart_diedebris_menu);
    edpart_diedebris_menu->x = menu->x + 10;
    edpart_diedebris_menu->y = menu->y + 40;
}

void edpartChangeDebrisScale(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type) {
        edpart_nearest_type->effect_scale = ((edui_slider_s *)(item))->value;
    }
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
    if (edpart_nearest_type == NULL) {
        return;
    }
    edpart_instanceflags_menu = eduiMenuCreate(70, 70, 300, 250, ed_fnt, edpartCancelInstanceFlagsMenu, "Instance Flags");
    if (edpart_instanceflags_menu != NULL) {
        eduiMenuAddItem(edpart_instanceflags_menu, eduiItemToggleCreate(PART_FLAG_DIE_WHEN_STOPPED, edblack,
                                                                        (edpart_nearest_type->flags & PART_FLAG_DIE_WHEN_STOPPED) != 0,
                                                                        group++, edpartChangeInstanceFlag, "Die when stopped"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(PART_FLAG_COLLIDE_WITH_CHARACTERS, edblack,
                                             (edpart_nearest_type->flags & PART_FLAG_COLLIDE_WITH_CHARACTERS) != 0, group++,
                                             edpartChangeInstanceFlag, "Collide with Characters"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(PART_FLAG_ACTIVE_CHARACTERS_ONLY, edblack,
                                             (edpart_nearest_type->flags & PART_FLAG_ACTIVE_CHARACTERS_ONLY) != 0, group++,
                                             edpartChangeInstanceFlag, "Interact only with Active Chars"));
        eduiMenuAddItem(edpart_instanceflags_menu, eduiItemToggleCreate(PART_FLAG_DAMAGE_CHARACTERS, edblack,
                                                                        (edpart_nearest_type->flags & PART_FLAG_DAMAGE_CHARACTERS) != 0,
                                                                        group++, edpartChangeInstanceFlag, "Damage Characters"));
        if (edpart_nearest_type->effect_ids[0] != 9999) {
            eduiMenuAddItem(edpart_instanceflags_menu,
                            eduiItemToggleCreate(PART_FLAG_KEEP_OFF_SCREEN, edblack,
                                                 (edpart_nearest_type->flags & PART_FLAG_KEEP_OFF_SCREEN) != 0, group++,
                                                 edpartChangeInstanceFlag, "Don't die when off-screen"));
        }
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(PART_FLAG_COLLECTIBLE, edblack, (edpart_nearest_type->flags & PART_FLAG_COLLECTIBLE) != 0,
                                             group++, edpartChangeInstanceFlag, "Is a Collectible"));
        eduiMenuAddItem(edpart_instanceflags_menu, eduiItemToggleCreate(PART_FLAG_ROTATE_RANDOMLY, edblack,
                                                                        (edpart_nearest_type->flags & PART_FLAG_ROTATE_RANDOMLY) != 0,
                                                                        group++, edpartChangeInstanceFlag, "Rotate Randomly"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(PART_FLAG_ORDERED_ROTATE_RANDOMLY, edblack,
                                             (edpart_nearest_type->flags & PART_FLAG_ORDERED_ROTATE_RANDOMLY) != 0, group++,
                                             edpartChangeInstanceFlag, "Ordered Rotate Randomly"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(PART_FLAG_NO_BOUNCE, edblack, (edpart_nearest_type->flags & PART_FLAG_NO_BOUNCE) != 0, group++,
                                             edpartChangeInstanceFlag, "No Bounce"));
        eduiMenuAddItem(edpart_instanceflags_menu, eduiItemToggleCreate(PART_FLAG_SLOT_CANNOT_BE_STOLEN, edblack,
                                                                        (edpart_nearest_type->flags & PART_FLAG_SLOT_CANNOT_BE_STOLEN) != 0,
                                                                        group++, edpartChangeInstanceFlag, "Slot Cannot be Stolen"));
        eduiMenuAddItem(edpart_instanceflags_menu, eduiItemToggleCreate(PART_FLAG_IGNORE_TERRAIN, edblack,
                                                                        (edpart_nearest_type->flags & PART_FLAG_IGNORE_TERRAIN) != 0,
                                                                        group++, edpartChangeInstanceFlag, "Ignore Terrain"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(PART_FLAG_FACE_MOVEMENT, edblack, (edpart_nearest_type->flags & PART_FLAG_FACE_MOVEMENT) != 0,
                                             group++, edpartChangeInstanceFlag, "Face Direction of Movement"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(PART_FLAG_DISABLE_DRAW, edblack, (edpart_nearest_type->flags & PART_FLAG_DISABLE_DRAW) != 0,
                                             group++, edpartChangeInstanceFlag, "Disable Draw"));
        eduiMenuAddItem(edpart_instanceflags_menu, eduiItemToggleCreate(PART_FLAG_REAL_TIME_LIGHTING, edblack,
                                                                        (edpart_nearest_type->flags & PART_FLAG_REAL_TIME_LIGHTING) != 0,
                                                                        group++, edpartChangeInstanceFlag, "Real Time Lighting"));
        eduiMenuAddItem(edpart_instanceflags_menu, eduiItemToggleCreate(PART_FLAG_IGNORE_CREATURE, edblack,
                                                                        (edpart_nearest_type->flags & PART_FLAG_IGNORE_CREATURE) != 0,
                                                                        group++, edpartChangeInstanceFlag, "Ignore Creature"));
        eduiMenuAddItem(edpart_instanceflags_menu,
                        eduiItemToggleCreate(PART_FLAG_THROWN, edblack, (edpart_nearest_type->flags & PART_FLAG_THROWN) != 0, group++,
                                             edpartChangeInstanceFlag, "Thrown"));
        eduiMenuAddItem(edpart_instanceflags_menu, eduiItemToggleCreate(PART_FLAG_CAN_DAMAGE_OWNER, edblack,
                                                                        (edpart_nearest_type->flags & PART_FLAG_CAN_DAMAGE_OWNER) != 0,
                                                                        group++, edpartChangeInstanceFlag, "Can Damage Owner"));
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
        edpart_debrissettings_menu = eduiMenuCreate(70, 70, 300, 300, ed_fnt, edpartCancelDebrisSettingsMenu, "Debris Settings");
        if (edpart_debrissettings_menu != NULL) {
            eduiMenuAddItem(edpart_debrissettings_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartTrail1DebrisMenu, "Trail 1 Debris..."));
            eduiMenuAddItem(edpart_debrissettings_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartTrail2DebrisMenu, "Trail 2 Debris..."));
            eduiMenuAddItem(edpart_debrissettings_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartEmitterDebrisMenu, "Emitter Debris..."));
            eduiMenuAddItem(edpart_debrissettings_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartImpactDebrisMenu, "Impact Debris..."));
            eduiMenuAddItem(edpart_debrissettings_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartDieDebrisMenu, "Die Debris..."));
            eduiMenuAddItem(edpart_debrissettings_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartImpactPartMenu, "Impact Part..."));
            eduiMenuAddItem(edpart_debrissettings_menu,
                            eduiItemToggleCreate(PART_FLAG_DEBRIS_STOPS_WITH_PART, edblack, (edpart_nearest_type->flags >> 21) & 1, 1,
                                                 edpartChangeInstanceFlag, "Debris stops when Part stops"));
            eduiMenuAddItem(edpart_debrissettings_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartDebrisScaleMenu, "Debris Scale..."));
        }
        eduiMenuAttach(menu, edpart_debrissettings_menu);
        edpart_debrissettings_menu->x = menu->x + 10;
        edpart_debrissettings_menu->y = menu->y + 40;
    }
}

typedef struct debris_animation_effect_s { // size 0xb0
    u8 unknown_000[0xa8];                  // 0x00
    i16 texture_frames;                    // 0xa8
    i8 texture_rows;                       // 0xaa
    i8 frame_divisor;                      // 0xab

    union { // size 0x4
        f32 rate;
        u32 packed_rate;
    } animation_rate; // 0xac
} DEBRIS_ANIMATION_EFFECT;

extern NUMTL *edpart_mtl;
void edbitsDrawCube(f32 x, f32 y, f32 z, f32 half_x, f32 half_y, f32 half_z, i32 rotation_z, i32 rotation_y, i32 rotation_x, i32 outer_z,
                    i32 outer_y, i32 colour, NUMTL *material);

typedef struct PART_TERRAIN_STATE { // size 0x20c
    u8 unknown_000[0x200];          // 0x000
    u32 platform_id;                // 0x200
    u8 unknown_204[5];              // 0x204
    i8 impact_material;             // 0x209
} PART_TERRAIN_STATE;

static i32 PartCleanupCursorPrivate = 0;
static i32 PartCleanupFramesPrivate = 0;
i32 part_page_on[8] = {0};
PARTTYPE_LOOKUP part_types[128] = {0};
PART *Part = 0;
i32 edpart_nearest_orphans = 0;
static NUVEC PartImpactNormalPrivate = {0};
RTLSET *PartRTL = 0;
f32 partglobaltime = 0;
i32 part_types_used = 0;
i32 part_emits_used = 0;
static i32 PartRaycastsEnabledPrivate = 1;
DEBRIS_INFO DebrisNullEffect = {
    "null",
    0,
    0,
    0,
    0,
    100,
    60,
    0.0f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    0,
    0,
    0,
    0,
    40000.0f,
    0.0f,
    25.0f,
    0.0f,
    0.0f,
    0.5f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0},
    0.0f,
    1.0f,
    0,
    0,
    0,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    {{0.0f, 64, 64, 64, 0},
     {1.0f, 0, 0, 0, 0},
     {0.0f, 0, 0, 0, 0},
     {0.0f, 0, 0, 0, 0},
     {0.0f, 0, 0, 0, 0},
     {0.0f, 0, 0, 0, 0},
     {0.0f, 0, 0, 0, 0},
     {0.0f, 0, 0, 0, 0}},
    {{0.0f, 64.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    0.125f,
    0.125f,
    0.0f,
    500.0f,
    {{0.0f, 500.0f}, {1.0f, 500.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.0f, 500.0f}, {1.0f, 500.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    -360.0f,
    360.0f,
    {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    0.0f,
    0.0f,
    256.0f,
    256.0f,
    NULL,
    0.0f,
    {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    0,
    0,
    3,
    0,
    4.0f,
    {1.0f, 1.0f, 1.0f},
    1.0f,
    0.1f,
    0.0f,
    {{0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    {-1, -1, -1, -1, -1, -1, -1, -1},
    {-1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0},
    0,
    0,
    0,
    0,
    0.0f,
    0.0f,
    0.5f,
    1.0f,
    0};
i32 DebrisSuspendDrawObjectSwitch = -1;
f32 debris_render_scale = 1.0f;
i32 PartLevelPage = 1;

void InitPartTable(char **names) {
    i32 count = 0;
    part_debris_count = count;
    PDebNameList = names;
    if (names != NULL && *names != NULL) {
        while (*names != NULL) {
            names++;
            count++;
        }
        part_debris_count = count;
    }
}

PARTDEBRISSYS *InitPartDebris(VARIPTR *buffer, VARIPTR *end, i32 capacity, i32 named_count, char **names, i32 page) {
    PARTDEBRISSYS *system;
    i32 i;
    i32 type;
    system = (PARTDEBRISSYS *)((buffer->addr + 15) & ~(u32)15);
    buffer->addr = ((buffer->addr + 15) & ~(u32)15) + sizeof(PARTDEBRISSYS);
    if (system != NULL) {
        memset(system, 0, sizeof(*system));
        system->capacity = capacity;
        system->named_count = named_count;
        system->entries = (PARTDEBRISENTRY *)((buffer->addr + 15) & ~(u32)15);
        buffer->addr = ((buffer->addr + 15) & ~(u32)15) + capacity * sizeof(PARTDEBRISENTRY);
        if (system->entries != NULL) {
            memset(system->entries, -1, capacity * sizeof(PARTDEBRISENTRY));
            i = 0;
            if (names != NULL) {
                for (; i < system->named_count; ++i) {
                    NuStrCpy(system->entries[i].name, names[i]);
                    system->entries[i].id = -1;
                    system->entries[i].id = PARTLookupTypePageOnly(system->entries[i].name, page);
                }
            }
            for (type = 0; i < system->capacity && type < 128; ++type) {
                system->entries[i].id = -1;
                if (part_types[type].variant_count > 0) {
                    NuStrCpy(system->entries[i].name, part_types[type].name);
                    system->entries[i].id = PARTLookupTypePageOnly(system->entries[i].name, page);
                    ++i;
                }
            }
            for (; i < system->capacity; ++i) {
                system->entries[i].id = -1;
            }
            return system;
        }
    }
    return NULL;
}

i32 FindPartDebris(PARTDEBRISSYS *system, char *name) {
    i32 index;
    if (system != NULL) {
        for (index = system->named_count; index < system->capacity; ++index) {
            if (NuStrICmp(name, system->entries[index].name) == 0) {
                return system->entries[index].id;
            }
        }
    }
    return -1;
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

void AddPartDebris(PARTDEBRISSYS *sys, i32 index, NUVEC *position) {
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

f32 brickimpactwait = 0;

void PartImpact_Brick(PART *part) {
    if (brickimpactwait <= 0.0f) {
        f32 speed = NuVecMag(&part->velocity);
        if (speed > 0.0f) {
            i32 angle;
            f32 volume;
            i32 sound_id;
            if (speed > 1.0f) {
                speed = 1.0f;
            }
            angle = ((i32)((1.0 - speed) * 16384.0 + 16384.0));
            volume = NuTrigTable[(angle >> 1) & 0x7fff];
            sound_id = GetSfxId("LegoSingle");
            PlaySfxByIdAndSetVolume(sound_id, &part->position, volume);
            brickimpactwait = 0.1f;
        }
    }
}

void PartStop_Flickerer(PART *part) {
    part->time_remaining = 5.0f;
    if ((part->flicker_flags & 1) != 0) {
        part->time_remaining = 2.5f;
    }
}

i32 PartDraw_Flickerer(PART *part) {
    i32 draw = 1;
    if (part->time_remaining > 0.0f && GameTimer.update_count % 6 >= 3) {
        f32 threshold = 2.0f;
        if ((part->flicker_flags & 1) != 0) {
            threshold = 1.0f;
        }
        if (part->time_remaining < threshold) {
            draw = 0;
        }
    }
    return draw;
}

PART *FindIncomingPart(void *owner, NUVEC *position, f32 radius, u32 flags, f32 range) {
    f32 radius_squared = radius * radius;
    PART *nearest = NULL;
    PART *part = Part;
    f32 nearest_distance = range > 0.0f ? range : 100.0f;
    f32 distance;
    i32 i;
    for (i = 0; i < MAXPARTS; ++i, ++part) {
        if ((part->active & PART_IN_USE) == 0 || part->owner == owner || (part->flags & flags) != flags) {
            continue;
        }
        if (range > 0.0f) {
            distance = NuVecDistSqr(&part->position, position, NULL);
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
                f32 limit;
                NuVecScale(&direction, &part->velocity, reciprocal);
                half_speed = (f32)((f64)speed * 0.5);
                distance = NuVecDistSqr(&part->position, position, NULL);
                limit = half_speed * half_speed;
                if (distance < limit && LineIntersectSphere(&part->position, &direction, position, radius_squared, NULL)) {
                    nearest_distance = distance;
                    nearest = part;
                }
            }
        }
    }
    return nearest;
}

void MakePartVector(NUVEC *velocity, NUVEC *direction, f32 scale) {
    if (direction != NULL) {
        i32 divisor;
        u16 z_random;
        u16 x_random;
        FindAnglesZX(direction, NULL, NULL);
        *velocity = v010;
        divisor = 65535 / (((PartVectorSpreadDegrees * 2 + 1) << 16) / 360) + 1;
        z_random = (u16)(qrand() / divisor - ((PartVectorSpreadDegrees << 16) / 360));
        x_random = (u16)(qrand() / divisor - ((PartVectorSpreadDegrees << 16) / 360));
        NuVecRotateZ(velocity, velocity, (u16)(z_random + temp_xrot));
        NuVecRotateX(velocity, velocity, (u16)(x_random + temp_zrot));
        NuVecScale(velocity, velocity, scale);
    } else {
        f64 random = (f64)qrand() * (f64)(1.0f / 65535.0f);
        velocity->x = (f32)(random + random - 1.0);
        velocity->y = AreaPickupGravity == 0.0f ? 0.0f : (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) + 1.0);
        random = (f64)qrand() * (f64)(1.0f / 65535.0f);
        velocity->z = (f32)(random + random - 1.0);
        NuVecScale(velocity, velocity, AreaPickupScale);
    }
}

PART *TargetPart(GAMEOBJECT *object, NUVEC *position, NUVEC *direction, f32 radius, f32 range_squared, i32 directional, i32 bolt_id) {
    NUVEC aim = *direction;
    NUVEC delta;
    NUVEC minimum;
    NUVEC maximum;
    PART *best = NULL;
    BOLTTYPE *bolt = BoltType_FindByID(bolt_id, WORLD);
    PART *part = Part;
    PART *previous = NULL;
    f32 nearest_distance = range_squared;
    f32 distance;
    f32 dot;
    u16 angle;
    i32 i;
    minimum.x = position->x - radius;
    minimum.z = position->z - radius;
    maximum.x = position->x + radius;
    maximum.z = position->z + radius;
    if (directional != 0) {
        aim = *direction;
        if ((bolt->flags_60 & 0x20000) != 0) {
            aim.y = 0.0f;
            NuVecNorm(&aim, &aim);
        }
    }
    for (i = 0; i < MAXPARTS; ++i, ++part) {
        if ((part->active & PART_IN_USE) == 0 ||
            ((part->flags & 0x8000000) == 0 && (part->flags & (PART_FLAG_COLLIDE_WITH_CHARACTERS | PART_FLAG_DAMAGE_CHARACTERS)) !=
                                                   (PART_FLAG_COLLIDE_WITH_CHARACTERS | PART_FLAG_DAMAGE_CHARACTERS))) {
            continue;
        }
        if (part->position.x < minimum.x || part->position.x > maximum.x || part->position.z < minimum.z || part->position.z > maximum.z) {
            continue;
        }
        distance = NuVecDistSqr(&part->position, position, &delta);
        if (!(distance < range_squared)) {
            continue;
        }
        if (directional == 0) {
            NuVecRotateY(&aim, &v001, NuAtan2D(part->position.x - position->x, part->position.z - position->z));
        }
        if ((bolt->flags_60 & 0x20000) != 0) {
            delta.y = 0.0f;
        }
        NuVecNorm(&delta, &delta);
        dot = NuVecDot(&delta, &aim);
        if (distance < TargetDist_Near2 && directional != 0) {
            angle = TargetDeg_Near;
        } else if (TargetDist_Mid2 > distance) {
            angle = TargetDeg_Mid;
        } else {
            angle = TargetDeg_Far;
        }
        if (!(dot > NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff]) || !(nearest_distance > distance)) {
            continue;
        }
        if (part == object->player.collision_target) {
            previous = part;
        } else {
            nearest_distance = distance;
            best = part;
        }
    }
    if (best != NULL) {
        return best;
    }
    if (previous != NULL) {
        return previous;
    }
    return NULL;
}

PART *Part_FindFromHSpecial(nuhspecial_s *special) {
    PART *part;
    i32 index;
    if (special == NULL) {
        return NULL;
    }
    part = Part;
    for (index = 0; index < MAXPARTS; index++, part++) {
        if ((part->active & PART_IN_USE) != 0 && NuSpecialCompare(&part->special, special)) {
            return part;
        }
    }
    return NULL;
}

void SetPartRTLSet(RTLSET *rtl_set) {
    PartRTL = rtl_set;
}

void NuPartSetSeed(i32 seed) {
    partseed = (u32)seed;
}

u32 NuPartGetSeed(void) {
    return partseed;
}

void NuPartResetGlobalTime(void) {
    partglobaltime = 0.0f;
}

void KillAllParts(void) {
    PART *part = Part;
    i32 i;
    if (part != NULL) {
        for (i = 0; i < MAXPARTS; ++i, ++part) {
            if ((part->active & PART_IN_USE) != 0) {
                KillPart(part, 0);
            }
        }
    }
}

void ResetParts(void) {
    if (Part != NULL) {
        memset(Part, 0, MAXPARTS * sizeof(PART));
    }
    part_slot_cursor = 0;
}

// MATCH-EXCEPTION: noinline; the original calls this out of line from UpdateParts.
__declspec(noinline) void DebrisEmitterPos(i32 handle, f32 x, f32 y, f32 z) {
    if (handle != -1) {
        debkeydata[handle].position.x = x;
        debkeydata[handle].position.y = y;
        debkeydata[handle].position.z = z;
        debkeydata[handle].orientation_dirty = 0.0f;
    }
}

void DebrisEmitterMomentum(i32 handle, f32 x, f32 y, f32 z) {
    if (handle != -1) {
        debkeydata[handle].emitter_momentum.x = x;
        debkeydata[handle].emitter_momentum.y = y;
        debkeydata[handle].emitter_momentum.z = z;
    }
}

void DebrisParticleMomentum(i32 handle, f32 x, f32 y, f32 z) {
    if (handle != -1) {
        debkeydata[handle].momentum.x = x;
        debkeydata[handle].momentum.y = y;
        debkeydata[handle].momentum.z = z;
    }
}

i32 PARTLookupType(char *name) {
    i32 index;
    if (name == NULL || name[0] == 0) {
        return -1;
    }
    for (index = 0; index < 128; ++index) {
        if (NuStrNICmp(name, part_types[index].name, 16) == 0) {
            return index;
        }
    }
    return -1;
}

static void DebrisMtxCopyPrivate(NUMTX *destination, const NUMTX *source) {
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

static void DebrisMtxRotateXPrivate(NUMTX *m, i32 a) {
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

static void DebrisMtxRotateYPrivate(NUMTX *m, i32 a) {
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

static void DebrisMtxRotateZPrivate(NUMTX *m, i32 a) {
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

i32 maxdebkeys = 0;

void DebFreeAllPanelEffects(void) {
    i32 j;
    i32 key;
    for (j = 0; j < maxdebkeys; ++j) {
        if (debkeydata[j].effect_index != 0 && debtab[debkeydata[j].effect_index]->time_group == DEBRIS_DRAW_PANEL) {
            key = j;
            DebFreeInstantly(&key);
        }
    }
}

void DebrisOrientation(i32 handle, i16 z, i16 y) {
    if (handle == -1) {
        return;
    }
    DebrisMtxCopyPrivate(&debkeydata[handle].effect_orientation, &ANI3_EVAL_IDENTITY_MATRIX);
    DebrisMtxRotateZPrivate(&debkeydata[handle].effect_orientation, z);
    DebrisMtxRotateYPrivate(&debkeydata[handle].effect_orientation, y);
    debkeydata[handle].orientation_dirty = 0.0f;
}

void DebrisEmitterOrientation(i32 handle, i16 z, i16 y, i16 x) {
    if (handle == -1) {
        return;
    }
    DebrisMtxCopyPrivate(&debkeydata[handle].emitter_orientation, &ANI3_EVAL_IDENTITY_MATRIX);
    DebrisMtxRotateZPrivate(&debkeydata[handle].emitter_orientation, z);
    DebrisMtxRotateYPrivate(&debkeydata[handle].emitter_orientation, y);
    DebrisMtxRotateXPrivate(&debkeydata[handle].emitter_orientation, x);
    debkeydata[handle].orientation_dirty = 0.0f;
}

void DebrisReflectionOrientation(i32 handle, i16 x, i16 y, f32 plane, f32 scale) {
    if (handle != -1) {
        debkeydata[handle].reflection_x = x;
        debkeydata[handle].reflection_y = y;
        debkeydata[handle].collision_plane = plane;
        debkeydata[handle].reflection_scale = scale;
    }
}

i32 DebrisGetParticleCount(i32 handle) {
    if (handle == -1) {
        return 0;
    }
    return debkeydata[handle].allocated_chunk_count;
}

void DebrisSetTrigger(i32 handle, i32 first, i32 second, f32 third) {
    if (handle != -1) {
        debkeydata[handle].trigger_first = first;
        debkeydata[handle].trigger_second = second;
        debkeydata[handle].switch_variable = third;
    }
}

void DebrisSetGroupID(i32 handle, i16 group) {
    if (handle != -1) {
        debkeydata[handle].render_group = group;
    }
}

void DebrisSetPriority(i32 handle, i16 priority, u8 automatic) {
    if (handle != -1) {
        debkeydata[handle].timed_flags = automatic;
        if (automatic == 0) {
            debkeydata[handle].render_priority = priority;
        }
    }
}

u16 DebrisQueryPriority(i32 effect_index) {
    if (effect_index >= 0 && effect_index < debinf_count && debtab[effect_index] != NULL) {
        switch (debtab[effect_index]->particle_type) {
            case DEBRIS_PARTICLE_ADDITIVE:
                return 20000;
            case DEBRIS_PARTICLE_MODULATIVE:
                return 40000;
            case DEBRIS_PARTICLE_SUBTRACTIVE:
                return 30000;
            case DEBRIS_PARTICLE_GLASS:
                return 10000;
        }
    }
    return 0;
}

void DebrisSetDrawFlag(i32 handle, i8 draw_flag) {
    if (handle != -1) {
        debkeydata[handle].draw_enabled = draw_flag;
    }
}

void DebrisStartOffset(i32 handle, f32 offset) {
    DebrisStartOffsetEx(debkeydata + handle, offset);
}

void AddDebrisEffect(i32 *handle, i32 effect_index, f32 x, f32 y, f32 z) {
    i32 i;
    DEBRIS_INFO *effect;
    i32 newly_allocated;
    i32 key_index;
    f32 now;
    newly_allocated = 0;
    if (effect_index == -1 || effect_index == 0 || effect_index >= debinf_count) {
        return;
    }
    effect = debtab[effect_index];
    if (effect == NULL) {
        return;
    }
    if (effect->disabled != 0) {
        return;
    }

    if (*handle == -1) {
        *handle = DebAlloc();
        newly_allocated = 1;
    }
    key_index = *handle;
    if (key_index == -1) {
        return;
    }

    now = effect->time_group == DEBRIS_DRAW_PANEL ? panelglobaltime : globaltime;
    debkeydata[key_index].effect_index = (i16)(effect_index);
    debkeydata[key_index].finite = 0;
    DebrisStartOffset(key_index, effect->emission_start_offset);
    debkeydata[key_index].generator = gensorttab[effect->generator_type];
    debkeydata[key_index].momentum_adjuster = gencodetab[effect->momentum_adjustment_type];
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
    debkeydata[key_index].portal_room = -1;
    debkeydata[key_index].orientation_flags = 7;
    debkeydata[key_index].gscene = NULL;
    debkeydata[key_index].orientation_dirty = 0.0f;
    debkeydata[key_index].cutoff_distance = 999999.0f;
    debkeydata[key_index].process_collision_sound = 0;
    debkeydata[key_index].last_update_time = now;
    debkeydata[key_index].editor_flag = 1;
    debkeydata[key_index].user_data = NULL;
    debkeydata[key_index].custom_facing = 0;
    if (debkeydata[key_index].emission_end_time < now) {
        debkeydata[key_index].emission_epoch = debkeydata[key_index].emission_end_time;
    } else {
        debkeydata[key_index].emission_epoch = now;
    }
    debkeydata[key_index].emission_disabled = 0;

    switch (effect->particle_type) {
        case DEBRIS_PARTICLE_SUBTRACTIVE:
            debkeydata[key_index].render_priority = 40000;
            break;
        case DEBRIS_PARTICLE_GLASS:
            debkeydata[key_index].render_priority = 20000;
            break;
        case DEBRIS_PARTICLE_MODULATIVE:
            debkeydata[key_index].render_priority = 50000;
            break;
        default:
            debkeydata[key_index].render_priority = 30000;
            break;
    }
    for (i = 0; i < 4; ++i) {
        debkeydata[key_index].collision_timers[i] = 9999;
        if (effect->sound_data[i * 3] != -1) {
            if (effect->sound_data[i * 3 + 1] == 4 || effect->sound_data[i * 3 + 1] == 3) {
                debkeydata[key_index].collision_timers[i] = 1;
            }
            debkeydata[key_index].process_collision_sound = 1;
        }
    }
    DebrisEmitterPos(key_index, x, y, z);
    DebrisEmitterOrientation(key_index, 0, 0, 0);
    DebrisOrientation(key_index, 0, 0);
    DebrisReflectionOrientation(key_index, 0, 0, 0, 0.9f);
    DebrisSetTrigger(key_index, 0, -1, 0);
    if (newly_allocated) {
        AddDebrisEffectToStack(debkeydata + key_index);
    }
}

void AddRotatedDebrisEffect(i32 *handle, i32 effect, f32 x, f32 y, f32 z, i16 rotation_x, i16 rotation_y) {
    AddDebrisEffect(handle, effect, x, y, z);
    DebrisEmitterOrientation(*handle, rotation_x, rotation_y, 0);
}

DEBRISPARTICLE *GenDebIndexSpheroid(DEBRISKEY *key, DEBRIS_INFO *effect, float time) {
    i32 trail;
    DEBRISPARTICLE *particle;
    NUVEC vector;
    i32 angle_y;
    f64 random;
    f32 sine;
    i32 angle_z;
    NUMTX scale;
    DEBRISPARTICLE *trail_particle;
    if (key->particle_cursor >= key->particle_count) {
        key->particle_cursor = 0;
    }
    if (effect->particle_type == DEBRIS_PARTICLE_GLASS) {
        particle = &key->particle_chunks[key->particle_cursor / 12]->particles[key->particle_cursor % 12];
    } else {
        particle = &key->particle_chunks[key->particle_cursor / 32]->particles[key->particle_cursor % 32];
    }
    ++key->particle_cursor;
    particle->start_time = time;
    key->emission_epoch = time;
    particle->inverse_lifetime = 64.0f / effect->particle_lifetime;

    vector.x = NuFsqrt(NuRandFloatSeeded(&debrisseed));
    vector.y = vector.z = 0.0f;
    angle_y = (i32)(NuRandFloatSeeded(&debrisseed) * 65536.0f);
    random = NuRandFloatSeeded(&debrisseed);
    sine = random + random - 1.0f;
    angle_z = (i16)DebrisNuASin(sine);
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

    for (trail = 0; trail < effect->trail_count; ++trail) {
        if (key->particle_cursor >= key->particle_count) {
            key->particle_cursor = 0;
        }
        if (effect->particle_type == DEBRIS_PARTICLE_GLASS) {
            trail_particle = &key->particle_chunks[key->particle_cursor / 12]->particles[key->particle_cursor % 12];
        } else {
            trail_particle = &key->particle_chunks[key->particle_cursor / 32]->particles[key->particle_cursor % 32];
        }
        ++key->particle_cursor;
        *trail_particle = *particle;
        trail_particle->start_time += (f32)(trail + 1) * effect->trail_time;
    }

    if (effect->native_data == NULL) {
        GenericDebinfoDmaTypeUpdate(effect);
    }
    return particle;
}

/* The SDK NuFabs; this unit keeps its own out-of-line copy. */
static f32 DebrisFabs(f32 f) {
    f32 rv;
    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

static f32 DebrisDivide(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f) {
        return 0.0f;
    }
    return a / b;
}

void DebrisEmitterOrientationMtx(i32 handle, NUMTX *source) {
    if (handle != -1) {
        debkeydata[handle].emitter_orientation = *source;
        debkeydata[handle].emitter_orientation.m30 = 0.0f;
        debkeydata[handle].emitter_orientation.m31 = 0.0f;
        debkeydata[handle].emitter_orientation.m32 = 0.0f;
        debkeydata[handle].orientation_dirty = 0.0f;
    }
}

void AddVariableShotDebrisEffectTimed5(i32 effect_index, NUVEC *position, NUVEC *momentum, NUVEC *position_delta, i32 count, f32 duration,
                                       NUMTX *emitter_orientation, NUMTX *particle_orientation, u16 render_priority, i8 timed_flags) {
    DEBRIS_INFO *effect;
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
    i32 particle_key_slot;
    i32 new_key_index;
    DEBRISKEY *key;
    i32 required_particles;
    i32 required_chunks;
    i32 new_chunk_count;
    i32 total_chunk_count;
    DEBRISPARTICLE *generated;
    i32 chunk_index;
    DEBRIS_CHUNK_CONTROL *control;
    DEBRIS_CHUNK_CONTROL **stack;
    DEBRIS_PARTICLE_CHUNK *last_chunk;

    if (debris_suspended != 0 || update_debris_enabled == 0 || effect_index == -1 || effect_index == 0 || effect_index >= debinf_count) {
        return;
    }
    effect = debtab[effect_index];
    if (effect == NULL || effect->disabled != 0 || count <= 0) {
        return;
    }
    if (DEBRIS_DRAW_PANEL != effect->time_group) {
        NUVEC extent;
        clipped_position.x = position->x;
        clipped_position.y = position->y;
        clipped_position.z = position->z;
        extent.x = extent.y = extent.z = 1.0f;
        if (NuCameraClipTestExtentsAxisAligned(&clipped_position, &extent, effect->clip_extent) == 0) {
            return;
        }
    }
    thinning = 0 != forced_debris_thinning ? debris_thinning_level
                                           : (effect->thinning < debris_thinning_level ? effect->thinning : debris_thinning_level);
    rate = DebrisDivide((f32)count, thinning);
    emission_interval = DebrisDivide(1.0f, rate);
    if (emitter_orientation == NULL) {
        emitter_orientation = &NuMtxSetIdentityTemplate;
    }
    if (particle_orientation == NULL) {
        particle_orientation = &NuMtxSetIdentityTemplate;
    }
    if (effect->time_group == DEBRIS_DRAW_PANEL) {
        now = panelglobaltime;
        panel_time = 1;
    } else {
        now = globaltime;
        panel_time = 0;
    }
    end_time = now + duration;
    emission_epoch = ((f32)(i32)DebrisDivide(now, emission_interval)) * emission_interval;
    emission_time = emission_epoch + emission_interval;
    emission_count = 0;
    next_emission_time = emission_time;
    i = 0;
    while (next_emission_time <= end_time) {
        if (++i >= 100) {
            break;
        }
        ++emission_count;
        next_emission_time += emission_interval;
    }
    if (emission_count == 0) {
        return;
    }
    particle_count = emission_count * (effect->trail_count + 1);
    particle_key_slot = -1;
    key = NULL;
    for (i = 0; i < 8; ++i) {
        if (effect->particle_keys[i] != -1) {
            key = &debkeydata[effect->particle_keys[i]];
            if (effect->particle_type == DEBRIS_PARTICLE_GLASS) {
                if (key->particle_count + particle_count <= 0x180) {
                    particle_key_slot = i;
                    break;
                }
            } else if (key->particle_count + particle_count <= 0x400) {
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
    if (key == NULL) {
        return;
    }
    required_particles = particle_count + key->particle_count;
    if (effect->particle_type == DEBRIS_PARTICLE_GLASS) {
        if (required_particles > key->allocated_chunk_count * 12) {
            if (key->allocated_chunk_count < 0) {
                key->allocated_chunk_count = 0;
            }
            required_chunks = (required_particles + 12 - 1) / 12;
            new_chunk_count = required_chunks - key->allocated_chunk_count;
            if (freedebchkptrg + new_chunk_count >= debrischunksglass || required_chunks > 32) {
                return;
            }
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
        if (required_particles > key->allocated_chunk_count * 32) {
            if (key->allocated_chunk_count < 0) {
                key->allocated_chunk_count = 0;
            }
            required_chunks = (required_particles + 32 - 1) / 32;
            new_chunk_count = required_chunks - key->allocated_chunk_count;
            if (freedebchkptr + new_chunk_count >= debrischunks || required_chunks > 32) {
                return;
            }
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
    if (momentum != NULL) {
        key->momentum.x = momentum->x;
        key->momentum.y = momentum->y;
        key->momentum.z = momentum->z;
    } else {
        key->momentum.x = 0.0f;
        key->momentum.y = 0.0f;
        key->momentum.z = 0.0f;
    }
    DebrisEmitterOrientationMtx(effect->particle_keys[particle_key_slot], emitter_orientation);
    key->emission_epoch = emission_epoch;
    i = 0;
    while (emission_time <= end_time) {
        if (++i >= 100) {
            break;
        }
        if (position_delta != NULL) {
            NuVecAddScale(&key->emission_position, position, position_delta, emission_time - end_time);
        } else {
            key->emission_position.x = position->x;
            key->emission_position.y = position->y;
            key->emission_position.z = position->z;
        }
        key->emission_time = emission_time;
        generated = key->generator(key, effect, emission_time);
        if (effect->process_spheres != 0 && i == 1 && generated != NULL) {
            DebrisProcessSpheres(generated, emission_time, effect, key, 1);
        }
        emission_time = emission_interval + key->emission_epoch;
    }
    DebrisGetControlStackLock();
    // MATCH-EXCEPTION: the original tests the 16-bit counts once before entering the loop.
    if (key->controlled_chunk_count < key->allocated_chunk_count) {
        for (chunk_index = key->controlled_chunk_count; chunk_index < key->allocated_chunk_count; ++chunk_index) {
            if (freechunkcontrolsptr < (debrischunks + debrischunksglass) * 2) {
                freechunkcontrols[freechunkcontrolsptr]->particle_chunk = key->particle_chunks[chunk_index];
                freechunkcontrols[freechunkcontrolsptr]->expiry_time =
                    now + effect->particle_lifetime + (f32)effect->trail_count * effect->trail_time;
                freechunkcontrols[freechunkcontrolsptr]->active = 1;
                freechunkcontrols[freechunkcontrolsptr]->owner = key;
                AddChunkControlToStack(freechunkcontrols[freechunkcontrolsptr], &debris_chunk_control_stack[panel_time]);
                ++freechunkcontrolsptr;
                ++key->controlled_chunk_count;
            }
        }
    }
    if (key->allocated_chunk_count == key->controlled_chunk_count) {
        last_chunk = key->particle_chunks[key->controlled_chunk_count - 1];
        stack = &debris_chunk_control_stack[panel_time];
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

void AddVariableShotDebrisEffectMtx4(i32 effect, NUVEC *position, NUVEC *momentum, i32 count, NUMTX *emitter_orientation,
                                     NUMTX *particle_orientation, u16 priority, i8 flags) {
    AddVariableShotDebrisEffectTimed5(effect, position, momentum, NULL, count * 30, debris_time_increment, emitter_orientation,
                                      particle_orientation, priority, flags);
}

void AddVariableShotDebrisEffectMtx3(i32 effect, NUVEC *position, NUVEC *momentum, i32 count, NUMTX *emitter_orientation,
                                     NUMTX *particle_orientation) {
    u16 priority;
    if (effect < 0 || debtab[effect] == NULL) {
        return;
    }
    switch (debtab[effect]->particle_type) {
        case DEBRIS_PARTICLE_MODULATIVE:
            priority = 40000;
            break;
        case DEBRIS_PARTICLE_SUBTRACTIVE:
            priority = 30000;
            break;
        case DEBRIS_PARTICLE_GLASS:
            priority = 10000;
            break;
        default:
            priority = 20000;
            break;
    }
    AddVariableShotDebrisEffectTimed5(effect, position, momentum, NULL, count * 30, debris_time_increment, emitter_orientation,
                                      particle_orientation, priority, 0);
}

void AddVariableShotDebrisEffectMtx(i32 effect, NUVEC *position, i32 count, i16 z_rotation, i16 y_rotation, NUMTX *particle_orientation) {
    NUMTX orientation;
    DebrisMtxCopyPrivate(&orientation, &ANI3_EVAL_IDENTITY_MATRIX);
    DebrisMtxRotateZPrivate(&orientation, z_rotation);
    DebrisMtxRotateYPrivate(&orientation, y_rotation);
    AddVariableShotDebrisEffectMtx3(effect, position, &APIObjectZeroMomentumPC, count, &orientation, particle_orientation);
}

void AddVariableShotDebrisEffect(i32 effect, NUVEC *position, i32 count, i16 z_rotation, i16 y_rotation) {
    AddVariableShotDebrisEffectMtx(effect, position, count, z_rotation, y_rotation, NULL);
}

i32 AddScaledVariableShotDebrisEffect(i32 effect, NUVEC *position, i32 count, f32 time, i16 z_rotation, i16 y_rotation, f32 scale) {
    i32 scaled = CreateScaledEffect(effect, scale);
    if (scaled != -1) {
        AddVariableShotDebrisEffectTimed1(scaled, position, count, time, z_rotation, y_rotation, NULL);
    }
    return scaled;
}

void DebrisDrawCalculateClipBoxes(DEBRIS_INFO *effect, DEBRISKEY *key) {
    NUMTX matrix;
    f32 padding;
    f32 radius;
    NUVEC extent;
    NUVEC minimum;
    DebrisMtxCopyPrivate(&matrix, &key->effect_orientation);
    NuMtxTranslate(&matrix, &key->position);
    extent.x = (DebrisFabs(effect->emitter_velocity.x) * (effect->emission_period_random + effect->emission_period) +
                effect->momentum_random_x * effect->particle_lifetime) +
               effect->emitter_random_extent.x;
    extent.y = (DebrisFabs(effect->emitter_velocity.y) * (effect->emission_period_random + effect->emission_period) +
                (DebrisFabs(effect->momentum_base) + effect->rotor_speed_x) * effect->particle_lifetime) +
               effect->emitter_random_extent.y;
    extent.z = (DebrisFabs(effect->emitter_velocity.z) * (effect->emission_period_random + effect->emission_period) +
                effect->rotor_speed_y * effect->particle_lifetime) +
               effect->emitter_random_extent.z;
    NuVecMtxRotate(&extent, &extent, &key->emitter_orientation);
    if (extent.x < 0.0f) {
        extent.x = -extent.x;
    }
    if (extent.y < 0.0f) {
        extent.y = -extent.y;
    }
    if (extent.z < 0.0f) {
        extent.z = -extent.z;
    }
    padding = (f32)((f64)0.0001f * effect->max_size);
    /* Rounds after each padding pass. */
    extent.x += padding;
    extent.y += padding;
    extent.z += padding;
    extent.x = (f32)(extent.x + (f64)0.2f);
    extent.y = (f32)(extent.y + (f64)0.2f);
    extent.z = (f32)(extent.z + (f64)0.2f);
    minimum.x = -extent.x;
    minimum.y = -extent.y;
    minimum.z = -extent.z;
    if (effect->gravity > 0.0f) {
        extent.y += (effect->gravity * effect->particle_lifetime) * effect->particle_lifetime;
    } else if (effect->gravity < 0.0f) {
        minimum.y += (effect->gravity * effect->particle_lifetime) * effect->particle_lifetime;
    }
    radius = MAX(MAX(extent.x, extent.z), MAX(extent.y, -minimum.y));
    key->clip_radius = radius;
    key->clip_min = minimum;
    key->clip_max = extent;
    DebrisMtxCopyPrivate(&key->clip_matrix, &matrix);
}

__declspec(align(16)) NUMTX xzfacingmtx = {0};
i32 g_renderingDebris = 0;
i32 edbits_editmode = 0;

void DebrisDraw(i32 unused, i32 pass) {
    DEBRIS_RENDER_CHUNK *chunk;
    i32 drawn = 0;
    if (debris_suspended != 0) {
        return;
    }
    _NuTimeBarSlotBegin(0, 12, "deb");
    DebMat[7]->texture_id = (i16)debris_rt;
    for (chunk = ParticleChunkRenderStack[pass]; chunk != NULL; chunk = chunk->next) {
        DEBRIS_INFO *effect;
        DEBRISKEY *key;
        NUMTX matrix;
        NUVEC position;
        f32 render_time;
        if (chunk->particle_chunk == NULL) {
            continue;
        }
        effect = chunk->effect;
        key = chunk->key;
        if (DebrisCutSceneMode != 0 && effect->cutscene_only != 0) {
            continue;
        }
        if (DebrisSuspendDrawObjectSwitch != -1 && key != NULL && key->trigger_second == DebrisSuspendDrawObjectSwitch) {
            continue;
        }
        if (effect->native_data == NULL) {
            if (freeDmaDebType == EDPP_MAX_DMADEBTYPES) {
                DebrisFreeOldestDmaDebTypeTable();
            }
            GenericDebinfoDmaTypeUpdate(effect);
            if (effect->native_data == NULL) {
                continue;
            }
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
                key->cutoff_distance > effect->sound_range) {
                visible = 0;
            }
            if (key->draw_enabled == 0) {
                visible = 0;
            }
            if (pass != 4 && visible && effect->generator_type == DEBRIS_GENERATOR_NORMAL && effect->use_explicit_clip_box == 0 &&
                !assigned) {
                NUVEC minimum;
                NUVEC maximum;
                if (key->clip_radius == 0.0f || *edbits_editor_enabled != 0) {
                    DebrisDrawCalculateClipBoxes(effect, key);
                }
                if (edbits_editmode == 0 && key->gscene != NULL && key->portal_room != -1) {
                    if (NuPortalClipTest(key->gscene, &key->position, key->clip_radius, key->portal_room) == 0) {
                        visible = 0;
                    }
                }
                minimum = key->clip_min;
                maximum = key->clip_max;
                if (NuCameraClipTestExtents(&minimum, &maximum, &key->clip_matrix, 0.0f, 0) == 0) {
                    continue;
                }
            }
            if (!visible) {
                continue;
            }
            DebrisMtxCopyPrivate(&matrix, &key->effect_orientation);
            position.x = key->position.x;
            position.y = key->position.y;
            position.z = key->position.z;
        } else {
            if (effect->sound_range > 0.0f && effect->use_explicit_clip_box == 0 &&
                CameraEmitterDistance(&chunk->position) > effect->sound_range) {
                continue;
            }
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
        if (key != NULL && key->custom_facing != 0) {
            NuRndrSetParticleRotation(&key->particle_orientation);
        } else if (effect->camera_facing != 0) {
            NuRndrSetParticleRotation(&xzfacingmtx);
        } else {
            NuRndrSetParticleRotation(NULL);
        }
        NuMtxPreScaleX(&matrix, debris_render_scale);
        if (effect->particle_type == DEBRIS_PARTICLE_GLASS) {
            NuRndrParticleGroup(chunk->particle_chunk, chunk->effect->native_data, DebMat[chunk->effect->particle_type], render_time,
                                &matrix, 4, effect->particle_parameter_a, effect->particle_parameter_b, effect->clip_extent,
                                effect->unknown_044);
        } else if (effect->use_explicit_clip_box != 0) {
            NuRndrSetDebBox(&effect->particle_box);
            NuRndrParticleGroup(chunk->particle_chunk, chunk->effect->native_data, DebMat[chunk->effect->particle_type], render_time,
                                &matrix, 6, effect->particle_parameter_a, effect->particle_parameter_b, effect->clip_extent,
                                effect->unknown_044);
        } else {
            NuRndrParticleGroup(chunk->particle_chunk, chunk->effect->native_data, DebMat[chunk->effect->particle_type], render_time,
                                &matrix, 0, effect->particle_parameter_a, effect->particle_parameter_b, effect->clip_extent,
                                effect->unknown_044);
        }
    }
    if (drawn != 0) {
        g_renderingDebris = g_renderingDebris == 0;
    }
    _NuTimeBarSlotEnd(0, 12);
}

void AddVariableShotDebrisEffectTimed3(i32 effect, NUVEC *position, NUVEC *momentum, i32 count, f32 time, NUMTX *emitter_orientation,
                                       NUMTX *particle_orientation) {
    u16 priority;
    if (effect < 0 || debtab[effect] == NULL) {
        return;
    }
    switch (debtab[effect]->particle_type) {
        case DEBRIS_PARTICLE_MODULATIVE:
            priority = 40000;
            break;
        case DEBRIS_PARTICLE_SUBTRACTIVE:
            priority = 30000;
            break;
        case DEBRIS_PARTICLE_GLASS:
            priority = 10000;
            break;
        default:
            priority = 20000;
            break;
    }
    AddVariableShotDebrisEffectTimed5(effect, position, momentum, NULL, count, time, emitter_orientation, particle_orientation, priority,
                                      0);
}

void AddVariableShotDebrisEffectTimed1(i32 effect, NUVEC *position, i32 count, f32 time, i16 z_rotation, i16 y_rotation,
                                       NUMTX *particle_orientation) {
    NUMTX orientation;
    DebrisMtxCopyPrivate(&orientation, &ANI3_EVAL_IDENTITY_MATRIX);
    DebrisMtxRotateZPrivate(&orientation, z_rotation);
    DebrisMtxRotateYPrivate(&orientation, y_rotation);
    AddVariableShotDebrisEffectTimed3(effect, position, &APIObjectZeroMomentumPC, count, time, &orientation, particle_orientation);
}

void AddVariableShotPARTEffect(i32 effect_index, NUVEC *position, f32 rate, f32 duration, NUMTX *orientation) {
    PARTTYPE_LOOKUP *type = &part_types[effect_index];
    f32 interval;
    f32 aligned_time;
    f32 emission_time;
    f32 end_time;
    i32 count;
    if (type->name[0] == 0 || type->emission_rate <= 0.0f) {
        return;
    }
    if (rate <= 0.0f) {
        rate = type->emission_rate;
    }
    if (orientation == NULL) {
        orientation = &NuMtxSetIdentityTemplate;
    }
    if (rate != 0.0f) {
        interval = 1.0f / rate;
    } else {
        interval = 0.0f;
    }
    end_time = partglobaltime + duration;
    if (partglobaltime == 0.0f || interval == 0.0f) {
        emission_time = 0.0f;
    } else {
        emission_time = partglobaltime / interval;
    }
    aligned_time = (i32)emission_time * interval;
    emission_time = aligned_time + interval;
    for (count = 0; end_time >= emission_time && ++count < 100;) {
        ADDPART params = Default_ADDPART;
        NUVEC part_position;
        NUVEC velocity;
        nuhspecial_s special;
        NUMTX matrix;
        f32 range;
        i16 angle_x;
        i16 angle_y;
        i16 angle_z;
        i32 variant;
        i32 axis;
        i32 sound;
        part_position.x = NuRandFloatSeeded(&partseed) * 2.0f * type->position_random.x - type->position_random.x;
        part_position.y = NuRandFloatSeeded(&partseed) * 2.0f * type->position_random.y - type->position_random.y;
        part_position.z = NuRandFloatSeeded(&partseed) * 2.0f * type->position_random.z - type->position_random.z;
        NuVecMtxTransformVU0(&part_position, &part_position, orientation);
        NuVecAdd(&part_position, &part_position, position);
        velocity.x = velocity.z = 0.0f;
        velocity.y = type->speed;
        velocity.x += NuRandFloatSeeded(&partseed) * 2.0f * type->velocity_random.x - type->velocity_random.x;
        velocity.y += NuRandFloatSeeded(&partseed) * 2.0f * type->velocity_random.y - type->velocity_random.y;
        velocity.z += NuRandFloatSeeded(&partseed) * 2.0f * type->velocity_random.z - type->velocity_random.z;
        NuVecMtxTransformVU0(&velocity, &velocity, orientation);
        params.gravity = type->gravity;
        params.position = &part_position;
        params.velocity = &velocity;
        params.unknown_20 = type->bounce;
        if (type->variant_count == 0) {
            variant = 0;
        } else if (type->variant_mode == 0 || type->variant_mode == 1) {
            variant = (i32)((f64)NuRandFloatSeeded(&partseed) * 65535.0) % type->variant_count;
        } else {
            variant = 0;
        }
        params.part_id = type->effect_ids[variant];
        if (type->effect_ids[variant] == 9999) {
            params.special = NULL;
        } else if (type->effect_ids[variant] != -1) {
            params.special = &special;
            NuGScnGetSpecial(&special, part_scene[type->scene_indices[variant]], params.part_id);
        }
        params.unknown_a4 = type->lifetime + NuRandFloatSeeded(&partseed) * type->lifetime_random;
        NuMtxSetIdentity(&matrix);
        range = (f32)type->rotation_random[0];
        angle_x = (i16)(i32)(NuRandFloatSeeded(&partseed) * 2.0f * range + type->rotation[0] - range);
        range = (f32)type->rotation_random[1];
        angle_y = (i16)(i32)(NuRandFloatSeeded(&partseed) * 2.0f * range + type->rotation[1] - range);
        range = (f32)type->rotation_random[2];
        angle_z = (i16)(i32)(NuRandFloatSeeded(&partseed) * 2.0f * range + type->rotation[2] - range);
        for (axis = 0; axis < 3; ++axis) {
            params.unknown_a8[axis] = type->rotation[axis];
            params.unknown_a8[axis + 3] = type->rotation_random[axis];
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
        params.unknown_68 = type->trail_rates[0];
        params.unknown_6c = type->trail_rates[1];
        params.effect_id_70 = type->impact_effect;
        if (type->impact_part == -1 && type->impact_part_name[0] != 0) {
            type->impact_part = PARTLookupType(type->impact_part_name);
        }
        params.effect_id_74 = type->impact_part;
        params.effect_id_78 = type->kill_effect;
        params.lighting_scale_x = type->lighting_scale.x;
        params.lighting_scale_y = type->lighting_scale.y;
        params.lighting_scale_z = type->lighting_scale.z;
        AddPart(&params);
        for (sound = 0; sound < 4; ++sound) {
            if (type->sounds[sound] != -1 && type->sound_modes[sound] == 3) {
                PlaySfxById(type->sounds[sound], &part_position);
            }
        }
        type->last_used_time = emission_time;
        emission_time += interval;
    }
}

PART *FindPart(NUVEC *position, i32 player, GAMEOBJECT *owner) {
    PART *part;
    f32 nearest;
    PART *result;
    i32 i;
    if (Part == NULL) {
        return NULL;
    }
    part = Part;
    nearest = 1000000.0f;
    result = NULL;
    for (i = 0; i < MAXPARTS; ++i, ++part) {
        if ((part->active & PART_IN_USE) != 0 && (player == -1 || part->unknown_206 == player) && (owner == NULL || part->owner == owner)) {
            if (position != NULL) {
                f32 distance = NuVecDistSqr(position, &part->position, NULL);
                if (distance < nearest) {
                    nearest = distance;
                    result = part;
                }
            } else {
                return part;
            }
        }
    }
    return result;
}

void DebrisSetFacing(i32 handle, u8 enabled, i16 x_angle, i16 y_angle) {
    if (handle != -1) {
        debkeydata[handle].custom_facing = enabled;
        DebrisMtxCopyPrivate(&debkeydata[handle].particle_orientation, &ANI3_EVAL_IDENTITY_MATRIX);
        DebrisMtxRotateXPrivate(&debkeydata[handle].particle_orientation, x_angle);
        DebrisMtxRotateYPrivate(&debkeydata[handle].particle_orientation, y_angle);
    }
}

void DebrisSetRoomID(i32 handle, NUGSCN *scene) {
    if (scene != NULL) {
        debkeydata[handle].portal_room = NuPortalWhichRoom(scene, &debkeydata[handle].position);
        debkeydata[handle].gscene = debkeydata[handle].portal_room != -1 ? scene : NULL;
    } else {
        debkeydata[handle].portal_room = -1;
        debkeydata[handle].gscene = NULL;
    }
}

void DebrisSetDetailLevels(i32 handle, i32 detail_levels) {
    debkeydata[handle].orientation_flags = (u8)detail_levels;
}

void AddDebrisEffectToStack(DEBRISKEY *key) {
    if (debris_keystack != NULL) {
        debris_keystack->next = key;
    }
    key->previous = debris_keystack;
    debris_keystack = key;
}

// MATCH-EXCEPTION: noinline; the original calls this out of line from UpdateParts.
__declspec(noinline) void DebrisOrientationMtx(i32 handle, NUMTX *source) {
    if (handle != -1) {
        debkeydata[handle].effect_orientation = *source;
        debkeydata[handle].effect_orientation.m30 = 0.0f;
        debkeydata[handle].effect_orientation.m31 = 0.0f;
        debkeydata[handle].effect_orientation.m32 = 0.0f;
        debkeydata[handle].orientation_dirty = 0.0f;
    }
}

void DebrisPosOrientationMtx(i32 handle, NUMTX *source) {
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

i32 AddScaledVariableShotDebrisEffect2(i32 effect, NUVEC *position, i32 count, f32 time, NUMTX *emitter, NUMTX *particle, f32 scale) {
    i32 scaled = CreateScaledEffect(effect, scale);
    if (scaled != -1) {
        AddVariableShotDebrisEffectTimed3(scaled, position, &APIObjectZeroMomentumPC, count, time, emitter, particle);
    }
    return scaled;
}

i32 AddScaledVariableShotDebrisEffect3(i32 effect, NUVEC *position, NUVEC *momentum, i32 count, f32 time, NUMTX *emitter_orientation,
                                       NUMTX *particle_orientation, f32 scale) {
    i32 scaled = CreateScaledEffect(effect, scale);
    if (scaled != -1) {
        AddVariableShotDebrisEffectTimed3(scaled, position, momentum, count, time, emitter_orientation, particle_orientation);
    }
    return scaled;
}

i32 AddScaledVariableShotDebrisEffect4(i32 effect, NUVEC *position, NUVEC *momentum, i32 count, i32 time, NUMTX *emitter_orientation,
                                       NUMTX *particle_orientation, u16 priority, i8 flags, f32 scale) {
    i32 scaled = CreateScaledEffect(effect, scale);
    if (scaled != -1) {
        AddVariableShotDebrisEffectTimed5(scaled, position, momentum, NULL, count, time, emitter_orientation, particle_orientation,
                                          priority, flags);
    }
    return scaled;
}

i32 AddScaledVariableShotDebrisEffect5(i32 effect, NUVEC *position, NUVEC *momentum, NUVEC *position_delta, i32 count, f32 time,
                                       NUMTX *emitter_orientation, NUMTX *particle_orientation, u16 priority, i8 flags, f32 scale) {
    i32 scaled = CreateScaledEffect(effect, scale);
    if (scaled != -1) {
        AddVariableShotDebrisEffectTimed5(scaled, position, momentum, position_delta, count, time, emitter_orientation,
                                          particle_orientation, priority, flags);
    }
    return scaled;
}

NUMTL *CreateCopyMat(NUMTL *source, i32 enable_uv_mode, i32 alpha_mode, i32 depth_mode, i32 filter_mode) {
    DEBRIS_MATERIAL *material;
    DEBRIS_MATERIAL *original = (DEBRIS_MATERIAL *)source;
    if (source == NULL) {
        return NULL;
    }
    material = (DEBRIS_MATERIAL *)NuMtlCreate3D(1);
    material->diffuse.x = 1.0f;
    material->diffuse.y = 1.0f;
    material->diffuse.z = 1.0f;
    material->opacity = 0.999f;
    material->attribs.cull_mode = NUMTL_CULL_NONE;
    material->attribs.z_mode = (u32)depth_mode & 3;
    material->attribs.filter_mode = (u32)filter_mode & 3;
    material->attribs.alpha_mode = (u32)alpha_mode & 15;
    if (enable_uv_mode != 0) {
        material->secondary.disable_depth_write = 1;
    }
    material->attribs.u_address = NUMTL_ADDRESS_CLAMP;
    material->attribs.v_address = NUMTL_ADDRESS_CLAMP;
    material->texture_id = original->texture_id;
    material->sort_priority = original->sort_priority;
    material->secondary.alpha_fail = 1;
    NuMtlUpdate((NUMTL *)material);
    return (NUMTL *)material;
}

i32 DebrisArenaInitialized = 0;
i16 *freedebkeys = 0;
DEBRIS_PART_HEADER **DmaDebTypes = 0;

void DebrisSetup2(VARIPTR *buffer, VARIPTR buffer_end, VARIPTR *trash_buffer, VARIPTR trash_end, char *texture_name, i32 chunk_count,
                  i32 glass_chunk_count, i32 key_count, i32 effect_count) {
    i32 i;
    i32 texture_id;
    if (debris_trash_space == 0) {
        debrischunks = chunk_count;
        debrischunksglass = glass_chunk_count;
    }
    debris_setup_called = 1;
    maxdebkeys = key_count;
    debinf_count = effect_count;
    DebrisArenaInitialized = 1;
    NuRegisterEndFrameCallBackFn(DebrisProcessTimeSlip);
    buffer->addr = (buffer->addr + 15) & ~15u;
    debris_chunk_controls = (DEBRIS_CHUNK_CONTROL *)buffer->addr;
    buffer->addr += (debrischunks + debrischunksglass) * 2 * sizeof(DEBRIS_CHUNK_CONTROL);
    freechunkcontrols = (DEBRIS_CHUNK_CONTROL **)buffer->addr;
    buffer->addr += (debrischunks + debrischunksglass) * 2 * sizeof(DEBRIS_CHUNK_CONTROL *);
    freedebchunks = (DEBRIS_PARTICLE_CHUNK **)buffer->addr;
    buffer->addr += debrischunks * sizeof(DEBRIS_PARTICLE_CHUNK *);
    freedebchunksglass = (DEBRIS_PARTICLE_CHUNK **)buffer->addr;
    buffer->addr += debrischunksglass * sizeof(DEBRIS_PARTICLE_CHUNK *);
    ParticleChunkToRender = (DEBRIS_RENDER_CHUNK *)buffer->addr;
    buffer->addr += (debrischunks + debrischunksglass) * sizeof(DEBRIS_RENDER_CHUNK);
    buffer->addr = (buffer->addr + 15) & ~15u;
    debkeydata = (DEBRISKEY *)buffer->addr;
    buffer->addr += maxdebkeys * sizeof(DEBRISKEY);
    freedebkeys = (i16 *)buffer->addr;
    buffer->addr += maxdebkeys * sizeof(i16);
    effecttypes = (DEBRIS_INFO *)buffer->addr;
    buffer->addr += debinf_count * sizeof(DEBRIS_INFO);
    debtab = (DEBRIS_INFO **)buffer->addr;
    buffer->addr += debinf_count * sizeof(DEBRIS_INFO *);
    DmaDebTypes = (DEBRIS_PART_HEADER **)buffer->addr;
    buffer->addr += EDPP_MAX_DMADEBTYPES * sizeof(DEBRIS_PART_HEADER *);
    spherecolldata = (DEBRIS_COLLISION_ENTRY *)buffer->addr;
    buffer->addr += maxdebkeys * sizeof(DEBRIS_COLLISION_ENTRY);
    toruscolldata = (DEBRIS_COLLISION_ENTRY *)buffer->addr;
    buffer->addr += maxdebkeys * sizeof(DEBRIS_COLLISION_ENTRY);
    debscale = (DEBRIS_SCALE_CACHE *)buffer->addr;
    buffer->addr += EDPP_SCALE_TYPES * sizeof(DEBRIS_SCALE_CACHE);
    memset(freedebkeys, 0, maxdebkeys * sizeof(i16));
    memset(debkeydata, 0, maxdebkeys * sizeof(DEBRISKEY));
    memset(effecttypes, 0, debinf_count * sizeof(DEBRIS_INFO));
    memset(debtab, 0, debinf_count * sizeof(DEBRIS_INFO *));
    memset(debscale, 0, EDPP_SCALE_TYPES * sizeof(u32));
    for (i = 0; i < maxdebkeys; ++i) {
        spherecolldata[i].key_index = i;
        spherecolldata[i].flags = 3;
        toruscolldata[i].key_index = i;
        toruscolldata[i].flags = 3;
    }
    effecttypes[0] = DebrisNullEffect;
    debtab[0] = effecttypes;
    edpp_types_used = 1;
    numspherecolldata = maxdebkeys;
    numtoruscolldata = maxdebkeys;
    DebMat[0] = CreateAlphaBlendTexture(buffer, buffer_end, texture_name, 1, 2, 256, 0);
    texture_id = ((DEBRIS_MATERIAL *)DebMat[0])->texture_id;
    ((DEBRIS_MATERIAL *)DebMat[0])->attribs.z_mode = NUMTL_ZMODE_READ_ONLY;
    ((DEBRIS_MATERIAL *)DebMat[0])->attribs.alpha_test = 1;
    NuMtlUpdate(DebMat[0]);
    DebMat[1] = CreateCopyMat(DebMat[0], 0, 0, 3, 0);
    DebMat[2] = CreateCopyMat(DebMat[0], 1, 1, 1, 1);
    DebMat[3] = CreateCopyMat(DebMat[0], 1, 3, 1, 1);
    DebMat[4] = CreateCopyMat(DebMat[0], 0, 1, 1, 1);
    DebMat[5] = CreateCopyMat(DebMat[0], 0, 2, 1, 1);
    DebMat[6] = CreateCopyMat(DebMat[0], 0, 0, 0, 1);
    ((DEBRIS_MATERIAL *)DebMat[0])->vertex_flags |= 0x100000;
    ((DEBRIS_MATERIAL *)DebMat[1])->vertex_flags |= 0x100000;
    ((DEBRIS_MATERIAL *)DebMat[2])->vertex_flags |= 0x100000;
    ((DEBRIS_MATERIAL *)DebMat[3])->vertex_flags |= 0x100000;
    ((DEBRIS_MATERIAL *)DebMat[4])->vertex_flags |= 0x100000;
    ((DEBRIS_MATERIAL *)DebMat[5])->vertex_flags |= 0x100000;
    ((DEBRIS_MATERIAL *)DebMat[6])->vertex_flags |= 0x100000;
    DebMat[6]->texture_id = texture_id;
    DebMat[5]->texture_id = texture_id;
    DebMat[4]->texture_id = texture_id;
    DebMat[3]->texture_id = texture_id;
    DebMat[2]->texture_id = texture_id;
    DebMat[1]->texture_id = texture_id;
    DebMat[0]->texture_id = texture_id;
    DebMat[7] = NuMtlCreateEx3D(1, 2);
    ((DEBRIS_MATERIAL *)DebMat[7])->diffuse.x = 1.0f;
    ((DEBRIS_MATERIAL *)DebMat[7])->diffuse.y = 1.0f;
    ((DEBRIS_MATERIAL *)DebMat[7])->diffuse.z = 1.0f;
    ((DEBRIS_MATERIAL *)DebMat[7])->attribs.cull_mode = NUMTL_CULL_NONE;
    ((DEBRIS_MATERIAL *)DebMat[7])->attribs.z_mode = NUMTL_ZMODE_READ_ONLY;
    ((DEBRIS_MATERIAL *)DebMat[7])->attribs.filter_mode = 0;
    ((DEBRIS_MATERIAL *)DebMat[7])->attribs.lighting = NUMTL_LIGHTING_NONE;
    ((DEBRIS_MATERIAL *)DebMat[7])->attribs.vertex_colour = 1;
    ((DEBRIS_MATERIAL *)DebMat[7])->attribs.u_address = NUMTL_ADDRESS_CLAMP;
    ((DEBRIS_MATERIAL *)DebMat[7])->attribs.v_address = NUMTL_ADDRESS_CLAMP;
    ((DEBRIS_MATERIAL *)DebMat[7])->opacity = 0.999f;
    ((DEBRIS_MATERIAL *)DebMat[7])->attribs.alpha_mode = NUMTL_ALPHA_BLEND;
    ((DEBRIS_MATERIAL *)DebMat[7])->secondary.alpha_fail = 1;
    ((DEBRIS_MATERIAL *)DebMat[7])->texture_id = 1;
    ((DEBRIS_MATERIAL *)DebMat[7])->sort_priority = 128;
    ((DEBRIS_MATERIAL *)DebMat[7])->vertex_flags |= 0x100000;
    ((DEBRIS_MATERIAL *)DebMat[7])->glass_parameter = 0.25f;
    ((DEBRIS_MATERIAL *)DebMat[7])->particle_type_tag = -105;
    NuMtlUpdate(DebMat[7]);
    DebMat[8] = CreateCopyMat(DebMat[0], 0, 6, 0, 1);
    for (i = 0; debtab[i] != NULL; ++i) {
        if (((DEBRIS_ANIMATION_EFFECT *)debtab[i])->frame_divisor == 0) {
            ((DEBRIS_ANIMATION_EFFECT *)debtab[i])->frame_divisor = 1;
        }
        ((DEBRIS_ANIMATION_EFFECT *)debtab[i])->animation_rate.rate =
            (f32)((((DEBRIS_ANIMATION_EFFECT *)debtab[i])->texture_frames * 60) / ((DEBRIS_ANIMATION_EFFECT *)debtab[i])->frame_divisor);
        ((DEBRIS_ANIMATION_EFFECT *)debtab[i])->animation_rate.packed_rate =
            (((DEBRIS_ANIMATION_EFFECT *)debtab[i])->animation_rate.packed_rate & 0xfffff800) +
            (((u16)((DEBRIS_ANIMATION_EFFECT *)debtab[i])->texture_frames * (((DEBRIS_ANIMATION_EFFECT *)debtab[i])->texture_rows - 1)) &
             0x3ff);
        debtab[i]->alpha_keys[0].value /= debtab[i]->particle_lifetime;
        debtab[i]->texture_u0 += 524288.0;
        debtab[i]->texture_u1 += 524288.0;
        debtab[i]->texture_v0 += 524288.0;
        debtab[i]->texture_v1 += 524288.0;
    }
    for (i = 0; i < debinf_count; ++i) {
        effecttypes[i].native_data = NULL;
    }
    freeDmaDebType = 0;
    if (debris_suspended == 0) {
        if (trash_buffer != NULL) {
            DebrisTrashableSetup(trash_buffer, &trash_end);
        } else {
            DebrisTrashableSetup(buffer, &buffer_end);
        }
    }
    for (i = 0; i < maxdebkeys; ++i) {
        freedebkeys[i] = (i16)i;
    }
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

void PartTimeSlip(void) {
    i32 i;
    PARTTYPE_LOOKUP *types = part_types;
    for (i = 0; i < 40; ++i) {
        if (part_emits[i].effect_id != -1) {
            part_emits[i].time_20 -= 800.0f;
            part_emits[i].time_24 -= 800.0f;
            part_emits[i].time_28 -= 800.0f;
        }
    }
    for (i = 0; i < 128; ++i) {
        if (types[i].effect_ids[0] != -1) {
            types[i].last_used_time -= 800.0f;
        }
    }
    partglobaltime -= 800.0f;
}

void PartCleanupTypes(void) {
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
        if (++PartCleanupCursorPrivate >= 128) {
            PartCleanupCursorPrivate = 0;
        }
    }
}

i32 CreateScaledPARTEffect(i32 effect_index, f32 requested_scale) {
    i32 closest_index;
    i32 i;
    i32 result = 0;
    f32 ratio;
    f32 closest_distance;
    // Inclusive upper check; loaded type IDs are 0..127.
    if (effect_index < 1) {
        return -1;
    }
    if (effect_index > 128) {
        return -1;
    }
    if (part_types[effect_index].effect_ids[0] == -1) {
        return -1;
    }
    if (part_types[effect_index].scale != 1.0f) {
        // The final word identifies the original, unscaled type.
        effect_index = part_types[effect_index].source_type;
        if (part_types[effect_index].effect_ids[0] == -1) {
            return -1;
        }
    }
    if (requested_scale == 1.0f) {
        result = effect_index;
    }
    if (requested_scale < 0.01f) {
        requested_scale = 0.01f;
    }

    if (result == 0) {
        ratio = requested_scale / part_types[effect_index].scale;
        if (ratio <= 1.0f) {
            ratio = 1.0f - ratio;
        } else {
            ratio = ratio - 1.0f;
        }
        closest_index = effect_index;
        closest_distance = ratio;
        for (i = 0; i < 128; ++i) {
            if (part_types[i].effect_ids[0] != -1 && part_types[i].source_type == (u32)effect_index) {
                ratio = requested_scale / part_types[i].scale;
                ratio = ratio <= 1.0f ? 1.0f - ratio : ratio - 1.0f;
                if (ratio < closest_distance) {
                    closest_index = i;
                    closest_distance = ratio;
                }
            }
        }
        if (closest_index != 0 && closest_distance < 1.1f) {
            return closest_index;
        }
        for (i = 0; i < 128; ++i) {
            PARTTYPE_LOOKUP *scaled;
            PARTTYPE_LOOKUP *source;
            if (part_types[i].effect_ids[0] != -1) {
                continue;
            }
            scaled = &part_types[i];
            source = &part_types[effect_index];
            *scaled = *source;
            edpartScaleType(i, requested_scale);
            scaled->last_used_time = partglobaltime;
            scaled->scale = requested_scale;
            scaled->source_type = effect_index;
            if (strlen(source->name) > 12) {
                char shortened[16];
                sprintf(shortened, "%s", source->name);
                shortened[12] = '\0';
                sprintf(scaled->name, "%s%03d", shortened, i);
            } else {
                sprintf(scaled->name, "%s%03d", source->name, i);
            }
            ++part_types_used;
            result = i;
            break;
        }
        if (result == 0) {
            result = closest_index;
        }
    }
    return result;
}

i32 AddScaledFiniteShotPART(i32 effect, NUVEC *position, i32 count, f32 scale) {
    i32 scaled = CreateScaledPARTEffect(effect, scale);
    if (scaled != -1) {
        AddFiniteShotPART(scaled, position, count);
    }
    return scaled;
}

i32 edpartLookupObjectInScene(char *name, NUGSCN *scene) {
    nuhspecial_s special;
    i32 count;
    i32 i;
    char *special_name;
    if (name[0] == 0) {
        return -1;
    }
    if (NuStrNCmp(name, PartNullInstanceName, 16) == 0) {
        return 9999;
    }
    if (scene == NULL) {
        return -1;
    }
    count = NuGScnNumSpecials(scene);
    for (i = 0; i < count; ++i) {
        NuGScnGetSpecial(&special, scene, i);
        if (NuSpecialExistsFn(&special)) {
            special_name = NuSpecialGetName(&special);
            if (NuStrNCmp(name, special_name, 16) == 0) {
                return i;
            }
        }
    }
    return -1;
}

i32 fun_006489a0(char *name) {
    if (edpart_load_particle_page == -1) {
        return LookupDebrisEffect(name);
    }
    return LookupDebrisEffectPageOnly(name, edpart_load_particle_page);
}

void edpartLoadSingleType(PARTTYPE_LOOKUP *type, i32 version, i32 page) {
    char name[16];
    i32 i;
    i32 scene;
    char mode;
    EdFileRead(type->name, 16);
    if (version >= 6) {
        for (i = 0; i < 8; ++i) {
            type->effect_pages[i] = EdFileReadChar();
        }
    } else {
        if (version >= 2) {
            type->effect_pages[0] = EdFileReadChar();
        } else {
            type->effect_pages[0] = 0;
        }
        for (i = 1; i < 8; ++i) {
            type->effect_pages[i] = -1;
        }
    }
    if (version >= 6) {
        for (i = 0; i < 8; ++i) {
            EdFileRead(type->object_names[i], 16);
            type->effect_ids[i] = -1;
            if (type->effect_pages[i] == 0) {
                for (scene = 0; scene < 32; ++scene) {
                    if (part_scene_pageid[scene] == page) {
                        type->effect_ids[i] = edpartLookupObjectInScene(type->object_names[i], part_scene[scene]);
                    }
                    if (type->effect_ids[i] != -1) {
                        type->scene_indices[i] = scene;
                        break;
                    }
                }
            } else if (type->effect_pages[i] == 1) {
                for (scene = 0; scene < 32; ++scene) {
                    if (part_scene_pageid[scene] == 0) {
                        type->effect_ids[i] = edpartLookupObjectInScene(type->object_names[i], part_scene[scene]);
                    }
                    if (type->effect_ids[i] != -1) {
                        type->scene_indices[i] = scene;
                        break;
                    }
                }
            }
            if (type->effect_ids[i] == -1 && type->object_names[i][0] != 0) {
                type->effect_ids[i] = 9998;
            }
        }
    } else {
        EdFileRead(type->object_names[0], 16);
        type->effect_ids[0] = -1;
        if (type->effect_pages[0] == 0) {
            for (scene = 0; scene < 32; ++scene) {
                if (part_scene_pageid[scene] == page) {
                    type->effect_ids[0] = edpartLookupObjectInScene(type->object_names[0], part_scene[scene]);
                }
                if (type->effect_ids[0] != -1) {
                    type->scene_indices[0] = scene;
                    break;
                }
            }
        } else if (type->effect_pages[0] == 1) {
            for (scene = 0; scene < 32; ++scene) {
                if (part_scene_pageid[scene] == 0) {
                    type->effect_ids[0] = edpartLookupObjectInScene(type->object_names[0], part_scene[scene]);
                }
                if (type->effect_ids[0] != -1) {
                    type->scene_indices[0] = scene;
                    break;
                }
            }
        }
        if (type->effect_ids[0] == -1 && type->object_names[0][0] != 0) {
            type->effect_ids[0] = 9998;
        }
    }
    type->variant_count = 0;
    for (i = 0; i < 8; ++i) {
        if (type->effect_ids[i] != -1 && type->effect_ids[i] != 9999) {
            ++type->variant_count;
        }
    }
    if (version >= 6) {
        type->variant_mode = EdFileReadChar();
    } else {
        type->variant_mode = 0;
    }
    if (version >= 16) {
        type->particle_scale = EdFileReadFloat();
        type->effect_scale = EdFileReadFloat();
    } else {
        type->particle_scale = 1.0f;
        type->effect_scale = 1.0f;
    }
    type->lifetime = EdFileReadFloat();
    if (version >= 14) {
        type->lifetime_random = EdFileReadFloat();
    } else {
        type->lifetime_random = 0.0f;
    }
    type->speed = EdFileReadFloat();
    type->gravity = EdFileReadFloat();
    type->emission_rate = EdFileReadFloat();
    if (version >= 8) {
        type->bounce = EdFileReadFloat();
    } else {
        type->bounce = 0.75f;
    }
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
    if (version >= 13) {
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
    if (type->effect_ids[0] == 9999) {
        type->flags |= 0x10;
    }
    if (version >= 9) {
        EdFileRead(name, 16);
        type->trail_effects[0] =
            (edpart_load_particle_page == -1 ? LookupDebrisEffect(name) : LookupDebrisEffectPageOnly(name, (i8)edpart_load_particle_page));
        EdFileRead(name, 16);
        type->trail_effects[1] =
            (edpart_load_particle_page == -1 ? LookupDebrisEffect(name) : LookupDebrisEffectPageOnly(name, (i8)edpart_load_particle_page));
        EdFileRead(name, 16);
        type->attached_effect =
            (edpart_load_particle_page == -1 ? LookupDebrisEffect(name) : LookupDebrisEffectPageOnly(name, (i8)edpart_load_particle_page));
        type->trail_rates[0] = EdFileReadFloat();
        type->trail_rates[1] = EdFileReadFloat();
    } else if (version >= 5) {
        EdFileRead(name, 16);
        type->trail_effects[0] =
            (edpart_load_particle_page == -1 ? LookupDebrisEffect(name) : LookupDebrisEffectPageOnly(name, (i8)edpart_load_particle_page));
        type->trail_effects[1] = -1;
        EdFileRead(name, 16);
        type->attached_effect =
            (edpart_load_particle_page == -1 ? LookupDebrisEffect(name) : LookupDebrisEffectPageOnly(name, (i8)edpart_load_particle_page));
        type->trail_rates[0] = EdFileReadFloat();
        type->trail_rates[1] = 60.0f;
    } else if (version >= 3) {
        type->trail_effects[0] = -1;
        type->trail_effects[1] = -1;
        type->attached_effect = -1;
        mode = EdFileReadChar();
        EdFileRead(name, 16);
        if (mode == 0) {
            type->trail_effects[0] = (edpart_load_particle_page == -1 ? LookupDebrisEffect(name)
                                                                      : LookupDebrisEffectPageOnly(name, (i8)edpart_load_particle_page));
        } else if (mode == 1) {
            type->attached_effect = (edpart_load_particle_page == -1 ? LookupDebrisEffect(name)
                                                                     : LookupDebrisEffectPageOnly(name, (i8)edpart_load_particle_page));
        }
        type->trail_rates[0] = EdFileReadFloat();
        type->trail_rates[1] = 60.0f;
    } else {
        type->trail_effects[0] = -1;
        type->trail_effects[1] = -1;
        type->attached_effect = -1;
        type->trail_rates[0] = 60.0f;
        type->trail_rates[1] = 60.0f;
    }
    if (version >= 4) {
        EdFileRead(name, 16);
        type->kill_effect =
            (edpart_load_particle_page == -1 ? LookupDebrisEffect(name) : LookupDebrisEffectPageOnly(name, (i8)edpart_load_particle_page));
        EdFileRead(name, 16);
        type->impact_effect =
            (edpart_load_particle_page == -1 ? LookupDebrisEffect(name) : LookupDebrisEffectPageOnly(name, (i8)edpart_load_particle_page));
    } else {
        type->impact_effect = -1;
        type->kill_effect = -1;
    }
    if (version >= 12) {
        EdFileRead(type->impact_part_name, 16);
        type->impact_part = PARTLookupType(type->impact_part_name);
    } else {
        type->impact_part_name[0] = 0;
        type->impact_part = -1;
    }
    if (version >= 7) {
        for (i = 0; i < 4; ++i) {
            EdFileRead(name, 16);
            type->sounds[i] = GetSfxIdN(name, 16);
        }
        for (i = 0; i < 4; ++i) {
            type->sound_modes[i] = EdFileReadChar();
        }
    } else {
        for (i = 0; i < 4; ++i) {
            type->sounds[i] = -1;
            type->sound_modes[i] = 0;
        }
    }
    if (version >= 10) {
        type->maximum_distance = EdFileReadFloat();
    } else {
        type->maximum_distance = 25.0f;
    }
    if (version >= 15) {
        type->lighting_scale.x = EdFileReadFloat();
        type->lighting_scale.y = EdFileReadFloat();
        type->lighting_scale.z = EdFileReadFloat();
    } else {
        type->lighting_scale.x = 1.0f;
        type->lighting_scale.y = 1.0f;
        type->lighting_scale.z = 1.0f;
    }
    type->scale = 1.0f;
    type->source_type = 0;
}

void edpartDetermineNearest(f32 distance) {
    NUVEC delta;
    f32 squared_distance;
    i32 i;
    if (edpart_nearest != -1 && part_emits[edpart_nearest].effect_id != -1) {
        NuVecSub(&delta, &PartEditorCameraPos, &part_emits[edpart_nearest].position);
        squared_distance = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        if (squared_distance == 0.0f) {
            return;
        }
    }
    edpart_nearest = -1;
    for (i = 0; i < 40; ++i) {
        if (part_emits[i].effect_id != -1) {
            NuVecSub(&delta, &PartEditorCameraPos, &part_emits[i].position);
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
        if (edpart_nearest_emit->effect_id != -1) {
            edpart_nearest_type = &part_types[edpart_nearest_emit->effect_id];
        }
    }
}

void edpartChangeNameMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_create_type != -1) {
        PARTTYPE_LOOKUP *type = &part_types[edpart_create_type];
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
    if (edpart_nearest_type) {
        edpart_nearest_type->maximum_distance = ((edui_slider_s *)(item))->value;
    }
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
                PARTTYPE_LOOKUP *source = &part_types[edpart_create_type];
                PARTTYPE_LOOKUP *copy = &part_types[index];
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
    if (menu->callback) {
        menu->callback(menu, parent);
    }
}

void edpartChangeTintR(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type) {
        edpart_nearest_type->lighting_scale.x = ((edui_slider_s *)item)->value;
    }
}

void edpartChangeTintG(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type) {
        edpart_nearest_type->lighting_scale.y = ((edui_slider_s *)item)->value;
    }
}

void edpartChangeTintB(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type) {
        edpart_nearest_type->lighting_scale.z = ((edui_slider_s *)item)->value;
    }
}

void edpartCancelTintMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_tint_menu);
    edpart_tint_menu = NULL;
}

void edpartChangeInstanceScale(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_nearest_type) {
        edpart_nearest_type->particle_scale = ((edui_slider_s *)item)->value;
    }
}

void edpartCancelInstanceScaleMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_instancescale_menu);
    edpart_instancescale_menu = NULL;
}

void edpartTintMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type != NULL) {
        edpart_tint_menu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, edpartCancelTintMenu, "Instance Tint");
        if (edpart_tint_menu != NULL) {
            eduiMenuAddItem(edpart_tint_menu, eduiItemSliderCreate(0, edblack, 0, edpartChangeTintR, 0.0f, 2.0f,
                                                                   edpart_nearest_type->lighting_scale.x, "Red Tint"));
            eduiMenuAddItem(edpart_tint_menu, eduiItemSliderCreate(0, edblack, 0, edpartChangeTintG, 0.0f, 2.0f,
                                                                   edpart_nearest_type->lighting_scale.y, "Green Tint"));
            eduiMenuAddItem(edpart_tint_menu, eduiItemSliderCreate(0, edblack, 0, edpartChangeTintB, 0.0f, 2.0f,
                                                                   edpart_nearest_type->lighting_scale.z, "Blue Tint"));
            eduiMenuAttach(menu, edpart_tint_menu);
            edpart_tint_menu->x = menu->x + 10;
            edpart_tint_menu->y = menu->y + 40;
        }
    }
}

void edpartInstanceScaleMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    edpart_instancescale_menu = eduiMenuCreate(70, 70, 180, 300, ed_fnt, edpartCancelInstanceScaleMenu, "Instance Scale");
    if (edpart_instancescale_menu != NULL && edpart_nearest_type != NULL) {
        eduiMenuAddItem(edpart_instancescale_menu,
                        eduiItemSliderCreate(0, edblack, 0, edpartChangeInstanceScale, 0.0f, edpart_superscale * 10.0f,
                                             edpart_nearest_type->particle_scale, "Instance Scale"));
        eduiMenuAttach(menu, edpart_instancescale_menu);
        edpart_instancescale_menu->x = menu->x + 10;
        edpart_instancescale_menu->y = menu->y + 40;
    }
}

void edpartCancelInstanceSettingsMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_instancesettings_menu);
    edpart_instancesettings_menu = NULL;
}

void edpartInstanceSettingsMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (edpart_nearest_type == NULL) {
        return;
    }
    edpart_instancesettings_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edpartCancelInstanceSettingsMenu, "Instance Settings");
    if (edpart_instancesettings_menu != NULL) {
        eduiMenuAddItem(edpart_instancesettings_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartChangeMaxLifeMenu, "Max Instance Life..."));
        eduiMenuAddItem(edpart_instancesettings_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edpartInstanceOrientMenu, "Instance Orientation..."));
        eduiMenuAddItem(edpart_instancesettings_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartInstanceFlagsMenu, "Instance Flags..."));
        eduiMenuAddItem(edpart_instancesettings_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartTintMenu, "Instance Tint..."));
        eduiMenuAddItem(edpart_instancesettings_menu, eduiItemSelCreate(1, edblack, 0, 0, edpartInstanceScaleMenu, "Instance Scale..."));
    }
    eduiMenuAttach(menu, edpart_instancesettings_menu);
    edpart_instancesettings_menu->x = menu->x + 10;
    edpart_instancesettings_menu->y = menu->y + 40;
}

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
        if (part_emits[index].instance_id == -1 || part_emits[index].instance_id == 99999) {
            continue;
        }
        NuVecSub(&delta, &PartEditorCameraPos, &part_emits[index].position);
        temp.value = delta.x;
        abs_delta.bits[0] = temp.bits & 0x7fffffff;
        if (!(abs_delta.value[0] <= edpart_copy_size)) {
            continue;
        }
        temp.value = delta.y;
        abs_delta.bits[1] = temp.bits & 0x7fffffff;
        if (!(abs_delta.value[1] <= edpart_copy_size)) {
            continue;
        }
        temp.value = delta.z;
        abs_delta.bits[2] = temp.bits & 0x7fffffff;
        if (!(abs_delta.value[2] <= edpart_copy_size)) {
            continue;
        }
        edpart_copy_source[edpart_copy_source_count++] = index;
        if (edpart_copy_source_count == 8) {
            break;
        }
    }
    edpart_copy_source_vec = PartEditorCameraPos;
    edpart_copyroty = 0;
}

void edpartHighlightNearest(void) {
    extern i32 edpart_copy_mode;
    extern i32 edpart_copyroty;
    extern i32 edpart_copy_enclosed;
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
            if (part_emits[index].instance_id == -1 || part_emits[index].instance_id == 99999) {
                continue;
            }
            NuVecSub(&delta, &PartEditorCameraPos, &part_emits[index].position);
            temp.value = delta.x;
            abs_delta.bits[0] = temp.bits & 0x7fffffff;
            if (!(abs_delta.value[0] <= edpart_copy_size)) {
                continue;
            }
            temp.value = delta.y;
            abs_delta.bits[1] = temp.bits & 0x7fffffff;
            if (!(abs_delta.value[1] <= edpart_copy_size)) {
                continue;
            }
            temp.value = delta.z;
            abs_delta.bits[2] = temp.bits & 0x7fffffff;
            if (!(abs_delta.value[2] <= edpart_copy_size)) {
                continue;
            }
            ++edpart_copy_enclosed;
            edbitsDrawDiagonalCross(part_emits[index].position.x, part_emits[index].position.y, part_emits[index].position.z, 0.05f, -1,
                                    edpart_mtl);
        }
        if (edpart_copy_source_count > 0) {
            for (index = 0; index < edpart_copy_source_count; ++index) {
                NuVecSub(&offset, &part_emits[edpart_copy_source[index]].position, &edpart_copy_source_vec);
                NuVecRotateY(&offset, &offset, edpart_copyroty);
                NuVecAdd(&destination, &PartEditorCameraPos, &offset);
                edbitsDrawDiagonalCross(destination.x, destination.y, destination.z, 0.05f, 0xff808080, edpart_mtl);
            }
        }
    } else if (edpart_nearest != -1) {
        edbitsDrawCube(part_emits[edpart_nearest].position.x, part_emits[edpart_nearest].position.y, part_emits[edpart_nearest].position.z,
                       0.5f, 0.5f, 0.5f, 0, 0, 0, 0, 0, -1, edpart_mtl);
    }
}

void edpartRender() {
    edcamSet();
    edpartDrawCursor();
    edpartHighlightNearest();
    if (edpart_active_menu) {
        eduiMenuRender(edpart_active_menu);
    }
}

void edpartChangeName(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edpart_create_type != -1) {
        i32 index;
        char *name = ((edui_textpicker_s *)(item))->value;
        NuStrNCpy(part_types[edpart_create_type].name, name, sizeof(part_types[edpart_create_type].name));
        for (index = 0; index < 40; ++index) {
            if (part_emits[index].effect_id == edpart_create_type) {
                NuStrNCpy(part_emits[index].name, name, sizeof(part_emits[index].name));
            }
        }
    }
}

void edpartCancelChangeNameMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edpart_name_menu);
    edpart_name_menu = NULL;
}

i32 PartGeneralPage = 0;

i32 edpartLoadPageEx(char *path, i8 mode, NUGSCN **scenes, i32 scene_count) {
    i32 page;
    i32 scene_slot;
    i32 i;
    i32 version;
    i32 type_count;
    i32 skipped_types;
    i32 type_slot;
    i32 emitter_count;
    i32 emitter_slot;
    i32 sound;
    PARTTYPE_LOOKUP discarded_type;
    if (mode == 0) {
        page = 0;
    } else {
        page = -1;
        for (i = 1; i < 8; ++i) {
            if (part_page_used[i] == 0) {
                page = i;
                break;
            }
        }
        if (page == -1) {
            return -1;
        }
    }
    scene_slot = 0;
    for (i = 0; i < scene_count; ++i) {
        if (scenes[i] != NULL) {
            while (part_scene[scene_slot] != NULL && scene_slot < 32) {
                ++scene_slot;
            }
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
        // MATCH-EXCEPTION: the original tests the low and high version bounds separately; one && folds them
        // into a single unsigned range compare.
        if (version < 1) {
            EdFileClose();
            return -1;
        }
        if (version <= 16) {
            type_count = EdFileReadInt();
            type_slot = 0;
            skipped_types = 0;
            if (type_count + part_types_used > 128) {
                skipped_types = type_count + part_types_used - 128;
                type_count -= skipped_types;
            }
            for (i = 0; i < type_count; ++i) {
                while (part_types[type_slot].name[0] != 0) {
                    ++type_slot;
                }
                edpartLoadSingleType(&part_types[type_slot], version, page);
                if (part_types[type_slot].name[0] != 0) {
                    part_types[type_slot].page = page;
                    part_types[type_slot].effect_list = mode;
                    ++part_types_used;
                }
            }
            for (i = 0; i < skipped_types; ++i) {
                edpartLoadSingleType(&discarded_type, version, page);
            }
            emitter_count = EdFileReadInt();
            emitter_slot = 0;
            if (emitter_count + part_emits_used > 40) {
                emitter_count = 40 - part_emits_used;
            }
            for (i = 0; i < emitter_count; ++i) {
                while (part_emits[emitter_slot].effect_id != -1) {
                    ++emitter_slot;
                }
                part_emits[emitter_slot].position.x = EdFileReadFloat();
                part_emits[emitter_slot].position.y = EdFileReadFloat();
                part_emits[emitter_slot].position.z = EdFileReadFloat();
                EdFileRead(part_emits[emitter_slot].name, 16);
                part_emits[emitter_slot].effect_id = PARTLookupTypePageOnly(part_emits[emitter_slot].name, page);
                part_emits[emitter_slot].rotation_30 = EdFileReadShort();
                part_emits[emitter_slot].rotation_2e = EdFileReadShort();
                part_emits[emitter_slot].rotation_2c = EdFileReadShort();
                if (version >= 11) {
                    part_emits[emitter_slot].unknown_44 = EdFileReadShort();
                    part_emits[emitter_slot].unknown_46 = EdFileReadShort();
                } else {
                    part_emits[emitter_slot].unknown_44 = 0;
                    part_emits[emitter_slot].unknown_46 = 0;
                }
                part_emits[emitter_slot].page = page;
                part_emits[emitter_slot].unknown_3c = 0;
                for (sound = 0; sound < 4; ++sound) {
                    if (part_types[part_emits[emitter_slot].effect_id].sounds[sound] != -1 &&
                        part_types[part_emits[emitter_slot].effect_id].sound_modes[sound] != 0) {
                        part_emits[emitter_slot].unknown_3c = 1;
                        break;
                    }
                }
                if (part_emits[emitter_slot].effect_id != -1) {
                    ++part_emits_used;
                    PARTStartOffset(emitter_slot, 0.0f);
                    part_emits[emitter_slot].unknown_3d = 1;
                }
            }
            EdFileSetReadWrongEndianess(0);
            EdFileClose();
            edpartDetermineNearest(1.0f);
            part_page_used[page] = 1;
            if (page == 0) {
                part_page_on[0] = 1;
            }
            if (mode == 0) {
                PartGeneralPage = page;
            } else if (mode == 1) {
                PartLevelPage = page;
            }
            return page;
        }
        EdFileClose();
    }
    return -1;
}

void RemovePARTEffect(i32 index) {
    if (index != -1 && part_emits[index].effect_id != -1) {
        --part_emits_used;
        part_emits[index].effect_id = -1;
    }
}

void PARTEmitterPos(i32 index, f32 x, f32 y, f32 z) {
    if (index != -1 && part_emits[index].effect_id != -1) {
        part_emits[index].position.x = x;
        part_emits[index].position.y = y;
        part_emits[index].position.z = z;
    }
}

void PARTEmitterOrientation(i32 index, i16 emitrotz, i16 emitroty, i16 emitrotx) {
    if (index != -1 && part_emits[index].effect_id != -1) {
        part_emits[index].rotation_30 = emitrotx;
        part_emits[index].rotation_2e = emitroty;
        part_emits[index].rotation_2c = emitrotz;
    }
}

void UpdatePartEmits(f32 time) {
    i32 switch_changes[32][2];
    i32 i;
    i32 sound;
    i32 axis;
    i32 remaining;
    i32 variant;
    PART_EMIT *emitter;
    PARTTYPE_LOOKUP *type;
    ADDPART params;
    NUVEC position;
    NUVEC velocity;
    NUMTX matrix;
    nuhspecial_s special;
    f32 interval;
    f32 shift;
    f32 previous_distance;
    f32 emission_time;
    f32 pause;
    f32 range;
    i16 angle_x;
    i16 angle_y;
    i16 angle_z;
    i32 last_switch_change = -1;
    for (i = 0; i < 40; ++i) {
        emitter = &part_emits[i];
        if (part_page_on[emitter->page] == 0 || emitter->effect_id == -1) {
            continue;
        }
        type = &part_types[emitter->effect_id];
        if (type->effect_ids[0] == -1) {
            continue;
        }
        interval = 1.0f / type->emission_rate;
        if (emitter->unknown_44 == 1 && emitter->unknown_3d != 2) {
            if (emitter->unknown_46 != -1) {
                switch (debris_object_switches[emitter->unknown_46]) {
                    case 0:
                        emitter->unknown_3d = 0;
                        break;
                    case 1:
                        emitter->unknown_3d = 1;
                        break;
                    case 2:
                        last_switch_change = (last_switch_change + 1) & 31;
                        emitter->unknown_3d = 2;
                        switch_changes[last_switch_change][1] = 4;
                        switch_changes[last_switch_change][0] = emitter->unknown_46;
                        break;
                    case 3:
                        emitter->unknown_3d = 2;
                        last_switch_change = (last_switch_change + 1) & 31;
                        switch_changes[last_switch_change][1] = 0;
                        switch_changes[last_switch_change][0] = emitter->unknown_46;
                        break;
                    case 4:
                        last_switch_change = (last_switch_change + 1) & 31;
                        switch_changes[last_switch_change][1] = 0;
                        switch_changes[last_switch_change][0] = emitter->unknown_46;
                        break;
                }
            }
            if (emitter->unknown_3d == 2) {
                PARTStartOffset(i, 0.0f);
                shift = emitter->time_24 - partglobaltime;
                emitter->time_24 -= shift;
                emitter->time_28 -= shift;
                emitter->time_20 = partglobaltime - interval;
            }
        }
        previous_distance = emitter->camera_distance;
        emitter->camera_distance = CameraEmitterDistance(&emitter->position);
        if (emitter->unknown_3c != 0) {
            for (sound = 0; sound < 4; ++sound) {
                if (type->sounds[sound] != -1) {
                    SetSfxBit_On(type->sounds[sound]);
                    if (type->sound_modes[sound] == 4) {
                        PlaySfxById(type->sounds[sound], &emitter->position);
                    }
                }
            }
        }
        remaining = 0;
        if ((type->flags & 0x400000) != 0) {
            if (emitter->camera_distance < type->maximum_distance && previous_distance >= type->maximum_distance) {
                emitter->time_20 = partglobaltime - interval + 0.001f;
                emitter->time_24 = partglobaltime;
                emitter->time_28 = emitter->time_24 - 1.0f;
            } else if (emitter->time_24 > emitter->time_28) {
                emitter->time_28 += time;
                emitter->time_24 += time;
                emitter->time_20 += time;
            }
        }
        emission_time = emitter->time_20 + interval;
        while (partglobaltime + time > emission_time && remaining++ < 100) {
            while (emission_time >= emitter->time_28 && emission_time > emitter->time_24 && partglobaltime + time > emission_time) {
                if (emitter->time_24 >= emitter->time_28) {
                    emitter->time_28 =
                        NuRandFloatSeeded(&partseed) * type->emission_period_random + (type->emission_period + emitter->time_24);
                    if (emitter->unknown_3c != 0 && emitter->unknown_3d != 0) {
                        for (sound = 0; sound < 4; ++sound) {
                            if (type->sounds[sound] != -1 && type->sound_modes[sound] == 1) {
                                PlaySfxById(type->sounds[sound], &emitter->position);
                            }
                        }
                    }
                    if (emitter->shots_remaining != 0 && --emitter->shots_remaining == 0) {
                        RemovePARTEffect(i);
                        emission_time = partglobaltime + 99999.9f;
                    }
                } else {
                    pause = NuRandFloatSeeded(&partseed) * type->emission_pause_random + type->emission_pause;
                    emitter->time_24 = emitter->time_28 + pause;
                    emission_time = emitter->time_24 + 0.001f;
                    emitter->time_20 = emission_time - interval + 0.001f;
                    if (emitter->unknown_3d == 2) {
                        emitter->unknown_3d = 0;
                    }
                    if (emitter->unknown_3c != 0 && emitter->unknown_3d != 0) {
                        for (sound = 0; sound < 4; ++sound) {
                            if (type->sounds[sound] != -1 && type->sound_modes[sound] == 2) {
                                PlaySfxById(type->sounds[sound], &emitter->position);
                            }
                        }
                    }
                }
            }
            if (partglobaltime + time <= emission_time) {
                continue;
            }
            if (emitter->unknown_3d != 0 && (type->maximum_distance == 0.0f || type->maximum_distance > emitter->camera_distance)) {
                params = Default_ADDPART;
                params.unknown_c0 = type->particle_scale;
                position.x = NuRandFloatSeeded(&partseed) * 2.0f * type->position_random.x - type->position_random.x;
                position.y = NuRandFloatSeeded(&partseed) * 2.0f * type->position_random.y - type->position_random.y;
                position.z = NuRandFloatSeeded(&partseed) * 2.0f * type->position_random.z - type->position_random.z;
                NuVecRotateZ(&position, &position, emitter->rotation_2c);
                NuVecRotateY(&position, &position, emitter->rotation_2e);
                NuVecRotateX(&position, &position, emitter->rotation_30);
                NuVecAdd(&position, &position, &emitter->position);
                params.position = &position;
                velocity.x = 0.0f;
                velocity.y = type->speed;
                velocity.z = 0.0f;
                velocity.x += NuRandFloatSeeded(&partseed) * 2.0f * type->velocity_random.x - type->velocity_random.x;
                velocity.y += NuRandFloatSeeded(&partseed) * 2.0f * type->velocity_random.y - type->velocity_random.y;
                velocity.z += NuRandFloatSeeded(&partseed) * 2.0f * type->velocity_random.z - type->velocity_random.z;
                NuVecRotateZ(&velocity, &velocity, emitter->rotation_2c);
                NuVecRotateY(&velocity, &velocity, emitter->rotation_2e);
                NuVecRotateX(&velocity, &velocity, emitter->rotation_30);
                params.velocity = &velocity;
                params.gravity = type->gravity;
                params.unknown_20 = type->bounce;
                variant = 0;
                if (type->variant_count != 0) {
                    if (type->variant_mode == 0) {
                        variant = (i32)(NuRandFloatSeeded(&partseed) * 65535.0f) % type->variant_count;
                    } else if (type->variant_mode == 1) {
                        // MATCH-EXCEPTION: the original treats unknown_33 as signed; the header declares it u8.
                        if (++*(i8 *)&emitter->unknown_33 >= type->variant_count) {
                            emitter->unknown_33 = 0;
                        }
                        variant = *(i8 *)&emitter->unknown_33;
                    }
                }
                params.part_id = type->effect_ids[variant];
                if (type->effect_ids[variant] == 9999 || type->effect_ids[variant] == 9998) {
                    params.special = NULL;
                } else if (type->effect_ids[variant] != -1) {
                    params.special = &special;
                    NuGScnGetSpecial(&special, part_scene[type->scene_indices[variant]], params.part_id);
                }
                params.unknown_a4 = type->lifetime + NuRandFloatSeeded(&partseed) * type->lifetime_random;
                NuMtxSetIdentity(&matrix);
                range = (f32)type->rotation_random[0];
                angle_x = (i16)(i32)(NuRandFloatSeeded(&partseed) * 2.0f * range + type->rotation[0] - range);
                range = (f32)type->rotation_random[1];
                angle_y = (i16)(i32)(NuRandFloatSeeded(&partseed) * 2.0f * range + type->rotation[1] - range);
                range = (f32)type->rotation_random[2];
                angle_z = (i16)(i32)(NuRandFloatSeeded(&partseed) * 2.0f * range + type->rotation[2] - range);
                for (axis = 0; axis < 3; ++axis) {
                    params.unknown_a8[axis] = type->rotation[axis];
                    params.unknown_a8[axis + 3] = type->rotation_random[axis];
                }
                NuMtxRotateX(&matrix, angle_x);
                NuMtxRotateY(&matrix, angle_y);
                NuMtxRotateZ(&matrix, angle_z);
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
                params.unknown_68 = type->trail_rates[0];
                params.unknown_6c = type->trail_rates[1];
                params.effect_id_70 = type->impact_effect;
                if (type->impact_part == -1 && type->impact_part_name[0] != 0) {
                    type->impact_part = PARTLookupType(type->impact_part_name);
                }
                params.effect_id_74 = type->impact_part;
                params.effect_scale = type->effect_scale;
                params.lighting_scale_x = type->lighting_scale.x;
                params.lighting_scale_y = type->lighting_scale.y;
                params.lighting_scale_z = type->lighting_scale.z;
                params.effect_id_78 = type->kill_effect;
                AddPart(&params);
                if (emitter->unknown_3c != 0) {
                    for (sound = 0; sound < 4; ++sound) {
                        if (type->sounds[sound] != -1 && type->sound_modes[sound] == 3) {
                            PlaySfxById(type->sounds[sound], &emitter->position);
                        }
                    }
                }
                type->last_used_time = emission_time;
            }
            emitter->time_20 = emission_time;
            emission_time += interval;
        }
    }
    for (i = 0; i <= last_switch_change; ++i) {
        debris_object_switches[switch_changes[i][0]] = switch_changes[i][1];
    }
}

i32 PartRayCast(NUVEC *position, NUVEC *movement, f32 radius, i32 flags) {
    i32 hit = NewRayCast(position, movement, radius, flags);
    NewRayCastGetImpactNormal(&PartImpactNormalPrivate);
    return hit;
}

i32 PartPlatId(void) {
    return TerrainPlatId();
}

void CastPart(PART *part, f32 time) {
    PART_TERRAIN_STATE *state = (PART_TERRAIN_STATE *)part;
    i32 hit;
    f32 distance;
    f32 radius;
    NUVEC movement;
    NUVEC cast_movement;
    part->active &= ~PART_STATE_IMPACT_PENDING;
    part->unknown_f4 = time;
    movement.x = time * part->velocity.x;
    movement.y = time * time * (0.5 * part->gravity) + time * part->velocity.y;
    movement.z = time * part->velocity.z;
    if (part->velocity.y > 0.0f && movement.y <= 0.0f) {
        time = -part->velocity.y / part->gravity;
        part->unknown_f4 = time;
        movement.x = time * part->velocity.x;
        movement.y = time * time * (0.5 * part->gravity) + time * part->velocity.y;
        movement.z = time * part->velocity.z;
    }
    distance = NuVecMagSqr(&movement);
    if (distance < 0.001f || distance > 10000.0f || !_finite(distance) || _isnan(distance)) {
        state->impact_material = -1;
        return;
    }
    if ((part->active & (PART_STATE_SCALING_UP | PART_STATE_SCALING_DOWN)) == PART_STATE_SCALING_UP) {
        radius = (part->time_step + time) / PARTSCALEUPTIME * part->radius;
        if (radius > part->radius) {
            radius = part->radius;
        }
        cast_movement = movement;
        hit = PartRayCast(&part->position, &cast_movement, radius, 0);
        if (hit == 0) {
            state->impact_material = -1;
            return;
        }
        state->impact_material = NewRayCastGetImpactTerrainType();
        if ((NewRayCastGetTOFI() > 0.0f || PartImpactNormalPrivate.y > 0.5f) &&
            ((part->flags & 0x10000000) == 0 || NewRayCastHitWallSpline() != 4)) {
            goto impact;
        }
        if (hit > 0x10) {
            part->position.x -= PartImpactNormalPrivate.x * NewRayCastGetEmbedDist() * 1.05f;
            part->position.y -= PartImpactNormalPrivate.y * NewRayCastGetEmbedDist() * 1.05f;
            part->position.z -= PartImpactNormalPrivate.z * NewRayCastGetEmbedDist() * 1.05f;
        }
    }
    part->active &= ~(PART_STATE_SCALING_UP | PART_STATE_SCALING_DOWN);
    cast_movement = movement;
    hit = PartRayCast(&part->position, &cast_movement, part->unknown_e0, 0);
    if (hit == 0) {
        state->impact_material = -1;
        return;
    }
    state->impact_material = NewRayCastGetImpactTerrainType();
    if ((NewRayCastGetTOFI() > 0.0f || PartImpactNormalPrivate.y > 0.5f) &&
        ((part->flags & 0x10000000) == 0 || NewRayCastHitWallSpline() != 4)) {
        goto impact;
    }
    if (hit > 0x10) {
        part->position.x -= PartImpactNormalPrivate.x * NewRayCastGetEmbedDist() * 1.05f;
        part->position.y -= PartImpactNormalPrivate.y * NewRayCastGetEmbedDist() * 1.05f;
        part->position.z -= PartImpactNormalPrivate.z * NewRayCastGetEmbedDist() * 1.05f;
    }
    radius = (part->time_step - time) / PARTSCALEUPTIME * part->radius;
    if (!(radius > 0.0f)) {
        return;
    }
    part->active = (part->active & ~(PART_STATE_SCALING_UP | PART_STATE_SCALING_DOWN)) | PART_STATE_SCALING_DOWN;
    cast_movement = movement;
    if (PartRayCast(&part->position, &cast_movement, radius, 0) == 0) {
        state->impact_material = -1;
        return;
    }
    state->impact_material = NewRayCastGetImpactTerrainType();
    if ((part->flags & 0x10000000) != 0 && NewRayCastHitWallSpline() == 4) {
        return;
    }
impact:
    part->active |= PART_STATE_IMPACT_PENDING;
    state->platform_id = PartPlatId();
    part->active ^= (((u32)(state->platform_id != (u32)-1) << 4) ^ part->active) & 0x10;
    part->unknown_f4 *= NewRayCastGetTOFI();
    part->impact_position.x = part->position.x + cast_movement.x;
    part->impact_position.y = part->position.y + cast_movement.y;
    part->impact_position.z = part->position.z + cast_movement.z;
    part->impact_normal = PartImpactNormalPrivate;
}

void edpartDestroy(i32 index) {
    if (part_emits[index].effect_id != -1) {
        --part_emits_used;
        part_emits[index].effect_id = -1;
    }
}

void edpartCreate(NUVEC *position, i32 type) {
    AddPARTEffect(type, position);
}

i32 edpartPtlChangeType(i32 index, i32 type) {
    if (part_emits[index].effect_id != type) {
        edpartDestroy(index);
        AddDebrisEffect(&part_emits[index].instance_id, type, part_emits[index].position.x, part_emits[index].position.y,
                        part_emits[index].position.z);
        part_emits[index].effect_id = type;
    }
    return index;
}

void edpartDestroyAllEffects(void) {
    i32 i;
    // Walks 512 slots here although part_emits holds 40.
    for (i = 0; i < 512; ++i) {
        if (part_emits[i].effect_id != -1) {
            part_emits[i].effect_id = -1;
            --part_emits_used;
        }
    }
}

void edpartDoInput(FRAME_PAD *pad) {
    extern i32 edpart_copy_mode;
    extern i32 edpart_copyroty;
    extern i32 edpart_rotz;
    extern i32 edpart_roty;
    extern i32 edpart_emitrotz;
    extern i32 edpart_emitroty;
    extern i32 edpart_emitrotx;
    extern i32 edpart_refrotz;
    extern i32 edpart_refroty;
    extern i32 edpart_dpad_mode;
    extern i32 edpart_create_type;
    extern i32 edpart_nearest_orphans;
    extern i32 edpart_nearest_duplicates;
    extern i32 edpart_snap_enabled;
    extern i32 edpart_cam_ax;
    extern i32 edpart_cam_ay;
    extern i8 edpart_effect_list;
    extern f32 edpart_copy_size;
    extern f32 edpart_offset;
    i32 nearest;
    i32 variant;
    i32 previous;
    if ((pad->digital_buttons & PAD_BUTTON_SELECT) == 0) {
        edcamMove(pad);
    }
    if (pad->digital_buttons & PAD_BUTTON_SELECT) {
        nearest = edpart_nearest;
        if (nearest != -1) {
            if (pad->digital_buttons_pressed & PAD_BUTTON_R1) {
                do {
                    ++nearest;
                    if (nearest == 40) {
                        nearest = 0;
                    }
                } while (part_emits[nearest].effect_id == -1);
                edpart_nearest = nearest;
            }
            if (pad->digital_buttons_pressed & PAD_BUTTON_R2) {
                do {
                    --nearest;
                    if (nearest == -1) {
                        nearest = 39;
                    }
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
            edpart_effect_list = part_types[edpart_create_type].effect_list;
        }
    }
    if (edpart_snap_enabled) {
        edcamGetPosAngSnap(&PartEditorCameraPos, &edpart_cam_ax, &edpart_cam_ay);
    } else {
        edcamGetPosAng(&PartEditorCameraPos, &edpart_cam_ax, &edpart_cam_ay);
    }
    if ((pad->digital_buttons & PAD_BUTTON_SELECT) == 0) {
        if (pad->digital_buttons_pressed & PAD_BUTTON_SQUARE) {
            edpartOptMenu();
            edpart_active_menu = edpart_opt_menu;
        }
        if ((pad->digital_buttons_pressed & PAD_BUTTON_CROSS) && edpart_copy_mode == 0 && edpart_create_type != -1) {
            AddPARTEffect(edpart_create_type, &PartEditorCameraPos);
        }
        if (pad->digital_buttons & PAD_BUTTON_CIRCLE) {
            if (edpart_copy_mode != 0) {
                edpartMultipleCopyCopy();
            } else if (edpart_nearest != -1) {
                edpartPlace(edpart_nearest, &PartEditorCameraPos);
            }
        }
        if ((pad->digital_buttons & PAD_BUTTON_R3) && edpart_copy_mode == 0 && edpart_nearest != -1) {
            edpartPlace(edpart_nearest, &PartEditorCameraPos);
        }
        if (pad->digital_buttons_pressed & PAD_BUTTON_TRIANGLE) {
            if (edpart_copy_mode != 0) {
                edpart_copy_source_count = 0;
                edpart_copyroty = 0;
            } else {
                if (edpart_nearest != -1) {
                    edpartDestroy(edpart_nearest);
                }
                edpart_nearest = -1;
            }
        }
    }
    if (edpart_copy_mode != 0) {
        edpart_copy_size += pad->analog_left_pad_up / 5000.0f;
        edpart_copy_size -= pad->analog_left_pad_down / 5000.0f;
        if (edpart_copy_size < 0.05f) {
            edpart_copy_size = 0.05f;
        } else if (edpart_copy_size > 2.0f) {
            edpart_copy_size = 2.0f;
        }
        edpart_copyroty += pad->analog_left_pad_right;
        edpart_copyroty -= pad->analog_left_pad_left;
    } else {
        if (edpart_dpad_mode == 1) {
            if (pad->digital_buttons & PAD_BUTTON_L3) {
                edpart_rotz = 0;
                edpart_roty = 0;
            }
            edpart_roty += pad->analog_left_pad_right;
            edpart_roty -= pad->analog_left_pad_left;
            edpart_rotz += pad->analog_left_pad_up;
            if (edpart_rotz > 0) {
                edpart_rotz = 0;
            }
            edpart_rotz -= pad->analog_left_pad_down;
            if (edpart_rotz < -0x8000) {
                edpart_rotz = -0x8000;
            }
        } else if (edpart_dpad_mode == 0) {
            if (pad->digital_buttons & PAD_BUTTON_L3) {
                edpart_emitrotz = 0;
                edpart_emitroty = 0;
                edpart_emitrotx = 0;
            }
            if (pad->digital_buttons & PAD_BUTTON_R3) {
                edpart_emitrotx += pad->analog_left_pad_right;
                edpart_emitrotx -= pad->analog_left_pad_left;
            } else {
                edpart_emitroty += pad->analog_left_pad_right;
                edpart_emitroty -= pad->analog_left_pad_left;
                edpart_emitrotz += pad->analog_left_pad_up;
                if (edpart_emitrotz > 0x8000) {
                    edpart_emitrotz = 0x8000;
                }
                edpart_emitrotz -= pad->analog_left_pad_down;
                if (edpart_emitrotz < -0x8000) {
                    edpart_emitrotz = -0x8000;
                }
            }
        } else if (edpart_dpad_mode == 3) {
            edpart_refroty += pad->analog_left_pad_right;
            edpart_refroty -= pad->analog_left_pad_left;
            edpart_refrotz += pad->analog_left_pad_up;
            if (edpart_refrotz > 0) {
                edpart_refrotz = 0;
            }
            edpart_refrotz -= pad->analog_left_pad_down;
            if (edpart_refrotz < -0x8000) {
                edpart_refrotz = -0x8000;
            }
        } else if (edpart_dpad_mode == 2) {
            if (pad->analog_left_pad_up == 255 || (pad->digital_buttons_pressed & PAD_BUTTON_UP)) {
                edpart_offset += 0.01f;
            }
            if (pad->analog_left_pad_down == 255 || (pad->digital_buttons_pressed & PAD_BUTTON_DOWN)) {
                edpart_offset -= 0.01f;
            }
            if (edpart_offset < 0.0f) {
                edpart_offset = 0.0f;
            }
        }
    }
    if (edpart_nearest == -1) {
        return;
    }
    edpart_nearest_emit = &part_emits[edpart_nearest];
    if (edpart_nearest_emit->effect_id != -1) {
        edpart_nearest_type = &part_types[edpart_nearest_emit->effect_id];
    }
    edpart_nearest_duplicates = 0;
    edpart_nearest_orphans = 0;
    for (variant = 0; variant < 8; ++variant) {
        i16 effect = edpart_nearest_type->effect_ids[variant];
        if (effect == -1 || effect == 9999) {
            continue;
        }
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

void edpartRegisterPointerToGameCharLocation(NUVEC *charloc) {
    edmainSetLocVec(charloc);
}

void edpartPtlShelve(i32 index) {
    if (part_emits[index].instance_id != -1 && part_emits[index].instance_id != 99999) {
        DebFreeInstantly(&part_emits[index].instance_id);
        part_emits[index].instance_id = 99999;
    }
}

void edpartMultipleCopyClear(void) {
    edpart_copy_source_count = 0;
    edpart_copyroty = 0;
}

i32 edpartPlace(i32 index, NUVEC *position) {
    part_emits[index].position = *position;
    part_emits[index].rotation_2c = (i16)edpart_emitrotz;
    part_emits[index].rotation_2e = (i16)edpart_emitroty;
    part_emits[index].rotation_30 = (i16)edpart_emitrotx;
    return index;
}

void edpartDrawCursor(void) {
    extern i32 edpart_copy_mode;
    extern i32 edpart_copyrotz;
    extern i32 edpart_copyroty;
    extern i32 edpart_rotz;
    extern i32 edpart_roty;
    extern i32 edpart_emitrotz;
    extern i32 edpart_emitroty;
    extern i32 edpart_emitrotx;
    extern i32 edpart_refrotz;
    extern i32 edpart_refroty;
    extern i32 edpart_dpad_mode;
    extern i32 edpart_readout;
    extern i32 edpart_num_orphans;
    extern i32 edpart_create_type;
    extern i32 edpart_nearest_orphans;
    extern i32 edpart_nearest_duplicates;
    extern i32 edpart_copy_enclosed;
    extern i32 part_types_used;
    extern i32 part_emits_used;
    extern i8 edpart_effect_list;
    extern f32 edpart_copy_size;
    extern NUMTL *edpart_mtl;
    extern NUMTL *edpart_boxmtl;
    i32 rotation_z;
    i32 rotation_y;
    NUVEC vector;
    NURND_VERTEX3D line[2];

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
    line[0].position.x = PartEditorCameraPos.x - vector.x;
    line[0].position.y = PartEditorCameraPos.y - vector.y;
    line[0].position.z = PartEditorCameraPos.z - vector.z;
    line[0].colour = 0xffffffff;
    line[1].position.x = PartEditorCameraPos.x;
    line[1].position.y = PartEditorCameraPos.y;
    line[1].position.z = PartEditorCameraPos.z;
    line[1].colour = 0xffffffff;
    NuRndrLine3d(line, edpart_mtl, NULL);
    line[0].position.x = PartEditorCameraPos.x;
    line[0].position.y = PartEditorCameraPos.y;
    line[0].position.z = PartEditorCameraPos.z;
    line[0].colour = 0xff00ff00;
    line[1].position.x = PartEditorCameraPos.x + vector.x;
    line[1].position.y = PartEditorCameraPos.y + vector.y;
    line[1].position.z = PartEditorCameraPos.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.0f;
    vector.y = 0.5f;
    vector.z = 0.0f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPos.x - vector.x;
    line[0].position.y = PartEditorCameraPos.y - vector.y;
    line[0].position.z = PartEditorCameraPos.z - vector.z;
    line[0].colour = 0xffffffff;
    line[1].position.x = PartEditorCameraPos.x;
    line[1].position.y = PartEditorCameraPos.y;
    line[1].position.z = PartEditorCameraPos.z;
    line[1].colour = 0xffffffff;
    NuRndrLine3d(line, edpart_mtl, NULL);
    line[0].position.x = PartEditorCameraPos.x;
    line[0].position.y = PartEditorCameraPos.y;
    line[0].position.z = PartEditorCameraPos.z;
    line[0].colour = 0xff00ff00;
    line[1].position.x = PartEditorCameraPos.x + vector.x;
    line[1].position.y = PartEditorCameraPos.y + vector.y;
    line[1].position.z = PartEditorCameraPos.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.0f;
    vector.y = 0.0f;
    vector.z = 0.5f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPos.x - vector.x;
    line[0].position.y = PartEditorCameraPos.y - vector.y;
    line[0].position.z = PartEditorCameraPos.z - vector.z;
    line[0].colour = 0xffffffff;
    line[1].position.x = PartEditorCameraPos.x;
    line[1].position.y = PartEditorCameraPos.y;
    line[1].position.z = PartEditorCameraPos.z;
    line[1].colour = 0xffffffff;
    NuRndrLine3d(line, edpart_mtl, NULL);
    line[0].position.x = PartEditorCameraPos.x;
    line[0].position.y = PartEditorCameraPos.y;
    line[0].position.z = PartEditorCameraPos.z;
    line[0].colour = 0xff00ff00;
    line[1].position.x = PartEditorCameraPos.x + vector.x;
    line[1].position.y = PartEditorCameraPos.y + vector.y;
    line[1].position.z = PartEditorCameraPos.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.55f;
    vector.y = 0.05f;
    vector.z = 0.0f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPos.x + vector.x;
    line[0].position.y = PartEditorCameraPos.y + vector.y;
    line[0].position.z = PartEditorCameraPos.z + vector.z;
    line[0].colour = 0xff00ff00;
    vector.x = 0.6f;
    vector.y = -0.05f;
    vector.z = 0.0f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[1].position.x = PartEditorCameraPos.x + vector.x;
    line[1].position.y = PartEditorCameraPos.y + vector.y;
    line[1].position.z = PartEditorCameraPos.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.6f;
    vector.y = 0.05f;
    vector.z = 0.0f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPos.x + vector.x;
    line[0].position.y = PartEditorCameraPos.y + vector.y;
    line[0].position.z = PartEditorCameraPos.z + vector.z;
    line[0].colour = 0xff00ff00;
    vector.x = 0.55f;
    vector.y = -0.05f;
    vector.z = 0.0f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[1].position.x = PartEditorCameraPos.x + vector.x;
    line[1].position.y = PartEditorCameraPos.y + vector.y;
    line[1].position.z = PartEditorCameraPos.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.0f;
    vector.y = 0.65f;
    vector.z = -0.025f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPos.x + vector.x;
    line[0].position.y = PartEditorCameraPos.y + vector.y;
    line[0].position.z = PartEditorCameraPos.z + vector.z;
    line[0].colour = 0xff00ff00;
    vector.x = 0.0f;
    vector.y = 0.6f;
    vector.z = 0.0f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[1].position.x = PartEditorCameraPos.x + vector.x;
    line[1].position.y = PartEditorCameraPos.y + vector.y;
    line[1].position.z = PartEditorCameraPos.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.0f;
    vector.y = 0.65f;
    vector.z = 0.025f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPos.x + vector.x;
    line[0].position.y = PartEditorCameraPos.y + vector.y;
    line[0].position.z = PartEditorCameraPos.z + vector.z;
    line[0].colour = 0xff00ff00;
    vector.x = 0.0f;
    vector.y = 0.6f;
    vector.z = 0.0f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[1].position.x = PartEditorCameraPos.x + vector.x;
    line[1].position.y = PartEditorCameraPos.y + vector.y;
    line[1].position.z = PartEditorCameraPos.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.0f;
    vector.y = 0.6f;
    vector.z = 0.0f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPos.x + vector.x;
    line[0].position.y = PartEditorCameraPos.y + vector.y;
    line[0].position.z = PartEditorCameraPos.z + vector.z;
    line[0].colour = 0xff00ff00;
    vector.x = 0.0f;
    vector.y = 0.55f;
    vector.z = 0.0f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[1].position.x = PartEditorCameraPos.x + vector.x;
    line[1].position.y = PartEditorCameraPos.y + vector.y;
    line[1].position.z = PartEditorCameraPos.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.0f;
    vector.y = 0.05f;
    vector.z = 0.55f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPos.x + vector.x;
    line[0].position.y = PartEditorCameraPos.y + vector.y;
    line[0].position.z = PartEditorCameraPos.z + vector.z;
    line[0].colour = 0xff00ff00;
    vector.x = 0.0f;
    vector.y = 0.05f;
    vector.z = 0.6f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[1].position.x = PartEditorCameraPos.x + vector.x;
    line[1].position.y = PartEditorCameraPos.y + vector.y;
    line[1].position.z = PartEditorCameraPos.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.0f;
    vector.y = 0.05f;
    vector.z = 0.6f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPos.x + vector.x;
    line[0].position.y = PartEditorCameraPos.y + vector.y;
    line[0].position.z = PartEditorCameraPos.z + vector.z;
    line[0].colour = 0xff00ff00;
    vector.x = 0.0f;
    vector.y = -0.05f;
    vector.z = 0.55f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[1].position.x = PartEditorCameraPos.x + vector.x;
    line[1].position.y = PartEditorCameraPos.y + vector.y;
    line[1].position.z = PartEditorCameraPos.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);
    vector.x = 0.0f;
    vector.y = -0.05f;
    vector.z = 0.55f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[0].position.x = PartEditorCameraPos.x + vector.x;
    line[0].position.y = PartEditorCameraPos.y + vector.y;
    line[0].position.z = PartEditorCameraPos.z + vector.z;
    line[0].colour = 0xff00ff00;
    vector.x = 0.0f;
    vector.y = -0.05f;
    vector.z = 0.6f;
    NuVecRotateZ(&vector, &vector, rotation_z);
    NuVecRotateY(&vector, &vector, rotation_y);
    line[1].position.x = PartEditorCameraPos.x + vector.x;
    line[1].position.y = PartEditorCameraPos.y + vector.y;
    line[1].position.z = PartEditorCameraPos.z + vector.z;
    line[1].colour = 0xff00ff00;
    NuRndrLine3d(line, edpart_mtl, NULL);

    if (edpart_copy_mode != 0) {
        edbitsDrawCube(PartEditorCameraPos.x, PartEditorCameraPos.y, PartEditorCameraPos.z, edpart_copy_size, edpart_copy_size,
                       edpart_copy_size, 0, 0, 0, 0, 0, 0xffffffff, edpart_mtl);
    } else {
        vector.x = 0.0f;
        vector.y = 0.375f;
        vector.z = 0.0f;
        NuVecRotateZ(&vector, &vector, edpart_emitrotz);
        NuVecRotateY(&vector, &vector, edpart_emitroty);
        NuVecRotateX(&vector, &vector, edpart_emitrotx);
        NuVecRotateZ(&vector, &vector, rotation_z);
        NuVecRotateY(&vector, &vector, rotation_y);
        line[0].position.x = PartEditorCameraPos.x;
        line[0].position.y = PartEditorCameraPos.y;
        line[0].position.z = PartEditorCameraPos.z;
        line[1].position.x = PartEditorCameraPos.x + vector.x;
        line[1].position.y = PartEditorCameraPos.y + vector.y;
        line[1].position.z = PartEditorCameraPos.z + vector.z;
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
        line[0].position.x = PartEditorCameraPos.x + vector.x;
        line[0].position.y = PartEditorCameraPos.y + vector.y;
        line[0].position.z = PartEditorCameraPos.z + vector.z;
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
            line[0].position.x = PartEditorCameraPos.x;
            line[0].position.y = PartEditorCameraPos.y;
            line[0].position.z = PartEditorCameraPos.z;
            line[1].position.x = PartEditorCameraPos.x + vector.x;
            line[1].position.y = PartEditorCameraPos.y + vector.y;
            line[1].position.z = PartEditorCameraPos.z + vector.z;
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
                edbitsDrawCube(PartEditorCameraPos.x + vector.x, PartEditorCameraPos.y + vector.y, PartEditorCameraPos.z + vector.z, 0.25f,
                               0.0f, 0.25f, edpart_refrotz, edpart_refroty, 0, rotation_z, rotation_y, 0xffff0000, edpart_mtl);
            }
        }
    }

    NuRndrRect2di(0x1720, 0x9b0, 0xf00, 0x410, 0x80808080, edpart_boxmtl);
    NuRndrRect2di(0x1710, 0x8f0, 0xf20, 0xc0, 0x80000000, edpart_boxmtl);
    NuRndrLine2di(0x1710, 0x9b0, 0x1710, 0xdc8, 0x80000000, edpart_boxmtl);
    NuRndrLine2di(0x2630, 0x9b0, 0x2630, 0xdc8, 0x80000000, edpart_boxmtl);
    NuRndrLine2di(0x1710, 0xdc8, 0x2630, 0xdc8, 0x80000000, edpart_boxmtl);
    NuQFntPushPrintMode(NUQFNT_MODE_2D);
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
            if (edpart_create_type == -1) {
                NuQFntPrintEx(system_qfont, 0x17c0, 0xa50, 0x10, "Current Type: <none>");
            } else {
                NuQFntPrintEx(system_qfont, 0x17c0, 0xa50, 0x10, "Current Type: %s", part_types[edpart_create_type].name);
            }
            if (edpart_create_type == -1) {
                NuQFntPrintEx(system_qfont, 0x17c0, 0xaf0, 0x10, "Current List: <none>");
            } else if (edpart_effect_list == 1) {
                NuQFntPrintEx(system_qfont, 0x17c0, 0xaf0, 0x10, "Current List: Level");
            } else if (edpart_effect_list == 0) {
                NuQFntPrintEx(system_qfont, 0x17c0, 0xaf0, 0x10, "Current List: General");
            }
        }
        if (edpart_copy_mode != 0) {
            if (edpart_copy_enclosed > 8) {
                NuQFntSetColour(system_qfont, 0x80000080);
            }
            NuQFntPrintEx(system_qfont, 0x17c0, 0xc30, 0x10, "Enclosed: %d", edpart_copy_enclosed);
            NuQFntSetColour(system_qfont, 0x80000000);
        } else {
            if (edpart_nearest == -1) {
                NuQFntPrintEx(system_qfont, 0x17c0, 0xc30, 0x10, "Highlight: <none>");
            } else {
                edbitsDrawCube(PartEditorCameraPos.x, PartEditorCameraPos.y, PartEditorCameraPos.z, edpart_nearest_type->position_random.x,
                               edpart_nearest_type->position_random.y, edpart_nearest_type->position_random.z, edpart_emitrotz,
                               edpart_emitroty, edpart_emitrotx, rotation_z, rotation_y, 0xff0000ff, edpart_mtl);
                NuQFntSet(system_qfont);
                NuQFntSetColour(system_qfont, 0x80000000);
                NuQFntPrintEx(system_qfont, 0x17c0, 0xc30, 0x10, "Highlight: %s", edpart_nearest_type->name);
                if (edpart_nearest_orphans == 0 && edpart_nearest_duplicates == 0) {
                    NuQFntPrintEx(system_qfont, 0x1860, 0xcd0, 0x10, "Instances: %d of %d", edpart_nearest_type->variant_count, 8);
                } else {
                    NuQFntSetColour(system_qfont, 0x80000080);
                    NuQFntPrintEx(system_qfont, 0x1860, 0xcd0, 0x10, "Instances: %d of %d (Or = %d, Du = %d)",
                                  edpart_nearest_type->variant_count, 8, edpart_nearest_orphans, edpart_nearest_duplicates);
                    NuQFntSetColour(system_qfont, 0x80000000);
                }
            }
        }
        NuQFntPrintEx(system_qfont, 0x17c0, 0xd70, 0x10, "Types: %d/%d; Emits: %d/%d", part_types_used, 128, part_emits_used, 40);
    } else if (edpart_readout == 1) {
        if (edpart_nearest == -1) {
            NuQFntPrintEx(system_qfont, 0x17c0, 0xa50, 0x10, "Highlight: <none>");
        } else {
            NuQFntPrintEx(system_qfont, 0x17c0, 0xa50, 0x10, "Highlight: %s", edpart_nearest_type->name);
            NuQFntPrintEx(system_qfont, 0x17c0, 0xaf0, 0x10, "XYZ: %0.2f %0.2f %0.2f", edpart_nearest_emit->position.x,
                          edpart_nearest_emit->position.y, edpart_nearest_emit->position.z);
            NuQFntPrintEx(system_qfont, 0x17c0, 0xb90, 0x10, "RotZ: %d", ((const i16 *)&edpart_nearest_emit->trailing_state_words[0])[0]);
            NuQFntPrintEx(system_qfont, 0x17c0, 0xc30, 0x10, "RotY: %d", ((const i16 *)&edpart_nearest_emit->trailing_state_words[0])[1]);
            NuQFntPrintEx(system_qfont, 0x17c0, 0xcd0, 0x10, "EmitRotZ: %d", edpart_nearest_emit->rotation_2c);
            NuQFntPrintEx(system_qfont, 0x17c0, 0xd70, 0x10, "EmitRotY: %d", edpart_nearest_emit->rotation_2e);
        }
    }
    NuQFntPopPrintMode();
    NuQFntPopCoordinateSystem();
}

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

void fun_00642520(void) {
}

static void PartRuntimeDirectionalLightsPrivate(NUVEC *direction0, NUVEC *colour0, NUVEC *direction1, NUVEC *colour1, NUVEC *direction2,
                                                NUVEC *colour2) {
    PCGlobalLightingState.direction[0] = *direction0;
    PCGlobalLightingState.direction[1] = *direction1;
    PCGlobalLightingState.direction[2] = *direction2;
    PCGlobalLightingState.intensity[0] = *colour0;
    PCGlobalLightingState.intensity[1] = *colour1;
    PCGlobalLightingState.intensity[2] = *colour2;
    NuRndrSetDirectionalLightsPS(direction0, colour0, direction1, colour1, direction2, colour2);
}

void DrawParts(i32 keep_offscreen) {
    PART *part = Part;
    i32 i;
    NUMTX scaled;
    NUMTX *matrix;
    NUVEC scale;
    for (i = 0; i < MAXPARTS; ++i, ++part) {
        if ((part->active & PART_IN_USE) == 0 || (part->flags & PART_FLAG_DISABLE_DRAW) != 0) {
            continue;
        }
        if (part->draw_fn != NULL && part->draw_fn(part) == 0) {
            continue;
        }
        if ((part->active & PART_STATE_LIT) != 0) {
            PartRuntimeDirectionalLightsPrivate(&part->lighting[3], &part->lighting[0], &part->lighting[4], &part->lighting[1],
                                                &part->lighting[5], &part->lighting[2]);
            PCGlobalLightingState.ambient = part->lighting[6];
            NuRndrSetAmbientLightPS(&part->lighting[6]);
        } else if ((part->active & PART_STATE_LIGHT_SCALE) != 0) {
            PCGlobalLightingState.ambient = part->lighting[6];
            NuRndrSetAmbientLightPS(&part->lighting[6]);
        }
        part->active &= ~PART_STATE_DRAWN;
        matrix = &part->transform;
        if (part->unknown_214 != 1.0f) {
            PartRuntimeMatrixCopyPrivate(&scaled, matrix);
            matrix = &scaled;
            scale.z = part->unknown_214;
            scale.x = scale.y = scale.z;
            NuMtxPreScale(matrix, &scale);
        }
        if (part->source_special != NULL && NuSpecialDrawAt(&part->special, matrix) != 0) {
            part->active |= 0x600;
        } else if (keep_offscreen == 0 &&
                   ((part->flags & PART_FLAG_KEEP_OFF_SCREEN) == 0 || (part->active & PART_STATE_BELOW_CUTOFF) != 0)) {
            KillPart(part, 0);
        }
    }
}

void AddScaledVariableShotPARTEffect(i32 effect, NUVEC *position, f32 rate, f32 time, NUMTX *orientation, f32 scale) {
    i32 scaled = CreateScaledPARTEffect(effect, scale);
    if (scaled != -1) {
        AddVariableShotPARTEffect(scaled, position, rate, time, orientation);
    }
}

void edpartDestroyAllPages(void) {
    i32 i;
    for (i = 0; i < 8; ++i) {
        if (part_page_used[i] != 0) {
            edpartClearPage(i);
        }
    }
}

i32 edpartProc(float delta_time, FRAME_PAD *pad) {
    edpart_count += 5;
    if (edpart_active_menu) {
        eduiMenuProcess(edpart_active_menu, delta_time, pad);
        return 0;
    }
    edpartDoInput(pad);
    edpartDetermineNearest(1.0f);
    return (pad->digital_buttons_pressed >> 11) & 1;
}

void fun_0064fd90(void) {
}

void fun_0064fda0(void) {
}

void UpdateParts(f32 time) {
    i32 i;
    i32 axis;
    i32 angle;
    PART *part;
    PART_TERRAIN_STATE *state;
    NUVEC velocity;
    NUVEC direction;
    NUMTX mtx;
    partglobaltime += time;
    if (partglobaltime > 900.0f) {
        PartTimeSlip();
    }
    PartCleanupTypes();
    part_platimpactcnt = 0;
    UpdatePartEmits(time);
    part = Part;
    for (i = 0; i < MAXPARTS; ++i, ++part) {
        state = (PART_TERRAIN_STATE *)part;
        if ((part->active & PART_IN_USE) == 0) {
            continue;
        }
        PartRuntimeMatrixCopyPrivate(&part->previous_transform, &part->transform);
        part->scale_time += time;
        if ((part->active & PART_STATE_STOPPED) == 0) {
            f32 movement_time = time;
            if ((part->flags & PART_FLAG_IGNORE_TERRAIN) == 0) {
                if (time >= part->unknown_f4) {
                    if (part->unknown_e0 < part->radius) {
                        part->active = (part->active & ~PART_STATE_SCALING_DOWN) | PART_STATE_SCALING_UP;
                    } else {
                        part->active &= ~(PART_STATE_SCALING_UP | PART_STATE_SCALING_DOWN);
                    }
                    if ((part->active & PART_STATE_IMPACT_PENDING) != 0) {
                        if ((part->flags & 0x80000) != 0 && (part->active & 0x10) != 0 && part_platimpactcnt < 4) {
                            PartPlatformImpactList[part_platimpactcnt++] = state->platform_id;
                        }
                        if ((part->flags & PART_FLAG_NO_BOUNCE) != 0) {
                            part->active |= PART_STATE_STOPPED;
                            if (part->stop_fn != NULL) {
                                part->stop_fn(part);
                            }
                            if ((part->flags & PART_FLAG_DIE_WHEN_STOPPED) != 0 || (part->active & 0x10) != 0) {
                                KillPart(part, 0);
                            }
                        } else {
                            movement_time -= part->unknown_f4;
                            part->position.x = part->impact_position.x;
                            part->position.y = part->impact_position.y;
                            part->position.z = part->impact_position.z;
                            FullReflect(&part->impact_normal, &part->velocity, &part->velocity);
                            if ((part->flags & PART_FLAG_ROTATE_RANDOMLY) != 0) {
                                NewPartRotation(part);
                            }
                            if ((part->flags & PART_FLAG_ORDERED_ROTATE_RANDOMLY) != 0) {
                                NewPartOrderedRotation(part);
                            }
                            part->velocity.x *= part->field_ec;
                            part->velocity.z *= part->field_ec;
                            if (part->impact_normal.y > 0.5f) {
                                part->velocity.y *= part->field_ec;
                                if (part->velocity.x * part->velocity.x + part->velocity.y * part->velocity.y +
                                        part->velocity.z * part->velocity.z <
                                    PartStopSpeedSquared) {
                                    part->active |= PART_STATE_STOPPED;
                                    if (part->stop_fn != NULL) {
                                        part->stop_fn(part);
                                    }
                                    if ((part->flags & PART_FLAG_DIE_WHEN_STOPPED) != 0 || (part->active & 0x10) != 0 ||
                                        (part->active & PART_STATE_BELOW_CUTOFF) != 0) {
                                        KillPart(part, 0);
                                    }
                                } else if ((part->active & PART_STATE_BELOW_CUTOFF) != 0) {
                                    KillPart(part, 0);
                                }
                            }
                        }
                        if (part->effect_id_1f0 != -1) {
                            i32 key = -1;
                            PartRuntimeMatrixCopyPrivate(&mtx, (NUMTX *)&ANI3_EVAL_IDENTITY_MATRIX);
                            NuMtxAlignY(&mtx, &part->impact_normal);
                            AddScaledFiniteShotDebrisEffect(&key, part->effect_id_1f0, &part->impact_position, NULL, NULL, 1,
                                                            part->effect_scale);
                            DebrisOrientationMtx(key, &mtx);
                        }
                        if (part->effect_id_1f8 != -1) {
                            AddFiniteShotPART(part->effect_id_1f8, &part->impact_position, 1);
                        }
                        if (part->impact_fn != NULL) {
                            part->impact_fn(part);
                        }
                    }
                    if (PartRaycastsEnabledPrivate != 0) {
                        CastPart(part, PartCollisionInterval);
                    }
                    if ((part->active & PART_STATE_IMPACT_PENDING) != 0 && part->unknown_f4 <= movement_time) {
                        movement_time = 0.0f;
                    }
                }
                part->unknown_f4 -= movement_time;
            }
            if (movement_time > 0.0f) {
                if (part->move_fn != NULL) {
                    part->move_fn(part, movement_time);
                } else {
                    part->position.x += part->velocity.x * movement_time;
                    part->position.y += part->velocity.y * movement_time;
                    part->velocity.y += part->gravity * movement_time;
                    part->position.z += part->velocity.z * movement_time;
                }
            }
            if ((part->active & PART_STATE_BELOW_CUTOFF) == 0 && part->unknown_f0 != 2000000.0f && part->position.y < part->unknown_f0) {
                part->active |= PART_STATE_BELOW_CUTOFF;
            }
            part->collision_min.x = part->position.x - part->radius;
            part->collision_max.x = part->position.x + part->radius;
            part->collision_min.y = part->position.y - part->radius;
            part->collision_max.y = part->position.y + part->radius;
            part->collision_min.z = part->position.z - part->radius;
            part->collision_max.z = part->position.z + part->radius;
            if (((part->active >> 5) & 3) == 1) {
                part->time_step += time;
                part->unknown_e0 = part->time_step / PARTSCALEUPTIME * part->radius;
                if (part->unknown_e0 > part->radius) {
                    part->unknown_e0 = part->radius;
                    part->active &= ~(PART_STATE_SCALING_UP | PART_STATE_SCALING_DOWN);
                }
            } else if (((part->active >> 5) & 3) == 2) {
                part->time_step -= time;
                if (part->time_step <= 0.0f) {
                    KillPart(part, 0);
                } else {
                    part->unknown_e0 = part->time_step / PARTSCALEUPTIME * part->radius;
                }
            }
            if ((part->flags & PART_FLAG_ROTATE_RANDOMLY) != 0) {
                for (axis = 0; axis < 2; ++axis) {
                    angle = (i32)((f32)part->unknown_124[3 + axis] * time);
                    switch (part->rotation_axes[axis]) {
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
            }
            if ((part->flags & PART_FLAG_ORDERED_ROTATE_RANDOMLY) != 0) {
                if (part->unknown_124[0] != 0) {
                    NuMtxPreRotateX(&part->transform, (i32)((f64)part->unknown_124[0] * time));
                }
                if (part->unknown_124[1] != 0) {
                    NuMtxPreRotateY(&part->transform, (i32)((f64)part->unknown_124[1] * time));
                }
                if (part->unknown_124[2] != 0) {
                    NuMtxPreRotateZ(&part->transform, (i32)((f64)part->unknown_124[2] * time));
                }
            }
            if ((part->flags & PART_FLAG_FACE_MOVEMENT) != 0) {
                direction.x = -part->velocity.x;
                direction.y = -part->velocity.y;
                direction.z = -part->velocity.z;
                PartRuntimeMatrixCopyPrivate(&mtx, (NUMTX *)&ANI3_EVAL_IDENTITY_MATRIX);
                NuMtxLookAtZ(&mtx, &direction);
                memmove(&part->transform, &mtx, 0x30);
            }
        }
        if ((part->active & PART_IN_USE) != 0) {
            if ((part->flags & PART_FLAG_COLLIDE_WITH_CHARACTERS) != 0 && part->collision_fn != NULL) {
                part->collision_fn(part);
            }
            if ((part->active & PART_IN_USE) != 0) {
                if (part->update_fn != NULL) {
                    part->update_fn(part);
                }
                if (part->time_remaining > 0.0f) {
                    part->time_remaining -= time;
                    if (part->time_remaining <= 0.0f) {
                        KillPart(part, 0);
                    }
                }
            }
        }
        if ((part->flags & 0x20000) != 0 && part->debris_handle != NULL) {
            velocity = part->velocity;
            PartRuntimeMatrixCopyPrivate(&mtx, (NUMTX *)&ANI3_EVAL_IDENTITY_MATRIX);
            velocity.x = -velocity.x;
            velocity.y = -velocity.y;
            velocity.z = -velocity.z;
            NuMtxLookAtY(&mtx, &velocity);
            DebrisOrientationMtx(*part->debris_handle, &mtx);
            DebrisEmitterPos(*part->debris_handle, part->position.x, part->position.y, part->position.z);
            if (((part->flags & PART_FLAG_DEBRIS_STOPS_WITH_PART) != 0 && (part->active & PART_STATE_STOPPED) != 0) ||
                (part->flags & PART_FLAG_DISABLE_DRAW) != 0) {
                DebrisStatusAlwaysOff(part->debris_handle);
            }
        }
        if (time != 0.0f) {
            if ((part->flags & 0x40000) != 0 && part->effect_id_1e0 != -1 &&
                !((part->flags & PART_FLAG_DEBRIS_STOPS_WITH_PART) != 0 && (part->active & PART_STATE_STOPPED) != 0) &&
                (part->flags & PART_FLAG_DISABLE_DRAW) == 0) {
                AddScaledVariableShotDebrisEffect1(part->effect_id_1e0, &part->position, (i32)(part->unknown_1e8), time, 0, 0, NULL,
                                                   part->effect_scale);
            }
            if ((part->flags & 0x40000) != 0 && part->effect_id_1e4 != -1 &&
                !((part->flags & PART_FLAG_DEBRIS_STOPS_WITH_PART) != 0 && (part->active & PART_STATE_STOPPED) != 0) &&
                (part->flags & PART_FLAG_DISABLE_DRAW) == 0) {
                AddScaledVariableShotDebrisEffect1(part->effect_id_1e4, &part->position, (i32)(part->unknown_1ec), time, 0, 0, NULL,
                                                   part->effect_scale);
            }
        }
        if (part->lighting_handle != NULL) {
            if ((part->flags & PART_FLAG_DISABLE_DRAW) == 0) {
                rtlDynamicSetPos(*part->lighting_handle, &part->position);
            } else {
                NUVEC offscreen;
                offscreen.x = 10000.0f;
                offscreen.y = 10000.0f;
                offscreen.z = 10000.0f;
                rtlDynamicSetPos(*part->lighting_handle, &offscreen);
            }
        }
        if (part->callback_1ac != NULL) {
            part->callback_1ac(part);
        }
        part->unknown_208 = 0;
        if ((part->active & PART_IN_USE) == 0) {
            KillPart(part, 0);
            memset(part, 0, sizeof(PART));
        }
    }
}

// MATCH-EXCEPTION: noinline; the original calls this out of line from UpdateParts.
__declspec(noinline) i32 AddScaledVariableShotDebrisEffect1(i32 effect, NUVEC *position, i32 count, f32 time, i16 z_rotation,
                                                            i16 y_rotation, NUMTX *orientation, f32 scale) {
    i32 scaled = CreateScaledEffect(effect, scale);
    if (scaled != -1) {
        AddVariableShotDebrisEffectTimed1(scaled, position, count, time, z_rotation, y_rotation, orientation);
    }
    return scaled;
}

void NuPartEnableRayCasts(i32 enabled) {
    PartRaycastsEnabledPrivate = enabled;
}

void edpartSetParticlePage(i32 page) {
    edpart_load_particle_page = page;
}

i32 edpartLookupObject(char *name) {
    if (edbits_base_scene == NULL) {
        return -1;
    }
    return edpartLookupObjectInScene(name, edbits_base_scene);
}

i32 edpartLoadPage(char *path, i8 mode, NUGSCN *scene) {
    return edpartLoadPageEx(path, mode, &scene, 1);
}

void DebrisSetup(VARIPTR *buffer, VARIPTR buffer_end, char *texture_name, i32 chunk_count, i32 key_count, i32 effect_count) {
    DebrisSetup2(buffer, buffer_end, buffer, buffer_end, texture_name, chunk_count, 0x20, key_count, effect_count);
}
