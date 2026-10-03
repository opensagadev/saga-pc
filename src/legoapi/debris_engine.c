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
