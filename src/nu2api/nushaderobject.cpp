#include "nu2api/PCInput.h"
#include "nu2api/nu3d/ShaderManager.h"
#include "nu2api/nu3d/nushader.h"
#include "nu2api/nu3d/nushader_internal.h"
#include "nu2api/nu3d/nushaderobject.h"
#include "nu2api/nu3d/nushaderobjectbase.h"
#include "nu2api/nu3d/shaderSemantics.h"
#include "nu2api/nuwin32/d3dCalls.h"
#include "editor/edlevelall.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/nu3d/nushadermanager_gen.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "gamelib/nuwind/nuwind.h"
#include "nu2api/nu3d/nupostfilter.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/NuEffectTex_pc.h"
#include "nu2api/nutex_pc.h"
#include "nu2api/nucore/nuapi.h"

extern "C" const f32 PCSamplerBiasMinimum;

/* Original caller/helper cohort required; these are logical signatures only.
 * Physical register ABI remains a compiler proof hold until the real dispatcher
 * is compiled in its original TU. No public stack adapters are authorized.
 */
static void *PCShaderMaterialTextureNative(NUMTL *material, u8 slot) {
    i32 id = material->shader.diffuse_map_ids[slot];
    if (id != 0) {
        return NuTextureGetDimensions(id)->resource;
    }
    return NULL;
}

void PC_SHADER_PARAMETER_LOCAL::SetScalarValues(i32 first_element, i32 count, const f32 *values) {
    if ((metadata & 0xf0000000U) == 0x10000000U) {
        PCDeviceSetVertexShaderConstantsF(register_index + first_element, values, count);
    } else {
        PCDeviceSetPixelShaderConstantsF(register_index + first_element, values, count);
    }
}

void PC_SHADER_PARAMETER_LOCAL::SetVectorValues(i32 first_element, i32 count, const f32 *values) {
    if ((metadata & 0xf0000000U) == 0x10000000U) {
        PCDeviceSetVertexShaderConstantsF(register_index + first_element, values, count);
    } else {
        PCDeviceSetPixelShaderConstantsF(register_index + first_element, values, count);
    }
}

void PC_SHADER_PARAMETER_LOCAL::SetMatrixValues(i32 first_element, i32 count, const f32 *values) {
    NUMTX transposed[32];
    for (i32 i = 0; i < count; ++i) {
        NuMtxTranspose(&transposed[i], (NUMTX *)(values + i * 16));
    }
    i32 vector_count = (i8)register_count - first_element * 4;
    if (count * 4 < vector_count) {
        vector_count = count * 4;
    }
    if ((metadata & 0xf0000000U) == 0x10000000U) {
        PCDeviceSetVertexShaderConstantsF(register_index + first_element * 4, (const f32 *)transposed, vector_count);
    } else {
        PCDeviceSetPixelShaderConstantsF(register_index + first_element * 4, (const f32 *)transposed, vector_count);
    }
}

i32 PC_SHADER_PARAMETER_LOCAL::GetRegisterIndex() {
    return register_index;
}

PC_SHADER_PARAMETER_LOCAL *PC_SHADER_PARAMETER_LOCAL::GetNext() {
    return next;
}

void PC_SHADER_PARAMETER_LOCAL::SetNext(PC_SHADER_PARAMETER_LOCAL *parameter) {
    next = parameter;
}

PC_SHADER_PARAMETER_LOCAL *PC_SHADER_PARAMETER_LOCAL::fun_007421b0() {
    return this;
}

extern "C" i32 NuShaderObjectGetHardwareVertexProcessing(void) {
    return PCDeviceHardwareVertexProcessingPC;
}

static f32 *PCShaderUnpackColour(f32 *output, i32 colour) {
    output[0] = (colour & 255) / 255.0f;
    output[1] = ((colour >> 8) & 255) / 255.0f;
    output[2] = ((colour >> 16) & 255) / 255.0f;
    output[3] = ((colour >> 24) & 255) / 255.0f;
    return output;
}

extern "C" i32 fun_00742230(i32 value) {
    return value;
}

extern "C" u32 PCShaderSamplerBiasBits(f32 bias) {
    f32 value = PCSamplerBiasMinimum > bias ? PCSamplerBiasMinimum : bias > 15.96875 ? 15.96875f : bias;
    return *reinterpret_cast<u32 *>(&value);
}

static void PCShaderSamplerWrap(i32 u_mode, u32 stage, i32 v_mode) {
    switch (u_mode) {
        case 1:
            PCDeviceSetSamplerState(stage, D3DSAMP_ADDRESSU, 3);
            break;
        case 2:
            PCDeviceSetSamplerState(stage, D3DSAMP_ADDRESSU, 2);
            break;
        case 3:
            PCDeviceSetSamplerState(stage, D3DSAMP_ADDRESSU, 4);
            break;
        default:
            PCDeviceSetSamplerState(stage, D3DSAMP_ADDRESSU, 1);
            break;
    }
    switch (v_mode) {
        case 1:
            PCDeviceSetSamplerState(stage, D3DSAMP_ADDRESSV, 3);
            break;
        case 2:
            PCDeviceSetSamplerState(stage, D3DSAMP_ADDRESSV, 2);
            break;
        case 3:
            PCDeviceSetSamplerState(stage, D3DSAMP_ADDRESSV, 4);
            break;
        default:
            PCDeviceSetSamplerState(stage, D3DSAMP_ADDRESSV, 1);
            break;
    }
}

/* Looks a uniform up in one constant table and marks the stage that provides it. */
static const char *PCShaderFindConstant(ShaderUniformRecord *uniform, ID3DXConstantTable *table, const char *name, i32 stage) {
    const char *handle;
    if (name != NULL && table != NULL) {
        handle = table->methods->get_constant_by_name(table, NULL, name);
    } else {
        handle = NULL;
    }
    if (handle != NULL && uniform->register_index >= 0) {
        uniform->stage_mask |= stage;
    }
    return handle;
}

void NuShaderObjectCreate(NuShaderObject *shader) {
    NuShaderObjectBaseCreate(shader);
    shader->active_parameters = 0;
    shader->compiled_state = 0;
}

void NuShaderObjectInit(NuShaderObject *shader, const u32 *key, i32 index, const char *vertex_source, i32 vertex_length,
                        const char *pixel_source, i32 pixel_length, i32 version) {
    NuShaderObjectBaseInit(shader, key, index);
    shader->version = version;
}

void NuShaderObjectUnInit(NuShaderObject *shader) {
    NuShaderObjectBaseUnInit(shader);
}

void PC_SHADER_PARAMETER_LOCAL::SetScalar(f32 value) {
    if ((metadata & 0xf0000000U) == 0x10000000U) {
        PCDeviceSetVertexShaderConstantsF(register_index, &value, 1);
    } else {
        PCDeviceSetPixelShaderConstantsF(register_index, &value, 1);
    }
}

void PC_SHADER_PARAMETER_LOCAL::SetValues(i32 first_element, i32 count, const f32 *values) {
    switch ((i32)(metadata << 4) >> 28) {
        case 1:
            SetScalarValues(first_element, count, values);
            break;
        case 2:
            SetVectorValues(first_element, count, values);
            break;
        case 3:
            SetMatrixValues(first_element, count, values);
            break;
        case 4:
            return;
    }
}

void PC_SHADER_PARAMETER_LOCAL::SetVector(const f32 *values) {
    if ((metadata & 0xf0000000U) == 0x10000000U) {
        PCDeviceSetVertexShaderConstantsF(register_index, values, 1);
    } else {
        PCDeviceSetPixelShaderConstantsF(register_index, values, 1);
    }
}

void PC_SHADER_PARAMETER_LOCAL::SetVectorElement(i32 index, const f32 *values) {
    if ((metadata & 0xf0000000U) == 0x10000000U) {
        PCDeviceSetVertexShaderConstantsF(register_index + index, values, 1);
    } else {
        PCDeviceSetPixelShaderConstantsF(register_index + index, values, 1);
    }
}

void PC_SHADER_PARAMETER_LOCAL::SetMatrixValue(i32 first_element, const f32 *values) {
    SetMatrixValues(first_element, 1, values);
}

extern "C" void NushaderobjectNuError(void) {
}

void PCShaderProbeHLSLConstants(NuShaderObject *shader, ID3DXConstantTable *vertex_constants, ID3DXConstantTable *pixel_constants) {
    i32 i;
    shader->active_parameters = NULL;
    for (i = 0; i < 100; ++i) {
        ShaderUniformRecord *uniform = &ShaderUniformRecords[i];
        ID3DXConstantTable *table = vertex_constants;
        const char *handle;
        i32 stage = 1;
        PC_SHADER_PARAMETER_LOCAL *parameter;
        D3DX_CONSTANT_DESCRIPTION description;
        UINT description_count;
        handle = PCShaderFindConstant(uniform, table, uniform->vertex_name, stage);
        if (handle == NULL) {
            table = pixel_constants;
            stage = 2;
            handle = PCShaderFindConstant(uniform, table, uniform->fragment_name, stage);
        }
        if (handle == NULL) {
            continue;
        }
        description_count = 1;
        if (PCDeviceHardwareVertexProcessingPC == PC_SHADER_MODEL_3_0 && uniform->register_index >= 0) {
            continue;
        }
        parameter = PCShaderAllocateParameter(shader, i);
        parameter->metadata = (parameter->metadata & 0x0fffffff) | ((u32)stage << 28);
        table->methods->get_constant_description(table, handle, &description, &description_count);
        if (description.type_kind == D3DXPT_FLOAT) {
            if (description.class_kind == D3DXPC_SCALAR) {
                parameter->metadata = (parameter->metadata & 0xf1ffffff) | 0x01000000;
            } else if (description.class_kind == D3DXPC_VECTOR) {
                parameter->metadata = (parameter->metadata & 0xf2ffffff) | 0x02000000;
            } else if (description.class_kind == D3DXPC_MATRIX_COLUMNS || description.class_kind == D3DXPC_MATRIX_ROWS) {
                parameter->metadata = (parameter->metadata & 0xf3ffffff) | 0x03000000;
            }
        } else if (description.type_kind == D3DXPT_SAMPLER || description.type_kind == D3DXPT_SAMPLER1D ||
                   description.type_kind == D3DXPT_SAMPLER2D || description.type_kind == D3DXPT_SAMPLER3D ||
                   description.type_kind == D3DXPT_SAMPLERCUBE) {
            parameter->metadata = (parameter->metadata & 0xf4ffffff) | 0x04000000;
        }
        parameter->register_index = (i16)description.register_index;
        parameter->register_count = (u8)description.register_count;
        parameter->array_count = (u8)description.elements;
        if ((uniform->stage_mask & 1) != 0 && (description.type_kind == D3DXPT_SAMPLER || description.type_kind == D3DXPT_SAMPLER1D ||
                                               description.type_kind == D3DXPT_SAMPLER2D || description.type_kind == D3DXPT_SAMPLER3D ||
                                               description.type_kind == D3DXPT_SAMPLERCUBE)) {
            parameter->register_index = (i16)(description.register_index + 0x101);
        }
        if ((PCDeviceHardwareVertexProcessingPC != PC_SHADER_MODEL_3_0 && uniform->binding_scope != 0) || uniform->register_index < 0) {
            PC_SHADER_PARAMETER_LOCAL *previous = shader->active_parameters;
            if (previous != NULL && parameter->register_index > previous->register_index) {
                while (previous->next != NULL && previous->next->register_index <= parameter->register_index) {
                    previous = previous->next;
                }
                parameter->next = previous->next;
                previous->next = parameter;
            } else {
                parameter->next = previous;
                shader->active_parameters = parameter;
            }
        }
    }
}

void NuShaderObjectDestroy(NuShaderObject *shader) {
    NuShaderObjectBaseUnInit(shader);
    NuShaderObjectBaseDestroy(shader);
}

void PC_SHADER_PARAMETER_LOCAL::SetElementValues(i32 index, const f32 *values) {
    switch ((i32)(metadata << 4) >> 28) {
        case 1:
            SetValues(index, 1, values);
            break;
        case 2:
            SetVectorElement(index, values);
            break;
        case 3:
            SetMatrixValues(index, 1, values);
            break;
        case 4:
            return;
    }
}

void PC_SHADER_PARAMETER_LOCAL::SetFloat2(const f32 *values) {
    if ((metadata & 0xf0000000U) == 0x10000000U) {
        PCDeviceSetVertexShaderConstantsF(register_index, values, 1);
    } else {
        PCDeviceSetPixelShaderConstantsF(register_index, values, 1);
    }
}

void PC_SHADER_PARAMETER_LOCAL::SetFloat3(const f32 *values) {
    if ((metadata & 0xf0000000U) == 0x10000000U) {
        PCDeviceSetVertexShaderConstantsF(register_index, values, 1);
    } else {
        PCDeviceSetPixelShaderConstantsF(register_index, values, 1);
    }
}

extern "C" void PCShaderSetMaterialParameters(NuShaderObject *shader, NUMTL *original_material) {
    NUMTL *material = original_material;
    PC_SHADER_PARAMETER_LOCAL *parameter = shader->active_parameters;
    NUVEC4 vector_2;
    NUVEC4 vector_10;
    NUVEC4 vector_11;
    NUVEC4 vector_16;
    NUVEC4 vector_83;
    NUVEC4 vector_51;
    NUVEC4 palette[4];
    NUVEC4 vector_91;
    NUVEC4 vector_92;
    NUVEC4 vector_12;
    NUVEC4 vector_13;
    NUVEC4 vector_80_0;
    NUVEC4 vector_80_1;
    NUVEC4 vector_52;
    NUVEC4 vector_53;
    NUVEC4 vector_54;
    NUVEC4 vector_57;
    NUVEC4 vector_55;
    NUVEC4 vector_58;
    NUVEC4 diffuse_vector;
    NUVEC4 vector_59;
    NUVEC4 vector_72;
    NUVEC4 vector_87;
    while (parameter != NULL) {
        const i32 semantic = (i8)parameter->semantic;
        switch (semantic) {
            case SHADER_UNIFORM_AMBIENT_COLOR: {
                parameter->SetVector(PCShaderUnpackColour(&diffuse_vector.x, material->shader.diffuse_colour));
                break;
            }
            case SHADER_UNIFORM_INCANDESCENT_GLOW: {
                const f32 *values = PCShaderUnpackColour(&vector_2.x, material->shader.specular_colour);
                vector_2.w = material->shader.parameter_100;
                parameter->SetVector(values);
                break;
            }
            case SHADER_UNIFORM_BITANGENT_FLIP: {
                const f32 sign = material->shader.flip_sign == 0 ? 1.0f : -1.0f;
                parameter->SetScalar(sign);
                break;
            }
            case SHADER_UNIFORM_SURFACE_PARAMS: {
                vector_10.x = material->shader.parameter_080;
                vector_10.y = 1.0f;
                vector_10.z = (f64)0.035f * material->shader.parameter_084;
                vector_10.w = material->shader.parameter_098;
                parameter->SetVector(&vector_10.x);
                break;
            }
            case SHADER_UNIFORM_SURFACE_PARAMS2: {
                vector_11.x = material->shader.texture_lerp;
                vector_11.y = material->shader.parameter_1d0 / material->shader.parameter_1c0;
                vector_11.z = 0.0f;
                vector_11.w = 0.0f;
                parameter->SetVector(&vector_11.x);
                break;
            }
            case SHADER_UNIFORM_VTF_K_NORMAL: {
                parameter->SetScalar(material->shader.parameter_064);
                break;
            }
            case SHADER_UNIFORM_GOOCH_PARAMS: {
                vector_16.x = material->shader.parameter_104;
                vector_16.y = material->shader.parameter_108;
                vector_16.z = 0.0f;
                vector_16.w = 0.0f;
                parameter->SetVector(&vector_16.x);
                break;
            }
            case SHADER_UNIFORM_CARPAINT_PARAMS: {
                vector_83.x = material->shader.parameter_1ac;
                vector_83.y = material->shader.parameter_1b0;
                vector_83.z = 0.0f;
                vector_83.w = 0.0f;
                parameter->SetVector(&vector_83.x);
                break;
            }
            case SHADER_UNIFORM_CARPAINT_TINTS: {
                for (i32 i = 0; i < 4; ++i) {
                    NUVEC4 vector_84;
                    const f32 *unpacked = PCShaderUnpackColour(&vector_84.x, material->shader.palette_colours[i]);
                    palette[i] = *reinterpret_cast<const NUVEC4 *>(unpacked);
                }
                if ((parameter->metadata & 0xf0000000u) == 0x10000000u) {
                    PCDeviceSetVertexShaderConstantsF(parameter->register_index, &palette[0].x, 4);
                } else {
                    PCDeviceSetPixelShaderConstantsF(parameter->register_index, &palette[0].x, 4);
                }
                break;
            }
            case SHADER_UNIFORM_EDGELIT_PARAMS: {
                vector_91.x = 1.0 / material->shader.parameter_1dc;
                vector_91.y = material->shader.parameter_1d4;
                vector_91.z = material->shader.parameter_1d8;
                vector_91.w = material->shader.parameter_1e0;
                parameter->SetVector(&vector_91.x);
                break;
            }
            case SHADER_UNIFORM_FRACTAL_PARAMS: {
                vector_92.x = (f64)0.1f * material->shader.parameter_1c0;
                vector_92.y = material->shader.parameter_1c8;
                vector_92.z = material->shader.parameter_1cc;
                vector_92.w = material->shader.parameter_1c4;
                parameter->SetVector(&vector_92.x);
                break;
            }
            case SHADER_UNIFORM_VTF_K_HEIGHT: {
                parameter->SetScalar(material->shader.parameter_060);
                break;
            }
            case SHADER_UNIFORM_SPECULAR_PARAMS: {
                vector_12.x = NuShaderManagerGetShininessFactor() * material->shader.parameter_07c;
                vector_12.y = material->shader.parameter_078;
                vector_12.z = material->shader.parameter_090;
                vector_12.w = material->shader.parameter_094;
                parameter->SetVector(&vector_12.x);
                break;
            }
            case SHADER_UNIFORM_ENVMAP_PARAMS: {
                vector_13.x = material->shader.parameter_08c;
                vector_13.y = material->shader.shininess;
                vector_13.z = material->shader.parameter_194;
                vector_13.w = 0.0f;
                parameter->SetVector(&vector_13.x);
                break;
            }
            case SHADER_UNIFORM_FX_ATTRIBUTES: {
                if ((material->shader.layer_flags & 0x8000u) && material->transform_scale == 0.0f) {
                    vector_80_0.x = 1.0f;
                    vector_80_0.y = material->effect_scale;
                    vector_80_0.z = material->effect_factor;
                    vector_80_0.w = (material->effect_scale * (f64)material->effect_factor) * (f64)0.2f;
                    parameter->SetVector(&vector_80_0.x);
                } else {
                    vector_80_1.x = material->transform_scale;
                    vector_80_1.y = material->effect_scale;
                    vector_80_1.z = material->effect_factor;
                    vector_80_1.w = (material->effect_scale * (f64)material->effect_factor) * (f64)0.2f;
                    parameter->SetVector(&vector_80_1.x);
                }
                break;
            }
            case SHADER_UNIFORM_WATER_TABLE: {
                NuShaderObjectBaseUpdateWaterTable(shader, material);
                break;
            }
            case SHADER_UNIFORM_LAYER0_SAMPLER: {
                const i32 stage = parameter->register_index;
                PCDeviceSetTexture(stage, PCShaderMaterialTextureNative(material, 0));
                PCShaderSamplerWrap((material->attribs.word >> 8) & 3, parameter->register_index, (material->attribs.word >> 10) & 3);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MAGFILTER, 2);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MINFILTER, 2);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MIPFILTER, 2);
                const f32 bias = material->shader.texture_bias[0] + PCShaderMaterialSamplerBiasOffset;
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MIPMAPLODBIAS, PCShaderSamplerBiasBits(bias));
                break;
            }
            case SHADER_UNIFORM_LAYER1_SAMPLER: {
                const i32 stage = parameter->register_index;
                PCDeviceSetTexture(stage, PCShaderMaterialTextureNative(material, 1));
                PCShaderSamplerWrap((material->attribs.word >> 8) & 3, parameter->register_index, (material->attribs.word >> 10) & 3);
                const f32 bias = material->shader.texture_bias[1] + PCShaderMaterialSamplerBiasOffset;
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MIPMAPLODBIAS, PCShaderSamplerBiasBits(bias));
                break;
            }
            case SHADER_UNIFORM_LAYER2_SAMPLER: {
                const i32 stage = parameter->register_index;
                PCDeviceSetTexture(stage, PCShaderMaterialTextureNative(material, 2));
                PCShaderSamplerWrap((material->attribs.word >> 8) & 3, parameter->register_index, (material->attribs.word >> 10) & 3);
                const f32 bias = material->shader.texture_bias[2] + PCShaderMaterialSamplerBiasOffset;
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MIPMAPLODBIAS, PCShaderSamplerBiasBits(bias));
                break;
            }
            case SHADER_UNIFORM_LAYER3_SAMPLER: {
                const i32 stage = parameter->register_index;
                PCDeviceSetTexture(stage, PCShaderMaterialTextureNative(material, 3));
                PCShaderSamplerWrap((material->attribs.word >> 8) & 3, parameter->register_index, (material->attribs.word >> 10) & 3);
                const f32 bias = material->shader.texture_bias[3] + PCShaderMaterialSamplerBiasOffset;
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MIPMAPLODBIAS, PCShaderSamplerBiasBits(bias));
                break;
            }
            case SHADER_UNIFORM_SPECULAR_SAMPLER: {
                const i32 id = material->shader.specular_map_tid;
                void *texture;
                if (id != 0) {
                    texture = NuTextureGetDimensions(id)->resource;
                } else {
                    texture = NULL;
                }
                PCDeviceSetTexture(parameter->register_index, texture);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSU, 1);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSV, 1);
                const f32 bias = material->shader.texture_bias[4] + PCShaderMaterialSamplerBiasOffset;
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MIPMAPLODBIAS, PCShaderSamplerBiasBits(bias));
                break;
            }
            case SHADER_UNIFORM_SPECULAR2_SAMPLER: {
                const i32 id = material->shader.lightmap_tids[0];
                void *texture;
                if (id != 0) {
                    texture = NuTextureGetDimensions(id)->resource;
                } else {
                    texture = NULL;
                }
                PCDeviceSetTexture(parameter->register_index, texture);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSU, 1);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSV, 1);
                const f32 bias = material->shader.parameter_0fc + PCShaderMaterialSamplerBiasOffset;
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MIPMAPLODBIAS, PCShaderSamplerBiasBits(bias));
                break;
            }
            case SHADER_UNIFORM_SURFACE_SAMPLER: {
                const i32 id = material->shader.normal_map_tid;
                void *texture;
                if (id != 0) {
                    texture = NuTextureGetDimensions(id)->resource;
                } else {
                    texture = NULL;
                }
                PCDeviceSetTexture(parameter->register_index, texture);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSU, 1);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSV, 1);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MAGFILTER, 2);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MINFILTER, 2);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MIPFILTER, 2);
                const f32 bias = material->shader.texture_bias[5] + PCShaderMaterialSamplerBiasOffset;
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MIPMAPLODBIAS, PCShaderSamplerBiasBits(bias));
                break;
            }
            case SHADER_UNIFORM_SURFACE2_SAMPLER: {
                const i32 id = material->shader.lightmap_tids[1];
                void *texture;
                if (id != 0) {
                    texture = NuTextureGetDimensions(id)->resource;
                } else {
                    texture = NULL;
                }
                PCDeviceSetTexture(parameter->register_index, texture);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSU, 1);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSV, 1);
                const f32 bias = material->shader.parameter_0f8 + PCShaderMaterialSamplerBiasOffset;
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MIPMAPLODBIAS, PCShaderSamplerBiasBits(bias));
                break;
            }
            case SHADER_UNIFORM_VERTEX_FETCH_SAMPLER: {
                const i32 id = material->shader.vtf_height_map_tid;
                void *texture;
                if (id != 0) {
                    texture = NuTextureGetDimensions(id)->resource;
                } else {
                    texture = NULL;
                }
                PCDeviceSetTexture(parameter->register_index, texture);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSU, 1);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSV, 1);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MAGFILTER, 1);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MINFILTER, 1);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MIPFILTER, 0);
                break;
            }
            case SHADER_UNIFORM_VTF_NORMAL_SAMPLER: {
                const i32 id = material->shader.vtf_normal_map_tid;
                void *texture;
                if (id != 0) {
                    texture = NuTextureGetDimensions(id)->resource;
                } else {
                    texture = NULL;
                }
                PCDeviceSetTexture(parameter->register_index, texture);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSU, 1);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSV, 1);
                const f32 bias = material->shader.parameter_0f4 + PCShaderMaterialSamplerBiasOffset;
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MIPMAPLODBIAS, PCShaderSamplerBiasBits(bias));
                break;
            }
            case SHADER_UNIFORM_DIFFENVMAP_SAMPLER_CUBE: {
                const i32 id = material->shader.clamp_map_tid;
                void *texture;
                if (id != 0) {
                    texture = NuTextureGetDimensions(id)->resource;
                } else {
                    texture = NULL;
                }
                PCDeviceSetTexture(parameter->register_index, texture);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSU, 3);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSV, 3);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSW, 3);
                break;
            }
            case SHADER_UNIFORM_ENVMAP_SAMPLER_CUBE: {
                PC_TEXTURE_RESOURCE *texture;
                if (PCDeviceHardwareVertexProcessingPC == PC_SHADER_MODEL_1_1 &&
                    ((material->shader.flags & 8u) != 0 || (material->shader.flags & 0x50000u) != 0)) {
                    texture = NuTexturePhongCube;
                } else if (PCDeviceHardwareVertexProcessingPC != PC_SHADER_MODEL_1_1 && (material->shader.flags & 0x50000u) != 0) {
                    texture = NuTextureEnvironmentCube;
                } else {
                    texture =
                        material->shader.envmap_cubic_tid > 0 ? NuTextureGetDimensions(material->shader.envmap_cubic_tid)->resource : NULL;
                }
                PCDeviceSetTexture(parameter->register_index, texture);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSU, 3);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSV, 3);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSW, 3);
                break;
            }
            case SHADER_UNIFORM_PS2_SHINEMAP_SAMPLER: {
                const i32 id = material->shader.shine_map_tid;
                void *texture;
                if (id != 0) {
                    texture = NuTextureGetDimensions(id)->resource;
                } else {
                    texture = NULL;
                }
                PCDeviceSetTexture(parameter->register_index, texture);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSU, 3);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSV, 3);
                break;
            }
            case SHADER_UNIFORM_LAYER0_DIFFUSE: {
                parameter->SetVector(PCShaderUnpackColour(&vector_51.x, material->shader.diffuse_colours[0]));
                break;
            }
            case SHADER_UNIFORM_LAYER1_DIFFUSE: {
                parameter->SetVector(PCShaderUnpackColour(&vector_52.x, material->shader.diffuse_colours[1]));
                break;
            }
            case SHADER_UNIFORM_LAYER2_DIFFUSE: {
                parameter->SetVector(PCShaderUnpackColour(&vector_53.x, material->shader.diffuse_colours[2]));
                break;
            }
            case SHADER_UNIFORM_LAYER3_DIFFUSE: {
                parameter->SetVector(PCShaderUnpackColour(&vector_54.x, material->shader.diffuse_colours[3]));
                break;
            }
            case SHADER_UNIFORM_LAYER_K_OPACITIES: {
                vector_55.x = material->shader.default_scale;
                vector_55.y = material->shader.parameter_028;
                vector_55.z = material->shader.parameter_02c;
                vector_55.w = material->shader.parameter_030;
                parameter->SetVector(&vector_55.x);
                break;
            }
            case SHADER_UNIFORM_SPECULAR_SPECULAR: {
                parameter->SetVector(PCShaderUnpackColour(&vector_57.x, material->shader.shadow_colour));
                break;
            }
            case SHADER_UNIFORM_SPECULAR2_SPECULAR: {
                parameter->SetVector(PCShaderUnpackColour(&vector_58.x, material->shader.reflection_colour));
                break;
            }
            case SHADER_UNIFORM_REFRACTION_COLOR: {
                const f32 *values = PCShaderUnpackColour(&vector_59.x, material->shader.tint_colour);
                vector_59.w = material->shader.parameter_0a4;
                parameter->SetVector(values);
                break;
            }
            case SHADER_UNIFORM_REFRACTION_K_INDEX: {
                const f32 value = (material->shader.parameter_09c - 1.0) * (f64)0.1f;
                parameter->SetScalar(value);
                break;
            }
            case SHADER_UNIFORM_BACK_BUFFER_SAMPLER: {
                GRAPHICS_RENDER_TEXTURE *texture = NuPostEffectGetBackBuffer(nuapi.frame_count);
                PCGraphicsBindEffectTexture(texture, parameter->register_index);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSU, 3);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSV, 3);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MIPFILTER, 0);
                break;
            }
            case SHADER_UNIFORM_UV_OFFSET0: {
                parameter->SetFloat2(material->shader.animation_offsets[0]);
                break;
            }
            case SHADER_UNIFORM_UV_OFFSET1: {
                parameter->SetFloat2(material->shader.animation_offsets[1]);
                break;
            }
            case SHADER_UNIFORM_UV_OFFSET2: {
                parameter->SetFloat2(material->shader.animation_offsets[2]);
                break;
            }
            case SHADER_UNIFORM_UV_OFFSET3: {
                parameter->SetFloat2(material->shader.animation_offsets[3]);
                break;
            }
            case SHADER_UNIFORM_LEGO_PARAMS: {
                vector_72.x = 0.05f;
                vector_72.y = material->transform_scale * (f64)0.32f;
                vector_72.z = 0.2f;
                vector_72.w = 0.8f;
                parameter->SetFloat3(&vector_72.x);
                break;
            }
            case SHADER_UNIFORM_WIND_SAMPLER: {
                if (NuWindCurrent(nuapi.wind) >= 0) {
                    const i32 id = NuWindCurrent(nuapi.wind);
                    PCGraphicsBindTextureDimensions(NuTextureGetDimensions(id), parameter->register_index);
                    PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSU, 1);
                    PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSV, 1);
                    PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSW, 1);
                    PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MIPFILTER, 0);
                    PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MIPMAPLODBIAS, 0);
                }
                break;
            }
            case SHADER_UNIFORM_WIND_PARAMS: {
                NUWIND *wind = nuapi.wind;
                if (wind != NULL) {
                    const f32 parameter24 = wind->parameter_24;
                    const i32 angle0 = (i32)(wind->accumulated_time * 10430.3779296875);
                    const f32 component0 = NuTrigTable[(angle0 >> 1) & 0x7fff] / 10.0;
                    const i32 angle1 = (i32)(wind->scaled_accumulated_time * 10430.3779296875);
                    const f32 component1 = NuTrigTable[(angle1 >> 1) & 0x7fff] / 30.0;
                    vector_87.x = wind->phase;
                    vector_87.y = parameter24;
                    vector_87.z = component0;
                    vector_87.w = component1;
                    parameter->SetFloat3(&vector_87.x);
                }
                break;
            }
            case SHADER_UNIFORM_TEX_ANIM_MAP_SAMPLER: {
                const i32 id = material->shader.extra_texture;
                void *texture;
                if (id != 0) {
                    texture = NuTextureGetDimensions(id)->resource;
                } else {
                    texture = NULL;
                }
                PCDeviceSetTexture(parameter->register_index, texture);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSU, 1);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSV, 1);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MAGFILTER, 1);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MINFILTER, 1);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MIPFILTER, 0);
                break;
            }
            case SHADER_UNIFORM_TEX_ANIM_CURVES_SAMPLER: {
                const i32 id = material->shader.environment_texture;
                void *texture;
                if (id != 0) {
                    texture = NuTextureGetDimensions(id)->resource;
                } else {
                    texture = NULL;
                }
                PCDeviceSetTexture(parameter->register_index, texture);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSU, 1);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_ADDRESSV, 1);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MAGFILTER, 0);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MINFILTER, 0);
                PCDeviceSetSamplerState(parameter->register_index, D3DSAMP_MIPFILTER, 0);
                break;
            }
            case SHADER_UNIFORM_DAPPLE_LIMIT: {
                parameter->SetScalar(material->shader.parameter_0a0);
                break;
            }
        }
        if (shader->version != 4) {
            ShaderUniformRecord *uniform = &ShaderUniformRecords[semantic];
            if (uniform->binding_scope != 0) {
                switch ((i32)(parameter->metadata << 4) >> 28) {
                    case 1:
                        parameter->SetValues(0, 1, uniform->values);
                        break;
                    case 2:
                        parameter->SetVectorElement(0, uniform->values);
                        break;
                    case 3:
                        parameter->SetMatrixValues(0, 1, uniform->values);
                        break;
                    case 4:
                        break;
                }
            }
        }
        parameter = parameter->next;
    }
}

extern "C" void fun_00743970(void) {
}
