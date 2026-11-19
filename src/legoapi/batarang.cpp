#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aisys.h"
#include "globals.h"
#include "legoapi/actions/character/legoapi_tag.h"
#include "legoapi/actions/combat/hits.h"
#include "legoapi/actions/movement/climbing.h"
#include "legoapi/actions/movement/jumping.h"
#include "legoapi/ai/core/legoai.h"
#include "legoapi/ai/game/creature.h"
#include "legoapi/ai/game/starwars_gameai.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/action_info.h"
#include "legoapi/characters/motion/animlist.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmos/fx/guidelines.h"
#include "legoapi/gizmos/transport/grapples.h"
#include "legoapi/gizmos/transport/tubes.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/batarang.h"
#include "legoapi/items/collect/detonator.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/camera.h"
#include "legoapi/menus/core/gamemessages.h"
#include "legoapi/menus/screens/customise.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legogame/batman_gameai.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "gameapi/edtools/edui.h"
static i16 id_ROBIN = -1;
extern "C" {
    const char *TagAsciiDown = "\xC2\xA3";
}

typedef struct batman_prepare_s {      // size 0x8
    u32 capabilities;                  // 0x0
    AI_PREPARING_SPECIAL_MOVE prepare; // 0x4
} BATMAN_PREPARE;

extern "C" {
    extern BATMAN_PREPARE batman_prepareforspecialmove[11];
}

typedef struct suit_ai_capabilities_s {
    u8 reserved_00[0x10];
    u32 path_capabilities;
} SUIT_AI_CAPABILITIES;

typedef PLAYER_BATARANG_SLOT BATARANG_NATIVE;
extern "C" {
    extern i32 LEGO_AIPATHCNX_VARIANT_FLAG0;
    extern i32 LEGO_AIPATHCNX_VARIANT_FLAG1;
    extern u32 LEGO_AIPATHCNX_BIGJUMP;
    extern u32 LEGO_AIPATHCNX_REQUIRESPERMISSION;
    extern u32 LEGO_AIPATHCNX_NO_DESTINATION_CHECK;
    extern u32 LEGO_AIPATHCNX_JUMP_NOW;
    extern u32 LEGO_AIPATHCNX_DONT_JUMP_NOW;
}

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
extern "C" {
    static f32 BatarangFabs(f32 f) {
        f32 rv;

        *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
        return rv;
    }
}

static i32 BatarangASin(f32 sin) {
    f32 abs;
    f32 sqrt;
    f32 magnitude;
    f32 range_sign;
    f32 value_sign;
    f32 octant;

    abs = BatarangFabs(sin);
    sqrt = NuFsqrt(1.0f - sin * sin);

    magnitude = MIN(sqrt, abs);

    range_sign = MAX(MIN((abs - 0.70710677f) * 3.40282e+38f, 1.0f), -1.0f);

    value_sign = MIN(sin * 3.40282e+38f, 1.0f);
    value_sign = MAX(value_sign, -1.0f);

    octant = range_sign * value_sign + value_sign;

    return (NEG_3_OVER_40 * POW2(range_sign * value_sign * magnitude) * POW3(range_sign * value_sign * magnitude) +
            (octant * PI_OVER_4 - (range_sign * value_sign * magnitude) +
             NEG_1_OVER_6 * (range_sign * value_sign * magnitude) * POW2(range_sign * value_sign * magnitude)) +
            NEG_5_OVER_112 * POW3(range_sign * value_sign * magnitude) * POW4(range_sign * value_sign * magnitude) +
            NEG_35_OVER_1152 * POW4(range_sign * value_sign * magnitude) * POW5(range_sign * value_sign * magnitude)) *
           MAX_SHORT_OVER_PI;
}

// noinline: the original calls BatarangContainsTarget out of line from Batarang_FindTarget; VC8 would inline it
// there.
static __declspec(noinline) i32 BatarangContainsTarget(BATARANG_NATIVE *batarang, void *target) {
    i32 index = 0;
    for (; index < batarang->active; ++index) {
        if (batarang->targets[index].object == target) {
            break;
        }
    }
    return index < batarang->active;
}

static i32 BatarangAppendTarget(BATARANG_NATIVE *batarang, void *selected, i32 type) {
    if (selected == NULL) {
        return 0;
    }
    if (batarang->active >= 5) {
        if ((i32)((f64)GameTimer.time_elapsed / (f64)0.2f) == (i32)((f64)GameTimer.last_time_elapsed / (f64)0.2f)) {
            return 1;
        }
        for (i32 index = 0; index < batarang->active - 1; ++index) {
            batarang->targets[index] = batarang->targets[index + 1];
        }
        --batarang->active;
    }
    batarang->targets[batarang->active].object = selected;
    batarang->targets[batarang->active].type = (u8)type;
    batarang->targets[batarang->active].lost = 0;
    ++batarang->active;
    return 2;
}

extern "C" {
    DETONATOR_s Detonator[10] = {0};
}

extern "C" i32 Batarang_FindTarget(WORLDINFO *world, GAMEOBJECT *object, i32 automatic) {
    NUVEC delta;

    union {
        f32 value;
        u32 bits;
    } absolute, absolute_result;

    BATARANG_NATIVE *batarang = object->player.batarang;
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
        if ((candidate->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 || (candidate->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 ||
            candidate->dead != 0 || (U64_PAIR(&CInfo[candidate->player.character_context].flags) & 0x8000) != 0 ||
            candidate->model_draw_result == 0 || candidate == object || candidate->player_index != -1 ||
            candidate->player.character_context == CHARACTER_CONTEXT_BEEN_TAKEN_OVER ||
            candidate->player.character_context == CHARACTER_CONTEXT_EATEN ||
            candidate->player.character_context == CHARACTER_CONTEXT_GET_IN ||
            (object->flags_1f8 & (APIOBJECT_SIDE_NEUTRAL | APIOBJECT_SIDE_GOODIE_BADDIE)) != 0 ||
            ((object->flags_1f8 ^ candidate->flags_1f8) & 1) == 0) {
            continue;
        }
        if (BatarangContainsTarget(batarang, candidate)) {
            continue;
        }
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
            if (((absolute.value = (candidate->player.camera_screen_position.x - batarang->sight_position.x)),
                 (absolute_result.bits = absolute.bits & 0x7fffffff), absolute_result.value) < (f64)0.1f &&
                ((absolute.value = (candidate->player.camera_screen_position.y - batarang->sight_position.y)),
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
            if (((absolute.value = (detonator->screen_position.x - batarang->sight_position.x)),
                 (absolute_result.bits = absolute.bits & 0x7fffffff), absolute_result.value) < (f64)0.1f &&
                ((absolute.value = (detonator->screen_position.y - batarang->sight_position.y)),
                 (absolute_result.bits = absolute.bits & 0x7fffffff), absolute_result.value) < (f64)0.1f) {
                distance = NuVecDistSqr(&detonator->detonate_position, &object->collision_position, NULL);
                if (distance < selected_distance) {
                    selected_distance = distance;
                    selected = detonator;
                    selected_type = 1;
                    selected_position = detonator->detonate_position;
                    platform = -1;
                }
            }
        }
    }
    GIZMOBLOWUP *blowup = NULL;
    GIZMOBLOWUP *nearest_blowup = NULL;
    GIZMOBLOWUP *candidate_blowup = world->gizmo_blowups;
    nearest_distance = 16.0f;
    for (i = 0; i < world->gizmo_blowup_count; ++i, ++candidate_blowup) {
        if ((candidate_blowup->status_flags & BLOWUP_STATUS_BLOWN_UP) != 0 ||
            (candidate_blowup->status_flags & BLOWUP_STATUS_ACTIVE) == 0 || (candidate_blowup->status_flags & BLOWUP_STATUS_VISIBLE) == 0 ||
            (candidate_blowup->status_flags & BLOWUP_STATUS_DRAWN) == 0 ||
            (candidate_blowup->draw_flags & BLOWUP_OPTION_BATARANG_TARGET) == 0 ||
            ((candidate_blowup->draw_flags & BLOWUP_OPTION_SHADOW_MODE_ONLY) != 0 && ShadowMode == 0)) {
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
                NuVecDistSqr(&candidate_blowup->mid_position, &object->collision_position, &delta) < selected_distance) {
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
        if (NewRayCastScaleYMask(&origin, &ray, 0.0f, 1.0f, 0, 0x1f) == 0 || (platform != -1 && TerrainPlatId() == platform)) {
            if (selected_type != -1 && BatarangAppendTarget(batarang, selected, selected_type) == 2) {
                return 1;
            }
        }
    }
    return 0;
}

extern "C" void Batarang_Release(GAMEOBJECT *object, i32 mode) {
    object->player.batarang->owner = object;
    object->player.batarang->flight_state.in_flight = 1;
    object->player.batarang->flight_state.current_target = 0;
    object->player.batarang->flight_state.flight_time = 0.0f;
    object->player.batarang->velocity.x = (f32)(NU_SIN_LUT(object->movement_facing_angle) * 4.0);
    object->player.batarang->velocity.y = (f32)(object->velocity.y * 0.5);
    object->player.batarang->velocity.z = (f32)(4.0 * NU_COS_LUT(object->movement_facing_angle));
    NewRumble(object->player.pad->bound_pad, 0.5f, 0);
    object->player.batarang->position = object->collision_position;
    if (object->player.flags_e24 & APIOBJECT_LOCATORS_VALID) {
        i32 joint = object->character_data->game_character->throw_locator;
        if (joint != -1 && object->character_model->points_of_interest[joint] != NULL) {
            object->player.batarang->position = *reinterpret_cast<NUVEC *>(&object->player.locator_matrices[joint].m30);
        }
    }
    if (object->player.batarang->active == 0 && mode == 2) {
        Batarang_FindTarget(WORLD, object, 1);
        if (object->player.batarang->active == 0) {
            object->player.batarang->targets[object->player.batarang->active].fallback_position.x =
                (f32)(NU_SIN_LUT(object->movement_facing_angle) * 2.0 + object->collision_position.x);
            object->player.batarang->targets[object->player.batarang->active].fallback_position.y = object->collision_position.y;
            object->player.batarang->targets[object->player.batarang->active].fallback_position.z =
                (f32)(2.0 * NU_COS_LUT(object->movement_facing_angle) + object->collision_position.z);
            object->player.batarang->targets[object->player.batarang->active].object =
                &object->player.batarang->targets[object->player.batarang->active].fallback_position;
            object->player.batarang->targets[object->player.batarang->active].type = 3;
            object->player.batarang->targets[object->player.batarang->active].lost = 0;
            ++object->player.batarang->active;
        }
    }
}

extern "C" {

    static u32 Batarang_GetTargetPos(BATARANG_NATIVE *batarang, int index, NUVEC *position) {
        if (batarang->flight_state.in_flight != 0 && batarang->active > 0 && index == batarang->active) {
            *position = batarang->owner->collision_position;
            if ((batarang->owner->player.flags_e24 & APIOBJECT_LOCATORS_VALID) != 0) {
                GAMEOBJECT *owner = batarang->owner;
                i32 joint = owner->character_data->game_character->throw_locator;
                if (joint != -1 && owner->character_model->points_of_interest[joint] != NULL) {
                    *position = *reinterpret_cast<NUVEC *>(&owner->player.locator_matrices[joint].m30);
                }
            }
            return 1;
        }
        if (index >= batarang->active) {
            return 0;
        }
        BATARANG_TARGET_s *target = &batarang->targets[index];
        if (target->type == 0) {
            *position = static_cast<GAMEOBJECT *>(target->object)->collision_position;
        } else if (target->type == 1) {
            *position = static_cast<DETONATOR_s *>(target->object)->detonate_position;
        } else if (target->type == 2) {
            *position = static_cast<GIZMOBLOWUP *>(target->object)->mid_position;
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
    object->player.character_context = CHARACTER_CONTEXT_THROW_QUICK;
    object->player.context_animation = ACTION_THROW_QUICK;
    object->player.context_timer = AnimDuration((i16)object->id, 0xb2, 0.0f, 0.0f, 1);
}

extern "C" {
    PLAYER_BATARANG_SLOT BatarangPackets[8] = {0};
}

void Batarangs_Reset(void) {
    i32 i;
    for (i = 0; i < 8; ++i) {
        PLAYER_BATARANG_SLOT *slot = &BatarangPackets[i];
        slot->flight_state.in_flight = 0;
        slot->active = 0;
        slot->owner = NULL;
        slot->cooldown = 50;
    }
}

extern "C" i32 Batarang_InitRicochet(BATARANG_NATIVE *batarang, NUVEC *normal) {
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
    i32 angle = (i16)(0x4000 - BatarangASin(cosine));
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

void Batarang_Ricochet(BATARANG_NATIVE *batarang) {
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

static i32 Batarang_ShouldRicochet(BATARANG_NATIVE *batarang) {
    NUVEC travel;
    NUVEC destination;
    NuVecScale(&travel, &batarang->velocity, FRAMETIME * 2.0f);
    NuVecAdd(&destination, &batarang->position, &travel);
    if (NewRayCastScaleYMask(&batarang->position, &travel, 0.0f, 1.0f, 0, 0x1f)) {
        i32 platform = TerrainPlatId();
        if (batarang->flight_state.in_flight != 0 && batarang->flight_state.current_target >= batarang->active) {
            return 1;
        }
        BATARANG_TARGET_s *current = &batarang->targets[batarang->flight_state.current_target];
        if (current->type == 0 || current->type == 1) {
            return 1;
        }
        if (current->type == 2) {
            return static_cast<GIZMOBLOWUP *>(current->object)->platform_id != platform;
        }
        if (current->type == 3) {
            return 1;
        }
    }
    return 0;
}

extern "C" void Batarangs_Draw(void) {
    BATARANG_NATIVE *batarang = reinterpret_cast<BATARANG_NATIVE *>(BatarangPackets);
    for (i32 index = 0; index < 8; ++index, ++batarang) {
        if (batarang->flight_state.in_flight && WORLD->level_objects[batarang->cooldown].active) {
            NUMTX matrix;
            NuMtxSetTranslation(&matrix, &batarang->position);
            NuSpecialDrawAt(&WORLD->level_objects[batarang->cooldown].special, &matrix);
        }
    }
}

void Batarangs_CheckLostData(void *data) {
    PLAYER_BATARANG_SLOT *batarang = BatarangPackets;
    i32 i;
    i32 j;
    i32 k;
    for (i = 0; i < 8; ++i, ++batarang) {
        if (batarang->owner == data) {
            batarang->flight_state.in_flight = 0;
            batarang->active = 0;
            batarang->owner = NULL;
            batarang->cooldown = 0x32;
        } else if (batarang->active > 0) {
            for (j = 0; j < batarang->active; ++j) {
                if (batarang->targets[j].object != data) {
                    continue;
                }
                if (data == batarang->owner) {
                    batarang->flight_state.in_flight = 0;
                    batarang->active = 0;
                    batarang->owner = NULL;
                    batarang->cooldown = 0x32;
                } else if (batarang->flight_state.in_flight != 0) {
                    batarang->targets[j].lost = 1;
                } else {
                    for (k = j; k < batarang->active - 1; ++k) {
                        batarang->targets[k] = batarang->targets[k + 1];
                    }
                    --batarang->active;
                }
            }
        }
    }
}

i32 BatarangGetObjectFromCharId(i32 character) {
    return 0x32 + (character == id_ROBIN);
}

extern "C" void Batarang_GetSightInfo(i32 character, i32 *red, i32 *green, i32 *blue, char *text) {
    if (character == id_ROBIN) {
        *red = 255;
        *green = 0;
        *blue = 31;
        if (text != NULL) {
            NuStrCpy(text, "\xc2\xb1");
        }
    } else {
        *red = 255;
        *green = 223;
        *blue = 0;
        if (text != NULL) {
            NuStrCpy(text, "\xc2\xa7");
        }
    }
}

i32 Batarang_StartTargetting(GAMEOBJECT *object) {
    PLAYER_BATARANG_SLOT *packet;
    if ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_GOT_BATARANG) != 0) {
        packet = object->player.batarang;
        packet->cooldown = (object->id == (u16)ID_DEFAULTCHARACTER_B) ? 51 : 50;
        packet = object->player.batarang;
        if (WORLD->level_objects[packet->cooldown].active != 0) {
            object->player.context_variant_flags |= APIOBJECT_CONTEXT_INVULNERABLE;
            object->player.context_phase = 0;
            object->player.context_timer = 0.0f;
            object->player.landing_followup = 0;
            object->player.character_context = CHARACTER_CONTEXT_BATARANG;
            object->player.context_animation = ACTION_TARGET;
            packet->owner = object;
            object->player.batarang->active = 0;
            object->player.batarang->sight_position.x = object->player.camera_screen_position.x;
            object->player.batarang->sight_position.y = object->player.camera_screen_position.y;
            object->player.batarang->sight_position.z = 1.0f;
            object->player.batarang->sight_velocity.z = 0.0f;
            object->player.batarang->sight_velocity.y = 0.0f;
            object->player.batarang->sight_velocity.x = 0.0f;
            packet = object->player.batarang;
            KeepPointOnScreen(&packet->sight_position, &packet->sight_velocity);
            return 1;
        }
    }
    return 0;
}

extern "C" {
    u32 GAMEPAD_JUMP = PAD_BUTTON_CROSS;
    u32 GAMEPAD_ACTION = PAD_BUTTON_SQUARE;
    ADD_GAME_MESSAGE AddGameMsg_Default = {NULL, &v000, &v000, 1.0f, 1.0f, 255,  255,  255,  128,  0,  1.0f, 0.0f, 0, -1,
                                           NULL, 0,     0.0f,  0.0f, NULL, NULL, NULL, NULL, NULL, -1, 0,    255,  0};
}

extern "C" void Batarang_MoveCode(GAMEOBJECT *object) {
    BATARANG_NATIVE *batarang;
    if (object->player.character_context == CHARACTER_CONTEXT_CATCH) {
        object->player.flags_e20 |= 0x00400000;
        if (object->player.hold_timer >= 0.25f) {
            Batarang_StartTargetting(object);
            return;
        }
        if (AnimPlaying(&object->anim_packet, object->player.context_animation, 0, 0) == NULL) {
            return;
        }
        if (object->player.pad->input_magnitude > 0.0f) {
            object->player.character_context = CHARACTER_CONTEXT_NONE;
            return;
        }
        f32 timer = object->player.context_timer - FRAMETIME;
        object->player.context_timer = timer;
        if (!(timer <= 0.0f)) {
            return;
        }
        if ((object->player.pad->buttons_held & GAMEPAD_ACTION) != 0) {
            if (Batarang_StartTargetting(object) != 0) {
                return;
            }
            object->player.hold_timer = 0.0f;
        }
        object->player.character_context = CHARACTER_CONTEXT_NONE;
        return;
    }
    if (object->player.character_context == CHARACTER_CONTEXT_BATARANG) {
        if (object->player.context_phase == 0) {
            object->player.flags_e20 |= 0x00400000;
            object->player.context_variant_flags |= APIOBJECT_CONTEXT_INVULNERABLE;
            if (((object->player.pad->buttons_pressed & GAMEPAD_JUMP) != 0)) {
                StartJump(object, 0);
                return;
            }
            if ((object->player.pad->buttons_held & GAMEPAD_ACTION) == 0 && !(object->player.context_timer < 0.3f)) {
                if (object->player.batarang->active > 0) {
                    object->player.context_variant_flags &= ~APIOBJECT_CONTEXT_INVULNERABLE;
                    object->player.context_phase = 1;
                    object->player.context_animation = ACTION_THROW;
                    f32 duration = AnimDuration((i16)object->id, 0x65, 0.0f, 0.0f, 1);
                    object->player.context_duration = duration;
                    if (duration <= 0.0f) {
                        object->player.context_duration = 0.5f;
                    }
                    object->player.context_timer = 0.0f;
                    object->player.context_variant_flags &= ~APIOBJECT_CONTEXT_EVENT_DONE;
                    if (object->character_model->model_data_b[object->player.context_animation] == NULL ||
                        AnimListFrame(object->character_model, object->player.context_animation, 0) != 0.0f) {
                        object->player.context_variant_flags |= APIOBJECT_CONTEXT_EVENT_DONE;
                        object->player.batarang_release = 1;
                    } else {
                        object->player.context_variant_flags &= ~APIOBJECT_CONTEXT_EVENT_DONE;
                    }
                } else {
                    object->player.character_context = CHARACTER_CONTEXT_NONE;
                }
            } else {
                if ((object->character_model->model_data_b[object->player.context_animation] == NULL ||
                     AnimPlaying(&object->anim_packet, object->player.context_animation, 1, 0) != NULL)) {
                    GAMEPAD *pad = object->player.pad;
                    object->player.context_timer += FRAMETIME;
                    f32 x = pad->input_direction_z * 1.25f;
                    f32 y = 1.25f * pad->input_direction_x;
                    if (object->player.context_timer < 0.25f) {
                        const f32 ramp = object->player.context_timer * 4.0f;
                        x *= ramp;
                        y *= ramp;
                    }
                    object->player.batarang->sight_velocity.x = SeekValF(object->player.batarang->sight_velocity.x, x, 10.0f);
                    object->player.batarang->sight_velocity.y = SeekValF(object->player.batarang->sight_velocity.y, y, 10.0f);
                    object->player.batarang->sight_position.x += object->player.batarang->sight_velocity.x * FRAMETIME;
                    object->player.batarang->sight_position.y += object->player.batarang->sight_velocity.y * FRAMETIME;
                }
                batarang = object->player.batarang;
                KeepPointOnScreen(&batarang->sight_position, &batarang->sight_velocity);
                if (Batarang_FindTarget(WORLD, object, 0) != 0) {
                    NewBuzzFrames(object->player.pad->bound_pad, 1, 0);
                }

                {
                    i32 colour[3];
                    char text[32];
                    NUVEC position;
                    Batarang_GetSightInfo((i16)object->id, &colour[0], &colour[1], &colour[2], text);
                    ADD_GAME_MESSAGE message;
                    memcpy(&message, &AddGameMsg_Default, sizeof(message));
                    message.scale = 1.0f;
                    message.green = static_cast<u8>(colour[1]);
                    message.red = static_cast<u8>(colour[0]);
                    message.blue = static_cast<u8>(colour[2]);
                    message.position = &object->player.batarang->sight_position;
                    message.text = text;
                    message.alpha = static_cast<i32>(((f64)0.2f * game_pulse + (f64)0.8f) * 128.0);
                    message.flags = GAMEMSG_ONE_FRAME | GAMEMSG_KEEP;
                    AddGameMsg(&message);
                    message.text = TagAsciiDown;
                    message.flags = GAMEMSG_WORLD_POSITION | GAMEMSG_FOLLOW | GAMEMSG_WORLD_SCALE | GAMEMSG_ONE_FRAME;
                    message.scale = 3.0f;
                    message.slot_flags = 4;
                    message.alpha = static_cast<i32>(128.0 * ((f64)0.8f - (f64)0.2f * game_pulse));
                    for (i32 i = 0; i < object->player.batarang->active; ++i) {
                        if (Batarang_GetTargetPos((object->player.batarang), i, &position) != 0) {
                            message.position = &position;
                            AddGameMsg(&message);
                        }
                    }
                }
            }
        } else if (object->player.context_phase == 1) {
            f32 *frame = NULL;
            object->player.context_variant_flags &= ~APIOBJECT_CONTEXT_INVULNERABLE;
            if ((object->player.pad->buttons_pressed & GAMEPAD_JUMP) != 0) {
                object->player.landing_followup = 1;
            }
            u32 released = (object->player.context_variant_flags >> 6) & 1;
            if (released && object->player.landing_followup == 1 && 0.2f <= object->player.context_timer) {
                StartJump(object, 0);
                object->player.flags_e24 |= APIOBJECT_NO_JUMP;
                goto finish;
            }
            if (released && object->player.pad->input_magnitude > 0.0f && object->player.context_timer >= 0.3f) {
                object->player.character_context = CHARACTER_CONTEXT_NONE;
                goto finish;
            }
            if (object->character_model->model_data_b[object->player.context_animation] != NULL) {
                frame = AnimPlaying(&object->anim_packet, object->player.context_animation, 1, 0);
                if (frame == NULL) {
                    goto finish;
                }
            }
            f32 timer = object->player.context_timer + FRAMETIME;
            object->player.context_timer = timer;
            if (object->player.context_duration <= timer) {
                object->player.character_context = CHARACTER_CONTEXT_NONE;
                if ((object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) != 0) {
                    return;
                }
            } else {
                if ((object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) != 0) {
                    return;
                }
                if (object->character_model->model_data_b[object->player.context_animation] != NULL) {
                    f32 marker = AnimListFrame(object->character_model, object->player.context_animation, 2);
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
            object->player.context_variant_flags |= APIOBJECT_CONTEXT_EVENT_DONE;
            object->player.batarang_release = 1;
        }
    } else {
        PLAYERCHARACTERCONFIG *runtime = object->character_data->game_character;
        batarang = object->player.batarang;
        if (((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0 || (runtime->flags_090 & CHARACTER_RUNTIME_GOT_BATARANG) == 0 ||
             batarang == NULL || batarang->flight_state.in_flight != 0 || object->use_model_origin == 0 ||
             (object->player.flags_e24 & APIOBJECT_LOCATORS_VALID) == 0 || object->model_draw_result == 0 ||
             !(object->player.camera_screen_position.z > 0.0f) || !(object->player.hold_timer >= 0.25f) || object->ground_flags == 0 ||
             ObjLandReady(object) == 0)) {
            return;
        }
        object->player.flags_e20 |= 0x00400000;
        if (0.25f <= object->player.hold_timer - FRAMETIME) {
            return;
        }
        Batarang_StartTargetting(object);
        return;
    }
finish:
    if ((object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) == 0 && object->player.batarang_release == 0) {
        object->player.flags_e20 |= 0x00400000;
    }
}

extern "C" i32 Batarang_SeekToTarget(BATARANG_NATIVE *batarang) {
    i32 reached = 0;
    NUVEC target;
    NUVEC normal;
    NUVEC desired;
    f32 rate;
    if (batarang == NULL) {
        return 0;
    }
    if (batarang->flight_state.current_target > batarang->active || batarang->targets[batarang->flight_state.current_target].lost != 0) {
        target = batarang->owner->collision_position;
    } else if (!Batarang_GetTargetPos(batarang, batarang->flight_state.current_target, &target)) {
        return 0;
    }

    rate = 5.0f;
    if (batarang->flight_state.flight_time > 1.0f) {
        rate = (batarang->flight_state.flight_time - 1.0f) * 5.0f + 5.0f;
    }
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
    if (NuVecDistSqr(&batarang->position, &target, NULL) < 0.0625f) {
        reached = 1;
    }
    return reached;
}

f32 BatarangCatwomanStunTime = 2.0f;

extern "C" void Batarangs_Update(void) {
    BATARANG_NATIVE *batarang = reinterpret_cast<BATARANG_NATIVE *>(BatarangPackets);
    for (i32 index = 0; index < 8; ++index, ++batarang) {
        if (batarang->flight_state.in_flight == 0) {
            continue;
        }
        batarang->flight_state.flight_time += FRAMETIME;
        if (batarang->flight_state.flight_time >= 2.0f && batarang->flight_state.flight_time > 4.0f) {
            batarang->flight_state.flight_time = 4.0f;
        }
        while (batarang->targets[batarang->flight_state.current_target].lost != 0 &&
               batarang->flight_state.current_target < batarang->active) {
            ++batarang->flight_state.current_target;
        }
        if (!Batarang_SeekToTarget(batarang)) {
            continue;
        }
        if (batarang->flight_state.current_target >= batarang->active) {
            batarang->owner->player.hold_timer = 0;
            NewBuzzFrames(batarang->owner->player.pad->bound_pad, 1, 0);
            GAMEOBJECT *owner = batarang->owner;
            if (owner->player.pad->buttons_held & GAMEPAD_ACTION) {
                Batarang_StartTargetting(owner);
            } else if (owner->player.pad->input_magnitude == 0 && owner->player.character_context == CHARACTER_CONTEXT_NONE &&
                       owner->character_model->model_data_b[0x98] != NULL) {
                owner->player.character_context = CHARACTER_CONTEXT_CATCH;
                owner->player.context_animation = ACTION_CATCH;
                batarang->owner->player.context_timer =
                    AnimDuration((i16)batarang->owner->id, batarang->owner->player.context_animation, 0, 0, 1);
            }
            batarang->flight_state.in_flight = 0;
            batarang->active = 0;
            batarang->owner = NULL;
            batarang->cooldown = 50;
            continue;
        }
        BATARANG_TARGET_s target = batarang->targets[batarang->flight_state.current_target];
        if (target.type != 3) {
            NewRumble(batarang->owner->player.pad->bound_pad, 0.5f, 0);
            NewBuzzFrames(batarang->owner->player.pad->bound_pad, 1, 0);
            GameCam_HitJudder();
        }
        ++batarang->flight_state.current_target;
        batarang->flight_state.flight_time = 0;
        if (target.type == 0) {
            GAMEOBJECT *victim = static_cast<GAMEOBJECT *>(target.object);
            if (victim->id == (u16)PunchRangeUnitId && victim->player_index == -1) {
                victim->portal_flags |= APIOBJECT_BEEN_HIT;
                PlayerTakeHitReaction(victim, batarang->owner, BatarangCatwomanStunTime, 0);
            } else if (!(victim->portal_flags & APIOBJECT_BOSS)) {
                ObjHitObj(batarang->owner, victim, 1, 0, 0, 1);
            }
        } else if (target.type == 1) {
            Detonator_Detonate(static_cast<DETONATOR_s *>(target.object));
        } else if (target.type == 2) {
            GizmoBlowupBlowup(static_cast<GIZMOBLOWUP *>(target.object), 1, BLOWUP_HIT_BATARANG, 2, NULL, 1);
        }
    }
}

extern "C" {

    i32 Batman_MidSpecialMove_Default(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        u32 capabilities = packet->path_info.connection->traversal_flags[packet->path_info.direction] & packet->capabilities;
        AIPATHNODE *destination =
            &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        AIPATHNODE *source = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
        AIPATHNODE *node;
        AIPATH *path;
        i32 skip_inside;
        i32 index;
        NuVecSub(&owner->special_move_look_position, &destination->position, &source->position);
        NuVecNorm(&owner->special_move_look_position, &owner->special_move_look_position);
        NuVecAdd(&owner->special_move_look_position, &owner->special_move_look_position, &destination->position);
        packet->movement_look_target = &owner->special_move_look_position;
        if (owner->player.character_context == CHARACTER_CONTEXT_LEDGE) {
            owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
        } else {
            owner->flags_ef4 |= 0x8000;
        }
        node = owner->ai_packet.special_move_node;
        if (node->has_special != 0 && (node->runtime_flags & 2) != 0) {
            path = owner->ai_packet.path_info.path;
            skip_inside = 1;
            if (path != NULL) {
                index = node - path->nodes;
                if ((path->updated_node_bits[index / 8] & (1u << (index % 8))) == 0) {
                    AIPathNodeUpdatePos(system, path, node);
                }
            }
            if (owner->ground_flags != 0 && owner->supporting_platform_id != -1 && CurrentTerrain != NULL &&
                NuSpecialGetInstanceix(&owner->ai_packet.special_move_node->special_handle) ==
                    CurrentTerrain->platforms[owner->supporting_platform_id].scene_object_index) {
                skip_inside = 0;
            }
        } else {
            skip_inside = 0;
        }
        if (((GAMEOBJECT *)object)->respawn_timer > 2.0f ||
            (skip_inside == 0 &&
             owner->ai_packet.inside_path_node == owner->ai_packet.special_move_node - owner->ai_packet.path_info.path->nodes &&
             (owner->ground_flags != 0 || owner->velocity.y < 0.0f))) {
            ClearSpecialMove(owner);
        } else if (owner->player.character_context != CHARACTER_CONTEXT_JUMP &&
                   (owner->position.y < owner->ai_packet.special_move_node->min_height ||
                    (packet->path_info.connection->traversal_flags[packet->path_info.direction] & AIPATHCNX_TEMPORARY_BLOCK) != 0)) {
            ClearSpecialMove(owner);
        }
        if (owner->ai_packet.special_move_node != NULL) {
            if ((capabilities & 0x40) == 0) {
                if ((owner->portal_flags & APIOBJECT_AI_DOUBLE_JUMP) != 0 && owner->velocity.y < 0.0f) {
                    owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
                    owner->portal_flags &= ~APIOBJECT_AI_DOUBLE_JUMP;
                }
                if ((capabilities & 4) != 0) {
                    owner->player.pad->buttons_held |= GAMEPAD_JUMP;
                }
            }
            owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
            owner->ai_packet.movement_stopping_distance = 0.0f;
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
            return 1;
        }
        return 0;
    }

    f32 BatmanJumpDistance[3] = {0.8f, 1.2f, 10000.0f};

    i32 Batman_PrepareJump(AIPACKET *packet, APIOBJECT *object, i32 checks) {
        i32 index;
        GAMEOBJECT *owner;
        u32 flags;
        i32 destination_index = 0;
        AIPATHNODE *destination = NULL;
        NUVEC offset;
        AIPATHCNX *connection;
        AIPATHNODE *source = NULL;
        f32 distance;
        f32 radius;
        i32 ready = 0;
        f32 jump_distance;
        i32 skip_ahead;
        GAMEOBJECT *other;
        f32 height;
        f64 tolerance;
        f32 clearance;

        owner = object->owner;
        flags = packet->path_info.connection->traversal_flags[packet->path_info.direction] & packet->capabilities;
        if ((packet->path_info.flags & AIPATHINFO_FLAG_ON_PATH) != 0) {
            source = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
            destination_index = packet->path_info.connection->node_indices[packet->path_info.direction == 0];
            destination = &packet->path_info.path->nodes[destination_index];
            distance = NuVecXZDistSqr(&object->position, &destination->position, &offset);
            if (!(distance < destination->radius_squared)) {
                if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & BATMAN_AIPATHCNX_NO_DESTINATION_CHECK) ==
                        0 &&
                    packet->inside_path_node != destination_index &&
                    (packet->path_info.path->previous_inside_node_bits[destination_index / 8] & (1 << (destination_index % 8))) != 0) {
                    if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                        return 1;
                    }
                    packet->path_connection_state = AIJUMP_APPROACH;
                } else if (packet->path_connection_state == AIJUMP_NONE) {
                    distance = packet->path_info.distance * packet->path_info.connection->horizontal_distance;
                    packet->path_connection_state = AIJUMP_APPROACH;
                    if (packet->path_info.direction != 0) {
                        if (distance < source->radius) {
                            packet->path_connection_state = AIJUMP_NONE;
                        }
                    } else if (distance > (f64)packet->path_info.connection->horizontal_distance - destination->radius) {
                        packet->path_connection_state = AIJUMP_NONE;
                    }
                }
                goto check;
            }
        }
        packet->path_connection_state = AIJUMP_NONE;
    check:
        if (packet->path_connection_state != AIJUMP_NONE) {
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
                                owner->player.character_context == CHARACTER_CONTEXT_TIGHTROPE)) {
                packet->movement_destination = object->position;
                ready = 1;
            }
            packet->movement_stopping_distance = 0.0f;
            if ((flags & 4) != 0) {
                jump_distance = BatmanJumpDistance[2];
            } else if ((flags & 2) != 0) {
                jump_distance = BatmanJumpDistance[1];
            } else if ((flags & 0x20) != 0) {
                jump_distance = BatmanJumpDistance[1];
            } else {
                jump_distance = BatmanJumpDistance[0];
            }
            if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & BATMAN_AIPATHCNX_DONT_JUMP_NOW) != 0) {
                packet->flags |= 0x800000;
                packet->path_connection_state = AIJUMP_APPROACH;
            }
            switch (packet->path_connection_state) {
                case AIJUMP_APPROACH:
                    if (ready != 0) {
                        skip_ahead = (packet->path_info.connection->traversal_flags[packet->path_info.direction] &
                                      BATMAN_AIPATHCNX_NO_DESTINATION_CHECK) == 0 &&
                                     packet->inside_path_node != destination_index &&
                                     (packet->path_info.path->previous_inside_node_bits[destination_index / 8] &
                                      (1 << (destination_index % 8))) != 0;
                        if (skip_ahead) {
                            if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                                return 1;
                            }
                        } else if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] &
                                    BATMAN_AIPATHCNX_DONT_JUMP_NOW) == 0) {
                            if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & BATMAN_AIPATHCNX_JUMP_NOW) !=
                                    0 ||
                                ((destination->runtime_flags | source->runtime_flags) & 2) == 0 ||
                                NuSpecialCompare(&source->special_handle, &destination->special_handle) != 0) {
                                packet->path_connection_state = AIJUMP_TAKE_OFF;
                            } else {
                                packet->path_connection_state = AIJUMP_BACK_UP;
                            }
                        }
                    }
                    break;
                case AIJUMP_BACK_UP:
                    if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & BATMAN_AIPATHCNX_JUMP_NOW) != 0) {
                        packet->path_connection_state = AIJUMP_TAKE_OFF;
                    } else {
                        distance = NuVecDist(&object->position, &destination->position, &offset);
                        if (((destination->runtime_flags | source->runtime_flags) & 2) == 0 ||
                            (f64)distance - destination->radius > jump_distance) {
                            packet->path_connection_state = AIJUMP_RUN_UP;
                            if ((destination->runtime_flags & 2) != 0) {
                                owner->ai_jump_destination_distance = 0.0f;
                            } else {
                                owner->ai_jump_destination_distance = distance;
                            }
                        }
                    }
                    break;
                case AIJUMP_RUN_UP:
                    if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & BATMAN_AIPATHCNX_JUMP_NOW) != 0) {
                        packet->path_connection_state = AIJUMP_TAKE_OFF;
                    } else {
                        distance = NuVecDist(&object->position, &destination->position, &offset);
                        if (((destination->runtime_flags | source->runtime_flags) & 2) == 0) {
                            owner->ai_jump_destination_distance = 0.0f;
                        } else if (!((f64)distance - destination->radius < jump_distance)) {
                            break;
                        }
                        if (!(distance > owner->ai_jump_destination_distance)) {
                            owner->ai_jump_destination_distance = distance;
                        } else {
                            packet->path_connection_state = AIJUMP_TAKE_OFF;
                            owner->ai_jump_destination_distance = 0.0f;
                        }
                    }
                    break;
                case AIJUMP_TAKE_OFF:
                    if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x26) != 0) {
                        if (0.0f >= owner->player.jump_reentry_timer) {
                            packet->path_connection_state = AIJUMP_JUMP;
                            owner->portal_flags |= APIOBJECT_AI_DOUBLE_JUMP;
                        }
                        break;
                    }
                    packet->path_connection_state = AIJUMP_JUMP;
                    owner->portal_flags &= ~APIOBJECT_AI_DOUBLE_JUMP;
                    // Single jumps immediately enter the launch state.
                case AIJUMP_JUMP:
                    if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                        return 1;
                    }
                    connection = packet->path_info.connection;
                    if ((connection->traversal_flags[packet->path_info.direction] & BATMAN_AIPATHCNX_NO_DESTINATION_CHECK) == 0) {
                        other = Obj;
                        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++other) {
                            if ((other->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 || other == owner ||
                                (other->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 || other->dead != 0) {
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
                            } else if (height - tolerance > owner->collision_min.y && height - tolerance > other->collision_min.y) {
                                continue;
                            }
                            clearance = (f32)((f64)other->ai_packet.mover_height + other->ai_packet.mover_height + packet->mover_height);
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
                            if (!((f64)clearance * clearance > distance)) {
                                continue;
                            }
                            packet->path_connection_state = AIJUMP_APPROACH;
                            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
                            return 1;
                        }
                    }
                    SetSpecialMove(owner, destination, NULL, 1);
                    owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
                    break;
            }
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
            return 1;
        }
        return 0;
    }

    u32 GAMEPAD_SPECIAL = PAD_BUTTON_CIRCLE;

    i32 Batman_PrepareSpecialUse(AIPACKET *packet, APIOBJECT *object, i32 checks_unused) {
        GAMEOBJECT *owner;
        AIPATHNODE *destination;
        AIPATHCNX *connection;
        AIPATHNODE *source;

        owner = object->owner;
        if (owner->special_move_timer <= 0.0f) {
            destination = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
            if (owner->player.character_context == CHARACTER_CONTEXT_TELEPORT) {
                packet->movement_destination = destination->position;
                packet->movement_stopping_distance = 0.0f;
                SetSpecialMove(owner, destination, NULL, 2);
                ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
                return 1;
            }
            if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                return 1;
            }
            connection = packet->path_info.connection;
            source = &packet->path_info.path->nodes[connection->node_indices[packet->path_info.direction]];
            packet->movement_destination = source->position;
            packet->movement_stopping_distance = 0.0f;
            owner->player.pad->buttons_held |= GAMEPAD_SPECIAL;
            owner->player.pad->buttons_pressed |= GAMEPAD_SPECIAL;
            GameObjectSetCanUse(owner, NULL, 3, 1, 0.0f);
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
            return 1;
        } else {
            packet->movement_destination = object->position;
            packet->movement_stopping_distance = 0.0f;
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
            return 1;
        }
    }

    i32 Batman_MidSpecialMove_SpecialUse(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        switch ((i8)owner->helper_wait) {
            case 0:
                if (owner->player.character_context == CHARACTER_CONTEXT_TELEPORT) {
                    owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                    owner->ai_packet.movement_stopping_distance = 0.0f;
                    ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
                    return 1;
                }
                owner->helper_wait = 1;
                owner->special_move_timer = 1.0f;
            case 1:
                owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                owner->ai_packet.movement_stopping_distance = 0.0f;
                if (owner->ai_packet.inside_path_node == owner->ai_packet.special_move_node - owner->ai_packet.path_info.path->nodes ||
                    owner->special_move_timer == 0.0f) {
                    ClearSpecialMove(owner);
                    return 0;
                }
                ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
                return 1;
        }
        return 0;
    }

    i32 Batman_PrepareBigJump(AIPACKET *packet, APIOBJECT *apiobject, i32 checks_unused) {
        GAMEOBJECT *owner = apiobject->owner;
        AIPATHCNX *connection = packet->path_info.connection;
        AIPATHNODE *destination = &packet->path_info.path->nodes[connection->node_indices[packet->path_info.direction == 0]];
        if ((connection->traversal_flags[packet->path_info.direction] & BATMAN_AIPATHCNX_DONT_JUMP_NOW) != 0) {
            packet->movement_destination = packet->path_info.path->nodes[connection->node_indices[packet->path_info.direction]].position;
            packet->flags |= 0x800000;
            packet->movement_stopping_distance = 0.0f;
            ((GAMEOBJECT *)apiobject)->ai_collision_priority |= 0x2000;
            return 1;
        }
        if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
            return 1;
        }
        StartBigJump(owner, &destination->position, 0, 0.5f, 1.0f, 0);
        SetSpecialMove(owner, destination, NULL, 3);
        return 0;
    }

    i32 Batman_MidSpecialMove_BigJump(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        if (owner->player.character_context == CHARACTER_CONTEXT_BIG_JUMP) {
            owner->player.launch_origin = owner->ai_packet.special_move_node->position;
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
            return 1;
        }
        ClearSpecialMove(owner);
        return 0;
    }

    i32 Batman_PrepareGlide(AIPACKET *packet, APIOBJECT *object, i32 checks_unused) {
        AIPATHNODE *destination =
            &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        GAMEOBJECT *owner = object->owner;
        if (TryToTeleportToNextNode(owner, destination, 1) != 0) {
            return 1;
        }
        if (owner->player.character_context != CHARACTER_CONTEXT_TIGHTROPE && owner->special_move_timer <= 0.0f) {
            SetSpecialMove(owner, destination, NULL, 4);
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
            return 1;
        }
        return 0;
    }

    i32 Batman_MidSpecialMove_Glide(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
        owner->ai_packet.movement_stopping_distance = 0.0f;
        ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
        owner->flags_ef4 |= 0x4000;
        if (owner->helper_wait == 0) {
            if (owner->player.character_context == CHARACTER_CONTEXT_JUMP) {
                owner->helper_wait = 1;
            } else {
                owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
                return 1;
            }
        }
        if (owner->player.character_context == CHARACTER_CONTEXT_TIGHTROPE ||
            (owner->player.character_context != CHARACTER_CONTEXT_JUMP &&
             (owner->ground_flags != 0 || owner->position.y < owner->ai_packet.special_move_node->min_height))) {
            ClearSpecialMove(owner);
            return 0;
        }
        return 1;
    }

    i32 Batman_PrepareClimb(AIPACKET *packet, APIOBJECT *object, i32 checks) {
        GAMEOBJECT *owner = object->owner;
        AIPATHNODE *source;
        AIPATHNODE *destination;
        if (owner->indy_climb == NULL || owner->indy_climb->connection != packet->path_info.connection) {
            owner->indy_climb = FindClimbObject(WORLD->climb_objects, packet->path_info.connection);
        }
        if (owner->indy_climb != NULL) {
            destination = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
            source = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
            if (TryToTeleportToNextNode(owner, destination, 0)) {
                return 1;
            }
            if (destination->position.y > source->position.y) {
                SetSpecialMove(owner, destination, source, 5);
            } else {
                if (owner->ground_flags && owner->position.y < source->min_height) {
                    return 0;
                }
                SetSpecialMove(owner, destination, source, 6);
            }
        }
        return 1;
    }

    i32 Batman_MidSpecialMove_ClimbUp(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        GUIDELINE_RECORD *guide;
        i32 endpoint;
        f32 distance;
        f32 height;
        if (owner->indy_climb != NULL && owner->indy_climb->connection == packet->path_info.connection) {
            if (owner->upper_position.y > owner->ai_packet.special_move_node->max_height) {
                owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
                SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
                return 1;
            }
            if (owner->player.character_context == CHARACTER_CONTEXT_CLIMB) {
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
                        if (owner->indy_guideline_state.endpoint == 0) {
                            owner->indy_guideline_state.endpoint = 1;
                        } else if (owner->indy_guideline_state.endpoint == endpoint) {
                            owner->indy_guideline_state.endpoint = -1;
                        }
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

    i32 Batman_MidSpecialMove_ClimbDown(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        GUIDELINE_RECORD *guide;
        i32 endpoint;
        f32 distance;
        f32 height;
        if (owner->indy_climb != NULL && owner->indy_climb->connection == packet->path_info.connection) {
            if (owner->player.character_context == CHARACTER_CONTEXT_CLIMB) {
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
                        if (owner->indy_guideline_state.endpoint == 1) {
                            owner->indy_guideline_state.endpoint = 0;
                        } else if (owner->indy_guideline_state.endpoint == endpoint) {
                            owner->indy_guideline_state.endpoint = -1;
                        }
                    }
                } else {
                    packet->movement_destination.x = owner->indy_climb->impact_x + owner->indy_climb->normal.x;
                    packet->movement_destination.y = owner->position.y;
                    packet->movement_destination.z = owner->indy_climb->impact_z + owner->indy_climb->normal.z;
                    packet->movement_stopping_distance = 0.0f;
                }
                return 1;
            }
            if (!owner->ground_flags || !(owner->position.y < owner->special_move_source->min_height)) {
                if (owner->ground_flags) {
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

    i32 Batman_PrepareGrapple(AIPACKET *packet, APIOBJECT *object, i32 checks_unused) {
        GAMEOBJECT *owner = object->owner;
        AIPATHNODE *source = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
        AIPATHNODE *destination =
            &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
            return 1;
        }
        ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
        if (owner->player.character_context == CHARACTER_CONTEXT_GRAPPLE) {
            GameObjectSetCanUse(owner, owner->player.grapple, 1, 0x32, 0.0f);
            SetSpecialMove(owner, destination, source, 7);
            return 1;
        }
        packet->movement_destination = source->position;
        packet->movement_stopping_distance = 0.0f;
        if (owner->special_move_timer <= 0.0f) {
            GameObjectSetCanUse(owner, owner->player.grapple, 1, 0x32, 0.0f);
            owner->player.pad->buttons_held |= GAMEPAD_SPECIAL;
            owner->player.pad->buttons_pressed |= GAMEPAD_SPECIAL;
        }
        return 0;
    }

    i32 Batman_MidSpecialMove_Grapple(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        if (owner->player.character_context == CHARACTER_CONTEXT_GRAPPLE) {
            GameObjectSetCanUse(owner, owner->player.grapple, 1, 0x32, 0.0f);
            if (owner->ai_packet.special_move_node->position.y > owner->special_move_source->position.y) {
                if (IndyGrappleCanJump(owner) != 0) {
                    owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                    owner->ai_packet.movement_stopping_distance = 0.0f;
                    owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
                    SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
                    return 1;
                }
                owner->player.pad->buttons_held |= GAMEPAD_SPECIAL;
                owner->ai_packet.movement_destination = owner->position;
                owner->ai_packet.movement_stopping_distance = 0.0f;
                return 1;
            }
            if ((owner->player.flags_e24 & APIOBJECT_GRAPPLE_AT_MAX_LENGTH) != 0) {
                owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                owner->ai_packet.movement_stopping_distance = 0.0f;
                owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
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

    i32 Batman_MidSpecialMove_StartJump(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        if (owner->player.character_context == CHARACTER_CONTEXT_JUMP) {
            SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
            owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
            owner->ai_packet.movement_stopping_distance = 0.0f;
        } else {
            owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
        }
        return 1;
    }

    i32 Batman_PrepareObstacle(AIPACKET *packet, APIOBJECT *object, i32 checks_unused) {
        i32 near_index;
        i32 far_index;
        AIPATHNODE *near_node;
        AIPATHNODE *far_node;
        NUVEC offset;
        f32 radius;
        f32 distance;
        AIPATHCNX *current;
        f32 edge;

        far_index = packet->path_info.distance > 0.5f ? 0 : 1;
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
            current = packet->path_info.connection;
            if (packet->fallback_path_info.connection == current) {
                edge = far_node->radius / current->horizontal_distance;
                if (far_index != 0) {
                    if (packet->fallback_path_info.distance < 1.0f - edge) {
                        goto reset_connection;
                    }
                } else if (packet->fallback_path_info.distance > edge) {
                    goto reset_connection;
                }
            }
            if (packet->path_info.direction == far_index) {
                return 0;
            }
            switch (packet->path_connection_state) {
                case 0:
                    if (current->open == 0) {
                        packet->path_connection_state = 1;
                    }
                    break;
                case 1:
                    if (current->open != 0) {
                        SetSpecialMove(object->owner, far_node, NULL, 0);
                    }
                    break;
            }
        } else {
        reset_connection:
            packet->path_connection_state = 0;
        }
        ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
        return 1;
    }

    i32 Batman_PrepareHoverTube(AIPACKET *packet, APIOBJECT *apiobject, i32 checks_unused) {
        GAMEOBJECT *object;
        AIPATHCNX *connection;
        i32 from;
        i32 to;
        AIPATHNODE *target;

        object = apiobject->owner;
        connection = packet->path_info.connection;
        from = connection->node_indices[packet->path_info.direction];
        to = connection->node_indices[packet->path_info.direction == 0];
        target = &packet->path_info.path->nodes[to];
        packet->movement_destination = packet->path_info.path->nodes[from].position;
        packet->movement_stopping_distance = 0.0f;
        if ((connection->traversal_flags[packet->path_info.direction] & BATMAN_AIPATHCNX_NO_DESTINATION_CHECK) == 0 &&
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
                        !(object->collision_min.y > object->player.attached_tube->top && object->velocity.y > 0.0f)) {
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
        ((GAMEOBJECT *)apiobject)->ai_collision_priority |= 0x2000;
        return 1;
    }

    i32 Batman_PrepareTakeOverJump(AIPACKET *packet, APIOBJECT *object, i32 checks_unused) {
        GAMEOBJECT *owner;
        GAMEOBJECT *participant;
        AIPATHNODE *destination;

        owner = object->owner;
        if ((owner->character_data->game_character->flags_090 & CHARACTER_RUNTIME_CAN_TAKE_OVER) != 0) {
            participant = owner->player.takeover_source;
            if (participant != NULL) {
                destination = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
                ReleaseTakeOver(participant, 1);
                StartBigJump(participant, &destination->position, 0, 0.5f, 1.0f, 0);
                SetSpecialMove(participant, destination, NULL, 3);
            }
            return 1;
        }
        return 0;
    }

    BATMAN_PREPARE batman_prepareforspecialmove[11] = {
        {4, Batman_PrepareJump},
        {0x23, Batman_PrepareJump},
        {8, Batman_PrepareGrapple},
        {0x20000000, Batman_PrepareObstacle},
        {0x10, Batman_PrepareSpecialUse},
        {0x40, Batman_PrepareHoverTube},
        {0x1000, Batman_PrepareBigJump},
        {0x8000, Batman_PrepareTakeOverJump},
        {0xc0000, Batman_PrepareGlide},
        {0x300000, Batman_PrepareClimb},
        {0, NULL},
    };
    i16 ID_DEFAULTCHARACTER_B = -1;

    i32 Batman_PreparingForSpecialMove(AIPACKET *packet, APIOBJECT *apiobject, i32 checks) {
        GAMEOBJECT *owner = apiobject->owner;
        u32 capabilities;
        BATMAN_PREPARE *entry;

        if (owner->player.character_context == CHARACTER_CONTEXT_LEDGE) {
            owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
        }
        if (packet->path_info.connection->traversal_flags[packet->path_info.direction] != 0 &&
            (packet->path_info.connection->traversal_flags[packet->path_info.direction] & AIPATHCNX_PERMANENT_BLOCK) == 0) {
            capabilities = packet->path_info.connection->traversal_flags[packet->path_info.direction] & packet->capabilities;
            if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x8000) != 0 &&
                (owner->character_data->game_character->flags_090 & CHARACTER_RUNTIME_CAN_TAKE_OVER) == 0 &&
                owner->takeover_target == NULL && owner->player.character_context != CHARACTER_CONTEXT_GET_IN &&
                (capabilities & 0xffff7fff) == 0) {
                AIScriptSetBaseScriptStateByName(&apiobject->ai->script_processor, "TakeOverJump");
                return 1;
            }
            if (capabilities != 0 && (packet->path_info.connection->traversal_flags[packet->path_info.direction] &
                                      (AIPATHCNX_TOO_LONG | AIPATHCNX_HIDDEN | AIPATHCNX_TEMPORARY_BLOCK)) == 0) {
                for (entry = batman_prepareforspecialmove; entry->prepare != NULL; ++entry) {
                    if ((entry->capabilities & capabilities) != 0) {
                        if (entry->prepare(packet, apiobject, checks) != 0) {
                            return 1;
                        }
                        break;
                    }
                }
            } else {
                packet->movement_destination = ((GAMEOBJECT *)apiobject)->position;
                packet->movement_stopping_distance = 0.0f;
                ((GAMEOBJECT *)apiobject)->ai_collision_priority |= 0x2000;
                return 1;
            }
        }
        if ((packet->flags & AIPACKET_SPECIAL_MOVE_AHEAD) != 0 && (owner->player.tag_flags & APIOBJECT_TAG_DISABLED) == 0 &&
            drop_back_in_timer > 0.0f &&
            (packet->path_info.connection->traversal_flags[packet->path_info.direction] &
             (AIPATHCNX_TOO_LONG | AIPATHCNX_HIDDEN | AIPATHCNX_PERMANENT_BLOCK | AIPATHCNX_TEMPORARY_BLOCK)) == 0) {
            TryToTeleportToNextNode(
                owner, &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]], 0);
        }
        return 0;
    }

    AI_MID_SPECIAL_MOVE batman_midspecialmovefns[8] = {
        Batman_MidSpecialMove_Default, Batman_MidSpecialMove_StartJump, Batman_MidSpecialMove_SpecialUse, Batman_MidSpecialMove_BigJump,
        Batman_MidSpecialMove_Glide,   Batman_MidSpecialMove_ClimbUp,   Batman_MidSpecialMove_ClimbDown,  Batman_MidSpecialMove_Grapple,
    };

    i32 Batman_MidSpecialMove(AISYS *system, AIPACKET *packet, APIOBJECT *apiobject) {
        GAMEOBJECT *object = (GAMEOBJECT *)apiobject;
        GAMEOBJECT *owner = object->objptr;
        AI_MID_SPECIAL_MOVE function;
        owner->special_move_timer -= FRAMETIME;
        if (owner->ai_packet.special_move_node != NULL) {
            if (owner->helper_type != 4 && TryToTeleportToNextNode(owner, owner->ai_packet.special_move_node, 0) != 0) {
                ClearSpecialMove(owner);
            } else {
                function = batman_midspecialmovefns[(i8)owner->helper_type];
                if (function != NULL) {
                    return function(system, packet, apiobject);
                }
            }
        }
        return 0;
    }

    i32 LEGO_AIPATHCNX_BLOCKAGE = 0;
    i32 LEGO_AIPATHCNX_WALLSHUFFLE = 0;
    u32 LEGO_AIPATHCNX_FORGOODIES = 0;
    u32 LEGO_AIPATHCNX_FORBADDIES = 0;

    void Batman_GameAISysInit(void) {
        LEGO_AIPATHCNX_BLOCKAGE = BATMAN_AIPATHCNX_BLOCKAGE;
        LEGO_AIPATHCNX_WALLSHUFFLE = BATMAN_AIPATHCNX_WALLSHUFFLE;
        LEGO_AIPATHCNX_VARIANT_FLAG0 = BATMAN_AIPATHCNX_VARIANT_FLAG0;
        LEGO_AIPATHCNX_VARIANT_FLAG1 = BATMAN_AIPATHCNX_VARIANT_FLAG1;
        LEGO_AIPATHCNX_JUMP = BATMAN_AIPATHCNX_JUMP;
        LEGO_AIPATHCNX_FORGOODIES = BATMAN_AIPATHCNX_FORGOODIES;
        LEGO_AIPATHCNX_FORBADDIES = BATMAN_AIPATHCNX_FORBADDIES;
        LEGO_AIPATHCNX_BIGJUMP = BATMAN_AIPATHCNX_BIGJUMP;
        LEGO_AIPATHCNX_REQUIRESPERMISSION = BATMAN_AIPATHCNX_REQUIRESPERMISSION;
        LEGO_AIPATHCNX_NO_DESTINATION_CHECK = BATMAN_AIPATHCNX_NO_DESTINATION_CHECK;
        LEGO_AIPATHCNX_JUMP_NOW = BATMAN_AIPATHCNX_JUMP_NOW;
        LEGO_AIPATHCNX_DONT_JUMP_NOW = BATMAN_AIPATHCNX_DONT_JUMP_NOW;
        InitFn_PreparingForSpecialMove(Batman_PreparingForSpecialMove);
        InitFn_MidSpecialMove(Batman_MidSpecialMove);
    }

    void PlayerInitCapabilitiesDefault(GAMEOBJECT *object) {
        CHARACTER_MODEL_LAYERS *model = object->character_model;
        CHARACTERDATA *character = object->character_data;
        object->ai_packet.capabilities = ((CHARACTER_AI_CAPABILITIES *)character)->path_capabilities | 0x20000040;
        if (model->action_animations[0x88] != NULL && model->action_animations[0x89] != NULL) {
            if ((character->game_character->flags_090 & CHARACTER_RUNTIME_TIGHTROPE_WALK) != 0) {
                object->ai_packet.capabilities |= 0x40000;
            } else {
                object->ai_packet.capabilities |= 0x80000;
            }
        }
        if (model->action_animations[0x8a] != NULL && model->action_animations[0x8b] != NULL && model->action_animations[0x8c] != NULL) {
            object->ai_packet.capabilities |= BATMAN_AIPATHCNX_WALLSHUFFLE;
        }
        if (model->action_animations[0xcd] != NULL) {
            object->ai_packet.capabilities |= BATMAN_AIPATHCNX_VARIANT_FLAG0;
        }
        if (model->action_animations[6] != NULL) {
            object->ai_packet.capabilities |= BATMAN_AIPATHCNX_JUMP;
        }
        if ((character->game_character->flags_090 & CHARACTER_RUNTIME_HIGH_JUMP) != 0) {
            object->ai_packet.capabilities |= 0x20;
        }
        if ((character->game_character->flags_090 & CHARACTER_RUNTIME_COMBAT_ROLL) != 0) {
            object->ai_packet.capabilities |= 2;
        }
        if (object->player.suit != NULL) {
            object->ai_packet.capabilities |= ((SUIT_AI_CAPABILITIES *)object->player.suit)->path_capabilities;
        }
        if ((object->flags_1f8 & APIOBJECT_SIDE_BADDIE) != 0) {
            object->ai_packet.capabilities |= BATMAN_AIPATHCNX_FORBADDIES;
        } else if ((object->flags_1f8 & APIOBJECT_SIDE_NEUTRAL) == 0) {
            object->ai_packet.capabilities |= BATMAN_AIPATHCNX_FORGOODIES;
        }
        object->ai_packet.capabilities |= BATMAN_AIPATHCNX_BIGJUMP;
        if ((character->model_flags & CHARACTER_MODEL_TELEPORT) != 0) {
            object->ai_packet.capabilities |= 0x10;
        }
        if ((character->model_flags & CHARACTER_MODEL_ZIPUP) != 0) {
            object->ai_packet.capabilities |= 8;
        }
        if ((character->model_flags & CHARACTER_MODEL_VEHICLE) != 0) {
            object->ai_packet.capabilities |= 0x10000;
        }
        if ((character->model_flags & (CHARACTER_MODEL_JEDI | CHARACTER_MODEL_BLASTER)) != 0 ||
            (character->game_character->flags_090 & CHARACTER_RUNTIME_CAN_TAKE_OVER) != 0) {
            object->ai_packet.capabilities |= 0x8000;
        }
        if (object->player_index != -1) {
            object->ai_packet.capabilities |= 0x20000;
        }
        if ((character->game_character->flags_090 & CHARACTER_RUNTIME_HAZARD_PROTECTION) != 0) {
            object->ai_packet.capabilities |= 0x800000;
        }
    }

    u32 AIPathParseBatmanFlag(char *name) {
        if (NuStrICmp(name, "JUMP") == 0) {
            return 1;
        }
        if (NuStrICmp(name, "COMBATROLL") == 0) {
            return 2;
        }
        if (NuStrICmp(name, "BATGLIDE") == 0) {
            return 4;
        }
        if (NuStrICmp(name, "GRAPPLE") == 0) {
            return 8;
        }
        if (NuStrICmp(name, "USEHATCH") == 0) {
            return 0x10;
        }
        if (NuStrICmp(name, "HOVERTUBE") == 0) {
            return 0x40;
        }
        if (NuStrICmp(name, "FORGOODIES") == 0) {
            return 0x80;
        }
        if (NuStrICmp(name, "FORBADDIES") == 0) {
            return 0x100;
        }
        if (NuStrICmp(name, "OBSTACLE") == 0) {
            return 0x20000000;
        }
        if (NuStrICmp(name, "JUMP_NOW") == 0) {
            return 0x400;
        }
        if (NuStrICmp(name, "DONT_JUMP_NOW") == 0) {
            return 0x800;
        }
        return 0;
    }
}

extern "C" f32 NuFdiv(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f) {
        return 0.0f;
    }
    f32 result = a / b;
    return result;
}
