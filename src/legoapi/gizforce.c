#include "saga.h"

i32 __cdecl GizmoForce_GetNumOutputs(GIZMO_PC *unused) {
    (void)unused;
    return 8;
}

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

typedef struct FORCE_RESET_GROUP_PC {
    union {
        struct {
            GIZFORCE_PC *forces[8];
            f32 combined_height;
        };
        u32 slot_words[9];
    };
    u16 runtime_flags;
    u8 member_count;
    u8 configured_count;
} FORCE_RESET_GROUP_PC;
STATIC_ASSERT(sizeof(FORCE_RESET_GROUP_PC) == 0x28);
STATIC_ASSERT(offsetof(FORCE_RESET_GROUP_PC, slot_words) == 0);
STATIC_ASSERT(offsetof(FORCE_RESET_GROUP_PC, combined_height) == 0x20);
STATIC_ASSERT(offsetof(FORCE_RESET_GROUP_PC, member_count) == 0x26);
STATIC_ASSERT(offsetof(FORCE_RESET_GROUP_PC, configured_count) == 0x27);

struct gizforce_pc_s {
    char name[16];
    NUVEC file_position;
    NUVEC position;
    GAMEANIMSET *anim_set;
    NUSPECIALHANDLE along_socket;
    GAMEOBJECT *using_object;
    u32 reset_word;
    FORCE_RESET_GROUP_PC *group;
    f32 force_strength;
    f32 field_48;
    f32 force_range;
    f32 field_50;
    f32 interaction_radius;
    f32 animation_speed;
    f32 start_frame, end_frame;
    f32 effect_scale;
    f32 radius;
    f32 vertical_range, horizontal_range;
    u32 config_flags;
    u8 unknown_078[4];
    u8 group_index, collision_mask, activation_mode;
    u8 unknown_07f;
    i16 room_id;
    i16 start_sfx_id, loop_sfx_id, stop_sfx_id;
    i16 blowup_type;
    i16 pickup_count, pickup_rotation_x, pickup_rotation_y;
    NUVEC effect_position;
    f32 activation_radius;
    u32 runtime_flags;
};
STATIC_ASSERT(sizeof(GIZFORCE_PC) == 0xa4);
STATIC_ASSERT(offsetof(GIZFORCE_PC, reset_word) == 0x3c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, group) == 0x40);
STATIC_ASSERT(offsetof(GIZFORCE_PC, field_48) == 0x48);
STATIC_ASSERT(offsetof(GIZFORCE_PC, field_50) == 0x50);
STATIC_ASSERT(offsetof(GIZFORCE_PC, radius) == 0x68);
STATIC_ASSERT(offsetof(GIZFORCE_PC, room_id) == 0x80);
STATIC_ASSERT(offsetof(GIZFORCE_PC, file_position) == 0x10);
STATIC_ASSERT(offsetof(GIZFORCE_PC, position) == 0x1c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, anim_set) == 0x28);
STATIC_ASSERT(offsetof(GIZFORCE_PC, along_socket) == 0x2c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, force_strength) == 0x44);
STATIC_ASSERT(offsetof(GIZFORCE_PC, force_range) == 0x4c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, interaction_radius) == 0x54);
STATIC_ASSERT(offsetof(GIZFORCE_PC, animation_speed) == 0x58);
STATIC_ASSERT(offsetof(GIZFORCE_PC, start_frame) == 0x5c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, end_frame) == 0x60);
STATIC_ASSERT(offsetof(GIZFORCE_PC, effect_scale) == 0x64);
STATIC_ASSERT(offsetof(GIZFORCE_PC, vertical_range) == 0x6c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, horizontal_range) == 0x70);
STATIC_ASSERT(offsetof(GIZFORCE_PC, config_flags) == 0x74);
STATIC_ASSERT(offsetof(GIZFORCE_PC, group_index) == 0x7c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, collision_mask) == 0x7d);
STATIC_ASSERT(offsetof(GIZFORCE_PC, activation_mode) == 0x7e);
STATIC_ASSERT(offsetof(GIZFORCE_PC, start_sfx_id) == 0x82);
STATIC_ASSERT(offsetof(GIZFORCE_PC, loop_sfx_id) == 0x84);
STATIC_ASSERT(offsetof(GIZFORCE_PC, stop_sfx_id) == 0x86);
STATIC_ASSERT(offsetof(GIZFORCE_PC, blowup_type) == 0x88);
STATIC_ASSERT(offsetof(GIZFORCE_PC, pickup_count) == 0x8a);
STATIC_ASSERT(offsetof(GIZFORCE_PC, pickup_rotation_x) == 0x8c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, pickup_rotation_y) == 0x8e);
STATIC_ASSERT(offsetof(GIZFORCE_PC, effect_position) == 0x90);
STATIC_ASSERT(offsetof(GIZFORCE_PC, activation_radius) == 0x9c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, runtime_flags) == 0xa0);

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
    return NuAtan2D(force->position.x - position->x, dz);
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
    i32 super_force, count = 0, i;
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
        NUVEC *draw_position;
        GIZFORCE_GROUP_BEST_PC *group;
        if (fields->using_object != NULL || fields->unavailable_flags != 0)
            continue;
        if ((fields->config_flags & 0x10) && !(object->character_data->model_flags & 4) && !super_force)
            continue;
        group = fields->group;
        if (group == NULL) {
            if (GizForce_Complete(force))
                continue;
        } else if (group->count != 0 && force != group->forces[group->count - 1]) {
            if (group->flags & 1)
                continue;
            if (fields->anim_set->state == 2)
                continue;
            if (group->forces[group->count - 1] != NULL &&
                ((((GIZFORCE_BEST_PC *)group->forces[group->count - 1])->anim_set->flags & 7) ||
                 ((GIZFORCE_BEST_PC *)group->forces[group->count - 1])->using_object != NULL))
                continue;
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
                draw_position = NuSpecialGetDrawPos(&animation->special);
                distance = NuVecDistSqr(draw_position, &object->collision_position, &delta);
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
            los->queried_at[oldest->index] = (u32)LevelTimer.update_count;
            if (((GIZFORCE_BEST_PC *)oldest->force)->config_flags & 0x800) {
                ((GIZFORCE_LOS_BEST_PC *)object->gizforce_los_info)->clear_bits[oldest->index >> 5] &=
                    ~(1u << (oldest->index & 31));
                if (oldest->animation != NULL) {
                    NUVEC *draw_position = NuSpecialGetDrawPos(&oldest->animation->special);
                    NuVecSub(&direction, draw_position, &object->collision_position);
                    if (NewRayCastScaleYMask(&object->collision_position, &direction, 0.0f, 1.0f, 0, 0) != 0) {
                        if (TerrainPlatId() == -1)
                            goto choose;
                        if (((GIZFORCE_PLATFORM_BEST_PC *)oldest->animation->object_data)->platform_id !=
                            TerrainPlatId())
                            goto choose;
                    }
                } else {
                    GAMEANIMOBJ *animation;
                    NuVecSub(&direction, &((GIZFORCE_BEST_PC *)oldest->force)->position, &object->collision_position);
                    if (NewRayCastScaleYMask(&object->collision_position, &direction, 0.0f, 1.0f, 0, 0) != 0) {
                        if (!(((GIZFORCE_BEST_PC *)oldest->force)->runtime_flags & 0x100))
                            goto choose;
                        if (TerrainPlatId() == -1)
                            goto choose;
                        for (animation = ((GIZFORCE_BEST_PC *)oldest->force)->anim_set->objects;;
                             animation = animation->next) {
                            if (animation == NULL)
                                goto choose;
                            if (((GIZFORCE_PLATFORM_BEST_PC *)animation->object_data)->platform_id == TerrainPlatId())
                                break;
                        }
                    }
                }
            }
            ((GIZFORCE_LOS_BEST_PC *)object->gizforce_los_info)->clear_bits[oldest->index >> 5] |=
                1u << (oldest->index & 31);
        }
    }
choose:
    los = (GIZFORCE_LOS_BEST_PC *)object->gizforce_los_info;
    for (i = 0; i < count; ++i) {
        record = &PossibleForceTargetsPc[i];
        if (los != NULL && record->index != -1 && !(los->clear_bits[record->index >> 5] & (1u << (record->index & 31))))
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
static FORCE_SFX_CONFIG_PC *GizForceSFX_force = 0;
static WORLD_FORCE_SFX_PC *GizForceSFX_worldinfo = 0;
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
            NuSpecialDrawAt((NUSPECIALHANDLE *)&force->along_socket, draw_matrix);
            if ((force->config_flags & GIZFORCE_CONFIG_DRAW_REFLECTION) != 0) {
                NUMTX reflection_matrix;
                if (MatrixReflection(draw_matrix, 2, draw_matrix->m31 + force->vertical_range,
                                     WORLD->current_level->reflection_y, &reflection_matrix) != 0) {
                    NuRndrStartReflectionRender(0);
                    NuSpecialDrawAt((NUSPECIALHANDLE *)&force->along_socket, &reflection_matrix);
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
__declspec(noinline) i32 __cdecl GizForce_GameObjUsingForce(GAMEOBJECT *object, GIZFORCE_PC *force) {
    return force != NULL && object != NULL && object->character_context == 8 &&
           ((TRIGGER_FORCE_OBJECT_PC *)object)->force == force;
}

char *__cdecl GizmoForce_GetOutputName(GIZMO_PC *unused0, i32 output_index) {
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

/* Private integration fragment: append to gizforce.c after its actual static
 * version owner. Extend its existing GIZFORCE_PC with the named native fields
 * in the companion layout file. */
extern const char GizmoRegistryGizForceNamePc[];

static __declspec(noinline) char *ForceCopyNamePrefixPc(char *source, char *destination, i32 capacity) {
    char *result = destination;
    i32 index = 0;
    if (destination == NULL || source == NULL)
        return NULL;
    while (index++ < capacity - 1) {
        char value = *source++;
        *destination++ = value;
        if (value == '_' || *source == '\0')
            break;
    }
    return result;
}

static __declspec(noinline) void ForceFindBaseSpecialPc(GIZFORCE_PC *force) {
    GAMEANIMOBJ *object;
    char *name;
    char socket_name[64];
    if (force == NULL)
        return;
    force->along_socket.scene = NULL;
    force->along_socket.special = NULL;
    force->along_socket.display_special = NULL;
    if (!(force->config_flags & 0x40))
        return;
    for (object = force->anim_set->objects; object != NULL; object = object->next) {
        name = NuSpecialGetName(&object->special);
        if (name != NULL) {
            memset(socket_name, 0, sizeof(socket_name));
            if (ForceCopyNamePrefixPc(name, socket_name, sizeof(socket_name)) != NULL) {
                NuStrCat(socket_name, "b");
                if (NuSpecialFind(object->special.scene, &force->along_socket, socket_name, 1) != 0)
                    break;
            }
        }
    }
}

static void __cdecl edgizforce_ReadAnimSetData(GAMEANIMOBJ *object, u8 version) {
    GIZFORCEANIMDATA_s temporary;
    GIZFORCEANIMDATA_s *data;
    if (object == NULL)
        return;
    data = (GIZFORCEANIMDATA_s *)object->object_data;
    if (data == NULL)
        data = &temporary;
    if (version > 8)
        data->flags = (u16)EdFileReadShort();
}

i32 __cdecl GizForces_Load(void *world_pointer, void *system_pointer) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    GIZFORCESYS_s *system = (GIZFORCESYS_s *)system_pointer;
    GIZFORCE_PC *force;
    u8 version;
    i32 index, length;
    char blowup_name[32];
    char sfx_name[64];
    NUVEC unused;
    version = (u8)EdFileReadChar();
    GizForceSFX_load_version = version;
    system->count = (u16)EdFileReadShort();
    if (system->count == 0)
        return 1;
    force = (GIZFORCE_PC *)system->forces;
    for (index = 0; index < system->count; ++index, ++force) {
        EdFileRead(force->name, 16);
        EdFileReadNuVec(&force->file_position);
        if (version == 1)
            EdFileReadNuVec(&unused);
        force->force_strength = EdFileReadFloat();
        if (version >= 8)
            force->force_range = EdFileReadFloat();
        force->interaction_radius = EdFileReadFloat();
        if (version == 1) {
            EdFileReadNuVec(&unused);
            EdFileReadShort();
        }
        force->config_flags = (u32)EdFileReadInt();
        if (version <= 12)
            force->config_flags |= 0x800;
        force->group_index = (u8)EdFileReadChar();
        if (version >= 11) {
            force->collision_mask = (u8)EdFileReadChar();
            if (version == 11 && force->collision_mask != 0)
                force->collision_mask = (u8)(1u << ((force->collision_mask - 1) & 31));
        }
        force->activation_mode = (u8)EdFileReadChar();
        if (version == 1)
            EdFileReadChar();
        GizmoFileReadGameAnimSet(force->anim_set, world, edgizforce_ReadAnimSetData, version,
                                 (char *)GizmoRegistryGizForceNamePc, force->name);
        force->start_frame = EdFileReadFloat();
        force->end_frame = EdFileReadFloat();
        if (version >= 6)
            force->effect_scale = EdFileReadFloat();
        force->horizontal_range = version >= 7 ? EdFileReadFloat() : 1.0f;
        if (version >= 3)
            force->vertical_range = EdFileReadFloat();
        force->blowup_type = -1;
        if (version >= 4) {
            if (version == 4) {
                force->blowup_type = EdFileReadShort();
            } else {
                length = (i8)EdFileReadChar();
                if (length != 0) {
                    EdFileRead(blowup_name, length);
                    force->blowup_type = (i16)GizmoBlowupGetNameTableId(blowup_name);
                    if (force->blowup_type != -1)
                        force->runtime_flags |= 0x400;
                }
            }
            force->pickup_count = EdFileReadShort();
            force->pickup_rotation_x = EdFileReadShort();
            force->pickup_rotation_y = EdFileReadShort();
            EdFileReadNuVec(&force->effect_position);
        }
        if (version >= 10)
            force->activation_radius = EdFileReadFloat();
        else
            force->activation_radius = world->area != NULL && (world->area->flags & 1) ? 12.0f : 1.75f;
        force->start_sfx_id = -1;
        force->loop_sfx_id = -1;
        force->stop_sfx_id = -1;
        if (version >= 14) {
            if (GizmoFileReadName(sfx_name))
                force->start_sfx_id = (i16)GetSfxId(sfx_name);
            if (GizmoFileReadName(sfx_name))
                force->loop_sfx_id = (i16)GetSfxId(sfx_name);
            if (GizmoFileReadName(sfx_name))
                force->stop_sfx_id = (i16)GetSfxId(sfx_name);
        }
        if (version <= 14) {
            force->start_sfx_id = -1;
            force->loop_sfx_id = -1;
            force->stop_sfx_id = -1;
        }
        if (force->config_flags & 0x40)
            ForceFindBaseSpecialPc(force);
    }
    return 1;
}

void __cdecl GameAnimSet_SetOffset(GAMEANIMSET *, NUVEC *);

typedef struct FORCE_RESET_PROGRESS_PC {
    u32 enabled[4], visible[4], reward_released[4], socket_hidden[4];
    u32 completion_released[4], completion_pending[4], destroyed[4];
    u8 group_members[8][8];
} FORCE_RESET_PROGRESS_PC;
STATIC_ASSERT(sizeof(FORCE_RESET_PROGRESS_PC) == 0xb0);
STATIC_ASSERT(offsetof(FORCE_RESET_PROGRESS_PC, visible) == 0x10);
STATIC_ASSERT(offsetof(FORCE_RESET_PROGRESS_PC, reward_released) == 0x20);
STATIC_ASSERT(offsetof(FORCE_RESET_PROGRESS_PC, socket_hidden) == 0x30);
STATIC_ASSERT(offsetof(FORCE_RESET_PROGRESS_PC, completion_released) == 0x40);
STATIC_ASSERT(offsetof(FORCE_RESET_PROGRESS_PC, completion_pending) == 0x50);
STATIC_ASSERT(offsetof(FORCE_RESET_PROGRESS_PC, destroyed) == 0x60);
STATIC_ASSERT(offsetof(FORCE_RESET_PROGRESS_PC, group_members) == 0x70);
STATIC_ASSERT(offsetof(GIZFORCESYS_s, forces) == 0);
STATIC_ASSERT(offsetof(GIZFORCESYS_s, count) == 0xe);
STATIC_ASSERT(offsetof(GIZFORCESYS_s, groups) == 0x18);
STATIC_ASSERT(sizeof(GIZFORCESYS_s) == 0x158);
STATIC_ASSERT(offsetof(GAMEANIMSET, objects) == 0x18);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, special) == 4);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, object_data) == 0x24);
STATIC_ASSERT(offsetof(GIZFORCEANIMDATA_s, platform_id) == 2);
STATIC_ASSERT(offsetof(WORLDINFO, terrain) == 0x295c);
STATIC_ASSERT(offsetof(WORLDINFO, current_gscn) == 0x13c);

static __declspec(noinline) void ForceResetSinglePc(WORLDINFO *world, GIZFORCE_PC *force) {
    GAMEANIMOBJ *object;
    force->runtime_flags = (force->runtime_flags & 0xffe2049fu) | 3;
    force->animation_speed = 1.0f;
    force->field_48 = 0.0f;
    force->field_50 = 0.0f;
    force->reset_word = 0;
    if (force->anim_set == NULL)
        return;
    for (object = force->anim_set->objects; object != NULL; object = object->next) {
        GIZFORCEANIMDATA_s *data = (GIZFORCEANIMDATA_s *)object->object_data;
        data->platform_id = -1;
        if (world->terrain != NULL && (data->flags & 1) == 0 && NuSpecialExistsFn(&object->special) != 0) {
            data->platform_id = (i16)FindPlatInst(NuSpecialGetInstanceix(&object->special));
            if (data->platform_id != -1)
                force->runtime_flags |= 0x100;
        }
    }
    force->room_id = world->current_gscn != NULL ? NuPortalWhichRoom(world->current_gscn, &force->file_position) : -1;
    GameAnimSet_SetOffset(force->anim_set, &v000);
    force->radius = 1.0f;
    force->position = force->file_position;
    GameAnimSet_GetCentreAndRadius(force->anim_set, &force->position, &force->radius, 2, 1, 1);
    GameAnimSet_EvaluateState(force->anim_set);
}

static __declspec(noinline) f32 ForceAnimatedHeightPc(GIZFORCE_PC *force) {
    GAMEANIMOBJ *object;
    f32 minimum_y = 1.0e9f, maximum_y = -1.0e9f;
    volatile f32 height;
    if (force == NULL || force->anim_set == NULL)
        return 0.0f;
    for (object = force->anim_set->objects; object != NULL; object = object->next) {
        NUVEC minimum, maximum;
        NuSpecialGetBounds(&object->special, &minimum, &maximum);
        if (minimum.y < minimum_y)
            minimum_y = minimum.y;
        if (maximum.y > maximum_y)
            maximum_y = maximum.y;
    }
    height = maximum_y - minimum_y;
    return height;
}

void __cdecl GizForces_Reset(void *world_ptr, void *data_ptr, void *progress_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZFORCESYS_s *system = (GIZFORCESYS_s *)data_ptr;
    FORCE_RESET_PROGRESS_PC *progress = (FORCE_RESET_PROGRESS_PC *)progress_ptr;
    GIZFORCE_PC *force;
    i32 index;
    memset(system->groups, 0, 0x140);
    force = (GIZFORCE_PC *)system->forces;
    for (index = 0; index < system->count; ++index, ++force) {
        i32 group_index;
        ForceResetSinglePc(world, force);
        if (progress != NULL && index < 128) {
            i32 word = index >> 5;
            u32 bit = 1u << (index & 31);
            force->runtime_flags = (force->runtime_flags & ~2u) | ((progress->visible[word] & bit) != 0 ? 2u : 0);
            force->runtime_flags = (force->runtime_flags & ~1u) | ((progress->enabled[word] & bit) != 0 ? 1u : 0);
            force->runtime_flags =
                (force->runtime_flags & ~0x200u) | ((progress->reward_released[word] & bit) != 0 ? 0x200u : 0);
            force->runtime_flags =
                (force->runtime_flags & ~0x800u) | ((progress->socket_hidden[word] & bit) != 0 ? 0x800u : 0);
            force->runtime_flags =
                (force->runtime_flags & ~0x4000u) | ((progress->completion_released[word] & bit) != 0 ? 0x4000u : 0);
            force->runtime_flags =
                (force->runtime_flags & ~0x8000u) | ((progress->completion_pending[word] & bit) != 0 ? 0x8000u : 0);
            force->runtime_flags =
                (force->runtime_flags & ~0x10000u) | ((progress->destroyed[word] & bit) != 0 ? 0x10000u : 0);
        }
        group_index = (i8)force->group_index;
        if (group_index >= 0 && group_index < 8) {
            force->group = (FORCE_RESET_GROUP_PC *)&system->groups[group_index];
            ++force->group->configured_count;
        } else {
            force->group = NULL;
        }
    }
    if (progress != NULL) {
        i32 group_index, member_index;
        for (group_index = 0; group_index < 8; ++group_index) {
            for (member_index = 0; member_index < 8; ++member_index) {
                u8 force_index = progress->group_members[group_index][member_index];
                if (force_index >= system->count)
                    continue;
                force = (GIZFORCE_PC *)system->forces + force_index;
                if ((force->runtime_flags & 0x20) == 0) {
                    if (force->group != NULL && force->group->member_count < 8) {
                        if (force->group->member_count != 0) {
                            FORCE_RESET_GROUP_PC *group = force->group;
                            f32 height = ForceAnimatedHeightPc(group->forces[group->member_count - 1]);
                            group->combined_height += height;
                        }
                        force->group->forces[force->group->member_count] = force;
                        ++force->group->member_count;
                        force->runtime_flags |= 0x20;
                    }
                }
                if ((force->runtime_flags & 0x20) != 0) {
                    NUVEC offset;
                    offset.x = 0.0f;
                    offset.y = GameAnimSet_GetCompletionRatio(force->anim_set) * force->group->combined_height;
                    offset.z = 0.0f;
                    GameAnimSet_SetOffset(force->anim_set, &offset);
                    force->radius = 1.0f;
                    force->position = force->file_position;
                    GameAnimSet_GetCentreAndRadius(force->anim_set, &force->position, &force->radius, 2, 1, 1);
                }
            }
        }
    }
}

STATIC_ASSERT(offsetof(GIZFORCE_PC, room_id) == 0x80);
void __cdecl GizForces_DrawPc(void *world_pointer, void *system_pointer, f32 unused) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    GIZFORCESYS_s *system = (GIZFORCESYS_s *)system_pointer;
    GIZFORCE_PC *force;
    GAMEANIMOBJ *object;
    NUMTX *draw_matrix, reflection;
    f32 reflection_height;
    i32 index;
    (void)unused;
    if (system == NULL)
        return;
    force = (GIZFORCE_PC *)system->forces;
    for (index = 0; index < system->count; ++index, ++force) {
        if (!(force->runtime_flags & 2) || !(force->runtime_flags & 0x80) ||
            (force->room_id >= 0 && world->rooms_visible_ptr[force->room_id] == 0))
            continue;
        ResetShadowMapRendering();
        if (force->config_flags & 4)
            GameAnimSet_DrawReflection(force->anim_set, 2, force->vertical_range, NULL);
        if (!(force->config_flags & 0x1000))
            EnableShadowMapRendering(0);
        if (!(force->config_flags & 0x40) || !(force->runtime_flags & 0x800))
            continue;
        object = force->anim_set->objects;
        if (object == NULL)
            continue;
        draw_matrix = NuSpecialGetDrawMtx((NUSPECIALHANDLE *)&object->special);
        NuSpecialDrawAt((NUSPECIALHANDLE *)&force->along_socket, draw_matrix);
        if (force->config_flags & 4) {
            reflection_height = force->vertical_range + draw_matrix->m31;
            if (MatrixReflection(draw_matrix, 2, reflection_height, WORLD->current_level->reflection_y, &reflection)) {
                NuRndrStartReflectionRender(0);
                NuSpecialDrawAt((NUSPECIALHANDLE *)&force->along_socket, &reflection);
                NuRndrEndReflectionRender();
            }
        }
    }
    ResetShadowMapRendering();
}

f32 ForceAnimationSeekRatePc = 5.0f;

extern f32 ForceAnimationSeekRatePc;
STATIC_ASSERT(sizeof(GIZFORCE_PC) == 0xa4);
STATIC_ASSERT(sizeof(FORCE_RESET_GROUP_PC) == 0x28);
STATIC_ASSERT(offsetof(FORCE_RESET_GROUP_PC, combined_height) == 0x20);
STATIC_ASSERT(offsetof(FORCE_RESET_GROUP_PC, member_count) == 0x26);
STATIC_ASSERT(offsetof(GIZFORCE_PC, anim_set) == 0x28);
STATIC_ASSERT(offsetof(GIZFORCE_PC, animation_speed) == 0x58);
STATIC_ASSERT(offsetof(GIZFORCE_PC, runtime_flags) == 0xa0);
STATIC_ASSERT(offsetof(GAMEOBJECT, powerup_time) == 0xdec);

static __declspec(noinline) i32 ForceUpdateCentrePc(GIZFORCE_PC *force, i32 frame_selection) {
    force->radius = 1.0f;
    force->position = force->file_position;
    return GameAnimSet_GetCentreAndRadius(force->anim_set, &force->position, &force->radius, frame_selection, 1, 1);
}

static __declspec(noinline) f32 ForceUpdateSpeedPc(GIZFORCE_PC *force, GAMEOBJECT *object) {
    f32 target = 1.0f;
    if (force != NULL && object != NULL) {
        if (Cheats_CheckFlags(0x400000) != 0 || object->powerup_time > 0.0f)
            target = 3.0f;
    }
    return SeekValF(force->animation_speed, target, ForceAnimationSeekRatePc);
}

static __declspec(noinline) void ForceGroupPushPc(GIZFORCE_PC *force) {
    if ((force->runtime_flags & 0x20) == 0 && force->group != NULL && force->group->member_count < 8) {
        if (force->group->member_count != 0) {
            FORCE_RESET_GROUP_PC *group = force->group;
            f32 height = ForceAnimatedHeightPc(group->forces[group->member_count - 1]);
            group->combined_height += height;
        }
        force->group->forces[force->group->member_count] = force;
        ++force->group->member_count;
        force->runtime_flags |= 0x20;
    }
}

static __declspec(noinline) void ForceGroupPopPc(GIZFORCE_PC *force) {
    if ((force->runtime_flags & 0x20) != 0 && force->group != NULL && force->group->member_count != 0 &&
        force->group->forces[force->group->member_count - 1] == force) {
        /* Retail clears slot COUNT (not COUNT-1); count8 aliases combined_height. */
        force->group->slot_words[force->group->member_count] = 0;
        --force->group->member_count;
        if (force->group->member_count != 0) {
            f64 old_height = force->group->combined_height;
            force->group->combined_height =
                (f32)(old_height - ForceAnimatedHeightPc(force->group->forces[force->group->member_count - 1]));
        }
        force->runtime_flags &= ~0x20u;
        GameAnimSet_SetOffset(force->anim_set, &v000);
    }
}

static __declspec(noinline) i32 ForceAnimationDonePc(GIZFORCE_PC *force) {
    if (force == NULL || force->anim_set == NULL)
        return 1;
    if (force->runtime_flags & 0x40)
        return force->anim_set->state == 0;
    return force->anim_set->state == 2;
}

extern i32 force_gizmotype_id;
STATIC_ASSERT(offsetof(GIZFORCE_PC, using_object) == 0x38);
STATIC_ASSERT(offsetof(FORCE_RESET_GROUP_PC, runtime_flags) == 0x24);
STATIC_ASSERT(offsetof(WORLDINFO, gizmo_sys) == 0x2ac8);
STATIC_ASSERT(sizeof(GIZMOSET_PC) == 0x14);
STATIC_ASSERT(sizeof(GIZMO_PC) == 8);

void __cdecl GizForces_Update(void *world_pointer, void *system_pointer, f32 unused_time) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    GIZFORCESYS_s *system = (GIZFORCESYS_s *)system_pointer;
    GIZMOSYS_PC *gizmos;
    GIZMOSET_PC *set;
    GIZMO_PC *gizmo;
    i32 index, group_index;
    (void)unused_time;
    if (world == NULL || system == NULL)
        return;
    gizmos = (GIZMOSYS_PC *)world->gizmo_sys;
    if (gizmos == NULL || gizmos->sets == NULL)
        return;
    for (group_index = 0; group_index < 8; ++group_index)
        ((FORCE_RESET_GROUP_PC *)&system->groups[group_index])->runtime_flags &= ~1u;
    system->visible_force_count = 0;
    system->hit_test_gizmo_count = 0;
    set = &gizmos->sets[force_gizmotype_id];
    gizmo = set->gizmos;
    for (index = 0; index < set->count; ++index, ++gizmo) {
        GIZFORCE_PC *force = (GIZFORCE_PC *)gizmo->object;
        GAMEOBJECT *user;
        i32 was_moving, playing_forwards = 0, started_shaking = 0, updating_animation = 0;
        GAMEANIMOBJ *object;
        FORCE_RESET_GROUP_PC *group;
        NUVEC offset;
        if (force->using_object != NULL && force->group != NULL && (force->runtime_flags & 0x20) == 0 &&
            (force->group->runtime_flags & 1) != 0 && force->using_object->character_context == 8) {
            force->using_object->character_context = -1;
            ((GIZFORCE_OBJECT_BEST_PC *)force->using_object)->target = NULL;
            force->using_object = NULL;
        }
        user = force->using_object;
        force->using_object = NULL;
        force->runtime_flags = (force->runtime_flags & ~0x200000u) | (user != NULL ? 0x200000u : 0);
        was_moving = (force->anim_set->flags >> 1) & 1;
        force->runtime_flags &= ~0x80u;
        if ((force->runtime_flags & 0x1000) != 0 && force->field_50 == 0.0f) {
            offset.x = 0.0f;
            offset.y = (force->runtime_flags & 0x20) != 0
                           ? GameAnimSet_GetCompletionRatio(force->anim_set) * force->group->combined_height
                           : 0.0f;
            offset.z = 0.0f;
            GameAnimSet_SetOffset(force->anim_set, &offset);
            force->runtime_flags &= ~0x1000u;
        }
        if ((force->runtime_flags & 3) != 3 || force->anim_set == NULL || (force->runtime_flags & 0x10010) != 0)
            continue;
        if (NuCameraClipTestSphere(&force->position, force->radius, &numtx_identity) == 0) {
            ((GIZFORCE_PC **)system->visible_forces)[system->visible_force_count++] = force;
            force->runtime_flags |= 0x80;
        }
        if (user == NULL && force->effect_scale > 1.0f && (force->config_flags & 2) == 0 && force->group == NULL &&
            GameAnimSet_GetCurrentFrame(force->anim_set) > force->effect_scale && !ForceAnimationDonePc(force))
            playing_forwards = 1;
        force->animation_speed = ForceUpdateSpeedPc(force, user);
        if (user == NULL && !playing_forwards && (force->runtime_flags & 0x8000) == 0) {
            force->field_50 = 0.0f;
            if (!ForceAnimationDonePc(force)) {
                GizForce_PlayBackwards(force);
            } else if ((force->runtime_flags & 0x20) == 0 &&
                       ((force->config_flags & 2) != 0 || force->field_48 > 0.0f)) {
                volatile f32 remaining = (f64)force->field_48 - FRAMETIME;
                force->field_48 = remaining;
                if (remaining <= 0.0f)
                    force->field_48 = 0.0f;
                if (force->field_48 == 0.0f) {
                    if ((force->config_flags & 0x40) != 0 && (force->runtime_flags & 0x800) != 0) {
                        GameAnimSet_SetVisibility(force->anim_set, 1);
                        force->runtime_flags &= ~0x800u;
                    }
                    GizForce_PlayBackwards(force);
                    force->runtime_flags &= ~0x2000u;
                }
            }
        } else {
            force->runtime_flags &= ~0x4000u;
            playing_forwards = 1;
            GizForce_PlayForwards(force);
            if ((force->runtime_flags & 0x40) != 0) {
                force->field_48 = 0.0f;
                force->field_50 = 0.0f;
            } else {
                force->field_48 = force->force_strength;
                if (force->force_range > 0.0f && (force->runtime_flags & 0x2000) == 0 && force->anim_set->state == 2) {
                    volatile f32 elapsed;
                    started_shaking = force->field_50 == 0.0f;
                    elapsed = (f64)force->field_50 + FRAMETIME;
                    force->field_50 = elapsed;
                    if (elapsed >= force->force_range) {
                        force->runtime_flags |= 0x2000;
                        force->field_50 = 0.0f;
                    }
                } else {
                    force->field_50 = 0.0f;
                }
                if ((force->config_flags & 0x40) != 0 && (force->runtime_flags & 0x800) == 0 &&
                    ForceAnimationDonePc(force) &&
                    (force->force_range == 0.0f || (force->runtime_flags & 0x2000) != 0)) {
                    GameAnimSet_SetVisibility(force->anim_set, 0);
                    force->runtime_flags |= 0x800;
                }
            }
        }
        if (force->field_50 > 0.0f) {
            offset.x = (f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.05f - (f64)0.025f;
            offset.y = (force->runtime_flags & 0x20) != 0
                           ? GameAnimSet_GetCompletionRatio(force->anim_set) * force->group->combined_height
                           : 0.0f;
            offset.y = (f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.05f - (f64)0.025f + offset.y;
            offset.z = (f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.05f - (f64)0.025f;
            GameAnimSet_SetOffset(force->anim_set, &offset);
            force->runtime_flags |= 0x1000;
            if (force->start_sfx_id != -1) {
                if (started_shaking || IsSfxLooping(force->start_sfx_id))
                    GameAudio_PlaySfxById(force->start_sfx_id, &force->position, 0, 0);
            } else {
                PlaySfx("LegShakeL", &force->position);
            }
        }
        if ((force->anim_set->flags & 1) != 0 || was_moving || (force->anim_set->flags & 4) != 0) {
            updating_animation = 1;
            if (force->group != NULL) {
                if ((force->runtime_flags & 0x20) == 0)
                    ForceGroupPushPc(force);
                if ((force->runtime_flags & 0x20) != 0) {
                    force->group->runtime_flags |= 1;
                    offset.x = 0.0f;
                    offset.y = GameAnimSet_GetCompletionRatio(force->anim_set) * force->group->combined_height;
                    offset.z = 0.0f;
                    GameAnimSet_SetOffset(force->anim_set, &offset);
                }
            }
            ForceUpdateCentrePc(force, 2);
            if (was_moving) {
                if (ForceAnimationDonePc(force)) {
                    if (force->loop_sfx_id != -1)
                        GameAudio_PlaySfxById(force->loop_sfx_id, &force->position, 0, 0);
                } else if (force->anim_set->state != 4 && force->stop_sfx_id != -1) {
                    GameAudio_PlaySfxById(force->stop_sfx_id, &force->position, 0, 0);
                }
            } else if ((force->anim_set->flags & 5) != 0 && force->field_50 == 0.0f && playing_forwards) {
                if (force->start_sfx_id != -1) {
                    if (force->anim_set->state == 0 || IsSfxLooping(force->start_sfx_id))
                        GameAudio_PlaySfxById(force->start_sfx_id, &force->position, 0, 0);
                } else {
                    PlaySfx((char *)GameAudioName00758be8Pc, &force->position);
                }
            }
            if ((force->config_flags & 0x2000) == 0) {
                for (object = force->anim_set->objects; object != NULL; object = object->next) {
                    GIZFORCEANIMDATA_s *data = (GIZFORCEANIMDATA_s *)object->object_data;
                    if ((data->flags & 2) == 0 && data->platform_id != -1)
                        AddShoveObject(&object->special, data->platform_id);
                }
            }
        }
        if (force->anim_set->state == 2) {
            if ((force->runtime_flags & 0x20) != 0) {
                if (GizForce_Complete(force) && user == NULL)
                    force->runtime_flags |= 0x40;
            } else if (!(force->force_range > 0.0f && (force->runtime_flags & 0x2000) == 0) &&
                       !((force->config_flags & 0x40) != 0 && (force->runtime_flags & 0x800) == 0) &&
                       (force->anim_set->animated_object_count != 0 || force->force_range > 0.0f ||
                        (force->config_flags & 0x40) != 0) &&
                       (force->runtime_flags & 0x4000) == 0) {
                if (force->blowup_type != -1) {
                    if ((force->config_flags & 0x20) != 0) {
                        for (object = force->anim_set->objects; object != NULL; object = object->next) {
                            NUVEC *position = NuSpecialGetDrawPos(&object->special);
                            if (position != NULL)
                                GizmoBlowUpTypeBlowUp(world, force->blowup_type, position);
                        }
                    } else {
                        GizmoBlowUpTypeBlowUp(world, force->blowup_type, &force->position);
                    }
                    GameAnimSet_SetVisibility(force->anim_set, 0);
                    force->runtime_flags |= 0x10000;
                }
                if (force->pickup_count != 0 && (force->runtime_flags & 0x200) == 0) {
                    i32 hearts = ReleaseHearts();
                    NUVEC direction, position;
                    NuVecAdd(&position, &force->position, &force->effect_position);
                    NuVecRotateX(&direction, &v010, force->pickup_rotation_x);
                    NuVecRotateY(&direction, &direction, force->pickup_rotation_y);
                    AddPickups((u16)force->pickup_count, hearts, 0, 0, &position, &direction, 2.0f, -1,
                               force->activation_radius, LevelConfigFarDistance, NULL, 1, 0);
                    force->runtime_flags |= 0x200;
                }
                force->runtime_flags |= 0x4000;
            }
        } else if (!updating_animation && force->anim_set->state == 0 && (force->runtime_flags & 0x20) != 0) {
            ForceGroupPopPc(force);
            force->runtime_flags &= ~0x40u;
        }
        group = force->group;
        if (group != NULL) {
            if (force->anim_set->state == 2) {
                if (was_moving && group->configured_count == group->member_count) {
                    group->runtime_flags |= 2;
                    force->group->runtime_flags |= 4;
                    group = force->group;
                    for (group_index = 0; group_index < group->configured_count; ++group_index) {
                        i32 mask = (i8)group->forces[group_index]->collision_mask;
                        if (mask != 0 && (mask & (1u << (group_index & 31))) == 0) {
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
        if ((force->runtime_flags & 0x8000) != 0 && GizForce_Complete(force))
            force->runtime_flags &= ~0x8000u;
        if ((force->runtime_flags & 0x10000) == 0 && (force->config_flags & 0x300) != 0)
            system->hit_test_gizmos[system->hit_test_gizmo_count++] = gizmo;
        if ((i8)force->runtime_flags < 0 && !GizForce_Complete(force)) {
            if ((force->config_flags & 0x400) != 0) {
                for (object = force->anim_set->objects; object != NULL; object = object->next) {
                    GIZFORCEANIMDATA_s *data = (GIZFORCEANIMDATA_s *)object->object_data;
                    NUVEC centre;
                    volatile f32 radius = 0.0f;
                    NUVEC particle_offset;
                    if ((data->field_0x04 & 1) != 0 || !NuSpecialExistsFn(&object->special))
                        continue;
                    NuSpecialGetRadius(&object->special, &centre, (f32 *)&radius);
                    radius = (f64)force->horizontal_range * (f64)1.2f * radius;
                    NuVecMtxTransformVU0(&centre, &centre, NuSpecialGetDrawMtx(&object->special));
                    particle_offset.x = 0.0f;
                    particle_offset.y = radius != 0.0f ? (f64)qrand() / (65535.0 / ((f64)radius * 0.5) + 1.0) : 0.0f;
                    particle_offset.z = 0.0f;
                    NuVecRotateZ(&particle_offset, &particle_offset, qrand());
                    NuVecRotateY(&particle_offset, &particle_offset, (u16)qrand());
                    NuVecAdd(&centre, &centre, &particle_offset);
                    if ((force->runtime_flags & 0x20000) == 0)
                        AddVariableShotDebrisEffectTimed1(
                            WORLD->game_debris_sys->entries[(force->config_flags & 0x10) != 0 ? 110 : 109].effect,
                            &centre, (i32)((f64)radius * 90.0), FRAMETIME, 0, 0, NULL);
                }
            } else if ((force->runtime_flags & 0x20000) == 0) {
                NUVEC centre, particle_offset;
                volatile f32 radius = 0.0f;
                GameAnimSet_GetCentreAndRadius(force->anim_set, &centre, (f32 *)&radius, 2, 1, 1);
                radius = (f64)force->horizontal_range * (f64)1.2f * radius;
                particle_offset.x = 0.0f;
                particle_offset.y = radius != 0.0f ? (f64)qrand() / (65535.0 / ((f64)radius * 0.5) + 1.0) : 0.0f;
                particle_offset.z = 0.0f;
                NuVecRotateZ(&particle_offset, &particle_offset, qrand());
                NuVecRotateY(&particle_offset, &particle_offset, (u16)qrand());
                NuVecAdd(&centre, &centre, &particle_offset);
                AddVariableShotDebrisEffectTimed1(
                    WORLD->game_debris_sys->entries[(force->config_flags & 0x10) != 0 ? 110 : 109].effect, &centre,
                    (i32)((f64)radius * 90.0), FRAMETIME, 0, 0, NULL);
            }
        }
        force->runtime_flags &= ~0x20000u;
        if ((force->config_flags & 0x400) != 0)
            for (object = force->anim_set->objects; object != NULL; object = object->next)
                ((GIZFORCEANIMDATA_s *)object->object_data)->field_0x04 &= ~1u;
    }
}

/* Append to gizforce.c; canonical164B PC record/system344 view already owned. */
STATIC_ASSERT(sizeof(GIZFORCE_PC) == 0xa4);
STATIC_ASSERT(offsetof(GIZFORCE_PC, start_sfx_id) == 0x82);
STATIC_ASSERT(offsetof(GIZFORCE_PC, loop_sfx_id) == 0x84);
STATIC_ASSERT(offsetof(GIZFORCE_PC, stop_sfx_id) == 0x86);
STATIC_ASSERT(offsetof(GIZFORCESYS_s, count) == 0xe);
void __cdecl GizForces_AddLevelSfx(void *world, void *system_pointer, i32 *ids, i32 *count, i32 capacity) {
    GIZFORCESYS_s *system = (GIZFORCESYS_s *)system_pointer;
    GIZFORCE_PC *force;
    i32 index;
    (void)world;
    if (system == NULL)
        return;
    force = (GIZFORCE_PC *)system->forces;
    for (index = 0; index < system->count; ++index, ++force) {
        if (force->start_sfx_id != -1)
            AddLevelSfxFromId(force->start_sfx_id, ids, count, capacity);
        if (force->loop_sfx_id != -1)
            AddLevelSfxFromId(force->loop_sfx_id, ids, count, capacity);
        if (force->stop_sfx_id != -1)
            AddLevelSfxFromId(force->stop_sfx_id, ids, count, capacity);
    }
}

i32 __cdecl GizmoForce_GetOutput(GIZMO_PC *gizmo, i32 output_index, i32 force_read) {
    GIZFORCE_PC *force = (GIZFORCE_PC *)gizmo->object;
    if (((!(force->runtime_flags & 2) && force->blowup_type == -1) || !(force->runtime_flags & 1)) && force_read == 0)
        return 0;
    switch (output_index) {
        case 0:
            return force->anim_set != NULL && force->anim_set->state == 2;
        case 1:
            return force->anim_set != NULL && force->anim_set->state != 0;
        case 2:
            return force->anim_set != NULL && force->anim_set->state == 0;
        case 3:
            return force->group != NULL && (force->group->runtime_flags & 2) != 0;
        case 4:
            return force->group != NULL && (force->group->runtime_flags & 4) != 0;
        case 5:
            return (force->runtime_flags & 0x10000) != 0;
        case 6:
            return GizForce_Complete(force);
        case 7:
            return (force->runtime_flags & 0x200000) != 0;
        default:
            return 0;
    }
}
typedef struct force_runtime_bits_pc_s {
    u32 active : 1;
    u32 visible : 1;
    u32 other : 30;
} FORCE_RUNTIME_BITS_PC;
STATIC_ASSERT(sizeof(FORCE_RUNTIME_BITS_PC) == 4);
STATIC_ASSERT(offsetof(GIZFORCE_PC, runtime_flags) == 0xa0);

void __cdecl GizmoForce_Activate(GIZMO_PC *gizmo, i32 active) {
    GIZFORCE_PC *force;
    if (gizmo == NULL)
        return;
    force = (GIZFORCE_PC *)gizmo->object;
    if (active != 0 && !(force->runtime_flags & 1))
        GameAnimSet_JumpToStart(force->anim_set);
    ((FORCE_RUNTIME_BITS_PC *)&force->runtime_flags)->active = active != 0;
    if (active != 0) {
        force->field_48 = 0.0f;
        force->field_50 = 0.0f;
        force->runtime_flags &= 0xfffeffafu;
        if (force->config_flags & 8)
            force->runtime_flags &= ~0x200u;
        force->runtime_flags &= 0xffff87ffu;
        GameAnimSet_EvaluateState(force->anim_set);
        force->position = force->file_position;
        force->radius = 1.0f;
        GameAnimSet_GetCentreAndRadius(force->anim_set, &force->position, &force->radius, 2, 1, 1);
    }
}
void __cdecl GizmoForce_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    GIZFORCE_PC *force;
    if (gizmo == NULL)
        return;
    force = (GIZFORCE_PC *)gizmo->object;
    if (force == NULL)
        return;
    GameAnimSet_SetVisibility(force->anim_set, visible);
    ((FORCE_RUNTIME_BITS_PC *)&force->runtime_flags)->visible = visible != 0;
}

i32 __cdecl GizmoForce_ActivateRev(GIZMO_PC *gizmo, i32 reverse, i32 mode) {
    GIZFORCE_PC *force;
    if (gizmo == NULL)
        return 0;
    force = (GIZFORCE_PC *)gizmo->object;
    if (force == NULL)
        return 0;
    if ((mode & 1) != 0)
        return reverse != ((force->runtime_flags >> 4) & 1);
    if (reverse != 0) {
        GizForce_PlayBackwards(force);
        force->runtime_flags = (force->runtime_flags & ~1u) | 0x10;
    } else {
        GizForce_PlayForwards(force);
        force->runtime_flags = (force->runtime_flags & ~0x10u) | 1;
    }
    return 1;
}

typedef struct ai_force_platform_record_pc_s {
    u8 unknown_00[0x40];
    NUMTX *scene_matrix;
    u8 unknown_44[0x28];
} AI_FORCE_PLATFORM_RECORD_PC;
STATIC_ASSERT(sizeof(AI_FORCE_PLATFORM_RECORD_PC) == 0x6c);
STATIC_ASSERT(offsetof(AI_FORCE_PLATFORM_RECORD_PC, scene_matrix) == 0x40);
typedef struct ai_force_platform_terrain_pc_s {
    u8 unknown_00[0x68];
    AI_FORCE_PLATFORM_RECORD_PC *platforms;
} AI_FORCE_PLATFORM_TERRAIN_PC;
STATIC_ASSERT(offsetof(AI_FORCE_PLATFORM_TERRAIN_PC, platforms) == 0x68);
typedef struct ai_force_platform_object_data_pc_s {
    u8 unknown_00[2];
    i16 platform_id;
} AI_FORCE_PLATFORM_OBJECT_DATA_PC;
STATIC_ASSERT(offsetof(AI_FORCE_PLATFORM_OBJECT_DATA_PC, platform_id) == 2);

void *__cdecl Condition_OnForcePlatformInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    GIZMO_PC *gizmo = GizmoFindByName(WORLD->gizmo_sys, force_gizmotype_id, name);
    GIZFORCE_PC *force;
    if (gizmo != NULL) {
        force = (GIZFORCE_PC *)gizmo->object;
        if (force != NULL && (force->runtime_flags & 0x100) != 0)
            return force;
    }
    return NULL;
}

f32 __cdecl Condition_OnForcePlatformPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    GIZFORCE_PC *force = (GIZFORCE_PC *)argument;
    GAMEOBJECT *object;
    i16 platform_id;
    AI_FORCE_PLATFORM_TERRAIN_PC *terrain;
    GAMEANIMOBJ *animation;
    if (force == NULL || packet == NULL || packet->owner == NULL)
        return 0.0f;
    object = packet->owner->objptr;
    if (object->field_281 == 0 && object->field_282 == 0)
        return 0.0f;
    platform_id = object->supporting_platform_id;
    if (platform_id == -1)
        return 0.0f;
    terrain = (AI_FORCE_PLATFORM_TERRAIN_PC *)CurrentTerrain;
    if (!((f64)terrain->platforms[platform_id].scene_matrix->m31 <= (f64)object->position.y))
        return 0.0f;
    animation = force->anim_set->objects;
    while (animation != NULL) {
        if (platform_id == ((AI_FORCE_PLATFORM_OBJECT_DATA_PC *)animation->object_data)->platform_id)
            return 1.0f;
        animation = animation->next;
    }
    return 0.0f;
}

f32 __cdecl Condition_PlayerOnForcePlatformPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                              char *name, void *argument) {
    GIZFORCE_PC *force = (GIZFORCE_PC *)argument;
    GAMEOBJECT *object;
    i16 platform_id;
    AI_FORCE_PLATFORM_TERRAIN_PC *terrain;
    GAMEANIMOBJ *animation;
    if (force == NULL)
        return 0.0f;
    object = (GAMEOBJECT *)Player[0];
    if (object == NULL || (object->field_281 == 0 && object->field_282 == 0))
        return 0.0f;
    platform_id = object->supporting_platform_id;
    if (platform_id == -1)
        return 0.0f;
    terrain = (AI_FORCE_PLATFORM_TERRAIN_PC *)CurrentTerrain;
    if (!((f64)terrain->platforms[platform_id].scene_matrix->m31 <= (f64)object->position.y))
        return 0.0f;
    animation = force->anim_set->objects;
    while (animation != NULL) {
        if (platform_id == ((AI_FORCE_PLATFORM_OBJECT_DATA_PC *)animation->object_data)->platform_id)
            return 1.0f;
        animation = animation->next;
    }
    return 0.0f;
}

f32 __cdecl Condition_EitherPlayerOnForcePlatformPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                    char *name, void *argument) {
    GIZFORCE_PC *force = (GIZFORCE_PC *)argument;
    GAMEOBJECT *first;
    GAMEOBJECT *second;
    AI_FORCE_PLATFORM_TERRAIN_PC *terrain;
    i32 eligible = 0;
    i16 platform_id;
    GAMEANIMOBJ *animation;
    if (force == NULL)
        return 0.0f;
    first = (GAMEOBJECT *)Player[0];
    if (first == NULL)
        return 0.0f;
    terrain = (AI_FORCE_PLATFORM_TERRAIN_PC *)CurrentTerrain;
    if (first->field_281 != 0 || first->field_282 != 0) {
        platform_id = first->supporting_platform_id;
        if (platform_id != -1 && (f64)terrain->platforms[platform_id].scene_matrix->m31 <= (f64)first->position.y)
            eligible = 1;
    }
    second = (GAMEOBJECT *)Player[1];
    if (second != NULL && (second->field_281 != 0 || second->field_282 != 0)) {
        platform_id = second->supporting_platform_id;
        if (platform_id != -1 && (f64)terrain->platforms[platform_id].scene_matrix->m31 <= (f64)second->position.y)
            eligible |= 2;
    }
    if (eligible == 0)
        return 0.0f;
    animation = force->anim_set->objects;
    while (animation != NULL) {
        if ((eligible & 1) != 0 &&
            first->supporting_platform_id == ((AI_FORCE_PLATFORM_OBJECT_DATA_PC *)animation->object_data)->platform_id)
            return 1.0f;
        if ((eligible & 2) != 0 &&
            second->supporting_platform_id == ((AI_FORCE_PLATFORM_OBJECT_DATA_PC *)animation->object_data)->platform_id)
            return 1.0f;
        animation = animation->next;
    }
    return 0.0f;
}

STATIC_ASSERT(offsetof(GIZFORCESYS_s, hit_test_gizmos) == 8);
STATIC_ASSERT(offsetof(GIZFORCESYS_s, hit_test_gizmo_count) == 0x12);
STATIC_ASSERT(offsetof(GIZFORCEANIMDATA_s, platform_id) == 2);
STATIC_ASSERT(offsetof(BOLT_PC, owner) == 4);
STATIC_ASSERT(offsetof(BOLT_PC, hit_platform) == 0xfa);
STATIC_ASSERT(offsetof(BOLT_PC, type_id) == 0x101);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_280) == 0x280);
STATIC_ASSERT(offsetof(GAMEPAD_PC, bound_pad) == 0);

/* Retail uses private EDI FORCE and two stack arguments; all three callers
 * are rebuilt below with this ordinary C contract. */
static __declspec(noinline) i32 __cdecl GizForces_BoltHitPlat(GIZFORCE_PC *force, WORLDINFO *world, i32 player_index) {
    u32 flags = force->runtime_flags;
    u32 config;
    GAMEANIMOBJ *animation;
    GAMEOBJECT *player;
    NUVEC *position;
    if ((flags & 2) == 0 || (flags & 1) == 0 || (flags & 0x10000) != 0)
        return 0;
    config = force->config_flags;
    if ((config & 0x100) != 0) {
        force->runtime_flags = flags | 0x8000;
    } else if ((config & 0x200) != 0) {
        if (force->blowup_type != -1) {
            if ((config & 0x20) != 0) {
                if (force->anim_set != NULL) {
                    animation = force->anim_set->objects;
                    while (animation != NULL) {
                        position = NuSpecialGetDrawPos(&animation->special);
                        if (position != NULL)
                            GizmoBlowUpTypeBlowUp(world, force->blowup_type, position);
                        animation = animation->next;
                    }
                }
            } else {
                GizmoBlowUpTypeBlowUp(world, force->blowup_type, &force->position);
            }
        }
        GameAnimSet_JumpToEnd(force->anim_set);
        ForceUpdateCentrePc(force, 2);
        GameAnimSet_SetVisibility(force->anim_set, 0);
        force->runtime_flags |= 0x10000;
    } else {
        return 0;
    }
    if (player_index != -1) {
        player = (GAMEOBJECT *)Player[player_index];
        if ((player->flags_1fc_word & 0x80) != 0)
            NewBuzz(player->pad->bound_pad, 0.1f, 0);
    }
    return 1;
}

i32 __cdecl GizForces_BoltHitPlatPc(WORLDINFO *world, void *system_pointer, BOLT_PC *bolt, u8 *hit_flags) {
    GIZFORCESYS_s *system = (GIZFORCESYS_s *)system_pointer;
    GIZFORCE_PC *force;
    GAMEANIMOBJ *animation;
    GIZMO_PC **gizmos;
    i32 count;
    i32 index;
    if (system == NULL)
        return 0;
    count = system->hit_test_gizmo_count;
    if (count <= 0)
        return 0;
    gizmos = system->hit_test_gizmos;
    for (index = 0; index < count; ++index) {
        force = (GIZFORCE_PC *)gizmos[index]->object;
        if ((force->runtime_flags & 2) == 0 || (force->runtime_flags & 1) == 0 || (force->runtime_flags & 0x100) == 0 ||
            (force->runtime_flags & 0x10000) != 0)
            continue;
        animation = force->anim_set->objects;
        while (animation != NULL) {
            if (((GIZFORCEANIMDATA_s *)animation->object_data)->platform_id == bolt->hit_platform) {
                (void)BoltType_FindByID((u8)bolt->type_id, world);
                if (bolt->owner != NULL)
                    GizForces_BoltHitPlat(force, world, (i8)((GAMEOBJECT *)bolt->owner)->field_280);
                else
                    GizForces_BoltHitPlat(force, world, -1);
                return 1;
            }
            animation = animation->next;
        }
    }
    return 0;
}

i32 __cdecl GizForces_BoltHit(WORLDINFO *world, void *system_pointer, GAMEOBJECT *object, NUVEC *points,
                              i32 point_count, f32 radius, NUVEC *minimum, NUVEC *maximum, BOLT_PC *bolt, i32 hit_type,
                              u8 *hit_flags) {
    GIZFORCESYS_s *system = (GIZFORCESYS_s *)system_pointer;
    GIZFORCE_PC *force;
    GIZFORCE_PC *best = NULL;
    f32 best_distance = 1000000000.0f;
    volatile f32 distance_spill;
    f32 distance;
    NUVEC *origin;
    NUVEC normal;
    i32 index;
    i32 point;
    i32 player_index;
    if (system == NULL || system->hit_test_gizmo_count == 0)
        return 0;
    for (index = 0; index < system->hit_test_gizmo_count; ++index) {
        force = (GIZFORCE_PC *)system->hit_test_gizmos[index]->object;
        if ((force->runtime_flags & 2) == 0 || (force->runtime_flags & 1) == 0 || (force->runtime_flags & 0x10000) != 0)
            continue;
        if ((f64)force->position.x - (f64)force->radius > (f64)maximum->x)
            continue;
        if ((f64)force->radius + (f64)force->position.x < (f64)minimum->x)
            continue;
        if ((f64)force->position.z - (f64)force->radius > (f64)maximum->z)
            continue;
        if ((f64)force->position.z + (f64)force->radius < (f64)minimum->z)
            continue;
        if ((f64)force->position.y - (f64)force->radius > (f64)maximum->y)
            continue;
        if ((f64)force->radius + (f64)force->position.y < (f64)minimum->y)
            continue;
        point = (i32)((u32)point_count - 1u);
        while (point >= 0) {
            if (SphereSphereOverlap(&force->position, force->radius, &points[point], radius) != 0)
                break;
            --point;
        }
        if (point < 0)
            continue;
        origin = object != NULL ? &object->collision_position : &points[point];
        distance_spill = NuVecDistSqr(origin, &force->position, NULL);
        distance = distance_spill;
        if (best_distance > distance) {
            best_distance = distance;
            best = force;
        }
    }
    if (best == NULL)
        return 0;
    if (hit_type != 2 && hit_type != 7 && bolt != NULL) {
        (void)BoltType_FindByID((u8)bolt->type_id, WORLD);
        (void)Player_HasDoubleBoltDamage_FromBolt(bolt);
    }
    (void)BoltType_FindByID((u8)bolt->type_id, world);
    player_index = bolt->owner != NULL ? (i8)((GAMEOBJECT *)bolt->owner)->field_280 : -1;
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
    if (BoltSys->stop_targeting != NULL)
        BoltSys->stop_targeting(object, points);
    return 1;
}

GIZMO_PC *__cdecl GizForces_GetBestBoltTarget(GIZMOSET_PC *set, f32 *output_distance, NUVEC *output_position,
                                              NUVEC *output_velocity, void *object_pointer, NUVEC *origin,
                                              NUVEC *direction, f32 radius, f32 range_squared, i32 directional,
                                              i32 planar, i32 bolt_id) {
    BOLTTYPE_PC *type = BoltType_FindByID(bolt_id, WORLD);
    f32 near_distance = TargetDist_Near2;
    u16 near_angle = TargetDeg_Near;
    f32 mid_distance = TargetDist_Mid2;
    u16 far_angle = TargetDeg_Far;
    u16 mid_angle = TargetDeg_Mid;
    GIZFORCESYS_s *system;
    GAMEOBJECT *object = (GAMEOBJECT *)object_pointer;
    GIZMO_PC *gizmo;
    GIZFORCE_PC *force;
    GIZMO_PC *best = NULL;
    GIZMO_PC *previous = NULL;
    NUVEC *best_position = NULL;
    NUVEC *previous_position = NULL;
    f32 best_distance = 1000000000.0f;
    NUVEC direction_copy, delta;
    volatile f32 minimum_x, minimum_z, maximum_x, maximum_z;
    volatile f32 distance_spill, dot_spill, dx_spill, dz_spill;
    f32 distance, dot;
    u16 target_angle;
    NUANG angle;
    i32 index;
    if (set == NULL || set->data == NULL || type == NULL)
        return NULL;
    direction_copy = *direction;
    minimum_x = (f64)origin->x - (f64)radius;
    minimum_z = (f64)origin->z - (f64)radius;
    maximum_x = (f64)origin->x + (f64)radius;
    maximum_z = (f64)radius + (f64)origin->z;
    if (directional != 0 && (type->flags_60 & 0x20000) != 0) {
        direction_copy.y = 0.0f;
        NuVecNorm(&direction_copy, &direction_copy);
    }
    system = (GIZFORCESYS_s *)set->data;
    for (index = 0; index < system->hit_test_gizmo_count; ++index) {
        gizmo = system->hit_test_gizmos[index];
        force = (GIZFORCE_PC *)gizmo->object;
        if ((force->runtime_flags & 2) == 0 || (force->runtime_flags & 1) == 0 || (force->runtime_flags & 0x10000) != 0)
            continue;
        if (minimum_x > force->position.x || maximum_x < force->position.x || minimum_z > force->position.z ||
            maximum_z < force->position.z)
            continue;
        if (planar == 0)
            distance_spill = NuVecDistSqr(&force->position, origin, &delta);
        else
            distance_spill = NuVecXZDistSqr(&force->position, origin, &delta);
        distance = distance_spill;
        if (!(range_squared > distance))
            continue;
        if (directional == 0) {
            dz_spill = (f64)force->position.z - (f64)origin->z;
            dx_spill = (f64)force->position.x - (f64)origin->x;
            angle = NuAtan2D(dx_spill, dz_spill);
            NuVecRotateY(&delta, &v001, angle);
        }
        if ((type->flags_60 & 0x20000) != 0)
            delta.y = 0.0f;
        NuVecNorm(&delta, &delta);
        dot_spill = NuVecDot(&delta, &direction_copy);
        dot = dot_spill;
        if (near_distance > distance && directional != 0)
            target_angle = near_angle;
        else if (mid_distance > distance)
            target_angle = mid_angle;
        else
            target_angle = far_angle;
        if (!(NuTrigTable[(((i32)target_angle + 0x4000) >> 1) & 0x7fff] < dot))
            continue;
        if (!(best_distance > distance))
            continue;
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

STATIC_ASSERT(offsetof(WORLDINFO, gizmo_sys) == 0x2ac8);
STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);
STATIC_ASSERT(offsetof(GIZFORCE_PC, reset_word) == 0x3c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, runtime_flags) == 0xa0);
STATIC_ASSERT(offsetof(AIPACKET_PC, owner) == 0xd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, objptr) == 0);

void *__cdecl Condition_ForceAtStartInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return GizmoFindByName(WORLD->gizmo_sys, force_gizmotype_id, name);
}

f32 __cdecl Condition_ForceAtStartPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    if (argument != NULL && GizmoGetOutput(WORLD->gizmo_sys, argument, 1, 1) == 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_ForceAtEndPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    if (argument != NULL && GizmoGetOutput(WORLD->gizmo_sys, argument, 0, 1) != 0)
        return 1.0f;
    return 0.0f;
}

void *__cdecl Condition_PlayerUsingForceInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    GIZMO_PC *gizmo = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, force_gizmotype_id, name);
    return gizmo != NULL ? gizmo->object : NULL;
}

f32 __cdecl Condition_UsingForcePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    if (GizForce_GameObjUsingForce(packet->owner->objptr, (GIZFORCE_PC *)argument) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_PlayerUsingForcePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                         void *argument) {
    if (GizForce_GameObjUsingForce(player, (GIZFORCE_PC *)argument) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_ForceBeingUsedPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    GIZFORCE_PC *force = (GIZFORCE_PC *)argument;
    return force != NULL && ((force->runtime_flags & 0x200000) != 0 || force->reset_word != 0) ? 1.0f : 0.0f;
}

typedef struct force_throw_target_view_pc_s {
    u8 unknown_0000[0xd10];
    GAMEOBJECT *target;
} FORCE_THROW_TARGET_VIEW_PC;
STATIC_ASSERT(offsetof(FORCE_THROW_TARGET_VIEW_PC, target) == 0xd10);

PART_PC *__cdecl GizForce_Throw(GAMEOBJECT *object, GIZFORCE_PC *force, f32 speed, f32 gravity, i32 unused) {
    NUSPECIALHANDLE *special = &force->anim_set->objects->special;
    NUMTX translated;
    NUMTX *matrix;
    NUVEC velocity;
    ADDPART_PC params;
    PART_PC *part;
    (void)unused;
    if (WORLD->current_level == MAULF_LDATA) {
        NuMtxSetTranslation(&translated, &ForceThrowMaulPositionPc);
        matrix = &translated;
    } else {
        matrix = NuSpecialGetMtx(special);
    }
    if (object == NULL || ((FORCE_THROW_TARGET_VIEW_PC *)object)->target == NULL)
        return NULL;
    MakeThrowVector(&velocity, (NUVEC *)&matrix->m30,
                    &((FORCE_THROW_TARGET_VIEW_PC *)object)->target->collision_position, &v000, speed, gravity);
    params = Default_ADDPART;
    params.matrix = matrix;
    params.velocity = &velocity;
    params.field_18 = 0.15f;
    params.field_14 = 0.15f;
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
    PlaySfx((char *)ForcePushSoundNamePc, &object->collision_position);
    GameAnimSet_SetVisibility(force->anim_set, 0);
    force->runtime_flags |= 0x10000;
    return part;
}
