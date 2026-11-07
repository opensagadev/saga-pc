#include "saga.h"

STATIC_ASSERT(offsetof(LEVEL_SCRIPT_PROCESS_PC, processor) + offsetof(AISCRIPTPROCESS_PC, flags) == 0x60);
STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);

void __cdecl AI_Activate(void *input, i32 active) {
    GIZMO_PC *gizmo = (GIZMO_PC *)input;
    LEVEL_SCRIPT_PROCESS_PC *process;
    if (gizmo != NULL && gizmo->object != NULL) {
        process = (LEVEL_SCRIPT_PROCESS_PC *)gizmo->object;
        process->processor.flags ^= (((active == 0) << 1) ^ process->processor.flags) & 2;
    }
}

#include <string.h>
#include <stdio.h>

typedef i32(__cdecl *AIACTIONEVAL_PC)(void *, void *, AIPACKET_PC *, char **, i32, i32, f32);

typedef struct AIACTIONDEF_LEVELSFX_PC {
    const char *name;
    AIACTIONEVAL_PC eval_fn;
} AIACTIONDEF_LEVELSFX_PC;

STATIC_ASSERT(offsetof(AIACTIONDEF_LEVELSFX_PC, name) == 0);
STATIC_ASSERT(offsetof(AIACTIONDEF_LEVELSFX_PC, eval_fn) == 4);

void *__cdecl AISysBufferAlloc(VARIPTR *, VARIPTR *, u32);

/* Native PC startup and callback providers; insert in gameai.c before GameAISysSetGame. */
typedef char *(__cdecl *AI_CHARACTER_NAME_PC)(u8);
typedef char *(__cdecl *AI_GLOBAL_CHARACTER_NAME_PC)(i32);
typedef void *(__cdecl *AI_GLOBAL_CHARACTER_HGOBJ_PC)(i32);
typedef void(__cdecl *AI_GLOBAL_CHARACTER_RENDER_PC)(NUVEC *, i16, i32, i32, void *);
typedef void(__cdecl *AI_CLEAR_CREATURES_PC)(void);
typedef APIOBJECT_PC *(__cdecl *AI_GET_NAMED_OBJECT_PC)(void *, char *);
typedef NUVEC *(__cdecl *AI_GET_CREATURE_ORIGIN_PC)(AISYS_PC *, AIPACKET_PC *);
extern AI_CHARACTER_NAME_PC LevelCharacterNameFn, SpecialRouteCharacterNameFn;
extern AI_GLOBAL_CHARACTER_NAME_PC GlobalCharacterNameFn;
extern AI_GLOBAL_CHARACTER_HGOBJ_PC GlobalCharacterHGobjFn;
extern AI_GLOBAL_CHARACTER_RENDER_PC GlobalCharacterRenderFn;
extern AI_CLEAR_CREATURES_PC ClearAICreaturesFn;

extern const char GameAIEveryonePc[];
extern const char *GameAISkipCharactersPc[4];
extern void(__cdecl *AIPathCnxHelperSysInitFn)(WORLDINFO *);
extern i32 LEGO_AIPATHCNX_VARIANT_FLAG0_PC, LEGO_AIPATHCNX_VARIANT_FLAG1_PC;
extern u32 LEGO_AIPATHCNX_BIGJUMP, LEGO_AIPATHCNX_REQUIRESPERMISSION;
extern u32 LEGO_AIPATHCNX_NO_DESTINATION_CHECK, LEGO_AIPATHCNX_JUMP_NOW, LEGO_AIPATHCNX_DONT_JUMP_NOW;
extern AIACTIONDEF_PC lego_aiactiondefs[], api_aiactiondefs[], *game_aiactiondefs;
extern AICONDITIONDEF_PC lego_aiconditiondefs[], *game_aiconditiondefs;
i32 __cdecl NuHGobjRndr(NUHGOBJ_EVAL_PC *, NUMTX *, i32, i16 *);
i32 __cdecl GameAIActionParseSpeed(char *, u8 *);
APIOBJECT_PC *__cdecl GetNamedAPIObject(void *, char *);
NUVEC *__cdecl GetAICreatureOrigin(AISYS_PC *, AIPACKET_PC *);
void __cdecl RegisterAIScriptActions(AIACTIONDEF_PC *);
void __cdecl RegisterAIScriptConditions(AICONDITIONDEF_PC *);
void __cdecl SetScriptErrorLevel(i32);
void __cdecl InitFn_LevelCharacterTypeID(AI_LEVEL_CHARACTER_TYPE_ID_PC);
void __cdecl InitFn_SpecialRouteCharacterTypeID(AI_LEVEL_CHARACTER_TYPE_ID_PC);
void __cdecl InitFn_LevelCharacterName(AI_CHARACTER_NAME_PC);
void __cdecl InitFn_SpecialRouteCharacterName(AI_CHARACTER_NAME_PC);
void __cdecl InitFn_LevelCharacterGlobalID(AI_LEVEL_CHARACTER_GLOBAL_ID_PC);
void __cdecl InitFn_GlobalCharacterTypeID(i32(__cdecl *)(char *));
void __cdecl InitFn_GlobalCharacterName(AI_GLOBAL_CHARACTER_NAME_PC);
void __cdecl InitFn_GlobalCharacterRender(AI_GLOBAL_CHARACTER_RENDER_PC);
void __cdecl InitFn_GlobalCharacterHGobj(AI_GLOBAL_CHARACTER_HGOBJ_PC);
void __cdecl InitFn_ClearAICreatures(AI_CLEAR_CREATURES_PC);
void __cdecl InitFn_GameAILoad(void(__cdecl *)(AISYS_PC *, i32, NUGSCN *, VARIPTR *, VARIPTR *));
void __cdecl InitFn_AIActionParseSpeed(AI_PARSE_SPEED_PC);
void __cdecl InitFn_AIRespawnOnPath(AI_RESPAWN_ON_PATH_PC);
void __cdecl InitFn_GetViewRange(AI_CHARACTER_DISTANCE_PC);
void __cdecl InitFn_GetHearDistance(AI_CHARACTER_DISTANCE_PC);
void __cdecl InitFn_GlobalGetMaxViewHeight(AI_CHARACTER_DISTANCE_PC);
void __cdecl InitFn_GlobalGetMinViewHeight(AI_CHARACTER_DISTANCE_PC);
void __cdecl InitFn_PreparingForSpecialMove(AI_PREPARING_SPECIAL_MOVE_PC);
void __cdecl InitFn_MidSpecialMove(AI_MID_SPECIAL_MOVE_PC);
void __cdecl InitFn_GetNamedAPIObject(AI_GET_NAMED_OBJECT_PC);
void __cdecl InitFn_GetAICreatureOrigin(AI_GET_CREATURE_ORIGIN_PC);
void __cdecl InitFn_AIBigJumpToDestination(AI_BIG_JUMP_TO_DESTINATION_PC);
void __cdecl InitFn_FindAlternativeSpecialObjectFn(AIFINDALTERNATIVESPECIALOBJECT *);
void __cdecl GameAILoad(AISYS_PC *, i32, NUGSCN *, VARIPTR *, VARIPTR *);
i32 __cdecl EquivalentObject_Find(WORLDINFO *, NUSPECIALHANDLE *);
void __cdecl ClearAICreatures(void);
void __cdecl LSW_Hub_InitAI(WORLDINFO *);
void __cdecl LSW_Hub_ResetAI(WORLDINFO *);
void __cdecl LSW_Hub_UpdateAI(WORLDINFO *);

u8 __cdecl LevelCharacterTypeID(char *name) {
    i32 index;
    if (NuStrICmp(name, (char *)GameAIEveryonePc) == 0)
        return 0x40;
    if (CurrentStoryCList != NULL) {
        for (index = 0; CurrentStoryCList[index].model_id != -1 && index < 64; ++index)
            if (NuStrICmp(name, apicharsys->char_data[CurrentStoryCList[index].model_id].name) == 0)
                return index;
    }
    return -1;
}
static __declspec(noinline) i32 __cdecl IsSpecialRouteCharacterSkippedPc(char *name) {
    i32 index;
    for (index = 0; GameAISkipCharactersPc[index] != NULL; ++index)
        if (NuStrICmp(name, (char *)GameAISkipCharactersPc[index]) == 0)
            return 1;
    return 0;
}
u8 __cdecl SpecialRouteCharacterTypeID(char *name) {
    i32 route, skipped = 0, suit, index;
    if (NuStrICmp(name, (char *)GameAIEveryonePc) == 0)
        return 0x40;
    for (suit = 0; suit < 10; ++suit)
        if (NuStrICmp(name, (char *)SuitRecordInitPc[suit].suit_character_name) == 0)
            return suit;
    if (CurrentStoryCList != NULL) {
        for (route = 0; CurrentStoryCList[route].model_id != -1 && route < 64;) {
            char *candidate = apicharsys->char_data[CurrentStoryCList[route + skipped].model_id].name;
            i32 skip = 0;
            for (index = 0; GameAISkipCharactersPc[index] != NULL; ++index) {
                if (NuStrICmp(candidate, (char *)GameAISkipCharactersPc[index]) == 0) {
                    skip = 1;
                    break;
                }
            }
            if (skip)
                ++skipped;
            else {
                if (NuStrICmp(name, apicharsys->char_data[CurrentStoryCList[route + skipped].model_id].name) == 0)
                    return route + 10;
                ++route;
            }
        }
    }
    return -1;
}
char *__cdecl LevelCharacterName(u8 index) {
    if (index != 0xff && CurrentStoryCList != NULL && CurrentStoryCList[index].model_id != -1)
        return apicharsys->char_data[CurrentStoryCList[index].model_id].name;
    return NULL;
}
char *__cdecl SpecialRouteCharacterName(u8 id) {
    i32 route = 0, skipped = 0;
    if (id == 0xff)
        return NULL;
    if (id < 10)
        return (char *)SuitRecordInitPc[id].suit_character_name;
    while (CurrentStoryCList[route + skipped].model_id != -1 && route + skipped < 64) {
        char *name = apicharsys->char_data[CurrentStoryCList[route + skipped].model_id].name;
        if (IsSpecialRouteCharacterSkippedPc(name))
            ++skipped;
        else {
            if (route + 10 == id)
                return name;
            ++route;
        }
    }
    return NULL;
}
i32 __cdecl LevelCharacterGlobalID(u8 index) {
    if (index != 0xff && CurrentStoryCList != NULL)
        return CurrentStoryCList[index].model_id;
    return -1;
}
i32 __cdecl GlobalCharacterTypeID(char *name) {
    i32 index;
    for (index = 0; index < apicharsys->character_count; ++index)
        if (NuStrICmp(name, apicharsys->char_data[index].name) == 0)
            return index;
    return -1;
}
char *__cdecl GlobalCharacterName(i32 id) {
    if (id != -1 && id < apicharsys->character_count)
        return apicharsys->char_data[id].name;
    return NULL;
}
void __cdecl GlobalCharacterRender(NUVEC *position, i16 angle, i32 character, i32 unused, void *creature) {
    i16 layers[32];
    i16 model_id;
    i32 count;
    CHARACTER_MODEL_LAYERS_PC *model;
    NUVEC scale;
    NUMTX matrix;
    (void)unused;
    (void)creature;
    if (character < CHARCOUNT) {
        model_id = apicharsys->playermodelids[character];
        if (model_id != -1) {
            model = &apicharsys->models[model_id];
            count = GCDataList[model->model_id].make_layer_list(
                model, layers, apicharsys->char_data[character].game_character->layer_mask);
            if (count > 0) {
                angle = NuAngAdd(angle, 0x8000);
                scale.x = scale.y = scale.z = CDataList[character].model_scale;
                NuMtxSetScale(&matrix, &scale);
                NuMtxTranslate(&matrix, position);
                NuMtxPreRotateY(&matrix, angle);
                NuHGobjRndr(model->hierarchy, &matrix, count, layers);
            }
        }
    }
}
void *__cdecl GlobalCharacterHGobj(i32 id) {
    i16 model;
    if (id == -1)
        return NULL;
    model = apicharsys->playermodelids[id];
    if (model == -1)
        return NULL;
    return apicharsys->models[model].hierarchy;
}
f32 __cdecl GetViewRange(i32 id) {
    if (id == -1)
        return 0.0f;
    return apicharsys->char_data[id].game_character->viewdistance;
}
f32 __cdecl GetHearDistance(i32 id) {
    if (id == -1)
        return 0.0f;
    return apicharsys->char_data[id].game_character->heardistance;
}
f32 __cdecl GetMaxViewHeight(i32 id) {
    if (id == -1)
        return 0.0f;
    return apicharsys->char_data[id].game_character->maxviewheight;
}
f32 __cdecl GetMinViewHeight(i32 id) {
    if (id == -1)
        return 0.0f;
    return apicharsys->char_data[id].game_character->minviewheight;
}

i32 __cdecl AIRespawnOnPath(APIOBJECT_PC *apiobject) {
    GAMEOBJECT *object = (GAMEOBJECT *)apiobject;
    GAMEOBJECT *owner = object->objptr;
    if (object->dead == 0 && (apiobject->ai->path_info.flags & 1) == 0 && (object->flags_1fc_word & 0x2000) != 0 &&
        (object->character_data->model_flags & 0x400000) == 0 && owner->character_context != 0x2b &&
        owner->takeover_source == NULL && object->respawn_timer > 2.0f) {
        if ((object->character_data->model_flags & 0x200000) == 0)
            StartBigJump(owner, &object->respawn_position, 0, 0.5f, 1.0f, 0);
        else {
            object->start_position = object->respawn_position;
            object->position = object->respawn_position;
            object->velocity = v000;
            ResetPlayerMoves(owner);
            object->respawn_timer = 0.0f;
        }
    }
    return 0;
}
i32 __cdecl AIBigJumpToDestination(APIOBJECT_PC *apiobject, NUVEC *destination) {
    GAMEOBJECT *object = (GAMEOBJECT *)apiobject;
    GAMEOBJECT *owner;
    if (destination == NULL || object == NULL || object->objptr == NULL || object->dead != 0 ||
        object->objptr->character_context == 0x2b)
        return 1;
    owner = object->objptr;
    if (owner->takeover_source != NULL)
        SnapCreaturePos(owner, destination, 0, NULL, 0);
    else if ((object->character_data->model_flags & 0x200000) == 0)
        StartBigJump(owner, destination, 0, 0.5f, 1.0f, 0);
    else {
        object->start_position = *destination;
        object->position = *destination;
        object->velocity = v000;
        ResetPlayerMoves(owner);
        object->respawn_timer = 0.0f;
    }
    return 1;
}
i32 __cdecl GameFindAlternativeSpecialObject(AISYS_PC *system, NUSPECIALHANDLE *special) {
    (void)system;
    return EquivalentObject_Find(WorldInfo_CurrentlyActive(), special);
}
i32 __cdecl Indy_PreparingForSpecialMove(AIPACKET_PC *, APIOBJECT_PC *, i32);
i32 __cdecl Indy_MidSpecialMove(AISYS_PC *, AIPACKET_PC *, APIOBJECT_PC *);
i32 __cdecl StarWars_PreparingForSpecialMove(AIPACKET_PC *, APIOBJECT_PC *, i32);
i32 __cdecl StarWars_MidSpecialMove(AISYS_PC *, AIPACKET_PC *, APIOBJECT_PC *);
i32 __cdecl Batman_PreparingForSpecialMove(AIPACKET_PC *, APIOBJECT_PC *, i32);
i32 __cdecl Batman_MidSpecialMove(AISYS_PC *, AIPACKET_PC *, APIOBJECT_PC *);
void __cdecl Indy_AIPathCnxHelperSysInit(WORLDINFO *);
void __cdecl StarWars_GameAISysInit(void);
void __cdecl Indy_GameAISysInit(void) {
    LEGO_AIPATHCNX_JUMP = 0x1;
    LEGO_AIPATHCNX_FORGOODIES = 0x100;
    LEGO_AIPATHCNX_FORBADDIES = 0x200;
    LEGO_AIPATHCNX_BIGJUMP = 0x4;
    LEGO_AIPATHCNX_NO_DESTINATION_CHECK = 0x20;
    LEGO_AIPATHCNX_JUMP_NOW = 0x40;
    LEGO_AIPATHCNX_DONT_JUMP_NOW = 0x80;
    LEGO_AIPATHCNX_VARIANT_FLAG0_PC = 0x10;
    InitFn_PreparingForSpecialMove(Indy_PreparingForSpecialMove);
    InitFn_MidSpecialMove(Indy_MidSpecialMove);
    HubInitAIFnPc = LSW_Hub_InitAI;
    HubResetAIFnPc = LSW_Hub_ResetAI;
    HubUpdateAIFnPc = LSW_Hub_UpdateAI;
    AIPathCnxHelperSysInitFn = Indy_AIPathCnxHelperSysInit;
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
void GameAISysSetGame(void) {
    AIPathCnxHelperSysInitFn = NULL;
    if (StartupGameFlag0 != 0)
        Indy_GameAISysInit();
    else if (StartupGameFlag1 != 0)
        StarWars_GameAISysInit();
    else
        Batman_GameAISysInit();
}
void GameAISysInit(void) {
    RegisterAIScriptActions(lego_aiactiondefs);
    RegisterAIScriptConditions(lego_aiconditiondefs);
    InitFn_LevelCharacterTypeID(LevelCharacterTypeID);
    InitFn_SpecialRouteCharacterTypeID(SpecialRouteCharacterTypeID);
    InitFn_LevelCharacterName(LevelCharacterName);
    InitFn_SpecialRouteCharacterName(SpecialRouteCharacterName);
    InitFn_LevelCharacterGlobalID(LevelCharacterGlobalID);
    InitFn_GlobalCharacterTypeID(GlobalCharacterTypeID);
    InitFn_GlobalCharacterName(GlobalCharacterName);
    InitFn_GlobalCharacterRender(GlobalCharacterRender);
    InitFn_GlobalCharacterHGobj(GlobalCharacterHGobj);
    InitFn_ClearAICreatures(ClearAICreatures);
    InitFn_GetCharacterGoalSpeedFn(GetCharacterGoalSpeed);
    InitFn_GetViewRange(GetViewRange);
    InitFn_GetHearDistance(GetHearDistance);
    InitFn_GlobalGetMaxViewHeight(GetMaxViewHeight);
    InitFn_GlobalGetMinViewHeight(GetMinViewHeight);
    InitFn_GameAILoad(GameAILoad);
    InitFn_AIActionParseSpeed(GameAIActionParseSpeed);
    InitFn_AIRespawnOnPath(AIRespawnOnPath);
    InitFn_AIBigJumpToDestination(AIBigJumpToDestination);
    InitFn_FindAlternativeSpecialObjectFn(GameFindAlternativeSpecialObject);
    InitFn_APIOBJECTFromObjIDFn(GameAPIOBJECTFromObjID);
    InitFn_GetNamedAPIObject(GetNamedAPIObject);
    InitFn_GetAICreatureOrigin(GetAICreatureOrigin);
    default_path_heighttol = 0.2f;
    AiLoadActivationDifficultyPc = 1;
    AiLoadMinimumRespawnTimePc = 0.0f;
    AiLoadMaximumRespawnTimePc = 0.0f;
    AiLoadMinimumRespawnCountPc = 0;
    AiLoadMaximumRespawnCountPc = 0;
    SetScriptErrorLevel(1);
    GameAISysSetGame();
}

const char GameAIEveryonePc[9] = "Everyone";
const char GameAIBatmanPc[7] = "Batman";
const char GameAIRobinPc[6] = "Robin";
const char GameAIGlidePackPc[11] = "Glide_Pack";
const char *GameAISkipCharactersPc[4] = {GameAIBatmanPc, GameAIRobinPc, GameAIGlidePackPc, NULL};
AI_CHARACTER_NAME_PC LevelCharacterNameFn = 0, SpecialRouteCharacterNameFn = 0;
AI_GLOBAL_CHARACTER_NAME_PC GlobalCharacterNameFn = 0;
AI_GLOBAL_CHARACTER_HGOBJ_PC GlobalCharacterHGobjFn = 0;
AI_GLOBAL_CHARACTER_RENDER_PC GlobalCharacterRenderFn = 0;
AI_CLEAR_CREATURES_PC ClearAICreaturesFn = 0;
i32 LEGO_AIPATHCNX_VARIANT_FLAG0_PC = 0, LEGO_AIPATHCNX_VARIANT_FLAG1_PC = 0;
typedef struct EQUIVALENTOBJECTGROUP_PC {
    i16 object_count;
    i16 byte_size;
    NUSPECIALHANDLE objects[1];
} EQUIVALENTOBJECTGROUP_PC;
STATIC_ASSERT(offsetof(EQUIVALENTOBJECTGROUP_PC, objects) == 4);
STATIC_ASSERT(offsetof(WORLDINFO, equivalent_groups) == 0x46b0);
STATIC_ASSERT(offsetof(WORLDINFO, equivalent_group_count) == 0x46b4);
i32 __cdecl NuHGobjRndr(NUHGOBJ_EVAL_PC *object, NUMTX *matrix, i32 count, i16 *layers) {
    __declspec(align(16)) NUMTX matrices[256];
    NuHGobjEval(object, 0, NULL, matrices);
    return NuHGobjRndrMtxDwa(object, matrix, count, layers, matrices, NULL, 0);
}
i32 __cdecl EquivalentObject_Find(WORLDINFO *world, NUSPECIALHANDLE *special) {
    EQUIVALENTOBJECTGROUP_PC *group;
    i32 group_index, index, candidate;
    if (special != NULL && NuSpecialExistsFn(special)) {
        group = (EQUIVALENTOBJECTGROUP_PC *)world->equivalent_groups;
        if (group != NULL) {
            for (group_index = 0; group_index < world->equivalent_group_count; ++group_index) {
                for (index = 0; index < group->object_count; ++index) {
                    if (NuSpecialCompare(special, &group->objects[index])) {
                        for (candidate = 0; candidate < group->object_count; ++candidate) {
                            if (NuSpecialGetVisibilityFn(&group->objects[candidate])) {
                                *special = group->objects[candidate];
                                return 1;
                            }
                        }
                        return 0;
                    }
                }
                group = (EQUIVALENTOBJECTGROUP_PC *)((u8 *)group + group->byte_size);
            }
        }
    }
    return 0;
}
void __cdecl ClearSpecialMove(GAMEOBJECT *object) {
    object->special_move_timer_pc = 1.0f;
    object->ai_packet.special_move_node = NULL;
    object->field_1086 = 0;
    object->field_1087 = 0;
    object->special_move_source_pc = NULL;
    object->indy_guideline_state.guideline = NULL;
    object->indy_guideline_state.endpoint = 0;
}
i32 __cdecl TightRope_SnapTo(GAMEOBJECT *, NUVEC *);
i32 __cdecl TryToTeleportToNextNode(GAMEOBJECT *object, AIPATHNODE_PC *node, i32 tightrope) {
    NUVEC position;
    i32 result = 0;
    if ((object->flags_1f8 & 0x400) == 0 && (object->ai_packet.path_info.flags & 1) != 0 &&
        (object->field_f10 >= OFFSCREEN_CATCHUP_TIME ||
         (object->model_draw_result == 0 && (object->tag_flags & 2) == 0 && drop_back_in_timer > 0.0f))) {
        position = node->position;
        position.y = object->bounds_half_height + position.y;
        if (NuCameraClipTestSphere(&position, object->bounds_half_height, &NuMtxSetIdentityTemplate) != 0) {
            if (tightrope != 0) {
                if (TightRope_SnapTo(object, &node->position) != 0) {
                    object->field_1086 = 0;
                    object->field_1087 = 0;
                    object->special_move_timer_pc = 0.0f;
                    object->ai_packet.path_update_flag = 0;
                    result = 1;
                }
            } else {
                object->ai_packet.owner->position = node->position;
                object->initial_position = object->ai_packet.owner->position;
                object->collision_position = object->ai_packet.owner->position;
                plr_lastpos = object->collision_position;
                object->start_position = object->collision_position;
                object->velocity = v000;
                InitSurfaceInfo(object);
                SetObjOnSurface(object, 0);
                object->special_move_timer_pc = 0.0f;
                object->field_1086 = 0;
                object->field_1087 = 0;
                object->ai_packet.path_update_flag = 0;
                if (object->takeover_source == NULL) {
                    Player_ClearContext(object, 0);
                    Player_ResetContexts(&object->ai_player_packet_pc);
                }
                result = 1;
            }
        }
    }
    return result;
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
    object = NULL;
    if (system != NULL) {
        for (index = 0; index < HIGHGAMEOBJECT; index++) {
            if ((Obj[index].flags_1fc & 1u) == 0)
                continue;
            if ((Obj[index].flags_1f8 & 0x400u) != 0 && Obj[index].ai_packet.creature_index != 0xff) {
                creature = &system->creatures[Obj[index].ai_packet.creature_index];
                if (creature != NULL && NuStrICmp(creature->name, name) == 0) {
                    object = &Obj[index];
                    break;
                }
            }
            if (NuStrICmp(Obj[index].character_data->name, name) == 0) {
                object = &Obj[index];
                break;
            }
        }
    }
    if (object != NULL)
        return (APIOBJECT_PC *)object;
    if (NuStrICmp(name, (char *)GameAIPlayerName) == 0)
        object = player;
    else if (NuStrICmp(name, (char *)GameAIPlayer2Name) == 0)
        object = player != Player[0] ? Player[0] : Player[1];
    else {
        for (index = 0; index < 8; index++) {
            if (Player[index] == NULL)
                continue;
            sprintf(player_name, GameAIPlayerFormat, index);
            if (NuStrICmp(player_name, name) == 0) {
                object = Player[index];
                break;
            }
        }
    }
    if (object != NULL)
        return (APIOBJECT_PC *)object;
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

void GameAILoad(AISYS_PC *system, i32 version, NUGSCN *unused2, VARIPTR *buffer, VARIPTR *buffer_end) {
    /* Retail allocates through this uninitialised cursor against the system's storage end, not the caller's
       buffer. */
    VARIPTR cursor;
    char tag[8];
    i32 tag_length;
    if (version <= 11 && system->path_sys != NULL) {
        i32 path_index;
        for (path_index = 0; path_index < system->path_sys->path_count; ++path_index) {
            i32 connection;
            AIPATH_PC *path = system->path_sys->paths[path_index];
            for (connection = 0; connection < path->connection_count; ++connection) {
                AIPATHCNX_PC *entry = &path->connections[connection];
                i32 side;
                for (side = 0; side < 2; ++side) {
                    entry->traversal_flags[side] = 0;
                    entry->original_traversal_flags[side] = 0;
                }
            }
        }
    }

    tag_length = (i8)EdFileReadChar();
    if (tag_length != 0 && tag_length <= 8) {
        EdFileRead(tag, tag_length);
        if (NuStrICmp(tag, "LEGO") == 0) {
            EdFileReadInt();
            system->game_sys = AISysBufferAlloc(&cursor, &system->storage_end, 4);
        }
    }
}

f32 __cdecl AdaptiveDifficultyUpdatePc(ADAPTIVEDIFFICULTY_PC *difficulty) {
    f32 threshold;
    f32 elapsed;
    i32 increase;
    if (difficulty != NULL && Cheat_IsOn(44) != 0 && BonusArea == 0 && GAMEDEMO == 0) {
        threshold = AdaptiveDifficultyRecoveryThresholdPc;
        if (difficulty->accumulator < 1.0f && (f64)difficulty->accumulator > 1.0 - (f64)threshold) {
            increase = 1;
        } else {
            if (!(difficulty->accumulator > 0.0f) || !((f64)threshold + 0.0 > (f64)difficulty->accumulator))
                return difficulty->accumulator;
            increase = 0;
        }
        elapsed = difficulty->time_elapsed + FRAMETIME;
        difficulty->time_elapsed = elapsed;
        if (difficulty->time_elapsed >= AdaptiveDifficultyRecoveryTimePc) {
            difficulty->accumulator = 0.5f;
            if (increase != 0)
                ++difficulty->level;
            else
                --difficulty->level;
            difficulty->time_elapsed = 0.0f;
        }
    }
    return difficulty->accumulator;
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

void TurnCodeCamSafe(GAMEOBJECT *object, NUMTX *matrix) {
    f32 time;
    f32 duration;
    f32 turn_duration;
    static i32 TURNPITCH = 200;
    static i16 myang = 0;
    TURNPITCH = 200;
    myang = 20;
    time = object->context_timer;
    duration = object->context_duration;
    turn_duration = duration * 0.95f;
    if (time < turn_duration) {
        f32 phase;
        i32 index;
        f32 wave;
        const f32 pitch_duration = duration * 0.5f * 0.5f;
        if (time < pitch_duration) {
            const f32 phase = 1.0f - (1.0f / pitch_duration) * time;
            const i32 index = (i32)(phase * 16384.0f + 32768.0f + 16384.0f);
            const f32 wave = NuTrigTable[(index >> 1) & 0x7fff];
            NuMtxPreRotateX(matrix, (i16)((i32)((wave + 1.0f) * -3640.0f)));
        }
        phase = 1.0f - (1.0f / turn_duration) * time;
        index = (i32)(phase * 32768.0f + 16384.0f);
        wave = NuTrigTable[(index >> 1) & 0x7fff];
        NuMtxPreRotateZ(matrix, (i16)((i32)((1.0f - (wave + 1.0f) * 0.5f) * 32768.0f)));
    }
}

/* Native 0x0043b730, 137 bytes. */
void __cdecl GameAIResetPathConnectionsPc(AISYS_PC *system) {
    AIPATH_PC *path;
    AIPATHCNX_PC *connection;
    i32 path_index, connection_index;
    if (system->path_sys != NULL) {
        for (path_index = 0; path_index < system->path_sys->path_count; ++path_index) {
            path = system->path_sys->paths[path_index];
            for (connection_index = 0; connection_index < path->connection_count; ++connection_index) {
                connection = &path->connections[connection_index];
                connection->traversal_flags[0] = connection->original_traversal_flags[0];
                if (LEGO_AIPATHCNX_BLOCKAGE != 0)
                    connection->traversal_flags[0] &= ~LEGO_AIPATHCNX_BLOCKAGE;
                connection->traversal_flags[1] = connection->original_traversal_flags[1];
                if (LEGO_AIPATHCNX_BLOCKAGE != 0)
                    connection->traversal_flags[1] &= ~LEGO_AIPATHCNX_BLOCKAGE;
            }
        }
    }
}

STATIC_ASSERT(offsetof(GAMEOBJECT, special_move_look_position_pc) == 0x1098);
STATIC_ASSERT(offsetof(GAMEOBJECT, launch_origin) == 0x754);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);
STATIC_ASSERT(offsetof(GAMEOBJECT, portal_flags) == 0xeec);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, special_move_node) == 0x44c);

void __cdecl SetSpecialMove(GAMEOBJECT *object, AIPATHNODE_PC *node, AIPATHNODE_PC *source, i8 type) {
    object->special_move_timer_pc = 0.0f;
    object->ai_packet.special_move_node = node;
    object->field_1086 = (u8)type;
    object->field_1087 = 0;
    object->special_move_source_pc = source;
}

i32 __cdecl AI_GetOutput(void *input, i32 output, i32 mode) {
    GIZMO_PC *gizmo = (GIZMO_PC *)input;
    (void)output;
    (void)mode;
    if (gizmo != NULL && gizmo->object != NULL)
        return (~(((LEVEL_SCRIPT_PROCESS_PC *)gizmo->object)->processor.flags >> 1)) & 1;
    return 0;
}

/* Studs the level's active AI characters drop (TotalLevelCoinTally); Gonk
 * droids and characters flagged 0x40 count for nothing. */
u32 __cdecl GameAI_TotalScore(void) {
    GAMEOBJECT *object = Obj;
    PLAYERCHARACTERCONFIG_PC *config;
    u32 total = 0;
    i32 i;
    for (i = 0; i < HIGHGAMEOBJECT; i++, object++) {
        if ((object->flags_1fc & 1) != 0 && (object->flags_1f8 & 0x400) != 0 &&
            object->ai_packet.creature_index != 0xff) {
            config = object->character_data->game_character;
            if ((config->flags_090 & 0x40) == 0 && object->id != id_GONKDROID)
                total += (u16)config->coin_value;
        }
    }
    return total;
}
