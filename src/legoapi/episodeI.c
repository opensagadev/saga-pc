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
            if (podsprint.ai_index == 0) {
                *out1 = 0;
                *out2 = podsprint.ai[i].vals[0];
            } else {
                i32 n = podsprint.ai_index;
                if (n >= 5)
                    n = 5;
                *out1 = podsprint.ai[i].vals[n - 1];
                *out2 = podsprint.ai[i].vals[n];
            }
            return;
        }
    }
}

extern f32 sebulba_goal_ahead_vals[4];
extern f32 sebulba_fallback_lerpf;
extern f32 sebulba_fallback_dist;
extern f32 sebulba_catchup_lerpf;
extern f32 sebulba_catchup_speed_mul;
extern f32 sebulba_catchup_dist;
extern f32 sebulba_fallback_speed_mul;

i32 __cdecl Action_NewSebulba(AISYS_PC *sys, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char **params,
                              i32 param_count, i32 first_time, f32 delta_time) {
    GAMEOBJECT *object;
    f32 goal;
    f32 base_speed;
    f32 distance;
    f32 ratio;
    f32 lerp;
    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL)
        return 1;
    object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
    if (first_time != 0) {
        object->run_speed_override = object->character_data->game_character->run_speed;
        object->movement_spline_offset.y = 0.0f;
    }
    if (podsprint.speed > 0.0f) {
        object->run_speed_override = 0.0f;
    } else if ((i8)podsprint.ai_state < 3) {
        goal = podsprint.ai_index < 4 ? sebulba_goal_ahead_vals[podsprint.ai_index] : sebulba_goal_ahead_vals[3];
        base_speed = object->character_data->game_character->run_speed;
        if (object->sock_position.sock_index != -1 &&
            object->sock_position.sock_index == player->sock_position.sock_index) {
            SOCKSYS_PC *sock_sys = WORLD->sock_sys;
            SOCK_PC *sock = &sock_sys->socks[object->sock_position.sock_index];
            if (player2 != NULL) {
                GAMEOBJECT *lead;
                distance = MidDistanceFromSockStart(sock_sys, &player2->sock_position) -
                           MidDistanceFromSockStart(WORLD->sock_sys, &player->sock_position);
                if (distance > sock->unknown_98 * 0.5)
                    distance -= sock->unknown_98;
                else if (distance < sock->unknown_98 * -0.5)
                    distance += sock->unknown_98;
                if (goal < 0.0f)
                    lead = distance > 0.0f ? player : player2;
                else
                    lead = distance > 0.0f ? player2 : player;
                distance = MidDistanceFromSockStart(WORLD->sock_sys, &object->sock_position) -
                           MidDistanceFromSockStart(WORLD->sock_sys, &lead->sock_position);
            } else {
                distance = MidDistanceFromSockStart(sock_sys, &object->sock_position) -
                           MidDistanceFromSockStart(WORLD->sock_sys, &player->sock_position);
            }
            if (distance > sock->unknown_98 * 0.5)
                distance -= sock->unknown_98;
            else if (distance < sock->unknown_98 * -0.5)
                distance += sock->unknown_98;
            ratio = distance - goal;
            if (ratio > 0.0f) {
                distance = sebulba_fallback_dist - goal;
                ratio = ratio / distance;
                if (ratio > 1.0f)
                    ratio = 1.0f;
                ratio = sebulba_fallback_speed_mul * ratio + (1.0f - ratio);
                lerp = sebulba_fallback_lerpf;
                goto seek;
            }
        } else {
            ratio = 0.0f;
        }
        ratio = ratio / sebulba_catchup_dist;
        if (ratio > 1.0f)
            ratio = 1.0f;
        ratio = sebulba_catchup_speed_mul * ratio + (1.0f - ratio);
        lerp = sebulba_catchup_lerpf;
    seek:
        object->run_speed_override = SeekValF(object->run_speed_override, ratio * base_speed, lerp);
    } else {
        base_speed = object->character_data->game_character->run_speed;
        if (podsprint.max_speed_msg != NULL && podsprint.min_speed_msg != NULL && podsprint.speed_step_msg != NULL) {
            base_speed =
                podsprint.max_speed_msg->value - (f32)(*(i16 *)podsprint.unknown_8c) * podsprint.speed_step_msg->value;
            if (base_speed < podsprint.min_speed_msg->value)
                base_speed = podsprint.min_speed_msg->value;
        }
        object->run_speed_override = SeekValF(object->run_speed_override, base_speed, 5.0f);
    }
    if (object->movement_spline_position.reached_end != 0 && (podsprint.flags & 4) == 0) {
        if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0)
            LoseCoins((GAMEOBJECT *)Player[0], 1);
        if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0)
            LoseCoins((GAMEOBJECT *)Player[1], 1);
        *(u16 *)&podsprint.flags |= 4;
        ResetLevel(WORLD, "Ep1_Podsprint_OutOfTime", 1);
        ++*(i16 *)podsprint.unknown_8c;
    }
    return 0;
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
    GIZAIMESSAGE *message;
    NUGSCN *scene;
    message = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
    scene = world->current_gscn;
    MaulA_ai_message = message;
    slots = LevHSpecial;
    NuSpecialFind(scene, &slots[0], "throw_object1", 1);
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
        switch (Lap) {
            case 2:
                NewCutScene(cs, world->cutscene_sys, 0, 1);
                break;
            case 3:
                CutScene_SnapToEnd(cs);
                break;
        }
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

/* Original episodeI.cpp private owner of the active PodRace block (PC 0x00882a8c). */
static PODRACE_RESOURCE_PC *PodRacePc = NULL;

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
            if (!(target != NULL && (target->flags_1fc_word & 1) != 0 && (target->flags_1fc_word & 0x1000) != 0)) {
                continue;
            }
            dx = target->collision_position.x - s->pos.x;
            dz = target->collision_position.z - s->pos.z;
            dist2 = dx * dx + dz * dz;
            if (PodRace_sniper_start_fire_radius * PodRace_sniper_start_fire_radius > dist2) {
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
                temp_zrot = NuAtan2D(-direction.y, distance);
                // Retail uses an aligned matrix local in its realigned stack frame.
                NuMtxSetRotationX(&mtx, (u16)temp_zrot);
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
                    f64 scale = (f64)r / NuFsqrt(dist_sqr);
                    dx *= scale;
                    position->x = node->position.x + dx;
                    dz *= scale;
                    position->z = node->position.z + dz;
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
    (void)system;
    (void)process;
    (void)ignored;
    if (first_time != 0 && PodRacePc->previous_lap_display == 0.0f) {
        for (i = 0; i < param_count; ++i) {
            char *value = NuStrIStr(params[i], PodRaceMaxLapTimeKeywordPc);
            if (value != NULL)
                PodRacePc->max_lap_time = AIParamToFloat((AISCRIPTPROCESS_PC *)packet, value + 11);
            else if ((value = NuStrIStr(params[i], PodRaceLapTimeKeywordPc)) != NULL)
                PodRacePc->previous_lap_display = AIParamToFloat((AISCRIPTPROCESS_PC *)packet, value + 8);
            else if ((value = NuStrIStr(params[i], PodRaceAttemptsKeywordPc)) != NULL)
                PodRacePc->attempts_per_increment = (i32)AIParamToFloat((AISCRIPTPROCESS_PC *)packet, value + 24);
            else if ((value = NuStrIStr(params[i], PodRaceIncrementKeywordPc)) != NULL)
                PodRacePc->lap_time_increment = AIParamToFloat((AISCRIPTPROCESS_PC *)packet, value + 10);
        }
        if (PodRacePc->max_lap_time == 0.0f)
            PodRacePc->max_lap_time = PodRacePc->previous_lap_display;
    }
    return 1;
}

static void *CreatePodRaceMinePc(NUVEC *pos);

extern NUSPECIALHANDLE SebulbaMineSpecialPc;
extern f32 SebulbaDistanceBiasPc, SebulbaThrowIntervalPc;
static f32 PodSebulbaThrowSpeedPc, PodSebulbaThrowGravityPc;

i32 Action_Sebulba(AISYS_PC *unused0, AISCRIPTPROCESS_PC *unused1, AIPACKET_PC *packet, char **unused3, i32 unused4,
                   i32 first_time, float unused6) {
    struct gameobject_s *object;
    struct gameobject_s *target;
    SOCKSYS_PC *sock;
    SOCKPOSITION_PC ahead;
    NUVEC goal;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;

    object = packet->owner->objptr;

    if (first_time != 0) {
        NuSpecialFind(vehicle_scene, &SebulbaMineSpecialPc, "mine", 1);
        object->portal_flags |= 0x4000;
        object->flags_ef4 |= 0x200000;
        object->ai_packet.movement_stopped = 1;
        object->movement_speed_scale_pc = 1.0f;
    }

    target = player;
    if (target != NULL && player2 != NULL) {
        f32 x = object->position.x - target->position.x;
        f32 z = object->position.z - target->position.z;
        f32 distance1 = x * x + z * z;
        f32 distance2;
        x = object->position.x - player2->position.x;
        z = object->position.z - player2->position.z;
        distance2 = x * x + z * z;
        if (distance1 < distance2)
            target = player;
        else
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
        f64 target_distance = MidDistanceFromSockStart(WORLD->sock_sys, &target->sock_position);
        f64 object_distance = MidDistanceFromSockStart(WORLD->sock_sys, &object->sock_position);
        float difference = target_distance - object_distance;
        difference += SebulbaDistanceBiasPc;
        (void)MidDistanceFromSockStart(WORLD->sock_sys, &object->sock_position);
        (void)MidDistanceFromSockStart(WORLD->sock_sys, &target->sock_position);
        wanted = (difference / test_factor + 1.0f) * avg_currentspeed_mul;
        speed = SeekLinearF(object->movement_speed_scale_pc, wanted, test_seek_rate);
        object->movement_speed_scale_pc = speed;
        if (speed > max_speed_mul)
            object->movement_speed_scale_pc = max_speed_mul;
        else if (speed < min_speed_mul)
            object->movement_speed_scale_pc = min_speed_mul;

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
        if (packet->script_processor.action_timer > SebulbaThrowIntervalPc) {
            if (do_mine != 0) {
                if (CreatePodRaceMinePc(&object->lower_position) != NULL)
                    packet->script_processor.action_timer = 0.0f;
            } else if (NuSpecialExistsFn(&SebulbaMineSpecialPc) != 0) {
                NUVEC target_delta;
                NUVEC throw_velocity;
                NUMTX matrix;
                ADDPART_PC part;
                NUVEC origin = object->position;
                origin.y += 1.0f;
                target_delta = target_delta;
                NuVecSub(&target_delta, &target->velocity, &target->collision_position);
                throw_velocity = throw_velocity;
                MakeThrowVector(&throw_velocity, &origin, &target->collision_position, &target_delta,
                                PodSebulbaThrowSpeedPc, PodSebulbaThrowGravityPc);
                NuVecAdd(&throw_velocity, &throw_velocity, &object->velocity);
                matrix = matrix;
                NuMtxSetTranslation(&matrix, &origin);
                part = Default_ADDPART;
                part.matrix = &matrix;
                part.velocity = &throw_velocity;
                part.special = &SebulbaMineSpecialPc;
                part.field_14 = 0.1f;
                part.field_18 = 0.1f;
                part.gravity = PodSebulbaThrowGravityPc;
                part.flags = 0x29b;
                part.collision_fn = PartCollide_3D;
                part.time_step = FRAMETIME;
                AddPart(&part);
                packet->script_processor.action_timer = 0.0f;
            }
        }
    } else {
        goal = ahead.midpoint;
        object->movement_speed_scale_pc = 1.0f;
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
    union {
        f32 value;
        u32 bits;
    } value, absolute;
    if (roll < 0.0f) {
        reader->requested_animation = 1;
        reader->overlay_animation = 0x26;
    } else {
        reader->requested_animation = 1;
        reader->overlay_animation = 0x4f;
    }
    reader->blend_source_time = pod_animtime[index];
    reader->blend_target_time = pod_animtime[index];
    value.value = roll;
    absolute.bits = value.bits & 0x7fffffff;
    reader->overlay_weight = absolute.value;
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
        PodRacePc->lap_countdown = 3.0f;
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
    UpdatePacemakerDisplay(world);
    PodRaceUpdate(world, FRAMETIME);
    if (netclient == 0)
        PodRaceAUpdate();
    UpdatePodRaceMines();
}

void __cdecl PodRaceA_AlwaysUpdate(WORLDINFO *world) {
    UpdatePodRaceLapDisplay(FRAMETIME);
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
    PodRaceADraw(world);
}

void __cdecl PodRaceCUpdate(WORLDINFO *world) {
    i32 i;
    UpdatePacemakerDisplay(world);
    UpdatePodRaceLapDisplay(FRAMETIME);
    PodRaceUpdate(world, FRAMETIME);
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

/* PodRace_A mine spawner and renderer. */
i32 __cdecl QuickNewRayCast(NUVEC *position, NUVEC *movement, f32 radius, i32 scan_flags, f32 max_distance, f32 step);
static f32 PodMineSockAlongPc[4] = {0.0f, 1.5f, 0.75f, 1.5f};
static f32 PodMineSpreadPc = 30.0f;
static f32 PodMineAheadPc = 100.0f;
static f32 PodMineMaxSockPc = 100.0f;
static f32 PodMineAreaScalePc = 1.0f;
static f32 PodMineHeightScalePc = 0.5f;
static u32 mine_max_tile = 0x2000;

static void *CreatePodRaceMinePc(NUVEC *pos) {
    NUVEC mine_pos;
    NUVEC ray_pos;
    NUVEC delta;
    MINE_ENTRY_PC *entry;
    f32 distance;
    f32 y;
    i32 i;
    i32 bit;
    y = GameShadow(NULL, pos, 5.0f, -1);
    if (y == 2000000.0)
        y = pos->y;
    mine_pos.x = pos->x;
    mine_pos.y = PodMineHeightScalePc * minesys.mine_radius + y;
    mine_pos.z = pos->z;
    for (i = 0; i < minesys.nomine_count; i++) {
        if (CheckPosAIArea(minesys.nomine_areas[i], &mine_pos, PodMineAreaScalePc * minesys.mine_radius) != 0)
            return NULL;
    }
    distance = NuVecXZDist(&mine_pos, &player->collision_position, &delta);
    if (distance > 0.0f) {
        NuVecScale(&delta, &delta, 1.0f / distance);
        ray_pos = player->collision_position;
        ray_pos.y = (f32)(ray_pos.y + 100.0);
        if (QuickNewRayCast(&ray_pos, &delta, 0.0f, 0, PodMineAreaScalePc * minesys.mine_radius + distance, 5.0f) != 0)
            return NULL;
    }
    entry = minesys.mines;
    for (i = 0; i < 64; i++, entry++) {
        if (entry->active == 0) {
            entry->active = 1;
            entry->pos = mine_pos;
            entry->grow_alpha = 0.0f;
            if (mine_max_tile != 0)
                entry->rotx = (u16)(NuRandInt() % mine_max_tile);
            entry->roty = (u16)NuRandInt();
            bit = 1 << i;
            *(u64 *)pod_mines_bitfield |= (u64)(i64)bit;
            *(u64 *)client_mines.present_words &= (u64)(i64)~bit;
            mine_count++;
            return entry;
        }
    }
    return NULL;
}

void __cdecl PodRaceADraw(WORLDINFO *world) {
    NUMTX mtx;
    NUVEC scale;
    i32 i;
    if (netclient == 0) {
        if (NuSpecialExistsFn(&minesys.mine_special) != 0) {
            MINE_ENTRY_PC *entry = minesys.mines;
            for (i = 0; i < 64; i++, entry++) {
                if (entry->active != 0) {
                    scale.x = scale.y = scale.z = entry->grow_alpha;
                    NuMtxSetScale(&mtx, &scale);
                    NuMtxTranslate(&mtx, &entry->pos);
                    NuMtxPreRotateX(&mtx, entry->rotx);
                    NuMtxPreRotateY(&mtx, entry->roty);
                    NuSpecialDrawAt(&minesys.mine_special, &mtx);
                }
            }
        }
    } else {
        for (i = 0; i < 64; i++) {
            if ((*(u64 *)client_mines.present_words & (u64)(i64)(1 << i)) != 0) {
                NuMtxSetIdentity(&mtx);
                NuMtxTranslate(&mtx, &client_mines.positions[i]);
                NuSpecialDrawAt(&minesys.mine_special, &mtx);
            }
        }
    }
}

void __cdecl PodRaceAUpdate(void) {
    NUVEC vec;
    if (Lap < 4 && MiniCutCam == 0) {
        minesys.update_timer += FRAMETIME;
        if (minesys.update_timer > 1.0f) {
            if (minesys.spawn_timer == 1000000000.0)
                minesys.spawn_timer = GameCam->sock_position.distance;
            if (GameCam->sock_position.distance < PodMineMaxSockPc &&
                GameCam->sock_position.distance >= minesys.spawn_timer) {
                vec.x = (f32)((0.5 - NuRandFloat()) * (PodMineSpreadPc * 2.0f));
                vec.y = 0.0f;
                vec.z = PodMineAheadPc;
                NuVecRotateY(&vec, &vec, player->sock_heading);
                NuVecAdd(&vec, &vec, &player->collision_position);
                if (CreatePodRaceMinePc(&vec) != NULL)
                    minesys.spawn_timer = PodMineSockAlongPc[Lap] + GameCam->sock_position.distance;
            }
        }
    }
}

static f32 pod_pacemaker_alpha = 0.0f;
static f32 PodPacemakerTextScalePc = 0.08f;

void __cdecl UpdatePacemakerDisplay(WORLDINFO *world) {
    NUVEC position;
    GAME_MESSAGE_PC *message;
    if (PodPacemakerPc == NULL)
        return;
    if (FadeSys.fade == 0.0f && pause_rndr_on == 0) {
        pod_pacemaker_alpha += FRAMETIME + FRAMETIME;
        if (pod_pacemaker_alpha > 1.0f)
            pod_pacemaker_alpha = 1.0f;
        if ((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.2f) < (f64)0.1f) {
            position.x = PodPacemakerPc->upper_position.x;
            position.y = (f32)(PodPacemakerPc->upper_position.y + 0.75);
            position.z = PodPacemakerPc->upper_position.z;
            message =
                AddGameMessage(" ", &position, PodPacemakerTextScalePc, NULL, 0.0f, 0xff, 0x3f, 0x3f, 0x10083, 0.0f);
            if (message != NULL) {
                message->alpha = (u8)(NuTrigTable[(i32)(pod_pacemaker_alpha * 16384.0) >> 1 & 0x7fff] * 128.0);
                message->icon = 0x134;
                if (world->level_objects[0x134].active != 0)
                    message->special = world->level_objects[0x134].special;
            }
        }
    } else {
        pod_pacemaker_alpha = 0.0f;
    }
}

static f32 podhurryalpha = 0.0f;
static f32 podstartracealpha = 0.0f;
static f32 podlapalpha = 0.0f;

void __cdecl UpdatePodRaceLapDisplay(f32 dt) {
    f32 target;
    i32 oldhurry;
    if (FadeSys.fade == 0.0f && MiniCutCam == 0 && CUTSTOPGAME == 0) {
        target = (Paused != 0 || (WORLD->current_level == PODRACEB_LDATA && GameTimer.time_elapsed >= 1.0f &&
                                  GameTimer.time_elapsed < 6.0f))
                     ? 1.0f
                     : 0.0f;
        podlapalpha = SeekLinearF(podlapalpha, target, dt + dt);
    } else {
        podlapalpha = 0.0f;
        podhurryalpha = 0.0f;
        podstartracealpha = 0.0f;
    }
    oldhurry = podhurry_i;
    if (Paused == 0 && PodRacePc != NULL && PodRacePc->lap_display < 10.0f) {
        podhurry_i = (i32)PodRacePc->lap_display;
        if (podhurryalpha < 1.0f) {
            podhurryalpha += dt + dt;
            if (podhurryalpha > 1.0f)
                podhurryalpha = 1.0f;
        }
        if (podhurry_i > 0 && podhurry_i != -1 && podhurry_i != oldhurry)
            TickTockSfxPc();
    } else {
        podhurryalpha = 0.0f;
    }
    if (Paused != 0)
        return;
    if (PodRacePc != NULL && PodRacePc->lap_countdown > 0.0f && podstartracealpha < 1.0f) {
        podstartracealpha += dt + dt;
        if (podstartracealpha > 1.0f)
            podstartracealpha = 1.0f;
    }
}

void __cdecl PodLoseSpeed(GAMEOBJECT *object, i32 hit, i32 rumble);
static f32 PodMineGrowTimePc = 1.0f;

static void PodRaceExplodeMinePc(MINE_ENTRY_PC *entry, GAMEOBJECT *object) {
    u64 mask;
    if (minesys.mine_debris != -1)
        AddGameDebris(WORLD->game_debris_sys, minesys.mine_debris, &entry->pos);
    if (minesys.mine_part != -1)
        AddFiniteShotPART(minesys.mine_part, &entry->pos, 1);
    GameCam_HitJudder();
    GameCam_NewShake(NULL, 0.75f, 1.0f, 1.0f);
    PlaySfx("Explode1", &object->collision_position);
    PodLoseSpeed(object, 1, 1);
    mask = (u64)(i64) ~(1 << (i32)(entry - minesys.mines));
    *(u64 *)pod_mines_bitfield &= mask;
    memset(entry, 0, sizeof(*entry));
    mine_count--;
}

static void PodRaceExplodeClientMinePc(NUVEC *position, GAMEOBJECT *object) {
    if (minesys.mine_debris != -1)
        AddGameDebris(WORLD->game_debris_sys, minesys.mine_debris, position);
    if (minesys.mine_part != -1)
        AddFiniteShotPART(minesys.mine_part, position, 1);
    GameCam_HitJudder();
    GameCam_NewShake(NULL, 0.75f, 1.0f, 1.0f);
    PlaySfx("Explode1", &object->collision_position);
}

void __cdecl UpdatePodRaceMines(void) {
    GAMEOBJECT *objects[64];
    NUVEC delta;
    MINE_ENTRY_PC *entry;
    GAMEOBJECT *object;
    i32 count;
    i32 i;
    i32 j;
    f32 radius;
    f32 reach;
    f32 distance;
    count = 0;
    object = Obj;
    for (i = 0; i < 64; i++, object++) {
        if (object != NULL && (object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 &&
            object != PodPacemakerPc)
            objects[count++] = object;
    }
    if (netclient == 0) {
        entry = minesys.mines;
        for (i = 0; i < 64; i++, entry++) {
            if (entry->active == 0)
                continue;
            NuVecSub(&delta, &entry->pos, &GameCam->position);
            if (GameCam->dir.x * delta.x + GameCam->dir.y * delta.y + GameCam->dir.z * delta.z < 0.0) {
                *(u64 *)pod_mines_bitfield &= (u64)(i64) ~(1 << (i32)(entry - minesys.mines));
                memset(entry, 0, sizeof(*entry));
                mine_count--;
                continue;
            }
            entry->grow_alpha = (entry->grow += FRAMETIME) * PodMineAreaScalePc / PodMineGrowTimePc;
            if (entry->grow_alpha > PodMineAreaScalePc)
                entry->grow_alpha = PodMineAreaScalePc;
            radius = entry->grow_alpha * minesys.mine_radius;
            for (j = 0; j < count; j++) {
                object = objects[j];
                if (object == NULL)
                    continue;
                delta.x = object->collision_position.x - entry->pos.x;
                delta.z = object->collision_position.z - entry->pos.z;
                reach = object->bounds_radius + radius;
                distance = delta.x * delta.x + delta.z * delta.z;
                if (distance < reach * reach) {
                    if (object->field_280 == 0xff) {
                        KillPlayer(object, 2, 1, NULL);
                        objects[j] = NULL;
                    } else {
                        PodRaceExplodeMinePc(entry, object);
                        if (object->dead != 0)
                            objects[j] = NULL;
                    }
                }
            }
        }
    } else {
        for (i = 0; i < 64; i++) {
            u64 mask = (u64)(i64)(1 << i);
            if ((mask & *(u64 *)client_mines.present_words) == 0 || (mask & *(u64 *)client_mines.exploded_words) != 0)
                continue;
            radius = minesys.mine_radius * PodMineAreaScalePc;
            for (j = 0; j < count; j++) {
                object = objects[j];
                if (object == NULL)
                    continue;
                delta.x = object->collision_position.x - client_mines.positions[i].x;
                delta.z = object->collision_position.z - client_mines.positions[i].z;
                reach = object->bounds_radius + radius;
                distance = delta.x * delta.x + delta.z * delta.z;
                if (distance < reach * reach) {
                    PodRaceExplodeClientMinePc(&client_mines.positions[i], object);
                    *(u64 *)client_mines.exploded_words |= mask;
                }
            }
        }
    }
}

typedef struct podrace_netpacket_reader_pc_s {
    f32 start_countdown;
    f32 remaining_time;
    f32 available_time;
    u32 unknown_0c;
    i32 lap;
} PODRACE_NETPACKET_READER_PC;
STATIC_ASSERT(offsetof(PODRACE_NETPACKET_READER_PC, lap) == 0x10);

static f32 PodSebulbaThrowTimePc = 2.0f;
static f32 PodSebulbaThrowJitterPc = 1.0f;
static f32 PodSebulbaThrowSpeedPc = 20.0f;
static f32 PodSebulbaThrowGravityPc = -10.0f;
static f32 PodSebulbaFarThrowPc = 10.0f;
static f32 PodSebulbaThrowSpreadPc = 3.0f;
static f32 PodSebulbaThrowAheadPc = 3.0f;

static void PodRaceMineKillPc(PART_PC *part, i32 reason) {
    if (minesys.mine_debris != -1)
        AddGameDebris(WORLD->game_debris_sys, minesys.mine_debris, &part->position);
    if (minesys.mine_part != -1)
        AddFiniteShotPART(minesys.mine_part, &part->position, 1);
}

static i32 PodRaceUpdatePodPc(RACEPOD_RESOURCE_PC *pod, WORLDINFO *world) {
    __declspec(align(16)) NUVEC4 direction;
    NUVEC origin;
    NUVEC velocity;
    NUVEC difference;
    NUVEC target;
    NUVEC target_velocity;
    NUMTX matrix;
    ADDPART_PC part;
    GAMEOBJECT *object;
    f32 length_sq;
    f32 dot;
    f32 length;
    f32 ratio;
    f32 speed;
    f32 distance_sq;
    if (pod->spline == NULL)
        return 0;
    object = pod->object;
    if (object == NULL || (object->flags_1fc & 1) == 0 || object->dead != 0) {
        memset(pod, 0, sizeof(*pod));
        return 0;
    }
    if (!(PodRacePc->lap_countdown > 0.0f)) {
        direction.x = pod->previous_position.x - pod->matrix.m30;
        direction.y = pod->previous_position.y - pod->matrix.m31;
        direction.z = pod->previous_position.z - pod->matrix.m32;
        length_sq = direction.x * direction.x + direction.y * direction.y + direction.z * direction.z;
        dot = direction.y * pod->matrix.m21 + direction.x * pod->matrix.m20 + direction.z * pod->matrix.m22;
        if (!(pod->start >= 1.0f)) {
            while (length_sq < 1.0f || dot < 0.0f) {
                pod->start = pod->start + PodRaceSplineIncrementPc;
                if (pod->start > 1.0f) {
                    pod->start = 1.0f;
                    break;
                }
                CalcSplinePoint(pod->spline, &pod->previous_position, pod->start);
                direction.x = pod->previous_position.x - pod->matrix.m30;
                direction.y = pod->previous_position.y - pod->matrix.m31;
                direction.z = pod->previous_position.z - pod->matrix.m32;
                length_sq = direction.x * direction.x + direction.y * direction.y + direction.z * direction.z;
                dot = direction.y * pod->matrix.m21 + direction.x * pod->matrix.m20 + direction.z * pod->matrix.m22;
            }
        }
        length = NuFsqrt(length_sq);
        if (pod->start >= 1.0f) {
            direction.x = pod->previous_axis.x * FRAMETIME;
            direction.y = pod->previous_axis.y * FRAMETIME;
            direction.z = pod->previous_axis.z * FRAMETIME;
            pod->matrix.m30 += direction.x;
            pod->matrix.m31 += direction.y;
            pod->matrix.m32 += direction.z;
            RacePodAlignPrivatePc(pod, &direction, (f32)(length / 5.0), 0);
            pod->previous_displacement = direction;
        } else if (length != 0.0f) {
            ratio = PodRacePc->mushroom_timer / PodRacePc->previous_lap_display;
            speed = pod->speed;
            if (ratio > 1.0f)
                ratio = 1.0f;
            if (WORLD->current_level == PODRACEB_LDATA && Lap == 1) {
                speed = (f32)(speed / (f64)0.995f);
                if (ratio < 0.01f)
                    speed = (f32)(speed * ratio / (f64)0.01f);
                else
                    speed = (f32)(speed * 1.5 - (ratio - (f64)0.01f) * speed);
            } else {
                speed = (f32)(speed * 1.5 - ratio * speed);
            }
            speed = speed / length;
            pod->previous_axis.x = direction.x * speed;
            pod->previous_axis.y = direction.y * speed;
            pod->previous_axis.z = direction.z * speed;
            direction.x = pod->previous_axis.x * FRAMETIME;
            direction.y = pod->previous_axis.y * FRAMETIME;
            direction.z = pod->previous_axis.z * FRAMETIME;
            pod->matrix.m30 = direction.x + pod->matrix.m30;
            pod->matrix.m31 += direction.y;
            pod->matrix.m32 += direction.z;
            RacePodAlignPrivatePc(pod, &direction, (f32)(length / 5.0), 0);
            pod->previous_displacement = direction;
        }
        if (world->current_level == PODRACEA_LDATA && pod->object != NULL && pod->model_id == id_SEBULBASPOD &&
            NuSpecialExistsFn(&minesys.mine_special) != 0) {
            pod->object->special_move_timer_pc -= FRAMETIME;
            if (pod->object->special_move_timer_pc < 0.0f) {
                object = pod->object;
                origin.x = object->position.x;
                origin.y = (f32)(object->position.y + 1.0);
                origin.z = object->position.z;
                if (player != NULL) {
                    GAMEOBJECT *heading_player = player;
                    distance_sq = NuVecDistSqr(&player->collision_position, &object->collision_position, &difference);
                    if (distance_sq > 4.0 * PodSebulbaThrowAheadPc * PodSebulbaThrowAheadPc) {
                        target.x = (f32)(PodSebulbaThrowSpreadPc -
                                         (PodSebulbaThrowSpreadPc + PodSebulbaThrowSpreadPc) * NuRandFloat());
                        target.y = 0.0f;
                        target.z = PodSebulbaThrowAheadPc;
                        NuVecRotateY(&target, &target, heading_player->sock_heading);
                        NuVecAdd(&target, &target, &player->collision_position);
                        target_velocity.x = 0.0f;
                        target_velocity.y = 0.0f;
                        target_velocity.z = CurrentSpeed;
                        NuVecRotateY(&target_velocity, &target_velocity, heading_player->sock_heading);
                        MakeThrowVector(&velocity, &origin, &target, &target_velocity, PodSebulbaThrowSpeedPc,
                                        PodSebulbaThrowGravityPc);
                        if (distance_sq > PodSebulbaFarThrowPc * PodSebulbaFarThrowPc) {
                            velocity.y = 0.0f;
                            NuVecNorm(&velocity, &velocity);
                            NuVecScale(&velocity, &velocity, PodSebulbaFarThrowPc);
                            NuVecAdd(&target, &origin, &velocity);
                            target_velocity.x = 0.0f;
                            target_velocity.y = 0.0f;
                            target_velocity.z = 0.0f;
                            MakeThrowVector(&velocity, &origin, &target, &target_velocity, PodSebulbaThrowSpeedPc,
                                            PodSebulbaThrowGravityPc);
                        }
                    }
                    NuMtxSetTranslation(&matrix, &origin);
                    part = Default_ADDPART;
                    part.matrix = &matrix;
                    part.velocity = &velocity;
                    part.field_14 = NuSpecialGetOriginRadius(&minesys.mine_special);
                    part.field_18 = part.field_14;
                    part.special = &minesys.mine_special;
                    part.flags = 0x29a;
                    part.gravity = PodSebulbaThrowGravityPc;
                    part.collision_fn = PartCollide_3D;
                    part.kill_fn = PodRaceMineKillPc;
                    part.time_step = FRAMETIME;
                    part.field_a4 = 5.0f;
                    AddPart(&part);
                    {
                        const f64 random = NuRandFloat();
                        pod->object->special_move_timer_pc =
                            (PodSebulbaThrowJitterPc + PodSebulbaThrowJitterPc) * random +
                            (PodSebulbaThrowTimePc - PodSebulbaThrowJitterPc);
                    }
                }
            }
        }
    }
    pod->matrix.m33 = 1.0f;
    if (pod->object != NULL) {
        pod->object->vehicle_orientation = pod->matrix;
        pod->object->position = pod->object->vehicle_orientation.translation;
        pod->object->ai_update_position = pod->object->position;
    }
    return 1;
}

void __cdecl PodRaceUpdate(WORLDINFO *world, f32 dt) {
    char reason[128];
    f32 old;
    i32 i;
    if (netclient != 0) {
        PodRacePc->lap_countdown = ((PODRACE_NETPACKET_READER_PC *)podrace_netpacket)->start_countdown;
        PodRacePc->lap_display = ((PODRACE_NETPACKET_READER_PC *)podrace_netpacket)->remaining_time;
        PodRacePc->previous_lap_display = ((PODRACE_NETPACKET_READER_PC *)podrace_netpacket)->available_time;
        Lap = ((PODRACE_NETPACKET_READER_PC *)podrace_netpacket)->lap;
    }
    if (PodRacePc->lap_countdown > 0.0f) {
        old = PodRacePc->lap_countdown;
        PodRacePc->lap_display = PodRacePc->previous_lap_display;
        avg_currentspeed_mul = 0.0f;
        if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0)
            ((GAMEOBJECT *)Player[0])->current_speed_mul = 0.0f;
        if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0)
            ((GAMEOBJECT *)Player[1])->current_speed_mul = 0.0f;
        if (netclient == 0 && FadeSys.fade == 0.0f)
            PodRacePc->lap_countdown -= dt;
        if (PodRacePc->lap_countdown < 2.9f && old >= 2.9f)
            PlaySfx("PodX_EngStartup1", NULL);
        if (PodRacePc->lap_countdown < 1.5f && old >= 1.5f)
            PlaySfx("PodX_EngStartup2", NULL);
        if (PodRacePc->lap_countdown <= 0.0f)
            PlaySfx("Pod_Race_Go", NULL);
        else if ((i32)PodRacePc->lap_countdown != (i32)old)
            PlaySfx("Pod_Race_Light", NULL);
    } else if (netclient == 0) {
        PodRacePc->mushroom_timer = dt + PodRacePc->mushroom_timer;
        if (PodRacePc->previous_lap_display > 0.0f) {
            PodRacePc->lap_display = PodRacePc->previous_lap_display - PodRacePc->mushroom_timer;
            if (PodRacePc->lap_display < 0.0f && FadeSys.fade == 0.0f) {
                PodRacePc->lap_display = 0.0f;
                if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0)
                    LoseCoins((GAMEOBJECT *)Player[0], 1);
                if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0)
                    LoseCoins((GAMEOBJECT *)Player[1], 1);
                PodRacePc->flags |= 2;
                KillPlayer(player, 2, 1, NULL);
                PodRacePc->attempts++;
                if (PodRacePc->previous_lap_display < PodRacePc->max_lap_time &&
                    PodRacePc->attempts_per_increment != 0 &&
                    PodRacePc->attempts % PodRacePc->attempts_per_increment == 0) {
                    PodRacePc->previous_lap_display = PodRacePc->lap_time_increment + PodRacePc->previous_lap_display;
                    if (PodRacePc->previous_lap_display > PodRacePc->max_lap_time)
                        PodRacePc->previous_lap_display = PodRacePc->max_lap_time;
                }
            }
        }
    }
    for (i = 0; i < 16; i++) {
        if (PodRacePc->lap_entries[i].next != NULL)
            PodRaceUpdatePodPc(&PodRacePc->lap_entries[i], world);
    }
    if ((PodRacePc->flags & 1) == 0 && ((Player[0] != NULL && ((GAMEOBJECT *)Player[0])->dead != 0) ||
                                        (Player[1] != NULL && ((GAMEOBJECT *)Player[1])->dead != 0))) {
        PodRacePc->flags |= 1;
        if ((PodRacePc->flags & 2) != 0)
            NuStrCpy(reason, "Ep1_PodRace_OutOfTime");
        else
            NuStrCpy(reason, "EP1_PODRACE_PODEXPLODE");
        ResetLevel(world, reason, 1);
    }
    if (netclient == 0) {
        ((PODRACE_NETPACKET_READER_PC *)podrace_netpacket)->start_countdown = PodRacePc->lap_countdown;
        ((PODRACE_NETPACKET_READER_PC *)podrace_netpacket)->lap = Lap;
        ((PODRACE_NETPACKET_READER_PC *)podrace_netpacket)->remaining_time = PodRacePc->lap_display;
        ((PODRACE_NETPACKET_READER_PC *)podrace_netpacket)->available_time = PodRacePc->previous_lap_display;
    }
}

/* PC 0x007ffd34/0x007ffd38/0x007ffd3c: mushroom collapse retry window. */
f32 mushroom_time_available = 15.0f;
f32 mushroom_max_time_available = 15.0f;
static f32 mushroom_countdown = 15.0f;

i32 __cdecl NuSpecialClipTestExtents(NUSPECIALHANDLE *special, NUMTX *matrix);

void __cdecl PodRaceBUpdate(WORLDINFO *world) {
    if (Lap == 2 && PodRacePc->mushroom_timer > 1.0f && mushroom_collapse != 0) {
        switch (((PODRACE_LEVFLAG_PC *)LevFlag)->mushroom_state) {
            case 0:
                if (GameCam->sock_position.distance > mushroom0_along) {
                    NewCutScene(NULL, world->cutscene_sys, "ep1_podrace_mushroom0", 1);
                    ((PODRACE_LEVFLAG_PC *)LevFlag)->mushroom_state = 1;
                }
                break;
            case 1:
                if (mushroom0_cut != NULL &&
                    (((CUT_INSTANCE_READER_PC *)mushroom0_cut->instance)->flags_88 & 0x1000) != 0) {
                    NewCutScene(NULL, world->cutscene_sys, "EP1_PODRACE_MUSHROOM1", 1);
                    mushroom_countdown = mushroom_time_available;
                    ((PODRACE_LEVFLAG_PC *)LevFlag)->mushroom_state = 2;
                }
                break;
            case 2:
                mushroom_countdown -= FRAMETIME;
                if (mushroom_countdown < 0.0f) {
                    NewCutScene(NULL, world->cutscene_sys, "EP1_PODRACE_MUSHROOM2", 1);
                    mushroom_n_attempts++;
                    if (mushroom_nattempts_per_increment > 0 &&
                        mushroom_n_attempts % mushroom_nattempts_per_increment == 0) {
                        mushroom_time_available += mushroom_time_increment;
                        if (mushroom_time_available > mushroom_max_time_available)
                            mushroom_time_available = mushroom_max_time_available;
                    }
                    ((PODRACE_LEVFLAG_PC *)LevFlag)->mushroom_state = 4;
                } else if (GameCam->sock_position.distance > mushroom2_along) {
                    ((PODRACE_LEVFLAG_PC *)LevFlag)->mushroom_state = 3;
                }
                break;
            case 3:
                if (NuSpecialExistsFn(&LevHSpecial[0]) != 0 &&
                    NuSpecialClipTestExtents(&LevHSpecial[0], NuSpecialGetDrawMtx(&LevHSpecial[0])) == 0)
                    ((PODRACE_LEVFLAG_PC *)LevFlag)->mushroom_state = 4;
                break;
        }
    }
    UpdatePacemakerDisplay(world);
    UpdatePodRaceLapDisplay(FRAMETIME);
    PodRaceUpdate(world, FRAMETIME);
    if (Lap == 1 && ((PODRACE_LEVFLAG_PC *)LevFlag)->podrace_state == 0 && PODRACE_ADATA != NULL &&
        Game.area_save[PODRACE_ADATA->index].area_complete == 0 && GameTimer.time_elapsed >= 3.0f) {
        Hint_SetComplete(0x27e);
        ((PODRACE_LEVFLAG_PC *)LevFlag)->podrace_state = 1;
    }
}

/* Pod sprint (Episode I bonus) level callbacks. */
typedef struct PODSPRINT_VIEW_PC {
    NUGSPLINE *finish_line;
    NUGSPLINE *halfway;
    PODSPRINT_AI_PC ai[6];
    GIZAIMESSAGE *lap_msg;
    GIZAIMESSAGE *max_speed_msg;
    GIZAIMESSAGE *min_speed_msg;
    GIZAIMESSAGE *speed_step_msg;
    GAMEOBJECT *pacemaker;
    AIAREA_PC *boulders;
    f32 arrow_timer;
    f32 countdown;
    f32 lap_time;
    u8 unknown_8c[2];
    i8 ai_state;
    i8 ai_index;
    u8 unknown_90[2];
    union {
        u16 flags;
        struct {
            i16 halfway : 1;
            i16 unknown_1 : 1;
            i16 finished : 2;
            i16 reset_requested : 1;
            i16 unknown_5 : 11;
        } bits;
    };
} PODSPRINT_VIEW_PC;
STATIC_ASSERT(sizeof(PODSPRINT_VIEW_PC) == sizeof(PODSPRINT_PC));
STATIC_ASSERT(offsetof(PODSPRINT_VIEW_PC, pacemaker) == 0x78);
STATIC_ASSERT(offsetof(PODSPRINT_VIEW_PC, ai_state) == 0x8e);
STATIC_ASSERT(offsetof(PODSPRINT_VIEW_PC, flags) == 0x92);

typedef struct PODSPRINT_NETPACKET_PC {
    i16 ai_state;
    u8 unknown_02[4];
    i16 countdown;
    i16 lap_time;
} PODSPRINT_NETPACKET_PC;

#define PODSPRINT ((PODSPRINT_VIEW_PC *)&podsprint)
#define PODSPRINT_NET ((PODSPRINT_NETPACKET_PC *)podsprint_netpacket)

/* PC 0x008824cc: last player positions used for the finish line tests. */
static NUVEC pod_old_pos[2];
/* PC 0x007ffd1c..0x007ffd24 and 0x007ffd88..0x007ffd94: pod sprint HUD and boulder tuning. */
static f32 PodSprintLapIconScalePc = 0.16f;
static f32 PodSprintCountdownYPc = 0.425f;
static f32 PodSprintCountdownScalePc = 0.75f;
static f32 PodSprintArrowSizePc = 0.05f;
static f32 PodSprintArrowHeightPc = 0.75f;
static f32 boulder_offset_y = 1.0f;
static f32 boulder_offset_y_seek = 1.0f;

extern f32 HudBossIconYPc;
i32 __cdecl XZLinesIntersect(NUVEC *a, NUVEC *b, NUVEC *c, NUVEC *d, f32 *first, f32 *second);

static void PodSprint_InitAISplinePc(WORLDINFO *world, PODSPRINT_AI_PC *ai, char *name) {
    NUGSPLINE *line;
    NUVEC *pts;
    i16 *vals;
    i32 count;
    i32 i = 0;
    if (world == NULL || ai == NULL || name == NULL)
        return;
    memset(ai, 0, sizeof(*ai));
    ai->spline = NuSplineFind(world->current_gscn, name);
    if (ai->spline == NULL || PODSPRINT->finish_line == NULL || PODSPRINT->halfway == NULL)
        return;
    count = 0;
    vals = ai->vals;
    for (; i < ai->spline->length - 1; i++) {
        if (count >= 6)
            break;
        pts = &ai->spline->pts[i];
        line = (count & 1) != 0 ? PODSPRINT->finish_line : PODSPRINT->halfway;
        if (XZLinesIntersect(&pts[0], &pts[1], &line->pts[0], &line->pts[1], NULL, NULL) != 0) {
            *vals++ = (i16)i;
            count++;
        }
    }
    if (ai->vals[5] == 0)
        ai->vals[5] = ai->spline->length;
}

void __cdecl PodSprintA_Init(WORLDINFO *world) {
    memset(&podsprint, 0, sizeof(podsprint));
    podsprint_netpacket = SetLevelHack(10);
    PODSPRINT->finish_line = NuSplineFind(world->current_gscn, "finish_line");
    if (PODSPRINT->finish_line != NULL && PODSPRINT->finish_line->length < 2)
        PODSPRINT->finish_line = NULL;
    PODSPRINT->halfway = NuSplineFind(world->current_gscn, "halfway");
    if (PODSPRINT->halfway != NULL && PODSPRINT->halfway->length < 2)
        PODSPRINT->halfway = NULL;
    PODSPRINT->lap_msg = CheckGizAIMessage(gizaimessagesys, "Lap", NULL);
    PODSPRINT->lap_msg->value = 0.0f;
    PODSPRINT->max_speed_msg = CheckGizAIMessage(gizaimessagesys, "sebulba_max_speed", NULL);
    PODSPRINT->min_speed_msg = CheckGizAIMessage(gizaimessagesys, "sebulba_min_speed", NULL);
    PODSPRINT->speed_step_msg = CheckGizAIMessage(gizaimessagesys, "sebulba_speed_step", NULL);
    PodSprint_InitAISplinePc(world, &PODSPRINT->ai[0], "ai_sebulba");
    PodSprint_InitAISplinePc(world, &PODSPRINT->ai[1], "ai_general");
    NuSpecialFind(world->current_gscn, &LevHSpecial[50], "bigrock_five", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[51], "bigrock_eight", 1);
}

void __cdecl PodSprintA_Reset(WORLDINFO *world) {
    i32 i;
    GAMEOBJECT *object;
    PODSPRINT->bits.reset_requested = 0;
    pod_old_pos[0].x = pod_old_pos[0].y = pod_old_pos[0].z = -1.0f;
    PODSPRINT->pacemaker = NULL;
    pod_old_pos[1].x = pod_old_pos[1].y = pod_old_pos[1].z = -1.0f;
    PODSPRINT->lap_time = 0.0f;
    if ((PODSPRINT->flags & 0xc) == 0 && (netclient == 0 || PODSPRINT_NET->ai_state <= 2)) {
        PODSPRINT->bits.halfway = 0;
        PODSPRINT->countdown = 3.0f;
        PODSPRINT->pacemaker = NULL;
        PODSPRINT->ai_state = 1;
        podstartracealpha = 0.0f;
        PODSPRINT->ai_index = 0;
    } else {
        PODSPRINT->bits.finished = 0;
        VehicleAreaRememberSpeed = 1.0f;
        PODSPRINT->ai_state = 3;
        PODSPRINT->ai_index = 4;
        PODSPRINT->bits.halfway = 0;
        for (i = 0; i < 2; i++) {
            object = i != 0 ? player2 : player;
            if (object != NULL && (object->flags_1fc_word & 0x1000) != 0) {
                object->field_dc8 = 1.0f;
                object->velocity.x = 0.0f;
                object->velocity.y = 0.0f;
                object->velocity.z = object->character_data->game_character->run_speed;
                NuVecRotateY(&object->velocity, &object->velocity, object->field_0x276);
            }
        }
        if (game_cutscenes_cutscene != NULL) {
            CutScene_SnapToEnd(game_cutscenes_cutscene);
            CutScene_StoppedFn_LSW((CUTINFO *)game_cutscenes_cutscene);
        }
    }
    PodRaceSnipersReset();
    PODSPRINT->boulders = AISysFindArea(world->ai_sys, "Boulders");
}

static void PodSprint_UpdatePacemakerPc(WORLDINFO *world) {
    NUVEC position;
    GAME_MESSAGE_PC *message;
    if (PODSPRINT->pacemaker == NULL) {
        PODSPRINT->pacemaker = GetNamedGameObject(world->ai_sys, "SebulbasPod");
        if (PODSPRINT->pacemaker == NULL)
            return;
    }
    if ((PODSPRINT->pacemaker->flags_1fc_word & 0x1000) == 0 || PODSPRINT->pacemaker->dead != 0)
        return;
    if (FadeSys.fade != 0.0f || pause_rndr_on != 0) {
        PODSPRINT->arrow_timer = 0.0f;
        return;
    }
    PODSPRINT->arrow_timer = (FRAMETIME + FRAMETIME) + PODSPRINT->arrow_timer;
    if (PODSPRINT->arrow_timer > 1.0f)
        PODSPRINT->arrow_timer = 1.0f;
    if ((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.2f) < (f64)0.1f) {
        position = PODSPRINT->pacemaker->upper_position;
        position.y += PodSprintArrowHeightPc;
        message = AddGameMessage(" ", &position, PodSprintArrowSizePc, NULL, 0.0f, 0xff, 0x3f, 0x3f, 0x10083, 0.0f);
        if (message != NULL) {
            message->alpha = (u8)(NuTrigTable[(i32)(PODSPRINT->arrow_timer * 16384.0) >> 1 & 0x7fff] * 128.0);
            message->icon = 0x134;
            if (world->level_objects[0x134].active != 0)
                message->special = world->level_objects[0x134].special;
        }
    }
}

static i32 PodSprint_UpdateCountdownPc(void) {
    f32 previous;
    if (PODSPRINT->countdown > 0.0f) {
        previous = PODSPRINT->countdown;
        if (FadeSys.fade == 0.0f)
            PODSPRINT->countdown -= FRAMETIME;
        if (Player[0] != NULL) {
            if (PODSPRINT->countdown > 0.0f) {
                ((GAMEOBJECT *)Player[0])->field_dc8 = 0.0f;
                ((GAMEOBJECT *)Player[0])->velocity.z = 0.0f;
                ((GAMEOBJECT *)Player[0])->velocity.x = 0.0f;
                ((GAMEOBJECT *)Player[0])->run_speed_override = 0.0f;
            } else {
                ((GAMEOBJECT *)Player[0])->run_speed_override = 1000000000.0f;
            }
        }
        if (Player[1] != NULL) {
            if (PODSPRINT->countdown > 0.0f) {
                ((GAMEOBJECT *)Player[1])->field_dc8 = 0.0f;
                ((GAMEOBJECT *)Player[1])->velocity.z = 0.0f;
                ((GAMEOBJECT *)Player[1])->velocity.x = 0.0f;
                ((GAMEOBJECT *)Player[1])->run_speed_override = 0.0f;
            } else {
                ((GAMEOBJECT *)Player[1])->run_speed_override = 1000000000.0f;
            }
        }
        if (PODSPRINT->countdown < 2.9f && previous >= 2.9f)
            PlaySfx("PodX_EngStartup1", NULL);
        if (PODSPRINT->countdown < 1.5f && previous >= 1.5f)
            PlaySfx("PodX_EngStartup2", NULL);
        if (PODSPRINT->countdown <= 0.0f) {
            PlaySfx("Pod_Race_Go", NULL);
            return 1;
        }
        if ((i32)PODSPRINT->countdown != (i32)previous)
            PlaySfx("Pod_Race_Light", NULL);
        return 1;
    }
    return 0;
}

static void PodSprint_UpdateLapsPc(WORLDINFO *world) {
    i32 i;
    NUVEC *old;
    NUVEC *line;
    NUVEC *line_end;
    if (PODSPRINT->finish_line != NULL && PODSPRINT->halfway != NULL) {
        PODSPRINT->lap_time += FRAMETIME;
        if ((PODSPRINT->flags & 1) != 0) {
            line = PODSPRINT->finish_line->pts;
            line_end = &line[1];
            for (i = 0, old = pod_old_pos; i < 2; i++, old++) {
                if (Player[i] != NULL && old->x != -1.0f && old->y != -1.0f && old->z != -1.0f &&
                    XZLinesIntersect(line, line_end, &((GAMEOBJECT *)Player[i])->position, old, NULL, NULL) != 0) {
                    PODSPRINT->bits.halfway = 0;
                    PODSPRINT->lap_time = 0.0f;
                    PODSPRINT->ai_index++;
                    PODSPRINT->ai_state++;
                    if (PODSPRINT->ai_state == 3) {
                        PODSPRINT->flags |= 8;
                        StoreLevelProgress(world);
                        ResetLevel(world, "ep1_podsprint_sebulba", 1);
                    } else if (PODSPRINT->ai_state > 3) {
                        if (FreePlay != 0)
                            CompleteLevel(world);
                        else if (NewCutScene(NULL, WORLD->cutscene_sys, "Ep1_Podrace_Outro1", 1) == 0 &&
                                 NewCutScene(NULL, WORLD->cutscene_sys, "Ep1_Podsprint_Outro1", 1) == 0)
                            CompleteLevel(world);
                    }
                    break;
                }
            }
        } else {
            line = PODSPRINT->halfway->pts;
            line_end = &line[1];
            for (i = 0, old = pod_old_pos; i < 2; i++, old++) {
                if (Player[i] != NULL &&
                    XZLinesIntersect(line, line_end, &((GAMEOBJECT *)Player[i])->position, old, NULL, NULL) != 0) {
                    PODSPRINT->flags |= 1;
                    PODSPRINT->ai_index++;
                    break;
                }
            }
        }
    }
    if (Player[0] != NULL)
        pod_old_pos[0] = ((GAMEOBJECT *)Player[0])->position;
    if (Player[1] != NULL)
        pod_old_pos[1] = ((GAMEOBJECT *)Player[1])->position;
    PODSPRINT->lap_msg->value = (f32)PODSPRINT->ai_state;
}

void __cdecl PodSprintA_Update(WORLDINFO *world) {
    i32 i;
    GAMEOBJECT *object;
    f32 seek;
    VehicleAreaRememberSpeed = 1.0f;
    if (nethost != 0) {
        PODSPRINT_NET->ai_state = PODSPRINT->ai_state;
        PODSPRINT_NET->countdown = (i16)PODSPRINT->countdown;
        PODSPRINT_NET->lap_time = (i16)PODSPRINT->lap_time;
    } else if (netclient != 0) {
        PODSPRINT->countdown = (f32)PODSPRINT_NET->countdown;
        PODSPRINT->ai_state = (i8)PODSPRINT_NET->ai_state;
        PODSPRINT->lap_time = (f32)PODSPRINT_NET->lap_time;
    }
    if (netclient == 0) {
        if (CutScene_PlayingOrRequested(NULL) != 0)
            return;
        if ((PODSPRINT->flags & 0xc) != 0) {
            if ((PODSPRINT->flags & 0x10) == 0) {
                PODSPRINT->flags |= 0x10;
                ResetLevel(world, NULL, 1);
            }
            return;
        }
    }
    if (PodSprint_UpdateCountdownPc() == 0) {
        PodSprint_UpdatePacemakerPc(world);
        if (netclient == 0)
            PodSprint_UpdateLapsPc(world);
        PodRaceSnipersUpdate();
    }
    object = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; i++, object++) {
        if ((object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 && object->field_280 == 0xff) {
            seek = 0.0f;
            if (PODSPRINT->boulders != NULL && WORLD->ai_sys != NULL && PODSPRINT->ai_state > 1 &&
                (*(u64 *)object->ai_area_mask & (u64)(i64)(1 << (PODSPRINT->boulders - WORLD->ai_sys->areas))) != 0)
                seek = boulder_offset_y;
            object->movement_spline_offset.y = SeekValF(object->movement_spline_offset.y, seek, boulder_offset_y_seek);
        }
    }
}

static void PodSprint_UpdateAlphaPc(void) {
    if (FadeSys.fade == 0.0f && MiniCutCam == 0 && CUTSTOPGAME == 0) {
        if (PODSPRINT->countdown > 0.0f) {
            if (Paused == 0 && podstartracealpha < 1.0f) {
                podstartracealpha += FRAMETIME + FRAMETIME;
                if (podstartracealpha > 1.0f)
                    podstartracealpha = 1.0f;
            }
            podlapalpha = 0.0f;
        } else {
            podlapalpha = SeekLinearF(podlapalpha, (Paused != 0 || PODSPRINT->lap_time < 6.0f) ? 1.0f : 0.0f,
                                      FRAMETIME + FRAMETIME);
        }
    } else {
        podlapalpha = 0.0f;
        podhurryalpha = 0.0f;
        podstartracealpha = 0.0f;
    }
}

void __cdecl PodSprintA_Panel(WORLDINFO *world) {
    char text[256];
    i32 count;
    f32 scale;
    LEVEL_OBJECT_RUNTIME_PC *icon;
    PodSprint_UpdateAlphaPc();
    if (Paused == 0 && podstartracealpha != 0.0f && PODSPRINT->countdown > 0.0f) {
        count = (i32)PODSPRINT->countdown + 1;
        if (count > 3)
            count = 3;
        sprintf(text, "%i", count);
        scale = NuFmod(PODSPRINT->countdown, 1.0f);
        if (scale >= 0.7f)
            scale = (f32)(((f64)scale - (f64)0.7f) / (f64)0.3f + 1.0);
        else
            scale = 1.0f;
        scale = PodSprintCountdownScalePc * scale;
        Text3DEx(text, 0.0f, PodSprintCountdownYPc, 1.0f, scale, scale, scale, 0, 0, 0xff, 0,
                 (u8)(podstartracealpha * 128.0));
    }
    if (podlapalpha > 0.0f) {
        icon = &world->level_objects[0x135 + PODSPRINT->ai_state];
        if (icon->active != 0)
            DrawPanel3DObject(0.0f, HudBossIconYPc, 1.0f, PodSprintLapIconScalePc, PodSprintLapIconScalePc,
                              PodSprintLapIconScalePc, 0, 0, 0, &icon->special, 0, podlapalpha);
    }
}

i32 __cdecl Action_MushroomCollapse(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 count, i32 first_time, f32 elapsed) {
    if (first_time != 0 && mushroom_collapse == 0) {
        i32 index;
        char *value;
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], "trigger_crash=");
            if (value != NULL)
                mushroom0_along = AIParamToFloat((AISCRIPTPROCESS_PC *)packet, value + 14);
            else if ((value = NuStrIStr(params[index], "trigger_collapse=")) != NULL)
                mushroom2_along = AIParamToFloat((AISCRIPTPROCESS_PC *)packet, value + 17);
            else if ((value = NuStrIStr(params[index], "max_time_available=")) != NULL)
                mushroom_max_time_available = AIParamToFloat((AISCRIPTPROCESS_PC *)packet, value + 19);
            else if ((value = NuStrIStr(params[index], "time_available=")) != NULL)
                mushroom_time_available = AIParamToFloat((AISCRIPTPROCESS_PC *)packet, value + 15);
            else if ((value = NuStrIStr(params[index], "time_increment=")) != NULL)
                mushroom_time_increment = AIParamToFloat((AISCRIPTPROCESS_PC *)packet, value + 15);
            else if ((value = NuStrIStr(params[index], "nattempts_per_increment=")) != NULL)
                mushroom_nattempts_per_increment = (i32)AIParamToFloat((AISCRIPTPROCESS_PC *)packet, value + 24);
            if (mushroom_max_time_available < mushroom_time_available)
                mushroom_max_time_available = mushroom_time_available;
        }
        mushroom_collapse = 1;
    }
    return 1;
}

void __cdecl PodRace_IncreaseLap(void) {
    Lap = (i32)((u32)Lap + 1u);
}

typedef struct pod_race_panel_netpacket_pc_s {
    f32 start_countdown;
    f32 remaining_time;
    f32 available_time;
    f32 lap_alpha;
    i32 lap;
} POD_RACE_PANEL_NETPACKET_PC;
STATIC_ASSERT(sizeof(POD_RACE_PANEL_NETPACKET_PC) == 0x14);
STATIC_ASSERT(offsetof(POD_RACE_PANEL_NETPACKET_PC, lap_alpha) == 0x0c);
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, lap_countdown) == 0xb000);
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, lap_display) == 0xb008);
STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
STATIC_ASSERT(sizeof(LEVEL_OBJECT_RUNTIME_PC) == 0x10);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, active) == 0x0e);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, special) == 0);

void __cdecl PodRacePanel(WORLDINFO *world) {
    char text[256];
    LEVEL_OBJECT_RUNTIME_PC *icon;
    f32 scale;
    i32 count;
    if (netclient != 0)
        podlapalpha = ((POD_RACE_PANEL_NETPACKET_PC *)podrace_netpacket)->lap_alpha;
    else
        ((POD_RACE_PANEL_NETPACKET_PC *)podrace_netpacket)->lap_alpha = podlapalpha;
    if (podlapalpha > 0.0f) {
        icon = &world->level_objects[(i32)((u32)Lap + 0x135u)];
        if (icon->active != 0)
            DrawPanel3DObject(0.0f, HudBossIconYPc, 1.0f, PodSprintLapIconScalePc, PodSprintLapIconScalePc,
                              PodSprintLapIconScalePc, 0, 0, 0, &icon->special, 0, podlapalpha);
    }
    if (podhurryalpha > 0.0f && PodRacePc->lap_display > 0.0f) {
        count = (i32)PodRacePc->lap_display + 1;
        sprintf(text, Text_TimeS, count);
        scale = NuFmod(PodRacePc->lap_display, 1.0f);
        if (scale >= 0.7f)
            scale = (f32)(((f64)scale - (f64)0.7f) / (f64)0.3f + 1.0);
        else
            scale = 1.0f;
        scale = PodSprintCountdownScalePc * scale;
        Text3DEx(text, 0.0f, PodSprintCountdownYPc, 1.0f, scale, scale, scale, 0, 255, 63, 0,
                 (u8)(i32)(podhurryalpha * 128.0));
    }
    if (podstartracealpha != 0.0f && PodRacePc->lap_countdown > 0.0f) {
        count = (i32)PodRacePc->lap_countdown + 1;
        sprintf(text, Text_TimeS, count);
        scale = NuFmod(PodRacePc->lap_countdown, 1.0f);
        if (scale >= 0.7f)
            scale = (f32)(((f64)scale - (f64)0.7f) / (f64)0.3f + 1.0);
        else
            scale = 1.0f;
        scale = PodSprintCountdownScalePc * scale;
        Text3DEx(text, 0.0f, PodSprintCountdownYPc, 1.0f, scale, scale, scale, 0, 0, 255, 0,
                 (u8)(i32)(podstartracealpha * 128.0));
    }
}
