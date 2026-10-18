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

#include <math.h>

struct POST_SHADER_PROGRAM_PC;

extern "C" {
    extern GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsPostEffectWorkTexturePc;
    extern PC_FRAMEBUFFER_PC *PostBlurFramebufferPc;
    extern POST_SHADER_PROGRAM_PC *PostBlur7x7ProgramPc, *PostCopyAuxProgramPc, *PostBlurAuxProgramPc;
    extern f32 PostBlurInputBiasPc;
    void __cdecl PostShaderProgramBindPc(POST_SHADER_PROGRAM_PC *);
    void __cdecl PCGraphicsSubmitViewportPc(i32, i32, i32, i32);
    void __cdecl PCGraphicsSetSamplerTuplePc(u32, const u32 *);
    u32 *__cdecl PCGraphicsEffectTextureSamplerPc(GRAPHICS_RENDER_TEXTURE_PC *);
    void __cdecl PCGraphicsDrawPostEffectQuadPc(void);
    void __cdecl PCGraphicsFramebufferBarrierPc(i32, i32);
    void __cdecl PCGraphicsAttachFramebuffer(PC_FRAMEBUFFER_PC *, i32, GRAPHICS_RENDER_TEXTURE_PC *, i32);
    void __cdecl PCGraphicsBindFramebuffer(PC_FRAMEBUFFER_PC *);
    void __cdecl PCGraphicsEffectTextureSetWrapFields(GRAPHICS_RENDER_TEXTURE_PC *, i32, i32);
    void __cdecl PCGraphicsEffectTextureSetFilterFields(GRAPHICS_RENDER_TEXTURE_PC *, i32, i32, i32);
    void __cdecl PCGraphicsEffectTextureSetLowNibbles(GRAPHICS_RENDER_TEXTURE_PC *, i32, i32);
}

extern "C" void __cdecl PostBlur7x7HighPc(GRAPHICS_RENDER_TEXTURE_PC *source, i32 source_lod,
                                          GRAPHICS_RENDER_TEXTURE_PC *textures, i32 first_lod, i32 iterations,
                                          i32 levels, bool mip_chain, f32 radius, POST_SHADER_PROGRAM_PC *program) {
    // Retail explicitly spills ceil's double result to binary32 before ftol2.
    volatile f32 rounded_radius = (f32)ceil((f64)radius);
    i32 guard = (i32)rounded_radius * 3;
    i32 work_width = PCGraphicsEffectTextureWidth(PCGraphicsPostEffectWorkTexturePc);
    i32 work_height = PCGraphicsEffectTextureHeight(PCGraphicsPostEffectWorkTexturePc);
    PostShaderProgramBindPc(program);
    for (i32 lod = 0; lod < levels; ++lod) {
        GRAPHICS_RENDER_TEXTURE_PC *input;
        i32 input_lod;
        if (lod == 0) {
            input = source;
            input_lod = source_lod;
        } else if (mip_chain) {
            input = textures;
            input_lod = first_lod + lod - 1;
        } else {
            input = textures + first_lod + lod - 1;
            input_lod = 0;
        }
        GRAPHICS_RENDER_TEXTURE_PC *output = mip_chain ? textures : textures + first_lod + lod;
        i32 output_lod = mip_chain ? first_lod + lod : 0;
        for (i32 pass = 0; pass < iterations; ++pass) {
            i32 width, height, output_width, output_height;
            PCGraphicsEffectTextureDimensions(input, (u16)input_lod, &width, &height);
            PCGraphicsEffectTextureDimensions(output, (u16)output_lod, &output_width, &output_height);
            volatile f32 dx = radius / (f32)width;
            volatile f32 dy = radius / (f32)height;
            f64 dx2 = (f64)dx * 2.0;
            f64 dy2 = (f64)dy * 2.0;
            f64 dx3 = (f64)dx * 3.0;
            f64 dy3 = (f64)dy * 3.0;
            f32 horizontal[7][4] = {
                {dx * 0.0f, 0, .34f, 0}, {dx, 0, .18f, 0},        {(f32)dx2, 0, .10f, 0},  {(f32)dx3, 0, .05f, 0},
                {-dx, 0, .18f, 0},       {(f32)-dx2, 0, .10f, 0}, {(f32)-dx3, 0, .05f, 0},
            };
            f32 vertical[7][4] = {
                {0, dy * 0.0f, .34f, 0}, {0, dy, .18f, 0},        {0, (f32)dy2, .10f, 0},  {0, (f32)dy3, .05f, 0},
                {0, -dy, .18f, 0},       {0, (f32)-dy2, .10f, 0}, {0, (f32)-dy3, .05f, 0},
            };
            f32 bias[4] = {1, 1, -.5f / (f32)width, -.5f / (f32)height};
            // Native PC output half-texel terms are positive 1/work extent.
            f32 output_bias[4] = {(f32)output_width / (f32)work_width, (f32)output_height / (f32)work_height,
                                  1.0f / (f32)work_width, 1.0f / (f32)work_height};
            PCGraphicsAttachFramebuffer(PostBlurFramebufferPc, 0, PCGraphicsPostEffectWorkTexturePc, 0);
            PCGraphicsBindFramebuffer(PostBlurFramebufferPc);
            PCGraphicsSubmitViewportPc(0, 0, output_width, output_height);
            PCDeviceSetVertexShaderConstantsF(0xa0, horizontal[0], 7);
            PCDeviceSetVertexShaderConstantsF(0xad, bias, 1);
            PCGraphicsEffectTextureSetWrapFields(input, 2, 2);
            if (width <= output_width && height <= output_height)
                PCGraphicsEffectTextureSetFilterFields(input, 1, 1, 1);
            else
                PCGraphicsEffectTextureSetFilterFields(input, 2, 2, 1);
            PCGraphicsEffectTextureSetLowNibbles(input, input_lod, input_lod);
            {
                GRAPHICS_RENDER_TEXTURE_PC *texture = input;
                if (texture != NULL) {
                    u32 *tuple = PCGraphicsEffectTextureSamplerPc(texture);
                    if ((u32)PCDeviceSamplerInvalidation[0][0] != tuple[0] ||
                        (u32)PCDeviceSamplerInvalidation[0][1] != tuple[1])
                        PCGraphicsSetSamplerTuplePc(0, PCGraphicsEffectTextureSamplerPc(texture));
                }
                PCGraphicsBindEffectTexture(texture, 0);
            }
            PCGraphicsDrawPostEffectQuadPc();
            PostShaderProgramBindPc(PostBlur7x7ProgramPc);
            {
                GRAPHICS_RENDER_TEXTURE_PC *texture = input;
                if (texture != NULL) {
                    u32 *tuple = PCGraphicsEffectTextureSamplerPc(texture);
                    if ((u32)PCDeviceSamplerInvalidation[0][0] != tuple[0] ||
                        (u32)PCDeviceSamplerInvalidation[0][1] != tuple[1])
                        PCGraphicsSetSamplerTuplePc(0, PCGraphicsEffectTextureSamplerPc(texture));
                }
                PCGraphicsBindEffectTexture(texture, 0);
            }
            if (output_height < work_height) {
                f32 edge[4] = {1, 0, 0, 1};
                PCDeviceSetVertexShaderConstantsF(0x80, edge, 1);
                PCGraphicsSubmitViewportPc(0, output_height, output_width, guard);
                PCGraphicsDrawPostEffectQuadPc();
            }
            if (output_width < work_width) {
                f32 edge[4] = {0, 1, 1, 0};
                PCGraphicsSubmitViewportPc(output_width, 0, guard, output_height);
                PCDeviceSetVertexShaderConstantsF(0x80, edge, 1);
                PCGraphicsDrawPostEffectQuadPc();
            }
            PCGraphicsFramebufferBarrierPc(0, 1);
            PCGraphicsAttachFramebuffer(PostBlurFramebufferPc, 0, output, output_lod);
            PCGraphicsBindFramebuffer(PostBlurFramebufferPc);
            PCGraphicsSubmitViewportPc(0, 0, output_width, output_height);
            PostShaderProgramBindPc(program);
            PCDeviceSetVertexShaderConstantsF(0xa0, vertical[0], 7);
            PCDeviceSetVertexShaderConstantsF(0xad, output_bias, 1);
            PCGraphicsEffectTextureSetWrapFields(output, 2, 2);
            PCGraphicsEffectTextureSetFilterFields(output, 1, 1, 1);
            PCGraphicsEffectTextureSetLowNibbles(output, output_lod, output_lod);
            PCGraphicsEffectTextureSetWrapFields(PCGraphicsPostEffectWorkTexturePc, 2, 2);
            PCGraphicsEffectTextureSetFilterFields(PCGraphicsPostEffectWorkTexturePc, 1, 1, 0);
            {
                GRAPHICS_RENDER_TEXTURE_PC *texture = PCGraphicsPostEffectWorkTexturePc;
                if (texture != NULL) {
                    u32 *tuple = PCGraphicsEffectTextureSamplerPc(texture);
                    if ((u32)PCDeviceSamplerInvalidation[0][0] != tuple[0] ||
                        (u32)PCDeviceSamplerInvalidation[0][1] != tuple[1])
                        PCGraphicsSetSamplerTuplePc(0, PCGraphicsEffectTextureSamplerPc(texture));
                }
                PCGraphicsBindEffectTexture(texture, 0);
            }
            PCGraphicsDrawPostEffectQuadPc();
            PCGraphicsFramebufferBarrierPc(0, 1);
            input = output;
            input_lod = output_lod;
        }
    }
}

extern "C" void __cdecl PostBlur7x7LowPc(GRAPHICS_RENDER_TEXTURE_PC *source, i32 source_lod,
                                         GRAPHICS_RENDER_TEXTURE_PC *textures, i32 first_lod, i32 iterations,
                                         i32 levels, bool mip_chain, f32 radius, POST_SHADER_PROGRAM_PC *program) {
    f32 weights[4] = {.25f, .25f, .25f, .25f};
    volatile f32 rounded_radius = (f32)ceil((f64)radius);
    i32 guard = (i32)rounded_radius * 3;
    i32 work_width = PCGraphicsPostEffectWorkTexturePc->width;
    i32 work_height = PCGraphicsPostEffectWorkTexturePc->height;
    PCDisplayHandlerBindProgramsPc((PC_DL_SHADER_PROGRAM_PAIR_PC *)program);
    for (i32 lod = 0; lod < levels; ++lod) {
        GRAPHICS_RENDER_TEXTURE_PC *input;
        i32 input_lod;
        if (lod == 0) {
            input = source;
            input_lod = source_lod;
        } else if (mip_chain) {
            input = textures;
            input_lod = first_lod + lod - 1;
        } else {
            input = textures + first_lod + lod - 1;
            input_lod = 0;
        }
        GRAPHICS_RENDER_TEXTURE_PC *output = mip_chain ? textures : textures + first_lod + lod;
        i32 output_lod = mip_chain ? first_lod + lod : 0;
        for (i32 pass = 0; pass < iterations; ++pass) {
            // x86 masks the shift count to five bits; dimensions are forced even.
            i32 width = (input->width >> ((u16)input_lod & 31)) & ~1;
            i32 height = (input->height >> ((u16)input_lod & 31)) & ~1;
            i32 output_width = (output->width >> ((u16)output_lod & 31)) & ~1;
            i32 output_height = (output->height >> ((u16)output_lod & 31)) & ~1;
            volatile f32 dx = radius / (f32)width;
            volatile f32 dy = radius / (f32)height;
            volatile f32 dx_negative_half = (f32)((f64)dx * -.5);
            volatile f32 dx_half = (f32)((f64)dx * .5);
            volatile f32 dy_half = (f32)((f64)dy * .5);
            volatile f32 dy_negative_half = (f32)((f64)dy * -.5);
            volatile f32 minus_dx = (f32)((f64)dx * -1.0);
            volatile f32 minus_dy = (f32)((f64)dy * -1.0);
            f32 horizontal[4][4] = {{dx_negative_half, dy, 0, 0},
                                    {dx, dy_half, 0, 0},
                                    {dx_half, minus_dy, 0, 0},
                                    {minus_dx, dy_half, 0, 0}};
            f32 vertical[4][4] = {{dx_half, dy, 0, 0},
                                  {dx, dy_negative_half, 0, 0},
                                  {dx_negative_half, minus_dy, 0, 0},
                                  {minus_dx, dy_negative_half, 0, 0}};
            f32 bias[4] = {1, 1, PostBlurInputBiasPc / (f32)width, PostBlurInputBiasPc / (f32)height};
            f32 output_bias[4] = {(f32)output_width / (f32)work_width, (f32)output_height / (f32)work_height,
                                  1.0f / (f32)work_width, 1.0f / (f32)work_height};
            PostBlurFramebufferPc->targets[0].Attach(PCGraphicsPostEffectWorkTexturePc, 0, false);
            PostBlurFramebufferPc->changed = 1;
            PCGraphicsBindFramebuffer(PostBlurFramebufferPc);
            {
                PC_VIEWPORT_PC viewport;
                viewport.x = 0;
                viewport.y = 0;
                viewport.width = output_width;
                viewport.height = output_height;
                viewport.min_z = 0;
                viewport.max_z = 1;
                NuVpSubmitPc(&viewport);
            }
            PCDeviceSetVertexShaderConstantsF(0, horizontal[0], 4);
            PCDeviceSetVertexShaderConstantsF(4, bias, 1);
            PCDeviceSetPixelShaderConstantsF(0, weights, 1);
            u32 level = input_lod & 15;
            input->flags = (((input->flags & 0xfca5dbf0) | level) & 0xffffff0f) | ((level | 0xa41b0) << 4);
            for (u32 stage = 0; stage < 4; ++stage) {
                if ((u32)PCDeviceSamplerInvalidation[stage][0] != input->flags ||
                    (u32)PCDeviceSamplerInvalidation[stage][1] != input->field_24)
                    PCGraphicsSetSamplerTuplePc(stage, &input->flags);
                PCDeviceSetTexture(stage, input->resource);
            }
            PCDeviceBackend.DrawPostEffectQuadPc();
            PostShaderProgramBindPc(PostBlurAuxProgramPc);
            if ((u32)PCDeviceSamplerInvalidation[0][0] != input->flags ||
                (u32)PCDeviceSamplerInvalidation[0][1] != input->field_24)
                PCGraphicsSetSamplerTuplePc(0, &input->flags);
            PCDeviceSetTexture(0, input->resource);
            if (output_height < work_height) {
                f32 edge[4] = {1, 0, 0, 1};
                PCDeviceSetVertexShaderConstantsF(0, edge, 1);
                {
                    PC_VIEWPORT_PC viewport;
                    viewport.x = 0;
                    viewport.y = output_height;
                    viewport.width = output_width;
                    viewport.height = guard;
                    viewport.min_z = 0;
                    viewport.max_z = 1;
                    NuVpSubmitPc(&viewport);
                }
                PCDeviceBackend.DrawPostEffectQuadPc();
            }
            if (output_width < work_width) {
                f32 edge[4] = {0, 1, 1, 0};
                {
                    PC_VIEWPORT_PC viewport;
                    viewport.x = output_width;
                    viewport.y = 0;
                    viewport.width = guard;
                    viewport.height = output_height;
                    viewport.min_z = 0;
                    viewport.max_z = 1;
                    NuVpSubmitPc(&viewport);
                }
                PCDeviceSetVertexShaderConstantsF(0, edge, 1);
                PCDeviceBackend.DrawPostEffectQuadPc();
            }
            PostBlurFramebufferPc->targets[0].Attach(output, output_lod, false);
            PostBlurFramebufferPc->changed = 1;
            PCGraphicsBindFramebuffer(PostBlurFramebufferPc);
            {
                PC_VIEWPORT_PC viewport;
                viewport.x = 0;
                viewport.y = 0;
                viewport.width = output_width;
                viewport.height = output_height;
                viewport.min_z = 0;
                viewport.max_z = 1;
                NuVpSubmitPc(&viewport);
            }
            PostShaderProgramBindPc(program);
            PCDeviceSetVertexShaderConstantsF(0, vertical[0], 4);
            PCDeviceSetVertexShaderConstantsF(4, output_bias, 1);
            PCDeviceSetPixelShaderConstantsF(0, weights, 1);
            output->flags =
                (((output->flags & 0xfca5dbf0) | (output_lod & 15)) & 0xffffff0f) | ((output_lod & 15) << 4) | 0xa41b00;
            PCGraphicsPostEffectWorkTexturePc->flags =
                ((PCGraphicsPostEffectWorkTexturePc->flags & 0xfffffbff) | 0x300);
            PCGraphicsPostEffectWorkTexturePc->flags =
                ((PCGraphicsPostEffectWorkTexturePc->flags & 0xffffdfff) | 0x1800);
            PCGraphicsPostEffectWorkTexturePc->flags =
                ((PCGraphicsPostEffectWorkTexturePc->flags & 0xfff5ffff) | 0x40000);
            PCGraphicsPostEffectWorkTexturePc->flags =
                ((PCGraphicsPostEffectWorkTexturePc->flags & 0xfc2fffff) | 0x200000);
            for (u32 stage = 0; stage < 4; ++stage) {
                GRAPHICS_RENDER_TEXTURE_PC *work = PCGraphicsPostEffectWorkTexturePc;
                if (work != NULL) {
                    if ((u32)PCDeviceSamplerInvalidation[stage][0] != work->flags ||
                        (u32)PCDeviceSamplerInvalidation[stage][1] != work->field_24)
                        PCGraphicsSetSamplerTuplePc(stage, &work->flags);
                    PCDeviceSetTexture(stage, work->resource);
                } else {
                    PCDeviceSetTexture(stage, NULL);
                }
            }
            PCDeviceBackend.DrawPostEffectQuadPc();
            input = output;
            input_lod = output_lod;
        }
    }
}

extern "C" void __cdecl PostBlur7x7DispatchPc(GRAPHICS_RENDER_TEXTURE_PC *source, i32 source_lod,
                                              GRAPHICS_RENDER_TEXTURE_PC *textures, i32 first_lod, i32 iterations,
                                              i32 levels, bool mip_chain, f32 radius, POST_SHADER_PROGRAM_PC *program) {
    if (PCDeviceHardwareVertexProcessingPC >= 2)
        PostBlur7x7HighPc(source, source_lod, textures, first_lod, iterations, levels, mip_chain, radius, program);
    else
        PostBlur7x7LowPc(source, source_lod, textures, first_lod, iterations, levels, mip_chain, radius,
                         PostCopyAuxProgramPc);
}

extern "C" {
#pragma bss_seg(".bss$PostBlurInputBiasPc")
    f32 PostBlurInputBiasPc = 0;
#pragma bss_seg()
}

extern "C" {
    extern PC_POST_BUFFER_PROXY_PC PostProxyBuffersPc[5];
    extern u8 PostBloomAttachPendingPc;
    extern const WCHAR PostEffectRenderLabelPc[];
    extern u32 PCGraphicsPostEffectWidthPc, PCGraphicsPostEffectHeightPc;
    extern GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsPostEffectBackBufferTexturePc;
    extern POST_SHADER_PROGRAM_PC *PostCopyTexProgramPc, *PostBlur5x5ProgramPc;
    extern i32 PCDeviceHardwareVertexProcessingPC;
    extern NUMTX PcGeometryViewMatrixPc, PcGeometryProjectionMatrixPc;
    void __cdecl PostBlur7x7DispatchPc(GRAPHICS_RENDER_TEXTURE_PC *, i32, GRAPHICS_RENDER_TEXTURE_PC *, i32, i32, i32,
                                       bool, f32, POST_SHADER_PROGRAM_PC *);
}

void NuMainFilter::RenderBloomPc() {
    PC_FRAMEBUFFER_PC *output =
        (PC_FRAMEBUFFER_PC *)PostResourcePortsPc[0].manager->entries[PostResourcePortsPc[0].index].data;
    PC_POST_BUFFER_PROXY_PC *color =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data;
    PC_POST_BUFFER_PROXY_PC *normal =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[2].manager->entries[PostResourcePortsPc[2].index].data;
    if (!color->resolved)
        color->resolved = true;
    GRAPHICS_RENDER_TEXTURE_PC *normal_texture = normal->texture;
    if (normal_texture != NULL && !normal->resolved)
        normal->resolved = true;
    GRAPHICS_RENDER_TEXTURE_PC *output_texture = output->targets[0].texture;
    GRAPHICS_RENDER_TEXTURE_PC *color_texture = color->texture;
    PCDeviceBackend.CopySurfaceTargetPc(&texture_surface, 2, 0);
    if (normal_texture != NULL) {
        if (PostBloomAttachPendingPc) {
            platform_fbo->targets[0].Attach(PCGraphicsPostEffectBackBufferTexturePc, 0, false);
            platform_fbo->changed = 1;
            PostBloomAttachPendingPc = 0;
        }
        PCGraphicsBindFramebuffer(platform_fbo);
        PC_VIEWPORT_PC viewport = {0, 0, PCGraphicsPostEffectWidthPc, PCGraphicsPostEffectHeightPc, 0.0f, 1.0f};
        NuVpSubmitPc(&viewport);
        if ((u32)PCDeviceSamplerInvalidation[0][0] != normal_texture->flags ||
            (u32)PCDeviceSamplerInvalidation[0][1] != normal_texture->field_24)
            PCGraphicsSetSamplerTuplePc(0, &normal_texture->flags);
        PCDeviceSetTexture(0, normal_texture->resource);
        PCDisplayHandlerBindProgramsPc((PC_DL_SHADER_PROGRAM_PAIR_PC *)PostCopyTexProgramPc);
        PCDeviceBackend.SetColorWriteMaskPc(1, 0, 0);
        PCDeviceBackend.DrawPostEffectQuadPc();
        PCDeviceSetTexture(0, NULL);
        PCDeviceBackend.SetColorWriteMaskPc(0, 0, 0);
    }
    PostBlur7x7DispatchPc(PCGraphicsPostEffectBackBufferTexturePc, 0, PCGraphicsPostEffectBackBufferTexturePc, 0,
                          normal_texture != NULL ? 5 : 2, 1, false, 1.0f, PostBlur5x5ProgramPc);
    PCGraphicsBindFramebuffer(output);
    PC_VIEWPORT_PC viewport = {0, 0, 0, 0, 0.0f, 1.0f};
    if (output != PcDefaultFramebuffer && output != PcBootSecondaryFramebuffer) {
        viewport.width = output_texture->width;
        viewport.height = output_texture->height;
    } else {
        viewport.width = PS2_REZ_W;
        viewport.height = PS2_REZ_H;
    }
    NuVpSubmitPc(&viewport);
    void *program;
    if (bloom->directional) {
        program = output == PcBootSecondaryFramebuffer && PCDeviceBackend.textures[0] != NULL ? programs[8]
                                                                                              : platform_programs[1];
    } else {
        program = output == PcBootSecondaryFramebuffer && PCDeviceBackend.textures[0] != NULL ? programs[6]
                                                                                              : platform_programs[0];
    }
    PCDisplayHandlerBindProgramsPc((PC_DL_SHADER_PROGRAM_PAIR_PC *)program);
    if (PCDeviceHardwareVertexProcessingPC < 4) {
        NUMTX inverse_view;
        NuMtxInvH(&inverse_view, &PcGeometryViewMatrixPc);
        PCDeviceSetVertexShaderConstantsF(12, &inverse_view.m00, 4);
        f32 fovy, aspect, z_near, z_far;
        PcGetGeometryPerspectivePc(&PcGeometryProjectionMatrixPc, &fovy, &aspect, &z_near, &z_far);
        NUVEC4 depth = {z_near, z_far, z_far - z_near, 1.0f / PcGeometryProjectionMatrixPc.m22};
        PCDeviceSetVertexShaderConstantsF(49, &depth.x, 1);
        f32 left, right, bottom, top;
        NuMtxGetFrustumD3D(&PcGeometryProjectionMatrixPc, &left, &right, &bottom, &top, &z_near, &z_far);
        NUVEC4 frustum = {left, right - left, bottom, top - bottom};
        PCDeviceSetVertexShaderConstantsF(50, &frustum.x, 1);
    }
    /* Preserve each native single-precision store before packing shader vectors. */
    volatile f32 blend = bloom->blend;
    volatile f32 intensity = bloom->intensity * blur_gain;
    NUVEC4 primary = {intensity, blend, 0.0f, 0.0f};
    PCDeviceSetVertexShaderConstantsF(platform_uniforms[0], &primary.x, 1);
    volatile f32 scale_range = (bloom->far_scale - bloom->near_scale) * bloom->intensity;
    volatile f32 near_angle = (f32)(bloom->near_angle / 180.0);
    volatile f32 angle_range = (f32)(180.0 / (bloom->far_angle - bloom->near_angle));
    NUVEC4 threshold = {bloom->near_scale * bloom->intensity, scale_range, near_angle, angle_range};
    PCDeviceSetVertexShaderConstantsF(platform_uniforms[1], &threshold.x, 1);
    if (bloom->directional) {
        volatile f32 directional_range = (bloom->direction_far_scale - bloom->direction_near_scale) * bloom->intensity;
        volatile f32 direction_angle = (f32)((bloom->direction_near_angle * 3.1415927410125732) / 180.0);
        volatile f32 direction_angle_range =
            (f32)(180.0 / ((bloom->direction_far_angle - bloom->direction_near_angle) * 3.1415927410125732));
        NUVEC4 directional = {bloom->intensity * bloom->direction_near_scale, directional_range, direction_angle,
                              direction_angle_range};
        PCDeviceSetVertexShaderConstantsF(platform_uniforms[2], &directional.x, 1);
        NUVEC4 direction = {bloom->direction.x, bloom->direction.y, bloom->direction.z, 1.0f};
        NuVecNorm((NUVEC *)&direction, (NUVEC *)&direction);
        direction.w = 1.0f;
        NUVEC4 direction_bias = {direction.x, direction.y, direction.z, bloom->direction_bias};
        PCDeviceSetVertexShaderConstantsF(platform_uniforms[3], &direction_bias.x, 1);
    }
    if (output == PcBootSecondaryFramebuffer && PCDeviceBackend.textures[0] != NULL) {
        color_texture->flags = (color_texture->flags & 0xfc25ffff) | 0x240000;
        PCGraphicsBindEffectTexturePc(0, color_texture);
    }
    PCGraphicsPostEffectBackBufferTexturePc->flags =
        ((PCGraphicsPostEffectBackBufferTexturePc->flags & 0xfff5ffff) | 0x40000) & 0xfc2fffff | 0x200000;
    PCGraphicsPostEffectBackBufferTexturePc->flags =
        ((PCGraphicsPostEffectBackBufferTexturePc->flags & 0xfffffbff) | 0x300) & 0xffffdfff | 0x1800;
    PCGraphicsBindEffectTexturePc(3, PCGraphicsPostEffectBackBufferTexturePc);
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(27, 0);
    PCDeviceSetRenderState(7, 0);
    PCDeviceSetRenderState(168, 15);
    if (output == PcBootSecondaryFramebuffer && PCDeviceBackend.textures[0] != NULL) {
        PCDeviceSetRenderState(27, 0);
        PCDeviceSetRenderState(15, 0);
    } else {
        PCDeviceSetRenderState(27, 1);
        PCDeviceSetRenderState(19, 2);
        PCDeviceSetRenderState(20, 2);
        PCDeviceSetRenderState(171, 1);
        PCDeviceSetRenderState(15, 0);
    }
    PCDeviceBackend.DrawPostEffectIndexedPc();
    PCDeviceSetTexture(0, NULL);
    PCDeviceSetTexture(3, NULL);
    color->texture = output_texture;
    color->kind = 0;
    color->enabled = true;
    color->resolved = false;
    PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data = color;
}

extern "C" f32 NuSpeedBlurVertexScalePc;
f32 NuSpeedBlurVertexScalePc = 40000.0f;
void NuSpeedBlurFilter::render() {
    extern POST_SHADER_PROGRAM_PC *PostCopyTexProgramPc;
    PC_FRAMEBUFFER_PC *output =
        (PC_FRAMEBUFFER_PC *)PostResourcePortsPc[0].manager->entries[PostResourcePortsPc[0].index].data;
    PC_POST_BUFFER_PROXY_PC *color =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data;
    if (!color->resolved)
        color->resolved = true;
    GRAPHICS_RENDER_TEXTURE_PC *color_texture = color->texture;
    f32 constants[8];
    constants[0] = 0.0f;
    constants[1] = parameters->scale * NuSpeedBlurVertexScalePc;
    computeSpeedBlur((NUVEC4 *)(constants + 4));
    PCDeviceBackend.CopySurfaceTargetPc(&texture_surface, 2, 0);
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(27, 0);
    PCDeviceSetRenderState(7, 0);
    PCDeviceSetRenderState(168, 15);
    PCGraphicsBindFramebuffer(output);
    if (output == PcBootSecondaryFramebuffer && PCDeviceBackend.textures[0] != NULL) {
        color_texture->flags = (color_texture->flags & 0xfc13ffff) | 0x120000;
        PCDisplayHandlerBindProgramsPc((PC_DL_SHADER_PROGRAM_PAIR_PC *)PostCopyTexProgramPc);
        PCGraphicsBindEffectTexturePc(0, color_texture);
        PCDeviceBackend.DrawPostEffectQuadPc();
    }
    PCDisplayHandlerBindProgramsPc((PC_DL_SHADER_PROGRAM_PAIR_PC *)programs[0]);
    PCDeviceSetVertexShaderConstantsF(platform_uniform, constants, 2);
    PCGraphicsPostEffectBackBufferTexturePc->flags =
        ((PCGraphicsPostEffectBackBufferTexturePc->flags & 0xfff5ffff) | 0x40000) & 0xfc2fffff | 0x200000;
    PCGraphicsPostEffectBackBufferTexturePc->flags =
        ((PCGraphicsPostEffectBackBufferTexturePc->flags & 0xfffffbff) | 0x300) & 0xffffdfff | 0x1800;
    PCGraphicsBindEffectTexturePc(0, PCGraphicsPostEffectBackBufferTexturePc);
    if (PCDeviceHardwareVertexProcessingPC <= 1) {
        PCGraphicsBindEffectTexturePc(1, PCGraphicsPostEffectBackBufferTexturePc);
        PCGraphicsBindEffectTexturePc(2, PCGraphicsPostEffectBackBufferTexturePc);
        PCGraphicsBindEffectTexturePc(3, PCGraphicsPostEffectBackBufferTexturePc);
    }
    PCDeviceSetRenderState(27, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(19, 5);
    PCDeviceSetRenderState(20, 6);
    PCDeviceSetRenderState(171, 1);
    PCDeviceBackend.DrawPostEffectIndexedPc();
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(27, 0);
    PCDeviceSetRenderState(7, 0);
    PCDeviceSetRenderState(168, 15);
}
