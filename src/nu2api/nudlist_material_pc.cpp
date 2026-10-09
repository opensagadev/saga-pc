#include "saga.h"
typedef struct PC_DL_MATERIAL_READER {
    u8 unknown_0000[0x40];
    u32 reflection_flags;
} PC_DL_MATERIAL_READER;
STATIC_ASSERT(offsetof(PC_DL_MATERIAL_READER, reflection_flags) == 0x40);
typedef struct PC_DL_FACE_ON_READER {
    NUMTX matrix;
    f32 scale_factor;
    NUMTX second_matrix;
} PC_DL_FACE_ON_READER;
STATIC_ASSERT(offsetof(PC_DL_FACE_ON_READER, matrix) == 0);
STATIC_ASSERT(offsetof(PC_DL_FACE_ON_READER, scale_factor) == 0x40);
STATIC_ASSERT(offsetof(PC_DL_FACE_ON_READER, second_matrix) == 0x44);
STATIC_ASSERT(sizeof(PC_DL_FACE_ON_READER) == 0x84);
STATIC_ASSERT(offsetof(NUMTX, m23) == 0x2c);
STATIC_ASSERT(offsetof(NUMTX, m30) == 0x30);
STATIC_ASSERT(offsetof(NUMTX, m31) == 0x34);
STATIC_ASSERT(offsetof(NUMTX, m32) == 0x38);
STATIC_ASSERT(offsetof(NUMTX, m33) == 0x3c);

/* Native private helper takes mode in EAX. Ordinary same-owner source only. */
static void PCDisplayHandlerSetZMode(i32 mode) {
    NuDisplayHandlerZModePc = mode;
    switch (mode) {
        case 0:
            PCDeviceSetRenderState(7, 1);
            PCDeviceSetRenderState(0xe, 1);
            PCDeviceSetRenderState(0x17, 4);
            break;
        case 1:
            PCDeviceSetRenderState(7, 1);
            PCDeviceSetRenderState(0xe, 0);
            PCDeviceSetRenderState(0x17, 4);
            break;
        case 2:
            PCDeviceSetRenderState(7, 1);
            PCDeviceSetRenderState(0xe, 1);
            PCDeviceSetRenderState(0x17, 8);
            break;
        case 3:
            PCDeviceSetRenderState(7, 0);
            break;
    }
}

extern "C" void __cdecl NuDisplayListTransformPc(void *arg) {
    NUMTX *matrix = (NUMTX *)arg;
    NUVEC4 tint = NuDisplayHandlerTintPc;
    f32 opacity = matrix->m33;
    f32 shadow = matrix->m23;
    if (opacity != NuDisplayHandlerOpacityPc) {
        if (opacity < 1.0f) {
            tint.w *= opacity;
            NuDisplayHandlerZModePc = 1;
            PCDeviceSetRenderState(7, 1);
            PCDeviceSetRenderState(0xe, 0);
            PCDeviceSetRenderState(0x17, 4);
            NuShaderManagerSetfv(0x38, &tint.x);
        } else {
            PC_DL_MATERIAL_READER *material = (PC_DL_MATERIAL_READER *)NuDisplayHandlerMaterialPc;
            PCDisplayHandlerSetZMode((material->reflection_flags >> 14) & 3);
            NuShaderManagerSetfv(0x38, &tint.x);
        }
        if (PCDeviceHardwareVertexProcessingPC == 4)
            NuDisplayHandlerOpacityPc = opacity;
    }
    if ((f64)shadow != (f64)PCGraphicsFrameState) {
        NuDisplayHandlerShadowPc = shadow;
        NuShaderManagerSetfv(0x63, &NuDisplayHandlerShadowPc);
        PCGraphicsFrameState = (i32)shadow;
    }
    matrix->m33 = 1.0f;
    matrix->m23 = 0.0f;
    NuShaderManagerSetfv(0x11, &matrix->m00);
    matrix->m33 = opacity;
    matrix->m23 = shadow;
}

extern "C" void __cdecl NuDisplayListTransformTransposePc(void *arg) {
    NUMTX *matrix = (NUMTX *)arg;
    NUVEC4 tint = NuDisplayHandlerTintPc;
    f32 opacity = matrix->m33;
    f32 shadow = matrix->m32;
    if (opacity != NuDisplayHandlerOpacityPc) {
        if (opacity < 1.0f) {
            tint.w *= opacity;
            NuDisplayHandlerZModePc = 1;
            PCDeviceSetRenderState(7, 1);
            PCDeviceSetRenderState(0xe, 0);
            PCDeviceSetRenderState(0x17, 4);
            NuShaderManagerSetfv(0x38, &tint.x);
        } else {
            PC_DL_MATERIAL_READER *material = (PC_DL_MATERIAL_READER *)NuDisplayHandlerMaterialPc;
            if (material != NULL) {
                PCDisplayHandlerSetZMode((material->reflection_flags >> 14) & 3);
                NuShaderManagerSetfv(0x38, &tint.x);
            }
        }
        if (PCDeviceHardwareVertexProcessingPC == 4)
            NuDisplayHandlerOpacityPc = opacity;
    }
    if ((f64)shadow != (f64)PCGraphicsFrameState) {
        NuDisplayHandlerShadowPc = shadow;
        NuShaderManagerSetfv(0x63, &NuDisplayHandlerShadowPc);
        PCGraphicsFrameState = (i32)shadow;
    }
    matrix->m33 = 1.0f;
    matrix->m32 = 0.0f;
    NuShaderManagerSetElementsfv_transpose(0x11, 0, 1, &matrix->m00);
    matrix->m33 = opacity;
    matrix->m32 = shadow;
}

extern "C" void __cdecl NuDisplayListFaceOnTransformPc(void *arg) {
    PC_DL_FACE_ON_READER *packet = (PC_DL_FACE_ON_READER *)arg;
    NUVEC4 position, scale, tint;
    f32 opacity;
    position.x = packet->matrix.m30;
    position.y = packet->matrix.m31;
    position.z = packet->matrix.m32;
    position.w = 1.0f;
    packet->matrix.m30 = 0.0f;
    packet->matrix.m31 = 0.0f;
    packet->matrix.m32 = 0.0f;
    opacity = packet->matrix.m33;
    packet->matrix.m33 = 1.0f;
    NuMtxTranspose(&packet->matrix, &packet->matrix);
    PCDeviceSetVertexShaderConstantsF(0x50, &packet->matrix.m00, 4);
    PCDeviceSetVertexShaderConstantsF(0x59, &position.x, 1);
    scale.x = packet->scale_factor;
    scale.y = 1.0f;
    scale.z = 0.0f;
    scale.w = 0.0f;
    PCDeviceSetVertexShaderConstantsF(0x54, &scale.x, 1);
    NuMtxTranspose(&packet->second_matrix, &packet->second_matrix);
    PCDeviceSetVertexShaderConstantsF(0x55, &packet->second_matrix.m00, 4);
    tint = NuDisplayHandlerTintPc;
    if (opacity < 1.0f) {
        tint.w *= opacity;
        NuDisplayHandlerZModePc = 1;
        PCDeviceSetRenderState(7, 1);
        PCDeviceSetRenderState(0xe, 0);
        PCDeviceSetRenderState(0x17, 4);
        /* Native count is 4 registers despite only four initialized tint floats. */
        PCDeviceSetVertexShaderConstantsF(0x28, &tint.x, 4);
    } else {
        PC_DL_MATERIAL_READER *material = (PC_DL_MATERIAL_READER *)NuDisplayHandlerMaterialPc;
        PCDisplayHandlerSetZMode((material->reflection_flags >> 14) & 3);
        PCDeviceSetVertexShaderConstantsF(0x28, &tint.x, 4);
    }
}

/* Minimum reader only; no material allocation, pointee or full extent claim. */
typedef struct PC_DL_E_MATERIAL_READER {
    NUMTL common;
    u8 unknown_009c[0x1c];
    i32 texture_id;
    u8 unknown_00bc[0x134];
    u32 variant_flags;
    i16 shader_id;
    u8 unknown_01f6[0x72];
    u8 face_on_variant;
    u8 unknown_0269[0x53];
    PC_TEXTURE_RESOURCE_PC *vertex_declaration;
} PC_DL_E_MATERIAL_READER;
STATIC_ASSERT(sizeof(NUMTL) == 0x9c);
STATIC_ASSERT(offsetof(PC_DL_E_MATERIAL_READER, common) == 0);
STATIC_ASSERT(offsetof(PC_DL_E_MATERIAL_READER, texture_id) == 0xb8);
STATIC_ASSERT(offsetof(PC_DL_E_MATERIAL_READER, variant_flags) == 0x1f0);
STATIC_ASSERT(offsetof(PC_DL_E_MATERIAL_READER, shader_id) == 0x1f4);
STATIC_ASSERT(offsetof(PC_DL_E_MATERIAL_READER, face_on_variant) == 0x268);
STATIC_ASSERT(offsetof(PC_DL_E_MATERIAL_READER, vertex_declaration) == 0x2bc);
STATIC_ASSERT(sizeof(PC_DL_E_MATERIAL_READER) == 0x2c0);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, parameters) + 0x4f * sizeof(PC_SHADER_PARAMETER_LOCAL_PC) == 0x3d0);

/* New C++ material owner, not a replacement or migration of nudlist_pc.c. */
struct PC_DL_SHADER_PROGRAM_PAIR_PC {
    PC_TEXTURE_RESOURCE_PC *vertex_shader, *pixel_shader;
};
STATIC_ASSERT(sizeof(PC_DL_SHADER_PROGRAM_PAIR_PC) == 8);
STATIC_ASSERT(offsetof(PC_DL_SHADER_PROGRAM_PAIR_PC, vertex_shader) == 0);
STATIC_ASSERT(offsetof(PC_DL_SHADER_PROGRAM_PAIR_PC, pixel_shader) == 4);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, textures) + sizeof(GRAPHICS_RENDER_TEXTURE_PC *) == 0x6b8);
STATIC_ASSERT(offsetof(ShaderUniformRecordPc, values) == 0x1c);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, unknown_4d4) == 0x4d4);

extern "C" {
    extern void *NuDisplayMaterialLastPc;
    extern PC_TEXTURE_RESOURCE_PC *NuDisplayHandlerVertexShaderCachePc, *NuDisplayHandlerPixelShaderCachePc;
    extern PC_DL_SHADER_PROGRAM_PAIR_PC *NuDisplayMaterialDebrisProgramsPc, *NuDisplayMaterialGlassProgramsPc;
    extern PC_DL_SHADER_PROGRAM_PAIR_PC *NuDisplayMaterialFaceOnDecalProgramsPc, *NuDisplayMaterialFaceOnProgramsPc;
    extern PC_DL_SHADER_PROGRAM_PAIR_PC *NuDisplayMaterialDefaultProgramsPc;
    extern PC_TEXTURE_RESOURCE_PC *NuDisplayMaterialDebrisDeclarationPc, *NuDisplayMaterialFaceOnDeclarationPc;
    extern PC_TEXTURE_RESOURCE_PC *NuDisplayMaterialDefaultDeclarationPc;
    extern i32 NuDisplayMaterialDistortTexturePc;
    extern i32 PCGraphicsFrameState, PCGraphicsBoundShader;
    void __cdecl PCDeviceSetVertexShaderPc(PC_TEXTURE_RESOURCE_PC *);
    void __cdecl PCDeviceSetPixelShaderPc(PC_TEXTURE_RESOURCE_PC *);
    void __cdecl PCDeviceSetVertexDeclarationPc(PC_TEXTURE_RESOURCE_PC *);
    void __cdecl NuShaderManagerBindShaderPc(PC_SHADER_OBJECT_FULL_VIEW *);
    void __cdecl NuMtlSetRenderStatesPS(NUMTL *);
    GRAPHICS_RENDER_TEXTURE_PC *__cdecl NuPostEffectGetBackBuffer(i32);
}

/* Genuine separate provider remains held until the full4472 dispatcher closes.
   Name/signature is a conditional prototype; no implementation is fabricated. */
extern "C" void __cdecl PCShaderSetMaterialParametersPc(PC_SHADER_OBJECT_FULL_VIEW *, NUMTL *);
static void PCDisplayHandlerBindProgramsPc(PC_DL_SHADER_PROGRAM_PAIR_PC *);

static void PCDisplayHandlerBindProgramsPc(PC_DL_SHADER_PROGRAM_PAIR_PC *programs) {
    if (programs != NULL) {
        if (NuDisplayHandlerVertexShaderCachePc != programs->vertex_shader) {
            PCDeviceSetVertexShaderPc(programs->vertex_shader);
            NuDisplayHandlerVertexShaderCachePc = programs->vertex_shader;
        }
        if (NuDisplayHandlerPixelShaderCachePc != programs->pixel_shader) {
            PCDeviceSetPixelShaderPc(programs->pixel_shader);
            NuDisplayHandlerPixelShaderCachePc = programs->pixel_shader;
        }
    } else {
        PCDeviceSetVertexShaderPc(NULL);
        PCDeviceSetPixelShaderPc(NULL);
        NuDisplayHandlerVertexShaderCachePc = NULL;
        NuDisplayHandlerPixelShaderCachePc = NULL;
    }
}

extern "C" void __cdecl NuDisplayListMaterialPc(void *arg) {
    PC_DL_E_MATERIAL_READER *material = (PC_DL_E_MATERIAL_READER *)arg;
    PC_SHADER_OBJECT_FULL_VIEW *shader =
        (PC_SHADER_OBJECT_FULL_VIEW *)PCShaderRecordFromHandle((u32)(i32)material->shader_id);
    NUMTX transposed;
    NuDisplayMaterialLastPc = material;
    NuDisplayHandlerMaterialPc = material;
    PCGraphicsBoundShader = -1;
    PCGraphicsFrameState = -1;
    if ((material->variant_flags & 0x100000) != 0) {
        PCDeviceSetVertexShaderPc(NULL);
        PCDeviceSetPixelShaderPc(NULL);
        NuDisplayHandlerVertexShaderCachePc = NULL;
        NuDisplayHandlerPixelShaderCachePc = NULL;
        NuShaderManagerBindShaderPc(NULL);
        PCDeviceSetVertexDeclarationPc(NuDisplayMaterialDebrisDeclarationPc);
        PCDisplayHandlerBindProgramsPc((u8)material->common.particle_type_tag == 0x97
                                           ? NuDisplayMaterialGlassProgramsPc
                                           : NuDisplayMaterialDebrisProgramsPc);
        if (PcWindowOptions.usehires == 0 || PCDeviceHardwareVertexProcessingPC != 4) {
            NuMtxTranspose(&transposed, &PcGeometryViewProjectionMatrixPc);
            PCDeviceSetVertexShaderConstantsF(0, &transposed.m00, 4);
            NuMtxTranspose(&transposed, &PcGeometryViewMatrixPc);
            PCDeviceSetVertexShaderConstantsF(0xc, &transposed.m00, 4);
            PCDeviceSetVertexShaderConstantsF(0x30, ShaderUniformRecordsPc[69].values, 1);
        }
        PCDeviceSetPixelShaderConstantsF(0, ShaderUniformRecordsPc[68].values, 1);
        if ((u8)material->common.particle_type_tag == 0x97) {
            GRAPHICS_RENDER_TEXTURE_PC *target = NuPostEffectGetBackBuffer(-1);
            PCDeviceSetTexture(0, target != NULL ? target->resource : NULL);
            PCDeviceSetSamplerState(0, 1, 3);
            PCDeviceSetSamplerState(0, 2, 3);
            PCDeviceSetSamplerState(0, 7, 0);
            PCDeviceSetTexture(1, NuTextureGetDimensions(NuDisplayMaterialDistortTexturePc)->resource);
        } else if (material->texture_id != 0) {
            PCDeviceSetTexture(0, NuTextureGetDimensions(material->texture_id)->resource);
        }
    } else if ((material->variant_flags & 0x200000) != 0) {
        PCShaderSetMaterialParametersPc(shader, &material->common);
        NuShaderManagerBindShaderPc(NULL);
        PCDeviceSetVertexShaderPc(NULL);
        PCDeviceSetPixelShaderPc(NULL);
        NuDisplayHandlerVertexShaderCachePc = NULL;
        NuDisplayHandlerPixelShaderCachePc = NULL;
        PCDeviceSetVertexDeclarationPc(NuDisplayMaterialFaceOnDeclarationPc);
        PCDisplayHandlerBindProgramsPc(PCDeviceBackend.textures[1] != NULL && (material->face_on_variant & 0xf) != 0
                                           ? NuDisplayMaterialFaceOnDecalProgramsPc
                                           : NuDisplayMaterialFaceOnProgramsPc);
        if (PCDeviceHardwareVertexProcessingPC != 4) {
            NuMtxTranspose(&transposed, &PcGeometryViewProjectionMatrixPc);
            PCDeviceSetVertexShaderConstantsF(0, &transposed.m00, 4);
            NuMtxTranspose(&transposed, &PcGeometryViewMatrixPc);
            PCDeviceSetVertexShaderConstantsF(0xc, &transposed.m00, 4);
            PCDeviceSetVertexShaderConstantsF(0x30, ShaderUniformRecordsPc[69].values, 1);
        }
        PCDeviceSetPixelShaderConstantsF(0, ShaderUniformRecordsPc[68].values, 1);
        if (material->texture_id != 0)
            PCDeviceSetTexture(0, NuTextureGetDimensions(material->texture_id)->resource);
    } else if (material->common.display_list != NULL) {
        if (shader != NULL) {
            PCShaderSetMaterialParametersPc(shader, &material->common);
            NuShaderManagerBindShaderPc(shader);
            PCDeviceSetVertexDeclarationPc(material->vertex_declaration);
            if (RenderFrameSceneFlagsPc != 0)
                PCDeviceSetPixelShaderPc(NULL);
        } else {
            PCGraphicsRenderTextureAllocationError();
            NuShaderManagerBindShaderPc(NULL);
        }
    } else if (shader != NULL) {
        PCShaderSetMaterialParametersPc(shader, &material->common);
        NuShaderManagerBindShaderPc(shader);
        PCDeviceSetVertexDeclarationPc(material->vertex_declaration);
    } else {
        NuShaderManagerBindShaderPc(NULL);
        PCDeviceSetVertexDeclarationPc(NuDisplayMaterialDefaultDeclarationPc);
        PCDisplayHandlerBindProgramsPc(NuDisplayMaterialDefaultProgramsPc);
    }
    NuDisplayHandlerReflectionPc = 0;
    NuMtlSetRenderStatesPS(&material->common);
    PCDeviceBackend.SetColorWriteMaskPc((shader->unknown_4d4 & 6) == 0, (shader->unknown_4d4 & 6) == 0, 1);
    PCDeviceBackend.SetColorWriteMaskPc((shader->unknown_4d4 & 8) == 0, (shader->unknown_4d4 & 8) == 0, 3);
    if (PCDeviceHardwareVertexProcessingPC != 4 && PCShaderCurrentObjectPc() != NULL) {
        if (PCShaderCurrentObjectPc() != NULL &&
            ((PC_SHADER_OBJECT_FULL_VIEW *)PCShaderCurrentObjectPc())->parameters[0x4f].register_index >= 0) {
            for (i32 i = 0; i < 32; ++i) {
                NUVEC4 group;
                group.x = 1.0f;
                group.y = 1.0f;
                group.z = 1.0f;
                group.w = 1.0f;
                NuShaderManagerSetElementfv(0x4f, i, &group.x);
            }
        }
        i32 count = RenderFrameShaderArrayCountPc;
        if (count != 0) {
            if (count >= 8)
                count = 8;
            f32(*values)[4] = RenderFrameShaderArrayPc;
            PC_SHADER_OBJECT_FULL_VIEW *current = (PC_SHADER_OBJECT_FULL_VIEW *)PCShaderCurrentObjectPc();
            NuShaderObjectSetElementsfv(current, 0x4e, 0, count, &values[0][0]);
        }
    }
}

typedef struct PC_DL_MATERIAL_STATES_READER {
    u8 unknown_0000[0x40];
    u32 attributes;
    u8 unknown_0044[0x1ac];
    u32 variant_flags;
} PC_DL_MATERIAL_STATES_READER;
STATIC_ASSERT(offsetof(PC_DL_MATERIAL_STATES_READER, attributes) == 0x40);
STATIC_ASSERT(offsetof(PC_DL_MATERIAL_STATES_READER, variant_flags) == 0x1f0);

/* PCDisplayHandlerSetZMode is the real private helper already in the BC cohort. */
extern "C" void __cdecl NuMtlSetRenderStatesPS(NUMTL *arg) {
    PC_DL_MATERIAL_STATES_READER *material = (PC_DL_MATERIAL_STATES_READER *)arg;
    i32 write_colour = 1, write_alpha = 1;
    u32 alpha;
    PCDisplayHandlerSetZMode((material->attributes >> 14) & 3);
    if (material->variant_flags & 0x100000) {
        PCDeviceSetRenderState(15, 1);
        PCDeviceSetRenderState(24, 2);
        PCDeviceSetRenderState(25, 5);
    } else {
        alpha = (material->attributes >> 20) & 7;
        if (alpha <= 1) {
            PCDeviceSetRenderState(15, 0);
        } else {
            PCDeviceSetRenderState(15, 1);
            if (alpha == 5)
                PCDeviceSetRenderState(24, (material->attributes >> 23) & 0xff);
            else
                PCDeviceSetRenderState(24, 0);
            PCDeviceSetRenderState(25, 5);
        }
    }
    switch (material->attributes & 15) {
        case 0:
            PCDeviceSetRenderState(27, 0);
            break;
        case 1:
            PCDeviceSetRenderState(27, 1);
            PCDeviceSetRenderState(171, 1);
            PCDeviceSetRenderState(19, 5);
            PCDeviceSetRenderState(20, 6);
            break;
        case 2:
            PCDeviceSetRenderState(27, 1);
            PCDeviceSetRenderState(171, 1);
            PCDeviceSetRenderState(19, 5);
            PCDeviceSetRenderState(20, 2);
            break;
        case 3:
            PCDeviceSetRenderState(27, 1);
            PCDeviceSetRenderState(171, 3);
            PCDeviceSetRenderState(19, 1);
            PCDeviceSetRenderState(20, 6);
            break;
        case 10:
            PCDeviceSetRenderState(27, 0);
            PCDeviceSetRenderState(15, 1);
            PCDeviceSetRenderState(24, (material->attributes >> 23) & 0xff);
            PCDeviceSetRenderState(25, 7);
            break;
        case 13:
            write_colour = 0;
            break;
    }
    switch ((material->attributes >> 12) & 3) {
        case 0:
            PCDeviceSetRenderState(22, NuDisplayHandlerReflectionPc != 0 ? 2 : 3);
            break;
        case 1:
            PCDeviceSetRenderState(22, NuDisplayHandlerReflectionPc != 0 ? 3 : 2);
            break;
        case 2:
            PCDeviceSetRenderState(22, 1);
            break;
    }
    if (!(material->attributes & 0x80000))
        write_alpha = 0;
    if (RenderFrameSceneFlagsPc != 0) {
        PCDeviceBackend.SetColorWriteMaskPc(1, 1, 0);
        NuDisplayHandlerZModePc = 0;
        PCDeviceSetRenderState(7, 1);
        PCDeviceSetRenderState(14, 1);
        PCDeviceSetRenderState(23, 4);
    } else {
        PCDeviceBackend.SetColorWriteMaskPc(write_colour == 0, write_alpha == 0, 0);
    }
    NuDisplayHandlerTintPc = NuDisplayHandlerDefaultTintPc;
    NuShaderManagerSetfv(0x38, &NuDisplayHandlerDefaultTintPc.x);
    NuDisplayHandlerOpacityPc = -1.0f;
}
