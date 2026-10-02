#include "saga.h"
#include <string.h>

i32 __cdecl PCBuildModelLayerList(CHARACTER_MODEL_LAYERS_PC *model, i16 *indices, u32 mask) {
    i32 count = 0, index = 0;
    u32 bit = 1;
    if (model == NULL)
        return 0;
    while (index < 32 && index < model->hierarchy->render_count) {
        if ((mask & bit) != 0) {
            *indices++ = (i16)index;
            ++count;
        }
        ++index;
        bit += bit;
    }
    return count;
}

void __cdecl PCStoreModelLocatorCoordinates(CHARACTER_MODEL_LAYERS_PC *model, NUMTX *world_matrix,
                                            NUMTX *joint_matrices, NUVEC *positions, NUMTX *matrices) {
    i32 index;
    NUMTX result;
    if (positions == NULL && matrices == NULL)
        return;
    for (index = 0; index < 16; ++index) {
        if (model->points_of_interest[index] != NULL) {
            NuHGobjPOIMtx(model->hierarchy, (u8)index, world_matrix, joint_matrices, &result);
            if (positions != NULL) {
                memcpy(&positions[index], &result.m30, sizeof(NUVEC));
            }
            if (matrices != NULL)
                memcpy(&matrices[index], &result, sizeof(result));
        }
    }
}
