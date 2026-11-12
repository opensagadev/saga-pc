#include "saga.h"
#pragma data_seg(".data$id_ROBIN")
static i16 id_ROBIN = -1;

/* Retail unit 0x403c90-0x407e40 (legoapi/items/collect/batarang.cpp): definitions in retail address order. */

extern "C" {
    i32 __cdecl Batman_PrepareJumpPc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks);
    void __cdecl ClearSpecialMove(GAMEOBJECT *);
}
typedef struct batman_prepare_pc_s {
    u32 capabilities;
    AI_PREPARING_SPECIAL_MOVE_PC prepare;
} BATMAN_PREPARE_PC;
STATIC_ASSERT(sizeof(BATMAN_PREPARE_PC) == 8);
extern "C" {
    extern BATMAN_PREPARE_PC batman_prepareforspecialmove[11];
    extern AI_MID_SPECIAL_MOVE_PC batman_midspecialmovefns[8];
    extern const char StarWarsTakeOverJumpPc[13];
}
STATIC_ASSERT(sizeof(AIPATHNODE_PC) == 0x5c);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, position) == 4);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, radius) == 0x10);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, radius_squared) == 0x14);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, runtime_flags) == 0x2b);
STATIC_ASSERT(offsetof(AIPATHNODE_PC, special_handle) == 0x40);
STATIC_ASSERT(offsetof(AIPACKET_PC, movement_destination) == 0x104);
STATIC_ASSERT(offsetof(AIPACKET_PC, movement_stopping_distance) == 0x110);
STATIC_ASSERT(offsetof(AIPACKET_PC, mover_height) == 0x120);
STATIC_ASSERT(offsetof(AIPACKET_PC, inside_path_node) == 0x124);
STATIC_ASSERT(offsetof(AIPACKET_PC, path_connection_state) == 0x139);
STATIC_ASSERT(offsetof(AIPACKET_PC, path_info) == 0x158);
STATIC_ASSERT(offsetof(AIPACKET_PC, flags) == 0x1e8);
STATIC_ASSERT(offsetof(AIPACKET_PC, capabilities) == 0x1f4);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_jump_destination_distance_pc) == 0xed0);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_reentry_timer) == 0xd28);
STATIC_ASSERT(offsetof(GAMEOBJECT, portal_flags) == 0xeec);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_collision_priority_pc) == 0x292);
typedef struct character_ai_capabilities_pc_s {
    u8 unknown_00[0x44];
    u32 path_capabilities;
} CHARACTER_AI_CAPABILITIES_PC;
typedef struct suit_ai_capabilities_pc_s {
    u8 unknown_00[0x10];
    u32 path_capabilities;
} SUIT_AI_CAPABILITIES_PC;
STATIC_ASSERT(sizeof(BATARANG_TARGET_s) == 0x14);
STATIC_ASSERT(offsetof(BATARANG_TARGET_s, lost) == 5);
STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, active) == 0x7c);
STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, flight_state) == 0x7d);
STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, owner) == 0x84);
STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, cooldown) == 0xac);
STATIC_ASSERT(sizeof(PLAYER_BATARANG_SLOT_PC) == 0xb4);
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
extern "C" {
    extern i32 LEGO_AIPATHCNX_VARIANT_FLAG0_PC, LEGO_AIPATHCNX_VARIANT_FLAG1_PC;
    extern u32 LEGO_AIPATHCNX_BIGJUMP, LEGO_AIPATHCNX_REQUIRESPERMISSION;
    extern u32 LEGO_AIPATHCNX_NO_DESTINATION_CHECK, LEGO_AIPATHCNX_JUMP_NOW, LEGO_AIPATHCNX_DONT_JUMP_NOW;
    void __cdecl InitFn_PreparingForSpecialMove(AI_PREPARING_SPECIAL_MOVE_PC);
    void __cdecl InitFn_MidSpecialMove(AI_MID_SPECIAL_MOVE_PC);
}

extern "C" {
    i32 __cdecl Batman_MidSpecialMove_BigJumpPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object);
    i32 __cdecl Batman_MidSpecialMove_ClimbDownPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object);
    i32 __cdecl Batman_MidSpecialMove_ClimbUpPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object);
    i32 __cdecl Batman_MidSpecialMove_DefaultPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object);
    i32 __cdecl Batman_MidSpecialMove_GlidePc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object);
    i32 __cdecl Batman_MidSpecialMove_GrapplePc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object);
    i32 __cdecl Batman_MidSpecialMove_SpecialUsePc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object);
    i32 __cdecl Batman_MidSpecialMove_StartJumpPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object);
    i32 __cdecl Batman_PrepareBigJumpPc(AIPACKET_PC *packet, APIOBJECT_PC *apiobject, i32 checks_unused);
    i32 __cdecl Batman_PrepareClimbPc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks);
    i32 __cdecl Batman_PrepareGlidePc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused);
    i32 __cdecl Batman_PrepareGrapplePc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused);
    i32 __cdecl Batman_PrepareHoverTubePc(AIPACKET_PC *packet, APIOBJECT_PC *apiobject, i32 checks_unused);
    i32 __cdecl Batman_PrepareJumpPc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks);
    i32 __cdecl Batman_PrepareObstaclePc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused);
    i32 __cdecl Batman_PrepareSpecialUsePc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused);
    i32 __cdecl Batman_PrepareTakeOverJumpPc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused);
}
extern "C" f32 __cdecl NuFdiv(f32 a, f32 b);
extern "C" void __cdecl Detonator_Detonate(DETONATOR_s *detonator);

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
extern "C" {

    static u32 Batarang_GetTargetPos(BATARANG_NATIVE_PC *batarang, int index, NUVEC *position) {
        if (batarang->flight_state.in_flight != 0 && batarang->active > 0 && index == batarang->active) {
            *position = batarang->owner->collision_position;
            if ((batarang->owner->flags_e24 & 8) != 0) {
                GAMEOBJECT *owner = batarang->owner;
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

extern "C" void Batarang_StartThrowQuick(GAMEOBJECT *object) {
    ResetAnimPacket(&object->anim_packet, -1);
    object->character_context = 0x5e;
    object->field_7aa = 0xb2;
    object->context_timer = AnimDuration((i16)object->id, 0xb2, 0.0f, 0.0f, 1);
}

void __cdecl Batarangs_Reset(void) {
    i32 i;
    for (i = 0; i < 8; ++i) {
        PLAYER_BATARANG_SLOT_PC *slot = &BatarangPacketsPc[i];
        slot->flight_state.in_flight = 0;
        slot->active = 0;
        slot->owner = NULL;
        slot->cooldown = 50;
    }
}

extern "C" i32 Batarang_InitRicochet(BATARANG_NATIVE_PC *batarang, NUVEC *normal) {
    if (batarang == NULL || normal == NULL) {
        return 0;
    }
    if (batarang->ricochet_state.count > 4) {
        batarang->ricochet_state.flags &= ~1u;
        --batarang->ricochet_state.count;
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
    i32 angle = (i16)(0x4000 - BatarangASinPc(cosine));
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

void Batarang_Ricochet(BATARANG_NATIVE_PC *batarang) {
    if (batarang == NULL || (batarang->ricochet_state.flags & 1) == 0) {
        return;
    }
    if (batarang->ricochet_state.timer < 0.2f) {
        batarang->ricochet_state.timer += FRAMETIME;
    } else {
        batarang->ricochet_state.flags &= ~1u;
        batarang->ricochet_normal = v000;
        batarang->ricochet_state.timer = 0.0f;
    }
}

static i32 Batarang_ShouldRicochet(BATARANG_NATIVE_PC *batarang) {
    NUVEC travel, destination;
    NuVecScale(&travel, &batarang->velocity, FRAMETIME * 2.0f);
    NuVecAdd(&destination, &batarang->position, &travel);
    if (NewRayCastScaleYMask(&batarang->position, &travel, 0.0f, 1.0f, 0, 0x1f)) {
        i32 platform = TerrainPlatId();
        if (batarang->flight_state.in_flight != 0 && batarang->flight_state.current_target >= batarang->active)
            return 1;
        BATARANG_TARGET_s *current = &batarang->targets[batarang->flight_state.current_target];
        if (current->type == 0 || current->type == 1)
            return 1;
        if (current->type == 2)
            return static_cast<GIZMOBLOWUP_PC *>(current->object)->platform_id != platform;
        if (current->type == 3)
            return 1;
    }
    return 0;
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

void __cdecl Batarangs_CheckLostData(void *data) {
    PLAYER_BATARANG_SLOT_PC *batarang = BatarangPacketsPc;
    i32 i, j, k;
    for (i = 0; i < 8; ++i, ++batarang) {
        if (batarang->owner == data) {
            batarang->flight_state.in_flight = 0;
            batarang->active = 0;
            batarang->owner = NULL;
            batarang->cooldown = 0x32;
        } else if (batarang->active > 0) {
            for (j = 0; j < batarang->active; ++j) {
                if (batarang->targets[j].object != data)
                    continue;
                if (data == batarang->owner) {
                    batarang->flight_state.in_flight = 0;
                    batarang->active = 0;
                    batarang->owner = NULL;
                    batarang->cooldown = 0x32;
                } else if (batarang->flight_state.in_flight != 0) {
                    batarang->targets[j].lost = 1;
                } else {
                    for (k = j; k < batarang->active - 1; ++k)
                        batarang->targets[k] = batarang->targets[k + 1];
                    --batarang->active;
                }
            }
        }
    }
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

extern "C" i32 Batarang_SeekToTarget(BATARANG_NATIVE_PC *batarang) {
    i32 reached = 0;
    NUVEC target;
    NUVEC normal;
    NUVEC desired;
    f32 rate;
    if (batarang == NULL) {
        return 0;
    }
    if (batarang->flight_state.current_target > batarang->active ||
        batarang->targets[batarang->flight_state.current_target].lost != 0) {
        target = batarang->owner->collision_position;
    } else if (!Batarang_GetTargetPos(batarang, batarang->flight_state.current_target, &target)) {
        return 0;
    }

    rate = 5.0f;
    if (batarang->flight_state.flight_time > 1.0f)
        rate = (batarang->flight_state.flight_time - 1.0f) * 5.0f + 5.0f;
    if (batarang->flight_state.flight_time < 4.0f) {
        if (Batarang_ShouldRicochet(batarang)) {
            NewRayCastGetImpactNormal(&normal);
            Batarang_InitRicochet(batarang, &normal);
        }
    }

    if ((batarang->ricochet_state.flags & 1) == 0) {
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
    } else {
        Batarang_Ricochet(batarang);
    }
    batarang->position.x += batarang->velocity.x * FRAMETIME;
    batarang->position.y += batarang->velocity.y * FRAMETIME;
    batarang->position.z += batarang->velocity.z * FRAMETIME;
    if (NuVecDistSqr(&batarang->position, &target, NULL) < 0.0625f)
        reached = 1;
    return reached;
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
extern "C" {

    i32 __cdecl Batman_MidSpecialMove_DefaultPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        u32 capabilities =
            packet->path_info.connection->traversal_flags[packet->path_info.direction] & packet->capabilities;
        AIPATHNODE_PC *destination =
            &packet->path_info.path
                 ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        AIPATHNODE_PC *source =
            &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
        AIPATHNODE_PC *node;
        AIPATH_PC *path;
        i32 skip_inside;
        i32 index;
        NuVecSub(&owner->special_move_look_position_pc, &destination->position, &source->position);
        NuVecNorm(&owner->special_move_look_position_pc, &owner->special_move_look_position_pc);
        NuVecAdd(&owner->special_move_look_position_pc, &owner->special_move_look_position_pc, &destination->position);
        packet->movement_look_target = &owner->special_move_look_position_pc;
        if (owner->character_context == 0x5b)
            owner->pad->buttons_pressed |= GAMEPAD_JUMP;
        else
            owner->flags_ef4 |= 0x8000;
        node = owner->ai_packet.special_move_node;
        if (node->has_special != 0 && (node->runtime_flags & 2) != 0) {
            path = owner->ai_packet.path_info.path;
            skip_inside = 1;
            if (path != NULL) {
                index = node - path->nodes;
                if ((path->updated_node_bits[index / 8] & (1u << (index % 8))) == 0)
                    AIPathNodeUpdatePos(system, path, node);
            }
            if (owner->field_281 != 0 && owner->supporting_platform_id != -1 && CurrentTerrain != NULL &&
                NuSpecialGetInstanceix(&owner->ai_packet.special_move_node->special_handle) ==
                    ((AI_SPECIAL_TERRAIN_VIEW_PC *)CurrentTerrain)
                        ->platforms[owner->supporting_platform_id]
                        .scene_object_index)
                skip_inside = 0;
        } else {
            skip_inside = 0;
        }
        if (((GAMEOBJECT *)object)->respawn_timer > 2.0f ||
            (skip_inside == 0 &&
             owner->ai_packet.inside_path_node ==
                 owner->ai_packet.special_move_node - owner->ai_packet.path_info.path->nodes &&
             (owner->field_281 != 0 || owner->velocity.y < 0.0f))) {
            ClearSpecialMove(owner);
        } else if (owner->character_context != 0 &&
                   (owner->position.y < owner->ai_packet.special_move_node->min_height ||
                    (packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x80000000) != 0)) {
            ClearSpecialMove(owner);
        }
        if (owner->ai_packet.special_move_node != NULL) {
            if ((capabilities & 0x40) == 0) {
                if ((owner->portal_flags & 0x1000) != 0 && owner->velocity.y < 0.0f) {
                    owner->pad->buttons_pressed |= GAMEPAD_JUMP;
                    owner->portal_flags &= ~0x1000;
                }
                if ((capabilities & 4) != 0)
                    owner->pad->buttons_held |= GAMEPAD_JUMP;
            }
            owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
            owner->ai_packet.movement_stopping_distance = 0.0f;
            ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
            return 1;
        }
        return 0;
    }
#pragma data_seg(".data$BatmanJumpDistancePc")
    f32 BatmanJumpDistancePc[3] = {0.8f, 1.2f, 10000.0f};
#pragma data_seg()

    i32 __cdecl Batman_PrepareJumpPc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
        i32 index;
        GAMEOBJECT *owner;
        u32 flags;
        i32 destination_index = 0;
        AIPATHNODE_PC *destination = NULL;
        NUVEC offset;
        AIPATHCNX_PC *connection;
        AIPATHNODE_PC *source = NULL;
        f32 distance;
        f32 radius;
        i32 ready = 0;
        f32 jump_distance;
        i32 skip_ahead;
        GAMEOBJECT *other;
        f32 height;
        f64 tolerance;
        f32 clearance;

        owner = (GAMEOBJECT *)object->owner;
        flags = packet->path_info.connection->traversal_flags[packet->path_info.direction] & packet->capabilities;
        if ((packet->path_info.flags & 1) != 0) {
            source =
                &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
            destination_index = packet->path_info.connection->node_indices[packet->path_info.direction == 0];
            destination = &packet->path_info.path->nodes[destination_index];
            distance = NuVecXZDistSqr(&object->position, &destination->position, &offset);
            if (!(distance < destination->radius_squared)) {
                if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x200) == 0 &&
                    packet->inside_path_node != destination_index &&
                    (packet->path_info.path->previous_inside_node_bits[destination_index / 8] &
                     (1 << (destination_index % 8))) != 0) {
                    if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                        return 1;
                    }
                    packet->path_connection_state = 1;
                } else if (packet->path_connection_state == 0) {
                    distance = packet->path_info.distance * packet->path_info.connection->horizontal_distance;
                    packet->path_connection_state = 1;
                    if (packet->path_info.direction != 0) {
                        if (source->radius > distance)
                            packet->path_connection_state = 0;
                    } else if (distance >
                               (f64)packet->path_info.connection->horizontal_distance - destination->radius) {
                        packet->path_connection_state = 0;
                    }
                }
                goto check;
            }
        }
        packet->path_connection_state = 0;
    check:
        if (packet->path_connection_state == 0) {
            return 0;
        }
        offset.x = destination->position.x - source->position.x;
        offset.y = 0.0f;
        offset.z = destination->position.z - source->position.z;
        radius = source->radius;
        if (radius < 0.0f) {
            radius = 0.0f;
        }
        NuVecScale(&offset, &offset, radius / packet->path_info.connection->horizontal_distance);
        NuVecAdd(&packet->movement_destination, &source->position, &offset);
        distance = NuVecXZDistSqr(&object->position, &packet->movement_destination, &offset);
        if (checks != 0 && (distance < (f64)ai_moveradius * ai_moveradius || owner->respawn_timer > jump_stuck_time ||
                            owner->character_context == 0x44)) {
            packet->movement_destination = object->position;
            ready = 1;
        }
        packet->movement_stopping_distance = 0.0f;
        if ((flags & 4) != 0)
            jump_distance = BatmanJumpDistancePc[2];
        else if ((flags & 2) != 0)
            jump_distance = BatmanJumpDistancePc[1];
        else if ((flags & 0x20) != 0)
            jump_distance = BatmanJumpDistancePc[1];
        else
            jump_distance = BatmanJumpDistancePc[0];
        if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x800) != 0) {
            packet->flags |= 0x800000;
            packet->path_connection_state = 1;
        }
        flags = packet->path_info.connection->traversal_flags[packet->path_info.direction];
        switch (packet->path_connection_state) {
            case 1:
                if (ready != 0) {
                    skip_ahead =
                        (packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x200) == 0 &&
                        packet->inside_path_node != destination_index &&
                        (packet->path_info.path->previous_inside_node_bits[destination_index / 8] &
                         (1 << (destination_index % 8))) != 0;
                    if (skip_ahead) {
                        if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                            return 1;
                        }
                    } else if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x800) ==
                               0) {
                        if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x400) != 0 ||
                            ((destination->runtime_flags | source->runtime_flags) & 2) == 0 ||
                            NuSpecialCompare(&source->special_handle, &destination->special_handle) != 0) {
                            packet->path_connection_state = 4;
                        } else {
                            packet->path_connection_state = 2;
                        }
                    }
                }
                break;
            case 2:
                if ((flags & 0x400) != 0) {
                    packet->path_connection_state = 4;
                } else {
                    distance = NuVecDist(&object->position, &destination->position, &offset);
                    if (((destination->runtime_flags | source->runtime_flags) & 2) == 0 ||
                        (f64)distance - destination->radius > jump_distance) {
                        packet->path_connection_state = 3;
                        if ((destination->runtime_flags & 2) != 0)
                            owner->ai_jump_destination_distance_pc = 0.0f;
                        else
                            owner->ai_jump_destination_distance_pc = distance;
                    }
                }
                break;
            case 3:
                if ((flags & 0x400) != 0) {
                    packet->path_connection_state = 4;
                } else {
                    distance = NuVecDist(&object->position, &destination->position, &offset);
                    if (((destination->runtime_flags | source->runtime_flags) & 2) == 0) {
                        owner->ai_jump_destination_distance_pc = 0.0f;
                    } else if (!((f64)distance - destination->radius < jump_distance)) {
                        break;
                    }
                    if (!(distance > owner->ai_jump_destination_distance_pc)) {
                        owner->ai_jump_destination_distance_pc = distance;
                    } else {
                        packet->path_connection_state = 4;
                        owner->ai_jump_destination_distance_pc = 0.0f;
                    }
                }
                break;
            case 4:
                if ((flags & 0x26) != 0) {
                    if (0.0f >= owner->jump_reentry_timer) {
                        packet->path_connection_state = 5;
                        owner->portal_flags |= 0x1000;
                    }
                    break;
                }
                packet->path_connection_state = 5;
                owner->portal_flags &= ~0x1000;
                // Single jumps immediately enter the launch state.
            case 5:
                if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                    return 1;
                }
                connection = packet->path_info.connection;
                if ((connection->traversal_flags[packet->path_info.direction] & 0x200) == 0) {
                    other = Obj;
                    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++other) {
                        if ((other->flags_1fc_word & 1) == 0 || other == owner ||
                            (other->flags_1fc_word & 0x1000) == 0 || other->dead != 0) {
                            continue;
                        }
                        if (other != player && other->ai_packet.path_info.connection == packet->path_info.connection &&
                            other->ai_packet.special_move_node == NULL &&
                            other->ai_packet.path_update_flag < packet->path_connection_state) {
                            continue;
                        }
                        height = destination->position.y;
                        tolerance = 0.5 * ai_moveradius;
                        if (owner->collision_min.y > height + tolerance) {
                            if (other->collision_min.y > height + tolerance) {
                                continue;
                            }
                        } else if (height - tolerance > owner->collision_min.y &&
                                   height - tolerance > other->collision_min.y) {
                            continue;
                        }
                        clearance = (f32)((f64)other->ai_packet.mover_height + other->ai_packet.mover_height +
                                          packet->mover_height);
                        offset.x = other->position.x - destination->position.x;
                        if (!(clearance > offset.x && offset.x > -clearance)) {
                            continue;
                        }
                        offset.z = other->position.z - destination->position.z;
                        if (!(clearance > offset.z) || !(other->collision_max.y >= height) ||
                            !((f64)height + owner->scaled_height >= other->collision_min.y)) {
                            continue;
                        }
                        distance = (f32)((f64)offset.x * offset.x + (f64)offset.z * offset.z);
                        if (!((f64)clearance * clearance > distance))
                            continue;
                        packet->path_connection_state = 1;
                        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
                        return 1;
                    }
                }
                SetSpecialMove(owner, destination, NULL, 1);
                owner->pad->buttons_pressed |= GAMEPAD_JUMP;
                break;
        }
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
        return 1;
    }

    i32 __cdecl Batman_PrepareSpecialUsePc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
        GAMEOBJECT *owner;
        AIPATHNODE_PC *destination;
        AIPATHCNX_PC *connection;
        AIPATHNODE_PC *source;

        owner = (GAMEOBJECT *)object->owner;
        if (owner->special_move_timer_pc <= 0.0f) {
            destination = &packet->path_info.path
                               ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
            if (owner->character_context == 0x0f) {
                packet->movement_destination = destination->position;
                packet->movement_stopping_distance = 0.0f;
                SetSpecialMove(owner, destination, NULL, 2);
                ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
                return 1;
            }
            if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                return 1;
            }
            connection = packet->path_info.connection;
            source = &packet->path_info.path->nodes[connection->node_indices[packet->path_info.direction]];
            packet->movement_destination = source->position;
            packet->movement_stopping_distance = 0.0f;
            owner->pad->buttons_held |= GAMEPAD_SPECIAL;
            owner->pad->buttons_pressed |= GAMEPAD_SPECIAL;
            GameObjectSetCanUse(owner, NULL, 3, 1, 0.0f);
            ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
            return 1;
        } else {
            packet->movement_destination = object->position;
            packet->movement_stopping_distance = 0.0f;
            ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
            return 1;
        }
    }

    i32 __cdecl Batman_MidSpecialMove_SpecialUsePc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        switch ((i8)owner->field_1087) {
            case 0:
                if (owner->character_context == 0x0f) {
                    owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                    owner->ai_packet.movement_stopping_distance = 0.0f;
                    ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
                    return 1;
                }
                owner->field_1087 = 1;
                owner->special_move_timer_pc = 1.0f;
            case 1:
                owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                owner->ai_packet.movement_stopping_distance = 0.0f;
                if (owner->ai_packet.inside_path_node ==
                        owner->ai_packet.special_move_node - owner->ai_packet.path_info.path->nodes ||
                    owner->special_move_timer_pc == 0.0f) {
                    ClearSpecialMove(owner);
                    return 0;
                }
                ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
                return 1;
        }
        return 0;
    }

    i32 __cdecl Batman_PrepareBigJumpPc(AIPACKET_PC *packet, APIOBJECT_PC *apiobject, i32 checks_unused) {
        GAMEOBJECT *owner = (GAMEOBJECT *)apiobject->owner;
        AIPATHCNX_PC *connection = packet->path_info.connection;
        AIPATHNODE_PC *destination =
            &packet->path_info.path->nodes[connection->node_indices[packet->path_info.direction == 0]];
        if ((connection->traversal_flags[packet->path_info.direction] & 0x800) != 0) {
            packet->movement_destination =
                packet->path_info.path->nodes[connection->node_indices[packet->path_info.direction]].position;
            packet->flags |= 0x800000;
            packet->movement_stopping_distance = 0.0f;
            ((GAMEOBJECT *)apiobject)->ai_collision_priority_pc |= 0x2000;
            return 1;
        }
        if (TryToTeleportToNextNode(owner, destination, 0) != 0)
            return 1;
        StartBigJump(owner, &destination->position, 0, 0.5f, 1.0f, 0);
        SetSpecialMove(owner, destination, NULL, 3);
        return 0;
    }

    i32 __cdecl Batman_MidSpecialMove_BigJumpPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        if (owner->character_context == 0x1f) {
            owner->launch_origin = owner->ai_packet.special_move_node->position;
            ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
            return 1;
        }
        ClearSpecialMove(owner);
        return 0;
    }

    i32 __cdecl Batman_PrepareGlidePc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
        AIPATHNODE_PC *destination =
            &packet->path_info.path
                 ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        if (TryToTeleportToNextNode(owner, destination, 1) != 0)
            return 1;
        if (owner->character_context != 0x44 && owner->special_move_timer_pc <= 0.0f) {
            SetSpecialMove(owner, destination, NULL, 4);
            ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
            return 1;
        }
        return 0;
    }

    i32 __cdecl Batman_MidSpecialMove_GlidePc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
        owner->ai_packet.movement_stopping_distance = 0.0f;
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
        owner->flags_ef4 |= 0x4000;
        if (owner->field_1087 == 0) {
            if (owner->character_context == 0) {
                owner->field_1087 = 1;
            } else {
                owner->pad->buttons_pressed |= GAMEPAD_JUMP;
                return 1;
            }
        }
        if (owner->character_context == 0x44 ||
            (owner->character_context != 0 &&
             (owner->field_281 != 0 || owner->position.y < owner->ai_packet.special_move_node->min_height))) {
            ClearSpecialMove(owner);
            return 0;
        }
        return 1;
    }

    i32 __cdecl Batman_PrepareClimbPc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        AIPATHNODE_PC *source, *destination;
        if (owner->indy_climb == NULL || owner->indy_climb->connection != packet->path_info.connection)
            owner->indy_climb = FindClimbObject(WORLD->climb_objects, packet->path_info.connection);
        if (owner->indy_climb != NULL) {
            destination = &packet->path_info.path
                               ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
            source =
                &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
            if (TryToTeleportToNextNode(owner, destination, 0))
                return 1;
            if (destination->position.y > source->position.y)
                SetSpecialMove(owner, destination, source, 5);
            else {
                if (owner->field_281 && owner->position.y < source->min_height)
                    return 0;
                SetSpecialMove(owner, destination, source, 6);
            }
        }
        return 1;
    }

    i32 __cdecl Batman_MidSpecialMove_ClimbUpPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        INDY_GUIDELINE_PC *guide;
        i32 endpoint;
        f32 distance, height;
        if (owner->indy_climb != NULL && owner->indy_climb->connection == packet->path_info.connection) {
            if (owner->upper_position.y > owner->ai_packet.special_move_node->max_height) {
                owner->pad->buttons_pressed |= GAMEPAD_JUMP;
                SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
                return 1;
            }
            if (owner->character_context == 0x43) {
                guide = GuideLine_FindNearest(&owner->upper_position, WORLD, &endpoint, &distance);
                if (guide != NULL && distance < 0.2f &&
                    !(owner->indy_guideline_state.endpoint == -1 && owner->indy_guideline_state.guideline == guide)) {
                    if (guide != owner->indy_guideline_state.guideline) {
                        owner->indy_guideline_state.endpoint = 0;
                        owner->indy_guideline_state.guideline = guide;
                    }
                    height = MIN(1.0f, guide->endpoints[owner->indy_guideline_state.endpoint].y - owner->position.y);
                    height = MAX(0.0f, height);
                    packet->movement_destination.x =
                        guide->endpoints[owner->indy_guideline_state.endpoint].x - owner->indy_climb->normal.x * height;
                    packet->movement_destination.y = guide->endpoints[owner->indy_guideline_state.endpoint].y;
                    packet->movement_destination.z =
                        guide->endpoints[owner->indy_guideline_state.endpoint].z - owner->indy_climb->normal.z * height;
                    owner->ai_packet.movement_stopping_distance = 0.0f;
                    if (distance < 0.05f) {
                        if (owner->indy_guideline_state.endpoint == 0)
                            owner->indy_guideline_state.endpoint = 1;
                        else if (owner->indy_guideline_state.endpoint == endpoint)
                            owner->indy_guideline_state.endpoint = -1;
                    }
                } else {
                    packet->movement_destination.x = owner->indy_climb->impact_x - owner->indy_climb->normal.x;
                    packet->movement_destination.y = owner->position.y;
                    packet->movement_destination.z = owner->indy_climb->impact_z - owner->indy_climb->normal.z;
                    owner->ai_packet.movement_stopping_distance = 0.0f;
                }
            } else {
                owner->ai_packet.movement_destination.x = owner->indy_climb->impact_x;
                owner->ai_packet.movement_destination.y = owner->ai_packet.special_move_node->position.y;
                owner->ai_packet.movement_destination.z = owner->indy_climb->impact_z;
                owner->ai_packet.movement_stopping_distance = 0.0f;
            }
            return 1;
        }
        ClearSpecialMove(owner);
        return 0;
    }

    i32 __cdecl Batman_MidSpecialMove_ClimbDownPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        INDY_GUIDELINE_PC *guide;
        i32 endpoint;
        f32 distance, height;
        if (owner->indy_climb != NULL && owner->indy_climb->connection == packet->path_info.connection) {
            if (owner->character_context == 0x43) {
                guide = GuideLine_FindNearest(&owner->upper_position, WORLD, &endpoint, &distance);
                if (guide != NULL && distance < 0.2f &&
                    !(owner->indy_guideline_state.endpoint == -1 && owner->indy_guideline_state.guideline == guide)) {
                    if (guide != owner->indy_guideline_state.guideline) {
                        owner->indy_guideline_state.endpoint = 1;
                        owner->indy_guideline_state.guideline = guide;
                    }
                    height = MIN(1.0f, owner->position.y - guide->endpoints[owner->indy_guideline_state.endpoint].y);
                    height = MAX(0.0f, height);
                    packet->movement_destination.x =
                        guide->endpoints[owner->indy_guideline_state.endpoint].x + owner->indy_climb->normal.x * height;
                    packet->movement_destination.y = guide->endpoints[owner->indy_guideline_state.endpoint].y;
                    packet->movement_destination.z =
                        guide->endpoints[owner->indy_guideline_state.endpoint].z + owner->indy_climb->normal.z * height;
                    owner->ai_packet.movement_stopping_distance = 0.0f;
                    if (distance < 0.05f) {
                        if (owner->indy_guideline_state.endpoint == 1)
                            owner->indy_guideline_state.endpoint = 0;
                        else if (owner->indy_guideline_state.endpoint == endpoint)
                            owner->indy_guideline_state.endpoint = -1;
                    }
                } else {
                    packet->movement_destination.x = owner->indy_climb->impact_x + owner->indy_climb->normal.x;
                    packet->movement_destination.y = owner->position.y;
                    packet->movement_destination.z = owner->indy_climb->impact_z + owner->indy_climb->normal.z;
                    packet->movement_stopping_distance = 0.0f;
                }
                return 1;
            }
            if (!owner->field_281 || !(owner->position.y < owner->special_move_source_pc->min_height)) {
                if (owner->field_281) {
                    owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                    owner->ai_packet.movement_stopping_distance = 0.0f;
                } else {
                    owner->ai_packet.movement_destination.x = owner->indy_climb->impact_x;
                    owner->ai_packet.movement_destination.y = owner->ai_packet.special_move_node->position.y;
                    owner->ai_packet.movement_destination.z = owner->indy_climb->impact_z;
                    owner->ai_packet.movement_stopping_distance = 0.0f;
                }
                return 1;
            }
        }
        ClearSpecialMove(owner);
        return 0;
    }

    i32 __cdecl Batman_PrepareGrapplePc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        AIPATHNODE_PC *source =
            &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
        AIPATHNODE_PC *destination =
            &packet->path_info.path
                 ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        if (TryToTeleportToNextNode(owner, destination, 0) != 0)
            return 1;
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
        if (owner->character_context == 0x46) {
            GameObjectSetCanUse(owner, owner->grapple, 1, 0x32, 0.0f);
            SetSpecialMove(owner, destination, source, 7);
            return 1;
        }
        packet->movement_destination = source->position;
        packet->movement_stopping_distance = 0.0f;
        if (owner->special_move_timer_pc <= 0.0f) {
            GameObjectSetCanUse(owner, owner->grapple, 1, 0x32, 0.0f);
            owner->pad->buttons_held |= GAMEPAD_SPECIAL;
            owner->pad->buttons_pressed |= GAMEPAD_SPECIAL;
        }
        return 0;
    }

    i32 __cdecl Batman_MidSpecialMove_GrapplePc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        if (owner->character_context == 0x46) {
            GameObjectSetCanUse(owner, owner->grapple, 1, 0x32, 0.0f);
            if (owner->ai_packet.special_move_node->position.y > owner->special_move_source_pc->position.y) {
                if (IndyGrappleCanJumpPc(owner) != 0) {
                    owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                    owner->ai_packet.movement_stopping_distance = 0.0f;
                    owner->pad->buttons_pressed |= GAMEPAD_JUMP;
                    SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
                    return 1;
                }
                owner->pad->buttons_held |= GAMEPAD_SPECIAL;
                owner->ai_packet.movement_destination = owner->position;
                owner->ai_packet.movement_stopping_distance = 0.0f;
                return 1;
            }
            if ((owner->flags_e24 & 0x80) != 0) {
                owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                owner->ai_packet.movement_stopping_distance = 0.0f;
                owner->pad->buttons_pressed |= GAMEPAD_JUMP;
                SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
                return 1;
            }
            owner->ai_packet.movement_destination = owner->position;
            owner->ai_packet.movement_stopping_distance = 0.0f;
            return 1;
        }
        ClearSpecialMove(owner);
        return 0;
    }

    i32 __cdecl Batman_MidSpecialMove_StartJumpPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        if (owner->character_context == 0) {
            SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
            owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
            owner->ai_packet.movement_stopping_distance = 0.0f;
        } else {
            owner->pad->buttons_pressed |= GAMEPAD_JUMP;
            ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
        }
        return 1;
    }

    i32 __cdecl Batman_PrepareObstaclePc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
        i32 near_index;
        i32 far_index;
        AIPATHNODE_PC *near_node;
        AIPATHNODE_PC *far_node;
        NUVEC offset;
        f32 radius;
        f32 distance;
        AIPATHCNX_PC *destination;
        AIPATHCNX_PC *current;
        f32 edge;

        far_index = !(packet->path_info.distance > 0.5f);
        near_index = far_index == 0;
        near_node = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[near_index]];
        far_node = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[far_index]];

        offset.x = far_node->position.x - near_node->position.x;
        offset.y = 0.0f;
        offset.z = far_node->position.z - near_node->position.z;
        radius = near_node->radius - packet->mover_height;
        if (radius < 0.0f) {
            radius = 0.0f;
        }
        NuVecScale(&offset, &offset, radius / packet->path_info.connection->horizontal_distance);
        NuVecAdd(&packet->movement_destination, &near_node->position, &offset);
        offset.x = packet->path_info.width;
        offset.y = 0.0f;
        offset.z = 0.0f;
        NuVecRotateY(&offset, &offset, packet->path_info.connection->rotation);
        NuVecAdd(&packet->movement_destination, &packet->movement_destination, &offset);
        packet->movement_stopping_distance = 0.0f;
        distance = NuVecXZDistSqr(&object->position, &packet->movement_destination, &offset);
        if (distance < ai_moveradius * ai_moveradius) {
            destination = packet->fallback_path_info.connection;
            current = packet->path_info.connection;
            if (destination == current) {
                edge = far_node->radius / destination->horizontal_distance;
                if (far_index != 0) {
                    if (1.0f - edge > packet->fallback_path_info.distance) {
                        goto reset_connection;
                    }
                } else if (edge < packet->fallback_path_info.distance) {
                    goto reset_connection;
                }
            }
            if (packet->path_info.direction == far_index) {
                return 0;
            }
            if (packet->path_connection_state == 0) {
                if (current->open == 0) {
                    packet->path_connection_state = 1;
                }
            } else if (packet->path_connection_state == 1 && current->open != 0) {
                SetSpecialMove((GAMEOBJECT *)object->owner, far_node, NULL, 0);
            }
        } else {
        reset_connection:
            packet->path_connection_state = 0;
        }
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
        return 1;
    }

    i32 __cdecl Batman_PrepareHoverTubePc(AIPACKET_PC *packet, APIOBJECT_PC *apiobject, i32 checks_unused) {
        GAMEOBJECT *object;
        AIPATHCNX_PC *connection;
        i32 from;
        i32 to;
        AIPATHNODE_PC *target;

        object = (GAMEOBJECT *)apiobject->owner;
        connection = packet->path_info.connection;
        from = connection->node_indices[packet->path_info.direction];
        to = connection->node_indices[packet->path_info.direction == 0];
        target = &packet->path_info.path->nodes[to];
        packet->movement_destination = packet->path_info.path->nodes[from].position;
        packet->movement_stopping_distance = 0.0f;
        if ((connection->traversal_flags[packet->path_info.direction] & 0x200) == 0 &&
            (packet->path_info.path->previous_inside_node_bits[to / 8] & (1 << (to % 8))) != 0) {
            if (TryToTeleportToNextNode(object, target, 0) != 0) {
                return 1;
            }
            packet->path_connection_state = 0;
        } else {
            if (packet->path_connection_state != 0 && TryToTeleportToNextNode(object, target, 0) != 0) {
                return 1;
            }
            switch (packet->path_connection_state) {
                case 0:
                    if (ObjInTube(object) == 0) {
                        break;
                    }
                    packet->path_connection_state = 1;
                    // Fall through.
                case 1:
                    if (ObjInTube(object) != 0 &&
                        !(object->collision_min.y > object->attached_tube_pc->top && object->velocity.y > 0.0f)) {
                        break;
                    }
                    packet->path_connection_state = 2;
                    // Fall through.
                case 2:
                    packet->path_connection_state = 0;
                    SetSpecialMove(object, target, NULL, 0);
                    packet->movement_destination = target->position;
                    packet->movement_stopping_distance = 0.0f;
                    break;
            }
        }
        ((GAMEOBJECT *)apiobject)->ai_collision_priority_pc |= 0x2000;
        return 1;
    }

    i32 __cdecl Batman_PrepareTakeOverJumpPc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
        GAMEOBJECT *owner;
        GAMEOBJECT *participant;
        AIPATHNODE_PC *destination;

        owner = (GAMEOBJECT *)object->owner;
        if ((owner->character_data->game_character->flags_090 & 0x40) != 0) {
            participant = owner->takeover_source;
            if (participant != NULL) {
                destination =
                    &packet->path_info.path
                         ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
                ReleaseTakeOver(participant, 1);
                StartBigJump(participant, &destination->position, 0, 0.5f, 1.0f, 0);
                SetSpecialMove(participant, destination, NULL, 3);
            }
            return 1;
        }
        return 0;
    }
#pragma data_seg(".data$batman_prepareforspecialmove")
    BATMAN_PREPARE_PC batman_prepareforspecialmove[11] = {
        {4, Batman_PrepareJumpPc},
        {0x23, Batman_PrepareJumpPc},
        {8, Batman_PrepareGrapplePc},
        {0x20000000, Batman_PrepareObstaclePc},
        {0x10, Batman_PrepareSpecialUsePc},
        {0x40, Batman_PrepareHoverTubePc},
        {0x1000, Batman_PrepareBigJumpPc},
        {0x8000, Batman_PrepareTakeOverJumpPc},
        {0xc0000, Batman_PrepareGlidePc},
        {0x300000, Batman_PrepareClimbPc},
        {0, NULL},
    };
#pragma data_seg()

    i32 __cdecl Batman_PreparingForSpecialMove(AIPACKET_PC *packet, APIOBJECT_PC *apiobject, i32 checks) {
        GAMEOBJECT *owner = (GAMEOBJECT *)apiobject->owner;
        u32 flags, capabilities;
        BATMAN_PREPARE_PC *entry;
        if (owner->character_context == 0x5b)
            owner->pad->buttons_pressed |= GAMEPAD_JUMP;
        flags = packet->path_info.connection->traversal_flags[packet->path_info.direction];
        if (flags != 0 && (flags & 0x40000000) == 0) {
            capabilities = flags & packet->capabilities;
            if ((flags & 0x8000) != 0 && (owner->character_data->game_character->flags_090 & 0x40) == 0 &&
                owner->takeover_target == NULL && owner->character_context != 0x3c &&
                (capabilities & 0xffff7fff) == 0) {
                AIScriptSetBaseScriptStateByName(&apiobject->ai->script_processor, (char *)StarWarsTakeOverJumpPc);
                return 1;
            }
            if (capabilities != 0 && (flags & 0x98000000) == 0) {
                for (entry = batman_prepareforspecialmove; entry->prepare != NULL; ++entry) {
                    if ((entry->capabilities & capabilities) != 0) {
                        if (entry->prepare(packet, apiobject, checks) != 0)
                            return 1;
                        break;
                    }
                }
            } else {
                packet->movement_destination = ((GAMEOBJECT *)apiobject)->position;
                packet->movement_stopping_distance = 0.0f;
                ((GAMEOBJECT *)apiobject)->ai_collision_priority_pc |= 0x2000;
                return 1;
            }
        }
        if ((packet->flags & 0x20000000) != 0 && (owner->tag_flags & 2) == 0 && drop_back_in_timer > 0.0f &&
            (packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0xd8000000) == 0)
            TryToTeleportToNextNode(
                owner,
                &packet->path_info.path
                     ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]],
                0);
        return 0;
    }
#pragma data_seg(".data$batman_midspecialmovefns")
    AI_MID_SPECIAL_MOVE_PC batman_midspecialmovefns[8] = {
        Batman_MidSpecialMove_DefaultPc,   Batman_MidSpecialMove_StartJumpPc, Batman_MidSpecialMove_SpecialUsePc,
        Batman_MidSpecialMove_BigJumpPc,   Batman_MidSpecialMove_GlidePc,     Batman_MidSpecialMove_ClimbUpPc,
        Batman_MidSpecialMove_ClimbDownPc, Batman_MidSpecialMove_GrapplePc,
    };
#pragma data_seg()

    i32 __cdecl Batman_MidSpecialMove(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *apiobject) {
        GAMEOBJECT *object = (GAMEOBJECT *)apiobject;
        GAMEOBJECT *owner = object->objptr;
        AI_MID_SPECIAL_MOVE_PC function;
        owner->special_move_timer_pc -= FRAMETIME;
        if (owner->ai_packet.special_move_node != NULL) {
            if (owner->field_1086 != 4 && TryToTeleportToNextNode(owner, owner->ai_packet.special_move_node, 0) != 0)
                ClearSpecialMove(owner);
            else {
                function = batman_midspecialmovefns[(i8)owner->field_1086];
                if (function != NULL)
                    return function(system, packet, apiobject);
            }
        }
        return 0;
    }

    void __cdecl Batman_GameAISysInit(void) {
        LEGO_AIPATHCNX_BLOCKAGE = 0x4000;
        LEGO_AIPATHCNX_WALLSHUFFLE = 0x400000;
        LEGO_AIPATHCNX_VARIANT_FLAG0_PC = 0x100000;
        LEGO_AIPATHCNX_VARIANT_FLAG1_PC = 0x200000;
        LEGO_AIPATHCNX_JUMP = 0x1;
        LEGO_AIPATHCNX_FORGOODIES = 0x80;
        LEGO_AIPATHCNX_FORBADDIES = 0x100;
        LEGO_AIPATHCNX_BIGJUMP = 0x1000;
        LEGO_AIPATHCNX_REQUIRESPERMISSION = 0x2000;
        LEGO_AIPATHCNX_NO_DESTINATION_CHECK = 0x200;
        LEGO_AIPATHCNX_JUMP_NOW = 0x400;
        LEGO_AIPATHCNX_DONT_JUMP_NOW = 0x800;
        InitFn_PreparingForSpecialMove(Batman_PreparingForSpecialMove);
        InitFn_MidSpecialMove(Batman_MidSpecialMove);
    }

    void __cdecl PlayerInitCapabilitiesDefaultPc(GAMEOBJECT *object) {
        CHARACTER_MODEL_LAYERS_PC *model = object->character_model;
        CHARACTERDATA_PC *character = object->character_data;
        object->ai_packet.capabilities = ((CHARACTER_AI_CAPABILITIES_PC *)character)->path_capabilities | 0x20000040;
        if (model->action_animations[0x88] != NULL && model->action_animations[0x89] != NULL) {
            if ((character->game_character->flags_090 & 0x80000) != 0)
                object->ai_packet.capabilities |= 0x40000;
            else
                object->ai_packet.capabilities |= 0x80000;
        }
        if (model->action_animations[0x8a] != NULL && model->action_animations[0x8b] != NULL &&
            model->action_animations[0x8c] != NULL)
            object->ai_packet.capabilities |= 0x400000;
        if (model->action_animations[0xcd] != NULL)
            object->ai_packet.capabilities |= 0x100000;
        if (model->action_animations[6] != NULL)
            object->ai_packet.capabilities |= 1;
        if ((character->game_character->flags_090 & 0x400000) != 0)
            object->ai_packet.capabilities |= 0x20;
        if ((character->game_character->flags_090 & 0x40000000) != 0)
            object->ai_packet.capabilities |= 2;
        if (object->suit != NULL)
            object->ai_packet.capabilities |= ((SUIT_AI_CAPABILITIES_PC *)object->suit)->path_capabilities;
        if ((object->flags_1f8 & 1) != 0)
            object->ai_packet.capabilities |= 0x100;
        else if ((object->flags_1f8 & 4) == 0)
            object->ai_packet.capabilities |= 0x80;
        object->ai_packet.capabilities |= 0x1000;
        if ((character->model_flags & 0x40000) != 0)
            object->ai_packet.capabilities |= 0x10;
        if ((character->model_flags & 0x100000) != 0)
            object->ai_packet.capabilities |= 8;
        if ((character->model_flags & 0x2000) != 0)
            object->ai_packet.capabilities |= 0x10000;
        if ((character->model_flags & 0x88) != 0 || (character->game_character->flags_090 & 0x40) != 0)
            object->ai_packet.capabilities |= 0x8000;
        if (object->field_280 != 0xff)
            object->ai_packet.capabilities |= 0x20000;
        if ((character->game_character->flags_090 & 0x4000000) != 0)
            object->ai_packet.capabilities |= 0x800000;
    }

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
}

extern "C" f32 __cdecl NuFdiv(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f)
        return 0.0f;
    f32 result = a / b;
    return result;
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
        f32 timer;
        if (detonator->active == 0)
            continue;
        detonator->timer += FRAMETIME;
        NuCameraTransformScreenClip(&detonator->field_0x18, &detonator->field_0x0c, 1, NULL);
        timer = detonator->timer;
        if (timer >= 10.7f) {
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
    static DETONATOR_s *DetonatorPlacementSlot(GAMEOBJECT *object) {
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
    DETONATOR_s *detonator = Detonator;
    DETONATOR_s *nearest = NULL;
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
    static DETONATOR_s *Detonator_FindOldest(void) {
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
            if (frame == NULL || !(*frame > 0.0f))
                return;
            if (*frame < marker)
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
