#include <stdio.h>
#include "saga.h"

f32 __cdecl PodSprint_InStartCountdown(WORLDINFO *world) {
    if (world->current_level == PODSPRINTA_LDATA)
        return podsprint.speed;
    return 0.0f;
}

float PodSprint_RollMul(GAMEOBJECT *object) {
    i16 id = ((i16)(object->id));
    if (id == FreePlayId007f1a98Pc || id == VehicleBoltId7f1a2cPc || id == VehicleBoltId7f1a9cPc)
        return 0.6f;
    if (id == id_XWING || id == VehicleBoltId7f1a24Pc || id == VehicleBoltId7f19a4Pc || id == id_NEW_REPUBLIC_GUNSHIP)
        return 0.8f;
    return 1.0f;
}

void PodSprint_GetIAlongVals(NUGSPLINE *spline, i16 *out1, i16 *out2) {
    i32 i;
    for (i = 0; i < 6; i++) {
        if (spline != NULL && podsprint.ai[i].spline == spline) {
            i32 n = podsprint.ai_index;
            if (n == 0) {
                *out1 = 0;
                *out2 = podsprint.ai[i].vals[0];
            } else {
                if (n >= 5)
                    n = 5;
                *out1 = podsprint.ai[i].vals[n - 1];
                *out2 = podsprint.ai[i].vals[n];
            }
            return;
        }
    }
}

void MaulA_Reset(WORLDINFO *world) {
    nuhspecial_s *slots = LevHSpecial;
    NuSpecialSetVisibility(&slots[4], 0);
    NuSpecialSetVisibility(&slots[5], 0);
    NuSpecialSetVisibility(&slots[6], 0);
    NuSpecialSetVisibility(&slots[7], 0);
    Maul_obj = FindGameObject(id_DARTHMAUL, 1, 1, 0, 0);
}

void MaulF_Init(WORLDINFO *world) {
    struct nuhspecial_s *slots;
    MaulA_ai_message = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
    slots = LevHSpecial;
    NuSpecialFind(world->current_gscn, &slots[0], "throw_object1", 1);
    NuSpecialFind(world->current_gscn, &slots[1], "throw_object2", 1);
    NuSpecialFind(world->current_gscn, &slots[2], "throw_object3", 1);
}

void PodRaceCReset(WORLDINFO *world) {
    void *cs;
    PodRaceReset();
    switch (Lap) {
        case 1:
            podrace_section = 2;
            break;
        case 2:
            podrace_section = 5;
            break;
        case 3:
            podrace_section = 8;
            break;
        default:
            break;
    }
    PodPacemakerPc = 0;
    PodRaceSnipersReset();
    cs = CutScene_Find(world->cutscene_sys, "ep1_podrace_avalanche");
    if (cs != NULL) {
        if (Lap == 2)
            NewCutScene(cs, world->cutscene_sys, 0, 1);
        else if (Lap == 3)
            CutScene_SnapToEnd(cs);
    }
}

typedef struct racepod_resource_pc_s {
    NUMTX matrix;
    NUVEC4 previous_axis;
    NUVEC4 previous_position;
    NUVEC4 previous_displacement;
    i32 pitch, yaw, unknown_0078;
    f32 speed;
    FLIGHT_SPLINE_RESOURCE_PC *spline;
    f32 start;
    i16 model_id;
    i16 unknown_008a;
    f32 distance;
    GAMEOBJECT *object;
    void *next;
    u8 unknown_0098[8];
} RACEPOD_RESOURCE_PC;
STATIC_ASSERT(sizeof(RACEPOD_RESOURCE_PC) == 0xa0);
STATIC_ASSERT(offsetof(RACEPOD_RESOURCE_PC, pitch) == 0x70);
STATIC_ASSERT(offsetof(RACEPOD_RESOURCE_PC, yaw) == 0x74);
STATIC_ASSERT(offsetof(RACEPOD_RESOURCE_PC, speed) == 0x7c);
STATIC_ASSERT(offsetof(RACEPOD_RESOURCE_PC, spline) == 0x80);
STATIC_ASSERT(offsetof(RACEPOD_RESOURCE_PC, start) == 0x84);
STATIC_ASSERT(offsetof(RACEPOD_RESOURCE_PC, model_id) == 0x88);
STATIC_ASSERT(offsetof(RACEPOD_RESOURCE_PC, distance) == 0x8c);
STATIC_ASSERT(offsetof(RACEPOD_RESOURCE_PC, object) == 0x90);
STATIC_ASSERT(offsetof(RACEPOD_RESOURCE_PC, next) == 0x94);

typedef struct podrace_resource_pc_s {
    FLIGHT_SPLINE_RESOURCE_PC splines[32];
    RACEPOD_RESOURCE_PC lap_entries[16];
    f32 lap_countdown;
    f32 mushroom_timer;
    f32 lap_display;
    f32 previous_lap_display;
    f32 max_lap_time, lap_time_increment;
    i32 attempts_per_increment;
    i32 attempts;
    u8 flags;
    u8 unknown_b021[15];
} PODRACE_RESOURCE_PC;
STATIC_ASSERT(sizeof(PODRACE_RESOURCE_PC) == 0xb030);
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, lap_entries) == 0xa600);
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, mushroom_timer) == 0xb004);
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, previous_lap_display) == 0xb00c);
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, flags) == 0xb020);

static void PodMatrixCopyPrivatePc(NUMTX *destination, const NUMTX *source) {
    destination->m00 = source->m00;
    destination->m01 = source->m01;
    destination->m02 = source->m02;
    destination->m03 = source->m03;
    destination->m10 = source->m10;
    destination->m11 = source->m11;
    destination->m12 = source->m12;
    destination->m13 = source->m13;
    destination->m20 = source->m20;
    destination->m21 = source->m21;
    destination->m22 = source->m22;
    destination->m23 = source->m23;
    destination->m30 = source->m30;
    destination->m31 = source->m31;
    destination->m32 = source->m32;
    destination->m33 = source->m33;
}

STATIC_ASSERT(offsetof(SNIPER_s, prev) == 0);
STATIC_ASSERT(offsetof(SNIPER_s, pos) == 0xc);
STATIC_ASSERT(offsetof(SNIPER_s, fire_timer) == 0x18);
STATIC_ASSERT(offsetof(SNIPER_s, state) == 0x1c);

void __cdecl PodRaceSnipersReset(void) {
    PodRace_nsnipers = 0;
    if (Lap != 0) {
        char name[16];
        NUGSPLINE *spline;
        i32 point;
        sprintf(name, "Sniper%d", Lap);
        spline = NuSplineFind(WORLD->current_gscn, name);
        if (spline != NULL && spline->length > 0) {
            for (point = 0; point < spline->length && PodRace_nsnipers < 10; point += 2) {
                SNIPER_s *sniper = &PodRace_snipers[PodRace_nsnipers];
                sniper->prev = spline->pts[point];
                sniper->pos = spline->pts[point + 1];
                sniper->fire_timer = NuRandFloat() * PodRace_sniper_fire_time + (f64)PodRace_sniper_fire_time * 0.5;
                sniper->state = 0.0f;
                ++PodRace_nsnipers;
            }
        }
    }
}

void PodRaceSnipersUpdate(void) {
    i32 i;
    NUVEC direction;
    static i32 player_ix = 0;
    i32 bolttype = (WORLD->current_level == PODSPRINTA_LDATA) ? 0x29 : 0x28;
    BOLTTYPE_PC *type = BoltType_FindByID(bolttype, WORLD);
    i32 n = PodRace_nsnipers;
    if (n <= 0)
        return;
    for (i = 0; i < n; i++) {
        i32 j;
        SNIPER_s *s = &PodRace_snipers[i];
        i32 in_range = 0;
        for (j = 0; j < 2; ++j) {
            f32 dz;
            f32 dx;
            f32 dist2;
            GAMEOBJECT *target = Player[j];
            if (!(target != NULL && (target->flags_1fc_half & 0x1001) == 0x1001)) {
                continue;
            }
            dz = target->collision_position.z - s->pos.z;
            dx = target->collision_position.x - s->pos.x;
            dist2 = dx * dx + dz * dz;
            if (dist2 < PodRace_sniper_start_fire_radius * PodRace_sniper_start_fire_radius) {
                in_range = 1;
                if (dist2 < PodRace_sniper_fire_radius * PodRace_sniper_fire_radius)
                    s->state = PodRace_sniper_fire_range_time;
            }
        }
        s->fire_timer += FRAMETIME;
        if (in_range) {
            if (s->fire_timer >= PodRace_sniper_fire_time) {
                NUVEC aim;
                f32 distance;
                NUMTX mtx;
                s->fire_timer = 0.0f;
                if (s->state > 0.0f) {
                    GAMEOBJECT *target = player2;
                    if (target == NULL) {
                        player_ix = 0;
                    } else {
                        player_ix = player_ix == 0;
                    }
                    if (player_ix == 0)
                        target = player;
                    if (target == NULL || type == NULL)
                        continue;
                    CalculateInterceptVector(&s->prev, &target->collision_position, &target->velocity, type->field_10,
                                             &aim, NULL);
                    NuVecAdd(&aim, &aim, &s->prev);
                    s->state -= FRAMETIME;
                    if (s->state < 0.0f)
                        s->state = 0.0f;
                } else {
                    aim.x = 0.0f;
                    aim.y = 0.0f;
                    aim.z = NuRandFloat() * PodRace_sniper_fire_radius;
                    NuVecRotateY(&aim, &aim, NuRandInt());
                    NuVecAdd(&aim, &aim, &s->pos);
                }

                distance = NuVecDist(&aim, &s->prev, &direction);
                temp_yrot = NuAtan2D(direction.x, direction.z);
                temp_xrot = NuAtan2D(-direction.y, distance);
                // Retail uses an aligned matrix local in its realigned stack frame.
                NuMtxSetRotationX(&mtx, (u16)temp_xrot);
                NuMtxRotateY(&mtx, (u16)temp_yrot);
                Bolt_Add(NULL, &s->prev, &mtx, bolttype, 0);
            }
            n = PodRace_nsnipers;
        }
    }
}

static RACEPOD_RESOURCE_PC *RacePodAllocatePrivatePc(void) {
    PODRACE_RESOURCE_PC *race = (PODRACE_RESOURCE_PC *)PodRacePc;
    i32 slot;
    for (slot = 0; slot < 16; ++slot) {
        if (race->lap_entries[slot].next == NULL)
            return &race->lap_entries[slot];
    }
    return NULL;
}

static void RacePodAlignPrivatePc(RACEPOD_RESOURCE_PC *pod, NUVEC4 *direction, f32 blend, i32 alternate) {
    __declspec(align(16)) NUVEC local;
    __declspec(align(16)) NUVEC rotated_direction;
    __declspec(align(16)) NUVEC4 axis;
    i32 yaw, pitch, direction_yaw, direction_pitch;
    f32 roll;
    f64 max_roll, min_roll;
    i32 angle;
    NuVecInvMtxRotate(&local, (NUVEC *)direction, &pod->matrix);
    yaw = (i16)NuAtan2D(local.x, local.z);
    pod->yaw = yaw;
    NuVecRotateY(&local, &local, -yaw);
    pitch = (i16)NuAtan2D(local.y, local.z);
    pod->pitch = pitch;
    NuMtxPreRotateY(&pod->matrix, yaw);
    NuMtxPreRotateX(&pod->matrix, -pitch);
    if (alternate != 0) {
        NuMtxPreRotateZ(&pod->matrix, 0x200);
        return;
    }
    direction_yaw = (i16)NuAtan2D(direction->x, direction->z);
    NuVecRotateY(&rotated_direction, (NUVEC *)direction, -direction_yaw);
    direction_pitch = (i16)NuAtan2D(rotated_direction.y, rotated_direction.z);
    axis.x = 1.0f;
    axis.y = axis.z = axis.w = 0.0f;
    NuVecRotateZ((NUVEC *)&axis, (NUVEC *)&axis, (i16)(i32)pod->previous_position.w + 0x4000);
    NuVecRotateX((NUVEC *)&axis, (NUVEC *)&axis, -direction_pitch);
    NuVecRotateY((NUVEC *)&axis, (NUVEC *)&axis, direction_yaw);
    NuVecInvMtxRotate((NUVEC *)&axis, (NUVEC *)&axis, &pod->matrix);
    roll = (f32)(i16)NuAtan2D(axis.x, axis.y);
    max_roll = 24000.0 * FRAMETIME;
    if (roll > max_roll)
        roll = (f32)(i16)(i32)max_roll;
    min_roll = -24000.0 * FRAMETIME;
    if (roll < min_roll)
        roll = (f32)(i16)(i32)min_roll;
    if (blend > 2.0f)
        roll = 0.0f;
    else if (blend > 1.0f)
        roll = (f32)((2.0 - blend) * roll);
    angle = -(i16)(i32)roll;
    NuMtxPreRotateZ(&pod->matrix, angle);
}

i32 __cdecl Action_CreatePod(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char **params,
                             i32 param_count, i32 first_time, f32 ignored) {
    i32 spline_id = -1, pacemaker = 0, damaged = 0;
    i16 model_id = -1;
    f32 start = 0.0f, end = 1.0f, time_factor = 1.0f;
    i32 i, spline_index;
    PODRACE_RESOURCE_PC *race;
    FLIGHT_SPLINE_RESOURCE_PC *spline;
    RACEPOD_RESOURCE_PC *pod;
    f64 duration;
    f32 difference;
    __declspec(align(16)) NUVEC4 position, next_position, direction;
    GAMEOBJECT *object;
    (void)system;
    (void)process;
    (void)ignored;
    if (first_time == 0 || param_count <= 0)
        return 1;
    for (i = 0; i < param_count; ++i) {
        char *value = NuStrIStr(params[i], PodCreateSplineKeywordPc);
        if (value != NULL)
            spline_id = (i32)AIParamToFloat((AISCRIPTPROCESS_PC *)packet, value + 10);
        else if ((value = NuStrIStr(params[i], (char *)AITransferLiteral7576f8)) != NULL) {
            if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                u8 type = LevelCharacterTypeIDFn(value + 5);
                model_id = type;
                if (type != 0xff)
                    model_id = LevelCharacterGlobalIDFn(type);
            }
        } else if ((value = NuStrIStr(params[i], PodCreateStartKeywordPc)) != NULL)
            start = AIParamToFloat((AISCRIPTPROCESS_PC *)packet, value + 13);
        else if ((value = NuStrIStr(params[i], PodCreateEndKeywordPc)) != NULL)
            end = AIParamToFloat((AISCRIPTPROCESS_PC *)packet, value + 11);
        else if ((value = NuStrIStr(params[i], PodCreateTimeKeywordPc)) != NULL)
            time_factor = AIParamToFloat((AISCRIPTPROCESS_PC *)packet, value + 12);
        else if (NuStrICmp(params[i], PodCreatePacemakerKeywordPc) == 0)
            pacemaker = 1;
        else if (NuStrICmp(params[i], PodCreateDamagedKeywordPc) == 0)
            damaged = 1;
    }
    if ((u32)spline_id > 31 || model_id == -1)
        return 1;
    race = (PODRACE_RESOURCE_PC *)PodRacePc;
    spline = &race->splines[0];
    for (spline_index = 0; spline_index < 32; ++spline_index) {
        spline = &race->splines[spline_index];
        if (spline->id == spline_id)
            break;
    }
    if (start < 0.0f)
        start = 0.0f;
    else if (start > 1.0f)
        start = 1.0f;
    if (end < 0.0f)
        end = 0.0f;
    else if (end > 1.0f)
        end = 1.0f;
    if (spline == NULL || spline->point_count == 0 || start == end)
        return 1;
    pod = RacePodAllocatePrivatePc();
    if (pod == NULL)
        return 1;
    pod->spline = spline;
    pod->start = start;
    pod->model_id = model_id;
    pod->next = (void *)1;
    duration = (f64)time_factor * race->previous_lap_display;
    difference = end - start;
    pod->speed = duration > 0.0 ? (f32)((difference * (f64)spline->length) / duration) : 1.0f;
    CalcSplinePointFromDist(spline, &position, start * spline->length);
    CalcSplinePointFromDist(spline, &next_position, (start + PodRaceSplineIncrementPc) * spline->length);
    PodMatrixCopyPrivatePc(&pod->matrix, &ANI3_EVAL_IDENTITY_MATRIX);
    pod->matrix.m30 = position.x;
    pod->matrix.m31 = position.y;
    pod->matrix.m32 = position.z;
    pod->matrix.m33 = position.w;
    direction.x = next_position.x - position.x;
    direction.y = next_position.y - position.y;
    direction.z = next_position.z - position.z;
    direction.w = 0.0f;
    RacePodAlignPrivatePc(pod, &direction, 1.0f, 0);
    pod->previous_axis.x = pod->previous_axis.y = pod->previous_axis.z = 0.0f;
    pod->previous_axis.w = 1.0f;
    pod->previous_position = position;
    pod->distance = start;
    object = AddDynamicCreature((i32)pod->model_id, (NUVEC *)spline, 0, NULL, NULL, NULL, 1, NULL, NULL, 0, 0);
    pod->object = object;
    if (object == NULL)
        return 1;
    object->flags_ef4 |= 0x04000000;
    object->movement_mode = 5;
    if (pacemaker != 0) {
        PodPacemakerPc = object;
        object->flags_ef4 |= 0x20;
    }
    if (damaged != 0)
        object->flags_ef4 |= 0x08000000;
    return 1;
}

void KaminoE_Init(WORLDINFO *world) {
    void *g;
    kaminoe_netpacket = SetLevelHack(0x14);
    g = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "endblock_b08");
    if (g != NULL)
        LevForce = *(i32 *)g;
}

i32 JediBInitLocator(WORLDINFO *world, NUVEC *position, i32 angle, AILOCATOR_PC *locator, f32 radius) {
    f32 floor_y;
    if (netclient != 0) {
        return 0;
    }
    if (world->ai_sys != NULL) {
        i32 index;
        i32 count = world->ai_sys->antinode_count;
        AIANTINODE_PC *node = world->ai_sys->antinodes;
        for (index = 0; index < count; index++, node++) {
            if (node->radius != 0.0f) {
                f32 dx = position->x - node->position.x;
                f32 dz = position->z - node->position.z;
                f32 dist_sqr = dx * dx + dz * dz;
                f32 r = node->radius + 1.0f;
                if (dist_sqr < r * r) {
                    f32 scale = r / NuFsqrt(dist_sqr);
                    position->x = node->position.x + dx * scale;
                    position->z = node->position.z + dz * scale;
                    break;
                }
            }
        }
    }
    memset(locator, 0, sizeof(AILOCATOR_PC));
    if (!(position->x * position->x + position->z * position->z < radius * radius)) {
        return 0;
    }
    floor_y = GameShadow(NULL, position, 5.0f, 0);
    if (floor_y != 2000000.0f) {
        position->y = floor_y;
    }
    AISysGetPathPos(world->ai_sys, position, &locator->path_info, NULL, 0xff);
    if ((locator->path_info.flags & AIPATHINFO_FLAG_ON_PATH) == 0) {
        return 0;
    }
    locator->position = *position;
    locator->rotation = angle;
    return 1;
}

void FactoryG_Init(WORLDINFO *world) {
    struct gizmo_pc_s *g;
    if (netclient != 0)
        return;
    g = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force5");
    if (g != NULL)
        force_array[0] = (GIZFORCE_s *)g->object;
    g = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force6");
    if (g != NULL)
        force_array[1] = (GIZFORCE_s *)g->object;
    g = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force7");
    if (g != NULL)
        force_array[2] = (GIZFORCE_s *)g->object;
    g = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force8");
    if (g != NULL)
        force_array[3] = (GIZFORCE_s *)g->object;
}

STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, max_lap_time) == 0xb010);
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, lap_time_increment) == 0xb014);
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, attempts_per_increment) == 0xb018);

i32 __cdecl Action_SetLapTime(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char **params,
                              i32 param_count, i32 first_time, f32 ignored) {
    i32 i;
    PODRACE_RESOURCE_PC *race = (PODRACE_RESOURCE_PC *)PodRacePc;
    (void)system;
    (void)process;
    (void)ignored;
    if (first_time != 0 && race->previous_lap_display == 0.0f) {
        for (i = 0; i < param_count; ++i) {
            char *value = NuStrIStr(params[i], PodRaceMaxLapTimeKeywordPc);
            if (value != NULL)
                race->max_lap_time = AIParamToFloat((AISCRIPTPROCESS_PC *)packet, value + 11);
            else if ((value = NuStrIStr(params[i], PodRaceLapTimeKeywordPc)) != NULL)
                race->previous_lap_display = AIParamToFloat((AISCRIPTPROCESS_PC *)packet, value + 8);
            else if ((value = NuStrIStr(params[i], PodRaceAttemptsKeywordPc)) != NULL)
                race->attempts_per_increment = (i32)AIParamToFloat((AISCRIPTPROCESS_PC *)packet, value + 24);
            else if ((value = NuStrIStr(params[i], PodRaceIncrementKeywordPc)) != NULL)
                race->lap_time_increment = AIParamToFloat((AISCRIPTPROCESS_PC *)packet, value + 10);
        }
        if (race->max_lap_time == 0.0f)
            race->max_lap_time = race->previous_lap_display;
    }
    return 1;
}

i32 Action_Sebulba(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet, char **unused3, i32 unused4,
                   i32 first_time, float unused6) {
    struct gameobject_s *object;
    nuhspecial_s special;
    struct gameobject_s *target;
    struct SOCKSYS *sock;
    SOCKPOSITION_PC ahead;
    NUVEC goal;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;

    object = packet->owner->objptr;

    if (first_time != 0) {
        NuSpecialFind(vehicle_scene, &special, "mine", 1);
        object->field_ef9 |= 0x40;
        object->field_f02 |= 0x20;
        object->ai_packet.movement_stopped = 1;
        object->run_speed_override = 1.0f;
    }

    target = player;
    if (target != NULL && player2 != NULL) {
        float x2 = object->position.x - player2->position.x;
        float z2 = object->position.z - player2->position.z;
        float x1 = object->position.x - target->position.x;
        float z1 = object->position.z - target->position.z;
        if (x2 * x2 + z2 * z2 <= x1 * x1 + z1 * z1)
            target = player2;
    }

    sock = WORLD->sock_sys;
    ahead = ahead;
    MoveSockPosition(sock, &object->sock_position, sockstep, &ahead);
    goal = goal;
    if (target != NULL) {
        f32 wanted;
        f32 speed;
        NUVEC target_offset;
        NUVEC object_offset;
        float target_distance = MidDistanceFromSockStart(sock, &target->sock_position);
        float object_distance = MidDistanceFromSockStart(sock, &object->sock_position);
        float difference = target_distance - object_distance;
        (void)MidDistanceFromSockStart(sock, &object->sock_position);
        (void)MidDistanceFromSockStart(sock, &target->sock_position);
        wanted = ((difference + 25.0f) / test_factor + 1.0f) * avg_currentspeed_mul;
        speed = SeekLinearF(object->run_speed_override, wanted, test_seek_rate);
        object->run_speed_override = speed;
        if (speed > max_speed_mul)
            object->run_speed_override = max_speed_mul;
        else if (speed < min_speed_mul)
            object->run_speed_override = min_speed_mul;

        target_offset = target_offset, object_offset = object_offset;
        NuVecSub(&target_offset, &target->position, &target->sock_position.midpoint);
        NuVecRotateY(&target_offset, &target_offset, -target->sock_heading);
        NuVecSub(&object_offset, &object->position, &object->sock_position.midpoint);
        NuVecRotateY(&object_offset, &object_offset, -object->sock_heading);
        goal.x = object_offset.x * sebulba_lerpf + target_offset.x * (1.0f - sebulba_lerpf);
        goal.z = 0.0f;
        goal.y = 0.0f;
        NuVecAdd(&goal, &goal, &ahead.midpoint);

        packet->script_processor.action_timer += FRAMETIME;
        if (packet->script_processor.action_timer > 1.0f) {
            if (do_mine != 0) {
                STUBBED_NAMED("CreatePodRaceMine");
            } else if (NuSpecialExistsFn(&special) != 0) {
                NUVEC target_delta;
                NUVEC throw_velocity;
                NUMTX matrix;
                ADDPART_PC part;
                NUVEC origin = object->position;
                origin.y += 1.0f;
                target_delta = target_delta;
                NuVecSub(&target_delta, &target->velocity, &target->collision_position);
                throw_velocity = throw_velocity;
                MakeThrowVector(&throw_velocity, &origin, &target->collision_position, &target_delta, 20.0f, -10.0f);
                NuVecAdd(&throw_velocity, &throw_velocity, &object->velocity);
                matrix = matrix;
                NuMtxSetTranslation(&matrix, &origin);
                part = Default_ADDPART;
                part.matrix = &matrix;
                part.velocity = &throw_velocity;
                part.special = &special;
                part.field_14 = 0.1f;
                part.field_18 = 0.1f;
                part.gravity = -10.0f;
                part.part_id = 0x29b;
                part.collision_fn = PartCollide_3D;
                part.time_step = FRAMETIME;
                AddPart(&part);
                packet->script_processor.action_timer = 0.0f;
            }
        }
    } else {
        goal = ahead.midpoint;
        object->run_speed_override = 1.0f;
    }

    AIMoveInstruction(packet, &goal, 0.0f, NULL, 1, 0.0f);
    return 0;
}

#include <math.h>

typedef struct pod_race_countdown_reader_pc_s {
    u8 unknown_0000[0xb000];
    f32 lap_countdown;
} POD_RACE_COUNTDOWN_READER_PC;
STATIC_ASSERT(offsetof(POD_RACE_COUNTDOWN_READER_PC, lap_countdown) == 0xb000);
extern f32 pod_animtime[2];
typedef struct pod_merge_packet_reader_pc_s {
    u8 unknown_00[0x10];
    f32 blend_source_time;
    f32 blend_target_time;
    u8 unknown_18[0x22];
    i16 requested_animation;
    u8 unknown_3c[6];
    i16 overlay_animation;
    f32 overlay_weight;
} POD_MERGE_PACKET_READER_PC;
STATIC_ASSERT(offsetof(POD_MERGE_PACKET_READER_PC, requested_animation) == 0x3a);
STATIC_ASSERT(offsetof(POD_MERGE_PACKET_READER_PC, blend_source_time) == 0x10);
STATIC_ASSERT(offsetof(POD_MERGE_PACKET_READER_PC, blend_target_time) == 0x14);
STATIC_ASSERT(offsetof(POD_MERGE_PACKET_READER_PC, overlay_animation) == 0x42);
STATIC_ASSERT(offsetof(POD_MERGE_PACKET_READER_PC, overlay_weight) == 0x44);

i32 __cdecl PodRace_InStartCountdown(WORLDINFO *world) {
    POD_RACE_COUNTDOWN_READER_PC *race = (POD_RACE_COUNTDOWN_READER_PC *)PodRacePc;
    return world->area != NULL && world->area == PODRACE_ADATA && race != NULL && race->lap_countdown > 0.0f;
}

void __cdecl SetPodMergeAnims(ANIMPACKET_PC *packet, i32 index) {
    POD_MERGE_PACKET_READER_PC *reader = (POD_MERGE_PACKET_READER_PC *)packet;
    f32 roll = pod_roll[index];
    reader->requested_animation = 1;
    reader->overlay_animation = roll < 0.0f ? 0x26 : 0x4f;
    reader->blend_source_time = pod_animtime[index];
    reader->blend_target_time = pod_animtime[index];
    reader->overlay_weight = (f32)fabs((f64)roll);
}

/* Episode I level callbacks installed by FixUpLevels (retail 0x4d82f0 .. 0x4dd770). */
typedef struct podrace_levflag_pc_s {
    u8 podrace_state;
    u8 mushroom_state;
    u8 remaining[14];
} PODRACE_LEVFLAG_PC;
STATIC_ASSERT(sizeof(PODRACE_LEVFLAG_PC) == 16);

typedef struct gungan_a_pc_s {
    i16 unknown_000;
    i16 count;
    AILOCATOR_PC *origins[32];
    AILOCATOR_PC *targets[32];
    f32 spawn_timer;
    i16 model_index;
    i16 unknown_10a;
    i16 models[4];
} GUNGAN_A_PC;
STATIC_ASSERT(sizeof(GUNGAN_A_PC) == 0x114);
STATIC_ASSERT(offsetof(GUNGAN_A_PC, targets) == 0x84);
STATIC_ASSERT(offsetof(GUNGAN_A_PC, spawn_timer) == 0x104);
STATIC_ASSERT(offsetof(GUNGAN_A_PC, model_index) == 0x108);
STATIC_ASSERT(offsetof(GUNGAN_A_PC, models) == 0x10c);

typedef struct mine_entry_pc_s {
    u32 active;
    NUVEC pos;
    f32 grow;
    f32 grow_alpha;
    u16 rotx;
    u16 roty;
} MINE_ENTRY_PC;
STATIC_ASSERT(sizeof(MINE_ENTRY_PC) == 0x1c);

typedef struct minesys_pc_s {
    NUSPECIALHANDLE mine_special;
    MINE_ENTRY_PC mines[64];
    f32 mine_radius;
    f32 spawn_timer;
    AIAREA_PC *nomine_areas[10];
    i16 nomine_count;
    i16 mine_debris;
    i32 mine_part;
    f32 update_timer;
} MINESYS_PC;
STATIC_ASSERT(sizeof(MINESYS_PC) == 0x748);
STATIC_ASSERT(offsetof(MINESYS_PC, mines) == 0xc);
STATIC_ASSERT(offsetof(MINESYS_PC, mine_radius) == 0x70c);
STATIC_ASSERT(offsetof(MINESYS_PC, spawn_timer) == 0x710);
STATIC_ASSERT(offsetof(MINESYS_PC, nomine_count) == 0x73c);
STATIC_ASSERT(offsetof(MINESYS_PC, mine_part) == 0x740);
STATIC_ASSERT(offsetof(MINESYS_PC, update_timer) == 0x744);

typedef struct client_mines_pc_s {
    NUVEC positions[64];
    u32 present_words[2];
    u32 exploded_words[2];
    u32 unknown_310[2];
} CLIENT_MINES_PC;
STATIC_ASSERT(sizeof(CLIENT_MINES_PC) == 0x318);
STATIC_ASSERT(offsetof(CLIENT_MINES_PC, present_words) == 0x300);

typedef struct retakeg_netpacket_pc_s {
    i16 guard_a;
    i16 guard_b;
} RETAKEG_NETPACKET_PC;

typedef struct cut_instance_reader_pc_s {
    u8 opaque_00_88[0x88];
    u32 flags_88;
    u32 unknown_8c;
    f32 current_frame;
} CUT_INSTANCE_READER_PC;
STATIC_ASSERT(offsetof(CUT_INSTANCE_READER_PC, flags_88) == 0x88);
STATIC_ASSERT(offsetof(CUT_INSTANCE_READER_PC, current_frame) == 0x90);

f32 gungan_a_time = 0.5f;
extern i32 PodRaceKeyPc[8];
MINESYS_PC minesys = {0};
CLIENT_MINES_PC client_mines = {0};
i32 pod_mines_bitfield[2] = {0};
i32 clients_mines_bitfield[2] = {0};
i32 mine_count = 0;
void *podrace_netpacket = NULL;
static i32 podhurry_i = 0;
GUNGAN_A_PC gungan_a = {0};
RETAKEG_NETPACKET_PC *retakeg_netpacket = NULL;
static i32 mushroom_n_attempts = 0;
static CUTINFO *mushroom0_cut = NULL;
static GIZAIMESSAGE *RetakeG_TotalGuards_msg = NULL;
static GIZAIMESSAGE *RetakeG_GuardsToRescue_msg = NULL;
static GIZAIMESSAGE *MaulA_hits_message = NULL;
extern i16 CharacterFixupIdPc007f19ac;
extern i16 CharacterFixupIdPc007f19b0;
extern i16 CharacterFixupIdPc007f19b4;

void __cdecl UpdatePodRaceMines(void);
void __cdecl PodRaceUpdate(WORLDINFO *world, f32 dt);
void __cdecl UpdatePacemakerDisplay(WORLDINFO *world);
void __cdecl UpdatePodRaceLapDisplay(f32 dt);
void __cdecl PodRaceAUpdate(void);
void __cdecl PodRaceADraw(WORLDINFO *world);
void __cdecl PodRaceSnipersUpdate(void);
void __cdecl ChrisAllocLevelStuff(WORLDINFO *world);
void __cdecl FlightSpline_Init(WORLDINFO *world, FLIGHT_SPLINE_RESOURCE_PC *splines, i32 capacity);

void __cdecl GunganA_Init(WORLDINFO *world) {
    char name[32];
    i32 i;
    if (netclient == 0) {
        memset(&gungan_a, 0, sizeof(gungan_a));
        gungan_a.models[0] = CharacterFixupIdPc007f19ac;
        gungan_a.models[1] = CharacterFixupIdPc007f19b0;
        gungan_a.models[2] = CharacterFixupIdPc007f19b4;
        gungan_a.models[3] = CharacterFixupIdPc007f19b0;
        for (i = 0; i < 32; i++) {
            sprintf(name, "origin_%d", i);
            gungan_a.origins[i] = AIPathFindLocator(world->ai_sys, name);
            sprintf(name, "target_%d", i);
            gungan_a.targets[i] = AIPathFindLocator(world->ai_sys, name);
            if (gungan_a.origins[i] == NULL || gungan_a.targets[i] == NULL)
                break;
            gungan_a.count++;
        }
    }
}

void __cdecl GunganA_Update(WORLDINFO *world) {
    GAMEOBJECT *object;
    i32 r;
    if (netclient != 0)
        return;
    gungan_a.spawn_timer += FRAMETIME;
    if (gungan_a.count != 0 && gungan_a.spawn_timer > gungan_a_time) {
        r = NuRand(NULL) % gungan_a.count;
        if (active_neutral_count < 4) {
            AILOCATOR_PC *origin;
            i32 model;
            gungan_a.model_index = (i16)((gungan_a.model_index + 1) % 4);
            model = gungan_a.models[gungan_a.model_index];
            origin = gungan_a.origins[r];
            object = AddDynamicCreature(model, &origin->position, origin->rotation, "Wildlife", &origin->path_info,
                                        NULL, 0, NULL, NULL, 0, 0);
        } else {
            i32 pick = NuRand(NULL);
            if ((pick & 1) != 0) {
                AILOCATOR_PC *origin = gungan_a.origins[r];
                object = AddDynamicCreature(id_STAP, &origin->position, origin->rotation, "STAP", &origin->path_info,
                                            NULL, 0, NULL, NULL, 0, 0);
            } else {
                AILOCATOR_PC *origin = gungan_a.origins[r];
                object = AddDynamicCreature(id_BATTLEDROID, &origin->position, origin->rotation, "Battledroid",
                                            &origin->path_info, NULL, 0, NULL, NULL, 0, 0);
            }
        }
        if (object != NULL)
            object->ai_packet.locator = gungan_a.targets[r];
        gungan_a.spawn_timer = 0.0f;
    }
}

static void InitPodRaceMines(void) {
    char name[32];
    i32 i;
    memset(&minesys, 0, sizeof(minesys));
    memset(&client_mines, 0, sizeof(client_mines));
    if (NuSpecialFind(vehicle_scene, &minesys.mine_special, "mine", 1) != 0) {
        minesys.mine_radius = NuSpecialGetOriginRadius(&minesys.mine_special);
        for (i = 0; i < 10; i++) {
            sprintf(name, "nomine_%d", i);
            minesys.nomine_areas[minesys.nomine_count] = AISysFindArea(WORLD->ai_sys, name);
            minesys.nomine_count++;
        }
        minesys.mine_debris = (i16)FindGameDebris(WORLD->game_debris_sys, "MINE_POP");
        minesys.mine_part = PARTLookupType("POD_MINE_PART");
    }
}

static void PodRaceResetMinesPc(void) {
    clients_mines_bitfield[0] = 0;
    clients_mines_bitfield[1] = 0;
    pod_mines_bitfield[0] = 0;
    pod_mines_bitfield[1] = 0;
    memset(minesys.mines, 0, sizeof(minesys.mines));
    memset(&client_mines, 0, sizeof(client_mines));
    minesys.spawn_timer = AIPositionUnsetPc;
    mine_count = 0;
    minesys.update_timer = 0.0f;
}

void __cdecl ResetPodStuff(void) {
    f32 duration;
    i32 i;
    i32 id = apicharsys->playermodelids[id_ANAKINSPOD];
    if (id != -1 && apicharsys->models[id].model_data_b[1] != NULL)
        duration = *(f32 *)apicharsys->models[id].model_data_b[1];
    else
        duration = 0.0f;
    for (i = 0; i < 2; i++) {
        pod_roll_target[i] = 0.0f;
        pod_roll[i] = 0.0f;
        pod_animtime[i] = 1.0f;
        if (duration > 0.0f)
            pod_animtime[i] = (f32)(qrand() * PlayerRandomScalePc * (duration - 1.0) + pod_animtime[i]);
    }
}

void __cdecl PodRaceInit(WORLDINFO *world) {
    PODRACE_RESOURCE_PC *podrace;
    i32 id;
    i32 i;
    podrace_netpacket = SetLevelHack(0x14);
    if (world->level_specific_data == NULL)
        ChrisAllocLevelStuff(world);
    podrace = (PODRACE_RESOURCE_PC *)world->level_specific_data;
    PodRacePc = podrace;
    memset(podrace, 0, sizeof(*podrace));
    if (netclient == 0)
        FlightSpline_Init(world, podrace->splines, 32);
    InitPodRaceMines();
    for (i = 0; i < 8; i++)
        PodRaceKeyPc[i] = -1;
    ResetPodStuff();
    id = apicharsys->playermodelids[id_ANAKINSPOD];
    if (id != -1) {
        CHARACTER_MODEL_LAYERS_PC *entry = &apicharsys->models[id];
        if (entry->model_data_b[1] != NULL)
            podanimendframe = AnimEndFrame(entry, 1);
    }
}

void __cdecl PodRaceReset(void) {
    PODRACE_RESOURCE_PC *podrace = (PODRACE_RESOURCE_PC *)PodRacePc;
    i32 i;
    podrace->mushroom_timer = 0.0f;
    memset(podrace->lap_entries, 0, sizeof(podrace->lap_entries));
    podrace->flags &= 0xfc;
    podhurry_i = -1;
    for (i = 0; i < 8; i++)
        PodRaceKeyPc[i] = -1;
}

void __cdecl PodRaceAInit(WORLDINFO *world) {
    PodRaceInit(world);
}

void __cdecl PodRaceAReset(WORLDINFO *world) {
    PodRaceReset();
    switch (Lap) {
        case 1:
            podrace_section = 3;
            break;
        case 2:
            podrace_section = 6;
            break;
        case 3:
            podrace_section = 9;
            break;
        default:
            break;
    }
    PodPacemakerPc = NULL;
    PodRaceResetMinesPc();
    if (Lap == 3 && nethost == 0 && netclient == 0)
        NewCutScene(NULL, world->cutscene_sys, "ep1_podrace_sebulba", 1);
}

void __cdecl PodRaceBInit(WORLDINFO *world) {
    PodRaceInit(world);
    mushroom_collapse = 0;
    mushroom_nattempts_per_increment = 1;
    mushroom_n_attempts = 0;
    if (Lap <= 1)
        ((PODRACE_RESOURCE_PC *)PodRacePc)->lap_countdown = 3.0f;
}

void __cdecl PodRaceBReset(WORLDINFO *world) {
    PodRaceReset();
    switch (Lap) {
        case 1:
            podrace_section = 1;
            break;
        case 2:
            podrace_section = 4;
            break;
        case 3:
            podrace_section = 7;
            break;
        default:
            break;
    }
    PodPacemakerPc = NULL;
    ((PODRACE_LEVFLAG_PC *)LevFlag)->podrace_state = 0;
    ((PODRACE_LEVFLAG_PC *)LevFlag)->mushroom_state = 0;
    mushroom0_cut = CutScene_Find(world->cutscene_sys, "EP1_PODRACE_MUSHROOM0");
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "collapsing_mush", 1);
}

void __cdecl PodRaceCInit(WORLDINFO *world) {
    char name[256];
    i32 i;
    PodRaceInit(world);
    ((PODRACE_LEVFLAG_PC *)LevFlag)->podrace_state = 0;
    for (i = 0; i < 10; i++) {
        sprintf(name, "boost0%i", i + 1);
        NuSpecialFind(world->current_gscn, &LevHSpecial[i], name, 1);
    }
    if (FreePlay != 0 || (LevelProgressData[world->current_level->index].unknown_2800[0] & 1) != 0) {
        for (i = 0; i < 10; i++) {
            if (NuSpecialExistsFn(&LevHSpecial[i]) != 0)
                NuSpecialSetVisibility(&LevHSpecial[i], 1);
        }
        ((PODRACE_LEVFLAG_PC *)LevFlag)->podrace_state = 2;
    } else {
        for (i = 0; i < 17; i++) {
            if (NuSpecialExistsFn(&LevHSpecial[i]) != 0)
                NuSpecialSetVisibility(&LevHSpecial[i], 0);
        }
    }
}

void __cdecl PodRaceAUpdatePc(WORLDINFO *world) {
    STUBBED_NAMED("UpdatePacemakerDisplay");
    STUBBED_NAMED("PodRaceUpdate");
    if (netclient == 0)
        STUBBED_NAMED("PodRaceAUpdate");
    STUBBED_NAMED("UpdatePodRaceMines");
}

void __cdecl PodRaceA_AlwaysUpdate(WORLDINFO *world) {
    STUBBED_NAMED("UpdatePodRaceLapDisplay");
    if (Lap == 3) {
        if (InStory() != 0) {
            if (LevelFixupSlotPc0087b0b8 != NULL) {
                other_level_override = LevelFixupSlotPc0087b0b8->index;
                return;
            }
        }
        if (LevelFixupSlotPc0087b0bc != NULL)
            other_level_override = LevelFixupSlotPc0087b0bc->index;
    }
}

void __cdecl PodRaceADrawPc(WORLDINFO *world) {
    STUBBED_NAMED("PodRaceADraw");
}

void __cdecl PodRaceCUpdate(WORLDINFO *world) {
    i32 i;
    STUBBED_NAMED("UpdatePacemakerDisplay");
    STUBBED_NAMED("UpdatePodRaceLapDisplay");
    STUBBED_NAMED("PodRaceUpdate");
    PodRaceSnipersUpdate();
    if (((PODRACE_LEVFLAG_PC *)LevFlag)->podrace_state == 0) {
        CUTINFO *cutscene = CutScene_Find(world->cutscene_sys, "Ep1_Podrace_TuskenRaiders");
        if (cutscene != NULL && (((CUT_INSTANCE_READER_PC *)cutscene->instance)->flags_88 & 0x1000) != 0)
            ((PODRACE_LEVFLAG_PC *)LevFlag)->podrace_state = 1;
    }
    if (((PODRACE_LEVFLAG_PC *)LevFlag)->podrace_state == 1) {
        for (i = 0; i < 10; i++) {
            if (NuSpecialExistsFn(&LevHSpecial[i]) != 0)
                NuSpecialSetVisibility(&LevHSpecial[i], 1);
        }
        ((PODRACE_LEVFLAG_PC *)LevFlag)->podrace_state = 2;
    }
}

void __cdecl RetakeG_Init(WORLDINFO *world) {
    retakeg_netpacket = SetLevelHack(4);
    RetakeG_TotalGuards_msg = CheckGizAIMessage(gizaimessagesys, "TotalGuards", NULL);
    RetakeG_GuardsToRescue_msg = CheckGizAIMessage(gizaimessagesys, "GuardsToRescue", NULL);
}

void __cdecl RetakeG_Reset(WORLDINFO *world) {
}

void __cdecl RetakeG_Update(WORLDINFO *world) {
    if (netclient == 0) {
        retakeg_netpacket->guard_b = (i16)RetakeG_GuardsToRescue_msg->value;
        retakeg_netpacket->guard_a = (i16)RetakeG_TotalGuards_msg->value;
    } else {
        RetakeG_GuardsToRescue_msg->value = (f32)retakeg_netpacket->guard_b;
        RetakeG_TotalGuards_msg->value = (f32)retakeg_netpacket->guard_a;
    }
}

void __cdecl RetakeG_Panel(WORLDINFO *world) {
    i16 ids[6] = {id_ROYALGUARD, id_ROYALGUARD, id_ROYALGUARD, id_ROYALGUARD, id_ROYALGUARD, id_ROYALGUARD};
    u8 alive[6] = {1, 1, 1, 1, 1, 1};
    i32 i;
    if (RetakeG_TotalGuards_msg != NULL && RetakeG_TotalGuards_msg->value > 0.0f) {
        i32 total = (i32)RetakeG_TotalGuards_msg->value;
        if (total > 6)
            total = 6;
        if (RetakeG_GuardsToRescue_msg != NULL && RetakeG_GuardsToRescue_msg->value > 0.0f) {
            i32 rescue = (i32)RetakeG_GuardsToRescue_msg->value;
            if (rescue > 6)
                rescue = 6;
            for (i = 0; i < rescue; i++)
                alive[i] = 0;
            DrawMeleeTargetsAlphaPc(ids, alive, NULL, total);
        }
    }
}

void __cdecl MaulA_Init(WORLDINFO *world) {
    MaulA_ai_message = CheckGizAIMessage(gizaimessagesys, "MaulOnTheRun", NULL);
    MaulA_hits_message = CheckGizAIMessage(gizaimessagesys, "Hits", NULL);
    NuSpecialFind(world->current_gscn, &LevHSpecial[4], "engine_1c", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[5], "engine_2c", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[6], "engine_1d", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[7], "engine_2d", 1);
}

void __cdecl MaulA_Update(WORLDINFO *world) {
}

void __cdecl MaulA_Panel(WORLDINFO *world) {
    if (netclient == 0) {
        if (Maul_obj != NULL && MaulA_ai_message != NULL && MaulA_ai_message->value == 0.0f &&
            MaulA_hits_message != NULL) {
            GAMEOBJECT *maul = Maul_obj;
            maul->current_hp = (u8)(i32)MaulA_hits_message->value;
            maul->field_107e = 3;
            DrawBossHitPoints(maul);
        } else {
            DrawBossHitPoints(NULL);
        }
    }
}

void __cdecl MaulD_Init(WORLDINFO *world) {
}

void __cdecl MaulD_Update(WORLDINFO *world) {
}

void __cdecl MaulE_Init(WORLDINFO *world) {
}

void __cdecl MaulE_Update(WORLDINFO *world) {
}

void __cdecl MaulF_Reset(WORLDINFO *world) {
    Maul_obj = FindGameObject(id_DARTHMAUL, 1, 1, 0, 0);
}

void __cdecl MaulF_Update(WORLDINFO *world) {
}

void __cdecl MaulF_Panel(WORLDINFO *world) {
    if (netclient == 0) {
        if (Maul_obj != NULL && MaulA_ai_message != NULL && MaulA_ai_message->value == 1.0f)
            DrawBossHitPoints(Maul_obj);
        else
            DrawBossHitPoints(NULL);
    }
}

void __cdecl AnakinsFlightB_Init(WORLDINFO *world) {
    i32 count;
    gunship_bolts[0] = (i16)BoltType_FindIDByName("trooper_red", world);
    gunship_flags[0] = 0;
    gunship_flags[1] = 0;
    gunship_flags[2] = 0;
    count = NuSpecialFind(world->current_gscn, &LevHSpecial[0], "minifig_1_1", 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[1], "minifig_1_2", 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[2], "minifig_1_3", 1);
    if (count == 3)
        hothtroopers = LevHSpecial;
}

void __cdecl AnakinsFlightB_Update(WORLDINFO *world) {
}

void __cdecl AnakinsFlightB_Draw(WORLDINFO *world) {
}

/* Empty level debug hook shared with the Episode II callbacks. */
void __cdecl KaminoEGizmoMissingPc(char *name) {
}
