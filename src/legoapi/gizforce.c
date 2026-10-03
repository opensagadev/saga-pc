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
