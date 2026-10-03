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
