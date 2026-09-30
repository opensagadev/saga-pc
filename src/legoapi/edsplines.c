#include "saga.h"

#define SPLINE_POINT(spline, index) ((NUVEC *)((u8 *)(spline)->pts + (index) * (i16)(spline)->pt_size))

#define SPLINE_POINT_ANGLES(spline, index, looping, pitch, angle)                                                      \
    do {                                                                                                               \
        NUVEC *_angle_current = &(spline)->pts[(index)];                                                               \
        NUVEC _angle_direction = {0.0f, 0.0f, 0.0f};                                                                   \
        NUVEC *_angle_previous = NULL;                                                                                 \
        NUVEC *_angle_next = NULL;                                                                                     \
        if ((index) > 0) {                                                                                             \
            _angle_previous = &(spline)->pts[(index) - 1];                                                             \
        } else if (looping) {                                                                                          \
            _angle_previous = &(spline)->pts[(spline)->length - 1];                                                    \
        }                                                                                                              \
        if (_angle_previous != NULL) {                                                                                 \
            _angle_direction.x += _angle_current->x - _angle_previous->x;                                              \
            _angle_direction.y += _angle_current->y - _angle_previous->y;                                              \
            _angle_direction.z += _angle_current->z - _angle_previous->z;                                              \
        }                                                                                                              \
        if ((index) + 1 < (spline)->length) {                                                                          \
            _angle_next = &(spline)->pts[(index) + 1];                                                                 \
        } else if (looping) {                                                                                          \
            _angle_next = &(spline)->pts[0];                                                                           \
        }                                                                                                              \
        if (_angle_next != NULL) {                                                                                     \
            _angle_direction.x += _angle_next->x - _angle_current->x;                                                  \
            _angle_direction.y += _angle_next->y - _angle_current->y;                                                  \
            _angle_direction.z += _angle_next->z - _angle_current->z;                                                  \
        }                                                                                                              \
        if ((pitch) != NULL) {                                                                                         \
            *(pitch) = NuAtan2D(_angle_direction.y, NuFsqrt(_angle_direction.x * _angle_direction.x +                  \
                                                            _angle_direction.z * _angle_direction.z));                 \
        }                                                                                                              \
        if ((angle) != NULL) {                                                                                         \
            *(angle) = NuAtan2D(_angle_direction.x, _angle_direction.z);                                               \
        }                                                                                                              \
    } while (0)

#define SPLINE_ROTATION_DIFFERENCE(current, target)                                                                    \
    (((i32)(u32)(target) - (i32)(u32)(current)) > 0x8000 ? ((i32)(u32)(target) - (i32)(u32)(current) - 0x10000)        \
                                                         : (((i32)(u32)(target) - (i32)(u32)(current)) < -0x8000       \
                                                                ? ((i32)(u32)(target) - (i32)(u32)(current) + 0x10000) \
                                                                : ((i32)(u32)(target) - (i32)(u32)(current))))

void __cdecl MoveSplinePosition(SPLINEPOS *position, f32 distance) {
    NUGSPLINE *spline;
    i32 segments;
    NUVEC offset;

    if (position == NULL || position->spline == NULL || position->spline->length <= 1) {
        return;
    }
    spline = position->spline;
    segments = spline->length - 1 + (position->looping != 0);
    if (position->segment >= segments) {
        return;
    }

    if (distance > 0.0f) {
        while (distance > 0.0f) {
            position->segment_distance += distance;
            if (!(position->segment_distance >= position->segment_length)) {
                break;
            }
            distance = position->segment_distance - position->segment_length;
            {
                i16 previous = position->segment;
                position->segment++;
                if (position->segment >= segments) {
                    if (!position->looping) {
                        position->finished = 1;
                        position->position = *SPLINE_POINT(spline, position->segment);
                        position->segment = previous;
                        position->along = 1.0f;
                        position->segment_distance = position->segment_length;
                        return;
                    }
                    position->segment = 0;
                }
            }
            {
                NUVEC *current = SPLINE_POINT(spline, position->segment);
                NUVEC *next = SPLINE_POINT(spline, (position->segment + 1) % spline->length);
                position->segment_distance = 0.0f;
                position->segment_length = NuVecDist(next, current, &offset);
                if (distance == 0.0f) {
                    position->position = *current;
                }
            }
            spline = position->spline;
        }
        if (!(distance > 0.0f)) {
            goto update_along;
        }
    } else if (distance < 0.0f && position->segment >= 0) {
        for (;;) {
            position->segment_distance += distance;
            if (!(position->segment_distance < 0.0f)) {
                break;
            }
            distance = position->segment_distance;
            {
                i16 previous = position->segment;
                position->segment--;
                if (position->segment < 0) {
                    if (!position->looping) {
                        position->finished = 1;
                        position->position = *SPLINE_POINT(spline, 0);
                        position->segment = previous;
                        position->along = 0.0f;
                        position->segment_distance = 0.0f;
                        return;
                    }
                    position->segment = (i16)(segments - 1);
                }
            }
            {
                NUVEC *current = SPLINE_POINT(spline, position->segment);
                NUVEC *next = SPLINE_POINT(spline, (position->segment + 1) % spline->length);
                position->segment_length = NuVecDist(next, current, &offset);
                position->segment_distance = position->segment_length;
                if (distance == 0.0f) {
                    position->position = *next;
                }
            }
            spline = position->spline;
        }
    } else {
        goto update_along;
    }

    {
        NUVEC *current = SPLINE_POINT(spline, position->segment);
        NUVEC *next = SPLINE_POINT(spline, (position->segment + 1) % spline->length);
        f32 fraction = 0.0f;

        NuVecSub(&offset, next, current);
        if (position->segment_length != 0.0f) {
            fraction = position->segment_distance / position->segment_length;
        }
        NuVecScale(&offset, &offset, fraction);
        NuVecAdd(&position->position, current, &offset);
    }

update_along:
    position->along = (position->segment_distance / position->segment_length + position->segment) / segments;
}

void __cdecl InitSplinePosition(SPLINEPOS *position, NUGSPLINE *spline, f32 distance, i32 looping) {
    NUVEC offset;
    NUVEC *first;
    NUVEC *second;

    if (position == NULL) {
        return;
    }
    memset(position, 0, sizeof(*position));
    if (spline == NULL || spline->length < 2) {
        return;
    }
    position->spline = spline;
    position->looping = (i8)looping;
    first = SPLINE_POINT(spline, 0);
    second = SPLINE_POINT(spline, 1);
    position->segment_length = NuVecDist(second, first, &offset);
    if (distance > 0.0f) {
        MoveSplinePosition(position, distance);
    } else {
        position->position = *first;
        position->along = 0.0f;
    }
}

void __cdecl PointAlongSpline(NUGSPLINE *spline, f32 along, NUVEC *position, u16 *angle, u16 *pitch, i32 looping) {
    u32 extent;
    u32 fixed;
    i32 index;
    NUVEC *current;
    NUVEC *next;
    f32 fraction;

    if (angle != NULL) {
        *angle = 0;
    }
    if (pitch != NULL) {
        *pitch = 0;
    }
    if (spline == NULL) {
        return;
    }
    if (along > 1.0f) {
        along = 1.0f;
    } else if (along < 0.0f) {
        along = 0.0f;
    }

    extent = (u32)(looping ? spline->length : spline->length - 1) << 16;
    fixed = (u32)(i32)((f32)extent * along);
    index = (i32)(fixed >> 16);
    current = &spline->pts[index];
    *position = *current;
    if (pitch != NULL || angle != NULL) {
        SPLINE_POINT_ANGLES(spline, index, looping, pitch, angle);
    }
    index++;
    if (index >= spline->length) {
        if (!looping) {
            return;
        }
        index = 0;
    }
    fixed &= 0xffff;
    if (fixed == 0) {
        return;
    }

    fraction = fixed * (1.0f / 65536.0f);
    next = &spline->pts[index];
    position->x += (next->x - current->x) * fraction;
    position->y += (next->y - current->y) * fraction;
    position->z += (next->z - current->z) * fraction;
    if (angle != NULL || pitch != NULL) {
        u16 next_angle = 0;
        u16 next_pitch = 0;
        SPLINE_POINT_ANGLES(spline, index, looping, pitch != NULL ? &next_pitch : NULL,
                            angle != NULL ? &next_angle : NULL);
        if (angle != NULL) {
            *angle = (u16)(*angle + (i32)(SPLINE_ROTATION_DIFFERENCE(*angle, next_angle) * fraction));
        }
        if (pitch != NULL) {
            *pitch = (u16)(*pitch + (i32)(SPLINE_ROTATION_DIFFERENCE(*pitch, next_pitch) * fraction));
        }
    }
}

void __cdecl GetNearestSplinePos(NUVEC *origin, SPLINEPOS *result, NUGSPLINE *spline, i32 looping, i16 first_point,
                                 i16 last_point) {
    i32 point_count;
    i32 logical_count;
    i32 index;
    i32 end;
    NUVEC *point;
    f32 nearest;
    NUVEC offset;
    NUVEC *current;
    NUVEC *next;

    if (result == NULL) {
        return;
    }
    memset(result, 0, sizeof(*result));
    if (spline == NULL || origin == NULL || spline->length <= 1) {
        return;
    }
    result->spline = spline;
    result->looping = (i8)looping;
    point_count = spline->length;
    logical_count = result->looping != 0 ? point_count + 1 : point_count;
    index = first_point;
    if (index < 0) {
        index = 0;
    } else if (index >= logical_count) {
        return;
    }
    end = point_count;
    if (last_point >= 0 && last_point < end) {
        end = last_point;
    }
    point = SPLINE_POINT(spline, index);
    nearest = 1000000000.0f;
    do {
        f32 distance = NuVecDistSqr(origin, point, &offset);
        if (distance < nearest) {
            nearest = distance;
            result->segment = (i16)index;
        }
        point = (NUVEC *)((u8 *)point + (i16)result->spline->pt_size);
        index++;
    } while (index < end);

    spline = result->spline;
    current = SPLINE_POINT(spline, result->segment);
    next = SPLINE_POINT(spline, (result->segment + 1) % spline->length);
    result->segment_distance = 0.0f;
    result->segment_length = NuVecDist(next, current, &offset);
    result->position = *current;
    result->along = (result->segment_distance / result->segment_length + result->segment) / (logical_count - 1);
}

#undef SPLINE_ROTATION_DIFFERENCE
#undef SPLINE_POINT_ANGLES
#undef SPLINE_POINT
