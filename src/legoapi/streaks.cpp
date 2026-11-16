#include "gameapi/edtools/edrender.h"
#include "legoapi/actions/character/streaks.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/misc/gamemain.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/numath/nuvec.h"
extern "C" {
    STREAK_NODE StreakNodes[128] = {0};
    NUMTL *streakmtl_ref = 0;
    STREAK_NODE *StreakNodesUsed = 0;
    STREAK_NODE *streaks_free = 0;
    STREAK_HEADER *streakhdrs_free = 0;
}
static STREAK_HEADER *streakhdrs_used = 0;
extern "C" {
    NUMTL *streakmtl = 0;
    STREAK_HEADER StreakHeaders[32] = {0};
}

extern "C" {
    f32 StreakAlphaScale = 2.0f;
}

/* Moves a header from the free list to the front of the used list. */
extern "C" STREAK_HEADER *fun_005a35c0(void) {
    STREAK_HEADER *header = streakhdrs_free;
    if (header != NULL) {
        streakhdrs_free = header->next;
        header->next = streakhdrs_used;
        if (streakhdrs_used != NULL) {
            streakhdrs_used->prev = header;
        }
        header->prev = NULL;
        streakhdrs_used = header;
        header->nodes = NULL;
    }
    return header;
}

// Takes the endpoint pointers in EDI/ESI and six stack arguments; the helper
// stays beside its sole caller for VC8's internal ABI.
extern "C" {
    static void StreakBezierPointPrivate(NUVEC *result, NUVEC *start, NUVEC *end, NUVEC *start_tangent, NUVEC *end_tangent,
                                         i32 negate_start, i32 negate_end, f32 amount) {
        NUVEC negative_start;
        NUVEC negative_end;
        NUVEC control_a;
        NUVEC control_b;
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
        result->x = start->x * start_weight + control_a.x * control_a_weight + control_b.x * control_b_weight + end->x * end_weight;
        result->y = start->y * start_weight + control_a.y * control_a_weight + control_b.y * control_b_weight + end->y * end_weight;
        result->z = start->z * start_weight + control_a.z * control_a_weight + control_b.z * control_b_weight + end->z * end_weight;
    }
}

extern "C" void AddStreakPoints(NUVEC *points, f32 duration, u32 colour, void **handle, i32 mode, GAMEOBJECT *owner) {
    NUVEC incoming;
    NUVEC outgoing;
    NUVEC edge_a;
    NUVEC edge_b;
    STREAK_HEADER *header = static_cast<STREAK_HEADER *>(*handle);
    if (header == NULL) {
        header = streakhdrs_free;
        if (header == NULL) {
            return;
        }
        streakhdrs_free = header->next;
        header->next = streakhdrs_used;
        if (streakhdrs_used != NULL) {
            streakhdrs_used->prev = header;
        }
        header->prev = NULL;
        streakhdrs_used = header;
        header->nodes = NULL;
        if (header == NULL) {
            return;
        }
        header->owner_slot = handle;
        *handle = header;
        header->colour = colour;
        header->flags = static_cast<u8>(mode);
        header->owner = owner;
    }
    STREAK_NODE *node = streaks_free;
    header->has_new_node = 1;
    header->colour = colour;
    if (node != NULL) {
        streaks_free = node->next;
        node->next = header->nodes;
        if (header->nodes != NULL) {
            header->nodes->prev = node;
        }
        node->prev = NULL;
        header->nodes = node;
    }
    if (header->nodes == NULL) {
        return;
    }
    header->nodes->position = points[0];
    header->nodes->previous_position = points[1];
    header->nodes->remaining_time = duration;
    if (header->nodes->next == NULL) {
        return;
    }
    node = header->nodes;
    STREAK_NODE *older = node->next;
    NuVecSub(&node->start_tangent, &older->position, &node->position);
    NuVecSub(&header->nodes->end_tangent, &header->nodes->next->previous_position, &header->nodes->previous_position);
    NuVecScale(&header->nodes->start_tangent, &header->nodes->start_tangent, 0.33333334f);
    NuVecScale(&header->nodes->end_tangent, &header->nodes->end_tangent, 0.33333334f);
    node = header->nodes;
    older = node->next;
    STREAK_NODE *oldest = older->next;
    if (oldest != NULL) {
        NuVecSub(&incoming, &older->position, &node->position);
        NuVecSub(&outgoing, &oldest->position, &older->position);
        NuVecAddScale(&header->nodes->next->start_tangent, &incoming, &outgoing, 0.5f);
        node = header->nodes;
        older = node->next;
        NuVecSub(&incoming, &older->previous_position, &node->previous_position);
        NuVecSub(&outgoing, &older->next->previous_position, &older->previous_position);
        NuVecAddScale(&header->nodes->next->end_tangent, &incoming, &outgoing, 0.5f);
        NuVecScale(&header->nodes->next->start_tangent, &header->nodes->next->start_tangent, 0.33333334f);
        NuVecScale(&header->nodes->next->end_tangent, &header->nodes->next->end_tangent, 0.33333334f);
        for (i32 index = 1; index < header->nodes->next->next->segment_count; ++index) {
            STREAK_NODE *newer = header->nodes->next;
            STREAK_NODE *segment = newer->next;
            StreakBezierPointPrivate(&segment->positions[index - 1], &newer->position, &segment->position, &newer->start_tangent,
                                     &segment->start_tangent, 0, 1,
                                     (1.0f / static_cast<f32>(segment->segment_count)) * static_cast<f32>(index));
            newer = header->nodes->next;
            segment = newer->next;
            StreakBezierPointPrivate(&segment->tangents[index - 1], &newer->previous_position, &segment->previous_position,
                                     &newer->end_tangent, &segment->end_tangent, 0, 1,
                                     (1.0f / static_cast<f32>(segment->segment_count)) * static_cast<f32>(index));
        }
    } else {
        NuVecSub(&older->start_tangent, &older->position, &node->position);
        NuVecSub(&older->end_tangent, &older->previous_position, &node->previous_position);
        NuVecScale(&header->nodes->next->start_tangent, &header->nodes->next->start_tangent, 0.33333334f);
        NuVecScale(&header->nodes->next->end_tangent, &header->nodes->next->end_tangent, 0.33333334f);
    }
    node = header->nodes;
    NuVecSub(&edge_a, &node->previous_position, &node->position);
    NuVecSub(&edge_b, &header->nodes->next->previous_position, &header->nodes->next->position);
    NuVecNorm(&edge_a, &edge_a);
    NuVecNorm(&edge_b, &edge_b);
    f64 count = static_cast<f64>(FRAMETIME) * 60.0;
    count += count;
    header->nodes->next->segment_count = static_cast<i32>(8.0 > count ? count : 8.0);
    for (i32 index = 1; index < header->nodes->next->segment_count; ++index) {
        STREAK_NODE *newer = header->nodes;
        STREAK_NODE *segment = newer->next;
        StreakBezierPointPrivate(&segment->positions[index - 1], &newer->position, &segment->position, &newer->start_tangent,
                                 &segment->start_tangent, 0, 1,
                                 (1.0f / static_cast<f32>(segment->segment_count)) * static_cast<f32>(index));
        newer = header->nodes;
        segment = newer->next;
        StreakBezierPointPrivate(&segment->tangents[index - 1], &newer->previous_position, &segment->previous_position, &newer->end_tangent,
                                 &segment->end_tangent, 0, 1, (1.0f / static_cast<f32>(segment->segment_count)) * static_cast<f32>(index));
    }
}

extern "C" void UpdateStreaks(f32 elapsed) {
    if (elapsed == 0.0f) {
        return;
    }
    STREAK_HEADER *header = streakhdrs_used;
    if (header == NULL) {
        return;
    }
    STREAK_HEADER *free_headers = streakhdrs_free;
    do {
        STREAK_HEADER *next_header = header->next;
        if (header->has_new_node == 0 && header->owner_slot != NULL && *header->owner_slot == header) {
            *header->owner_slot = NULL;
            header->owner_slot = NULL;
        }
        header->has_new_node = 0;
        for (STREAK_NODE *node = header->nodes; node != NULL; node = node->next) {
            node->remaining_time -= elapsed;
            if (node->remaining_time <= 0.0f && node->next == NULL) {
                if (node->prev != NULL) {
                    node->prev->next = node->next;
                } else {
                    header->nodes = node->next;
                }
                if (node->next != NULL) {
                    node->next->prev = node->prev;
                }
                node->next = streaks_free;
                streaks_free = node;
                break;
            }
        }
        if (header->nodes == NULL) {
            if (header->prev != NULL) {
                header->prev->next = header->next;
            } else {
                streakhdrs_used = header->next;
            }
            if (header->next != NULL) {
                header->next->prev = header->prev;
            }
            header->next = free_headers;
            free_headers = header;
        }
        header = next_header;
    } while (header != NULL);
    streakhdrs_free = free_headers;
}

extern "C" f32 StreakAlphaScale;
#define STREAK_SET_ALPHA(colour, header, node)                                                                                             \
    (colour) = (header)->colour;                                                                                                           \
    ((u8 *)&(colour))[3] = (u8)(((f64)(i32)(colour >> 24) * MAX(0.0f, node->remaining_time) * StreakAlphaScale) > 255.0                    \
                                    ? 255.0                                                                                                \
                                    : ((f64)(i32)(colour >> 24) * MAX(0.0f, node->remaining_time) * StreakAlphaScale))

extern "C" void DrawStreaks(void) {
    for (STREAK_HEADER *header = streakhdrs_used; header != NULL; header = header->next) {
        NURND_VERTEX3D vertices[256];
        i32 vertex_count = 0;
        u32 colour;
        for (STREAK_NODE *node = header->nodes; node != NULL; node = node->next) {
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
        NuRndrTriStrip3dClip(vertices, vertex_count, NULL, header->flags != 0 ? streakmtl_ref : streakmtl);
    }
}

#undef STREAK_SET_ALPHA

extern "C" void ResetStreaks(void) {
    STREAK_HEADER *header = streakhdrs_used;
    if (header == NULL) {
        return;
    }
    STREAK_HEADER *free_headers = streakhdrs_free;
    STREAK_NODE *free_nodes = streaks_free;
    do {
        while (header->nodes != NULL) {
            STREAK_NODE *node = header->nodes;
            if (node->prev != NULL) {
                node->prev->next = node->next;
            } else {
                header->nodes = node->next;
            }
            if (node->next != NULL) {
                node->next->prev = node->prev;
            }
            node->next = free_nodes;
            free_nodes = node;
        }
        STREAK_HEADER *old_header = header;
        if (old_header->prev != NULL) {
            old_header->prev->next = old_header->next;
        } else {
            header = old_header->next;
        }
        if (old_header->next != NULL) {
            old_header->next->prev = old_header->prev;
        }
        old_header->next = free_headers;
        free_headers = old_header;
    } while (header != NULL);
    streaks_free = free_nodes;
    streakhdrs_free = free_headers;
    streakhdrs_used = NULL;
}

extern "C" void InitStreaks(VARIPTR *buffer, VARIPTR end, char *name) {
    i32 i;
    for (i = 0; i < 32; ++i) {
        StreakHeaders[i].index = static_cast<u16>(i);
        StreakHeaders[i].next = &StreakHeaders[i + 1];
        StreakHeaders[i].prev = reinterpret_cast<STREAK_HEADER *>(reinterpret_cast<u32>(StreakHeaders) + (i - 1) * sizeof(STREAK_HEADER));
    }
    StreakHeaders[i - 1].next = NULL;
    StreakHeaders[0].prev = NULL;
    streakhdrs_free = StreakHeaders;
    streakhdrs_used = NULL;
    for (i = 0; i < 128; ++i) {
        StreakNodes[i].next = &StreakNodes[i + 1];
        StreakNodes[i].prev = reinterpret_cast<STREAK_NODE *>(reinterpret_cast<u32>(StreakNodes) + (i - 1) * sizeof(STREAK_NODE));
    }
    StreakNodes[i - 1].next = NULL;
    StreakNodes[0].prev = NULL;
    streaks_free = StreakNodes;
    StreakNodesUsed = NULL;
    streakmtl = NuMtlCreate3D(1);
    streakmtl->attribs.u_address = NUMTL_ADDRESS_WRAP;
    streakmtl->attribs.v_address = NUMTL_ADDRESS_WRAP;
    streakmtl->opacity = 1.0f;
    streakmtl->attribs.z_mode = NUMTL_ZMODE_READ_ONLY;
    streakmtl->attribs.alpha_mode = NUMTL_ALPHA_ADDITIVE;
    streakmtl->attribs.filter_mode = 1;
    streakmtl->sort_priority = 500;
    streakmtl->attribs.lighting = NUMTL_LIGHTING_NONE;
    streakmtl->attribs.vertex_colour = 1;
    buffer->addr = (buffer->addr + 15) & ~15u;
    streakmtl->particle_type_tag = -112;
    streakmtl->effect_scale = 5.0f;
    streakmtl->texture_id = static_cast<i16>(NuTexRead(name, buffer, end));
    NuMtlUpdate(streakmtl);
    streakmtl_ref = NuMtlCreate3D(1);
    streakmtl_ref->attribs.u_address = NUMTL_ADDRESS_WRAP;
    streakmtl_ref->attribs.v_address = NUMTL_ADDRESS_WRAP;
    streakmtl_ref->opacity = 1.0f;
    streakmtl_ref->attribs.z_mode = NUMTL_ZMODE_NORMAL;
    streakmtl_ref->attribs.alpha_mode = NUMTL_ALPHA_ADDITIVE;
    streakmtl_ref->attribs.filter_mode = 1;
    streakmtl_ref->attribs.lighting = NUMTL_LIGHTING_NONE;
    streakmtl_ref->attribs.vertex_colour = 1;
    streakmtl_ref->particle_type_tag = -112;
    streakmtl_ref->effect_scale = 5.0f;
    streakmtl_ref->sort_priority = 1;
    streakmtl_ref->texture_id = streakmtl->texture_id;
    NuMtlUpdate(streakmtl_ref);
}
