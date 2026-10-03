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
