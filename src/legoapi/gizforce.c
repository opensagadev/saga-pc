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
