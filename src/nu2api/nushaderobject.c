#include "saga.h"

void __cdecl NuShaderObjectUnInit(PC_SHADER_OBJECT_FULL_VIEW *shader) {
    NuShaderObjectBaseUnInit(shader);
}

void __cdecl NuShaderObjectCreate(PC_SHADER_OBJECT_FULL_VIEW *shader) {
    NuShaderObjectBaseCreate(shader);
    shader->active_parameters = 0;
    shader->field_18 = 0;
}
void __cdecl NuShaderObjectDestroy(PC_SHADER_OBJECT_FULL_VIEW *shader) {
    NuShaderObjectBaseUnInit(shader);
    NuShaderObjectBaseDestroy(shader);
}

void __cdecl NuShaderObjectInit(PC_SHADER_OBJECT_FULL_VIEW *shader, const u32 *key, i32 index,
                                const char *vertex_source, i32 vertex_length, const char *pixel_source,
                                i32 pixel_length, i32 version) {
    NuShaderObjectBaseInit(shader, key, index);
    shader->version = version;
}
void __cdecl PCShaderProbeHLSLConstants(PC_SHADER_OBJECT_FULL_VIEW *shader, ID3DXConstantTable *vertex_constants,
                                        ID3DXConstantTable *pixel_constants) {
    i32 i;
    shader->active_parameters = NULL;
    for (i = 0; i < 100; ++i) {
        ShaderUniformRecordPc *uniform = &ShaderUniformRecordsPc[i];
        ID3DXConstantTable *table = vertex_constants;
        const char *handle = NULL;
        i32 stage = 1;
        PC_SHADER_PARAMETER_LOCAL_PC *parameter;
        D3DX_CONSTANT_DESCRIPTION_PC description;
        UINT description_count = 1;
        if (uniform->vertex_name != NULL && table != NULL)
            handle = table->methods->get_constant_by_name(table, NULL, uniform->vertex_name);
        if (handle != NULL && uniform->register_index >= 0)
            uniform->stage_mask |= stage;
        if (handle == NULL) {
            table = pixel_constants;
            stage = 2;
            if (uniform->fragment_name != NULL && table != NULL)
                handle = table->methods->get_constant_by_name(table, NULL, uniform->fragment_name);
            if (handle != NULL && uniform->register_index >= 0)
                uniform->stage_mask |= stage;
        }
        if (handle == NULL)
            continue;
        if (PCDeviceHardwareVertexProcessingPC == 4 && uniform->register_index >= 0)
            continue;
        parameter = PCShaderAllocateParameter(shader, i);
        parameter->metadata = (parameter->metadata & 0x0fffffff) | ((u32)stage << 28);
        table->methods->get_constant_description(table, handle, &description, &description_count);
        if (description.type_kind == 3) {
            if (description.class_kind == 0)
                parameter->metadata = (parameter->metadata & 0xf1ffffff) | 0x01000000;
            else if (description.class_kind == 1)
                parameter->metadata = (parameter->metadata & 0xf2ffffff) | 0x02000000;
            else if (description.class_kind == 2 || description.class_kind == 3)
                parameter->metadata = (parameter->metadata & 0xf3ffffff) | 0x03000000;
        } else if (description.type_kind >= 10 && description.type_kind <= 14)
            parameter->metadata = (parameter->metadata & 0xf4ffffff) | 0x04000000;
        parameter->register_index = (i16)description.register_index;
        parameter->register_count = (u8)description.register_count;
        parameter->array_count = (u8)description.elements;
        if ((uniform->stage_mask & 1) != 0 && description.type_kind >= 10 && description.type_kind <= 14)
            parameter->register_index = (i16)(description.register_index + 0x101);
        if ((PCDeviceHardwareVertexProcessingPC != 4 && uniform->field_18 != 0) || uniform->register_index < 0) {
            PC_SHADER_PARAMETER_LOCAL_PC *previous = shader->active_parameters;
            if (previous != NULL && previous->register_index < parameter->register_index) {
                while (previous->next != NULL && previous->next->register_index <= parameter->register_index)
                    previous = previous->next;
                parameter->next = previous->next;
                previous->next = parameter;
            } else {
                parameter->next = previous;
                shader->active_parameters = parameter;
            }
        }
    }
}
