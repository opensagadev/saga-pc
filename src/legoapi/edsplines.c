#include "saga.h"

#define SPLINE_POINT(spline, index) ((NUVEC *)((u8 *)(spline)->pts + (index) * (i16)(spline)->pt_size))

i32 __cdecl RotDiff(u16 current, u16 target) {
    i32 difference = (i32)target - (i32)current;

    if (difference > 0x8000) {
        difference -= 0x10000;
    } else if (difference < -0x8000) {
        difference += 0x10000;
    }
    return difference;
}

static void SplinePointAngles(NUGSPLINE *spline, i32 index, i32 looping, u16 *pitch, u16 *angle) {
    NUVEC *current;
    NUVEC *previous;
    NUVEC *next;
    i32 previous_index;
    i32 next_index;
    f32 dx;
    f32 dy;
    f32 dz;

    if (pitch == NULL && angle == NULL) {
        return;
    }
    dx = 0.0f;
    dy = 0.0f;
    dz = 0.0f;
    current = &spline->pts[index];
    previous = NULL;
    previous_index = index - 1;
    if (previous_index < 0 && looping) {
        previous = &spline->pts[spline->length - 1];
    } else if (previous_index >= 0) {
        previous = &spline->pts[previous_index];
    }
    if (previous != NULL) {
        dx += current->x - previous->x;
        dy += current->y - previous->y;
        dz += current->z - previous->z;
    }
    next = NULL;
    next_index = index + 1;
    if (next_index >= spline->length && looping) {
        next = spline->pts;
    } else if (next_index < spline->length) {
        next = &spline->pts[next_index];
    }
    if (next != NULL) {
        dx += next->x - current->x;
        dy += next->y - current->y;
        dz += next->z - current->z;
    }
    if (pitch != NULL) {
        *pitch = NuAtan2D(dy, NuFsqrt(dx * dx + dz * dz));
    }
    if (angle != NULL) {
        *angle = NuAtan2D(dx, dz);
    }
}

void __cdecl MoveSplinePosition(SPLINEPOS *position, f32 distance) {
    i32 points;
    NUVEC offset;
    NUVEC *current, *next;
    f32 fraction;

    if (position == NULL || position->spline == NULL || position->spline->length <= 1)
        return;
    if (position->looping)
        points = position->spline->length + 1;
    else
        points = position->spline->length;
    if (position->segment >= points - 1)
        return;

    if (distance >= 0.0f) {
        if (distance > 0.0f)
            do {
                position->segment_distance += distance;
                if (!(position->segment_distance >= position->segment_length)) {
                    current = SPLINE_POINT(position->spline, position->segment);
                    next = SPLINE_POINT(position->spline, (position->segment + 1) % position->spline->length);
                    NuVecSub(&offset, next, current);
                    fraction =
                        position->segment_length != 0.0f ? position->segment_distance / position->segment_length : 0.0f;
                    NuVecScale(&offset, &offset, fraction);
                    NuVecAdd(&position->position, current, &offset);
                    break;
                }
                distance = position->segment_distance - position->segment_length;
                position->segment++;
                if (position->segment >= points - 1) {
                    if (!position->looping) {
                        position->finished = 1;
                        position->position = *SPLINE_POINT(position->spline, position->segment);
                        position->segment--;
                        position->along = 1.0f;
                        position->segment_distance = position->segment_length;
                        return;
                    }
                    position->segment = 0;
                }
                current = SPLINE_POINT(position->spline, position->segment);
                next = SPLINE_POINT(position->spline, (position->segment + 1) % position->spline->length);
                position->segment_distance = 0.0f;
                position->segment_length = NuVecDist(next, current, &offset);
                if (distance == 0.0f)
                    position->position = *current;
            } while (distance > 0.0f);
    } else if (position->segment >= 0 && distance < 0.0f) {
        do {
            position->segment_distance += distance;
            if (!(position->segment_distance < 0.0f)) {
                current = SPLINE_POINT(position->spline, position->segment);
                next = SPLINE_POINT(position->spline, (position->segment + 1) % position->spline->length);
                NuVecSub(&offset, next, current);
                fraction =
                    position->segment_length != 0.0f ? position->segment_distance / position->segment_length : 0.0f;
                NuVecScale(&offset, &offset, fraction);
                NuVecAdd(&position->position, current, &offset);
                break;
            }
            distance = position->segment_distance;
            position->segment--;
            if (position->segment < 0) {
                if (!position->looping) {
                    position->finished = 1;
                    position->position = *SPLINE_POINT(position->spline, 0);
                    position->segment++;
                    position->along = 0.0f;
                    position->segment_distance = 0.0f;
                    return;
                }
                position->segment = (i16)(points - 2);
            }
            current = SPLINE_POINT(position->spline, position->segment);
            next = SPLINE_POINT(position->spline, (position->segment + 1) % position->spline->length);
            position->segment_length = NuVecDist(next, current, &offset);
            position->segment_distance = position->segment_length;
            if (distance == 0.0f)
                position->position = *next;
        } while (distance < 0.0f);
    }
    fraction = position->segment_distance / position->segment_length;
    position->along = (fraction + position->segment) / (points - 1);
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
    i32 next_index;
    NUVEC *current;
    NUVEC *next;
    f32 fraction;
    u16 next_angle;
    u16 next_pitch;

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

    if (looping) {
        extent = (u32)spline->length << 16;
    } else {
        extent = (u32)(spline->length - 1) << 16;
    }
    fixed = (u32)(i32)((f32)extent * along);
    index = (i32)(fixed >> 16);
    current = &spline->pts[index];
    *position = *current;
    SplinePointAngles(spline, index, looping, pitch, angle);
    next_index = index + 1;
    if (next_index >= spline->length) {
        if (!looping) {
            return;
        }
        next_index = 0;
    } else if (next_index < 0) {
        return;
    }
    fixed &= 0xffff;
    if (fixed == 0) {
        return;
    }

    next = &spline->pts[next_index];
    fraction = fixed * (1.0f / 65536.0f);
    position->x += (next->x - current->x) * fraction;
    position->y += (next->y - current->y) * fraction;
    position->z += (next->z - current->z) * fraction;
    SplinePointAngles(spline, next_index, looping, pitch != NULL ? &next_pitch : NULL,
                      angle != NULL ? &next_angle : NULL);
    if (angle != NULL) {
        *angle += (i32)(RotDiff(*angle, next_angle) * fraction);
    }
    if (pitch != NULL) {
        *pitch += (i32)(RotDiff(*pitch, next_pitch) * fraction);
    }
}

void __cdecl GetNearestSplinePos(NUVEC *origin, SPLINEPOS *result, NUGSPLINE *spline, i32 looping, i16 first_point,
                                 i16 last_point) {
    f32 nearest = 1000000000.0f;
    i16 point_count;
    i32 logical_count, index;
    NUVEC *point, *current, *next;
    NUVEC offset;

    if (result == NULL)
        return;
    memset(result, 0, sizeof(*result));
    if (origin == NULL || spline == NULL || spline->length <= 1)
        return;
    result->spline = spline;
    result->looping = (i8)looping;
    point_count = spline->length;
    logical_count = result->looping ? point_count + 1 : point_count;
    if (first_point >= 0) {
        index = first_point;
        if (index >= logical_count)
            return;
    } else
        index = 0;
    if (last_point < 0 || last_point > point_count)
        last_point = point_count;
    point = SPLINE_POINT(spline, index);
    do {
        f32 distance = NuVecDistSqr(origin, point, &offset);
        if (distance < nearest) {
            nearest = distance;
            result->segment = (i16)index;
        }
        point = (NUVEC *)((u8 *)point + (i16)result->spline->pt_size);
        index++;
    } while (index < last_point);

    spline = result->spline;
    current = SPLINE_POINT(spline, result->segment);
    next = SPLINE_POINT(spline, (result->segment + 1) % spline->length);
    result->segment_distance = 0.0f;
    result->segment_length = NuVecDist(next, current, &offset);
    result->position = *current;
    nearest = result->segment_distance / result->segment_length;
    result->along = (nearest + result->segment) / (logical_count - 1);
}

void EvaluateSplineXZIntersection(NUGSPLINE *first, i32 first_looping, SPLINEPOS *first_position, NUGSPLINE *second,
                                  i32 second_looping, SPLINEPOS *second_position) {
    f32 closest = 1000000000.0f;
    i32 first_count, second_count, i, j, first_segment, second_segment;
    NUVEC *first_start, *first_end;
    f32 distance, first_fraction, second_fraction;
    NUVEC direction;

    memset(first_position, 0, sizeof(*first_position));
    memset(second_position, 0, sizeof(*second_position));
    first_position->spline = first;
    first_position->looping = (i8)first_looping;
    second_position->spline = second;
    second_position->looping = (i8)second_looping;
    if (first == NULL || second == NULL || first->length == 0 || second->length == 0)
        return;
    first_count = first->length;
    if (first_looping)
        first_count++;
    second_count = second->length;
    if (second_looping)
        second_count++;
    for (i = 0; i < first_count - 1; ++i) {
        first_segment = i % first->length;
        first_start = &first->pts[first_segment];
        first_end = &first->pts[(i + 1) % first->length];
        /* Retail includes the wraparound segment of the second spline even
         * without looping, and only stops this inner scan at an intersection. */
        for (j = 0; j < second_count; ++j) {
            second_segment = j % second->length;
            distance = XZLinesClosest(first_start, first_end, &second->pts[second_segment],
                                      &second->pts[(j + 1) % second->length], &first_fraction, &second_fraction);
            if (distance < closest) {
                closest = distance;
                first_position->segment = (i16)first_segment;
                first_position->segment_distance = first_fraction;
                second_position->segment = (i16)second_segment;
                second_position->segment_distance = second_fraction;
                if (distance == 0.0f)
                    break;
            }
        }
    }
    first_position->segment_length = NuVecDist(&first->pts[(first_position->segment + 1) % first->length],
                                               &first->pts[first_position->segment], &direction);
    first_position->segment_distance *= first_position->segment_length;
    MoveSplinePosition(first_position, 0.00001f);
    second_position->segment_length = NuVecDist(&second->pts[(second_position->segment + 1) % second->length],
                                                &second->pts[second_position->segment], &direction);
    second_position->segment_distance *= second_position->segment_length;
    MoveSplinePosition(second_position, 0.00001f);
}

#undef SPLINE_POINT

i32 __cdecl OutSideSplineArea(NUVEC *position, NUGSPLINE *spline, NUVEC *edge_end, NUVEC *edge_start, i32 inside) {
    f32 best = 1000000000.0f;
    i32 closest_index, count, i;
    NUVEC closest;

    if (position == NULL || spline == NULL)
        return 0;
    {
        NUVEC ray_start, ray_end, end;
        f32 end_y;
        i32 intersections, odd;
        ray_end = *position;
        ray_start = *position;
        end_y = ray_start.y;
        ray_start.y = -ray_start.z;
        ray_start.z = end_y;
        ray_end.z = ray_end.y;
        ray_end.y = 100000.0f;
        end = spline->pts[0];
        end_y = end.y;
        end.y = -end.z;
        end.z = end_y;
        intersections = 0;
        for (i = 1; i < spline->length; ++i) {
            NUVEC start = end;
            end = spline->pts[i];
            end_y = end.y;
            end.y = -end.z;
            end.z = end_y;
            intersections += LineIntersectXY(&ray_start, &ray_end, &start, &end, NULL, NULL);
        }
        odd = intersections & 1;
        if (inside == 0) {
            if (odd != 0)
                return 0;
        } else if (odd == 0)
            return 0;
    }
    closest_index = 0;
    closest = spline->pts[0];
    count = spline->length;
    for (i = 0; i < count - 1; ++i) {
        NUVEC point = spline->pts[i];
        NUVEC delta;
        f32 distance;
        delta.x = point.x - position->x;
        delta.z = point.z - position->z;
        distance = delta.x * delta.x + delta.z * delta.z;
        if (distance < best) {
            closest_index = i;
            closest = point;
            best = distance;
        }
    }
    {
        NUVEC next_point, previous_point, end_point, start_point, a, b;
        i32 previous, next;
        if (closest_index == 0) {
            previous = count - 2;
            next = 1;
        } else if (closest_index == count - 2) {
            previous = closest_index - 1;
            next = 0;
        } else {
            previous = closest_index - 1;
            next = closest_index + 1;
        }
        next_point = spline->pts[next];
        previous_point = spline->pts[previous];
        a.x = next_point.x - position->x;
        a.z = next_point.z - position->z;
        b.x = previous_point.x - position->x;
        b.z = previous_point.z - position->z;
        if (b.x * b.x + b.z * b.z > a.x * a.x + a.z * a.z) {
            end_point = next_point;
            start_point = closest;
        } else {
            end_point = closest;
            start_point = previous_point;
        }
        if (edge_end != NULL)
            *edge_end = end_point;
        if (edge_start != NULL)
            *edge_start = start_point;
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
