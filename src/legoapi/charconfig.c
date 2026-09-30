#include "saga.h"

i32 __cdecl LayerFromName(PLAYERCHARACTERCONFIG_PC *character, const char *name) {
    i32 i;
    for (i = 0; i < character->layer_count; ++i) {
        if (NuStrICmp(name, character->layers[i].name) == 0) {
            return character->layers[i].mask_bit;
        }
    }
    return -1;
}

i32 __cdecl MakeLayerList_Name(CHARACTER_MODEL_LAYERS_PC *model, i16 *output, u32 mask) {
    PLAYERCHARACTERCONFIG_PC *character;
    i32 count;
    i32 bit;
    u32 flag;
    if (output == NULL || model == NULL) {
        return 0;
    }
    character = &GCDataList[model->model_id];
    count = 0;
    flag = 1;
    for (bit = 0; bit < 32; ++bit, flag <<= 1) {
        i32 layer;
        i16 hierarchy_layer;
        if ((mask & flag) == 0 || bit >= character->layer_count) {
            continue;
        }
        if (character->layer_lookup == NULL) {
            for (layer = 0; layer < character->layer_count; ++layer) {
                if (character->layers[layer].mask_bit == bit) {
                    break;
                }
            }
            if (layer == character->layer_count) {
                continue;
            }
        } else {
            layer = character->layer_lookup[bit];
        }
        if (layer == -1) {
            continue;
        }
        hierarchy_layer = character->layers[layer].hierarchy_layer_index;
        if (hierarchy_layer != -1) {
            ++count;
            *output++ = hierarchy_layer;
        }
    }
    return count;
}
