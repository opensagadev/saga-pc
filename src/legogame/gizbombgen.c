#include "gameapi/edtools/edfile.h"
#include "gamelib/util/Utilities.h"
#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/traps/gizbombgen.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/cable.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/fx.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "legogame/gamestatus.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nuvec.h"

typedef struct bomb_generator_progress_s {
    u32 active[3];
    u32 visible[3];
} BOMB_GENERATOR_PROGRESS;

i32 GizBombGens_Load(void *world_ptr, void *system_ptr) {
    const u8 version = (u8)(EdFileReadChar());
    GIZBOMBGENSYS_s *system = system_ptr;
    system->count = (u16)(EdFileReadShort());
    if (system->count != 0) {
        GIZBOMBGEN_s *bomb_generator = system->bomb_generators;
        i32 index;
        for (index = 0; index < system->count; ++index, ++bomb_generator) {
            EdFileRead(bomb_generator->name, sizeof(bomb_generator->name));
            EdFileReadNuVec(&bomb_generator->position);
            bomb_generator->interval = EdFileReadInt();
            bomb_generator->flags |= GIZBOMBGEN_FLAG_ACTIVE | GIZBOMBGEN_FLAG_VISIBLE;
            GizmoFileReadGameAnimSet(bomb_generator->anim_set, world_ptr, NULL, version, (char *)("BombGenerator"), bomb_generator->name);
        }
    }
    return 1;
}

void *GizBombGens_ReserveBufferSpace(void *pointer) {
    WORLDINFO *world = pointer;
    GIZBOMBGENSYS_s *system;
    i32 i;
    system = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, sizeof(*system));
    system->capacity = world->current_level->max_bombgens;
    system->bomb_generators = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, system->capacity * sizeof(*system->bomb_generators));
    system->anim_object_pool =
        GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 0, (u16)world->current_level->max_bombgen_objects);
    for (i = 0; i < system->capacity; ++i) {
        system->bomb_generators[i].anim_set =
            GameAnimSet_Create(&world->giz_buffer, &world->buffer_end, system->anim_object_pool, world->game_anim_sys);
    }
    world->giz_bombgen_sys = system;
    return system;
}

void GizmoBombGen_Activate(GIZMO *gizmo, i32 active) {
    GIZBOMBGEN_s *generator;
    if (gizmo != NULL) {
        generator = gizmo->object;
        generator->active = active != 0;
    }
}

void GizmoBombGen_SetVisibility(GIZMO *gizmo, i32 visible) {
    GIZBOMBGEN_s *generator;
    if (gizmo != NULL && gizmo->object != NULL) {
        generator = gizmo->object;
        GameAnimSet_SetVisibility(generator->anim_set, visible);
        generator->visible = visible != 0;
    }
}

NUVEC *GizmoBombGen_GetPos(GIZMO *gizmo) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return NULL;
    }
    return &gizmo->bomb_generator->position;
}

void GizBombGens_SetResetFlag(void *world_ptr, void *data, void *progress) {
    WORLDINFO *w = world_ptr;
    GIZBOMBGENSYS_s *s = data;
    s->reset = 1;
    if (come_from_an_editor != 0) {
        s->reset_saved = 0;
    } else {
        s->reset_saved = w->unknown_0114[1];
    }
    s->progress = progress;
}

static void BombGeneratorsApplyReset(GIZBOMBGENSYS_s *system, BOMB_GENERATOR_PROGRESS *progress) {
    GIZBOMBGEN_s *generator = system->bomb_generators;
    i32 index;
    i32 shared_bomb;
    for (index = 0; index < system->count; ++index, ++generator) {
        generator->flags |= 3;
        if (generator->anim_set != NULL) {
            GameAnimSet_EvaluateState(generator->anim_set);
        }
        if (generator->generated_bomb != NULL && (system->flags & 2) != 0 &&
            (generator->generated_bomb->flags_1fc_word & APIOBJECT_FLAG_IN_USE) != 0 &&
            (generator->generated_bomb->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0) {
            KillGameObject(generator->generated_bomb, KILL_REASON_REMOVED, 0);
        }
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
        if (netclient != 0) {
            break;
        }
        player = Player[index];
        if (player != NULL && BombGenerator_PlayerBomb[index] != NULL && player->cable != NULL && player->cable->target != NULL &&
            player->cable->target->id == id_DRAGBOMB) {
            KillGameObject(player->cable->target, KILL_REASON_REMOVED, 0);
            Player[index]->cable = NULL;
        }
        player = Player[index];
        if (player == NULL || (player->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0 || BombGenerator_PlayerBomb[index] == NULL ||
            player->cable != NULL) {
            continue;
        }
        if (index != 0 && shared_bomb != 0 && Player[0]->cable != NULL && Player[0]->cable->target != NULL &&
            Player[0]->cable->target->id == id_DRAGBOMB) {
            bomb = Player[0]->cable->target;
        } else {
            NUVEC position;
            position.x = 0.0f;
            position.y = 0.0f;
            position.z = -2.0f;
            NuVecRotateY(&position, &position, Player[index]->movement_angle);
            NuVecAdd(&position, &position, &Player[index]->collision_position);
            bomb = AddDynamicCreature((i16)id_DRAGBOMB, &position, 0, "dragbomb", NULL, NULL, 0, NULL, NULL, 0, 0);
        }
        if (bomb != NULL) {
            CABLE_s *created = CreateCable(Player[index], bomb, 0);
            Player[index]->cable = created;
            cable = Player[index]->cable;
            if (cable != NULL) {
                cable->max_length = 1000000000.0f;
            }
        }
    }
}

void GizBombGens_Update(void *world_pointer, void *system_pointer, f32 delta_time) {
    WORLDINFO *world = world_pointer;
    GIZBOMBGENSYS_s *system = system_pointer;
    u16 model;
    GAMEOBJECT *player;
    CABLE_s *cable;
    if (system != NULL) {
        GIZBOMBGEN_s *generator;
        i32 index;
        if ((system->flags & 1) != 0) {
            BombGeneratorsApplyReset(system, system->progress);
            system->flags &= (u16)~1u;
        }
        generator = system->bomb_generators;
        for (index = 0; index < system->count; ++index, ++generator) {
            if ((generator->flags & GIZBOMBGEN_FLAG_VISIBLE) == 0 || (generator->flags & 1) == 0 || generator->anim_set == NULL) {
                continue;
            }
            if (generator->generated_bomb != NULL) {
                GAMEOBJECT *bomb;
                GameAnimSet_SetVisibility(generator->anim_set, 0);
                bomb = generator->generated_bomb;
                if (bomb->dead != 0 || (bomb->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0) {
                    GAMEANIMSET *anim_set = generator->anim_set;
                    generator->generated_bomb = NULL;
                    GameAnimSet_Reset(anim_set);
                    GameAnimSet_EvalAnim(generator->anim_set);
                    GameAnimSet_SetVisibility(generator->anim_set, 1);
                }
            } else {
                GameAnimSet_Play(generator->anim_set, 1.0f, 1);
                if (generator->anim_set->state == GAMEANIMSET_STATE_AT_END) {
                    NUVEC position;
                    GameAnimSet_GetAveragePos(generator->anim_set, &position, 2, 1, 1);
                    if (netclient == 0) {
                        generator->generated_bomb =
                            AddDynamicCreature((i16)id_DRAGBOMB, &position, 0, "dragbomb", NULL, NULL, 1, NULL, NULL, 0, 0);
                        if (Cheat_IsOn(5) != 0) {
                            AddGameDebris(world->game_debris_sys, 0x7d, &position);
                            AddGameDebris(world->game_debris_sys, 0x7e, &position);
                        }
                    }
                }
            }
        }
    }
    player = Player[0];
    model = id_DRAGBOMB;
    BombGenerator_PlayerBomb[0] = NULL;
    if (player != NULL) {
        cable = player->cable;
        if (cable != NULL && cable->target != NULL && cable->target->id == model) {
            BombGenerator_PlayerBomb[0] = cable->target;
        }
    }
    player = Player[1];
    BombGenerator_PlayerBomb[1] = NULL;
    if (player != NULL) {
        cable = player->cable;
        if (cable != NULL && cable->target != NULL && cable->target->id == model) {
            BombGenerator_PlayerBomb[1] = cable->target;
        }
    }
}

i32 GizBombGens_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    if (world != NULL) {
        return world->current_level->max_bombgens;
    }
    return 0;
}

void GizBombGens_AddGizmos(GIZMOSYS *giz, i32 type, void *world, void *data) {
    GIZBOMBGENSYS_s *s = data;
    i32 i;
    if (s == NULL) {
        return;
    }
    for (i = 0; i < s->count; ++i) {
        if (NuStrLen(s->bomb_generators[i].name) != 0) {
            AddGizmo(giz, type, NULL, &s->bomb_generators[i]);
        }
    }
}

char *GizmoBombGen_GetGizmoName(GIZMO *gizmo) {
    if (gizmo != NULL) {
        return gizmo->bomb_generator->name;
    }
    return NULL;
}

i32 GizmoBombGen_GetOutput(GIZMO *gizmo, i32 output, i32 mode) {
    return gizmo->bomb_generator->flags & GIZBOMBGEN_FLAG_ACTIVE;
}

char *GizmoBombGen_GetOutputName(GIZMO *gizmo, i32 output) {
    return "Active";
}

i32 GizmoBombGen_GetNumOutputs(GIZMO *gizmo) {
    return 1;
}

void *GizBombGens_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 0x18);
}

void GizBombGens_ClearProgress(void *owner, void *progress_ptr) {
    BOMB_PROGRESS *progress = progress_ptr;
    if (progress == NULL) {
        return;
    }
    memset(progress->enabled, -1, sizeof(progress->enabled));
    memset(progress->visible, -1, sizeof(progress->visible));
}

GIZBOMBGEN_s *GizBombGen_FindByName(GIZBOMBGENSYS_s *system, char *name) {
    GIZBOMBGEN_s *generator = NULL;
    i32 index;
    if (system != NULL && name != NULL) {
        generator = system->bomb_generators;
        for (index = 0; index < system->count; ++index, ++generator) {
            if (NuStrICmp(generator->name, name) == 0) {
                return generator;
            }
        }
    }
    // The original returns the end pointer when no generator has this name.
    return generator;
}

void GizBombGens_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    BOMB_PROGRESS *progress = progress_ptr;
    BOMB_PROGRESS_SYSTEM *system = data;
    BOMB_PROGRESS_RECORD *record;
    i32 index;
    i32 word;
    u32 bit;
    if (progress == NULL) {
        return;
    }
    memset(progress->enabled, -1, sizeof(progress->enabled));
    memset(progress->visible, -1, sizeof(progress->visible));

    record = system->records;
    for (index = 0; index < system->count; ++index, ++record) {
        if (index >= 96) {
            break;
        }
        word = index / 32;
        bit = 1u << (index & 31);
        if (!(record->flags & 2)) {
            progress->visible[word] &= ~bit;
        }
        if (!(record->flags & 1)) {
            progress->enabled[word] &= ~bit;
        }
    }
}

i32 bombgen_gizmotype_id = -1;
char *GizmoRegistryGizBombGenNamePtr = "BombGenerator";
GAMEOBJECT *BombGenerator_PlayerBomb[2] = {0};
static ADDGIZMOTYPE GizmoRegistryGizBombGenAddType = {0};

ADDGIZMOTYPE *GizBombGen_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE *addtype = &GizmoRegistryGizBombGenAddType;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->name = GizmoRegistryGizBombGenNamePtr;
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
    addtype->fns.get_max_gizmos_fn = (i32(*)(void *))GizBombGens_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)GizBombGens_AddGizmos;
    addtype->fns.late_update_fn = (void (*)(void *, void *, f32))GizBombGens_Update;
    addtype->fns.get_gizmo_name_fn = (char *(*)(GIZMO *))GizmoBombGen_GetGizmoName;
    addtype->fns.get_output_fn = (i32(*)(GIZMO *, i32, i32))GizmoBombGen_GetOutput;
    addtype->fns.get_output_name_fn = (char *(*)(GIZMO *, i32))GizmoBombGen_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)GizmoBombGen_GetNumOutputs;
    addtype->fns.activate_fn = (void (*)(GIZMO *, i32))GizmoBombGen_Activate;
    addtype->fns.set_visibility_fn = (void (*)(GIZMO *, i32))GizmoBombGen_SetVisibility;
    addtype->fns.get_pos_fn = (void *)GizmoBombGen_GetPos;
    addtype->fns.allocate_progress_data_fn = (void *(*)(VARIPTR *, VARIPTR *))GizBombGens_AllocateProgressData;
    addtype->fns.clear_progress_fn = (void (*)(void *, void *))GizBombGens_ClearProgress;
    addtype->fns.store_progress_fn = (void (*)(void *, void *, void *))GizBombGens_StoreProgress;
    addtype->fns.reset_fn = (void (*)(void *, void *, void *))GizBombGens_SetResetFlag;
    addtype->fns.reserve_buffer_space_fn = (void *(*)(void *))GizBombGens_ReserveBufferSpace;
    addtype->fns.load_fn = (i32(*)(void *, void *))GizBombGens_Load;
    bombgen_gizmotype_id = type_id;
    return addtype;
}
