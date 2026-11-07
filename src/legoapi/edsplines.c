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

static void *GizSpinner_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = ((WORLDINFO *)(world_ptr));
    world->spinners = NULL;
    world->spinner_count = 0;
    if (world->current_level->max_spinners != 0) {
        i32 index;
        world->spinner_anim_pool = GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 0,
                                                                world->current_level->max_spinneranim_objs);

        world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 16);
        world->spinners = ((GIZSPINNER_s *)(world->giz_buffer.void_ptr));
        world->giz_buffer.addr += world->current_level->max_spinners * sizeof(GIZSPINNER_s);
        memset(world->spinners, 0, world->current_level->max_spinners * sizeof(GIZSPINNER_s));

        for (index = 0; index < world->current_level->max_spinners; ++index) {
            world->spinners[index].anim_set = GameAnimSet_Create(&world->giz_buffer, &world->buffer_end,
                                                                 world->spinner_anim_pool, world->game_anim_sys);
        }

        world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 4);
        return world->spinners;
    }
    return NULL;
}

void GizSpinners_InitTerrain(WORLDINFO *world) {
    i32 index;
    i32 use_default_terrain;
    if (world->spinners == NULL || world->current_level->max_spinners == 0) {
        return;
    }

    index = 0;
    use_default_terrain = 0;
    do {
        if ((world->spinners[index].flags & GIZSPINNER_FLAG_VALID) != 0) {
            NUMTX matrix;
            i32 instance = NuSpecialGetInstanceix(&world->spinners[index].special);
            FindPlatInst(instance);
            matrix = matrix;
            NuMtxSetIdentity(&matrix);
            NuMtxSetRotationY(&matrix, 0);
            NuMtxRotateY(&matrix, world->spinners[index].rotation);
            NuMtxTranslate(&matrix, &world->spinners[index].position);
            world->spinners[index].matrix = matrix;

            if (NuSpecialCompare(&world->spinners[index].special, &thingsSceneBase) != 0) {
                use_default_terrain = 1;
            }
            if (use_default_terrain != 0) {
                i32 arm;
                if (LEGOTHINGSSCENE_TER_SPINBASE != -1) {
                    world->spinners[index].platform_id =
                        NewPlatPickupInst(&world->spinners[index].matrix, LEGOTHINGSSCENE_TER_SPINBASE);
                } else {
                    world->spinners[index].platform_id = -1;
                }
                for (arm = 0; arm < world->spinners[index].type; ++arm) {
                    if (LEGOTHINGSSCENE_TER_SPINARM != -1) {
                        world->spinners[index].arms[arm].platform_id =
                            NewPlatPickupInst(&world->spinners[index].arms[arm].matrix, LEGOTHINGSSCENE_TER_SPINARM);
                    } else {
                        world->spinners[index].arms[arm].platform_id = -1;
                    }
                    PlatInstRotate(world->spinners[index].arms[arm].platform_id, 1);
                }
            } else {
                i32 arm;
                world->spinners[index].platform_id = ((i16)(NewPlatInst(&world->spinners[index].matrix, instance)));
                for (arm = 0; arm < world->spinners[index].type; ++arm) {
                    world->spinners[index].arms[arm].platform_id = -1;
                }
            }
        }
        ++index;
    } while (world->current_level->max_spinners > index);
}

static void GizSpinner_Draw(void *world_ptr, void *unused1, float unused2) {
    i32 index;
    NUMTX reflection_matrix;
    WORLDINFO *world = ((WORLDINFO *)(world_ptr));
    if (world->spinners == NULL || world->current_level->max_spinners == 0) {
        return;
    }

    for (index = 0; index < world->current_level->max_spinners; ++index) {
        struct GIZSPINNER_s *spinner;
        u8 special_exists;
        struct WORLDINFO_s *reflection_world;
        EnableShadowMapRendering(0);
        spinner = &world->spinners[index];
        if ((spinner->flags & GIZSPINNER_FLAG_VALID) == 0 ||
            ((spinner->flags & GIZSPINNER_FLAG_HIDE_BASE) != 0 && editor_active == 0)) {
            continue;
        }

        special_exists = NuSpecialExistsFn(&spinner->special) != 0;
        if (special_exists) {
            NuSpecialDrawAt(&spinner->special, &spinner->matrix);
            if (spinner->type != 0 && NuSpecialCompare(&spinner->special, &thingsSceneBase) != 0 &&
                NuSpecialExistsFn(&thingsSceneArm) != 0) {
                i32 arm;
                GIZSPINNERARM_s *arms = spinner->arms;
                for (arm = 0; arm < spinner->type; ++arm) {
                    NuSpecialDrawAt(&thingsSceneArm, &arms[arm].matrix);
                }
            }
        }

        ResetShadowMapRendering();
        if ((spinner->state_flags & GIZSPINNER_STATE_SHADOW_PLATFORM) == 0 ||
            NuSpecialExistsFn(&spinner->special) == 0 || NuSpecialGetVisibilityFn(&spinner->special) == 0) {
            continue;
        }

        reflection_world = WORLD != NULL && WORLD->current_level != NULL ? WORLD : world;
        if (MatrixReflectionVU0_AXISY(&spinner->matrix, spinner->ground_height,
                                      reflection_world->current_level->reflection_y, &reflection_matrix) == 0) {
            continue;
        }
        NuRndrStartReflectionRender(0);
        NuSpecialDrawAt(&spinner->special, &reflection_matrix);
        if (spinner->type != 0 && NuSpecialCompare(&spinner->special, &thingsSceneBase) != 0 &&
            NuSpecialExistsFn(&thingsSceneArm) != 0) {
            i32 arm;
            GIZSPINNERARM_s *arms = spinner->arms;
            for (arm = 0; arm < spinner->type; ++arm) {
                NuSpecialDrawAt(&thingsSceneArm, &arms[arm].matrix);
            }
        }
        NuRndrEndReflectionRender();
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

void __cdecl CalcSplinePoint(FLIGHT_SPLINE_RESOURCE_PC *spline, NUVEC4 *result, f32 along);
void __cdecl CalcSplinePointFromDist(FLIGHT_SPLINE_RESOURCE_PC *spline, NUVEC4 *result, f32 distance);

void __cdecl CalcSplinePoint(FLIGHT_SPLINE_RESOURCE_PC *spline, NUVEC4 *result, f32 along) {
    f32 scaled = (spline->point_count - 1) * along;
    i32 index = (i32)scaled;
    f32 fraction = scaled - index;
    __declspec(align(16)) NUVEC4 direction;
    __declspec(align(16)) NUVEC4 first, last;
    __declspec(align(16)) NUVEC4 constant, linear, quadratic, coefficient, cubic;
    NUVEC4 *previous, *current, *next, *following;
    f32 squared, cubed, sum;
    if (index <= 0) {
        direction.x = spline->points[0].x - spline->points[1].x;
        direction.y = spline->points[0].y - spline->points[1].y;
        direction.z = spline->points[0].z - spline->points[1].z;
        index = 0;
        NuVecNorm((NUVEC *)&direction, (NUVEC *)&direction);
        direction.w = 1.0f;
        direction.x *= 10.0;
        direction.y *= 10.0;
        direction.z *= 10.0;
        first.x = spline->points[0].x + direction.x;
        first.y = spline->points[0].y + direction.y;
        first.z = spline->points[0].z + direction.z;
        first.w = spline->points[0].w;
        previous = &first;
    } else {
        if (index > spline->point_count - 1)
            index = spline->point_count;
        previous = &spline->points[index - 1];
    }
    current = &spline->points[index];
    next = &spline->points[index + 1];
    if (index >= spline->point_count - 2) {
        NUVEC4 *before_end = &spline->points[spline->point_count - 2];
        NUVEC4 *end_point = &spline->points[spline->point_count - 1];
        direction.x = end_point->x - before_end->x;
        direction.y = end_point->y - before_end->y;
        direction.z = end_point->z - before_end->z;
        NuVecNorm((NUVEC *)&direction, (NUVEC *)&direction);
        direction.w = 1.0f;
        direction.x *= 10.0;
        direction.y *= 10.0;
        direction.z *= 10.0;
        last.x = spline->points[spline->point_count - 1].x + direction.x;
        last.y = spline->points[spline->point_count - 1].y + direction.y;
        last.z = spline->points[spline->point_count - 1].z + direction.z;
        last.w = spline->points[spline->point_count - 1].w;
        following = &last;
    } else {
        following = &spline->points[index + 2];
    }
    constant.x = current->x + current->x;
    constant.y = current->y + current->y;
    constant.z = current->z + current->z;
    linear.x = next->x - previous->x;
    linear.y = next->y - previous->y;
    linear.z = next->z - previous->z;
    linear.x *= fraction;
    linear.y *= fraction;
    linear.z *= fraction;
    quadratic.x = previous->x + previous->x;
    quadratic.y = previous->y + previous->y;
    quadratic.z = previous->z + previous->z;
    coefficient.x = current->x * 5.0;
    coefficient.y = current->y * 5.0;
    coefficient.z = current->z * 5.0;
    quadratic.x -= coefficient.x;
    quadratic.y -= coefficient.y;
    quadratic.z -= coefficient.z;
    coefficient.x = next->x * 4.0;
    coefficient.y = next->y * 4.0;
    coefficient.z = next->z * 4.0;
    quadratic.x += coefficient.x;
    quadratic.y += coefficient.y;
    quadratic.z += coefficient.z;
    quadratic.x -= following->x;
    quadratic.y -= following->y;
    quadratic.z -= following->z;
    sum = fraction * fraction;
    quadratic.x *= sum;
    quadratic.y *= sum;
    quadratic.z *= sum;
    cubic.x = current->x - next->x;
    cubic.y = current->y - next->y;
    cubic.z = current->z - next->z;
    cubic.x *= 3.0;
    cubic.y *= 3.0;
    cubic.z *= 3.0;
    cubic.x -= previous->x;
    cubic.y -= previous->y;
    cubic.z -= previous->z;
    cubic.x = cubic.x + following->x;
    cubic.y = following->y + cubic.y;
    cubic.z += following->z;
    sum *= fraction;
    cubic.x *= sum;
    cubic.y *= sum;
    cubic.z *= sum;
    sum = linear.x + constant.x;
    sum += quadratic.x;
    sum += cubic.x;
    result->x = sum * 0.5;
    sum = linear.y + constant.y;
    sum += quadratic.y;
    sum += cubic.y;
    result->y = sum * 0.5;
    sum = linear.z + constant.z;
    sum += quadratic.z;
    sum += cubic.z;
    result->z = sum * 0.5;
    result->w = (next->w - current->w) * fraction + current->w;
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

void ChrisAnakinBInit() {
    nuhspecial_s centre;
    RadialMoveCentre.x = 0.0f;
    RadialMoveCentre.y = 0.0f;
    RadialMoveCentre.z = 0.0f;
    RadialMoveCentre.w = 1.0f;
    MaxRadialCamY = 8.36f;
    NuSpecialList(WORLD->current_gscn);

    if (NuSpecialFind(WORLD->current_gscn, &centre, "Centre", 1) != 0 && NuSpecialExistsFn(&centre) != 0) {
        NUVEC *position = NuSpecialGetPos(&centre);
        memcpy(&RadialMoveCentre, position, sizeof(NUVEC));
    }
}

/* Anakin C door pairs; the table ends with an empty name. */
anakin_door_setup_s DoorSetupList[15] = {
    {"door1", "door1r", 15.0f, 1.0f, 0, {0}, {-1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
    {"door2", "door2r", 15.0f, 1.0f, 0, {0}, {1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
    {"door3", "door3r", 15.0f, 1.0f, 0, {0}, {1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
    {"door4", "door4r", 15.0f, 1.0f, 0, {0}, {-1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
    {"door5", "door5r", 15.0f, 1.0f, 0, {0}, {1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
    {"door6", "door6r", 15.0f, 1.0f, 0, {0}, {-1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
    {"door7", "door7r", 15.0f, 1.0f, 0, {0}, {1.0f, 1.0f, 0.0f}, -0.5f, 0.0f},
    {"door8", "door8r", 15.0f, 1.0f, 0, {0}, {0.0f, -1.0f, 0.0f}, -0.5f, 0.0f},
    {"door9", "door9r", 15.0f, 1.0f, 0, {0}, {0.0f, 1.0f, 0.0f}, -0.5f, 0.0f},
    {"door10", "door10r", 15.0f, 1.0f, 0, {0}, {0.0f, -1.0f, 0.0f}, -0.5f, 0.0f},
    {"door11", "door11r", 15.0f, 1.0f, 0, {0}, {1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
    {"door12", "door12r", 15.0f, 1.0f, 0, {0}, {-1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
    {"door15", "door15r", 15.0f, 1.0f, 0, {0}, {1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
    {"door16", "door16r", 15.0f, 1.0f, 0, {0}, {-1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
    {"", NULL, 0.0f, 0.0f, 0, {0}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f},
};

/* PC file records share the established 0x530-byte evaluator layout. */
typedef struct FLIGHT_SPLINE_FILE_PC {
    NUVEC4 points[64];
    i32 point_count;
    f32 current_value;
    f32 reset_value;
    f32 rate;
    f32 length;
    f32 cumulative_distances[64];
    i32 reset_state;
    i32 current_state;
    i32 previous_id;
    i32 order;
    i32 id;
    i32 enabled;
    u32 unknown_52c;
} FLIGHT_SPLINE_FILE_PC;
STATIC_ASSERT(sizeof(FLIGHT_SPLINE_FILE_PC) == sizeof(FLIGHT_SPLINE_RESOURCE_PC));
STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, point_count) == 0x400);
STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, reset_value) == 0x408);
STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, rate) == 0x40c);
STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, length) == 0x410);
STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, cumulative_distances) == 0x414);
STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, reset_state) == 0x514);
STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, previous_id) == 0x51c);
STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, order) == 0x520);
STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, id) == 0x524);
STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, enabled) == 0x528);
STATIC_ASSERT(offsetof(WORLDINFO, config_file) == 0x80);

#pragma data_seg(".data$FlightSplineExtensionPc")
char *FlightSplineExtensionPc = ".FSP";
#pragma data_seg(".data$FlightSplineDefaultIncrementPc")
f32 FlightSplineDefaultIncrementPc = 0.009999999776482582f;
#pragma data_seg()

void __cdecl FlightSplineCapacityDiagnosticPc(char *filename);

static __declspec(noinline) void FlightSplineLoadRecordPc(FLIGHT_SPLINE_FILE_PC *spline, i32 version, i32 index) {
    i32 point;
    spline->point_count = EdFileReadInt();
    spline->reset_value = EdFileReadFloat();
    spline->id = EdFileReadInt();
    spline->enabled = 1;
    if (version >= 2) {
        spline->rate = EdFileReadFloat();
        spline->reset_state = EdFileReadInt();
    } else {
        spline->reset_state = 0;
        spline->rate = 0.0f;
    }
    if (version >= 3) {
        spline->previous_id = EdFileReadInt();
        spline->order = EdFileReadInt();
    } else {
        spline->previous_id = -1;
        spline->order = index;
    }
    for (point = 0; point < spline->point_count; ++point) {
        spline->points[point].x = EdFileReadFloat();
        spline->points[point].y = EdFileReadFloat();
        spline->points[point].z = EdFileReadFloat();
        spline->points[point].w = EdFileReadFloat();
    }
}

static __declspec(noinline) f32 FlightSplineLegacyLengthPc(FLIGHT_SPLINE_RESOURCE_PC *spline) {
    f32 distance = 0.0f;
    f32 along = 1.0f;
    __declspec(align(16)) NUVEC4 current, previous;
    NUVEC difference;
    f32 squared;
    if (spline->point_count != 0) {
        CalcSplinePoint(spline, &current, along);
        do {
            previous = current;
            if (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA)
                along -= PodRaceSplineIncrementPc;
            else
                along -= FlightSplineDefaultIncrementPc;
            if (along < 0.0f)
                along = 0.0f;
            CalcSplinePoint(spline, &current, along);
            difference.x = current.x - previous.x;
            difference.y = current.y - previous.y;
            difference.z = current.z - previous.z;
            squared = (f32)((f64)difference.x * difference.x + (f64)difference.y * difference.y +
                            (f64)difference.z * difference.z);
            distance = (f32)((f64)NuFsqrt(squared) + distance);
        } while (along > 0.0f);
    }
    return distance;
}

static __declspec(noinline) f32 FlightSplineCumulativeLengthPc(FLIGHT_SPLINE_RESOURCE_PC *spline) {
    f32 distance = 0.0f;
    i32 point, sample;
    f32 point_value, along, squared;
    __declspec(align(16)) NUVEC4 previous, current;
    NUVEC difference;
    for (point = 0; point < spline->point_count; ++point) {
        point_value = (f32)point;
        along = (f32)((f64)point_value / spline->point_count);
        CalcSplinePoint(spline, &previous, along);
        for (sample = 1; sample <= 10; ++sample) {
            along = (f32)(((f64)sample / 10.0 + point_value) / spline->point_count);
            CalcSplinePoint(spline, &current, along);
            difference.x = current.x - previous.x;
            difference.y = current.y - previous.y;
            difference.z = current.z - previous.z;
            squared = (f32)((f64)difference.x * difference.x + (f64)difference.y * difference.y +
                            (f64)difference.z * difference.z);
            distance = (f32)((f64)NuFsqrt(squared) + distance);
            previous = current;
        }
        spline->cumulative_distances[point] = distance;
    }
    return distance;
}

void __cdecl FlightSpline_Init(WORLDINFO *world, FLIGHT_SPLINE_RESOURCE_PC *splines, i32 capacity) {
    char filename[256];
    i32 version, count, i;
    FLIGHT_SPLINE_FILE_PC *spline;
    sprintf(filename, "%s%s", world->config_file, FlightSplineExtensionPc);
    EdFileSetMedia(1);
    if (!EdFileOpen(filename, 0))
        return;
    version = EdFileReadInt();
    count = EdFileReadInt();
    if (count > capacity)
        FlightSplineCapacityDiagnosticPc(filename);
    for (i = 0; i < count; ++i)
        FlightSplineLoadRecordPc((FLIGHT_SPLINE_FILE_PC *)&splines[i], version, i);
    if (version > 3) {
        for (i = 0; i < count; ++i) {
            spline = (FLIGHT_SPLINE_FILE_PC *)&splines[i];
            spline->length = EdFileReadFloat();
            if (version > 4) {
                i32 point;
                for (point = 0; point < spline->point_count; ++point)
                    spline->cumulative_distances[point] = EdFileReadFloat();
            } else {
                spline->length = FlightSplineCumulativeLengthPc(&splines[i]);
            }
        }
    } else {
        for (i = 0; i < count; ++i)
            splines[i].length = FlightSplineLegacyLengthPc(&splines[i]);
    }
    for (; i < capacity; ++i)
        splines[i].point_count = 0;
    EdFileClose();
}
