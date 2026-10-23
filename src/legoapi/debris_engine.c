#include "saga.h"

#include <stdio.h>
#include <string.h>

void __cdecl DebrisGetControlStackLock(void) {
    while (debris_control_stack_lock != 0) {
    }
    debris_control_stack_lock = 1;
}

void __cdecl DebrisReleaseControlStackLock(void) {
    debris_control_stack_lock = 0;
}

void __cdecl AddChunkControlToStack(DEBRIS_CHUNK_CONTROL_PC *control, DEBRIS_CHUNK_CONTROL_PC **stack) {
    DEBRIS_CHUNK_CONTROL_PC *current = *stack;
    while (current != NULL && current->expiry_time < control->expiry_time) {
        stack = &current->next;
        current = current->next;
    }
    control->next = current;
    *stack = control;
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
    i32 extra = key->previous_allocated_chunk_count - key->allocated_chunk_count;
    i32 i, j, render_count, panel_time;
    if (extra == 0)
        return;
    if (extra > 0) {
        if (effect->particle_type == 7) {
            if (freedebchkptrg + extra >= debrischunksglass)
                return;
            for (i = 0; i < extra; ++i) {
                key->particle_chunks[key->allocated_chunk_count + i] = freedebchunksglass[freedebchkptrg + i];
                for (j = 0; j < 12; ++j) {
                    key->particle_chunks[key->allocated_chunk_count + i]->particles[j].start_time = 0.0f;
                    key->particle_chunks[key->allocated_chunk_count + i]->particles[j].inverse_lifetime = 32768.0f;
                }
            }
            if (key->allocated_chunk_count == 0) {
                render_count = debrischunks + debrischunksglass;
                for (i = 0; i < render_count; ++i) {
                    if (ParticleChunkToRender[i].particle_chunk == NULL) {
                        ParticleChunkToRender[i].particle_chunk = key->particle_chunks[0];
                        ParticleChunkToRender[i].effect = effect;
                        ParticleChunkToRender[i].key = key;
                        ParticleChunkToRender[i].render_priority = key->render_priority;
                        if (key->field_2f6 != 0)
                            AddChunkToRenderStack(&ParticleChunkToRender[i],
                                                  &ParticleChunkRenderStack[effect->time_group]);
                        break;
                    }
                }
            }
            key->allocated_chunk_count += extra;
            if (key->allocated_chunk_count == key->previous_allocated_chunk_count)
                key->particle_count = key->previous_particle_count;
            else
                key->particle_count += 12;
            freedebchkptrg += extra;
            LinkDmaParticalSets(key->particle_chunks, key->allocated_chunk_count);
        } else {
            if (freedebchkptr + extra >= debrischunks)
                return;
            for (i = 0; i < extra; ++i) {
                key->particle_chunks[key->allocated_chunk_count + i] = freedebchunks[freedebchkptr + i];
                for (j = 0; j < 32; ++j) {
                    key->particle_chunks[key->allocated_chunk_count + i]->particles[j].start_time = 0.0f;
                    key->particle_chunks[key->allocated_chunk_count + i]->particles[j].inverse_lifetime = 32768.0f;
                }
            }
            if (key->allocated_chunk_count == 0) {
                render_count = debrischunksglass + debrischunks;
                for (i = 0; i < render_count; ++i) {
                    if (ParticleChunkToRender[i].particle_chunk == NULL) {
                        ParticleChunkToRender[i].particle_chunk = key->particle_chunks[0];
                        ParticleChunkToRender[i].effect = effect;
                        ParticleChunkToRender[i].key = key;
                        ParticleChunkToRender[i].render_priority = key->render_priority;
                        if (key->field_2f6 != 0)
                            AddChunkToRenderStack(&ParticleChunkToRender[i],
                                                  &ParticleChunkRenderStack[effect->time_group]);
                        break;
                    }
                }
            }
            key->allocated_chunk_count += extra;
            if (key->allocated_chunk_count == key->previous_allocated_chunk_count)
                key->particle_count = key->previous_particle_count;
            else
                key->particle_count += 32;
            freedebchkptr += extra;
            LinkDmaParticalSets(key->particle_chunks, key->allocated_chunk_count);
        }
        return;
    }
    if (freechunkcontrolsptr - extra > (debrischunks + debrischunksglass) * 2)
        return;
    DebrisGetControlStackLock();
    for (i = extra; i < 0; ++i) {
        freechunkcontrols[freechunkcontrolsptr]->particle_chunk = key->particle_chunks[key->allocated_chunk_count + i];
        if (effect->particle_type == 7)
            freechunkcontrols[freechunkcontrolsptr]->active = 7;
        else
            freechunkcontrols[freechunkcontrolsptr]->active = 0;
        if (effect->time_group == 4) {
            freechunkcontrols[freechunkcontrolsptr]->expiry_time =
                (i8)debtab[key->effect_index]->trail_count * debtab[key->effect_index]->trail_time +
                (debtab[key->effect_index]->particle_lifetime + panelglobaltime);
            panel_time = 1;
        } else {
            freechunkcontrols[freechunkcontrolsptr]->expiry_time =
                (i8)debtab[key->effect_index]->trail_count * debtab[key->effect_index]->trail_time +
                (debtab[key->effect_index]->particle_lifetime + globaltime);
            panel_time = 0;
        }
        AddChunkControlToStack(freechunkcontrols[freechunkcontrolsptr], &debris_chunk_control_stack[panel_time]);
        ++freechunkcontrolsptr;
    }
    DebrisReleaseControlStackLock();
    if (extra == -key->allocated_chunk_count) {
        render_count = debrischunksglass + debrischunks;
        for (i = 0; i < render_count; ++i) {
            if (ParticleChunkToRender[i].particle_chunk == key->particle_chunks[0]) {
                ParticleChunkToRender[i].effect = debtab[key->effect_index];
                ParticleChunkToRender[i].key = NULL;
                ParticleChunkToRender[i].effect_orientation = key->effect_orientation;
                ParticleChunkToRender[i].position.x = key->position.x;
                ParticleChunkToRender[i].position.y = key->position.y;
                ParticleChunkToRender[i].position.z = key->position.z;
                ParticleChunkToRender[i].render_priority = key->render_priority;
                break;
            }
        }
    } else {
        LinkDmaParticalSets(&key->particle_chunks[key->allocated_chunk_count + extra], -extra);
        render_count = debrischunksglass + debrischunks;
        for (i = 0; i < render_count; ++i) {
            if (ParticleChunkToRender[i].particle_chunk == NULL) {
                ParticleChunkToRender[i].particle_chunk = key->particle_chunks[key->allocated_chunk_count + extra];
                ParticleChunkToRender[i].effect = debtab[key->effect_index];
                ParticleChunkToRender[i].key = NULL;
                ParticleChunkToRender[i].effect_orientation = key->effect_orientation;
                ParticleChunkToRender[i].position.x = key->position.x;
                ParticleChunkToRender[i].position.y = key->position.y;
                ParticleChunkToRender[i].position.z = key->position.z;
                ParticleChunkToRender[i].render_priority = key->render_priority;
                if (key->field_2f6 != 0)
                    AddChunkToRenderStack(&ParticleChunkToRender[i],
                                          &ParticleChunkRenderStack[debtab[key->effect_index]->time_group]);
                break;
            }
        }
    }
    for (i = extra; i < 0; ++i)
        key->particle_chunks[key->allocated_chunk_count + i] = NULL;
    key->allocated_chunk_count = key->previous_allocated_chunk_count;
    key->particle_count = key->previous_particle_count;
    if (key->previous_particle_count != 0)
        LinkDmaParticalSets(key->particle_chunks, key->previous_allocated_chunk_count);
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

void DebFreeAllChunksInstantly(void) {
    i32 i;
    for (i = 0; i < maxdebkeys; ++i) {
        i32 handle;
        if (debkeydata[i].effect_index == 0) {
            continue;
        }
        handle = i;
        DebFreeChunksInstantly(&handle);
    }
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
    DEBRIS_INFO_PC *effect;
    DEBRIS_CHUNK_CONTROL_PC *control, *current, **cursor;
    i32 i, linked, panel_time;
    if (*handle == -1)
        return;
    if (DEBRIS_ENGINE_KEYS_PC[*handle].allocation_locked != 0)
        return;
    DEBRIS_ENGINE_KEYS_PC[*handle].allocation_locked = 1;
    for (i = freedebkeyptr; i < maxdebkeys; ++i) {
        if (freedebkeys[i] == *handle) {
            DEBRIS_ENGINE_KEYS_PC[*handle].allocation_locked = 0;
            *handle = -1;
            return;
        }
    }
    if (DEBRIS_ENGINE_KEYS_PC[*handle].effect_index == 0) {
        DEBRIS_ENGINE_KEYS_PC[*handle].allocation_locked = 0;
        return;
    }
    effect = debtab[DEBRIS_ENGINE_KEYS_PC[*handle].effect_index];
    if (DEBRIS_ENGINE_KEYS_PC[*handle].allocated_chunk_count != 0) {
        DebrisGetControlStackLock();
        for (i = 0; i < DEBRIS_ENGINE_KEYS_PC[*handle].allocated_chunk_count; ++i) {
            RemoveAnyChunkControls(DEBRIS_ENGINE_KEYS_PC[*handle].particle_chunks[i]);
            freechunkcontrols[freechunkcontrolsptr]->particle_chunk = DEBRIS_ENGINE_KEYS_PC[*handle].particle_chunks[i];
            if (effect->particle_type == 7)
                freechunkcontrols[freechunkcontrolsptr]->active = 7;
            else
                freechunkcontrols[freechunkcontrolsptr]->active = 0;
            freechunkcontrols[freechunkcontrolsptr]->owner = NULL;
            if (effect->time_group == 4) {
                freechunkcontrols[freechunkcontrolsptr]->expiry_time =
                    (i8)debtab[DEBRIS_ENGINE_KEYS_PC[*handle].effect_index]->trail_count *
                        debtab[DEBRIS_ENGINE_KEYS_PC[*handle].effect_index]->trail_time +
                    (debtab[DEBRIS_ENGINE_KEYS_PC[*handle].effect_index]->particle_lifetime + panelglobaltime);
                panel_time = 1;
            } else {
                freechunkcontrols[freechunkcontrolsptr]->expiry_time =
                    (i8)debtab[DEBRIS_ENGINE_KEYS_PC[*handle].effect_index]->trail_count *
                        debtab[DEBRIS_ENGINE_KEYS_PC[*handle].effect_index]->trail_time +
                    (debtab[DEBRIS_ENGINE_KEYS_PC[*handle].effect_index]->particle_lifetime + globaltime);
                panel_time = 0;
            }
            control = freechunkcontrols[freechunkcontrolsptr];
            cursor = &debris_chunk_control_stack[panel_time];
            current = *cursor;
            while (current != NULL && current->expiry_time < control->expiry_time) {
                cursor = &current->next;
                current = current->next;
            }
            control->next = current;
            *cursor = control;
            ++freechunkcontrolsptr;
        }
        DebrisReleaseControlStackLock();
        for (i = 0; i < debrischunks + debrischunksglass; ++i) {
            if (ParticleChunkToRender[i].particle_chunk == DEBRIS_ENGINE_KEYS_PC[*handle].particle_chunks[0]) {
                ParticleChunkToRender[i].effect = effect;
                ParticleChunkToRender[i].key = NULL;
                ParticleChunkToRender[i].effect_orientation = DEBRIS_ENGINE_KEYS_PC[*handle].effect_orientation;
                ParticleChunkToRender[i].position.x = DEBRIS_ENGINE_KEYS_PC[*handle].position.x;
                ParticleChunkToRender[i].position.y = DEBRIS_ENGINE_KEYS_PC[*handle].position.y;
                ParticleChunkToRender[i].position.z = DEBRIS_ENGINE_KEYS_PC[*handle].position.z;
                break;
            }
        }
    }
    linked = 0;
    for (i = 0; i < 8; ++i) {
        if (effect->particle_keys[i] == *handle) {
            linked = 1;
            effect->particle_keys[i] = -1;
        }
    }
    if (linked == 0) {
        if (DEBRIS_ENGINE_KEYS_PC[*handle].next == NULL) {
            debris_keystack = DEBRIS_ENGINE_KEYS_PC[*handle].previous;
            if (DEBRIS_ENGINE_KEYS_PC[*handle].previous != NULL)
                DEBRIS_ENGINE_KEYS_PC[*handle].previous->next = NULL;
        } else {
            DEBRIS_ENGINE_KEYS_PC[*handle].next->previous = DEBRIS_ENGINE_KEYS_PC[*handle].previous;
            if (DEBRIS_ENGINE_KEYS_PC[*handle].previous != NULL)
                DEBRIS_ENGINE_KEYS_PC[*handle].previous->next = DEBRIS_ENGINE_KEYS_PC[*handle].next;
        }
        DEBRIS_ENGINE_KEYS_PC[*handle].next = NULL;
        DEBRIS_ENGINE_KEYS_PC[*handle].previous = NULL;
    }
    for (i = 0; i < DEBRIS_ENGINE_KEYS_PC[*handle].allocated_chunk_count; ++i)
        DEBRIS_ENGINE_KEYS_PC[*handle].particle_chunks[i] = NULL;
    DEBRIS_ENGINE_KEYS_PC[*handle].allocated_chunk_count = 0;
    DEBRIS_ENGINE_KEYS_PC[*handle].effect_index = 0;
    DEBRIS_ENGINE_KEYS_PC[*handle].allocation_index = -1;
    freedebkeys[--freedebkeyptr] = (i16)*handle;
    DEBRIS_ENGINE_KEYS_PC[*handle].allocation_locked = 0;
    *handle = -1;
}

void __cdecl DebrisProcessControlChunks(i32 panel_time) {
    const f32 now = panel_time == 1 ? panelglobaltime : globaltime;
    DEBRIS_CHUNK_CONTROL_PC *control = debris_chunk_control_stack[panel_time];
    i32 i;

    DebrisGetControlStackLock();
    for (; control != NULL && control->expiry_time <= now; control = debris_chunk_control_stack[panel_time]) {
        debris_chunk_control_stack[panel_time] = control->next;
        control->next = NULL;

        if (control->active == 5) {
            DEBRIS_INFO_PC *effect = debtab[control->effect_index];
            DEBRISPARTICLE_PC *particle = &control->particle_chunk->particles[control->particle_index];
            const f32 time = control->collision_time;
            const f32 old_y = particle->momentum.y;
            const f32 gravity_velocity = 2.0f * (time * effect->gravity);
            const f32 new_y = -control->restitution * (gravity_velocity + old_y) - gravity_velocity;
            f32 first, second;
            particle->momentum.y = new_y;
            particle->position.y -= time * new_y - time * old_y;
            if (SolveQuadratic(effect->gravity, particle->momentum.y, particle->position.y - control->collision_plane,
                               &first, &second) != 0) {
                if (effect->particle_lifetime > MAX(first, second)) {
                    const f32 interval = MAX(first, second) - control->collision_time;
                    control->collision_time = MAX(first, second);
                    if (!(interval <= 0.02f)) {
                        control->expiry_time += interval;
                        AddChunkControlToStack(control, &debris_chunk_control_stack[panel_time]);
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
            f32 old_x, old_z, impulse, new_x, new_z;
            NuVecRotateY(&normal, &normal, control->rotation_y);
            old_x = particle->momentum.x;
            old_z = particle->momentum.z;
            impulse = -(1.0f + control->restitution) * (old_x * normal.x + old_z * normal.z);
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

        if (control->active == 2) {
            LinkDmaParticalSets(&control->particle_chunk, 1);
            control->active = 3;
            control->expiry_time += 0.1f;
            AddChunkControlToStack(control, &debris_chunk_control_stack[panel_time]);
            continue;
        }

        if (control->active == 9) {
            LinkDmaParticalSets(&control->particle_chunk, 1);
            control->active = 8;
            control->expiry_time += 0.1f;
            AddChunkControlToStack(control, &debris_chunk_control_stack[panel_time]);
            continue;
        }

        if (control->active == 0 || control->active == 7) {
            for (i = 0; i < debrischunks + debrischunksglass; ++i) {
                if (ParticleChunkToRender[i].particle_chunk == control->particle_chunk) {
                    RemoveChunkFromRenderStack(&ParticleChunkToRender[i],
                                               &ParticleChunkRenderStack[ParticleChunkToRender[i].effect->time_group]);
                    ParticleChunkToRender[i].particle_chunk = NULL;
                    ParticleChunkToRender[i].effect = NULL;
                    ParticleChunkToRender[i].key = NULL;
                    break;
                }
            }
            control->expiry_time += 0.1f;
            if (control->active == 0)
                control->active = 2;
            else if (control->active == 7)
                control->active = 9;
            AddChunkControlToStack(control, &debris_chunk_control_stack[panel_time]);
            continue;
        }

        if (control->active == 1) {
            DEBRIS_INFO_PC *effect = debtab[control->owner->effect_index];
            i32 chunk_index = 0;
            for (i = 0; i < control->owner->allocated_chunk_count; ++i) {
                if (control->owner->particle_chunks[i] == control->particle_chunk) {
                    chunk_index = i;
                    break;
                }
            }
            for (i = chunk_index; i + 1 < control->owner->allocated_chunk_count; ++i) {
                control->owner->particle_chunks[i] = control->owner->particle_chunks[i + 1];
            }
            control->owner->particle_chunks[control->owner->allocated_chunk_count - 1] = NULL;
            --control->owner->allocated_chunk_count;
            --control->owner->previous_allocated_chunk_count;
            --control->owner->controlled_chunk_count;

            if (control->owner->allocated_chunk_count > 0) {
                if (effect->particle_type == 7) {
                    control->owner->particle_count -= 12;
                    control->owner->previous_particle_count -= 12;
                    if (control->owner->particle_cursor > (chunk_index + 1) * 12) {
                        control->owner->particle_cursor -= 12;
                    }
                } else {
                    control->owner->particle_count -= 32;
                    control->owner->previous_particle_count -= 32;
                    if (control->owner->particle_cursor > (chunk_index + 1) * 32) {
                        control->owner->particle_cursor -= 32;
                    }
                }
            } else {
                control->owner->particle_count = 0;
                control->owner->previous_particle_count = 0;
            }
            if (control->owner->allocated_chunk_count > 0) {
                LinkDmaParticalSets(control->owner->particle_chunks, control->owner->allocated_chunk_count);
            } else {
                for (i = 0; i < 8; ++i) {
                    if (effect->particle_keys[i] != -1 &&
                        &DEBRIS_ENGINE_KEYS_PC[effect->particle_keys[i]] == control->owner) {
                        control->owner->effect_index = 0;
                        control->owner->allocation_index = -1;
                        --freedebkeyptr;
                        freedebkeys[freedebkeyptr] = effect->particle_keys[i];
                        effect->particle_keys[i] = -1;
                    }
                }
                control->owner->particle_chunks[0] = NULL;
            }

            if (chunk_index == 0) {
                for (i = 0; i < debrischunks + debrischunksglass; ++i) {
                    if (ParticleChunkToRender[i].particle_chunk == control->particle_chunk) {
                        ParticleChunkToRender[i].particle_chunk = control->owner->particle_chunks[0];
                        if (control->owner->allocated_chunk_count == 0) {
                            RemoveChunkFromRenderStack(&ParticleChunkToRender[i],
                                                       &ParticleChunkRenderStack[effect->time_group]);
                            ParticleChunkToRender[i].effect = NULL;
                            ParticleChunkToRender[i].key = NULL;
                        }
                        break;
                    }
                }
            }

            control->expiry_time += 0.1f;
            control->active = effect->particle_type == 7 ? 9 : 2;
            AddChunkControlToStack(control, &debris_chunk_control_stack[panel_time]);
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
    for (key = debris_keystack; key != NULL; key = next) {
        next = key->previous;
        effect = debtab[key->effect_index];
        if (key->trigger_first != 1 || key->finite == 2)
            continue;
        if (key->trigger_second != -1) {
            switch (debris_object_switches[key->trigger_second]) {
                case 0:
                    key->finite = 0;
                    break;
                case 1:
                    key->finite = 1;
                    break;
                case 2:
                    last_switch_change = (last_switch_change + 1) & 31;
                    switch_changes[last_switch_change][1] = 3;
                    key->finite = 2;
                    switch_changes[last_switch_change][0] = key->trigger_second;
                    break;
                case 3:
                    last_switch_change = (last_switch_change + 1) & 31;
                    switch_changes[last_switch_change][1] = 0;
                    switch_changes[last_switch_change][0] = key->trigger_second;
                    break;
                case 4:
                    last_switch_change = (last_switch_change + 1) & 31;
                    switch_changes[last_switch_change][1] = 0;
                    key->finite = 2;
                    switch_changes[last_switch_change][0] = key->trigger_second;
                    break;
            }
        }
        if (key->finite != 2)
            continue;
        DebrisStartOffsetEx(key, 0.0f);
        key->emission_disabled = 0;
        shift = key->emission_time - globaltime;
        key->emission_time -= shift;
        key->emission_end_time -= shift;
        key->emission_epoch = globaltime;
        if (key->process_collision_sound == 0)
            continue;
        volume = 0.0f;
        key->cutoff_distance = CameraEmitterDistance(&key->position);
        range = effect->sound_range_override == 0.0f
                    ? (effect->sound_range == 0.0f ? effect->clip_extent : effect->sound_range)
                    : effect->sound_range_override;
        if (range > key->cutoff_distance) {
            for (i = 0; i < 4; ++i)
                if (effect->sound_data[i * 3] != -1)
                    SetSfxBit_On(effect->sound_data[i * 3]);
            volume = (range - key->cutoff_distance) / range;
        }
        if (range > key->cutoff_distance)
            DEBRIS_EMISSION_SOUND(key, effect, 1, volume);
    }
    for (i = 0; i <= last_switch_change; ++i)
        debris_object_switches[switch_changes[i][0]] = (u8)switch_changes[i][1];
}

void __cdecl DebrisTimeSlip(i32 group) {
    DEBRIS_CHUNK_CONTROL_PC *control;
    i32 i, chunk_index, particle_index, glass;
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
        DEBRIS_INFO_PC *effect = debtab[DEBRIS_ENGINE_KEYS_PC[i].effect_index];
        if ((effect->time_group == 4 && group == 1) || (effect->time_group != 4 && group == 0)) {
            DEBRIS_ENGINE_KEYS_PC[i].last_update_time -= 800.0;
            glass = effect->particle_type == 7;
            DEBRIS_ENGINE_KEYS_PC[i].emission_epoch -= 800.0;
            DEBRIS_ENGINE_KEYS_PC[i].emission_time -= 800.0;
            DEBRIS_ENGINE_KEYS_PC[i].previous_emission_time -= 800.0;
            DEBRIS_ENGINE_KEYS_PC[i].emission_end_time -= 800.0;
            for (chunk_index = 0; chunk_index < 32; ++chunk_index) {
                if (DEBRIS_ENGINE_KEYS_PC[i].particle_chunks[chunk_index] != NULL) {
                    if (glass) {
                        for (particle_index = 0; particle_index < 12; ++particle_index)
                            DEBRIS_ENGINE_KEYS_PC[i]
                                .particle_chunks[chunk_index]
                                ->particles[particle_index]
                                .start_time -= 800.0;
                    } else {
                        for (particle_index = 0; particle_index < 32; ++particle_index)
                            DEBRIS_ENGINE_KEYS_PC[i]
                                .particle_chunks[chunk_index]
                                ->particles[particle_index]
                                .start_time -= 800.0;
                    }
                }
            }
        }
    }
    for (i = 1; i < debinf_count; ++i) {
        if (debtab[i] != NULL &&
            ((debtab[i]->time_group == 4 && group == 1) || (debtab[i]->time_group != 4 && group == 0))) {
            debtab[i]->last_render_time -= 800.0;
            if (debtab[i]->native_data != NULL)
                debtab[i]->native_data->last_render_time -= 800.0;
        }
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

i32 __cdecl CreateScaledEffect(i32 effect_index, f32 requested_scale) {
    DEBRIS_INFO_PC *effect, *scaled;
    i32 i, closest_index, scaled_index;
    f32 ratio, distance, closest_distance;
    char shortened[16];
    if (effect_index < 1 || effect_index > debinf_count || debtab[effect_index] == NULL)
        return -1;
    effect = debtab[effect_index];
    if (effect->scale != 1.0f) {
        effect_index = effect->base_effect_index;
        effect = debtab[effect_index];
        if (effect == NULL)
            return -1;
    }
    if (requested_scale == 1.0f && effect_index != 0)
        return effect_index;
    if (requested_scale < 0.01f)
        requested_scale = 0.01f;
    for (i = 0; i < EDPP_SCALE_TYPES; ++i) {
        DEBRIS_SCALE_CACHE_PC *entry = &debscale[i];
        if (entry->base_effect_index == effect_index && entry->scale == requested_scale &&
            entry->scaled_effect_index != 0) {
            scaled = debtab[entry->scaled_effect_index];
            if (scaled != NULL && scaled->base_effect_index == effect_index)
                return entry->scaled_effect_index;
            entry->scale = 0.0f;
        }
    }
    closest_index = effect_index;
    ratio = requested_scale / effect->scale;
    closest_distance = ratio > 1.0f ? ratio - 1.0f : 1.0f - ratio;
    for (i = 1; i < debinf_count; ++i) {
        scaled = debtab[i];
        if (scaled != NULL && scaled->base_effect_index == effect_index) {
            ratio = TerrainDivideNonzeroPC(requested_scale, scaled->scale);
            distance = ratio > 1.0f ? ratio - 1.0f : 1.0f - ratio;
            if (distance < closest_distance) {
                closest_distance = distance;
                closest_index = i;
            }
        }
    }
    if (closest_index != 0 && closest_distance < 0.1f)
        return closest_index;
    for (scaled_index = 1; scaled_index < debinf_count && debtab[scaled_index] != NULL; ++scaled_index) {
    }
    if (scaled_index == debinf_count)
        return closest_index;
    scaled = &effecttypes[scaled_index];
    debtab[scaled_index] = scaled;
    *scaled = *effect;
    scaled->native_data = NULL;
    for (i = 0; i < 8; ++i)
        scaled->particle_keys[i] = -1;
    DebrisReScale(scaled_index, requested_scale);
    scaled = debtab[scaled_index];
    scaled->last_render_time = scaled->time_group == 4 ? panelglobaltime : globaltime;
    scaled->scale = requested_scale;
    scaled->base_effect_index = effect_index;
    if (strlen(debtab[effect_index]->name) <= 12) {
        sprintf(scaled->name, "%s%03d", debtab[effect_index]->name, scaled_index);
    } else {
        sprintf(shortened, GizAIMessage_NameFormat, debtab[effect_index]->name);
        shortened[12] = 0;
        sprintf(scaled->name, "%s%03d", shortened, scaled_index);
    }
    ++edpp_types_used;
    for (i = 0; i < EDPP_SCALE_TYPES; ++i) {
        if (debscale[i].scale == 0.0f) {
            debscale[i].base_effect_index = effect_index;
            debscale[i].scaled_effect_index = scaled_index;
            debscale[i].scale = requested_scale;
            break;
        }
    }
    return scaled_index;
}

void __cdecl DebrisReScale(i32 effect_index, f32 scale) {
    DEBRIS_INFO_PC *effect;
    i32 i;
    u8 generator;
    if (effect_index < 0 || effect_index >= debinf_count || debtab[effect_index] == NULL)
        return;
    effect = debtab[effect_index];
    for (i = 0; i < 8; ++i) {
        effect->width_keys[i].value *= scale;
        effect->height_keys[i].value *= scale;
    }
    effect->min_size *= scale;
    effect->max_size *= scale;
    effect->momentum_base *= scale;
    effect->gravity *= scale;
    generator = (u8)effect->generator_type;
    if (generator == 0 || generator == 8 || generator == 9 || generator == 10) {
        effect->emitter_random_extent.x *= scale;
        effect->emitter_random_extent.y *= scale;
        effect->emitter_random_extent.z *= scale;
        effect->momentum_random_x *= scale;
        effect->rotor_speed_x *= scale;
        effect->rotor_speed_y *= scale;
    } else if (generator == 6 || generator == 7 || generator == 11 || generator == 12) {
        effect->emitter_random_extent.x *= scale;
        effect->momentum_random_x *= scale;
    }
    effect->jib_x_amplitude *= scale;
    effect->jib_y_amplitude *= scale;
    GenericDebinfoDmaTypeUpdate(effect);
}

void __cdecl DebrisPopulateInstance(i32 handle, f32 duration) {
    DEBRISKEY_PC *key;
    DEBRIS_INFO_PC *effect;
    f32 now, end_time, emission_time, thinning, interval;
    i32 remaining;
    if (duration < 0.0f || handle == -1)
        return;
    key = &((DEBRISKEY_PC *)debkeydata)[handle];
    effect = debtab[key->effect_index];
    if (effect->frequency == 0)
        return;
    if (duration == 0.0f)
        duration = effect->particle_lifetime;
    now = effect->time_group == 4 ? panelglobaltime : globaltime;
    end_time = now + debris_time_increment;
    emission_time = now - duration;
    if (key->previous_allocated_chunk_count == 0) {
        thinning = forced_debris_thinning != 0 || !(debris_thinning_level > effect->thinning) ? debris_thinning_level
                                                                                              : effect->thinning;
        DebReAlloc(key, (i32)((f32)effect->max_particles / thinning));
    }
    key->finite = 1;
    if (key->allocated_chunk_count <= 0)
        return;
    thinning = forced_debris_thinning != 0 || !(debris_thinning_level > effect->thinning) ? debris_thinning_level
                                                                                          : effect->thinning;
    interval = 1.0f / ((f32)effect->frequency / thinning);
    key->emission_epoch = emission_time;
    emission_time += interval;
    for (remaining = 999; emission_time < end_time && remaining != 0; --remaining) {
        DEBRISPARTICLE_PC *particle = key->generator(key, effect, emission_time);
        if (effect->process_spheres != 0 && particle != NULL)
            DebrisProcessSpheres(particle, emission_time, effect, key, 0);
        emission_time = key->emission_epoch + interval;
    }
}

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

i32 __cdecl DebrisSingleCollisionCheckScaleYFlag(i32 key_index, NUVEC *position, f32 radius, f32 y_scale, u8 flags) {
    DEBRISKEY_PC *key = &DEBRIS_ENGINE_KEYS_PC[key_index];
    DEBRIS_INFO_PC *effect;
    DEBRIS_COLLISION_EFFECT_PC *collision;
    i32 i;
    if (key->effect_index == 0 || key->effect_index == -1)
        return 0;
    effect = debtab[key->effect_index];
    if (effect == NULL || (effect->collision_flags & flags) == 0 || effect->process_spheres <= 0)
        return 0;
    collision = (DEBRIS_COLLISION_EFFECT_PC *)effect;
    for (i = 0; i < effect->process_spheres; ++i) {
        DEBRIS_PROCESS_SPHERE_PC *sphere = &key->process_spheres[i];
        f32 age = globaltime - sphere->time;
        f32 time, effect_radius, dx, dy, dz;
        if (!(age >= 0.0f && age <= effect->particle_lifetime))
            continue;
        debris_collide_pt.x = sphere->momentum.x * age;
        debris_collide_pt.y = sphere->momentum.y * age;
        debris_collide_pt.z = sphere->momentum.z * age;
        debris_collide_pt.y = age * age * effect->gravity + debris_collide_pt.y;
        debris_collide_pt.x += sphere->position.x;
        debris_collide_pt.y += sphere->position.y;
        debris_collide_pt.z += sphere->position.z;
        time = age / effect->particle_lifetime;
        {
            i32 segment;
            f32 numerator, denominator;
            effect_radius = 0.0f;
            for (segment = 0; segment < 7; ++segment) {
                if (time >= collision->collision_keys[segment].time &&
                    time <= collision->collision_keys[segment + 1].time) {
                    numerator = time - collision->collision_keys[segment].time;
                    denominator = collision->collision_keys[segment + 1].time - collision->collision_keys[segment].time;
                    effect_radius = collision->collision_keys[segment].value +
                                    (numerator / denominator) * (collision->collision_keys[segment + 1].value -
                                                                 collision->collision_keys[segment].value);
                    break;
                }
            }
        }
        if (!(effect_radius > 0.0f))
            continue;
        dx = position->x - debris_collide_pt.x;
        dy = position->y - debris_collide_pt.y;
        dz = position->z - debris_collide_pt.z;
        if (y_scale != 1.0f)
            dy *= (radius + effect_radius) / (radius * y_scale + effect_radius);
        if ((radius + effect_radius) * (radius + effect_radius) > dx * dx + dy * dy + dz * dz)
            return 1;
    }
    return 0;
}
i32 __cdecl DebrisSingleTorusCollisionCheckScaleYFlag(i32 key_index, NUVEC *position, f32 radius, f32 y_scale,
                                                      u8 flags) {
    DEBRISKEY_PC *key = &DEBRIS_ENGINE_KEYS_PC[key_index];
    DEBRIS_INFO_PC *effect;
    DEBRIS_COLLISION_EFFECT_PC *collision;
    f32 age, time, ring_radius, horizontal_radius, vertical_radius, dx, dy, dz;
    if (key->effect_index == 0 || key->effect_index == -1)
        return 0;
    effect = debtab[key->effect_index];
    if (effect == NULL || (effect->collision_flags & flags) == 0)
        return 0;
    collision = (DEBRIS_COLLISION_EFFECT_PC *)effect;
    if (collision->torus_lifetime == 0.0f)
        return 0;
    age = globaltime > key->emission_time ? globaltime - key->emission_time : globaltime - key->previous_emission_time;
    if (!(age > 0.0f && age < collision->torus_lifetime))
        return 0;
    time = age / collision->torus_lifetime;
    {
        i32 segment;
        f32 numerator, denominator;
        ring_radius = 0.0f;
        for (segment = 0; segment < 7; ++segment) {
            if (time >= collision->torus_keys1[segment].time && time <= collision->torus_keys1[segment + 1].time) {
                numerator = time - collision->torus_keys1[segment].time;
                denominator = collision->torus_keys1[segment + 1].time - collision->torus_keys1[segment].time;
                ring_radius = collision->torus_keys1[segment].value +
                              (numerator / denominator) *
                                  (collision->torus_keys1[segment + 1].value - collision->torus_keys1[segment].value);
                break;
            }
        }
    }
    {
        i32 segment;
        f32 numerator, denominator;
        horizontal_radius = 0.0f;
        for (segment = 0; segment < 7; ++segment) {
            if (time >= collision->torus_keys2[segment].time && time <= collision->torus_keys2[segment + 1].time) {
                numerator = time - collision->torus_keys2[segment].time;
                denominator = collision->torus_keys2[segment + 1].time - collision->torus_keys2[segment].time;
                horizontal_radius = collision->torus_keys2[segment].value +
                                    (numerator / denominator) * (collision->torus_keys2[segment + 1].value -
                                                                 collision->torus_keys2[segment].value);
                break;
            }
        }
    }
    {
        i32 segment;
        f32 numerator, denominator;
        vertical_radius = 0.0f;
        for (segment = 0; segment < 7; ++segment) {
            if (time >= collision->torus_keys3[segment].time && time <= collision->torus_keys3[segment + 1].time) {
                numerator = time - collision->torus_keys3[segment].time;
                denominator = collision->torus_keys3[segment + 1].time - collision->torus_keys3[segment].time;
                vertical_radius = collision->torus_keys3[segment].value +
                                  (numerator / denominator) * (collision->torus_keys3[segment + 1].value -
                                                               collision->torus_keys3[segment].value);
                break;
            }
        }
    }
    ring_radius *= collision->torus_radius1;
    horizontal_radius *= collision->torus_radius2;
    vertical_radius *= collision->torus_radius2;
    debris_collide_pt.x = position->x - key->position.x;
    debris_collide_pt.y = 0.0f;
    debris_collide_pt.z = position->z - key->position.z;
    NuVecNorm(&debris_collide_pt, &debris_collide_pt);
    NuVecScale(&debris_collide_pt, &debris_collide_pt, ring_radius);
    debris_collide_pt.x += key->position.x;
    debris_collide_pt.y += key->position.y;
    debris_collide_pt.z += key->position.z;
    dx = position->x - debris_collide_pt.x;
    dy = position->y - debris_collide_pt.y;
    dz = position->z - debris_collide_pt.z;
    if (horizontal_radius != vertical_radius || y_scale != 1.0f)
        dy *= (radius + horizontal_radius) / (radius * y_scale + vertical_radius);
    return (radius + horizontal_radius) * (radius + horizontal_radius) > dx * dx + dy * dy + dz * dz;
}
i32 __cdecl DebrisCollisionCheckScaleYFlag(NUVEC *position, f32 radius, f32 y_scale, u8 flags) {
    i32 i;
    for (i = 0; i < numspherecolldata; ++i)
        if ((spherecolldata[i].flags & flags) != 0 &&
            DebrisSingleCollisionCheckScaleYFlag(spherecolldata[i].key_index, position, radius, y_scale, flags) != 0)
            return spherecolldata[i].key_index;
    return -1;
}
i32 __cdecl DebrisTorusCollisionCheckScaleYFlag(NUVEC *position, f32 radius, f32 y_scale, u8 flags) {
    i32 i;
    for (i = 0; i < numtoruscolldata; ++i)
        if ((toruscolldata[i].flags & flags) != 0 &&
            DebrisSingleTorusCollisionCheckScaleYFlag(toruscolldata[i].key_index, position, radius, y_scale, flags) !=
                0)
            return toruscolldata[i].key_index;
    return -1;
}
i32 __cdecl DebrisPreCheckCollisions(NUVEC *position, f32 radius) {
    DEBRISKEY_PC *key = debris_keystack;
    numspherecolldata = 0;
    numtoruscolldata = 0;
    while (key != NULL) {
        i32 effect_index = key->effect_index, key_index = key->allocation_index;
        DEBRISKEY_PC *previous = key->previous;
        if (debtab[effect_index] == NULL) {
            DebFreeInstantly(&key_index);
            key->allocation_index = -1;
        }
        if (key->finite != 0) {
            if (DebrisSingleCollisionCheckScaleYFlag(key_index, position, radius, 1.0f, 3) != 0) {
                spherecolldata[numspherecolldata].key_index = key_index;
                spherecolldata[numspherecolldata++].flags = debtab[effect_index]->collision_flags;
            }
            if (DebrisSingleTorusCollisionCheckScaleYFlag(key_index, position, radius, 1.0f, 3) != 0) {
                toruscolldata[numtoruscolldata].key_index = key_index;
                toruscolldata[numtoruscolldata++].flags = debtab[effect_index]->collision_flags;
            }
        }
        key = previous;
    }
    return numspherecolldata + numtoruscolldata;
}

i32 edanimParamCreate(i32 instance_id) {
    i32 start;
    i32 index;
    EDANIM_PARAM_PC *param;
    nuhspecial_s special;
    i32 page;
    if (edanim_params_used == 64) {
        return -1;
    }
    start = edanim_next_param;
    index = start;
    while (AnimParams[index].instance_id != -1) {
        index = index + 1 >= 65 ? 0 : index + 1;
        if (index == start) {
            edanim_next_param = start;
            return -1;
        }
    }
    param = &(AnimParams[index]);
    edanim_next_param = index;
    (*param).effect_count = 0;
    (*param).instance_id = instance_id;

    NuGScnGetSpecial(&special, edbits_base_scene, instance_id);
    (*param).platform_id = FindPlatInst(NuSpecialGetInstanceix(&special));
    (*param).bounce_impulse = 0.0f;
    (*param).bounce_spring = 0.0f;
    (*param).bounce_damping = 0.0f;
    if ((*param).platform_id != -1) {
        PlatInstBounce((*param).platform_id, 0.0f, 0.0f, 0.0f);
    }
    page = edbits_anim_page;
    (*param).page = ((i8)(page));
    edanim_page_used[page] = 1;
    edanim_page_on[page] = 1;
    if (!edanim_page_scene[page]) {
        edanim_page_scene[page] = edbits_base_scene;
    }
    edanim_next_param = index + 1;
    ++edanim_params_used;
    return index;
}

i32 __cdecl DebrisTorusCollisionCheckScaleY(NUVEC *p, f32 r, f32 y) {
    return DebrisTorusCollisionCheckScaleYFlag(p, r, y, 3);
}
i32 __cdecl DebrisCollisionCheckScaleY(NUVEC *p, f32 r, f32 y) {
    return DebrisCollisionCheckScaleYFlag(p, r, y, 3);
}
i32 __cdecl DebrisCollisionCheckFlag(NUVEC *p, f32 r, u8 f) {
    return DebrisCollisionCheckScaleYFlag(p, r, 1.0f, f);
}
i32 __cdecl DebrisCollisionCheck(NUVEC *p, f32 r) {
    return DebrisCollisionCheckScaleYFlag(p, r, 1.0f, 3);
}
i32 __cdecl DebrisAllCollisionCheckScaleYFlag(NUVEC *p, f32 r, f32 y, u8 f) {
    i32 result = DebrisCollisionCheckScaleYFlag(p, r, y, f);
    if (result == -1) {
        DebrisTorusCollisionCheckScaleYFlag(p, r, y, f);
        return -1;
    }
    return result;
}

extern i32 DebrisGlassInitialized;
extern void *DebrisGlassCopyMaterial;
extern f32 DebrisGlassU1;
extern f32 DebrisGlassV1;
i32 __cdecl NuPower2(i32 value);

void __cdecl DebrisGlassInit(void) {
    NUMTL *material;
    f32 size;

    if (DebrisGlassInitialized != 0) {
        return;
    }
    size = (f32)NuPower2(PS2_REZ_W);
    DebrisGlassU1 = (f32)((PS2_REZ_W - 1.0) / size);
    size = (f32)NuPower2(PS2_REZ_H);
    DebrisGlassV1 = (f32)((PS2_REZ_H - 1.0) / size);
    if (DebrisGlassCopyMaterial == NULL) {
        material = NuMtlCreateEx(1, 14);
        material->diffuse_r = 1.0f;
        material->diffuse_g = 1.0f;
        material->diffuse_b = 1.0f;
        material->opacity = 0.999f;
        material->texture_id = 1;
        material->attribs.unknown_1_1_2 = 1;
        material->attribs.unknown_1_4_8 = 1;
        material->attribs.cull_mode = 2;
        material->attribs.z_mode = 3;
        material->attribs.alpha_mode = 0;
        material->attribs.filter_mode = 0;
        material->attribs.unknown_2_1_2 = 2;
        material->attribs.unknown_2_4 = 1;
        DebrisGlassCopyMaterial = material;
        NuMtlUpdate(material);
    }
    DebrisGlassInitialized = 1;
}
extern i32 DebrisProcessingEnabledPc, globalframes;
extern i32 debris_sort_delay, debris_sort_this_stack, debris_update_delay, debris_update_this_stack;
extern i32 DebrisDmaCleanupDelayPc, DebrisDmaCleanupIndexPc;
void __cdecl edppDeleteEffect(i32);
extern i32 edpp_nearest;
extern NUVEC edpp_cam_pos;
void __cdecl edppDetermineNearest(f32);
void __cdecl SortDebrisRenderStack(void) {
    i32 stack_index;
    DEBRIS_RENDER_CHUNK_PC *chunk;
    if (--debris_sort_delay > 0)
        return;
    debris_sort_delay = 6;
    stack_index = debris_sort_this_stack;
    while (ParticleChunkRenderStack[stack_index] == NULL) {
        if (++stack_index >= 5)
            stack_index = 0;
        if (stack_index == debris_sort_this_stack)
            break;
    }
    debris_sort_this_stack = stack_index;
    chunk = ParticleChunkRenderStack[stack_index];
    while (chunk != NULL && chunk->next != NULL) {
        DEBRIS_RENDER_CHUNK_PC *next = chunk->next;
        if (chunk->render_priority < next->render_priority ||
            (chunk->render_priority == next->render_priority &&
             chunk->effect->particle_type > next->effect->particle_type)) {
            RemoveChunkFromRenderStack(next, &ParticleChunkRenderStack[stack_index]);
            AddChunkToRenderStack(next, &ParticleChunkRenderStack[debris_sort_this_stack]);
            return;
        }
        chunk = next;
    }
}
void __cdecl UpdateDebrisRenderStackPriority(void) {
    i32 stack_index;
    DEBRIS_RENDER_CHUNK_PC *chunk;
    if (--debris_update_delay > 0)
        return;
    debris_update_delay = 6;
    stack_index = debris_update_this_stack;
    while (ParticleChunkRenderStack[stack_index] == NULL) {
        if (++stack_index >= 5)
            stack_index = 0;
        if (stack_index == debris_update_this_stack) {
            debris_update_this_stack = stack_index;
            if (ParticleChunkRenderStack[stack_index] == NULL)
                return;
            break;
        }
    }
    debris_update_this_stack = stack_index;
    for (chunk = ParticleChunkRenderStack[debris_sort_this_stack]; chunk != NULL; chunk = chunk->next) {
        DEBRISKEY_PC *key = chunk->key;
        if (key != NULL) {
            if (key->timed_flags != 0) {
                f32 priority = (u16)key->render_priority + key->cutoff_distance * 25.0;
                if (priority > 65535.0)
                    priority = -1.0f;
                chunk->render_priority = (u16)(i32)priority;
            } else
                chunk->render_priority = key->render_priority;
        }
    }
}
void __cdecl DebrisCleanUpDmaDebTypeTables(void) {
    if (++DebrisDmaCleanupDelayPc > 5) {
        DEBRIS_INFO_PC *effect = debtab[DebrisDmaCleanupIndexPc];
        DebrisDmaCleanupDelayPc = 0;
        if (effect != NULL && effect->native_data != NULL) {
            f32 now = effect->time_group == 4 ? panelglobaltime : globaltime;
            if (now > effect->last_render_time + 5.0) {
                DmaDebTypes[--freeDmaDebType] = effect->native_data;
                debtab[DebrisDmaCleanupIndexPc]->native_data = NULL;
                if (debtab[DebrisDmaCleanupIndexPc]->scale != 1.0f)
                    edppDeleteEffect(DebrisDmaCleanupIndexPc);
            }
        }
        if (++DebrisDmaCleanupIndexPc >= debinf_count)
            DebrisDmaCleanupIndexPc = 1;
    }
}
void __cdecl Debris(i32 update_panel_time) {
    if (update_debris_enabled == 0 || DebrisProcessingEnabledPc == 0 || debris_suspended != 0)
        return;
    panelglobaltime += debris_time_increment;
    debris_render_panel_time += debris_time_increment;
    DebrisProcessControlChunks(1);
    if (update_panel_time == 0) {
        globaltime += debris_time_increment;
        debris_render_global_time += debris_time_increment;
        ++globalframes;
        DebrisCleanUpDmaDebTypeTables();
        DebrisProcessTriggers();
        DebrisProcessAllocation();
        DebrisProcessGeneration();
        DebrisProcessControlChunks(0);
    }
}

i32 edbriProc(f32 delta_time, FRAME_PAD_PC *pad) {
    if (edbri_active_menu) {
        eduiMenuProcess(edbri_active_menu, delta_time, pad);
        return 0;
    }
    edbriDoInput(pad);
    edbriDetermineNearest(1.0f);
    return (pad->digital_buttons_pressed >> 11) & 1;
}
static void edbricbFileLoad(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    char path[256];
    edbriMakePath(path);
    edbriBridgesReset();
    if (NuFileExists(path))
        edbriLoadPage(path, edbits_base_scene);
    edbriStartAllPages();
    edbriDetermineNearest(1.0f);
    eduiCreateMessageMenu(menu, "Loaded OK", 1);
}
void __cdecl DebrisShift(NUMTX *transform) {
    i32 i;
    for (i = 0; i < maxdebkeys; ++i) {
        DEBRISKEY_PC *key = &DEBRIS_ENGINE_KEYS_PC[i];
        DEBRIS_INFO_PC *effect;
        i32 slot;
        if (key->effect_index == 0)
            continue;
        effect = debtab[key->effect_index];
        for (slot = 0; slot < 8; ++slot)
            if (effect->particle_keys[slot] == i)
                break;
        if (slot != 8) {
            i32 group = effect->time_group == 4;
            if (key->controlled_chunk_count != 0) {
                i32 chunk;
                for (chunk = 0; chunk < key->controlled_chunk_count; ++chunk) {
                    DEBRIS_CHUNK_CONTROL_PC *control;
                    for (control = debris_chunk_control_stack[group]; control != NULL; control = control->next) {
                        if (control->particle_chunk == key->particle_chunks[chunk]) {
                            RemoveChunkControlFromStack(control, &debris_chunk_control_stack[group]);
                            freechunkcontrols[--freechunkcontrolsptr] = control;
                            break;
                        }
                    }
                }
                key->controlled_chunk_count = 0;
            }
            DebReAlloc(key, 0);
        } else {
            NuMtxMulR(&key->effect_orientation, transform, &key->effect_orientation);
            NuVecMtxTransform(&key->position, &key->position, transform);
            key->orientation_dirty = 0.0f;
        }
    }
    for (i = 0; i < debrischunks + debrischunksglass; ++i) {
        DEBRIS_RENDER_CHUNK_PC *chunk = &ParticleChunkToRender[i];
        if (chunk->particle_chunk != NULL && chunk->key == NULL) {
            NuMtxMulR(&chunk->effect_orientation, transform, &chunk->effect_orientation);
            NuVecMtxTransform(&chunk->position, &chunk->position, transform);
        }
    }
}
i32 __cdecl DebrisFindAllOfType(i32 effect_index, NUVEC *positions, i32 *handles, i32 capacity, i32 skip,
                                i32 active_only) {
    i32 count = 0, i;
    for (i = 0; i < maxdebkeys; ++i) {
        DEBRISKEY_PC *key = &DEBRIS_ENGINE_KEYS_PC[i];
        if (key->effect_index == effect_index && (active_only == 0 || key->allocated_chunk_count != 0)) {
            i32 output_index = count - skip;
            if (output_index >= 0 && output_index < capacity) {
                if (positions != NULL)
                    positions[output_index] = key->position;
                if (handles != NULL)
                    handles[output_index] = i;
            }
            ++count;
        }
    }
    return count - skip;
}
i32 __cdecl DebrisGetRingProperties(i32 handle, NUVEC *origin, NUVEC *direction, f32 *radius) {
    DEBRISKEY_PC *key = &DEBRIS_ENGINE_KEYS_PC[handle];
    DEBRIS_INFO_PC *effect = debtab[key->effect_index];
    NUVEC axis;
    if (effect == NULL)
        return 0;
    *origin = key->position;
    axis.x = 0.0f;
    if (effect->generator_type == 11) {
        axis.y = 1.0f;
        axis.z = 0.0f;
    } else {
        axis.y = 0.0f;
        axis.z = 1.0f;
    }
    NuVecMtxTransform(&axis, &axis, &key->emitter_orientation);
    *direction = axis;
    if (effect->generator_type == 6 || effect->generator_type == 7 || effect->generator_type == 11) {
        *radius = effect->emitter_random_extent.x;
        return 1;
    }
    *radius = 0.0f;
    return 0;
}
i32 __cdecl DebrisGetConeProperties(i32 handle, NUVEC *origin, NUVEC *end, f32 *start_radius, f32 *end_radius,
                                    NUVEC *direction, f32 *length, f32 *speed) {
    DEBRISKEY_PC *key = &DEBRIS_ENGINE_KEYS_PC[handle];
    DEBRIS_INFO_PC *effect = debtab[key->effect_index];
    NUVEC axis;
    if (effect == NULL)
        return 0;
    *origin = key->position;
    if (effect->generator_type == 8) {
        f32 cone_length;
        axis.x = 0.0f;
        axis.y = 1.0f;
        axis.z = 0.0f;
        NuVecMtxTransform(&axis, &axis, &key->emitter_orientation);
        *direction = axis;
        *speed = effect->momentum_base;
        cone_length = effect->momentum_base * effect->particle_lifetime;
        *length = cone_length;
        end->x = cone_length * direction->x + origin->x;
        end->y = direction->y * *length + origin->y;
        end->z = *length * direction->z + origin->z;
        *start_radius = (effect->emitter_random_extent.x + effect->emitter_random_extent.z) * 0.5;
        *end_radius =
            (effect->momentum_random_x + effect->rotor_speed_y) / 2.8f * effect->particle_lifetime + *start_radius;
        return 1;
    }
    end->x = end->y = end->z = 0.0f;
    *direction = *end;
    *start_radius = *end_radius = *length = *speed = 0.0f;
    return 0;
}

void __cdecl DebrisStatusAlwaysOff(i32 *handle) {
    if (*handle != -1)
        ((DEBRISKEY_PC *)debkeydata)[*handle].field_2f4 = 0;
}
