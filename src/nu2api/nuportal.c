#include "saga.h"
#include <math.h>

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

i32 __cdecl clipTestSphere(NUPORTALSPHERE_PC *sphere, NUPORTAL_FRUSTUM_PC *frustum) {
    i32 fully_inside = 0;
    i32 i;
    f32 camera_distance;
    for (i = 0; i < frustum->plane_count; ++i) {
        NUPLANE *plane = &frustum->planes[i];
        f32 distance =
            plane->a * sphere->center.x + plane->b * sphere->center.y + plane->c * sphere->center.z + plane->d;
        if (distance < -sphere->radius)
            return 0;
        if (distance > sphere->radius)
            ++fully_inside;
    }
    camera_distance = NuPortalCameraPlane.a * sphere->center.x + NuPortalCameraPlane.b * sphere->center.y +
                      NuPortalCameraPlane.c * sphere->center.z + NuPortalCameraPlane.d;
    if (camera_distance < -sphere->radius)
        return 0;
    if (camera_distance > sphere->radius)
        ++fully_inside;
    return fully_inside == frustum->plane_count + 1 ? 1 : 2;
}

i32 __cdecl NuPortalClipTest(NUGSCN *scene, NUVEC *position, f32 radius, i16 room_id) {
    i32 i;
    if (scene->max_portals == 0 || scene->camera_room == room_id)
        return 1;
    for (i = 0; i < scene->num_portal_frusta; ++i) {
        NUPORTAL_FRUSTUM_PC *frustum = scene->portal_frusta[i];
        if (frustum != NULL && frustum->room_id == room_id) {
            NUPORTALSPHERE_PC sphere;
            i32 result;
            sphere.center = *position;
            sphere.radius = radius;
            result = clipTestSphere(&sphere, frustum);
            if (result != 0)
                return result;
        }
    }
    return 0;
}

i32 __cdecl clipTestBox(NUVEC *minimum, NUVEC *maximum, NUPLANE *planes, i32 plane_count) {
    NUVEC min = *minimum;
    NUVEC max = *maximum;
    i32 inside_vertices = 0;
    i32 plane_index;
    for (plane_index = 0; plane_index < plane_count; ++plane_index) {
        const NUPLANE *plane = &planes[plane_index];
        i32 inside_plane = 0;
        f32 distance1, distance2, distance3, distance4;
        f32 distance5, distance6, distance7, distance8;
        distance1 = plane->a * min.x + plane->b * min.y + plane->c * min.z + plane->d;
        if (!(distance1 < 0.0f))
            ++inside_plane, ++inside_vertices;
        distance2 = plane->a * max.x + plane->b * min.y + plane->c * min.z + plane->d;
        if (!(distance2 < 0.0f))
            ++inside_plane, ++inside_vertices;
        distance3 = plane->a * max.x + plane->b * max.y + plane->c * min.z + plane->d;
        if (!(distance3 < 0.0f))
            ++inside_plane, ++inside_vertices;
        distance4 = plane->a * min.x + plane->b * max.y + plane->c * min.z + plane->d;
        if (!(distance4 < 0.0f))
            ++inside_plane, ++inside_vertices;
        distance5 = plane->a * min.x + plane->b * min.y + plane->c * max.z + plane->d;
        if (!(distance5 < 0.0f))
            ++inside_plane, ++inside_vertices;
        distance6 = plane->a * max.x + plane->b * min.y + plane->c * max.z + plane->d;
        if (!(distance6 < 0.0f))
            ++inside_plane, ++inside_vertices;
        distance7 = plane->a * max.x + plane->b * max.y + plane->c * max.z + plane->d;
        if (!(distance7 < 0.0f))
            ++inside_plane, ++inside_vertices;
        distance8 = plane->a * min.x + plane->b * max.y + plane->c * max.z + plane->d;
        if (!(distance8 < 0.0f))
            ++inside_plane, ++inside_vertices;
        if (inside_plane == 0) {
            return 0;
        }
    }
    return inside_vertices == plane_count * 8 ? 1 : 2;
}

i32 __cdecl NuPortalClipTestBox(NUVEC *center, NUVEC *extent, NUPORTAL_FRUSTUM_PC *frustum) {
    i32 i;
    for (i = 0; i < frustum->plane_count; ++i) {
        const NUPLANE *plane = &frustum->planes[i];
        const f32 distance = plane->a * center->x + plane->b * center->y + plane->c * center->z + plane->d;
        const f32 radius = fabsf(plane->a) * extent->x + fabsf(plane->b) * extent->y + fabsf(plane->c) * extent->z;
        if (distance < -radius) {
            return 0;
        }
    }
    return 1;
}
