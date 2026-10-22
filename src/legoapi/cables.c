#include "saga.h"
#include <math.h>

#include <string.h>

void InitCables(WORLDINFO *world) {
    memset(cables, 0, sizeof(cables));

    targetmomf = default_targetmomf;
    ownermomf = default_ownermomf;
    max_cable_length = default_max_cable_length;
    tow_length = default_tow_length;

    if (GUNSHIP_ADATA != NULL && world != NULL && world->area == GUNSHIP_ADATA) {
        targetmomf = 4.0f;
        max_cable_length = 10.0f;
        tow_length = 0.0f;
    }
}
struct CABLE_RECORD_PC {
    GAMEOBJECT *source;
    GAMEOBJECT *target;
    NUVEC points[16];
    NUVEC velocities[16];
    f32 segment_lengths[15];
    f32 max_length;
    f32 total_length;
    u8 unused_01cc[4];
    f32 slack;
    f32 pull_time;
    u8 wrap_indices[15];
    u8 point_count;
    u8 wrap_count;
    u8 unused_01e9;
    u16 flags;
};
STATIC_ASSERT(offsetof(CABLE_RECORD_PC, source) == 0x0);
STATIC_ASSERT(offsetof(CABLE_RECORD_PC, target) == 0x4);
STATIC_ASSERT(offsetof(CABLE_RECORD_PC, points) == 0x8);
STATIC_ASSERT(offsetof(CABLE_RECORD_PC, velocities) == 0xc8);
STATIC_ASSERT(offsetof(CABLE_RECORD_PC, segment_lengths) == 0x188);
STATIC_ASSERT(offsetof(CABLE_RECORD_PC, max_length) == 0x1c4);
STATIC_ASSERT(offsetof(CABLE_RECORD_PC, total_length) == 0x1c8);
STATIC_ASSERT(offsetof(CABLE_RECORD_PC, slack) == 0x1d0);
STATIC_ASSERT(offsetof(CABLE_RECORD_PC, pull_time) == 0x1d4);
STATIC_ASSERT(offsetof(CABLE_RECORD_PC, wrap_indices) == 0x1d8);
STATIC_ASSERT(offsetof(CABLE_RECORD_PC, point_count) == 0x1e7);
STATIC_ASSERT(offsetof(CABLE_RECORD_PC, wrap_count) == 0x1e8);
STATIC_ASSERT(offsetof(CABLE_RECORD_PC, flags) == 0x1ea);
STATIC_ASSERT(sizeof(CABLE_RECORD_PC) == 0x1ec);

// The retail cable code tests the character context flags as a 64-bit field.
typedef struct cable_context_info_pc_s {
    const char *name;
    i32 action;
    u64 flags;
} CABLE_CONTEXT_INFO_PC;
STATIC_ASSERT(sizeof(CABLE_CONTEXT_INFO_PC) == 0x10);
STATIC_ASSERT(offsetof(CABLE_CONTEXT_INFO_PC, flags) == 0x8);

extern const f64 GameObjectNearFloorNoFloorHeight;
i32 XZLinesIntersect(NUVEC *a, NUVEC *b, NUVEC *c, NUVEC *d, float *first, float *second);

// The retail unit carries a private copy of the safe divide used by the cable code.
static f32 CableFdiv(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f)
        return 0.0f;
    return a / b;
}
CABLE_RECORD_PC *__cdecl CreateCable(GAMEOBJECT *source, GAMEOBJECT *target, i32 sound) {
    i32 i;
    CABLE_RECORD_PC *cable;
    if (target == NULL || source == NULL)
        return NULL;
    for (i = 0; i < 8; ++i) {
        cable = ((CABLE_RECORD_PC *)cables) + i;
        if ((cable->flags & 1) == 0) {
            cable->flags |= 1;
            cable->source = source;
            cable->target = target;
            if (source == player || source == player2)
                GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
            if (sound)
                PlaySfx("TowCable_Fire", &source->collision_position);
            return cable;
        }
    }
    return NULL;
}

static void ReleaseCable(CABLE_RECORD_PC *cable, i32 snapped) {
    i32 i;
    cable->flags |= 4;
    if (cable->target != NULL) {
        cable->target->flags_ef4 &= ~0x20u;
        cable->target->flags_ef4 &= ~0x200u;
    }
    if (snapped)
        GameAudio_PlaySfxById(GetSfxId("TowCable_Snap"), cable->points, 1, 0);
    else
        GameAudio_PlaySfxById(GetSfxId("TowCable_Detach"), cable->points, 1, 0);
    if (cable->source != NULL) {
        if (cable->source == player || cable->source == player2) {
            GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
        }
    }
    if (cable->source != NULL && cable->source->cable == cable) {
        cable->source->cable = NULL;
        cable->source = NULL;
    }
    cable->target = NULL;
    for (i = 0; i < cable->point_count; ++i) {
        cable->velocities[i].y = 0.0f;
    }
}

// Topples a towed AT-ST once the cable has pulled for long enough.
static void CableAtstToppled(GAMEOBJECT *target) {
    GAMEOBJECT *object;
    i32 i;
    target->character_context = 0x3d;
    target->field_7aa = 0x85;
    target->flags_ef4 |= 0x800000;
    target->context_duration = AnimDuration((i16)target->id, 0x85, 0.0f, 0.0f, 1);
    target->context_timer = 0.0f;
    IncrementMinikitCounter(target);
    object = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        if ((object->flags_1fc_word & 0x1000) && object->dead == 0 && object->cable != NULL &&
            ((CABLE_RECORD_PC *)object->cable)->target == target)
            ReleaseCable((CABLE_RECORD_PC *)object->cable, 1);
    }
}

void __cdecl UpdateCables(void) {
    CABLE_RECORD_PC *cable = (CABLE_RECORD_PC *)cables;
    NUVEC closest;
    NUVEC delta;
    NUVEC path[16];
    NUVEC target_impulse;
    NUVEC source_impulse;
    f32 along0;
    f32 along1;
    i32 index;

    for (index = 0; index < 8; ++index, ++cable) {
        GAMEOBJECT *source;
        GAMEOBJECT *target;
        i32 flags = cable->flags;
        i32 release;
        i32 snapped;
        i32 topple;
        i32 count;
        i32 i;

        if (!(flags & 1))
            continue;
        if (flags & 4) {
            i32 grounded = 1;
            cable->total_length = 0.0f;
            for (i = 0; i < cable->point_count; ++i) {
                f32 ground = GameShadow(NULL, &cable->points[i], 5.0f, -1);
                if (ground == GameObjectNearFloorNoFloorHeight)
                    ground = cable->points[i].y;
                cable->velocities[i].x -= (cable->velocities[i].x * cable_damping) * FRAMETIME;
                cable->velocities[i].y -= FRAMETIME * cable_gravity;
                cable->velocities[i].z -= (cable_damping * cable->velocities[i].z) * FRAMETIME;
                cable->points[i].x += FRAMETIME * cable->velocities[i].x;
                cable->points[i].y += cable->velocities[i].y * FRAMETIME;
                cable->points[i].z += FRAMETIME * cable->velocities[i].z;
                if (ground > cable->points[i].y) {
                    cable->points[i].y = ground;
                    cable->velocities[i].x -= (cable->velocities[i].x * cable_ground_damping) * FRAMETIME;
                    cable->velocities[i].z -= (cable_ground_damping * cable->velocities[i].z) * FRAMETIME;
                } else {
                    grounded = 0;
                }
                if (i < cable->point_count - 1) {
                    f32 length = NuVecDist(&cable->points[i + 1], &cable->points[i], &delta);
                    cable->segment_lengths[i] = length;
                    cable->total_length = length + cable->total_length;
                }
            }
            if (grounded) {
                if (cable->source != NULL && cable->source->cable == cable)
                    cable->source->cable = NULL;
                memset(cable, 0, sizeof(*cable));
            }
            continue;
        }

        release = 0;
        topple = 0;
        snapped = 0;
        target = cable->target;
        if (target != NULL && target->floor_height < -100.0f)
            release = 1;
        source = cable->source;
        if (source == NULL || !(source->flags_1fc_word & 0x1000) || source->dead != 0 || (source->field_e20 & 0x20) ||
            (target != NULL && target->character_context == 0x17) ||
            ((flags & 2) && source->pad->bound_pad != NULL &&
             (source->pad->bound_pad->digital_buttons_pressed & GAMEPAD_SPECIAL))) {
            release = 1;
        } else {
            flags |= 2;
            cable->flags = (u16)flags;
            path[0] = source->collision_position;
            count = 1;
            if (target == NULL || !(target->flags_1fc_word & 0x1000) || target->dead != 0) {
                release = 1;
            } else {
                if (target->id == (u16)id_ATAT) {
                    i32 last;
                    NUVEC *last_position;
                    if (cable->wrap_count == 0) {
                        f32 nearest = AIPositionUnsetPc;
                        i32 selected = 0;
                        for (i = 0; i < 4; ++i) {
                            f32 distance = NuVecDistSqr(
                                &path[0], NUMTX_GET_ROW_VEC(&cable->target->locator_matrices[atat_locators[i]], 3),
                                &delta);
                            if (distance < nearest) {
                                nearest = distance;
                                selected = i;
                            }
                        }
                        cable->wrap_indices[cable->wrap_count++] = (u8)selected;
                    }
                    target = cable->target;
                    last = cable->wrap_indices[cable->wrap_count - 1];
                    last_position = NUMTX_GET_ROW_VEC(&target->locator_matrices[atat_locators[last]], 3);
                    if (cable->wrap_count > 1) {
                        i32 previous = cable->wrap_indices[cable->wrap_count - 2];
                        i32 next = (last + 1) % 4;
                        i32 direction;
                        NUVEC *next_position;
                        NUVEC *previous_position;
                        i32 angle;
                        i32 next_angle;
                        i32 turn;
                        if (next == previous) {
                            next = (last + 3) % 4;
                            direction = -1;
                        } else {
                            direction = 1;
                        }
                        next_position = NUMTX_GET_ROW_VEC(&target->locator_matrices[atat_locators[next]], 3);
                        previous_position = NUMTX_GET_ROW_VEC(&target->locator_matrices[atat_locators[previous]], 3);
                        NuVecSub(&delta, &path[0], last_position);
                        angle = NuAtan2D(delta.x, delta.z);
                        NuVecSub(&delta, next_position, last_position);
                        next_angle = NuAtan2D(delta.x, delta.z);
                        turn = NuAngSub(next_angle, angle) * direction;
                        if (cable->wrap_count < 15 && turn > 0) {
                            cable->wrap_indices[cable->wrap_count++] = (u8)next;
                            if (cable->wrap_count == 15) {
                                if (WORLD->current_level == LevelFixupSlotPc0087b208)
                                    DeactivatePlayer(cable->target, 60.0f, NULL);
                                else
                                    DeactivatePlayer(cable->target, 30.0f, NULL);
                                PlaySfx("AT_AT_FALL", &cable->target->collision_position);
                                release = 1;
                            }
                        } else {
                            i32 previous_angle;
                            NuVecSub(&delta, last_position, previous_position);
                            previous_angle = NuAtan2D(delta.x, delta.z);
                            if (NuAngSub(previous_angle, angle) * direction < 0) {
                                cable->wrap_indices[cable->wrap_count] = 0xff;
                                --cable->wrap_count;
                            }
                        }
                    } else {
                        i32 next = (last + 1) % 4;
                        NUVEC *next_position = NUMTX_GET_ROW_VEC(&target->locator_matrices[atat_locators[next]], 3);
                        NUVEC *opposite_position =
                            NUMTX_GET_ROW_VEC(&target->locator_matrices[atat_locators[(last + 2) % 4]], 3);
                        i32 previous = (last + 3) % 4;
                        NUVEC *previous_position =
                            NUMTX_GET_ROW_VEC(&target->locator_matrices[atat_locators[previous]], 3);
                        NUVEC *previous_opposite_position =
                            NUMTX_GET_ROW_VEC(&target->locator_matrices[atat_locators[(previous + 3) % 4]], 3);
                        if (XZLinesIntersect(&path[0], last_position, next_position, opposite_position, &along0,
                                             &along1)) {
                            cable->wrap_indices[cable->wrap_count++] = (u8)next;
                        } else if (XZLinesIntersect(&path[0], last_position, previous_position,
                                                    previous_opposite_position, &along0, &along1)) {
                            cable->wrap_indices[cable->wrap_count++] = (u8)previous;
                        }
                    }
                } else if (target->id == id_DRAGBOMB) {
                    target->character_context = 0x34;
                    cable->target->context_timer = 0.0f;
                }
                if (cable->wrap_count != 0) {
                    for (i = cable->wrap_count - 1; i >= 0; --i) {
                        i32 locator = cable->wrap_indices[i];
                        if (cable->target->character_model->points_of_interest[locator] != NULL)
                            path[count++] =
                                *NUMTX_GET_ROW_VEC(&cable->target->locator_matrices[atat_locators[locator]], 3);
                    }
                } else {
                    count = 2;
                    path[1] = cable->target->collision_position;
                }
                if (release == 0) {
                    if (!(cable->source->character_data->game_character->flags_090 & 0x400)) {
                        release = 1;
                    } else {
                        f32 distance_sq = NuVecDistSqr(&path[0], &path[1], &delta);
                        if (NewRayCastScaleYMask(&path[1], &delta, 0.0f, 1.0f, 0,
                                                 GetSurfaceInfoRuntimeMaskPc.word | 0x1f)) {
                            release = 1;
                            snapped = 1;
                        } else {
                            i32 obstruction = 0;
                            for (i = 0; i < HIGHGAMEOBJECT; ++i) {
                                GAMEOBJECT *object = &Obj[i];
                                f32 line_distance;
                                f32 combined;
                                if (!(object->flags_1fc_word & 1) || !(object->flags_1fc_word & 0x1000) ||
                                    object->dead != 0 || (object->field_e20 & 0x20) || (i8)object->flags_1fc_low < 0)
                                    continue;
                                if (WORLD->area != NULL && WORLD->area == HOTHBATTLE_ADATA &&
                                    object->id != (u16)id_ATAT && object->id != id_DRAGBOMB &&
                                    object->id != GameObjectRotationAtstId && object->id != (u16)id_ATST_LOWRES)
                                    continue;
                                if (object->cable != NULL &&
                                    (((CABLE_RECORD_PC *)object->cable)->target == cable->target ||
                                     ((CABLE_RECORD_PC *)object->cable)->target == cable->source))
                                    continue;
                                if (object == cable->source || object == cable->target)
                                    continue;
                                line_distance =
                                    NuLineToPointDistSqrEx(&path[0], &path[1], &object->collision_position, &closest);
                                combined = object->bounds_half_height + object->bounds_radius;
                                if (combined * combined > line_distance &&
                                    object->collision_position.y + object->bounds_half_height > closest.y &&
                                    closest.y > object->collision_position.y - object->bounds_half_height) {
                                    f32 dx = object->collision_position.x - closest.x;
                                    f32 dz = object->collision_position.z - closest.z;
                                    if (object->bounds_radius * object->bounds_radius > dx * dx + dz * dz) {
                                        obstruction = 1;
                                        break;
                                    }
                                }
                            }
                            if (obstruction) {
                                release = 1;
                                snapped = 1;
                            } else {
                                target = cable->target;
                                if (target != NULL && (target->flags_1fc_word & 0x1000)) {
                                    if (target->id != (u16)id_ATAT) {
                                        target->flags_ef4 |= 0x200;
                                        AIScriptSetBaseScriptStateByName(&cable->target->ai_packet.script_processor,
                                                                         "BeingTowed");
                                        cable->target->ai_packet.opponent = cable->source;
                                        FaceOpponent(cable->target, &cable->source->collision_position);
                                        target = cable->target;
                                        if ((target->flags_ef0 & 0x8000) &&
                                            !(((CABLE_CONTEXT_INFO_PC *)CInfo)[target->character_context].flags & 1))
                                            target->movement_facing_angle += 0x8000;
                                        cable->target->ai_packet.goal_speed_mode = 1;
                                        cable->target->flags_ef0 |= 0x8000;
                                    }
                                    if (distance_sq > tow_length * tow_length) {
                                        f32 strength = 1.0f;
                                        if (distance_sq > max_cable_length * max_cable_length)
                                            strength = distance_sq / (max_cable_length * max_cable_length);
                                        NuVecScale(&delta, &delta, FRAMETIME);
                                        NuVecScale(&target_impulse, &delta, strength * targetmomf);
                                        NuVecScale(&source_impulse, &delta, strength * ownermomf);
                                        if (cable->max_length == MoveSplineRunSpeedSentinel &&
                                            cable->target->id != (u16)id_ATAT) {
                                            target_impulse.y = 0.0f;
                                            NuVecAdd(&cable->target->velocity, &cable->target->velocity,
                                                     &target_impulse);
                                            if (cable->total_length > tow_length) {
                                                cable->pull_time += FRAMETIME;
                                                cable->target->flags_ef4 |= 0x20;
                                            } else {
                                                cable->pull_time = 0.0f;
                                            }
                                            if ((cable->target->id == GameObjectRotationAtstId ||
                                                 cable->target->id == (u16)id_ATST_LOWRES) &&
                                                cable->pull_time > 1.0f) {
                                                topple = 1;
                                                release = 1;
                                            }
                                        }
                                        NuVecSub(&cable->source->velocity, &cable->source->velocity, &source_impulse);
                                    }
                                }
                                if (release == 0) {
                                    cable->total_length = 0.0f;
                                    if (cable->max_length == MoveSplineRunSpeedSentinel) {
                                        cable->point_count = (u8)count;
                                        for (i = 0; i < count; ++i) {
                                            cable->points[i] = path[i];
                                            if (i < count - 1) {
                                                f32 length = NuVecDist(&path[i + 1], &path[i], &delta);
                                                cable->segment_lengths[i] = length;
                                                cable->total_length = length + cable->total_length;
                                            }
                                        }
                                    } else {
                                        f32 remaining;
                                        cable->points[0] = path[0];
                                        remaining = cable_speed * FRAMETIME + cable->max_length;
                                        cable->max_length = remaining;
                                        cable->point_count = 1;
                                        for (i = 0; i < count - 1; ++i) {
                                            f32 length = NuVecDist(&path[i + 1], &path[i], &delta);
                                            if (!(remaining >= length)) {
                                                cable->segment_lengths[i] = remaining;
                                                cable->total_length += remaining;
                                                NuVecScale(&delta, &delta, CableFdiv(remaining, length));
                                                NuVecAdd(&cable->points[cable->point_count],
                                                         &cable->points[cable->point_count - 1], &delta);
                                                ++cable->point_count;
                                                break;
                                            }
                                            cable->segment_lengths[i] = length;
                                            cable->total_length += length;
                                            cable->points[cable->point_count] = path[i + 1];
                                            remaining -= length;
                                            ++cable->point_count;
                                            if (i == count - 2) {
                                                cable->max_length = AIPositionUnsetPc;
                                                PlaySfx("TowCable_Latch", &path[i + 1]);
                                            }
                                        }
                                    }
                                    if (cable->source != NULL && cable->target != NULL) {
                                        for (i = 0; i < cable->point_count; ++i) {
                                            if (i != 0)
                                                cable->velocities[i] = cable->target->velocity;
                                            else
                                                cable->velocities[i] = cable->source->velocity;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        target = cable->target;
        if (release)
            ReleaseCable(cable, snapped);
        if (topple)
            CableAtstToppled(target);
    }
}

void __cdecl DrawCables(void) {
    CABLE_RECORD_PC *cable = (CABLE_RECORD_PC *)cables;
    NURND_VERTEX3D_PC vertices[2];
    NUVEC step;
    i32 index;

    nsegments_drawn = 0;
    vertices[0].colour = 0xff000000;
    vertices[1].colour = 0xff000000;
    for (index = 0; index < 8; ++index, ++cable) {
        i32 flags = cable->flags;
        f32 y_span;
        f32 distance_along;
        i32 segment;
        if (!(flags & 1))
            continue;
        y_span = cable->points[cable->point_count - 1].y - cable->points[0].y;
        distance_along = 0.0f;
        if (flags & 4) {
            cable->slack += cable_slack * FRAMETIME * slack_factor;
            if (cable->slack > cable_slack)
                cable->slack = cable_slack;
        } else if (cable->total_length >= tow_length) {
            cable->slack = 0.0f;
        } else {
            f32 fraction = cable->total_length / tow_length;
            if (fraction > 1.0f)
                fraction = 1.0f;
            cable->slack = (1.0f - fraction) * cable_slack;
        }
        if (cable->slack == 0.0f) {
            for (segment = 0; segment < cable->point_count - 1; ++segment) {
                f32 start_fraction = CableFdiv(distance_along, cable->total_length);
                f32 end_fraction;
                vertices[0].position = cable->points[segment];
                vertices[0].position.y = start_fraction * y_span + cable->points[0].y;
                distance_along += cable->segment_lengths[segment];
                end_fraction = CableFdiv(distance_along, cable->total_length);
                vertices[1].position = cable->points[segment + 1];
                vertices[1].position.y = y_span * end_fraction + cable->points[0].y;
                if (solid_cable != 0)
                    DrawRopeSingle(&vertices[0].position, &vertices[1].position, 1.0f, ropemtl, end_fraction,
                                   end_fraction, 10.0f, 5.0f);
                else
                    NuRndrLine3d(vertices, ZipUpLineMaterialPc, NULL);
            }
        } else {
            f32 subdivision_scale = cable->total_length * nsegments_per_unit;
            NUVEC endpoint = cable->points[0];
            f32 ground_start;
            f32 ground_end;
            endpoint.y = CableFdiv(distance_along, cable->total_length) * y_span + cable->points[0].y;
            ground_end = GameShadow(NULL, &endpoint, 5.0f, -1);
            if (ground_end == GameObjectNearFloorNoFloorHeight)
                ground_end = endpoint.y;
            for (segment = 0; segment < cable->point_count - 1; ++segment) {
                NUVEC previous;
                NUVEC cursor;
                f32 fraction;
                f32 subdivisions;
                f32 step_length;
                f32 local_distance;
                ground_start = ground_end;
                previous = endpoint;
                fraction = CableFdiv(distance_along + cable->segment_lengths[segment], cable->total_length);
                endpoint = cable->points[segment + 1];
                endpoint.y = fraction * y_span + cable->points[0].y;
                ground_end = GameShadow(NULL, &endpoint, 5.0f, -1);
                if (ground_end == GameObjectNearFloorNoFloorHeight)
                    ground_end = endpoint.y;
                NuVecSub(&step, &endpoint, &previous);
                subdivisions = CableFdiv(cable->segment_lengths[segment], cable->total_length) * subdivision_scale;
                step_length = CableFdiv(cable->segment_lengths[segment], (f32)ceil((f64)subdivisions));
                NuVecScale(&step, &step, CableFdiv(step_length, cable->segment_lengths[segment]));
                cursor = previous;
                for (local_distance = 0.0f; local_distance + 0.5f * step_length < cable->segment_lengths[segment];
                     local_distance += step_length) {
                    f32 start_sag;
                    f32 start_fraction;
                    f32 start_floor;
                    f32 phase;
                    f32 end_sag;
                    f32 end_fraction;
                    f32 end_floor;
                    vertices[0].position = cursor;
                    start_sag =
                        cable->slack *
                        NU_SIN_LUT((i32)(CableFdiv(distance_along + local_distance, cable->total_length) * 32768.0f));
                    vertices[0].position.y -= start_sag;
                    start_fraction = CableFdiv(local_distance, cable->segment_lengths[segment]);
                    start_floor = start_fraction * ground_end + (1.0f - start_fraction) * ground_start + 0.05f;
                    if (vertices[0].position.y < start_floor)
                        vertices[0].position.y = start_floor;
                    NuVecAdd(&cursor, &cursor, &step);
                    vertices[1].position = cursor;
                    phase = CableFdiv(distance_along + local_distance + step_length, cable->total_length);
                    if (phase > 1.0f)
                        phase = 1.0f;
                    end_sag = cable->slack * NU_SIN_LUT((i32)(phase * 32768.0f));
                    vertices[1].position.y -= end_sag;
                    end_fraction = CableFdiv(local_distance + step_length, cable->segment_lengths[segment]);
                    end_floor = end_fraction * ground_end + (1.0f - end_fraction) * ground_start + 0.05f;
                    if (vertices[1].position.y < end_floor)
                        vertices[1].position.y = end_floor;
                    if (solid_cable != 0)
                        DrawRopeSingle(&vertices[0].position, &vertices[1].position, 1.0f, ropemtl, end_sag, end_sag,
                                       10.0f, 5.0f);
                    else
                        NuRndrLine3d(vertices, ZipUpLineMaterialPc, NULL);
                    ++nsegments_drawn;
                }
                distance_along += cable->segment_lengths[segment];
            }
        }
    }
}

GAMEOBJECT *__cdecl CableTargetGameObject(GAMEOBJECT *source, NUVEC *position, f32 radius) {
    GAMEOBJECT *nearest = NULL, *candidate = Obj;
    f32 nearest_distance = 100000000.0f;
    f32 minimum_x = position->x - radius, maximum_x = position->x + radius;
    f32 minimum_z = position->z - radius, maximum_z = position->z + radius;
    f32 radius_squared = radius * radius;
    i32 i;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++candidate) {
        i8 context = candidate->character_context;
        NUVEC delta;
        f32 distance;
        if (candidate == source || !(candidate->flags_1fc_word & 0x1000) || candidate->dead || candidate == Player[0] ||
            candidate == Player[1] || (CInfo[context].flags & 0x8000) || context == 0x3b || context == 0x17 ||
            context == 0x41 || context == 0x3d)
            continue;
        if (candidate->id != id_DRAGBOMB && candidate->id != (u16)id_ATAT &&
            candidate->id != GameObjectRotationAtstId && candidate->id != (u16)id_ATST_LOWRES)
            continue;
        if (candidate->collision_position.x < minimum_x || candidate->collision_position.x > maximum_x ||
            candidate->collision_position.z < minimum_z || candidate->collision_position.z > maximum_z)
            continue;
        distance = NuVecDistSqr(&candidate->collision_position, position, &delta);
        if (candidate->id == id_DRAGBOMB)
            distance = (f32)((f64)distance * (f64)0.8f);
        else if (candidate->id == (u16)id_ATAT)
            distance = (f32)((f64)distance * (f64)0.9f);
        if (distance < radius_squared && distance < nearest_distance) {
            nearest_distance = distance;
            nearest = candidate;
        }
    }
    return nearest;
}
