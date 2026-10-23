#include "saga.h"

// Retail uses EDI/ESI for the endpoint pointers and six stack arguments.
// Keep the complete helper beside its sole caller for VC8's internal ABI.
extern "C" {
    static __declspec(noinline) void StreakBezierPointPrivate(NUVEC *result, NUVEC *start, NUVEC *end,
                                                              NUVEC *start_tangent, NUVEC *end_tangent,
                                                              i32 negate_start, i32 negate_end, f32 amount) {
        NUVEC negative_start, negative_end, control_a, control_b;
        if (negate_start != 0) {
            NuVecScale(&negative_start, start_tangent, -1.0f);
            start_tangent = &negative_start;
        }
        if (negate_end != 0) {
            NuVecScale(&negative_end, end_tangent, -1.0f);
            end_tangent = &negative_end;
        }
        NuVecAdd(&control_a, start, start_tangent);
        NuVecAdd(&control_b, end, end_tangent);
        f32 inverse = 1.0f - amount;
        f32 start_weight = inverse * inverse * inverse;
        f32 end_weight = amount * amount * amount;
        f64 three_amount = static_cast<f64>(amount) * 3.0;
        f32 control_a_weight = static_cast<f32>(inverse * three_amount * inverse);
        f32 control_b_weight = static_cast<f32>(amount * three_amount * inverse);
        result->x = start->x * start_weight + control_a.x * control_a_weight + control_b.x * control_b_weight +
                    end->x * end_weight;
        result->y = start->y * start_weight + control_a.y * control_a_weight + control_b.y * control_b_weight +
                    end->y * end_weight;
        result->z = start->z * start_weight + control_a.z * control_a_weight + control_b.z * control_b_weight +
                    end->z * end_weight;
    }
}

extern "C" void __cdecl AddStreakPoints(NUVEC *points, f32 duration, u32 colour, void **handle, i32 mode,
                                        GAMEOBJECT *owner) {
    NUVEC incoming, outgoing, edge_a, edge_b;
    STREAK_HEADER_PC *header = static_cast<STREAK_HEADER_PC *>(*handle);
    if (header == NULL) {
        header = StreakHeadersFreePc;
        if (header == NULL)
            return;
        StreakHeadersFreePc = header->next;
        header->next = StreakHeadersUsedPc;
        if (StreakHeadersUsedPc != NULL)
            StreakHeadersUsedPc->prev = header;
        header->prev = NULL;
        StreakHeadersUsedPc = header;
        header->nodes = NULL;
        if (header == NULL)
            return;
        header->owner_slot = handle;
        *handle = header;
        header->colour = colour;
        header->flags = static_cast<u8>(mode);
        header->owner = owner;
    }
    STREAK_NODE_PC *node = StreakNodesFreePc;
    header->has_new_node = 1;
    header->colour = colour;
    if (node != NULL) {
        StreakNodesFreePc = node->next;
        node->next = header->nodes;
        if (header->nodes != NULL)
            header->nodes->prev = node;
        node->prev = NULL;
        header->nodes = node;
    }
    node = header->nodes;
    if (node == NULL)
        return;
    node->position = points[0];
    node->previous_position = points[1];
    node->remaining_time = duration;
    if (node->next == NULL)
        return;
    STREAK_NODE_PC *older = node->next;
    NuVecSub(&node->start_tangent, &older->position, &node->position);
    NuVecSub(&header->nodes->end_tangent, &header->nodes->next->previous_position, &header->nodes->previous_position);
    NuVecScale(&header->nodes->start_tangent, &header->nodes->start_tangent, 0.3333333432674408f);
    NuVecScale(&header->nodes->end_tangent, &header->nodes->end_tangent, 0.3333333432674408f);
    node = header->nodes;
    older = node->next;
    if (older->next != NULL) {
        NuVecSub(&incoming, &older->position, &node->position);
        NuVecSub(&outgoing, &older->next->position, &older->position);
        NuVecAddScale(&older->start_tangent, &incoming, &outgoing, 0.5f);
        NuVecSub(&incoming, &older->previous_position, &node->previous_position);
        NuVecSub(&outgoing, &older->next->previous_position, &older->previous_position);
        NuVecAddScale(&older->end_tangent, &incoming, &outgoing, 0.5f);
        NuVecScale(&header->nodes->next->start_tangent, &header->nodes->next->start_tangent, 0.3333333432674408f);
        NuVecScale(&header->nodes->next->end_tangent, &header->nodes->next->end_tangent, 0.3333333432674408f);
        older = header->nodes->next;
        if (older->next->segment_count > 1) {
            STREAK_NODE_PC *newer = header->nodes->next;
            STREAK_NODE_PC *segment = newer->next;
            for (i32 index = 1; index < segment->segment_count; ++index) {
                f32 amount = (1.0f / static_cast<f32>(segment->segment_count)) * static_cast<f32>(index);
                StreakBezierPointPrivate(&segment->positions[index - 1], &newer->position, &segment->position,
                                         &newer->start_tangent, &segment->start_tangent, 0, 1, amount);
                newer = header->nodes->next;
                segment = newer->next;
                amount = (1.0f / static_cast<f32>(segment->segment_count)) * static_cast<f32>(index);
                StreakBezierPointPrivate(&segment->tangents[index - 1], &newer->previous_position,
                                         &segment->previous_position, &newer->end_tangent, &segment->end_tangent, 0, 1,
                                         amount);
                newer = header->nodes->next;
                segment = newer->next;
            }
        }
    } else {
        NuVecSub(&older->start_tangent, &older->position, &node->position);
        NuVecSub(&older->end_tangent, &older->previous_position, &node->previous_position);
        NuVecScale(&header->nodes->next->start_tangent, &header->nodes->next->start_tangent, 0.3333333432674408f);
        NuVecScale(&header->nodes->next->end_tangent, &header->nodes->next->end_tangent, 0.3333333432674408f);
    }
    node = header->nodes;
    NuVecSub(&edge_a, &node->previous_position, &node->position);
    NuVecSub(&edge_b, &header->nodes->next->previous_position, &header->nodes->next->position);
    NuVecNorm(&edge_a, &edge_a);
    NuVecNorm(&edge_b, &edge_b);
    f64 count = static_cast<f64>(FRAMETIME) * 60.0;
    count += count;
    header->nodes->next->segment_count = static_cast<i32>(8.0 > count ? count : 8.0);
    if (header->nodes->next->segment_count > 1) {
        STREAK_NODE_PC *newer = header->nodes;
        STREAK_NODE_PC *segment = newer->next;
        for (i32 index = 1; index < segment->segment_count; ++index) {
            f32 amount = (1.0f / static_cast<f32>(segment->segment_count)) * static_cast<f32>(index);
            StreakBezierPointPrivate(&segment->positions[index - 1], &newer->position, &segment->position,
                                     &newer->start_tangent, &segment->start_tangent, 0, 1, amount);
            newer = header->nodes;
            segment = newer->next;
            amount = (1.0f / static_cast<f32>(segment->segment_count)) * static_cast<f32>(index);
            StreakBezierPointPrivate(&segment->tangents[index - 1], &newer->previous_position,
                                     &segment->previous_position, &newer->end_tangent, &segment->end_tangent, 0, 1,
                                     amount);
            newer = header->nodes;
            segment = newer->next;
        }
    }
}

extern "C" void __cdecl UpdateStreaks(f32 elapsed) {
    if (elapsed == 0.0f)
        return;
    STREAK_HEADER_PC *header = StreakHeadersUsedPc;
    STREAK_HEADER_PC *free_headers = StreakHeadersFreePc;
    while (header != NULL) {
        STREAK_HEADER_PC *next_header = header->next;
        if (header->has_new_node == 0 && header->owner_slot != NULL && *header->owner_slot == header) {
            *header->owner_slot = NULL;
            header->owner_slot = NULL;
        }
        header->has_new_node = 0;
        STREAK_NODE_PC *node = header->nodes;
        while (node != NULL) {
            STREAK_NODE_PC *next_node = node->next;
            node->remaining_time -= elapsed;
            if (node->remaining_time <= 0.0f && next_node == NULL) {
                if (node->prev != NULL)
                    node->prev->next = node->next;
                else
                    header->nodes = node->next;
                if (node->next != NULL)
                    node->next->prev = node->prev;
                node->next = StreakNodesFreePc;
                StreakNodesFreePc = node;
            }
            node = next_node;
        }
        if (header->nodes == NULL) {
            if (header->prev != NULL)
                header->prev->next = header->next;
            else
                StreakHeadersUsedPc = header->next;
            if (header->next != NULL)
                header->next->prev = header->prev;
            header->next = free_headers;
            free_headers = header;
        }
        header = next_header;
    }
    StreakHeadersFreePc = free_headers;
}

extern "C" f32 StreakAlphaScalePc;
#define STREAK_FADE(node) ((node)->remaining_time < 0.0f ? 0.0f : (node)->remaining_time)
#define STREAK_ALPHA(colour, node) ((f64)(i32)((colour) >> 24) * STREAK_FADE(node) * StreakAlphaScalePc)
#define STREAK_SET_ALPHA(colour, header, node)                                                                         \
    (colour) = (header)->colour;                                                                                       \
    ((u8 *)&(colour))[3] = (u8)(STREAK_ALPHA(colour, node) > 255.0 ? 255.0 : STREAK_ALPHA(colour, node))
extern "C" void __cdecl DrawStreaks(void) {
    for (STREAK_HEADER_PC *header = StreakHeadersUsedPc; header != NULL; header = header->next) {
        NURND_VERTEX3D_PC vertices[256];
        i32 vertex_count = 0;
        u32 colour;
        for (STREAK_NODE_PC *node = header->nodes; node != NULL; node = node->next) {
            STREAK_SET_ALPHA(colour, header, node);
            vertices[vertex_count].position = node->position;
            vertices[vertex_count].u = 0.01f;
            vertices[vertex_count].colour = colour;
            vertices[vertex_count + 1].position = node->previous_position;
            vertices[vertex_count + 1].u = 0.99f;
            vertices[vertex_count + 1].colour = colour;
            vertex_count += 2;
            if (node->next != NULL) {
                for (i32 index = 1; index < node->next->segment_count; ++index) {
                    STREAK_SET_ALPHA(colour, header, node);
                    vertices[vertex_count].position = node->next->positions[index - 1];
                    vertices[vertex_count].colour = colour;
                    vertices[vertex_count].u = 0.01f;
                    ++vertex_count;
                    STREAK_SET_ALPHA(colour, header, node);
                    vertices[vertex_count].position = node->next->tangents[index - 1];
                    vertices[vertex_count].u = 1.01f;
                    vertices[vertex_count].colour = colour;
                    ++vertex_count;
                }
            }
        }
        for (i32 index = 0; index < vertex_count; index += 2) {
            f32 v = (f32)((f64)index / (f32)vertex_count * (f64)0.99f);
            vertices[index].v = v;
            vertices[index + 1].v = v;
        }
        NuRndrTriStrip3dClip(vertices, vertex_count, NULL,
                             header->flags != 0 ? StreakReflectionMaterialPc : StreakMaterialPc);
    }
}
#undef STREAK_SET_ALPHA
#undef STREAK_ALPHA
#undef STREAK_FADE

extern "C" void __cdecl ResetStreaks(void) {
    STREAK_HEADER_PC *header = StreakHeadersUsedPc;
    if (header == NULL)
        return;
    STREAK_HEADER_PC *free_headers = StreakHeadersFreePc;
    STREAK_NODE_PC *free_nodes = StreakNodesFreePc;
    do {
        while (header->nodes != NULL) {
            STREAK_NODE_PC *node = header->nodes;
            if (node->prev != NULL)
                node->prev->next = node->next;
            else
                header->nodes = node->next;
            if (node->next != NULL)
                node->next->prev = node->prev;
            node->next = free_nodes;
            free_nodes = node;
        }
        STREAK_HEADER_PC *old_header = header;
        if (old_header->prev != NULL)
            old_header->prev->next = old_header->next;
        else
            header = old_header->next;
        if (old_header->next != NULL)
            old_header->next->prev = old_header->prev;
        old_header->next = free_headers;
        free_headers = old_header;
    } while (header != NULL);
    StreakNodesFreePc = free_nodes;
    StreakHeadersFreePc = free_headers;
    StreakHeadersUsedPc = NULL;
}

struct STREAK_MATERIAL_SCALE_PC {
    u8 unknown_00[0x64];
    f32 effect_scale;
};
STATIC_ASSERT(offsetof(STREAK_MATERIAL_SCALE_PC, effect_scale) == 0x64);
STATIC_ASSERT(offsetof(PC_SCENE_MTL_VIEW, sort_pri) == 0x76);

extern "C" void __cdecl InitStreaks(VARIPTR *buffer, VARIPTR end, char *name) {
    for (i32 i = 0; i < 32; ++i) {
        StreakHeadersPc[i].index = static_cast<u16>(i);
        StreakHeadersPc[i].next = &StreakHeadersPc[i + 1];
        StreakHeadersPc[i].prev = reinterpret_cast<STREAK_HEADER_PC *>(reinterpret_cast<u32>(StreakHeadersPc) +
                                                                       (i - 1) * sizeof(STREAK_HEADER_PC));
    }
    StreakHeadersFreePc = StreakHeadersPc;
    StreakHeadersPc[31].next = NULL;
    StreakHeadersPc[0].prev = NULL;
    StreakHeadersUsedPc = NULL;
    for (i32 i = 0; i < 128; ++i) {
        StreakNodesPc[i].next = &StreakNodesPc[i + 1];
        StreakNodesPc[i].prev =
            reinterpret_cast<STREAK_NODE_PC *>(reinterpret_cast<u32>(StreakNodesPc) + (i - 1) * sizeof(STREAK_NODE_PC));
    }
    StreakNodesFreePc = StreakNodesPc;
    StreakNodesPc[127].next = NULL;
    StreakNodesPc[0].prev = NULL;
    StreakNodesUsedPc = NULL;
    StreakMaterialPc = NuMtlCreate3D(1);
    StreakMaterialPc->attribs.unknown_1_1_2 = 0;
    StreakMaterialPc->attribs.unknown_1_4_8 = 0;
    StreakMaterialPc->opacity = 1.0f;
    StreakMaterialPc->attribs.z_mode = 1;
    StreakMaterialPc->attribs.alpha_mode = 2;
    StreakMaterialPc->attribs.filter_mode = 1;
    reinterpret_cast<PC_SCENE_MTL_VIEW *>(StreakMaterialPc)->sort_pri = 500;
    StreakMaterialPc->attribs.unknown_2_1_2 = 2;
    StreakMaterialPc->attribs.unknown_2_4 = 1;
    buffer->addr = (buffer->addr + 15) & ~15u;
    StreakMaterialPc->particle_type_tag = -112;
    reinterpret_cast<STREAK_MATERIAL_SCALE_PC *>(StreakMaterialPc)->effect_scale = 5.0f;
    StreakMaterialPc->texture_id = static_cast<i16>(NuTexReadPc(name, buffer, end));
    NuMtlUpdate(StreakMaterialPc);
    StreakReflectionMaterialPc = NuMtlCreate3D(1);
    StreakReflectionMaterialPc->attribs.unknown_1_1_2 = 0;
    StreakReflectionMaterialPc->attribs.unknown_1_4_8 = 0;
    StreakReflectionMaterialPc->opacity = 1.0f;
    StreakReflectionMaterialPc->attribs.z_mode = 0;
    StreakReflectionMaterialPc->attribs.alpha_mode = 2;
    StreakReflectionMaterialPc->attribs.filter_mode = 1;
    StreakReflectionMaterialPc->attribs.unknown_2_1_2 = 2;
    StreakReflectionMaterialPc->attribs.unknown_2_4 = 1;
    StreakReflectionMaterialPc->particle_type_tag = -112;
    reinterpret_cast<STREAK_MATERIAL_SCALE_PC *>(StreakReflectionMaterialPc)->effect_scale = 5.0f;
    reinterpret_cast<PC_SCENE_MTL_VIEW *>(StreakReflectionMaterialPc)->sort_pri = 1;
    StreakReflectionMaterialPc->texture_id = StreakMaterialPc->texture_id;
    NuMtlUpdate(StreakReflectionMaterialPc);
}
