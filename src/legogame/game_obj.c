#include "saga.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>


STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_107e) == 0x107e);
STATIC_ASSERT(offsetof(GAMEOBJECT, current_hp) == 0x107f);
STATIC_ASSERT(offsetof(PART_PC, part_id) == 0x13e);
STATIC_ASSERT(offsetof(PART_PC, active) == 0x144);
STATIC_ASSERT(sizeof(PART_PC) == 0x220);
STATIC_ASSERT(offsetof(GAMESAVE, difficulty) == 1);
typedef struct creature_crate_part_view_pc_s {
    u8 unknown_000[0x20c];
    f32 spawn_delay;
} CREATURE_CRATE_PART_VIEW_PC;
i32 __cdecl TagCode(GAMEOBJECT *source, GAMEOBJECT *destination, i32 takeover, i32 blend_camera, i32 unused);
void __cdecl MovePlayer_NETWORK(GAMEOBJECT *);
typedef struct grapple_rotation_pc_s {
    u8 unknown_00[0x30];
    NUVEC hook_position;
} GRAPPLE_ROTATION_PC;
extern const f64 GameObjectNearFloorNoFloorHeight;
extern u8 SuperCarryPlayerRgbPc[2][3];
extern i32 FalconDebKey[2];
void __cdecl DieRumble(GAMEOBJECT *);
void __cdecl GetTakeOverPos(GAMEOBJECT *, NUVEC *);
extern i16 CharacterFixupIdPc007f18c4;
extern const char BeenKilledPc[11];
extern const char EP3DogfightDiePc[17];
typedef struct kill_zipup_pc_s {
    u8 unknown_00[0x68];
    u16 runtime_flags;
} KILL_ZIPUP_PC;
typedef struct combat_coinpacket_pc_s {
    u32 coins;
} COMBAT_COINPACKET_PC;
typedef struct shield_state_pc_s {
    u8 unknown_000[0xd30];
    f32 shield_enabled;
    f32 shield_timer;
} SHIELD_STATE_PC;
typedef struct reaction_fields_pc_s {
    u8 unknown_00[0x6c4];
    u8 reset_contexts;
    u8 unknown_6c5[0x750 - 0x6c5];
    f32 animation_speed;
    u8 unknown_754[0x778 - 0x754];
    f32 reaction_duration;
} REACTION_FIELDS_PC;
/* Full PC ObjHitObj source contract: complete 5764-byte original interval,
   physical calling convention and finite reader cohorts independently reviewed.
   PC-only reaction and adaptive branches and PC DWORD masks are preserved.
   Existing partial part/object providers retain their explicit runtime gaps. */
typedef struct combat_reaction_owner_pc_s {
    u8 unknown_000[0x7bb];
    i8 attacker_slot;
} COMBAT_REACTION_OWNER_PC;
typedef struct combat_attack_counter_pc_s {
    u8 unknown_000[0x1083];
    u8 attack_count;
} COMBAT_ATTACK_COUNTER_PC;
typedef struct collision_exclusion_view_pc_s {
    u8 unknown_0000[0x298];
    GAMEOBJECT *excluded_object;
    u8 unknown_029c[4];
    u64 exclusion_mask;
} COLLISION_EXCLUSION_VIEW_PC;
typedef struct collision_platform_push_view_pc_s {
    u8 unknown_0000[0xdc4];
    f32 platform_push_timer;
} COLLISION_PLATFORM_PUSH_VIEW_PC;
typedef struct collision_techno_view_pc_s {
    u8 unknown_0000[0x68];
    u8 flags;
} COLLISION_TECHNO_VIEW_PC;
typedef struct collision_masks_view_pc_s {
    u8 unknown_0000[0x1e8];
    u64 collision_identity_mask;
    u64 collision_contact_mask;
} COLLISION_MASKS_VIEW_PC;
#define PANEL_PLAYER_ACTIVE(flags) (((flags) & 0x1000) != 0 && ((flags) & 0x80) != 0)

typedef struct KILLPARTS_MINIKIT_PIECE_PC {
    NUSPECIALHANDLE special;
    NUMTX matrix;
    u8 unknown_4c[0x1c];
} KILLPARTS_MINIKIT_PIECE_PC;
typedef struct KILLPARTS_MINIKIT_PIECES_PC {
    u8 unknown_0[4];
    KILLPARTS_MINIKIT_PIECE_PC *pieces;
    u8 piece_count;
} KILLPARTS_MINIKIT_PIECES_PC;
typedef struct KILLPARTS_RENDER_PC {
    const char *name;
    NUSPECIALHANDLE **rigid_specials;
    u8 unknown_8[0xc];
} KILLPARTS_RENDER_PC;
typedef struct KILLPARTS_OBJECT_LIGHT_PC {
    u8 unknown_0[0x4d8];
    PART_LIGHTING_DATA_PC lighting;
} KILLPARTS_OBJECT_LIGHT_PC;
typedef struct KILLPARTS_PART_PC {
    u8 unknown_0[0x20c];
    f32 speeder_index;
} KILLPARTS_PART_PC;
typedef struct KILLPARTS_RIDER_LIGHT_PC {
    u8 unknown_0[0x628];
    NUVEC intensity[2];
} KILLPARTS_RIDER_LIGHT_PC;
void __cdecl PartImpact_Brick(PART_PC *part);

void KillParts(GAMEOBJECT *object, i32 animation, i32 excluded_layer, i32 mode, float vertical_scale,
               i32 use_vehicle_velocity, u16 *angle_override);

static void __cdecl CreatureCrate_Stop(PART_PC *part) {
    GAMEOBJECT *object = part->owner;
    u16 debris;
    i32 type;
    if (object == NULL)
        return;
    debris = (u16)FindGameDebris(WORLD->game_debris_sys, "CRATE_POP");
    if (debris != 0xffff)
        AddGameDebris(WORLD->game_debris_sys, (i16)debris, &part->position);
    type = PARTLookupType("CRATE_PART");
    if (type != -1)
        AddFiniteShotPART(type, &part->position, 1);
    object->flags_1fc_word |= 0x1000;
    object->ai_packet.reset_mode = 2;
    PlaySfx("Explode1", &part->position);
    GameCam_Judder(GameCam, 0.1f, 0, NULL);
    NewRumbleAllPlayers(0.0f, 0.0f, 2, 0);
}

static void __cdecl CreatureCrate_MoveFn(PART_PC *part, f32 elapsed) {
    CREATURE_CRATE_PART_VIEW_PC *crate = (CREATURE_CRATE_PART_VIEW_PC *)part;
    crate->spawn_delay -= elapsed;
    if (crate->spawn_delay <= 0.0f) {
        part->move_fn = NULL;
        part->draw_fn = NULL;
    }
}

static i32 __cdecl CreatureCrate_DrawFn(PART_PC *part) {
    if (((CREATURE_CRATE_PART_VIEW_PC *)part)->spawn_delay > 0.0f)
        return 0;
    return 1;
}

void __cdecl SpawnCreatureFromCrate(GAMEOBJECT *object, f32 height, f32 delay) {
    NUVEC velocity = {0.0f, 0.0f, 0.0f};
    NUMTX matrix;
    ADDPART_PC parameters;
    PART_PC *part;
    if (NuSpecialExistsFn(&WORLD->level_objects[0xe6].special) == 0)
        return;
    NuMtxSetTranslation(&matrix, &object->collision_position);
    matrix.m31 += height;
    parameters = Default_ADDPART;
    parameters.matrix = &matrix;
    parameters.velocity = &velocity;
    parameters.field_14 = 0.1f;
    parameters.field_18 = 0.1f;
    parameters.special = &WORLD->level_objects[0xe6].special;
    parameters.time_step = FRAMETIME;
    parameters.gravity = -2.0f;
    parameters.flags = 0x313;
    parameters.owner = object;
    parameters.draw_fn = CreatureCrate_DrawFn;
    parameters.move_fn = CreatureCrate_MoveFn;
    parameters.stop_fn = CreatureCrate_Stop;
    part = AddPart(&parameters);
    if (part == NULL)
        return;
    part->force_flags = (u16)ObjHitObj_Flags(object);
    if (delay > 0.0f) {
        ((CREATURE_CRATE_PART_VIEW_PC *)part)->spawn_delay = delay;
    } else {
        part->move_fn = NULL;
        part->draw_fn = NULL;
        ((CREATURE_CRATE_PART_VIEW_PC *)part)->spawn_delay = 0.0f;
    }
    object->flags_1fc_word &= ~0x1000u;
    object->ai_packet.reset_mode = 3;
}

void __cdecl ManageGameObjects(void) {
    i32 i, j, area, alive;
    GAMEOBJECT *candidate, *other, *vehicle, *target;
    AICREATURE_PC *creature;
    AISYS_PC *system;
    AIGROUP_PC *group;
    AILOCATOR_PC *locator, *respawn_locator;
    u32 saved_flags, limit;
    i64 mask;
    GAMEOBJECT *object = Obj;
    memset(aicreature_sets_alive, 0, sizeof(aicreature_sets_alive));
    for (i = 0; i < HIGHGAMEOBJECT; ++i) {
        candidate = &object[i];
        if ((candidate->flags_1fc_word & 0x1000) != 0 && candidate->dead == 0 && candidate->ai_packet.creature_set != 0)
            ++aicreature_sets_alive[candidate->ai_packet.creature_set - 1];
    }
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        if ((object->flags_1fc_word & 1) == 0)
            continue;
        if ((object->flags_1f8 & 0x40000) == 0) {
            if ((object->flags_1fc_word & 0x1000) == 0) {
                if (object->ai_packet.creature_index == 0xff) {
                    if (object->ai_packet.reset_mode == 1) {
                        object->ai_packet.spawn_delay -= FRAMETIME;
                        if (object->ai_packet.spawn_delay <= 0.0f) {
                            object->flags_1fc_word |= 0x1000;
                            object->ai_packet.spawn_delay = 0.0f;
                            object->ai_packet.reset_mode = 2;
                        }
                    } else if (object->ai_packet.reset_mode != 3) {
                        if ((object->flags_1f8 & 0x4000) == 0) {
                            if (object->ai_packet.group != NULL) {
                                alive = 0;
                                for (j = 0; j < 4; ++j) {
                                    if (object->ai_packet.group->rows[j].is_alive != 0) {
                                        alive = 1;
                                        break;
                                    }
                                }
                                if (alive == 0) {
                                    DestroyAIGroup(object->ai_packet.group);
                                } else {
                                    for (j = 0; j < object->ai_packet.group->member_count; ++j) {
                                        if (object->ai_packet.group->members[j] == (APIOBJECT_PC *)object)
                                            object->ai_packet.group->members[j] = NULL;
                                    }
                                }
                            }
                            FreeTorpedoPacket(&object->torpedo);
                            RemoveGameObject(object, 1);
                        } else {
                            AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet,
                                            &object->ai_packet.script_processor, FRAMETIME);
                        }
                    }
                } else {
                    system = WORLD->ai_sys;
                    creature = &system->creatures[object->ai_packet.creature_index];
                    switch (object->ai_packet.reset_mode) {
                        case 0:
                            switch (creature->activation_type) {
                                case 0:
                                    if (Game.difficulty >= creature->activation_difficulty)
                                        ResetAICreature(object, system);
                                    else
                                        object->ai_packet.reset_mode = 4;
                                    break;
                                case 1:
                                    if (creature->activate_area != NULL) {
                                        if (player != NULL) {
                                            area = creature->activate_area - system->areas;
                                            mask = 1 << area;
                                            if ((U64_PAIR(player->ai_area_mask) & mask) != 0) {
                                                if (Game.difficulty >= creature->activation_difficulty) {
                                                    if (creature->count > 1 && creature->start_stagger > 0.0f &&
                                                        object->ai_packet.group_member_index != 0) {
                                                        object->ai_packet.spawn_delay =
                                                            (f32)(u32)object->ai_packet.group_member_index *
                                                            creature->start_stagger;
                                                        object->ai_packet.reset_mode = 1;
                                                    } else {
                                                        ResetAICreature(object, system);
                                                    }
                                                } else {
                                                    object->ai_packet.reset_mode = 4;
                                                }
                                            }
                                        }
                                    } else {
                                        creature->activation_type = 0;
                                    }
                                    break;
                                case 2:
                                    if (Game.difficulty >= creature->activation_difficulty)
                                        AIScriptProcess(system, (APIOBJECT_PC *)object, &object->ai_packet,
                                                        &object->ai_packet.script_processor, FRAMETIME);
                                    break;
                            }
                            break;
                        case 1:
                            group = object->ai_packet.group;
                            if (group != NULL && (group->flags & 2) == 0)
                                break;
                            alive = 1;
                            if (creature->activation_type == 1) {
                                area = creature->activate_area - system->areas;
                                mask = 1 << area;
                                if (player == NULL || (U64_PAIR(player->ai_area_mask) & mask) == 0 ||
                                    Game.difficulty < creature->activation_difficulty)
                                    alive = 0;
                            }
                            if (alive != 0) {
                                object->ai_packet.spawn_delay -= FRAMETIME;
                                if (object->ai_packet.spawn_delay <= 0.0f) {
                                    if (group != NULL) {
                                        locator = object->ai_packet.script_processor.locator;
                                        respawn_locator = object->ai_packet.respawn_locator;
                                        for (j = 0; j < object->ai_packet.group->member_count; ++j) {
                                            if (object->ai_packet.group->members[j] != NULL) {
                                                target = object->ai_packet.group->members[j]->owner;
                                                target->ai_packet.spawn_delay = 0.0f;
                                                target->ai_packet.script_processor.locator = locator;
                                                target->ai_packet.respawn_locator = respawn_locator;
                                                ResetAICreature(target, WORLD->ai_sys);
                                                if (object->ai_packet.group->leader == NULL)
                                                    object->ai_packet.group->leader =
                                                        object->ai_packet.group->members[j];
                                            }
                                        }
                                        object->ai_packet.group->flags &= ~8u;
                                        object->ai_packet.group->flags &= ~4u;
                                    } else {
                                        object->ai_packet.spawn_delay = 0.0f;
                                        if (object->id == (u16)id_PKDROID) {
                                            ResetAICreature(object, WORLD->ai_sys);
                                            SpawnCreatureFromCrate(object, 3.0f, 0.0f);
                                        } else {
                                            ResetAICreature(object, WORLD->ai_sys);
                                        }
                                    }
                                }
                            } else if (creature->count > 1 && creature->start_stagger > 0.0f) {
                                object->ai_packet.spawn_delay =
                                    (f32)(u32)object->ai_packet.group_member_index * creature->start_stagger;
                            }
                            break;
                    }
                }
            } else if (object->dead != 0) {
                if (object->tag_object_callback != NULL) {
                    object->tag_object_callback(object);
                    object->tag_object_callback = NULL;
                }
                if (object->field_1010 > 0.0f) {
                    object->field_1010 -= FRAMETIME;
                    if (object->field_1010 <= 0.0f) {
                        object->field_1010 = 0.0f;
                        other = NULL;
                        if (WORLD->current_level == PODSPRINTA_LDATA)
                            other = GetOtherActivePlayer(object);
                        if (other != NULL) {
                            object->ai_update_position = object->position = object->start_position = other->position;
                            GameObjectOrigin(object);
                            object->velocity = other->velocity;
                            object->field_0x276 = object->facing_angle = object->movement_facing_angle =
                                other->field_0x276;
                        } else {
                            SetToLastSafePos(object);
                            GameObjectOrigin(object);
                            object->velocity.x = object->velocity.z = 0.0f;
                            object->velocity.y = -0.1f;
                            object->field_0x276 = object->facing_angle = object->movement_facing_angle;
                        }
                        saved_flags = object->flags_1fc_bit_13;
                        ResetPlayerMoves(object);
                        object->dead = 0;
                        object->flags_1fc_bit_13 = saved_flags;
                        object->current_hp = object->field_107e;
                        object->shield_hitpoints = object->character_data->game_character->shield_hitpoints;
                        object->saved_hitpoints = 4;
                        object->flags_ef0 |= 0x80;
                        object->character_context = 0xff;
                        if (object->field_280 != 0xff)
                            object->spawn_protection_timer = 2.5f;
                        AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, 0xff, 1);
                        vehicle = object->takeover_owner;
                        if (vehicle != NULL && (vehicle->rider == NULL || vehicle->rider == object) &&
                            (vehicle->flags_1fc_word & 0x1000) != 0 && vehicle->character_context != 0x2b &&
                            vehicle->dead == 0) {
                            GetTakeOverPos(vehicle, &object->position);
                            TakeOverGameObject(object, object->takeover_owner, 0, 1);
                        }
                        if ((object->flags_1fc_word & 0x80) != 0 && WORLD->current_level == SPEEDERCHASEA_LDATA &&
                            disable_narrow_socks == 0)
                            GameCam_Blend(GameCam, 0.5f, 0.0f, 0);
                    } else if (WORLD->current_level == PODSPRINTA_LDATA) {
                        other = GetOtherActivePlayer(object);
                        if (other != NULL)
                            object->position = other->position;
                    }
                } else {
                    if ((object->death_timer += FRAMETIME) >= object->death_delay) {
                        object->death_timer = object->death_delay;
                        other = Obj;
                        for (j = 0; j < HIGHGAMEOBJECT; ++j, ++other) {
                            U64_PAIR(other->collision_contact_mask) &= ~U64_PAIR(object->collision_identity_mask);
                            U64_PAIR(other->hostility_mask) &= ~((u64)1 << object->hostility_group);
                            U64_PAIR(&other->field_eb0) &= ~((u64)1 << object->hostility_group);
                            U64_PAIR(other->ai_opponent_exclusion_mask) &= ~((u64)1 << object->hostility_group);
                        }
                        if ((object->flags_1f8 & 0x4000) != 0) {
                            if ((object->flags_1f8 & 0x400) == 0) {
                                object->field_1010 = 2.0f;
                            } else {
                                creature = &WORLD->ai_sys->creatures[object->ai_packet.creature_index];
                                if (object->respawnable) {
                                    if (!object->respawn_at_origin) {
                                        if (object->id == (u16)id_STAP2 && WORLD->current_level == NEGOTIATIONSC_LDATA)
                                            object->field_1010 = 5.0f;
                                        else
                                            object->field_1010 = 1.0f;
                                        continue;
                                    }
                                } else {
                                    if (creature->max_respawn_count != -1 && object->ai_packet.respawn_locator == NULL)
                                        limit = creature->min_respawn_count +
                                                (Game.difficulty - 1) *
                                                    (creature->max_respawn_count - creature->min_respawn_count) / 9 +
                                                1;
                                    else
                                        limit = object->ai_packet.respawn_count + 1;
                                    if (object->ai_packet.respawn_count >= limit) {
                                        object->flags_1fc_word &= ~0x1000;
                                        object->ai_packet.reset_mode = 4;
                                        continue;
                                    }
                                }
                                object->flags_1fc_word &= ~0x1000;
                                object->ai_packet.reset_mode = 1;
                                object->ai_packet.spawn_delay = 1.0 - ((f64)(u32)Game.difficulty - 1.0) / 9.0;
                                object->ai_packet.spawn_delay =
                                    creature->max_respawn_time * object->ai_packet.spawn_delay +
                                    creature->min_respawn_time * (1.0f - object->ai_packet.spawn_delay);
                            }
                        } else if (object->respawnable) {
                            object->field_1010 = 1.0f;
                        } else {
                            if (object->ai_packet.group != NULL) {
                                alive = 0;
                                for (j = 0; j < 4; ++j) {
                                    if (object->ai_packet.group->rows[j].is_alive != 0) {
                                        alive = 1;
                                        break;
                                    }
                                }
                                if (alive == 0) {
                                    DestroyAIGroup(object->ai_packet.group);
                                } else {
                                    for (j = 0; j < object->ai_packet.group->member_count; ++j) {
                                        if (object->ai_packet.group->members[j] == (APIOBJECT_PC *)object)
                                            object->ai_packet.group->members[j] = NULL;
                                    }
                                }
                            }
                            FreeTorpedoPacket(&object->torpedo);
                            RemoveGameObject(object, 1);
                        }
                    }
                }
            }
        } else if (nethost != 0 && (object->flags_1fc_word & 0x1000) == 0 && object->ai_packet.creature_index == 0xff &&
                   object->ai_packet.reset_mode != 1 && object->ai_packet.reset_mode != 3 &&
                   (object->flags_1f8 & 0x4000) != 0) {
            AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet,
                            &object->ai_packet.script_processor, FRAMETIME);
        }
    }
}

static i32 Collide2Objects(APIOBJECT_PC *first, APIOBJECT_PC *second) {
    f32 first_vertical_velocity = first->velocity.y;
    f32 second_vertical_velocity = second->velocity.y;
    GAMEOBJECT *first_object = (GAMEOBJECT *)first->owner;
    GAMEOBJECT *second_object = (GAMEOBJECT *)second->owner;
    f32 maximum_velocity, velocity_squared, maximum_squared, scale;
    i32 collision_result = ForcePushed_SuperPush_Occurring(first_object, second_object);
    if (collision_result != 0) {
        collision_result = ((COLLISION_MASKS_VIEW_PC *)second_object)->collision_identity_mask &
                                       ((COLLISION_MASKS_VIEW_PC *)first_object)->collision_contact_mask ||
                                   ((COLLISION_MASKS_VIEW_PC *)second_object)->collision_contact_mask &
                                       ((COLLISION_MASKS_VIEW_PC *)first_object)->collision_identity_mask
                               ? 1
                               : 2;
        ((COLLISION_MASKS_VIEW_PC *)first)->collision_contact_mask |=
            ((COLLISION_MASKS_VIEW_PC *)second)->collision_identity_mask;
        ((COLLISION_MASKS_VIEW_PC *)second)->collision_contact_mask |=
            ((COLLISION_MASKS_VIEW_PC *)first)->collision_identity_mask;
        return collision_result;
    }
    if (WORLD->current_level == DOGFIGHTA_LDATA)
        collision_result = APIObjectCollision(first, second);
    else
        collision_result = APIObjectCollision2D(first, second);
    if (collision_result == 2 && VehicleArea == 0) {
        if ((first_object->character_data->game_character->flags_090 & 0x1000) &&
            first->horizontal_velocity_magnitude > first_object->character_data->game_character->run_speed * 0.5)
            StartFlatten(first_object, second_object);
        else if ((second_object->character_data->game_character->flags_090 & 0x1000) &&
                 second->horizontal_velocity_magnitude > second_object->character_data->game_character->run_speed * 0.5)
            StartFlatten(second_object, first_object);
        else if (second_vertical_velocity < 0.0f && (first_object->character_data->game_character->flags_090 & 0x40) &&
                 first_object->level_object_index == -1)
            first->velocity.y += second_vertical_velocity * 0.5;
        else if (first_vertical_velocity < 0.0f && (second_object->character_data->game_character->flags_090 & 0x40) &&
                 second_object->level_object_index == -1)
            second->velocity.y += first_vertical_velocity * 0.5;
    }
    maximum_velocity = first->character_data->game_character->run_speed * 1.25;
    if (maximum_velocity < 1.0f)
        maximum_velocity = 1.0f;
    velocity_squared = first->velocity.x * first->velocity.x + first->velocity.z * first->velocity.z;
    maximum_squared = maximum_velocity * maximum_velocity;
    if (velocity_squared > maximum_squared) {
        scale = maximum_velocity / NuFsqrt(velocity_squared);
        first->velocity.x *= scale;
        first->velocity.z *= scale;
    }
    maximum_velocity = second->character_data->game_character->run_speed * 1.25;
    if (maximum_velocity < 1.0f)
        maximum_velocity = 1.0f;
    velocity_squared = second->velocity.x * second->velocity.x + second->velocity.z * second->velocity.z;
    maximum_squared = maximum_velocity * maximum_velocity;
    if (velocity_squared > maximum_squared) {
        scale = maximum_velocity / NuFsqrt(velocity_squared);
        second->velocity.x *= scale;
        second->velocity.z *= scale;
    }
    return collision_result;
}

void __cdecl CollideGameObjects(WORLDINFO *world) {
    i32 object_count = HIGHGAMEOBJECT, index, player_index, collision_count = 0;
    u32 player_slot_mask = 0, vertical_movement_mask = 0, no_vertical_movement_mask = 0, player_collision_mask = 0,
        flattened_mask = 0, narrow_speeder_mask = 0;
    GAMEOBJECT *object, *selected_player;
    APIOBJECT_PC *collision_objects[64], **object_cursor;
    NUVEC collision_minimums[64], collision_maximums[64], *minimum_cursor, *maximum_cursor, direction;
    PLAYERCHARACTERCONFIG_PC *data;
    COLLISION_EXCLUSION_VIEW_PC *exclusion;
    u64 bit;
    u32 flags, is_player;
    i8 context;
    if (world->area_index != -1 && (world->area->flags & 0x80))
        return;
    for (index = 0, object = Obj; index < object_count; ++index, ++object) {
        flags = object->flags_1fc_word;
        if (!(flags & 1))
            continue;
        bit = (u64)1 << object->hostility_group;
        if (object->field_280 != 0xff)
            player_slot_mask |= bit;
        if (object->character_data->game_character->field_028 > 0)
            vertical_movement_mask |= bit;
        else
            no_vertical_movement_mask |= bit;
        data = object->character_data->game_character;
        is_player = (flags >> 7) & 1;
        if ((is_player || (object->flags_ef4 & 0x80000)) && !(data->flags_090 & 0x40))
            player_collision_mask |= bit;
        if (object->character_context == 0x3d && !is_player)
            flattened_mask |= bit;
        if (WORLD->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks == 0 &&
            object->id == (u16)id_SPEEDERBIKE && object->field_280 != 0xff)
            narrow_speeder_mask |= bit;
    }
    object_cursor = collision_objects;
    minimum_cursor = collision_minimums;
    maximum_cursor = collision_maximums;
    for (index = 0, object = Obj; index < HIGHGAMEOBJECT; ++index, ++object) {
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 || object->dead != 0 ||
            object->model_draw_result == 0 || object->use_model_origin < 2)
            continue;
        context = object->character_context;
        if ((U64_PAIR(&CInfo[context].flags) & 0x8000) || context == 0x3b || context == 0x3c || context == 0xf ||
            context == 0x39)
            continue;
        if (context == 0x26 && object->special_move_pc != -1 && (object->context_variant_flags & 0x200))
            continue;
        if (object->character_context == 0x1f && object->field_7b3 == 1)
            continue;
        data = object->character_data->game_character;
        if ((data->flags_090 & 0x8000) || (object->flags_e20 & 0x20) ||
            (context == 0x47 && (((COLLISION_TECHNO_VIEW_PC *)object->techno)->flags & 1)))
            continue;
        *minimum_cursor++ = object->collision_min;
        *maximum_cursor++ = object->collision_max;
        *object_cursor++ = (APIOBJECT_PC *)object;
        ++collision_count;
        exclusion = (COLLISION_EXCLUSION_VIEW_PC *)object;
        exclusion->exclusion_mask = 0;
        data = object->character_data->game_character;
        if (data->flags_090 & 0x1000)
            exclusion->exclusion_mask = flattened_mask;
        if (object->level_object_index != -1) {
            exclusion->exclusion_mask |= player_collision_mask;
            for (player_index = 0; player_index < 8; ++player_index) {
                selected_player = (GAMEOBJECT *)Player[player_index];
                if (selected_player == NULL || !(selected_player->flags_1fc_word & 1) ||
                    !(selected_player->flags_1fc_word & 0x1000) || object == selected_player ||
                    !(((COLLISION_PLATFORM_PUSH_VIEW_PC *)selected_player)->platform_push_timer > 0) ||
                    selected_player->supporting_platform_id != object->level_object_index ||
                    (selected_player->terrain_platform_id == object->level_object_index &&
                     (selected_player->field_281 != 0 || GameObjectNearFloor(selected_player, 3.0f, NULL) != 0)))
                    continue;
                direction.x = object->collision_position.x - selected_player->collision_position.x;
                direction.y = 0;
                direction.z = object->collision_position.z - selected_player->collision_position.z;
                NuVecNorm(&direction, &direction);
                object->movement_direction_x = direction.x;
                object->movement_direction_z = direction.z;
            }
        }
        if (object->character_context == 0 && object->airborne_collision_target != NULL)
            exclusion->exclusion_mask |= (u64)1 << object->airborne_collision_target->hostility_group;
        if (WORLD->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks == 0 &&
            object->id == (u16)id_SPEEDERBIKE && object->field_280 != 0xff)
            exclusion->exclusion_mask |= narrow_speeder_mask;
        if (VehicleArea != 0 && WORLD->current_level != CollisionVehicleExcludedLevelPc &&
            WORLD->current_level != DOGFIGHTA_LDATA) {
            if (object->field_280 != 0xff) {
                if (object->character_data->game_character->field_028 != 0)
                    exclusion->exclusion_mask |= vertical_movement_mask;
                else
                    exclusion->exclusion_mask |= (~player_slot_mask) & vertical_movement_mask;
            } else {
                if (object->character_data->game_character->field_028 != 0)
                    exclusion->exclusion_mask |=
                        (vertical_movement_mask & player_slot_mask) | no_vertical_movement_mask;
                else
                    exclusion->exclusion_mask |= (~player_slot_mask) & vertical_movement_mask;
            }
        }
    }
    APIObjectCollisions(collision_count, collision_objects, collision_minimums, collision_maximums, Collide2Objects);
}

void __cdecl GameObjectRotation(GAMEOBJECT *object, i32 mode) {
    PLAYERCHARACTERCONFIG_PC *game;
    u16 x;
    u16 z;
    u32 flags;
    i32 magnet;
    i32 sliding;
    i32 grapple;
    i32 alignment = 0;
    i32 reflection;
    NUVEC direction;

    if ((object->character_data->game_character->flags_090 & 0x200000) != 0) {
        NUVEC normal;
        NUVEC corner;
        NUVEC next;
        NUVEC after;
        NUVEC first_edge;
        NUVEC second_edge;
        NUVEC points[4];
        const NUVEC *offsets;
        f32 drop = object->position.y - object->start_position.y;
        f32 rate = drop / FRAMETIME;
        f32 height;
        i32 index;
        if (object->id == GameObjectRotationBuggyId) {
            offsets = GAMEOBJECT_ROTATION_BUGGY_OFFSETS;
        } else if (object->id == GameObjectRotationRadioCarId) {
            offsets = GAMEOBJECT_ROTATION_RADIOCAR_OFFSETS;
        } else if (object->id == GameObjectRotationPoliceCarId) {
            offsets = GAMEOBJECT_ROTATION_POLICECAR_OFFSETS;
        } else if (object->id == GameObjectRotationMowerId) {
            offsets = GAMEOBJECT_ROTATION_MOWER_OFFSETS;
        } else {
            offsets = GAMEOBJECT_ROTATION_DEFAULT_OFFSETS;
        }
        for (index = 0; index < 4; ++index) {
            NuVecRotateY(&points[index], (NUVEC *)&offsets[index], object->field_0x276);
            NuVecAdd(&points[index], &points[index], &object->lower_position);
            height = GameShadow(NULL, &points[index], 5.0f, -1);
            if (height == GameObjectNearFloorNoFloorHeight) {
                object->suspension[index].height = object->lower_position.y;
                object->suspension[index].velocity = drop / FRAMETIME;
            } else {
                f64 gravity_delta;
                object->suspension[index].height = drop + object->suspension[index].height;
                gravity_delta = (f64)FRAMETIME * -5.0;
                object->suspension[index].velocity = (f32)(gravity_delta + object->suspension[index].velocity);
                object->suspension[index].height += object->suspension[index].velocity * FRAMETIME;
                if (object->suspension[index].height < height) {
                    object->suspension[index].height = height;
                    object->suspension[index].velocity = rate;
                }
                height = object->lower_position.y - object->bounds_radius * 2.0f;
                if (object->suspension[index].height < height) {
                    object->suspension[index].height = height;
                    object->suspension[index].velocity = rate;
                } else {
                    height = object->bounds_radius * 3.0 + object->lower_position.y;
                    if (object->suspension[index].height > height) {
                        object->suspension[index].height = height;
                        object->suspension[index].velocity = rate;
                    }
                }
            }
            corner.x = points[index].x;
            corner.y = object->suspension[index].height;
            corner.z = points[index].z;
        }
        normal = v000;
        for (index = 0; index < 4;) {
            i32 next_index = (index + 1) % 4;
            i32 after_index = (next_index + 1) % 4;
            corner.x = points[index].x;
            corner.y = object->suspension[index].height;
            corner.z = points[index].z;
            next.x = points[next_index].x;
            next.y = object->suspension[next_index].height;
            next.z = points[next_index].z;
            after.x = points[after_index].x;
            after.y = object->suspension[after_index].height;
            after.z = points[after_index].z;
            NuVecSub(&first_edge, &next, &corner);
            NuVecSub(&second_edge, &after, &corner);
            NuVecCross(&first_edge, &first_edge, &second_edge);
            NuVecAdd(&normal, &normal, &first_edge);
            index++;
        }
        NuVecScale(&normal, &normal, 0.25f);
        if (normal.y > 0.0f) {
            NuVecNorm(&object->suspension_normal, &normal);
        }
    }

    if (0 == ((object->flags_1fc & 4))) {
        if (0 != ((object->character_data->game_character->flags_090 & 0x200000))) {
            FindAnglesZX(&object->suspension_normal, &object->field_1056, &object->field_1058);
        } else if (object->surface_normal.x != 0.0f || object->surface_normal.y != 1.0f ||
                   object->surface_normal.z != 0.0f) {
            FindAnglesZX(&object->surface_normal, &object->field_1056, &object->field_1058);
        } else {
            object->field_1058 = 0;
            object->field_1056 = 0;
        }
    }

    x = object->field_1056;
    temp_zrot = x;
    z = object->field_1058;
    temp_xrot = z;
    game = object->character_data->game_character;
    flags = game->flags_090;
    sliding = (flags & 0x200) != 0 && object->character_context == 0x33;
    magnet = (flags & 0x10000000) != 0 && object->character_context == 0x44;
    grapple = object->character_context == 0x46 && object->field_7b3 == 0;

    if (object->flags_107a == 2 &&
        ((object->character_data->model_flags & 0x100) != 0 || (flags & 0x200000) != 0 || sliding)) {
        f32 hover =
            (object->character_data->model_flags & 0x2000) != 0 ? GetVehicleHoverHeight(object, NULL) : game->field_028;
        if (hover > 0.0f) {
            if (GameObjectNearFloorNoFloorHeight != object->floor_height) {
                f32 limit = 2.5 * hover;
                f32 height = object->collision_min.y - object->floor_height;
                f32 fraction;
                if (height < hover) {
                    fraction = 1.0f;
                } else if (limit > height) {
                    fraction = 1.0f - (height - hover) / (limit - hover);
                } else {
                    fraction = 0.0f;
                }
                x = (u16)((f32)RotDiff(0, temp_zrot) * fraction);
                z = (u16)((f32)RotDiff(0, temp_xrot) * fraction);
            } else {
                x = 0;
                z = 0;
            }
            alignment = 2;
        } else if (object->field_281 != 0 || (object->character_data->game_character->flags_090 & 0x200000) != 0 ||
                   sliding) {
            alignment = 1;
        } else if ((object->character_data->model_flags & 0x100) != 0 &&
                   0.0f == object->character_data->game_character->field_028 &&
                   (object->character_data->game_character->flags_090 & 0x100) != 0 &&
                   GameObjectNearFloorNoFloorHeight != object->floor_height) {
            f32 vertical_gap = object->collision_min.y - object->floor_height;
            if (vertical_gap < object->bounds_half_height)
                alignment = 1;
        }

        if (object->id == GameObjectRotationAtstId && VehicleArea == 0) {
            x = (u16)((f32)RotDiff(0, temp_zrot) * 0.5);
            z = (u16)((f32)RotDiff(0, temp_xrot) * 0.5);
        }
    }

    reflection = 0 != (0x2000 & object->character_data->model_flags) &&
                 object->character_data->game_character->field_028 > 0.0f &&
                 (object->field_283 == 7 || object->field_283 == 0x10) && object->field_280 == 0xff;

    if (mode == 1) {
        object->field_1052 = x;
        object->field_1054 = z;
        if (alignment == 0) {
            object->pitch_angle = object->roll_angle = 0;
        } else if (magnet) {
            if (object->field_7aa == 6 || object->field_7aa == 5) {
                object->pitch_angle = 0;
                object->roll_angle = 0;
            } else {
                object->pitch_angle = object->field_7a4;
                object->roll_angle = object->field_7a8;
            }
        } else if (grapple) {
            GRAPPLE_ROTATION_PC *hook = (GRAPPLE_ROTATION_PC *)object->grapple;
            NuVecSub(&direction, &hook->hook_position, &object->collision_position);
            FindAnglesZX(&direction, &object->pitch_angle, &object->roll_angle);
        } else if (reflection) {
            object->pitch_angle = object->field_105c;
            object->roll_angle = object->field_105e;
        } else if (!(alignment == 2 || object->surface_normal.y > NuTrigTable[0x3000])) {
            object->pitch_angle = object->roll_angle = 0;
        } else {
            object->pitch_angle = x;
            object->roll_angle = z;
        }
    } else if (mode == 2) {
        if (object->field_1052 != object->field_1056) {
            object->field_1052 = SeekRot(object->field_1052, x, 8.0f);
        }
        if (object->field_1054 != object->field_1058) {
            object->field_1054 = SeekRot(object->field_1054, z, 8.0f);
        }
        if (object->flags_107a == 2) {
            f32 speed;
            u16 target_x, target_z;
            if (magnet) {
                if (object->field_7aa == 6 || object->field_7aa == 5) {
                    object->pitch_angle = 0;
                    target_z = 0;
                    object->roll_angle = 0;
                    target_x = 0;
                } else {
                    target_x = object->field_7a4;
                    target_z = object->field_7a8;
                }
            } else if (grapple) {
                GRAPPLE_ROTATION_PC *hook = (GRAPPLE_ROTATION_PC *)object->grapple;
                NuVecSub(&direction, &hook->hook_position, &object->collision_position);
                FindAnglesZX(&direction, &target_x, &target_z);
            } else if (reflection) {
                target_x = object->field_105c;
                target_z = object->field_105e;
            } else if (alignment == 2 || (alignment != 0 && object->surface_normal.y > NuTrigTable[0x3000])) {
                target_x = x;
                target_z = z;
            } else {
                target_z = 0;
                target_x = 0;
            }

            speed = object->id == GameObjectRotationAtstId && VehicleArea == 0 ? 3.0f : 8.0f;
            if (object->pitch_angle != target_x) {
                object->pitch_angle = SeekRot(object->pitch_angle, target_x, speed);
            }
            if (object->roll_angle != target_z) {
                object->roll_angle = SeekRot(object->roll_angle, target_z, speed);
            }
        }
    }
}

void __cdecl PlayerTakeHit(GAMEOBJECT *object, GAMEOBJECT *attacker) {
    i32 state;
    i32 animation;
    f32 duration;
    if ((i8)object->field_280 != -1)
        return;
    if (object->jump_variant_state == 1 && object->character_model->model_data_b[0x3e] != NULL)
        animation = 0x3e;
    else if (object->jump_variant_state != 1 && object->character_model->model_data_b[0x3d] != NULL)
        animation = 0x3d;
    else {
        if (object->portal_flags & 0x08000000) {
            ResetAnimPacket(&object->anim_packet, -1);
            Player_ClearContext(object, 1);
            Player_ResetContexts(object->ai_player_packet_pc);
        }
        return;
    }
    state = object->jump_variant_state;
    if (object->character_context == 0x1b && object->force_target != NULL &&
        object->force_target->character_context == 0x1c)
        object->force_target->character_context = -1;
    object->field_7aa = (i16)animation;
    ResetAnimPacket(&object->anim_packet, -1);
    Player_ClearContext(object, 1);
    Player_ResetContexts(object->ai_player_packet_pc);
    object->character_context = 0x15;
    duration = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
    object->context_timer = duration;
    if (duration <= 0.0f)
        object->context_timer = 1.0f;
    object->jump_variant_state = state;
    SetFlicker(object, 0.4f);
    if (attacker != NULL) {
        object->facing_angle = object->movement_facing_angle = object->field_0x276 =
            NuAtan2D(attacker->collision_position.x - object->collision_position.x,
                     attacker->collision_position.z - object->collision_position.z);
    }
}

i32 __cdecl PlayerTakeHitReactionPc(GAMEOBJECT *target, GAMEOBJECT *attacker, f32 duration, u32 flags) {
    REACTION_FIELDS_PC *view = (REACTION_FIELDS_PC *)target;
    void **animations;
    i32 choices[16], count = 0, animation, saved_jump_state;
    f32 speed, timer, dx, dz;
    i16 index;
    if (target->field_280 == 0xff) {
        animations = target->character_model->model_data_b;
        if (animations[0xa7] != NULL && animations[0xaa] != NULL)
            choices[count++] = 0xa7;
        if (animations[0xa8] != NULL && animations[0xab] != NULL)
            choices[count++] = 0xa8;
        if (animations[0xa9] != NULL && animations[0xac] != NULL)
            choices[count++] = 0xa9;
        if (!(flags & 0x200)) {
            if (animations[0x3d] != NULL)
                choices[count++] = 0x3d;
            if (attacker != NULL && (attacker->flags_1fc_word & 0x80) && animations[0xb9] != NULL)
                choices[count++] = 0xb9;
        }
        if (count != 0)
            animation = choices[qrand() / (65535 / count + 1)];
        else
            animation = 1;
        if (animation != -1) {
            saved_jump_state = target->jump_variant_state;
            target->field_7aa = (i16)animation;
            speed = AnimSpeed(target->character_model, (i16)animation);
            view->animation_speed = speed;
            if (target->field_7aa != 0xb9 && target->field_7aa != 0x3d) {
                if (speed <= 0.0f)
                    view->animation_speed = 1.0f;
                else if (speed <= (f64)0.8f)
                    view->animation_speed = 0.8f;
            }
            ResetAnimPacket(&target->anim_packet, -1);
            Player_ClearContext(target, 1);
            Player_ResetContexts(&view->reset_contexts);
            view->reaction_duration = duration;
            target->character_context = 0x5a;
            target->field_7b3 = 0;
            if (attacker != NULL) {
                target->facing_angle = target->movement_facing_angle = target->field_0x276 =
                    NuAtan2D(attacker->collision_position.x - target->collision_position.x,
                             attacker->collision_position.z - target->collision_position.z);
            }
            index = target->field_7aa;
            if (target->character_model->model_data_b[index] != NULL &&
                (target->character_model->animation_configs[index]->flags & 2)) {
                target->context_timer = (f32)(((f64)qrand() * 1.5259021893143654e-05) * 0.5 + 1.0);
            } else {
                timer = AnimDuration((i16)target->id, index, 0.0f, 0.0f, index != 0xb9);
                target->context_timer = timer;
                if (timer <= 0.0f)
                    target->context_timer = 0.6f;
            }
            if (attacker != NULL) {
                dx = target->collision_position.x - attacker->collision_position.x;
                dz = target->collision_position.z - attacker->collision_position.z;
                target->movement_facing_angle = NuAtan2D(-dx, -dz);
            }
            target->jump_variant_state = saved_jump_state;
            return target->field_7aa == 0xb9 ? 2 : 1;
        }
    }
    return 0;
}

i32 __cdecl DeactivatePlayer(GAMEOBJECT *object, f32 duration, GAMEOBJECT *source) {
    f32 model_state;
    if (object->character_context != 0x17 || object->context_timer < duration) {
        model_state = object->origin_scale_override;
        Player_ClearContext(object, 1);
        if (object->character_context != 0x3e) {
            Player_ResetContexts(object->ai_player_packet_pc);
            object->character_context = 0x17;
            object->origin_scale_override = model_state;
            if (AIScriptSetBaseScriptStateByName((AISCRIPTPROCESS_PC *)&object->ai_packet,
                                                 (char *)PlayerBeenDeactivatedPc))
                AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet,
                                (AISCRIPTPROCESS_PC *)&object->ai_packet, FRAMETIME);
            if (source != NULL && (source->character_data->model_flags & 0x40) &&
                object->character_data->game_character->weapon_action == 1 &&
                object->character_model->model_data_b[0x62] != NULL) {
                duration = 4.0f;
                object->field_7aa = 0x62;
            } else if (object->character_model->model_data_b[0x81] != NULL &&
                       !(object->character_model->animation_configs[0x81]->flags & 2))
                object->field_7aa = 0x81;
            else
                object->field_7aa = object->character_model->model_data_b[0x41] != NULL ? 0x41 : 1;
            if (object->character_data->model_flags & 0x20)
                SetProtocolDroidDeactivatedAction(object);
            object->action_movement_state = 0;
            object->context_timer = duration;
            object->context_duration = 0.0f;
            object->context_hover_target_y = 0.0f;
            NewBuzz(object->pad->bound_pad, 0.1f, 0);
            return 1;
        }
    }
    return 0;
}

void __cdecl ActivatePlayer(GAMEOBJECT *object) {
    object->character_context = -1;
    if (object->character_model->model_data_b[0x80] != NULL) {
        object->character_context = 0x41;
        object->field_7aa = 0x80;
        object->context_timer = AnimDuration((i16)object->id, 0x80, 0.0f, 0.0f, 1);
        object->context_duration = 0.0f;
    }
}

void __cdecl GameObjectToCameraCode(GAMEOBJECT *object) {
    PLAYERCHARACTERCONFIG_PC *data;
    u32 mask;

    if (GameTimer.update_count != 0 && object->ai_update_distance < WORLD->current_level->blob_shadow_fade_far) {
        if (object->ai_update_distance > WORLD->current_level->blob_shadow_fade_near) {
            object->shadow_opacity =
                1.0f - (object->ai_update_distance - WORLD->current_level->blob_shadow_fade_near) /
                           (WORLD->current_level->blob_shadow_fade_far - WORLD->current_level->blob_shadow_fade_near);
        } else {
            object->shadow_opacity = 1.0f;
        }
    } else {
        object->shadow_opacity = 0.0f;
    }
    if ((u32)GameTimer.update_count > 0) {
        data = object->character_data->game_character;
        if ((data->flags_094 & 0x10) == 0) {
            if (object->dead != 0 &&
                (mask = apicharsys->char_data[((CHARACTER_MODEL_LAYERS_PC *)object->character_model)->model_id]
                            .game_character->layer_mask_dead) != 0) {
                object->layer_mask = mask;
            } else if (object->ai_update_distance > data->layer_distance_low) {
                object->layer_mask = data->layer_mask_low;
            } else if (object->ai_update_distance > data->layer_distance_medium) {
                object->layer_mask = data->layer_mask_medium;
            } else {
                object->layer_mask = data->layer_mask;
            }
            object->layer_mask |= object->extra_layer_mask;
            object->layer_mask = AdjustLayerBits(object->layer_mask, object);
        }
    }
}

void __cdecl SetKillPartMom(NUVEC *momentum) {
    momentum->x = 0.0f;
    momentum->y = 1.0f;
    momentum->z = 0.0f;
    NuVecRotateZ(momentum, momentum, qrand());
    NuVecRotateX(momentum, momentum, qrand());
}

void __cdecl TiePart_Kill(PART_PC *part, i32 unused) {
    AddGameDebris(WORLD->game_debris_sys, 0x6a, &part->position);
}

void __cdecl TiePart_KillExplode(PART_PC *part, i32 unused) {
    AddGameDebris(WORLD->game_debris_sys, 0x6b, &part->position);
    AddPartDebris(WORLD->part_debris_sys, 3, &part->position);
}

void __cdecl TiePart_Impact(PART_PC *part) {
    AddGameDebris(WORLD->game_debris_sys, 0x6a, &part->position);
}

/* Native local helper takes the particle in ESI and time on the stack. This
 * logical signature is private to the coherent compiler-generated caller group. */
static void __cdecl TiePart_MoveCorePC(PART_PC *part, f32 time) {
    NUVEC position;
    position.x = part->position.x + part->velocity.x * time;
    position.y = part->position.y + part->velocity.y * time;
    position.z = part->position.z + part->velocity.z * time;
    NuMtxRotateZ(&part->transform, part->rotation_z);
    part->position.x = position.x;
    part->position.y = position.y;
    part->position.z = position.z;
    AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[100].effect, &position, 10, FRAMETIME, 0, 0,
                                      NULL);
}

void __cdecl AtatPart_Stop(PART_PC *part) {
    PlaySfx(KillPartsAtatExplodeTextPc, &part->position);
    PartStop_Flickerer(part);
}

void AtatPart_Update(PART_PC *part) {
    f32 choice = NuFloatRand(((NURAND *)(&GAMERAND))) * 100.0f + 1.0f;
    if (part->scale_time < 1.0f) {
        AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[118].effect, &part->position,
                                          ((i32)(part->scale_time * 20.0f)), FRAMETIME, 0, 0, NULL);
    } else if (part->scale_time < 2.0f) {
        if (choice < 10.0f) {
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[120].effect, &part->position,
                                              ((i32)(part->scale_time * 3.0f)), FRAMETIME, qrand(), qrand(),
                                              NULL);
        } else if (choice > 90.0f) {
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[120].effect, &part->position,
                                              ((i32)(part->scale_time * 15.0f)), FRAMETIME, qrand(), qrand(),
                                              NULL);
        }
    } else if (part->scale_time < 5.0f) {
        if (choice < 10.0f) {
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[120].effect, &part->position, 15,
                                              FRAMETIME, qrand(), qrand(), NULL);
        } else if (choice > 70.0f) {
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[120].effect, &part->position, 30,
                                              FRAMETIME, qrand(), qrand(), NULL);
        }
        if (choice < 30.0f) {
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[119].effect, &part->position, 1,
                                              FRAMETIME, qrand(), qrand(), NULL);
        } else if (choice < 70.0f) {
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[119].effect, &part->position, 3,
                                              FRAMETIME, qrand(), qrand(), NULL);
        } else {
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[119].effect, &part->position, 20,
                                              FRAMETIME, qrand(), qrand(), NULL);
        }
    } else if (part->scale_time < 10.0f) {
        if (choice < 30.0f) {
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[119].effect, &part->position, 1,
                                              FRAMETIME, qrand(), qrand(), NULL);
        } else if (choice < 70.0f) {
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[119].effect, &part->position, 3,
                                              FRAMETIME, qrand(), qrand(), NULL);
        } else {
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[119].effect, &part->position, 20,
                                              FRAMETIME, qrand(), qrand(), NULL);
        }
    }
}
static i32 KillPartsTieSpinRatePc = 200000;

void __cdecl TieSpinZPart_Move(PART_PC *part, f32 time) {
    NUVEC trail;
    part->field_124[3] = KillPartsTieSpinRatePc;
    part->rotation_z = ((u16)(((i32)(((f64)KillPartsTieSpinRatePc) * FRAMETIME))));
    NuMtxPreTranslate(&part->transform, &KillPartsTieSpinTranslationPc);
    TiePart_MoveCorePC(part, time);
    trail.x = part->velocity.x * KillPartsTieTrailScalePc;
    trail.y = part->velocity.y * KillPartsTieTrailScalePc;
    trail.z = part->velocity.z * KillPartsTieTrailScalePc;
    AddVariableShotDebrisEffectTimed3(WORLD->game_debris_sys->entries[96].effect, &part->position, &trail, 10,
                                      FRAMETIME, NULL, NULL);
}

void __cdecl TiePart_Move(PART_PC *part, f32 time) {
    part->field_124[3] = -32768;
    part->rotation_z = ((u16)(((i32)(-32768.0 * FRAMETIME))));
    TiePart_MoveCorePC(part, time);
}

void __cdecl KillParts_TIEFIGHTER(ADDPART_PC *params, i32 part_index, i32 variant, GAMEOBJECT *object, i32 mode,
                                  u16 xrot, u16 yrot, NUVEC *velocity) {
    if (variant != 0)
        params->flags = 0x10;
    else
        params->flags = 0x400;
    if (((f64)object->horizontal_velocity_magnitude) / object->character_data->game_character->run_speed <=
        0.25) {
        params->flags = 0x90;
        params->stop_fn = PartStop_Flickerer;
        params->draw_fn = PartDraw_Flickerer;
        params->impact_fn = PartImpact_Brick;
        params->gravity = -8.0f;
        params->velocity = velocity;
        AddPart(params);
        return;
    }
    switch (mode) {
        default: {
            params->field_20 = 0.5f;
            params->flags = 0x91;
            params->kill_fn = TiePart_Kill;
            params->draw_fn = PartDraw_Flickerer;
            params->impact_fn = TiePart_Impact;
            params->velocity = velocity;
            break;
        }
        case 1: {
            NUVEC spin_velocity;
            params->flags = 0x111;
            spin_velocity.x = object->velocity.x * 0.75f;
            spin_velocity.y = object->velocity.x * 0.75f;
            spin_velocity.z = object->velocity.z * 0.75f;
            if (part_index == 5) {
                NuVecRotateX(&spin_velocity, &spin_velocity, -((i32)yrot));
                NuVecRotateY(&spin_velocity, &spin_velocity, -((i32)xrot));
                params->move_fn = TiePart_Move;
            } else {
                NuVecRotateX(&spin_velocity, &spin_velocity, yrot);
                NuVecRotateY(&spin_velocity, &spin_velocity, xrot);
                params->move_fn = TieSpinZPart_Move;
            }
            params->field_a4 = 2.0f;
            params->kill_fn = TiePart_Kill;
            params->impact_fn = TiePart_Impact;
            params->velocity = &spin_velocity;
            PlaySfx(KillPartsTieSpinsTextPc, &object->collision_position);
            break;
        }
        case 0: {
            params->flags = 0x111;
            params->kill_fn = TiePart_KillExplode;
            params->field_a4 = FRAMETIME;
            params->velocity = &v000;
            break;
        }
    }
    AddPart(params);
}

void __cdecl InitBikeParts(void) {
    memset(KillPartsBikesPc, 0, sizeof(KillPartsBikesPc));
}

static i32 FindFreeBikePart(void) {
    i32 index;
    for (index = 0; index < 4; ++index) {
        if (KillPartsBikesPc[index].active == 0)
            return index;
    }
    return -1;
}

void SpeederPart_Update(PART_PC *part) {
    f32 distance =
        ((GAMEOBJECT *)(Player[0]))->horizontal_velocity_magnitude * FRAMETIME * ((f64)(1.1f));
    f32 time = part->time_remaining / part->initial_time;
    KILLPARTS_BIKE_PC *bike =
        &KillPartsBikesPc[((i32)(((KILLPARTS_PART_PC *)part)->speeder_index))];
    NUVEC *camera = ((NUVEC *)(&GameCam->mtx.m30));
    NUVEC previous_position;
    NUVEC position;
    NUVEC velocity;
    NUMTX matrix;
    if (bike->spline_position.spline != NULL) {
        u16 tilt_angle;
        f32 tilt;
        f32 tilt_magnitude;
        u16 spin;
        u16 yaw, pitch;
        previous_position = bike->spline_position.position;
        MoveSplinePosition(&bike->spline_position, distance);
        PointAlongSpline(bike->spline_position.spline, bike->spline_position.along, &position, &yaw, &pitch, 1);
        yaw -= 0x8000;
        if (distance < 0.0f) {
            yaw += 0x8000;
            pitch = -pitch;
        }
        tilt_angle = ((u16)(((i32)(
            -((1.0f - (NU_SIN_LUT(((i32)(time * 32768.0f + 16384.0f))) + 1.0f) * 0.5f) * 5461.0f)))));
        tilt = NU_SIN_LUT(((i32)(time * 32768.0f + 16384.0f)));
        *((u32 *)(&tilt_magnitude)) = *((u32 *)(&tilt)) & 0x7fffffff;
        spin = ((u16)(
            ((i32)((NU_SIN_LUT(((i32)(time * 16384.0f + 32768.0f + 16384.0f))) + 1.0f) * 196608.0f))));
        part->rotation_x = SeekRot(part->rotation_x, tilt_angle - pitch, 10.0f);
        part->rotation_y =
            SeekRot(part->rotation_y, yaw + ((i32)(2730.0f - (1.0f - tilt_magnitude) * 5461.0f)), 10.0f);
        part->rotation_z = SeekRot(part->rotation_z, spin + yaw, 10.0f);
        GameShadow(NULL, &bike->spline_position.position, 3.0f, 0);
        NuMtxSetIdentity(&matrix);
        NuMtxPreRotateX(&matrix, part->rotation_x);
        NuMtxPreRotateY(&matrix, part->rotation_y);
        NuMtxPreRotateZ(&matrix, part->rotation_z);
        NuMtxTranslate(&matrix, &bike->spline_position.position);
        part->transform = matrix;
        NuVecSub(&velocity, &bike->spline_position.position, &previous_position);
        NuVecScale(&velocity, &velocity, 1.0f / FRAMETIME);
        part->velocity = velocity;
        AddVariableShotDebrisEffectTimed1(
            WORLD->game_debris_sys->entries[96].effect,
            &KillPartsBikesPc[((i32)(((KILLPARTS_PART_PC *)part)->speeder_index))]
                 .spline_position.position,
            5, FRAMETIME, pitch, yaw, NULL);
    } else {
        u16 spin;
        u16 turn;
        union {
            u32 bits;
            f32 value;
        } tilt;
        previous_position.x = part->position.x;
        previous_position.y = part->position.y;
        previous_position.z = part->position.z;
        tilt.value = NU_SIN_LUT(((i32)(time * 32768.0f + 16384.0f)));
        tilt.bits &= 0x7fffffff;
        spin = ((u16)(
            ((i32)((NU_SIN_LUT(((i32)(time * 16384.0f + 32768.0f + 16384.0f))) + 1.0f) * 196608.0f))));
        part->rotation_x = SeekRot(part->rotation_x, 0xd556, 10.0f);
        part->rotation_y =
            SeekRot(part->rotation_y, ((GAMEOBJECT *)(Player[0]))->field_0x276 + 0x4000, 10.0f);
        part->rotation_z = SeekRot(part->rotation_z, spin, 10.0f);
        GameShadow(NULL, &previous_position, 3.0f, 0);
        velocity = v001;
        turn = ((u16)(((i32)(2730.0f - (1.0f - tilt.value) * 5461.0f))));
        NuVecRotateY(&velocity, &velocity, turn);
        NuVecScale(&velocity, &velocity,
                   ((GAMEOBJECT *)(Player[0]))->horizontal_velocity_magnitude / FRAMETIME);
        velocity.y = 1.0f / NU_COS_LUT(part->rotation_x);
        position.x = SeekLinearF(previous_position.x, camera->x, FRAMETIME * ((f64)(1.8f)));
        position.y = SeekLinearF(previous_position.y, camera->y, FRAMETIME * ((f64)(1.8f)));
        position.z = SeekLinearF(previous_position.z, camera->z, FRAMETIME * ((f64)(1.8f)));
        NuVecAdd(&position, &position, &velocity);
        NuMtxSetIdentity(&matrix);
        NuMtxPreRotateX(&matrix, part->rotation_x);
        NuMtxPreRotateY(&matrix, part->rotation_y);
        NuMtxPreRotateZ(&matrix, part->rotation_z);
        NuMtxTranslate(&matrix, &position);
        part->transform = matrix;
        AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[96].effect, &position, 5, FRAMETIME,
                                          part->rotation_z, part->rotation_x, NULL);
    }
    if (bike->rider != NULL) {
        if ((bike->rider->flags_1fc_word & 1) != 0 && (bike->rider->flags_1fc_word & 0x1000) != 0) {
            bike->rider->flags_ef0 |= 0x400000;
            bike->rider->flags_1fc_word |= 0x20;
            bike->rider->flags_ef4 |= 0x20;
            bike->rider_position.x = SeekLinearF(bike->rider_position.x, camera->x, FRAMETIME);
            bike->rider_position.y = SeekLinearF(bike->rider_position.y, camera->y - 2.0f, FRAMETIME * 1.5f);
            bike->rider_position.z = SeekLinearF(bike->rider_position.z, camera->z, FRAMETIME);
            bike->rider->position = bike->rider_position;
            bike->rider->ai_update_position = bike->rider_position;
            bike->rider->velocity = velocity;
            bike->rider->pitch_angle += ((i32)(NuFloatRand(((NURAND *)(&GAMERAND))) * 2730.0f));
            bike->rider->field_0x276 += ((i32)(NuFloatRand(((NURAND *)(&GAMERAND))) * 2730.0f));
            bike->rider->roll_angle += ((i32)(NuFloatRand(((NURAND *)(&GAMERAND))) * 2730.0f));
            ((KILLPARTS_RIDER_LIGHT_PC *)bike->rider)->intensity[0].x = 1.0f;
            ((KILLPARTS_RIDER_LIGHT_PC *)bike->rider)->intensity[0].y = 1.0f;
            ((KILLPARTS_RIDER_LIGHT_PC *)bike->rider)->intensity[0].z = 1.0f;
            ((KILLPARTS_RIDER_LIGHT_PC *)bike->rider)->intensity[1].x = 1.0f;
            ((KILLPARTS_RIDER_LIGHT_PC *)bike->rider)->intensity[1].y = 1.0f;
            ((KILLPARTS_RIDER_LIGHT_PC *)bike->rider)->intensity[1].z = 1.0f;
        } else {
            bike->rider = NULL;
        }
    }
}

i32 SpeederPart_Draw(PART_PC *part) {
    return 1;
}

void __cdecl KillParts_ATAT(ADDPART_PC *params, i32 unused, i32 variant, GAMEOBJECT *object) {
    PART_PC *part;
    params->flags = variant == 0 ? 0x500 : 0x110;
    params->velocity->z = 0.0f;
    params->velocity->y = 0.0f;
    params->velocity->x = 0.0f;
    params->update_fn = AtatPart_Update;
    params->stop_fn = AtatPart_Stop;
    params->draw_fn = PartDraw_Flickerer;
    part = AddPart(params);
    if (part != NULL)
        part->time_remaining = 10.0f;
}

void __cdecl LoseHelmet(GAMEOBJECT *object, i32 unused1, i32 unused2) {
    i32 locator;
    NUVEC velocity;
    ADDPART_PC params;
    (void)unused1;
    (void)unused2;
    if (object->helmet == 0)
        return;
    locator = object->character_data->game_character->helmet_locator;
    if (locator != -1 && object->character_model->points_of_interest[locator] != NULL) {
        velocity.x = velocity.z = 0.0f;
        velocity.y = 1.0f;
        NuVecRotateZ(&velocity, &velocity, qrand());
        NuVecRotateX(&velocity, &velocity, qrand());
        params = Default_ADDPART;
        params.matrix = &object->locator_matrices[locator];
        params.velocity = &velocity;
        params.field_14 = 0.1f;
        params.field_18 = 0.1f;
        params.gravity = -5.0f;
        params.special = &WORLD->level_objects[object->helmet + 0xf9].special;
        params.flags = 0x90;
        params.time_step = FRAMETIME;
        params.field_c4 = 1;
        params.stop_fn = PartStop_Flickerer;
        params.draw_fn = PartDraw_Flickerer;
        params.impact_fn = PartImpact_Brick;
        AddPart(&params);
    }
    object->helmet = 0;
}
#pragma data_seg(".data$SHIELDFLICKERTIME")
static f32 SHIELDFLICKERTIME = 0.10000000149011612f;
#pragma data_seg()

void __cdecl ObjHitShield(GAMEOBJECT *attacker, GAMEOBJECT *target, i32 damage, void *unused_bolt) {
    SHIELD_STATE_PC *shield = (SHIELD_STATE_PC *)target;
    i32 health;
    f32 first_rumble, second_rumble;
    (void)unused_bolt;
    if (target == NULL)
        return;
    if (attacker != NULL && !(attacker->flags_1fc_word & 1))
        attacker = NULL;
    if (shield->shield_enabled == 1.0f && shield->shield_timer > 0.0f) {
        if (attacker != NULL && !(attacker->flags_1fc_word & 0x80) && attacker->character_context != 0x39)
            damage = 0;
    }
    if (attacker != NULL && (attacker->flags_1fc_word & 0x80))
        AlertSurroundingCreatures(attacker, &attacker->collision_position);
    GameAudio_PlaySfx(0x41, &target->collision_position, GameAudio_GetPlrSfxBits(attacker), 0);
    if (target->flags_1fc_word & 0x80)
        TakeHitRumble(target, 0.6f);
    if (MiniCutCam != 0 && (target->flags_1fc_word & 0x80))
        damage = 0;
    if (target->shield_hitpoints > 0 && damage > 0) {
        if ((target->flags_1fc_word & 0x80) && Player_HasInvincibility(target))
            damage = 0;
        health = target->shield_hitpoints - damage;
        if (health <= 0) {
            PlaySfx((char *)ShieldFlickerSfxPc, &target->collision_position);
            shield->shield_timer = 0.5f;
            target->shield_hitpoints = 0;
            shield->shield_enabled = 0.0f;
            first_rumble = 0.8f;
            second_rumble = 0.3f;
        } else {
            PlaySfx((char *)ShieldHitSfxPc, &target->collision_position);
            first_rumble = 0.5f;
            target->shield_hitpoints = (u8)health;
            second_rumble = 0.0f;
            shield->shield_timer = SHIELDFLICKERTIME;
        }
        if (attacker != NULL) {
            if (first_rumble > 0.0f)
                NewRumble(attacker->pad->bound_pad, first_rumble, 0);
            if (second_rumble > 0.0f)
                NewRumble(attacker->pad->bound_pad, second_rumble, 0);
        }
        return;
    }
    if (attacker != NULL)
        NewBuzz(attacker->pad->bound_pad, 0.1f, 0);
}

GAMEOBJECT *__cdecl TargetGameObject(GAMEOBJECT *object, NUVEC *position, NUVEC *direction, f32 radius,
                                     f32 range_squared, u32 model_mask, i32 directional, i32 require_drawn,
                                     i32 bolt_id) {
    NUVEC aim = *direction;
    BOLTTYPE_PC *bolt = BoltType_FindByID(bolt_id, WORLD);
    u16 near_angle = TargetDeg_Near;
    u16 mid_angle = TargetDeg_Mid;
    u16 far_angle = TargetDeg_Far;
    f32 near_distance = TargetDist_Near2;
    f32 mid_distance = TargetDist_Mid2;
    f32 min_x, max_x, min_z, max_z;
    i32 arcade;
    GAMEOBJECT *target;
    GAMEOBJECT *best = NULL;
    GAMEOBJECT *previous = NULL;
    i32 i;
    if ((object->flags_1fc & 0x80) != 0 &&
        (WORLD->current_level == DEATHSTAR2BATTLEA_LDATA || WORLD->current_level == ASTEROIDCHASED_LDATA)) {
        near_angle = 0x2aaa;
        mid_angle = 0x1c71;
        far_angle = 0xe38;
        near_distance = 6.25f;
        mid_distance = 100.0f;
    }
    min_x = position->x - radius;
    min_z = position->z - radius;
    max_x = radius + position->x;
    max_z = radius + position->z;
    if (directional != 0) {
        aim = *direction;
        if (bolt != NULL && (bolt->flags_60 & 0x20000) != 0) {
            aim.y = 0.0f;
            NuVecNorm(&aim, &aim);
        }
    }
    arcade = Arcade_GetMode(NULL) == 0;
    target = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++target) {
        i8 context;
        NUVEC delta;
        f32 distance;
        if (target == object || (target->flags_1fc_word & 0x1000) == 0 || target->dead != 0 ||
            (require_drawn != 0 && target->model_draw_result == 0) || (target->flags_ef4 & 1) != 0) {
            continue;
        }
        context = target->character_context;
        if (context == 0x3b || context == 0x39 || context == 0x3c || (U64_PAIR(&CInfo[context].flags) & 0x8000) != 0) {
            continue;
        }
        if (model_mask != 0) {
            if ((target->character_data->model_flags & model_mask) == 0) {
                continue;
            }
        } else if (arcade == 0 && ((target->flags_1f8 ^ object->flags_1f8) & 0x10001) == 0) {
            continue;
        }
        if (context == 0x5f || context == 0x60) {
            continue;
        }
        if (arcade == 0 && (object->flags_1fc & 0x80) != 0 && (target->flags_ef4 & 0x800000) == 0) {
            if ((((u64)1 << target->hostility_group) & WORLD->object_sys->hostility_masks[object->hostility_group]) ==
                0) {
                continue;
            }
        }
        if (target->collision_position.x < min_x || target->collision_position.x > max_x ||
            target->collision_position.z < min_z || target->collision_position.z > max_z) {
            continue;
        }
        if (WORLD->current_level == DEATHSTARRESCUEA_LDATA && target->field_280 == 0xff && target->position.y < -1.5f &&
            GameCam->sock_position.sock_index == 2) {
            continue;
        }
        distance = NuVecDistSqr(&target->collision_position, position, &delta);
        if (distance < range_squared) {
            f32 dot;
            u16 angle;
            if (directional == 0) {
                NuVecRotateY(
                    &aim, &v001,
                    NuAtan2D(target->collision_position.x - position->x, target->collision_position.z - position->z));
            }
            if (bolt != NULL && (bolt->flags_60 & 0x20000) != 0) {
                delta.y = 0.0f;
            }
            NuVecNorm(&delta, &delta);
            dot = NuVecDot(&delta, &aim);
            angle = near_distance > distance && directional != 0 ? near_angle
                                                                 : (mid_distance > distance ? mid_angle : far_angle);
            if (dot > NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff]) {
                if (target == object->collision_target) {
                    previous = target;
                } else {
                    range_squared = distance;
                    best = target;
                }
            }
        }
    }
    return best != NULL ? best : previous;
}

void __cdecl ClearLastSafeTakeOver(GAMEOBJECT *object) {
    i32 index;
    GAMEOBJECT *candidate;
    if (object == NULL || (object->portal_flags & 0x100000) != 0)
        return;
    for (index = 0; index < HIGHGAMEOBJECT; ++index) {
        candidate = &Obj[index];
        if (candidate != NULL && (candidate->flags_1fc_word & 1) && (candidate->flags_1fc_word & 0x1000) &&
            candidate->takeover_owner == object)
            candidate->takeover_owner = NULL;
    }
}

i32 __cdecl Tag_UpdateHint(HINT_PC *hint) {
    WORLDINFO *world = WORLD;
    u32 hint_flags;
    i16 tc14_id;
    i16 hint_id;
    i32 i;
    if (world->area != NULL && world->area == HUB_ADATA)
        return 0;
    hint_flags = LswForceUsedHintFlagsPc;
    tc14_id = id_TC14;
    for (i = 0; i < 8; ++i) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[i];
        if (object != NULL) {
            u32 flags = object->flags_1fc_word;
            if (PANEL_PLAYER_ACTIVE(flags) && (i16)object->id == tc14_id) {
                hint_flags |= 2;
                LswForceUsedHintFlagsPc = hint_flags;
            }
        }
    }
    hint_id = hint->id;
    switch (hint_id) {
        case 602: {
            GAMEOBJECT *first;
            GAMEOBJECT *second;
            i32 total;
            i32 controlled;
            if ((hint_flags & 1) == 0)
                break;
            if (Tag_DoneFirst != 0) {
                Hint_SetCompletePc(hint);
                return 0;
            }
            if (VehicleArea != 0 || world->current_level == HUB_LDATA)
                break;
            first = player;
            if (first != NULL && (first->flags_1fc & 0x80) != 0 && first->takeover_source != NULL)
                break;
            second = player2;
            if (second != NULL && (second->flags_1fc & 0x80) != 0 && second->takeover_source != NULL)
                break;
            controlled = 0;
            total = 0;
            for (i = 0; i < 8; ++i) {
                GAMEOBJECT *object = (GAMEOBJECT *)Player[i];
                if (object != NULL) {
                    u32 flags = object->flags_1fc_word;
                    ++total;
                    if (PANEL_PLAYER_ACTIVE(flags))
                        ++controlled;
                }
            }
            if (total == 2 && controlled == total)
                break;
            if ((first != NULL && first->takeover_source == NULL) ||
                (second != NULL && second->takeover_source == NULL))
                return 1;
            break;
        }
        case 603: {
            HINT_PC *previous;
            if ((hint_flags & 4) != 0) {
                previous = Hint_FindHint(602);
                if (Tag_DoneFirst == 2) {
                    Hint_SetCompletePc(hint);
                    return 0;
                }
                if (Tag_DoneFirst != 1)
                    break;
                if (previous != NULL && Hint_isComplete(previous) == 0)
                    break;
                if (VehicleArea != 0 || WORLD->current_level == HUB_LDATA)
                    break;
                if ((player != NULL && player->takeover_source == NULL) ||
                    (player2 != NULL && player2->takeover_source == NULL))
                    return 1;
                break;
            }
            if (Tag_DoneFirst > 1) {
                Tag_DoneFirst = 1;
                return 0;
            }
            break;
        }
        case 600:
            if (FreePlay == 0)
                break;
            if (player != NULL && (player->flags_1fc & 0x80) != 0 && player->takeover_source != NULL)
                break;
            if (player2 != NULL && (player2->flags_1fc & 0x80) != 0 && player2->takeover_source != NULL)
                break;
            return 1;
        case 606: {
            GAMEOBJECT *first = (GAMEOBJECT *)Player[0];
            GAMEOBJECT *second;
            i16 dagobah_id = id_LUKESKYWALKERDAGOBAH;
            if (first == NULL || (first->flags_1fc & 0x80) == 0 || first->takeover_source == NULL ||
                (i16)first->id == dagobah_id) {
                second = (GAMEOBJECT *)Player[1];
                if (second == NULL || (second->flags_1fc & 0x80) == 0 || second->takeover_source == NULL ||
                    (i16)second->id == dagobah_id)
                    break;
            }
            if (world->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks == 0)
                break;
            return 1;
        }
        case 605:
        case 611:
        case 650: {
            i32 count = HIGHGAMEOBJECT;
            GAMEOBJECT *object = Obj;
            i16 grab_id;
            i16 grab_r2_id;
            i = 0;
            if (count <= 0)
                break;
            grab_id = id_GRABCONTROL;
            grab_r2_id = id_GRABR2CONTROL;
            do {
                if ((object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 && object->dead == 0 &&
                    (object->flags_e20 & 0x80000000u) != 0) {
                    if ((i16)object->id == grab_id || (i16)object->id == grab_r2_id) {
                        if (hint_id == 650)
                            return 1;
                    } else if (object->character_data->move_fn == Move_BEAST) {
                        if (hint_id == 611)
                            return 1;
                    } else if (hint_id == 605) {
                        return 1;
                    }
                }
                ++i;
                ++object;
            } while (i < count);
            return 0;
        }
        case 651: {
            GAMEOBJECT *first = (GAMEOBJECT *)Player[0];
            GAMEOBJECT *second;
            i16 grab_id = id_GRABCONTROL;
            i16 grab_r2_id = id_GRABR2CONTROL;
            if ((first != NULL && (first->flags_1fc & 0x80) != 0 &&
                 ((i16)first->id == grab_id || (i16)first->id == grab_r2_id)) ||
                ((second = (GAMEOBJECT *)Player[1]) != NULL && (second->flags_1fc & 0x80) != 0 &&
                 ((i16)second->id == grab_id || (i16)second->id == grab_r2_id)))
                return 1;
            break;
        }
        case 654:
            if (VehicleArea == 0)
                break;
            {
                i32 found = 0;
                for (i = 0; found == 0 && i < 2; ++i) {
                    GAMEOBJECT *object = (GAMEOBJECT *)Player[i];
                    if (object != NULL && (object->flags_1fc & 0x80) != 0 && object->dead == 0 &&
                        (object->flags_e24 & 0x10) != 0)
                        found = 1;
                }
                if (found != 0)
                    return 1;
            }
            break;
        case 649: {
            GAMEOBJECT *object;
            TORPEDOPACKET_PC *packet;
            if (VehicleArea == 0)
                break;
            object = (GAMEOBJECT *)Player[0];
            if (object != NULL) {
                packet = object->torpedo;
                if (packet != NULL && packet->count > 0 && packet->target != NULL)
                    return 1;
            }
            object = (GAMEOBJECT *)Player[1];
            if (object == NULL)
                break;
            packet = object->torpedo;
            if (packet != NULL && packet->count > 0 && packet->target != NULL)
                return 1;
            break;
        }
        case 647: {
            GAMEOBJECT *first;
            GAMEOBJECT *second;
            i16 speeder_id;
            if (world->current_level != SPEEDERCHASEA_LDATA)
                break;
            first = (GAMEOBJECT *)Player[0];
            speeder_id = id_SPEEDERBIKE;
            if (first != NULL && (i16)first->id == speeder_id)
                return 1;
            second = (GAMEOBJECT *)Player[1];
            if (second != NULL && (i16)second->id == speeder_id)
                return 1;
            break;
        }
        default:
            break;
    }
    return 0;
}

/* PC434930227bytes,cdecl(model,object,buffer). Native name trim scans fixed14..0. */
char *GameObj_GetNamePc(i32 model, GAMEOBJECT *object, char *buffer) {
    if (object != NULL) {
        if (object->rider != NULL && object->rider->character_data->name_id != -1)
            model = (i16)object->rider->id;
        else
            model = (i16)object->id;
    } else if (model == -1)
        return TTab[tUNKNOWN];
    if (buffer != NULL) {
        i32 custom = -1;
        if (model == PermanentWeirdo1IdPc) {
            if (Game.customiser_start[0].use_saved_name != 0)
                custom = 0;
        } else if (model == PermanentWeirdo2IdPc) {
            if (Game.customiser_start[1].use_saved_name != 0)
                custom = 1;
        }
        if (custom != -1) {
            i32 i;
            NuStrCpy(buffer, Game.customiser_start[custom].name);
            for (i = 14; i >= 0 && buffer[i] == ' '; --i)
                buffer[i] = 0;
            if (i < 0)
                NuStrCpy(buffer, "?");
            return buffer;
        }
    }
    return TTab[CDataList[model].name_id];
}
#pragma bss_seg(".bss$player_tag_from")
GAMEOBJECT *player_tag_from = 0;
#pragma bss_seg()

void __cdecl UpdateGameObjects(WORLDINFO *world) {
    i32 i;
    i32 setting;
    GAMEOBJECT *object;
    u32 interval;
    PLAYERCHARACTERCONFIG_PC *configuration;
    NUVEC delta;
    f32 distance;
    i32 tier;
    f32 frame_time;
    f32 flicker_decay;
    f32 vertical_displacement;
    f32 vertical_velocity;
    f32 elapsed;
    i32 lighting_start;
    GAMEOBJECT *dagobah_luke;
    NUVEC offset;
    i32 yoda_proximity;
    f32 target;
    i32 result;
    {
        Hub_PadSpeed[0] = Hub_PadSpeed[1] = 0.0f;
        SetPlayer();
        AIFireIntervalMul = 1.0f;
        CarWashHack = NULL;
        if (Cheat_IsOn(0x2c) != 0) {
            setting = adtab[adaptivedifficulty.level][3];
            if (setting == 1)
                AIFireIntervalMul = (f64)AIFireIntervalMul * 0.5;
            else if (setting == -1)
                AIFireIntervalMul = (f64)AIFireIntervalMul * 2.0;
        }
        GhostLightMul = SeekLinearF(GhostLightMul, (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 0.5 + 0.75),
                                    (f32)((f64)FRAMETIME * 2.0));
        if (GhostLightMul == GhostLightTargetMul)
            GhostLightTargetMul = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 0.5 + 0.75);
        object = Obj;
        for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
            if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0)
                continue;
            if (0 != (object->flags_ef4 & 8))
                object->ai_elapsed_time = 0.0f;
            object->ai_elapsed_time += FRAMETIME;
            interval = 1;
            if (timebase_updates != 0 && (object->flags_1f8 & 0x400) != 0) {
                if ((object->portal_flags & 0x8000000) != 0 || (0x20 & object->flags_ef4) != 0 ||
                    0 != (U64_PAIR(&CInfo[(i8)object->character_context].flags) & 0x200) ||
                    object->supporting_platform_id != -1 ||
                    (object->rider != NULL && object->character_context == 0x3b)) {
                    object->flags_ef4 |= 0x10;
                    interval = 1;
                } else if (Technos_FindControllingTechno(object) != NULL) {
                    object->flags_ef4 |= 0x10;
                    interval = 1;
                } else if (object->active_trigger_set != NULL && (object->active_trigger_set->flags_low & 2) != 0) {
                    object->flags_ef4 |= 0x10;
                    interval = 1;
                } else if (object->ai_packet.group != NULL && 0 != (object->ai_packet.group->flags & 8)) {
                    object->flags_ef4 |= 0x10;
                    interval = 1;
                } else if (world->rooms_visible_ptr[object->portal_room] == 0 || object->model_draw_result == 0) {
                    interval = 10;
                    object->flags_ef4 &= ~0x10u;
                } else if (object->field_281 == 0) {
                    object->flags_ef4 |= 0x10;
                    interval = 1;
                } else {
                    distance = NuVecDistSqr(&object->collision_position, &GameCam->position, &delta);
                    configuration = object->character_data->game_character;
                    for (tier = 0; tier < 4; ++tier) {
                        if (configuration->ai_update_intervals[tier] == 0)
                            break;
                        if (configuration->ai_update_distances[tier] * configuration->ai_update_distances[tier] <
                            distance) {
                            interval = configuration->ai_update_intervals[tier];
                            break;
                        }
                    }
                    if (interval == 1)
                        object->flags_ef4 |= 0x10;
                    else
                        object->flags_ef4 &= ~0x10u;
                }
            } else {
                object->flags_ef4 |= 0x10;
            }
            if (((u32)object->hostility_group + (u32)GameTimer.update_count) % interval == 0) {
                object->flags_ef4 |= 8;
                if ((object->flags_1f8 & 0x400) != 0 && (object->flags_1f8 & 0x40000) == 0)
                    object->position = object->ai_update_position;
            } else {
                object->flags_ef4 &= ~8u;
            }
        }
    }
    GameAntinode_Update(world->game_antinode_sys);
    GameAIProcess();

    // AI movement precedes the separate player movement pass.
    object = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 ||
            (object->flags_1f8 & 0x400) == 0)
            continue;
        object->flags_1fc_word ^= (((object->character_context == 0) << 14) ^ object->flags_1fc_word) & 0x4000;
        flicker_decay = object->vehicle_hit_timer - FRAMETIME;
        object->vehicle_hit_timer = flicker_decay;
        if (flicker_decay < -1.0f)
            SetFlicker(object, 0.0f);
        if (object->timer_d34 > 0.0f)
            object->timer_d34 -= FRAMETIME;
        if (object->field_d54 > 0.0f)
            object->field_d54 -= FRAMETIME;
        if ((object->flags_1f8 & 0x40000) != 0) {
            MovePlayer_NETWORK(object);
        } else if ((object->flags_ef4 & 8) != 0) {
            frame_time = FRAMETIME;
            FRAMETIME = object->ai_elapsed_time;
            object->flags_e20 &= ~0x20;
            if (object->spawn_protection_timer > 0.0f)
                object->spawn_protection_timer -= FRAMETIME;
            object->powerup_time = 0.0f;
            if (object->move_override != NULL)
                object->move_override(object);
            else if (object->movement_spline_position.spline != NULL)
                MovePlayerSpline(object);
            else
                MovePlayer(object);
            if ((object->portal_flags & 0x400000) != 0 && (object->flags_1fc_word & 4) == 0) {
                object->portal_room = -1;
                if ((object->portal_flags & 0x800000) != 0 && world->current_gscn != NULL)
                    object->portal_room = NuPortalWhichRoom(world->current_gscn, &object->collision_position);
            }
            FRAMETIME = frame_time;
        }
        if (object->character_data->game_character->weapon_action == 10) {
            if (object->model_draw_result != 0) {
                if ((object->flags_ef4 & 0x100) == 0 && tieonsfxwait <= 0.0f) {
                    PlaySfx("veh_tie_by", &object->collision_position);
                    tieonsfxwait = ((f64)qrand() * ((f64)(1.0f / 65535.0f))) * 0.5f + 0.5f;
                }
            } else if ((object->flags_ef4 & 0x100) != 0 && tieoffsfxwait <= 0.0f) {
                PlaySfx("veh_tie_by", &object->collision_position);
                tieoffsfxwait = ((f64)qrand() * ((f64)(1.0f / 65535.0f))) * 0.5f + 0.5f;
            }
        }
    }

    // Player movement has its own timers, power-up and controller processing.
    for (i = 0; i < 8; ++i) {
        object = Player[i];
        if (object == NULL || (object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0) {
            continue;
        }
        object->flags_e20 ^= (((VehicleArea != 0 && (object->field_280 == 0 || object->field_280 == 1) &&
                                (object->flags_1fc_word & 0x80) == 0)
                               << 5) ^
                              object->flags_e20) &
                             0x20;
        if (object->hud_icon_timer > 0.0f)
            object->hud_icon_timer -= FRAMETIME;
        if (object->tag_cooldown > 0.0f)
            object->tag_cooldown -= FRAMETIME;
        if (object->spawn_protection_timer > 0.0f)
            object->spawn_protection_timer -= FRAMETIME;
        PowerUp_Update(object);
        if ((object->flags_1fc_word & 0x80) != 0 && (Cheats_CheckFlags(0x1000) != 0 || object->powerup_time > 0.0f))
            RegenerateHearts(object);
        object->flags_ef4 &= ~0x10000000u;
        flicker_decay = 1.0f;
        if (object->vehicle_hit_timer > 0.0f && (object->flags_1fc_word & 0x80) != 0 && Cheat_IsOn(0x2c) != 0) {
            setting = adtab[adaptivedifficulty.level][2];
            if (setting == 1)
                flicker_decay = 2.0f;
            else if (setting == -1)
                flicker_decay = 0.5f;
        }
        object->vehicle_hit_timer -= FRAMETIME * flicker_decay;
        if (object->vehicle_hit_timer < -1.0f)
            SetFlicker(object, 0.0f);
        if (object->timer_d34 > 0.0f)
            object->timer_d34 -= FRAMETIME;
        if (object->field_d54 > 0.0f)
            object->field_d54 -= FRAMETIME;
        object->flags_1fc_word ^= (((object->character_context == 0) << 14) ^ object->flags_1fc_word) & 0x4000;
        if ((object->flags_1f8 & 0x40000) != 0) {
            MovePlayer_NETWORK(object);
        } else {
            if ((object->flags_1fc_word & 0x80) != 0) {
                if (object->pad->bound_pad != NULL) {
                    UpdateRumble(&object->pad->rumble);
                    Player_ToggleCharacter(object, 0, 1);
                }
            } else if (FreePlay != 0) {
                Player_ToggleCharacter(object, 0, 0);
            }
            if (object->move_override != NULL)
                object->move_override(object);
            else if (object->movement_spline_position.spline != NULL)
                MovePlayerSpline(object);
            else
                MovePlayer(object);
        }
        if ((object->portal_flags & 0x400000) != 0 && (object->flags_1fc_word & 4) == 0) {
            object->portal_room = -1;
            if ((object->portal_flags & 0x800000) != 0 && world->current_gscn != NULL)
                object->portal_room = NuPortalWhichRoom(world->current_gscn, &object->collision_position);
        }
    }
    CurrentSpeedOverride = -1000.0f;
    AveragePlayerCurrentSpeedMul();

    // AI-controlled characters use their accumulated AI interval for a full
    // terrain update when their script was processed. Between those updates,
    // the original advances the last resolved velocity and still animates the
    // object every frame.
    object = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 ||
            (object->flags_1f8 & 0x400) == 0) {
            object->portal_flags &= ~1;
            if (object->timer_ecc > 0.0f)
                object->timer_ecc -= FRAMETIME;
            continue;
        }
        if ((object->flags_ef4 & 8) != 0) {
            frame_time = FRAMETIME;
            FRAMETIME = object->ai_elapsed_time;
            TerrainPlayer(object);

            object->ai_update_position = object->position;

            vertical_displacement = object->position.y - object->start_position.y;
            elapsed = object->ai_elapsed_time;
            if (vertical_displacement == 0.0f || elapsed == 0.0f)
                vertical_velocity = 0.0f;
            else
                vertical_velocity = vertical_displacement / elapsed;
            object->ai_vertical_velocity = vertical_velocity;
            FRAMETIME = frame_time;
        } else {
            PreResetCode(object);
            PostResetCode(object);
            if ((object->flags_1f8 & 0x40000) == 0) {
                object->position.x += object->velocity.x * FRAMETIME;
                object->position.y += object->ai_vertical_velocity * FRAMETIME;
                object->position.z += object->velocity.z * FRAMETIME;
            } else {
                GameObjectOrigin(object);
            }
        }

        AnimatePlayer(object);
        object->context_target_position = NULL;
        if (world->sock_sys != NULL && (object->portal_flags & 0x4000) != 0) {
            ComplexSockPosition(world->sock_sys, &object->position, (i8)object->sock_position.sock_index,
                                object->sock_position.segment, &object->sock_position);
            ComplexSockAngles(&object->sock_angles);
        } else {
            object->sock_position.segment = -1;
            object->sock_position.sock_index = -1;
        }
        if (WORLD->current_level != SPEEDERCHASEA_LDATA || disable_narrow_socks != 0 ||
            object->id != (u16)id_SPEEDERBIKE || object->dead == 0)
            UpdateLastSafePosition(object);
        object->collision_scale = object->dead == 1 ? 1.0f - object->death_timer / object->death_delay : 1.0f;
        if (object->collision_scale_override != 1000000000.0)
            object->collision_scale = object->collision_scale_override;
        object->origin_scale = object->character_data->model_scale * object->collision_scale;
        ScaleGameObject(object);
        GameObjectDimensions(object);
        AddSurfaceRipples(object);
        if (object->use_model_origin != 0xff) {
            ++object->use_model_origin;
        }
        NuCameraTransformScreenClip(&object->camera_screen_position, &object->collision_position, 1, NULL);
        GameObjectToCameraCode(object);
        object->portal_flags &= ~1;
        if (object->timer_ecc > 0.0f)
            object->timer_ecc -= FRAMETIME;
    }

    CurrentSpeed = 0.0f;
    for (i = 0; i < 8; ++i) {
        object = Player[i];
        if (object == NULL || (object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0) {
            continue;
        }

        if (VehicleArea != 0 || (object->character_data->model_flags & 0x2000) != 0) {
            object->reset_velocity = object->velocity;
            object->pre_terrain_speed = NuFsqrt(object->reset_velocity.x * object->reset_velocity.x +
                                                object->reset_velocity.y * object->reset_velocity.y +
                                                object->reset_velocity.z * object->reset_velocity.z);
        }
        TerrainPlayer(object);
        object->ai_update_position = object->position;
        vertical_displacement = object->position.y - object->start_position.y;
        elapsed = object->ai_elapsed_time;
        if (vertical_displacement == 0.0f || elapsed == 0.0f)
            vertical_velocity = 0.0f;
        else
            vertical_velocity = vertical_displacement / elapsed;
        object->ai_vertical_velocity = vertical_velocity;
        AnimatePlayer(object);
        object->context_target_position = NULL;
        if (VehicleArea != 0 || (object->character_data->model_flags & 0x2000) != 0) {
            object->post_terrain_speed =
                NuFsqrt(object->velocity.x * object->velocity.x + object->velocity.y * object->velocity.y +
                        object->velocity.z * object->velocity.z);
        }
        if (object->previous_sock_position != NULL)
            *object->previous_sock_position = object->sock_position;
        if (world->sock_sys != NULL &&
            ((object->flags_1fc_word & 0x80) != 0 || VehicleArea != 0 || world->current_level == UpdateSockets_LDATA)) {
            ComplexSockPosition(world->sock_sys, &object->position, (i8)object->sock_position.sock_index,
                                object->sock_position.segment, &object->sock_position);
            ComplexSockAngles(&object->sock_angles);
        } else {
            object->sock_position.segment = -1;
            object->sock_position.sock_index = -1;
            if (object->previous_sock_position != NULL) {
                object->previous_sock_position->segment = -1;
                object->previous_sock_position->sock_index = -1;
            }
        }
        if (object == Player[0]) {
            if (object->sock_position.sock_index != -1)
                CurrentSpeed = world->sock_sys->socks[(i8)object->sock_position.sock_index].current_speed;
            BaseCurrentSpeed = CurrentSpeed;
            if (CurrentSpeedOverride != -1000.0f)
                CurrentSpeed = CurrentSpeedOverride;
        }
        UpdateLastSafePosition(object);
        if (object->sock_position.sock_index != -1) {
            plr_lastpos = object->start_position;
            if (object->previous_sock_position != NULL && object->previous_sock_position->sock_index == -1) {
                *object->previous_sock_position = object->sock_position;
            }
        } else {
            plr_lastpos = object->position;
        }
        KeepOnScreen(object);
        Doors_Check(world, object);
        object->collision_scale = object->dead == 1 ? 1.0f - object->death_timer / object->death_delay : 1.0f;
        if (FreePlay != 0 && WORLD->area != NULL && WORLD->area == PODSPRINT_ADATA &&
            object->id != (u16)id_ANAKINSNEWPOD && object->id != (u16)id_ANAKINSNEWPODGREEN &&
            object->id != (u16)id_SEBULBASPOD) {
            object->collision_scale *= (f64)0.55f;
        }
        if (object->collision_scale_override != 1000000000.0)
            object->collision_scale = object->collision_scale_override;
        object->origin_scale = object->character_data->model_scale * object->collision_scale;
        ScaleGameObject(object);
        GameObjectDimensions(object);
        if ((object->flags_1fc_word & 0x80) != 0 && FadeSys.fade == 0.0f && InDoubleScoreZone(object) != 0) {
            DoubleScore |= (i32)(1u << ((u8)object->field_280 & 31));
        }
        UpdateCoinPacket((PLAYER_COIN_PACKET_PC *)object->coin_packet, (object->flags_1fc_word >> 7) & 1,
                         (i8)object->field_280);
        if ((object->flags_1fc_word & 0x80) != 0 && object->dead == 0 && object->character_context != 0x2b &&
            !(object->character_context == 0x0f && object->field_7b3 == 1)) {
            GizmoPickups_Collide(world, object, 1);
        } else {
            ResetCoinPacket(object->coin_packet);
        }
        AddSurfaceRipples(object);
        if ((object->character_data->model_flags & 0x2000) != 0 && object->dead == 0 &&
            object->model_draw_result != 0 && (object->flags_e20 & 0x20) == 0) {
            AddSurfaceDebris(object);
        }
        if (object->use_model_origin != 0xff) {
            ++object->use_model_origin;
        }
        NuCameraTransformScreenClip(&object->camera_screen_position, &object->collision_position, 1, NULL);
        GameObjectToCameraCode(object);
    }

    if (MainFrameCounters.third_frame == -1)
        lighting_start = 0;
    else
        lighting_start = (MainFrameCounters.third_frame * HIGHGAMEOBJECT) / 3;
    dagobah_luke = FindGameObject(id_LUKESKYWALKERDAGOBAH, 0, 1, 1, 0);
    for (i = 0; i < HIGHGAMEOBJECT; ++i) {
        object = &Obj[i];
        if (world->rooms_visible_ptr[object->portal_room] != 0 && (object->flags_1fc_word & 0x1001) == 0x1001) {
            yoda_proximity = StartupGameFlag1 != 0 ? TakeOverYodaSeekDistanceHack(object, dagobah_luke, &offset) : 0;
            target = (object->flags_e20 & 0x80000000) != 0 || yoda_proximity != 0 ? 1.0f : 0.0f;
            object->arrow_blend = SeekLinearF(object->arrow_blend, target, FRAMETIME + FRAMETIME);
            if (object->model_draw_result == 0)
                continue;
            if (i >= lighting_start)
                LightGameObject(object, world->rtl_set);
            if (object->arrow_blend > 0.0f) {
                NUVEC position;
                ADD_GAME_MESSAGE_PC message;
                char text[128];
                i32 joint = object->character_data->game_character->ride_locator;
                f32 height;
                f32 amplitude;
                u16 base;
                u16 phase;
                if (joint != -1 && object->character_model->points_of_interest[joint] != NULL) {
                    position = *(NUVEC *)&object->locator_matrices[joint].m30;
                } else {
                    position = object->upper_position;
                }
                height = (f32)((f64)object->bounds_half_height * 0.5);
                if (height < (f64)0.21f)
                    height = 0.21f;
                position.y = position.y + height;
                amplitude = (f32)((f64)object->bounds_radius * (f64)0.05f);
                base = (u16)((object->hostility_group & 7) << 13);
                position.x =
                    NuTrigTable[((base + (u16)(i32)(NuFmod(GameTimer.time_elapsed, 1.5f) / 1.5 * 65536.0)) >> 1) &
                                0x7fff] *
                        amplitude +
                    position.x;
                position.y =
                    NuTrigTable[((base + (u16)(i32)(NuFmod(GameTimer.time_elapsed, 1.174f) / (f64)1.174f * 65536.0)) >>
                                 1) &
                                0x7fff] *
                        (amplitude * 0.75) +
                    position.y;
                position.z =
                    NuTrigTable[((base + (u16)(i32)(NuFmod(GameTimer.time_elapsed, 1.259f) / (f64)1.259f * 65536.0)) >>
                                 1) &
                                0x7fff] *
                        amplitude +
                    position.z;
                NuStrCpy(text, TagAsciiDownPc);
                message = AddGameMsg_Default;
                message.scale = InteractionArrowScalePc;
                message.text = text;
                message.position = &position;
                phase = (u16)(i32)(((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0) * 65536.0);
                message.alpha = (u8)(i32)(((f64)NuTrigTable[phase >> 1] * 24.0 + 104.0) * object->arrow_blend);
                message.flags = 0x1083;
                message.unknown_4f = 4;
                AddGameMsg(&message);
            }
        }
    }
    CollideGameObjects(world);
    if (do_player_tag != 0) {
        result = 0;
        if (player_tag_to != NULL && player_tag_from != NULL && player_tag_to != player_tag_from &&
            (player_tag_to->flags_1fc_word & 0x1001) == 0x1001 && player_tag_to->dead == 0 &&
            (player_tag_to->tag_flags & 2) == 0 && (player_tag_from->flags_1fc_word & 0x1001) == 0x1001 &&
            player_tag_from->dead == 0 && (2 & player_tag_from->tag_flags) == 0) {
            result = TagCode(player_tag_to, player_tag_from, 0, 0, 1);
            if (result == 1) {
                GameAudio_PlaySfx(0x22, &player_tag_to->collision_position, 0, 0);
                GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
                RememberPlayerIDs(0, Player[0] != NULL ? (i16)((GAMEOBJECT *)Player[0])->id : -1,
                                  Player[1] != NULL ? (i16)((GAMEOBJECT *)Player[1])->id : -1);
                player_tag_to->hud_icon_timer = 2.0f;
                player_tag_from->hud_icon_timer = 2.0f;
            }
        }
        if (result != 2) {
            player_tag_to = NULL;
            player_tag_timer = 0.0f;
            player_tag_from = NULL;
            do_player_tag = 0;
        }
    } else if (player_tag_timer > 0.0f) {
        player_tag_timer -= FRAMETIME;
        if (player_tag_timer < 0.0f || player_tag_to == NULL || player_tag_from == NULL ||
            0x1001 != ((player_tag_to->flags_1fc_word & 0x1001)) || player_tag_to->dead != 0 ||
            (player_tag_to->tag_flags & 2) != 0 || (player_tag_from->flags_1fc_word & 0x1001) != 0x1001 ||
            player_tag_from->dead != 0 || (player_tag_from->tag_flags & 2) != 0) {
            player_tag_to = NULL;
            player_tag_timer = 0.0f;
            player_tag_from = NULL;
            do_player_tag = 0;
        }
    }
    {
        GAMEOBJECT *player0 = Player[0];
        GAMEOBJECT *player1 = Player[1];
        i32 show[2];
        show[1] = show[0] = 0;
        if ((VehicleArea != 0 || (WORLD->area != NULL && (WORLD->area->flags & 0x104) == 4) ||
             (WORLD->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks == 0)) &&
            player0 != NULL && (player0->flags_1fc_word & 0x80) != 0 && player1 != NULL &&
            (player1->flags_1fc_word & 0x80) != 0) {
            show[1] = show[0] = 1;
        } else {
            if (player0 != NULL && (player0->flags_1fc_word & 0x80) != 0 && (player0->flags_e24 & 8) != 0 &&
                player0->rider != NULL &&
                (player0->id == GameObjectRotationAtstId || player0->id == (u16)id_ATST_LOWRES ||
                 player0->id == (u16)id_ATAT)) {
                show[0] = 2;
            }
            if (player1 != NULL && (player1->flags_1fc_word & 0x80) != 0 && (player1->flags_e24 & 8) != 0 &&
                player1->rider != NULL &&
                (player1->id == GameObjectRotationAtstId || player1->id == (u16)id_ATST_LOWRES ||
                 player1->id == (u16)id_ATAT)) {
                show[1] = 2;
            }
        }
        for (i = 0; i < 2; ++i) {
            NUVEC position;
            ADD_GAME_MESSAGE_PC message;
            u16 phase;
            object = Player[i];
            if (object == NULL || show[i] == 0 || object->dead != 0) {
                continue;
            }
            if (object->hud_icon_timer > 0.0f && VehicleArea == 0) {
                continue;
            }
            if (show[i] == 2) {
                i32 joint = object->character_data->game_character->ride_locator;
                if (joint != -1 && object->character_model->points_of_interest[joint] != NULL) {
                    position = *(NUVEC *)&object->locator_matrices[joint].m30;
                } else {
                    position = object->upper_position;
                }
                position.y = object->bounds_half_height * 0.5 + position.y;
            } else if (world->current_level == DOGFIGHTA_LDATA) {
                position.x = 0.0f;
                position.y = object->character_top * object->origin_scale;
                position.z = 0.0f;
                NuVecMtxRotate(&position, &position, &object->vehicle_orientation);
                NuVecAdd(&position, &position, &object->position);
            } else {
                position.x = object->collision_position.x;
                position.y = object->bounds_half_height * 1.5 + object->collision_position.y;
                position.z = object->collision_position.z;
            }
            message = AddGameMsg_Default;
            message.scale = 0.666f;
            message.text = TagAsciiDownPc;
            message.position = &position;
            message.red = SuperCarryPlayerRgbPc[i][0];
            message.green = SuperCarryPlayerRgbPc[i][1];
            message.blue = SuperCarryPlayerRgbPc[i][2];
            phase = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0);
            message.flags = 0x83;
            message.unknown_4f = 4;
            message.alpha = (u8)(i32)((f64)NuTrigTable[phase >> 1] * 48.0 + 80.0);
            AddGameMsg(&message);
        }
    }
    if (MissionSys != NULL && Mission_Active(MissionSys) != NULL && MissionSys->active_mission != NULL) {
        CheckMissionEnd(MissionSys);
    }
}

i32 __cdecl KillGameObject(GAMEOBJECT *object, i32 reason, i32 unused) {
    NUVEC momentum, position;
    i32 debris[4];
    i32 part_debris;
    i32 requested_reason;
    i32 terminal_kill;
    GAMEOBJECT *rider;
    AIGROUP_PC *group;
    GAMEOBJECT *candidate;
    PART_PC *part;
    BOLT *bolt;
    i32 i;
    terminal_kill = 0;
    requested_reason = reason;
    if (reason == 5)
        reason = 4;
    if ((object->flags_1fc_word & 0x80) != 0) {
        ++AreaGlobals_PC.field_00;
        ++LevDeaths;
    }
    NuSound3ClearLoopHold(&object->upper_position, 0);
    NuSound3ClearLoopHold(&object->collision_position, 0);
    NuSound3ClearLoopHold(&object->lower_position, 0);
    Grapple_RemoveDynamic(object);
    Shards_HandleLostObj(WORLD, object);
    part_debris = -1;
    debris[0] = -1;
    debris[1] = -1;
    debris[2] = -1;
    debris[3] = -1;
    if (reason != 4) {
        if ((object->character_data->model_flags & 0x4000000) != 0) {
            debris[0] = 73;
            debris[1] = 74;
            debris[2] = 75;
        } else if (object->character_data->move_fn == Move_CANNON) {
            debris[0] = 75;
            debris[1] = 117;
        } else if (object->id == (u16)VehicleBoltId7f1910Pc) {
            debris[0] = 38;
            part_debris = 0;
        } else if (object->id == (u16)id_CLONEWALKER || object->id == (u16)id_ATST_LOWRES) {
            debris[1] = 130;
            debris[2] = 133;
            debris[3] = 132;
        } else if (object->id == GameObjectRotationAtstId) {
            debris[0] = 44;
        } else if (object->id == (u16)id_ATAT) {
            GameCam_NewShake(GameCam, 1.0f, 1.0f, 1.0f);
            NewRumbleAllPlayers(0.7f, 0.0f, 0, 0);
        } else if (object->id == (u16)id_XWING || object->id == (u16)VehicleBoltId7f1a24Pc ||
                   object->id == (u16)VehicleBoltId7f19a4Pc || object->id == (u16)id_TIEFIGHTER ||
                   object->id == (u16)VehicleBoltId7f1a28Pc || object->id == (u16)VehicleBoltId7f196cPc ||
                   object->id == (u16)id_TIEBOMBER || object->id == (u16)VehicleBoltId7f1a2cPc ||
                   object->id == (u16)VehicleBoltId7f195cPc) {
            debris[0] = 34;
            if (WORLD->current_level == DEATHSTARRESCUEE_LDATA && object->field_280 == 0xff) {
                GameCam_Judder(GameCam, qrand() < 32768 ? -0.75f : 0.75f, 2, NULL);
                NewRumbleAllPlayers(1.0f, 0.1f, 0, 0);
            }
        } else if (object->id == (u16)VehicleBoltId7f19a8Pc) {
            debris[0] = 34;
        } else if (object->id == (u16)id_MOUSEDROID) {
            debris[0] = 69;
            part_debris = 1;
        } else if (object->id == id_DRAGBOMB) {
            GameCam_NewShake(GameCam, 1.0f, 1.0f, 1.0f);
            NewRumbleAllPlayers(1.0f, 0.1f, 0, 0);
            AddExplosion(&object->collision_position, (f32)((f64)AreaPickupScale * (f64)0.6f), 0.5f, NULL, -1, 0x807);
            if (Cheat_IsOn(5)) {
                debris[0] = 125;
                debris[1] = 126;
            } else {
                debris[0] = 82;
                debris[1] = 83;
                debris[2] = 84;
                part_debris = 3;
            }
            PlaySfx("exp_thermalDet", &object->collision_position);
        } else if (object->id == (u16)CharacterFixupIdPc007f18c4) {
            part_debris = 5;
        } else if (object->id == (u16)VehicleBoltId7f18b4Pc) {
            debris[0] = 130;
            debris[1] = 133;
        }
        if (object->character_context != 43)
            PlayDieSfx(object);
        if ((object->flags_1fc_word & 0x80) != 0)
            DieRumble(object);
        if ((object->character_data->model_flags & 0x2000) != 0) {
            momentum.x = (f32)((f64)object->velocity.x * 0.5);
            momentum.y = (f32)((f64)object->velocity.y * 0.5);
            momentum.z = (f32)((f64)object->velocity.z * 0.5);
        } else {
            momentum = v000;
        }
        for (i = 0; i < 4; ++i) {
            if (debris[i] != -1) {
                if ((object->character_data->model_flags & 0x2000) != 0)
                    AddGameDebrisMomentum(WORLD->game_debris_sys, debris[i], &object->collision_position, &momentum,
                                          &momentum);
                else
                    AddGameDebris(WORLD->game_debris_sys, debris[i], &object->collision_position);
            }
        }
        if (part_debris != -1)
            AddPartDebris(WORLD->part_debris_sys, part_debris, &object->collision_position);
        BlowUpSnakeBody(object);
    } else {
        DestroySnakeBody(object);
    }
    if (object->character_context == 43) {
        object->character_context = -1;
        if ((object->flags_1fc_word & 0x80) != 0 &&
            (WORLD->current_level == HOTHBATTLEA_LDATA || WORLD->current_level == GUNSHIPA_LDATA))
            GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
    }
    if (reason == 4) {
        terminal_kill = 1;
        reason = 2;
    } else if (WORLD->current_level == SPEEDERCHASEA_LDATA && object->ai_packet.creature_set == 2) {
        GIZAIMESSAGE *message = CheckGizAIMessage(gizaimessagesys, "SpeedersKilled", NULL);
        if (message != NULL)
            message->value = (f32)((f64)message->value + 1.0);
        ++speeder_hitpoints_lost;
    }
    if ((object->character_data->model_flags & 0x2000) == 0) {
        object->velocity.x = 0.0f;
        object->velocity.z = 0.0f;
    } else {
        object->velocity.x = (f32)((f64)object->velocity.x * 0.5);
        object->velocity.y = (f32)(0.5 * ((f64)object->velocity.y));
        object->velocity.z = (f32)((f64)object->velocity.z * 0.5);
    }
    if (reason == 3)
        reason = 2;
    object->dead = (u8)reason;
    object->current_hp = 0;
    if (object->character_context == 7 || object->field_e33 == 1)
        object->flags_e20 |= 0x10000;
    else if (object->character_context == 6 || object->field_e33 == 2)
        object->flags_e20 = object->flags_e20 & (~0x10000);
    if (object->helmet != 0)
        LoseHelmet(object, 1, 0);
    rider = object->rider;
    Player_ClearContext(object, 1);
    if (terminal_kill != 0 && NULL != rider)
        KillGameObject(rider, 4, 0);
    ReleaseLever(object);
    Player_ResetContexts(object->ai_player_packet_pc);
    if ((object->flags_1fc_word & 0x80) != 0) {
        if (object->torpedo != NULL) {
            while (object->torpedo->count != 0) {
                AddVariableShotDebrisEffect(WORLD->game_debris_sys->entries[71].effect,
                                            &object->torpedo->pickup_positions[object->torpedo->count], 30, 0, 0);
                --object->torpedo->count;
            }
        }
        if (object->dead == 0 || (object->flags_1fc_word & 0x80) == 0 ||
            (object->character_data->model_flags & 0x2000) == 0 ||
            (WORLD->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks == 0))
            GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
        if ((object->flags_1f8 & 0x40000) == 0) {
            if ((object->takeover_owner != NULL &&
                 (object->takeover_owner->rider == NULL || object == object->takeover_owner->rider))) {
                GetTakeOverPos(object->takeover_owner, &position);
                object->start_position = position;
                object->position = position;
                object->ai_update_position = position;
            } else {
                SetToLastSafePos(object);
            }
        }
        GameObjectOrigin(object);
        AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, 0xff, 1);
    }
    object->death_timer = 0.0f;
    if (object->dead != 2)
        object->dead = 1;
    if (object->dead != 2)
        object->dead = 1;
    if (1 == object->dead)
        object->start_position = object->position;
    switch ((i8)object->dead) {
        case 2:
            object->death_delay = 0.0f;
            break;
        default:
            object->death_delay = 0.5f;
            break;
    }
    if (object->death_delay <= 0.0f)
        object->movement_lean_angle = 0;
    if (terminal_kill == 0 &&
        AIScriptSetBaseScriptStateByName(&object->ai_packet.script_processor, (char *)BeenKilledPc))
        AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, &object->ai_packet.script_processor,
                        FRAMETIME);
    group = object->ai_packet.group;
    if (group != NULL) {
        group->member_is_alive &= ~(u32)((u64)1 << object->ai_packet.group_member_index);
        group->rows[object->ai_packet.group_row].is_alive &=
            ~(u8)((u64)1 << (object->ai_packet.group_member_index - object->ai_packet.group_row * group->count_across));
        group = object->ai_packet.group;
        if (group->leader == (APIOBJECT_PC *)object) {
            group->leader = NULL;
            if ((group->flags & 4) != 0) {
                for (i = group->member_count - 1; i >= 0; --i) {
                    if (group->members[i] != NULL) {
                        candidate = (GAMEOBJECT *)group->members[i]->owner;
                        if (object != candidate && (candidate->flags_1fc_word & 1) != 0 &&
                            (candidate->flags_1fc_word & 0x1000) != 0 && candidate->dead == 0) {
                            group->leader = (APIOBJECT_PC *)candidate;
                            break;
                        }
                    }
                }
            } else {
                for (i = 0; i < object->ai_packet.group->member_count; ++i) {
                    group = object->ai_packet.group;
                    if (group->members[i] == NULL)
                        continue;
                    candidate = (GAMEOBJECT *)group->members[i]->owner;
                    if (candidate == NULL) {
                        group->members[i] = NULL;
                    } else if (object != candidate && (candidate->flags_1fc_word & 1) != 0 &&
                               (candidate->flags_1fc_word & 0x1000) != 0 && candidate->dead == 0) {
                        group->leader = (APIOBJECT_PC *)candidate;
                        break;
                    }
                }
            }
        }
    }
    object->ai_packet.flags &= ~0x5000;
    object->ai_packet.nearest_opponent = NULL;
    object->ai_packet.target_dc = NULL;
    object->ai_packet.target_ec = NULL;
    object->ai_packet.target_metric_e0 = 1000000000.0f;
    object->ai_packet.target_metric_f0 = 1000000000.0f;
    object->ai_awareness_mask = 0;
    object->ai_seen_mask = 0;
    object->field_eb4 = object->field_eb0 = 0;
    object->opponent = NULL;
    object->ai_packet.opponent = NULL;
    object->alert_target = NULL;
    object->field_ec4 = 0.0f;
    if (object->reciprocal_target != NULL) {
        object->reciprocal_target->reciprocal_target = NULL;
        object->reciprocal_target = NULL;
    }
    if (object->character_context == 71 && object->grapple != NULL) {
        ((KILL_ZIPUP_PC *)object->grapple)->runtime_flags &= ~0x100;
        object->grapple = NULL;
    }
    Batarangs_CheckLostData(object);
    SpecialMove_ReleaseVictim(object);
    ReleaseEat(object);
    ReleaseBuildIt(object, 0);
    ReleasePush(object);
    candidate = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++candidate) {
        if ((candidate->flags_1fc_word & 1) == 0 || (candidate->flags_1fc_word & 0x1000) == 0 || candidate == object)
            continue;
        if (candidate->character_context == 27 && candidate->force_target == object)
            ReleaseForce(candidate, 0);
        if (candidate->force_target == object)
            candidate->force_target = NULL;
        if (candidate->airborne_collision_target == object)
            candidate->airborne_collision_target = NULL;
    }
    ReleaseForce(object, 0);
    if (object->field_280 == 0xff) {
        bolt = Bolt;
        for (i = 0; i < 32; ++i, ++bolt)
            if (bolt->active != 0 && bolt->owner == object)
                bolt->owner = NULL;
        part = Part;
        for (i = 0; i < MAXPARTS; ++i, ++part)
            if ((part->active & 1) != 0 && part->owner == object)
                part->owner = NULL;
        if (Explosions[0].time < Explosions[0].duration && Explosions[0].owner == object)
            Explosions[0].owner = NULL;
        if (Explosions[1].time < Explosions[1].duration && Explosions[1].owner == object)
            Explosions[1].owner = NULL;
        if (Explosions[2].time < Explosions[2].duration && Explosions[2].owner == object)
            Explosions[2].owner = NULL;
        if (Explosions[3].time < Explosions[3].duration && Explosions[3].owner == object)
            Explosions[3].owner = NULL;
        if (Explosions[4].time < Explosions[4].duration && Explosions[4].owner == object)
            Explosions[4].owner = NULL;
        if (Explosions[5].time < Explosions[5].duration && Explosions[5].owner == object)
            Explosions[5].owner = NULL;
        if (Explosions[6].time < Explosions[6].duration && Explosions[6].owner == object)
            Explosions[6].owner = NULL;
        if (Explosions[7].time < Explosions[7].duration && Explosions[7].owner == object)
            Explosions[7].owner = NULL;
    }
    if (DOGFIGHTA_LDATA != NULL && WorldInfo_CurrentlyActive()->current_level == DOGFIGHTA_LDATA && BonusWinner == -1 &&
        (0x40000 & object->flags_1f8) == 0)
        ResetLevel(WorldInfo_CurrentlyActive(), (char *)EP3DogfightDiePc, 1);
    if (0 != terminal_kill && object->field_280 == 0xff) {
        AICREATURE_PC *creature;
        i32 persistent;
        i32 respawn;
        u32 limit;
        f32 delay;
        if (requested_reason == 5) {
            object->flags_1fc_word &= ~0x1000;
            object->ai_packet.reset_mode = 4;
        } else {
            persistent = object->respawnable;
            respawn = 0;
            creature = NULL;
            if ((object->flags_1f8 & 0x4000) != 0 && (object->flags_1f8 & 0x400) != 0) {
                creature = &WORLD->ai_sys->creatures[object->ai_packet.creature_index];
                respawn = object->respawn_at_origin;
                if (!persistent) {
                    if (creature->max_respawn_count != -1 && object->ai_packet.respawn_locator == NULL)
                        limit =
                            creature->min_respawn_count +
                            ((creature->max_respawn_count - creature->min_respawn_count) * ((i32)Game.difficulty - 1)) /
                                9 +
                            1;
                    else
                        limit = object->ai_packet.respawn_count + 1;
                    if (object->ai_packet.respawn_count < limit) {
                        persistent = 1;
                        respawn = 1;
                    }
                }
            }
            if (!persistent) {
                object->flags_1fc_word &= ~0x1000;
                object->ai_packet.reset_mode = 4;
            } else if (respawn && creature != NULL) {
                object->flags_1fc_word &= ~0x1000;
                object->ai_packet.reset_mode = 1;
                delay = (f32)(1.0 - ((f64)Game.difficulty - 1.0) / 9.0);
                object->ai_packet.spawn_delay = delay;
                if (creature->min_respawn_time > 0.0f && creature->max_respawn_time > 0.0f)
                    object->ai_packet.spawn_delay =
                        creature->max_respawn_time * delay + creature->min_respawn_time * (1.0f - delay);
                else
                    object->ai_packet.spawn_delay =
                        (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 2.0 * (f64)object->ai_packet.spawn_delay);
            } else {
                object->field_1010 = 1.0f;
            }
        }
    }
    if (object->torpedo != NULL && object->field_280 == 0xff) {
        FreeTorpedoPacket(&object->torpedo);
        object->torpedo = NULL;
    }
    if (VehicleArea != 0 && object->field_280 < 2)
        DebFreeInstantly(&FalconDebKey[object->field_280]);
    return 1;
}

i32 __cdecl KillPlayer(GAMEOBJECT *object, i32 reason, i32 ignore_spawn_protection, NUVEC *unused) {
    i32 other, active;
    (void)unused;
    if (object->dead != 0)
        return 0;
    if (ignore_spawn_protection != 0)
        object->spawn_protection_timer = 0.0f;
    else if (object->spawn_protection_timer > 0.0f)
        return 0;
    if (Arcade != 0) {
        active = 0;
        other = -1;
        if (object->field_280 == 0)
            other = 1;
        else if (object->field_280 == 1)
            other = 0;
        if (other != -1) {
            if (Player[other] != NULL && (((GAMEOBJECT *)Player[other])->flags_1f8 & 0x40000) != 0)
                active = 1;
            Arcade_PlayerKilled(other, active);
        }
    }
    return KillGameObject(object, reason, 0);
}

void SpeederPart_Kill(PART_PC *part, i32 unused) {
    AddGameDebris(WORLD->game_debris_sys, 0x6a, &part->position);
    if (((KILLPARTS_PART_PC *)part)->speeder_index != -1.0f) {
        GAMEOBJECT *rider =
            KillPartsBikesPc[((i32)(((KILLPARTS_PART_PC *)part)->speeder_index))].rider;
        if (rider != NULL && (rider->flags_1fc_word & 1) != 0 && (rider->flags_1fc_word & 0x1000) != 0) {
            KillPartsBikesPc[((i32)(((KILLPARTS_PART_PC *)part)->speeder_index))]
                .rider->flags_ef0 &= ~0x400000;
            KillParts(
                KillPartsBikesPc[((i32)(((KILLPARTS_PART_PC *)part)->speeder_index))].rider,
                -1, -1, 1, 0.0f, 0, NULL);
            KillGameObject(
                KillPartsBikesPc[((i32)(((KILLPARTS_PART_PC *)part)->speeder_index))].rider, 2,
                0);
        }
        memset(&KillPartsBikesPc[((i32)(((KILLPARTS_PART_PC *)part)->speeder_index))], 0,
               sizeof(KILLPARTS_BIKE_PC));
    }
}

void __cdecl KillParts_SpeederBike(ADDPART_PC *params, i32 unused1, i32 unused2, GAMEOBJECT *object) {
    PART_PC *part;
    KILLPARTS_PART_PC *bike_part;
    f32 distance;
    NUVEC position;
    NUVEC velocity;
    u16 yaw, pitch;
    params->flags = 0x400;
    velocity.x = object->velocity.x * 1.5f;
    velocity.y = 0.0f;
    velocity.z = object->velocity.z * 1.5f;
    params->kill_fn = SpeederPart_Kill;
    params->update_fn = SpeederPart_Update;
    params->draw_fn = SpeederPart_Draw;
    params->velocity = &velocity;
    part = AddPart(params);
    if (part == NULL)
        return;
    part->time_remaining = 1.5f;
    part->initial_time = 1.5f;
    bike_part = ((KILLPARTS_PART_PC *)part);
    bike_part->speeder_index = ((f32)(FindFreeBikePart()));
    if (bike_part->speeder_index == -1.0f || object->movement_spline_position.spline == NULL) {
        KillPart(part, 0);
        return;
    }
    distance = NuVecMag(&part->velocity) * FRAMETIME;
    KillPartsBikesPc[((i32)bike_part->speeder_index)].active = 1;
    KillPartsBikesPc[((i32)bike_part->speeder_index)].spline_position = object->movement_spline_position;
    if (object->rider != NULL) {
        KillPartsBikesPc[((i32)bike_part->speeder_index)].rider = object->rider;
        KillPartsBikesPc[((i32)bike_part->speeder_index)].rider->velocity = velocity;
        KillPartsBikesPc[((i32)bike_part->speeder_index)].rider->velocity.y += 1.5f;
        KillPartsBikesPc[((i32)bike_part->speeder_index)].rider_position = object->position;
    } else {
        KillPartsBikesPc[((i32)bike_part->speeder_index)].rider = NULL;
    }
    PointAlongSpline(KillPartsBikesPc[((i32)bike_part->speeder_index)].spline_position.spline,
                     KillPartsBikesPc[((i32)bike_part->speeder_index)].spline_position.along, &position,
                     &yaw, &pitch, 1);
    yaw -= 0x8000;
    if (distance < 0.0f) {
        yaw += 0x8000;
        pitch = -pitch;
    }
    part->rotation_y = yaw;
    part->rotation_x = -pitch;
    part->rotation_z = 0;
}

void KillParts(GAMEOBJECT *object, i32 animation, i32 excluded_layer, i32 mode, float vertical_scale,
               i32 use_vehicle_velocity, u16 *angle_override) {
    u16 random_rotation_a;
    u16 random_rotation_b;
    i32 random_variant;
    CHARACTERDATA_PC *layer_character;
    u32 layers;
    NUMTX joint_matrices[256];
    i16 render_indices[32];
    i32 render_count;
    i32 render_index;
    i32 part_index = 0;
    i32 retry = 0;
    if ((object->character_data->model_flags & 0x02000000) != 0 ||
        (object->character_data->game_character->flags_090 & 0x8000) != 0)
        return;
    if (((i16)object->id) == id_BOBAFETT && WORLD->current_level == SARLACCPITA_LDATA && FreePlay == 0 &&
        ((u8)object->field_280) == 0xff)
        return;

    GameAudio_PlaySfx(0x4f, &object->collision_position, 0, 0);

    if ((object->character_data->model_flags & 0x04000000) != 0 && ((i16)object->id) != id_MINIDROIDEKA) {
        i32 i;
        KILLPARTS_MINIKIT_PIECES_PC *pieces =
            ((KILLPARTS_MINIKIT_PIECES_PC **)Char_MiniKit)[((i16)object->id)];
        if (pieces == NULL)
            return;
        for (i = 0; i < pieces->piece_count; ++i) {
            ADDPART_PC params;
            PART_PC *part;
            NUMTX matrix;
            NUVEC momentum;
            if (!NuSpecialExistsFn(&pieces->pieces[i].special))
                continue;
            matrix = pieces->pieces[i].matrix;
            NuMtxMul(&matrix, &matrix, &object->transform);
            SetKillPartMom(&momentum);
            NuVecScale(&momentum, &momentum, 3.0f);
            momentum.y += 5.0f;
            params = Default_ADDPART;
            momentum.x += object->velocity.x * 0.75f;
            momentum.y += object->velocity.y * 0.75f;
            momentum.z += object->velocity.z * 0.75f;
            params.matrix = &matrix;
            params.velocity = &momentum;
            params.field_14 = 0.2f;
            params.field_18 = 0.2f;
            params.gravity = -10.0f;
            params.special = &pieces->pieces[i].special;
            params.lighting_data = Cheats_CheckFlags(1)
                                       ? ((PART_LIGHTING_DATA_PC *)(&KillPartsZeroLightingPc))
                                       : ((PART_LIGHTING_DATA_PC *)(
                                             &((KILLPARTS_OBJECT_LIGHT_PC *)object)->lighting));
            params.time_step = FRAMETIME;
            params.flags = mode == 0 ? 0x480 : 0x90;
            params.field_c4 = 1;
            params.stop_fn = PartStop_Flickerer;
            params.draw_fn = PartDraw_Flickerer;
            params.impact_fn = PartImpact_Brick;
            part = AddPart(&params);
            if (part != NULL)
                part->time_remaining = ((f64)(qrand())) * PlayerRandomScalePc * 2.0f + 7.0f;
        }
        return;
    }

    random_rotation_a =
        ((u16)(((i32)(((f64)(qrand())) * PlayerRandomScalePc * 25486.0f - 12743.0f))));
    random_rotation_b =
        ((u16)(((i32)(((f64)(qrand())) * PlayerRandomScalePc * 16384.0f - 8192.0f))));

    do {
        random_variant = qrand() / 21846;
    } while (random_variant == KillPartsPreviousVariantPc && retry++ < 8);
    layer_character = object->character_data;
    KillPartsPreviousVariantPc = random_variant;

    layers = (layer_character->game_character->flags_094 & 0x40000) != 0
                     ? object->layer_mask
                     : AdjustLayerBits(layer_character->game_character->layer_mask_dead, object);
    if (animation != -1)
        layers = 1u << animation;
    else if (excluded_layer != -1)
        layers &= ~(1u << excluded_layer);

    EvalModelAnim(object->character_model, &object->anim_packet, &object->transform, joint_matrices, NULL, NULL, NULL,
                  layers);
    render_count =
        object->character_data->game_character->make_layer_list(object->character_model, render_indices, layers);
    for (render_index = 0; render_index < render_count; ++render_index) {
        KILLPARTS_RENDER_PC *render;
        i32 joint;
        const i16 layer = render_indices[render_index];
        if (layer == -1 || (animation != -1 && layer != animation) || (excluded_layer != -1 && layer == excluded_layer))
            continue;
        render =
            ((KILLPARTS_RENDER_PC *)(&object->character_model->hierarchy->render_parts[layer]));
        if (render->rigid_specials == NULL)
            continue;
        for (joint = 0; joint < object->character_model->hierarchy->joint_count; ++joint, ++part_index) {
            ADDPART_PC params;
            NUMTX matrix;
            NUVEC momentum;
            if (render->rigid_specials[joint] == NULL)
                continue;
            if (VehicleArea != 0 && (object->character_data->model_flags & 0x2000) != 0 &&
                object->character_data->game_character->field_028 > 0.0f) {
                NuMtxMul(&matrix, &joint_matrices[joint], &object->transform);
                SetKillPartMom(&momentum);
                NuVecScale(&momentum, &momentum, 4.0f);
                momentum.y += 6.0f;
                params = Default_ADDPART;
                momentum.x += object->velocity.x * 0.75f;
                momentum.y += object->velocity.y * 0.75f;
                momentum.z += object->velocity.z * 0.75f;
                params.field_14 = 0.4f;
                params.field_18 = 0.4f;
                params.gravity = -10.0f;
            } else {
                SetKillPartMom(&momentum);
                momentum.y += vertical_scale;
                if (animation != -1)
                    momentum.y += 1.0f;
                if ((object->character_data->model_flags & 0x2000) != 0) {
                    momentum.x += object->velocity.x * 0.75f;
                    momentum.y += object->velocity.y * 0.75f;
                    momentum.z += object->velocity.z * 0.75f;
                } else if (use_vehicle_velocity != 0) {
                    momentum.x += object->reset_velocity.x * 0.75f;
                    momentum.y += object->reset_velocity.y * 0.75f;
                    momentum.z += object->reset_velocity.z * 0.75f;
                } else if (angle_override != NULL) {
                    momentum.x += NuTrigTable[(*angle_override >> 1) & 0x7fff] * 2.0f;
                    momentum.z += NuTrigTable[((((u16)(*angle_override + 0x4000))) >> 1) & 0x7fff] * 2.0f;
                }
                NuMtxMul(&matrix, &joint_matrices[joint], &object->transform);
                params = Default_ADDPART;
                params.field_14 = 0.1f;
                params.field_18 = 0.1f;
                params.gravity = -5.0f;
            }
            params.matrix = &matrix;
            params.velocity = &momentum;
            params.special = ((NUSPECIALHANDLE *)(render->rigid_specials[joint]));
            params.lighting_data = Cheats_CheckFlags(1)
                                       ? ((PART_LIGHTING_DATA_PC *)(&KillPartsZeroLightingPc))
                                       : ((PART_LIGHTING_DATA_PC *)(
                                             &((KILLPARTS_OBJECT_LIGHT_PC *)object)->lighting));
            if (((i16)object->id) == id_TIEFIGHTER) {
                KillParts_TIEFIGHTER(&params, part_index, mode, object, random_variant, random_rotation_a,
                                     random_rotation_b, &momentum);
            } else if (((i16)object->id) == id_ATAT) {
                KillParts_ATAT(&params, part_index, mode, object);
            } else if (((i16)object->id) == id_SPEEDERBIKE) {
                KillParts_SpeederBike(&params, part_index, mode, object);
            } else {
                PART_PC *part;
                params.flags = mode == 0 ? 0x480 : 0x90;
                params.impact_fn = PartImpact_Brick;
                params.stop_fn = PartStop_Flickerer;
                params.draw_fn = PartDraw_Flickerer;
                params.time_step = FRAMETIME;
                params.field_c4 = 1;
                part = AddPart(&params);
                if (part != NULL)
                    part->time_remaining = ((f64)(qrand())) * PlayerRandomScalePc * 2.0f + 7.0f;
            }
        }
    }

    if ((object->character_data->flags & 1) != 0 && VehicleArea != 0 &&
        (object->character_data->model_flags & 0x2000) != 0 &&
        object->character_data->game_character->field_028 > 0.0f) {
        NUSPECIALHANDLE *special = CharScene_FindHSpecial(WORLD, ((i16)object->id));
        if (special != NULL) {
            ADDPART_PC params;
            PART_PC *part;
            NUMTX matrix = object->transform;
            NUVEC momentum;
            SetKillPartMom(&momentum);
            NuVecScale(&momentum, &momentum, 4.0f);
            momentum.y += 6.0f;
            params = Default_ADDPART;
            momentum.x += object->velocity.x * 0.75f;
            momentum.y += object->velocity.y * 0.75f;
            momentum.z += object->velocity.z * 0.75f;
            params.matrix = &matrix;
            params.velocity = &momentum;
            params.field_14 = 0.8f;
            params.field_18 = 0.4f;
            params.gravity = -10.0f;
            params.special = special;
            params.lighting_data = Cheats_CheckFlags(1)
                                       ? ((PART_LIGHTING_DATA_PC *)(&KillPartsZeroLightingPc))
                                       : ((PART_LIGHTING_DATA_PC *)(
                                             &((KILLPARTS_OBJECT_LIGHT_PC *)object)->lighting));
            params.time_step = FRAMETIME;
            params.flags = mode == 0 ? 0x480 : 0x90;
            params.field_c4 = 1;
            params.stop_fn = PartStop_Flickerer;
            params.draw_fn = PartDraw_Flickerer;
            params.impact_fn = PartImpact_Brick;
            part = AddPart(&params);
            if (part != NULL)
                part->time_remaining = ((f64)(qrand())) * PlayerRandomScalePc * 2.0f + 3.0f;
        }
    }
    if (object->character_data->game_character->weapon_action == 0)
        Customiser_AddPartAccessories(CharacterCustomiser, object, animation, mode, vertical_scale);
}

void __cdecl InstantKillParts(GAMEOBJECT *object, i32 mode, f32 vertical_scale) {
    i32 excluded_layer;
    excluded_layer = object->id == (u16)id_BODYGUARD && object->current_hp <= 1 ? 4 : -1;
    KillParts(object, -1, excluded_layer, mode, vertical_scale, 0, NULL);
}

i32 __cdecl ObjHitObj(GAMEOBJECT *attacker, GAMEOBJECT *target, i32 damage, u16 flags, i32 probe, i32 ignored) {
    i32 reaction_trigger;

    i32 no_hurt, no_impact, no_rumble, throw_up, random, original_damage, result, health, coins, hearts;
    i16 *parts_angle;
    BOLT *bolt;
    u16 computed_angle;
    f32 random_unit;

    if (target == NULL || (target->flags_1fc_word & 1) == 0 || (target->flags_1fc_word & 0x1000) == 0 ||
        target->dead != 0 || target->field_1010 > 0.0f)
        return 0;
    no_hurt = objhitobj_nohurtsfx;
    no_impact = objhitobj_noimpactsfx;
    no_rumble = objhitobj_noattackerrumble;
    throw_up = objhitobj_throwkillpartsup;
    parts_angle = objhitobj_killparts_yrot;
    result = 0;
    reaction_trigger = 0;
    random = qrand();
    original_damage = damage;
    random_unit = (f32)((f64)random * 1.5259021893143654e-05);
    objhitobj_nohurtsfx = 0;
    objhitobj_noimpactsfx = 0;
    objhitobj_noattackerrumble = 0;
    objhitobj_throwkillpartsup = 0;
    objhitobj_killparts_yrot = NULL;
    bolt = objhitobj_bolt;
    objhitobj_bolt = NULL;
    if (Hub_InMenu() && ((target->flags_1fc_word & 0x80)))
        return 0;
    if (target->character_data->game_character->flags_090 & 0x8000)
        return 0;
    if (target->character_context == 95 || target->character_context == 96)
        return 0;
    if (target->character_context == 90 && (!target->field_7b3 || (attacker && !(attacker->flags_1fc_word & 0x80))))
        return 0;
    if (!(damage == -1 && (flags & 0x200))) {
        if (target->character_context == 0 &&
            (target->action_movement_state == 4 || target->action_movement_state == 3))
            return 0;
        if (target->character_context == 14 || target->character_context == 13)
            return 0;
    }
    if (U64_PAIR(&CInfo[target->character_context].flags) & 0x400000000ULL)
        return 0;
    if (attacker && !(attacker->flags_1fc_word & 1))
        attacker = NULL;
    if (target->field_280 != 0xff)
        ObstacleCamHoldUntilPlayersMove = 0;
    if (target->character_context == 81 || target->character_context == 76) {
        damage = 0;
    }
    if (attacker && ((attacker->flags_1fc_word & 0x80)))
        AlertSurroundingCreatures(attacker, &target->collision_position);
    if (damage > 0 || damage == -1)
        target->portal_flags |= 1;
    if (flags & 0x100) {
        if ((U64_PAIR(&CInfo[target->character_context].flags) & 0x4000000) ||
            (target->character_context != -1 && (target->character_context == LEGOCONTEXT_LAND_SLAM ||
                                                 target->character_context == LEGOCONTEXT_LAND_LUNGE))) {
            i32 rotation_difference = abs(RotDiff(attacker->movement_facing_angle, target->movement_facing_angle));
            if (rotation_difference > 16384 &&
                !(LEGOCONTEXT_COMBO != -1 && attacker->character_context == LEGOCONTEXT_COMBO &&
                  attacker->combo_branch == 6)) {
                if ((target->flags_1fc_word & 0x80))
                    NewRumble(target->pad->bound_pad, 0.75f, 0);
                if (LEGOCONTEXT_HOLD != -1 && target->character_context == LEGOCONTEXT_HOLD) {
                    NewBlockAction(target);
                    if ((target->id == (u16)id_IMPERIALGUARD || target->id == (u16)id_GAMORREANGUARD) &&
                        (target->character_context == 12 || target->character_context == 24))
                        GameAudio_PlaySfx(74, &target->collision_position, 0, 0);
                }
                return 0;
            }
        }
    }
    if (target->origin_scale_override >= 1.0f) {
        if (flags & 0x100) {
            if (damage == -1 || (LEGOCONTEXT_COMBO != -1 && attacker->character_context == LEGOCONTEXT_COMBO &&
                                 attacker->combo_branch == 6))
                ObjHitShield(attacker, target, target->shield_hitpoints, bolt);
            else
                ObjHitShield(attacker, target, original_damage, bolt);
        } else {
            ObjHitShield(attacker, target, 0, bolt);
        }
        return 0;
    }
    if (flags == 0)
        flags = ObjHitObj_Flags(attacker);
    if ((target->portal_flags & 0x08000000) || WORLD->current_level == VADERC_LDATA) {
        if (damage > 1 || damage < 0)
            damage = 1;
    } else if (attacker && (flags & 0x100) && attacker->character_context == 5 && attacker->combo_branch == 6 &&
               target->field_280 == 0xff) {
        damage = -1;
    }
    if ((target->portal_flags & 0x00080000) && (flags & 0x80)) {
        damage = 0;
    } else {
        if (WORLD->current_level == SPEEDERCHASEA_LDATA && !disable_narrow_socks && (flags & 4) &&
            ((target->flags_1fc_word & 0x80))) {
            damage = 0;
            goto attributed_hit;
        }
        if (WORLD->current_level == SPEEDERCHASEA_LDATA && target->id == (u16)id_SPEEDERBIKE &&
            !(0x80 & target->flags_1fc_word)) {
            if (attacker && !(attacker->character_data->model_flags & 0x2000))
                damage = 0;
            else if (target->ai_packet.creature_set != 2)
                damage = 0;
            goto attributed_hit;
        }
        if (WORLD->area == HOTHBATTLE_ADATA &&
            (target->id == (u16)id_ATAT || (attacker && attacker->id == (u16)id_ATAT)) &&
            !(target->flags_1fc_word & 0x80) && (!attacker || !(attacker->flags_1fc_word & 0x80))) {
            if (damage == -1 && target->id == (u16)id_ATAT && target->character_context != 23)
                damage = 0;
            else if (damage == -1 && target->id == (u16)id_ATAT && target->character_context == 23)
                damage = -1;
            else if (attacker && attacker->id == (u16)id_ATAT && target->id == (u16)id_DRAGBOMB)
                damage = -1;
            else
                damage = 0;
            goto attributed_hit;
        }
        if (flags & 1)
            goto attributed_hit;
        if ((flags & 2) && !(0x80 & target->flags_1fc_word) && WORLD->current_level != HUB_LDATA) {
            if (!StartupGameFlag1 && !(target->portal_flags & 0x08000000) && CombatHitReactionIntervalPc != 0 &&
                attacker != NULL && attacker->field_280 != 0xff && target->field_280 == 0xff &&
                !(target->flags_1fc_word & 2)) {
                COMBAT_ATTACK_COUNTER_PC *counter = (COMBAT_ATTACK_COUNTER_PC *)attacker;
                counter->attack_count = (u8)((counter->attack_count + 1) % CombatHitReactionIntervalPc);
                if (counter->attack_count == 0) {
                    reaction_trigger = 1;
                    goto attributed_hit;
                }
            }
            damage = 0;
            goto attributed_hit;
        }
        if ((4 & flags) && (target->flags_1f8 & 0x400) && !(target->flags_1f8 & 0x10005) &&
            WORLD->current_level != HUB_LDATA) {
            damage = 0;
            goto attributed_hit;
        }
        if ((flags & 8) && !(flags & 4) && ((target->flags_1fc_word & 0x80)) && WORLD->current_level != HUB_LDATA &&
            VADERC_LDATA != WORLD->current_level) {
            damage = 0;
            goto attributed_hit;
        }
        if (MiniCutCam && ((target->flags_1fc_word & 0x80)))
            damage = 0;
        else if (target->pad == ViewCamGetGamePad())
            damage = 0;
        else if ((target->character_data->model_flags & 0x20000000) && target->rider == NULL)
            damage = 0;
        else if ((flags & 4) && !(target->flags_1fc_word & 0x80) && !(target->portal_flags & 0x08000000) &&
                 !StartupGameFlag1 && CombatHitReactionIntervalPc != 0 && attacker != NULL &&
                 !(2 & target->flags_1fc_word)) {
            COMBAT_ATTACK_COUNTER_PC *counter = (COMBAT_ATTACK_COUNTER_PC *)attacker;
            counter->attack_count = (u8)((counter->attack_count + 1) % CombatHitReactionIntervalPc);
            if (counter->attack_count == 0)
                reaction_trigger = 1;
        }
    }
attributed_hit:
    if (attacker)
        target->last_attacker = attacker;
    if (WORLD->current_level == HUB_LDATA && attacker && (attacker->flags_1fc_word & 0x80) &&
        AIScriptSetBaseScriptStateByName(&*(AISCRIPTPROCESS_PC *)&target->ai_packet, "TakenHitFromPlayer")) {
        AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)target, &target->ai_packet,
                        &*(AISCRIPTPROCESS_PC *)&target->ai_packet, FRAMETIME);
        goto impact;
    }
    if (AIScriptSetBaseScriptStateByName(&*(AISCRIPTPROCESS_PC *)&target->ai_packet, "TakenHit"))
        AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)target, &target->ai_packet,
                        &*(AISCRIPTPROCESS_PC *)&target->ai_packet, FRAMETIME);
impact:
    if (!no_impact) {
        if (attacker && attacker->id == (u16)id_BODYGUARD)
            PlaySfx("Grv_GuardImpact", &target->collision_position);
        else if (attacker && attacker->id == (u16)id_IMPERIALGUARD)
            PlaySfx("wpn_bib_stab", &target->collision_position);
        else if (bolt) {
            if (bolt->type_id != 27 && bolt->type_id != 28 && bolt->type_id != 29 && WORLD->area &&
                (WORLD->area == PODRACE_ADATA || WORLD->area == PODSPRINT_ADATA)) {
                PlaySfx("Pod_TuskHit", &bolt->position);
                no_hurt = 1;
            }
        } else if (VehicleArea || (WORLD->current_level == SPEEDERCHASEA_LDATA && !disable_narrow_socks)) {
            i32 bits = attacker && (u8)(attacker->field_280) <= 1 ? 1 << attacker->field_280 : 0;
            GameAudio_PlaySfx(40, &target->collision_position, bits, 0);
        } else if (flags & 0x40) {
            if (attacker && attacker->character_context == 38 &&
                (AnimMiscFlags(attacker->character_model, attacker->field_7aa) & 4))
                PlaySfx("WhipHit", &target->collision_position);
            else {
                i32 bits = attacker && (u8)(attacker->field_280) <= 1 ? 1 << attacker->field_280 : 0;
                GameAudio_PlaySfx(74, &target->collision_position, bits, 0);
            }
        } else if (attacker) {
            if (attacker->character_data->model_flags & 8)
                GameAudio_PlaySfx(65, &target->collision_position, GameAudio_GetPlrSfxBits(attacker), 0);
            else
                GameAudio_PlaySfx(74, &target->collision_position, 0, 0);
        }
    }
    if (target->spawn_protection_timer > 0.0f || (target->flags_ef0 & 0x00400000)) {
        if (target->flags_ef0 & 0x00001000)
            PlayerTakeHit(target, attacker);
        goto finish;
    }
    if (target->character_context == 21 && (target->portal_flags & 0x08000000)) {
        target->spawn_protection_timer = 2.5f;
        goto finish;
    }
    if ((target->character_data->model_flags & 0x10) && !target->current_hp && (flags & 0x40) && random_unit > 0.75f) {
        DeactivatePlayer(target, 5.0f, NULL);
        goto finish;
    }
    if (damage != -1 && !(target->vehicle_hit_timer <= 0.0f) && (!attacker || !(attacker->flags_1fc_word & 0x80)))
        goto finish;
    if (target->character_context == 45 && !Player_HasFastBuild(target))
        GizBuildIt_SetToStart(target->grapple, 1, 1);
    Player_ClearContext(target, 0);
    ReleaseForce(target, 1);
    if (target->helmet && 1)
        LoseHelmet(target, 0, 0);
    if (damage != -1) {
        if (!target->current_hp || (((target->flags_1fc_word & 0x80)) && Player_HasInvincibility(target))) {
            if (target->id == (u16)id_ROYALGUARD || target->id == (u16)id_WOOKIEE)
                SetFlicker(target, 0.4f);
            PlayerTakeHit(target, attacker);
            if ((target->flags_1fc_word & 0x80))
                TakeHitRumble(target, 0.666f);
            goto hurt;
        }
        health = target->current_hp - damage;
    } else
        health = 0;
    no_impact = 0;
    if (health <= 0) {
        no_impact = players_cannot_exit_speeder && target->id == (u16)id_SPEEDERBIKE && target->rider &&
                    target->field_280 != 0xff;
        if ((target->character_data->model_flags & 0x20000000) && !ObjIsTargetSpeeder(target) && !no_impact) {
            coins = 0;
            if (((target->flags_1fc_word & 0x80)) && target->coin_packet &&
                ((COMBAT_COINPACKET_PC *)target->coin_packet)->coins && BonusWinner == -1)
                coins = LoseCoins(target, 1);
            AddPickups(coins, 0, 0, 0, &target->collision_position, NULL, 2.0f, attacker ? (i8)attacker->field_280 : -1,
                       1.0f, 2000000.0f, attacker, 1, 0);
            DeactivatePlayer(target, 1000000000.0f, NULL);
            target->current_hp = target->field_107e;
            SetFlicker(target, 0.4f);
            goto hurt;
        }
    }
    if ((target->character_data->model_flags & 0x40000000) && (health <= 0 || target->character_context == 23))
        goto refill;
    if (health <= 0 &&
        !((target->portal_flags & 0x08000000) && (!FreePlay || WORLD->current_level != CRUISERC_LDATA))) {
        if (probe) {
            result = 2;
            goto finish;
        }
        if (target->character_context == 93) {
            PopBalloon(target);
            target->current_hp = 1;
            goto hurt;
        }
        if (!StartupGameFlag1) {
            u16 angle;
            f32 speed;
            COMBAT_REACTION_OWNER_PC *reaction = (COMBAT_REACTION_OWNER_PC *)target;
            if (reaction_trigger) {
                i32 reaction_result = PlayerTakeHitReactionPc(target, attacker, CombatReactionDurationPc, flags);
                if (reaction_result != 0) {
                    target->current_hp = 1;
                    SetFlicker(target, 0.4f);
                    if (reaction_result == 2 && attacker != NULL && (attacker->flags_1fc_word & 0x80) && !no_rumble)
                        KillRumble(attacker);
                    goto hurt;
                }
            }
            if (StartupGameFlag1 || (flags & 0x200) || attacker == NULL || target->field_280 != 0xff ||
                (target->portal_flags & 0x08000000) || 0x8000 <= qrand() || (target->flags_1fc_word & 2))
                goto ordinary_death;
            target->character_context = 95;
            target->field_7aa = 0xb8;
            if (target->character_model->model_data_b[0xb8] == NULL)
                target->field_7aa = 5;
            target->current_hp = 1;
            target->context_timer = 0.0f;
            target->context_duration = 2.0f;
            angle = NuAtan2D(target->collision_position.x - attacker->collision_position.x,
                             target->collision_position.z - attacker->collision_position.z);
            speed = (f32)(((f64)qrand() * 1.5259021893143654e-05 * 0.4000000059604645 + 0.800000011920929) *
                          CombatReactionHorizontalScalePc);
            target->external_force.x = NuTrigTable[angle >> 1] * speed;
            target->external_force.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * speed;
            target->movement_facing_angle = angle + 0x8000;
            reaction->attacker_slot =
                attacker->field_280 == 0 || attacker->field_280 == 1 ? (i8)attacker->field_280 : -1;
            if (qrand() < 0x5555) {
                target->context_variant_flags |= 0x100;
                target->velocity.y =
                    (f32)(((f64)qrand() * 1.5259021893143654e-05 * 0.30000001192092896 + 0.20000000298023224) *
                          CombatReactionVerticalScalePc);
                target->external_force.x *= 2.0f;
                target->external_force.z *= 2.0f;
            } else {
                target->context_variant_flags &= ~0x100;
                target->velocity.y =
                    (f32)(((f64)qrand() * 1.5259021893143654e-05 * 0.4000000059604645 + 0.800000011920929) *
                          CombatReactionVerticalScalePc);
            }
            ResetAnimPacket(&target->anim_packet, -1);
            SetFlicker(target, 0.4f);
            if ((attacker->flags_1fc_word & 0x80) && !no_rumble)
                KillRumble(attacker);
            goto hurt;
        }
    ordinary_death:
        target->current_hp = 0;
        hearts = 0;
        coins = 0;
        if (target->field_280 != 0xff) {
            if ((target->flags_1fc_word & 0x80)) {
                if (target->coin_packet && ((COMBAT_COINPACKET_PC *)target->coin_packet)->coins && BonusWinner == -1)
                    coins = LoseCoins(target, 1);
                if (!BuildUpTotal)
                    BuildUpScale = 1.5f;
                DrawBuildUpTime = 1.0f;
                builduptime = 1.0f;
            }
        } else if (!(target->flags_1fc_word & 0x80)) {
            coins = BonusArea ? (u16)(target->character_data->game_character->coin_value) : (Cheat_IsOn(16) ? 350 : 0);
            hearts = ReleaseHearts();
        }
        if (coins > 0 || hearts)
            AddPickups(coins, hearts, 0, 0, &target->collision_position, NULL, 2.0f, -1, 1.0f, 2000000.0f, attacker,
                       !BonusArea && coins < 2500, BonusArea ? 1 : 0);
        if (target->torpedo && WORLD->area != SPEEDERCHASE_ADATA && (!WORLD->area || !(WORLD->area->flags & 4)))
            DropTorpedoPickups(target->torpedo, target->torpedo->count);
        if (!parts_angle) {
            if (!attacker)
                parts_angle = NULL;
            else if (flags & 0x240) {
                computed_angle =
                    NuAtan2D(target->position.x - attacker->position.x, target->position.z - attacker->position.z);
                parts_angle = (i16 *)(&computed_angle);
            }
        }
        if (WORLD->current_level == VADERC_LDATA && !netclient) {
            GIZAIMESSAGE *message = CheckGizAIMessage(gizaimessagesys, "FinalFight", NULL);
            if (message && message->value == 1.0f && (!attacker || ((attacker->flags_1fc_word & 0x80)))) {
                LevelCompletionFallbackFlagPC = 1;
                if (FreePlay)
                    CompleteLevel(WORLD);
                else {
                    char name[16];
                    NUSPECIALHANDLE special;
                    i32 i;
                    for (i = 1; i != 12; ++i) {
                        sprintf(name, "rock%d", i);
                        if (NuSpecialFind(WORLD->current_gscn, &special, name, 1))
                            NuSpecialSetVisibility(&special, 0);
                    }
                    if (attacker && attacker->id == (u16)id_OBIWANKENOBIEP3 && target->id == (u16)id_ANAKINJEDI)
                        NewCutScene(NULL, WORLD->cutscene_sys, (char *)"ep3_darthvader_outro2", 1);
                    else
                        NewCutScene(NULL, WORLD->cutscene_sys, (char *)"ep3_darthvader_outro1", 1);
                }
                return 0;
            }
        }
        if (no_impact && target->rider) {
            KillParts(target->rider, -1, target->rider->id == (u16)id_BODYGUARD ? 4 : -1, 1, 0.0f, 0,
                      (u16 *)(parts_angle));
            KillGameObject(target->rider, 2, 0);
        }
        KillParts(target, -1, (u16)id_BODYGUARD == target->id ? 4 : -1, 1, throw_up ? 1.0f : 0.0f, 0,
                  (u16 *)(parts_angle));
        KillGameObject(target, 2, 0);
        if ((target->flags_1fc_word & 0x80))
            GameCam_Judder(GameCam, 0.2f, 0, NULL);
        if (attacker != NULL && (attacker->flags_1fc_word & 0x80) && (target->flags_1f8 & 1) &&
            !(target->character_data->model_flags & 0x80000) && (flags & 4))
            AdaptiveDifficultyAdjustPc(&adaptivedifficulty, CombatNpcKillDifficultyAmountPc);
        else if ((target->flags_1fc_word & 0x80) && (flags & 2))
            AdaptiveDifficultyAdjustPc(&adaptivedifficulty, CombatPlayerDeathDifficultyAmountPc);
        result = 2;
        goto finish;
    }
    if ((target->flags_1fc_word & 0x80))
        TakeHitRumble(target, 0.666f);
    target->current_hp = health;
    if (damage != 0) {
        if (AIScriptSetBaseScriptStateByName(&*(AISCRIPTPROCESS_PC *)&target->ai_packet, "LostHitPoints"))
            AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)target, &target->ai_packet,
                            &*(AISCRIPTPROCESS_PC *)&target->ai_packet, FRAMETIME);
        SetFlicker(target, 0.4f);
        if (target->id == (u16)id_SNAKE)
            SnakeBeenHit(target);
    }
    result = 1;
    PlayerTakeHit(target, attacker);
    if (target->field_280 != 0xff && damage > 0) {
        target->saved_hitpoints -= damage;
        if (target->saved_hitpoints < 1)
            target->saved_hitpoints = 1;
        if ((target->character_data->model_flags & 0x20) && target->origin_joint_ready) {
            i32 index = target->saved_hitpoints - 1;
            if (index > 2)
                index = 2;
            index = objhit_damage_joints[2 - index];
            if (target->character_model->points_of_interest[index])
                AddGameDebris(WORLD->game_debris_sys, 113, (NUVEC *)(&target->locator_matrices[index].m30));
        }
    }
    if (target->id == (u16)id_BODYGUARD && target->current_hp == 1)
        KillParts(target, 4, -1, 1, 0.0f, 0, NULL);
    goto hurt;
hurt:
    if (!no_hurt && result != 2 && target->character_context != 23)
        PlayHurtSfx(target);
    goto finish;
finish:
    if (attacker && ((attacker->flags_1fc_word & 0x80))) {
        if (!no_rumble) {
            if (result == 2)
                KillRumble(attacker);
            else
                HitRumble(attacker);
        }
        if (!(flags & 0x4000) && result == 2 && (u8)(attacker->field_280) <= 1 && Arcade)
            Arcade_Kill((i8)attacker->field_280, (i8)target->field_280);
    }
    return result;
refill:
    coins = 0;
    if (((target->flags_1fc_word & 0x80)) && target->coin_packet &&
        ((COMBAT_COINPACKET_PC *)target->coin_packet)->coins && BonusWinner == -1)
        coins = LoseCoins(target, 1);
    AddPickups(coins, 0, 0, 0, &target->collision_position, NULL, 2.0f, attacker ? (i8)attacker->field_280 : -1, 1.0f,
               2000000.0f, attacker, 1, 0);
    if (target->character_context != 23)
        SetFlicker(target, 0.4f);
    Player_ClearContext(target, 1);
    target->current_hp = target->field_107e;
    goto hurt;
}
