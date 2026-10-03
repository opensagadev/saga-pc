#include "saga.h"

#define ALIGN_UP_16(address) (((address) + 15u) & ~15u)

void NuPortalInit(void) {
    fstack.void_ptr = fstackmem;
    fstack_top.addr = ALIGN_UP_16((u32)fstackmem);
    fstack_limit.void_ptr = fstackmem + NU_PORTAL_STACK_SIZE;
}

i16 __cdecl NuPortalWhichRoom(NUGSCN *scene, NUVEC *position) {
    i16 candidates[4];
    i16 candidate_count = 0;
    i32 room_index;

    if (scene == NULL || position == NULL || scene->num_rooms <= 0) {
        return -1;
    }

    for (room_index = 0; room_index < scene->num_rooms; ++room_index) {
        NUGSCNROOM *room = &scene->rooms[room_index];
        i32 plane_index;
        i32 inside = 1;

        for (plane_index = 0; plane_index < room->plane_count; ++plane_index) {
            NUPLANE *plane = &room->planes[plane_index];
            f32 distance = plane->a * position->x + plane->b * position->y + plane->c * position->z + plane->d;
            if (distance > 0.0f) {
                inside = 0;
                break;
            }
        }
        if (inside == 0) {
            continue;
        }

        if (candidate_count == 2) {
            NUGSCNROOM *first = &scene->rooms[candidates[0]];
            NUGSCNROOM *second = &scene->rooms[candidates[1]];
            if (first->priority < second->priority) {
                candidates[0] = (i16)room_index;
            } else {
                candidates[1] = (i16)room_index;
            }
        } else if (candidate_count < 4) {
            candidates[candidate_count] = (i16)room_index;
        }
        ++candidate_count;

        if (candidate_count == 2) {
            NUGSCNROOM *first = &scene->rooms[candidates[0]];
            NUGSCNROOM *second = &scene->rooms[candidates[1]];
            if (((first->visibility_flags | second->visibility_flags) & 1) == 0) {
                break;
            }
        }
    }

    if (candidate_count == 1) {
        return candidates[0];
    }
    if (candidate_count == 0) {
        return -1;
    }

    {
        NUGSCNROOM *first = &scene->rooms[candidates[0]];
        NUGSCNROOM *second = &scene->rooms[candidates[1]];
        i32 first_portal;

        for (first_portal = 0; first_portal < first->portal_count; ++first_portal) {
            i16 portal_index = first->portal_indices[first_portal];
            i32 second_portal;

            for (second_portal = 0; second_portal < second->portal_count; ++second_portal) {
                if (second->portal_indices[second_portal] == portal_index) {
                    NUPORTAL *portal = &scene->portals[portal_index];
                    f32 side = portal->plane.a * position->x + portal->plane.b * position->y +
                               portal->plane.c * position->z + portal->plane.d;
                    return side < 0.0f ? (i16)portal->front_room : (i16)portal->back_room;
                }
            }
        }
    }
    return -1;
}

void __cdecl NuPortalMaxDepth(NUGSCN *scene, i32 depth) {
    scene->portal_depth = depth;
}
