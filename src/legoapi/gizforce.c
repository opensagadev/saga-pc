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
static void DrawLightningBolts(GAMEOBJECT *object, GAMEOBJECT *target, i32 lightning_index) {
    f32 height;
    f32 distance;
    NUVEC reflected_origin;
    NUVEC primary;
    NUVEC secondary;
    NUVEC target_position;
    NUVEC delta;
    ForceLightning_Origin(object, &primary, &secondary);

    if (primary.y != 1000000000.0f) {
        f32 height;
        f32 distance;
        target_position.x = target->collision_position.x;
        target_position.z = target->collision_position.z;
        height = target->upper_position.y - target->lower_position.y;
        target_position.y =
            target->lower_position.y + height * 0.25f + height * (1.0f / 17.0f) * qrand() * 1.5259022e-05f;
        distance = NuVecDist(&target_position, &primary, &delta);
        NuLgtLaser(lightning_type, lightning_sizew[lightning_index], lightning_sizel[lightning_index],
                   lightning_sizewab[lightning_index], &primary, &delta, lightning_col[lightning_index],
                   lightning_endw[lightning_index], distance);
        PlaySfx("ForceLightningLp", &target->collision_position);

        if (object->field_1088 != 0 && object->field_1087 == 2 && NuIOS_IsLowEndDevice() == 0 && lightning_type != 0) {
            NUVEC reflected_target;
            f32 plane;
            reflected_origin = primary;
            reflected_target = target_position;
            plane = WORLD->current_level->reflection_y;
            if (plane == 2000000.0f)
                plane = object->shadow_opacity;
            reflected_origin.y = plane - (reflected_origin.y - plane);
            reflected_target.y = plane - (reflected_target.y - plane);
            distance = NuVecDist(&reflected_target, &reflected_origin, &delta);
            NuLgtLaser(lightning_type, lightning_sizew[lightning_index], lightning_sizel[lightning_index],
                       lightning_sizewab[lightning_index], &reflected_origin, &delta, lightning_col[lightning_index],
                       lightning_endw[lightning_index], distance);
        }
    }

    if (secondary.y == 1000000000.0f) {
        return;
    }
    target_position.x = target->collision_position.x;
    target_position.z = target->collision_position.z;
    height = target->upper_position.y - target->lower_position.y;
    target_position.y = target->lower_position.y + height * 0.25f + height * (1.0f / 17.0f) * qrand() * 1.5259022e-05f;
    distance = NuVecDist(&target_position, &secondary, &delta);
    NuLgtLaser(lightning_type, lightning_sizew[lightning_index], lightning_sizel[lightning_index],
               lightning_sizewab[lightning_index], &secondary, &delta, lightning_col[lightning_index],
               lightning_endw[lightning_index], distance);
    PlaySfx("ForceLightningLp", &target->collision_position);

    if (object->field_1088 != 0 && object->field_1087 == 2 && NuIOS_IsLowEndDevice() == 0 && lightning_type != 0) {
        NUVEC reflected_origin = secondary;
        NUVEC reflected_target = target_position;
        f32 plane = WORLD->current_level->reflection_y;
        if (plane == 2000000.0f)
            plane = object->shadow_opacity;
        reflected_origin.y = plane - (reflected_origin.y - plane);
        reflected_target.y = plane - (reflected_target.y - plane);
        distance = NuVecDist(&reflected_target, &reflected_origin, &delta);
        NuLgtLaser(lightning_type, lightning_sizew[lightning_index], lightning_sizel[lightning_index],
                   lightning_sizewab[lightning_index], &reflected_origin, &delta, lightning_col[lightning_index],
                   lightning_endw[lightning_index], distance);
    }
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
    u8 unknown_050[8];
    f32 animation_speed;
    f32 start_frame;
    f32 end_frame;
    u8 unknown_064[0x10];
    u32 config_flags;
    u8 unknown_078[0x28];
    u32 runtime_flags;
};
STATIC_ASSERT(offsetof(GIZFORCE_PC, position) == 0x1c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, anim_set) == 0x28);
STATIC_ASSERT(offsetof(GIZFORCE_PC, force_range) == 0x4c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, config_flags) == 0x74);
STATIC_ASSERT(offsetof(GIZFORCE_PC, runtime_flags) == 0xa0);
STATIC_ASSERT(offsetof(GIZFORCE_PC, animation_speed) == 0x58);
STATIC_ASSERT(offsetof(GIZFORCE_PC, start_frame) == 0x5c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, end_frame) == 0x60);

static void GizForce_PlayForwardAnimationPc(GIZFORCE_PC *force) {
    f32 speed;
    if (force != NULL) {
        GameAnimSet_SetRepeating(force->anim_set, 0);
        if (force->animation_speed < 0.0f)
            speed = force->end_frame * force->animation_speed;
        else
            speed = force->start_frame * force->animation_speed;
        GameAnimSet_Play(force->anim_set, speed, 0);
    }
}

static void GizForce_PlayBackwardAnimationPc(GIZFORCE_PC *force) {
    f32 speed;
    if (force != NULL) {
        GameAnimSet_SetRepeating(force->anim_set, 0);
        if (force->animation_speed < 0.0f)
            speed = force->start_frame * -force->animation_speed;
        else
            speed = force->end_frame * -force->animation_speed;
        GameAnimSet_Play(force->anim_set, speed, 0);
    }
}

void __cdecl GizForce_PlayForwards(GIZFORCE_PC *force) {
    if (force != NULL) {
        if (force->runtime_flags & 0x40)
            GizForce_PlayBackwardAnimationPc(force);
        else
            GizForce_PlayForwardAnimationPc(force);
    }
}

void __cdecl GizForce_PlayBackwards(GIZFORCE_PC *force) {
    if (force != NULL) {
        if (force->runtime_flags & 0x40)
            GizForce_PlayForwardAnimationPc(force);
        else
            GizForce_PlayBackwardAnimationPc(force);
    }
}

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

static void *GizForces_ReserveBufferSpace(void *world_ptr) {
    i32 i;
    WORLDINFO *world = ((WORLDINFO *)(world_ptr));
    GIZFORCESYS_s *force_sys =
        ((GIZFORCESYS_s *)(GameBufferAlloc(&world->giz_buffer, &world->buffer_end, sizeof(GIZFORCESYS_s))));

    force_sys->capacity = world->current_level->max_force;
    force_sys->forces = ((GIZFORCE_s *)(GameBufferAlloc(&world->giz_buffer, &world->buffer_end,
                                                        force_sys->capacity * sizeof(GIZFORCE_s))));
    force_sys->hit_test_gizmos = ((GIZMO_PC **)(GameBufferAlloc(&world->giz_buffer, &world->buffer_end,
                                                                force_sys->capacity * sizeof(GIZMO_PC *))));
    force_sys->visible_forces = ((GIZFORCE_s **)(GameBufferAlloc(&world->giz_buffer, &world->buffer_end,
                                                                 force_sys->capacity * sizeof(GIZFORCE_s *))));
    force_sys->anim_pool = GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 8,
                                                        world->current_level->max_force_objects);

    for (i = 0; i < force_sys->capacity; ++i) {
        force_sys->forces[i].anim_set =
            GameAnimSet_Create(&world->giz_buffer, &world->buffer_end, force_sys->anim_pool, world->game_anim_sys);
    }
    world->giz_force_sys = force_sys;
    return force_sys;
}

void GizForces_Draw(void *world_ptr, void *data, float unused2) {
    struct GIZFORCE_s *force;
    u16 force_sys_count;
    i32 index;
    WORLDINFO *world = (WORLDINFO *)(world_ptr);
    GIZFORCESYS_s *force_sys = (GIZFORCESYS_s *)(data);
    if (force_sys == NULL) {
        return;
    }

    force = force_sys->forces;
    force_sys_count = force_sys->count;
    for (index = 0; index < force_sys_count; ++index, ++force) {
        if ((force->progress_flags & GIZFORCE_PROGRESS_VISIBLE) == 0 || (i8)(force->progress_flags) >= 0 ||
            (force->room_id >= 0 && world->rooms_visible_ptr[force->room_id] == 0)) {
            continue;
        }

        ResetShadowMapRendering();
        if ((force->config_flags & GIZFORCE_CONFIG_DRAW_REFLECTION) != 0) {
            GameAnimSet_DrawReflection(force->anim_set, 2, force->vertical_range, NULL);
        }
        if ((force->config_flags & GIZFORCE_CONFIG_CAST_SHADOW) == 0) {
            EnableShadowMapRendering(0);
        }
        if ((force->config_flags & GIZFORCE_CONFIG_ALONG_SOCKET) != 0) {
            struct gameanimobj_s *anim_object;
            struct numtx_s *draw_matrix;
            if ((force->runtime_flags & GIZFORCE_RUNTIME_ALONG_SOCKET_HIDDEN) == 0) {
                continue;
            }

            anim_object = force->anim_set->objects;
            if (anim_object == NULL) {
                continue;
            }
            draw_matrix = NuSpecialGetDrawMtx(&anim_object->special);
            NuSpecialDrawAt(&force->along_socket, draw_matrix);
            if ((force->config_flags & GIZFORCE_CONFIG_DRAW_REFLECTION) != 0) {
                NUMTX reflection_matrix;
                if (MatrixReflection(draw_matrix, 2, draw_matrix->m31 + force->vertical_range,
                                     WORLD->current_level->reflection_y, &reflection_matrix) != 0) {
                    NuRndrStartReflectionRender(0);
                    NuSpecialDrawAt(&force->along_socket, &reflection_matrix);
                    NuRndrEndReflectionRender();
                }
            }
        }
    }
    ResetShadowMapRendering();
}

typedef struct trigger_force_object_pc_s {
    u8 unknown000[0x10cc];
    GIZFORCE_PC *force;
} TRIGGER_FORCE_OBJECT_PC;
STATIC_ASSERT(offsetof(TRIGGER_FORCE_OBJECT_PC, force) == 0x10cc);
i32 __cdecl GizForce_GameObjUsingForce(GAMEOBJECT *object, GIZFORCE_PC *force) {
    return force != NULL && object != NULL && object->character_context == 8 &&
           ((TRIGGER_FORCE_OBJECT_PC *)object)->force == force;
}

static char *GizmoForce_GetOutputName(GIZMO_PC *unused0, i32 output_index) {
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

GIZFORCE_s *GizForces_FindForce(WORLDINFO *world, char *name) {
    GIZMO_PC *gizmo = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, name);
    return gizmo != NULL ? ((GIZFORCE_s *)(gizmo->object)) : NULL;
}

static void GizmoForce_Activate(GIZMO_PC *gizmo, i32 activate) {
    struct GIZFORCE_s *force;
    if (gizmo == NULL) {
        return;
    }
    force = ((GIZFORCE_s *)(gizmo->object));
    if (activate == 0) {
        force->progress_flags &= ((u8)(~GIZFORCE_PROGRESS_ENABLED));
        return;
    }

    if (!force->progress_enabled) {
        GameAnimSet_JumpToStart(force->anim_set);
    }
    force->field_0xaa &= ((u8)(~GIZFORCE_STATE_DESTROYED_OR_THROWN));
    force->progress_enabled = 1;
    force->progress_reverse_active = 0;
    force->progress_animation_reversed = 0;
    force->field_0x48 = 0.0f;
    force->field_0x50 = 0.0f;
    if ((force->config_flags & GIZFORCE_CONFIG_RESET_STATE_ON_ACTIVATE) != 0) {
        force->runtime_reward_released = 0;
    }
    force->runtime_along_socket_hidden = 0;
    force->runtime_offset_applied = 0;
    force->runtime_force_range_complete = 0;
    force->runtime_completion_released = 0;
    GameAnimSet_EvaluateState(force->anim_set);
    force->radius = 1.0f;
    force->position = force->file_position;
    GameAnimSet_GetCentreAndRadius(force->anim_set, &force->position, &force->radius, 2, 1, 1);
}
