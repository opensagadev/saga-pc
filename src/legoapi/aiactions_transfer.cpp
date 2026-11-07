#include "saga.h"

extern "C" i32 special_debug;
extern "C" void __cdecl PCSpecialEmptyHandler_candidate(void);
extern "C" {
    AI_HELPER_SLOT_PC *__cdecl AIPathCnxHelperSys_AddHelper(AI_HELPER_SYSTEM_PC *system, AIPATHCNX_PC *connection,
                                                            u8 direction, void *target, u8 type) {
        if (system == NULL || system->count >= system->capacity || connection == NULL || target == NULL)
            return NULL;
        i32 count = system->count;
        AI_HELPER_SLOT_PC *helper = system->helpers;
        for (i32 index = 0; index < count; ++index, ++helper) {
            if (helper->connection == connection && helper->type == type && helper->target == target) {
                if (helper->direction != direction)
                    helper->direction = 0xff;
                return helper;
            }
        }
        helper = &system->helpers[count];
        ++system->count;
        helper->connection = connection;
        helper->direction = direction;
        helper->target = target;
        helper->type = type;
        return helper;
    }
    i32 __cdecl CharCategory_FindByName(char *name) {
        if (CharCategory != NULL) {
            for (i32 index = 0; index < CHARCATEGORYCOUNT; ++index)
                if (NuStrICmp(CharCategory[index].name, name) == 0)
                    return index;
        }
        return -1;
    }
    SOCK_PC *__cdecl FindSock(SOCKSYS_PC *sys, char *name) {
        if (sys != NULL) {
            SOCK_PC *sock = sys->socks;
            for (i32 index = 0; index < 64; ++index, ++sock)
                if (sock->valid && NuStrICmp(name, sock->name) == 0)
                    return sock;
        }
        return NULL;
    }
    void __cdecl SockOn(SOCKSYS_PC *sys, i32 index);
    void __cdecl SockOff(SOCKSYS_PC *sys, i32 index);
}

GAMEOBJECT *GizTurret_GetTgt(GIZTURRET_s *, NUMTX *matrix) {
    NUVEC forward = {0.0f, 0.0f, -1.0f};

    NuVecMtxRotate(&forward, &forward, matrix);
    GAMEOBJECT *best = NULL;
    f32 best_distance = 1000000000.0f;
    GAMEOBJECT *object = Obj;
    for (i32 i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        if ((object->flags_1fc_half & 0x1001) != 0x1001 || object->dead != 0 || (object->flags_1f8 & 1) == 0 ||
            object->model_draw_result == 0) {
            continue;
        }

        NUVEC direction;
        const f32 distance =
            NuVecDistSqr(&object->collision_position, reinterpret_cast<NUVEC *>(&matrix->m30), &direction);
        if (!(best_distance > distance)) {
            continue;
        }
        const f32 length = NuFsqrt(distance);
        const f32 scale = length > 0.0f ? 1.0f / length : 0.0f;
        direction.x *= scale;
        direction.y *= scale;
        direction.z *= scale;
        if (!(NuVecDot(&direction, &forward) > NU_COS_LUT(gizturret_test_ang))) {
            continue;
        }
        best = object;
        best_distance = distance;
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
    const f32 distance = NuVecDistSqr(&intercept, origin, &direction);
    const f32 length = NuFsqrt(distance);
    const f32 scale = length > 0.0f ? 1.0f / length : 0.0f;
    direction.x *= scale;
    direction.y *= scale;
    direction.z *= scale;

    if (NuVecDot(&direction, &forward) > NU_COS_LUT(gizturret_test_ang) || fallback == 0) {
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
                                        desired_pitch = turret->field_0x58;
                                        pitch_rate = -pitch_rate;
                                    } else if (pitch_rate > 0.0f)
                                        desired_pitch = turret->field_0x5c;
                                    else
                                        desired_pitch = turret->pitch;
                                    f32 yaw_rate = -pad->input_direction_z * turret->yaw_turn_speed;
                                    if (yaw_rate < 0.0f) {
                                        desired_yaw = turret->field_0x64;
                                        yaw_rate = -yaw_rate;
                                    } else if (yaw_rate > 0.0f)
                                        desired_yaw = turret->field_0x68;
                                    else
                                        desired_yaw = turret->yaw;
                                    turret->pitch = SeekRot(turret->pitch, desired_pitch, pitch_rate);
                                    turret->yaw = SeekRot(turret->yaw, desired_yaw, yaw_rate);
                                    if (turret->fire_cooldown > 0.0f &&
                                        (turret->controller->pad->buttons_pressed & GAMEPAD_ACTION)) {
                                        if (turret->fire_cooldown > turret->fire_interval * gizturret_rapid_fire_rate)
                                            turret->fire_cooldown = turret->fire_interval * gizturret_rapid_fire_rate;
                                        turret->rapid_fire_pending = 1;
                                    }
                                    if (turret->fire_cooldown == 0.0f &&
                                        ((turret->controller->pad->buttons_held & GAMEPAD_ACTION) ||
                                         (turret->rapid_fire_pending))) {
                                        should_fire = 1;
                                        if (turret->rapid_fire_pending)
                                            turret->fire_cooldown = turret->fire_interval * gizturret_rapid_fire_rate;
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
                                    desired_pitch = turret->field_0x6c;
                                    desired_yaw = turret->field_0x70;
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
                                if (turret->field_0xe4 != NULL) {
                                    NUVEC *target_position, *target_velocity = NULL;
                                    NUVEC local_direction;
                                    f32 distance;
                                    switch (static_cast<i8>(turret->field_0x12c)) {
                                        case 0: {
                                            GAMEOBJECT *target = static_cast<GAMEOBJECT *>(turret->field_0xe4);
                                            target_velocity = &target->velocity;
                                            target_position = &target->collision_position;
                                            break;
                                        }
                                        case 1:
                                            target_position = reinterpret_cast<NUVEC *>(
                                                static_cast<u8 *>(turret->field_0xe4) + 0x11c);
                                            break;
                                        case 2:
                                            target_position = static_cast<NUVEC *>(turret->field_0xe4);
                                            break;
                                        default:
                                            goto autonomous_apply;
                                    }
                                    distance = NuVecDistSqr(target_position, &turret->field_0x3c, &local_direction);
                                    if (target_position == NULL)
                                        goto autonomous_apply;
                                    if ((turret->behavior_flags & 0x200) && target_velocity != NULL &&
                                        bolt_type != NULL) {
                                        GizTurret_CalculateInterceptVector(
                                            reinterpret_cast<NUVEC *>(&reference->m30), primary_draw, target_position,
                                            target_velocity, bolt_type->field_10, &direction, NULL,
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
                                            if (turret->field_0x64 != 0 && desired_yaw > turret->field_0x64)
                                                desired_yaw = turret->field_0x64;
                                            else if (turret->field_0x68 != 0 && desired_yaw < turret->field_0x68)
                                                desired_yaw = turret->field_0x68;
                                        }
                                        if (!(turret->behavior_flags & 0x40)) {
                                            NuVecRotateY(&local_direction, &local_direction, -desired_yaw);
                                            if (secondary_base != NULL) {
                                                local_direction.y -= turret->field_0xa4.m31;
                                                NuVecInvMtxRotate(&local_direction, &local_direction,
                                                                  &turret->field_0xa4);
                                                local_direction.y -= turret->field_0x74[0].y;
                                            }
                                            desired_pitch =
                                                NuAngAdd(static_cast<i32>(
                                                             NuAtan2(local_direction.y,
                                                                     NuFsqrt(local_direction.x * local_direction.x +
                                                                             local_direction.z * local_direction.z)) *
                                                             10430.3779296875f),
                                                         0);
                                            if (turret->field_0x58 != 0 && desired_pitch > turret->field_0x58)
                                                desired_pitch = turret->field_0x58;
                                            else if (turret->field_0x5c != 0 && desired_pitch < turret->field_0x5c)
                                                desired_pitch = turret->field_0x5c;
                                        }
                                    }
                                    if ((turret->behavior_flags & 0x100) && turret->primary_anim_obj != NULL &&
                                        NuSpecialExistsFn(&turret->primary_anim_obj->special) &&
                                        !NuSpecialGetOnScreenFn(&turret->primary_anim_obj->special))
                                        goto autonomous_apply;
                                    if (distance < turret->field_0xf0 * turret->field_0xf0)
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
                                        NuMtxMul(primary_draw, &turret->field_0xa4, secondary_draw);
                                    }
                                    if (turret->pitch != 0 && !(turret->behavior_flags & 0x40))
                                        NuMtxPreRotateX(primary_draw, turret->pitch);
                                }
                                if (turret->primary_anim_obj != NULL)
                                    NuSpecialUpdate(&turret->primary_anim_obj->special);
                            }
                            if (turret->secondary_anim_obj != NULL)
                                NuSpecialUpdate(&turret->secondary_anim_obj->special);
                            if (turret->field_0x126 != -1 && player != NULL) {
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
                                // These are absolute differences of unsigned angles, not wrapped rotation differences.
                                if (nearby && (abs(static_cast<i32>(static_cast<u16>(desired_pitch)) -
                                                   static_cast<i32>(static_cast<u16>(turret->pitch))) > 0x400 ||
                                               abs(static_cast<i32>(static_cast<u16>(desired_yaw)) -
                                                   static_cast<i32>(static_cast<u16>(turret->yaw))) > 0x400)) {
                                    if (!rotation_sound_playing || IsSfxLooping(turret->field_0x126))
                                        GameAudio_PlaySfxById(turret->field_0x126,
                                                              reinterpret_cast<NUVEC *>(&reference->m30), 0, 0);
                                    turret->rotation_sound_playing = 1;
                                }
                            }
                            if (should_fire && bolt_type != NULL && MiniCutCam == 0 &&
                                (turret->behavior_flags & 0x4000) == 0) {
                                if (turret->field_0x12a != -1) {
                                    GameAudio_PlaySfxById(turret->field_0x12a,
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
                                    scatter_yaw = static_cast<i32>(spread - ((random * spread) + (random * spread)));
                                }
                                i32 sound_muzzle = 0;
                                if (turret->field_0x130 != 0)
                                    sound_muzzle = qrand() / (65535 / turret->field_0x130 + 1);
                                turret->fired_this_frame = 1;
                                for (i32 muzzle = 0; muzzle < turret->field_0x130; ++muzzle) {
                                    NUVEC *offset = &turret->field_0x74[muzzle];
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
                                        Bolt_Add(NULL, reinterpret_cast<NUVEC *>(&muzzle_matrix.m30), &direction_matrix,
                                                 static_cast<i8>(turret->bolt_type_id), flags);
                                    if (bolt != NULL && (turret->behavior_flags & 0x8000))
                                        bolt->flags |= 0x10;
                                }
                            }
                        }
            }
        }
    }
}

static i32 NuSpecialFindMultiDisplayPc(NUGSCN *scene, NUSPECIALHANDLE *dest, char *name, i32 capacity, i32 flags) {
    i32 count = 0;
    NUDISPLAY_SPECIAL_PC *special = scene->display_list->specials;
    for (i32 index = 0; index < scene->display_list->special_count; ++index, ++special) {
        if (NuStrIStr(special->name, name) != NULL) {
            dest->scene = scene;
            dest->display_special = (NUMTX *)special;
            dest->special = NULL;
            ++dest;
            if (++count >= capacity)
                return count;
        }
    }
    if (flags != 0 && special_debug != 0)
        PCSpecialEmptyHandler_candidate();
    return count;
}
extern "C" i32 __cdecl NuSpecialFindMulti(NUGSCN *scene, NUSPECIALHANDLE *dest, char *name, i32 capacity, i32 flags) {
    i32 count = 0;
    if (scene == NULL) {
        PCSpecialEmptyHandler_candidate();
        return 0;
    }
    if (scene->display_list != NULL)
        return NuSpecialFindMultiDisplayPc(scene, dest, name, capacity, flags);
    NUSPECIAL_LEGACY_PC *special = scene->specials;
    for (i32 index = 0; index < scene->special_count; ++index, ++special) {
        if (NuStrIStr(special->name, name) != NULL) {
            dest->scene = scene;
            dest->special = (NUMTX *)special;
            dest->display_special = NULL;
            ++dest;
            if (++count >= capacity)
                return count;
        }
    }
    if (flags != 0 && special_debug != 0)
        PCSpecialEmptyHandler_candidate();
    return count;
}

extern "C" {}

extern "C" {}

extern "C" {}

extern "C" {
    u32 __cdecl AIPathParseBatmanFlagPc(char *name) {
        if (NuStrICmp(name, "JUMP") == 0)
            return 1;
        if (NuStrICmp(name, "COMBATROLL") == 0)
            return 2;
        if (NuStrICmp(name, "BATGLIDE") == 0)
            return 4;
        if (NuStrICmp(name, "GRAPPLE") == 0)
            return 8;
        if (NuStrICmp(name, "USEHATCH") == 0)
            return 0x10;
        if (NuStrICmp(name, "HOVERTUBE") == 0)
            return 0x40;
        if (NuStrICmp(name, "FORGOODIES") == 0)
            return 0x80;
        if (NuStrICmp(name, "FORBADDIES") == 0)
            return 0x100;
        if (NuStrICmp(name, "OBSTACLE") == 0)
            return 0x20000000;
        if (NuStrICmp(name, "JUMP_NOW") == 0)
            return 0x400;
        if (NuStrICmp(name, "DONT_JUMP_NOW") == 0)
            return 0x800;
        return 0;
    }
    u32 __cdecl AIPathParseIndyFlagPc(char *name) {
        if (NuStrICmp(name, "DBLJUMP") == 0)
            return 2;
        if (NuStrICmp(name, "BIGJUMP") == 0)
            return 4;
        if (NuStrICmp(name, "JUMP") == 0)
            return 1;
        if (NuStrICmp(name, "GRAPPLE") == 0)
            return 8;
        if (NuStrICmp(name, "CLIMB") == 0)
            return 0x10;
        if (NuStrICmp(name, "NO_DEST_CHECK") == 0)
            return 0x20;
        if (NuStrICmp(name, "JUMP_NOW") == 0)
            return 0x40;
        if (NuStrICmp(name, "DONT_JUMP_NOW") == 0)
            return 0x80;
        if (NuStrICmp(name, "FORGOODIES") == 0)
            return 0x100;
        if (NuStrICmp(name, "FORBADDIES") == 0)
            return 0x200;
        if (NuStrICmp(name, "OBSTACLE") == 0)
            return 0x20000000;
        if (NuStrICmp(name, "WHIPSWING") == 0)
            return 0x400;
        return 0;
    }
    u32 __cdecl AIPathParseStarWarsFlagPc(char *name) {
        if (NuStrICmp(name, "DBLJUMP") == 0)
            return 2;
        if (NuStrICmp(name, "R2D2GLIDE") == 0)
            return 4;
        if (NuStrICmp(name, "ZIPUP") == 0)
            return 8;
        if (NuStrICmp(name, "USEHATCH") == 0)
            return 0x10;
        if (NuStrICmp(name, "JARJARJUMP") == 0)
            return 0x20;
        if (NuStrICmp(name, "HOVERTUBE") == 0)
            return 0x40;
        if (NuStrICmp(name, "SWAMP") == 0)
            return 0x4000;
        if (NuStrICmp(name, "TAKEOVER") == 0)
            return 0x8000;
        if (NuStrICmp(name, "VEHICLE") == 0)
            return 0x10000;
        if (NuStrICmp(name, "PARTY") == 0)
            return 0x20000;
        if (NuStrICmp(name, "FORGOODIES") == 0)
            return 0x80;
        if (NuStrICmp(name, "FORBADDIES") == 0)
            return 0x100;
        if (NuStrICmp(name, "OBSTACLE") == 0)
            return 0x20000000;
        if (NuStrICmp(name, "JUMP_NOW") == 0)
            return 0x400;
        if (NuStrICmp(name, "DONT_JUMP_NOW") == 0)
            return 0x800;
        if (NuStrICmp(name, "BLOCKAGE") == 0)
            return 0x40000;
        if (NuStrICmp(name, "JUMP") == 0)
            return 1;
        if (NuStrICmp(name, "DONTTOGGLE") == 0)
            return 0x80000;
        if (NuStrICmp(name, "FULLTERRAIN") == 0)
            return 0x100000;
        return 0;
    }
}

extern "C" {
    static i32 Ledges_Load(void *world_info, void *) {
        WORLDINFO *world = static_cast<WORLDINFO *>(world_info);
        if (world->ledge_count != 0)
            return 0;
        i32 version = EdFileReadInt();
        world->ledge_count = EdFileReadInt();
        for (i32 i = 0; i < world->ledge_count; ++i) {
            LEDGE_s *ledge = &static_cast<LEDGE_s *>(world->ledges)[i];
            EdFileRead(ledge->name, 8);
            EdFileReadNuVec(&ledge->position);
            ledge->y_rotation = EdFileReadShort();
            ledge->type_code = EdFileReadChar();
            if (version > 1) {
                ledge->field_0x1c = EdFileReadShort();
                ledge->field_0x1e = EdFileReadShort();
                if (version == 2)
                    ledge->flags = 0;
                else
                    ledge->flags = EdFileReadUnsignedChar();
            } else {
                ledge->field_0x1c = -1;
                ledge->field_0x1e = -1;
                ledge->flags = 0;
            }
        }
        return 1;
    }
}

extern "C" {}

extern "C" {
    void *__cdecl CutScene_FindInst(CUTSYS_PC *system, char *name) {
        if (system != NULL) {
            for (i32 index = 0; index < system->count; ++index)
                if (NuStrICmp(system->records[index]->name, name) == 0)
                    return system->records[index]->instance;
        }
        return NULL;
    }
}
typedef struct bulk_ai_panel_pc_s {
    u8 unknown_00[0x50];
    NUVEC position;
    u8 unknown_5c[5];
    u8 model_variant;
    u8 flags;
    u8 unknown_63;
    NUVEC floor_position;
} BULK_AI_PANEL_PC;
STATIC_ASSERT(offsetof(BULK_AI_PANEL_PC, position) == 0x50);
STATIC_ASSERT(offsetof(BULK_AI_PANEL_PC, model_variant) == 0x61);
STATIC_ASSERT(offsetof(BULK_AI_PANEL_PC, flags) == 0x62);
STATIC_ASSERT(offsetof(BULK_AI_PANEL_PC, floor_position) == 0x64);
typedef struct bulk_ai_full_object_pc_s {
    APIOBJECT_PC object;
    u8 trailing[0x2b8 - sizeof(APIOBJECT_PC)];
} BULK_AI_FULL_OBJECT_PC;
STATIC_ASSERT(sizeof(BULK_AI_FULL_OBJECT_PC) == 0x2b8);
typedef struct bulk_ai_full_packet_pc_s {
    AIPACKET_PC packet;
    u8 trailing[0x210 - sizeof(AIPACKET_PC)];
} BULK_AI_FULL_PACKET_PC;
STATIC_ASSERT(sizeof(BULK_AI_FULL_PACKET_PC) == 0x210);
extern "C" {
    void __cdecl AISysGetPathPos2(AISYS_PC *system, NUVEC *position, AIPATHINFO_PC *path_info, NUVEC *path_position,
                                  AIPATH_PC *, i32) {
        if (position != NULL && system != NULL && path_info != NULL) {
            BULK_AI_FULL_OBJECT_PC full_object;
            BULK_AI_FULL_PACKET_PC full_packet;
            AIPACKET_PC &packet = full_packet.packet;
            memset(&full_object, 0, sizeof(full_object));
            memset(&full_packet, 0, sizeof(full_packet));
            APIOBJECT_PC *object = &full_object.object;
            object->ai = &packet;
            object->position = *position;
            packet.terrain_origin = *position;
            AISysGetCharacterPathPos(system, object, &packet, 0xff, 1);
            *path_info = packet.path_info;
            if (path_position != NULL)
                *path_position = packet.last_path_position;
        }
    }
    i32 __cdecl GizPanel_CanUsePanel(GAMEOBJECT *object, BULK_AI_PANEL_PC *panel) {
        if (panel == NULL || object == NULL)
            return 0;
        if (SuperWeirdo(object) != 0)
            return 1;
        switch (panel->model_variant) {
            case 0:
                if ((object->character_data->model_flags & 0x40) != 0 ||
                    (object->character_data->model_flags & 0x1000010) == 0x1000010)
                    return 1;
                break;
            case 1:
                if ((object->character_data->model_flags & 0x20) != 0 ||
                    (object->character_data->model_flags & 0x1000010) == 0x1000010)
                    return 1;
                break;
            case 3:
                if (object->helmet == 5 || (object->character_data->game_character->flags_090 & 0x40000) != 0)
                    return 1;
                if (GCDataList[(i16)object->id].weapon_action == 1 || GCDataList[(i16)object->id].weapon_action == 8)
                    return 1;
                if ((i16)object->id == FreePlayId007f199cPc || (i16)object->id == id_THEEMPEROR ||
                    (i16)object->id == FreePlayId007f1a4cPc || (i16)object->id == FreePlayId007f1a48Pc ||
                    (i16)object->id == FreePlayId007f1a44Pc)
                    return 1;
                break;
            case 2:
                if ((object->character_data->model_flags & 0x1000000) != 0 || object->helmet == 6)
                    return 1;
                break;
        }
        return 0;
    }
}

STATIC_ASSERT(offsetof(ZIPUP_s, flags) == 0x68);
STATIC_ASSERT(offsetof(ZIPUP_s, runtime_flags) == 0x69);
STATIC_ASSERT(offsetof(ZIPUP_s, lower_position) == 0x10);
STATIC_ASSERT(offsetof(ZIPUP_s, hook_origin) == 0x1c);
STATIC_ASSERT(offsetof(ZIPUP_s, upper_position) == 0x28);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, run_speed) == 0x1c);
extern "C" void __cdecl StartBallooning(GAMEOBJECT *, i32);

extern "C" {}

extern "C" struct AIPATHCNXCONTROLLER_s *__cdecl AIPathCnxControllerCreate(AI_CONTROL_SYSTEM_PC *, AISYS_PC *,
                                                                           AIPATH_PC *, char *, char *, i32, char *,
                                                                           i32, i32);
