#include "saga.h"
STATIC_ASSERT(offsetof(GIZSPINNER_s, position) == 0x5c);
STATIC_ASSERT(offsetof(GIZSPINNER_s, rotation) == 0x7c);
STATIC_ASSERT(offsetof(GIZSPINNER_s, initial_rotation) == 0x82);
STATIC_ASSERT(offsetof(GIZSPINNER_s, state_flags) == 0x84);
STATIC_ASSERT(offsetof(GIZSPINNER_s, type) == 0x8b);
STATIC_ASSERT(offsetof(GIZSPINNER_s, field_0x08c) == 0x8c);
STATIC_ASSERT(offsetof(GIZSPINNER_s, field_0x094) == 0x94);
STATIC_ASSERT(offsetof(GIZSPINNER_s, field_0x098) == 0x98);
extern "C" {
    i32 GizSpinner_GetTargetPoints(GIZSPINNER_s *spinner, NUVEC *positions, NUVEC *directions) {
        if (spinner == NULL || spinner->type == 0)
            return 0;
        u32 flags = spinner->state_flags & 6;
        u16 step = static_cast<u16>(65536 / spinner->type);
        f32 y = spinner->position.y + spinner->field_0x098;
        u16 base = spinner->rotation + spinner->initial_rotation + spinner->field_0x08c;
        u16 position_angle = base - 0x8000;
        u16 direction_angle = base + ((flags == 0 || flags == 6) ? -0x4000 : 0x4000);
        i32 count = 0;
        for (; count < spinner->type; ++count) {
            if (directions != NULL) {
                directions[count].x = NuTrigTable[direction_angle >> 1];
                directions[count].y = 0.0f;
                directions[count].z = NuTrigTable[((direction_angle + 0x4000) >> 1) & 0x7fff];
            }
            if (positions != NULL) {
                positions[count].x = spinner->field_0x094 * NuTrigTable[position_angle >> 1] + spinner->position.x;
                positions[count].y = y;
                positions[count].z =
                    spinner->field_0x094 * NuTrigTable[((position_angle + 0x4000) >> 1) & 0x7fff] + spinner->position.z;
            }
            direction_angle += step;
            position_angle += step;
        }
        return count;
    }

    f32 GizSpinner_GetNearestTargetPoint(GIZSPINNER_s *spinner, NUVEC *origin, NUVEC *position, NUVEC *direction,
                                         i32 check_direction) {
        NUVEC positions[8], directions[8];
        i32 count = GizSpinner_GetTargetPoints(spinner, positions, directions);
        if (count == 0 || origin == NULL)
            return -1.0f;
        f32 nearest_distance = 1000000000.0f;
        NUVEC *nearest_position = NULL;
        NUVEC *nearest_direction = NULL;
        for (i32 i = 0; i < count; ++i) {
            f32 distance = NuVecDistSqr(origin, &positions[i], NULL);
            if (check_direction != 0) {
                u16 angle = spinner->field_0x08c + 0x4000 - spinner->rotation - spinner->initial_rotation +
                            (-65536 / spinner->type) * i;
                NUVEC offset;
                NuVecSub(&offset, origin, &spinner->position);
                NuVecRotateY(&offset, &offset, angle);
                if (!(distance < nearest_distance && offset.z >= 0.0f))
                    continue;
            } else if (!(distance < nearest_distance)) {
                continue;
            }
            nearest_distance = distance;
            nearest_position = &positions[i];
            nearest_direction = &directions[i];
        }
        if (nearest_position == NULL)
            return -1.0f;
        *position = *nearest_position;
        *direction = *nearest_direction;
        return nearest_distance;
    }
}
