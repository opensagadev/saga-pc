#include <stdio.h>
#include "saga.h"

STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);
STATIC_ASSERT(offsetof(GIZTURRET_s, name) == 8);
STATIC_ASSERT(offsetof(GIZTURRET_s, anim_set) == 0x18);
STATIC_ASSERT(offsetof(GIZTURRET_s, primary_anim_obj) == 0x1c);
STATIC_ASSERT(offsetof(GIZTURRET_s, position) == 0x24);
STATIC_ASSERT(offsetof(GIZTURRET_s, opponent_origin) == 0x30);
STATIC_ASSERT(offsetof(GIZTURRET_s, opponent_bounds) == 0x48);
STATIC_ASSERT(offsetof(GIZTURRET_s, pitch) == 0x54);
STATIC_ASSERT(offsetof(GIZTURRET_s, muzzle_offsets) == 0x74);
STATIC_ASSERT(offsetof(GIZTURRET_s, relative_matrix) == 0xa4);
STATIC_ASSERT(offsetof(GIZTURRET_s, opponent) == 0xe4);
STATIC_ASSERT(offsetof(GIZTURRET_s, opponent_range) == 0xec);
STATIC_ASSERT(offsetof(GIZTURRET_s, fire_interval) == 0xfc);
STATIC_ASSERT(offsetof(GIZTURRET_s, behavior_flags) == 0x108);
STATIC_ASSERT(offsetof(GIZTURRET_s, completion_score) == 0x10c);
STATIC_ASSERT(offsetof(GIZTURRET_s, pickup_pitch) == 0x110);
STATIC_ASSERT(offsetof(GIZTURRET_s, pickup_yaw) == 0x112);
STATIC_ASSERT(offsetof(GIZTURRET_s, pickup_offset) == 0x114);
STATIC_ASSERT(offsetof(GIZTURRET_s, pickup_radius) == 0x120);
STATIC_ASSERT(offsetof(GIZTURRET_s, blowup_type) == 0x124);
STATIC_ASSERT(offsetof(GIZTURRET_s, rotate_sfx) == 0x126);
STATIC_ASSERT(offsetof(GIZTURRET_s, fire_sfx) == 0x12a);
STATIC_ASSERT(offsetof(GIZTURRET_s, opponent_type) == 0x12c);
STATIC_ASSERT(offsetof(GIZTURRET_s, state) == 0x12e);
STATIC_ASSERT(offsetof(GIZTURRET_s, shots_required) == 0x131);
STATIC_ASSERT(offsetof(GIZTURRET_s, shots_fired) == 0x132);
STATIC_ASSERT(offsetof(GIZTURRET_s, y_rotation) == 0x134);
STATIC_ASSERT(offsetof(GIZTURRET_s, destroy_sfx) == 0x138);
STATIC_ASSERT(offsetof(GIZTURRET_s, flags) == 0x13a);
STATIC_ASSERT(offsetof(GIZTURRETSYS_s, anim_pool) == 4);
STATIC_ASSERT(offsetof(GIZTURRETSYS_s, count) == 8);
STATIC_ASSERT(offsetof(GIZTURRETSYS_s, capacity) == 0xa);
STATIC_ASSERT(offsetof(GIZTURRETSYS_s, opponent_cursor) == 0xc);
STATIC_ASSERT(offsetof(WORLDINFO, giz_turret_sys) == 0x46b8);

typedef struct TURRET_ANIM_DATA_PC {
    u8 flags, role;
    i16 platform_id;
} TURRET_ANIM_DATA_PC;
STATIC_ASSERT(sizeof(TURRET_ANIM_DATA_PC) == 4);

typedef struct {
    u32 enabled[2], visible[2], reward[2], fired[2], stopped[2], completed[2];
    u8 timers[64];
} TURRET_PROGRESS_PC;

STATIC_ASSERT(sizeof(TURRET_PROGRESS_PC) == 0x70);
STATIC_ASSERT(offsetof(TURRET_PROGRESS_PC, timers) == 0x30);

extern "C" {
    extern const char GizmoRegistryGizTurretsNamePc[];
    extern char *GizmoRegistryGizTurretsNamePtrPc;
    extern ADDGIZMOTYPE_PC Default_ADDGIZMOTYPE;
    extern i32 turret_gizmotype_id;
    extern char TurretOutputBufferPc[32];
    extern const char TurretDestroyedOutputTextPc[10];
    extern const char TurretFiredOutputTextPc[6];
    extern const char TurretFiredShotsFormatPc[15];
    i32 GizTurretAnimDataVersionPc = 7;
    ADDGIZMOTYPE_PC GizmoRegistryGizTurretsAddTypePc = {0};

    static __declspec(noinline) void GizTurretClearPc(GIZTURRET_s *turret) {
        GAMEANIMSET *set;
        if (turret == NULL)
            return;
        set = turret->anim_set;
        memset(turret, 0, sizeof(*turret));
        turret->behavior_flags |= 0x802;
        turret->fire_interval = 1.0f;
        turret->pitch_turn_speed = 8.0f;
        turret->flags &= 0xf7fe;
        turret->yaw_turn_speed = 8.0f;
        turret->anim_set = set;
        turret->initial_state = 1;
        turret->shots_required = 1;
        turret->fire_sfx = -1;
        turret->rotate_sfx = -1;
        turret->destroy_sfx = -1;
        turret->muzzle_count = 1;
        turret->blowup_type = -1;
        turret->bolt_type_id = -1;
        if (WORLD->area != NULL && (WORLD->area->flags & 1))
            turret->pickup_radius = 12.0f;
        else
            turret->pickup_radius = 1.75f;
    }

    static void __cdecl GizTurretReadAnimSetDataPc(GAMEANIMOBJ *object, u8 version) {
        TURRET_ANIM_DATA_PC scratch;
        TURRET_ANIM_DATA_PC *data;
        if (object == NULL || version <= 2)
            return;
        data = (TURRET_ANIM_DATA_PC *)object->object_data;
        if (data == NULL)
            data = &scratch;
        if ((u8)GizTurretAnimDataVersionPc > 4) {
            data->flags = (u8)EdFileReadChar();
            data->role = (u8)EdFileReadChar();
        } else {
            data->flags = (u8)EdFileReadShort();
        }
    }

    i32 __cdecl GizTurrets_Load(WORLDINFO *world, GIZTURRETSYS_s *system) {
        u8 version = (u8)EdFileReadChar();
        i32 index, vector_index, object_index;
        GIZTURRET_s *turret;
        GAMEANIMOBJ *object;
        TURRET_ANIM_DATA_PC *data;
        char name[256];
        system->count = (u16)EdFileReadShort();
        if (system->count == 0)
            return 1;
        turret = system->turrets;
        for (index = 0; index < system->count; ++index, ++turret) {
            GizTurretClearPc(turret);
            EdFileRead(turret->name, 16);
            GizmoFileReadGameAnimSet(turret->anim_set, world, GizTurretReadAnimSetDataPc, version,
                                     (char *)GizmoRegistryGizTurretsNamePc, turret->name);
            EdFileReadNuVec(&turret->position);
            EdFileReadNuVec(&turret->opponent_origin);
            EdFileReadNuVec(&turret->initial_origin);
            EdFileReadNuVec(&turret->opponent_bounds);
            turret->pitch_upper = EdFileReadInt();
            turret->pitch_lower = EdFileReadInt();
            turret->yaw_upper = EdFileReadInt();
            turret->yaw_lower = EdFileReadInt();
            turret->fixed_pitch = EdFileReadInt();
            turret->fixed_yaw = EdFileReadInt();
            if (version > 1)
                turret->behavior_flags = (u32)EdFileReadInt();
            turret->muzzle_count = (u8)EdFileReadChar();
            for (vector_index = 0; vector_index < turret->muzzle_count; ++vector_index)
                EdFileReadNuVec(&turret->muzzle_offsets[vector_index]);
            turret->opponent_range = EdFileReadFloat();
            turret->fire_range = EdFileReadFloat();
            turret->reflection_alpha = EdFileReadFloat();
            turret->fire_interval = EdFileReadFloat();
            turret->pitch_turn_speed = EdFileReadFloat();
            turret->yaw_turn_speed = EdFileReadFloat();
            turret->completion_score = (i16)EdFileReadShort();
            turret->pickup_pitch = (i16)EdFileReadShort();
            turret->pickup_yaw = (i16)EdFileReadShort();
            EdFileReadNuVec(&turret->pickup_offset);
            if (version >= 6)
                turret->pickup_radius = EdFileReadFloat();
            else if (world->area != NULL && (world->area->flags & 1))
                turret->pickup_radius = 12.0f;
            else
                turret->pickup_radius = 1.75f;
            turret->initial_state = (u8)EdFileReadChar();
            if (version > 3) {
                turret->shots_required = (u8)EdFileReadChar();
                turret->base_y_rotation = (i16)EdFileReadShort();
            }
            if (GizmoFileReadName(name))
                turret->bolt_type_id = BoltType_FindIDByName(name, world);
            if (GizmoFileReadName(name))
                turret->fire_sfx = (i16)GetSfxId(name);
            if (GizmoFileReadName(name))
                turret->rotate_sfx = (i16)GetSfxId(name);
            if (version >= 7 && GizmoFileReadName(name))
                turret->destroy_sfx = (i16)GetSfxId(name);
            if (GizmoFileReadName(name)) {
                turret->blowup_type = (i16)GizmoBlowupGetNameTableId(name);
                if (turret->blowup_type != -1)
                    turret->flags |= 0x100;
            }
            turret->y_rotation = (u16)EdFileReadShort();
            if (version <= 4) {
                object_index = 0;
                for (object = turret->anim_set->objects; object != NULL; object = object->next, ++object_index) {
                    data = (TURRET_ANIM_DATA_PC *)object->object_data;
                    if (object_index == 0)
                        data->role = 1;
                    else if (object_index == 1)
                        data->role = 2;
                    else if (data->flags & 1)
                        data->role = 3;
                }
            }
        }
        return 1;
    }

    void __cdecl GizTurrets_PostLoad(void *world, void *pointer) {
        GIZTURRETSYS_s *system = (GIZTURRETSYS_s *)pointer;
        GIZTURRET_s *turret;
        i32 i;
        if (system == NULL)
            return;
        turret = system->turrets;
        for (i = 0; i < system->count; ++i, ++turret) {
            if (turret->flags & 0x100) {
                i32 type_id = GizmoBlowupGetTypeFromNameTableId((WORLDINFO *)world, turret->blowup_type);
                turret->flags &= ~0x100;
                turret->blowup_type = (i16)type_id;
            }
        }
    }

    void __cdecl GizTurrets_AddLevelSfx(void *world, void *system_pointer, i32 *ids, i32 *count, i32 capacity) {
        GIZTURRETSYS_s *system = (GIZTURRETSYS_s *)system_pointer;
        GIZTURRET_s *turret;
        i32 index;
        (void)world;
        if (system == NULL)
            return;
        turret = system->turrets;
        for (index = 0; index < system->count; ++index, ++turret) {
            if (turret->fire_sfx != -1)
                AddLevelSfxFromId(turret->fire_sfx, ids, count, capacity);
            if (turret->rotate_sfx != -1)
                AddLevelSfxFromId(turret->rotate_sfx, ids, count, capacity);
            if (turret->destroy_sfx != -1)
                AddLevelSfxFromId(turret->destroy_sfx, ids, count, capacity);
        }
    }

    void *__cdecl GizTurrets_ReserveBufferSpace(void *pointer) {
        WORLDINFO *world = (WORLDINFO *)pointer;
        GIZTURRETSYS_s *system;
        i32 i;
        system = (GIZTURRETSYS_s *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, sizeof(*system));
        system->capacity = world->current_level->max_turrets;
        system->turrets = (GIZTURRET_s *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end,
                                                         system->capacity * sizeof(*system->turrets));
        system->anim_pool =
            GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 4, system->capacity * 2);
        for (i = 0; i < system->capacity; ++i)
            system->turrets[i].anim_set =
                GameAnimSet_Create(&world->giz_buffer, &world->buffer_end, system->anim_pool, world->game_anim_sys);
        world->giz_turret_sys = (GIZTURRETSYS_s *)system;
        return system;
    }

    void __cdecl GizmoTurret_Activate(GIZMO_PC *gizmo, i32 active) {
        GIZTURRET_s *turret;
        if (gizmo != NULL) {
            turret = (GIZTURRET_s *)gizmo->object;
            turret->active = active != 0;
            if (turret->active) {
                turret->flags &= (u16)~0x30;
                turret->shots_fired = 0;
            }
        }
    }

    i32 __cdecl GizmoTurret_ActivateRev(GIZMO_PC *gizmo, i32 reverse, i32 mode) {
        GIZTURRET_s *turret;
        if (gizmo == NULL || gizmo->object == NULL)
            return 0;
        turret = (GIZTURRET_s *)gizmo->object;
        if ((mode & 1) != 0) {
            if (turret->active)
                return reverse;
            turret->aim_at_fixed_angles = 0;
            return reverse == 0;
        }
        turret->active = reverse == 0;
        return 1;
    }

    void __cdecl GizmoTurret_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
        GIZTURRET_s *turret;
        if (gizmo != NULL && gizmo->object != NULL) {
            turret = (GIZTURRET_s *)gizmo->object;
            GameAnimSet_SetVisibility(turret->anim_set, visible);
            turret->visible = visible != 0;
        }
    }

    NUVEC *__cdecl GizmoTurret_GetPos(GIZMO_PC *gizmo) {
        if (gizmo == NULL || gizmo->object == NULL)
            return NULL;
        return &((GIZTURRET_s *)gizmo->object)->position;
    }

    static __declspec(noinline) void GizTurrets_Reset(GIZTURRET_s *turret, WORLDINFO *world) {
        GAMEANIMOBJ *object;
        TURRET_ANIM_DATA_PC *data;
        NUMTX *primary, *secondary;
        turret->flags |= 6;
        turret->state = turret->initial_state;
        turret->shots_fired = 0;
        turret->primary_anim_obj = NULL;
        turret->secondary_anim_obj = NULL;
        if (turret->anim_set == NULL)
            return;
        for (object = turret->anim_set->objects; object != NULL; object = object->next) {
            data = (TURRET_ANIM_DATA_PC *)object->object_data;
            if (data->role == 1)
                turret->primary_anim_obj = object;
            else if (data->role == 2)
                turret->secondary_anim_obj = object;
            data->platform_id = -1;
            if (world->terrain != NULL && NuSpecialExistsFn(&object->special)) {
                data->platform_id = (i16)FindPlatInst(NuSpecialGetInstanceix(&object->special));
                if (data->platform_id != -1)
                    turret->flags |= 0x200;
            }
        }
        turret->room_id = world->current_gscn != NULL ? NuPortalWhichRoom(world->current_gscn, &turret->position) : -1;
        if (turret->behavior_flags & 2) {
            turret->opponent_origin = turret->position;
            GameAnimSet_GetAveragePos(turret->anim_set, &turret->opponent_origin, 0, 1, 1);
            turret->initial_origin = turret->opponent_origin;
        }
        if (turret->behavior_flags & 0x800) {
            turret->fire_range = turret->opponent_range;
            turret->initial_origin = turret->opponent_origin;
        }
        GameAnimSet_EvaluateState(turret->anim_set);
        if (turret->primary_anim_obj != NULL && NuSpecialExistsFn(&turret->primary_anim_obj->special) &&
            turret->secondary_anim_obj != NULL && NuSpecialExistsFn(&turret->secondary_anim_obj->special)) {
            primary = NuSpecialGetMtx(&turret->primary_anim_obj->special);
            secondary = NuSpecialGetMtx(&turret->secondary_anim_obj->special);
            NuMtxInv(&turret->relative_matrix, secondary);
            NuMtxMul(&turret->relative_matrix, primary, &turret->relative_matrix);
        }
    }

    void __cdecl GizTurrets_ResetPc(WORLDINFO *world, GIZTURRETSYS_s *system, void *progress_pointer) {
        i32 index;
        TURRET_PROGRESS_PC *progress = (TURRET_PROGRESS_PC *)progress_pointer;
        GAMEANIMOBJ *object;
        TURRET_ANIM_DATA_PC *data;
        GIZTURRET_s *turret = system->turrets;
        for (index = 0; index < system->count; ++index, ++turret) {
            GizTurrets_Reset(turret, world);
            if (progress != NULL && index < 64) {
                i32 word = index / 32;
                u32 bit = 1 << (index & 31);
                turret->visible = (progress->visible[word] & bit) != 0;
                turret->active = (progress->enabled[word] & bit) != 0;
                turret->aim_locked = (progress->reward[word] & bit) != 0;
                turret->aim_at_fixed_angles = (progress->fired[word] & bit) != 0;
                turret->update_disabled = (progress->stopped[word] & bit) != 0;
                turret->reward_released = (progress->completed[word] & bit) != 0;
                if (progress->timers[index] != 0xff)
                    turret->state = progress->timers[index];
            }
            if (turret->anim_set != NULL) {
                for (object = turret->anim_set->objects; object != NULL; object = object->next) {
                    data = (TURRET_ANIM_DATA_PC *)object->object_data;
                    if (data->role == 3)
                        NuSpecialSetVisibility(&object->special, turret->update_disabled);
                }
            }
        }
    }

    GAMEOBJECT *GizTurret_GetTgt(GIZTURRET_s *, NUMTX *matrix) {
        f32 best_distance = 1000000000.0f;
        NUVEC forward = {0.0f, 0.0f, -1.0f};
        GAMEOBJECT *best = NULL;

        NuVecMtxRotate(&forward, &forward, matrix);
        GAMEOBJECT *object = Obj;
        for (i32 i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
            if ((object->flags_1fc_word & 1) && (object->flags_1fc_word & 0x1000) && object->dead == 0 &&
                (object->flags_1f8 & 1) && object->model_draw_result != 0) {
                NUVEC direction;
                f32 distance =
                    NuVecDistSqr(&object->collision_position, reinterpret_cast<NUVEC *>(&matrix->m30), &direction);
                if (distance < best_distance) {
                    f32 length = NuFsqrt(distance);
                    f32 scale;
                    if (length > 0.0f)
                        scale = 1.0f / length;
                    else
                        scale = 0.0f;
                    direction.x *= scale;
                    direction.y *= scale;
                    direction.z *= scale;
                    f32 dot = NuVecDot(&direction, &forward);
                    if (dot > NU_COS_LUT(gizturret_test_ang)) {
                        best = object;
                        best_distance = distance;
                    }
                }
            }
        }
        return best;
    }

    void GizTurret_CalculateInterceptVector(NUVEC *origin, NUMTX *matrix, NUVEC *target, NUVEC *velocity, f32 speed,
                                            NUVEC *intercept_out, NUVEC *velocity_out, u32 fallback) {
        NUVEC forward = {0.0f, 0.0f, -1.0f};
        NuVecMtxRotate(&forward, &forward, matrix);

        NUVEC intercept;
        NUVEC intercept_velocity;
        CalculateInterceptVector(origin, target, velocity, speed, &intercept, &intercept_velocity);

        NUVEC direction;
        f32 distance = NuVecDistSqr(&intercept, origin, &direction);
        f32 length = NuFsqrt(distance);
        f32 scale;
        if (length > 0.0f)
            scale = 1.0f / length;
        else
            scale = 0.0f;
        direction.x *= scale;
        direction.y *= scale;
        direction.z *= scale;

        f32 dot = NuVecDot(&direction, &forward);
        if (dot > NU_COS_LUT(gizturret_test_ang) || fallback == 0) {
            if (intercept_out != NULL) {
                *intercept_out = intercept;
            }
            if (velocity_out != NULL) {
                *velocity_out = intercept_velocity;
            }
        } else {
            if (intercept_out != NULL) {
                *intercept_out = forward;
            }
            if (velocity_out != NULL) {
                *velocity_out = *origin;
            }
        }
    }

    extern "C" void GizTurrets_Update(void *context, void *system_ptr, float frame_time) {
        NUVEC direction;
        i32 should_fire;
        GIZTURRETSYS_s *system = static_cast<GIZTURRETSYS_s *>(system_ptr);
        if (system != NULL) {
            GIZTURRET_s *turret = system->turrets;
            {
                for (i32 index = 0; index < system->count; ++index, ++turret) {
                    turret->fired_this_frame = 0;
                    u32 rotation_sound_playing = turret->rotation_sound_playing;
                    turret->rotation_sound_playing = 0;
                    if (turret->visible)
                        if (turret->active)
                            if (!turret->update_disabled) {
                                BOLTTYPE_PC *bolt_type = BoltType_FindByID(static_cast<i8>(turret->bolt_type_id),
                                                                           static_cast<WORLDINFO *>(context));
                                GAMEOBJECT *autoaim_target = NULL;
                                should_fire = 0;
                                NUMTX fallback_draw, fallback_base;
                                NUMTX *primary_draw, *primary_base, *secondary_draw, *secondary_base, *reference;
                                u16 home_yaw = 0;
                                if (turret->primary_anim_obj != NULL &&
                                    NuSpecialExistsFn(&turret->primary_anim_obj->special)) {
                                    primary_draw = NuSpecialGetDrawMtx(&turret->primary_anim_obj->special);
                                    primary_base = NuSpecialGetMtx(&turret->primary_anim_obj->special);
                                    if (turret->secondary_anim_obj != NULL &&
                                        NuSpecialExistsFn(&turret->secondary_anim_obj->special)) {
                                        secondary_base = NuSpecialGetMtx(&turret->secondary_anim_obj->special);
                                        secondary_draw = NuSpecialGetDrawMtx(&turret->secondary_anim_obj->special);
                                        reference = secondary_base;
                                    } else {
                                        reference = primary_base;
                                        secondary_draw = NULL;
                                        secondary_base = NULL;
                                    }
                                } else {
                                    NuMtxSetTranslation(&fallback_draw, &turret->position);
                                    NuMtxPreRotateX(&fallback_draw, turret->pitch);
                                    NuMtxPreRotateY(&fallback_draw, turret->yaw);
                                    NuMtxSetTranslation(&fallback_base, &turret->position);
                                    NuMtxPreRotateY(&fallback_base, turret->base_y_rotation);
                                    primary_draw = &fallback_draw;
                                    primary_base = NULL;
                                    secondary_draw = NULL;
                                    secondary_base = NULL;
                                    reference = &fallback_base;
                                    home_yaw = turret->base_y_rotation;
                                }
                                turret->fire_cooldown -= frame_time;
                                if (turret->fire_cooldown < 0.0f)
                                    turret->fire_cooldown = 0.0f;
                                i32 desired_pitch = 0, desired_yaw = static_cast<i16>(home_yaw);
                                if (turret->controller != NULL) {
                                    GAMEOBJECT *controller = turret->controller;
                                    if (controller->takeover_source != NULL && (controller->flags_1fc & 0x80) != 0) {
                                        GAMEPAD_PC *pad = controller->pad;
                                        f32 pitch_rate = -pad->input_direction_x * turret->pitch_turn_speed;
                                        if (pitch_rate < 0.0f) {
                                            desired_pitch = turret->pitch_upper;
                                            pitch_rate = -pitch_rate;
                                        } else if (pitch_rate > 0.0f)
                                            desired_pitch = turret->pitch_lower;
                                        else
                                            desired_pitch = turret->pitch;
                                        f32 yaw_rate = -pad->input_direction_z * turret->yaw_turn_speed;
                                        if (yaw_rate < 0.0f) {
                                            desired_yaw = turret->yaw_upper;
                                            yaw_rate = -yaw_rate;
                                        } else if (yaw_rate > 0.0f)
                                            desired_yaw = turret->yaw_lower;
                                        else
                                            desired_yaw = turret->yaw;
                                        turret->pitch = SeekRot(turret->pitch, desired_pitch, pitch_rate);
                                        turret->yaw = SeekRot(turret->yaw, desired_yaw, yaw_rate);
                                        if (turret->fire_cooldown > 0.0f &&
                                            (turret->controller->pad->buttons_pressed & GAMEPAD_ACTION)) {
                                            if (turret->fire_cooldown >
                                                turret->fire_interval * gizturret_rapid_fire_rate)
                                                turret->fire_cooldown =
                                                    turret->fire_interval * gizturret_rapid_fire_rate;
                                            turret->rapid_fire_pending = 1;
                                        }
                                        if (turret->fire_cooldown == 0.0f &&
                                            ((turret->controller->pad->buttons_held & GAMEPAD_ACTION) ||
                                             (turret->rapid_fire_pending))) {
                                            should_fire = 1;
                                            if (turret->rapid_fire_pending)
                                                turret->fire_cooldown =
                                                    turret->fire_interval * gizturret_rapid_fire_rate;
                                            else
                                                turret->fire_cooldown = turret->fire_interval;
                                            turret->rapid_fire_pending = 0;
                                            if (turret->behavior_flags & 0x200)
                                                autoaim_target = GizTurret_GetTgt(turret, primary_draw);
                                        }
                                    } else {
                                        turret->pitch = SeekRot(turret->pitch, 0, turret->pitch_turn_speed);
                                        turret->yaw = SeekRot(turret->yaw, home_yaw, turret->yaw_turn_speed);
                                        turret->rapid_fire_pending = 0;
                                    }
                                } else {
                                    if (turret->aim_locked)
                                        goto autonomous_apply;
                                    if (turret->aim_at_fixed_angles) {
                                        desired_pitch = turret->fixed_pitch;
                                        desired_yaw = turret->fixed_yaw;
                                        goto autonomous_apply;
                                    }
                                    if ((turret->behavior_flags & 0xc0) == 0xc0) {
                                        if ((turret->behavior_flags & 0x100) && turret->primary_anim_obj != NULL &&
                                            NuSpecialExistsFn(&turret->primary_anim_obj->special) &&
                                            !NuSpecialGetOnScreenFn(&turret->primary_anim_obj->special))
                                            goto autonomous_apply;
                                    try_fire:
                                        if (turret->fire_cooldown == 0.0f) {
                                            turret->fire_cooldown = (NuRandFloat() + 0.5f) * turret->fire_interval;
                                            should_fire = 1;
                                        }
                                        goto autonomous_apply;
                                    }
                                    if (turret->opponent != NULL) {
                                        NUVEC *target_position, *target_velocity = NULL;
                                        NUVEC local_direction;
                                        f32 distance;
                                        switch (static_cast<i8>(turret->opponent_type)) {
                                            case 0: {
                                                GAMEOBJECT *target = static_cast<GAMEOBJECT *>(turret->opponent);
                                                target_velocity = &target->velocity;
                                                target_position = &target->collision_position;
                                                break;
                                            }
                                            case 1:
                                                target_position = reinterpret_cast<NUVEC *>(
                                                    static_cast<u8 *>(turret->opponent) + 0x11c);
                                                break;
                                            case 2:
                                                target_position = static_cast<NUVEC *>(turret->opponent);
                                                break;
                                            default:
                                                goto autonomous_apply;
                                        }
                                        distance =
                                            NuVecDistSqr(target_position, &turret->initial_origin, &local_direction);
                                        if (target_position == NULL)
                                            goto autonomous_apply;
                                        if ((turret->behavior_flags & 0x200) && target_velocity != NULL &&
                                            bolt_type != NULL) {
                                            GizTurret_CalculateInterceptVector(
                                                reinterpret_cast<NUVEC *>(&reference->m30), primary_draw,
                                                target_position, target_velocity, bolt_type->field_10, &direction, NULL,
                                                turret->controller != NULL);
                                        } else
                                            NuVecSub(&direction, target_position,
                                                     reinterpret_cast<NUVEC *>(&reference->m30));
                                        if ((turret->behavior_flags & 0xc0) != 0xc0) {
                                            NuVecInvMtxRotate(&local_direction, &direction, reference);
                                            if (!(turret->behavior_flags & 0x80)) {
                                                desired_yaw = NuAngAdd(
                                                    static_cast<i32>(NuAtan2(-local_direction.x, -local_direction.z) *
                                                                     10430.3779296875f),
                                                    0);
                                                if (turret->yaw_upper != 0 && desired_yaw > turret->yaw_upper)
                                                    desired_yaw = turret->yaw_upper;
                                                else if (turret->yaw_lower != 0 && desired_yaw < turret->yaw_lower)
                                                    desired_yaw = turret->yaw_lower;
                                            }
                                            if (!(turret->behavior_flags & 0x40)) {
                                                NuVecRotateY(&local_direction, &local_direction, -desired_yaw);
                                                if (secondary_base != NULL) {
                                                    local_direction.y -= turret->relative_matrix.m31;
                                                    NuVecInvMtxRotate(&local_direction, &local_direction,
                                                                      &turret->relative_matrix);
                                                    local_direction.y -= turret->muzzle_offsets[0].y;
                                                }
                                                desired_pitch = NuAngAdd(
                                                    static_cast<i32>(
                                                        NuAtan2(local_direction.y,
                                                                NuFsqrt(local_direction.x * local_direction.x +
                                                                        local_direction.z * local_direction.z)) *
                                                        10430.3779296875f),
                                                    0);
                                                if (turret->pitch_upper != 0 && desired_pitch > turret->pitch_upper)
                                                    desired_pitch = turret->pitch_upper;
                                                else if (turret->pitch_lower != 0 &&
                                                         desired_pitch < turret->pitch_lower)
                                                    desired_pitch = turret->pitch_lower;
                                            }
                                        }
                                        if ((turret->behavior_flags & 0x100) && turret->primary_anim_obj != NULL &&
                                            NuSpecialExistsFn(&turret->primary_anim_obj->special) &&
                                            !NuSpecialGetOnScreenFn(&turret->primary_anim_obj->special))
                                            goto autonomous_apply;
                                        if (distance < turret->fire_range * turret->fire_range)
                                            goto try_fire;
                                    } else {
                                        // The original performs this initial home seek before the common aiming seek.
                                        turret->pitch = SeekRot(turret->pitch, 0, turret->pitch_turn_speed);
                                        turret->yaw = SeekRot(turret->yaw, home_yaw, turret->yaw_turn_speed);
                                        turret->rapid_fire_pending = 0;
                                    }
                                autonomous_apply:
                                    if (!(turret->primary_anim_obj != NULL &&
                                          NuSpecialExistsFn(&turret->primary_anim_obj->special)))
                                        desired_yaw = NuAngAdd(desired_yaw, turret->base_y_rotation);
                                    if (turret->behavior_flags & 0x400) {
                                        turret->pitch = desired_pitch;
                                        turret->yaw = desired_yaw;
                                    } else {
                                        turret->pitch = SeekRot(turret->pitch, desired_pitch, turret->pitch_turn_speed);
                                        turret->yaw = SeekRot(turret->yaw, desired_yaw, turret->yaw_turn_speed);
                                    }
                                }
                                if (turret->primary_anim_obj != NULL) {
                                    if (NuSpecialExistsFn(&turret->primary_anim_obj->special)) {
                                        if (secondary_base == NULL) {
                                            *primary_draw = *primary_base;
                                            if (turret->yaw != 0)
                                                NuMtxPreRotateY(primary_draw, turret->yaw);
                                        } else {
                                            *secondary_draw = *secondary_base;
                                            if (turret->yaw != 0)
                                                NuMtxPreRotateY(secondary_draw, turret->yaw);
                                            NuMtxMul(primary_draw, &turret->relative_matrix, secondary_draw);
                                        }
                                        if (turret->pitch != 0 && !(turret->behavior_flags & 0x40))
                                            NuMtxPreRotateX(primary_draw, turret->pitch);
                                    }
                                    if (turret->primary_anim_obj != NULL)
                                        NuSpecialUpdate(&turret->primary_anim_obj->special);
                                }
                                if (turret->secondary_anim_obj != NULL)
                                    NuSpecialUpdate(&turret->secondary_anim_obj->special);
                                if (turret->rotate_sfx != -1 && player != NULL) {
                                    bool nearby = WORLD->area != NULL && (WORLD->area->flags & 1);
                                    if (!nearby) {
                                        NUVEC delta;
                                        delta.x = reference->m30 - player->collision_position.x;
                                        delta.y = reference->m31 - player->collision_position.y;
                                        delta.z = reference->m32 - player->collision_position.z;
                                        nearby = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z < 9.0f &&
                                                 player->model_draw_result != 0;
                                        if (!nearby && player2 != NULL) {
                                            delta.x = reference->m30 - player2->collision_position.x;
                                            delta.y = reference->m31 - player2->collision_position.y;
                                            delta.z = reference->m32 - player2->collision_position.z;
                                            nearby = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z < 9.0f &&
                                                     player2->model_draw_result != 0;
                                        }
                                    }
                                    // These are absolute differences of unsigned angles, not wrapped rotation
                                    // differences.
                                    if (nearby && (abs(static_cast<i32>(static_cast<u16>(desired_pitch)) -
                                                       static_cast<i32>(static_cast<u16>(turret->pitch))) > 0x400 ||
                                                   abs(static_cast<i32>(static_cast<u16>(desired_yaw)) -
                                                       static_cast<i32>(static_cast<u16>(turret->yaw))) > 0x400)) {
                                        if (!rotation_sound_playing || IsSfxLooping(turret->rotate_sfx))
                                            GameAudio_PlaySfxById(turret->rotate_sfx,
                                                                  reinterpret_cast<NUVEC *>(&reference->m30), 0, 0);
                                        turret->rotation_sound_playing = 1;
                                    }
                                }
                                if (should_fire && bolt_type != NULL && MiniCutCam == 0 &&
                                    (turret->behavior_flags & 0x4000) == 0) {
                                    if (turret->fire_sfx != -1) {
                                        GameAudio_PlaySfxById(turret->fire_sfx,
                                                              reinterpret_cast<NUVEC *>(&reference->m30), 0, 0);
                                        BoltNoSfxPc = 1;
                                    }
                                    i32 scatter_pitch = 0, scatter_yaw = 0;
                                    if (bolt_type->field_40 != 0) {
                                        f32 spread = static_cast<f32>(static_cast<i32>(bolt_type->field_40));
                                        f32 random = NuRandFloat();
                                        scatter_pitch =
                                            static_cast<i32>(spread - ((random * spread) + (random * spread))) / 2;
                                        spread = static_cast<f32>(static_cast<i32>(bolt_type->field_40));
                                        random = NuRandFloat();
                                        scatter_yaw =
                                            static_cast<i32>(spread - ((random * spread) + (random * spread)));
                                    }
                                    i32 sound_muzzle = 0;
                                    if (turret->muzzle_count != 0)
                                        sound_muzzle = qrand() / (65535 / turret->muzzle_count + 1);
                                    turret->fired_this_frame = 1;
                                    for (i32 muzzle = 0; muzzle < turret->muzzle_count; ++muzzle) {
                                        NUVEC *offset = &turret->muzzle_offsets[muzzle];
                                        NUMTX muzzle_matrix = *primary_draw;
                                        if (offset->x != 0.0f || offset->y != 0.0f || offset->z != 0.0f) {
                                            NuMtxPreRotateY(&muzzle_matrix, 0x8000);
                                            NuMtxPreTranslate(&muzzle_matrix, offset);
                                        }
                                        NUMTX direction_matrix = *primary_draw;
                                        if (turret->behavior_flags & 0x40)
                                            NuMtxPreRotateX(&direction_matrix, turret->pitch);
                                        NuMtxPreRotateY(&direction_matrix, 0x8000);
                                        if (bolt_type->field_40 != 0) {
                                            NuMtxPreRotateX(&direction_matrix, scatter_pitch);
                                            NuMtxPreRotateY(&direction_matrix, scatter_yaw);
                                        }
                                        if (autoaim_target != NULL) {

                                            GizTurret_CalculateInterceptVector(
                                                reinterpret_cast<NUVEC *>(&muzzle_matrix.m30), primary_draw,
                                                &autoaim_target->collision_position, &autoaim_target->velocity,
                                                bolt_type->field_10, &direction, NULL, turret->controller != NULL);
                                            FindAnglesXY(&direction, NULL, NULL);
                                            NUANGVEC angles;
                                            angles.x = static_cast<u16>(temp_zrot);
                                            angles.y = static_cast<u16>(temp_yrot);
                                            NuMtxSetRotationXYVU0(&direction_matrix, &angles);
                                        }
                                        i32 flags = turret->controller != NULL ? 4 : 2;
                                        if (muzzle != sound_muzzle)
                                            BoltNoSfxPc = 1;
                                        BOLT *bolt =
                                            Bolt_Add(NULL, reinterpret_cast<NUVEC *>(&muzzle_matrix.m30),
                                                     &direction_matrix, static_cast<i8>(turret->bolt_type_id), flags);
                                        if (bolt != NULL && (turret->behavior_flags & 0x8000))
                                            bolt->flags |= 0x10;
                                    }
                                }
                            }
                }
            }
        }
    }

    i32 __cdecl GizTurrets_UpdateHint(HINT_PC *hint) {
        WORLDINFO *world = WorldInfo_CurrentlyActive();
        struct GIZTURRETSYS_s *system = world->giz_turret_sys;
        GIZTURRET_s *turret;
        i32 index;
        (void)hint;
        if (world->area != NULL && world->area == HUB_ADATA)
            return 0;
        if (VehicleArea != 0)
            return 0;
        if (system != NULL) {
            turret = system->turrets;
            for (index = 0; index < system->count; ++index, ++turret) {
                if (turret->active && turret->visible && (turret->behavior_flags & 0x4000) != 0 &&
                    (turret->behavior_flags & 0x10) == 0 && turret->opponent != NULL &&
                    NuVecDistSqr(&GameCam->position, &turret->position, NULL) < 36.0f)
                    return 1;
            }
        }
        return 0;
    }

    static i32 GizTurretPointWithinBoundsPc(GIZTURRET_s *turret, NUVEC *position) {
        NUVEC local;
        if (turret->behavior_flags & 1) {
            NuVecSub(&local, position, &turret->opponent_origin);
            NuVecRotateY(&local, &local, -(i32)turret->y_rotation);
            if (local.x > turret->opponent_bounds.x)
                return 0;
            if (local.x < -turret->opponent_bounds.x)
                return 0;
            if (local.y > turret->opponent_bounds.y)
                return 0;
            if (local.y < 0.0f)
                return 0;
            if (local.z > turret->opponent_bounds.z)
                return 0;
            if (local.z < -turret->opponent_bounds.z)
                return 0;
        }
        return 1;
    }

    void __cdecl GizTurrets_OpponentSelection(GIZTURRETSYS_s *system, i32 goody_count, APIOBJECT_PC **goodies,
                                              i32 baddy_count, APIOBJECT_PC **baddies) {
        GIZTURRET_s *turret;
        APIOBJECT_PC **candidates;
        GAMEOBJECT *candidate, *selected;
        i32 count, index;
        f32 distance, selected_distance = 1000000000.0f;
        u32 flags, candidate_flag;
        if (system == NULL || system->count == 0)
            return;
        system->opponent_cursor = (i8)(system->opponent_cursor + 1);
        if (system->opponent_cursor >= (i32)system->count)
            system->opponent_cursor = 0;
        turret = &system->turrets[system->opponent_cursor];
        if (turret->flags & 1)
            return;
        turret->opponent = NULL;
        if (!(turret->flags & 4) || !(turret->flags & 2) || (turret->flags & 0x38))
            return;
        if (turret->behavior_flags & 4) {
            candidates = baddies;
            count = baddy_count;
        } else {
            candidates = goodies;
            count = goody_count;
        }
        if (count == 0 || candidates == NULL)
            return;
        selected = NULL;
        for (index = 0; index < count; ++index) {
            candidate = (GAMEOBJECT *)candidates[index]->owner;
            flags = turret->behavior_flags;
            if (!(flags & 0x4000) && (candidate->character_data->model_flags & 0x80000))
                continue;
            if ((flags & 0x20) && candidate->rider == NULL)
                continue;
            distance = NuVecDistSqr(&candidate->collision_position, &turret->opponent_origin, NULL);
            if (!(distance < turret->opponent_range * turret->opponent_range))
                continue;
            if (!GizTurretPointWithinBoundsPc(turret, &candidate->collision_position))
                continue;
            flags = turret->behavior_flags;
            if (flags & 0x10) {
                if (selected == NULL)
                    goto select;
                candidate_flag = candidate->character_data->model_flags & 0x10;
                if (candidate_flag && !(selected->character_data->model_flags & 0x10))
                    goto select;
                if (candidate_flag != (selected->character_data->model_flags & 0x10))
                    continue;
            } else if (flags & 8) {
                if (selected == NULL)
                    goto select;
                candidate_flag = (candidate->flags_1fc_word >> 7) & 1;
                if (candidate_flag && !(selected->flags_1fc_word & 0x80))
                    goto select;
                if (candidate_flag != ((selected->flags_1fc_word >> 7) & 1))
                    continue;
            }
            if (!(distance < selected_distance))
                continue;
        select:
            selected = candidate;
            selected_distance = distance;
        }
        if (selected != NULL) {
            turret->opponent = selected;
            turret->opponent_type = 0;
        }
    }

    void __cdecl GizTurrets_Draw(void *world_pointer, void *system_pointer, f32 unused) {
        WORLDINFO *world = (WORLDINFO *)world_pointer;
        GIZTURRETSYS_s *system = (GIZTURRETSYS_s *)system_pointer;
        GIZTURRET_s *turret;
        i32 index;
        (void)unused;
        if (system == NULL)
            return;
        turret = system->turrets;
        for (index = 0; index < system->count; ++index, ++turret) {
            if (!(turret->flags & 4) || (turret->room_id >= 0 && world->rooms_visible_ptr[turret->room_id] == 0))
                continue;
            if (turret->behavior_flags & 0x1000)
                GameAnimSet_DrawReflection(turret->anim_set, 2, turret->reflection_alpha, NULL);
        }
    }

    i32 __cdecl GizTurrets_GetMaxGizmos(void *world_ptr) {
        WORLDINFO *world = (WORLDINFO *)world_ptr;
        if (world != NULL)
            return world->current_level->max_turrets;
        return 0;
    }

    void __cdecl GizTurrets_AddGizmos(GIZMOSYS_PC *giz, i32 type, void *world, void *data) {
        GIZTURRETSYS_s *s = (GIZTURRETSYS_s *)data;
        i32 i;
        (void)world;
        if (s == NULL)
            return;
        for (i = 0; i < s->count; ++i)
            if (NuStrLen(((GIZTURRET_s *)s->turrets)[i].name) != 0)
                AddGizmo(giz, type, NULL, &((GIZTURRET_s *)s->turrets)[i]);
    }

    char *__cdecl GizmoTurret_GetGizmoName(GIZMO_PC *gizmo) {
        if (gizmo != NULL)
            return ((GIZTURRET_s *)gizmo->object)->name;
        return NULL;
    }

    i32 __cdecl GizmoTurret_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
        GIZTURRET_s *turret = (GIZTURRET_s *)gizmo->object;
        (void)mode;
        switch (output) {
            case 0:
                if (turret->aim_at_fixed_angles || turret->update_disabled)
                    return 1;
                break;
            case 1:
                if ((turret->flags & 0x80) != 0)
                    return 1;
                break;
            case 2:
                if (turret->shots_fired >= turret->shots_required)
                    return 1;
                break;
        }
        return 0;
    }

    char *__cdecl GizmoTurret_GetOutputName(GIZMO_PC *gizmo, i32 output) {
        switch (output) {
            case 0:
                return (char *)TurretDestroyedOutputTextPc;
            case 1:
                return (char *)TurretFiredOutputTextPc;
            case 2: {
                GIZTURRET_s *turret = (GIZTURRET_s *)gizmo->object;
                if (turret != NULL) {
                    sprintf(TurretOutputBufferPc, TurretFiredShotsFormatPc, turret->shots_required);
                    return TurretOutputBufferPc;
                }
            }
        }
        return NULL;
    }

    i32 __cdecl GizmoTurret_GetNumOutputs(GIZMO_PC *gizmo) {
        return 3;
    }

    void *__cdecl GizTurrets_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
        return GizmoBufferAlloc(buffer, end, 0x70);
    }

    void __cdecl GizTurrets_ClearProgress(void *owner, void *progress_ptr) {
        TURRET_PROGRESS_PC *progress = (TURRET_PROGRESS_PC *)progress_ptr;
        if (progress == NULL)
            return;
        memset(progress->enabled, -1, sizeof(progress->enabled));
        memset(progress->visible, -1, sizeof(progress->visible));
        memset(progress->reward, 0, sizeof(progress->reward));
        memset(progress->fired, 0, sizeof(progress->fired));
        memset(progress->stopped, 0, sizeof(progress->stopped));
        memset(progress->completed, 0, sizeof(progress->completed));
        memset(progress->timers, 255, 64);
    }

    void __cdecl GizTurrets_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
        TURRET_PROGRESS_PC *progress = (TURRET_PROGRESS_PC *)progress_ptr;
        GIZTURRETSYS_s *system = (GIZTURRETSYS_s *)data;
        GIZTURRET_s *record;
        i32 index;
        i32 word;
        u32 bit;
        if (progress == NULL)
            return;
        memset(progress->enabled, -1, sizeof(progress->enabled));
        memset(progress->visible, -1, sizeof(progress->visible));
        memset(progress->reward, 0, sizeof(progress->reward));
        memset(progress->fired, 0, sizeof(progress->fired));
        memset(progress->stopped, 0, sizeof(progress->stopped));
        memset(progress->completed, 0, sizeof(progress->completed));
        memset(progress->timers, 255, 64);

        record = system->turrets;
        for (index = 0; index < system->count; ++index, ++record) {
            if (index >= 64)
                break;
            word = index / 32;
            bit = 1u << (index & 31);
            if (!(record->flags & 4))
                progress->visible[word] &= ~bit;
            if (!(record->flags & 2))
                progress->enabled[word] &= ~bit;
            if (record->flags & 8)
                progress->reward[word] |= bit;
            if (record->flags & 0x10)
                progress->fired[word] |= bit;
            if (record->flags & 0x20)
                progress->stopped[word] |= bit;
            if (record->flags & 0x400)
                progress->completed[word] |= bit;
            progress->timers[index] = record->state;
        }
    }

    i32 __cdecl GizTurrets_Hit(WORLDINFO *world, GIZTURRET_s *turret, NUVEC *hit_position, i32 player, i32 damage) {
        f32 judder;
        GAMEANIMOBJ *object;
        TURRET_ANIM_DATA_PC *data;
        NUVEC centre, position, direction;
        (void)hit_position;
        if ((i8)turret->state > 0) {
            if (damage == -1)
                turret->state = 0;
            else
                turret->state = (u8)(turret->state - (u8)damage);
            if ((i8)turret->state <= 0) {
                turret->state = 0;
                judder = qrand() < 0x8000 ? 0.4f : -0.4f;
                GameCam_Judder(GameCam, judder, 2, NULL);
                NewRumbleAllPlayers(1.0f, 0.0f, 0, 0);
                if (turret->behavior_flags & 0x2000) {
                    turret->flags |= 0x10;
                } else {
                    turret->flags |= 0x20;
                    GameAnimSet_SetVisibility(turret->anim_set, 0);
                    if (turret->anim_set != NULL) {
                        for (object = turret->anim_set->objects; object != NULL; object = object->next) {
                            data = (TURRET_ANIM_DATA_PC *)object->object_data;
                            if (data->role == 3)
                                NuSpecialSetVisibility(&object->special, 1);
                        }
                    }
                }
                if (turret->blowup_type != -1)
                    GizmoBlowUpTypeBlowUp(world, turret->blowup_type,
                                          NuSpecialGetDrawPos(&turret->primary_anim_obj->special));
                centre = turret->position;
                GameAnimSet_GetCentreAndRadius(turret->anim_set, &centre, NULL, 2, 1, 1);
                if (turret->destroy_sfx != -1)
                    GameAudio_PlaySfxById(turret->destroy_sfx, &centre, 0, 0);
                if (turret->completion_score != 0 &&
                    ((turret->flags & 0x400) == 0 || (turret->behavior_flags & 0x40000) != 0)) {
                    NuVecAdd(&position, &centre, &turret->pickup_offset);
                    NuVecRotateX(&direction, &v010, (u16)turret->pickup_pitch);
                    NuVecRotateY(&direction, &direction, (u16)turret->pickup_yaw);
                    AddPickups((u16)turret->completion_score, 0, 0, 0, &position, &direction, 2.0f, -1,
                               turret->pickup_radius, 2000000.0f, NULL, 1, 0);
                    turret->flags |= 0x400;
                }
                if ((turret->behavior_flags & 0x4010) == 0x4000 && LEGOHINT_SHOOTCAMERAS != -1)
                    Hint_SetComplete(LEGOHINT_SHOOTCAMERAS);
            } else if (player != -1 && (((GAMEOBJECT *)Player[player])->flags_1fc_word & 0x80) != 0) {
                NewBuzz(((GAMEOBJECT *)Player[player])->pad->bound_pad, 0.1f, 0);
            }
            return 1;
        }
        return 0;
    }

    i32 __cdecl GizTurrets_BoltHitPlat(WORLDINFO *world, void *system_pointer, BOLT_PC *bolt, u8 *hit_flags) {
        GIZTURRETSYS_s *system = (GIZTURRETSYS_s *)system_pointer;
        GIZTURRET_s *turret;
        GAMEANIMOBJ *animation;
        BOLTTYPE_PC *type;
        GAMEOBJECT *owner;
        i32 count, index, damage, player;
        if (system != NULL) {
            count = system->count;
            turret = system->turrets;
            for (index = 0; index < count; ++index, ++turret) {
                if ((turret->flags & 4) == 0 || (turret->flags & 2) == 0 || (turret->flags & 0x30) != 0 ||
                    (turret->flags & 0x200) == 0)
                    continue;
                animation = turret->anim_set->objects;
                while (animation != NULL) {
                    if (((TURRET_ANIM_DATA_PC *)animation->object_data)->platform_id == bolt->hit_platform) {
                        type = BoltType_FindByID((u8)bolt->type_id, world);
                        owner = (GAMEOBJECT *)bolt->owner;
                        if (owner != NULL && (turret->behavior_flags & 0x10000) != 0 && owner->rider == NULL)
                            damage = 0;
                        else if (owner != NULL && (turret->behavior_flags & 0x20000) != 0 &&
                                 (owner->flags_1fc_word & 0x80) == 0)
                            damage = 0;
                        else
                            damage = (u8)type->field_3c;
                        player = owner != NULL ? (i8)owner->field_280 : -1;
                        GizTurrets_Hit(world, turret, &bolt->position, player, damage);
                        return 1;
                    }
                    animation = animation->next;
                }
            }
        }
        return 0;
    }

    GIZMO_PC *__cdecl GizTurrets_GetBestBoltTarget(GIZMOSET_PC *set, f32 *output_distance, NUVEC *output_position,
                                                   NUVEC *output_velocity, void *object_pointer, NUVEC *origin,
                                                   NUVEC *direction, f32 radius, f32 range_squared, i32 directional,
                                                   i32 planar, i32 bolt_id) {
        BOLTTYPE_PC *type = BoltType_FindByID(bolt_id, WORLD);
        f32 near_distance = TargetDist_Near2;
        u16 near_angle = TargetDeg_Near;
        f32 mid_distance = TargetDist_Mid2;
        u16 far_angle = TargetDeg_Far;
        u16 mid_angle = TargetDeg_Mid;
        GAMEOBJECT *object = (GAMEOBJECT *)object_pointer;
        GIZMO_PC *gizmo;
        GIZTURRET_s *turret;
        NUVEC *position;
        GIZMO_PC *best = NULL;
        GIZMO_PC *previous = NULL;
        NUVEC *best_position = NULL;
        NUVEC *previous_position = NULL;
        f32 best_distance = 1000000000.0f;
        NUVEC direction_copy, delta;
        f32 minimum_x, minimum_z, maximum_x, maximum_z;
        f32 distance_spill, dot_spill, dx_spill, dz_spill;
        f32 distance, dot;
        u16 target_angle;
        NUANG angle;
        i32 index;
        if (set != NULL && type != NULL) {
            direction_copy = *direction;
            minimum_x = (f64)origin->x - (f64)radius;
            minimum_z = (f64)origin->z - (f64)radius;
            maximum_x = (f64)origin->x + (f64)radius;
            maximum_z = (f64)radius + (f64)origin->z;
            if (directional != 0 && (type->flags_60 & 0x20000) != 0) {
                direction_copy.y = 0.0f;
                NuVecNorm(&direction_copy, &direction_copy);
            }
            gizmo = set->gizmos;
            for (index = 0; index < set->count; ++index, ++gizmo) {
                turret = (GIZTURRET_s *)gizmo->object;
                if ((turret->flags & 4) == 0 || (turret->flags & 2) == 0 || (turret->flags & 0x30) != 0 ||
                    turret->primary_anim_obj == NULL)
                    continue;
                position = NuSpecialGetDrawPos(&turret->primary_anim_obj->special);
                if (position == NULL)
                    continue;
                if (minimum_x > position->x || maximum_x < position->x || minimum_z > position->z ||
                    maximum_z < position->z)
                    continue;
                if (planar == 0)
                    distance_spill = NuVecDistSqr(position, origin, &delta);
                else
                    distance_spill = NuVecXZDistSqr(position, origin, &delta);
                distance = distance_spill;
                if (!(range_squared > distance))
                    continue;
                if (directional == 0) {
                    dz_spill = (f64)position->z - (f64)origin->z;
                    dx_spill = (f64)position->x - (f64)origin->x;
                    angle = NuAtan2D(dx_spill, dz_spill);
                    NuVecRotateY(&delta, &v001, angle);
                }
                if ((type->flags_60 & 0x20000) != 0)
                    delta.y = 0.0f;
                NuVecNorm(&delta, &delta);
                dot_spill = NuVecDot(&delta, &direction_copy);
                dot = dot_spill;
                if (near_distance > distance && directional != 0)
                    target_angle = near_angle;
                else if (mid_distance > distance)
                    target_angle = mid_angle;
                else
                    target_angle = far_angle;
                if (!(NuTrigTable[(((i32)target_angle + 0x4000) >> 1) & 0x7fff] < dot))
                    continue;
                if (!(best_distance > distance))
                    continue;
                if (object != NULL && (void *)gizmo == (void *)object->collision_target) {
                    previous = gizmo;
                    previous_position = position;
                } else {
                    best_distance = distance;
                    best = gizmo;
                    best_position = position;
                }
            }
            if (best != NULL) {
                *output_position = *best_position;
                *output_distance = best_distance;
                output_velocity->x = 0.0f;
                output_velocity->y = 0.0f;
                output_velocity->z = 0.0f;
                return best;
            }
            if (previous != NULL) {
                *output_position = *previous_position;
                *output_distance = 1000000000.0f;
                output_velocity->x = 0.0f;
                output_velocity->y = 0.0f;
                output_velocity->z = 0.0f;
                return previous;
            }
        }
        return NULL;
    }

    i32 __cdecl GizTurrets_BoltHit(WORLDINFO *world, void *system_pointer, GAMEOBJECT *object, NUVEC *points,
                                   i32 point_count, f32 radius, NUVEC *minimum, NUVEC *maximum, BOLT_PC *bolt,
                                   i32 hit_type, u8 *hit_flags) {
        GIZTURRETSYS_s *system = (GIZTURRETSYS_s *)system_pointer;
        GIZTURRET_s *turret;
        GIZTURRET_s *best = NULL;
        BOLTTYPE_PC *type;
        GAMEOBJECT *owner;
        NUVEC *centre, *origin;
        NUVEC normal;
        f32 turret_radius_spill, distance_spill;
        f32 turret_radius, distance;
        f32 best_distance = 1000000000.0f;
        i32 index, point, player;
        if (system != NULL) {
            turret = system->turrets;
            for (index = 0; index < system->count; ++index, ++turret) {
                if ((turret->flags & 4) == 0 || (turret->flags & 2) == 0 || (turret->flags & 0x30) != 0 ||
                    turret->primary_anim_obj == NULL || (turret->flags & 0x200) != 0)
                    continue;
                turret_radius_spill = NuSpecialGetOriginRadius(&turret->primary_anim_obj->special);
                turret_radius = turret_radius_spill;
                centre = NuSpecialGetDrawPos(&turret->primary_anim_obj->special);
                if ((f64)maximum->x < (f64)centre->x - (f64)turret_radius)
                    continue;
                if ((f64)minimum->x > (f64)centre->x + (f64)turret_radius)
                    continue;
                if ((f64)maximum->z < (f64)centre->z - (f64)turret_radius)
                    continue;
                if ((f64)minimum->z > (f64)centre->z + (f64)turret_radius)
                    continue;
                if ((f64)maximum->y < (f64)centre->y - (f64)turret_radius)
                    continue;
                if ((f64)minimum->y > (f64)centre->y + (f64)turret_radius)
                    continue;
                for (point = (i32)((u32)point_count - 1u); point >= 0; --point) {
                    if (SphereSphereOverlap(centre, turret_radius, &points[point], radius) != 0) {
                        origin = object != NULL ? &object->collision_position : &points[point];
                        distance_spill = NuVecDistSqr(origin, centre, NULL);
                        distance = distance_spill;
                        if (distance < best_distance) {
                            best_distance = distance;
                            best = turret;
                        }
                        break;
                    }
                }
            }
        }
        if (best != NULL) {
            if (hit_type != 2 && hit_type != 7 && bolt != NULL) {
                (void)BoltType_FindByID((u8)bolt->type_id, WORLD);
                (void)Cheats_CheckFlags(2);
            }
            type = BoltType_FindByID((u8)bolt->type_id, world);
            owner = (GAMEOBJECT *)bolt->owner;
            player = owner != NULL ? (i8)owner->field_280 : -1;
            if (GizTurrets_Hit(world, best, &bolt->position, player, (u8)type->field_3c) != 0) {
                if (object != NULL) {
                    NewRumble(object->pad->bound_pad, 0.4f, 0);
                    GameCam_HitJudder();
                }
            } else {
                NuVecSub(&normal, &best->position, &bolt->position);
                NuVecNorm(&normal, &normal);
                Bolt_AddDeflectedBolt(bolt, &bolt->direction, &normal, hit_flags);
            }
            if (BoltSys->stop_targeting != NULL)
                BoltSys->stop_targeting(object, points);
            return 1;
        }
        return 0;
    }

    u32 __cdecl GizTurrets_TotalScore(WORLDINFO *world) {
        GIZTURRETSYS_s *system;
        GIZTURRET_s *turret;
        u32 total = 0;
        i32 i;
        system = (GIZTURRETSYS_s *)world->giz_turret_sys;
        if (system != NULL) {
            turret = system->turrets;
            if (turret != NULL) {
                for (i = 0; i < system->count; i++, turret++)
                    total += (u16)turret->completion_score;
            }
        }
        return total;
    }

    ADDGIZMOTYPE_PC *__cdecl GizTurrets_RegisterGizmo(i32 type_id) {
        ADDGIZMOTYPE_PC *addtype = &GizmoRegistryGizTurretsAddTypePc;
        *addtype = Default_ADDGIZMOTYPE;
        addtype->name = GizmoRegistryGizTurretsNamePtrPc;
        addtype->fns.early_update_fn = NULL;
        addtype->fns.panel_draw_fn = NULL;
        addtype->fns.get_visibility_fn = NULL;
        addtype->fns.using_special_fn = NULL;
        addtype->prefix = "";
        addtype->fns.options = 0x70;
        addtype->fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))GizTurrets_GetMaxGizmos;
        addtype->fns.add_gizmos_fn = (void *)GizTurrets_AddGizmos;
        addtype->fns.late_update_fn = (void(__cdecl *)(void *, void *, f32))GizTurrets_Update;
        addtype->fns.draw_fn = (void(__cdecl *)(void *, void *, f32))GizTurrets_Draw;
        addtype->fns.get_gizmo_name_fn = (char *(__cdecl *)(GIZMO_PC *))GizmoTurret_GetGizmoName;
        addtype->fns.get_output_fn = (i32(__cdecl *)(GIZMO_PC *, i32, i32))GizmoTurret_GetOutput;
        addtype->fns.get_output_name_fn = (char *(__cdecl *)(GIZMO_PC *, i32))GizmoTurret_GetOutputName;
        addtype->fns.get_num_outputs_fn = (void *)GizmoTurret_GetNumOutputs;
        addtype->fns.activate_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizmoTurret_Activate;
        addtype->fns.activate_rev_fn = (i32(__cdecl *)(GIZMO_PC *, i32, i32))GizmoTurret_ActivateRev;
        addtype->fns.set_visibility_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizmoTurret_SetVisibility;
        addtype->fns.get_pos_fn = (void *)GizmoTurret_GetPos;
        addtype->fns.bolt_hit_plat_fn = (void *)GizTurrets_BoltHitPlat;
        addtype->fns.get_best_bolt_target_fn =
            (GIZMO_PC * (__cdecl *)(GIZMOSET_PC *, f32 *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, f32, f32, i32,
                                    i32, i32)) GizTurrets_GetBestBoltTarget;
        addtype->fns.bolt_hit_fn = (void *)GizTurrets_BoltHit;
        addtype->fns.allocate_progress_data_fn =
            (void *(__cdecl *)(VARIPTR *, VARIPTR *))GizTurrets_AllocateProgressData;
        addtype->fns.clear_progress_fn = (void(__cdecl *)(void *, void *))GizTurrets_ClearProgress;
        addtype->fns.store_progress_fn = (void(__cdecl *)(void *, void *, void *))GizTurrets_StoreProgress;
        addtype->fns.reset_fn = (void(__cdecl *)(void *, void *, void *))GizTurrets_ResetPc;
        addtype->fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))GizTurrets_ReserveBufferSpace;
        addtype->fns.load_fn = (i32(__cdecl *)(void *, void *))GizTurrets_Load;
        addtype->fns.post_load_fn = (void(__cdecl *)(void *, void *))GizTurrets_PostLoad;
        addtype->fns.add_level_sfx_fn = (void *)GizTurrets_AddLevelSfx;
        turret_gizmotype_id = type_id;
        return addtype;
    }
}
