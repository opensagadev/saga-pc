#include "saga.h"
extern const char AIGameCharacterName[11];
extern const char AISetLayerKeywordPc[11];
extern const char AIClearLayerKeywordPc[13];
extern const char AIOverrideAllKeywordPc[9];

i32 __cdecl Action_SetLayer(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char **params,
                            i32 count, i32 first_time, f32 delta_time) {
    GAMEOBJECT *object = NULL;
    APIOBJECT_PC *api_object;
    i32 index, layer;
    char *value;
    u32 set_layers = 0;
    u32 clear_layers = 0;
    if (first_time == 0)
        return 1;
    if (packet != NULL && packet->owner != NULL && packet->owner->objptr != NULL)
        object = packet->owner->objptr;
    for (index = 0; index < count; ++index) {
        value = NuStrIStr(params[index], (char *)AIGameCharacterName);
        if (value != NULL) {
            if (GetNamedGameObjectCallback != NULL) {
                api_object = GetNamedGameObjectCallback(system, value + 10);
                object = api_object != NULL ? api_object->owner : NULL;
            } else {
                object = NULL;
            }
            continue;
        }
        value = NuStrIStr(params[index], (char *)AISetLayerKeywordPc);
        if (value != NULL) {
            layer = (i32)AIParamToFloat(process, value + 10);
            if ((u32)(layer - 1) <= 31)
                set_layers |= 1u << (layer - 1);
            continue;
        }
        value = NuStrIStr(params[index], (char *)AIClearLayerKeywordPc);
        if (value != NULL) {
            layer = (i32)AIParamToFloat(process, value + 12);
            if ((u32)(layer - 1) <= 31)
                clear_layers |= 1u << (layer - 1);
        }
    }
    if (object != NULL)
        object->extra_layer_mask = (object->extra_layer_mask | set_layers) & ~clear_layers;
    return 1;
}

i32 __cdecl Action_OverrideAnimation(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char **params,
                                     i32 count, i32 first_time, f32 delta_time) {
    i32 from = -1;
    i32 to = -1;
    i32 index;
    char *value;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL || first_time == 0)
        return 1;
    for (index = 0; index < count; ++index) {
        if (NuStrICmp(params[index], AIOverrideAllKeywordPc) == 0) {
            from = (u16)apicharsys->model_id_capacity;
            continue;
        }
        value = NuStrIStr(params[index], (char *)"from=");
        if (value != NULL) {
            from = (u16)FindAnimIX(packet->owner->character_data, value + 5);
            continue;
        }
        value = NuStrIStr(params[index], (char *)"to=");
        if (value != NULL) {
            to = (u16)FindAnimIX(packet->owner->character_data, value + 3);
            continue;
        }
        if (process != NULL)
            process->action_timer = AIParamToFloatEx(packet, process, params[0]);
    }
    if ((i16)to == -1)
        from = -1;
    packet->animation_override_from = (i16)from;
    packet->animation_override_to = (i16)to;
    return 1;
}
