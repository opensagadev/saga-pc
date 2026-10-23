#include "saga.h"

i32 __cdecl Batarang_StartTargetting(GAMEOBJECT *object) {
    PLAYER_BATARANG_SLOT_PC *packet;
    if ((object->character_data->game_character->flags_090 & 0x20000000) == 0)
        return 0;
    packet = object->batarang;
    packet->cooldown = (i16)(50 + (object->id == (u16)ID_DEFAULTCHARACTER_B));
    packet = object->batarang;
    if (WORLD->level_objects[packet->cooldown].active == 0)
        return 0;
    object->context_variant_flags |= 0x20000;
    object->field_7b3 = 0;
    object->context_timer = 0.0f;
    object->landing_followup = 0;
    object->character_context = 0x4d;
    object->field_7aa = 0x90;
    packet->owner = object;
    object->batarang->active = 0;
    object->batarang->sight_position.x = object->camera_screen_position.x;
    object->batarang->sight_position.y = object->camera_screen_position.y;
    object->batarang->sight_position.z = 1.0f;
    object->batarang->sight_velocity.z = 0.0f;
    object->batarang->sight_velocity.y = 0.0f;
    object->batarang->sight_velocity.x = 0.0f;
    packet = object->batarang;
    KeepPointOnScreen(&packet->sight_position, &packet->sight_velocity);
    return 1;
}

typedef struct BATARANG_LOST_TARGET_PC {
    void *object;
    u8 type;
    u8 lost;
    u8 opaque_06[14];
} BATARANG_LOST_TARGET_PC;
STATIC_ASSERT(sizeof(BATARANG_LOST_TARGET_PC) == 0x14);
STATIC_ASSERT(offsetof(BATARANG_LOST_TARGET_PC, object) == 0);
STATIC_ASSERT(offsetof(BATARANG_LOST_TARGET_PC, lost) == 5);

typedef struct BATARANG_LOST_READER_PC {
    BATARANG_LOST_TARGET_PC targets[5];
    u8 opaque_64[0x18];
    u8 active;
    u8 in_flight;
    u8 opaque_7e[6];
    GAMEOBJECT *owner;
    u8 opaque_88[0x24];
    i16 cooldown;
    u8 opaque_ae[6];
} BATARANG_LOST_READER_PC;
STATIC_ASSERT(sizeof(BATARANG_LOST_READER_PC) == 0xb4);
STATIC_ASSERT(offsetof(BATARANG_LOST_READER_PC, active) == 0x7c);
STATIC_ASSERT(offsetof(BATARANG_LOST_READER_PC, in_flight) == 0x7d);
STATIC_ASSERT(offsetof(BATARANG_LOST_READER_PC, owner) == 0x84);
STATIC_ASSERT(offsetof(BATARANG_LOST_READER_PC, cooldown) == 0xac);
STATIC_ASSERT(sizeof(PLAYER_BATARANG_SLOT_PC) == 0xb4);

void __cdecl Batarangs_CheckLostData(void *data) {
    BATARANG_LOST_READER_PC *batarang = (BATARANG_LOST_READER_PC *)BatarangPacketsPc;
    BATARANG_LOST_READER_PC *end = batarang + 8;
    do {
        if (batarang->owner == data) {
        reset:
            batarang->in_flight = 0;
            batarang->active = 0;
            batarang->owner = NULL;
            batarang->cooldown = 0x32;
        } else {
            BATARANG_LOST_TARGET_PC *target_data = batarang->targets;
            i32 target;
            for (target = 0; target < batarang->active; ++target, ++target_data) {
                i32 move;
                if (target_data->object != data)
                    continue;
                if (batarang->owner == data)
                    goto reset;
                if (batarang->in_flight != 0) {
                    target_data->lost = 1;
                    continue;
                }
                for (move = target + 1; move < batarang->active; ++move)
                    batarang->targets[move - 1] = batarang->targets[move];
                --batarang->active;
            }
        }
        ++batarang;
    } while (batarang != end);
}

i32 __cdecl BatarangGetObjectFromCharIdPc(i32 character) {
    return 0x32 + (character == id_ROBIN);
}

extern "C" void Batarang_GetSightInfo(i32 character, i32 *red, i32 *green, i32 *blue, char *text) {
    *red = 255;
    if (character == id_ROBIN) {
        *green = 0;
        *blue = 31;
        if (text != NULL)
            NuStrCpy(text, "\xc2\xb1");
    } else {
        *green = 223;
        *blue = 0;
        if (text != NULL)
            NuStrCpy(text, "\xc2\xa7");
    }
}

void __cdecl Batarangs_Reset(void) {
    BATARANG_LOST_READER_PC *slot = (BATARANG_LOST_READER_PC *)BatarangPacketsPc;
    i32 i;
    for (i = 0; i < 8; ++i) {
        slot[i].in_flight = 0;
        slot[i].active = 0;
        slot[i].owner = NULL;
        slot[i].cooldown = 50;
    }
}

typedef PLAYER_BATARANG_SLOT_PC BATARANG_NATIVE_PC;
#define BATARANG_PACKET_PC(object) ((object)->batarang)

STATIC_ASSERT(offsetof(GAMEOBJECT, locator_matrices) == 0x804);
STATIC_ASSERT(offsetof(DETONATOR_s, field_0x0c) == 0xc);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, mid_position) == 0x4c);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, platform_id) == 0x102);
STATIC_ASSERT(offsetof(GAMEOBJECT, camera_screen_position) + 8 == 0xc64);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7b3) == 0x7b3);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_duration) == 0x784);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_281) == 0x281);

extern "C" void Batarang_GetSightInfo(i32, i32 *, i32 *, i32 *, char *);
extern "C" f32 __cdecl NuFdiv(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f)
        return 0.0f;
    f32 result = a / b;
    return result;
}
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define PI_OVER_4 0.785398f
#define NEG_1_OVER_6 PCPathArcSineCoefficient3
#define NEG_3_OVER_40 PCPathArcSineCoefficient5
#define NEG_5_OVER_112 PCPathArcSineCoefficient7
#define NEG_35_OVER_1152 PCPathArcSineCoefficient9
#define MAX_SHORT_OVER_PI 10430.4f
#define POW2(x) ((x) * (x))
#define POW3(x) ((x) * POW2(x))
#define POW4(x) (POW2(x) * POW2(x))
#define POW5(x) (POW2(x) * POW3(x))
static i32 __cdecl BatarangASinPc(f32 sin) {
    f32 abs;
    f32 sqrt;
    f32 unknown_a;
    f32 unknown_b;
    f32 unknown_c;
    f32 unknown_d;

    union {
        f32 value;
        u32 bits;
    } magnitude;
    magnitude.value = sin;
    magnitude.bits &= 0x7fffffff;
    abs = magnitude.value;
    sqrt = NuFsqrt(1.0f - sin * sin);

    unknown_a = MIN(sqrt, abs);

    unknown_b = MAX(MIN((abs - 0.70710677f) * 3.40282e+38f, 1.0f), -1.0f);

    unknown_c = MIN(sin * 3.40282e+38f, 1.0f);
    unknown_c = MAX(unknown_c, -1.0f);

    unknown_d = unknown_b * unknown_c + unknown_c;

    return (NEG_3_OVER_40 * POW2(unknown_b * unknown_c * unknown_a) * POW3(unknown_b * unknown_c * unknown_a) +
            (unknown_d * PI_OVER_4 - (unknown_b * unknown_c * unknown_a) +
             NEG_1_OVER_6 * (unknown_b * unknown_c * unknown_a) * POW2(unknown_b * unknown_c * unknown_a)) +
            NEG_5_OVER_112 * POW3(unknown_b * unknown_c * unknown_a) * POW4(unknown_b * unknown_c * unknown_a) +
            NEG_35_OVER_1152 * POW4(unknown_b * unknown_c * unknown_a) * POW5(unknown_b * unknown_c * unknown_a)) *
           MAX_SHORT_OVER_PI;
}

#undef MIN
#undef MAX
#undef PI_OVER_4
#undef NEG_1_OVER_6
#undef NEG_3_OVER_40
#undef NEG_5_OVER_112
#undef NEG_35_OVER_1152
#undef MAX_SHORT_OVER_PI
#undef POW2
#undef POW3
#undef POW4
#undef POW5
extern "C" {
    static u32 Batarang_GetTargetPos(BATARANG_NATIVE_PC *batarang, int index, NUVEC *position) {
        if (batarang->flight_state.in_flight != 0 && batarang->active != 0 && batarang->active == index) {
            GAMEOBJECT *owner = batarang->owner;
            *position = owner->collision_position;
            if ((owner->flags_e24 & 8) != 0) {
                i32 joint = owner->character_data->game_character->throw_locator;
                if (joint != -1 && owner->character_model->points_of_interest[joint] != NULL)
                    *position = *reinterpret_cast<NUVEC *>(&owner->locator_matrices[joint].m30);
            }
            return 1;
        }
        if (index >= batarang->active)
            return 0;
        BATARANG_TARGET_s *target = &batarang->targets[index];
        if (target->type == 0) {
            *position = static_cast<GAMEOBJECT *>(target->object)->collision_position;
        } else if (target->type == 1) {
            *position = static_cast<DETONATOR_s *>(target->object)->field_0x0c;
        } else if (target->type == 2) {
            *position = static_cast<GIZMOBLOWUP_PC *>(target->object)->mid_position;
        } else if (target->type == 3) {
            *position = *static_cast<NUVEC *>(target->object);
        } else {
            return 0;
        }
        return 1;
    }
}

extern "C" i32 Batarang_InitRicochet(BATARANG_NATIVE_PC *batarang, NUVEC *normal) {
    if (batarang == NULL || normal == NULL) {
        return 0;
    }
    if (batarang->ricochet_state.count >= 5) {
        --batarang->ricochet_state.count;
        batarang->ricochet_state.flags &= ~1u;
        batarang->ricochet_normal = v000;
        return 0;
    }
    batarang->ricochet_state.flags |= 1;
    batarang->ricochet_normal = *normal;
    ++batarang->ricochet_state.count;
    batarang->ricochet_state.timer = 0.0f;
    NUVEC original_velocity = batarang->velocity;
    NUVEC reflected_velocity = batarang->velocity;
    NUVEC axis;
    NuVecCross(&axis, &batarang->ricochet_normal, &reflected_velocity);
    NuVecNorm(&axis, &axis);
    NUMTX rotation;
    NuMtxSetIdentity(&rotation);
    f32 magnitudes = NuVecMag(&batarang->ricochet_normal) * NuVecMag(&reflected_velocity);
    f32 dot = NuVecDot(&batarang->ricochet_normal, &reflected_velocity);
    f32 cosine = NuFdiv(dot, magnitudes);
    i16 angle = 0x4000 - BatarangASinPc(cosine);
    NuMtxSetRotationAxis(&rotation, 0x8000 - angle * 2, &axis);
    NuVecMtxRotate(&reflected_velocity, &reflected_velocity, &rotation);
    f32 speed = NuVecNorm(&original_velocity, &original_velocity);
    NuVecNorm(&reflected_velocity, &reflected_velocity);
    NuVecAdd(&reflected_velocity, &reflected_velocity, &original_velocity);
    NuVecNorm(&reflected_velocity, &reflected_velocity);
    NuVecScale(&reflected_velocity, &reflected_velocity, speed * 0.8f);
    batarang->velocity.x = SeekLinearF(batarang->velocity.x, reflected_velocity.x, 10.0f);
    batarang->velocity.y = SeekLinearF(batarang->velocity.y, reflected_velocity.y, 10.0f);
    batarang->velocity.z = SeekLinearF(batarang->velocity.z, reflected_velocity.z, 10.0f);
    return 1;
}

static void Batarang_Ricochet(BATARANG_NATIVE_PC *batarang) {
    if (batarang == NULL || (batarang->ricochet_state.flags & 1) == 0) {
        return;
    }
    if (batarang->ricochet_state.timer < 0.2f) {
        batarang->ricochet_state.timer += FRAMETIME;
    } else {
        batarang->ricochet_state.timer = 0.0f;
        batarang->ricochet_state.flags &= ~1u;
        batarang->ricochet_normal = v000;
    }
}

static i32 Batarang_ShouldRicochet(BATARANG_NATIVE_PC *batarang) {
    NUVEC travel, destination;
    NuVecScale(&travel, &batarang->velocity, FRAMETIME * 2.0f);
    NuVecAdd(&destination, &batarang->position, &travel);
    if (!NewRayCastScaleYMask(&batarang->position, &travel, 0.0f, 1.0f, 0, 0x1f))
        return 0;
    i32 platform = TerrainPlatId();
    if (batarang->flight_state.in_flight != 0 && batarang->flight_state.current_target >= batarang->active)
        return 1;
    BATARANG_TARGET_s *current = &batarang->targets[batarang->flight_state.current_target];
    switch (current->type) {
        case 0:
        case 1:
        case 3:
            return 1;
        case 2:
            return platform != static_cast<GIZMOBLOWUP_PC *>(current->object)->platform_id;
        default:
            return 0;
    }
}

extern "C" i32 Batarang_SeekToTarget(BATARANG_NATIVE_PC *batarang) {
    if (batarang == NULL) {
        return 0;
    }
    NUVEC target;
    if (batarang->flight_state.current_target > batarang->active ||
        batarang->targets[batarang->flight_state.current_target].lost != 0) {
        target = batarang->owner->collision_position;
    } else if (!Batarang_GetTargetPos(batarang, batarang->flight_state.current_target, &target)) {
        return 0;
    }

    const f32 rate =
        batarang->flight_state.flight_time > 1.0f ? (batarang->flight_state.flight_time - 1.0f) * 5.0f + 5.0f : 5.0f;
    if (batarang->flight_state.flight_time < 4.0f) {
        if (Batarang_ShouldRicochet(batarang)) {
            NUVEC normal;
            NewRayCastGetImpactNormal(&normal);
            Batarang_InitRicochet(batarang, &normal);
        }
    }

    if ((batarang->ricochet_state.flags & 1) != 0) {
        Batarang_Ricochet(batarang);
    } else {
        NUVEC desired;
        NuVecSub(&desired, &target, &batarang->position);
        NuVecNorm(&desired, &desired);
        NuVecScale(&desired, &desired, 5.0f);
        batarang->velocity.x = SeekValF(batarang->velocity.x, desired.x, rate);
        batarang->velocity.y = SeekValF(batarang->velocity.y, desired.y, rate);
        batarang->velocity.z = SeekValF(batarang->velocity.z, desired.z, rate);
        batarang->ricochet_state.timer += FRAMETIME;
        if (batarang->ricochet_state.timer > 0.2f && batarang->ricochet_state.count > 0) {
            --batarang->ricochet_state.count;
            batarang->ricochet_state.timer = 0.0f;
        }
    }
    batarang->position.x += batarang->velocity.x * FRAMETIME;
    batarang->position.y += batarang->velocity.y * FRAMETIME;
    batarang->position.z += batarang->velocity.z * FRAMETIME;
    return NuVecDistSqr(&batarang->position, &target, NULL) < 0.0625f;
}

extern "C" void Batarang_MoveCode(GAMEOBJECT *object) {
    BATARANG_NATIVE_PC *batarang;
    if (object->character_context == 0x50) {
        object->flags_e20 |= 0x00400000;
        if (object->hold_timer >= 0.25f) {
            Batarang_StartTargetting(object);
            return;
        }
        if (AnimPlaying(&object->anim_packet, object->field_7aa, 0, 0) == NULL) {
            return;
        }
        if (object->pad->input_magnitude > 0.0f) {
            object->character_context = -1;
            return;
        }
        f32 timer = object->context_timer - FRAMETIME;
        object->context_timer = timer;
        if (!(timer <= 0.0f)) {
            return;
        }
        if ((object->pad->buttons_held & GAMEPAD_ACTION) != 0) {
            if (Batarang_StartTargetting(object) != 0) {
                return;
            }
            object->hold_timer = 0.0f;
        }
        object->character_context = -1;
        return;
    }
    if (object->character_context == 0x4d) {
        if (object->field_7b3 == 0) {
            object->flags_e20 |= 0x00400000;
            object->context_variant_flags |= 0x20000;
            if (((object->pad->buttons_pressed & GAMEPAD_JUMP) != 0)) {
                StartJump(object, 0);
                return;
            }
            if ((object->pad->buttons_held & GAMEPAD_ACTION) == 0 && !(object->context_timer < 0.3f)) {
                if (BATARANG_PACKET_PC(object)->active > 0) {
                    object->context_variant_flags &= ~0x20000U;
                    object->field_7b3 = 1;
                    object->field_7aa = 0x65;
                    f32 duration = AnimDuration((i16)object->id, 0x65, 0.0f, 0.0f, 1);
                    object->context_duration = duration;
                    if (duration <= 0.0f) {
                        object->context_duration = 0.5f;
                    }
                    object->context_timer = 0.0f;
                    object->context_variant_flags &= ~0x40U;
                    if (object->character_model->model_data_b[object->field_7aa] == NULL ||
                        AnimListFrame(object->character_model, object->field_7aa, 0) != 0.0f) {
                        object->context_variant_flags |= 0x40;
                        object->batarang_release = 1;
                    } else {
                        object->context_variant_flags &= ~0x40U;
                    }
                } else {
                    object->character_context = -1;
                }
            } else {
                if ((object->character_model->model_data_b[object->field_7aa] == NULL ||
                     AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) != NULL)) {
                    GAMEPAD_PC *pad = object->pad;
                    object->context_timer += FRAMETIME;
                    f32 x = pad->input_direction_z * 1.25f;
                    f32 y = 1.25f * pad->input_direction_x;
                    if (object->context_timer < 0.25f) {
                        const f32 ramp = object->context_timer * 4.0f;
                        x *= ramp;
                        y *= ramp;
                    }
                    BATARANG_PACKET_PC(object)->sight_velocity.x =
                        SeekValF(BATARANG_PACKET_PC(object)->sight_velocity.x, x, 10.0f);
                    BATARANG_PACKET_PC(object)->sight_velocity.y =
                        SeekValF(BATARANG_PACKET_PC(object)->sight_velocity.y, y, 10.0f);
                    BATARANG_PACKET_PC(object)->sight_position.x +=
                        BATARANG_PACKET_PC(object)->sight_velocity.x * FRAMETIME;
                    BATARANG_PACKET_PC(object)->sight_position.y +=
                        BATARANG_PACKET_PC(object)->sight_velocity.y * FRAMETIME;
                }
                batarang = BATARANG_PACKET_PC(object);
                KeepPointOnScreen(&batarang->sight_position, &batarang->sight_velocity);
                if (Batarang_FindTarget(WORLD, object, 0) != 0) {
                    NewBuzzFrames(object->pad->bound_pad, 1, 0);
                }

                {
                    i32 colour[3];
                    char text[32];
                    NUVEC position;
                    Batarang_GetSightInfo((i16)object->id, &colour[0], &colour[1], &colour[2], text);
                    ADD_GAME_MESSAGE_PC message;
                    memcpy(&message, &AddGameMsg_Default, sizeof(message));
                    message.scale = 1.0f;
                    message.green = static_cast<u8>(colour[1]);
                    message.position = &BATARANG_PACKET_PC(object)->sight_position;
                    message.red = static_cast<u8>(colour[0]);
                    message.blue = static_cast<u8>(colour[2]);
                    message.text = text;
                    message.alpha = static_cast<i32>(((f64)0.2f * game_pulse + (f64)0.8f) * 128.0);
                    message.flags = 0x1080;
                    AddGameMsg(&message);
                    message.text = TagAsciiDownPc;
                    message.flags = 0x87;
                    message.scale = 3.0f;
                    message.unknown_4f = 4;
                    message.alpha = static_cast<i32>(128.0 * ((f64)0.8f - (f64)0.2f * game_pulse));
                    for (i32 i = 0; i < BATARANG_PACKET_PC(object)->active; ++i) {
                        if (Batarang_GetTargetPos(BATARANG_PACKET_PC(object), i, &position) != 0) {
                            message.position = &position;
                            AddGameMsg(&message);
                        }
                    }
                }
            }
        } else if (object->field_7b3 == 1) {
            f32 *frame = NULL;
            object->context_variant_flags &= ~0x20000U;
            if ((object->pad->buttons_pressed & GAMEPAD_JUMP) != 0) {
                object->landing_followup = 1;
            }
            u32 released = (object->context_variant_flags >> 6) & 1;
            if (released && object->landing_followup == 1 && 0.2f <= object->context_timer) {
                StartJump(object, 0);
                object->flags_e24 |= 0x1000;
                goto finish;
            }
            if (released && object->pad->input_magnitude > 0.0f && object->context_timer >= 0.3f) {
                object->character_context = -1;
                goto finish;
            }
            if (object->character_model->model_data_b[object->field_7aa] != NULL) {
                frame = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
                if (frame == NULL) {
                    goto finish;
                }
            }
            f32 timer = object->context_timer + FRAMETIME;
            object->context_timer = timer;
            if (object->context_duration <= timer) {
                object->character_context = -1;
                if ((object->context_variant_flags & 0x40) != 0) {
                    return;
                }
            } else {
                if ((object->context_variant_flags & 0x40) != 0) {
                    return;
                }
                if (object->character_model->model_data_b[object->field_7aa] != NULL) {
                    f32 marker = AnimListFrame(object->character_model, object->field_7aa, 2);
                    if (frame == NULL) {
                        goto finish;
                    }
                    if (!(*frame > 0.0f)) {
                        goto finish;
                    }
                    if (!(*frame >= marker)) {
                        goto finish;
                    }
                }
            }
            object->context_variant_flags |= 0x40;
            object->batarang_release = 1;
        }
    } else {
        PLAYERCHARACTERCONFIG_PC *runtime = object->character_data->game_character;
        batarang = BATARANG_PACKET_PC(object);
        if (((object->flags_1fc & 0x80) == 0 || (runtime->flags_090 & 0x20000000) == 0 || batarang == NULL ||
             batarang->flight_state.in_flight != 0 || object->use_model_origin == 0 || (object->flags_e24 & 8) == 0 ||
             object->model_draw_result == 0 || !(object->camera_screen_position.z > 0.0f) ||
             !(object->hold_timer >= 0.25f) || object->field_281 == 0 || ObjLandReady(object) == 0)) {
            return;
        }
        object->flags_e20 |= 0x00400000;
        if (0.25f <= object->hold_timer - FRAMETIME) {
            return;
        }
        Batarang_StartTargetting(object);
        return;
    }
finish:
    if ((object->context_variant_flags & 0x40) == 0 && object->batarang_release == 0) {
        object->flags_e20 |= 0x00400000;
    }
}

extern "C" void Batarang_StartThrowQuick(GAMEOBJECT *object) {
    ResetAnimPacket(&object->anim_packet, -1);
    object->character_context = 0x5e;
    object->field_7aa = 0xb2;
    object->context_timer = AnimDuration((i16)object->id, 0xb2, 0.0f, 0.0f, 1);
}

static __declspec(noinline) i32 BatarangContainsTargetPc(BATARANG_NATIVE_PC *batarang, void *target) {
    i32 index = 0;
    for (; index < batarang->active; ++index)
        if (batarang->targets[index].object == target)
            break;
    return index < batarang->active;
}
static i32 BatarangAppendTargetPc(BATARANG_NATIVE_PC *batarang, void *selected, i32 type) {
    if (selected == NULL)
        return 0;
    if (batarang->active >= 5) {
        if ((i32)((f64)GameTimer.time_elapsed / (f64)0.2f) == (i32)((f64)GameTimer.last_time_elapsed / (f64)0.2f))
            return 1;
        for (i32 index = 0; index < batarang->active - 1; ++index)
            batarang->targets[index] = batarang->targets[index + 1];
        --batarang->active;
    }
    batarang->targets[batarang->active].object = selected;
    batarang->targets[batarang->active].type = (u8)type;
    batarang->targets[batarang->active].lost = 0;
    ++batarang->active;
    return 2;
}
extern "C" i32 __cdecl Batarang_FindTarget(WORLDINFO *world, GAMEOBJECT *object, i32 automatic) {
    NUVEC delta;
    union {
        f32 value;
        u32 bits;
    } absolute, absolute_result;
    BATARANG_NATIVE_PC *batarang = BATARANG_PACKET_PC(object);
    i32 platform = -1;
    i32 selected_type = -1;
    NUVEC forward;
    NUVEC selected_position;
    NUVEC origin;
    NUVEC ray;
    void *selected;
    f32 selected_distance;
    f32 nearest_distance;
    f32 distance;
    GAMEOBJECT *nearest_object;
    GAMEOBJECT *first_object;
    GAMEOBJECT *candidate;
    i32 i;
    NuVecRotateY(&forward, &v001, object->movement_facing_angle);
    selected = NULL;
    selected_distance = 64.0f;
    nearest_object = NULL;
    first_object = NULL;
    nearest_distance = 16.0f;
    candidate = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++candidate) {
        if ((candidate->flags_1fc_word & 1) == 0 || (candidate->flags_1fc_word & 0x1000) == 0 || candidate->dead != 0 ||
            (U64_PAIR(&CInfo[candidate->character_context].flags) & 0x8000) != 0 || candidate->model_draw_result == 0 ||
            candidate == object || candidate->field_280 != 0xff || candidate->character_context == 0x3b ||
            candidate->character_context == 0x39 || candidate->character_context == 0x3c ||
            (object->flags_1f8 & 0x10004) != 0 || ((object->flags_1f8 ^ candidate->flags_1f8) & 1) == 0) {
            continue;
        }
        if (BatarangContainsTargetPc(batarang, candidate))
            continue;
        if (automatic != 0) {
            distance = NuVecDistSqr(&candidate->collision_position, &object->collision_position, &delta);
            if (!(delta.x * forward.x + delta.z * forward.z > 0.0f)) {
                continue;
            }
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest_object = candidate;
            }
        } else {
            if (((absolute.value = (candidate->camera_screen_position.x - batarang->sight_position.x)),
                 (absolute_result.bits = absolute.bits & 0x7fffffff), absolute_result.value) < (f64)0.1f &&
                ((absolute.value = (candidate->camera_screen_position.y - batarang->sight_position.y)),
                 (absolute_result.bits = absolute.bits & 0x7fffffff), absolute_result.value) < (f64)0.2f &&
                NuVecDistSqr(&candidate->collision_position, &object->collision_position, &delta) < 64.0f) {
                first_object = candidate;
                break;
            }
        }
    }
    if (nearest_object != NULL && nearest_distance < 64.0f) {
        selected_distance = nearest_distance;
        first_object = nearest_object;
    }
    if (first_object != NULL) {
        selected = first_object;
        selected_type = 0;
        platform = -1;
        selected_position.x = first_object->collision_position.x;
        selected_position.y = (f32)((first_object->upper_position.y + first_object->collision_position.y) * 0.5);
        selected_position.z = first_object->collision_position.z;
    }
    if (automatic == 0) {
        DETONATOR_s *detonator = Detonator;
        for (i = 10; i != 0; --i, ++detonator) {
            if (detonator->active == 0 || detonator->draw_result == 0 || detonator->timer == 0.0f) {
                continue;
            }
            i32 target;
            for (target = 0; target < batarang->active; ++target) {
                if (batarang->targets[target].object == detonator) {
                    break;
                }
            }
            if (target < batarang->active) {
                continue;
            }
            if (((absolute.value = (detonator->field_0x18.x - batarang->sight_position.x)),
                 (absolute_result.bits = absolute.bits & 0x7fffffff), absolute_result.value) < (f64)0.1f &&
                ((absolute.value = (detonator->field_0x18.y - batarang->sight_position.y)),
                 (absolute_result.bits = absolute.bits & 0x7fffffff), absolute_result.value) < (f64)0.1f) {
                distance = NuVecDistSqr(&detonator->field_0x0c, &object->collision_position, NULL);
                if (distance < selected_distance) {
                    selected_distance = distance;
                    selected = detonator;
                    selected_type = 1;
                    selected_position = detonator->field_0x0c;
                    platform = -1;
                }
            }
        }
    }
    GIZMOBLOWUP_PC *blowup = NULL;
    GIZMOBLOWUP_PC *nearest_blowup = NULL;
    GIZMOBLOWUP_PC *candidate_blowup = world->gizmo_blowups;
    nearest_distance = 16.0f;
    for (i = 0; i < world->gizmo_blowup_count; ++i, ++candidate_blowup) {
        if ((candidate_blowup->status_flags & 1) != 0 || (candidate_blowup->status_flags & 0x800000) == 0 ||
            (candidate_blowup->status_flags & 0x4000) == 0 || (candidate_blowup->status_flags & 0x8000) == 0 ||
            (candidate_blowup->draw_flags & 1) == 0 ||
            ((candidate_blowup->draw_flags & 0x20) != 0 && ShadowMode == 0)) {
            continue;
        }
        i32 target;
        for (target = 0; target < batarang->active; ++target) {
            if (batarang->targets[target].object == candidate_blowup) {
                break;
            }
        }
        if (target < batarang->active) {
            continue;
        }
        if (automatic != 0) {
            distance = NuVecDistSqr(&candidate_blowup->mid_position, &object->collision_position, &delta);
            if (!(delta.x * forward.x + delta.z * forward.z > 0.0f)) {
                continue;
            }
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest_blowup = candidate_blowup;
            }
        } else {
            if (((absolute.value = (candidate_blowup->screen_position.x - batarang->sight_position.x)),
                 (absolute_result.bits = absolute.bits & 0x7fffffff), absolute_result.value) < (f64)0.1f &&
                ((absolute.value = (candidate_blowup->screen_position.y - batarang->sight_position.y)),
                 (absolute_result.bits = absolute.bits & 0x7fffffff), absolute_result.value) < (f64)0.1f &&
                NuVecDistSqr(&candidate_blowup->mid_position, &object->collision_position, &delta) <
                    selected_distance) {
                blowup = candidate_blowup;
                break;
            }
        }
    }
    if (nearest_blowup != NULL && nearest_distance < selected_distance) {
        blowup = nearest_blowup;
    }
    if (blowup != NULL) {
        selected_position = blowup->mid_position;
        selected = blowup;
        selected_type = 2;
        platform = blowup->platform_id;
    }
    if (selected != NULL) {
        origin.x = object->collision_position.x;
        origin.y = (f32)((object->upper_position.y + object->collision_position.y) * 0.5);
        origin.z = object->collision_position.z;
        NuVecSub(&ray, &selected_position, &origin);
        if (NewRayCastScaleYMask(&origin, &ray, 0.0f, 1.0f, 0, 0x1f) == 0 ||
            (platform != -1 && TerrainPlatId() == platform)) {
            if (selected_type != -1 && BatarangAppendTargetPc(batarang, selected, selected_type) == 2) {
                return 1;
            }
        }
    }
    return 0;
}

extern "C" void __cdecl Batarang_Release(GAMEOBJECT *object, i32 mode) {
    BATARANG_PACKET_PC(object)->owner = object;
    BATARANG_PACKET_PC(object)->flight_state.in_flight = 1;
    BATARANG_PACKET_PC(object)->flight_state.current_target = 0;
    BATARANG_PACKET_PC(object)->flight_state.flight_time = 0.0f;
    BATARANG_PACKET_PC(object)->velocity.x = (f32)(NU_SIN_LUT(object->movement_facing_angle) * 4.0);
    BATARANG_PACKET_PC(object)->velocity.y = (f32)(object->velocity.y * 0.5);
    BATARANG_PACKET_PC(object)->velocity.z = (f32)(4.0 * NU_COS_LUT(object->movement_facing_angle));
    NewRumble(object->pad->bound_pad, 0.5f, 0);
    BATARANG_PACKET_PC(object)->position = object->collision_position;
    if (object->flags_e24 & 8) {
        i32 joint = object->character_data->game_character->throw_locator;
        if (joint != -1 && object->character_model->points_of_interest[joint] != NULL)
            BATARANG_PACKET_PC(object)->position = *reinterpret_cast<NUVEC *>(&object->locator_matrices[joint].m30);
    }
    if (BATARANG_PACKET_PC(object)->active == 0 && mode == 2) {
        Batarang_FindTarget(WORLD, object, 1);
        if (BATARANG_PACKET_PC(object)->active == 0) {
            BATARANG_PACKET_PC(object)->targets[BATARANG_PACKET_PC(object)->active].fallback_position.x =
                (f32)(NU_SIN_LUT(object->movement_facing_angle) * 2.0 + object->collision_position.x);
            BATARANG_PACKET_PC(object)->targets[BATARANG_PACKET_PC(object)->active].fallback_position.y =
                object->collision_position.y;
            BATARANG_PACKET_PC(object)->targets[BATARANG_PACKET_PC(object)->active].fallback_position.z =
                (f32)(2.0 * NU_COS_LUT(object->movement_facing_angle) + object->collision_position.z);
            BATARANG_PACKET_PC(object)->targets[BATARANG_PACKET_PC(object)->active].object =
                &BATARANG_PACKET_PC(object)->targets[BATARANG_PACKET_PC(object)->active].fallback_position;
            BATARANG_PACKET_PC(object)->targets[BATARANG_PACKET_PC(object)->active].type = 3;
            BATARANG_PACKET_PC(object)->targets[BATARANG_PACKET_PC(object)->active].lost = 0;
            ++BATARANG_PACKET_PC(object)->active;
        }
    }
}

f32 BatarangCatwomanStunTimePc = 2.0f;
extern "C" void __cdecl Batarangs_Update(void) {
    BATARANG_NATIVE_PC *batarang = reinterpret_cast<BATARANG_NATIVE_PC *>(BatarangPacketsPc);
    for (i32 index = 0; index < 8; ++index, ++batarang) {
        if (batarang->flight_state.in_flight == 0)
            continue;
        batarang->flight_state.flight_time += FRAMETIME;
        if (!(batarang->flight_state.flight_time < 2.0f) && batarang->flight_state.flight_time > 4.0f)
            batarang->flight_state.flight_time = 4.0f;
        while (batarang->targets[batarang->flight_state.current_target].lost != 0 &&
               batarang->flight_state.current_target < batarang->active)
            ++batarang->flight_state.current_target;
        if (!Batarang_SeekToTarget(batarang))
            continue;
        if (batarang->flight_state.current_target >= batarang->active) {
            batarang->owner->hold_timer = 0;
            NewBuzzFrames(batarang->owner->pad->bound_pad, 1, 0);
            GAMEOBJECT *owner = batarang->owner;
            if (owner->pad->buttons_held & GAMEPAD_ACTION)
                Batarang_StartTargetting(owner);
            else if (owner->pad->input_magnitude == 0 && owner->character_context == -1 &&
                     owner->character_model->model_data_b[0x98] != NULL) {
                owner->character_context = 0x50;
                owner->field_7aa = 0x98;
                batarang->owner->context_timer =
                    AnimDuration((i16)batarang->owner->id, batarang->owner->field_7aa, 0, 0, 1);
            }
            batarang->flight_state.in_flight = 0;
            batarang->active = 0;
            batarang->owner = NULL;
            batarang->cooldown = 50;
            continue;
        }
        BATARANG_TARGET_s target = batarang->targets[batarang->flight_state.current_target];
        if (target.type != 3) {
            NewRumble(batarang->owner->pad->bound_pad, 0.5f, 0);
            NewBuzzFrames(batarang->owner->pad->bound_pad, 1, 0);
            GameCam_HitJudder();
        }
        ++batarang->flight_state.current_target;
        batarang->flight_state.flight_time = 0;
        if (target.type == 0) {
            GAMEOBJECT *victim = static_cast<GAMEOBJECT *>(target.object);
            if (victim->id == (u16)PunchRangeUnitIdPc && victim->field_280 == 0xff) {
                victim->portal_flags |= 1;
                PlayerTakeHitReactionPc(victim, batarang->owner, BatarangCatwomanStunTimePc, 0);
            } else if (!(victim->portal_flags & 0x8000000))
                ObjHitObj(batarang->owner, victim, 1, 0, 0, 1);
        } else if (target.type == 1)
            Detonator_Detonate(static_cast<DETONATOR_s *>(target.object));
        else if (target.type == 2)
            GizmoBlowupBlowup(static_cast<GIZMOBLOWUP_PC *>(target.object), 1, 2, 2, NULL, 1);
    }
}
extern "C" void __cdecl Batarangs_Draw(void) {
    BATARANG_NATIVE_PC *batarang = reinterpret_cast<BATARANG_NATIVE_PC *>(BatarangPacketsPc);
    for (i32 index = 0; index < 8; ++index, ++batarang) {
        if (batarang->flight_state.in_flight && WORLD->level_objects[batarang->cooldown].active) {
            NUMTX matrix;
            NuMtxSetTranslation(&matrix, &batarang->position);
            NuSpecialDrawAt(&WORLD->level_objects[batarang->cooldown].special, &matrix);
        }
    }
}
struct BATARANG_DETONATOR_HIT_VIEW_PC {
    u8 unknown00[0xc];
    f32 scale;
    u8 unknown10[0x10];
    f32 delay;
};
STATIC_ASSERT(offsetof(BATARANG_DETONATOR_HIT_VIEW_PC, scale) == 0xc);
STATIC_ASSERT(offsetof(BATARANG_DETONATOR_HIT_VIEW_PC, delay) == 0x20);
extern "C" {
    f32 DetonatorHitScaleArmedPc = 0.75f;
    f32 DetonatorHitScaleIdlePc = 0.08f;
    f32 BatarangDetonatorHitDelayPc = 2.0f;
}
extern "C" void __cdecl Detonator_Detonate(DETONATOR_s *detonator) {
    Batarangs_CheckLostData(detonator);
    Detonate(&detonator->field_0x0c, 0);
    BATARANG_DETONATOR_HIT_VIEW_PC *hit_data = static_cast<BATARANG_DETONATOR_HIT_VIEW_PC *>(detonator->field_0x34);
    detonator->active = 0;
    if (hit_data != NULL) {
        hit_data->delay = BatarangDetonatorHitDelayPc;
        detonator->field_0x34 = NULL;
    }
}

extern "C" void __cdecl Detonators_Update(void) {
    DETONATOR_s *detonator = Detonator;
    for (i32 index = 0; index < 10; ++index, ++detonator) {
        if (detonator->active == 0)
            continue;
        detonator->timer += FRAMETIME;
        NuCameraTransformScreenClip(&detonator->field_0x18, &detonator->field_0x0c, 1, NULL);
        if (detonator->timer >= 10.7f) {
            Detonator_Detonate(detonator);
        } else if (detonator->field_0x34 != NULL) {
            BATARANG_DETONATOR_HIT_VIEW_PC *hit_data =
                static_cast<BATARANG_DETONATOR_HIT_VIEW_PC *>(detonator->field_0x34);
            if (!(detonator->timer >= 10.0f) && (detonator->object == NULL || detonator->object->dead != 0 ||
                                                 !(detonator->object->field_de0 >= 0.3f))) {
                hit_data->scale = DetonatorHitScaleIdlePc;
            } else {
                hit_data->scale = DetonatorHitScaleArmedPc;
            }
        }
    }
}

extern "C" void __cdecl Detonators_Draw(void) {
    if (!WORLD->level_objects[0xec].active)
        return;
    DETONATOR_s *detonator = Detonator;
    for (i32 index = 0; index < 10; ++index, ++detonator) {
        NUMTX matrix;
        detonator->draw_result = 0;
        if (!detonator->active)
            continue;
        NuMtxSetRotationY(&matrix, detonator->rotation_y);
        NuMtxRotateZ(&matrix, detonator->rotation_z);
        NuMtxRotateX(&matrix, detonator->rotation_x);
        NuMtxTranslate(&matrix, &detonator->position);
        NuSpecialDrawAt(&WORLD->level_objects[0xec].special, &matrix);
        if ((u32)GameTimer.update_count % 6 < 3 &&
            (detonator->timer >= 10.0f ||
             (detonator->object != NULL && detonator->object->dead == 0 && detonator->object->field_de0 >= 0.3f))) {
            if (WORLD->level_objects[0xef].active)
                detonator->draw_result = (u8)NuSpecialDrawAt(&WORLD->level_objects[0xef].special, &matrix);
        } else {
            if (WORLD->level_objects[0xee].active)
                detonator->draw_result = (u8)NuSpecialDrawAt(&WORLD->level_objects[0xee].special, &matrix);
        }
    }
}

extern "C" const f64 GameObjectNearFloorNoFloorHeight;

extern "C" {
    static __declspec(noinline) DETONATOR_s *DetonatorPlacementSlot(GAMEOBJECT *object) {
        i32 count = 0;
        i32 available = -1;
        for (i32 index = 0; index < 10; ++index) {
            if (Detonator[index].active != 0 && Detonator[index].object == object) {
                if (++count == 3)
                    return NULL;
            } else if (available == -1) {
                available = index;
            }
        }
        if (available == -1)
            return NULL;
        return &Detonator[available];
    }
}

extern "C" DETONATOR_s *__cdecl Detonator_FindNearest(NUVEC *position, f32 radius, GAMEOBJECT *owner) {
    DETONATOR_s *nearest = NULL;
    DETONATOR_s *detonator = Detonator;
    f32 nearest_distance = radius != 0.0f ? radius * radius : 1000000000.0;
    for (i32 index = 0; index < 10; ++index, ++detonator) {
        if (detonator->active == 0)
            continue;
        if (owner != NULL && detonator->object != owner)
            continue;
        f32 distance = NuVecDistSqr(position, &detonator->position, NULL);
        if (distance < nearest_distance) {
            nearest_distance = distance;
            nearest = detonator;
        }
    }
    return nearest;
}

#define DETONATOR_CONSIDER_OLDEST(detonator)                                                                           \
    if ((detonator)->active != 0 && (detonator)->timer > oldest_time) {                                                \
        oldest_time = (detonator)->timer;                                                                              \
        oldest = (detonator);                                                                                          \
    }

extern "C" {
    static __declspec(noinline) DETONATOR_s *Detonator_FindOldest(void) {
        f32 oldest_time = -1.0f;
        DETONATOR_s *oldest = NULL;
        DETONATOR_CONSIDER_OLDEST(&Detonator[0]);
        DETONATOR_CONSIDER_OLDEST(&Detonator[1]);
        DETONATOR_CONSIDER_OLDEST(&Detonator[2]);
        DETONATOR_CONSIDER_OLDEST(&Detonator[3]);
        DETONATOR_CONSIDER_OLDEST(&Detonator[4]);
        DETONATOR_CONSIDER_OLDEST(&Detonator[5]);
        DETONATOR_CONSIDER_OLDEST(&Detonator[6]);
        DETONATOR_CONSIDER_OLDEST(&Detonator[7]);
        DETONATOR_CONSIDER_OLDEST(&Detonator[8]);
        DETONATOR_CONSIDER_OLDEST(&Detonator[9]);
        return oldest;
    }
}

extern "C" void __cdecl Detonator_MoveCode(GAMEOBJECT *object) {
    if (object->character_context != 0x49 && object->character_context != 0x48) {
        if (WORLD->level_objects[0xec].active == 0 || (object->flags_1fc & 0x80) == 0 ||
            (object->field_281 == 0 && object->jump_variant_state != 1) ||
            (ObjLandReady(object) == 0 && (*(u64 *)&CInfo[object->character_context].flags & 0x2000) == 0)) {
            object->flags_e20 |= 0x8000;
            object->field_de0 = 0.0f;
            return;
        }
        if ((object->pad->buttons_held & GAMEPAD_SPECIAL) != 0) {
            if (!(object->field_de0 > 0.0f) && (object->flags_e20 & 0x8000) != 0)
                return;
            object->flags_e20 |= 0x8000;
            object->field_de0 += FRAMETIME;
            if (object->field_de0 >= 0.3f && object->field_de0 >= 1.0f) {
                DETONATOR_s *oldest = Detonator_FindOldest();
                if (oldest != NULL) {
                    Detonator_Detonate(oldest);
                    object->field_de0 = 0.7f;
                } else {
                    object->field_de0 = 0.0f;
                    object->flags_e20 |= 0x8000;
                }
            }
            return;
        }
        if (object->field_de0 > 0.0f && object->field_de0 < 0.3f) {
            DETONATOR_s *nearest =
                Detonator_FindNearest(&object->lower_position, object->bounds_radius + 0.0775f, object);
            if (nearest != NULL) {
                object->field_7b3 = 0;
                object->character_context = 0x49;
                FastWeaponIn(object, 0);
                object->field_7aa = 0x66;
                object->grapple = nearest;
                u16 angle =
                    (u16)NuAtan2D(nearest->position.x - object->position.x, nearest->position.z - object->position.z);
                nearest = (DETONATOR_s *)object->grapple;
                object->movement_facing_angle = angle;
                object->launch_origin.x = nearest->position.x - NU_SIN_LUT(angle) * 0.1684f;
                object->launch_origin.z = nearest->position.z - NU_COS_LUT(angle) * 0.1684f;
                object->takeover_start_angle = (u16)RotDiff(angle, nearest->rotation_y);
            } else if (DetonatorPlacementSlot(object) != NULL &&
                       object->floor_height != GameObjectNearFloorNoFloorHeight &&
                       object->lower_position.y - object->floor_height < 0.155f) {
                object->launch_origin.x = object->lower_position.x;
                object->launch_origin.y = object->floor_height;
                object->launch_origin.z = object->lower_position.z;
                if (Detonator_FindNearest(&object->launch_origin, 0.155f, NULL) == NULL) {
                    object->field_7b3 = 0;
                    object->character_context = 0x48;
                    FastWeaponIn(object, 0);
                    object->field_7aa = 0x8d;
                    object->field_7a4 = object->field_1056;
                    object->takeover_start_angle = (u16)qrand();
                    object->field_7a8 = object->field_1058;
                } else {
                    GameAudio_PlaySfx(0x32, NULL, 0, 0);
                }
            } else {
                GameAudio_PlaySfx(0x32, NULL, 0, 0);
            }
            if (object->character_context == 0x48 || object->character_context == 0x49) {
                object->velocity.z = 0.0f;
                object->velocity.x = 0.0f;
                if (AnimPlaying(&object->anim_packet, object->field_7aa, 1, 1) != NULL)
                    ResetAnimPacket(&object->anim_packet, -1);
                if (object->character_model->model_data_b[object->field_7aa] != NULL)
                    object->context_timer = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
                else
                    object->context_timer = 1.0f;
            }
        }
        object->flags_e20 &= ~0x8000;
        object->field_de0 = 0.0f;
        return;
    }

    object->flags_e20 |= 0x8000;
    object->field_de0 = 0.0f;
    f32 *frame = NULL;
    if (object->character_model->model_data_b[object->field_7aa] != NULL) {
        frame = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
        if (frame == NULL)
            return;
    }
    object->context_timer -= FRAMETIME;
    if (object->context_timer <= 0.0f) {
        object->character_context = -1;
        if (object->field_7b3 != 0)
            return;
    } else {
        if (object->field_7b3 != 0)
            return;
        if (object->character_model->model_data_b[object->field_7aa] != NULL) {
            f32 marker =
                AnimListFrame(object->character_model, object->field_7aa, object->character_context == 0x49 ? 0 : 2);
            if (frame == NULL || !(*frame > 0.0f) || *frame < marker)
                return;
        } else if (!(object->context_timer < 0.5f)) {
            return;
        }
    }
    object->field_7b3 = 1;
    if (object->character_context == 0x49) {
        DETONATOR_s *detonator = (DETONATOR_s *)object->grapple;
        if (detonator != NULL) {
            detonator->active = 0;
            if (((DETONATOR_s *)object->grapple)->field_0x34 != NULL) {
                GameAntinode_UnregisterAntiNode(WORLD->game_antinode_sys, ((DETONATOR_s *)object->grapple)->field_0x34);
                ((DETONATOR_s *)object->grapple)->field_0x34 = NULL;
            }
        }
        return;
    }
    DETONATOR_s *detonator = DetonatorPlacementSlot(object);
    if (detonator != NULL) {
        detonator->position = object->launch_origin;
        detonator->active = 1;
        detonator->object = object;
        detonator->rotation_x = object->field_7a4;
        detonator->rotation_y = (u16)(object->takeover_start_angle + object->field_0x276);
        detonator->rotation_z = object->field_7a8;
        detonator->field_0x0c.z = 0.0f;
        detonator->field_0x0c.x = 0.0f;
        detonator->field_0x0c.y = 0.0775f;
        NuVecRotateZ(&detonator->field_0x0c, &detonator->field_0x0c, detonator->rotation_z);
        NuVecRotateX(&detonator->field_0x0c, &detonator->field_0x0c, detonator->rotation_x);
        NuVecAdd(&detonator->field_0x0c, &detonator->field_0x0c, &detonator->position);
        detonator->timer = 0.0f;
        detonator->field_0x34 = GameAntinode_RegisterAntiNode(WORLD->game_antinode_sys, &detonator->field_0x0c, 1.0f,
                                                              1.0f, 1.0f, 0, 0, 0.0f);
        PlaySfx((char *)"imp_thermalDet_attach", &detonator->position);
    } else {
        GameAudio_PlaySfx(0x32, NULL, 0, 0);
    }
}
