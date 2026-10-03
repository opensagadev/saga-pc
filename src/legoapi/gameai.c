#include "saga.h"

#include <string.h>
#include <stdio.h>

typedef i32(__cdecl *AIACTIONEVAL_PC)(void *, void *, AIPACKET_PC *, char **, i32, i32, f32);

typedef struct AIACTIONDEF_LEVELSFX_PC {
    const char *name;
    AIACTIONEVAL_PC eval_fn;
} AIACTIONDEF_LEVELSFX_PC;

STATIC_ASSERT(offsetof(AIACTIONDEF_LEVELSFX_PC, name) == 0);
STATIC_ASSERT(offsetof(AIACTIONDEF_LEVELSFX_PC, eval_fn) == 4);

void *__cdecl AISysBufferAlloc(VARIPTR *cursor, VARIPTR *end, u32 size) {
    u32 aligned;

    if (cursor == NULL || end == NULL || cursor->addr + size >= end->addr) {
        return NULL;
    }
    aligned = (cursor->addr + 15) & ~15u;
    cursor->addr = aligned + size;
    memset((void *)aligned, 0, size);
    return (void *)aligned;
}

void GameAISysSetGame(void) {
    /* PC selects one of three game-specific AI initializers from global state. */
    STUBBED_NAMED("GameAISysSetGame: PC game selection and dispatch");
}

void GameAISysInit(void) {
    /*
     * PC registers the AI callback tables, initializes their defaults, and
     * finishes by dispatching through GameAISysSetGame. Those callback setters
     * and globals are not reconstructed yet.
     */
    STUBBED_NAMED("GameAISysInit: AI callback registration and defaults");
    GameAISysSetGame();
}

void __cdecl GameAIScriptAddLevelSfx(WORLDINFO *world, NULISTHDR *scripts) {
    NULISTLNK *script_link;

    if (scripts == NULL) {
        return;
    }

    script_link = NuLinkedListGetHead(scripts);
    while (script_link != NULL) {
        AISCRIPT_PC *script = (AISCRIPT_PC *)script_link;
        NULISTLNK *state_link = NuLinkedListGetHead(&script->states);

        while (state_link != NULL) {
            AISTATE_PC *state = (AISTATE_PC *)state_link;
            NULISTLNK *action_link = NuLinkedListGetHead(&state->actions);

            while (action_link != NULL) {
                AIACTION_PC *action = (AIACTION_PC *)action_link;
                AIACTIONDEF_LEVELSFX_PC *definition = (AIACTIONDEF_LEVELSFX_PC *)action->definition;

                if (definition != NULL && definition->eval_fn == Action_PlaySfx && action->param_count > 0) {
                    i32 index;
                    for (index = 0; index < action->param_count; ++index) {
                        char *name = NuStrIStr(action->params[index], (char *)AIScriptActionNameArgPrefix);
                        if (name != NULL) {
                            AddLevSfx(world, NULL, name + 5, -1);
                        }
                    }
                }

                action_link = NuLinkedListGetNext(&state->actions, action_link);
            }
            state_link = NuLinkedListGetNext(&script->states, state_link);
        }
        script_link = NuLinkedListGetNext(scripts, script_link);
    }
}

APIOBJECT_PC *__cdecl GetNamedAPIObject(void *system_pointer, char *name) {
    AISYS_PC *system;
    GAMEOBJECT *object;
    AICREATURE_PC *creature;
    i32 index;
    char player_name[64];
    system = (AISYS_PC *)system_pointer;
    if (system != NULL) {
        for (index = 0; index < HIGHGAMEOBJECT; index++) {
            object = &Obj[index];
            if ((object->flags_1fc & 1u) == 0)
                continue;
            if ((object->flags_1f8 & 0x400u) != 0 && object->ai_packet.creature_index != 0xff) {
                creature = &system->creatures[object->ai_packet.creature_index];
                if (creature != NULL && NuStrICmp(creature->name, name) == 0)
                    goto found_object;
            }
            if (NuStrICmp(object->character_data->name, name) == 0)
                goto found_object;
        }
    }
    goto player_fallback;
found_object:
    if (object != NULL)
        return (APIOBJECT_PC *)object;
player_fallback:
    if (NuStrICmp(name, (char *)GameAIPlayerName) == 0)
        return (APIOBJECT_PC *)player;
    if (NuStrICmp(name, (char *)GameAIPlayer2Name) == 0)
        return (APIOBJECT_PC *)(Player[0] != player ? Player[0] : Player[1]);
    for (index = 0; index < 8; index++) {
        if (Player[index] == NULL)
            continue;
        sprintf(player_name, GameAIPlayerFormat, index);
        if (NuStrICmp(player_name, name) == 0)
            return (APIOBJECT_PC *)Player[index];
    }
    return NULL;
}

NUVEC *__cdecl GetAICreatureOrigin(AISYS_PC *system, AIPACKET_PC *packet) {
    AICREATURE_PC *creature;
    NUVEC offset;
    i32 across;
    if (system == NULL || packet == NULL || packet->creature_index == 0xff)
        return NULL;
    if ((packet->navigation_flags & 8u) == 0) {
        creature = &system->creatures[packet->creature_index];
        across = ((i32)packet->group_column + 1) / 2;
        offset.x = (f32)(across * creature->x_spacing);
        if ((packet->group_column & 1) != 0)
            offset.x = -offset.x;
        offset.y = 0.0f;
        offset.z = (f32)(-creature->z_spacing * (i32)packet->group_row);
        NuVecRotateY(&offset, &offset, creature->y_rotation);
        NuVecAdd(&packet->creature_origin, &offset, &creature->position);
        packet->navigation_flags |= 8u;
    }
    return &packet->creature_origin;
}

f32 __cdecl AdaptiveDifficultyAdjustPc(ADAPTIVEDIFFICULTY_PC *difficulty, f32 amount) {
    if (difficulty != NULL && Cheat_IsOn(44) && !BonusArea && !GAMEDEMO) {
        f32 sum = amount + difficulty->accumulator;
        difficulty->accumulator = sum;
        if (sum >= 1.0f) {
            if (difficulty->level < 4) {
                ++difficulty->level;
                difficulty->accumulator = 0.5f;
                difficulty->time_elapsed = 0.0f;
            }
        } else if (sum <= 0.0f && difficulty->level > -4) {
            --difficulty->level;
            difficulty->accumulator = 0.5f;
            difficulty->time_elapsed = 0.0f;
        }
    }
    return difficulty->accumulator;
}

extern AREA_DATA_PC *BATTLEOVERCORUSCANT_ADATA;
extern f32 VehicleLoopTimePc;
i32 __cdecl TwistLevel(LEVELDATA_PC *);
void __cdecl Hint_SetComplete(i32);
void __cdecl LoopCode(GAMEOBJECT *object, i32 jump_pressed, i32 unused, GAMEPAD_PC *pad, i32 allow_loop) {
    GAMEOBJECT *other;
    i32 index, angle_difference, loop_request = 0, roll_request = 0, turn_request = 0;
    (void)unused;
    if (WORLD->area != NULL) {
        if (WORLD->area == BATTLEOVERCORUSCANT_ADATA)
            allow_loop = 0;
        else if (WORLD->area == PODSPRINT_ADATA)
            return;
    }
    if (object->character_context == 0x3a) {
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f)
            object->character_context = -1;
        return;
    }
    if (object->character_context == 0x36) {
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f) {
            object->character_context = -1;
            object->secondary_lean_angle = 0;
            object->delayed_turn_timer = .3f;
        } else {
            index = (i32)((1.0 - (f64)object->context_timer / (f64)object->context_duration) * 32768.0 + 16384.0);
            object->secondary_lean_angle =
                (u16)(i32)((1.0 - ((f64)NuTrigTable[(index >> 1) & 0x7fff] + 1.0) * .5) * 65536.0);
        }
        return;
    }
    if (!(object->character_data->game_character->field_088 > 0.0f) || object->character_context != -1)
        return;
    other = GetOtherActivePlayer(object);
    if (allow_loop != 0 && object->in_narrow_socket != 0 && other != NULL && other->in_narrow_socket != 0 &&
        other->character_context == 0x36 && other->context_timer < (f64)other->context_duration - (f64).2f &&
        other->context_timer > .3f && GoingForwardsAlongNarrowSock(other) == GoingForwardsAlongNarrowSock(object)) {
        loop_request = 2;
        jump_pressed = 1;
    } else if (object->delayed_turn_timer <= 0.0f) {
        if (TwistLevel(WORLD->current_level))
            angle_difference = RotDiff(0, pad->input_angle);
        else
            angle_difference = RotDiff(object->field_0x276, GamePad_InputAngle(object, pad));
        if (object->movement_spline_position.spline == NULL && pad->input_magnitude > 0.0f) {
            if ((u32)(angle_difference + 0x5fff) <= 0x58e2)
                roll_request = 2;
            else if ((u32)(angle_difference - 0x71d) <= 0x58e2)
                roll_request = 1;
            else if ((angle_difference < 0 ? -angle_difference : angle_difference) >= 0x4000)
                turn_request = 1;
            else
                loop_request = 1;
        } else
            loop_request = 1;
        if (allow_loop == 0) {
            loop_request = 0;
            turn_request = 0;
        }
    }
    if (jump_pressed == 0)
        return;
    if (loop_request != 0) {
        if (loop_request != 2 && object->in_narrow_socket && other != NULL &&
            (other->character_context == 0x2a || other->character_context == 0x36 || other->character_context == 0x3a))
            return;
        object->character_context = 0x36;
        object->field_7aa = 1;
        object->context_duration = object->context_timer = VehicleLoopTimePc;
        PlaySfx((char *)TurnLoopSfx, &object->collision_position);
        Hint_SetComplete(0x287);
    } else if (roll_request != 0) {
        if (object->in_narrow_socket && other != NULL &&
            (other->character_context == 0x2a || other->character_context == 0x36))
            return;
        object->context_duration = object->context_timer = .75f;
        object->character_context = 0x3a;
        object->field_7aa = 1;
        object->field_7b3 = (u8)(roll_request - 1);
        PlaySfx((char *)TurnLoopSfx, &object->collision_position);
    } else if (turn_request != 0) {
        if (object->in_narrow_socket && other != NULL &&
            (other->character_context == 0x2a || other->character_context == 0x36 || other->character_context == 0x3a))
            return;
        StartTurn(object);
    }
}
