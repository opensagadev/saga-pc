#include "gameapi/ai/aisys/aiscript_types.h"
#include "globals.h"
#include "legoapi/actions/character/legoapi_tag.h"
#include "legoapi/actions/character/tagging.h"
#include "legoapi/actions/movement/jumping.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/animation_ids.h"
#include "legoapi/characters/motion/contexts.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/gizmo/object/takeoverobjects.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/render/fx.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>

extern void *Door_Last;
extern u64 CharacterMaskAll;
i8 Area_CharIDInCurrentList(i32 character_id);

TAKEOVEROBJECT takeoverobjects[8] = {0};
i32 num_takeoverobjects = 0;

void ClearTakeOverObjectSys(void) {
    memset(takeoverobjects, 0, sizeof(takeoverobjects));
    num_takeoverobjects = 0;
}

void RegisterTakeOverObject(GAMEOBJECT *object) {
    i32 index;
    AISCRIPT *script;
    i32 level;
    if ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_DONT_MOVE) != 0 || (object->flags_1f8 & 0x4000) == 0 ||
        (WORLD->current_level->flags & LEVEL_FORGET_TAKEOVERS) != 0) {
        return;
    }
    level = WORLD->current_level->area_level_index;
    if (num_takeoverobjects >= 8 || (object->flags_1f8 & APIOBJECT_CREATURE) == 0) {
        return;
    }
    for (index = 0; index < num_takeoverobjects; ++index) {
        if (takeoverobjects[index].object == object) {
            return;
        }
    }
    takeoverobjects[index].object = object;
    script = object->ai_packet.script_processor.script;
    if (script != NULL && script->name != NULL) {
        NuStrNCpy(takeoverobjects[index].script_name, script->name, sizeof(takeoverobjects[index].script_name));
    }
    takeoverobjects[index].character_id = object->id;
    takeoverobjects[index].source_creature = object->ai_packet.creature_index;
    takeoverobjects[index].registered_level = level;
    takeoverobjects[index].current_level = level;
    ++num_takeoverobjects;
}

void StoreStatusTakeOverObjectSys(void) {
    i32 index;
    i32 level;
    u8 character_index;
    GAMEOBJECT *object;
    if (netclient != 0) {
        return;
    }
    level = WORLD->current_level->area_level_index;
    for (index = 0; index < num_takeoverobjects; ++index) {
        object = takeoverobjects[index].object;
        if (object == NULL) {
            continue;
        }
        takeoverobjects[index].last_safe_position = object->last_safe_position;
        takeoverobjects[index].heading = object->unknown_276;
        if (object->player.rider != NULL && LEGOCONTEXT_BEENTAKENOVER != -1 &&
            object->player.rider->player.character_context == LEGOCONTEXT_BEENTAKENOVER && object->player_index != -1 &&
            Door_Last != NULL && ((struct DOOR_s *)Door_Last)->takeover_character_mask != 0 &&
            (((struct DOOR_s *)Door_Last)->takeover_character_mask == CharacterMaskAll ||
             ((character_index = (u8)Area_CharIDInCurrentList((i16)object->id)) < 63 &&
              (((struct DOOR_s *)Door_Last)->takeover_character_mask & ((u64)1 << character_index)) != 0))) {
            takeoverobjects[index].contact_index = takeoverobjects[index].object->unknown_280;
        } else {
            takeoverobjects[index].contact_index = 0xff;
        }
        takeoverobjects[index].hitpoints = takeoverobjects[index].object->current_hp;
        if (takeoverobjects[index].hitpoints == 0 && takeoverobjects[index].object->unknown_107e != 0) {
            takeoverobjects[index].hitpoints = takeoverobjects[index].object->unknown_107e;
        }
        takeoverobjects[index].current_level = (u8)level;
    }
}

void ReStoreStatusTakeOverObjectSys(i32 restore_progress) {
    i32 level;
    i32 index;
    i32 object_index;
    GAMEOBJECT *object;
    GAMEOBJECT *controller;
    GAMEOBJECT *candidate;
    if (netclient != 0) {
        return;
    }
    level = WORLD->current_level->area_level_index;
    for (index = 0; index < num_takeoverobjects; ++index) {
        if (restore_progress != 0 || takeoverobjects[index].current_level != level) {
            takeoverobjects[index].object = NULL;
        }
        if (takeoverobjects[index].contact_index != 0xff) {
            takeoverobjects[index].current_level = (u8)level;
        }
        if (takeoverobjects[index].registered_level == level) {
            if (takeoverobjects[index].object == NULL) {
                if (takeoverobjects[index].source_creature == 0xff) {
                    if (takeoverobjects[index].current_level == level) {
                        takeoverobjects[index].object = AddDynamicCreature(
                            takeoverobjects[index].character_id, &takeoverobjects[index].last_safe_position, takeoverobjects[index].heading,
                            takeoverobjects[index].script_name, NULL, NULL, 1, NULL, NULL, 0, 0);
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
                KillGameObject(takeoverobjects[index].object, KILL_REASON_TAKEN_OVER, 0);
                takeoverobjects[index].object = NULL;
            }
        } else if (takeoverobjects[index].current_level == level && takeoverobjects[index].object == NULL) {
            takeoverobjects[index].object =
                AddDynamicCreature(takeoverobjects[index].character_id, &takeoverobjects[index].last_safe_position,
                                   takeoverobjects[index].heading, takeoverobjects[index].script_name, NULL, NULL, 1, NULL, NULL, 0, 0);
        }
        object = takeoverobjects[index].object;
        if (object != NULL) {
            object->current_hp = takeoverobjects[index].hitpoints;
            if (!(takeoverobjects[index].contact_index != 0xff && (controller = Player[takeoverobjects[index].contact_index]) != NULL)) {
                SnapCreaturePos(takeoverobjects[index].object, &takeoverobjects[index].last_safe_position, takeoverobjects[index].heading,
                                NULL, 1);
            } else {
                takeoverobjects[index].object->dead = 0;
                takeoverobjects[index].object->flags_1fc_word |= APIOBJECT_FLAG_CHARACTER;
                takeoverobjects[index].object->flags_1fc_word |= APIOBJECT_FLAG_IN_USE;
                takeoverobjects[index].object->ai_packet.reset_mode = AIPACKET_RESET_ACTIVE;
                SnapCreaturePos(takeoverobjects[index].object, &controller->position, controller->unknown_276,
                                &controller->ai_packet.path_info, 1);
                InitPlayerAI(controller);
                TakeOverGameObject(controller, takeoverobjects[index].object, 0, 1);
            }
        }
    }
}

TAG_TRANSFER TagTransfers[2];

void Tag_ResetTransfers(void) {
    TagTransfers[0].elapsed = 0.5f;
    TagTransfers[1].elapsed = 0.5f;
}

i32 Tag_DoneAny = 0;

void Tag_NewTransfer(GAMEOBJECT *source, GAMEOBJECT *target) {
    i32 particle;
    f32 height;
    f32 lower;
    NUVEC *previous;
    if (target->unknown_280 == 0 || target->unknown_280 == 1) {
        TagTransfers[target->player_index].source = source;
        TagTransfers[target->player_index].elapsed = 0.0f;
        for (particle = 0; particle < 3; ++particle) {
            height = TagTransfers[target->player_index].height_fraction[particle] =
                (f32)((f64)qrand() * PlayerRandomScale * (f64)0.4f + (f64)0.4f);
            previous = &TagTransfers[target->player_index].previous[particle];
            previous->x = source->collision_position.x;
            previous->z = source->collision_position.z;
            lower = source->collision_min.y;
            previous->y = (f32)((f64)lower + ((f64)source->collision_max.y - lower) * height);
        }
    }
    if ((target->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
        if (Tag_DoneFirst == 0) {
            Tag_DoneFirst = 1;
        } else if (Tag_DoneFirst == 1) {
            Tag_DoneFirst = 2;
        }
        Tag_DoneAny = 1;
    }
}

void Tag_UpdateTransfers(i32 effect0, i32 effect1, i32 count) {
    i32 player;
    i32 particle;
    i32 angle;
    f32 elapsed;
    f32 phase;
    f32 height;
    f32 lower;
    TAG_TRANSFER *transfer;
    GAMEOBJECT *source;
    GAMEOBJECT *target;
    NUVEC from;
    NUVEC to;
    NUVEC position;
    for (player = 0; player < 2; ++player) {
        transfer = &TagTransfers[player];
        if (Player[player] == NULL) {
            transfer->elapsed = 0.5f;
            continue;
        }
        if (!(transfer->elapsed < 0.5f)) {
            continue;
        }
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

f32 BUCK_RIDERJUMPCLEARANCE = 1.0f;

void Buck_StartRiderJump(GAMEOBJECT *rider, GAMEOBJECT *mount) {
    u16 angle = (u16)qrand();
    f32 speed = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.09f + (f64)0.01f);
    f32 clearance;
    PLAYERCHARACTERCONFIG *character;
    rider->velocity.x = NuTrigTable[angle >> 1] * speed;
    rider->velocity.z = speed * NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    StartJump(rider, 0);
    rider->player.airborne_collision_target = mount;
    rider->player.tag_flags |= 1;
    rider->velocity.x = 0.0f;
    rider->velocity.z = 0.0f;
    rider->player.pad->buttons_held = 0;
    rider->player.pad->buttons_pressed = 0;
    clearance = mount->upper_position.y;
    if ((mount->character_data->game_character->flags_094 & CHARACTER_OPTION_BUCK_RIDER) != 0) {
        clearance += BUCK_RIDERJUMPCLEARANCE;
    }
    clearance -= rider->lower_position.y;
    clearance = (f32)((f64)clearance + (f64)0.1f);
    character = rider->character_data->game_character;
    if (clearance > character->jump_height) {
        rider->velocity.y = NuFsqrt((f32)(-2.0 * (f64)character->gravity * clearance));
    }
}

TAG_DRAW_ICON BuckStartExtraFn = 0;

void Buck_Start(GAMEOBJECT *object, GAMEOBJECT *rider) {
    CHARACTER_MODEL_LAYERS *model;
    if (LEGOCONTEXT_BUCK == -1 || LEGOACT_BUCK == -1) {
        return;
    }
    model = object->character_model;
    if (model->model_data_b[LEGOACT_BUCK] == NULL) {
        return;
    }
    if (rider != NULL) {
        NewRumble(rider->player.pad->bound_pad, 0.6f, 0);
    }
    object->player.character_context = (i8)LEGOCONTEXT_BUCK;
    object->player.unknown_7aa = LEGOACT_BUCK;
    object->player.context_timer = AnimDuration((i16)object->id, LEGOACT_BUCK, 0.0f, 0.0f, 1);
    if (BuckStartExtraFn != NULL) {
        BuckStartExtraFn(object);
    }
}

void Buck_MoveCode(GAMEOBJECT *object, i32 start) {
    if (LEGOCONTEXT_BUCK != -1 && object->player.character_context == LEGOCONTEXT_BUCK) {
        object->player.context_timer -= FRAMETIME;
        if (object->player.context_timer <= 0.0f) {
            object->player.character_context = CHARACTER_CONTEXT_NONE;
        }
    } else if (object->player.character_context == CHARACTER_CONTEXT_NONE && start != 0) {
        Buck_Start(object, object->player.takeover_source);
    }
}

void fun_005a0440(void) {
}
