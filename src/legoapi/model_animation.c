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

void __cdecl EvalModelAnim(CHARACTER_MODEL_LAYERS_PC *model, ANIMPACKET_PC *packet, NUMTX *world_matrix,
                           NUMTX *joint_matrices, DWA_WEIGHT_BLOCK_PC ***dwa_output, NUVEC *locator_positions,
                           NUMTX *locator_matrices, u32 layer_mask) {
    i16 render_indices[32];
    i32 render_count = ModelAnimLayerListDispatchPc(model, render_indices, layer_mask);
    if (render_count < 1) {
        return;
    }
    if (dwa_output != NULL) {
        if (packet->blending != 0 && packet->blend_source_animation >= 0 &&
            packet->blend_source_animation < apicharsys->model_id_capacity &&
            model->model_data_c[packet->blend_source_animation] != NULL && packet->blend_target_animation >= 0 &&
            packet->blend_target_animation < apicharsys->model_id_capacity &&
            model->model_data_c[packet->blend_target_animation] != NULL) {
            volatile f32 fraction = packet->blend_elapsed_pc / packet->blend_duration_pc;
            *dwa_output =
                NuHGobjEvalDwaBlend2(render_count, render_indices, model->model_data_c[packet->blend_source_animation],
                                     packet->blend_source_time, model->model_data_c[packet->blend_target_animation],
                                     packet->blend_target_time, fraction);
        } else if (packet->blending != 0 && packet->blend_target_animation >= 0 &&
                   packet->blend_target_animation < apicharsys->model_id_capacity &&
                   model->model_data_c[packet->blend_target_animation] != NULL) {
            *dwa_output =
                NuHGobjEvalDwa2(render_count, render_indices, model->model_data_c[packet->blend_target_animation],
                                packet->blend_target_time);
        } else if (packet->blending != 0 && packet->blend_source_animation >= 0 &&
                   packet->blend_source_animation < apicharsys->model_id_capacity &&
                   model->model_data_c[packet->blend_source_animation] != NULL) {
            *dwa_output =
                NuHGobjEvalDwa2(render_count, render_indices, model->model_data_c[packet->blend_source_animation],
                                packet->blend_source_time);
        } else if (packet->blending == 0 && packet->animation_index >= 0 &&
                   packet->animation_index < apicharsys->model_id_capacity &&
                   model->model_data_c[packet->animation_index] != NULL) {
            *dwa_output = NuHGobjEvalDwa2(render_count, render_indices, model->model_data_c[packet->animation_index],
                                          packet->current_time);
        } else {
            *dwa_output = NULL;
        }
    }
    if (packet->blending != 0 && packet->blend_source_animation >= 0 &&
        packet->blend_source_animation < apicharsys->model_id_capacity &&
        model->model_data_b[packet->blend_source_animation] != NULL && packet->blend_target_animation >= 0 &&
        packet->blend_target_animation < apicharsys->model_id_capacity &&
        model->model_data_b[packet->blend_target_animation] != NULL) {
        volatile f32 fraction = packet->blend_elapsed_pc / packet->blend_duration_pc;
        NuHGobjEvalAnimBlend2(model->hierarchy, model->model_data_b[packet->blend_source_animation],
                              packet->blend_source_time, model->model_data_b[packet->blend_target_animation],
                              packet->blend_target_time, fraction, 0, NULL, joint_matrices);
    } else if (packet->blending != 0 && packet->blend_target_animation >= 0 &&
               packet->blend_target_animation < apicharsys->model_id_capacity &&
               model->model_data_b[packet->blend_target_animation] != NULL) {
        NuHGobjEvalAnim2(model->hierarchy, model->model_data_b[packet->blend_target_animation],
                         packet->blend_target_time, 0, NULL, joint_matrices);
    } else if (packet->blending != 0 && packet->blend_source_animation >= 0 &&
               packet->blend_source_animation < apicharsys->model_id_capacity &&
               model->model_data_b[packet->blend_source_animation] != NULL) {
        NuHGobjEvalAnim2(model->hierarchy, model->model_data_b[packet->blend_source_animation],
                         packet->blend_source_time, 0, NULL, joint_matrices);
    } else if (packet->blending == 0 && packet->animation_index >= 0 &&
               packet->animation_index < apicharsys->model_id_capacity &&
               model->model_data_b[packet->animation_index] != NULL) {
        NuHGobjEvalAnim2(model->hierarchy, model->model_data_b[packet->animation_index], packet->current_time, 0, NULL,
                         joint_matrices);
    } else {
        NuHGobjEval(model->hierarchy, 0, NULL, joint_matrices);
    }
    PCStoreModelLocatorCoordinates(model, world_matrix, joint_matrices, locator_positions, locator_matrices);
}
