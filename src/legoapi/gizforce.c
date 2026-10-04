#include "saga.h"

void __cdecl ForceLightning_Origin(GAMEOBJECT *object, NUVEC *primary, NUVEC *secondary) {
    PLAYERCHARACTERCONFIG_PC *config;
    i32 joint;
    *primary = object->collision_position;
    if (secondary != NULL)
        secondary->y = 1000000000.0f;
    if (object->origin_joint_ready == 0)
        return;
    config = object->character_data->game_character;
    joint = config->hand_locators[0];
    if (joint == -1 || object->character_model->points_of_interest[joint] == NULL)
        return;
    primary->x = object->locator_matrices[joint].m30;
    primary->y = object->locator_matrices[joint].m31;
    primary->z = object->locator_matrices[joint].m32;
    if (secondary == NULL || (object->weapon_scale != 0.0f && object->field_e33 != 2))
        return;
    joint = config->hand_locators[1];
    if (joint != -1 && object->character_model->points_of_interest[joint] != NULL) {
        secondary->x = object->locator_matrices[joint].m30;
        secondary->y = object->locator_matrices[joint].m31;
        secondary->z = object->locator_matrices[joint].m32;
    }
}

typedef struct force_glow_reader_pc_s {
    u8 unknown_0000[0xd14];
    GAMEOBJECT *force_glow_object;
    GAMEOBJECT *force_glow_candidate;
} FORCE_GLOW_READER_PC;
STATIC_ASSERT(offsetof(FORCE_GLOW_READER_PC, force_glow_object) == 0xd14);
STATIC_ASSERT(offsetof(FORCE_GLOW_READER_PC, force_glow_candidate) == 0xd18);
STATIC_ASSERT(offsetof(PART_PC, field_206) == 0x206);
STATIC_ASSERT(offsetof(PART_PC, gravity) == 0xe8);
void __cdecl EndForce(GAMEOBJECT *object, i32 mode) {
    if (object != NULL && !(object->flags_1f8 & 0x40000)) {
        FORCE_GLOW_READER_PC *glow = (FORCE_GLOW_READER_PC *)object;
        object->flags_e20 &= ~0x20000;
        glow->force_glow_object = NULL;
        glow->force_glow_candidate = NULL;
        object->character_context = -1;
    }
    GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
}
void __cdecl ReleaseForce(GAMEOBJECT *object, i32 mode) {
    if (object->character_context == 0x1b) {
        if (object->force_target != NULL) {
            if (object->force_target->character_context == 0x1c) {
                object->force_target->character_context = -1;
                object->force_target->force_target = NULL;
            }
            object->force_target = NULL;
        }
    } else if (object->character_context == 0x1d) {
        if (object->force_part != NULL) {
            u8 mask = (u8)(1u << (object->field_280 & 31));
            object->force_part->field_206 = (i8)((u8)object->force_part->field_206 & ~mask);
            if (object->force_part->field_206 == 0)
                object->force_part->field_206 = -1;
            object->force_part->gravity = GameDeflectPartGravity;
            object->force_part = NULL;
        }
        object->character_context = -1;
    } else if (object->character_context == 8) {
        if (object->flags_1fc_word & 0x80)
            GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
    } else
        return;
    EndForce(object, mode);
}

typedef struct force_glow_packet_pc_s {
    u8 unknown000[0x650];
    void *object, *candidate;
    u8 unknown658[0x6bc - 0x658];
    f32 alpha, target, recovery, unknown6c8, hold;
} FORCE_GLOW_PACKET_PC;
STATIC_ASSERT(offsetof(FORCE_GLOW_PACKET_PC, object) == 0x650);
STATIC_ASSERT(offsetof(FORCE_GLOW_PACKET_PC, candidate) == 0x654);
STATIC_ASSERT(offsetof(FORCE_GLOW_PACKET_PC, alpha) == 0x6bc);
STATIC_ASSERT(offsetof(FORCE_GLOW_PACKET_PC, target) == 0x6c0);
STATIC_ASSERT(offsetof(FORCE_GLOW_PACKET_PC, recovery) == 0x6c4);
STATIC_ASSERT(offsetof(FORCE_GLOW_PACKET_PC, hold) == 0x6cc);
void __cdecl ResetForceGlow(void *packet) {
    FORCE_GLOW_PACKET_PC *view = (FORCE_GLOW_PACKET_PC *)packet;
    view->alpha = 0.0f;
    view->target = 0.0f;
    view->object = NULL;
    view->hold = 0.0f;
    view->candidate = NULL;
    view->recovery = 0.2f;
}

struct gizforce_pc_s {
    u8 unknown_000[0x1c];
    NUVEC position;
    GAMEANIMSET *anim_set;
    u8 unknown_02c[0x20];
    f32 force_range;
    u8 unknown_050[0x24];
    u32 config_flags;
    u8 unknown_078[0x28];
    u32 runtime_flags;
};
STATIC_ASSERT(offsetof(GIZFORCE_PC, position) == 0x1c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, anim_set) == 0x28);
STATIC_ASSERT(offsetof(GIZFORCE_PC, force_range) == 0x4c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, config_flags) == 0x74);
STATIC_ASSERT(offsetof(GIZFORCE_PC, runtime_flags) == 0xa0);
typedef struct GIZFORCE_ANIMATION_DATA_PC {
    u8 unknown_00[2];
    i16 platform_id;
} GIZFORCE_ANIMATION_DATA_PC;
STATIC_ASSERT(offsetof(GIZFORCE_ANIMATION_DATA_PC, platform_id) == 2);
i32 __cdecl GizForce_StoodOnForce(GIZFORCE_PC *force, GAMEOBJECT *object) {
    GAMEANIMOBJ *animation;
    if (!(force->runtime_flags & 0x100) || object->terrain_platform_id == -1)
        return 0;
    for (animation = force->anim_set->objects; animation != NULL; animation = animation->next)
        if (((GIZFORCE_ANIMATION_DATA_PC *)animation->object_data)->platform_id == object->terrain_platform_id)
            return 1;
    return 0;
}
u16 __cdecl GizForces_AngleToForce(NUVEC *position, GIZFORCE_PC *force) {
    f32 dz = force->position.z - position->z;
    f32 dx = force->position.x - position->x;
    return NuAtan2D(dx, dz);
}
i32 __cdecl GizForce_Complete(GIZFORCE_PC *force) {
    if ((force->config_flags & 2) || (force->force_range > 0.0f && !(force->runtime_flags & 0x2000)))
        return 0;
    if ((force->config_flags & 0x40) && !(force->runtime_flags & 0x800))
        return 0;
    if (force->anim_set == NULL)
        return 1;
    if (force->runtime_flags & 0x40)
        return force->anim_set->state == 0;
    return force->anim_set->state == 2;
}

typedef struct GIZFORCE_GROUP_BEST_PC {
    GIZFORCE_PC *forces[9];
    u8 flags;
    u8 unknown_25;
    u8 count;
} GIZFORCE_GROUP_BEST_PC;
STATIC_ASSERT(offsetof(GIZFORCE_GROUP_BEST_PC, flags) == 0x24);
STATIC_ASSERT(offsetof(GIZFORCE_GROUP_BEST_PC, count) == 0x26);
typedef struct GIZFORCE_BEST_PC {
    u8 unknown_000[0x1c];
    NUVEC position;
    GAMEANIMSET *anim_set;
    u8 unknown_02c[0xc];
    GAMEOBJECT *using_object;
    u32 unavailable_flags;
    GIZFORCE_GROUP_BEST_PC *group;
    u8 unknown_044[0x10];
    f32 interaction_radius;
    u8 unknown_058[0x1c];
    u32 config_flags;
    u8 unknown_078[0x28];
    u32 runtime_flags;
} GIZFORCE_BEST_PC;
STATIC_ASSERT(offsetof(GIZFORCE_BEST_PC, position) == 0x1c);
STATIC_ASSERT(offsetof(GIZFORCE_BEST_PC, anim_set) == 0x28);
STATIC_ASSERT(offsetof(GIZFORCE_BEST_PC, using_object) == 0x38);
STATIC_ASSERT(offsetof(GIZFORCE_BEST_PC, unavailable_flags) == 0x3c);
STATIC_ASSERT(offsetof(GIZFORCE_BEST_PC, group) == 0x40);
STATIC_ASSERT(offsetof(GIZFORCE_BEST_PC, interaction_radius) == 0x54);
STATIC_ASSERT(offsetof(GIZFORCE_BEST_PC, config_flags) == 0x74);
STATIC_ASSERT(offsetof(GIZFORCE_BEST_PC, runtime_flags) == 0xa0);
typedef struct GIZFORCE_SYS_BEST_PC {
    u8 unknown_000[4];
    GIZFORCE_PC **visible_forces;
    u8 unknown_008[8];
    u16 visible_count;
    u8 unknown_012[2];
    GAMEANIMOBJPOOL *anim_pool;
} GIZFORCE_SYS_BEST_PC;
STATIC_ASSERT(offsetof(GIZFORCE_SYS_BEST_PC, visible_forces) == 4);
STATIC_ASSERT(offsetof(GIZFORCE_SYS_BEST_PC, visible_count) == 0x10);
STATIC_ASSERT(offsetof(GIZFORCE_SYS_BEST_PC, anim_pool) == 0x14);

typedef struct GIZFORCE_LOS_BEST_PC {
    u32 clear_bits[12];
    u32 queried_at[1];
} GIZFORCE_LOS_BEST_PC;
STATIC_ASSERT(offsetof(GIZFORCE_LOS_BEST_PC, queried_at) == 0x30);
typedef struct GIZFORCE_OBJECT_BEST_PC {
    u8 unknown_000[0x10cc];
    GIZFORCE_PC *target;
    GAMEANIMOBJ *animation;
} GIZFORCE_OBJECT_BEST_PC;
STATIC_ASSERT(offsetof(GIZFORCE_OBJECT_BEST_PC, target) == 0x10cc);
STATIC_ASSERT(offsetof(GIZFORCE_OBJECT_BEST_PC, animation) == 0x10d0);
typedef struct GIZFORCE_PLATFORM_BEST_PC {
    u8 unknown_000[2];
    i16 platform_id;
} GIZFORCE_PLATFORM_BEST_PC;
STATIC_ASSERT(offsetof(GIZFORCE_PLATFORM_BEST_PC, platform_id) == 2);
extern GIZFORCE_TARGET_RECORD_PC PossibleForceTargetsPc[];
extern TIMER LevelTimer;
i32 __cdecl SuperWeirdo(GAMEOBJECT *);
i32 __cdecl TerrainPlatId(void);
i32 __cdecl NewRayCastScaleYMask(NUVEC *, NUVEC *, f32, f32, i32, u32);
i32 __cdecl GizForce_FindBestForceTarget(void *system_ptr, GAMEOBJECT *object) {
    GIZFORCE_SYS_BEST_PC *system = (GIZFORCE_SYS_BEST_PC *)system_ptr;
    GIZFORCE_OBJECT_BEST_PC *target = (GIZFORCE_OBJECT_BEST_PC *)object;
    GIZFORCE_LOS_BEST_PC *los;
    GIZFORCE_TARGET_RECORD_PC *oldest = NULL, *best = NULL, *record;
    f32 oldest_time = 1000000000.0f, best_distance = 1000000000.0f, facing_x, facing_z, distance;
    i32 super_force, count = 0, i, index, clear;
    NUVEC delta, direction;
    if (system == NULL || object == NULL)
        return 0;
    super_force = SuperWeirdo(object) || ((object->flags_1fc & 0x80) && Cheat_IsOn(25));
    target->target = NULL;
    target->animation = NULL;
    facing_x = NuTrigTable[object->movement_facing_angle >> 1];
    facing_z = NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff];
    for (i = 0; i < system->visible_count; ++i) {
        GIZFORCE_PC *force = system->visible_forces[i];
        GIZFORCE_BEST_PC *fields = (GIZFORCE_BEST_PC *)force;
        GAMEANIMOBJ *animation;
        GIZFORCE_GROUP_BEST_PC *group = fields->group;
        if (fields->using_object != NULL || fields->unavailable_flags != 0)
            continue;
        if ((fields->config_flags & 0x10) && !(object->character_data->model_flags & 4) && !super_force)
            continue;
        if (group == NULL) {
            if (GizForce_Complete(force))
                continue;
        } else if (group->count != 0) {
            GIZFORCE_PC *last = group->forces[group->count - 1];
            GIZFORCE_BEST_PC *last_fields = (GIZFORCE_BEST_PC *)last;
            if (last != force) {
                if ((group->flags & 1) || fields->anim_set->state == 2)
                    continue;
                if (last != NULL && ((last_fields->anim_set->flags & 7) || last_fields->using_object != NULL))
                    continue;
            }
        }
        if (GizForce_StoodOnForce(force, object))
            continue;
        distance = NuVecDistSqr(&fields->position, &object->collision_position, &delta);
        if (distance > fields->interaction_radius * fields->interaction_radius)
            continue;
        if (fields->config_flags & 0x400) {
            for (animation = fields->anim_set->objects; animation != NULL; animation = animation->next) {
                if (animation->flags & 1)
                    continue;
                distance = NuVecDistSqr(NuSpecialGetDrawPos(&animation->special), &object->collision_position, &delta);
                if (delta.x * facing_x + delta.z * facing_z < 0.0f)
                    continue;
                record = &PossibleForceTargetsPc[count++];
                record->force = force;
                record->animation = animation;
                record->distance = distance;
                record->index = animation - system->anim_pool->objects;
            }
        } else {
            if (delta.x * facing_x + delta.z * facing_z < 0.0f)
                continue;
            record = &PossibleForceTargetsPc[count++];
            record->force = force;
            record->animation = NULL;
            record->distance = distance;
            record->index =
                fields->anim_set->objects != NULL ? fields->anim_set->objects - system->anim_pool->objects : -1;
        }
    }
    los = (GIZFORCE_LOS_BEST_PC *)object->gizforce_los_info;
    if (los != NULL) {
        for (i = 0; i < count; ++i) {
            f32 time = (f32)los->queried_at[PossibleForceTargetsPc[i].index];
            if (time < oldest_time) {
                oldest_time = time;
                oldest = &PossibleForceTargetsPc[i];
            }
        }
        if (oldest != NULL) {
            GIZFORCE_BEST_PC *fields = (GIZFORCE_BEST_PC *)oldest->force;
            index = oldest->index;
            los->queried_at[index] = (u32)LevelTimer.update_count;
            if (!(fields->config_flags & 0x800))
                los->clear_bits[index >> 5] |= 1u << (index & 31);
            else {
                los->clear_bits[index >> 5] &= ~(1u << (index & 31));
                clear = 0;
                if (oldest->animation != NULL) {
                    NuVecSub(&direction, NuSpecialGetDrawPos(&oldest->animation->special), &object->collision_position);
                    clear = NewRayCastScaleYMask(&object->collision_position, &direction, 0.0f, 1.0f, 0, 0) == 0;
                    if (!clear && TerrainPlatId() != -1)
                        clear = ((GIZFORCE_PLATFORM_BEST_PC *)oldest->animation->object_data)->platform_id ==
                                TerrainPlatId();
                } else {
                    GAMEANIMOBJ *animation;
                    NuVecSub(&direction, &fields->position, &object->collision_position);
                    clear = NewRayCastScaleYMask(&object->collision_position, &direction, 0.0f, 1.0f, 0, 0) == 0;
                    if (!clear && (fields->runtime_flags & 0x100) && TerrainPlatId() != -1) {
                        for (animation = fields->anim_set->objects; animation != NULL; animation = animation->next)
                            if (((GIZFORCE_PLATFORM_BEST_PC *)animation->object_data)->platform_id == TerrainPlatId()) {
                                clear = 1;
                                break;
                            }
                    }
                }
                if (clear)
                    los->clear_bits[index >> 5] |= 1u << (index & 31);
            }
        }
    }
    los = (GIZFORCE_LOS_BEST_PC *)object->gizforce_los_info;
    for (i = 0; i < count; ++i) {
        record = &PossibleForceTargetsPc[i];
        index = record->index;
        if (los != NULL && index != -1 && !(los->clear_bits[index >> 5] & (1u << (index & 31))))
            continue;
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

void __cdecl GizForce_ResetLOS(GAMEOBJECT *object) {
    if (object->gizforce_los_info != NULL)
        memset(object->gizforce_los_info, 0, 0x630);
}

typedef struct force_sfx_config_pc_s {
    u8 unknown_00[0x28];
    GAMEANIMSET *anim_set;
    u8 unknown_2c[0x82 - 0x2c];
    i16 process_sfx, complete_sfx, return_sfx;
    u8 unknown_88[0xa4 - 0x88];
} FORCE_SFX_CONFIG_PC;
STATIC_ASSERT(sizeof(FORCE_SFX_CONFIG_PC) == 0xa4);
STATIC_ASSERT(offsetof(FORCE_SFX_CONFIG_PC, anim_set) == 0x28);
STATIC_ASSERT(offsetof(FORCE_SFX_CONFIG_PC, process_sfx) == 0x82);
STATIC_ASSERT(offsetof(FORCE_SFX_CONFIG_PC, complete_sfx) == 0x84);
STATIC_ASSERT(offsetof(FORCE_SFX_CONFIG_PC, return_sfx) == 0x86);
typedef struct force_sfx_system_pc_s {
    FORCE_SFX_CONFIG_PC *forces;
    u8 unknown_04[0xe - 4];
    u16 count;
} FORCE_SFX_SYSTEM_PC;
STATIC_ASSERT(offsetof(FORCE_SFX_SYSTEM_PC, forces) == 0);
STATIC_ASSERT(offsetof(FORCE_SFX_SYSTEM_PC, count) == 0xe);
typedef struct world_force_sfx_pc_s {
    u8 unknown_0000[0x46ac];
    FORCE_SFX_SYSTEM_PC *force_system;
} WORLD_FORCE_SFX_PC;
STATIC_ASSERT(offsetof(WORLD_FORCE_SFX_PC, force_system) == 0x46ac);
static i32 GizForceSFX_load_version = 99999;
static FORCE_SFX_CONFIG_PC *GizForceSFX_force;
static WORLD_FORCE_SFX_PC *GizForceSFX_worldinfo;
static FORCE_SFX_CONFIG_PC *FindForceForSfxPc(WORLD_FORCE_SFX_PC *world, char *name) {
    FORCE_SFX_SYSTEM_PC *system = world->force_system;
    FORCE_SFX_CONFIG_PC *force;
    i32 i;
    if (system == NULL)
        return NULL;
    force = system->forces;
    for (i = 0; i < system->count; ++i, ++force) {
        GAMEANIMOBJ *object;
        if (force->anim_set == NULL)
            continue;
        for (object = force->anim_set->objects; object != NULL; object = object->next) {
            char *special_name = NuSpecialGetName(&object->special);
            if (special_name != NULL && NuStrICmp(name, special_name) == 0)
                return force;
        }
    }
    return NULL;
}
static void __cdecl GizForceSFX_forcename(NUFPAR_PC *parser) {
    GizForceSFX_force = NULL;
    if (NuFParGetWord(parser) != 0)
        GizForceSFX_force = FindForceForSfxPc(GizForceSFX_worldinfo, parser->word_buffer_ptr);
}
static void __cdecl GizForceSFX_processsfx(NUFPAR_PC *parser) {
    if (GizForceSFX_force != NULL && NuFParGetWord(parser) != 0 && GizForceSFX_force->process_sfx == -1)
        GizForceSFX_force->process_sfx = (i16)GetSfxId(parser->word_buffer_ptr);
}
static void __cdecl GizForceSFX_completesfx(NUFPAR_PC *parser) {
    if (GizForceSFX_force != NULL && NuFParGetWord(parser) != 0 && GizForceSFX_force->complete_sfx == -1)
        GizForceSFX_force->complete_sfx = (i16)GetSfxId(parser->word_buffer_ptr);
}
static void __cdecl GizForceSFX_returnsfx(NUFPAR_PC *parser) {
    if (GizForceSFX_force != NULL && NuFParGetWord(parser) != 0 && GizForceSFX_force->return_sfx == -1)
        GizForceSFX_force->return_sfx = (i16)GetSfxId(parser->word_buffer_ptr);
}
static NUFPCOMJMP GizForceSFX_ConfigKeywords[] = {{"forcename", GizForceSFX_forcename},
                                                  {"processsfx", GizForceSFX_processsfx},
                                                  {"completesfx", GizForceSFX_completesfx},
                                                  {"returnsfx", GizForceSFX_returnsfx},
                                                  {NULL, NULL}};
void __cdecl GizForceSFX_Configure(WORLDINFO *world_ptr, char *config) {
    WORLD_FORCE_SFX_PC *world = (WORLD_FORCE_SFX_PC *)world_ptr;
    NUFPAR_PC *parser;
    i32 inside = 0;
    if (GizForceSFX_load_version > 15 || world == NULL || world->force_system == NULL ||
        world->force_system->count == 0)
        return;
    GizForceSFX_force = NULL;
    GizForceSFX_worldinfo = world;
    parser = NuFParCreateMem("ForceSFX", config, 0xffff);
    if (parser == NULL)
        return;
    NuFParPushCom(parser, GizForceSFX_ConfigKeywords);
    while (NuFParGetLine(parser) != 0) {
        while (NuFParGetWord(parser) != 0) {
            if (inside) {
                if (NuStrICmp(parser->word_buffer_ptr, "forcesfx_end") == 0)
                    inside = 0;
                else
                    NuFParInterpretWord(parser);
            } else if (NuStrICmp(parser->word_buffer_ptr, "forcesfx_start") == 0)
                inside = 1;
        }
    }
    NuFParPopCom(parser);
    NuFParDestroy(parser);
}
