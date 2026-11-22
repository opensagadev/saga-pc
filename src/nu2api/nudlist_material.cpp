// Display-list material/transform handlers and post-filter members of the platform range; the 7x7
// high/direct blurs and the Gaussian kernel helpers are in nu3d_includes.cpp.
#include "nu2api/NuEffectTex_pc.h"
#include "nu2api/PCInput.h"
#include "nu2api/nu3d/ShaderManager.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nupostfilter.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nurndr_platform.h"
#include "nu2api/nu3d/nushader.h"
#include "nu2api/nu3d/nushader_plain.h"
#include "nu2api/nu3d/nushadermanager_gen.h"
#include "nu2api/nu3d/nushaderobject.h"
#include "nu2api/nu3d/shaderSemantics.h"
#include "nu2api/nu3d_pc_includes.h"
#include "nu2api/nucamera_dlist.h"
#include "nu2api/nucore/NuMainFilter.h"
#include "nu2api/nucore/NuMainFilterGen.h"
#include "nu2api/nucore/NuPostFilterGen.h"
#include "nu2api/nucore/NuSpeedBlurFilter.h"
#include "nu2api/nucore/NuSpeedBlurFilterGen.h"
#include "nu2api/nugobj.h"
#include "nu2api/nuhgo_dlist.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/numotionaccumfilter_gen.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nustream.h"
#include "nu2api/nuwin32/d3dCalls.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include <math.h>

/* Private helper; takes mode in EAX. */
static void PCDisplayHandlerSetZMode(i32 mode) {
    NuDisplayHandlerZMode = mode;
    switch (mode) {
        case NUMTL_ZMODE_NORMAL:
            PCDeviceSetRenderState(D3DRS_ZENABLE, 1);
            PCDeviceSetRenderState(D3DRS_ZWRITEENABLE, 1);
            PCDeviceSetRenderState(D3DRS_ZFUNC, 4);
            break;
        case NUMTL_ZMODE_READ_ONLY:
            PCDeviceSetRenderState(D3DRS_ZENABLE, 1);
            PCDeviceSetRenderState(D3DRS_ZWRITEENABLE, 0);
            PCDeviceSetRenderState(D3DRS_ZFUNC, 4);
            break;
        case NUMTL_ZMODE_ALWAYS:
            PCDeviceSetRenderState(D3DRS_ZENABLE, 1);
            PCDeviceSetRenderState(D3DRS_ZWRITEENABLE, 1);
            PCDeviceSetRenderState(D3DRS_ZFUNC, 8);
            break;
        case NUMTL_ZMODE_OFF:
            PCDeviceSetRenderState(D3DRS_ZENABLE, 0);
            break;
    }
}

extern "C" void NuDisplayListPerfEndEvent(void) {
    D3DPERF_EndEvent();
}

extern "C" PC_FRAMEBUFFER *PcGetDefaultFramebuffer(void) {
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
            NuDisplayHandlerZMode = NUMTL_ZMODE_READ_ONLY;
            PCDeviceSetRenderState(D3DRS_ZENABLE, 1);
            PCDeviceSetRenderState(D3DRS_ZWRITEENABLE, 0);
            PCDeviceSetRenderState(D3DRS_ZFUNC, 4);
            NuShaderManagerSetfv(SHADER_UNIFORM_K_TINT, &tint.x);
        } else {
            NUMTL *material = NuDisplayHandlerMaterial;
            PCDisplayHandlerSetZMode((material->attribs.word >> 14) & 3);
            NuShaderManagerSetfv(SHADER_UNIFORM_K_TINT, &tint.x);
        }
        if (PCDeviceHardwareVertexProcessingPC == PC_SHADER_MODEL_3_0) {
            NuDisplayHandlerOpacity = opacity;
        }
    }
    if ((f64)shadow != (f64)PCGraphicsFrameState) {
        NuDisplayHandlerShadow = shadow;
        NuShaderManagerSetfv(SHADER_UNIFORM_SHADOW_CASTING_OBJECT, &NuDisplayHandlerShadow);
        PCGraphicsFrameState = (i32)shadow;
    }
    matrix->m33 = 1.0f;
    matrix->m23 = 0.0f;
    NuShaderManagerSetfv(SHADER_UNIFORM_WORLD, &matrix->m00);
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
            NuDisplayHandlerZMode = NUMTL_ZMODE_READ_ONLY;
            PCDeviceSetRenderState(D3DRS_ZENABLE, 1);
            PCDeviceSetRenderState(D3DRS_ZWRITEENABLE, 0);
            PCDeviceSetRenderState(D3DRS_ZFUNC, 4);
            NuShaderManagerSetfv(SHADER_UNIFORM_K_TINT, &tint.x);
        } else {
            NUMTL *material = NuDisplayHandlerMaterial;
            if (material != NULL) {
                PCDisplayHandlerSetZMode((material->attribs.word >> 14) & 3);
                NuShaderManagerSetfv(SHADER_UNIFORM_K_TINT, &tint.x);
            }
        }
        if (PCDeviceHardwareVertexProcessingPC == PC_SHADER_MODEL_3_0) {
            NuDisplayHandlerOpacity = opacity;
        }
    }
    if ((f64)shadow != (f64)PCGraphicsFrameState) {
        NuDisplayHandlerShadow = shadow;
        NuShaderManagerSetfv(SHADER_UNIFORM_SHADOW_CASTING_OBJECT, &NuDisplayHandlerShadow);
        PCGraphicsFrameState = (i32)shadow;
    }
    matrix->m33 = 1.0f;
    matrix->m32 = 0.0f;
    NuShaderManagerSetElementsfv_transpose(SHADER_UNIFORM_WORLD, 0, 1, &matrix->m00);
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
    NUMTX *matrix = packet.mtx_ptr;
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
    values.x = *packet.f32_ptr;
    packet.char_ptr += sizeof(f32);
    values.y = 1.0f;
    values.z = values.w = 0.0f;
    PCDeviceSetVertexShaderConstantsF(0x54, &values.x, 1);
    NuMtxTranspose(packet.mtx_ptr, packet.mtx_ptr);
    PCDeviceSetVertexShaderConstantsF(0x55, packet.f32_ptr, 4);
    tint = NuDisplayHandlerTint;
    if (opacity < 1.0f) {
        tint.w *= opacity;
        NuDisplayHandlerZMode = NUMTL_ZMODE_READ_ONLY;
        PCDeviceSetRenderState(D3DRS_ZENABLE, 1);
        PCDeviceSetRenderState(D3DRS_ZWRITEENABLE, 0);
        PCDeviceSetRenderState(D3DRS_ZFUNC, 4);
        /* The count is 4 registers despite only four initialized tint floats. */
        PCDeviceSetVertexShaderConstantsF(0x28, &tint.x, 4);
    } else {
        NUMTL *material = NuDisplayHandlerMaterial;
        PCDisplayHandlerSetZMode((material->attribs.word >> 14) & 3);
        PCDeviceSetVertexShaderConstantsF(0x28, &tint.x, 4);
    }
}

extern "C" {
    static void PCGraphicsBindStageTexture(u32 stage, GRAPHICS_RENDER_TEXTURE *texture) {
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

static void PCDisplayHandlerBindPrograms(struct POST_SHADER_PROGRAM *programs);

static void PCDisplayHandlerBindPrograms(struct POST_SHADER_PROGRAM *programs) {
    if (programs != NULL) {
        if (NuDisplayHandlerVertexShaderCache != programs->vertex_shader) {
            PCDeviceSetVertexShader(programs->vertex_shader);
            NuDisplayHandlerVertexShaderCache = programs->vertex_shader;
        }
        if (NuDisplayHandlerPixelShaderCache != programs->pixel_shader) {
            PCDeviceSetPixelShader(programs->pixel_shader);
            NuDisplayHandlerPixelShaderCache = programs->pixel_shader;
        }
    } else {
        PCDeviceSetVertexShader(NULL);
        PCDeviceSetPixelShader(NULL);
        NuDisplayHandlerVertexShaderCache = NULL;
        NuDisplayHandlerPixelShaderCache = NULL;
    }
}

extern "C" void NuDlistCopyChar(char *dst, const char *src) {
    *dst = *src;
}

extern "C" u32 PCStrLen(const char *text) {
    return strlen(text);
}

extern "C" char *PCMemCpyS(char *dst, u32 size, const char *src, u32 count) {
    memcpy_s(dst, size, src, count);
    return dst;
}

extern "C" char *PCMemMoveS(char *dst, u32 size, const char *src, u32 count) {
    memmove_s(dst, size, src, count);
    return dst;
}

extern "C" void PCDisplayHandlerSetZModeHook(i32 mode) {
    PCDisplayHandlerSetZMode(mode);
}

extern "C" void NuDisplayListMaterial(void *arg) {
    NUMTL *material = (NUMTL *)arg;
    NuShaderObject *shader = (NuShaderObject *)PCShaderRecordFromHandle((u32)(i32)material->shader.shader_handles[0]);
    NUMTX transposed;
    NuDisplayMaterialLast = material;
    NuDisplayHandlerMaterial = material;
    PCGraphicsBoundShader = -1;
    PCGraphicsFrameState = -1;
    if ((material->shader.vertex_format & 0x100000) != 0) {
        PCDeviceSetVertexShader(NULL);
        PCDeviceSetPixelShader(NULL);
        NuDisplayHandlerVertexShaderCache = NULL;
        NuDisplayHandlerPixelShaderCache = NULL;
        NuShaderManagerBindShader(NULL);
        PCDeviceSetVertexDeclaration(NuDisplayMaterialDebrisDeclaration);
        PCDisplayHandlerBindPrograms(material->material_kind == 0x97 ? NuDisplayMaterialGlassPrograms : NuDisplayMaterialDebrisPrograms);
        if (PcWindowOptions.usehires == 0 || PCDeviceHardwareVertexProcessingPC != PC_SHADER_MODEL_3_0) {
            NuMtxTranspose(&transposed, &PcGeometryViewProjectionMatrix);
            PCDeviceSetVertexShaderConstantsF(0, &transposed.m00, 4);
            NuMtxTranspose(&transposed, &PcGeometryViewMatrix);
            PCDeviceSetVertexShaderConstantsF(0xc, &transposed.m00, 4);
            PCDeviceSetVertexShaderConstantsF(0x30, ShaderUniformRecords[69].values, 1);
        }
        PCDeviceSetPixelShaderConstantsF(0, ShaderUniformRecords[68].values, 1);
        if (material->material_kind == 0x97) {
            GRAPHICS_RENDER_TEXTURE *target = NuPostEffectGetBackBuffer(-1);
            if (target != NULL) {
                PCDeviceSetTexture(0, target->resource);
            } else {
                PCDeviceSetTexture(0, NULL);
            }
            PCDeviceSetSamplerState(0, D3DSAMP_ADDRESSU, 3);
            PCDeviceSetSamplerState(0, D3DSAMP_ADDRESSV, 3);
            PCDeviceSetSamplerState(0, D3DSAMP_MIPFILTER, 0);
            PCDeviceSetTexture(1, NuTextureGetDimensions(NuDisplayMaterialDistortTexture)->resource);
        } else if (material->shader.diffuse_map_ids[0] != 0) {
            PCDeviceSetTexture(0, NuTextureGetDimensions(material->shader.diffuse_map_ids[0])->resource);
        }
    } else if ((material->shader.vertex_format & 0x200000) != 0) {
        PCShaderSetMaterialParameters(shader, material);
        NuShaderManagerBindShader(NULL);
        PCDeviceSetVertexShader(NULL);
        PCDeviceSetPixelShader(NULL);
        NuDisplayHandlerVertexShaderCache = NULL;
        NuDisplayHandlerPixelShaderCache = NULL;
        PCDeviceSetVertexDeclaration(NuDisplayMaterialFaceOnDeclaration);
        PCDisplayHandlerBindPrograms(PCDeviceBackend.textures[1] != NULL && (material->shader.shadow_set & 0xf) != 0
                                         ? NuDisplayMaterialFaceOnDecalPrograms
                                         : NuDisplayMaterialFaceOnPrograms);
        if (PCDeviceHardwareVertexProcessingPC != PC_SHADER_MODEL_3_0) {
            NuMtxTranspose(&transposed, &PcGeometryViewProjectionMatrix);
            PCDeviceSetVertexShaderConstantsF(0, &transposed.m00, 4);
            NuMtxTranspose(&transposed, &PcGeometryViewMatrix);
            PCDeviceSetVertexShaderConstantsF(0xc, &transposed.m00, 4);
            PCDeviceSetVertexShaderConstantsF(0x30, ShaderUniformRecords[69].values, 1);
        }
        PCDeviceSetPixelShaderConstantsF(0, ShaderUniformRecords[68].values, 1);
        if (material->shader.diffuse_map_ids[0] != 0) {
            PCDeviceSetTexture(0, NuTextureGetDimensions(material->shader.diffuse_map_ids[0])->resource);
        }
    } else if (material->display_list != NULL) {
        if (shader != NULL) {
            PCShaderSetMaterialParameters(shader, material);
            NuShaderManagerBindShader(shader);
            PCDeviceSetVertexDeclaration(material->vertex_declaration);
            if (RenderFrameSceneFlags != 0) {
                PCDeviceSetPixelShader(NULL);
            }
        } else {
            PCGraphicsRenderTextureAllocationError();
            NuShaderManagerBindShader(NULL);
        }
    } else if (shader != NULL) {
        PCShaderSetMaterialParameters(shader, material);
        NuShaderManagerBindShader(shader);
        PCDeviceSetVertexDeclaration(material->vertex_declaration);
    } else {
        NuShaderManagerBindShader(NULL);
        PCDeviceSetVertexDeclaration(NuDisplayMaterialDefaultDeclaration);
        PCDisplayHandlerBindPrograms(NuDisplayMaterialDefaultPrograms);
    }
    NuDisplayHandlerReflection = 0;
    NuMtlSetRenderStatesPS(material);
    if ((shader->stage_flags & 6) == 0) {
        PCDeviceBackend.SetColorWriteMask(1, 1, 1);
    } else {
        PCDeviceBackend.SetColorWriteMask(0, 0, 1);
    }
    u32 write_mask = ~shader->stage_flags & 8;
    write_mask >>= 3;
    PCDeviceBackend.SetColorWriteMask(write_mask, write_mask, 3);
    if (PCDeviceHardwareVertexProcessingPC != PC_SHADER_MODEL_3_0 && PCShaderCurrentObject() != NULL) {
        if (PCShaderCurrentObject() != NULL && (PCShaderCurrentObject())->parameters[0x4f].register_index >= 0) {
            for (i32 i = 0; i < 32; ++i) {
                NUVEC4 group;
                group.x = 1.0f;
                group.y = 1.0f;
                group.z = 1.0f;
                group.w = 1.0f;
                NuShaderManagerSetElementfv(SHADER_UNIFORM_VERTEX_GROUP_STATES, i, &group.x);
            }
        }
        i32 count = nuspecial_vertex_noffsets;
        if (count != 0) {
            if (count >= 8) {
                count = 8;
            }
            NuShaderObjectSetElementsfv(PCShaderCurrentObject(), SHADER_UNIFORM_OFFSET_TABLE, 0, count, &nuspecial_vertex_offsets[0][0]);
        }
    }
}

/* PCDisplayHandlerSetZMode is the real private helper already in the BC cohort. */
extern "C" void NuMtlSetRenderStatesPS(NUMTL *material) {
    i32 write_colour = 1;
    i32 write_alpha = 1;
    u32 alpha;
    PCDisplayHandlerSetZMode((material->attribs.word >> 14) & 3);
    if (material->shader.vertex_format & 0x100000) {
        PCDeviceSetRenderState(D3DRS_ALPHATESTENABLE, 1);
        PCDeviceSetRenderState(D3DRS_ALPHAREF, 2);
        PCDeviceSetRenderState(D3DRS_ALPHAFUNC, 5);
    } else {
        alpha = (material->attribs.word >> 20) & 7;
        switch (alpha) {
            case 0:
            case 1:
                PCDeviceSetRenderState(D3DRS_ALPHATESTENABLE, 0);
                break;
            case 5:
                PCDeviceSetRenderState(D3DRS_ALPHATESTENABLE, 1);
                PCDeviceSetRenderState(D3DRS_ALPHAREF, (material->attribs.word >> 23) & 0xff);
                PCDeviceSetRenderState(D3DRS_ALPHAFUNC, 5);
                break;
            default:
                PCDeviceSetRenderState(D3DRS_ALPHATESTENABLE, 1);
                PCDeviceSetRenderState(D3DRS_ALPHAREF, 0);
                PCDeviceSetRenderState(D3DRS_ALPHAFUNC, 5);
                break;
        }
    }
    switch (material->attribs.word & 15) {
        case NUMTL_ALPHA_OPAQUE:
            PCDeviceSetRenderState(D3DRS_ALPHABLENDENABLE, 0);
            break;
        case NUMTL_ALPHA_BLEND:
            PCDeviceSetRenderState(D3DRS_ALPHABLENDENABLE, 1);
            PCDeviceSetRenderState(D3DRS_BLENDOP, 1);
            PCDeviceSetRenderState(D3DRS_SRCBLEND, 5);
            PCDeviceSetRenderState(D3DRS_DESTBLEND, 6);
            break;
        case NUMTL_ALPHA_ADDITIVE:
            PCDeviceSetRenderState(D3DRS_ALPHABLENDENABLE, 1);
            PCDeviceSetRenderState(D3DRS_BLENDOP, 1);
            PCDeviceSetRenderState(D3DRS_SRCBLEND, 5);
            PCDeviceSetRenderState(D3DRS_DESTBLEND, 2);
            break;
        case NUMTL_ALPHA_SUBTRACTIVE:
            PCDeviceSetRenderState(D3DRS_ALPHABLENDENABLE, 1);
            PCDeviceSetRenderState(D3DRS_BLENDOP, 3);
            PCDeviceSetRenderState(D3DRS_SRCBLEND, 1);
            PCDeviceSetRenderState(D3DRS_DESTBLEND, 6);
            break;
        case NUMTL_ALPHA_TEST:
            PCDeviceSetRenderState(D3DRS_ALPHABLENDENABLE, 0);
            PCDeviceSetRenderState(D3DRS_ALPHATESTENABLE, 1);
            PCDeviceSetRenderState(D3DRS_ALPHAREF, (material->attribs.word >> 23) & 0xff);
            PCDeviceSetRenderState(D3DRS_ALPHAFUNC, 7);
            break;
        case NUMTL_ALPHA_NO_COLOUR:
            write_colour = 0;
            break;
    }
    switch ((material->attribs.word >> 12) & 3) {
        case NUMTL_CULL_BACK:
            if (NuDisplayHandlerReflection != 0) {
                PCDeviceSetRenderState(D3DRS_CULLMODE, 2);
            } else {
                PCDeviceSetRenderState(D3DRS_CULLMODE, 3);
            }
            break;
        case NUMTL_CULL_FRONT:
            if (NuDisplayHandlerReflection != 0) {
                PCDeviceSetRenderState(D3DRS_CULLMODE, 3);
            } else {
                PCDeviceSetRenderState(D3DRS_CULLMODE, 2);
            }
            break;
        case NUMTL_CULL_NONE:
            PCDeviceSetRenderState(D3DRS_CULLMODE, 1);
            break;
    }
    if (!(material->attribs.word & 0x80000)) {
        write_alpha = 0;
    }
    if (RenderFrameSceneFlags != 0) {
        PCDeviceBackend.SetColorWriteMask(1, 1, 0);
        NuDisplayHandlerZMode = NUMTL_ZMODE_NORMAL;
        PCDeviceSetRenderState(D3DRS_ZENABLE, 1);
        PCDeviceSetRenderState(D3DRS_ZWRITEENABLE, 1);
        PCDeviceSetRenderState(D3DRS_ZFUNC, 4);
    } else {
        PCDeviceBackend.SetColorWriteMask(write_colour == 0, write_alpha == 0, 0);
    }
    NuDisplayHandlerTint = NuDisplayHandlerDefaultTint;
    NuShaderManagerSetfv(SHADER_UNIFORM_K_TINT, &NuDisplayHandlerDefaultTint.x);
    NuDisplayHandlerOpacity = -1.0f;
}

// NuShaderProgramMakeCurrent and the per-stage sampler binds, inlined here.
// One vertex-shader tap offset.
struct __declspec(align(16)) POST_BLUR_LOW_OFFSET {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

static __forceinline void PostBlurLowSetOffset(POST_BLUR_LOW_OFFSET &offset, f32 x, f32 y) {
    offset.x = x;
    offset.y = y;
}

extern "C" void PostBlur7x7Low(GRAPHICS_RENDER_TEXTURE *source, i32 source_lod, GRAPHICS_RENDER_TEXTURE *textures, i32 first_lod,
                               i32 iterations, i32 levels, bool mip_chain, f32 radius, POST_SHADER_PROGRAM *program) {
    f32 weights[4] = {.25f, .25f, .25f, .25f};
    f32 rounded_radius = (f32)ceil((f64)radius);
    i32 guard = (i32)rounded_radius * 3;
    PC_VIEWPORT viewport;
    i32 work_width = PCGraphicsPostEffectWorkTexture->width;
    i32 work_height = PCGraphicsPostEffectWorkTexture->height;
    PCDisplayHandlerBindPrograms(program);
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
            PostBlurLowSetOffset(horizontal[0], dx * -.5, dy);
            PostBlurLowSetOffset(vertical[0], dx * .5, dy);
            PostBlurLowSetOffset(horizontal[1], dx, dy * .5);
            PostBlurLowSetOffset(vertical[1], dx, dy * -.5);
            PostBlurLowSetOffset(horizontal[2], dx * .5, dy * -1.0);
            PostBlurLowSetOffset(vertical[2], dx * -.5, dy * -1.0);
            PostBlurLowSetOffset(horizontal[3], dx * -1.0, dy * .5);
            PostBlurLowSetOffset(vertical[3], dx * -1.0, dy * -.5);
            f32 bias[4] = {1, 1, PostBlurInputBias / (f32)width, PostBlurInputBias / (f32)height};
            f32 output_bias[4] = {(f32)output_width / (f32)work_width, (f32)output_height / (f32)work_height,
                                  PostBlurOutputBias / (f32)work_width, PostBlurOutputBias / (f32)work_height};
            {
                PC_FRAMEBUFFER *framebuffer = PostBlurFramebuffer;
                framebuffer->targets[0].Attach((PCGraphicsPostEffectWorkTexture), 0, false);
                framebuffer->changed = 1;
            }
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
            if ((u32)PCDeviceSamplerInvalidation[0][0] != input->flags || (u32)PCDeviceSamplerInvalidation[0][1] != input->sampler_extra) {
                PCGraphicsSetSamplerTuple(0, &input->flags);
            }
            PCDeviceSetTexture(0, input->resource);
            if ((u32)PCDeviceSamplerInvalidation[1][0] != input->flags || (u32)PCDeviceSamplerInvalidation[1][1] != input->sampler_extra) {
                PCGraphicsSetSamplerTuple(1, &input->flags);
            }
            PCDeviceSetTexture(1, input->resource);
            if ((u32)PCDeviceSamplerInvalidation[2][0] != input->flags || (u32)PCDeviceSamplerInvalidation[2][1] != input->sampler_extra) {
                PCGraphicsSetSamplerTuple(2, &input->flags);
            }
            PCDeviceSetTexture(2, input->resource);
            if ((u32)PCDeviceSamplerInvalidation[3][0] != input->flags || (u32)PCDeviceSamplerInvalidation[3][1] != input->sampler_extra) {
                PCGraphicsSetSamplerTuple(3, &input->flags);
            }
            PCDeviceSetTexture(3, input->resource);
            PCDeviceBackend.DrawPostEffectQuad();
            {
                POST_SHADER_PROGRAM *bound = (PostBlurAuxProgram);
                if (bound != NULL) {
                    if (NuDisplayHandlerVertexShaderCache != bound->vertex_shader) {
                        PCDeviceSetVertexShader(bound->vertex_shader);
                        NuDisplayHandlerVertexShaderCache = bound->vertex_shader;
                    }
                    if (NuDisplayHandlerPixelShaderCache != bound->pixel_shader) {
                        PCDeviceSetPixelShader(bound->pixel_shader);
                        NuDisplayHandlerPixelShaderCache = bound->pixel_shader;
                    }
                } else {
                    PCDeviceSetVertexShader(NULL);
                    PCDeviceSetPixelShader(NULL);
                    NuDisplayHandlerVertexShaderCache = NULL;
                    NuDisplayHandlerPixelShaderCache = NULL;
                }
            }
            if ((u32)PCDeviceSamplerInvalidation[0][0] != input->flags || (u32)PCDeviceSamplerInvalidation[0][1] != input->sampler_extra) {
                PCGraphicsSetSamplerTuple(0, &input->flags);
            }
            PCDeviceSetTexture(0, input->resource);
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
            {
                PC_FRAMEBUFFER *framebuffer = PostBlurFramebuffer;
                framebuffer->targets[0].Attach(output, output_lod, false);
                framebuffer->changed = 1;
            }
            PCGraphicsBindFramebuffer(PostBlurFramebuffer);
            viewport.x = 0;
            viewport.y = 0;
            viewport.width = output_width;
            viewport.height = output_height;
            viewport.min_z = 0;
            viewport.max_z = 1;
            NuVpSubmit(&viewport);
            {
                POST_SHADER_PROGRAM *bound = program;
                if (bound != NULL) {
                    if (NuDisplayHandlerVertexShaderCache != bound->vertex_shader) {
                        PCDeviceSetVertexShader(bound->vertex_shader);
                        NuDisplayHandlerVertexShaderCache = bound->vertex_shader;
                    }
                    if (NuDisplayHandlerPixelShaderCache != bound->pixel_shader) {
                        PCDeviceSetPixelShader(bound->pixel_shader);
                        NuDisplayHandlerPixelShaderCache = bound->pixel_shader;
                    }
                } else {
                    PCDeviceSetVertexShader(NULL);
                    PCDeviceSetPixelShader(NULL);
                    NuDisplayHandlerVertexShaderCache = NULL;
                    NuDisplayHandlerPixelShaderCache = NULL;
                }
            }
            PCDeviceSetVertexShaderConstantsF(0, &vertical[0].x, 4);
            PCDeviceSetVertexShaderConstantsF(4, output_bias, 1);
            PCDeviceSetPixelShaderConstantsF(0, weights, 1);
            output->flags = (((output->flags & 0xfca5dbf0) | (output_lod & 15)) & 0xffffff0f) | ((output_lod & 15) << 4) | 0xa41b00;
            PCGraphicsPostEffectWorkTexture->flags =
                (((PCGraphicsPostEffectWorkTexture->flags & 0xfffffbff) | 0x300) & 0xffffdfff) | 0x1800;
            PCGraphicsPostEffectWorkTexture->flags =
                (((PCGraphicsPostEffectWorkTexture->flags & 0xfff5ffff) | 0x40000) & 0xfc2fffff) | 0x200000;
            PCGraphicsBindStageTexture(0, PCGraphicsPostEffectWorkTexture);
            PCGraphicsBindStageTexture(1, PCGraphicsPostEffectWorkTexture);
            PCGraphicsBindStageTexture(2, PCGraphicsPostEffectWorkTexture);
            PCGraphicsBindStageTexture(3, PCGraphicsPostEffectWorkTexture);
            PCDeviceBackend.DrawPostEffectQuad();
            input = output;
            input_lod = output_lod;
        }
    }
}

extern "C" void PostBlur7x7Dispatch(GRAPHICS_RENDER_TEXTURE *source, i32 source_lod, GRAPHICS_RENDER_TEXTURE *textures, i32 first_lod,
                                    i32 iterations, i32 levels, bool mip_chain, f32 radius, POST_SHADER_PROGRAM *program) {
    if (PCDeviceHardwareVertexProcessingPC >= 2) {
        PostBlur7x7High(source, source_lod, textures, first_lod, iterations, levels, mip_chain, radius, program);
    } else {
        PostBlur7x7Low(source, source_lod, textures, first_lod, iterations, levels, mip_chain, radius, PostCopyAuxProgram);
    }
}

extern "C" {
    f32 PostBlurInputBias = 0;
    f32 RenderFrameShaderTime = 0.0f;
    f32 PostBlurDirectOutputBias = 0;
    f32 PostBlurDirectInputBias = 0;
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
        PCDisplayHandlerBindPrograms(PostCopyTexProgram);
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
    POST_SHADER_PROGRAM *program;
    if (bloom->directional) {
        program = output == PcBootSecondaryFramebuffer && PCDeviceBackend.textures[0] != NULL ? programs[8] : platform_programs[1];
    } else {
        program = output == PcBootSecondaryFramebuffer && PCDeviceBackend.textures[0] != NULL ? programs[6] : platform_programs[0];
    }
    PCDisplayHandlerBindPrograms(program);
    if (PCDeviceHardwareVertexProcessingPC < 4) {
        NUMTX view_inverse;
        NuMtxInvH(&view_inverse, &PcGeometryViewMatrix);
        PCDeviceSetVertexShaderConstantsF(12, &view_inverse.m00, 4);
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
        NuVecNorm(&direction.xyz, &direction.xyz);
        direction.w = 1.0f;
        direction.w = bloom->direction_bias;
        PCDeviceSetVertexShaderConstantsF(platform_uniforms[3], &direction.x, 1);
    }
    if (output == PcBootSecondaryFramebuffer && PCDeviceBackend.textures[0] != NULL) {
        color_texture->flags = (color_texture->flags & 0xfc25ffff) | 0x240000;
        PCGraphicsBindStageTexture(0, color_texture);
    }
    PCGraphicsPostEffectBackBufferTexture->flags =
        ((PCGraphicsPostEffectBackBufferTexture->flags & 0xfff5ffff) | 0x40000) & 0xfc2fffff | 0x200000;
    PCGraphicsPostEffectBackBufferTexture->flags =
        ((PCGraphicsPostEffectBackBufferTexture->flags & 0xfffffbff) | 0x300) & 0xffffdfff | 0x1800;
    PCGraphicsBindStageTexture(3, PCGraphicsPostEffectBackBufferTexture);
    PCDeviceSetRenderState(D3DRS_CULLMODE, 1);
    PCDeviceSetRenderState(D3DRS_ALPHATESTENABLE, 0);
    PCDeviceSetRenderState(D3DRS_ALPHABLENDENABLE, 0);
    PCDeviceSetRenderState(D3DRS_ZENABLE, 0);
    PCDeviceSetRenderState(D3DRS_COLORWRITEENABLE, 15);
    if (output == PcBootSecondaryFramebuffer && PCDeviceBackend.textures[0] != NULL) {
        PCDeviceSetRenderState(D3DRS_ALPHABLENDENABLE, 0);
        PCDeviceSetRenderState(D3DRS_ALPHATESTENABLE, 0);
    } else {
        PCDeviceSetRenderState(D3DRS_ALPHABLENDENABLE, 1);
        PCDeviceSetRenderState(D3DRS_SRCBLEND, 2);
        PCDeviceSetRenderState(D3DRS_DESTBLEND, 2);
        PCDeviceSetRenderState(D3DRS_BLENDOP, 1);
        PCDeviceSetRenderState(D3DRS_ALPHATESTENABLE, 0);
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
    PCDeviceStretchRect(surface.surface, NULL, output->targets[0].surface, NULL, scaled ? 2 : 0);
}

/* Same unit as ??_GNuMainFilter: the atexit thunks for its static surfaces expand the inline destructor. */
extern "C" void PostFilterCopyTexture(GRAPHICS_RENDER_TEXTURE *destination, i32 destination_level, GRAPHICS_RENDER_TEXTURE *source,
                                      i32 source_level, POST_SHADER_PROGRAM *program, GRAPHICS_RENDER_TEXTURE *extra) {
    if (program == PostCopyTexProgram) {
        static PC_EFFECT_SURFACE source_surface;
        static PC_EFFECT_SURFACE destination_surface;
        source_surface.Attach(source, source_level, false);
        destination_surface.Attach(destination, destination_level, false);
        bool scaled = destination_surface.width != source_surface.width || destination_surface.height != source_surface.height;
        PCDeviceStretchRect(source_surface.surface, NULL, destination_surface.surface, NULL, scaled ? 2 : 0);
        return;
    }
    PostFilterBlitTexture(destination, destination_level, source, source_level, program, extra);
}

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
    i32 NuDisplayHandlerZMode = -1;
    NUVEC4 NuDisplayHandlerDefaultTint = {1.0f, 1.0f, 1.0f, 1.0f};
    /* Output half-texel term of PostBlur7x7Low (PostBlurInputBias is its input term). */
    f32 PostBlurOutputBias = 1.0f;
}
f32 NuSpeedBlurVertexScale = 40000.0f;

void NuSpeedBlurFilter::render() {
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
    PCDeviceSetRenderState(D3DRS_CULLMODE, 1);
    PCDeviceSetRenderState(D3DRS_ALPHATESTENABLE, 0);
    PCDeviceSetRenderState(D3DRS_ALPHABLENDENABLE, 0);
    PCDeviceSetRenderState(D3DRS_ZENABLE, 0);
    PCDeviceSetRenderState(D3DRS_COLORWRITEENABLE, 15);
    PCGraphicsBindFramebuffer(output);
    if (output == PcBootSecondaryFramebuffer && PCDeviceBackend.textures[0] != NULL) {
        color_texture->flags = (color_texture->flags & 0xfc13ffff) | 0x120000;
        PCDisplayHandlerBindPrograms(PostCopyTexProgram);
        PCGraphicsBindStageTexture(0, color_texture);
        PCDeviceBackend.DrawPostEffectQuad();
    }
    PCDisplayHandlerBindPrograms(programs[0]);
    PCDeviceSetVertexShaderConstantsF(platform_uniform, constants, 2);
    PCGraphicsPostEffectBackBufferTexture->flags =
        ((PCGraphicsPostEffectBackBufferTexture->flags & 0xfff5ffff) | 0x40000) & 0xfc2fffff | 0x200000;
    PCGraphicsPostEffectBackBufferTexture->flags =
        ((PCGraphicsPostEffectBackBufferTexture->flags & 0xfffffbff) | 0x300) & 0xffffdfff | 0x1800;
    PCGraphicsBindStageTexture(0, PCGraphicsPostEffectBackBufferTexture);
    if (PCDeviceHardwareVertexProcessingPC <= 1) {
        PCGraphicsBindStageTexture(1, PCGraphicsPostEffectBackBufferTexture);
        PCGraphicsBindStageTexture(2, PCGraphicsPostEffectBackBufferTexture);
        PCGraphicsBindStageTexture(3, PCGraphicsPostEffectBackBufferTexture);
    }
    PCDeviceSetRenderState(D3DRS_ALPHABLENDENABLE, 1);
    PCDeviceSetRenderState(D3DRS_ALPHATESTENABLE, 0);
    PCDeviceSetRenderState(D3DRS_SRCBLEND, 5);
    PCDeviceSetRenderState(D3DRS_DESTBLEND, 6);
    PCDeviceSetRenderState(D3DRS_BLENDOP, 1);
    PCDeviceBackend.DrawPostEffectIndexed();
    PCDeviceSetRenderState(D3DRS_CULLMODE, 1);
    PCDeviceSetRenderState(D3DRS_ALPHATESTENABLE, 0);
    PCDeviceSetRenderState(D3DRS_ALPHABLENDENABLE, 0);
    PCDeviceSetRenderState(D3DRS_ZENABLE, 0);
    PCDeviceSetRenderState(D3DRS_COLORWRITEENABLE, 15);
}
