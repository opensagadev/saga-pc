// Display-list material/transform handlers and post-filter members of the platform range; the 7x7
// high/direct blurs and the Gaussian kernel helpers are in nu3d_includes.cpp.
#include "saga.h"
#include <math.h>

typedef struct PC_DL_MATERIAL_READER { // size 0x44
    u8 unknown_0000[0x40];             // 0x00
    u32 reflection_flags;              // 0x40
} PC_DL_MATERIAL_READER;

/* Private helper; takes mode in EAX. */
static void PCDisplayHandlerSetZMode(i32 mode) {
    NuDisplayHandlerZMode = mode;
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

extern "C" void fun_005f8a30(void) {
    D3DPERF_EndEvent();
}

extern "C" PC_FRAMEBUFFER *fun_005f8a40(void) {
    return PcDefaultFramebuffer;
}

extern "C" void NuDisplayListTransform(void *arg) {
    NUMTX *matrix = (NUMTX *)arg;
    NUVEC4 tint = NuDisplayHandlerTint;
    f32 opacity = matrix->m33;
    f32 shadow = matrix->m23;
    if (NuDisplayHandlerOpacity != opacity) {
        if (opacity < 1.0f) {
            tint.w *= opacity;
            NuDisplayHandlerZMode = 1;
            PCDeviceSetRenderState(7, 1);
            PCDeviceSetRenderState(0xe, 0);
            PCDeviceSetRenderState(0x17, 4);
            NuShaderManagerSetfv(0x38, &tint.x);
        } else {
            PC_DL_MATERIAL_READER *material = (PC_DL_MATERIAL_READER *)NuDisplayHandlerMaterial;
            PCDisplayHandlerSetZMode((material->reflection_flags >> 14) & 3);
            NuShaderManagerSetfv(0x38, &tint.x);
        }
        if (PCDeviceHardwareVertexProcessingPC == 4) {
            NuDisplayHandlerOpacity = opacity;
        }
    }
    if ((f64)shadow != (f64)PCGraphicsFrameState) {
        NuDisplayHandlerShadow = shadow;
        NuShaderManagerSetfv(0x63, &NuDisplayHandlerShadow);
        PCGraphicsFrameState = (i32)shadow;
    }
    matrix->m33 = 1.0f;
    matrix->m23 = 0.0f;
    NuShaderManagerSetfv(0x11, &matrix->m00);
    matrix->m33 = opacity;
    matrix->m23 = shadow;
}

extern "C" void NuDisplayListTransformTranspose(void *arg) {
    NUMTX *matrix = (NUMTX *)arg;
    NUVEC4 tint = NuDisplayHandlerTint;
    f32 opacity = matrix->m33;
    f32 shadow = matrix->m32;
    if (NuDisplayHandlerOpacity != opacity) {
        if (opacity < 1.0f) {
            tint.w *= opacity;
            NuDisplayHandlerZMode = 1;
            PCDeviceSetRenderState(7, 1);
            PCDeviceSetRenderState(0xe, 0);
            PCDeviceSetRenderState(0x17, 4);
            NuShaderManagerSetfv(0x38, &tint.x);
        } else {
            PC_DL_MATERIAL_READER *material = (PC_DL_MATERIAL_READER *)NuDisplayHandlerMaterial;
            if (material != NULL) {
                PCDisplayHandlerSetZMode((material->reflection_flags >> 14) & 3);
                NuShaderManagerSetfv(0x38, &tint.x);
            }
        }
        if (PCDeviceHardwareVertexProcessingPC == 4) {
            NuDisplayHandlerOpacity = opacity;
        }
    }
    if ((f64)shadow != (f64)PCGraphicsFrameState) {
        NuDisplayHandlerShadow = shadow;
        NuShaderManagerSetfv(0x63, &NuDisplayHandlerShadow);
        PCGraphicsFrameState = (i32)shadow;
    }
    matrix->m33 = 1.0f;
    matrix->m32 = 0.0f;
    NuShaderManagerSetElementsfv_transpose(0x11, 0, 1, &matrix->m00);
    matrix->m33 = opacity;
    matrix->m32 = shadow;
}

// Packet: NUMTX world, f32 magnitude, NUMTX face_on (0x84 bytes), read with a moving cursor.
extern "C" void NuDisplayListFaceOnTransform(void *arg) {
    VARIPTR packet;
    NUVEC4 values;
    NUVEC4 tint;
    f32 opacity;
    packet.void_ptr = arg;
    NUMTX *matrix = (NUMTX *)packet.void_ptr;
    values.x = matrix->m30;
    values.y = matrix->m31;
    values.z = matrix->m32;
    values.w = 1.0f;
    matrix->m30 = 0.0f;
    matrix->m31 = 0.0f;
    matrix->m32 = 0.0f;
    opacity = matrix->m33;
    matrix->m33 = 1.0f;
    NuMtxTranspose(matrix, matrix);
    PCDeviceSetVertexShaderConstantsF(0x50, &matrix->m00, 4);
    packet.char_ptr += sizeof(NUMTX);
    PCDeviceSetVertexShaderConstantsF(0x59, &values.x, 1);
    values.x = *(f32 *)packet.void_ptr;
    packet.char_ptr += sizeof(f32);
    values.y = 1.0f;
    values.z = values.w = 0.0f;
    PCDeviceSetVertexShaderConstantsF(0x54, &values.x, 1);
    NuMtxTranspose((NUMTX *)packet.void_ptr, (NUMTX *)packet.void_ptr);
    PCDeviceSetVertexShaderConstantsF(0x55, (f32 *)packet.void_ptr, 4);
    tint = NuDisplayHandlerTint;
    if (opacity < 1.0f) {
        tint.w *= opacity;
        NuDisplayHandlerZMode = 1;
        PCDeviceSetRenderState(7, 1);
        PCDeviceSetRenderState(0xe, 0);
        PCDeviceSetRenderState(0x17, 4);
        /* The count is 4 registers despite only four initialized tint floats. */
        PCDeviceSetVertexShaderConstantsF(0x28, &tint.x, 4);
    } else {
        PC_DL_MATERIAL_READER *material = (PC_DL_MATERIAL_READER *)NuDisplayHandlerMaterial;
        PCDisplayHandlerSetZMode((material->reflection_flags >> 14) & 3);
        PCDeviceSetVertexShaderConstantsF(0x28, &tint.x, 4);
    }
}

/* Minimum reader only; no material allocation, pointee or full extent claim. */
typedef struct PC_DL_E_MATERIAL_READER {     // size 0x2c0
    NUMTL common;                            // 0x000
    u8 unknown_009c[0x1c];                   // 0x09c
    i32 texture_id;                          // 0x0b8
    u8 unknown_00bc[0x134];                  // 0x0bc
    u32 variant_flags;                       // 0x1f0
    i16 shader_id;                           // 0x1f4
    u8 unknown_01f6[0x72];                   // 0x1f6
    u8 face_on_variant;                      // 0x268
    u8 unknown_0269[0x53];                   // 0x269
    PC_TEXTURE_RESOURCE *vertex_declaration; // 0x2bc
} PC_DL_E_MATERIAL_READER;

struct PC_DL_SHADER_PROGRAM_PAIR_PC {   // size 0x8
    PC_TEXTURE_RESOURCE *vertex_shader; // 0x0
    PC_TEXTURE_RESOURCE *pixel_shader;  // 0x4
};

extern "C" {
    static void PCGraphicsBindEffectTexturePc(u32 stage, GRAPHICS_RENDER_TEXTURE *texture) {
        if (texture != NULL) {
            const u32 *tuple = &texture->flags;
            if ((u32)PCDeviceSamplerInvalidation[stage][0] != tuple[0] || (u32)PCDeviceSamplerInvalidation[stage][1] != tuple[1]) {
                PCGraphicsSetSamplerTuple(stage, tuple);
            }
            PCDeviceSetTexture(stage, texture->resource);
        } else {
            PCDeviceSetTexture(stage, NULL);
        }
    }
}

extern "C" void PCShaderSetMaterialParametersPc(NuShaderObject *, NUMTL *);
static void PCDisplayHandlerBindPrograms(PC_DL_SHADER_PROGRAM_PAIR_PC *);

static void PCDisplayHandlerBindPrograms(PC_DL_SHADER_PROGRAM_PAIR_PC *programs) {
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

extern "C" void fun_005fa110(char *dst, const char *src) {
    *dst = *src;
}

extern "C" u32 fun_005fa120(const char *text) {
    return strlen(text);
}

extern "C" char *fun_005fa140(char *dst, u32 size, const char *src, u32 count) {
    memcpy_s(dst, size, src, count);
    return dst;
}

extern "C" char *fun_005fa170(char *dst, u32 size, const char *src, u32 count) {
    memmove_s(dst, size, src, count);
    return dst;
}

extern "C" void fun_005fa620(i32 mode) {
    PCDisplayHandlerSetZMode(mode);
}

extern "C" void NuDisplayListMaterial(void *arg) {
    PC_DL_E_MATERIAL_READER *material = (PC_DL_E_MATERIAL_READER *)arg;
    NuShaderObject *shader = (NuShaderObject *)PCShaderRecordFromHandle((u32)(i32)material->shader_id);
    NUMTX transposed;
    NuDisplayMaterialLast = material;
    NuDisplayHandlerMaterial = material;
    PCGraphicsBoundShader = -1;
    PCGraphicsFrameState = -1;
    if ((material->variant_flags & 0x100000) != 0) {
        PCDeviceSetVertexShaderPc(NULL);
        PCDeviceSetPixelShaderPc(NULL);
        NuDisplayHandlerVertexShaderCachePc = NULL;
        NuDisplayHandlerPixelShaderCachePc = NULL;
        NuShaderManagerBindShader(NULL);
        PCDeviceSetVertexDeclarationPc(NuDisplayMaterialDebrisDeclaration);
        PCDisplayHandlerBindPrograms((u8)material->common.particle_type_tag == 0x97 ? NuDisplayMaterialGlassPrograms
                                                                                    : NuDisplayMaterialDebrisPrograms);
        if (PcWindowOptions.usehires == 0 || PCDeviceHardwareVertexProcessingPC != 4) {
            NuMtxTranspose(&transposed, &PcGeometryViewProjectionMatrix);
            PCDeviceSetVertexShaderConstantsF(0, &transposed.m00, 4);
            NuMtxTranspose(&transposed, &PcGeometryViewMatrix);
            PCDeviceSetVertexShaderConstantsF(0xc, &transposed.m00, 4);
            PCDeviceSetVertexShaderConstantsF(0x30, ShaderUniformRecordsPc[69].values, 1);
        }
        PCDeviceSetPixelShaderConstantsF(0, ShaderUniformRecordsPc[68].values, 1);
        if ((u8)material->common.particle_type_tag == 0x97) {
            GRAPHICS_RENDER_TEXTURE *target = NuPostEffectGetBackBuffer(-1);
            if (target != NULL) {
                PCDeviceSetTexture(0, target->resource);
            } else {
                PCDeviceSetTexture(0, NULL);
            }
            PCDeviceSetSamplerState(0, 1, 3);
            PCDeviceSetSamplerState(0, 2, 3);
            PCDeviceSetSamplerState(0, 7, 0);
            PCDeviceSetTexture(1, NuTextureGetDimensions(NuDisplayMaterialDistortTexture)->resource);
        } else if (material->texture_id != 0) {
            PCDeviceSetTexture(0, NuTextureGetDimensions(material->texture_id)->resource);
        }
    } else if ((material->variant_flags & 0x200000) != 0) {
        PCShaderSetMaterialParametersPc(shader, &material->common);
        NuShaderManagerBindShader(NULL);
        PCDeviceSetVertexShaderPc(NULL);
        PCDeviceSetPixelShaderPc(NULL);
        NuDisplayHandlerVertexShaderCachePc = NULL;
        NuDisplayHandlerPixelShaderCachePc = NULL;
        PCDeviceSetVertexDeclarationPc(NuDisplayMaterialFaceOnDeclaration);
        PCDisplayHandlerBindPrograms(PCDeviceBackend.textures[1] != NULL && (material->face_on_variant & 0xf) != 0
                                         ? NuDisplayMaterialFaceOnDecalPrograms
                                         : NuDisplayMaterialFaceOnPrograms);
        if (PCDeviceHardwareVertexProcessingPC != 4) {
            NuMtxTranspose(&transposed, &PcGeometryViewProjectionMatrix);
            PCDeviceSetVertexShaderConstantsF(0, &transposed.m00, 4);
            NuMtxTranspose(&transposed, &PcGeometryViewMatrix);
            PCDeviceSetVertexShaderConstantsF(0xc, &transposed.m00, 4);
            PCDeviceSetVertexShaderConstantsF(0x30, ShaderUniformRecordsPc[69].values, 1);
        }
        PCDeviceSetPixelShaderConstantsF(0, ShaderUniformRecordsPc[68].values, 1);
        if (material->texture_id != 0) {
            PCDeviceSetTexture(0, NuTextureGetDimensions(material->texture_id)->resource);
        }
    } else if (material->common.display_list != NULL) {
        if (shader != NULL) {
            PCShaderSetMaterialParametersPc(shader, &material->common);
            NuShaderManagerBindShader(shader);
            PCDeviceSetVertexDeclarationPc(material->vertex_declaration);
            if (RenderFrameSceneFlags != 0) {
                PCDeviceSetPixelShaderPc(NULL);
            }
        } else {
            PCGraphicsRenderTextureAllocationError();
            NuShaderManagerBindShader(NULL);
        }
    } else if (shader != NULL) {
        PCShaderSetMaterialParametersPc(shader, &material->common);
        NuShaderManagerBindShader(shader);
        PCDeviceSetVertexDeclarationPc(material->vertex_declaration);
    } else {
        NuShaderManagerBindShader(NULL);
        PCDeviceSetVertexDeclarationPc(NuDisplayMaterialDefaultDeclaration);
        PCDisplayHandlerBindPrograms(NuDisplayMaterialDefaultPrograms);
    }
    NuDisplayHandlerReflection = 0;
    NuMtlSetRenderStatesPS(&material->common);
    if ((shader->stage_flags & 6) == 0) {
        PCDeviceBackend.SetColorWriteMask(1, 1, 1);
    } else {
        PCDeviceBackend.SetColorWriteMask(0, 0, 1);
    }
    u32 write_mask = ~shader->stage_flags & 8;
    write_mask >>= 3;
    PCDeviceBackend.SetColorWriteMask(write_mask, write_mask, 3);
    if (PCDeviceHardwareVertexProcessingPC != 4 && PCShaderCurrentObjectPc() != NULL) {
        if (PCShaderCurrentObjectPc() != NULL && ((NuShaderObject *)PCShaderCurrentObjectPc())->parameters[0x4f].register_index >= 0) {
            for (i32 i = 0; i < 32; ++i) {
                NUVEC4 group;
                group.x = 1.0f;
                group.y = 1.0f;
                group.z = 1.0f;
                group.w = 1.0f;
                NuShaderManagerSetElementfv(0x4f, i, &group.x);
            }
        }
        i32 count = nuspecial_vertex_noffsets;
        if (count != 0) {
            if (count >= 8) {
                count = 8;
            }
            NuShaderObjectSetElementsfv((NuShaderObject *)PCShaderCurrentObjectPc(), 0x4e, 0, count, &nuspecial_vertex_offsets[0][0]);
        }
    }
}

typedef struct PC_DL_MATERIAL_STATES_READER { // size 0x1f4
    u8 unknown_0000[0x40];                    // 0x000
    u32 attributes;                           // 0x040
    u8 unknown_0044[0x1ac];                   // 0x044
    u32 variant_flags;                        // 0x1f0
} PC_DL_MATERIAL_STATES_READER;

/* PCDisplayHandlerSetZMode is the real private helper already in the BC cohort. */
extern "C" void NuMtlSetRenderStatesPS(NUMTL *arg) {
    PC_DL_MATERIAL_STATES_READER *material = (PC_DL_MATERIAL_STATES_READER *)arg;
    i32 write_colour = 1;
    i32 write_alpha = 1;
    u32 alpha;
    PCDisplayHandlerSetZMode((material->attributes >> 14) & 3);
    if (material->variant_flags & 0x100000) {
        PCDeviceSetRenderState(15, 1);
        PCDeviceSetRenderState(24, 2);
        PCDeviceSetRenderState(25, 5);
    } else {
        alpha = (material->attributes >> 20) & 7;
        switch (alpha) {
            case 0:
            case 1:
                PCDeviceSetRenderState(15, 0);
                break;
            case 5:
                PCDeviceSetRenderState(15, 1);
                PCDeviceSetRenderState(24, (material->attributes >> 23) & 0xff);
                PCDeviceSetRenderState(25, 5);
                break;
            default:
                PCDeviceSetRenderState(15, 1);
                PCDeviceSetRenderState(24, 0);
                PCDeviceSetRenderState(25, 5);
                break;
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
            if (NuDisplayHandlerReflection != 0) {
                PCDeviceSetRenderState(22, 2);
            } else {
                PCDeviceSetRenderState(22, 3);
            }
            break;
        case 1:
            if (NuDisplayHandlerReflection != 0) {
                PCDeviceSetRenderState(22, 3);
            } else {
                PCDeviceSetRenderState(22, 2);
            }
            break;
        case 2:
            PCDeviceSetRenderState(22, 1);
            break;
    }
    if (!(material->attributes & 0x80000)) {
        write_alpha = 0;
    }
    if (RenderFrameSceneFlags != 0) {
        PCDeviceBackend.SetColorWriteMask(1, 1, 0);
        NuDisplayHandlerZMode = 0;
        PCDeviceSetRenderState(7, 1);
        PCDeviceSetRenderState(14, 1);
        PCDeviceSetRenderState(23, 4);
    } else {
        PCDeviceBackend.SetColorWriteMask(write_colour == 0, write_alpha == 0, 0);
    }
    NuDisplayHandlerTint = NuDisplayHandlerDefaultTint;
    NuShaderManagerSetfv(0x38, &NuDisplayHandlerDefaultTint.x);
    NuDisplayHandlerOpacity = -1.0f;
}

extern "C" {
    extern GRAPHICS_RENDER_TEXTURE *PCGraphicsPostEffectWorkTexture;
    extern PC_FRAMEBUFFER *PostBlurFramebuffer;
    extern POST_SHADER_PROGRAM_PC *PostBlur7x7Program;
    extern POST_SHADER_PROGRAM_PC *PostCopyAuxProgram;
    extern POST_SHADER_PROGRAM_PC *PostBlurAuxProgram;
    extern f32 PostBlurInputBiasPc;
    void NuShaderProgramMakeCurrent(POST_SHADER_PROGRAM_PC *);
    void PCGraphicsAttachFramebuffer(PC_FRAMEBUFFER *, i32, GRAPHICS_RENDER_TEXTURE *, i32);
    void PCGraphicsEffectTextureSetWrapFields(GRAPHICS_RENDER_TEXTURE *, i32, i32);
    void PCGraphicsEffectTextureSetFilterFields(GRAPHICS_RENDER_TEXTURE *, i32, i32, i32);
    void PCGraphicsEffectTextureSetLowNibbles(GRAPHICS_RENDER_TEXTURE *, i32, i32);
}
extern "C" void PostBlur7x7High(GRAPHICS_RENDER_TEXTURE *source, i32 source_lod, GRAPHICS_RENDER_TEXTURE *textures, i32 first_lod,
                                i32 iterations, i32 levels, bool mip_chain, f32 radius, POST_SHADER_PROGRAM_PC *program);

// NuShaderProgramMakeCurrent and the per-stage sampler binds, inlined here.
#define POST_BLUR_BIND_PROGRAM(bound_program)                                                                                              \
    do {                                                                                                                                   \
        POST_SHADER_PROGRAM_PC *bound = (bound_program);                                                                                   \
        if (bound != NULL) {                                                                                                               \
            if (NuDisplayHandlerVertexShaderCachePc != bound->vertex_shader) {                                                             \
                PCDeviceSetVertexShaderPc(bound->vertex_shader);                                                                           \
                NuDisplayHandlerVertexShaderCachePc = bound->vertex_shader;                                                                \
            }                                                                                                                              \
            if (NuDisplayHandlerPixelShaderCachePc != bound->pixel_shader) {                                                               \
                PCDeviceSetPixelShaderPc(bound->pixel_shader);                                                                             \
                NuDisplayHandlerPixelShaderCachePc = bound->pixel_shader;                                                                  \
            }                                                                                                                              \
        } else {                                                                                                                           \
            PCDeviceSetVertexShaderPc(NULL);                                                                                               \
            PCDeviceSetPixelShaderPc(NULL);                                                                                                \
            NuDisplayHandlerVertexShaderCachePc = NULL;                                                                                    \
            NuDisplayHandlerPixelShaderCachePc = NULL;                                                                                     \
        }                                                                                                                                  \
    } while (0)
#define POST_BLUR_LOW_ATTACH(texture, lod)                                                                                                 \
    do {                                                                                                                                   \
        PC_FRAMEBUFFER *framebuffer = PostBlurFramebuffer;                                                                                 \
        framebuffer->targets[0].Attach((texture), (lod), false);                                                                           \
        framebuffer->changed = 1;                                                                                                          \
    } while (0)
#define POST_BLUR_LOW_BIND_INPUT(stage)                                                                                                    \
    do {                                                                                                                                   \
        if ((u32)PCDeviceSamplerInvalidation[stage][0] != input->flags || (u32)PCDeviceSamplerInvalidation[stage][1] != input->unknown_24) \
            PCGraphicsSetSamplerTuple(stage, &input->flags);                                                                               \
        PCDeviceSetTexture(stage, input->resource);                                                                                        \
    } while (0)
#define POST_BIND_GLOBAL_TEXTURE(stage, global)                                                                                            \
    do {                                                                                                                                   \
        GRAPHICS_RENDER_TEXTURE *bound = (global);                                                                                         \
        if (bound != NULL) {                                                                                                               \
            u32 *tuple = &bound->flags;                                                                                                    \
            if ((u32)PCDeviceSamplerInvalidation[stage][0] != tuple[0] || (u32)PCDeviceSamplerInvalidation[stage][1] != tuple[1])          \
                PCGraphicsSetSamplerTuple(stage, tuple);                                                                                   \
            PCDeviceSetTexture(stage, bound->resource);                                                                                    \
        } else {                                                                                                                           \
            PCDeviceSetTexture(stage, NULL);                                                                                               \
        }                                                                                                                                  \
    } while (0)

// One vertex-shader tap offset; rounds both coordinates through f32 temporaries.
struct __declspec(align(16)) POST_BLUR_LOW_OFFSET {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

#define POST_BLUR_LOW_SET_OFFSET(offset, offset_x, offset_y)                                                                               \
    do {                                                                                                                                   \
        f32 set_x = (f32)(offset_x);                                                                                                       \
        f32 set_y = (f32)(offset_y);                                                                                                       \
        (offset).x = set_x;                                                                                                                \
        (offset).y = set_y;                                                                                                                \
    } while (0)

extern "C" void PostBlur7x7Low(GRAPHICS_RENDER_TEXTURE *source, i32 source_lod, GRAPHICS_RENDER_TEXTURE *textures, i32 first_lod,
                               i32 iterations, i32 levels, bool mip_chain, f32 radius, POST_SHADER_PROGRAM_PC *program) {
    f32 weights[4] = {.25f, .25f, .25f, .25f};
    f32 rounded_radius = (f32)ceil((f64)radius);
    i32 guard = (i32)rounded_radius * 3;
    PC_VIEWPORT viewport;
    i32 work_width = PCGraphicsPostEffectWorkTexture->width;
    i32 work_height = PCGraphicsPostEffectWorkTexture->height;
    PCDisplayHandlerBindPrograms((PC_DL_SHADER_PROGRAM_PAIR_PC *)program);
    for (i32 lod = 0; lod < levels; ++lod) {
        GRAPHICS_RENDER_TEXTURE *input;
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
        GRAPHICS_RENDER_TEXTURE *output = mip_chain ? textures : textures + first_lod + lod;
        i32 output_lod = mip_chain ? first_lod + lod : 0;
        for (i32 pass = 0; pass < iterations; ++pass) {
            // x86 masks the shift count to five bits; dimensions are forced even.
            i32 width = (input->width >> ((u16)input_lod & 31)) & ~1;
            i32 height = (input->height >> ((u16)input_lod & 31)) & ~1;
            i32 output_width = (output->width >> ((u16)output_lod & 31)) & ~1;
            i32 output_height = (output->height >> ((u16)output_lod & 31)) & ~1;
            f32 dx = radius / (f32)width;
            f32 dy = radius / (f32)height;
            POST_BLUR_LOW_OFFSET horizontal[4];
            POST_BLUR_LOW_OFFSET vertical[4];
            horizontal[0].z = 0;
            horizontal[0].w = 0;
            horizontal[1].z = 0;
            horizontal[1].w = 0;
            horizontal[2].z = 0;
            horizontal[2].w = 0;
            horizontal[3].z = 0;
            horizontal[3].w = 0;
            vertical[0].z = 0;
            vertical[0].w = 0;
            vertical[1].z = 0;
            vertical[1].w = 0;
            vertical[2].z = 0;
            vertical[2].w = 0;
            vertical[3].z = 0;
            vertical[3].w = 0;
            POST_BLUR_LOW_SET_OFFSET(horizontal[0], dx * -.5, dy);
            POST_BLUR_LOW_SET_OFFSET(vertical[0], dx * .5, dy);
            POST_BLUR_LOW_SET_OFFSET(horizontal[1], dx, dy * .5);
            POST_BLUR_LOW_SET_OFFSET(vertical[1], dx, dy * -.5);
            POST_BLUR_LOW_SET_OFFSET(horizontal[2], dx * .5, dy * -1.0);
            POST_BLUR_LOW_SET_OFFSET(vertical[2], dx * -.5, dy * -1.0);
            POST_BLUR_LOW_SET_OFFSET(horizontal[3], dx * -1.0, dy * .5);
            POST_BLUR_LOW_SET_OFFSET(vertical[3], dx * -1.0, dy * -.5);
            f32 bias[4] = {1, 1, PostBlurInputBiasPc / (f32)width, PostBlurInputBiasPc / (f32)height};
            f32 output_bias[4] = {(f32)output_width / (f32)work_width, (f32)output_height / (f32)work_height, 1.0f / (f32)work_width,
                                  1.0f / (f32)work_height};
            POST_BLUR_LOW_ATTACH(PCGraphicsPostEffectWorkTexture, 0);
            PCGraphicsBindFramebuffer(PostBlurFramebuffer);
            viewport.x = 0;
            viewport.y = 0;
            viewport.width = output_width;
            viewport.height = output_height;
            viewport.min_z = 0;
            viewport.max_z = 1;
            NuVpSubmit(&viewport);
            PCDeviceSetVertexShaderConstantsF(0, &horizontal[0].x, 4);
            PCDeviceSetVertexShaderConstantsF(4, bias, 1);
            PCDeviceSetPixelShaderConstantsF(0, weights, 1);
            u32 level = input_lod & 15;
            input->flags = (((input->flags & 0xfca5dbf0) | level) & 0xffffff0f) | ((level | 0xa41b0) << 4);
            POST_BLUR_LOW_BIND_INPUT(0);
            POST_BLUR_LOW_BIND_INPUT(1);
            POST_BLUR_LOW_BIND_INPUT(2);
            POST_BLUR_LOW_BIND_INPUT(3);
            PCDeviceBackend.DrawPostEffectQuad();
            POST_BLUR_BIND_PROGRAM(PostBlurAuxProgram);
            POST_BLUR_LOW_BIND_INPUT(0);
            if (output_height < work_height) {
                f32 edge[4] = {1, 0, 0, 1};
                PCDeviceSetVertexShaderConstantsF(0, edge, 1);
                viewport.x = 0;
                viewport.y = output_height;
                viewport.width = output_width;
                viewport.height = guard;
                viewport.min_z = 0;
                viewport.max_z = 1;
                NuVpSubmit(&viewport);
                PCDeviceBackend.DrawPostEffectQuad();
            }
            if (output_width < work_width) {
                f32 edge[4] = {0, 1, 1, 0};
                viewport.x = output_width;
                viewport.y = 0;
                viewport.width = guard;
                viewport.height = output_height;
                viewport.min_z = 0;
                viewport.max_z = 1;
                NuVpSubmit(&viewport);
                PCDeviceSetVertexShaderConstantsF(0, edge, 1);
                PCDeviceBackend.DrawPostEffectQuad();
            }
            POST_BLUR_LOW_ATTACH(output, output_lod);
            PCGraphicsBindFramebuffer(PostBlurFramebuffer);
            viewport.x = 0;
            viewport.y = 0;
            viewport.width = output_width;
            viewport.height = output_height;
            viewport.min_z = 0;
            viewport.max_z = 1;
            NuVpSubmit(&viewport);
            POST_BLUR_BIND_PROGRAM(program);
            PCDeviceSetVertexShaderConstantsF(0, &vertical[0].x, 4);
            PCDeviceSetVertexShaderConstantsF(4, output_bias, 1);
            PCDeviceSetPixelShaderConstantsF(0, weights, 1);
            output->flags = (((output->flags & 0xfca5dbf0) | (output_lod & 15)) & 0xffffff0f) | ((output_lod & 15) << 4) | 0xa41b00;
            PCGraphicsPostEffectWorkTexture->flags =
                (((PCGraphicsPostEffectWorkTexture->flags & 0xfffffbff) | 0x300) & 0xffffdfff) | 0x1800;
            PCGraphicsPostEffectWorkTexture->flags =
                (((PCGraphicsPostEffectWorkTexture->flags & 0xfff5ffff) | 0x40000) & 0xfc2fffff) | 0x200000;
            POST_BIND_GLOBAL_TEXTURE(0, PCGraphicsPostEffectWorkTexture);
            POST_BIND_GLOBAL_TEXTURE(1, PCGraphicsPostEffectWorkTexture);
            POST_BIND_GLOBAL_TEXTURE(2, PCGraphicsPostEffectWorkTexture);
            POST_BIND_GLOBAL_TEXTURE(3, PCGraphicsPostEffectWorkTexture);
            PCDeviceBackend.DrawPostEffectQuad();
            input = output;
            input_lod = output_lod;
        }
    }
}

extern "C" void PostBlur7x7Dispatch(GRAPHICS_RENDER_TEXTURE *source, i32 source_lod, GRAPHICS_RENDER_TEXTURE *textures, i32 first_lod,
                                    i32 iterations, i32 levels, bool mip_chain, f32 radius, POST_SHADER_PROGRAM_PC *program) {
    if (PCDeviceHardwareVertexProcessingPC >= 2) {
        PostBlur7x7High(source, source_lod, textures, first_lod, iterations, levels, mip_chain, radius, program);
    } else {
        PostBlur7x7Low(source, source_lod, textures, first_lod, iterations, levels, mip_chain, radius, PostCopyAuxProgram);
    }
}

extern "C" {
    f32 PostBlurInputBiasPc = 0;
    f32 RenderFrameShaderTimePc = 0.0f;
    f32 PostBlurDirectOutputBiasPc = 0;
    f32 PostBlurDirectInputBiasPc = 0;
}

extern "C" {
    extern u8 PostBloomAttachPending;
    extern const WCHAR PostEffectRenderLabel[];
    extern u32 PCGraphicsPostEffectWidth;
    extern u32 PCGraphicsPostEffectHeight;
    extern GRAPHICS_RENDER_TEXTURE *PCGraphicsPostEffectBackBufferTexture;
    extern POST_SHADER_PROGRAM_PC *PostCopyTexProgram;
    extern POST_SHADER_PROGRAM_PC *PostBlur5x5Program;
    extern PC_EFFECT_SURFACE *PcSurfaceRegistry[256];
}

/* Header-inline: ??_GNuMainFilter (emitted here with the ctor) expands both. */
inline PC_EFFECT_SURFACE::~PC_EFFECT_SURFACE() {
    Reset();
    PcSurfaceRegistry[registry_index] = NULL;
}

inline NuPostFilterGen::~NuPostFilterGen() {
}

extern "C" void PCGraphicsCopyFramebufferTex2D(i32 attachment, GRAPHICS_RENDER_TEXTURE *texture, i32 level, i32 x, i32 y, i32 width,
                                               i32 height) {
    (void)x;
    (void)y;
    if (attachment == 0) {
        bool resize = width != PS2_REZ_W || height != PS2_REZ_H;
        PC_EFFECT_SURFACE target;
        target.Attach(texture, level, false);
        PCDeviceBackend.CopySurfaceTarget(&target, resize ? 2 : 1, 0);
    } else {
        /* An empty operation; no argument is read. */
        PCGraphicsRenderTextureAllocationError();
    }
}

NuMainFilter::NuMainFilter() {
    platform_fbo = NULL;
}

void NuMainFilter::RenderBloom() {
    PC_FRAMEBUFFER *output = (PC_FRAMEBUFFER *)PostPortOutFramebuffer.manager->entries[PostPortOutFramebuffer.index].data;
    PC_POST_BUFFER_PROXY *color = (PC_POST_BUFFER_PROXY *)PostPortColorBuffer.manager->entries[PostPortColorBuffer.index].data;
    PC_POST_BUFFER_PROXY *normal = (PC_POST_BUFFER_PROXY *)PostPortNormalBuffer.manager->entries[PostPortNormalBuffer.index].data;
    if (!color->resolved) {
        color->resolved = true;
    }
    GRAPHICS_RENDER_TEXTURE *normal_texture = normal->texture;
    if (normal_texture != NULL && !normal->resolved) {
        normal->resolved = true;
    }
    GRAPHICS_RENDER_TEXTURE *output_texture = output->targets[0].texture;
    GRAPHICS_RENDER_TEXTURE *color_texture = color->texture;
    PCDeviceBackend.CopySurfaceTarget(&texture_surface, 2, 0);
    if (normal_texture != NULL) {
        if (PostBloomAttachPending) {
            PC_FRAMEBUFFER *framebuffer = platform_fbo;
            framebuffer->targets[0].Attach(PCGraphicsPostEffectBackBufferTexture, 0, false);
            framebuffer->changed = 1;
            PostBloomAttachPending = 0;
        }
        PCGraphicsBindFramebuffer(platform_fbo);
        PC_VIEWPORT viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = PCGraphicsPostEffectWidth;
        viewport.height = PCGraphicsPostEffectHeight;
        viewport.min_z = 0.0f;
        viewport.max_z = 1.0f;
        NuVpSubmit(&viewport);
        u32 *tuple = &normal_texture->flags;
        if ((u32)PCDeviceSamplerInvalidation[0][0] != tuple[0] || (u32)PCDeviceSamplerInvalidation[0][1] != tuple[1]) {
            PCGraphicsSetSamplerTuple(0, tuple);
        }
        PCDeviceSetTexture(0, normal_texture->resource);
        PCDisplayHandlerBindPrograms((PC_DL_SHADER_PROGRAM_PAIR_PC *)PostCopyTexProgram);
        PCDeviceBackend.SetColorWriteMask(1, 0, 0);
        PCDeviceBackend.DrawPostEffectQuad();
        PCDeviceSetTexture(0, NULL);
        PCDeviceBackend.SetColorWriteMask(0, 0, 0);
        PostBlur7x7Dispatch(PCGraphicsPostEffectBackBufferTexture, 0, PCGraphicsPostEffectBackBufferTexture, 0, 5, 1, false, 1.0f,
                            PostBlur5x5Program);
    } else {
        PostBlur7x7Dispatch(PCGraphicsPostEffectBackBufferTexture, 0, PCGraphicsPostEffectBackBufferTexture, 0, 2, 1, false, 1.0f,
                            PostBlur5x5Program);
    }
    PCGraphicsBindFramebuffer(output);
    {
        PC_VIEWPORT viewport;
        if (output != PcDefaultFramebuffer && output != PcBootSecondaryFramebuffer) {
            viewport.x = 0;
            viewport.y = 0;
            viewport.width = output_texture->width;
            viewport.height = output_texture->height;
        } else {
            viewport.x = 0;
            viewport.y = 0;
            viewport.width = PS2_REZ_W;
            viewport.height = PS2_REZ_H;
        }
        viewport.min_z = 0.0f;
        viewport.max_z = 1.0f;
        NuVpSubmit(&viewport);
    }
    void *program;
    if (bloom->directional) {
        program = output == PcBootSecondaryFramebuffer && PCDeviceBackend.textures[0] != NULL ? programs[8] : platform_programs[1];
    } else {
        program = output == PcBootSecondaryFramebuffer && PCDeviceBackend.textures[0] != NULL ? programs[6] : platform_programs[0];
    }
    PCDisplayHandlerBindPrograms((PC_DL_SHADER_PROGRAM_PAIR_PC *)program);
    if (PCDeviceHardwareVertexProcessingPC < 4) {
        NUMTX inverse_view;
        NuMtxInvH(&inverse_view, &PcGeometryViewMatrix);
        PCDeviceSetVertexShaderConstantsF(12, &inverse_view.m00, 4);
        f32 fovy;
        f32 aspect;
        f32 z_near;
        f32 z_far;
        NuMtxGetPerspectiveD3D(&PcGeometryProjectionMatrix, &fovy, &aspect, &z_near, &z_far);
        NUVEC4 depth = {z_near, z_far, z_far - z_near, 1.0f / PcGeometryProjectionMatrix.m22};
        PCDeviceSetVertexShaderConstantsF(49, &depth.x, 1);
        NUVEC4 frustum;
        NuMtxGetFrustumD3D(&PcGeometryProjectionMatrix, &frustum.x, &frustum.y, &frustum.z, &frustum.w, &z_near, &z_far);
        frustum.y = frustum.y - frustum.x;
        frustum.w = frustum.w - frustum.z;
        PCDeviceSetVertexShaderConstantsF(50, &frustum.x, 1);
    }
    /* Preserve each single-precision store before packing shader vectors. */
    f32 blend = bloom->blend;
    f32 intensity = bloom->intensity * blur_gain;
    __declspec(align(16)) NUVEC4 primary = {intensity, blend, 0.0f, 0.0f};
    PCDeviceSetVertexShaderConstantsF(platform_uniforms[0], &primary.x, 1);
    f32 scale_range = (bloom->far_scale - bloom->near_scale) * bloom->intensity;
    f32 near_angle = (f32)(bloom->near_angle / 180.0);
    f32 angle_range = (f32)(180.0 / (bloom->far_angle - bloom->near_angle));
    NUVEC4 threshold = {bloom->near_scale * bloom->intensity, scale_range, near_angle, angle_range};
    PCDeviceSetVertexShaderConstantsF(platform_uniforms[1], &threshold.x, 1);
    if (bloom->directional) {
        f32 directional_range = (bloom->direction_far_scale - bloom->direction_near_scale) * bloom->intensity;
        f32 direction_angle = (f32)((bloom->direction_near_angle * 3.1415927410125732) / 180.0);
        f32 direction_angle_range = (f32)(180.0 / ((bloom->direction_far_angle - bloom->direction_near_angle) * 3.1415927410125732));
        NUVEC4 directional = {bloom->intensity * bloom->direction_near_scale, directional_range, direction_angle, direction_angle_range};
        PCDeviceSetVertexShaderConstantsF(platform_uniforms[2], &directional.x, 1);
        const NUVEC *source = &bloom->direction;
        f32 direction_y = source->y;
        f32 direction_z = source->z;
        NUVEC4 direction = {source->x, direction_y, direction_z, 1.0f};
        NuVecNorm((NUVEC *)&direction, (NUVEC *)&direction);
        direction.w = 1.0f;
        direction.w = bloom->direction_bias;
        PCDeviceSetVertexShaderConstantsF(platform_uniforms[3], &direction.x, 1);
    }
    if (output == PcBootSecondaryFramebuffer && PCDeviceBackend.textures[0] != NULL) {
        color_texture->flags = (color_texture->flags & 0xfc25ffff) | 0x240000;
        PCGraphicsBindEffectTexturePc(0, color_texture);
    }
    PCGraphicsPostEffectBackBufferTexture->flags =
        ((PCGraphicsPostEffectBackBufferTexture->flags & 0xfff5ffff) | 0x40000) & 0xfc2fffff | 0x200000;
    PCGraphicsPostEffectBackBufferTexture->flags =
        ((PCGraphicsPostEffectBackBufferTexture->flags & 0xfffffbff) | 0x300) & 0xffffdfff | 0x1800;
    POST_BIND_GLOBAL_TEXTURE(3, PCGraphicsPostEffectBackBufferTexture);
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
    PCDeviceBackend.DrawPostEffectIndexed();
    PCDeviceSetTexture(0, NULL);
    PCDeviceSetTexture(3, NULL);
    color->texture = output_texture;
    color->kind = 0;
    color->enabled = true;
    color->resolved = false;
    PostPortColorBuffer.manager->entries[PostPortColorBuffer.index].data = color;
}

/* Stretch-copies a colour texture into the output framebuffer's first target. */
void NuPostFilterGen::copy(GRAPHICS_RENDER_TEXTURE *color, PC_FRAMEBUFFER *output) {
    static PC_EFFECT_SURFACE surface;
    surface.Attach(color, 0, false);
    bool scaled = output->targets[0].width != surface.width || output->targets[0].height != surface.height;
    PCDeviceStretchRectPc(surface.surface, NULL, output->targets[0].surface, NULL, scaled ? 2 : 0);
}

/* Same unit as ??_GNuMainFilter: the atexit thunks for its static surfaces expand the inline destructor. */
extern "C" void PostFilterCopyTexture(GRAPHICS_RENDER_TEXTURE *destination, i32 destination_level, GRAPHICS_RENDER_TEXTURE *source,
                                      i32 source_level, POST_SHADER_PROGRAM_PC *program, GRAPHICS_RENDER_TEXTURE *extra) {
    if (program == PostCopyTexProgram) {
        static PC_EFFECT_SURFACE source_surface;
        static PC_EFFECT_SURFACE destination_surface;
        source_surface.Attach(source, source_level, false);
        destination_surface.Attach(destination, destination_level, false);
        bool scaled = destination_surface.width != source_surface.width || destination_surface.height != source_surface.height;
        PCDeviceStretchRectPc(source_surface.surface, NULL, destination_surface.surface, NULL, scaled ? 2 : 0);
        return;
    }
    PostFilterBlitTexture(destination, destination_level, source, source_level, program, extra);
}

extern "C" void PCGraphicsDestroyFramebuffer(PC_FRAMEBUFFER *);

void NuMainFilter::destroyResources() {
    NuMainFilterGen::destroyResources();
    PCGraphicsDestroyFramebuffer(platform_fbo);
}

void NuMainFilter::initTextureResources(i32, i32) {
    texture_surface.Attach(PCGraphicsPostEffectBackBufferTexture, 0, false);
}

void NuMainFilter::destroyTextureResources() {
    texture_surface.Reset();
}

extern "C" f32 NuSpeedBlurVertexScale;
extern "C" {
    NUVEC4 NuDisplayHandlerDefaultTint = {1.0f, 1.0f, 1.0f, 1.0f};
}
f32 NuSpeedBlurVertexScale = 40000.0f;

void NuSpeedBlurFilter::render() {
    extern POST_SHADER_PROGRAM_PC *PostCopyTexProgram;
    PC_FRAMEBUFFER *output = (PC_FRAMEBUFFER *)PostPortOutFramebuffer.manager->entries[PostPortOutFramebuffer.index].data;
    PC_POST_BUFFER_PROXY *color = (PC_POST_BUFFER_PROXY *)PostPortColorBuffer.manager->entries[PostPortColorBuffer.index].data;
    if (!color->resolved) {
        color->resolved = true;
    }
    GRAPHICS_RENDER_TEXTURE *color_texture = color->texture;
    __declspec(align(16)) f32 constants[8];
    constants[0] = 0.0f;
    constants[1] = parameters->scale * NuSpeedBlurVertexScale;
    computeSpeedBlur((NUVEC4 *)(constants + 4));
    PCDeviceBackend.CopySurfaceTarget(&texture_surface, 2, 0);
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(27, 0);
    PCDeviceSetRenderState(7, 0);
    PCDeviceSetRenderState(168, 15);
    PCGraphicsBindFramebuffer(output);
    if (output == PcBootSecondaryFramebuffer && PCDeviceBackend.textures[0] != NULL) {
        color_texture->flags = (color_texture->flags & 0xfc13ffff) | 0x120000;
        PCDisplayHandlerBindPrograms((PC_DL_SHADER_PROGRAM_PAIR_PC *)PostCopyTexProgram);
        PCGraphicsBindEffectTexturePc(0, color_texture);
        PCDeviceBackend.DrawPostEffectQuad();
    }
    PCDisplayHandlerBindPrograms((PC_DL_SHADER_PROGRAM_PAIR_PC *)programs[0]);
    PCDeviceSetVertexShaderConstantsF(platform_uniform, constants, 2);
    PCGraphicsPostEffectBackBufferTexture->flags =
        ((PCGraphicsPostEffectBackBufferTexture->flags & 0xfff5ffff) | 0x40000) & 0xfc2fffff | 0x200000;
    PCGraphicsPostEffectBackBufferTexture->flags =
        ((PCGraphicsPostEffectBackBufferTexture->flags & 0xfffffbff) | 0x300) & 0xffffdfff | 0x1800;
    POST_BIND_GLOBAL_TEXTURE(0, PCGraphicsPostEffectBackBufferTexture);
    if (PCDeviceHardwareVertexProcessingPC <= 1) {
        POST_BIND_GLOBAL_TEXTURE(1, PCGraphicsPostEffectBackBufferTexture);
        POST_BIND_GLOBAL_TEXTURE(2, PCGraphicsPostEffectBackBufferTexture);
        POST_BIND_GLOBAL_TEXTURE(3, PCGraphicsPostEffectBackBufferTexture);
    }
    PCDeviceSetRenderState(27, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(19, 5);
    PCDeviceSetRenderState(20, 6);
    PCDeviceSetRenderState(171, 1);
    PCDeviceBackend.DrawPostEffectIndexed();
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(27, 0);
    PCDeviceSetRenderState(7, 0);
    PCDeviceSetRenderState(168, 15);
}
