#include "saga.h"

STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);
STATIC_ASSERT(offsetof(GIZBOMBGEN_s, name) == 0);

typedef struct bombgen_activation_reader_pc_s {
    u8 unknown_00[0x1c];
    GAMEANIMSET *anim_set;
    u8 unknown_20[0xa];
    u16 flags;
} BOMBGEN_ACTIVATION_READER_PC;
STATIC_ASSERT(offsetof(BOMBGEN_ACTIVATION_READER_PC, anim_set) == 0x1c);
STATIC_ASSERT(offsetof(BOMBGEN_ACTIVATION_READER_PC, flags) == 0x2a);
STATIC_ASSERT(sizeof(BOMBGEN_ACTIVATION_READER_PC) == 0x2c);

void __cdecl GizmoBombGen_Activate(GIZMO_PC *gizmo, i32 active) {
    BOMBGEN_ACTIVATION_READER_PC *generator;
    if (gizmo != NULL) {
        generator = (BOMBGEN_ACTIVATION_READER_PC *)gizmo->object;
        generator->flags ^= (u16)((active != 0) ^ generator->flags) & 1;
    }
}

void __cdecl GizmoBombGen_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    BOMBGEN_ACTIVATION_READER_PC *generator;
    if (gizmo != NULL && gizmo->object != NULL) {
        generator = (BOMBGEN_ACTIVATION_READER_PC *)gizmo->object;
        GameAnimSet_SetVisibility(generator->anim_set, visible);
        generator->flags ^= (u16)(((visible != 0) << 1) ^ generator->flags) & 2;
    }
}

char *__cdecl GizmoBombGen_GetGizmoName(GIZMO_PC *gizmo) {
    if (gizmo != NULL)
        return ((GIZBOMBGEN_s *)gizmo->object)->name;
    return NULL;
}

extern i32 obstacle_gizmotype_id;
extern const char GizObstacleActionNamePC[5];
extern const char GizObstacleActionBackwardPC[9];
extern const char GizObstacleActionForwardPC[8];
extern const char GizObstacleActionStayOpenPC[9];
extern const char GizObstacleActionStayShutPC[9];
extern const char GizmoSnapOptionNamePc[5];

void __cdecl GizObstacle_JumpToStart(GIZOBSTACLE *obstacle) {
    if (obstacle && obstacle->anim_set)
        GameAnimSet_JumpToStart(obstacle->anim_set);
}
void __cdecl GizObstacle_JumpToEnd(GIZOBSTACLE *obstacle) {
    if (obstacle && obstacle->anim_set)
        GameAnimSet_JumpToEnd(obstacle->anim_set);
}
void __cdecl GizObstacle_PlayForwards(GIZOBSTACLE *obstacle) {
    if (obstacle) {
        GameAnimSet_SetRepeating(obstacle->anim_set, obstacle->playback_state == 2);
        if (obstacle->animation_speed < 0.0f)
            GameAnimSet_Play(obstacle->anim_set, obstacle->animation_speed * obstacle->backward_speed_factor, 0);
        else
            GameAnimSet_Play(obstacle->anim_set, obstacle->animation_speed * obstacle->forward_speed_factor, 0);
    }
}
void __cdecl GizObstacle_PlayBackwards(GIZOBSTACLE *obstacle) {
    if (obstacle) {
        GameAnimSet_SetRepeating(obstacle->anim_set, obstacle->playback_state == 2);
        if (obstacle->animation_speed < 0.0f)
            GameAnimSet_Play(obstacle->anim_set, -obstacle->animation_speed * obstacle->forward_speed_factor, 0);
        else
            GameAnimSet_Play(obstacle->anim_set, -obstacle->animation_speed * obstacle->backward_speed_factor, 0);
    }
}
void __cdecl GizActions_PlayObstacle(GIZFLOW_PC *flow, FLOWBOX_PC *box, char **params, i32 count) {
    char *name = NULL;
    i32 forwards = 1, snap = 0, stay_open = 0, stay_shut = 0, i;
    GIZMO_PC *gizmo;
    GIZOBSTACLE *obstacle;
    u32 flags;
    for (i = 0; i < count; ++i) {
        char *value = NuStrIStr(params[i], (char *)GizObstacleActionNamePC);
        if (value)
            name = value + NuStrLen(GizObstacleActionNamePC) + 1;
        else if (!NuStrICmp(params[i], GizObstacleActionBackwardPC))
            forwards = 0;
        else if (!NuStrICmp(params[i], GizObstacleActionForwardPC))
            forwards = 1;
        else if (!NuStrICmp(params[i], GizmoSnapOptionNamePc))
            snap = 1;
        else if (!NuStrICmp(params[i], GizObstacleActionStayOpenPC))
            stay_open = 1;
        else if (!NuStrICmp(params[i], GizObstacleActionStayShutPC))
            stay_shut = 1;
    }
    if (!name)
        return;
    gizmo = (GIZMO_PC *)GizmoFindByName(flow->gizmo_sys, obstacle_gizmotype_id, name);
    obstacle = gizmo ? (GIZOBSTACLE *)gizmo->object : NULL;
    if (!obstacle)
        return;
    if (forwards) {
        if (snap)
            GizObstacle_JumpToEnd(obstacle);
        else
            GizObstacle_PlayForwards(obstacle);
    } else if (snap)
        GizObstacle_JumpToStart(obstacle);
    else
        GizObstacle_PlayBackwards(obstacle);
    flags = obstacle->runtime_flags;
    flags &= ~0xcu;
    flags |= (stay_shut & 1) << 3;
    flags |= stay_open << 2;
    obstacle->runtime_flags = flags;
}

void GizActions_PlayRadio(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int count) {
    char *special_name;
    char *blowup_name;
    i32 play;
    i32 index;
    if (count <= 0) {
        return;
    }
    special_name = NULL;
    blowup_name = NULL;
    play = 1;
    for (index = 0; index < count; ++index) {
        char *value = NuStrIStr(params[index], "BlowUp=");
        if (value != NULL) {
            blowup_name = value + NuStrLen("BlowUp=");
        } else if ((value = NuStrIStr(params[index], "Special=")) != NULL) {
            special_name = value + NuStrLen("Special=");
        } else if (NuStrICmp(params[index], "FALSE") == 0) {
            play = 0;
        }
    }
    if (special_name != NULL || blowup_name != NULL) {
        PlayRadio(special_name, blowup_name, play);
    }
}

STATIC_ASSERT(offsetof(WORLDINFO, progress_flag_5174) == 0x5170);

static void GizActions_ActivateBelt(GIZFLOW_PC *unused0, FLOWBOX_PC *unused1, char **params, int count) {
    i32 index;
    i32 active = 1;
    for (index = 0; index < count; ++index) {
        if (NuStrICmp(params[index], "FALSE") == 0) {
            active = 0;
        } else if (NuStrICmp(params[index], "TRUE") == 0) {
            active = 1;
        }
    }
    WorldInfo_CurrentlyActive()->progress_flag_5174 = ((i8)(active));
}

extern const char GizActionNameSetVisibilityPc[14];
extern const char GizActionNameSetGizmoVisibilityPc[19];
extern const char GizActionNameSetPickupVisibilityPc[20];
extern const char GizActionNameActivateGizmoPc[14];
extern const char GizActionNameActivateCharPc[13];
extern const char GizActionNameTurnOnFlowBoxPc[14];
extern const char GizActionNameActivateEffectPc[15];
extern const char GizActionNameActivatePartEffectPc[19];
extern const char GizActionNameChangeTechnoTargetPc[19];
extern const char GizActionNameSetAIMessagePc[13];
extern const char GizActionNameSetAIStatePc[11];
extern const char GizActionNameCompleteLevelPc[14];
extern const char GizActionNameGoToNewLevelPc[13];
extern const char GizActionNameGoThroughDoorPc[14];
extern const char GizActionNamePlayObstaclePc[13];
extern const char GizActionNamePlaySpecialPc[12];
extern const char GizActionNamePlayForcePc[10];
extern const char GizActionNameChangeObstTriggerTypePc[22];
extern const char GizActionNamePlayRadioPc[10];
extern const char GizActionNamePlayCutscenePc[13];
extern const char GizActionNameHitBlowupPc[10];
extern const char GizActionNameActivateBeltPc[13];
extern const char GizActionNameEnableSockPc[11];
extern void __cdecl GizActions_SetVisibilityPc(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);
extern void __cdecl GizActions_SetGizmoVisibilityPc(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);
extern void __cdecl GizActions_SetPickupVisibilityPc(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);
extern void __cdecl GizActions_ActivateGizmoPc(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);
extern void __cdecl GizActions_ActivateCharPc(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);
extern void __cdecl GizActions_TurnOnFlowBoxPc(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);
extern void __cdecl GizAction_ActivateEffect(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);
extern void __cdecl GizAction_ActivatePartEffect(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);
extern void __cdecl GizAction_ChangeTechnoTgt(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);
extern void __cdecl GizAction_SetAIMessage(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);
extern void __cdecl GizAction_SetAIState(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);
extern void __cdecl GizActions_CompleteLevel(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);
extern void __cdecl GizActions_GoToNewLevelPc(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);
extern void __cdecl GizActions_GoThroughDoor(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);
extern void __cdecl GizActions_PlaySpecialPc(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);
extern void __cdecl GizActions_PlayForcePc(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);
extern void __cdecl GizActions_ChangeObstTriggerType(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);
extern void __cdecl GizActions_PlayCutscenePc(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);
extern void __cdecl GizActions_HitBlowup(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);
extern void __cdecl GizActions_EnableSock(GIZFLOW_PC *, FLOWBOX_PC *, char **, i32);

GIZACTIONDEF_PC game_gizactiondefs[24] = {
    {GizActionNameSetVisibilityPc, GizActions_SetVisibilityPc},
    {GizActionNameSetGizmoVisibilityPc, GizActions_SetGizmoVisibilityPc},
    {GizActionNameSetPickupVisibilityPc, GizActions_SetPickupVisibilityPc},
    {GizActionNameActivateGizmoPc, GizActions_ActivateGizmoPc},
    {GizActionNameActivateCharPc, GizActions_ActivateCharPc},
    {GizActionNameTurnOnFlowBoxPc, GizActions_TurnOnFlowBoxPc},
    {GizActionNameActivateEffectPc, GizAction_ActivateEffect},
    {GizActionNameActivatePartEffectPc, GizAction_ActivatePartEffect},
    {GizActionNameChangeTechnoTargetPc, GizAction_ChangeTechnoTgt},
    {GizActionNameSetAIMessagePc, GizAction_SetAIMessage},
    {GizActionNameSetAIStatePc, GizAction_SetAIState},
    {GizActionNameCompleteLevelPc, GizActions_CompleteLevel},
    {GizActionNameGoToNewLevelPc, GizActions_GoToNewLevelPc},
    {GizActionNameGoThroughDoorPc, GizActions_GoThroughDoor},
    {GizActionNamePlayObstaclePc, GizActions_PlayObstacle},
    {GizActionNamePlaySpecialPc, GizActions_PlaySpecialPc},
    {GizActionNamePlayForcePc, GizActions_PlayForcePc},
    {GizActionNameChangeObstTriggerTypePc, GizActions_ChangeObstTriggerType},
    {GizActionNamePlayRadioPc, GizActions_PlayRadio},
    {GizActionNamePlayCutscenePc, GizActions_PlayCutscenePc},
    {GizActionNameHitBlowupPc, GizActions_HitBlowup},
    {GizActionNameActivateBeltPc, GizActions_ActivateBelt},
    {GizActionNameEnableSockPc, GizActions_EnableSock},
    {NULL, NULL},
};

void GameRegisterGizActions(void) {
    RegisterGizActions(game_gizactiondefs);
}

typedef struct ACTIVATE_CHAR_BUILDIT_VIEW_PC {
    u8 unknown_00[0x34];
    NUVEC position;
    u8 unknown_40[0x34];
    u16 angle;
} ACTIVATE_CHAR_BUILDIT_VIEW_PC;
STATIC_ASSERT(offsetof(ACTIVATE_CHAR_BUILDIT_VIEW_PC, position) == 0x34);
STATIC_ASSERT(offsetof(ACTIVATE_CHAR_BUILDIT_VIEW_PC, angle) == 0x74);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, creature_index) == 0x400);
STATIC_ASSERT(sizeof(AICREATURE_PC) == 0xa4);
extern i32 gizbuildit_gizmotype_id;
GAMEOBJECT *__cdecl ActivateCharacter(char *, NUVEC *, i32);
void __cdecl DeactivateCharacter(char *);
void __cdecl GizBuildIt_KillParts(GIZBUILDIT_s *);

void __cdecl GizActions_ActivateCharPc(GIZFLOW_PC *flow, FLOWBOX_PC *unused, char **params, i32 count) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    char *name = NULL;
    i32 active = 1;
    ACTIVATE_CHAR_BUILDIT_VIEW_PC *buildit = NULL;
    i32 index;
    GAMEOBJECT *player;
    AICREATURE_PC *creature;
    (void)unused;
    for (index = 0; index < count; ++index) {
        char *value = NuStrIStr(params[index], "name=");
        if (value != NULL)
            name = value + 5;
        else if (NuStrICmp(params[index], "TRUE") == 0)
            active = 1;
        else if (NuStrICmp(params[index], "FALSE") == 0)
            active = 0;
        else if ((value = NuStrIStr(params[index], "buildit=")) != NULL) {
            GIZMO_PC *gizmo = GizmoFindByName(flow->gizmo_sys, gizbuildit_gizmotype_id, value + 8);
            if (gizmo != NULL && gizmo->object != NULL)
                buildit = (ACTIVATE_CHAR_BUILDIT_VIEW_PC *)gizmo->object;
        }
    }
    if (active != 0 && name != NULL) {
        if (ActivateCharacter(name, buildit != NULL ? &buildit->position : NULL,
                              buildit != NULL ? buildit->angle : 0) == NULL &&
            buildit != NULL)
            GizBuildIt_KillParts((GIZBUILDIT_s *)buildit);
    } else if (active == 0 && name != NULL) {
        player = (GAMEOBJECT *)Player[0];
        if (player != NULL && (player->flags_1fc_word & 0x1000) != 0 && player->dead == 0 &&
            player->ai_packet.creature_index != 0xff) {
            creature = &world->ai_sys->creatures[player->ai_packet.creature_index];
            if (creature != NULL && NuStrICmp(creature->name, name) == 0)
                ReleaseTakeOver((GAMEOBJECT *)Player[0], 0);
        }
        player = (GAMEOBJECT *)Player[1];
        if (player != NULL && (player->flags_1fc_word & 0x1000) != 0 && player->dead == 0 &&
            player->ai_packet.creature_index != 0xff) {
            creature = &world->ai_sys->creatures[((GAMEOBJECT *)Player[0])->ai_packet.creature_index];
            if (creature != NULL && NuStrICmp(creature->name, name) == 0)
                ReleaseTakeOver((GAMEOBJECT *)Player[1], 0);
        }
        DeactivateCharacter(name);
    }
}

i32 GizBombGens_Load(void *world_ptr, void *system_ptr) {
    const u8 version = (u8)(EdFileReadChar());
    GIZBOMBGENSYS_s *system = (GIZBOMBGENSYS_s *)(system_ptr);
    system->count = (u16)(EdFileReadShort());
    if (system->count != 0) {
        i32 index = 0;
        GIZBOMBGEN_s *bomb_generator = system->bomb_generators;
        do {
            EdFileRead(bomb_generator->name, sizeof(bomb_generator->name));
            EdFileReadNuVec(&bomb_generator->position);
            bomb_generator->interval = EdFileReadInt();
            bomb_generator->flags |= GIZBOMBGEN_FLAG_ACTIVE | GIZBOMBGEN_FLAG_VISIBLE;
            GizmoFileReadGameAnimSet(bomb_generator->anim_set, world_ptr, NULL, version, (char *)("BombGenerator"),
                                     bomb_generator->name);
            ++index;
            ++bomb_generator;
        } while (system->count > index);
    }
    return 1;
}

extern void *__cdecl GizmoBufferAlloc(VARIPTR *, VARIPTR *, i32);
typedef struct gizobstacle_system_pc_s {
    GIZOBSTACLE *obstacles;
    GIZMO_PC **active_gizmos;
    u16 count, capacity, active_gizmo_count, unknown_00e;
    GAMEANIMOBJPOOL *anim_pool;
} GIZOBSTACLE_SYSTEM_PC;
STATIC_ASSERT(sizeof(GIZOBSTACLE_SYSTEM_PC) == 0x14);
STATIC_ASSERT(offsetof(GIZOBSTACLE_SYSTEM_PC, count) == 8);
STATIC_ASSERT(offsetof(GIZOBSTACLE_SYSTEM_PC, capacity) == 0xa);
STATIC_ASSERT(offsetof(GIZOBSTACLE_SYSTEM_PC, active_gizmo_count) == 0xc);
STATIC_ASSERT(offsetof(GIZOBSTACLE_SYSTEM_PC, anim_pool) == 0x10);
typedef struct gizobstacle_progress_pc_s {
    u32 enabled[4], visible[4], pickups_spawned[4], ai_active[4], blocked[4], trigger_latched[4], destroyed[4];
} GIZOBSTACLE_PROGRESS_PC;
STATIC_ASSERT(sizeof(GIZOBSTACLE_PROGRESS_PC) == 0x70);

void __cdecl GizmoObstacle_Activate(GIZMO_PC *ptr, i32 activate) {
    GIZMO_PC *gizmo = (GIZMO_PC *)ptr;
    GIZOBSTACLE *obstacle;
    if (!gizmo)
        return;
    obstacle = (GIZOBSTACLE *)gizmo->object;
    if (activate && !(obstacle->progress_flags & 1))
        GameAnimSet_JumpToStart(obstacle->anim_set);
    obstacle->proximity_output = 0;
    obstacle->progress_flags = (obstacle->progress_flags & ~1u) | (activate != 0);
    if (activate) {
        obstacle->runtime_flags &= ~0x80u;
        obstacle->progress_flags &= ~0x100u;
        GameAnimSet_EvaluateState(obstacle->anim_set);
    }
}
void __cdecl GizmoObstacle_SetVisibility(GIZMO_PC *ptr, i32 visibility) {
    GIZMO_PC *gizmo = (GIZMO_PC *)ptr;
    GIZOBSTACLE *obstacle = gizmo ? (GIZOBSTACLE *)gizmo->object : NULL;
    if (!obstacle)
        return;
    GameAnimSet_SetVisibility(obstacle->anim_set, visibility);
    obstacle->progress_flags = (obstacle->progress_flags & ~2u) | ((visibility != 0) << 1);
}
NUVEC *__cdecl GizmoObstacle_GetPos(GIZMO_PC *gizmo) {
    GIZOBSTACLE *obstacle = gizmo ? (GIZOBSTACLE *)gizmo->object : NULL;
    return obstacle ? &obstacle->position : NULL;
}
char *__cdecl GizmoObstacle_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo ? (char *)gizmo->object : NULL;
}
i32 __cdecl GizmoObstacle_GetNumOutputs(GIZMO_PC *gizmo) {
    (void)gizmo;
    return 5;
}
i32 __cdecl GizmoObstacle_GetOutput(GIZMO_PC *gizmo, i32 output, i32 ignore_activation) {
    GIZOBSTACLE *obstacle = (GIZOBSTACLE *)gizmo->object;
    if (!obstacle)
        return 0;
    if ((obstacle->progress_flags & 3) != 3 && !ignore_activation)
        return 0;
    switch (output) {
        case 0:
            return obstacle->anim_set && obstacle->anim_set->state == 2;
        case 1:
            return obstacle->anim_set && obstacle->anim_set->state != 0;
        case 2:
            return obstacle->anim_set && obstacle->proximity_output != 0;
        case 3:
            return obstacle->anim_set && obstacle->anim_set->state == 0;
        case 4:
            return obstacle->anim_set && obstacle->anim_set->state == 1;
        default:
            return 0;
    }
}
i32 __cdecl GizmoObstacle_ActivateRev(GIZMO_PC *ptr, i32 activate, i32 flags) {
    GIZMO_PC *gizmo = (GIZMO_PC *)ptr;
    GIZOBSTACLE *obstacle = gizmo ? (GIZOBSTACLE *)gizmo->object : NULL;
    i32 ai_active, reverse;
    if (!obstacle)
        return 0;
    if ((obstacle->progress_flags & 0x100) && (obstacle->anim_set->state == 2 || obstacle->anim_set->state == 1))
        obstacle->progress_flags = (obstacle->progress_flags & ~0x100u) | 1;
    ai_active = (obstacle->runtime_flags >> 2) & 1;
    if (ai_active)
        obstacle->progress_flags = (obstacle->progress_flags & ~0x100u) | 1;
    if (!(flags & 1)) {
        if (activate) {
            GizObstacle_PlayBackwards((GIZOBSTACLE *)obstacle);
            obstacle->progress_flags = (obstacle->progress_flags & ~1u) | 0x100;
        } else
            obstacle->progress_flags = (obstacle->progress_flags & ~0x100u) | 1;
        return 1;
    }
    reverse = (obstacle->progress_flags >> 8) & 1;
    if (activate == reverse && (!(flags & 4) || ((obstacle->progress_flags >> 1) & 1) != activate))
        return 0;
    if (activate == 1 && ai_active)
        return 0;
    return 1;
}
void *__cdecl GizObstacles_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, sizeof(GIZOBSTACLE_PROGRESS_PC));
}
void __cdecl GizObstacles_ClearProgress(void *world, void *ptr) {
    GIZOBSTACLE_PROGRESS_PC *progress = (GIZOBSTACLE_PROGRESS_PC *)ptr;
    i32 i;
    (void)world;
    if (!progress)
        return;
    for (i = 0; i < 4; ++i) {
        progress->enabled[i] = progress->visible[i] = 0xffffffff;
        progress->pickups_spawned[i] = progress->ai_active[i] = progress->blocked[i] = 0;
        progress->trigger_latched[i] = progress->destroyed[i] = 0;
    }
}

GIZOBSTACLE *GizObstacle_FindByName(GIZOBSTACLESYS_s *system, char *name) {
    i32 index;
    if (system == NULL || name == NULL) {
        return NULL;
    }

    for (index = 0; index < system->count; ++index) {
        GIZOBSTACLE *obstacle = &system->obstacles[index];
        if (NuStrICmp(obstacle->name, name) == 0) {
            return obstacle;
        }
    }

    return NULL;
}
void __cdecl GizObstacles_StoreProgress(void *world, void *ptr, void *progress_ptr) {
    GIZOBSTACLE_SYSTEM_PC *system = (GIZOBSTACLE_SYSTEM_PC *)ptr;
    GIZOBSTACLE_PROGRESS_PC *progress = (GIZOBSTACLE_PROGRESS_PC *)progress_ptr;
    i32 i;
    GizObstacles_ClearProgress(world, progress);
    if (!progress)
        return;
    for (i = 0; i < system->count && i < 128; ++i) {
        GIZOBSTACLE *obstacle = &system->obstacles[i];
        i32 word = i >> 5;
        u32 bit = 1u << (i & 31);
        if (!(obstacle->progress_flags & 2))
            progress->visible[word] &= ~bit;
        if (!(obstacle->progress_flags & 1))
            progress->enabled[word] &= ~bit;
        if (obstacle->runtime_flags & 1)
            progress->pickups_spawned[word] |= bit;
        if (obstacle->runtime_flags & 4)
            progress->ai_active[word] |= bit;
        if (obstacle->runtime_flags & 8)
            progress->blocked[word] |= bit;
        if (obstacle->runtime_flags & 0x40)
            progress->trigger_latched[word] |= bit;
        if (obstacle->runtime_flags & 0x80)
            progress->destroyed[word] |= bit;
    }
}
void __cdecl GizObstacles_AddLevelSfx(void *world, void *ptr, i32 *ids, i32 *count, i32 maximum) {
    GIZOBSTACLE_SYSTEM_PC *system = (GIZOBSTACLE_SYSTEM_PC *)ptr;
    i32 i;
    (void)world;
    if (!system)
        return;
    for (i = 0; i < system->count; ++i) {
        GIZOBSTACLE *obstacle = &system->obstacles[i];
        if (obstacle->start_sfx_id != -1)
            AddLevelSfxFromId(obstacle->start_sfx_id, ids, count, maximum);
        if (obstacle->stop_sfx_id != -1)
            AddLevelSfxFromId(obstacle->stop_sfx_id, ids, count, maximum);
    }
}

static void __cdecl Gizobstacle_ReadAnimSetData(GAMEANIMOBJ *object, u8 version) {
    u16 fallback_data = 0;
    u16 *data;
    if (!object)
        return;
    data = (u16 *)object->object_data;
    if (!data)
        data = &fallback_data;
    if (version > 7)
        *data = (u16)EdFileReadShort();
    else if (object->flags & 2)
        *data |= 1;
}
void *__cdecl GizObstacles_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZOBSTACLE_SYSTEM_PC *system;
    i32 i;
    system = (GIZOBSTACLE_SYSTEM_PC *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, sizeof(*system));
    system->capacity = world->current_level->max_obstacles;
    system->obstacles = (GIZOBSTACLE *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, system->capacity * 0xa0);
    system->active_gizmos = (GIZMO_PC **)GameBufferAlloc(&world->giz_buffer, &world->buffer_end,
                                                         system->capacity * sizeof(*system->active_gizmos));
    system->anim_pool = GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 4,
                                                     (u16)world->current_level->max_obstacle_objects);
    for (i = 0; i < system->capacity; ++i)
        system->obstacles[i].anim_set =
            GameAnimSet_Create(&world->giz_buffer, &world->buffer_end, system->anim_pool, world->game_anim_sys);
    world->giz_obstacle_sys = system;
    return system;
}
i32 __cdecl GizObstacles_Load(void *world_ptr, void *data) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZOBSTACLE_SYSTEM_PC *system = (GIZOBSTACLE_SYSTEM_PC *)data;
    u8 version = (u8)EdFileReadChar();
    i32 index;
    GIZOBSTACLE *obstacle;
    system->count = (u16)EdFileReadShort();
    obstacle = (GIZOBSTACLE *)system->obstacles;
    for (index = 0; index < system->count; ++index, ++obstacle) {
        EdFileRead(obstacle->name, sizeof(obstacle->name));
        EdFileReadNuVec(&obstacle->position);
        if (version > 1)
            EdFileReadNuVec(&obstacle->secondary_position);
        else
            obstacle->secondary_position = obstacle->position;
        obstacle->auto_return_delay = EdFileReadFloat();
        obstacle->trigger_radius = EdFileReadFloat();
        if (version > 2) {
            EdFileReadNuVec(&obstacle->trigger_box_half_extents);
            obstacle->trigger_box_yaw = EdFileReadShort();
        } else
            obstacle->trigger_box_half_extents.x = obstacle->trigger_box_half_extents.y =
                obstacle->trigger_box_half_extents.z = .25f;
        obstacle->config_flags = EdFileReadInt();
        if (version >= 12)
            obstacle->exclude_flags = EdFileReadInt();
        if (version == 6) {
            EdFileReadShort();
            EdFileReadChar();
        }
        obstacle->playback_state = EdFileReadChar();
        obstacle->mode = EdFileReadChar();
        obstacle->trigger_mode = version >= 7 ? (u8)EdFileReadChar() : 0xff;
        GizmoFileReadGameAnimSet(obstacle->anim_set, world, Gizobstacle_ReadAnimSetData, version,
                                 (char *)GizObstacleTypeNamePc, obstacle->name);
        obstacle->forward_speed_factor = version >= 4 ? EdFileReadFloat() : 1.f;
        obstacle->backward_speed_factor = version >= 5 ? EdFileReadFloat() : obstacle->forward_speed_factor;
        if (version >= 8)
            obstacle->reflection_alpha = EdFileReadFloat();
        obstacle->blowup_type = -1;
        if (version >= 9) {
            if (version >= 10) {
                char name[32];
                i32 length = (i8)EdFileReadChar();
                if (length) {
                    EdFileRead(name, length);
                    obstacle->blowup_type = (i16)GizmoBlowupGetNameTableId(name);
                    if (obstacle->blowup_type != -1)
                        obstacle->runtime_flags |= 2;
                }
            } else
                obstacle->blowup_type = EdFileReadShort();
            obstacle->pickup_count = EdFileReadShort();
            obstacle->pickup_direction_x = EdFileReadShort();
            obstacle->pickup_direction_y = EdFileReadShort();
            EdFileReadNuVec(&obstacle->pickup_offset);
        }
        obstacle->pickup_scatter_height =
            version >= 11 ? EdFileReadFloat() : (world->area && (world->area->flags & 1) ? 12.f : 1.75f);
        obstacle->start_sfx_id = obstacle->stop_sfx_id = -1;
        if (version >= 13) {
            char name[32];
            if (GizmoFileReadName(name))
                obstacle->start_sfx_id = (i16)GetSfxId(name);
        } else if (GizObstacle_SetDefaultSFXFn)
            GizObstacle_SetDefaultSFXFn(world, (GIZOBSTACLE *)obstacle);
        if (version >= 14) {
            char name[32];
            if (GizmoFileReadName(name))
                obstacle->stop_sfx_id = (i16)GetSfxId(name);
        }
    }
    return 1;
}
void __cdecl GizObstacles_PostLoad(void *world, void *data) {
    GIZOBSTACLE_SYSTEM_PC *system = (GIZOBSTACLE_SYSTEM_PC *)data;
    i32 i;
    GIZOBSTACLE *obstacle;
    if (!system)
        return;
    obstacle = (GIZOBSTACLE *)system->obstacles;
    for (i = 0; i < system->count; ++i, ++obstacle) {
        if (obstacle->runtime_flags & 2) {
            obstacle->blowup_type = (i16)GizmoBlowupGetTypeFromNameTableId((WORLDINFO *)world, obstacle->blowup_type);
            obstacle->runtime_flags &= ~2u;
        }
    }
}

static void GizObstacle_ResetOnePrivate(WORLDINFO *world, GIZOBSTACLE *obstacle) {
    GAMEANIMSET *set;
    GAMEANIMOBJ *object;
    obstacle->progress_flags = (obstacle->progress_flags & ~0xcu) | 3;
    obstacle->runtime_flags &= 0xffffff03u;
    obstacle->animation_speed = 1.f;
    set = obstacle->anim_set;
    if (!set)
        return;
    for (object = set->objects; object; object = object->next) {
        i16 *object_data = (i16 *)object->object_data;
        object_data[1] = -1;
        if (world->terrain && !(object_data[0] & 1) && NuSpecialExistsFn(&object->special)) {
            object_data[1] = (i16)FindPlatInst(NuSpecialGetInstanceix(&object->special));
            if (object_data[1] != -1)
                obstacle->runtime_flags |= 0x10;
        }
        if (object_data[0] & 2)
            obstacle->runtime_flags |= 0x20;
    }
    obstacle->room_id = world->current_gscn ? (i16)NuPortalWhichRoom(world->current_gscn, &obstacle->position) : -1;
    if (obstacle->config_flags & 2)
        GameAnimSet_GetAveragePos(obstacle->anim_set, &obstacle->secondary_position, 0, 1, 1);
    GameAnimSet_EvaluateState(obstacle->anim_set);
    obstacle->evaluated_radius = 1.f;
    obstacle->evaluated_position = obstacle->position;
    GameAnimSet_GetCentreAndRadius(obstacle->anim_set, &obstacle->evaluated_position, &obstacle->evaluated_radius, 2, 1,
                                   1);
}
void __cdecl GizObstacles_Reset(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZOBSTACLE_SYSTEM_PC *system = (GIZOBSTACLE_SYSTEM_PC *)data;
    GIZOBSTACLE_PROGRESS_PC *progress = (GIZOBSTACLE_PROGRESS_PC *)progress_ptr;
    i32 i;
    GIZOBSTACLE *obstacle = (GIZOBSTACLE *)system->obstacles;
    for (i = 0; i < system->count; ++i, ++obstacle) {
        GizObstacle_ResetOnePrivate(world, obstacle);
        if (progress && i < 128) {
            i32 word = i >> 5;
            u32 bit = 1u << (i & 31);
            obstacle->progress_flags = (obstacle->progress_flags & ~2u) | ((progress->visible[word] & bit) != 0) * 2;
            obstacle->progress_flags = (obstacle->progress_flags & ~1u) | ((progress->enabled[word] & bit) != 0);
            obstacle->runtime_flags = (obstacle->runtime_flags & ~1u) | ((progress->pickups_spawned[word] & bit) != 0);
            obstacle->runtime_flags = (obstacle->runtime_flags & ~4u) | ((progress->ai_active[word] & bit) != 0) * 4;
            obstacle->runtime_flags = (obstacle->runtime_flags & ~8u) | ((progress->blocked[word] & bit) != 0) * 8;
            obstacle->runtime_flags =
                (obstacle->runtime_flags & ~0x40u) | ((progress->trigger_latched[word] & bit) != 0) * 0x40;
            obstacle->runtime_flags =
                (obstacle->runtime_flags & ~0x80u) | ((progress->destroyed[word] & bit) != 0) * 0x80;
        }
    }
    GizObstacleTriggerCountPc = 0;
}

typedef struct gizobstacle_object_data_pc_s {
    i16 flags;
    i16 platform_id;
} GIZOBSTACLE_OBJECT_DATA_PC;
STATIC_ASSERT(sizeof(GIZOBSTACLE_OBJECT_DATA_PC) == 4);
STATIC_ASSERT(offsetof(GIZOBSTACLE_OBJECT_DATA_PC, flags) == 0);
STATIC_ASSERT(offsetof(GIZOBSTACLE_OBJECT_DATA_PC, platform_id) == 2);
STATIC_ASSERT(offsetof(GIZOBSTACLE, triggering_object) == 0x38);
STATIC_ASSERT(offsetof(GIZOBSTACLE, auto_return_timer) == 0x40);
extern NUVEC *GizObstacleTriggersPc[16];
extern i32(__cdecl *GizObstacleExcludeFnPc)(GIZOBSTACLE *, GAMEOBJECT *);
extern LEVELDATA_PC *ObstacleReturnLevelPc;
void __cdecl AddShoveObject(NUSPECIALHANDLE *, i16);
void __cdecl GizmoBlowUpTypeBlowUp(WORLDINFO *, i32, NUVEC *);

void __cdecl GizObstacle_Stop(GIZOBSTACLE *obstacle) {
    if (obstacle)
        GameAnimSet_Stop(obstacle->anim_set);
}
static void __cdecl GizObstacle_EvalAveragePosAndRadius(GIZOBSTACLE *obstacle, i32 state) {
    obstacle->evaluated_radius = 1.0f;
    obstacle->evaluated_position = obstacle->position;
    GameAnimSet_GetCentreAndRadius(obstacle->anim_set, &obstacle->evaluated_position, &obstacle->evaluated_radius,
                                   state, 1, 1);
}
static i32 __cdecl GizObstacle_PosWithinBox(GIZOBSTACLE *obstacle, NUVEC *position) {
    NUVEC local;
    if (obstacle->mode != 5 && obstacle->mode != 6)
        return 1;
    NuVecSub(&local, position, &obstacle->secondary_position);
    NuVecRotateY(&local, &local, -(u16)obstacle->trigger_box_yaw);
    if (local.x > obstacle->trigger_box_half_extents.x || local.x < -obstacle->trigger_box_half_extents.x)
        return 0;
    if (local.y > obstacle->trigger_box_half_extents.y || local.y < 0.0f)
        return 0;
    if (local.z > obstacle->trigger_box_half_extents.z || local.z < -obstacle->trigger_box_half_extents.z)
        return 0;
    return 1;
}
static i32 __cdecl GizObstacle_SatisfyingTerrainChecks(GIZOBSTACLE *obstacle, GAMEOBJECT *object) {
    GAMEANIMOBJ *anim_object;
    if ((i8)obstacle->trigger_mode < 0) {
        if (!(obstacle->config_flags & 0x10))
            return 1;
        if (!object->field_281 && !(object->flags_ef4 & 0x20000000))
            return 0;
        if (object->supporting_platform_id >= 0 && obstacle->anim_set && (obstacle->runtime_flags & 0x10)) {
            for (anim_object = obstacle->anim_set->objects; anim_object; anim_object = anim_object->next)
                if (((GIZOBSTACLE_OBJECT_DATA_PC *)anim_object->object_data)->platform_id ==
                    object->supporting_platform_id)
                    return 1;
        }
        return 0;
    }
    if ((!object->field_281 && !(object->flags_ef4 & 0x20000000)) ||
        obstacle->trigger_mode != object->jump_animation_state)
        return 0;
    return 1;
}
static void __cdecl GizObstacleUpdate_Proximity(GIZOBSTACLE *obstacle) {
    i32 state;
    obstacle->proximity_output = 0;
    if (!(obstacle->runtime_flags & 8)) {
        f32 closest = obstacle->trigger_radius * obstacle->trigger_radius;
        NUVEC *closest_position = NULL;
        GAMEOBJECT *closest_object = NULL;
        u32 players = 0;
        i32 i;
        GAMEOBJECT *object;
        if (obstacle->mode == 1 || obstacle->mode == 2 || obstacle->mode == 5 || obstacle->mode == 6) {
            if ((obstacle->mode == 1 || obstacle->mode == 5) && GameCam->mode != -1 && obstacle->trigger_mode == 0xff &&
                !(obstacle->config_flags & 0x4435)) {
                f32 distance = NuVecDistSqr(&GameCam->position, &obstacle->secondary_position, NULL);
                if (distance < closest && GizObstacle_PosWithinBox(obstacle, &GameCam->position)) {
                    closest = distance;
                    closest_position = &GameCam->position;
                }
            }
            if (!(obstacle->config_flags & 1)) {
                for (i = 0; i < GizObstacleTriggerCountPc; ++i) {
                    f32 distance = NuVecDistSqr(&obstacle->secondary_position, GizObstacleTriggersPc[i], NULL);
                    if (distance < closest && GizObstacle_PosWithinBox(obstacle, GizObstacleTriggersPc[i])) {
                        closest = distance;
                        closest_position = GizObstacleTriggersPc[i];
                    }
                }
            }
        }
        for (i = 0, object = Obj; i < HIGHGAMEOBJECT; ++i, ++object) {
            i8 player;
            f32 distance;
            if (!(object->flags_1fc_word & 1) || !(object->flags_1fc_word & 0x1000) || object->dead ||
                (LEGOCONTEXT_DOOMED != -1 && object->character_context == LEGOCONTEXT_DOOMED) ||
                !(object->flags_1044 & 1) || (object->flags_e20 & 0x20))
                continue;
            player = (i8)object->field_280;
            if (player != -1)
                players |= 1u << ((u8)player & 31);
            if ((obstacle->config_flags & 4) && player == -1)
                continue;
            if (((obstacle->config_flags & 0x20) && (i8)object->flags_1fc >= 0) ||
                (CInfo[object->character_context].flags & 0x8000) ||
                ((obstacle->config_flags & 0x400) && !object->rider) ||
                ((obstacle->config_flags & 0x4000) && !(object->character_data->model_flags & 4)) ||
                ((obstacle->config_flags & 8) && !(object->character_data->model_flags & 0x2000)) ||
                (GizObstacleExcludeFnPc && GizObstacleExcludeFnPc(obstacle, object)))
                continue;
            if ((obstacle->mode == 2 || obstacle->mode == 6) && !object->field_281 &&
                !(object->flags_ef4 & 0x20000000) && object->jump_variant_state != 1) {
                GAMEANIMOBJ *anim_object = obstacle->anim_set ? obstacle->anim_set->objects : NULL;
                while (anim_object) {
                    i16 platform = ((GIZOBSTACLE_OBJECT_DATA_PC *)anim_object->object_data)->platform_id;
                    if (platform != -1 && platform == object->supporting_platform_id)
                        break;
                    anim_object = anim_object->next;
                }
                if (!anim_object)
                    continue;
            }
            distance = NuVecDistSqr(&obstacle->secondary_position, &object->lower_position, NULL);
            if (distance < closest) {
                if (GizObstacle_PosWithinBox(obstacle, &object->collision_position) &&
                    GizObstacle_SatisfyingTerrainChecks(obstacle, object)) {
                    closest = distance;
                    closest_position = &object->lower_position;
                    closest_object = object;
                    if (player != -1)
                        players &= ~(1u << ((u8)player & 31));
                }
            } else if ((obstacle->config_flags & 0x2000) && player != -1 &&
                       distance < obstacle->trigger_radius * obstacle->trigger_radius &&
                       GizObstacle_PosWithinBox(obstacle, &object->collision_position) &&
                       GizObstacle_SatisfyingTerrainChecks(obstacle, object)) {
                players &= ~(1u << ((u8)player & 31));
            }
        }
        if (((obstacle->config_flags & 1) && !closest_position) ||
            (!(obstacle->config_flags & 1) && closest_position && (!(obstacle->config_flags & 0x2000) || !players))) {
            obstacle->triggering_object = closest_object;
            obstacle->proximity_output = 1;
        }
    }
    state = obstacle->anim_set->state;
    if (state != 0 && state != 2 &&
        !(WORLD->current_level == ObstacleReturnLevelPc && LevGizObst[0] == obstacle && state == 4))
        return;
    if (state == 2 && !obstacle->playback_state)
        return;
    if (!state) {
        if (obstacle->proximity_output || (obstacle->runtime_flags & 0x44)) {
            GizObstacle_PlayForwards(obstacle);
            obstacle->auto_return_timer = obstacle->auto_return_delay;
        }
    } else if (!obstacle->proximity_output && !(obstacle->runtime_flags & 0x44)) {
        if (!(obstacle->auto_return_timer > 0.0f))
            GizObstacle_PlayBackwards(obstacle);
        else if (state == 2)
            obstacle->auto_return_timer -= FRAMETIME;
    }
}
static void __cdecl GizObstacleUpdate_NoTrigger(GIZOBSTACLE *obstacle) {
    if (!(obstacle->config_flags & 0x800) && !(obstacle->runtime_flags & 4) && !obstacle->anim_set->state)
        return;
    obstacle->runtime_flags |= 8;
    GizObstacleUpdate_Proximity(obstacle);
}
static void __cdecl GizObstacleUpdate_TechnoOnly(GIZOBSTACLE *obstacle) {
    (void)obstacle;
}
static void __cdecl GizObstacleUpdate_PushOnly(GIZOBSTACLE *obstacle) {
    (void)obstacle;
}
static void __cdecl GizObstacleUpdate_AutoStart(GIZOBSTACLE *obstacle) {
    if (obstacle->config_flags & 0x800) {
        obstacle->runtime_flags |= 8;
        GizObstacleUpdate_Proximity(obstacle);
    } else if (!obstacle->anim_set->state || obstacle->anim_set->state == 3)
        GizObstacle_PlayForwards(obstacle);
}
static void(__cdecl *GizObstacleUpdateFnsPc[8])(GIZOBSTACLE *) = {
    GizObstacleUpdate_AutoStart,  GizObstacleUpdate_Proximity, GizObstacleUpdate_Proximity, GizObstacleUpdate_NoTrigger,
    GizObstacleUpdate_TechnoOnly, GizObstacleUpdate_Proximity, GizObstacleUpdate_Proximity, GizObstacleUpdate_PushOnly};
void __cdecl GizObstacles_Update(void *world_ptr, void *data, f32 time) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZOBSTACLE_SYSTEM_PC *system = (GIZOBSTACLE_SYSTEM_PC *)data;
    GIZMOSET_PC *set;
    GIZMO_PC *gizmo;
    i32 i;
    (void)time;
    if (!world || !system || !world->gizmo_sys || !((GIZMOSYS_PREFIX_PC *)world->gizmo_sys)->sets)
        return;
    system->active_gizmo_count = 0;
    set = &((GIZMOSYS_PREFIX_PC *)world->gizmo_sys)->sets[obstacle_gizmotype_id];
    for (i = 0, gizmo = set->gizmos; i < set->count; ++i, ++gizmo) {
        GIZOBSTACLE *obstacle = (GIZOBSTACLE *)gizmo->object;
        GAMEOBJECT *previous = obstacle->triggering_object;
        GAMEANIMSET *animation;
        obstacle->triggering_object = NULL;
        if (!(obstacle->progress_flags & 2) || !(obstacle->progress_flags & 5) || !obstacle->anim_set ||
            (obstacle->progress_flags & 0x100) || (obstacle->runtime_flags & 0x80))
            continue;
        if (obstacle->mode == 7) {
            if (!(obstacle->progress_flags & 8))
                obstacle->animation_speed = obstacle->playback_state == 1 ? -1.0f : 0.0f;
            else
                obstacle->triggering_object = previous;
            if (obstacle->animation_speed != 0.0f)
                GizObstacle_PlayForwards(obstacle);
            else
                GizObstacle_Stop(obstacle);
        } else if (obstacle->progress_flags & 4) {
            if (obstacle->animation_speed == 0.0f)
                GizObstacle_Stop(obstacle);
            else
                GizObstacle_PlayForwards(obstacle);
        } else if (GizObstacleUpdateFnsPc[obstacle->mode])
            GizObstacleUpdateFnsPc[obstacle->mode](obstacle);
        animation = obstacle->anim_set;
        if (animation->flags & 7) {
            GizObstacle_EvalAveragePosAndRadius(obstacle, 2);
            if (obstacle->runtime_flags & 0x20) {
                GAMEANIMOBJ *object;
                for (object = obstacle->anim_set->objects; object; object = object->next) {
                    GIZOBSTACLE_OBJECT_DATA_PC *object_data = (GIZOBSTACLE_OBJECT_DATA_PC *)object->object_data;
                    if (object_data->flags & 2)
                        AddShoveObject(&object->special, object_data->platform_id);
                }
            }
            if (obstacle->animation_speed < 0.0f) {
                if (obstacle->stop_sfx_id != -1 &&
                    (obstacle->anim_set->state == 2 || IsSfxLooping(obstacle->stop_sfx_id)))
                    GameAudio_PlaySfxById(obstacle->stop_sfx_id, &obstacle->evaluated_position, 0, 0);
            } else if (obstacle->start_sfx_id != -1 &&
                       (!obstacle->anim_set->state || IsSfxLooping(obstacle->start_sfx_id)))
                GameAudio_PlaySfxById(obstacle->start_sfx_id, &obstacle->evaluated_position, 0, 0);
        }
        animation = obstacle->anim_set;
        if (animation->state == 2)
            obstacle->runtime_flags &= ~0x40u;
        if ((!obstacle->playback_state || obstacle->playback_state == 3 || (obstacle->config_flags & 0x80)) &&
            ((animation->flags & 2) || !animation->animated_object_count) && animation->state == 2 &&
            (obstacle->blowup_type != -1 || obstacle->pickup_count)) {
            if (obstacle->blowup_type != -1) {
                if (obstacle->config_flags & 0x200) {
                    GAMEANIMOBJ *object;
                    for (object = animation ? animation->objects : NULL; object; object = object->next) {
                        NUVEC *position = NuSpecialGetDrawPos(&object->special);
                        if (position)
                            GizmoBlowUpTypeBlowUp(world, obstacle->blowup_type, position);
                    }
                } else
                    GizmoBlowUpTypeBlowUp(world, obstacle->blowup_type, &obstacle->evaluated_position);
                GameAnimSet_SetVisibility(obstacle->anim_set, 0);
                obstacle->runtime_flags |= 0x80;
            }
            if (obstacle->pickup_count && (!(obstacle->runtime_flags & 1) || (obstacle->config_flags & 0x80))) {
                NUVEC position, direction;
                NuVecAdd(&position, &obstacle->evaluated_position, &obstacle->pickup_offset);
                NuVecRotateX(&direction, &v010, (u16)obstacle->pickup_direction_x);
                NuVecRotateY(&direction, &direction, (u16)obstacle->pickup_direction_y);
                AddPickups((u16)obstacle->pickup_count, 0, 0, 0, &position, &direction, 2.0f, -1,
                           obstacle->pickup_scatter_height, 2000000.0f, NULL, 1, 0);
                obstacle->runtime_flags |= 1;
            }
            if (obstacle->playback_state == 3) {
                GameAnimSet_JumpToStart(obstacle->anim_set);
                GizObstacle_EvalAveragePosAndRadius(obstacle, 2);
                GameAnimSet_SetVisibility(obstacle->anim_set, 1);
                obstacle->runtime_flags &= ~0x80u;
            }
        }
        if (!(obstacle->runtime_flags & 0x80) && (obstacle->config_flags & 0x1800))
            system->active_gizmos[system->active_gizmo_count++] = gizmo;
        obstacle->animation_speed = 1.0f;
        obstacle->progress_flags &= ~0xcu;
    }
    GizObstacleTriggerCountPc = 0;
}
extern const char GizObstacleTriggerLimitMessagePc[59];
void __cdecl GizObstacles_AddTrigger(NUVEC *position) {
    NUVEC message_position;
    if (!position)
        return;
    if (GizObstacleTriggerCountPc >= 16) {
        message_position = v001;
        AddGameMessage((char *)GizObstacleTriggerLimitMessagePc, &message_position, 0.4f, NULL, 0.4f, 200, 200, 200,
                       0x2080, 0.0f);
        return;
    }
    GizObstacleTriggersPc[GizObstacleTriggerCountPc++] = position;
}
void __cdecl GizObstacle_SetTechnoControlled(GIZOBSTACLE *obstacle, f32 speed) {
    obstacle->progress_flags |= 4;
    obstacle->animation_speed = speed;
}
void __cdecl GizObstacle_SetPushControlled(GIZOBSTACLE *obstacle, GAMEOBJECT *object, f32 speed) {
    if (!obstacle->playback_state && obstacle->anim_set->state == 2 && !(obstacle->config_flags & 0x100))
        return;
    obstacle->progress_flags |= 8;
    obstacle->triggering_object = object;
    obstacle->animation_speed = speed;
}
GIZOBSTACLE *__cdecl GizObstacle_FindNearest(void *ptr, NUVEC *position, GAMEOBJECT *object, f32 *distance_squared,
                                             i32 mode) {
    GIZOBSTACLE_SYSTEM_PC *system = (GIZOBSTACLE_SYSTEM_PC *)ptr;
    GIZOBSTACLE *nearest = NULL, *obstacle;
    f32 closest = 1.0e9f;
    i32 i;
    if (!system)
        return NULL;
    for (i = 0, obstacle = system->obstacles; i < system->count; ++i, ++obstacle) {
        f32 distance;
        if ((mode != -1 && obstacle->mode != mode) || !(obstacle->progress_flags & 2) ||
            !(obstacle->progress_flags & 1) || (obstacle->runtime_flags & 0x80))
            continue;
        if (object && obstacle->anim_set) {
            NUVEC average;
            GameAnimSet_GetAveragePos(obstacle->anim_set, &average, 2, 1, 0);
            distance = NuVecDistSqr(position, &average, NULL);
        } else
            distance = NuVecDistSqr(position, &obstacle->position, NULL);
        if (distance < closest) {
            closest = distance;
            nearest = obstacle;
        }
    }
    if (distance_squared)
        *distance_squared = closest;
    return nearest;
}
i32 __cdecl GizObstacles_Hit(void *world, GIZOBSTACLE *obstacle, NUVEC *position, i32 player, i32 damage) {
    (void)position;
    (void)damage;
    if (!(obstacle->progress_flags & 2) || !(obstacle->progress_flags & 1) || (obstacle->runtime_flags & 0x80))
        return 0;
    if (obstacle->config_flags & 0x800)
        obstacle->runtime_flags |= 0x40;
    else {
        if (!(obstacle->config_flags & 0x1000))
            return 0;
        if (obstacle->blowup_type != -1) {
            if (obstacle->config_flags & 0x200) {
                GAMEANIMOBJ *object;
                for (object = obstacle->anim_set ? obstacle->anim_set->objects : NULL; object; object = object->next) {
                    NUVEC *draw_position = NuSpecialGetDrawPos(&object->special);
                    if (draw_position)
                        GizmoBlowUpTypeBlowUp((WORLDINFO *)world, obstacle->blowup_type, draw_position);
                }
            } else
                GizmoBlowUpTypeBlowUp((WORLDINFO *)world, obstacle->blowup_type, &obstacle->evaluated_position);
        }
        GameAnimSet_JumpToEnd(obstacle->anim_set);
        GizObstacle_EvalAveragePosAndRadius(obstacle, 2);
        GameAnimSet_SetVisibility(obstacle->anim_set, 0);
        obstacle->runtime_flags |= 0x80;
    }
    if (player != -1 && (((GAMEOBJECT *)Player[player])->flags_1fc & 0x80))
        NewBuzz(((GAMEOBJECT *)Player[player])->pad->bound_pad, 0.1f, 0);
    return 1;
}
GIZMO_PC *__cdecl GizObstacles_GetBestBoltTarget(GIZMOSET_PC *set, f32 *result_distance, NUVEC *result_position,
                                                 NUVEC *result_velocity, void *object_ptr, NUVEC *position,
                                                 NUVEC *direction, f32 radius, f32 range_squared, i32 directional,
                                                 i32 planar, i32 bolt_id) {
    BOLTTYPE_PC *type = BoltType_FindByID(bolt_id, WORLD);
    u16 near_angle = TargetDeg_Near, mid_angle = TargetDeg_Mid, far_angle = TargetDeg_Far;
    f32 near_distance = TargetDist_Near2, mid_distance = TargetDist_Mid2;
    GIZOBSTACLE_SYSTEM_PC *system;
    NUVEC aim;
    f32 min_x, max_x, min_z, max_z, nearest_distance;
    GAMEOBJECT *object = (GAMEOBJECT *)object_ptr;
    GIZMO_PC *best = NULL, *previous = NULL;
    NUVEC *best_position = NULL, *previous_position = NULL;
    i32 i;
    if (!set || !set->data || !type)
        return NULL;
    system = (GIZOBSTACLE_SYSTEM_PC *)set->data;
    aim = *direction;
    min_x = position->x - radius;
    max_x = position->x + radius;
    min_z = position->z - radius;
    max_z = position->z + radius;
    nearest_distance = 1.0e9f;
    if (directional && (type->flags_60 & 0x20000)) {
        aim.y = 0.0f;
        NuVecNorm(&aim, &aim);
    }
    for (i = 0; i < system->active_gizmo_count; ++i) {
        GIZMO_PC *gizmo = system->active_gizmos[i];
        GIZOBSTACLE *obstacle = (GIZOBSTACLE *)gizmo->object;
        NUVEC *target = &obstacle->evaluated_position;
        NUVEC delta;
        f32 distance, dot;
        u16 angle;
        if (!(obstacle->progress_flags & 2) || !(obstacle->progress_flags & 1) || (obstacle->runtime_flags & 0x80))
            continue;
        if (target->x < min_x || target->x > max_x || target->z < min_z || target->z > max_z)
            continue;
        distance = planar ? NuVecXZDistSqr(target, position, &delta) : NuVecDistSqr(target, position, &delta);
        if (!(range_squared > distance))
            continue;
        if (!directional)
            NuVecRotateY(&aim, &v001, NuAtan2D(target->x - position->x, target->z - position->z));
        if (type->flags_60 & 0x20000)
            delta.y = 0.0f;
        NuVecNorm(&delta, &delta);
        dot = NuVecDot(&delta, &aim);
        if (near_distance > distance && directional)
            angle = near_angle;
        else if (mid_distance > distance)
            angle = mid_angle;
        else
            angle = far_angle;
        if (!(dot > NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff]) || !(nearest_distance > distance))
            continue;
        if (object && (void *)object->collision_target == gizmo) {
            previous = gizmo;
            previous_position = target;
        } else {
            nearest_distance = distance;
            best = gizmo;
            best_position = target;
        }
    }
    if (best) {
        *result_position = *best_position;
        *result_distance = nearest_distance;
        result_velocity->x = result_velocity->y = result_velocity->z = 0.0f;
        return best;
    }
    if (!previous)
        return NULL;
    *result_position = *previous_position;
    *result_distance = 1.0e9f;
    result_velocity->x = result_velocity->y = result_velocity->z = 0.0f;
    return previous;
}
void __cdecl GameAnimSet_DrawReflection(GAMEANIMSET *, i32, f32, NUMTX *);
void __cdecl GizObstacles_Draw(void *world_ptr, void *data, f32 time) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZOBSTACLE_SYSTEM_PC *system = (GIZOBSTACLE_SYSTEM_PC *)data;
    GIZOBSTACLE *obstacle;
    i32 i;
    (void)time;
    if (!system)
        return;
    for (i = 0, obstacle = system->obstacles; i < system->count; ++i, ++obstacle) {
        if ((obstacle->progress_flags & 2) && (obstacle->room_id < 0 || world->rooms_visible_ptr[obstacle->room_id]) &&
            !(obstacle->runtime_flags & 0x80) && (obstacle->config_flags & 0x40))
            GameAnimSet_DrawReflection(obstacle->anim_set, 2, obstacle->reflection_alpha, NULL);
    }
}

extern const char *GizObstacleRegistrationNamePtrPc;

STATIC_ASSERT(offsetof(GIZOBSTACLE_SYSTEM_PC, obstacles) == 0);
STATIC_ASSERT(offsetof(GIZOBSTACLE_SYSTEM_PC, active_gizmos) == 4);
STATIC_ASSERT(offsetof(BOLT_PC, owner) == 4);
STATIC_ASSERT(offsetof(BOLT_PC, position) == 0x88);
STATIC_ASSERT(offsetof(BOLT_PC, direction) == 0xac);
STATIC_ASSERT(offsetof(BOLT_PC, hit_platform) == 0xfa);
STATIC_ASSERT(offsetof(BOLT_PC, type_id) == 0x101);

static i32 __cdecl GizObstacles_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    return world ? world->current_level->max_obstacles : 0;
}
static void __cdecl GizObstacles_AddGizmos(void *gizmo_sys, i32 type_id, void *world, void *data) {
    GIZOBSTACLE_SYSTEM_PC *system = (GIZOBSTACLE_SYSTEM_PC *)data;
    i32 i;
    (void)world;
    if (system)
        for (i = 0; i < system->count; ++i)
            if (NuStrLen(system->obstacles[i].name))
                AddGizmo(gizmo_sys, type_id, NULL, &system->obstacles[i]);
}
static char *__cdecl GizmoObstacle_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    (void)gizmo;
    switch (output) {
        case 0:
            return "AtEnd";
        case 1:
            return "NotAtStart";
        case 2:
            return "Proximity";
        case 3:
            return "AtStart";
        case 4:
            return "PlayingForward";
        default:
            return NULL;
    }
}
static i32 __cdecl GizObstacles_BoltHitPlat(void *world, void *data, BOLT_PC *bolt, u8 *hit_flags) {
    GIZOBSTACLE_SYSTEM_PC *system = (GIZOBSTACLE_SYSTEM_PC *)data;
    i32 i, count;
    (void)hit_flags;
    if (!system)
        return 0;
    count = system->active_gizmo_count;
    for (i = 0; i < count; ++i) {
        GIZOBSTACLE *obstacle = (GIZOBSTACLE *)system->active_gizmos[i]->object;
        GAMEANIMOBJ *object;
        if (!(obstacle->progress_flags & 2) || !(obstacle->progress_flags & 1) || !(obstacle->runtime_flags & 0x10) ||
            (obstacle->runtime_flags & 0x80))
            continue;
        for (object = obstacle->anim_set->objects; object; object = object->next) {
            typedef struct obstacle_anim_object_data_pc_s {
                u16 flags;
                i16 platform;
            } OBSTACLE_ANIM_OBJECT_DATA_PC;
            OBSTACLE_ANIM_OBJECT_DATA_PC *object_data = (OBSTACLE_ANIM_OBJECT_DATA_PC *)object->object_data;
            STATIC_ASSERT(sizeof(OBSTACLE_ANIM_OBJECT_DATA_PC) == 4);
            STATIC_ASSERT(offsetof(OBSTACLE_ANIM_OBJECT_DATA_PC, platform) == 2);
            if (object_data->platform == bolt->hit_platform) {
                BOLTTYPE_PC *type = BoltType_FindByID(bolt->type_id, (WORLDINFO *)world);
                GAMEOBJECT *owner = (GAMEOBJECT *)bolt->owner;
                i32 player = owner ? (i8)owner->field_280 : -1;
                GizObstacles_Hit(world, obstacle, &bolt->position, player, type->field_3c);
                return 1;
            }
        }
    }
    return 0;
}
static i32 __cdecl GizObstacles_BoltHit(void *world, void *data, void *attacker_ptr, NUVEC *points, i32 point_count,
                                        f32 radius, NUVEC *minimum, NUVEC *maximum, BOLT_PC *bolt, u32 hit_type,
                                        u8 *hit_flags) {
    GIZOBSTACLE_SYSTEM_PC *system = (GIZOBSTACLE_SYSTEM_PC *)data;
    GAMEOBJECT *attacker = (GAMEOBJECT *)attacker_ptr;
    GIZOBSTACLE *nearest = NULL;
    f32 nearest_distance = 1000000000.0f;
    i32 i;
    if (!system)
        return 0;
    for (i = 0; i < system->active_gizmo_count; ++i) {
        GIZOBSTACLE *obstacle = (GIZOBSTACLE *)system->active_gizmos[i]->object;
        i32 point_index;
        if (!(obstacle->progress_flags & 2) || !(obstacle->progress_flags & 1) || (obstacle->runtime_flags & 0x80))
            continue;
        if (obstacle->evaluated_position.x - obstacle->evaluated_radius > maximum->x ||
            minimum->x > (f64)obstacle->evaluated_position.x + obstacle->evaluated_radius ||
            obstacle->evaluated_position.z - obstacle->evaluated_radius > maximum->z ||
            minimum->z > (f64)obstacle->evaluated_position.z + obstacle->evaluated_radius ||
            obstacle->evaluated_position.y - obstacle->evaluated_radius > maximum->y ||
            minimum->y > (f64)obstacle->evaluated_position.y + obstacle->evaluated_radius)
            continue;
        for (point_index = point_count - 1; point_index >= 0; --point_index) {
            NUVEC *point = &points[point_index];
            f32 distance;
            if (!SphereSphereOverlap(&obstacle->evaluated_position, obstacle->evaluated_radius, point, radius))
                continue;
            if (attacker)
                point = &attacker->collision_position;
            distance = NuVecDistSqr(point, &obstacle->evaluated_position, 0);
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest = obstacle;
            }
            break;
        }
    }
    if (!nearest)
        return 0;
    if (hit_type != 2 && hit_type != 7 && bolt) {
        BoltType_FindByID(bolt->type_id, WORLD);
        Cheats_CheckFlags(2);
    }
    {
        BOLTTYPE_PC *type = BoltType_FindByID(bolt->type_id, (WORLDINFO *)world);
        GAMEOBJECT *owner = (GAMEOBJECT *)bolt->owner;
        i32 player = owner ? (i8)owner->field_280 : -1;
        if (GizObstacles_Hit(world, nearest, &bolt->position, player, type->field_3c)) {
            if (attacker) {
                NewRumble(attacker->pad->bound_pad, 0.4f, 0);
                GameCam_HitJudder();
            }
        } else {
            NUVEC normal;
            NuVecSub(&normal, &nearest->evaluated_position, &bolt->position);
            NuVecNorm(&normal, &normal);
            Bolt_AddDeflectedBolt(bolt, &bolt->direction, &normal, hit_flags);
        }
    }
    if (BoltSys->stop_targeting)
        BoltSys->stop_targeting(attacker, points);
    return 1;
}

ADDGIZMOTYPE_PC *__cdecl GizObstacles_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE_PC GizObstaclesAddTypePc;
    ADDGIZMOTYPE_PC *addtype = &GizObstaclesAddTypePc;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->name = GizObstacleRegistrationNamePtrPc;
    addtype->prefix = "";
    addtype->fns.options = 0x70;
    addtype->fns.early_update_fn = NULL;
    addtype->fns.panel_draw_fn = NULL;
    addtype->fns.get_visibility_fn = NULL;
    addtype->fns.using_special_fn = NULL;
    addtype->fns.get_max_gizmos_fn = GizObstacles_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = GizObstacles_AddGizmos;
    addtype->fns.late_update_fn = GizObstacles_Update;
    addtype->fns.draw_fn = GizObstacles_Draw;
    addtype->fns.get_gizmo_name_fn = GizmoObstacle_GetGizmoName;
    addtype->fns.get_output_fn = GizmoObstacle_GetOutput;
    addtype->fns.get_output_name_fn = GizmoObstacle_GetOutputName;
    addtype->fns.get_num_outputs_fn = GizmoObstacle_GetNumOutputs;
    addtype->fns.activate_fn = GizmoObstacle_Activate;
    addtype->fns.activate_rev_fn = GizmoObstacle_ActivateRev;
    addtype->fns.set_visibility_fn = GizmoObstacle_SetVisibility;
    addtype->fns.get_pos_fn = GizmoObstacle_GetPos;
    addtype->fns.bolt_hit_plat_fn = GizObstacles_BoltHitPlat;
    addtype->fns.get_best_bolt_target_fn = GizObstacles_GetBestBoltTarget;
    addtype->fns.bolt_hit_fn = GizObstacles_BoltHit;
    addtype->fns.allocate_progress_data_fn = GizObstacles_AllocateProgressData;
    addtype->fns.clear_progress_fn = GizObstacles_ClearProgress;
    addtype->fns.store_progress_fn = GizObstacles_StoreProgress;
    addtype->fns.reset_fn = GizObstacles_Reset;
    addtype->fns.reserve_buffer_space_fn = GizObstacles_ReserveBufferSpace;
    addtype->fns.load_fn = GizObstacles_Load;
    addtype->fns.post_load_fn = GizObstacles_PostLoad;
    addtype->fns.add_level_sfx_fn = GizObstacles_AddLevelSfx;
    obstacle_gizmotype_id = type_id;
    return addtype;
}

extern const char CharacterFixupNamePc00752dfc[9];

typedef struct bomb_generator_runtime_pc_s {
    char name[16];
    NUVEC position;
    GAMEANIMSET *anim_set;
    i32 interval;
    GAMEOBJECT *generated_bomb;
    u16 unknown_28;
    u16 flags;
} BOMB_GENERATOR_RUNTIME_PC;
typedef struct bomb_generator_system_runtime_pc_s {
    BOMB_GENERATOR_RUNTIME_PC *items;
    u16 count, capacity;
    void *pool;
    u16 flags, unknown_0e;
    void *progress;
} BOMB_GENERATOR_SYSTEM_RUNTIME_PC;
typedef struct bomb_generator_progress_pc_s {
    u32 active[3], visible[3];
} BOMB_GENERATOR_PROGRESS_PC;
STATIC_ASSERT(sizeof(BOMB_GENERATOR_RUNTIME_PC) == 0x2c);
STATIC_ASSERT(offsetof(BOMB_GENERATOR_RUNTIME_PC, anim_set) == 0x1c);
STATIC_ASSERT(offsetof(BOMB_GENERATOR_RUNTIME_PC, generated_bomb) == 0x24);
STATIC_ASSERT(offsetof(BOMB_GENERATOR_RUNTIME_PC, flags) == 0x2a);
STATIC_ASSERT(sizeof(BOMB_GENERATOR_SYSTEM_RUNTIME_PC) == 0x14);
STATIC_ASSERT(offsetof(BOMB_GENERATOR_SYSTEM_RUNTIME_PC, count) == 4);
STATIC_ASSERT(offsetof(BOMB_GENERATOR_SYSTEM_RUNTIME_PC, flags) == 0xc);
STATIC_ASSERT(offsetof(BOMB_GENERATOR_SYSTEM_RUNTIME_PC, progress) == 0x10);
STATIC_ASSERT(sizeof(BOMB_GENERATOR_PROGRESS_PC) == 0x18);
STATIC_ASSERT(offsetof(BOMB_GENERATOR_PROGRESS_PC, visible) == 0xc);
STATIC_ASSERT(offsetof(GAMEOBJECT, cable) == 0x1040);
STATIC_ASSERT(offsetof(CABLE_s, target) == 4);
STATIC_ASSERT(offsetof(CABLE_s, max_length) == 0x1c4);
STATIC_ASSERT(offsetof(WORLDINFO, game_debris_sys) == 0x134);

static void __cdecl BombGeneratorsApplyResetPc(BOMB_GENERATOR_SYSTEM_RUNTIME_PC *system,
                                               BOMB_GENERATOR_PROGRESS_PC *progress) {
    BOMB_GENERATOR_RUNTIME_PC *generator = system->items;
    i32 index;
    i32 shared_bomb;
    for (index = 0; index < system->count; ++index, ++generator) {
        GAMEOBJECT *bomb;
        generator->flags |= 3;
        if (generator->anim_set != NULL)
            GameAnimSet_EvaluateState(generator->anim_set);
        bomb = generator->generated_bomb;
        if (bomb != NULL && (system->flags & 2) != 0 && (bomb->flags_1fc_word & 1) != 0 &&
            (bomb->flags_1fc_word & 0x1000) != 0)
            KillGameObject(bomb, 4, 0);
        generator->generated_bomb = NULL;
        if (progress != NULL && index < 96) {
            i32 word = index / 32;
            u32 mask = 1u << (index & 31);
            generator->flags = (u16)((generator->flags & ~2u) | (((progress->visible[word] & mask) != 0) << 1));
            generator->flags = (u16)((generator->flags & ~1u) | ((progress->active[word] & mask) != 0));
        }
    }
    shared_bomb = BombGenerator_PlayerBomb[0] != NULL && BombGenerator_PlayerBomb[0] == BombGenerator_PlayerBomb[1];
    for (index = 0; index < 2; ++index) {
        GAMEOBJECT *player;
        CABLE_s *cable;
        GAMEOBJECT *bomb;
        if (netclient != 0)
            break;
        player = (GAMEOBJECT *)Player[index];
        if (player != NULL && BombGenerator_PlayerBomb[index] != NULL && player->cable != NULL &&
            ((CABLE_s *)player->cable)->target != NULL && ((CABLE_s *)player->cable)->target->id == id_DRAGBOMB) {
            KillGameObject(((CABLE_s *)player->cable)->target, 4, 0);
            ((GAMEOBJECT *)Player[index])->cable = NULL;
        }
        player = (GAMEOBJECT *)Player[index];
        if (player == NULL || (player->flags_1fc_word & 0x80) == 0 || BombGenerator_PlayerBomb[index] == NULL ||
            player->cable != NULL)
            continue;
        if (index != 0 && shared_bomb != 0 && ((GAMEOBJECT *)Player[0])->cable != NULL &&
            ((CABLE_s *)((GAMEOBJECT *)Player[0])->cable)->target != NULL &&
            ((CABLE_s *)((GAMEOBJECT *)Player[0])->cable)->target->id == id_DRAGBOMB) {
            bomb = ((CABLE_s *)((GAMEOBJECT *)Player[0])->cable)->target;
        } else {
            NUVEC position;
            position.x = 0.0f;
            position.y = 0.0f;
            position.z = -2.0f;
            NuVecRotateY(&position, &position, player->field_0x276);
            NuVecAdd(&position, &position, &((GAMEOBJECT *)Player[index])->collision_position);
            bomb = AddDynamicCreature((i16)id_DRAGBOMB, &position, 0, (char *)CharacterFixupNamePc00752dfc, NULL, NULL,
                                      0, NULL, NULL, 0, 0);
        }
        if (bomb != NULL) {
            CABLE_RECORD_PC *created = CreateCable((GAMEOBJECT *)Player[index], bomb, 0);
            ((GAMEOBJECT *)Player[index])->cable = created;
            cable = (CABLE_s *)((GAMEOBJECT *)Player[index])->cable;
            if (cable != NULL)
                cable->max_length = 1000000000.0f;
        }
    }
}

void __cdecl GizBombGens_Update(void *world_pointer, void *system_pointer, f32 delta_time) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    BOMB_GENERATOR_SYSTEM_RUNTIME_PC *system = (BOMB_GENERATOR_SYSTEM_RUNTIME_PC *)system_pointer;
    u16 model;
    GAMEOBJECT *player;
    CABLE_s *cable;
    (void)delta_time;
    if (system != NULL) {
        BOMB_GENERATOR_RUNTIME_PC *generator;
        i32 index;
        if ((system->flags & 1) != 0) {
            BombGeneratorsApplyResetPc(system, (BOMB_GENERATOR_PROGRESS_PC *)system->progress);
            system->flags &= (u16)~1u;
        }
        generator = system->items;
        for (index = 0; index < system->count; ++index, ++generator) {
            if ((generator->flags & 2) == 0 || (generator->flags & 1) == 0 || generator->anim_set == NULL)
                continue;
            if (generator->generated_bomb != NULL) {
                GAMEOBJECT *bomb;
                GameAnimSet_SetVisibility(generator->anim_set, 0);
                bomb = generator->generated_bomb;
                if (bomb->dead != 0 || (bomb->flags_1fc_word & 0x1000) == 0) {
                    GAMEANIMSET *anim_set = generator->anim_set;
                    generator->generated_bomb = NULL;
                    GameAnimSet_Reset(anim_set);
                    GameAnimSet_EvalAnim(generator->anim_set);
                    GameAnimSet_SetVisibility(generator->anim_set, 1);
                }
            } else {
                GameAnimSet_Play(generator->anim_set, 1.0f, 1);
                if (generator->anim_set->state == 2) {
                    NUVEC position;
                    GameAnimSet_GetAveragePos(generator->anim_set, &position, 2, 1, 1);
                    if (netclient == 0) {
                        generator->generated_bomb =
                            AddDynamicCreature((i16)id_DRAGBOMB, &position, 0, (char *)CharacterFixupNamePc00752dfc,
                                               NULL, NULL, 1, NULL, NULL, 0, 0);
                        if (Cheat_IsOn(5) != 0) {
                            AddGameDebris(world->game_debris_sys, 0x7d, &position);
                            AddGameDebris(world->game_debris_sys, 0x7e, &position);
                        }
                    }
                }
            }
        }
    }
    player = (GAMEOBJECT *)Player[0];
    model = id_DRAGBOMB;
    BombGenerator_PlayerBomb[0] = NULL;
    if (player != NULL) {
        cable = (CABLE_s *)player->cable;
        if (cable != NULL && cable->target != NULL && cable->target->id == model)
            BombGenerator_PlayerBomb[0] = cable->target;
    }
    player = (GAMEOBJECT *)Player[1];
    BombGenerator_PlayerBomb[1] = NULL;
    if (player != NULL) {
        cable = (CABLE_s *)player->cable;
        if (cable != NULL && cable->target != NULL && cable->target->id == model)
            BombGenerator_PlayerBomb[1] = cable->target;
    }
}

STATIC_ASSERT(offsetof(GIZBOMBGEN_s, flags) == 0x2a);

i32 __cdecl GizmoBombGen_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    (void)output;
    (void)mode;
    return ((GIZBOMBGEN_s *)gizmo->object)->flags & 1;
}
