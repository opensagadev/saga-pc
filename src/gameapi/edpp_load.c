#include "saga.h"
typedef struct edpp_file_particle_pc_s {
    NUVEC position;
    i32 effect_index;
    i32 instance_id;
    i16 rotation_z, rotation_y;
    i16 emitter_rotation_z, emitter_rotation_y, emitter_rotation_x;
    u8 unknown_01e[2];
    f32 start_offset;
    char name[16];
    i32 switch_type, switch_id;
    f32 switch_variable;
    i16 reflection_rotation_z, reflection_rotation_y;
    f32 reflection_offset, reflection_bounce;
    i16 render_group;
    u16 render_priority;
    u8 dynamic_priority;
    i8 page, detail_levels;
    u8 facing_mode;
    i16 facing_rotation_x, facing_rotation_y;
} EDPP_FILE_PARTICLE_PC;
STATIC_ASSERT(sizeof(EDPP_FILE_PARTICLE_PC) == 0x58);
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, render_priority) == 0x4e);
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, dynamic_priority) == 0x50);

STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, position) == 0);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->position) == sizeof(((EDPP_PARTICLE_PC *)0)->position));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, effect_index) == 0xc);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->effect_index) == sizeof(((EDPP_PARTICLE_PC *)0)->effect_index));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, instance_id) == 0x10);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->instance_id) == sizeof(((EDPP_PARTICLE_PC *)0)->instance_id));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, rotation_z) == 0x14);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->rotation_z) == sizeof(((EDPP_PARTICLE_PC *)0)->rotation_z));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, rotation_y) == 0x16);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->rotation_y) == sizeof(((EDPP_PARTICLE_PC *)0)->rotation_y));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, emitter_rotation_z) == 0x18);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->emitter_rotation_z) ==
              sizeof(((EDPP_PARTICLE_PC *)0)->emitter_rotation_z));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, emitter_rotation_y) == 0x1a);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->emitter_rotation_y) ==
              sizeof(((EDPP_PARTICLE_PC *)0)->emitter_rotation_y));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, emitter_rotation_x) == 0x1c);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->emitter_rotation_x) ==
              sizeof(((EDPP_PARTICLE_PC *)0)->emitter_rotation_x));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, start_offset) == 0x20);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->start_offset) == sizeof(((EDPP_PARTICLE_PC *)0)->start_offset));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, name) == 0x24);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->name) == sizeof(((EDPP_PARTICLE_PC *)0)->name));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, switch_type) == 0x34);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->switch_type) == sizeof(((EDPP_PARTICLE_PC *)0)->switch_type));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, switch_id) == 0x38);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->switch_id) == sizeof(((EDPP_PARTICLE_PC *)0)->switch_id));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, switch_variable) == 0x3c);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->switch_variable) ==
              sizeof(((EDPP_PARTICLE_PC *)0)->switch_variable));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, reflection_rotation_z) == 0x40);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->reflection_rotation_z) ==
              sizeof(((EDPP_PARTICLE_PC *)0)->reflection_rotation_z));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, reflection_rotation_y) == 0x42);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->reflection_rotation_y) ==
              sizeof(((EDPP_PARTICLE_PC *)0)->reflection_rotation_y));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, reflection_offset) == 0x44);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->reflection_offset) ==
              sizeof(((EDPP_PARTICLE_PC *)0)->reflection_offset));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, reflection_bounce) == 0x48);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->reflection_bounce) ==
              sizeof(((EDPP_PARTICLE_PC *)0)->reflection_bounce));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, render_group) == 0x4c);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->render_group) == sizeof(((EDPP_PARTICLE_PC *)0)->render_group));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, page) == 0x51);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->page) == sizeof(((EDPP_PARTICLE_PC *)0)->page));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, detail_levels) == 0x52);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->detail_levels) == sizeof(((EDPP_PARTICLE_PC *)0)->detail_levels));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, facing_mode) == 0x53);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->facing_mode) == sizeof(((EDPP_PARTICLE_PC *)0)->facing_mode));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, facing_rotation_x) == 0x54);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->facing_rotation_x) ==
              sizeof(((EDPP_PARTICLE_PC *)0)->facing_rotation_x));
STATIC_ASSERT(offsetof(EDPP_FILE_PARTICLE_PC, facing_rotation_y) == 0x56);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->facing_rotation_y) ==
              sizeof(((EDPP_PARTICLE_PC *)0)->facing_rotation_y));
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->render_priority) == 2);
STATIC_ASSERT(sizeof(((EDPP_FILE_PARTICLE_PC *)0)->dynamic_priority) == 1);

static void UpdateTotalPtls(DEBRIS_INFO_PC *effect) {
    f32 elapsed = 0.0f, active = 0.0f;
    f64 emission;
    f32 ratio;
    i32 i;
    i16 count;
    if (effect->particle_lifetime > 0.0f) {
        emission = (f64)effect->emission_period_random + effect->emission_period;
        do {
            f32 remaining = effect->particle_lifetime - elapsed;
            if (remaining < emission) {
                active += remaining;
                elapsed += remaining;
            } else {
                active += emission;
                elapsed += emission;
            }
            remaining = effect->particle_lifetime - elapsed;
            if (effect->emission_pause > remaining)
                elapsed += remaining;
            else
                elapsed += effect->emission_pause;
        } while (effect->particle_lifetime > elapsed);
    }
    /* Native68d080 spills the ratio to DWORD before multiplying frequency. */
    ratio = active / elapsed;
    count = (i16)(i32)(effect->frequency * ratio * effect->particle_lifetime);
    if (count < 1)
        count = 1;
    effect->max_particles = (i16)(count * (effect->trail_count + 1));
    for (i = 0; i < 512; ++i) {
        i32 id = edpp_ptls[i].instance_id;
        if (id != -1 && id != 99999) {
            DEBRISKEY_PC *key = &debkeydata[id];
            if (debtab[key->effect_index] == effect)
                DebReAlloc(key, effect->max_particles);
        }
    }
}

void cbPtlSetFacing(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    EDPP_PARTICLE_PC *particle = &edpp_ptls[edpp_nearest];
    particle->facing_mode = item->highlighted;
    if (item->highlighted) {
        particle->facing_rotation_x = edpp_facrotx;
        particle->facing_rotation_y = edpp_facroty;
        DebrisSetFacing(particle->instance_id, 1, particle->facing_rotation_x, particle->facing_rotation_y);
    } else {
        particle->facing_rotation_x = 0;
        particle->facing_rotation_y = 0;
        DebrisSetFacing(particle->instance_id, 0, 0, 0);
    }
}

static __inline void edppLoadDebrisSetGroupID(i32 handle, i16 group) {
    if (handle != -1)
        debkeydata[handle].render_group = group;
}

void cbPtlChangePriority(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edpp_ptls[edpp_nearest].render_priority = (i16)((edui_slider_s *)item)->value;
    debkeydata[edpp_ptls[edpp_nearest].instance_id].render_priority = (i16)((edui_slider_s *)item)->value;
}

void cbPtlToggleDynamicPriority(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edpp_ptls[edpp_nearest].dynamic_priority = item->highlighted;
    debkeydata[edpp_ptls[edpp_nearest].instance_id].timed_flags = item->highlighted;
}

void cbPtlCancelInstanceSettingsMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edptl_instancesettings_menu);
    edptl_instancesettings_menu = NULL;
}

void cbPtlSetXZFacing(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    debtab[edpp_ptls[edpp_nearest].effect_index]->camera_facing = item->highlighted;
}

void cbPtlCancelTextureMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    if (textureselectmenu) {
        eduiMenuDestroy(textureselectmenu);
        textureselectmenu = NULL;
    }
    eduiMenuDestroy(texturemenu);
    texturemenu = NULL;
}

void cbPtlInstanceSettingsMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    if (edpp_nearest == -1 || edpp_ptls[edpp_nearest].instance_id == -1)
        return;

    edptl_instancesettings_menu =
        eduiMenuCreate(70, 70, 250, 300, ed_fnt, cbPtlCancelInstanceSettingsMenu, "Instance Settings");
    if (edptl_instancesettings_menu == NULL)
        return;

    eduiMenuAddItem(edptl_instancesettings_menu,
                    eduiItemSliderCreateInt(0, edblack, 0, cbPtlChangePriority, 0, 0xffff,
                                            ((u16)(edpp_ptls[edpp_nearest].render_priority)), "Base Priority"));
    eduiMenuAddItem(edptl_instancesettings_menu,
                    eduiItemToggleCreate(1, edblack, edpp_ptls[edpp_nearest].dynamic_priority, 1,
                                         cbPtlToggleDynamicPriority, "Dynamic Priority"));
    eduiMenuAddItem(edptl_instancesettings_menu,
                    eduiItemSelCreate(1, edblack, 0, 0, edptlcbScaleEffectMenu, "Scale Effect..."));
    eduiMenuAddItem(edptl_instancesettings_menu, eduiItemToggleCreate(0, edblack, edpp_ptls[edpp_nearest].facing_mode,
                                                                      2, cbPtlSetFacing, "Facing Enabled"));
    eduiMenuAttach(parent, edptl_instancesettings_menu);
    edptl_instancesettings_menu->x = parent->x + 10;
    edptl_instancesettings_menu->y = parent->y + 40;
}

void cbPtlSelTextureType(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
    struct eduimenu_s *parent;
    if (edpp_nearest != -1 && edpp_ptls[edpp_nearest].instance_id != -1) {
        i32 type = debkeydata[edpp_ptls[edpp_nearest].instance_id].effect_index;
        DEBRIS_INFO_PC *effect = debtab[type];
        i8 old_type = ((i8)(effect->particle_type));
        if (old_type != item->data) {
            switch (item->data) {
                case 0:
                    edpp_ptls[edpp_nearest].render_priority = 20000;
                    break;
                case 2:
                    edpp_ptls[edpp_nearest].render_priority = ((i16)(40000));
                    break;
                case 3:
                    edpp_ptls[edpp_nearest].render_priority = 30000;
                    break;
                case 7:
                    edpp_ptls[edpp_nearest].render_priority = 10000;
                    break;
            }
        }
        if (item->data == 7) {
            effect->time_group = 2;
        } else if (old_type == 7) {
            effect->time_group = 0;
        }
        if (item->data == 7 ||
            debtab[debkeydata[edpp_ptls[edpp_nearest].instance_id].effect_index]->particle_type == 7) {
            i32 index;
            for (index = 0; index < 512; ++index) {
                if (edpp_ptls[index].effect_index == type) {
                    edppPtlShelve(index);
                }
            }
            effect->particle_type = ((u8)(item->data));
            GenericDebinfoDmaTypeUpdate(effect);
            for (index = 0; index < 512; ++index) {
                EDPP_PARTICLE_PC *particle = &edpp_ptls[index];
                if (particle->effect_index != type || particle->instance_id != 99999) {
                    continue;
                }
                particle->instance_id = -1;
                AddDebrisEffect(&particle->instance_id, type, particle->position.x, particle->position.y,
                                particle->position.z);
                if (particle->instance_id == -1) {
                    particle->instance_id = 99999;
                    continue;
                }
                debkeydata[particle->instance_id].field_2f9 = 0;
                DebrisOrientation(particle->instance_id, particle->rotation_z, particle->rotation_y);
                DebrisEmitterOrientation(particle->instance_id, particle->emitter_rotation_z,
                                         particle->emitter_rotation_y, particle->emitter_rotation_x);
                DebrisStartOffset(particle->instance_id, particle->start_offset);
                DebrisSetTrigger(particle->instance_id, particle->switch_type, particle->switch_id,
                                 particle->switch_variable);
                DebrisReflectionOrientation(particle->instance_id, particle->reflection_rotation_z,
                                            particle->reflection_rotation_y, particle->reflection_offset,
                                            particle->reflection_bounce);
                DebrisSetFacing(particle->instance_id, particle->facing_mode, particle->facing_rotation_x,
                                particle->facing_rotation_y);
                edppLoadDebrisSetGroupID(particle->instance_id, particle->render_group);
                DebrisSetRoomID(particle->instance_id, EdppPageScenesPc[particle->page]);
            }
        } else {
            effect->particle_type = ((u8)(item->data));
        }
    }
    parent = menu->parent;
    if (parent) {
        eduiMenuDetach(menu);
    }
    if (menu->callback) {
        menu->callback(menu, parent);
    }
}

void cbPtlTextureMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    if (!(edpp_nearest == -1 || edpp_ptls[edpp_nearest].instance_id == -1)) {
        DEBRIS_INFO_PC *effect = debtab[debkeydata[edpp_ptls[edpp_nearest].instance_id].effect_index];
        texturemenu = eduiMenuCreate(70, 70, 180, 300, ed_fnt, cbPtlCancelTextureMenu, "Texture");
        if (texturemenu) {
            eduiMenuAddItem(texturemenu, eduiItemCheckCreate(0, colours, effect->particle_type == 0, 1,
                                                             cbPtlSelTextureType, "Addative"));
            eduiMenuAddItem(texturemenu, eduiItemCheckCreate(2, colours, effect->particle_type == 2, 1,
                                                             cbPtlSelTextureType, "Modulative"));
            eduiMenuAddItem(texturemenu, eduiItemCheckCreate(3, colours, effect->particle_type == 3, 1,
                                                             cbPtlSelTextureType, "Subtractive"));
            if (!(effect->generator_type == 0 || effect->generator_type == 8)) {
                eduiMenuAddItem(texturemenu, eduiItemSelCreate(7, edgrey, 0, 0, NULL, "Glass"));
            } else {
                eduiMenuAddItem(texturemenu, eduiItemCheckCreate(7, colours, effect->particle_type == 7, 1,
                                                                 cbPtlSelTextureType, "Glass"));
            }
            if (effect->particle_type == 7) {
                eduiMenuAddItem(texturemenu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Texture Selector..."));
            } else {
                eduiMenuAddItem(texturemenu,
                                eduiItemSelCreate(1, colours, 0, 0, cbPtlTextureSelectMenu, "Texture Selector..."));
            }
            eduiMenuAddItem(texturemenu, eduiItemToggleCreate(0, edblack, ((i8)(effect->camera_facing)), 2,
                                                              cbPtlSetXZFacing, "Default to XZ Plane"));
            eduiMenuAttach(menu, texturemenu);
            texturemenu->x = menu->x + 10;
            texturemenu->y = menu->y + 40;
        }
    }
}

void cbPtlQuickDeleteMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    edptl_quickdel_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, cbPtlCancelQuickDeleteMenu, "Quick Delete");
    if (edptl_quickdel_menu) {
        i32 type;
        for (type = 1; type < debinf_count; ++type) {
            EDPP_PARTICLE_PC *particle;
            DEBRIS_INFO_PC *effect;
            if (!debtab[type]) {
                continue;
            }
            effect = debtab[type];
            if (effect->category != edpp_effect_list) {
                continue;
            }

            for (particle = edpp_ptls; particle < &edpp_ptls[512]; ++particle) {
                if (particle->instance_id != -1 && particle->instance_id != 99999 && particle->effect_index == type) {
                    break;
                }
            }
            if (particle < &edpp_ptls[512]) {
                eduiMenuAddItem(edptl_quickdel_menu,
                                eduiItemSelCreate(type, edblack, 0, 1, cbPtlQuickDeleteType, effect->name));
            } else {
                eduiMenuAddItem(edptl_quickdel_menu,
                                eduiItemSelCreate(type, eddarkred, 0, 1, cbPtlQuickDeleteType, effect->name));
            }
        }
        eduiMenuAttach(menu, edptl_quickdel_menu);
        edptl_quickdel_menu->x = menu->x + 10;
        edptl_quickdel_menu->y = menu->y + 40;
    }
}

i32 __cdecl edppLoadPage(char *path, i8 flag, NUGSCN *scene) {
    i32 page, n, version, requested, skipped;
    i32 type_index = 1, instance_index = 0;
    DEBRIS_INFO_PC discarded;
    i8 category = (i8)(flag);
    i32 page_index = -1;
    if (flag == 0) {
        category = 0;
        page_index = 0;
    } else if (flag == 5) {
        category = 5;
        page_index = 1;
    } else {
        if (flag == 6)
            category = 1;
        for (page = 2; page < 8; ++page) {
            if (edpp_page_used[page] == 0) {
                page_index = page;
                break;
            }
        }
        if (page_index == -1)
            return -1;
    }

    EdFileSetMedia(EdppUseMemcardPc == 0 ? 1 : 2);
    if (EdFileOpen(path, 0) == 0) {
        return -1;
    }
    EdFileSetReadWrongEndianess(1);

    version = EdFileReadInt();
    if (version < 5 || version > 41) {
        EdFileSetReadWrongEndianess(0);
        EdFileClose();
        return -1;
    }

    edpp_page_used[page_index] = 1;
    EdppPageScenesPc[page_index] = scene;

    requested = EdFileReadInt();
    skipped = 0;
    if (requested + edpp_types_used > debinf_count) {
        skipped = requested + edpp_types_used - debinf_count;
        requested = debinf_count - edpp_types_used;
        EdppTypesErrorPc = 300;
    }

    for (n = 0; n < requested; n++) {
        i32 index;
        DEBRIS_INFO_PC *effect;
        index = type_index;
        while (debtab[index] != NULL) {
            index++;
        }

        type_index = index;
        effect = &effecttypes[index];
        FileLoadSingleEffectType(effect, version, (char)(category));
        effect->native_data = NULL;
        effect->last_render_time = 0.0f;
        effect->page = (u8)(page_index);
        debtab[index] = effect;
        UpdateTotalPtls(debtab[index]);
        edpp_types_used++;
    }

    for (n = 0; n < skipped; ++n)
        FileLoadSingleEffectType(&discarded, version, category);

    if (flag == 1 || flag == 2) {
        i32 instances = EdFileReadInt();
        if (instances + edpp_instances_used > 512)
            instances = 512 - edpp_instances_used;
        edpp_num_orphans = 0;
        for (n = 0; n < instances; ++n) {
            i32 index = instance_index;
            EDPP_FILE_PARTICLE_PC *particle;
            while (edpp_ptls[index].instance_id != -1)
                ++index;
            instance_index = index;
            particle = (EDPP_FILE_PARTICLE_PC *)&edpp_ptls[index];
            particle->page = (i8)(page_index);
            particle->position.x = EdFileReadFloat();
            particle->position.y = EdFileReadFloat();
            particle->position.z = EdFileReadFloat();
            if (version <= 6) {
                particle->rotation_z = 0;
                particle->rotation_y = 0;
                particle->emitter_rotation_z = (i16)(EdFileReadInt());
                particle->emitter_rotation_y = (i16)(EdFileReadInt());
            } else {
                particle->rotation_z = EdFileReadShort();
                particle->rotation_y = EdFileReadShort();
                particle->emitter_rotation_z = EdFileReadShort();
                particle->emitter_rotation_y = EdFileReadShort();
            }
            particle->emitter_rotation_x = version > 21 ? EdFileReadShort() : 0;
            if (version > 33)
                particle->start_offset = EdFileReadFloat();
            else if (version > 7)
                particle->start_offset = (f32)(EdFileReadInt()) / 60.0f;
            else
                particle->start_offset = 0.0f;
            EdFileRead(particle->name, 16);
            particle->instance_id = 99999;
            particle->effect_index = LookupDebrisEffectPageOnly(particle->name, (char)(page_index));
            if (particle->effect_index == -1)
                ++edpp_num_orphans;

            if (version > 8) {
                particle->switch_type = EdFileReadInt();
                particle->switch_id = EdFileReadInt();
                particle->switch_variable = EdFileReadFloat();
            } else {
                particle->switch_type = 0;
                particle->switch_id = -1;
                particle->switch_variable = 0.0f;
            }
            if (version > 11) {
                particle->reflection_rotation_z = EdFileReadShort();
                particle->reflection_rotation_y = EdFileReadShort();
                particle->reflection_offset = EdFileReadFloat();
                particle->reflection_bounce = version == 12 ? 0.9f : EdFileReadFloat();
            } else {
                particle->reflection_rotation_z = 0;
                particle->reflection_rotation_y = 0;
                particle->reflection_offset = 0.0f;
                particle->reflection_bounce = 0.9f;
            }
            particle->render_group = version > 14 ? EdFileReadShort() : 0;
            if (version > 26) {
                particle->render_priority = EdFileReadUnsignedShort();
            } else {
                particle->render_priority = 20000;
                if (particle->effect_index != -1) {
                    u8 particle_type = debtab[particle->effect_index]->particle_type;
                    if (particle_type == 2)
                        particle->render_priority = 40000;
                    else if (particle_type == 3)
                        particle->render_priority = 30000;
                    else if (particle_type == 7)
                        particle->render_priority = 10000;
                }
            }
            particle->dynamic_priority = version > 26 ? EdFileReadChar() : 0;
            particle->detail_levels = version > 35 ? EdFileReadChar() : 7;
            particle->facing_mode = version > 37 ? EdFileReadChar() : 0;
            particle->facing_rotation_x = version > 37 ? EdFileReadShort() : 0;
            particle->facing_rotation_y = version > 37 ? EdFileReadShort() : 0;
            ++edpp_instances_used;
        }
    }

    EdFileSetReadWrongEndianess(0);
    EdFileClose();
    edppDetermineNearest(1.0f);

    if (flag == 0) {
        DEBPAGE_GENERAL = page_index;
    } else if (flag == 5) {
        DEBPAGE_CHARACTER = page_index;
    } else if (flag == 1) {
        DEBPAGE_AREA = page_index;
    }
    return page_index;
}

extern i32 edbits_particle_general_page;
extern i32 edbits_particle_char_page;

void __cdecl cbPtlAddEffect(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    eduimenu_s *parent = NULL;
    i32 index;
    if (edpp_types_used >= debinf_count) {
        eduiCreateMessageMenu(menu, "Out of particle slots", 0);
        return;
    }
    for (index = 1; index < debinf_count; ++index) {
        if (debtab[index] == NULL) {
            debtab[index] = &effecttypes[index];
            memcpy(debtab[index], debtab[0], sizeof(DEBRIS_INFO_PC));
            debtab[index]->status = 1;
            sprintf(debtab[index]->name, "New%d", index);
            debtab[index]->category = edpp_effect_list;
            if (edpp_effect_list == 0) {
                debtab[index]->page = (i8)edbits_particle_general_page;
                edpp_page_used[edbits_particle_general_page] = 1;
            } else if (edpp_effect_list == 5) {
                debtab[index]->page = (i8)edbits_particle_char_page;
                edpp_page_used[edbits_particle_char_page] = 1;
            } else if (edpp_effect_list == 1) {
                debtab[index]->page = (i8)edbits_particle_level_page;
                edpp_page_used[edbits_particle_level_page] = 1;
            }
            ++edpp_types_used;
            edpp_create_type = index;
            UpdateTotalPtls(debtab[index]);
            break;
        }
    }
    if (menu->parent != NULL) {
        parent = menu->parent;
        eduiMenuDetach(menu);
    }
    if (menu->callback != NULL)
        menu->callback(menu, parent);
}

extern struct eduimenu_s *changegenratemenu;
extern struct eduimenu_s *etimemenu;
extern struct eduimenu_s *emittimemenu;

static void cbCancelChangeGenRateMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(changegenratemenu);
    changegenratemenu = NULL;
}

static void cbCancelChangeETimeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(etimemenu);
    etimemenu = NULL;
}

static void cbPtlCancelEmitTimeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(emittimemenu);
    emittimemenu = NULL;
}

static void cbPtlChangeGenRate(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    DEBRIS_INFO_PC *effect;
    i32 instance_id;
    if (edpp_nearest == -1)
        return;
    instance_id = edpp_ptls[edpp_nearest].instance_id;
    if (instance_id == -1)
        return;
    effect = debtab[debkeydata[instance_id].effect_index];
    effect->frequency = (i16)(i32)((edui_slider_s *)item)->value;
    UpdateTotalPtls(effect);
}

static void cbPtlChangeETime(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    DEBRIS_INFO_PC *effect;
    i32 instance_id;
    if (edpp_nearest == -1)
        return;
    instance_id = edpp_ptls[edpp_nearest].instance_id;
    if (instance_id == -1)
        return;
    effect = debtab[debkeydata[instance_id].effect_index];
    effect->particle_lifetime = ((edui_slider_s *)item)->value;
    UpdateTotalPtls(effect);
}

static void cbPtlChangeIvalOn(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    DEBRIS_INFO_PC *effect;
    i32 instance_id;
    if (edpp_nearest == -1)
        return;
    instance_id = edpp_ptls[edpp_nearest].instance_id;
    if (instance_id == -1)
        return;
    effect = debtab[debkeydata[instance_id].effect_index];
    effect->emission_period = ((edui_slider_s *)item)->value;
    UpdateTotalPtls(effect);
}

static void cbPtlChangeIvalOnRan(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    DEBRIS_INFO_PC *effect;
    i32 instance_id;
    if (edpp_nearest == -1)
        return;
    instance_id = edpp_ptls[edpp_nearest].instance_id;
    if (instance_id == -1)
        return;
    effect = debtab[debkeydata[instance_id].effect_index];
    effect->emission_period_random = ((edui_slider_s *)item)->value;
    UpdateTotalPtls(effect);
}

static void cbPtlChangeIvalOff(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    DEBRIS_INFO_PC *effect;
    i32 instance_id;
    if (edpp_nearest == -1)
        return;
    instance_id = edpp_ptls[edpp_nearest].instance_id;
    if (instance_id == -1)
        return;
    effect = debtab[debkeydata[instance_id].effect_index];
    effect->emission_pause = ((edui_slider_s *)item)->value;
    UpdateTotalPtls(effect);
}

static void cbPtlChangeIvalOffRan(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    DEBRIS_INFO_PC *effect;
    i32 instance_id;
    if (edpp_nearest == -1)
        return;
    instance_id = edpp_ptls[edpp_nearest].instance_id;
    if (instance_id == -1)
        return;
    effect = debtab[debkeydata[instance_id].effect_index];
    effect->emission_pause_random = ((edui_slider_s *)item)->value;
    UpdateTotalPtls(effect);
}

void __cdecl edptlcbApplyNumGhosts(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    DEBRIS_INFO_PC *effect;
    i32 instance_id;
    if (edpp_nearest == -1)
        return;
    instance_id = edpp_ptls[edpp_nearest].instance_id;
    if (instance_id == -1)
        return;
    effect = debtab[debkeydata[instance_id].effect_index];
    effect->trail_count = (i8)(i32)((edui_slider_s *)item)->value;
    UpdateTotalPtls(effect);
}

void __cdecl cbChangeGenRateMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    if (!(edpp_nearest == -1 || edpp_ptls[edpp_nearest].instance_id == -1)) {
        DEBRIS_INFO_PC *effect = debtab[debkeydata[edpp_ptls[edpp_nearest].instance_id].effect_index];
        changegenratemenu = eduiMenuCreate(70, 70, 250, 250, ed_fnt, cbCancelChangeGenRateMenu, "Particles per Second");
        if (changegenratemenu == NULL)
            return;
        eduiMenuAddItem(changegenratemenu,
                        eduiItemSliderCreateInt(0, colours, 0, cbPtlChangeGenRate, 0, 1200 * edptl_superscale,
                                                effect->frequency, "Particles per Second"));
        eduiMenuAttach(parent, changegenratemenu);
        changegenratemenu->x = parent->x + 10;
        changegenratemenu->y = parent->y + 40;
    }
}

void __cdecl cbChangeETimeMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    if (!(edpp_nearest == -1 || edpp_ptls[edpp_nearest].instance_id == -1)) {
        DEBRIS_INFO_PC *effect = debtab[debkeydata[edpp_ptls[edpp_nearest].instance_id].effect_index];
        etimemenu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, cbCancelChangeETimeMenu, "Particle Life");
        if (etimemenu == NULL)
            return;
        eduiMenuAddItem(etimemenu, eduiItemSliderCreate(0, colours, 0, cbPtlChangeETime, 0.0f, 10.0f,
                                                        effect->particle_lifetime, "Particle Life"));
        eduiMenuAttach(parent, etimemenu);
        etimemenu->x = parent->x + 10;
        etimemenu->y = parent->y + 40;
    }
}

void __cdecl cbPtlEmitTimeMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    DEBRIS_INFO_PC *effect;
    if (edpp_nearest == -1 || edpp_ptls[edpp_nearest].instance_id == -1)
        return;
    effect = debtab[debkeydata[edpp_ptls[edpp_nearest].instance_id].effect_index];
    emittimemenu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, cbPtlCancelEmitTimeMenu, "Emitter Timing");
    if (emittimemenu != NULL) {
        eduiMenuAddItem(emittimemenu,
                        eduiItemSliderCreate(0, colours, 0, cbPtlChangeIvalOn, 0.01f, 5.0f * edptl_superscale,
                                             effect->emission_period, "On Time"));
        eduiMenuAddItem(emittimemenu,
                        eduiItemSliderCreate(0, colours, 0, cbPtlChangeIvalOnRan, 0.0f, 5.0f * edptl_superscale,
                                             effect->emission_period_random, "Random On Time"));
        eduiMenuAddItem(emittimemenu,
                        eduiItemSliderCreate(0, colours, 0, cbPtlChangeIvalOff, 0.0f, 5.0f * edptl_superscale,
                                             effect->emission_pause, "Off Time"));
        eduiMenuAddItem(emittimemenu,
                        eduiItemSliderCreate(0, colours, 0, cbPtlChangeIvalOffRan, 0.0f, 5.0f * edptl_superscale,
                                             effect->emission_pause_random, "Random Off Time"));
        eduiMenuAttach(parent, emittimemenu);
        emittimemenu->x = parent->x + 10;
        emittimemenu->y = parent->y + 40;
    }
}
