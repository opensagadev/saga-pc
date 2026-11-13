#include "saga.h"
#include <string.h>

/* Retail unit 0x59f970-0x5a043e: definitions in retail address order. */

STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, game_character) == 0x24);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1f8) == 0x1f8);
STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
STATIC_ASSERT(offsetof(LEVELDATA_PC, flags) == 0x64);
STATIC_ASSERT(offsetof(LEVELDATA_PC, area_level_index) == 0xc8);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, script_processor) +
                  offsetof(AISCRIPTPROCESS_PC, script) ==
              0x2cc);
STATIC_ASSERT(offsetof(AISCRIPT_PC, name) == 8);
STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, creature_index) == 0x400);
extern void *Door_Last;
extern u64 CharacterMaskAllPc;
i8 __cdecl Area_CharIDInCurrentList(i32 character_id);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, reset_mode) == 0x406);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, path_info) == 0x420);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_0x276) == 0x27a);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, current_hp) == 0x107f);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);

void __cdecl ClearTakeOverObjectSys(void) {
    memset(takeoverobjects, 0, sizeof(takeoverobjects));
    num_takeoverobjects = 0;
}

void __cdecl RegisterTakeOverObject(GAMEOBJECT *object) {
    i32 index;
    AISCRIPT_PC *script;
    i32 level;
    if ((object->character_data->game_character->flags_090 & 0x80) != 0 || (object->flags_1f8 & 0x4000) == 0 ||
        (WORLD->current_level->flags & 0x100000) != 0)
        return;
    level = WORLD->current_level->area_level_index;
    if (num_takeoverobjects >= 8 || (object->flags_1f8 & 0x400) == 0)
        return;
    for (index = 0; index < num_takeoverobjects; ++index) {
        if (takeoverobjects[index].object == object)
            return;
    }
    takeoverobjects[index].object = object;
    script = object->ai_packet.script_processor.script;
    if (script != NULL && script->name != NULL)
        NuStrNCpy(takeoverobjects[index].script_name, script->name, 16);
    takeoverobjects[index].character_id = object->id;
    takeoverobjects[index].source_creature = object->ai_packet.creature_index;
    takeoverobjects[index].registered_level = level;
    takeoverobjects[index].current_level = level;
    ++num_takeoverobjects;
}

void __cdecl StoreStatusTakeOverObjectSys(void) {
    i32 index;
    i32 level;
    u8 character_index;
    GAMEOBJECT *object;
    if (netclient != 0)
        return;
    level = WORLD->current_level->area_level_index;
    for (index = 0; index < num_takeoverobjects; ++index) {
        object = takeoverobjects[index].object;
        if (object == NULL)
            continue;
        takeoverobjects[index].last_safe_position = object->last_safe_position;
        takeoverobjects[index].heading = object->field_0x276;
        if (object->rider != NULL && LEGOCONTEXT_BEENTAKENOVER != -1 &&
            object->rider->character_context == LEGOCONTEXT_BEENTAKENOVER && object->field_280 != 0xff &&
            Door_Last != NULL && ((struct DOOR_s *)Door_Last)->takeover_character_mask != 0 &&
            (((struct DOOR_s *)Door_Last)->takeover_character_mask == CharacterMaskAllPc ||
             ((character_index = (u8)Area_CharIDInCurrentList((i16)object->id)) < 63 &&
              (((struct DOOR_s *)Door_Last)->takeover_character_mask & ((u64)1 << character_index)) != 0)))
            takeoverobjects[index].contact_index = takeoverobjects[index].object->field_280;
        else
            takeoverobjects[index].contact_index = 0xff;
        takeoverobjects[index].hitpoints = takeoverobjects[index].object->current_hp;
        if (takeoverobjects[index].hitpoints == 0 && takeoverobjects[index].object->field_107e != 0)
            takeoverobjects[index].hitpoints = takeoverobjects[index].object->field_107e;
        takeoverobjects[index].current_level = (u8)level;
    }
}

void __cdecl ReStoreStatusTakeOverObjectSys(i32 restore_progress) {
    i32 level, index, object_index;
    GAMEOBJECT *object, *controller, *candidate;
    if (netclient != 0)
        return;
    level = WORLD->current_level->area_level_index;
    for (index = 0; index < num_takeoverobjects; ++index) {
        if (restore_progress != 0 || takeoverobjects[index].current_level != level)
            takeoverobjects[index].object = NULL;
        if (takeoverobjects[index].contact_index != 0xff)
            takeoverobjects[index].current_level = (u8)level;
        if (takeoverobjects[index].registered_level == level) {
            if (takeoverobjects[index].object == NULL) {
                if (takeoverobjects[index].source_creature == 0xff) {
                    if (takeoverobjects[index].current_level == level) {
                        takeoverobjects[index].object = AddDynamicCreature(
                            takeoverobjects[index].character_id, &takeoverobjects[index].last_safe_position,
                            takeoverobjects[index].heading, takeoverobjects[index].script_name, NULL, NULL, 1, NULL,
                            NULL, 0, 0);
                    }
                } else {
                    candidate = Obj;
                    for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index, ++candidate) {
                        if (candidate->ai_packet.creature_index == takeoverobjects[index].source_creature) {
                            takeoverobjects[index].object = candidate;
                            break;
                        }
                    }
                }
            }
            if (takeoverobjects[index].object != NULL && takeoverobjects[index].current_level != level) {
                KillGameObject(takeoverobjects[index].object, 5, 0);
                takeoverobjects[index].object = NULL;
            }
        } else if (takeoverobjects[index].current_level == level && takeoverobjects[index].object == NULL) {
            takeoverobjects[index].object = AddDynamicCreature(
                takeoverobjects[index].character_id, &takeoverobjects[index].last_safe_position,
                takeoverobjects[index].heading, takeoverobjects[index].script_name, NULL, NULL, 1, NULL, NULL, 0, 0);
        }
        object = takeoverobjects[index].object;
        if (object != NULL) {
            object->current_hp = takeoverobjects[index].hitpoints;
            if (!(takeoverobjects[index].contact_index != 0xff &&
                  (controller = (GAMEOBJECT *)Player[takeoverobjects[index].contact_index]) != NULL)) {
                SnapCreaturePos(takeoverobjects[index].object, &takeoverobjects[index].last_safe_position,
                                takeoverobjects[index].heading, NULL, 1);
            } else {
                takeoverobjects[index].object->dead = 0;
                takeoverobjects[index].object->flags_1fc_word |= 0x1000;
                takeoverobjects[index].object->flags_1fc_word |= 1;
                takeoverobjects[index].object->ai_packet.reset_mode = 2;
                SnapCreaturePos(takeoverobjects[index].object, &controller->position, controller->field_0x276,
                                &controller->ai_packet.path_info, 1);
                InitPlayerAI(controller);
                TakeOverGameObject(controller, takeoverobjects[index].object, 0, 1);
            }
        }
    }
}

void __cdecl Tag_ResetTransfers(void) {
    TagTransfersPc[0].elapsed = 0.5f;
    TagTransfersPc[1].elapsed = 0.5f;
}

void __cdecl Tag_NewTransfer(GAMEOBJECT *source, GAMEOBJECT *target) {
    i32 particle;
    f32 height;
    f32 lower;
    NUVEC *previous;
    if (target->field_280 == 0 || target->field_280 == 1) {
        TagTransfersPc[(i8)target->field_280].source = source;
        TagTransfersPc[(i8)target->field_280].elapsed = 0.0f;
        for (particle = 0; particle < 3; ++particle) {
            height = TagTransfersPc[(i8)target->field_280].height_fraction[particle] =
                (f32)((f64)qrand() * PlayerRandomScalePc * (f64)0.4f + (f64)0.4f);
            previous = &TagTransfersPc[(i8)target->field_280].previous[particle];
            previous->x = source->collision_position.x;
            previous->z = source->collision_position.z;
            lower = source->collision_min.y;
            previous->y = (f32)((f64)lower + ((f64)source->collision_max.y - lower) * height);
        }
    }
    if ((target->flags_1fc_word & 0x80) != 0) {
        if (Tag_DoneFirst == 0)
            Tag_DoneFirst = 1;
        else if (Tag_DoneFirst == 1)
            Tag_DoneFirst = 2;
        Tag_DoneAny = 1;
    }
}

void __cdecl Tag_UpdateTransfers(i32 effect0, i32 effect1, i32 count) {
    i32 player;
    i32 particle;
    i32 angle;
    f32 elapsed;
    f32 phase;
    f32 height;
    f32 lower;
    TAG_TRANSFER_PC *transfer;
    GAMEOBJECT *source;
    GAMEOBJECT *target;
    NUVEC from;
    NUVEC to;
    NUVEC position;
    for (player = 0; player < 2; ++player) {
        transfer = &TagTransfersPc[player];
        if (Player[player] == NULL) {
            transfer->elapsed = 0.5f;
            continue;
        }
        if (!(transfer->elapsed < 0.5f))
            continue;
        elapsed = transfer->elapsed + FRAMETIME;
        transfer->elapsed = elapsed;
        if (elapsed > 0.5f) {
            transfer->elapsed = 0.5f;
            continue;
        }
        phase = elapsed + elapsed;
        for (particle = 0; particle < 3; ++particle) {
            source = transfer->source;
            from.x = source->collision_position.x;
            from.z = source->collision_position.z;
            lower = source->collision_min.y;
            height = transfer->height_fraction[particle];
            from.y = (f32)((f64)lower + ((f64)source->collision_max.y - lower) * height);
            target = Player[player];
            to.x = target->collision_position.x;
            to.z = target->collision_position.z;
            lower = target->collision_min.y;
            to.y = (f32)((f64)lower + ((f64)target->collision_max.y - lower) * height);
            if (particle == 1) {
                angle = (i32)((f64)phase * 16384.0);
                phase = NuTrigTable[(angle >> 1) & 0x7fff];
            } else if (particle == 2) {
                angle = (i32)((f64)phase * 16384.0 + 16384.0);
                phase = 1.0f - NuTrigTable[(angle >> 1) & 0x7fff];
            }
            position.x = (f32)((f64)from.x + ((f64)to.x - from.x) * phase);
            position.y = (f32)((f64)from.y + ((f64)to.y - from.y) * phase);
            position.z = (f32)((f64)from.z + ((f64)to.z - from.z) * phase);
            if (particle == 1) {
                angle = (i32)((f64)phase * 65536.0);
                position.y = (f32)((f64)position.y + NuTrigTable[(angle >> 1) & 0x7fff] * (f64)0.005f);
            } else if (particle == 2) {
                angle = (i32)((f64)phase * 32768.0 + 16384.0);
                position.y = (f32)((f64)position.y + NuTrigTable[(angle >> 1) & 0x7fff] * (f64)0.01f);
            }
            AddVariableShotDebrisEffectTimed1(player == 0 ? effect0 : effect1, &position, count, FRAMETIME, 0, 0, NULL);
            transfer->previous[particle] = position;
        }
    }
}

void __cdecl Buck_StartRiderJump(GAMEOBJECT *rider, GAMEOBJECT *mount) {
    u16 angle = (u16)qrand();
    f32 speed = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.09f + (f64)0.01f);
    f32 clearance;
    PLAYERCHARACTERCONFIG_PC *character;
    rider->velocity.x = NuTrigTable[angle >> 1] * speed;
    rider->velocity.z = speed * NuTrigTable[(((i32)angle + 0x4000) >> 1) & 0x7fff];
    StartJump(rider, 0);
    rider->airborne_collision_target = mount;
    rider->tag_flags |= 1;
    rider->velocity.x = 0.0f;
    rider->velocity.z = 0.0f;
    rider->pad->buttons_held = 0;
    rider->pad->buttons_pressed = 0;
    clearance = mount->upper_position.y;
    if ((mount->character_data->game_character->flags_094 & 0x20000) != 0)
        clearance += BUCK_RIDERJUMPCLEARANCE;
    clearance -= rider->lower_position.y;
    clearance = (f32)((f64)clearance + (f64)0.1f);
    character = rider->character_data->game_character;
    if (clearance > character->jump_height)
        rider->velocity.y = NuFsqrt((f32)(-2.0 * (f64)character->gravity * clearance));
}

void __cdecl Buck_Start(GAMEOBJECT *object, GAMEOBJECT *rider) {
    CHARACTER_MODEL_LAYERS_PC *model;
    if (LEGOCONTEXT_BUCK == -1 || LEGOACT_BUCK == -1)
        return;
    model = (CHARACTER_MODEL_LAYERS_PC *)object->character_model;
    if (model->model_data_b[LEGOACT_BUCK] == NULL)
        return;
    if (rider != NULL)
        NewRumble(rider->pad->bound_pad, 0.6f, 0);
    object->character_context = (i8)LEGOCONTEXT_BUCK;
    object->field_7aa = LEGOACT_BUCK;
    object->context_timer = AnimDuration((i16)object->id, LEGOACT_BUCK, 0.0f, 0.0f, 1);
    if (BuckStartExtraFn != NULL)
        BuckStartExtraFn(object);
}

void __cdecl Buck_MoveCode(GAMEOBJECT *object, i32 start) {
    if (LEGOCONTEXT_BUCK != -1 && object->character_context == LEGOCONTEXT_BUCK) {
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f)
            object->character_context = -1;
    } else if (object->character_context == -1 && start != 0) {
        Buck_Start(object, object->takeover_source);
    }
}
