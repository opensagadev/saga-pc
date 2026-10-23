#include "saga.h"

void __cdecl NuDebrisRendererFlushBuffers(void) {
    if (NuDebrisUseSystemMemory == 0 && NuDebrisVertexData != NULL)
        PCDeviceUnlockVertexBuffer(NuDebrisVertexBuffers[NuDebrisCurrentBufferIndex]);
    NuDebrisVertexData = NULL;
    NuDebrisCurrentBufferIndex = 0;
    NuDebrisUseSystemMemory = 0;
    NuDebrisCurrentVertexCount = 0;
    NuDebrisLastHeader = NULL;
    NuDebrisFrameVertexCount = 0;
}

i32 __cdecl NuDebrisRendererNextBuffer(void) {
    if (NuDebrisUseSystemMemory == 0 && NuDebrisVertexData != NULL)
        PCDeviceUnlockVertexBuffer(NuDebrisVertexBuffers[NuDebrisCurrentBufferIndex]);
    if (NuDebrisUseSystemMemory != 0) {
        if (NuDebrisCurrentBufferIndex + 1 >= 8) {
            PCClipScreenUnsupported();
            return 0;
        }
        NuDebrisCurrentBufferIndex = (NuDebrisCurrentBufferIndex + 1) & 7;
    } else if (NuDebrisCurrentBufferIndex + 1 >= 4) {
        NuDebrisCurrentBufferIndex = 0;
        NuDebrisUseSystemMemory = 1;
    } else {
        NuDebrisCurrentBufferIndex = (NuDebrisCurrentBufferIndex + 1) & 3;
    }
    NuDebrisCurrentVertexCount = 0;
    if (NuDebrisUseSystemMemory != 0)
        NuDebrisVertexData = NuDebrisSystemBuffers[NuDebrisCurrentBufferIndex];
    else
        PCDeviceLockVertexBuffer(0, NuDebrisVertexBufferSize, &NuDebrisVertexData, 0x2000,
                                 NuDebrisVertexBuffers[NuDebrisCurrentBufferIndex]);
    return 1;
}

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

void __cdecl NuRndrParticleSetRepeat(NUVEC *position) {
    NUVEC repeat;
    repeat.x = (position->x - NuRndrDebBase.x) / NuRndrDebRange.x;
    repeat.y = (position->y - NuRndrDebBase.y) / NuRndrDebRange.y;
    repeat.z = (position->z - NuRndrDebBase.z) / NuRndrDebRange.z;
    repeat.x -= (f32)((i32)(repeat.x + 65536.0) - 65536);
    repeat.y -= (f32)((i32)(repeat.y + 65536.0) - 65536);
    repeat.z -= (f32)((i32)(repeat.z + 65536.0) - 65536);
    position->x = NuRndrDebBase.x + repeat.x * NuRndrDebRange.x;
    position->y = NuRndrDebBase.y + repeat.y * NuRndrDebRange.y;
    position->z = NuRndrDebBase.z + repeat.z * NuRndrDebRange.z;
}

void __cdecl AddParticleGroupToDisplayList(NATIVE_PARTICLE_PACKET_PC *group) {
    NUDISPLAYLIST_PC *list = group->material->display_list;
    VARIPTR *buffer;
    list->scene->render_flags |= 2;
    (&list->scene->material_used_front)[(list->scene->render_flags >> 15) & 1][list->material_id / 8] |=
        (u8)(1 << (list->material_id % 8));
    DisplayListUpdateRenderState(list, &render_state);
    NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15;
    buffer = NuDisplayListStreamCursor;
    ((NUDISPLAYLISTITEM *)list->material_last)->next = buffer->void_ptr;
    list->items = buffer->void_ptr;
    buffer->addr += sizeof(NUDISPLAYLISTITEM);
    ((NUDISPLAYLISTITEM *)buffer->void_ptr)->type = 0x8d;
    ((NUDISPLAYLISTITEM *)buffer->void_ptr)->id = 1;
    ((NUDISPLAYLISTITEM *)buffer->void_ptr)->next = (NUDISPLAYLISTITEM *)list->dynamic_geometry + 1;
    list->material_last = buffer->void_ptr;
    buffer->addr += sizeof(NUDISPLAYLISTITEM);
    ((NUDISPLAYLISTITEM *)list->items)->type = 0xa7;
    ((NUDISPLAYLISTITEM *)list->items)->id = 3;
    ((NUDISPLAYLISTITEM *)list->items)->next = group;
    list->items = (NUDISPLAYLISTITEM *)list->items + 1;
}

void __cdecl BuildDebrisVerts(DEBRIS_PART_HEADER_PC *header, DEBRIS_PARTICLE_CHUNK_PC *chunk, NUMTL *material, f32 time,
                              NUMTX *matrix, i32 particle_type, f32 unused_a, f32 unused_b, f32 unused_c,
                              f32 near_clip) {
    u32 emitted = 0;
    f32 u0, v0, u1, v1;
    i32 i;
    (void)unused_a;
    (void)unused_b;
    (void)unused_c;
    if (material->particle_type_tag == -105) {
        u0 = 0.0f;
        u1 = 1.0f;
        v1 = 1.0f;
        v0 = 0.0f;
    } else {
        u0 = header->texture_u0;
        u1 = header->texture_u1;
        v0 = header->texture_v0;
        v1 = header->texture_v1;
    }
    for (i = 0; i != 32; ++i) {
        DEBRISPARTICLE_PC *particle = &chunk->particles[i];
        f32 age = time - particle->start_time;
        u32 frame_index = (u32)(age * particle->inverse_lifetime);
        if (frame_index < 63) {
            f32 fraction = age * particle->inverse_lifetime - (f32)frame_index;
            f32 distance;
            DEBRIS_PARTICLE_FRAME_PC *first = &header->frames[frame_index];
            NUVEC position, rotated, corners[4];
            NUVEC *corner;
            i32 count;
            u32 colour;
            u32 index;
            position.x = particle->position.x + particle->momentum.x * age;
            position.y =
                (f32)(particle->position.y + particle->momentum.y * age + header->gravity * age * age * (f64)0.945f);
            position.z = particle->position.z + particle->momentum.z * age;
            rotated.x = position.x * matrix->m00 + position.y * matrix->m10 + position.z * matrix->m20;
            rotated.y = position.x * matrix->m01 + position.y * matrix->m11 + position.z * matrix->m21;
            rotated.z = position.x * matrix->m02 + position.y * matrix->m12 + position.z * matrix->m22;
            NuRndr_DebrisMtx.m30 = rotated.x + matrix->m30;
            NuRndr_DebrisMtx.m31 = rotated.y + matrix->m31;
            NuRndr_DebrisMtx.m32 = rotated.z + matrix->m32;
            if (particle_type == 6 || particle_type == 7)
                NuRndrParticleSetRepeat((NUVEC *)&NuRndr_DebrisMtx.m30);
            distance = NuRndr_DebrisPlane.y * NuRndr_DebrisMtx.m31 + NuRndr_DebrisPlane.x * NuRndr_DebrisMtx.m30 +
                       NuRndr_DebrisPlane.z * NuRndr_DebrisMtx.m32 + NuRndr_DebrisPlane.w;
            if (distance < near_clip)
                continue;
            if (NuDebrisCurrentVertexCount + emitted + 6 > NuDebrisMaxVertexCount) {
                VARIPTR *buffer;
                NATIVE_PARTICLE_PACKET_PC *packet;
                NuDebrisCurrentPacket->vertex_count += (i32)emitted;
                NuDebrisFrameVertexCount += emitted;
                emitted = 0;
                if (NuDebrisRendererNextBuffer() == 0)
                    return;
                NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15;
                buffer = NuDisplayListStreamCursor;
                packet = (NATIVE_PARTICLE_PACKET_PC *)buffer->void_ptr;
                packet->material = NuDebrisCurrentPacket->material;
                packet->vertex_buffer_index = (u8)NuDebrisCurrentBufferIndex;
                packet->use_system_memory_vb = NuDebrisUseSystemMemory;
                packet->first_vertex = (i32)NuDebrisCurrentVertexCount;
                packet->vertex_count = 0;
                buffer->addr += sizeof(*packet);
                NuDebrisCurrentPacket = packet;
                AddParticleGroupToDisplayList(packet);
            }
            corners[0].x = first[1].position.x * fraction + first->position.x * (1.0f - fraction);
            corners[0].y = first[1].position.y * fraction + first->position.y * (1.0f - fraction);
            corners[0].z = first[1].position.z * fraction + first->position.z * (1.0f - fraction);
            corners[1].x = first[1].extent.x * fraction + first->extent.x * (1.0f - fraction);
            corners[1].y = first[1].extent.y * fraction + first->extent.y * (1.0f - fraction);
            corners[1].z = first[1].extent.z * fraction + first->extent.z * (1.0f - fraction);
            corners[2].x = first[1].texture_offset.x * fraction + first->texture_offset.x * (1.0f - fraction);
            corners[2].y = first[1].texture_offset.y * fraction + first->texture_offset.y * (1.0f - fraction);
            corners[2].z = first[1].texture_offset.z * fraction + first->texture_offset.z * (1.0f - fraction);
            corners[3].x = (corners[1].x - corners[2].x) + corners[0].x;
            corners[3].y = (corners[1].y - corners[2].y) + corners[0].y;
            corners[3].z = (corners[1].z - corners[2].z) + corners[0].z;
            corner = corners;
            for (count = 4; count != 0; --count) {
                NuVecMtxTransform(corner, corner, &NuRndr_DebrisMtx);
                ++corner;
            }
            colour = first->colour;
            index = NuDebrisCurrentVertexCount + emitted;
            *(NUVEC *)&((NULGTVERTEX_PC *)NuDebrisVertexData)[index].x = corners[0];
            ((NULGTVERTEX_PC *)NuDebrisVertexData)[index].colour = colour;
            ((NULGTVERTEX_PC *)NuDebrisVertexData)[index].u = u0;
            ((NULGTVERTEX_PC *)NuDebrisVertexData)[index].v = v1;
            *(NUVEC *)&((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 1].x = corners[2];
            ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 1].colour = colour;
            ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 1].u = u1;
            ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 1].v = v1;
            *(NUVEC *)&((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 2].x = corners[1];
            ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 2].colour = colour;
            ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 2].u = u1;
            ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 2].v = v0;
            *(NUVEC *)&((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 3].x = corners[0];
            ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 3].colour = colour;
            ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 3].u = u0;
            ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 3].v = v1;
            *(NUVEC *)&((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 4].x = corners[1];
            ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 4].colour = colour;
            ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 4].u = u1;
            ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 4].v = v0;
            *(NUVEC *)&((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 5].x = corners[3];
            ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 5].colour = colour;
            ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 5].u = u0;
            ((NULGTVERTEX_PC *)NuDebrisVertexData)[index + 5].v = v0;
            emitted += 6;
        }
    }
    NuDebrisCurrentPacket->vertex_count += (i32)emitted;
    NuDebrisCurrentVertexCount += emitted;
    NuDebrisFrameVertexCount += emitted;
}

void __cdecl NuRndrParticleGroup(DEBRIS_PARTICLE_CHUNK_PC *chunks, DEBRIS_PART_HEADER_PC *header, NUMTL *material,
                                 f32 time, NUMTX *matrix, i32 particle_type, f32 a, f32 b, f32 c, f32 near_clip) {
    DEBRIS_PARTICLE_CHUNK_PC *chunk;
    i32 done, count;
    if (header == NULL) {
        NuDebrisLastHeader = NULL;
        return;
    }
    if (material == NULL || (material->particle_type_tag == -105 && PCDeviceHardwareVertexProcessingPC < 2))
        return;
    if (header != NuDebrisLastHeader) {
        NUCAMERA camera;
        VARIPTR *buffer;
        NATIVE_PARTICLE_PACKET_PC *packet;
        if (material->attribs.unknown_2_1_2 != 2 || material->attribs.unknown_2_4 == 0) {
            material->attribs.unknown_2_1_2 = 2;
            material->attribs.unknown_2_4 = 1;
            NuMtlUpdate(material);
        }
        if (NuRndr_DebrisRotMtxPtr == NULL)
            NuMtxCalcDebrisFaceOn(&NuRndr_DebrisMtx);
        else
            NuRndr_DebrisMtx = *NuRndr_DebrisRotMtxPtr;
        NuCameraGet(&camera);
        NuRndr_DebrisPlane.x = camera.mtx.m20;
        NuRndr_DebrisPlane.y = camera.mtx.m21;
        NuRndr_DebrisPlane.z = camera.mtx.m22;
        NuRndr_DebrisPlane.w =
            -(camera.mtx.m30 * camera.mtx.m20 + camera.mtx.m31 * camera.mtx.m21 + camera.mtx.m32 * camera.mtx.m22);
        header->last_render_time = time;
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15;
        buffer = NuDisplayListStreamCursor;
        packet = (NATIVE_PARTICLE_PACKET_PC *)buffer->void_ptr;
        buffer->addr = (u32)(packet + 1);
        packet->use_system_memory_vb = NuDebrisUseSystemMemory;
        packet->vertex_buffer_index = (u8)NuDebrisCurrentBufferIndex;
        packet->first_vertex = (i32)NuDebrisCurrentVertexCount;
        packet->material = material;
        packet->vertex_count = 0;
        NuDebrisCurrentPacket = packet;
        if (NuDebrisVertexData == NULL)
            PCDeviceLockVertexBuffer(0, NuDebrisVertexBufferSize, &NuDebrisVertexData, 0x2000,
                                     NuDebrisVertexBuffers[NuDebrisCurrentBufferIndex]);
        AddParticleGroupToDisplayList(NuDebrisCurrentPacket);
        NuDebrisLastHeader = header;
    }
    chunk = chunks;
    done = count = 0;
    while (!done) {
        DEBRIS_PARTICLE_CHUNK_PC *next = chunk->next;
        switch ((i8)chunk->command) {
            case 0x4e:
                if (next != NULL) {
                    BuildDebrisVerts(header, chunk, material, time, matrix, particle_type, a, b, c, near_clip);
                    chunk = next;
                }
                break;
            case 0x52:
                BuildDebrisVerts(header, chunk, material, time, matrix, particle_type, a, b, c, near_clip);
                done = 1;
                break;
        }
        if (++count > 0x100)
            break;
    }
}
/* Append to the current nuparticle_runtime.c. PC platform implementation of
 * the source-facing NuInitDebrisRenderer family; no Android GL operations. */
void __cdecl NuDebrisCreateVertexBuffersPc(void) {
    i32 index;
    NuDebrisCurrentBufferIndex = 0;
    NuDebrisUseSystemMemory = 0;
    NuDebrisCurrentVertexCount = 0;
    NuDebrisLastHeader = NULL;
    for (index = 0; index != 4; ++index)
        PCDeviceCreateVertexBuffer(NuDebrisVertexBufferSize, 0x208, 0, 0, &NuDebrisVertexBuffers[index]);
    PCDeviceLockVertexBuffer(0, NuDebrisVertexBufferSize, &NuDebrisVertexData, 0x2000,
                             NuDebrisVertexBuffers[NuDebrisCurrentBufferIndex]);
}

void __cdecl NuDebrisReleaseVertexBuffersPc(void) {
    PC_BUFFER_RESOURCE_PC **buffer = NuDebrisVertexBuffers;
    do {
        PCDeviceReleaseTexture((PC_TEXTURE_RESOURCE_PC **)buffer);
        ++buffer;
    } while (buffer != NuDebrisVertexBuffers + 4);
}

void __cdecl NuInitDebrisRenderer(VARIPTR *buffer, VARIPTR buffer_end) {
    i32 index;
    (void)buffer_end;
    NuDebrisMaxVertexCount = NuDebrisIdealDynamicVBSizePc / 24u;
    NuDebrisVertexBufferSize = NuDebrisMaxVertexCount * 24u;
    for (index = 0; index != 8; ++index) {
        NuDebrisSystemBuffers[index] = buffer->void_ptr;
        buffer->addr += NuDebrisVertexBufferSize;
    }
    NuDebrisCreateVertexBuffersPc();
}
