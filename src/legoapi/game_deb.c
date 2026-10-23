#include "saga.h"

struct nuvec_s *edmainQueryLocVec(void);
extern void *debris_cutoff_camera_vec;
extern i32 debris_render_group;

PARTDEBRISSYS *__cdecl InitPartDebris(VARIPTR *buffer, VARIPTR *end, i32 capacity, i32 named_count, char **names,
                                      i32 page) {
    PARTDEBRISSYS *system;
    i32 i, type;
    system = (PARTDEBRISSYS *)((buffer->addr + 15) & ~(u32)15);
    buffer->addr = ((buffer->addr + 15) & ~(u32)15) + sizeof(PARTDEBRISSYS);
    if (system == NULL)
        return NULL;
    system->entries = NULL;
    system->named_count = named_count;
    system->capacity = capacity;
    system->entries = (PARTDEBRISENTRY *)((buffer->addr + 15) & ~(u32)15);
    buffer->addr = ((buffer->addr + 15) & ~(u32)15) + capacity * sizeof(PARTDEBRISENTRY);
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
    for (type = 0; i < system->capacity && type < 128; ++type) {
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

    if (name == NULL) {
        return -1;
    }

    if ((u8)page <= 7 && edpp_page_used[page] != 0) {
        for (i = 1; i < debinf_count; ++i) {
            if (i != ignore && debtab[i] != NULL && debtab[i]->page == page && NuStrICmp(debtab[i]->name, name) == 0) {
                return i;
            }
        }
    }

    for (i = 1; i < debinf_count; ++i) {
        if (i != ignore && debtab[i] != NULL &&
            ((debtab[i]->page == 0 && edpp_page_used[0] != 0) || (debtab[i]->page == 1 && edpp_page_used[1] != 0)) &&
            NuStrICmp(debtab[i]->name, name) == 0) {
            return i;
        }
    }

    for (i = 1; i < debinf_count; ++i) {
        if (i != ignore && debtab[i] != NULL && edpp_page_used[debtab[i]->page] != 0 &&
            NuStrICmp(debtab[i]->name, name) == 0) {
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

    if (name == NULL) {
        return -1;
    }

    if ((u8)page <= 7 && edpp_page_used[page] != 0) {
        for (i = 1; i < debinf_count; ++i) {
            if (debtab[i] != NULL && debtab[i]->page == page && NuStrICmp(debtab[i]->name, name) == 0) {
                return i;
            }
        }
    }

    for (i = 1; i < debinf_count; ++i) {
        if (debtab[i] != NULL &&
            ((debtab[i]->page == 0 && edpp_page_used[0] != 0) || (debtab[i]->page == 1 && edpp_page_used[1] != 0)) &&
            NuStrICmp(debtab[i]->name, name) == 0) {
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
        nuhspecial_s special;
        NuGScnGetSpecial(&special, edanim_page_scene[AnimParams[parameter_index].page],
                         AnimParams[parameter_index].instance_id);
        return NuVecDist(NuSpecialGetPos(&special), edmainQueryLocVec(), NULL);
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
    f64 random_product;
    f32 interval;
    f32 phase_product;
    if (index == -1)
        return;
    emitter = &part_emits[index];
    if (emitter->effect_id == -1)
        return;
    type = &part_types[emitter->effect_id];
    if (type->emission_period_random == 0.0f && type->emission_pause_random == 0.0f) {
        period = type->emission_period + type->emission_pause;
        phase_product = (i32)(partglobaltime / period) * period;
        emitter->time_24 = phase_product + offset;
        emitter->time_28 = emitter->time_24 + type->emission_period;
        while (partglobaltime > emitter->time_24 && partglobaltime > emitter->time_28) {
            if (emitter->time_24 >= emitter->time_28)
                emitter->time_28 = emitter->time_24 + type->emission_period;
            else
                emitter->time_24 = emitter->time_28 + type->emission_pause;
        }
    } else {
        emitter->time_24 = partglobaltime;
        random_product = (f64)NuRandFloatSeeded(&partseed) * type->emission_period_random;
        emitter->time_28 = ((f64)type->emission_period + emitter->time_24) + random_product;
    }
    interval = 1.0f / type->emission_rate;
    if (partglobaltime > emitter->time_28)
        emitter->time_20 = emitter->time_24 - interval;
    else
        emitter->time_20 = (i32)(partglobaltime / interval) * interval;
    emitter->time_38 = partglobaltime;
}

__declspec(noinline) i32 __cdecl AddPARTEffect(i32 effect, NUVEC *position) {
    i32 i;
    if (part_emits_used == 40)
        return -1;
    if (part_types[effect].effect_ids[0] == -1)
        return -1;
    for (i = 0; i < 40; ++i) {
        if (part_emits[i].effect_id == -1) {
            part_emits[i].position = *position;
            part_emits[i].effect_id = effect;
            NuStrNCpy(part_emits[i].name, part_types[effect].name, 16);
            PARTStartOffset(i, 0.0f);
            part_emits[i].camera_distance = 9999.9f;
            ++part_emits_used;
            part_emits[i].rotation_30 = 0;
            part_emits[i].rotation_2e = 0;
            part_emits[i].rotation_2c = 0;
            part_emits[i].page = part_types[effect].page;
            part_emits[i].shots_remaining = 0;
            part_emits[i].field_33 = 0;
            part_emits[i].field_44 = 0;
            part_emits[i].field_46 = 0;
            part_emits[i].field_3d = 1;
            return i;
        }
    }
    return -1;
}

i32 __cdecl AddFiniteShotPART(i32 effect, NUVEC *position, i32 count) {
    i32 index;
    if (effect == -1)
        return effect;
    if (part_types[effect].effect_ids[0] == -1)
        return -1;
    index = AddPARTEffect(effect, position);
    if (index != -1) {
        part_emits[index].shots_remaining = count;
        part_emits[index].time_24 = partglobaltime;
        part_emits[index].time_28 = part_types[effect].emission_period + partglobaltime +
                                    NuRandFloatSeeded(&partseed) * part_types[effect].emission_period_random;
        part_emits[index].time_20 = partglobaltime - 1.0f / part_types[effect].emission_rate;
    }
    return index;
}

i32 DebAlloc(void) {
    i32 i;
    i32 slot;
    if (freedebkeyptr >= maxdebkeys) {
        return -1;
    }

    slot = freedebkeys[freedebkeyptr++];
    for (i = 0; i < 32; ++i) {
        debkeydata[slot].particle_chunks[i] = NULL;
    }
    debkeydata[slot].allocated_chunk_count = 0;
    debkeydata[slot].particle_cursor = 0;
    debkeydata[slot].particle_count = 0;
    debkeydata[slot].previous_allocated_chunk_count = 0;
    debkeydata[slot].previous_particle_count = 0;
    debkeydata[slot].controlled_chunk_count = 0;
    debkeydata[slot].field_2f2 = -1;
    debkeydata[slot].gscene = NULL;
    debkeydata[slot].field_2f4 = 1;
    debkeydata[slot].field_2f6 = 1;
    debkeydata[slot].field_2f7 = 1;
    debkeydata[slot].user_data = NULL;
    debkeydata[slot].allocation_index = slot;
    debkeydata[slot].field_2fa = 0;
    return slot;
}

void __cdecl DebrisStartOffsetEx(DEBRISKEY_PC *key, f32 offset) {
    i32 frame_count = (i32)(60.0 * offset);
    DEBRIS_INFO_PC *effect;
    f32 now;
    f32 start;
    f32 interval;
    if (key == NULL) {
        return;
    }
    effect = debtab[key->effect_index];
    now = effect->time_group == 4 ? panelglobaltime : globaltime;

    if (effect->emission_period_random == 0.0f && effect->emission_pause_random == 0.0f) {
        interval = effect->emission_period + effect->emission_pause;
        start = (f32)((i32)(now / interval)) * interval;
        if (effect->generator_type == 7 && effect->emission_pause == 0.0f) {
            key->emitter_rotation_x = (i16)((i32)(effect->rotor_speed_x * (f32)frame_count));
            key->emitter_rotation_y = (i16)((i32)(effect->rotor_speed_y * (f32)frame_count));
            key->emission_time = start + interval;
        } else {
            key->emission_time = start + offset + interval;
        }
        while (now < key->emission_time) {
            key->emission_time -= interval;
        }
    } else {
        key->emission_time = now;
    }
    key->emission_end_time = NuRandFloatSeeded(&debrisseed) * effect->emission_period_random +
                             (effect->emission_period + key->emission_time);
    key->previous_emission_time = -10.0f;
}

i32 __cdecl LookupDebrisEffect(char *name) {
    return LookupDebrisEffectPage(name, 0);
}

void __cdecl InitPartTable(char **names) {
    i32 count = 0;
    part_debris_count = count;
    PartDebrisNameListPc = names;
    if (names != NULL && *names != NULL) {
        while (*names != NULL) {
            names++;
            count++;
        }
        part_debris_count = count;
    }
}
