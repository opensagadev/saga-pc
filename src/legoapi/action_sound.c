#include "saga.h"

i32 __cdecl Action_PlaySfx(void *sys, void *processor, AIPACKET_PC *packet, char **params, i32 param_count,
                           i32 first_time, f32 delta_time) {
    NUVEC position;
    NUVEC *position_ptr;
    char *name;
    char play_count;
    f32 repeat_delay;
    f32 start_delay;
    i32 index;

    (void)packet;
    (void)delta_time;
    if (first_time == 0 || param_count <= 0)
        return 1;

    position.x = 1.0e9f;
    position.y = 1.0e9f;
    position.z = 1.0e9f;
    position_ptr = NULL;
    name = NULL;
    play_count = 1;
    repeat_delay = 0.0f;
    start_delay = 0.0f;

    for (index = 0; index < param_count; ++index) {
        char *value;

        value = NuStrIStr(params[index], (char *)AIScriptActionNameArgPrefix);
        if (value != NULL) {
            name = value + 5;
            continue;
        }
        value = NuStrIStr(params[index], (char *)PlayCountArgPrefix);
        if (value != NULL) {
            play_count = (char)AIParamToFloat((AISCRIPTPROCESS_PC *)processor, value + 10);
            continue;
        }
        value = NuStrIStr(params[index], (char *)RepeatDelayArgPrefix);
        if (value != NULL) {
            repeat_delay = AIParamToFloat((AISCRIPTPROCESS_PC *)processor, value + 9);
            continue;
        }
        value = NuStrIStr(params[index], (char *)StartDelayArgPrefix);
        if (value != NULL) {
            start_delay = AIParamToFloat((AISCRIPTPROCESS_PC *)processor, value + 11);
            continue;
        }
        value = NuStrIStr(params[index], (char *)XArgPrefix);
        if (value != NULL) {
            position.x = AIParamToFloat((AISCRIPTPROCESS_PC *)processor, value + 2);
            continue;
        }
        value = NuStrIStr(params[index], (char *)YArgPrefix);
        if (value != NULL) {
            position.y = AIParamToFloat((AISCRIPTPROCESS_PC *)processor, value + 2);
            continue;
        }
        value = NuStrIStr(params[index], (char *)ZArgPrefix);
        if (value != NULL) {
            position.z = AIParamToFloat((AISCRIPTPROCESS_PC *)processor, value + 2);
            continue;
        }
        value = NuStrIStr(params[index], (char *)CharacterPosArgPrefix);
        if (value != NULL) {
            GAMEOBJECT *object;
            object = GetNamedGameObject(sys, value + 14);
            position_ptr = &object->collision_position;
        }
    }

    if (position_ptr == NULL && (position.x != 1.0e9f || position.y != 1.0e9f || position.z != 1.0e9f)) {
        position_ptr = &position;
    }
    if (name != NULL) {
        PlayRepeatSfx(name, -1, start_delay, (i32)play_count, repeat_delay, position_ptr);
    }
    return 1;
}
