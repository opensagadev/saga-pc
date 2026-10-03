#include "saga.h"

extern void *debris_cutoff_camera_vec;
extern i32 debris_render_group;

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

void DebrisRegisterCutoffCameraVec(void *position) {
    debris_cutoff_camera_vec = position;
}

void DebrisSetRenderGroup(i32 group) {
    debris_render_group = group;
}

i32 __cdecl PartDebrisCreatePart_candidate(i32 id, NUVEC *position, i32 flag) {
    STUBBED_NAMED("PartDebrisCreatePart_candidate: part template lookup and spawn");
    return -1;
}

#include "saga.h"
extern void *debkeydata;
#define DEBRIS_KEYS_PC ((DEBRISKEY_PC *)debkeydata)
i32 DebAlloc(void) {
    i32 i;
    i32 slot;
    DEBRISKEY_PC *key;
    if (freedebkeyptr >= maxdebkeys) {
        return -1;
    }

    slot = freedebkeys[freedebkeyptr++];
    key = DEBRIS_KEYS_PC + slot;
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

#undef DEBRIS_KEYS_PC
