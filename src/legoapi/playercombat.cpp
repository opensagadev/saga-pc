#include "saga.h"

extern "C" void __cdecl JediLightCode(GAMEOBJECT *object) {
    if (object->dynamic_light_id != -1) {
        NUVEC colour;
        NUVEC position;
        if (object->character_context == 0x1b && (object->flags_e20 & 0x500) == 0x100 && object->force_target != NULL) {
            rtlDynamicEnable(object->dynamic_light_id, 1);
            NUVEC delta;
            f32 distance = NuVecDist(&object->force_target->collision_position, &object->collision_position, &delta);
            rtlDynamicSetRadii(object->dynamic_light_id, distance * 0.5f, distance * 0.5f + 0.5f);
            colour.x = 0.0f;
            colour.y = qrand() < 0x8000 ? 2.0f : 0.25f;
            colour.z = colour.y;
            rtlDynamicSetColours(object->dynamic_light_id, &colour, NULL);
            position.x = delta.x * 0.5f + object->collision_position.x;
            position.z = delta.z * 0.5f + object->collision_position.z;
            position.y = delta.y * 0.5f + object->collision_position.y;
            rtlDynamicSetPos(object->dynamic_light_id, &position);
            return;
        }
        if (!(object->weapon_scale > 0.0f) || object->sabre_flags == 0)
            return;
        position = v000;
        if (object->origin_joint_ready == 0)
            return;
        i32 point_count = 0;
        u32 f0xe23 = object->flags_e20;
        for (i32 blade = 0; blade < 4; ++blade) {
            if (object->origin_joint_ready == 0)
                break;
            if ((f0xe23 & 0x08000000) == 0)
                continue;
            PLAYERCHARACTERCONFIG_PC *data = object->character_data->game_character;
            i32 first = data->streak_joints[blade][0];
            if (first == -1 || object->character_model->points_of_interest[first] == NULL)
                continue;
            i32 second = data->streak_joints[blade][1];
            if (second == -1 || object->character_model->points_of_interest[second] == NULL)
                continue;
            NuVecAdd(&position, &position, reinterpret_cast<NUVEC *>(&object->locator_matrices[first].m30));
            NuVecAdd(&position, &position, reinterpret_cast<NUVEC *>(&object->locator_matrices[second].m30));
            point_count += 2;
        }
        if (point_count == 0)
            return;
        NuVecScale(&position, &position, 1.0f / point_count);
        rtlDynamicEnable(object->dynamic_light_id, 1);
        f32 radius = object->sabre_collision_radius * 3.0f * object->weapon_scale * object->origin_scale;
        rtlDynamicSetRadii(object->dynamic_light_id, radius + 0.25f, radius + 0.5f);
        f32 brightness = qrand() < 0x8000 ? 0.003921569f : 0.007843138f;
        i32 blade = -1;
        if (object->field_280 != 0xff && Cheat_IsOn(0x19))
            blade = 0;
        else if (object->field_280 != 0xff && Player_HasPurpleForce(object))
            blade = 3;
        else if (object->id == id_GRIEVOUS) {
            f32 phase =
                (NuTrigTable[(u16)(i32)(NuFmod(GameTimer.time_elapsed, 2.0f) * 0.5f * 65536.0f) >> 1] + 1.0f) * 0.5f;
            colour.x =
                ((BladeTab[2].colour[0] - static_cast<f32>(BladeTab[1].colour[0])) * phase + BladeTab[1].colour[0]) *
                brightness;
            colour.y =
                ((BladeTab[2].colour[1] - static_cast<f32>(BladeTab[1].colour[1])) * phase + BladeTab[1].colour[1]) *
                brightness;
            colour.z =
                ((BladeTab[2].colour[2] - static_cast<f32>(BladeTab[1].colour[2])) * phase + BladeTab[1].colour[2]) *
                brightness;
        } else
            blade = static_cast<i8>(object->character_data->game_character->blade_colour);
        if (blade != -1) {
            colour.x = BladeTab[blade].colour[0] * brightness;
            colour.y = BladeTab[blade].colour[1] * brightness;
            colour.z = BladeTab[blade].colour[2] * brightness;
        }
        rtlDynamicSetColours(object->dynamic_light_id, &colour, NULL);
        rtlDynamicSetColours(object->dynamic_light_id, &colour, NULL);
        rtlDynamicSetPos(object->dynamic_light_id, &position);
    }
}

extern "C" NUVEC *__cdecl GetZapOrigin(GAMEOBJECT *object) {
    NUVEC *origin = &object->collision_position;
    if (object->origin_joint_ready != 0) {
        PLAYERCHARACTERCONFIG_PC *config = object->character_data->game_character;
        i32 joint = config->weapon_shoot_joints[0];
        if (joint == -1 || object->character_model->points_of_interest[joint] == NULL)
            joint = config->weapon_joints[0];
        if (joint != -1 && object->character_model->points_of_interest[joint] != NULL)
            origin = (NUVEC *)&object->locator_matrices[joint].m30;
    }
    return origin;
}

extern "C" {
    extern f32 PlayerOpponent_Range2;
    GAMEOBJECT *__cdecl PlayerOpponent(GAMEOBJECT *object, f32 range, i32 arc) {
        NUVEC forward, delta;
        f32 nearest_distance = 1.0e8f;
        GAMEOBJECT *nearest = NULL;
        if (object->pad->input_magnitude > 0.0f)
            NuVecRotateY(&forward, &v001, GamePad_InputAngle(object, object->pad));
        for (i32 i = 0; i < HIGHGAMEOBJECT; ++i) {
            GAMEOBJECT *target = &Obj[i];
            if (target == object || (target->flags_1fc_word & 0x1001) != 0x1001 || target->dead != 0 ||
                (target->flags_1f8 & 1) != 0 || target->character_context == 0x39 ||
                target->character_context == 0x3b || target->character_context == 0x3c ||
                (target->character_data->game_character->flags_090 & 0x8000) != 0 ||
                (CInfo[target->character_context].flags & 0x8000) != 0 || target == object->takeover_source)
                continue;
            f32 distance = NuVecDistSqr(&target->position, &object->position, &delta);
            if (distance >= range * range ||
                (object->pad->input_magnitude > 0.0f && forward.x * delta.x + forward.z * delta.z < 0.0f))
                continue;
            if (arc != 0) {
                u16 angle = NuAtan2D(target->position.x - object->position.x, target->position.z - object->position.z);
                i32 difference = RotDiff(object->movement_facing_angle, angle);
                if (difference < -arc || difference > arc)
                    continue;
            }
            if (distance < nearest_distance) {
                nearest = target;
                nearest_distance = distance;
            }
        }
        PlayerOpponent_Range2 = nearest_distance;
        return nearest;
    }

    typedef struct COMBO_AI_VIEW_PC {
        u8 unknown_000[0x39c];
        GAMEOBJECT **primary_target_ref;
        f32 nearest_opponent_metric;
    } COMBO_AI_VIEW_PC;
    STATIC_ASSERT(offsetof(COMBO_AI_VIEW_PC, primary_target_ref) == 0x39c);
    STATIC_ASSERT(offsetof(COMBO_AI_VIEW_PC, nearest_opponent_metric) == 0x3a0);

    u8 __cdecl SetComboOpponent(GAMEOBJECT *object, f32 range, i32 update_heading, i32 mode) {
        i32 searched = 0;
        u8 result = 0;
        COMBO_AI_VIEW_PC *ai = (COMBO_AI_VIEW_PC *)object;
        object->force_target = NULL;
        object->blowup_target = NULL;
        if ((object->flags_1fc & 0x80) != 0 && mode == 0 && object->pad->input_magnitude > 0.0f) {
            object->force_target = ObjOpponent(object, range, 0.0f, 0, 0, 1);
            searched = 1;
            if (object->force_target != NULL)
                result = 1;
        }
        if (object->force_target == NULL && ai->primary_target_ref != NULL && ai->nearest_opponent_metric < range) {
            ComboOpponent_Range2 = ai->nearest_opponent_metric;
            object->force_target = *ai->primary_target_ref;
            result = 1;
            goto finished;
        }
        if ((object->flags_1fc & 0x80) == 0) {
            if ((object->portal_flags & 0x40) != 0 || object->field_f00 == 5) {
                object->blowup_target =
                    (BLOWUP_TARGET_PC *)GizmoBlowUpOpponent(object, range, 0.0f, 0.0f, mode, 0, 0, 0);
                if (object->blowup_target != NULL) {
                    ComboOpponent_Range2 = GizmoBlowUpOpponent_Range2;
                    result = 4;
                }
            }
            goto finished;
        }
        if (!searched) {
            object->force_target = ObjOpponent(object, range, 0.0f, 0, 0, 1);
            if (object->force_target != NULL)
                result = 1;
        }
        if (object->force_target == NULL) {
            object->blowup_target = (BLOWUP_TARGET_PC *)GizmoBlowUpOpponent(object, range, 0.0f, 0.0f, mode, 0, 0, 0);
            if (object->blowup_target != NULL) {
                ComboOpponent_Range2 = GizmoBlowUpOpponent_Range2;
                result = 4;
                goto finished;
            }
            object->force_target = PlayerOpponent(object, range, 0x2000);
            if (object->force_target != NULL) {
                ComboOpponent_Range2 = PlayerOpponent_Range2;
                result = 2;
                goto finished;
            }
        }
        if (!result && !searched) {
            object->force_target = ObjOpponent(object, range, 0.0f, 0, 0, 1);
            result = object->force_target != NULL;
        }
    finished:
        if (update_heading != 0 && object->force_target == NULL && object->pad->input_magnitude > 0.0f &&
            (object->flags_e20 & 0x200000) != 0)
            object->movement_facing_angle = object->current_input_angle;
        return result;
    }

    void __cdecl SwipeCode(GAMEOBJECT *object, i32 pressed, i32 held) {
        if (object->character_context == 0x10) {
            f32 *frame = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
            if (frame != NULL) {
                if ((object->context_variant_flags & 0x40) == 0) {
                    f32 hit_frame = AnimListFrame(object->character_model, object->field_7aa, 0);
                    if (hit_frame >= 1.0f && hit_frame <= *frame)
                        ComboHitFrame(object, 1);
                }
                object->context_timer -= FRAMETIME;
                if (object->context_timer <= 0.0f) {
                    object->flags_e20 |= 4;
                    object->character_context = -1;
                    if (object->field_7aa == 0x29) {
                        if ((object->context_variant_flags & 0x40) == 0) {
                            ComboHitFrame(object, 1);
                            object->character_context = 0x10;
                            object->context_timer = 0.001f;
                        } else if (held)
                            StartHold(object);
                    }
                }
            }
        } else if ((object->flags_e20 & 0x10000) != 0 && object->field_e33 == 0 && pressed && object->field_281 != 0 &&
                   object->character_model != NULL &&
                   (object->character_context == 1 || object->character_context == -1 ||
                    object->character_context == 2 || object->character_context == 4 ||
                    object->character_context == 3) &&
                   SetComboOpponent(object, 0.6f, 0, 2) == 1) {
            NUVEC *position;
            if (object->force_target != NULL)
                position = &object->force_target->position;
            else if (object->blowup_target != NULL)
                position = &object->blowup_target->mid_position;
            else
                return;
            NUVEC delta;
            NuVecSub(&delta, position, &object->position);
            NuVecRotateY(&delta, &delta, -object->movement_facing_angle);
            if ((object->pad->input_magnitude <= 0.0f || fabs((f64)delta.x) <= fabs((f64)delta.z)) && delta.z < 0.0f &&
                ComboOpponent_Range2 < 0.48999998f) {
                object->field_7aa = 0x29;
                if (object->character_model->model_data_b[0x29] != NULL) {
                    object->context_timer = AnimDuration(object->id, 0x29, 0.0f, 0.0f, 1);
                    if (object->context_timer > 0.0f) {
                        object->character_context = 0x10;
                        object->context_variant_flags &= ~0x40;
                        PlaySabreSfx(NULL, object, NULL, 1);
                        NewRumble(object->pad->bound_pad, 0.4f, 0);
                    }
                }
            }
        }
    }
}
