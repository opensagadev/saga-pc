#include "saga.h"

DEBRIS_PARTICLE_CHUNK_PC *__cdecl CreateDmaParticleSet(void *memory, i32 *size) {
    DEBRIS_PARTICLE_CHUNK_PC *chunk = (DEBRIS_PARTICLE_CHUNK_PC *)memory;
    i32 i;
    chunk->command = 0x52;
    chunk->next = NULL;
    chunk->field_014 = 0;
    chunk->field_018 = 0;
    chunk->field_01c = 0;
    chunk->particles[0].position.x = 0.0f;
    for (i = 0; i < 32; ++i) {
        DEBRISPARTICLE_PC *particle = &chunk->particles[i];
        particle->position.x = 1.0f;
        particle->position.y = 2.0f;
        particle->position.z = 3.0f;
        particle->momentum.x = 4.0f;
        particle->momentum.y = 5.0f;
        particle->momentum.z = 6.0f;
        particle->start_time = -1.0f;
        particle->inverse_lifetime = 128.0f;
    }
    chunk->end_command = 0;
    *size = sizeof(*chunk);
    return chunk;
}

DEBRIS_PARTICLE_CHUNK_PC *__cdecl CreateDmaParticleSetGlass(void *memory, i32 *size) {
    return CreateDmaParticleSet(memory, size);
}

DEBRIS_PART_HEADER_PC *__cdecl CreateDmaPartEffectList(void *memory, i32 *size) {
    u32 address = ((u32)memory + 0xf) & ~0xf;
    *size = sizeof(DEBRIS_PART_HEADER_PC);
    return (DEBRIS_PART_HEADER_PC *)address;
}

void __cdecl LinkDmaParticalSets(DEBRIS_PARTICLE_CHUNK_PC **chunks, i32 count) {
    DEBRIS_PARTICLE_CHUNK_PC *chunk = chunks[count - 1];
    i32 i;
    chunk->command = 0x52;
    chunk->next = NULL;
    for (i = count - 2; i >= 0; --i) {
        chunk = chunks[i];
        chunk->command = 0x4e;
        chunk->next = chunks[i + 1];
    }
}

void __cdecl DebrisReserveTrashableSpace(VARIPTR *buffer, VARIPTR end, i32 chunk_count, i32 glass_chunk_count) {
    if (debris_trash_space == 0) {
        buffer->addr = (buffer->addr + 0x7f) & ~0x7f;
        debris_trash_space = buffer->addr;
        debrischunks = chunk_count;
        debrischunksglass = glass_chunk_count;
        buffer->addr += chunk_count * 0x430 + glass_chunk_count * 0x1b0 + EDPP_MAX_DMADEBTYPES * 0xa4c;
        debris_trash_size = buffer->addr - debris_trash_space;
    }
}

void __cdecl DebrisTrashableSetup(VARIPTR *buffer, VARIPTR *end) {
    u32 cursor;
    i32 chunk_count, i, size;
    if (debris_setup_called == 0) {
        return;
    }
    cursor = debris_trash_space;
    if (cursor == 0) {
        if (buffer == NULL) {
            return;
        }
        buffer->addr = (buffer->addr + 0xf) & ~0xf;
        cursor = buffer->addr;
    }
    chunk_count = debrischunks + debrischunksglass;
    memset(debris_chunk_controls, 0, chunk_count * 2 * sizeof(DEBRIS_CHUNK_CONTROL_PC));
    memset(freechunkcontrols, 0, chunk_count * 2 * sizeof(DEBRIS_CHUNK_CONTROL_PC *));
    memset(freedebchunks, 0, debrischunks * sizeof(DEBRIS_PARTICLE_CHUNK_PC *));
    memset(freedebchunksglass, 0, debrischunksglass * sizeof(DEBRIS_PARTICLE_CHUNK_PC *));
    memset(ParticleChunkToRender, 0, chunk_count * sizeof(DEBRIS_RENDER_CHUNK_PC));
    for (i = 0; i < debrischunks; ++i) {
        freedebchunks[i] = CreateDmaParticleSet((void *)cursor, &size);
        cursor += size;
    }
    for (i = 0; i < debrischunksglass; ++i) {
        freedebchunksglass[i] = CreateDmaParticleSetGlass((void *)cursor, &size);
        cursor += size;
    }
    freedebchkptr = 0;
    freedebchkptrg = 0;
    for (i = 0; i < EDPP_MAX_DMADEBTYPES; ++i) {
        DmaDebTypes[i] = CreateDmaPartEffectList((void *)cursor, &size);
        cursor += size;
    }
    if (debris_trash_space == 0) {
        debris_trash_space = buffer->addr;
        debris_trash_size = cursor - buffer->addr;
        buffer->addr = cursor;
    }
    debris_chunk_control_stack[0] = NULL;
    debris_chunk_control_stack[1] = NULL;
    for (i = 0; i < chunk_count * 2; ++i) {
        freechunkcontrols[i] = &debris_chunk_controls[i];
    }
    freechunkcontrolsptr = 0;
    for (i = 0; i < chunk_count; ++i) {
        ParticleChunkToRender[i].particle_chunk = NULL;
        ParticleChunkToRender[i].effect = NULL;
        ParticleChunkToRender[i].key = NULL;
        ParticleChunkToRender[i].previous = NULL;
        ParticleChunkToRender[i].next = NULL;
    }
    for (i = 0; i < 5; ++i) {
        ParticleChunkRenderStack[i] = NULL;
    }
}

void __cdecl AddChunkToRenderStack(DEBRIS_RENDER_CHUNK_PC *chunk, DEBRIS_RENDER_CHUNK_PC **stack) {
    DEBRIS_RENDER_CHUNK_PC *current = *stack;
    chunk->previous = NULL;
    chunk->next = NULL;
    if (current != NULL) {
        u16 priority = chunk->render_priority;
        u16 current_priority = current->render_priority;
        if (priority <= current_priority &&
            (priority != current_priority || current->effect->particle_type < chunk->effect->particle_type)) {
            DEBRIS_RENDER_CHUNK_PC *next = current->next;
            while (next != NULL) {
                u16 next_priority = next->render_priority;
                if (next_priority <= priority &&
                    (priority != next_priority || chunk->effect->particle_type <= next->effect->particle_type)) {
                    break;
                }
                current = next;
                next = next->next;
            }
            current->next = chunk;
            chunk->previous = current;
            chunk->next = next;
            if (next != NULL) {
                next->previous = chunk;
            }
            return;
        }
        chunk->next = current;
        current->previous = chunk;
    }
    *stack = chunk;
}

void __cdecl RemoveChunkFromRenderStack(DEBRIS_RENDER_CHUNK_PC *chunk, DEBRIS_RENDER_CHUNK_PC **stack) {
    if (*stack == chunk) {
        DEBRIS_RENDER_CHUNK_PC *next = chunk->next;
        *stack = next;
        if (next != NULL) {
            next->previous = NULL;
        }
    } else if (chunk->previous != NULL) {
        chunk->previous->next = chunk->next;
        if (chunk->next != NULL) {
            chunk->next->previous = chunk->previous;
        }
    }
    chunk->previous = NULL;
    chunk->next = NULL;
}

void __cdecl RemoveChunkControlFromStack(DEBRIS_CHUNK_CONTROL_PC *control, DEBRIS_CHUNK_CONTROL_PC **stack) {
    DEBRIS_CHUNK_CONTROL_PC **link = stack;
    while (*link != NULL) {
        if (*link == control) {
            *link = control->next;
            break;
        }
        link = &(*link)->next;
    }
    control->next = NULL;
}

void __cdecl RemoveAnyChunkControls(DEBRIS_PARTICLE_CHUNK_PC *chunk) {
    i32 i;
    for (i = 0; i < 2; ++i) {
        DEBRIS_CHUNK_CONTROL_PC *control = debris_chunk_control_stack[i];
        while (control != NULL) {
            DEBRIS_CHUNK_CONTROL_PC *next = control->next;
            if (control->particle_chunk == chunk) {
                RemoveChunkControlFromStack(control, &debris_chunk_control_stack[i]);
                freechunkcontrols[--freechunkcontrolsptr] = control;
                control->particle_chunk = NULL;
            }
            control = next;
        }
    }
}

void __cdecl DebrisProcessSpheres(DEBRISPARTICLE_PC *particle, f32 time, DEBRIS_INFO_PC *effect, DEBRISKEY_PC *key,
                                  i32 finite) {
    i32 index;
    if (!(time >= key->sphere_next_time)) {
        return;
    }
    if (key->sphere_skip_count > 0) {
        --key->sphere_skip_count;
        return;
    }
    index = key->sphere_index;
    key->process_spheres[index].time = time;
    key->process_spheres[index].position.x = particle->position.x + key->position.x;
    key->process_spheres[index].position.y = particle->position.y + key->position.y;
    key->process_spheres[index].position.z = particle->position.z + key->position.z;
    key->process_spheres[index].momentum = particle->momentum;
    if (++key->sphere_index >= effect->process_spheres) {
        key->sphere_index = 0;
    }
    if (finite != 0) {
        key->sphere_skip_count = key->sphere_index;
    }
    key->sphere_next_time = time + effect->particle_lifetime / (f32)effect->process_spheres;
}

void __cdecl DebReAlloc2(DEBRISKEY_PC *key) {
    DEBRIS_INFO_PC *effect = debtab[key->effect_index];
    i32 old_count = key->allocated_chunk_count;
    i32 extra = key->previous_allocated_chunk_count - old_count;
    i32 glass = effect->particle_type == 7;
    i32 i, j, render_count = debrischunks + debrischunksglass;
    DEBRIS_RENDER_CHUNK_PC *render;
    if (extra == 0) {
        return;
    }
    if (extra > 0) {
        i32 *free_count = glass ? &freedebchkptrg : &freedebchkptr;
        i32 capacity = glass ? debrischunksglass : debrischunks;
        DEBRIS_PARTICLE_CHUNK_PC **chunks = glass ? freedebchunksglass : freedebchunks;
        if (*free_count + extra >= capacity) {
            return;
        }
        for (i = 0; i < extra; ++i) {
            key->particle_chunks[key->allocated_chunk_count + i] = chunks[*free_count + i];
            for (j = 0; j < (glass ? 12 : 32); ++j) {
                key->particle_chunks[key->allocated_chunk_count + i]->particles[j].start_time = 0.0f;
                key->particle_chunks[key->allocated_chunk_count + i]->particles[j].inverse_lifetime = 32768.0f;
            }
        }
        if (key->allocated_chunk_count == 0) {
            for (i = 0; i < render_count; ++i) {
                if (ParticleChunkToRender[i].particle_chunk == NULL) {
                    render = &ParticleChunkToRender[i];
                    render->particle_chunk = key->particle_chunks[0];
                    render->effect = effect;
                    render->key = key;
                    render->render_priority = key->render_priority;
                    if (key->field_2f6 != 0) {
                        AddChunkToRenderStack(render, &ParticleChunkRenderStack[effect->time_group]);
                    }
                    break;
                }
            }
        }
        key->allocated_chunk_count += (i16)extra;
        if (key->allocated_chunk_count == key->previous_allocated_chunk_count) {
            key->particle_count = key->previous_particle_count;
        } else {
            key->particle_count += glass ? 12 : 32;
        }
        *free_count += extra;
        LinkDmaParticalSets(key->particle_chunks, key->allocated_chunk_count);
        return;
    }
    if (freechunkcontrolsptr - extra > (debrischunks + debrischunksglass) * 2) {
        return;
    }
    DebrisGetControlStackLock();
    for (i = extra; i < 0; ++i) {
        DEBRIS_CHUNK_CONTROL_PC *control = freechunkcontrols[freechunkcontrolsptr];
        DEBRIS_INFO_PC *current_effect;
        i32 panel_time;
        control->particle_chunk = key->particle_chunks[key->allocated_chunk_count + i];
        control->active = effect->particle_type == 7 ? 7 : 0;
        panel_time = effect->time_group == 4;
        current_effect = debtab[key->effect_index];
        control->expiry_time = current_effect->particle_lifetime + (panel_time ? panelglobaltime : globaltime) +
                               (i8)current_effect->trail_count * current_effect->trail_time;
        AddChunkControlToStack(control, &debris_chunk_control_stack[panel_time]);
        ++freechunkcontrolsptr;
    }
    DebrisReleaseControlStackLock();
    if (extra == -key->allocated_chunk_count) {
        for (i = 0; i < render_count; ++i) {
            if (ParticleChunkToRender[i].particle_chunk == key->particle_chunks[0]) {
                render = &ParticleChunkToRender[i];
                render->effect = debtab[key->effect_index];
                render->key = NULL;
                render->effect_orientation = key->effect_orientation;
                render->position = key->position;
                render->render_priority = key->render_priority;
                break;
            }
        }
    } else {
        LinkDmaParticalSets(&key->particle_chunks[key->allocated_chunk_count + extra], -extra);
        for (i = 0; i < render_count; ++i) {
            if (ParticleChunkToRender[i].particle_chunk == NULL) {
                render = &ParticleChunkToRender[i];
                render->particle_chunk = key->particle_chunks[key->allocated_chunk_count + extra];
                render->effect = debtab[key->effect_index];
                render->key = NULL;
                render->effect_orientation = key->effect_orientation;
                render->position = key->position;
                render->render_priority = key->render_priority;
                if (key->field_2f6 != 0) {
                    AddChunkToRenderStack(render, &ParticleChunkRenderStack[render->effect->time_group]);
                }
                break;
            }
        }
    }
    for (i = extra; i < 0; ++i) {
        key->particle_chunks[key->allocated_chunk_count + i] = NULL;
    }
    key->allocated_chunk_count = key->previous_allocated_chunk_count;
    key->particle_count = key->previous_particle_count;
    if (key->previous_particle_count != 0) {
        LinkDmaParticalSets(key->particle_chunks, key->previous_allocated_chunk_count);
    }
}

void __cdecl DebReAlloc(DEBRISKEY_PC *key, i32 particle_count) {
    DEBRIS_INFO_PC *effect = debtab[key->effect_index];
    key->previous_particle_count = (i16)particle_count;
    key->previous_allocated_chunk_count =
        (i16)(effect->particle_type == 7 ? (particle_count + 11) / 12 : (particle_count + 31) / 32);
    if (key->previous_allocated_chunk_count > 32) {
        key->previous_allocated_chunk_count = 32;
        key->previous_particle_count = effect->particle_type == 7 ? 0x180 : 0x400;
    }
    if (key->allocated_chunk_count == key->previous_allocated_chunk_count) {
        key->particle_count = key->previous_particle_count;
    } else if (key->previous_allocated_chunk_count != 0 && key->allocated_chunk_count == 0) {
        DebReAlloc2(key);
    }
}

extern void *debkeydata;
#define DEBRIS_ENGINE_KEYS_PC ((DEBRISKEY_PC *)debkeydata)

void __cdecl RemoveDebrisEffectFromStack(DEBRISKEY_PC *key) {
    if (key->next == NULL) {
        debris_keystack = key->previous;
        if (key->previous != NULL) {
            key->previous->next = NULL;
        }
    } else {
        key->next->previous = key->previous;
        if (key->previous != NULL) {
            key->previous->next = key->next;
        }
    }
    key->previous = NULL;
    key->next = NULL;
}

void __cdecl DebFreeChunksInstantly(i32 *handle) {
    DEBRISKEY_PC *key = &DEBRIS_ENGINE_KEYS_PC[*handle];
    DEBRIS_INFO_PC *effect;
    i32 i;
    if (key->effect_index == 0 || key->allocated_chunk_count == 0) {
        return;
    }
    effect = debtab[key->effect_index];
    DebrisGetControlStackLock();
    for (i = 0; i < key->allocated_chunk_count; ++i) {
        DEBRIS_CHUNK_CONTROL_PC *control;
        i32 panel_time;
        RemoveAnyChunkControls(key->particle_chunks[i]);
        control = freechunkcontrols[freechunkcontrolsptr];
        control->particle_chunk = key->particle_chunks[i];
        control->active = effect->particle_type == 7 ? 9 : 2;
        control->owner = NULL;
        panel_time = effect->time_group == 4;
        control->expiry_time = panel_time ? panelglobaltime : globaltime;
        AddChunkControlToStack(control, &debris_chunk_control_stack[panel_time]);
        ++freechunkcontrolsptr;
    }
    DebrisReleaseControlStackLock();
    for (i = 0; i < debrischunks + debrischunksglass; ++i) {
        DEBRIS_RENDER_CHUNK_PC *render = &ParticleChunkToRender[i];
        if (render->particle_chunk == key->particle_chunks[0]) {
            if (key->field_2f6 != 0) {
                RemoveChunkFromRenderStack(render, &ParticleChunkRenderStack[effect->time_group]);
            }
            render->particle_chunk = NULL;
            render->effect = NULL;
        }
    }
    for (i = 0; i < key->allocated_chunk_count; ++i) {
        key->particle_chunks[i] = NULL;
    }
    key->particle_count = 0;
    key->allocated_chunk_count = 0;
    key->previous_particle_count = 0;
    key->previous_allocated_chunk_count = 0;
    key->controlled_chunk_count = 0;
    key->particle_cursor = 0;
}

void __cdecl DebFreeOrphansInstantly(DEBRIS_INFO_PC *effect) {
    i32 i;
    DebrisGetControlStackLock();
    for (i = 0; i < debrischunks + debrischunksglass; ++i) {
        DEBRIS_RENDER_CHUNK_PC *render = &ParticleChunkToRender[i];
        DEBRIS_CHUNK_CONTROL_PC *control;
        i32 panel_time;
        if (render->key != NULL || render->effect != effect) {
            continue;
        }
        RemoveAnyChunkControls(render->particle_chunk);
        control = freechunkcontrols[freechunkcontrolsptr];
        control->particle_chunk = render->particle_chunk;
        control->active = effect->particle_type == 7 ? 9 : 2;
        control->owner = NULL;
        panel_time = effect->time_group == 4;
        control->expiry_time = panel_time ? panelglobaltime : globaltime;
        AddChunkControlToStack(control, &debris_chunk_control_stack[panel_time]);
        ++freechunkcontrolsptr;
        RemoveChunkFromRenderStack(render, &ParticleChunkRenderStack[(i8)effect->time_group]);
        render->particle_chunk = NULL;
        render->effect = NULL;
    }
    DebrisReleaseControlStackLock();
}

void __cdecl DebFreeInstantly(i32 *handle) {
    i32 index, i, linked;
    DEBRISKEY_PC *key;
    DEBRIS_INFO_PC *effect;
    if (*handle == -1) {
        return;
    }
    index = *handle;
    key = &DEBRIS_ENGINE_KEYS_PC[index];
    if (key->allocation_locked != 0) {
        return;
    }
    key->allocation_locked = 1;
    for (i = freedebkeyptr; i < maxdebkeys; ++i) {
        if (freedebkeys[i] == index) {
            key->allocation_locked = 0;
            *handle = -1;
            return;
        }
    }
    if (key->effect_index == 0) {
        key->allocation_locked = 0;
        return;
    }
    effect = debtab[key->effect_index];
    DebFreeChunksInstantly(handle);
    linked = 0;
    for (i = 0; i < 8; ++i) {
        if (effect->particle_keys[i] == *handle) {
            effect->particle_keys[i] = -1;
            linked = 1;
        }
    }
    if (linked == 0) {
        RemoveDebrisEffectFromStack(&DEBRIS_ENGINE_KEYS_PC[*handle]);
    }
    DEBRIS_ENGINE_KEYS_PC[*handle].effect_index = 0;
    DEBRIS_ENGINE_KEYS_PC[*handle].allocation_index = -1;
    freedebkeys[--freedebkeyptr] = (i16)*handle;
    DEBRIS_ENGINE_KEYS_PC[*handle].allocation_locked = 0;
    *handle = -1;
}

void __cdecl DebFree(i32 *handle) {
    DEBRISKEY_PC *key;
    DEBRIS_INFO_PC *effect;
    i32 i, linked;
    if (*handle == -1) {
        return;
    }
    key = &DEBRIS_ENGINE_KEYS_PC[*handle];
    if (key->allocation_locked != 0) {
        return;
    }
    key->allocation_locked = 1;
    for (i = freedebkeyptr; i < maxdebkeys; ++i) {
        if (freedebkeys[i] == *handle) {
            key->allocation_locked = 0;
            *handle = -1;
            return;
        }
    }
    if (key->effect_index == 0) {
        key->allocation_locked = 0;
        return;
    }
    effect = debtab[key->effect_index];
    if (key->allocated_chunk_count != 0) {
        DebrisGetControlStackLock();
        for (i = 0; i < key->allocated_chunk_count; ++i) {
            DEBRIS_CHUNK_CONTROL_PC *control;
            DEBRIS_INFO_PC *current_effect;
            i32 panel_time;
            RemoveAnyChunkControls(key->particle_chunks[i]);
            control = freechunkcontrols[freechunkcontrolsptr];
            control->particle_chunk = key->particle_chunks[i];
            control->active = effect->particle_type == 7 ? 7 : 0;
            control->owner = NULL;
            panel_time = effect->time_group == 4;
            current_effect = debtab[key->effect_index];
            control->expiry_time = current_effect->particle_lifetime + (panel_time ? panelglobaltime : globaltime) +
                                   (i8)current_effect->trail_count * current_effect->trail_time;
            AddChunkControlToStack(control, &debris_chunk_control_stack[panel_time]);
            ++freechunkcontrolsptr;
        }
        DebrisReleaseControlStackLock();
        for (i = 0; i < debrischunks + debrischunksglass; ++i) {
            DEBRIS_RENDER_CHUNK_PC *render = &ParticleChunkToRender[i];
            if (render->particle_chunk == key->particle_chunks[0]) {
                render->key = NULL;
                render->effect = effect;
                render->effect_orientation = key->effect_orientation;
                render->position = key->position;
                break;
            }
        }
    }
    linked = 0;
    for (i = 0; i < 8; ++i) {
        if (effect->particle_keys[i] == *handle) {
            effect->particle_keys[i] = -1;
            linked = 1;
        }
    }
    if (linked == 0) {
        RemoveDebrisEffectFromStack(key);
    }
    for (i = 0; i < key->allocated_chunk_count; ++i) {
        key->particle_chunks[i] = NULL;
    }
    key->allocated_chunk_count = 0;
    key->effect_index = 0;
    key->allocation_index = -1;
    freedebkeys[--freedebkeyptr] = (i16)*handle;
    key->allocation_locked = 0;
    *handle = -1;
}

void __cdecl DebrisProcessControlChunks(i32 panel_time) {
    i32 i, slot;
    const f32 now = panel_time == 1 ? panelglobaltime : globaltime;
    DEBRIS_CHUNK_CONTROL_PC **stack = &debris_chunk_control_stack[panel_time];

    DebrisGetControlStackLock();
    while (*stack != NULL && (*stack)->expiry_time <= now) {
        DEBRIS_CHUNK_CONTROL_PC *control = *stack;
        *stack = control->next;
        control->next = NULL;

        if (control->active == 5) {
            DEBRIS_INFO_PC *effect = debtab[control->effect_index];
            DEBRISPARTICLE_PC *particle = &control->particle_chunk->particles[control->particle_index];
            const f32 time = control->collision_time;
            const f32 old_y = particle->momentum.y;
            const f64 gravity_velocity = 2.0 * (time * effect->gravity);
            const f32 gravity_sum = (f32)(old_y + gravity_velocity);
            const f32 reflected_y = (f32)(-control->restitution * gravity_sum);
            const f32 new_y = (f32)(reflected_y - gravity_velocity);
            f32 first, second;
            particle->momentum.y = new_y;
            particle->position.y -= time * new_y - time * old_y;
            if (SolveQuadratic(effect->gravity, new_y, particle->position.y - control->collision_plane, &first,
                               &second) != 0) {
                const f32 next_time = first > second ? first : second;
                if (effect->particle_lifetime > next_time) {
                    const f32 interval = next_time - control->collision_time;
                    control->collision_time = next_time;
                    if (interval > 0.02f) {
                        control->expiry_time += interval;
                        AddChunkControlToStack(control, stack);
                        continue;
                    }
                }
            }
            freechunkcontrols[--freechunkcontrolsptr] = control;
            control->particle_chunk = NULL;
            continue;
        }

        if (control->active == 6) {
            NUVEC normal = {1.0f, 0.0f, 0.0f};
            DEBRISPARTICLE_PC *particle = &control->particle_chunk->particles[control->particle_index];
            f32 old_x, old_z, projection, impulse, new_x, new_z;
            NuVecRotateY(&normal, &normal, control->rotation_y);
            old_x = particle->momentum.x;
            old_z = particle->momentum.z;
            projection = old_x * normal.x + old_z * normal.z;
            impulse = (f32)(-(1.0 + control->restitution) * projection);
            new_x = impulse * normal.x + old_x;
            new_z = impulse * normal.z + old_z;
            particle->momentum.x = new_x;
            particle->momentum.z = new_z;
            particle->position.x -= new_x * control->collision_time - old_x * control->collision_time;
            particle->position.z -= new_z * control->collision_time - old_z * control->collision_time;
            freechunkcontrols[--freechunkcontrolsptr] = control;
            control->particle_chunk = NULL;
            continue;
        }

        if (control->active == 0 || control->active == 7) {
            DEBRIS_RENDER_CHUNK_PC *render;
            render = NULL;
            for (i = 0; i < debrischunks + debrischunksglass; ++i) {
                if (ParticleChunkToRender[i].particle_chunk == control->particle_chunk) {
                    render = &ParticleChunkToRender[i];
                    break;
                }
            }
            if (render != NULL) {
                RemoveChunkFromRenderStack(render, &ParticleChunkRenderStack[render->effect->time_group]);
                render->particle_chunk = NULL;
                render->effect = NULL;
                render->key = NULL;
            }
            control->expiry_time = (f32)(control->expiry_time + (f64)0.1f);
            control->active = control->active == 7 ? 9 : 2;
            AddChunkControlToStack(control, stack);
            continue;
        }

        if (control->active == 1) {
            DEBRISKEY_PC *key = control->owner;
            DEBRIS_INFO_PC *effect = debtab[key->effect_index];
            i32 chunk_index = 0;
            const i32 old_chunk_count = key->allocated_chunk_count;
            for (i = 0; i < key->allocated_chunk_count; ++i) {
                if (key->particle_chunks[i] == control->particle_chunk) {
                    chunk_index = i;
                    break;
                }
            }

            for (i = chunk_index; i + 1 < old_chunk_count; ++i) {
                key->particle_chunks[i] = key->particle_chunks[i + 1];
            }
            key->particle_chunks[old_chunk_count - 1] = NULL;
            --key->previous_allocated_chunk_count;
            --key->controlled_chunk_count;
            --key->allocated_chunk_count;

            if (key->allocated_chunk_count <= 0) {
                key->particle_count = 0;
                key->previous_particle_count = 0;
                for (slot = 0; slot < 8; ++slot) {
                    const i16 key_index = effect->particle_keys[slot];
                    if (key_index != -1 && &DEBRIS_ENGINE_KEYS_PC[key_index] == key) {
                        key->effect_index = 0;
                        key->allocation_index = -1;
                        --freedebkeyptr;
                        freedebkeys[freedebkeyptr] = key_index;
                        effect->particle_keys[slot] = -1;
                    }
                }
                key->particle_chunks[0] = NULL;
            } else {
                const i32 particles_per_chunk = effect->particle_type == 7 ? 12 : 32;
                key->particle_count -= (i16)(particles_per_chunk);
                key->previous_particle_count -= (i16)(particles_per_chunk);
                if ((chunk_index + 1) * particles_per_chunk < key->particle_cursor) {
                    key->particle_cursor -= (i16)(particles_per_chunk);
                }
                LinkDmaParticalSets(key->particle_chunks, key->allocated_chunk_count);
            }

            if (chunk_index == 0) {
                DEBRIS_RENDER_CHUNK_PC *render_chunk;
                render_chunk = NULL;
                for (i = 0; i < debrischunks + debrischunksglass; ++i) {
                    if (ParticleChunkToRender[i].particle_chunk == control->particle_chunk) {
                        render_chunk = &ParticleChunkToRender[i];
                        break;
                    }
                }
                if (render_chunk != NULL) {
                    render_chunk->particle_chunk = key->particle_chunks[0];
                    if (key->allocated_chunk_count == 0) {
                        RemoveChunkFromRenderStack(render_chunk, &ParticleChunkRenderStack[effect->time_group]);
                        render_chunk->effect = NULL;
                        render_chunk->key = NULL;
                    }
                }
            }

            control->expiry_time = (f32)(control->expiry_time + (f64)0.1f);
            control->active = effect->particle_type == 7 ? 9 : 2;
            AddChunkControlToStack(control, stack);
            continue;
        }

        if (control->active == 2 || control->active == 9) {
            LinkDmaParticalSets(&control->particle_chunk, 1);
            control->active = control->active == 9 ? 8 : 3;
            control->expiry_time = (f32)(control->expiry_time + (f64)0.1f);
            AddChunkControlToStack(control, stack);
            continue;
        }

        if (control->active == 3) {
            --freedebchkptr;
            freedebchunks[freedebchkptr] = control->particle_chunk;
            for (i = 0; i < 32; ++i) {
                control->particle_chunk->particles[i].start_time = 10000000000.0f;
                control->particle_chunk->particles[i].inverse_lifetime = 128.0f;
            }
            freechunkcontrols[--freechunkcontrolsptr] = control;
            control->particle_chunk = NULL;
            continue;
        }

        if (control->active == 8) {
            --freedebchkptrg;
            freedebchunksglass[freedebchkptrg] = control->particle_chunk;
            for (i = 0; i < 12; ++i) {
                control->particle_chunk->particles[i].start_time = 10000000000.0f;
                control->particle_chunk->particles[i].inverse_lifetime = 128.0f;
            }
            freechunkcontrols[--freechunkcontrolsptr] = control;
            control->particle_chunk = NULL;
            continue;
        }
    }
    DebrisReleaseControlStackLock();
}

/* Original inline sound-event loop, kept private to the debris source group. */
#define DEBRIS_EMISSION_SOUND(key, effect, event, volume)                                                              \
    do {                                                                                                               \
        i32 sound_index;                                                                                               \
        for (sound_index = 0; sound_index < 4; ++sound_index) {                                                        \
            if ((effect)->sound_data[sound_index * 3] != -1 && (effect)->sound_data[sound_index * 3 + 1] == (event))   \
                PlaySfxByIdEx((effect)->sound_data[sound_index * 3], &(key)->position, (volume), 1.0f);                \
        }                                                                                                              \
    } while (0)

f32 __cdecl CameraEmitterDistance(NUVEC *position) {
    if (debris_cutoff_camera_vec != NULL)
        return NuVecDist(position, (NUVEC *)debris_cutoff_camera_vec, NULL);
    return 0.0f;
}

void __cdecl DebrisProcessAllocation(void) {
    DEBRISKEY_PC *key = debris_keystack;
    while (key != NULL) {
        DEBRISKEY_PC *next = key->previous;
        if (key->previous_particle_count != key->particle_count)
            DebReAlloc2(key);
        key = next;
    }
}

void __cdecl DebrisProcessGeneration(void) {
    DEBRISKEY_PC *next;
    DEBRIS_INFO_PC *effect;
    f32 now;
    f32 sound_range;
    f32 volume;
    f32 thinning;
    f32 elapsed;
    f32 frequency;
    f32 emission_interval;
    f32 emission_time;
    f32 pause;
    i32 transitions;
    DEBRISPARTICLE_PC *particle;
    DEBRISKEY_PC *key;
    i32 i;
    i32 emission;
    i32 handle;
    for (key = debris_keystack; key != NULL;) {
        next = key->previous;
        effect = debtab[key->effect_index];
        now = effect->time_group == 4 ? panelglobaltime : globaltime;
        if (key->emission_disabled != 0) {
            key = next;
            continue;
        }
        key->cutoff_distance = CameraEmitterDistance(&key->position);
        if (key->finite != 0) {
            if (key->cutoff_distance < effect->cut_on ||
                (effect->clip_extent > 0.0f && key->cutoff_distance > effect->clip_extent)) {
                if (effect->use_explicit_clip_box == 0 && effect->time_group != 4)
                    key->finite = 0;
            }
            if (debris_render_group != 0 && key->render_group != 0 && debris_render_group != key->render_group)
                key->finite = 0;
            if (((i8)key->orientation_flags & debris_detail_level) == 0)
                key->finite = 0;
        }
        sound_range = effect->sound_range_override;
        if (sound_range == 0.0f)
            sound_range = effect->sound_range;
        if (sound_range == 0.0f)
            sound_range = effect->clip_extent;
        volume = 1.0f;
        if (sound_range > key->cutoff_distance) {
            for (i = 0; i < 4; ++i)
                if (effect->sound_data[i * 3] != -1)
                    SetSfxBit_On(effect->sound_data[i * 3]);
            volume = (sound_range - key->cutoff_distance) / sound_range;
        }
        if (key->process_collision_sound != 0 && key->finite != 0 && sound_range > key->cutoff_distance)
            DEBRIS_EMISSION_SOUND(key, effect, 4, volume);
        if (effect->status == 0)
            key->finite = 0;
        if (effect->status == 2)
            key->finite = 1;
        if (key->field_2f4 == 0)
            key->finite = 0;
        else {
            if (key->field_2f4 == 2)
                key->finite = 1;
            if (key->finite != 0 && key->previous_allocated_chunk_count == 0) {
                DebReAlloc(key, effect->max_particles);
                thinning = forced_debris_thinning != 0
                               ? debris_thinning_level
                               : (effect->thinning < debris_thinning_level ? effect->thinning : debris_thinning_level);
                DebReAlloc(key, (i32)(effect->max_particles / thinning));
            }
        }
        elapsed = now - key->last_update_time;
        key->emission_position.x = key->emitter_momentum.x * elapsed;
        key->emission_position.y = key->emitter_momentum.y * elapsed;
        key->emission_position.z = key->emitter_momentum.z * elapsed;
        frequency = (f32)effect->frequency;
        thinning = forced_debris_thinning != 0
                       ? debris_thinning_level
                       : (effect->thinning < debris_thinning_level ? effect->thinning : debris_thinning_level);
        emission_interval = frequency > 0.0f ? 1.0f / (frequency / thinning) : 0.0f;
        emission_time = key->emission_epoch + emission_interval;
        pause = 0.0f;
        for (emission = 1; emission != 100 && emission_time < now + debris_time_increment; ++emission) {
            transitions = 100;
            while (emission_time >= key->emission_end_time && emission_time >= key->emission_time &&
                   --transitions != 0) {
                if (!(key->emission_time < key->emission_end_time)) {
                    key->emission_end_time = key->emission_time + effect->emission_period +
                                             NuRandFloatSeeded(&debrisseed) * effect->emission_period_random;
                    if (key->finite_count > 0) {
                        if (--key->finite_count == 0) {
                            handle = key->allocation_index;
                            DebFree(&handle);
                            emission_time = (f32)((f64)emission_time + 99999.0);
                            key->process_collision_sound = 0;
                        }
                    }
                    if (key->finite_count < 0 && ++key->finite_count == 0) {
                        key->field_2f4 = 0;
                        key->finite = 0;
                    }
                    if (key->process_collision_sound != 0 && key->finite != 0 && sound_range > key->cutoff_distance)
                        DEBRIS_EMISSION_SOUND(key, effect, 1, volume);
                    pause = 0.0f;
                } else {
                    key->previous_emission_time = key->emission_time;
                    pause = effect->emission_pause + NuRandFloatSeeded(&debrisseed) * effect->emission_pause_random;
                    key->emission_time = key->emission_end_time + pause;
                    if (key->finite == 2)
                        key->finite = 0;
                    if (key->process_collision_sound != 0 && key->finite != 0 && sound_range > key->cutoff_distance)
                        DEBRIS_EMISSION_SOUND(key, effect, 2, volume);
                }
            }
            if (pause > 0.0f) {
                emission_time = key->emission_time - emission_interval;
            } else if (key->allocated_chunk_count > 0 && key->finite != 0) {
                particle = key->generator(key, effect, emission_time);
                if (effect->process_spheres != 0 && particle != NULL && emission == 1)
                    DebrisProcessSpheres(particle, emission_time, effect, key, 0);
                if (key->process_collision_sound != 0 && sound_range > key->cutoff_distance)
                    DEBRIS_EMISSION_SOUND(key, effect, 3, volume);
            }
            key->emission_epoch = emission_time;
            emission_time += emission_interval;
        }
        if (key->finite == 0) {
            if (key->field_2f4 != 0 && key->trigger_second == -1 && key->cutoff_distance >= effect->cut_on &&
                (effect->clip_extent == 0.0f || key->cutoff_distance <= effect->clip_extent) &&
                (debris_render_group == 0 || key->render_group == 0 || debris_render_group == key->render_group) &&
                ((i8)key->orientation_flags & debris_detail_level) != 0)
                key->finite = 1;
            else if (key->previous_allocated_chunk_count != 0)
                DebReAlloc(key, 0);
        }
        key = next;
    }
}

void __cdecl DebrisProcessTriggers(void) {
    i32 last_switch_change;
    DEBRIS_INFO_PC *effect;
    DEBRISKEY_PC *next;
    f32 shift;
    f32 range;
    f32 volume;
    DEBRISKEY_PC *key;
    i32 i;
    i32 switch_changes[32][2];
    last_switch_change = -1;
    for (key = debris_keystack; key != NULL;) {
        effect = debtab[key->effect_index];
        next = key->previous;
        if (key->trigger_first == 1 && key->finite != 2 && key->trigger_second != -1) {
            switch (debris_object_switches[key->trigger_second]) {
                case 0:
                    key->finite = 0;
                    break;
                case 1:
                    key->finite = 1;
                    break;
                case 3:
                    last_switch_change = (last_switch_change + 1) & 31;
                    switch_changes[last_switch_change][0] = key->trigger_second;
                    switch_changes[last_switch_change][1] = 0;
                    break;
                case 2:
                    last_switch_change = (last_switch_change + 1) & 31;
                    key->finite = 2;
                    switch_changes[last_switch_change][0] = key->trigger_second;
                    switch_changes[last_switch_change][1] = 3;
                    goto restart_emission;
                case 4: {
                    last_switch_change = (last_switch_change + 1) & 31;
                    key->finite = 2;
                    switch_changes[last_switch_change][0] = key->trigger_second;
                    switch_changes[last_switch_change][1] = 0;
                restart_emission:
                    DebrisStartOffsetEx(key, 0.0f);
                    shift = key->emission_time - globaltime;
                    key->emission_disabled = 0;
                    key->emission_epoch = globaltime;
                    key->emission_time -= shift;
                    key->emission_end_time -= shift;
                    if (key->process_collision_sound != 0) {
                        key->cutoff_distance = CameraEmitterDistance(&key->position);
                        range = effect->sound_range_override;
                        if (range == 0.0f)
                            range = effect->sound_range;
                        if (range == 0.0f)
                            range = effect->clip_extent;
                        volume = 0.0f;
                        if (range > key->cutoff_distance) {
                            if (effect->sound_data[0] != -1)
                                SetSfxBit_On(effect->sound_data[0]);
                            if (effect->sound_data[3] != -1)
                                SetSfxBit_On(effect->sound_data[3]);
                            if (effect->sound_data[6] != -1)
                                SetSfxBit_On(effect->sound_data[6]);
                            if (effect->sound_data[9] != -1)
                                SetSfxBit_On(effect->sound_data[9]);
                            volume = (range - key->cutoff_distance) / range;
                        }
                        if (range > key->cutoff_distance) {
                            if (effect->sound_data[0] != -1 && effect->sound_data[1] == 1)
                                PlaySfxByIdEx(effect->sound_data[0], &key->position, volume, 1.0f);
                            if (effect->sound_data[3] != -1 && effect->sound_data[4] == 1)
                                PlaySfxByIdEx(effect->sound_data[3], &key->position, volume, 1.0f);
                            if (effect->sound_data[6] != -1 && effect->sound_data[7] == 1)
                                PlaySfxByIdEx(effect->sound_data[6], &key->position, volume, 1.0f);
                            if (effect->sound_data[9] != -1 && effect->sound_data[10] == 1)
                                PlaySfxByIdEx(effect->sound_data[9], &key->position, volume, 1.0f);
                        }
                    }
                    break;
                }
            }
        }
        key = next;
    }
    for (i = 0; i <= last_switch_change; ++i)
        debris_object_switches[switch_changes[i][0]] = switch_changes[i][1];
}

void __cdecl DebrisTimeSlip(i32 group) {
    DEBRIS_CHUNK_CONTROL_PC *control;
    i32 i, chunk_index, particle_index;
    if (group == 0) {
        globaltime = debris_render_global_time;
        for (control = debris_chunk_control_stack[0]; control != NULL; control = control->next)
            control->expiry_time -= 800.0;
    } else if (group == 1) {
        panelglobaltime = debris_render_panel_time;
        for (control = debris_chunk_control_stack[1]; control != NULL; control = control->next)
            control->expiry_time -= 800.0;
    }
    for (i = 0; i < maxdebkeys; ++i) {
        DEBRISKEY_PC *key = &DEBRIS_ENGINE_KEYS_PC[i];
        DEBRIS_INFO_PC *effect = debtab[key->effect_index];
        if (effect->time_group == 4 ? group != 1 : group != 0)
            continue;
        key->last_update_time -= 800.0;
        key->emission_epoch -= 800.0;
        key->emission_time -= 800.0;
        key->previous_emission_time -= 800.0;
        key->emission_end_time -= 800.0;
        for (chunk_index = 0; chunk_index < 32; ++chunk_index) {
            DEBRIS_PARTICLE_CHUNK_PC *chunk = key->particle_chunks[chunk_index];
            i32 count;
            if (chunk == NULL)
                continue;
            count = effect->particle_type == 7 ? 12 : 32;
            for (particle_index = 0; particle_index < count; ++particle_index)
                chunk->particles[particle_index].start_time -= 800.0;
        }
    }
    for (i = 1; i < debinf_count; ++i) {
        DEBRIS_INFO_PC *effect = debtab[i];
        if (effect == NULL)
            continue;
        if (effect->time_group == 4 ? group != 1 : group != 0)
            continue;
        effect->last_render_time -= 800.0;
        if (effect->native_data != NULL)
            effect->native_data->last_render_time -= 800.0;
    }
}

void __cdecl DebrisProcessTimeSlip(void) {
    if (globaltime - 1.0 > debris_render_global_time)
        DebrisTimeSlip(0);
    else if (globaltime > 900.0)
        debris_render_global_time -= 800.0;
    if (panelglobaltime - 1.0 > debris_render_panel_time)
        DebrisTimeSlip(1);
    else if (panelglobaltime > 900.0)
        debris_render_panel_time -= 800.0;
}

void __cdecl DebrisFreeOldestDmaDebTypeTable(void) {
    i32 i, oldest = 0;
    f32 oldest_age = 0.0f;
    for (i = 1; i < debinf_count; ++i) {
        DEBRIS_INFO_PC *effect = debtab[i];
        f32 age;
        if (effect == NULL || effect->native_data == NULL)
            continue;
        age = (effect->time_group == 4 ? panelglobaltime : globaltime) - effect->last_render_time;
        if (age > oldest_age) {
            oldest_age = age;
            oldest = i;
        }
    }
    if (oldest != 0) {
        DmaDebTypes[--freeDmaDebType] = debtab[oldest]->native_data;
        debtab[oldest]->native_data = NULL;
    }
}

void __cdecl DebFreeAllCreatedEffects(void) {
    i32 i;
    for (i = 0; i < maxdebkeys; ++i) {
        DEBRISKEY_PC *key = &((DEBRISKEY_PC *)debkeydata)[i];
        if (key->effect_index != 0 && key->field_2f9 != 0) {
            i32 handle = i;
            DebFreeInstantly(&handle);
        }
    }
}
