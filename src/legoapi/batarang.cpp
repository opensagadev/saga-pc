#include "legoapi/props/objects/legoapi_signal.h"
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
#include "gameapi/ai/aisys/gameaiscript.h"
#include "legoapi/gizmo/object/gizbuildit.h"
#include "legoapi/world/levels/episodeV.h"
static i16 id_ROBIN = -1;
extern "C" {
    const char *TagAsciiDown = "\xC2\xA3";
}

typedef PLAYER_BATARANG_SLOT BATARANG_NATIVE;

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
extern "C" {
    static f32 BatarangFabs(f32 f) {
        f32 rv;

        *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
        return rv;
    }
}

static i16 BatarangASin(f32 sin) {
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

static i16 BatarangACos(f32 cos) {
    return 0x4000 - BatarangASin(cos);
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
            candidate->dead != 0 || (CInfo[candidate->player.character_context].flags & 0x8000) != 0 || candidate->model_draw_result == 0 ||
            candidate == object || candidate->player_index != -1 ||
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
            object->player.batarang->position = object->player.locator_matrices[joint].translation;
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
                    *position = owner->player.locator_matrices[joint].translation;
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

/* An idle slot: nothing in flight, no owner, the throw cooldown restarted. */
static void BatarangSlotReset(PLAYER_BATARANG_SLOT *slot) {
    slot->flight_state.in_flight = 0;
    slot->active = 0;
    slot->owner = NULL;
    slot->cooldown = 50;
}

void Batarangs_Reset(void) {
    i32 i;
    for (i = 0; i < 8; ++i) {
        PLAYER_BATARANG_SLOT *slot = &BatarangPackets[i];
        BatarangSlotReset(slot);
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
    i32 angle = BatarangACos(cosine);
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
            BatarangSlotReset(batarang);
        } else if (batarang->active > 0) {
            for (j = 0; j < batarang->active; ++j) {
                if (batarang->targets[j].object != data) {
                    continue;
                }
                if (data == batarang->owner) {
                    BatarangSlotReset(batarang);
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
            if (batarang->owner->player.pad->buttons_held & GAMEPAD_ACTION) {
                Batarang_StartTargetting(batarang->owner);
            } else if (batarang->owner->player.pad->input_magnitude == 0 &&
                       batarang->owner->player.character_context == CHARACTER_CONTEXT_NONE &&
                       batarang->owner->character_model->model_data_b[0x98] != NULL) {
                batarang->owner->player.character_context = CHARACTER_CONTEXT_CATCH;
                batarang->owner->player.context_animation = ACTION_CATCH;
                batarang->owner->player.context_timer =
                    AnimDuration((i16)batarang->owner->id, batarang->owner->player.context_animation, 0, 0, 1);
            }
            BatarangSlotReset(batarang);
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

    i16 ID_DEFAULTCHARACTER_B = -1;
}

extern "C" f32 NuFdiv(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f) {
        return 0.0f;
    }
    f32 result = a / b;
    return result;
}
