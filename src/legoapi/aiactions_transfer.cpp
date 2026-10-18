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
    void __cdecl SockOn(SOCKSYS_PC *sys, i32 index) {
        if (sys != NULL && (u32)index < 64 && (sys->socks[index].flags & 0x100) != 0)
            sys->socks[index].flags &= ~0x100;
    }
    void __cdecl SockOff(SOCKSYS_PC *sys, i32 index) {
        if (sys != NULL && (u32)index < 64 && (sys->socks[index].flags & 0x100) == 0)
            sys->socks[index].flags |= 0x100;
    }
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
    bool should_fire;
    GIZTURRETSYS_s *system = static_cast<GIZTURRETSYS_s *>(system_ptr);
    if (system != NULL) {
        GIZTURRET_s *turret = system->turrets;
        if (system->count != 0) {
            i32 index = 0;
            do {
                turret->fired_this_frame = 0;
                u8 rotation_sound_playing = turret->rotation_sound_playing;
                turret->rotation_sound_playing = 0;
                if (turret->visible)
                    if (turret->active)
                        if (!turret->update_disabled) {
                            BOLTTYPE_PC *bolt_type = BoltType_FindByID(static_cast<i8>(turret->bolt_type_id),
                                                                       static_cast<WORLDINFO *>(context));
                            NUMTX fallback_draw, fallback_base;
                            NUMTX *primary_draw, *primary_base, *secondary_draw = NULL, *secondary_base = NULL,
                                                                *reference;
                            u16 home_yaw = 0;
                            i32 base_yaw = 0;
                            if (turret->primary_anim_obj != NULL &&
                                NuSpecialExistsFn(&turret->primary_anim_obj->special)) {
                                primary_draw = NuSpecialGetDrawMtx(&turret->primary_anim_obj->special);
                                primary_base = NuSpecialGetMtx(&turret->primary_anim_obj->special);
                                if (turret->secondary_anim_obj != NULL &&
                                    NuSpecialExistsFn(&turret->secondary_anim_obj->special)) {
                                    secondary_base = NuSpecialGetMtx(&turret->secondary_anim_obj->special);
                                    secondary_draw = NuSpecialGetDrawMtx(&turret->secondary_anim_obj->special);
                                    reference = secondary_base;
                                } else
                                    reference = primary_base;
                            } else {
                                NuMtxSetTranslation(&fallback_draw, &turret->position);
                                NuMtxPreRotateX(&fallback_draw, turret->pitch);
                                NuMtxPreRotateY(&fallback_draw, turret->yaw);
                                NuMtxSetTranslation(&fallback_base, &turret->position);
                                NuMtxPreRotateY(&fallback_base, turret->base_y_rotation);
                                primary_draw = &fallback_draw;
                                primary_base = NULL;
                                reference = &fallback_base;
                                home_yaw = turret->base_y_rotation;
                                base_yaw = static_cast<i16>(home_yaw);
                            }
                            turret->fire_cooldown -= frame_time;
                            if (turret->fire_cooldown < 0.0f)
                                turret->fire_cooldown = 0.0f;
                            i32 desired_pitch = 0, desired_yaw = base_yaw;
                            should_fire = false;
                            GAMEOBJECT *autoaim_target = NULL;
                            if (turret->controller != NULL) {
                                GAMEOBJECT *controller = turret->controller;
                                if (controller->batarang != NULL && static_cast<i8>(controller->flags_1fc_half) < 0) {
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
                                        f32 rapid_interval = turret->fire_interval * gizturret_rapid_fire_rate;
                                        if (turret->fire_cooldown > rapid_interval)
                                            turret->fire_cooldown = rapid_interval;
                                        turret->rapid_fire_pending = 1;
                                    }
                                    if (turret->fire_cooldown == 0.0f &&
                                        ((turret->controller->pad->buttons_held & GAMEPAD_ACTION) ||
                                         (turret->rapid_fire_pending))) {
                                        turret->fire_cooldown = turret->fire_interval;
                                        if (turret->rapid_fire_pending)
                                            turret->fire_cooldown *= gizturret_rapid_fire_rate;
                                        turret->rapid_fire_pending = 0;
                                        if (turret->behavior_flags & 0x200)
                                            autoaim_target = GizTurret_GetTgt(turret, primary_draw);
                                        should_fire = bolt_type != NULL;
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
                                    if (turret->fire_cooldown == 0.0f) {
                                        turret->fire_cooldown = (NuRandFloat() + 0.5f) * turret->fire_interval;
                                        should_fire = bolt_type != NULL;
                                    }
                                    goto autonomous_apply;
                                }
                                // The original performs this initial home seek before the common aiming seek.
                                if (turret->field_0xe4 == NULL) {
                                    turret->pitch = SeekRot(turret->pitch, 0, turret->pitch_turn_speed);
                                    turret->yaw = SeekRot(turret->yaw, home_yaw, turret->yaw_turn_speed);
                                    turret->rapid_fire_pending = 0;
                                    goto autonomous_apply;
                                }
                                {
                                    NUVEC *target_position, *target_velocity = NULL;
                                    NUVEC local_direction, world_direction;
                                    f32 distance;
                                    switch (turret->field_0x12c) {
                                        case 0: {
                                            GAMEOBJECT *target = static_cast<GAMEOBJECT *>(turret->field_0xe4);
                                            target_velocity = &target->velocity;
                                            target_position = &target->collision_position;
                                            distance =
                                                NuVecDistSqr(target_position, &turret->field_0x3c, &local_direction);
                                            break;
                                        }
                                        case 1:
                                            target_position = reinterpret_cast<NUVEC *>(
                                                static_cast<u8 *>(turret->field_0xe4) + 0x11c);
                                            distance =
                                                NuVecDistSqr(target_position, &turret->field_0x3c, &local_direction);
                                            break;
                                        case 2:
                                            target_position = static_cast<NUVEC *>(turret->field_0xe4);
                                            distance =
                                                NuVecDistSqr(target_position, &turret->field_0x3c, &local_direction);
                                            break;
                                        default:
                                            goto autonomous_apply;
                                    }
                                    should_fire = bolt_type != NULL;
                                    if ((turret->behavior_flags & 0x200) && target_velocity != NULL &&
                                        bolt_type != NULL) {
                                        GizTurret_CalculateInterceptVector(
                                            reinterpret_cast<NUVEC *>(&reference->m30), primary_draw, target_position,
                                            target_velocity, bolt_type->field_10, &world_direction, NULL,
                                            turret->controller != NULL);
                                        should_fire = true;
                                    } else
                                        NuVecSub(&world_direction, target_position,
                                                 reinterpret_cast<NUVEC *>(&reference->m30));
                                    if ((turret->behavior_flags & 0xc0) != 0xc0) {
                                        NuVecInvMtxRotate(&local_direction, &world_direction, reference);
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
                                        should_fire = false;
                                    else if (distance < turret->field_0xf0 * turret->field_0xf0 &&
                                             turret->fire_cooldown == 0.0f)
                                        turret->fire_cooldown = (NuRandFloat() + 0.5f) * turret->fire_interval;
                                    else
                                        should_fire = false;
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
                            if (should_fire && MiniCutCam == 0 && (turret->behavior_flags & 0x4000) == 0) {
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
                                        angles.x = static_cast<u16>(temp_xrot);
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
            } while (++index, ++turret, index < static_cast<u16>(system->count));
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

extern "C" {
    i32 __cdecl Action_EngageObject(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
        (void)sys;
        if (packet == NULL || packet->owner == NULL || (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner == NULL) {
            return 1;
        }

        GAMEOBJECT *object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
        if (param_5 != 0) {
            packet->movement_instruction_parameter = idealgoalrange;
            processor->action_data_5 = engagefiretime;

            char *name = NULL;
            i32 nearest = 0;
            for (i32 index = 0; index < param_4; ++index) {
                char *value = NuStrIStr(params[index], (char *)"name");
                if (value != NULL) {
                    name = value + 5;
                } else if ((value = NuStrIStr(params[index], (char *)"fireinterval")) != NULL) {
                    processor->action_data_5 = AIParamToFloat(processor, value + 13);
                } else if (NuStrIStr(params[index], (char *)"nearest") != NULL) {
                    nearest = 1;
                } else if (NuStrIStr(params[index], (char *)"predictive") != NULL) {
                    processor->action_data_1 = 1;
                }
            }

            processor->action_timer = NuRandFloat() * processor->action_data_5;
            if (name != NULL) {
                if (nearest != 0) {
                    NUSPECIALHANDLE specials[50];
                    const i32 special_count = NuSpecialFindMulti(WORLD->current_gscn, specials, name, 50, 0);
                    if (special_count > 0) {
                        f32 nearest_distance = 1.0e9f;
                        NUVEC difference;
                        for (i32 index = 0; index < special_count; ++index) {
                            NUVEC *position = NuSpecialGetDrawPos(&specials[index]);
                            const f32 distance =
                                NuVecDistSqr(&((APIOBJECT_PC *)packet->owner)->position, position, &difference);
                            if (distance < nearest_distance) {
                                nearest_distance = distance;
                                processor->action_data_3 = position;
                            }
                        }
                    }
                } else {
                    NUSPECIALHANDLE special;
                    if (NuSpecialFind(WORLD->current_gscn, &special, name, 1) != 0) {
                        processor->action_data_3 = NuSpecialGetDrawPos(&special);
                    }
                }
            }

            if (processor->action_data_3 != NULL) {
                object->portal_flags |= 0x10000;
                object->quick_shoot_timer = NuRandFloat() * processor->action_data_5;
            }
        }

        NUVEC *target = static_cast<NUVEC *>(processor->action_data_3);
        if (target == NULL) {
            return 0;
        }

        packet->movement_look_target = target;
        processor->action_timer -= param_6;
        object->field_e4c = *target;
        object->portal_flags |= 0x8000;
        object->portal_flags |= 0x20;
        if (processor->action_timer > 0.0f) {
            return 0;
        }

        object->portal_flags = (object->portal_flags & ~0x20000u) | ((processor->action_data_1 & 1) << 17);
        processor->action_timer = (0.5f + NuRandFloat()) * processor->action_data_5;
        object->pad->buttons_pressed |= GAMEPAD_ACTION;
        return 0;
    }

    i32 __cdecl Action_FaceCamera(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
        (void)sys;
        if (packet == NULL || packet->owner == NULL || (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner == NULL) {
            return 1;
        }

        GAMEOBJECT *object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
        f32 completion_time = 0.0f;
        if (param_5 != 0) {
            f32 minimum_time = 0.0f;
            f32 maximum_time = 0.0f;
            for (i32 index = 0; index < param_4; ++index) {
                if (NuStrICmp("look_at_camera", params[index]) == 0) {
                    processor->action_data_1 = 1;
                } else {
                    char *value = NuStrIStr(params[index], (char *)"mintime");
                    if (value != NULL) {
                        minimum_time = AIParamToFloatEx(packet, processor, value + 8);
                    } else if ((value = NuStrIStr(params[index], (char *)"maxtime")) != NULL) {
                        maximum_time = AIParamToFloatEx(packet, processor, value + 8);
                    } else {
                        processor->action_timer = AIParamToFloat(processor, params[index]);
                    }
                }
            }
            if (processor->action_timer == 0.0f && maximum_time > minimum_time) {
                processor->action_timer = NuRandFloat() * (maximum_time - minimum_time) + minimum_time;
            }
        }

        NUMTX *camera = NuCameraGetMtx();
        if (camera != NULL) {
            packet->movement_look_target = (NUVEC *)&camera->m30;
            if (processor->action_data_1 != 0) {
                SetHeadTarget(object, object->ai_packet.movement_look_target, 6, 1.0f, 0.0f, 0.0f);
            }
        }
        if (processor->action_timer > completion_time) {
            processor->action_timer -= param_6;
            if (completion_time >= processor->action_timer) {
                processor->action_timer = 0.0f;
                return 1;
            }
        }
        return 0;
    }

    i32 __cdecl Action_CycleCharacter(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_4, i32 param_5, f32 param_6) {
        (void)sys;
        if (packet == NULL || packet->owner == NULL)
            return 1;

        GAMEOBJECT *object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
        if (object == NULL || FreePlay == 0 || static_cast<i8>(object->field_280) == -1)
            return 1;

        i32 single_switch = 0;
        if (param_5 != 0) {
            processor->action_data_1 = static_cast<u8>(CHARCATEGORYCOUNT);
            processor->action_data_2 = static_cast<u8>(CHARCATEGORYCOUNT);

            for (i32 index = 0; index < param_4; ++index) {
                char *value = NuStrIStr(params[index], (char *)"time");
                if (value != NULL) {
                    processor->action_data_4 = AIParamToFloat(processor, value + 5);
                    continue;
                }

                value = NuStrIStr(params[index], (char *)"Category");
                if (value != NULL) {
                    const i32 category = CharCategory_FindByName(value + 9);
                    if (category != -1) {
                        if (processor->action_data_1 >= CHARCATEGORYCOUNT)
                            processor->action_data_1 = static_cast<u8>(category);
                        else
                            processor->action_data_2 = static_cast<u8>(category);
                        break;
                    }
                    continue;
                }

                value = NuStrIStr(params[index], (char *)"min_speed=");
                if (value != NULL) {
                    processor->action_data_5 = AIParamToFloat(processor, value + 10);
                    continue;
                }

                if (NuStrICmp(params[index], "SingleSwitch") == 0)
                    single_switch = 1;
            }
        }

        if (object->character_context == 0xb)
            return 0;

        const i32 first_category = processor->action_data_1;
        if (first_category < CHARCATEGORYCOUNT) {
            if (processor->action_data_5 == 0.0f ||
                ((APIOBJECT_PC *)packet->owner)->character_data->game_character->run_speed >=
                    processor->action_data_5) {
                if (CharCategory_IsCategory(object, first_category) != 0)
                    return 1;

                const i32 second_category = processor->action_data_2;
                if (second_category < CHARCATEGORYCOUNT && CharCategory_IsCategory(object, second_category) != 0)
                    return 1;
            }

            processor->action_timer += param_6;
            if (processor->action_timer > processor->action_data_4) {
                processor->action_timer = 0.0f;
                object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
            }
            return 0;
        }

        if (processor->action_data_5 != 0.0f) {
            if (((APIOBJECT_PC *)packet->owner)->character_data->game_character->run_speed >= processor->action_data_5)
                return 1;
        } else {
            if (single_switch != 0)
                object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
            return 1;
        }

        processor->action_timer += param_6;
        if (processor->action_timer > processor->action_data_4) {
            processor->action_timer = 0.0f;
            object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
        }
        return 1;
    }

    i32 __cdecl Action_CnxHelper(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
        (void)packet;
        (void)param_6;
        if (param_5 == 0 || param_4 <= 0) {
            return 1;
        }

        AIPATH_PC *path = NULL;
        char *from = NULL;
        char *to = NULL;
        void *grapple = NULL;
        f32 jump_off_dy = 0.0f;
        bool both_ways = false;
        for (i32 index = 0; index < param_4; ++index) {
            char *value = NuStrIStr(params[index], (char *)"pathname=");
            if (value != NULL) {
                path = AISysFindPath(sys, value + NuStrLen("pathname="));
                continue;
            }
            if (NuStrIStr(params[index], (char *)"bothways") != NULL) {
                both_ways = true;
                continue;
            }
            value = NuStrIStr(params[index], (char *)"from=");
            if (value != NULL) {
                from = value + NuStrLen("from=");
                continue;
            }
            value = NuStrIStr(params[index], (char *)"to=");
            if (value != NULL) {
                to = value + NuStrLen("to=");
                continue;
            }
            value = NuStrIStr(params[index], (char *)"jump_off_dy=");
            if (value != NULL) {
                jump_off_dy = AIParamToFloat(processor, value + NuStrLen("jump_off_dy="));
                continue;
            }
            value = NuStrIStr(params[index], (char *)"grapple=");
            if (value != NULL) {
                GIZMO_PC *gizmo =
                    (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, grapple_gizmotype_id, value + NuStrLen("grapple="));
                if (gizmo != NULL) {
                    grapple = gizmo->object;
                }
            }
        }

        if (from != NULL && to != NULL) {
            i32 direction = 0;
            AIPATHCNX_PC *connection = static_cast<AIPATHCNX_PC *>(AIPAthFindPathCnx(sys, path, from, to, &direction));
            if (connection != NULL) {
                if (both_ways) {
                    direction = 0xff;
                }
                if (grapple != NULL) {
                    AI_HELPER_SLOT_PC *helper = AIPathCnxHelperSys_AddHelper(WORLD->ai_connection_helpers, connection,
                                                                             static_cast<u8>(direction), grapple, 1);
                    if (helper != NULL) {
                        helper->jump_off_dy = jump_off_dy;
                    }
                }
            }
        }
        return 1;
    }

    i32 __cdecl Action_EnableSock(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_4, i32 param_5, f32 param_6) {
        (void)sys;
        (void)packet;
        (void)param_6;
        if (param_5 != 0) {
            i32 sock_index = -1;
            i32 enabled = 1;
            for (i32 index = 0; index < param_4; ++index) {
                char *value = NuStrIStr(params[index], (char *)"ix=");
                if (value != NULL) {
                    sock_index = static_cast<i32>(AIParamToFloat(processor, value + 3));
                } else if ((value = NuStrIStr(params[index], (char *)"name=")) != NULL) {
                    SOCK_PC *sock = FindSock(WORLD->sock_sys, value + 5);
                    if (sock != NULL) {
                        sock_index = sock - WORLD->sock_sys->socks;
                    }
                } else if (NuStrIStr(params[index], (char *)"FALSE") != NULL) {
                    enabled = 0;
                }
            }
            if (enabled == 0) {
                SockOff(WORLD->sock_sys, sock_index);
            } else {
                SockOn(WORLD->sock_sys, sock_index);
            }
        }
        return 1;
    }

    i32 __cdecl Action_SetVisibility(AISYS_PC *, AISCRIPTPROCESS_PC *, AIPACKET_PC *, char **params, i32 param_count,
                                     i32 first_time, float) {
        if (first_time == 0) {
            return 1;
        }

        NUSPECIALHANDLE special = {};
        i32 visible = 1;
        for (i32 index = 0; index < param_count; ++index) {
            char *value = NuStrIStr(params[index], (char *)"name");
            if (value != NULL) {
                NuSpecialFind(WORLD->current_gscn, &special, value + 5, 1);
            } else if (NuStrIStr(params[index], (char *)"FALSE") != NULL) {
                visible = 0;
            }
        }
        if (NuSpecialExistsFn(&special) != 0) {
            NuSpecialSetVisibility(&special, visible);
        }
        return 1;
    }
}

extern "C" {
    static i32 Action_EatVictim(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_4, i32 param_5, f32 param_6) {
        (void)sys;
        (void)processor;
        (void)params;
        (void)param_4;
        (void)param_5;
        (void)param_6;
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL) {
            EatVictim(packet->owner->objptr);
        }
        return 1;
    }
}

extern "C" {
    static i32 Action_ReleaseVictim(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_4, i32 param_5, f32 param_6) {
        (void)sys;
        (void)processor;
        (void)params;
        (void)param_4;
        (void)param_5;
        (void)param_6;
        if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL) {
            ReleaseEat(packet->owner->objptr);
        }
        return 1;
    }
}

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
extern "C" u32 LEGO_AIPATHCNX_BIGJUMP, LEGO_AIPATHCNX_REQUIRESPERMISSION, LEGO_AIPATHCNX_NO_DESTINATION_CHECK;
extern "C" u32 LEGO_AIPATHCNX_JUMP_NOW, LEGO_AIPATHCNX_DONT_JUMP_NOW;
static u32 ParseAIPathCnxFlagPc(char *name) {
    u32 flag;
    if (StartupGameFlag0 != 0)
        flag = AIPathParseIndyFlagPc(name);
    else if (StartupGameFlag1 != 0)
        flag = AIPathParseStarWarsFlagPc(name);
    else
        flag = AIPathParseBatmanFlagPc(name);
    if (flag != 0)
        return flag;
    if (NuStrICmp(name, "BLOCK") == 0)
        return 0x80000000;
    if (NuStrICmp(name, "BIGJUMP") == 0)
        return LEGO_AIPATHCNX_BIGJUMP;
    if (NuStrICmp(name, "REQUIRESPERMISSION") == 0)
        return LEGO_AIPATHCNX_REQUIRESPERMISSION;
    if (NuStrICmp(name, "NO_DESTINATION_CHECK") == 0)
        return LEGO_AIPATHCNX_NO_DESTINATION_CHECK;
    return 0;
}

extern "C" {
    i32 __cdecl Action_SetPathCnxFlag(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 param_6) {
        (void)processor;
        (void)packet;
        (void)param_6;
        if (sys == NULL || sys->path_sys == NULL || sys->path_sys->path_count == 0 || first_time == 0 ||
            param_count < 1) {
            return 1;
        }

        char *from = NULL;
        char *to = NULL;
        u32 add_flags = 0;
        u32 remove_flags = 0;
        bool set = true;
        bool both_ways = false;
        for (i32 index = 0; index < param_count; ++index) {
            char *match = NuStrIStr(params[index], (char *)"from=");
            if (match != NULL) {
                from = match + 5;
                continue;
            }
            match = NuStrIStr(params[index], (char *)"to=");
            if (match != NULL) {
                to = match + 3;
                continue;
            }
            const u32 flag = ParseAIPathCnxFlagPc(params[index]);
            if (flag != 0) {
                add_flags |= flag;
                if (flag == static_cast<u32>(LEGO_AIPATHCNX_JUMP_NOW)) {
                    remove_flags |= static_cast<u32>(LEGO_AIPATHCNX_DONT_JUMP_NOW);
                } else if (flag == static_cast<u32>(LEGO_AIPATHCNX_DONT_JUMP_NOW)) {
                    remove_flags |= static_cast<u32>(LEGO_AIPATHCNX_JUMP_NOW);
                } else if (flag == 0x20000000u) {
                    both_ways = true;
                }
                continue;
            }
            if (NuStrICmp(params[index], "bothways") == 0) {
                both_ways = true;
            } else if (NuStrICmp(params[index], "FALSE") == 0) {
                set = false;
            }
        }

        if (from != NULL && to != NULL) {
            i32 direction = 0;
            AIPATHCNX_PC *connection =
                static_cast<AIPATHCNX_PC *>(AIPAthFindPathCnx(sys, sys->path_sys->active_path, from, to, &direction));
            if (connection != NULL) {
                if (set) {
                    connection->traversal_flags[direction] =
                        (connection->traversal_flags[direction] | add_flags) & ~remove_flags;
                    if (both_ways) {
                        connection->traversal_flags[direction ^ 1] =
                            (connection->traversal_flags[direction ^ 1] | add_flags) & ~remove_flags;
                    }
                } else {
                    connection->traversal_flags[direction] &= ~add_flags;
                    if (both_ways) {
                        connection->traversal_flags[direction ^ 1] &= ~add_flags;
                    }
                }
            }
        }
        return 1;
    }
}

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

typedef struct bulk_ai_lever_pc_s {
    u8 unknown_00[0x40];
    NUVEC floor_position;
    u8 unknown_4c[0x1c];
    NUVEC position;
    u8 unknown_74[4];
    f32 pull_progress;
    u8 unknown_7c[0x12];
    u16 animation_frame;
    u8 unknown_90[4];
    u16 flags;
} BULK_AI_LEVER_PC;
typedef struct bulk_ai_panel_pc_s {
    u8 unknown_00[0x50];
    NUVEC position;
    u8 unknown_5c[5];
    u8 model_variant;
    u8 flags;
    u8 unknown_63;
    NUVEC floor_position;
} BULK_AI_PANEL_PC;
typedef struct bulk_ai_techno_pc_s {
    u8 unknown_00[0x10];
    NUVEC position;
    NUVEC ground_position;
    u8 unknown_28[0x14];
    u8 flags;
} BULK_AI_TECHNO_PC;
STATIC_ASSERT(offsetof(BULK_AI_LEVER_PC, floor_position) == 0x40);
STATIC_ASSERT(offsetof(BULK_AI_LEVER_PC, position) == 0x68);
STATIC_ASSERT(offsetof(BULK_AI_LEVER_PC, pull_progress) == 0x78);
STATIC_ASSERT(offsetof(BULK_AI_LEVER_PC, animation_frame) == 0x8e);
STATIC_ASSERT(offsetof(BULK_AI_LEVER_PC, flags) == 0x94);
STATIC_ASSERT(offsetof(BULK_AI_PANEL_PC, position) == 0x50);
STATIC_ASSERT(offsetof(BULK_AI_PANEL_PC, model_variant) == 0x61);
STATIC_ASSERT(offsetof(BULK_AI_PANEL_PC, flags) == 0x62);
STATIC_ASSERT(offsetof(BULK_AI_PANEL_PC, floor_position) == 0x64);
STATIC_ASSERT(offsetof(BULK_AI_TECHNO_PC, position) == 0x10);
STATIC_ASSERT(offsetof(BULK_AI_TECHNO_PC, ground_position) == 0x1c);
STATIC_ASSERT(offsetof(BULK_AI_TECHNO_PC, flags) == 0x3c);
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
                return (object->character_data->model_flags & 0x40) != 0 ||
                       (object->character_data->model_flags & 0x1000010) == 0x1000010;
            case 1:
                return (object->character_data->model_flags & 0x20) != 0 ||
                       (object->character_data->model_flags & 0x1000010) == 0x1000010;
            case 2:
                return (object->character_data->model_flags & 0x1000000) != 0 || object->helmet == 6;
            case 3:
                if (object->helmet == 5 || (object->character_data->game_character->flags_090 & 0x40000) != 0)
                    return 1;
                if (GCDataList[(i16)object->id].weapon_action == 8 || GCDataList[(i16)object->id].weapon_action == 1)
                    return 1;
                return object->id == FreePlayId007f199cPc || object->id == id_THEEMPEROR ||
                       object->id == FreePlayId007f1a4cPc || object->id == FreePlayId007f1a48Pc ||
                       object->id == FreePlayId007f1a44Pc;
            default:
                return 0;
        }
    }
    i32 __cdecl GizTechno_CanUseTechno(GAMEOBJECT *, BULK_AI_TECHNO_PC *) {
        return 1;
    }
    i32 __cdecl Action_PullLever(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_count, i32 first_time, f32 elapsed) {
        i32 instant = 0;
        if (first_time != 0) {
            for (i32 index = 0; index < param_count; ++index) {
                char *name = NuStrIStr(params[index], (char *)"lever=");
                if (name != NULL) {
                    GIZMO_PC *gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, lever_gizmotype_id, name + 6);
                    if (gizmo == NULL || gizmo->object == NULL)
                        continue;
                    BULK_AI_LEVER_PC *lever = static_cast<BULK_AI_LEVER_PC *>(gizmo->object);
                    processor->action_data_3 = lever;
                    if ((lever->flags & 2))
                        continue;
                    processor->action_pos = lever->floor_position;
                    f32 height = GameShadow(NULL, &processor->action_pos, 5.0f, -1);
                    if (height != 2000000.0f)
                        processor->action_pos.y = height;
                    AISysGetPathPos2(system, &processor->action_pos, &processor->path_info, &processor->action_pos,
                                     NULL, 0xff);
                } else if (NuStrICmp(params[index], "instant") == 0) {
                    instant = 1;
                }
            }
        }
        BULK_AI_LEVER_PC *lever = static_cast<BULK_AI_LEVER_PC *>(processor->action_data_3);
        if (lever == NULL)
            return 1;
        if (instant != 0) {
            lever->flags |= 2;
            lever->animation_frame = 0x8000;
            lever->pull_progress = 1.0f;
            return 1;
        }
        if (packet == NULL || packet->owner == NULL || (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner == NULL)
            return 1;
        GAMEOBJECT *object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
        AIMoveInstruction(packet, &processor->action_pos, 0.0f, &processor->path_info, 1, 0.0f);
        if ((object->flags_ef0 & 0x800000) != 0) {
            f32 distance = NuVecDistSqr(&packet->terrain_origin, &processor->action_pos, NULL);
            if (distance < ai_moveradius * ai_moveradius) {
                packet->movement_look_target = &lever->position;
                object->pad->buttons_pressed |= GAMEPAD_SPECIAL;
            }
        } else if (FreePlay != 0) {
            processor->action_timer -= elapsed;
            if (processor->action_timer < 0.0f) {
                processor->action_timer = 0.5f;
                object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
            }
        }
        return object->character_context == 0x4a && object->grapple == lever;
    }

    i32 __cdecl Action_UsePanel(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_count, i32 first_time, f32 elapsed) {
        if (packet == NULL || packet->owner == NULL || (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner == NULL)
            return 1;
        GAMEOBJECT *object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
        if (first_time != 0) {
            for (i32 index = 0; index < param_count; ++index) {
                char *name = NuStrIStr(params[index], (char *)"name=");
                if (name == NULL)
                    continue;
                GIZMO_PC *gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, gizpanel_gizmotype_id, name + 5);
                if (gizmo == NULL || gizmo->object == NULL)
                    continue;
                BULK_AI_PANEL_PC *panel = static_cast<BULK_AI_PANEL_PC *>(gizmo->object);
                processor->action_data_3 = panel;
                if ((panel->flags & 2) != 0)
                    continue;
                processor->action_pos = panel->floor_position;
                f32 height = GameShadow(NULL, &processor->action_pos, 5.0f, -1);
                if (height != 2000000.0f)
                    processor->action_pos.y = height;
                AISysGetPathPos2(system, &processor->action_pos, &processor->path_info, &processor->action_pos, NULL,
                                 0xff);
            }
        }
        BULK_AI_PANEL_PC *panel = static_cast<BULK_AI_PANEL_PC *>(processor->action_data_3);
        if (panel == NULL || (panel->flags & 2) != 0)
            return 1;
        AIMoveInstruction(packet, &processor->action_pos, 0.0f, &processor->path_info, 1, 0.0f);
        if (GizPanel_CanUsePanel(object, panel) != 0) {
            f32 distance = NuVecDistSqr(&packet->terrain_origin, &processor->action_pos, NULL);
            if (distance < ai_moveradius * ai_moveradius) {
                packet->movement_look_target = &panel->position;
                object->pad->buttons_pressed |= GAMEPAD_SPECIAL;
            }
        } else if (FreePlay != 0) {
            processor->action_timer -= elapsed;
            if (processor->action_timer < 0.0f) {
                processor->action_timer = 0.5f;
                object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
            }
        }
        return object->character_context == 0x0b && object->grapple == panel;
    }

    i32 __cdecl Action_UseTechno(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_count, i32 first_time, f32 elapsed) {
        if (packet == NULL || packet->owner == NULL || (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner == NULL)
            return 1;
        GAMEOBJECT *object = (GAMEOBJECT *)((APIOBJECT_PC *)packet->owner)->owner;
        if (first_time != 0) {
            for (i32 index = 0; index < param_count; ++index) {
                char *name = NuStrIStr(params[index], (char *)"name=");
                if (name == NULL)
                    continue;
                GIZMO_PC *gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, techno_gizmotype_id, name + 5);
                if (gizmo == NULL || gizmo->object == NULL)
                    continue;
                BULK_AI_TECHNO_PC *techno = static_cast<BULK_AI_TECHNO_PC *>(gizmo->object);
                processor->action_data_3 = techno;
                processor->action_pos = techno->ground_position;
                f32 height = GameShadow(NULL, &processor->action_pos, 5.0f, -1);
                if (height != 2000000.0f)
                    processor->action_pos.y = height;
                AISysGetPathPos2(system, &processor->action_pos, &processor->path_info, &processor->action_pos, NULL,
                                 0xff);
            }
        }
        BULK_AI_TECHNO_PC *techno = static_cast<BULK_AI_TECHNO_PC *>(processor->action_data_3);
        if (techno == NULL || (techno->flags & 1) == 0 || (techno->flags & 2) == 0 || (techno->flags & 8) != 0)
            return 1;
        AIMoveInstruction(packet, &processor->action_pos, 0.0f, &processor->path_info, 1, 0.0f);
        if (GizTechno_CanUseTechno(object, techno) == 0) {
            if (FreePlay != 0) {
                processor->action_timer -= elapsed;
                if (processor->action_timer < 0.0f) {
                    processor->action_timer = 0.5f;
                    object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
                }
            }
        } else {
            f32 distance = NuVecDistSqr(&packet->terrain_origin, &processor->action_pos, NULL);
            if (distance < ai_moveradius * ai_moveradius) {
                packet->movement_look_target = &techno->position;
                GameObjectSetCanUse(object, techno, 2, 1, 0.0f);
            }
        }
        return 0;
    }
}

extern "C" i32 Action_ReleaseLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *, AIPACKET_PC *packet, char **params,
                                     i32 param_count, i32 first_time, float) {
    if (first_time == 0) {
        return 1;
    }

    GAMEOBJECT *object = packet != NULL && packet->owner != NULL ? packet->owner->objptr : NULL;
    for (i32 index = 0; index < param_count; ++index) {
        char *value = NuStrIStr(params[index], "character=");
        if (value != NULL) {
            object = GetNamedGameObject(sys, value + sizeof("character=") - 1);
        }
    }
    if (object != NULL) {
        object->ai_packet.locator = NULL;
    }
    return 1;
}

STATIC_ASSERT(offsetof(ZIPUP_s, flags) == 0x68);
STATIC_ASSERT(offsetof(ZIPUP_s, runtime_flags) == 0x69);
STATIC_ASSERT(offsetof(ZIPUP_s, lower_position) == 0x10);
STATIC_ASSERT(offsetof(ZIPUP_s, hook_origin) == 0x1c);
STATIC_ASSERT(offsetof(ZIPUP_s, upper_position) == 0x28);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, run_speed) == 0x1c);
extern "C" void __cdecl StartBallooning(GAMEOBJECT *, i32);

extern "C" {
    static __declspec(noinline) ZIPUP_s *FindSpawnZipUp(char *name) {
        ZIPUP_s *zipup = reinterpret_cast<ZIPUP_s *>(WORLD->zipups);
        for (i32 index = 0; index < WORLD->zipup_count; ++index, ++zipup) {
            if ((zipup->flags & 0xc0) == 0xc0 && NuStrICmp(name, zipup->name) == 0)
                return zipup;
        }
        return NULL;
    }

    static __declspec(noinline) ZIPUP_s *LocalGetRandomSpawnZipUp(ZIPUP_s **zipups, i32 count) {
        i32 eligible = 0;
        if (count > 32)
            count = 32;
        for (i32 index = 0; index < count; ++index) {
            if (zipups[index] != NULL && (zipups[index]->flags & 0x40) != 0)
                ++eligible;
        }
        if (eligible == 0)
            return NULL;
        i32 selection = qrand() / (65535 / eligible + 1);
        ZIPUP_s *result = zipups[selection];
        zipups[selection] = NULL;
        return result;
    }

    static __declspec(noinline) AILOCATOR_PC *FirstSpawnSetLocator(AILOCATORSET_PC *set, AISYS_PC *sys) {
        if (set == NULL || set->locator_count == 0)
            return NULL;
        return &sys->locators[set->locator_entries[0]];
    }
}

extern "C" AILOCATOR_PC *__cdecl LocalGetNearestLocator(void *locator_array, i32 count, f32 clip_radius,
                                                        NUVEC *position, f32 max_distance, i32 outside_camera,
                                                        f32 max_delta_y, f32 min_delta_y) {
    AILOCATOR_PC **locators = static_cast<AILOCATOR_PC **>(locator_array);
    f32 nearest_distance = max_distance == 1000000000.0f ? max_distance : max_distance * max_distance;
    i32 candidates[64];
    i32 candidate_count = 0;
    if (count > 64) {
        count = 64;
    }
    for (i32 i = 0; i < count; ++i) {
        if (locators[i] == NULL) {
            continue;
        }
        if (outside_camera != 0) {
            if (NuCameraClipTestSphere(&locators[i]->position, 0.0f, &numtx_identity) != 0) {
                continue;
            }
        } else if (clip_radius > 0.0f &&
                   NuCameraClipTestSphere(&locators[i]->position, clip_radius, &numtx_identity) == 0) {
            continue;
        }
        candidates[candidate_count++] = i;
    }
    i32 nearest = -1;
    for (i32 i = 0; i < candidate_count; ++i) {
        NUVEC delta;
        const f32 distance = NuVecDistSqr(position, &locators[candidates[i]]->position, &delta);
        if (min_delta_y != 1000000000.0f && min_delta_y > delta.y) {
            continue;
        }
        if (max_delta_y != 1000000000.0f && delta.y > max_delta_y) {
            continue;
        }
        if (!(nearest_distance > distance)) {
            continue;
        }
        nearest_distance = distance;
        nearest = i;
    }
    if (nearest == -1) {
        return NULL;
    }
    AILOCATOR_PC *result = locators[candidates[nearest]];
    locators[candidates[nearest]] = NULL;
    return result;
}

extern "C" i32 __cdecl Action_CreateCreatures(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                              char **params, i32 param_count, i32 first_time, f32 unused_delta) {
    AILOCATOR_PC *locators[64] = {};
    ZIPUP_s *zipups[32];
    i16 models[10];
    char script[64] = "default";
    NUVEC offset = {0.0f, 0.0f, 0.0f};
    if (first_time == 0)
        return 1;
    NUVEC position;
    AIPATHINFO_PC *path;
    i32 yaw;
    if (packet != NULL && packet->owner != NULL) {
        path = &packet->path_info;
        position = packet->owner->position;
        yaw = packet->owner->field_0x276;
    } else {
        yaw = 0;
        path = NULL;
    }
    i32 minimum = -1, maximum = -1, count = 1;
    i32 grouped = 1, across = 2;
    f32 xspacing = 0.8f, zspacing = 0.8f;
    i32 locator_count = 0, zipup_count = 0, model_count = 0;
    AILOCATORSET_PC *locator_set = NULL;
    i32 start_locator = 0, inherit = 1;
    f32 max_range = 1000000000.0f, min_dy = 1000000000.0f, max_dy = 1000000000.0f;
    i32 offscreen = 0, nearest = 0, onscreen = 0;
    i32 zipdown = 0;
    f32 fall_chance = 0.0f;
    i32 surface = 1, creature_set = 0;
    f32 crate_height = 0.0f, crate_delay = 0.0f;
    GAMEOBJECT *rider = NULL;
    i32 no_gun = 0, cheap = 0, ballooning = 0;
    i64 add_capabilities = 0, remove_capabilities = 0;
    char *state = NULL;
    for (i32 index = 0; index < param_count; ++index) {
        char *value;
        if ((value = NuStrIStr(params[index], "mingroupsize")) != NULL)
            minimum = static_cast<i32>(AIParamToFloat(processor, value + 13));
        else if ((value = NuStrIStr(params[index], "maxgroupsize")) != NULL)
            maximum = static_cast<i32>(AIParamToFloat(processor, value + 13));
        else if ((value = NuStrIStr(params[index], "groupsize")) != NULL)
            count = static_cast<i32>(AIParamToFloat(processor, value + 10));
        else if ((value = NuStrIStr(params[index], "start_locator_set=")) != NULL) {
            value += 18;
            if (NuStrICmp("myset", value) == 0)
                locator_set = processor->locator_set;
            else
                locator_set = AIPathFindLocatorSet(WORLD->ai_sys, value);
            if (locator_set != NULL)
                start_locator = 1;
        } else if (NuStrICmp("dont_inherit_locator_set", params[index]) == 0)
            inherit = 0;
        else if ((value = NuStrIStr(params[index], "locator_set=")) != NULL) {
            value += 12;
            if (NuStrICmp("myset", value) == 0)
                locator_set = processor->locator_set;
            else
                locator_set = AIPathFindLocatorSet(WORLD->ai_sys, value);
            if (locator_set != NULL) {
                AILocatorSet_CheckLocatorsStillAssigned(sys, locator_set);
                for (i32 i = 0; i < locator_set->locator_count && locator_count < 64; ++i) {
                    if (locator_set->assigned[i] == 0xff)
                        locators[locator_count++] = &sys->locators[locator_set->locator_entries[i]];
                }
            }
        } else if ((value = NuStrIStr(params[index], "locator=")) != NULL) {
            if (locator_count < 64) {
                value += 8;
                AILOCATOR_PC *locator;
                if (NuStrICmp("mylocator", value) == 0)
                    locator = processor->locator;
                else
                    locator = AIPathFindLocator(sys, value);
                if (locator != NULL)
                    locators[locator_count++] = locator;
            }
        } else if ((value = NuStrIStr(params[index], "max_range_to_player=")) != NULL)
            max_range = AIParamToFloat(processor, value + 20);
        else if ((value = NuStrIStr(params[index], "mindy=")) != NULL)
            min_dy = AIParamToFloat(processor, value + 6);
        else if ((value = NuStrIStr(params[index], "maxdy=")) != NULL)
            max_dy = AIParamToFloat(processor, value + 6);
        else if ((value = NuStrIStr(params[index], "zipup")) != NULL) {
            if (locator_count < 32) {
                value += 6;
                ZIPUP_s *zipup = FindSpawnZipUp(value);
                if (zipup != NULL)
                    zipups[zipup_count++] = zipup;
            }
        } else if (NuStrIStr(params[index], "type=randommap") != NULL) {
            if (WORLD->current_level == HUB_LDATA) {
                i16 model = Hub_GetRandomCharType();
                if (model != -1 && model_count < 10)
                    models[model_count++] = model;
            }
        } else if (NuStrIStr(params[index], "type=generic_goon") != NULL) {
            if (model_count < 10) {
                i16 model = GetGenericGoon(0);
                if (model != -1)
                    models[model_count++] = model;
            }
        } else if ((value = NuStrIStr(params[index], "type")) != NULL) {
            value += 5;
            if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                if (WORLD->current_level != HUB_LDATA) {
                    u8 type = LevelCharacterTypeIDFn(value);
                    if (type != 0xff) {
                        i16 model = LevelCharacterGlobalIDFn(type);
                        if (model != -1 && model_count < 10)
                            models[model_count++] = model;
                    }
                } else {
                    i16 model;
                    if (NuStrICmp("Barman", value) == 0)
                        model = id_BARMAN;
                    else if (NuStrICmp("JABBA", value) == 0)
                        model = DefaultAnimationJabbaIdPc;
                    else if (NuStrICmp("CANTINABAND", value) == 0)
                        model = id_CANTINABAND;
                    else
                        continue;
                    if (apicharsys->playermodelids[model] != -1 && model_count < 10)
                        models[model_count++] = model;
                }
            }
        } else if ((value = NuStrIStr(params[index], "state=")) != NULL)
            state = value + 6;
        else if ((value = NuStrIStr(params[index], "script")) != NULL)
            NuStrCpy(script, value + 7);
        else if ((value = NuStrIStr(params[index], "xspacing")) != NULL)
            xspacing = AIParamToFloat(processor, value + 9);
        else if ((value = NuStrIStr(params[index], "zspacing")) != NULL)
            zspacing = AIParamToFloat(processor, value + 9);
        else if ((value = NuStrIStr(params[index], "xoffset")) != NULL)
            offset.x = AIParamToFloat(processor, value + 8);
        else if ((value = NuStrIStr(params[index], "yoffset")) != NULL)
            offset.y = AIParamToFloat(processor, value + 8);
        else if ((value = NuStrIStr(params[index], "zoffset")) != NULL)
            offset.z = AIParamToFloat(processor, value + 8);
        else if ((value = NuStrIStr(params[index], "nacross")) != NULL)
            across = static_cast<i32>(AIParamToFloat(processor, value + 8));
        else if (NuStrIStr(params[index], "addtoset=myset") != NULL)
            creature_set = processor->creature_set;
        else if ((value = NuStrIStr(params[index], "addtoset=")) != NULL)
            creature_set = static_cast<i32>(AIParamToFloat(processor, value + 9));
        else if ((value = NuStrIStr(params[index], "crate_height=")) != NULL)
            crate_height = AIParamToFloat(processor, value + 13);
        else if ((value = NuStrIStr(params[index], "crate_delay=")) != NULL)
            crate_delay = AIParamToFloat(processor, value + 12);
        else if (NuStrICmp("not_grouped", params[index]) == 0)
            grouped = 0;
        else if (NuStrICmp("dont_set_on_surface", params[index]) == 0)
            surface = 0;
        else if (NuStrICmp("zipdown", params[index]) == 0) {
            zipdown = 1;
            surface = 0;
        } else if (NuStrICmp("nearest_player", params[index]) == 0)
            nearest = 1;
        else if (NuStrICmp("off_screen", params[index]) == 0 || NuStrICmp("offscreen", params[index]) == 0)
            offscreen = 1;
        else if (NuStrICmp("on_screen", params[index]) == 0)
            onscreen = 1;
        else if ((value = NuStrIStr(params[index], "fall_chance=")) != NULL)
            fall_chance = AIParamToFloat(processor, value + 12);
        else if (NuStrICmp("CheapAsChips", params[index]) == 0)
            cheap = 1;
        else if (NuStrIStr(params[index], "ridden_by=myself") != NULL) {
            if (packet != NULL && packet->owner != NULL)
                rider = packet->owner->objptr;
        } else if ((value = NuStrIStr(params[index], "ridden_by=")) != NULL)
            rider = GetNamedGameObject(sys, value + 10);
        else if (NuStrICmp("no_gun", params[index]) == 0)
            no_gun = 1;
        else if (NuStrICmp("ballooning", params[index]) == 0) {
            ballooning = 1;
            surface = 0;
        } else if ((value = NuStrIStr(params[index], "lose_capability=")) != NULL)
            remove_capabilities |= static_cast<i32>(ParseAIPathCnxFlagPc(value + NuStrLen("lose_capability=")));
        else if ((value = NuStrIStr(params[index], "capability=")) != NULL)
            add_capabilities |= static_cast<i32>(ParseAIPathCnxFlagPc(value + NuStrLen("capability=")));
    }
    if (minimum >= 0) {
        if (maximum > minimum) {
            count = minimum;
            count += NuRand(NULL) % (maximum + 1 - minimum);
        } else if (maximum == minimum)
            count = maximum;
    }
    f32 delay = 0.0f;
    i32 offset_applied = 0;
    while (count != 0) {
        if ((locator_count | zipup_count) == 0 && rider == NULL && start_locator == 0)
            break;
        if (model_count == 0)
            break;
        i32 model = models[NuRand(NULL) % model_count];
        AILOCATOR_PC *locator = NULL;
        ZIPUP_s *zipup = NULL;
        if ((locator_count | start_locator) != 0) {
            if (start_locator != 0 && locator_set != NULL) {
                locator = FirstSpawnSetLocator(locator_set, sys);
            } else {
                f32 clip_radius = 0.0f;
                if (offscreen != 0)
                    clip_radius = 0.5f + apicharsys->char_data[static_cast<i16>(model)].collision_radius;
                if (nearest != 0)
                    locator = LocalGetNearestLocator(locators, locator_count, clip_radius, &player->position, max_range,
                                                     onscreen, max_dy, min_dy);
                else if (max_range == 1000000000.0f)
                    locator = LocalGetRandomLocator(locators, locator_count, clip_radius, NULL, 1000000000.0f, onscreen,
                                                    max_dy, min_dy);
                else
                    locator = LocalGetRandomLocator(locators, locator_count, clip_radius, &player->collision_position,
                                                    max_range, onscreen, max_dy, min_dy);
            }
            if (locator == NULL)
                break;
            path = &locator->path_info;
            position = locator->position;
            yaw = locator->flags;
            offset_applied = 0;
        } else if (zipup_count != 0) {
            zipup = LocalGetRandomSpawnZipUp(zipups, zipup_count);
            if (zipup == NULL)
                break;
            position = zipup->lower_position;
            path = NULL;
            surface = 0;
        } else if (rider != NULL) {
            path = &rider->ai_packet.path_info;
            position = rider->ai_packet.last_path_position;
            yaw = rider->field_0x276;
        }
        if (offset_applied == 0 && (offset.x != 0.0f || offset.y != 0.0f || offset.z != 0.0f)) {
            NuVecRotateY(&offset, &offset, yaw);
            NuVecAdd(&position, &position, &offset);
            offset_applied = 1;
        }
        if (count > 0) {
            i32 members = 1;
            i32 make_group = 0;
            if (grouped == 0)
                --count;
            else {
                members = count;
                make_group = count > 1;
                count = 0;
            }
            AIGROUP_PC *group = NULL;
            for (i32 member = 0; member < members; ++member) {
                if (member == 0 && make_group != 0) {
                    PLAYERCHARACTERCONFIG_PC *data = apicharsys->char_data[model].game_character;
                    group = CreateAIGroup(sys, across, xspacing, zspacing, data->run_speed);
                }
                GAMEOBJECT *object = AddDynamicCreature(model, &position, yaw, script, path, group, surface, NULL, NULL,
                                                        0, creature_set);
                if (object == NULL)
                    continue;
                if (state != NULL)
                    AIScriptSetBaseScriptStateByName(&object->ai_packet.script_processor, state);
                object->ai_packet.locator = locator;
                if (inherit != 0)
                    object->ai_packet.locator_set = locator_set;
                if (no_gun != 0)
                    object->portal_flags &= ~4;
                if (crate_height != 0.0f)
                    SpawnCreatureFromCrate(object, crate_height, delay);
                delay += crate_delay;
                if (locator_set != NULL && object->ai_packet.locator != NULL) {
                    for (i32 i = 0; i < locator_set->locator_count; ++i) {
                        if (locator_set->locator_entries[i] == object->ai_packet.locator - sys->locators) {
                            locator_set->assigned[i] = object->hostility_group;
                            break;
                        }
                    }
                }
                if (zipup != NULL) {
                    object->attached_zipup = zipup;
                    zipup->runtime_flags |= 1;
                    ZIPUP_s *attached_zipup = static_cast<ZIPUP_s *>(object->attached_zipup);
                    object->context_variant_flags |= 0x20;
                    object->character_context = 0x47;
                    object->field_7aa = 0x2a;
                    object->movement_facing_angle =
                        NuAtan2D(attached_zipup->upper_position.x - attached_zipup->lower_position.x,
                                 attached_zipup->upper_position.z - attached_zipup->lower_position.z);
                    object->field_e22 |= 1;
                    object->velocity.y = 0.0f;
                    object->context_timer = 0.0f;
                    object->field_e33 = static_cast<u8>(object->weapon_scale < 1.0f);
                    PlaySfx("ZipUp", &object->collision_position);
                    PlayJumpSfx(object, 0);
                    object->jump_variant_state = 0;
                    ZIPUP_s *current_zipup = static_cast<ZIPUP_s *>(object->attached_zipup);
                    f32 dx = current_zipup->hook_origin.x - attached_zipup->lower_position.x;
                    f32 dz = current_zipup->hook_origin.z - attached_zipup->lower_position.z;
                    i32 angle = NuAtan2D(current_zipup->hook_origin.y - attached_zipup->lower_position.y,
                                         NuFsqrt(dx * dx + dz * dz));
                    i32 rotation = 0x4000 - (angle < 0 ? -angle : angle);
                    if (angle < 0)
                        rotation = -rotation;
                    object->field_7a4 = rotation;
                } else if (zipdown != 0 && !(fall_chance > 0.0f && fall_chance > NuRandFloat())) {
                    object->character_context = 0x35;
                    object->field_7aa = 0x2a;
                    object->external_force = object->upper_position;
                    SetWeaponOut(object);
                }
                if (cheap != 0) {
                    object->flags_1fc_word |= 0x420;
                }
                if (rider != NULL)
                    TakeOverGameObject(rider, object, 0, 1);
                else if (ballooning != 0) {
                    StartBallooning(object, 0);
                    if (locator != NULL)
                        SetBallooningHeight(object, locator->position.y);
                }
                if (add_capabilities != 0)
                    object->ai_packet.capabilities |= static_cast<u32>(add_capabilities);
                if (remove_capabilities != 0)
                    object->ai_packet.capabilities &= ~static_cast<u32>(remove_capabilities);
            }
        }
    }
    return 1;
}

typedef struct ai_action_controller_world_pc_s {
    u8 unknown_0000[0x4670];
    AI_CONTROL_SYSTEM_PC *control_system;
} AI_ACTION_CONTROLLER_WORLD_PC;
STATIC_ASSERT(offsetof(AI_ACTION_CONTROLLER_WORLD_PC, control_system) == 0x4670);
extern "C" struct AIPATHCNXCONTROLLER_s *__cdecl AIPathCnxControllerCreate(AI_CONTROL_SYSTEM_PC *, AISYS_PC *,
                                                                           AIPATH_PC *, char *, char *, i32, char *,
                                                                           i32, i32);
extern "C" void __cdecl AIPathCnxControllerSetOnRange(struct AIPATHCNXCONTROLLER_s *, i32, i32);
extern "C" const char AIControllerLiteralPc00757e34[10];
extern "C" const char AIControllerLiteralPc00757d98[10];
extern "C" const char AIControllerLiteralPc00757d70[16];
extern "C" const char AIControllerLiteralPc00757db4[9];
extern "C" const char AIControllerLiteralPc00757de4[9];
extern "C" const char AIControllerLiteralPc00757e18[9];
extern "C" const char AIControllerLiteralPc00757e40[10];
extern "C" const char AIControllerLiteralPc00757dc8[14];
extern "C" const char AIControllerLiteralPc00757da4[7];
extern "C" const char AIControllerLiteralPc00757e10[8];
extern "C" const char AIControllerLiteralPc00757d60[12];
extern "C" const char AIControllerLiteralPc00757dac[8];
extern "C" const char AIControllerLiteralPc00757dc0[7];
extern "C" const char AIControllerLiteralPc00757dfc[5];
extern "C" const char AIControllerLiteralPc00757d6c[3];
extern "C" const char AIControllerLiteralPc00757d80[14];
extern "C" const char AIControllerLiteralPc00757e04[9];
extern "C" const char AIControllerLiteralPc00757d54[10];
extern "C" const char AIControllerLiteralPc00757dd8[12];
extern "C" const char AIControllerLiteralPc00757d90[7];
extern "C" const char AIControllerLiteralPc00757e24[13];
extern "C" const char AIControllerLiteralPc00757df0[10];

extern "C" i32 __cdecl Action_CnxController(struct AISYS_PC *sys, struct AISCRIPTPROCESS_PC *processor,
                                            struct AIPACKET_PC *packet, char **params, i32 param_count, i32 first_time,
                                            f32 param_6) {
    (void)packet;
    (void)param_6;
    if (first_time == 0) {
        return 1;
    }

    struct AIPATH_PC *path = NULL;
    char *from = NULL;
    char *to = NULL;
    char *target_name = NULL;
    i32 target_type = 0;
    i32 fake_animation_id = -1;
    i32 gizmo_output = 0;
    u32 on_flags = 0;
    u32 off_flags = 0;
    bool both_ways = false;
    bool check_visible = false;
    bool on_obstacle_open = false;
    bool off_obstacle_open = false;

    for (i32 index = 0; index < param_count; ++index) {
        char *param = params[index];
        if (NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757e40)) != NULL) {
            continue;
        }
        char *match = NuStrIStr(param, const_cast<char *>("from="));
        if (match != NULL) {
            from = match + 5;
            continue;
        }
        match = NuStrIStr(param, const_cast<char *>("to="));
        if (match != NULL) {
            to = match + 3;
            continue;
        }
        match = NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757e34));
        if (match != NULL) {
            path = AISysFindPath(sys, match + 9);
            continue;
        }
        if (NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757e24)) != NULL) {
            check_visible = true;
            continue;
        }
        if (NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757e18)) != NULL) {
            both_ways = true;
            continue;
        }

        match = NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757e10));
        if (match != NULL) {
            char *value = match + 8;
            if (NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757d80)) != NULL) {
                on_obstacle_open = true;
            } else if (NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757d70)) != NULL) {
                on_obstacle_open = false;
            } else {
                on_flags |= ParseAIPathCnxFlagPc(value);
            }
            continue;
        }
        match = NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757e04));
        if (match != NULL) {
            char *value = match + 9;
            if (NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757d80)) != NULL) {
                off_obstacle_open = true;
            } else if (NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757d70)) != NULL) {
                off_obstacle_open = false;
            } else {
                off_flags |= ParseAIPathCnxFlagPc(value);
            }
            continue;
        }

        if ((match = NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757dfc))) != NULL) {
            target_name = match + 4;
            target_type = 0;
            continue;
        }
        if ((match = NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757df0))) != NULL) {
            target_name = match + 9;
            target_type = 1;
            continue;
        }
        if ((match = NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757de4))) != NULL) {
            target_name = match + 8;
            target_type = 2;
            continue;
        }
        match = NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757dd8));
        if (match != NULL) {
            fake_animation_id = static_cast<i32>(AIParamToFloat(processor, match + 11));
            target_type = 5;
            continue;
        }
        match = NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757dc8));
        if (match != NULL) {
            gizmo_output = static_cast<i32>(AIParamToFloat(processor, match + 13));
            continue;
        }
        if ((match = NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757dc0))) != NULL) {
            target_name = match + 6;
            target_type = 3;
        } else if ((match = NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757db4))) != NULL) {
            target_name = match + NuStrLen(AIControllerLiteralPc00757db4);
            target_type = 6;
        } else if ((match = NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757dac))) != NULL) {
            target_name = match + 7;
            target_type = 4;
        } else if ((match = NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757da4))) != NULL) {
            target_name = match + 6;
            target_type = 7;
        } else if ((match = NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757d98))) != NULL) {
            target_name = match + 9;
            target_type = 8;
        } else if ((match = NuStrIStr(param, const_cast<char *>(AIControllerLiteralPc00757d90))) != NULL) {
            target_name = match + 6;
            target_type = 9;
        }
    }

    WORLDINFO *world = WORLD;
    struct AIPATHCNXCONTROLLER_s *controller =
        AIPathCnxControllerCreate(((AI_ACTION_CONTROLLER_WORLD_PC *)world)->control_system, world->ai_sys, path, from,
                                  to, target_type, target_name, fake_animation_id, gizmo_output);
    if (controller == NULL) {
        return 1;
    }
    controller->on_flags = on_flags;
    controller->off_flags = off_flags;
    controller->flags_word =
        static_cast<u16>((controller->flags_word & 0xffe1) | (both_ways ? 2 : 0) | (check_visible ? 4 : 0) |
                         (on_obstacle_open ? 8 : 0) | (off_obstacle_open ? 0x10 : 0));

    for (i32 index = 0; index < param_count; ++index) {
        char *match = NuStrIStr(params[index], const_cast<char *>(AIControllerLiteralPc00757e40));
        if (match == NULL) {
            continue;
        }
        char range[64];
        NuStrCpy(range, match + 10);
        char *separator = NuStrIStr(range, const_cast<char *>(AIControllerLiteralPc00757d6c));
        if (separator == NULL) {
            continue;
        }
        *separator = '\0';
        char *end_text = separator + 2;
        i32 start_frame = NuStrICmp(range, AIControllerLiteralPc00757d60) == 0 ? -2
                          : NuStrICmp(range, AIControllerLiteralPc00757d54) == 0
                              ? -1
                              : static_cast<i32>(AIParamToFloat(processor, range));
        i32 end_frame = NuStrICmp(end_text, AIControllerLiteralPc00757d60) == 0 ? -2
                        : NuStrICmp(end_text, AIControllerLiteralPc00757d54) == 0
                            ? -1
                            : static_cast<i32>(AIParamToFloat(processor, end_text));
        if (end_frame != -1 && end_frame != -2 && end_frame < start_frame) {
            continue;
        }
        AIPathCnxControllerSetOnRange(controller, start_frame, end_frame);
    }
    return 1;
}
