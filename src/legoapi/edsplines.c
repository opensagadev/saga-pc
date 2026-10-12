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

void EvaluateSplineXZIntersection(NUGSPLINE *first, i32 first_looping, SPLINEPOS *first_position, NUGSPLINE *second,
                                  i32 second_looping, SPLINEPOS *second_position) {
    memset(first_position, 0, sizeof(*first_position));
    memset(second_position, 0, sizeof(*second_position));
    first_position->spline = first;
    first_position->looping = ((i8)(first_looping));
    second_position->spline = second;
    second_position->looping = ((i8)(second_looping));
    if (!(second == NULL || first == NULL || first->length == 0 || second->length == 0)) {
        i32 i;
        NUVEC direction;
        const i32 second_count = second_looping != 0 ? second->length + 1 : second->length;
        const i32 first_count = first_looping != 0 ? first->length + 1 : first->length;
        f32 closest = 1000000000.0f;
        f32 first_fraction, second_fraction;
        for (i = 0; i < first_count - 1; ++i) {
            i32 j;
            const i32 first_segment = i % first->length;
            NUVEC *first_start = &first->pts[first_segment];
            NUVEC *first_end = &first->pts[(i + 1) % first->length];
            // Retail includes the wraparound segment of the second spline even
            // without looping, and only stops this inner scan at an intersection.
            for (j = 0; j < second_count; ++j) {
                const i32 second_segment = j % second->length;
                const f32 distance =
                    XZLinesClosest(first_start, first_end, &second->pts[second_segment],
                                   &second->pts[(j + 1) % second->length], &first_fraction, &second_fraction);
                if (distance < closest) {
                    first_position->segment_distance = first_fraction;
                    first_position->segment = ((i16)(first_segment));
                    second_position->segment_distance = second_fraction;
                    second_position->segment = ((i16)(second_segment));
                    closest = distance;
                    if (distance == 0.0f)
                        break;
                }
            }
        }

        direction = direction;
        first_position->segment_length = NuVecDist(&first->pts[(first_position->segment + 1) % first->length],
                                                   &first->pts[first_position->segment], &direction);
        first_position->segment_distance *= first_position->segment_length;
        MoveSplinePosition(first_position, 0.00001f);
        second_position->segment_length = NuVecDist(&second->pts[(second_position->segment + 1) % second->length],
                                                    &second->pts[second_position->segment], &direction);
        second_position->segment_distance *= second_position->segment_length;
        MoveSplinePosition(second_position, 0.00001f);
    }
}

#undef SPLINE_ROTATION_DIFFERENCE
#undef SPLINE_POINT_ANGLES
#undef SPLINE_POINT

i32 __cdecl OutSideSplineArea(NUVEC *position, NUGSPLINE *spline, NUVEC *edge_end, NUVEC *edge_start, i32 inside) {
    NUVEC ray_start;
    NUVEC ray_end;
    NUVEC end;
    NUVEC closest;
    NUVEC before;
    NUVEC after;
    i32 intersections = 0;
    i32 closest_index = 0;
    i32 i;
    i32 previous;
    i32 next;
    f32 best = 1000000000.0f;
    f32 ax;
    f32 az;
    f32 bx;
    f32 bz;

    if (position == NULL || spline == NULL) {
        return 0;
    }
    ray_start.x = position->x;
    ray_start.y = -position->z;
    ray_start.z = position->y;
    ray_end.x = position->x;
    ray_end.y = 100000.0f;
    ray_end.z = position->y;
    end.x = spline->pts[0].x;
    end.y = -spline->pts[0].z;
    end.z = spline->pts[0].y;
    for (i = 1; i < spline->length; ++i) {
        NUVEC start = end;
        end.x = spline->pts[i].x;
        end.y = -spline->pts[i].z;
        end.z = spline->pts[i].y;
        intersections += LineIntersectXY(&ray_start, &ray_end, &start, &end, NULL, NULL);
    }
    if ((intersections & 1) != 0 ? inside == 0 : inside != 0) {
        return 0;
    }
    closest = spline->pts[0];
    for (i = 0; i < spline->length - 1; ++i) {
        f32 dx = spline->pts[i].x - position->x;
        f32 dz = spline->pts[i].z - position->z;
        f32 distance = dx * dx + dz * dz;
        if (distance < best) {
            closest_index = i;
            closest = spline->pts[i];
            best = distance;
        }
    }
    previous = closest_index == 0 ? spline->length - 2 : closest_index - 1;
    next = closest_index == spline->length - 2 ? 0 : closest_index + 1;
    before = spline->pts[previous];
    after = spline->pts[next];
    ax = after.x - position->x;
    az = after.z - position->z;
    bx = before.x - position->x;
    bz = before.z - position->z;
    if (bx * bx + bz * bz > ax * ax + az * az) {
        if (edge_end != NULL) {
            *edge_end = after;
        }
        if (edge_start != NULL) {
            *edge_start = closest;
        }
    } else {
        if (edge_end != NULL) {
            *edge_end = closest;
        }
        if (edge_start != NULL) {
            *edge_start = before;
        }
    }
    return 1;
}

extern i32 RenderHubFromCutscenePlayerPc;
extern const char RenderSplineBountyStartPc[13];
extern const char RenderSplineShopStartPc[11];
STATIC_ASSERT(offsetof(WORLDINFO, camera_splines) == 0x2ac4);
STATIC_ASSERT(offsetof(LEVELSPLINE_PC, scene) == 0);
STATIC_ASSERT(offsetof(LEVELSPLINE_PC, max_points) == 0xa);
STATIC_ASSERT(offsetof(LEVELSPLINE_PC, level) == 0xc);
void __cdecl LevelSplines_InitForLevel(WORLDINFO *world) {
    i32 i;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3u) & ~3u;
    world->camera_splines = (NUGSPLINE **)world->giz_buffer.void_ptr;
    world->giz_buffer.addr += LEVELSPLINECOUNT * sizeof(*world->camera_splines);
    memset(world->camera_splines, 0, LEVELSPLINECOUNT * sizeof(*world->camera_splines));
    if (LevSplList == NULL)
        return;
    for (i = 0; i < LEVELSPLINECOUNT; ++i) {
        LEVELSPLINE_PC *entry = &LevSplList[i];
        NUGSCN *scene;
        NUGSPLINE *spline;
        char name[64];
        if ((entry->area != -1 && world->area_index != entry->area) ||
            (entry->level != -1 && world->level_idx != entry->level))
            continue;
        scene = *(entry->scene != NULL ? entry->scene : &world->current_gscn);
        if (scene != NULL) {
            spline = NuSplineFind(scene, (char *)entry->name);
            world->camera_splines[i] = spline;
            if (spline != NULL && ((entry->min_points > 0 && spline->length < entry->min_points) ||
                                   (entry->max_points > 0 && entry->max_points >= entry->min_points &&
                                    spline->length > entry->max_points)))
                world->camera_splines[i] = NULL;
        }
        if (levspl_i_start != -1) {
            name[0] = '\0';
            if (Mission_Active(NULL) != NULL)
                NuStrCpy(name, RenderSplineBountyStartPc);
            else if (world->area_index != -1 && (ADataList[world->area_index].flags & 0x40) &&
                     RenderHubFromCutscenePlayerPc != 0)
                NuStrCpy(name, RenderSplineShopStartPc);
            if (name[0] != '\0') {
                spline = NuSplineFind(scene, name);
                if (spline != NULL && spline->length >= 2) {
                    world->camera_splines[levspl_i_start] = spline;
                    if (levspl_i_startcam != -1)
                        world->camera_splines[levspl_i_startcam] = NULL;
                }
            }
        }
    }
}

void __cdecl CalcSplinePoint(FLIGHT_SPLINE_RESOURCE_PC *spline, NUVEC4 *result, f32 along);
void __cdecl CalcSplinePointFromDist(FLIGHT_SPLINE_RESOURCE_PC *spline, NUVEC4 *result, f32 distance);

void __cdecl CalcSplinePoint(FLIGHT_SPLINE_RESOURCE_PC *spline, NUVEC4 *result, f32 along) {
    f32 scaled = (spline->point_count - 1) * along;
    i32 index = (i32)scaled;
    f32 fraction = scaled - index;
    __declspec(align(16)) NUVEC4 first, last;
    __declspec(align(16)) NUVEC direction;
    NUVEC4 *previous, *current, *next, *following;
    f32 squared, cubed;
    if (index > 0) {
        if (index > spline->point_count - 1)
            index = spline->point_count;
        previous = &spline->points[index - 1];
    } else {
        direction.x = spline->points[0].x - spline->points[1].x;
        direction.y = spline->points[0].y - spline->points[1].y;
        direction.z = spline->points[0].z - spline->points[1].z;
        NuVecNorm(&direction, &direction);
        direction.x *= 10.0;
        direction.y *= 10.0;
        direction.z *= 10.0;
        first.x = spline->points[0].x + direction.x;
        first.y = spline->points[0].y + direction.y;
        first.z = spline->points[0].z + direction.z;
        first.w = spline->points[0].w;
        previous = &first;
        index = 0;
    }
    current = &spline->points[index];
    next = &spline->points[index + 1];
    if (index >= spline->point_count - 2) {
        i32 end = spline->point_count - 1;
        direction.x = spline->points[end].x - spline->points[end - 1].x;
        direction.y = spline->points[end].y - spline->points[end - 1].y;
        direction.z = spline->points[end].z - spline->points[end - 1].z;
        NuVecNorm(&direction, &direction);
        direction.x *= 10.0;
        direction.y *= 10.0;
        direction.z *= 10.0;
        last.x = spline->points[end].x + direction.x;
        last.y = spline->points[end].y + direction.y;
        last.z = spline->points[end].z + direction.z;
        last.w = spline->points[end].w;
        following = &last;
    } else {
        following = &spline->points[index + 2];
    }
    squared = fraction * fraction;
    cubed = (f32)((f64)fraction * fraction * fraction);
    /* Original vector arithmetic rounds each vector operation to its PC f32 field. */
#define SPLINE_COMPONENT(member)                                                                                       \
    {                                                                                                                  \
        f32 constant = current->member * 2.0f;                                                                         \
        f32 linear = next->member - previous->member;                                                                  \
        f32 quadratic = previous->member * 2.0f;                                                                       \
        f32 coefficient = current->member * 5.0;                                                                       \
        f32 cubic = current->member - next->member;                                                                    \
        f32 sum;                                                                                                       \
        linear *= fraction;                                                                                            \
        quadratic -= coefficient;                                                                                      \
        coefficient = next->member * 4.0;                                                                              \
        quadratic += coefficient;                                                                                      \
        quadratic -= following->member;                                                                                \
        quadratic *= squared;                                                                                          \
        cubic *= 3.0;                                                                                                  \
        cubic -= previous->member;                                                                                     \
        cubic += following->member;                                                                                    \
        cubic *= cubed;                                                                                                \
        sum = linear + constant;                                                                                       \
        sum += quadratic;                                                                                              \
        sum += cubic;                                                                                                  \
        result->member = sum * 0.5;                                                                                    \
    }
    SPLINE_COMPONENT(x);
    SPLINE_COMPONENT(y);
    SPLINE_COMPONENT(z);
#undef SPLINE_COMPONENT
    result->w = current->w + (next->w - current->w) * fraction;
}

void __cdecl CalcSplinePointFromDist(FLIGHT_SPLINE_RESOURCE_PC *spline, NUVEC4 *result, f32 distance) {
    i32 index;
    if (distance >= spline->length) {
        distance = 1.0f;
    } else {
        for (index = 0; index < spline->point_count; ++index) {
            f32 end = spline->cumulative_distances[index];
            if (end > distance) {
                f32 start = index == 0 ? 0.0f : spline->cumulative_distances[index - 1];
                f32 segment_fraction = (distance - start) / (end - start);
                distance = (segment_fraction + index) / spline->point_count;
                break;
            }
        }
    }
    CalcSplinePoint(spline, result, distance);
}

void ChrisAnakinCReset() {
    struct anakin_door_setup_s *setup;
    anakin_door_s *door = AnakinC;
    i32 count = 0;
    for (setup = DoorSetupList; count < 12; ++setup) {
        if (setup->name == NULL) {
            for (; count < 12; ++count, ++door)
                door->active = 0;
            return;
        }
        if (!NuSpecialFind(WORLD->current_gscn, &door->special, setup->name, 1) || !NuSpecialExistsFn(&door->special))
            continue;

        if (NuSpecialFind(WORLD->current_gscn, &door->secondary_special, setup->secondary_name, 1))
            door->has_secondary = ((i16)(NuSpecialExistsFn(&door->secondary_special)));

        NuMtxSetIdentity(&door->original_matrix);
        door->matrix = door->original_matrix = *NuSpecialGetMtx(&door->special);
        if (door->has_secondary != 0) {
            NuMtxSetIdentity(&door->original_secondary_matrix);
            door->secondary_matrix = door->original_secondary_matrix = *NuSpecialGetMtx(&door->secondary_special);
        }
        door->platform_id = ((i16)(FindPlatInst(NuSpecialGetInstanceix(&door->special))));
        door->active = 1;
        door->flags = ((u8)(setup->flags));
        door->direction = setup->direction;
        door->offset = setup->initial_offset;
        door->speed = setup->speed;
        door->minimum_offset = setup->minimum_offset;
        door->unknown_134 = setup->unknown_20;
        ++door;
        ++count;
    }
}

void ChrisAnakinCInit() {
    NuSpecialList(WORLD->current_gscn);
    ChrisAnakinCReset();
}
