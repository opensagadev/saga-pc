#include <stdio.h>
#include "saga.h"

f32 __cdecl PodSprint_InStartCountdown(WORLDINFO *world) {
    if (world->current_level == PODSPRINTA_LDATA)
        return podsprint.speed;
    return 0.0f;
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
    u8 unknown_b000[12];
    f32 previous_lap_display;
    f32 max_lap_time, lap_time_increment;
    i32 attempts_per_increment;
} PODRACE_RESOURCE_PC;
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, lap_entries) == 0xa600);
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, previous_lap_display) == 0xb00c);

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

static void PodRaceSnipersUpdate(void) {
    i32 i;
    NUVEC direction;
    static i32 player_ix;
    i32 bolttype = (WORLD->current_level == PODSPRINTA_LDATA) ? 0x29 : 0x28;
    INIT_BOLT_CONFIG_PC *type = BoltType_FindByID(bolttype, WORLD);
    i32 n = PodRace_nsnipers;
    if (n <= 0 || max_nsnipers <= 0)
        return;
    for (i = 0; i < n && i < max_nsnipers; i++) {
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
                aim = aim;
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
                    CalculateInterceptVector(&s->prev, &target->collision_position, &target->velocity, type->speed,
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
                mtx = mtx;
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

i32 __cdecl Action_CreatePod(void *system, void *process, void *packet, char **params, i32 param_count, i32 first_time,
                             f32 ignored) {
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

static i32 JediBInitLocator(WORLDINFO *world, NUVEC *position, i32 angle, AILOCATOR_PC *locator, f32 radius) {
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

STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, max_lap_time) == 0xb010);
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, lap_time_increment) == 0xb014);
STATIC_ASSERT(offsetof(PODRACE_RESOURCE_PC, attempts_per_increment) == 0xb018);

i32 __cdecl Action_SetLapTime(void *system, void *process, void *packet, char **params, i32 param_count, i32 first_time,
                              f32 ignored) {
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
