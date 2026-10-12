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

static void cbPtlQuickDeleteMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    edptl_quickdel_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, cbPtlCancelQuickDeleteMenu, "Quick Delete");
    if (edptl_quickdel_menu) {
        i32 type;
        for (type = 1; type < EDPP_MAX_TYPES; ++type) {
            i32 index;
            DEBRIS_INFO_PC *effect = debtab[type];
            if (!effect || effect->category != edpp_effect_list) {
                continue;
            }

            for (index = 0; index < 512; ++index) {
                if (edpp_ptls[index].instance_id != 99999 && edpp_ptls[index].instance_id != -1 &&
                    edpp_ptls[index].effect_index == type) {
                    break;
                }
            }
            if (index < 512) {
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
        EdppUnresolvedInstancesPc = 0;
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
                ++EdppUnresolvedInstancesPc;

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
