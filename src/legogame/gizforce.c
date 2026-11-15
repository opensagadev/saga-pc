#include "gameapi/edptl.h"
#include "gameapi/edtools/edfile.h"
#include "gamelib/util/Utilities.h"
#include "legoapi/actions/combat/shovesys.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/playeritems.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmos/door/plugs.h"
#include "legoapi/gizmos/door/zipups.h"
#include "legoapi/gizmos/fx/edgizshadowmachine.h"
#include "legoapi/gizmos/object/gizobstacles.h"
#include "legoapi/gizmos/object/hatmachine.h"
#include "legoapi/gizmos/object/lever.h"
#include "legoapi/gizmos/transport/gizportal.h"
#include "legoapi/gizmos/transport/tubes.h"
#include "legoapi/gizmos/traps/gizforce.h"
#include "legoapi/gizmos/trigger/gizspecial.h"
#include "legoapi/gizmos/trigger/minicut.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolt.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/camera.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/supportall.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/props/objects/techno.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/render/light/shadow.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episodeIII.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "legogame/game.h"
#include "legogame/gamestatus.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/nuportal.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic/sfx.h"

typedef struct GIZFORCE_GROUP_BEST {
    GIZFORCE *forces[9];
    u8 flags;
    u8 unknown_25;
    u8 count;
} GIZFORCE_GROUP_BEST;

typedef struct GIZFORCE_BEST {
    u8 unknown_000[0x1c];
    NUVEC position;
    GAMEANIMSET *anim_set;
    u8 unknown_02c[0xc];
    GAMEOBJECT *using_object;
    u32 unavailable_flags;
    GIZFORCE_GROUP_BEST *group;
    u8 unknown_044[0x10];
    f32 interaction_radius;
    u8 unknown_058[0x1c];
    u32 config_flags;
    u8 unknown_078[0x28];
    u32 runtime_flags;
} GIZFORCE_BEST;

typedef struct GIZFORCE_SYS_BEST {
    u8 unknown_000[4];
    GIZFORCE **visible_forces;
    u8 unknown_008[8];
    u16 visible_count;
    u8 unknown_012[2];
    GAMEANIMOBJPOOL *anim_pool;
} GIZFORCE_SYS_BEST;

typedef struct GIZFORCE_OBJECT_BEST {
    u8 unknown_000[0x10cc];
    GIZFORCE *target;
    GAMEANIMOBJ *animation;
} GIZFORCE_OBJECT_BEST;

typedef struct force_sfx_config_s {
    u8 unknown_00[0x28];
    GAMEANIMSET *anim_set;
    u8 unknown_2c[0x82 - 0x2c];
    i16 process_sfx;
    i16 complete_sfx;
    i16 return_sfx;
    u8 unknown_88[0xa4 - 0x88];
} FORCE_SFX_CONFIG;

typedef struct force_sfx_system_s {
    FORCE_SFX_CONFIG *forces;
    u8 unknown_04[0xe - 4];
    u16 count;
} FORCE_SFX_SYSTEM;

typedef struct world_force_sfx_s {
    u8 unknown_0000[0x46ac];
    FORCE_SFX_SYSTEM *force_system;
} WORLD_FORCE_SFX;

typedef struct trigger_force_object_s {
    u8 unknown_000[0x10cc];
    GIZFORCE *force;
} TRIGGER_FORCE_OBJECT;

void GameAnimSet_SetOffset(GAMEANIMSET *, NUVEC *);

typedef struct registry_force_record_pc_s {
    u8 unknown_00[0x28];
    GAMEANIMSET *anim_set;
    u8 unknown_2c[0x5c];
    i16 blowup_type;
    u8 unknown_8a[0x16];
    u32 runtime_flags;
} REGISTRY_FORCE_RECORD;

typedef struct registry_force_system_pc_s {
    REGISTRY_FORCE_RECORD *forces;
    REGISTRY_FORCE_RECORD **visible_forces;
    GIZMO_PC **hit_test_gizmos;
    u16 capacity;
    u16 count;
    u16 visible_force_count;
    u16 hit_test_gizmo_count;
    GAMEANIMOBJPOOL *anim_pool;
    GIZFORCEGROUP_s groups[8];
} REGISTRY_FORCE_SYSTEM;

void RegisterGizmoTypes(VARIPTR *, VARIPTR *, REGISTER_GIZMO_CONFIG_TYPE_FN *, i32);
ADDGIZMOTYPE *GizBuildIts_RegisterGizmo(i32);
ADDGIZMOTYPE *NewBlowup_RegisterGizmo(i32);
ADDGIZMOTYPE *GizmoPickups_RegisterGizmo(i32);
ADDGIZMOTYPE *Shards_RegisterGizmo(i32);
ADDGIZMOTYPE *Signals_RegisterGizmo(i32);
ADDGIZMOTYPE *Grapples_RegisterGizmo(i32);
ADDGIZMOTYPE *TightRopes_RegisterGizmo(i32);
ADDGIZMOTYPE *Ledges_RegisterGizmo(i32);
ADDGIZMOTYPE *Spinner_RegisterGizmo(i32);
ADDGIZMOTYPE *SecurityDoors_RegisterGizmo(i32);
ADDGIZMOTYPE *Attractos_RegisterGizmo(i32);
ADDGIZMOTYPE *GuideLines_RegisterGizmo(i32);
ADDGIZMOTYPE *GizTurrets_RegisterGizmo(i32);
ADDGIZMOTYPE *AI_RegisterGizmo(i32);
ADDGIZMOTYPE *GizTimer_RegisterGizmo(i32);
ADDGIZMOTYPE *GizRandom_RegisterGizmo(i32);
ADDGIZMOTYPE *Door_RegisterGizmo(i32);
ADDGIZMOTYPE *GizAIMessage_RegisterGizmo(i32);
ADDGIZMOTYPE *Push_RegisterGizmo(i32);
ADDGIZMOTYPE *GizBombGen_RegisterGizmo(i32);
ADDGIZMOTYPE *GizPanel_RegisterGizmo(i32);
ADDGIZMOTYPE *Teleport_RegisterGizmo(i32);
ADDGIZMOTYPE *GizTorpMachine_RegisterGizmo(i32);

void GizForce_ResetLOS(GAMEOBJECT *object) {
    if (object->gizforce_los_info != NULL) {
        memset(object->gizforce_los_info, 0, 0x630);
    }
}

static FORCE_SFX_CONFIG *FindForceForSfx(WORLD_FORCE_SFX *world, char *name) {
    FORCE_SFX_SYSTEM *system = world->force_system;
    FORCE_SFX_CONFIG *force;
    i32 i;
    if (system != NULL) {
        force = system->forces;
        for (i = 0; i < system->count; ++i, ++force) {
            GAMEANIMOBJ *object;
            if (force->anim_set == NULL) {
                continue;
            }
            for (object = force->anim_set->objects; object != NULL; object = object->next) {
                char *special_name = NuSpecialGetName(&object->special);
                if (special_name != NULL && NuStrICmp(name, special_name) == 0) {
                    return force;
                }
            }
        }
    }
    return NULL;
}

static FORCE_SFX_CONFIG *GizForceSFX_force = 0;
static WORLD_FORCE_SFX *GizForceSFX_worldinfo = 0;

static void GizForceSFX_forcename(NUFPAR *parser) {
    GizForceSFX_force = NULL;
    if (NuFParGetWord(parser) != 0) {
        GizForceSFX_force = FindForceForSfx(GizForceSFX_worldinfo, parser->word_buffer_ptr);
    }
}

static void GizForceSFX_processsfx(NUFPAR *parser) {
    if (GizForceSFX_force != NULL && NuFParGetWord(parser) != 0 && GizForceSFX_force->process_sfx == -1) {
        GizForceSFX_force->process_sfx = (i16)GetSfxId(parser->word_buffer_ptr);
    }
}

static void GizForceSFX_completesfx(NUFPAR *parser) {
    if (GizForceSFX_force != NULL && NuFParGetWord(parser) != 0 && GizForceSFX_force->complete_sfx == -1) {
        GizForceSFX_force->complete_sfx = (i16)GetSfxId(parser->word_buffer_ptr);
    }
}

static void GizForceSFX_returnsfx(NUFPAR *parser) {
    if (GizForceSFX_force != NULL && NuFParGetWord(parser) != 0 && GizForceSFX_force->return_sfx == -1) {
        GizForceSFX_force->return_sfx = (i16)GetSfxId(parser->word_buffer_ptr);
    }
}

static i32 GizForceSFX_load_version = 99999;
static NUFPCOMJMP GizForceSFX_ConfigKeywords[] = {
    {"forcename",   GizForceSFX_forcename  },
    {"processsfx",  GizForceSFX_processsfx },
    {"completesfx", GizForceSFX_completesfx},
    {"returnsfx",   GizForceSFX_returnsfx  },
    {NULL,          NULL                   }
};

void GizForceSFX_Configure(WORLDINFO *world_ptr, char *config) {
    WORLD_FORCE_SFX *world = (WORLD_FORCE_SFX *)world_ptr;
    NUFPAR *parser;
    i32 inside;
    if (GizForceSFX_load_version >= 16 || world == NULL || world->force_system == NULL || world->force_system->count == 0) {
        return;
    }
    GizForceSFX_force = NULL;
    GizForceSFX_worldinfo = world;
    parser = NuFParCreateMem("ForceSFX", config, 0xffff);
    if (parser == NULL) {
        return;
    }
    NuFParPushCom(parser, GizForceSFX_ConfigKeywords);
    inside = 0;
    while (NuFParGetLine(parser) != 0) {
        while (NuFParGetWord(parser) != 0) {
            if (inside) {
                if (NuStrICmp(parser->word_buffer_ptr, "forcesfx_end") == 0) {
                    inside = 0;
                } else {
                    NuFParInterpretWord(parser);
                }
            } else if (NuStrICmp(parser->word_buffer_ptr, "forcesfx_start") == 0) {
                inside = 1;
            }
        }
    }
    NuFParPopCom(parser);
    NuFParDestroy(parser);
}

static char *ForceCopyNamePrefix(char *destination, char *source, i32 capacity) {
    char *result = destination;
    i32 index = 0;
    if (destination == NULL || source == NULL) {
        return NULL;
    }
    do {
        char value;
        if (index++ >= capacity - 1) {
            break;
        }
        value = *source;
        *destination++ = value;
        source++;
        if (value == '_') {
            break;
        }
    } while (*source != '\0');
    return result;
}

static void ForceFindBaseSpecial(GIZFORCE *force) {
    GAMEANIMOBJ *object;
    char *name;
    char socket_name[64];
    if (force == NULL) {
        return;
    }
    memset(&force->along_socket, 0, sizeof(force->along_socket));
    if (!(force->config_flags & 0x40)) {
        return;
    }
    for (object = force->anim_set->objects; object != NULL; object = object->next) {
        name = NuSpecialGetName(&object->special);
        if (name != NULL) {
            memset(socket_name, 0, sizeof(socket_name));
            if (ForceCopyNamePrefix(socket_name, name, sizeof(socket_name)) != NULL) {
                NuStrCat(socket_name, "b");
                if (NuSpecialFind(object->special.scene, &force->along_socket, socket_name, 1) != 0) {
                    break;
                }
            }
        }
    }
}

static void edgizforce_ReadAnimSetData(GAMEANIMOBJ *object, u8 version) {
    GIZFORCEANIMDATA_s temporary;
    GIZFORCEANIMDATA_s *data;
    if (object == NULL) {
        return;
    }
    data = object->object_data;
    if (data == NULL) {
        data = &temporary;
    }
    if (version > 8) {
        data->flags = (u16)EdFileReadShort();
    }
}

i32 GizForces_Load(void *world_pointer, void *system_pointer) {
    WORLDINFO *world = world_pointer;
    GIZFORCESYS_s *system = system_pointer;
    GIZFORCE *force;
    u8 version;
    i32 index;
    i32 length;
    char blowup_name[32];
    char sfx_name[64];
    NUVEC unused;
    version = (u8)EdFileReadChar();
    GizForceSFX_load_version = version;
    system->count = (u16)EdFileReadShort();
    if (system->count == 0) {
        return 1;
    }
    force = (GIZFORCE *)system->forces;
    for (index = 0; index < system->count; ++index, ++force) {
        EdFileRead(force->name, 16);
        EdFileReadNuVec(&force->file_position);
        if (version == 1) {
            EdFileReadNuVec(&unused);
        }
        force->force_strength = EdFileReadFloat();
        if (version >= 8) {
            force->force_range = EdFileReadFloat();
        }
        force->interaction_radius = EdFileReadFloat();
        if (version == 1) {
            EdFileReadNuVec(&unused);
            EdFileReadShort();
        }
        force->config_flags = (u32)EdFileReadInt();
        if (version <= 12) {
            force->config_flags |= 0x800;
        }
        force->group_index = (u8)EdFileReadChar();
        if (version >= 11) {
            force->collision_mask = (u8)EdFileReadChar();
            if (version == 11 && force->collision_mask != 0) {
                force->collision_mask = (u8)(1 << (force->collision_mask - 1));
            }
        }
        force->activation_mode = (u8)EdFileReadChar();
        if (version == 1) {
            EdFileReadChar();
        }
        GizmoFileReadGameAnimSet(force->anim_set, world, edgizforce_ReadAnimSetData, version, "GizForce", force->name);
        force->start_frame = EdFileReadFloat();
        force->end_frame = EdFileReadFloat();
        if (version >= 6) {
            force->effect_scale = EdFileReadFloat();
        }
        if (version >= 7) {
            force->horizontal_range = EdFileReadFloat();
        } else {
            force->horizontal_range = 1.0f;
        }
        if (version >= 3) {
            force->vertical_range = EdFileReadFloat();
        }
        force->blowup_type = -1;
        if (version >= 4) {
            if (version >= 5) {
                length = EdFileReadChar();
                if (length != 0) {
                    EdFileRead(blowup_name, length);
                    force->blowup_type = (i16)GizmoBlowupGetNameTableId(blowup_name);
                    if (force->blowup_type != -1) {
                        force->runtime_flags |= 0x400;
                    }
                }
            } else {
                force->blowup_type = EdFileReadShort();
            }
            force->pickup_count = EdFileReadShort();
            force->pickup_rotation_x = EdFileReadShort();
            force->pickup_rotation_y = EdFileReadShort();
            EdFileReadNuVec(&force->effect_position);
        }
        if (version >= 10) {
            force->activation_radius = EdFileReadFloat();
        } else if (world->area != NULL && (world->area->flags & AREAFLAG_VEHICLE_AREA)) {
            force->activation_radius = 12.0f;
        } else {
            force->activation_radius = 1.75f;
        }
        force->start_sfx_id = -1;
        force->loop_sfx_id = -1;
        force->stop_sfx_id = -1;
        if (version >= 14) {
            if (GizmoFileReadName(sfx_name)) {
                force->start_sfx_id = (i16)GetSfxId(sfx_name);
            }
            if (GizmoFileReadName(sfx_name)) {
                force->loop_sfx_id = (i16)GetSfxId(sfx_name);
            }
            if (GizmoFileReadName(sfx_name)) {
                force->stop_sfx_id = (i16)GetSfxId(sfx_name);
            }
        }
        if (version <= 14) {
            force->start_sfx_id = -1;
            force->loop_sfx_id = -1;
            force->stop_sfx_id = -1;
        }
        if (force->config_flags & 0x40) {
            ForceFindBaseSpecial(force);
        }
    }
    return 1;
}

void GizForces_PostLoad(void *world, void *pointer) {
    REGISTRY_FORCE_SYSTEM *system = pointer;
    REGISTRY_FORCE_RECORD *force;
    i32 i;
    if (system == NULL) {
        return;
    }
    force = system->forces;
    for (i = 0; i < system->count; ++i, ++force) {
        if (force->runtime_flags & 0x400) {
            i32 type_id = GizmoBlowupGetTypeFromNameTableId(world, force->blowup_type);
            force->runtime_flags &= ~0x400u;
            force->blowup_type = (i16)type_id;
        }
    }
}

void GizForces_AddLevelSfx(void *world, void *system_pointer, i32 *ids, i32 *count, i32 capacity) {
    GIZFORCESYS_s *system = system_pointer;
    GIZFORCE *force;
    i32 index;
    (void)world;
    if (system == NULL) {
        return;
    }
    force = (GIZFORCE *)system->forces;
    for (index = 0; index < system->count; ++index, ++force) {
        if (force->start_sfx_id != -1) {
            AddLevelSfxFromId(force->start_sfx_id, ids, count, capacity);
        }
        if (force->loop_sfx_id != -1) {
            AddLevelSfxFromId(force->loop_sfx_id, ids, count, capacity);
        }
        if (force->stop_sfx_id != -1) {
            AddLevelSfxFromId(force->stop_sfx_id, ids, count, capacity);
        }
    }
}

void *GizForces_ReserveBufferSpace(void *pointer) {
    WORLDINFO *world = pointer;
    REGISTRY_FORCE_SYSTEM *system;
    i32 i;
    system = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, sizeof(*system));
    system->capacity = world->current_level->max_force;
    system->forces = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, system->capacity * sizeof(*system->forces));
    system->hit_test_gizmos = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, system->capacity * sizeof(void *));
    system->visible_forces = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, system->capacity * sizeof(void *));
    system->anim_pool =
        GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 8, (u16)world->current_level->max_force_objects);
    for (i = 0; i < system->capacity; ++i) {
        system->forces[i].anim_set = GameAnimSet_Create(&world->giz_buffer, &world->buffer_end, system->anim_pool, world->game_anim_sys);
    }
    world->giz_force_sys = (GIZFORCESYS_s *)system;
    return system;
}

void GizForce_SetVisibility(GIZFORCE *force, i32 visible) {
    if (force != NULL) {
        GameAnimSet_SetVisibility(force->anim_set, visible);
        force->visible = visible != 0;
    }
}

void GizmoForce_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    GIZFORCE *force;
    if (gizmo == NULL) {
        return;
    }
    force = gizmo->object;
    if (force == NULL) {
        return;
    }
    GameAnimSet_SetVisibility(force->anim_set, visible);
    force->visible = visible != 0;
}

NUVEC *GizmoForce_GetPos(GIZMO_PC *gizmo) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return NULL;
    }
    return &((FLOW_FORCE_POSITION *)gizmo->object)->position;
}

static i32 ForceUpdateCentre(GIZFORCE *force, i32 frame_selection) {
    force->position = force->file_position;
    force->radius = 1.0f;
    return GameAnimSet_GetCentreAndRadius(force->anim_set, &force->position, &force->radius, frame_selection, 1, 1);
}

static void ForceResetSingle(WORLDINFO *world, GIZFORCE *force) {
    GAMEANIMOBJ *object;
    force->runtime_flags = (force->runtime_flags & 0xffe2049fu) | 3;
    force->animation_speed = 1.0f;
    force->unknown_48 = 0.0f;
    force->unknown_50 = 0.0f;
    force->reset_word = 0;
    if (force->anim_set == NULL) {
        return;
    }
    for (object = force->anim_set->objects; object != NULL; object = object->next) {
        GIZFORCEANIMDATA_s *data = object->object_data;
        data->platform_id = -1;
        if (world->terrain != NULL && (data->flags & 1) == 0 && NuSpecialExistsFn(&object->special) != 0) {
            data->platform_id = (i16)FindPlatInst(NuSpecialGetInstanceix(&object->special));
            if (data->platform_id != -1) {
                force->runtime_flags |= 0x100;
            }
        }
    }
    force->room_id = world->current_gscn != NULL ? NuPortalWhichRoom(world->current_gscn, &force->file_position) : -1;
    GameAnimSet_SetOffset(force->anim_set, &v000);
    force->position = force->file_position;
    force->radius = 1.0f;
    GameAnimSet_GetCentreAndRadius(force->anim_set, &force->position, &force->radius, 2, 1, 1);
    GameAnimSet_EvaluateState(force->anim_set);
}

f32 ForceAnimationSeekRate = 5.0f;

static f32 ForceUpdateSpeed(GIZFORCE *force, GAMEOBJECT *object) {
    f32 target = 1.0f;
    if (force != NULL) {
        // MATCH-EXCEPTION: the original stores the default speed again once force is known (second fstp of 1.0).
        target = 1.0f;
        if (object != NULL && (Cheats_CheckFlags(0x400000) != 0 || object->powerup_time > 0.0f)) {
            target = 3.0f;
        }
    }
    return SeekValF(force->animation_speed, target, ForceAnimationSeekRate);
}

static void GizForce_PlayForwardsFn(GIZFORCE *force) {
    f32 speed;
    if (force != NULL) {
        GameAnimSet_SetRepeating(force->anim_set, 0);
        if (force->animation_speed < 0.0f) {
            speed = force->end_frame * force->animation_speed;
            GameAnimSet_Play(force->anim_set, speed, 0);
        } else {
            speed = force->start_frame * force->animation_speed;
            GameAnimSet_Play(force->anim_set, speed, 0);
        }
    }
}

static void GizForce_PlayBackwardsFn(GIZFORCE *force) {
    f32 speed;
    if (force != NULL) {
        GameAnimSet_SetRepeating(force->anim_set, 0);
        if (force->animation_speed < 0.0f) {
            speed = force->start_frame * -force->animation_speed;
            GameAnimSet_Play(force->anim_set, speed, 0);
        } else {
            speed = force->end_frame * -force->animation_speed;
            GameAnimSet_Play(force->anim_set, speed, 0);
        }
    }
}

void GizForce_PlayForwards(GIZFORCE *force) {
    if (force != NULL) {
        if (force->runtime_flags & 0x40) {
            GizForce_PlayBackwardsFn(force);
        } else {
            GizForce_PlayForwardsFn(force);
        }
    }
}

void GizForce_PlayBackwards(GIZFORCE *force) {
    if (force != NULL) {
        if (force->runtime_flags & 0x40) {
            GizForce_PlayForwardsFn(force);
        } else {
            GizForce_PlayBackwardsFn(force);
        }
    }
}

static f32 GizForce_GetHeight(GIZFORCE *force) {
    GAMEANIMOBJ *object;
    f32 minimum_y;
    f32 maximum_y;
    f32 height;
    if (force == NULL || force->anim_set == NULL) {
        return 0.0f;
    }
    minimum_y = 1.0e9f;
    maximum_y = -1.0e9f;
    for (object = force->anim_set->objects; object != NULL; object = object->next) {
        NUVEC minimum;
        NUVEC maximum;
        NuSpecialGetBounds(&object->special, &minimum, &maximum);
        if (minimum.y < minimum_y) {
            minimum_y = minimum.y;
        }
        if (maximum.y > maximum_y) {
            maximum_y = maximum.y;
        }
    }
    height = maximum_y - minimum_y;
    return height;
}

static void GizForce_AddToStack(GIZFORCE *force) {
    if ((force->runtime_flags & 0x20) == 0 && force->group != NULL && force->group->member_count < 8) {
        if (force->group->member_count != 0) {
            GIZFORCE *previous = force->group->forces[force->group->member_count - 1];
            force->group->combined_height += GizForce_GetHeight(previous);
        }
        force->group->forces[force->group->member_count] = force;
        ++force->group->member_count;
        force->runtime_flags |= 0x20;
    }
}

static void ForceGroupPop(GIZFORCE *force) {
    if ((force->runtime_flags & 0x20) != 0 && force->group != NULL && force->group->member_count != 0 &&
        force->group->forces[force->group->member_count - 1] == force) {
        /* Clears slot COUNT (not COUNT-1); count8 aliases combined_height. */
        force->group->slot_words[force->group->member_count] = 0;
        --force->group->member_count;
        if (force->group->member_count != 0) {
            GIZFORCE *top = force->group->forces[force->group->member_count - 1];
            force->group->combined_height -= GizForce_GetHeight(top);
        }
        force->runtime_flags &= ~0x20u;
        GameAnimSet_SetOffset(force->anim_set, &v000);
    }
}

i32 GizForce_UpdateHint(HINT *hint) {
    i32 index;
    (void)hint;
    if (WORLD->area != NULL && WORLD->area == HUB_ADATA) {
        return 0;
    }
    for (index = 0; index < 2; ++index) {
        GAMEOBJECT *object = Player[index];
        if (object != NULL && (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && object->transform_alpha > 0.0f &&
            object->transform_target != NULL && object->glow_kind == 0) {
            return 1;
        }
    }
    return 0;
}

void GizForces_Draw(void *world_pointer, void *system_pointer, f32 unused) {
    WORLDINFO *world = world_pointer;
    GIZFORCESYS_s *system = system_pointer;
    GIZFORCE *force;
    GAMEANIMOBJ *object;
    NUMTX *draw_matrix;
    NUMTX reflection;
    f32 reflection_height;
    i32 index;
    (void)unused;
    if (system == NULL) {
        return;
    }
    force = (GIZFORCE *)system->forces;
    for (index = 0; index < system->count; ++index, ++force) {
        if (!(force->runtime_flags & 2) || !(force->runtime_flags & 0x80) ||
            (force->room_id >= 0 && world->rooms_visible_ptr[force->room_id] == 0)) {
            continue;
        }
        ResetShadowMapRendering();
        if (force->config_flags & 4) {
            GameAnimSet_DrawReflection(force->anim_set, 2, force->vertical_range, NULL);
        }
        if (!(force->config_flags & 0x1000)) {
            EnableShadowMapRendering(0);
        }
        if (!(force->config_flags & 0x40) || !(force->runtime_flags & 0x800)) {
            continue;
        }
        object = force->anim_set->objects;
        if (object == NULL) {
            continue;
        }
        draw_matrix = NuSpecialGetDrawMtx(&object->special);
        NuSpecialDrawAt(&force->along_socket, draw_matrix);
        if (force->config_flags & 4) {
            reflection_height = force->vertical_range + draw_matrix->m31;
            if (MatrixReflection(draw_matrix, 2, reflection_height, WORLD->current_level->reflection_y, &reflection)) {
                NuRndrStartReflectionRender(0);
                NuSpecialDrawAt(&force->along_socket, &reflection);
                NuRndrEndReflectionRender();
            }
        }
    }
    ResetShadowMapRendering();
}

i32 GizForce_StoodOnForce(GIZFORCE *force, GAMEOBJECT *object) {
    GAMEANIMOBJ *animation;
    if (!(force->runtime_flags & 0x100) || object->terrain_platform_id == -1) {
        return 0;
    }
    for (animation = force->anim_set->objects; animation != NULL; animation = animation->next) {
        if (((GIZFORCE_ANIMATION_DATA *)animation->object_data)->platform_id == object->terrain_platform_id) {
            return 1;
        }
    }
    return 0;
}

u16 GizForces_AngleToForce(NUVEC *position, GIZFORCE *force) {
    f32 dz = force->position.z - position->z;
    return NuAtan2D(force->position.x - position->x, dz);
}

i32 ForceAnimationDone(GIZFORCE *force) {
    if (force != NULL && force->anim_set != NULL) {
        if (force->runtime_flags & 0x40) {
            if (force->anim_set->state != 0) {
                return 0;
            }
        } else if (force->anim_set->state != 2) {
            return 0;
        }
    }
    return 1;
}

i32 GizForce_Complete(GIZFORCE *force) {
    if (!(force->config_flags & 2) && (!(force->force_range > 0.0f) || (force->runtime_flags & 0x2000)) &&
        (!(force->config_flags & 0x40) || (force->runtime_flags & 0x800))) {
        if (force->anim_set != NULL) {
            if (force->runtime_flags & 0x40) {
                if (force->anim_set->state != 0) {
                    return 0;
                }
            } else if (force->anim_set->state != 2) {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

i32 GizForce_GameObjUsingForce(GAMEOBJECT *object, GIZFORCE *force) {
    return force != NULL && object != NULL && object->character_context == CHARACTER_CONTEXT_FORCE &&
           ((TRIGGER_FORCE_OBJECT *)object)->force == force;
}

PART_PC *GizForce_Throw(GAMEOBJECT *object, GIZFORCE *force, f32 speed, f32 gravity, i32 unused) {
    nuhspecial_s *special = &force->anim_set->objects->special;
    NUMTX translated;
    NUMTX *matrix;
    NUVEC velocity;
    ADDPART params;
    PART_PC *part;
    (void)unused;
    if (WORLD->current_level == MAULF_LDATA) {
        NuMtxSetTranslation(&translated, &ForceThrowMaulPosition);
        matrix = &translated;
    } else {
        matrix = NuSpecialGetMtx(special);
    }
    if (object == NULL || object->force_throw_target == NULL) {
        return NULL;
    }
    MakeThrowVector(&velocity, (NUVEC *)&matrix->m30, &object->force_throw_target->collision_position, &v000, speed, gravity);
    params = Default_ADDPART;
    params.matrix = matrix;
    params.velocity = &velocity;
    params.unknown_18 = 0.15f;
    params.unknown_14 = 0.15f;
    params.gravity = gravity;
    params.special = special;
    params.flags = 0xc39b;
    params.owner = object;
    params.hostility_group = object->hostility_group;
    params.collision_fn = PartCollide_3D;
    params.kill_fn = PartKill_ForceThrow;
    params.time_step = FRAMETIME;
    part = AddPart(&params);
    NewRumble(object->pad->bound_pad, 0.5f, 0);
    PlaySfx("JForcePush", &object->collision_position);
    GameAnimSet_SetVisibility(force->anim_set, 0);
    force->runtime_flags |= 0x10000;
    return part;
}

i32 GizForces_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    if (world != NULL) {
        return world->current_level->max_force;
    }
    return 0;
}

void GizForces_AddGizmos(GIZMOSYS *giz, i32 type, void *world, void *data) {
    GIZFORCESYS_s *s = data;
    i32 i;
    (void)world;
    if (s == NULL) {
        return;
    }
    for (i = 0; i < s->count; ++i) {
        if (NuStrLen(((ADD_FORCE *)s->forces)[i].name) != 0) {
            AddGizmo(giz, type, NULL, &((ADD_FORCE *)s->forces)[i]);
        }
    }
}

char *GizmoForce_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? (char *)gizmo->object : NULL;
}

i32 GizmoForce_GetOutput(GIZMO_PC *gizmo, i32 output_index, i32 force_read) {
    GIZFORCE *force = gizmo->object;
    if (((!(force->runtime_flags & 2) && force->blowup_type == -1) || !(force->runtime_flags & 1)) && force_read == 0) {
        return 0;
    }
    switch (output_index) {
        case 0:
            if (force->anim_set != NULL && force->anim_set->state == 2) {
                return 1;
            }
            break;
        case 1:
            if (force->anim_set != NULL && force->anim_set->state != 0) {
                return 1;
            }
            break;
        case 2:
            if (force->anim_set != NULL && force->anim_set->state == 0) {
                return 1;
            }
            break;
        case 3:
            if (force->group != NULL && (force->group->runtime_flags & 2) != 0) {
                return 1;
            }
            break;
        case 4:
            if (force->group != NULL && (force->group->runtime_flags & 4) != 0) {
                return 1;
            }
            break;
        case 5:
            if (force->runtime_flags & 0x10000) {
                return 1;
            }
            break;
        case 6:
            return GizForce_Complete(force);
        case 7:
            if (force->runtime_flags & 0x200000) {
                return 1;
            }
            break;
    }
    return 0;
}

char *GizmoForce_GetOutputName(GIZMO_PC *unused0, i32 output_index) {
    switch (output_index) {
        case 0:
            return ((char *)("AtEnd"));
        case 1:
            return ((char *)("NotAtStart"));
        case 2:
            return ((char *)("AtStart"));
        case 3:
            return ((char *)("StackComplete"));
        case 4:
            return ((char *)("StackCompleteInOrder"));
        case 5:
            return ((char *)("Destroyed/Thrown"));
        case 6:
            return ((char *)("Complete"));
        case 7:
            return ((char *)("BeingUsed"));
        default:
            return NULL;
    }
}

i32 GizmoForce_GetNumOutputs(GIZMO_PC *unused) {
    (void)unused;
    return 8;
}

void *GizForces_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 0xb0);
}

void GizForces_ClearProgress(void *owner, void *progress_ptr) {
    FORCE_PROGRESS *progress = progress_ptr;
    if (progress == NULL) {
        return;
    }
    memset(progress->enabled, -1, sizeof(progress->enabled));
    memset(progress->visible, -1, sizeof(progress->visible));
    memset(progress->reward, 0, sizeof(progress->reward));
    memset(progress->socket, 0, sizeof(progress->socket));
    memset(progress->released, 0, sizeof(progress->released));
    memset(progress->pending, 0, sizeof(progress->pending));
    memset(progress->destroyed, 0, sizeof(progress->destroyed));
    memset(progress->group_members, -1, 64);
}

void GizForces_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    FORCE_PROGRESS *progress = progress_ptr;
    FORCE_PROGRESS_SYSTEM *system = data;
    FORCE_PROGRESS_RECORD *record;
    FORCE_PROGRESS_GROUP *force_group;
    i32 index;
    i32 group;
    i32 slot;
    i32 word;
    u32 bit;
    GizForces_ClearProgress(world_ptr, progress);
    if (progress == NULL) {
        return;
    }
    record = system->records;
    for (index = 0; index < system->count; ++index, ++record) {
        if (index >= 128) {
            break;
        }
        word = index / 32;
        bit = 1u << (index & 31);
        if (!record->visible) {
            progress->visible[word] &= ~bit;
        }
        if (!record->enabled) {
            progress->enabled[word] &= ~bit;
        }
        if (record->reward_released) {
            progress->reward[word] |= bit;
        }
        if (record->socket_hidden) {
            progress->socket[word] |= bit;
        }
        if (record->completion_released) {
            progress->released[word] |= bit;
        }
        if (record->completion_pending) {
            progress->pending[word] |= bit;
        }
        if (record->destroyed) {
            progress->destroyed[word] |= bit;
        }
    }
    for (group = 0; group < 8; ++group) {
        force_group = &system->groups[group];
        for (slot = 0; slot < 8; ++slot) {
            if (slot < force_group->member_count) {
                progress->group_members[group][slot] = (u8)(force_group->members[slot] - system->records);
            } else {
                progress->group_members[group][slot] = 0xff;
            }
        }
    }
}

static i32 GizForces_BoltHitPlat(GIZFORCE *force, WORLDINFO *world, i32 player_index) {
    u32 flags = force->runtime_flags;
    u32 config;
    GAMEANIMOBJ *animation;
    GAMEOBJECT *player;
    NUVEC *position;
    if ((flags & 2) == 0 || (flags & 1) == 0 || (flags & 0x10000) != 0) {
        return 0;
    }
    config = force->config_flags;
    if ((config & 0x100) != 0) {
        force->runtime_flags = flags | 0x8000;
        if (player_index != -1) {
            player = Player[player_index];
            if ((player->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                NewBuzz(player->pad->bound_pad, 0.1f, 0);
            }
        }
        return 1;
    } else if ((config & 0x200) != 0) {
        if (force->blowup_type != -1) {
            if ((config & 0x20) != 0) {
                if (force->anim_set != NULL) {
                    animation = force->anim_set->objects;
                    while (animation != NULL) {
                        position = NuSpecialGetDrawPos(&animation->special);
                        if (position != NULL) {
                            GizmoBlowUpTypeBlowUp(world, force->blowup_type, position);
                        }
                        animation = animation->next;
                    }
                }
            } else {
                GizmoBlowUpTypeBlowUp(world, force->blowup_type, &force->position);
            }
        }
        GameAnimSet_JumpToEnd(force->anim_set);
        ForceUpdateCentre(force, 2);
        GameAnimSet_SetVisibility(force->anim_set, 0);
        force->runtime_flags |= 0x10000;
    } else {
        return 0;
    }
    if (player_index != -1) {
        player = Player[player_index];
        if ((player->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
            NewBuzz(player->pad->bound_pad, 0.1f, 0);
        }
    }
    return 1;
}

i32 GizForces_BoltHitPlatPc(WORLDINFO *world, void *system_pointer, BOLT_PC *bolt, u8 *hit_flags) {
    GIZFORCESYS_s *system = system_pointer;
    GIZFORCE *force;
    GAMEANIMOBJ *animation;
    i32 count;
    i32 index;
    if (system != NULL) {
        count = system->hit_test_gizmo_count;
        for (index = 0; index < count; ++index) {
            force = system->hit_test_gizmos[index]->object;
            if ((force->runtime_flags & 2) == 0 || (force->runtime_flags & 1) == 0 || (force->runtime_flags & 0x100) == 0 ||
                (force->runtime_flags & 0x10000) != 0) {
                continue;
            }
            animation = force->anim_set->objects;
            while (animation != NULL) {
                if (((GIZFORCEANIMDATA_s *)animation->object_data)->platform_id == bolt->hit_platform) {
                    (void)BoltType_FindByID(bolt->type_id, world);
                    GizForces_BoltHitPlat(force, world, bolt->owner != NULL ? bolt->owner->player_index : -1);
                    return 1;
                }
                animation = animation->next;
            }
        }
    }
    return 0;
}

GIZMO_PC *GizForces_GetBestBoltTarget(GIZMOSET *set, f32 *output_distance, NUVEC *output_position, NUVEC *output_velocity,
                                      void *object_pointer, NUVEC *origin, NUVEC *direction, f32 radius, f32 range_squared, i32 directional,
                                      i32 planar, i32 bolt_id) {
    BOLTTYPE *type = BoltType_FindByID(bolt_id, WORLD);
    f32 near_distance = TargetDist_Near2;
    u16 near_angle = TargetDeg_Near;
    f32 mid_distance = TargetDist_Mid2;
    u16 far_angle = TargetDeg_Far;
    u16 mid_angle = TargetDeg_Mid;
    GIZFORCESYS_s *system;
    GAMEOBJECT *object = object_pointer;
    GIZMO_PC *gizmo;
    GIZFORCE *force;
    GIZMO_PC *best;
    GIZMO_PC *previous;
    NUVEC *best_position;
    NUVEC *previous_position;
    f32 best_distance;
    NUVEC direction_copy;
    NUVEC delta;
    f32 minimum_x;
    f32 minimum_z;
    f32 maximum_x;
    f32 maximum_z;
    f32 distance_spill;
    f32 dot_spill;
    f32 dx_spill;
    f32 dz_spill;
    f32 distance;
    f32 dot;
    u16 target_angle;
    NUANG angle;
    i32 index;
    if (set == NULL || set->data == NULL || type == NULL) {
        return NULL;
    }
    best_distance = 1000000000.0f;
    best = NULL;
    previous = NULL;
    best_position = NULL;
    previous_position = NULL;
    direction_copy = *direction;
    minimum_x = (f64)origin->x - (f64)radius;
    minimum_z = (f64)origin->z - (f64)radius;
    maximum_x = (f64)origin->x + (f64)radius;
    maximum_z = (f64)radius + (f64)origin->z;
    if (directional != 0 && (type->flags_60 & 0x20000) != 0) {
        direction_copy.y = 0.0f;
        NuVecNorm(&direction_copy, &direction_copy);
    }
    system = set->data;
    for (index = 0; index < system->hit_test_gizmo_count; ++index) {
        gizmo = system->hit_test_gizmos[index];
        force = gizmo->object;
        if ((force->runtime_flags & 2) == 0 || (force->runtime_flags & 1) == 0 || (force->runtime_flags & 0x10000) != 0) {
            continue;
        }
        if (force->position.x < minimum_x || force->position.x > maximum_x || force->position.z < minimum_z ||
            force->position.z > maximum_z) {
            continue;
        }
        if (planar == 0) {
            distance_spill = NuVecDistSqr(&force->position, origin, &delta);
        } else {
            distance_spill = NuVecXZDistSqr(&force->position, origin, &delta);
        }
        distance = distance_spill;
        if (!(range_squared > distance)) {
            continue;
        }
        if (directional == 0) {
            dz_spill = (f64)force->position.z - (f64)origin->z;
            dx_spill = (f64)force->position.x - (f64)origin->x;
            angle = NuAtan2D(dx_spill, dz_spill);
            NuVecRotateY(&delta, &v001, angle);
        }
        if ((type->flags_60 & 0x20000) != 0) {
            delta.y = 0.0f;
        }
        NuVecNorm(&delta, &delta);
        dot_spill = NuVecDot(&delta, &direction_copy);
        dot = dot_spill;
        if (near_distance > distance && directional != 0) {
            target_angle = near_angle;
        } else if (mid_distance > distance) {
            target_angle = mid_angle;
        } else {
            target_angle = far_angle;
        }
        if (!(NuTrigTable[((target_angle + 0x4000) >> 1) & 0x7fff] < dot)) {
            continue;
        }
        if (!(best_distance > distance)) {
            continue;
        }
        if (object != NULL && (void *)gizmo == (void *)object->collision_target) {
            previous = gizmo;
            previous_position = &force->position;
        } else {
            best_distance = distance;
            best = gizmo;
            best_position = &force->position;
        }
    }
    if (best != NULL) {
        *output_position = *best_position;
        *output_distance = best_distance;
        output_velocity->x = 0.0f;
        output_velocity->y = 0.0f;
        output_velocity->z = 0.0f;
        return best;
    }
    if (previous != NULL) {
        *output_position = *previous_position;
        *output_distance = 1000000000.0f;
        output_velocity->x = 0.0f;
        output_velocity->y = 0.0f;
        output_velocity->z = 0.0f;
        return previous;
    }
    return NULL;
}

i32 GizForces_BoltHit(WORLDINFO *world, void *system_pointer, GAMEOBJECT *object, NUVEC *points, i32 point_count, f32 radius,
                      NUVEC *minimum, NUVEC *maximum, BOLT_PC *bolt, i32 hit_type, u8 *hit_flags) {
    GIZFORCESYS_s *system = system_pointer;
    GIZFORCE *force;
    GIZFORCE *best = NULL;
    f32 best_distance = 1000000000.0f;
    f32 distance_spill;
    f32 distance;
    NUVEC *origin;
    NUVEC normal;
    i32 index;
    i32 point;
    i32 player_index;
    if (system != NULL) {
        for (index = 0; index < system->hit_test_gizmo_count; ++index) {
            force = system->hit_test_gizmos[index]->object;
            if ((force->runtime_flags & 2) == 0 || (force->runtime_flags & 1) == 0 || (force->runtime_flags & 0x10000) != 0) {
                continue;
            }
            if (maximum->x < force->position.x - force->radius || minimum->x > force->radius + force->position.x ||
                maximum->z < force->position.z - force->radius || minimum->z > force->radius + force->position.z ||
                maximum->y < force->position.y - force->radius || minimum->y > force->radius + force->position.y) {
                continue;
            }
            for (point = point_count - 1; point >= 0; --point) {
                if (SphereSphereOverlap(&force->position, force->radius, &points[point], radius) != 0) {
                    origin = object != NULL ? &object->collision_position : &points[point];
                    distance_spill = NuVecDistSqr(origin, &force->position, NULL);
                    distance = distance_spill;
                    if (distance < best_distance) {
                        best_distance = distance;
                        best = force;
                    }
                    break;
                }
            }
        }
    }
    if (best != NULL) {
        if (hit_type != 2 && hit_type != 7 && bolt != NULL) {
            (void)BoltType_FindByID(bolt->type_id, WORLD);
            (void)Player_HasDoubleBoltDamage_FromBolt(bolt);
        }
        (void)BoltType_FindByID(bolt->type_id, world);
        player_index = bolt->owner != NULL ? bolt->owner->player_index : -1;
        if (GizForces_BoltHitPlat(best, world, player_index) != 0) {
            if (object != NULL) {
                NewRumble(object->pad->bound_pad, 0.4f, 0);
                GameCam_HitJudder();
            }
        } else {
            NuVecSub(&normal, &best->position, &bolt->position);
            NuVecNorm(&normal, &normal);
            GameAudio_PlaySfx(0x29, &bolt->position, 0, 0);
            Bolt_AddDeflectedBolt(bolt, &bolt->direction, &normal, hit_flags);
        }
        if (BoltSys->stop_targeting != NULL) {
            BoltSys->stop_targeting(object, points);
        }
        return 1;
    }
    return 0;
}

GIZFORCE_s *GizForces_FindForce(WORLDINFO *world, char *name) {
    GIZMO_PC *gizmo = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, name);
    return gizmo != NULL ? ((GIZFORCE_s *)(gizmo->object)) : NULL;
}

/* Studs the level's force objects release (TotalLevelCoinTally). */
u32 GizForce_TotalScore(WORLDINFO *world) {
    GIZFORCESYS_s *system;
    GIZFORCE *force;
    u32 total = 0;
    i32 i;
    system = world->giz_force_sys;
    if (system != NULL) {
        force = (GIZFORCE *)system->forces;
        if (force != NULL) {
            for (i = 0; i < system->count; i++, force++) {
                total += (u16)force->pickup_count;
            }
        }
    }
    return total;
}

void GizmoForce_Activate(GIZMO_PC *gizmo, i32 active) {
    GIZFORCE *force;
    if (gizmo == NULL) {
        return;
    }
    force = gizmo->object;
    if (active != 0 && !(force->runtime_flags & 1)) {
        GameAnimSet_JumpToStart(force->anim_set);
    }
    force->enabled = active != 0;
    if (active != 0) {
        force->unknown_48 = 0.0f;
        force->unknown_50 = 0.0f;
        force->runtime_flags &= 0xfffeffafu;
        if (force->config_flags & 8) {
            force->runtime_flags &= ~0x200u;
        }
        force->runtime_flags &= 0xffff87ffu;
        GameAnimSet_EvaluateState(force->anim_set);
        force->position = force->file_position;
        force->radius = 1.0f;
        GameAnimSet_GetCentreAndRadius(force->anim_set, &force->position, &force->radius, 2, 1, 1);
    }
}

i32 GizmoForce_ActivateRev(GIZMO_PC *gizmo, i32 reverse, i32 mode) {
    GIZFORCE *force;
    if (gizmo == NULL) {
        return 0;
    }
    force = gizmo->object;
    if (force == NULL) {
        return 0;
    }
    if ((mode & 1) != 0) {
        return reverse != ((force->runtime_flags >> 4) & 1);
    }
    if (reverse != 0) {
        GizForce_PlayBackwards(force);
        force->runtime_flags = (force->runtime_flags & ~1u) | 0x10;
    } else {
        GizForce_PlayForwards(force);
        force->runtime_flags = (force->runtime_flags & ~0x10u) | 1;
    }
    return 1;
}

void GizForces_Reset(void *world_ptr, void *data_ptr, void *progress_ptr) {
    WORLDINFO *world = world_ptr;
    GIZFORCESYS_s *system = data_ptr;
    FORCE_RESET_PROGRESS *progress = progress_ptr;
    GIZFORCE *force;
    i32 index;
    NUVEC offset;
    memset(system->groups, 0, 0x140);
    force = (GIZFORCE *)system->forces;
    for (index = 0; index < system->count; ++index, ++force) {
        ForceResetSingle(world, force);
        if (progress != NULL && index < 128) {
            i32 word = index / 32;
            u32 bit = 1u << (index & 31);
            force->visible = (progress->visible[word] & bit) != 0;
            force->enabled = (progress->enabled[word] & bit) != 0;
            force->reward_released = (progress->reward_released[word] & bit) != 0;
            force->socket_hidden = (progress->socket_hidden[word] & bit) != 0;
            force->completion_released = (progress->completion_released[word] & bit) != 0;
            force->completion_pending = (progress->completion_pending[word] & bit) != 0;
            force->destroyed = (progress->destroyed[word] & bit) != 0;
        }
        if ((i8)force->group_index >= 0 && (i8)force->group_index < 8) {
            force->group = (FORCE_RESET_GROUP *)&system->groups[(i8)force->group_index];
            ++force->group->configured_count;
        } else {
            force->group = NULL;
        }
    }
    if (progress != NULL) {
        i32 group_index;
        i32 member_index;
        for (group_index = 0; group_index < 8; ++group_index) {
            for (member_index = 0; member_index < 8; ++member_index) {
                u8 force_index = progress->group_members[group_index][member_index];
                if (force_index >= system->count) {
                    continue;
                }
                force = (GIZFORCE *)system->forces + force_index;
                if ((force->runtime_flags & 0x20) == 0) {
                    if (force->group != NULL && force->group->member_count < 8) {
                        if (force->group->member_count != 0) {
                            GIZFORCE *previous = force->group->forces[force->group->member_count - 1];
                            force->group->combined_height += GizForce_GetHeight(previous);
                        }
                        force->group->forces[force->group->member_count] = force;
                        ++force->group->member_count;
                        force->runtime_flags |= 0x20;
                    }
                }
                if ((force->runtime_flags & 0x20) != 0) {
                    offset.x = 0.0f;
                    offset.y = GameAnimSet_GetCompletionRatio(force->anim_set) * force->group->combined_height;
                    offset.z = 0.0f;
                    GameAnimSet_SetOffset(force->anim_set, &offset);
                    force->position = force->file_position;
                    force->radius = 1.0f;
                    GameAnimSet_GetCentreAndRadius(force->anim_set, &force->position, &force->radius, 2, 1, 1);
                }
            }
        }
    }
}

void GizForces_Update(void *world_pointer, void *system_pointer, f32 unused_time) {
    WORLDINFO *world = world_pointer;
    GIZFORCESYS_s *system = system_pointer;
    NUVEC offset;
    NUVEC particle_offset;
    NUVEC direction;
    NUVEC position;
    NUVEC centre;
    f32 radius;
    GIZMOSYS *gizmos;
    GIZMOSET *set;
    GIZMO_PC *gizmo;
    i32 index;
    i32 group_index;
    (void)unused_time;
    if (world == NULL || system == NULL) {
        return;
    }
    gizmos = world->gizmo_sys;
    if (gizmos == NULL || gizmos->sets == NULL) {
        return;
    }
    for (group_index = 0; group_index < 8; ++group_index) {
        ((FORCE_RESET_GROUP *)&system->groups[group_index])->runtime_flags &= ~1u;
    }
    system->visible_force_count = 0;
    system->hit_test_gizmo_count = 0;
    set = &gizmos->sets[force_gizmotype_id];
    gizmo = set->gizmos;
    for (index = 0; index < set->count; ++index, ++gizmo) {
        GIZFORCE *force = gizmo->object;
        GAMEOBJECT *user;
        i32 was_moving;
        i32 still_rising;
        i32 playing_forwards;
        i32 updating_animation;
        i32 started_shaking = 0;
        GAMEANIMOBJ *object;
        FORCE_RESET_GROUP *group;
        if (force->using_object != NULL && force->group != NULL && (force->runtime_flags & 0x20) == 0 &&
            (force->group->runtime_flags & 1) != 0 && force->using_object->character_context == CHARACTER_CONTEXT_FORCE) {
            force->using_object->character_context = CHARACTER_CONTEXT_NONE;
            ((GIZFORCE_OBJECT_BEST *)force->using_object)->target = NULL;
            force->using_object = NULL;
        }
        user = force->using_object;
        force->using_object = NULL;
        playing_forwards = 0;
        updating_animation = 0;
        force->being_used = user != NULL;
        was_moving = (force->anim_set->flags >> 1) & 1;
        force->runtime_flags &= ~0x80u;
        if ((force->runtime_flags & 0x1000) != 0 && force->unknown_50 == 0.0f) {
            offset.x = 0.0f;
            if ((force->runtime_flags & 0x20) == 0) {
                offset.y = 0.0f;
            } else {
                offset.y = GameAnimSet_GetCompletionRatio(force->anim_set) * force->group->combined_height;
            }
            offset.z = 0.0f;
            GameAnimSet_SetOffset(force->anim_set, &offset);
            force->runtime_flags &= ~0x1000u;
        }
        if ((force->runtime_flags & 2) == 0 || (force->runtime_flags & 1) == 0 || force->anim_set == NULL ||
            (force->runtime_flags & 0x10) != 0 || (force->runtime_flags & 0x10000) != 0) {
            continue;
        }
        if (NuCameraClipTestSphere(&force->position, force->radius, &NuMtxSetIdentityTemplate) == 0) {
            ((GIZFORCE **)system->visible_forces)[system->visible_force_count++] = force;
            force->runtime_flags |= 0x80;
        }
        still_rising = user == NULL && force->effect_scale > 1.0f && (force->config_flags & 2) == 0 && force->group == NULL &&
                       GameAnimSet_GetCurrentFrame(force->anim_set) > force->effect_scale && !ForceAnimationDone(force);
        force->animation_speed = ForceUpdateSpeed(force, user);
        if (user == NULL && !still_rising && (force->runtime_flags & 0x8000) == 0) {
            force->unknown_50 = 0.0f;
            if (ForceAnimationDone(force)) {
                if ((force->runtime_flags & 0x20) == 0 && ((force->config_flags & 2) != 0 || force->unknown_48 > 0.0f)) {
                    f32 remaining = (f64)force->unknown_48 - FRAMETIME;
                    force->unknown_48 = remaining;
                    if (remaining <= 0.0f) {
                        force->unknown_48 = 0.0f;
                    }
                    if (force->unknown_48 == 0.0f) {
                        if ((force->config_flags & 0x40) != 0 && (force->runtime_flags & 0x800) != 0) {
                            GameAnimSet_SetVisibility(force->anim_set, 1);
                            force->runtime_flags &= ~0x800u;
                        }
                        GizForce_PlayBackwards(force);
                        force->runtime_flags &= ~0x2000u;
                    }
                }
            } else {
                GizForce_PlayBackwards(force);
            }
        } else {
            force->runtime_flags &= ~0x4000u;
            playing_forwards = 1;
            GizForce_PlayForwards(force);
            if ((force->runtime_flags & 0x40) != 0) {
                force->unknown_48 = 0.0f;
                force->unknown_50 = 0.0f;
            } else {
                force->unknown_48 = force->force_strength;
                if (force->force_range > 0.0f && (force->runtime_flags & 0x2000) == 0 && force->anim_set->state == 2) {
                    if (force->unknown_50 == 0.0f) {
                        started_shaking = 1;
                    }
                    force->unknown_50 += FRAMETIME;
                    if (force->unknown_50 >= force->force_range) {
                        force->runtime_flags |= 0x2000;
                        force->unknown_50 = 0.0f;
                    }
                } else {
                    force->unknown_50 = 0.0f;
                }
                if ((force->config_flags & 0x40) != 0 && (force->runtime_flags & 0x800) == 0 && ForceAnimationDone(force) &&
                    (force->force_range == 0.0f || (force->runtime_flags & 0x2000) != 0)) {
                    GameAnimSet_SetVisibility(force->anim_set, 0);
                    force->runtime_flags |= 0x800;
                }
            }
        }
        if (force->unknown_50 > 0.0f) {
            offset.x = (f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.05f - (f64)0.025f;
            offset.y =
                (force->runtime_flags & 0x20) != 0 ? GameAnimSet_GetCompletionRatio(force->anim_set) * force->group->combined_height : 0.0f;
            offset.y = (f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.05f - (f64)0.025f + offset.y;
            offset.z = (f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.05f - (f64)0.025f;
            GameAnimSet_SetOffset(force->anim_set, &offset);
            force->runtime_flags |= 0x1000;
            if (force->start_sfx_id != -1) {
                if (started_shaking || IsSfxLooping(force->start_sfx_id)) {
                    GameAudio_PlaySfxById(force->start_sfx_id, &force->position, 0, 0);
                }
            } else {
                PlaySfx("LegShakeL", &force->position);
            }
        }
        if ((force->anim_set->flags & 1) != 0 || was_moving || (force->anim_set->flags & 4) != 0) {
            updating_animation = 1;
            if (force->group != NULL) {
                if ((force->runtime_flags & 0x20) == 0) {
                    GizForce_AddToStack(force);
                }
                if ((force->runtime_flags & 0x20) != 0) {
                    force->group->runtime_flags |= 1;
                    offset.x = 0.0f;
                    offset.y = GameAnimSet_GetCompletionRatio(force->anim_set) * force->group->combined_height;
                    offset.z = 0.0f;
                    GameAnimSet_SetOffset(force->anim_set, &offset);
                }
            }
            ForceUpdateCentre(force, 2);
            if (was_moving) {
                if (ForceAnimationDone(force)) {
                    if (force->loop_sfx_id != -1) {
                        GameAudio_PlaySfxById(force->loop_sfx_id, &force->position, 0, 0);
                    }
                } else if (force->anim_set->state != 4 && force->stop_sfx_id != -1) {
                    GameAudio_PlaySfxById(force->stop_sfx_id, &force->position, 0, 0);
                }
            } else if (((force->anim_set->flags & 1) != 0 || (force->anim_set->flags & 4) != 0) && force->unknown_50 == 0.0f &&
                       playing_forwards) {
                if (force->start_sfx_id != -1) {
                    if (force->anim_set->state == 0 || IsSfxLooping(force->start_sfx_id)) {
                        GameAudio_PlaySfxById(force->start_sfx_id, &force->position, 0, 0);
                    }
                } else {
                    PlaySfx("LegoForm", &force->position);
                }
            }
            if ((force->config_flags & 0x2000) == 0) {
                for (object = force->anim_set->objects; object != NULL; object = object->next) {
                    GIZFORCEANIMDATA_s *data = object->object_data;
                    if ((data->flags & 2) == 0 && data->platform_id != -1) {
                        AddShoveObject(&object->special, data->platform_id);
                    }
                }
            }
        }
        if (force->anim_set->state == 2) {
            if ((force->runtime_flags & 0x20) != 0) {
                if (GizForce_Complete(force) && user == NULL) {
                    force->runtime_flags |= 0x40;
                }
            } else if (!(force->force_range > 0.0f && (force->runtime_flags & 0x2000) == 0) &&
                       !((force->config_flags & 0x40) != 0 && (force->runtime_flags & 0x800) == 0) &&
                       (force->anim_set->animated_object_count != 0 || force->force_range > 0.0f || (force->config_flags & 0x40) != 0) &&
                       (force->runtime_flags & 0x4000) == 0) {
                if (force->blowup_type != -1) {
                    if ((force->config_flags & 0x20) != 0) {
                        if (force->anim_set != NULL) {
                            for (object = force->anim_set->objects; object != NULL; object = object->next) {
                                NUVEC *position = NuSpecialGetDrawPos(&object->special);
                                if (position != NULL) {
                                    GizmoBlowUpTypeBlowUp(world, force->blowup_type, position);
                                }
                            }
                        }
                    } else {
                        GizmoBlowUpTypeBlowUp(world, force->blowup_type, &force->position);
                    }
                    GameAnimSet_SetVisibility(force->anim_set, 0);
                    force->runtime_flags |= 0x10000;
                }
                if (force->pickup_count != 0 && (force->runtime_flags & 0x200) == 0) {
                    i32 hearts = ReleaseHearts();
                    NuVecAdd(&position, &force->position, &force->effect_position);
                    NuVecRotateX(&direction, &v010, (u16)force->pickup_rotation_x);
                    NuVecRotateY(&direction, &direction, (u16)force->pickup_rotation_y);
                    AddPickups((u16)force->pickup_count, hearts, 0, 0, &position, &direction, 2.0f, -1, force->activation_radius,
                               LevelConfigFarDistance, NULL, 1, 0);
                    force->runtime_flags |= 0x200;
                }
                force->runtime_flags |= 0x4000;
            }
        } else if (!updating_animation && force->anim_set->state == 0 && (force->runtime_flags & 0x20) != 0) {
            ForceGroupPop(force);
            force->runtime_flags &= ~0x40u;
        }
        group = force->group;
        if (group != NULL) {
            if (force->anim_set->state == 2) {
                if (was_moving && group->configured_count == group->member_count) {
                    group->runtime_flags |= 2;
                    force->group->runtime_flags |= 4;
                    group = force->group;
                    for (group_index = 0; group_index < force->group->configured_count; ++group_index) {
                        const i8 mask = (i8)group->forces[group_index]->collision_mask;
                        if (mask != 0 && (mask & (1 << group_index)) == 0) {
                            group->runtime_flags &= ~4u;
                            break;
                        }
                    }
                }
            } else {
                group->runtime_flags &= ~2u;
                force->group->runtime_flags &= ~4u;
            }
        }
        if ((force->runtime_flags & 0x8000) != 0 && GizForce_Complete(force)) {
            force->runtime_flags &= ~0x8000u;
        }
        if (!force->destroyed && (force->config_flags & 0x300) != 0) {
            system->hit_test_gizmos[system->hit_test_gizmo_count++] = gizmo;
        }
        if ((i8)force->runtime_flags < 0 && !GizForce_Complete(force)) {
            if ((force->config_flags & 0x400) != 0) {
                for (object = force->anim_set->objects; object != NULL; object = object->next) {
                    GIZFORCEANIMDATA_s *data = object->object_data;
                    if ((data->unknown_04 & 1) != 0 || !NuSpecialExistsFn(&object->special)) {
                        continue;
                    }
                    radius = 0.0f;
                    NuSpecialGetRadius(&object->special, &centre, &radius);
                    radius = (f64)force->horizontal_range * (f64)1.2f * radius;
                    NuVecMtxTransformVU0(&centre, &centre, NuSpecialGetDrawMtx(&object->special));
                    particle_offset.x = 0.0f;
                    if (radius == 0.0f) {
                        particle_offset.y = 0.0f;
                    } else {
                        particle_offset.y = (f64)qrand() / (65535.0 / ((f64)radius * 0.5) + 1.0);
                    }
                    particle_offset.z = 0.0f;
                    NuVecRotateZ(&particle_offset, &particle_offset, qrand());
                    NuVecRotateY(&particle_offset, &particle_offset, (u16)qrand());
                    NuVecAdd(&centre, &centre, &particle_offset);
                    if ((force->config_flags & 0x10) != 0) {
                        if ((force->runtime_flags & 0x20000) == 0) {
                            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[110].effect, &centre,
                                                              (i32)((f64)radius * 90.0), FRAMETIME, 0, 0, NULL);
                        }
                    } else if ((force->runtime_flags & 0x20000) == 0) {
                        AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[109].effect, &centre, (i32)((f64)radius * 90.0),
                                                          FRAMETIME, 0, 0, NULL);
                    }
                }
            } else if ((force->runtime_flags & 0x20000) == 0) {
                radius = 0.0f;
                GameAnimSet_GetCentreAndRadius(force->anim_set, &centre, &radius, 2, 1, 1);
                radius = (f64)force->horizontal_range * (f64)1.2f * radius;
                particle_offset.x = 0.0f;
                if (radius == 0.0f) {
                    particle_offset.y = 0.0f;
                } else {
                    particle_offset.y = (f64)qrand() / (65535.0 / ((f64)radius * 0.5) + 1.0);
                }
                particle_offset.z = 0.0f;
                NuVecRotateZ(&particle_offset, &particle_offset, qrand());
                NuVecRotateY(&particle_offset, &particle_offset, (u16)qrand());
                NuVecAdd(&centre, &centre, &particle_offset);
                if ((force->config_flags & 0x10) != 0) {
                    AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[110].effect, &centre, (i32)((f64)radius * 90.0),
                                                      FRAMETIME, 0, 0, NULL);
                } else {
                    AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[109].effect, &centre, (i32)((f64)radius * 90.0),
                                                      FRAMETIME, 0, 0, NULL);
                }
            }
        }
        force->runtime_flags &= ~0x20000u;
        if ((force->config_flags & 0x400) != 0) {
            for (object = force->anim_set->objects; object != NULL; object = object->next) {
                ((GIZFORCEANIMDATA_s *)object->object_data)->unknown_04 &= ~1u;
            }
        }
    }
}

i32 GizForce_FindBestForceTarget(void *system_ptr, GAMEOBJECT *object) {
    GIZFORCE_SYS_BEST *system = system_ptr;
    GIZFORCE_OBJECT_BEST *target = (GIZFORCE_OBJECT_BEST *)object;
    GIZFORCE_LOS_BEST *los;
    GIZFORCE_TARGET_RECORD *oldest = NULL;
    GIZFORCE_TARGET_RECORD *best = NULL;
    GIZFORCE_TARGET_RECORD *record;
    f32 best_distance = 1000000000.0f;
    f32 oldest_time = 1000000000.0f;
    f32 distance;
    i32 super_force;
    i32 count = 0;
    i32 i;
    NUVEC delta;
    NUVEC direction;
    NUVEC facing;
    if (system == NULL || object == NULL) {
        return 0;
    }
    super_force = SuperWeirdo(object) || ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) && Cheat_IsOn(25));
    target->target = NULL;
    target->animation = NULL;
    facing.x = NuTrigTable[object->movement_facing_angle >> 1];
    facing.z = NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff];
    for (i = 0; i < system->visible_count; ++i) {
        GIZFORCE *force = system->visible_forces[i];
        GIZFORCE_BEST *fields = (GIZFORCE_BEST *)force;
        GAMEANIMOBJ *animation;
        NUVEC *draw_position;
        GIZFORCE_GROUP_BEST *group;
        if (fields->using_object != NULL || fields->unavailable_flags != 0) {
            continue;
        }
        if ((fields->config_flags & 0x10) && !(object->character_data->model_flags & CHARACTER_MODEL_BADDIE) && !super_force) {
            continue;
        }
        group = fields->group;
        if (group != NULL) {
            if (group->count != 0 && force != group->forces[group->count - 1]) {
                if (group->flags & 1) {
                    continue;
                }
                if (fields->anim_set->state == 2) {
                    continue;
                }
                if (group->forces[group->count - 1] != NULL && ((((GIZFORCE_BEST *)group->forces[group->count - 1])->anim_set->flags & 1) ||
                                                                (((GIZFORCE_BEST *)group->forces[group->count - 1])->anim_set->flags & 2) ||
                                                                (((GIZFORCE_BEST *)group->forces[group->count - 1])->anim_set->flags & 4) ||
                                                                ((GIZFORCE_BEST *)group->forces[group->count - 1])->using_object != NULL)) {
                    continue;
                }
            }
        } else if (GizForce_Complete(force)) {
            continue;
        }
        if (GizForce_StoodOnForce(force, object)) {
            continue;
        }
        distance = NuVecDistSqr(&fields->position, &object->collision_position, &delta);
        if (distance > fields->interaction_radius * fields->interaction_radius) {
            continue;
        }
        if (fields->config_flags & 0x400) {
            for (animation = fields->anim_set->objects; animation != NULL; animation = animation->next) {
                if (animation->flags & 1) {
                    continue;
                }
                draw_position = NuSpecialGetDrawPos(&animation->special);
                distance = NuVecDistSqr(draw_position, &object->collision_position, &delta);
                if (delta.x * facing.x + delta.z * facing.z < 0.0f) {
                    continue;
                }
                PossibleForceTargets[count].force = force;
                PossibleForceTargets[count].animation = animation;
                PossibleForceTargets[count].distance = distance;
                PossibleForceTargets[count].index = animation - system->anim_pool->objects;
                ++count;
            }
        } else {
            if (delta.x * facing.x + delta.z * facing.z < 0.0f) {
                continue;
            }
            PossibleForceTargets[count].force = force;
            PossibleForceTargets[count].animation = NULL;
            PossibleForceTargets[count].distance = distance;
            PossibleForceTargets[count].index =
                fields->anim_set->objects != NULL ? fields->anim_set->objects - system->anim_pool->objects : -1;
            ++count;
        }
    }
    los = object->gizforce_los_info;
    if (los != NULL) {
        record = PossibleForceTargets;
        for (i = 0; i < count; ++i, ++record) {
            f32 time = (f32)los->queried_at[record->index];
            if (time < oldest_time) {
                oldest_time = time;
                oldest = record;
            }
        }
        if (oldest != NULL) {
            los->queried_at[oldest->index] = LevelTimer.update_count;
            if (((GIZFORCE_BEST *)oldest->force)->config_flags & 0x800) {
                ((GIZFORCE_LOS_BEST *)object->gizforce_los_info)->clear_bits[oldest->index >> 5] &= ~(1u << (oldest->index & 31));
                if (oldest->animation != NULL) {
                    NUVEC *draw_position = NuSpecialGetDrawPos(&oldest->animation->special);
                    NuVecSub(&direction, draw_position, &object->collision_position);
                    if (NewRayCastScaleYMask(&object->collision_position, &direction, 0.0f, 1.0f, 0, 0) != 0) {
                        if (TerrainPlatId() == -1) {
                            goto choose;
                        }
                        if (((GIZFORCE_PLATFORM_BEST *)oldest->animation->object_data)->platform_id != TerrainPlatId()) {
                            goto choose;
                        }
                    }
                } else {
                    GAMEANIMOBJ *animation;
                    NuVecSub(&direction, &((GIZFORCE_BEST *)oldest->force)->position, &object->collision_position);
                    if (NewRayCastScaleYMask(&object->collision_position, &direction, 0.0f, 1.0f, 0, 0) != 0) {
                        if (!(((GIZFORCE_BEST *)oldest->force)->runtime_flags & 0x100)) {
                            goto choose;
                        }
                        if (TerrainPlatId() == -1) {
                            goto choose;
                        }
                        for (animation = ((GIZFORCE_BEST *)oldest->force)->anim_set->objects;; animation = animation->next) {
                            if (animation == NULL) {
                                goto choose;
                            }
                            if (((GIZFORCE_PLATFORM_BEST *)animation->object_data)->platform_id == TerrainPlatId()) {
                                break;
                            }
                        }
                    }
                }
            }
            ((GIZFORCE_LOS_BEST *)object->gizforce_los_info)->clear_bits[oldest->index >> 5] |= 1u << (oldest->index & 31);
        }
    }
choose:
    los = object->gizforce_los_info;
    record = PossibleForceTargets;
    for (i = 0; i < count; ++i, ++record) {
        if (los != NULL && record->index != -1 && !(los->clear_bits[record->index >> 5] & (1u << (record->index & 31)))) {
            continue;
        }
        if (record->distance < best_distance) {
            best_distance = record->distance;
            best = record;
        }
    }
    if (best != NULL) {
        target->target = best->force;
        target->animation = best->animation;
    }
    return 0;
}

char *GizmoRegistryGizForceNamePtrPc = "GizForce";
static ADDGIZMOTYPE GizmoRegistryGizForceAddTypePc = {0};

ADDGIZMOTYPE *GizForce_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE *addtype = &GizmoRegistryGizForceAddTypePc;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->name = GizmoRegistryGizForceNamePtrPc;
    addtype->fns.early_update_fn = NULL;
    addtype->fns.panel_draw_fn = NULL;
    addtype->fns.get_visibility_fn = NULL;
    addtype->fns.using_special_fn = NULL;
    addtype->prefix = "";
    addtype->fns.options = 0xb0;
    addtype->fns.get_max_gizmos_fn = (i32 (*)(void *))GizForces_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)GizForces_AddGizmos;
    addtype->fns.late_update_fn = (void (*)(void *, void *, f32))GizForces_Update;
    addtype->fns.draw_fn = (void (*)(void *, void *, f32))GizForces_Draw;
    addtype->fns.get_gizmo_name_fn = (char *(*)(GIZMO_PC *))GizmoForce_GetGizmoName;
    addtype->fns.get_output_fn = (i32 (*)(GIZMO_PC *, i32, i32))GizmoForce_GetOutput;
    addtype->fns.get_output_name_fn = (char *(*)(GIZMO_PC *, i32))GizmoForce_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)GizmoForce_GetNumOutputs;
    addtype->fns.activate_fn = (void (*)(GIZMO_PC *, i32))GizmoForce_Activate;
    addtype->fns.activate_rev_fn = (i32 (*)(GIZMO_PC *, i32, i32))GizmoForce_ActivateRev;
    addtype->fns.set_visibility_fn = (void (*)(GIZMO_PC *, i32))GizmoForce_SetVisibility;
    addtype->fns.get_pos_fn = (void *)GizmoForce_GetPos;
    addtype->fns.bolt_hit_plat_fn = (void *)GizForces_BoltHitPlatPc;
    addtype->fns.get_best_bolt_target_fn =
        (GIZMO_PC * (*)(GIZMOSET *, f32 *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, f32, f32, i32, i32, i32))
            GizForces_GetBestBoltTarget;
    addtype->fns.bolt_hit_fn = (void *)GizForces_BoltHit;
    addtype->fns.allocate_progress_data_fn = (void *(*)(VARIPTR *, VARIPTR *))GizForces_AllocateProgressData;
    addtype->fns.clear_progress_fn = (void (*)(void *, void *))GizForces_ClearProgress;
    addtype->fns.store_progress_fn = (void (*)(void *, void *, void *))GizForces_StoreProgress;
    addtype->fns.reset_fn = (void (*)(void *, void *, void *))GizForces_Reset;
    addtype->fns.reserve_buffer_space_fn = (void *(*)(void *))GizForces_ReserveBufferSpace;
    addtype->fns.load_fn = (i32 (*)(void *, void *))GizForces_Load;
    addtype->fns.post_load_fn = (void (*)(void *, void *))GizForces_PostLoad;
    addtype->fns.add_level_sfx_fn = (void *)GizForces_AddLevelSfx;
    force_gizmotype_id = type_id;
    return addtype;
}

/* Gizmo set registered by LoadPermData when StartupGameFlag0 is set. */
void RegisterDefaultGizmoTypes(VARIPTR *buffer, VARIPTR *end) {
    REGISTER_GIZMO_CONFIG_TYPE_FN functions[24];
    functions[0] = GizObstacles_RegisterGizmo;
    functions[1] = GizBuildIts_RegisterGizmo;
    functions[2] = NewBlowup_RegisterGizmo;
    functions[3] = GizmoPickups_RegisterGizmo;
    functions[4] = Grapples_RegisterGizmo;
    functions[5] = TightRopes_RegisterGizmo;
    functions[6] = Levers_RegisterGizmo;
    functions[7] = Spinner_RegisterGizmo;
    functions[8] = Technos_RegisterGizmo;
    functions[9] = MiniCut_RegisterGizmo;
    functions[10] = Tubes_RegisterGizmo;
    functions[11] = ZipUps_RegisterGizmo;
    functions[12] = GizTurrets_RegisterGizmo;
    functions[13] = AI_RegisterGizmo;
    functions[14] = GizTimer_RegisterGizmo;
    functions[15] = GizRandom_RegisterGizmo;
    functions[16] = GizSpecial_RegisterGizmo;
    functions[17] = Door_RegisterGizmo;
    functions[18] = Plugs_RegisterGizmo;
    functions[19] = GizAIMessage_RegisterGizmo;
    functions[20] = Push_RegisterGizmo;
    functions[21] = EdGizShadowMachine_RegisterGizmo;
    functions[22] = Portal_RegisterGizmo;
    functions[23] = NULL;
    RegisterGizmoTypes(buffer, end, functions, 12);
}

void RegisterAlternateGizmoTypes(VARIPTR *buffer, VARIPTR *end) {
    REGISTER_GIZMO_CONFIG_TYPE_FN functions[29];
    functions[0] = GizObstacles_RegisterGizmo;
    functions[1] = GizBuildIts_RegisterGizmo;
    functions[2] = NewBlowup_RegisterGizmo;
    functions[3] = GizmoPickups_RegisterGizmo;
    functions[4] = Shards_RegisterGizmo;
    functions[5] = Signals_RegisterGizmo;
    functions[6] = Grapples_RegisterGizmo;
    functions[7] = TightRopes_RegisterGizmo;
    functions[8] = Ledges_RegisterGizmo;
    functions[9] = Levers_RegisterGizmo;
    functions[10] = Spinner_RegisterGizmo;
    functions[11] = Technos_RegisterGizmo;
    functions[12] = SecurityDoors_RegisterGizmo;
    functions[13] = Attractos_RegisterGizmo;
    functions[14] = MiniCut_RegisterGizmo;
    functions[15] = GuideLines_RegisterGizmo;
    functions[16] = Tubes_RegisterGizmo;
    functions[17] = ZipUps_RegisterGizmo;
    functions[18] = GizTurrets_RegisterGizmo;
    functions[19] = AI_RegisterGizmo;
    functions[20] = GizTimer_RegisterGizmo;
    functions[21] = GizRandom_RegisterGizmo;
    functions[22] = GizSpecial_RegisterGizmo;
    functions[23] = Door_RegisterGizmo;
    functions[24] = GizAIMessage_RegisterGizmo;
    functions[25] = Push_RegisterGizmo;
    functions[26] = EdGizShadowMachine_RegisterGizmo;
    functions[27] = Portal_RegisterGizmo;
    functions[28] = NULL;
    RegisterGizmoTypes(buffer, end, functions, 12);
}

void RegisterLswGizmoTypes(VARIPTR *buffer, VARIPTR *end) {
    REGISTER_GIZMO_CONFIG_TYPE_FN functions[29];
    functions[0] = GizObstacles_RegisterGizmo;
    functions[1] = GizBuildIts_RegisterGizmo;
    functions[2] = GizForce_RegisterGizmo;
    functions[3] = NewBlowup_RegisterGizmo;
    functions[4] = GizmoPickups_RegisterGizmo;
    functions[5] = Levers_RegisterGizmo;
    functions[6] = Spinner_RegisterGizmo;
    functions[7] = MiniCut_RegisterGizmo;
    functions[8] = Tubes_RegisterGizmo;
    functions[9] = ZipUps_RegisterGizmo;
    functions[10] = GizTurrets_RegisterGizmo;
    functions[11] = GizBombGen_RegisterGizmo;
    functions[12] = AI_RegisterGizmo;
    functions[13] = GizSpecial_RegisterGizmo;
    functions[14] = GizAIMessage_RegisterGizmo;
    functions[15] = GizTimer_RegisterGizmo;
    functions[16] = GizRandom_RegisterGizmo;
    functions[17] = GizPanel_RegisterGizmo;
    functions[18] = HatMachine_RegisterGizmo;
    functions[19] = Push_RegisterGizmo;
    functions[20] = Door_RegisterGizmo;
    functions[21] = Teleport_RegisterGizmo;
    functions[22] = GizTorpMachine_RegisterGizmo;
    functions[23] = EdGizShadowMachine_RegisterGizmo;
    functions[24] = Portal_RegisterGizmo;
    functions[25] = Grapples_RegisterGizmo;
    functions[26] = Plugs_RegisterGizmo;
    functions[27] = Technos_RegisterGizmo;
    functions[28] = NULL;
    RegisterGizmoTypes(buffer, end, functions, 12);
}
