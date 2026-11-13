#include "saga.h"

/* Retail unit 0x465dd0-0x4667eb: definitions in retail address order. */

typedef struct {
    GIZBOMBGEN_s *items;
    u16 count, capacity;
    void *pool;
    i16 reset : 1;
    i16 reset_saved : 1;
    i16 reset_flags_unused : 14;
    u16 opaque_0e;
    void *progress;
} ADD_BOMB_RESET_SYS_PC;
extern i32 come_from_an_editor;
typedef struct {
    u32 enabled[3], visible[3];
} BOMB_PROGRESS_PC;
typedef struct {
    u8 unused00[0x2a];
    u8 flags, unused2b;
} BOMB_PROGRESS_RECORD_PC;
typedef struct {
    BOMB_PROGRESS_RECORD_PC *records;
    u16 count;
} BOMB_PROGRESS_SYSTEM_PC;
typedef struct bombgen_activation_reader_pc_s {
    u8 unknown_00[0x1c];
    GAMEANIMSET *anim_set;
    u8 unknown_20[0xa];
    u16 flags;
} BOMBGEN_ACTIVATION_READER_PC;
typedef struct bombgen_activation_flags_pc_s {
    u16 active : 1;
    u16 visible : 1;
    u16 unknown : 14;
} BOMBGEN_ACTIVATION_FLAGS_PC;
extern const char CharacterFixupNamePc00752dfc[9];
typedef struct bomb_generator_runtime_pc_s {
    char name[16];
    NUVEC position;
    GAMEANIMSET *anim_set;
    i32 interval;
    GAMEOBJECT *generated_bomb;
    u16 unknown_28;
    union {
        u16 flags;
        struct {
            u16 active : 1;
            u16 visible : 1;
            u16 unknown_flags : 14;
        };
    };
} BOMB_GENERATOR_RUNTIME_PC;
typedef struct bomb_generator_system_runtime_pc_s {
    BOMB_GENERATOR_RUNTIME_PC *items;
    u16 count, capacity;
    void *pool;
    i16 flags;
    u16 unknown_0e;
    void *progress;
} BOMB_GENERATOR_SYSTEM_RUNTIME_PC;
typedef struct bomb_generator_progress_pc_s {
    u32 active[3], visible[3];
} BOMB_GENERATOR_PROGRESS_PC;
extern char AIRegistryTextPc007575bc[7];

i32 GizBombGens_Load(void *world_ptr, void *system_ptr) {
    const u8 version = (u8)(EdFileReadChar());
    GIZBOMBGENSYS_s *system = (GIZBOMBGENSYS_s *)(system_ptr);
    system->count = (u16)(EdFileReadShort());
    if (system->count != 0) {
        GIZBOMBGEN_s *bomb_generator = system->bomb_generators;
        i32 index;
        for (index = 0; index < system->count; ++index, ++bomb_generator) {
            EdFileRead(bomb_generator->name, sizeof(bomb_generator->name));
            EdFileReadNuVec(&bomb_generator->position);
            bomb_generator->interval = EdFileReadInt();
            bomb_generator->flags |= GIZBOMBGEN_FLAG_ACTIVE | GIZBOMBGEN_FLAG_VISIBLE;
            GizmoFileReadGameAnimSet(bomb_generator->anim_set, world_ptr, NULL, version, (char *)("BombGenerator"),
                                     bomb_generator->name);
        }
    }
    return 1;
}

void *__cdecl GizBombGens_ReserveBufferSpace(void *pointer) {
    WORLDINFO *world = (WORLDINFO *)pointer;
    GIZBOMBGENSYS_s *system;
    i32 i;
    system = (GIZBOMBGENSYS_s *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, sizeof(*system));
    system->capacity = world->current_level->max_bombgens;
    system->bomb_generators = (GIZBOMBGEN_s *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end,
                                                              system->capacity * sizeof(*system->bomb_generators));
    system->anim_object_pool = GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 0,
                                                            (u16)world->current_level->max_bombgen_objects);
    for (i = 0; i < system->capacity; ++i)
        system->bomb_generators[i].anim_set = GameAnimSet_Create(
            &world->giz_buffer, &world->buffer_end, (GAMEANIMOBJPOOL *)system->anim_object_pool, world->game_anim_sys);
    world->giz_bombgen_sys = system;
    return system;
}

void __cdecl GizmoBombGen_Activate(GIZMO_PC *gizmo, i32 active) {
    BOMBGEN_ACTIVATION_READER_PC *generator;
    if (gizmo != NULL) {
        generator = (BOMBGEN_ACTIVATION_READER_PC *)gizmo->object;
        ((BOMBGEN_ACTIVATION_FLAGS_PC *)&generator->flags)->active = active != 0;
    }
}

void __cdecl GizmoBombGen_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    BOMBGEN_ACTIVATION_READER_PC *generator;
    if (gizmo != NULL && gizmo->object != NULL) {
        generator = (BOMBGEN_ACTIVATION_READER_PC *)gizmo->object;
        GameAnimSet_SetVisibility(generator->anim_set, visible);
        ((BOMBGEN_ACTIVATION_FLAGS_PC *)&generator->flags)->visible = visible != 0;
    }
}

NUVEC *__cdecl GizmoBombGen_GetPos(GIZMO_PC *gizmo) {
    if (gizmo == NULL || gizmo->object == NULL)
        return NULL;
    return &((GIZBOMBGEN_s *)gizmo->object)->position;
}

void __cdecl GizBombGens_SetResetFlag(void *world_ptr, void *data, void *progress) {
    WORLDINFO *w = (WORLDINFO *)world_ptr;
    ADD_BOMB_RESET_SYS_PC *s = (ADD_BOMB_RESET_SYS_PC *)data;
    s->reset = 1;
    if (come_from_an_editor != 0)
        s->reset_saved = 0;
    else
        s->reset_saved = w->unknown_0114[1];
    s->progress = progress;
}

static void __cdecl BombGeneratorsApplyResetPc(BOMB_GENERATOR_SYSTEM_RUNTIME_PC *system,
                                               BOMB_GENERATOR_PROGRESS_PC *progress) {
    BOMB_GENERATOR_RUNTIME_PC *generator = system->items;
    i32 index;
    i32 shared_bomb;
    for (index = 0; index < system->count; ++index, ++generator) {
        generator->flags |= 3;
        if (generator->anim_set != NULL)
            GameAnimSet_EvaluateState(generator->anim_set);
        if (generator->generated_bomb != NULL && (system->flags & 2) != 0 &&
            (generator->generated_bomb->flags_1fc_word & 1) != 0 &&
            (generator->generated_bomb->flags_1fc_word & 0x1000) != 0)
            KillGameObject(generator->generated_bomb, 4, 0);
        generator->generated_bomb = NULL;
        if (progress != NULL && index < 96) {
            i32 word = index / 32;
            u32 mask = 1u << (index & 31);
            generator->visible = (progress->visible[word] & mask) != 0;
            generator->active = (progress->active[word] & mask) != 0;
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
            NuVecRotateY(&position, &position, ((GAMEOBJECT *)Player[index])->field_0x276);
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

i32 __cdecl GizBombGens_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    if (world != NULL)
        return world->current_level->max_bombgens;
    return 0;
}

void __cdecl GizBombGens_AddGizmos(GIZMOSYS_PC *giz, i32 type, void *world, void *data) {
    GIZBOMBGENSYS_s *s = (GIZBOMBGENSYS_s *)data;
    i32 i;
    (void)world;
    if (s == NULL)
        return;
    for (i = 0; i < s->count; ++i)
        if (NuStrLen(s->bomb_generators[i].name) != 0)
            AddGizmo(giz, type, NULL, &s->bomb_generators[i]);
}

char *__cdecl GizmoBombGen_GetGizmoName(GIZMO_PC *gizmo) {
    if (gizmo != NULL)
        return ((GIZBOMBGEN_s *)gizmo->object)->name;
    return NULL;
}

i32 __cdecl GizmoBombGen_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    (void)output;
    (void)mode;
    return ((GIZBOMBGEN_s *)gizmo->object)->flags & 1;
}

char *__cdecl GizmoBombGen_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    return AIRegistryTextPc007575bc;
}

i32 __cdecl GizmoBombGen_GetNumOutputs(GIZMO_PC *gizmo) {
    return 1;
}

void *__cdecl GizBombGens_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 0x18);
}

void __cdecl GizBombGens_ClearProgress(void *owner, void *progress_ptr) {
    BOMB_PROGRESS_PC *progress = (BOMB_PROGRESS_PC *)progress_ptr;
    if (progress == NULL)
        return;
    memset(progress->enabled, -1, sizeof(progress->enabled));
    memset(progress->visible, -1, sizeof(progress->visible));
}

void __cdecl GizBombGens_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    BOMB_PROGRESS_PC *progress = (BOMB_PROGRESS_PC *)progress_ptr;
    BOMB_PROGRESS_SYSTEM_PC *system = (BOMB_PROGRESS_SYSTEM_PC *)data;
    BOMB_PROGRESS_RECORD_PC *record;
    i32 index;
    i32 word;
    u32 bit;
    if (progress == NULL)
        return;
    memset(progress->enabled, -1, sizeof(progress->enabled));
    memset(progress->visible, -1, sizeof(progress->visible));

    record = system->records;
    for (index = 0; index < system->count; ++index, ++record) {
        if (index >= 96)
            break;
        word = index / 32;
        bit = 1u << (index & 31);
        if (!(record->flags & 2))
            progress->visible[word] &= ~bit;
        if (!(record->flags & 1))
            progress->enabled[word] &= ~bit;
    }
}
char *GizmoRegistryGizBombGenNamePtrPc = "BombGenerator";
static ADDGIZMOTYPE_PC GizmoRegistryGizBombGenAddTypePc = {0};

ADDGIZMOTYPE_PC *__cdecl GizBombGen_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE_PC *addtype = &GizmoRegistryGizBombGenAddTypePc;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->name = GizmoRegistryGizBombGenNamePtrPc;
    addtype->fns.early_update_fn = NULL;
    addtype->fns.draw_fn = NULL;
    addtype->fns.panel_draw_fn = NULL;
    addtype->fns.activate_rev_fn = NULL;
    addtype->fns.get_visibility_fn = NULL;
    addtype->fns.using_special_fn = NULL;
    addtype->fns.bolt_hit_plat_fn = NULL;
    addtype->fns.get_best_bolt_target_fn = NULL;
    addtype->fns.bolt_hit_fn = NULL;
    addtype->fns.post_load_fn = NULL;
    addtype->fns.add_level_sfx_fn = NULL;
    addtype->prefix = "";
    addtype->fns.options = 0x18;
    addtype->fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))GizBombGens_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)GizBombGens_AddGizmos;
    addtype->fns.late_update_fn = (void(__cdecl *)(void *, void *, f32))GizBombGens_Update;
    addtype->fns.get_gizmo_name_fn = (char *(__cdecl *)(GIZMO_PC *))GizmoBombGen_GetGizmoName;
    addtype->fns.get_output_fn = (i32(__cdecl *)(GIZMO_PC *, i32, i32))GizmoBombGen_GetOutput;
    addtype->fns.get_output_name_fn = (char *(__cdecl *)(GIZMO_PC *, i32))GizmoBombGen_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)GizmoBombGen_GetNumOutputs;
    addtype->fns.activate_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizmoBombGen_Activate;
    addtype->fns.set_visibility_fn = (void(__cdecl *)(GIZMO_PC *, i32))GizmoBombGen_SetVisibility;
    addtype->fns.get_pos_fn = (void *)GizmoBombGen_GetPos;
    addtype->fns.allocate_progress_data_fn = (void *(__cdecl *)(VARIPTR *, VARIPTR *))GizBombGens_AllocateProgressData;
    addtype->fns.clear_progress_fn = (void(__cdecl *)(void *, void *))GizBombGens_ClearProgress;
    addtype->fns.store_progress_fn = (void(__cdecl *)(void *, void *, void *))GizBombGens_StoreProgress;
    addtype->fns.reset_fn = (void(__cdecl *)(void *, void *, void *))GizBombGens_SetResetFlag;
    addtype->fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))GizBombGens_ReserveBufferSpace;
    addtype->fns.load_fn = (i32(__cdecl *)(void *, void *))GizBombGens_Load;
    bombgen_gizmotype_id = type_id;
    return addtype;
}
