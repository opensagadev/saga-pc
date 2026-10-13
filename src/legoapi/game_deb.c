#include "saga.h"

struct nuvec_s *edmainQueryLocVec(void);
extern void *debris_cutoff_camera_vec;
extern i32 debris_render_group;

PARTDEBRISSYS *__cdecl InitPartDebris(VARIPTR *buffer, VARIPTR *end, i32 capacity, i32 named_count, char **names,
                                      i32 page) {
    PARTDEBRISSYS *system;
    i32 i, type;
    buffer->addr = (buffer->addr + 15) & ~(u32)15;
    system = (PARTDEBRISSYS *)buffer->void_ptr;
    buffer->addr += sizeof(PARTDEBRISSYS);
    if (system == NULL)
        return system;
    system->entries = NULL;
    system->capacity = capacity;
    system->named_count = named_count;
    buffer->addr = (buffer->addr + 15) & ~(u32)15;
    system->entries = (PARTDEBRISENTRY *)buffer->void_ptr;
    buffer->addr += capacity * sizeof(PARTDEBRISENTRY);
    if (system->entries == NULL)
        return NULL;
    memset(system->entries, -1, capacity * sizeof(PARTDEBRISENTRY));
    i = 0;
    if (names != NULL) {
        for (; i < system->named_count; ++i) {
            NuStrCpy(system->entries[i].name, names[i]);
            system->entries[i].id = -1;
            system->entries[i].id = PARTLookupTypePageOnly(system->entries[i].name, page);
        }
    }
    for (type = 0; type < 128 && i < system->capacity; ++type) {
        system->entries[i].id = -1;
        if (part_types[type].variant_count > 0) {
            NuStrCpy(system->entries[i].name, part_types[type].name);
            system->entries[i].id = PARTLookupTypePageOnly(system->entries[i].name, page);
            ++i;
        }
    }
    for (; i < system->capacity; ++i)
        system->entries[i].id = -1;
    return system;
}

i32 __cdecl LookupDebrisEffectPageIgnore(char *name, i8 page, i32 ignore) {
    i32 i;
    u8 requested_page = (u8)page;

    if (name == NULL) {
        return -1;
    }

    if (requested_page < 8 && edpp_page_used[requested_page] != 0) {
        for (i = 1; i < debinf_count; ++i) {
            DEBRIS_INFO_PC *effect = debtab[i];
            if (i != ignore && effect != NULL && effect->page == requested_page && NuStrICmp(effect->name, name) == 0) {
                return i;
            }
        }
    }

    for (i = 1; i < debinf_count; ++i) {
        DEBRIS_INFO_PC *effect = debtab[i];
        if (i != ignore && effect != NULL &&
            ((effect->page == 0 && edpp_page_used[0] != 0) || (effect->page == 1 && edpp_page_used[1] != 0)) &&
            NuStrICmp(effect->name, name) == 0) {
            return i;
        }
    }

    for (i = 1; i < debinf_count; ++i) {
        DEBRIS_INFO_PC *effect = debtab[i];
        if (i != ignore && effect != NULL && edpp_page_used[(i8)effect->page] != 0 &&
            NuStrICmp(effect->name, name) == 0) {
            return i;
        }
    }
    return -1;
}

i32 __cdecl LookupDebrisEffectPage(char *name, i8 page) {
    return LookupDebrisEffectPageIgnore(name, page, 0);
}

i32 __cdecl LookupDebrisEffectPageOnly(char *name, i8 page) {
    i32 i;
    u8 requested_page = (u8)page;

    if (name == NULL) {
        return -1;
    }

    if (requested_page < 8 && edpp_page_used[requested_page] != 0) {
        for (i = 1; i < debinf_count; ++i) {
            DEBRIS_INFO_PC *effect = debtab[i];
            if (effect != NULL && effect->page == requested_page && NuStrICmp(effect->name, name) == 0) {
                return i;
            }
        }
    }

    for (i = 1; i < debinf_count; ++i) {
        DEBRIS_INFO_PC *effect = debtab[i];
        if (effect != NULL &&
            ((effect->page == 0 && edpp_page_used[0] != 0) || (effect->page == 1 && edpp_page_used[1] != 0)) &&
            NuStrICmp(effect->name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void __cdecl DebrisTypeStatusAlwaysOff(i32 type) {
    if (type != -1) {
        debtab[type]->status = 0;
    }
}

void __cdecl DebrisTypeStatusAlwaysOn(i32 type) {
    if (type != -1) {
        debtab[type]->status = 2;
    }
}

void DebrisSetSeed(i32 seed) {
    debrisseed = (u32)seed;
}

u32 DebrisGetSeed(void) {
    return debrisseed;
}

void DebrisSetTimeIncrement(f32 increment) {
    debris_time_increment = increment;
}

void DebrisSetCutSceneMode(i32 enabled) {
    DebrisCutSceneMode = enabled;
}

float edanimPlayerAnimDistance(i32 parameter_index) {
    if (edmainQueryLocVec() != NULL) {
        struct nuvec_s *position;
        nuhspecial_s special;
        NuGScnGetSpecial(&special, edanim_page_scene[AnimParams[parameter_index].page],
                         AnimParams[parameter_index].instance_id);
        position = edmainQueryLocVec();
        return NuVecDist(NuSpecialGetPos(&special), position, NULL);
    }
    return 0.0f;
}

void DebrisRegisterCutoffCameraVec(void *position) {
    debris_cutoff_camera_vec = position;
}

void DebrisSetRenderGroup(i32 group) {
    debris_render_group = group;
}

__declspec(noinline) void __cdecl PARTStartOffset(i32 index, f32 offset) {
    PART_EMIT_PC *emitter;
    PARTTYPE_LOOKUP_PC *type;
    f32 period;
    f32 end;
    f32 interval;
    f32 last_emission;
    if (index == -1)
        return;
    emitter = &part_emits[index];
    if (emitter->effect_id == -1)
        return;
    type = &part_types[emitter->effect_id];
    if (type->emission_period_random == 0.0f && type->emission_pause_random == 0.0f) {
        period = type->emission_period + type->emission_pause;
        emitter->time_24 = (i32)(partglobaltime / period) * period + offset;
        emitter->time_28 = emitter->time_24 + type->emission_period;
        while (partglobaltime > emitter->time_24 && partglobaltime > emitter->time_28) {
            if (emitter->time_24 >= emitter->time_28)
                emitter->time_28 = emitter->time_24 + type->emission_period;
            else
                emitter->time_24 = emitter->time_28 + type->emission_pause;
        }
    } else {
        emitter->time_24 = partglobaltime;
        end = partglobaltime + type->emission_period;
        emitter->time_28 = end + NuRandFloatSeeded(&partseed) * type->emission_period_random;
    }
    interval = 1.0f / type->emission_rate;
    if (partglobaltime > emitter->time_28)
        last_emission = emitter->time_24 - interval;
    else
        last_emission = (i32)(partglobaltime / interval) * interval;
    emitter->time_20 = last_emission;
    emitter->time_38 = partglobaltime;
}

__declspec(noinline) i32 __cdecl AddPARTEffect(i32 effect, NUVEC *position) {
    i32 i;
    PART_EMIT_PC *emitter;
    PARTTYPE_LOOKUP_PC *type;
    if (part_emits_used == 40 || part_types[effect].effect_ids[0] == -1)
        return -1;
    for (i = 0; i < 40; ++i) {
        if (part_emits[i].effect_id != -1)
            continue;
        emitter = &part_emits[i];
        emitter->position = *position;
        emitter->effect_id = effect;
        type = &part_types[effect];
        NuStrNCpy(emitter->name, type->name, 16);
        PARTStartOffset(i, 0.0f);
        emitter->rotation_30 = 0;
        emitter->camera_distance = 9999.9f;
        emitter->page = type->page;
        emitter->rotation_2e = 0;
        emitter->rotation_2c = 0;
        emitter->shots_remaining = 0;
        emitter->field_33 = 0;
        emitter->field_3d = 1;
        emitter->field_44 = 0;
        emitter->field_46 = 0;
        ++part_emits_used;
        return i;
    }
    return -1;
}

i32 __cdecl AddFiniteShotPART(i32 effect, NUVEC *position, i32 count) {
    i32 index = -1;
    PARTTYPE_LOOKUP_PC *type;
    PART_EMIT_PC *emitter;
    f32 end;
    if (effect != -1) {
        type = &part_types[effect];
        if (type->effect_ids[0] != -1) {
            index = AddPARTEffect(effect, position);
            if (index != -1) {
                emitter = &part_emits[index];
                emitter->shots_remaining = count;
                emitter->time_24 = partglobaltime;
                end = partglobaltime + type->emission_period;
                emitter->time_28 = end + NuRandFloatSeeded(&partseed) * type->emission_period_random;
                emitter->time_20 = partglobaltime - 1.0f / type->emission_rate;
            }
        }
    }
    return index;
}

i32 DebAlloc(void) {
    i32 i;
    i32 slot;
    DEBRISKEY_PC *key;
    if (freedebkeyptr >= maxdebkeys) {
        return -1;
    }

    slot = freedebkeys[freedebkeyptr++];
    key = debkeydata + slot;
    for (i = 0; i < 32; ++i) {
        key->particle_chunks[i] = NULL;
    }
    key->allocated_chunk_count = 0;
    key->particle_cursor = 0;
    key->particle_count = 0;
    key->previous_allocated_chunk_count = 0;
    key->previous_particle_count = 0;
    key->controlled_chunk_count = 0;
    key->field_2f2 = -1;
    key->gscene = NULL;
    key->field_2f4 = 1;
    key->field_2f6 = 1;
    key->field_2f7 = 1;
    key->user_data = NULL;
    key->allocation_index = slot;
    key->field_2fa = 0;
    return slot;
}

void __cdecl DebrisStartOffsetEx(DEBRISKEY_PC *key, f32 offset) {
    i32 frame_count = (i32)((f64)offset * 60.0);
    i16 effect_index;
    DEBRIS_INFO_PC *effect;
    f32 now;
    f32 start;
    f32 period;
    f32 interval;
    if (key == NULL) {
        return;
    }
    effect_index = key->effect_index;
    effect = debtab[effect_index];
    now = effect->time_group == 4 ? panelglobaltime : globaltime;

    if (effect->emission_period_random == 0.0f && effect->emission_pause_random == 0.0f) {
        interval = effect->emission_period + effect->emission_pause;
        start = (f32)((i32)(now / interval)) * interval;
        if (effect->generator_type == 7 && effect->emission_pause == 0.0f) {

            key->emitter_rotation_x = (i16)((i32)(effect->rotor_speed_x * (f32)frame_count));
            key->emitter_rotation_y = (i16)((i32)(effect->rotor_speed_y * (f32)frame_count));
        } else {
            start += offset;
        }
        start += interval;
        key->emission_time = start;
        while (now < key->emission_time) {
            key->emission_time -= interval;
        }
    } else {
        start = now;
        key->emission_time = start;
    }
    start = key->emission_time;
    period = effect->emission_period;
    key->previous_emission_time = -10.0f;
    key->emission_end_time = NuRandFloatSeeded(&debrisseed) * effect->emission_period_random + start + period;
}

i32 __cdecl LookupDebrisEffect(char *name) {
    return LookupDebrisEffectPage(name, 0);
}

void __cdecl InitPartTable(char **names) {
    part_debris_count = 0;
    PartDebrisNameListPc = names;
    if (names != NULL) {
        while (names[part_debris_count] != NULL) {
            part_debris_count++;
        }
    }
}
