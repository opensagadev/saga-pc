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
