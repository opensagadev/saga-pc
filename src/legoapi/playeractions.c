#include "saga.h"
i32 __cdecl NewBlockAction(GAMEOBJECT *object) {
    API_ACTION_INFO_PC *info = (API_ACTION_INFO_PC *)PlayerBlockActionInfoPc;
    i32 actions[3];
    i32 count = 0;
    i32 action;
    for (action = 0; action < apicharsys->model_id_capacity && count < 3; ++action)
        if ((info[action].flags & 8) != 0 && object->character_model->model_data_b[action] != NULL)
            actions[count++] = action;
    if (count == 0)
        return 0;
    action = actions[0];
    if (count != 1) {
        do {
            action = actions[qrand() / (0xffff / count + 1)];
        } while (action == object->previous_block_animation);
    }
    object->previous_block_animation = (i16)action;
    object->field_7aa = (i16)action;
    return 1;
}
