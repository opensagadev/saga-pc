#include "saga.h"

bool PCFrustumClipSet::TestBox(const NUVEC *minimum, const NUVEC *maximum, i32 group_index) {
    PC_FRUSTUM_GROUP_PC *group = &first_group + group_index;
    __declspec(align(16)) NUVEC4 corners[8];
    corners[0].x = minimum->x;
    corners[0].y = minimum->y;
    corners[0].z = minimum->z;
    corners[1].x = minimum->x;
    corners[1].y = maximum->y;
    corners[1].z = minimum->z;
    corners[2].x = maximum->x;
    corners[2].y = maximum->y;
    corners[2].z = minimum->z;
    corners[3].x = maximum->x;
    corners[3].y = minimum->y;
    corners[3].z = minimum->z;
    corners[4].x = minimum->x;
    corners[4].y = minimum->y;
    corners[4].z = maximum->z;
    corners[5].x = minimum->x;
    corners[5].y = maximum->y;
    corners[5].z = maximum->z;
    corners[6].x = maximum->x;
    corners[6].y = maximum->y;
    corners[6].z = maximum->z;
    corners[7].x = maximum->x;
    corners[7].y = minimum->y;
    corners[7].z = maximum->z;
    for (i32 plane_index = 0; plane_index < group->plane_count; ++plane_index) {
        const NUPLANE *plane = &group->planes[plane_index];
        i32 corner_index;
        for (corner_index = 0; corner_index < 8; ++corner_index) {
            __declspec(align(16)) NUVEC4 point = corners[corner_index];
            f32 distance = plane->a * point.x + point.y * plane->b + plane->c * point.z;
            distance += plane->d;
            if (distance > 0.0f)
                break;
        }
        if (corner_index == 8)
            return false;
    }
    return true;
}

i32 PCFrustumClipSet::TestBoxes(const NUVEC *minimum, const NUVEC *maximum) {
    i32 result = 0;
    for (i32 group = 0; group < active_render_set_count; ++group) {
        if (TestBox(minimum, maximum, group))
            result |= 1 << group;
    }
    return result;
}
