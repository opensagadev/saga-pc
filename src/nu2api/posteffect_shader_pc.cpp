#include "saga.h"
#include <vector>

struct POST_VERTEX_SHADER_CACHE_ENTRY_PC {
    PC_TEXTURE_RESOURCE_PC *shader;
    const DWORD *tokens;
};
struct POST_PIXEL_SHADER_CACHE_ENTRY_PC {
    PC_TEXTURE_RESOURCE_PC *shader;
    const DWORD *tokens;
};
struct POST_SHADER_PROGRAM_PC {
    PC_TEXTURE_RESOURCE_PC *vertex_shader;
    PC_TEXTURE_RESOURCE_PC *pixel_shader;
    u32 unknown_08[2];
};
struct POST_SHADER_PROGRAM_POOL_PC {
    POST_SHADER_PROGRAM_PC programs[64];
    u8 occupied[8];
    i32 cursor;
    POST_SHADER_PROGRAM_PC *__thiscall Allocate(void);
};
STATIC_ASSERT(sizeof(POST_VERTEX_SHADER_CACHE_ENTRY_PC) == 8);
STATIC_ASSERT(offsetof(POST_VERTEX_SHADER_CACHE_ENTRY_PC, tokens) == 4);
STATIC_ASSERT(sizeof(POST_PIXEL_SHADER_CACHE_ENTRY_PC) == 8);
STATIC_ASSERT(offsetof(POST_PIXEL_SHADER_CACHE_ENTRY_PC, tokens) == 4);
STATIC_ASSERT(sizeof(std::vector<POST_VERTEX_SHADER_CACHE_ENTRY_PC>) == 16);
STATIC_ASSERT(sizeof(std::vector<POST_PIXEL_SHADER_CACHE_ENTRY_PC>) == 16);
STATIC_ASSERT(sizeof(POST_SHADER_PROGRAM_PC) == 16);
STATIC_ASSERT(offsetof(POST_SHADER_PROGRAM_POOL_PC, occupied) == 0x400);
STATIC_ASSERT(offsetof(POST_SHADER_PROGRAM_POOL_PC, cursor) == 0x408);
STATIC_ASSERT(sizeof(POST_SHADER_PROGRAM_POOL_PC) == 0x40c);
extern "C" {
#pragma bss_seg(".bss$PostShaderProgramPoolPc")
    POST_SHADER_PROGRAM_POOL_PC PostShaderProgramPoolPc;
#pragma bss_seg(".bss$PostVertexShaderCachePc")
    std::vector<POST_VERTEX_SHADER_CACHE_ENTRY_PC> PostVertexShaderCachePc;
#pragma bss_seg(".bss$PostPixelShaderCachePc")
    std::vector<POST_PIXEL_SHADER_CACHE_ENTRY_PC> PostPixelShaderCachePc;
#pragma bss_seg()
}

POST_SHADER_PROGRAM_PC *__thiscall POST_SHADER_PROGRAM_POOL_PC::Allocate(void) {
    i32 index = cursor;
    for (; index < 64; ++index) {
        if (((i8)occupied[index >> 3] >> (index & 7) & 1) == 0)
            break;
    }
    if (index >= 64) {
        for (index = 0; index < cursor; ++index) {
            if (((i8)occupied[index >> 3] >> (index & 7) & 1) == 0)
                break;
        }
        if (index >= cursor)
            return NULL;
    }
    occupied[index >> 3] |= (u8)(1 << (index & 7));
    cursor = (index + 1) % 64;
    return &programs[index];
}

extern "C" POST_SHADER_PROGRAM_PC *__cdecl PostShaderProgramCreatePc(i32 mode, const DWORD *vertex_tokens,
                                                                     const DWORD *pixel_tokens) {
    POST_VERTEX_SHADER_CACHE_ENTRY_PC vertex = {NULL, vertex_tokens};
    POST_PIXEL_SHADER_CACHE_ENTRY_PC pixel = {NULL, pixel_tokens};
    std::vector<POST_VERTEX_SHADER_CACHE_ENTRY_PC>::iterator vertex_it = PostVertexShaderCachePc.begin();
    for (; vertex_it != PostVertexShaderCachePc.end(); ++vertex_it) {
        if (vertex_it->tokens == vertex_tokens) {
            vertex = *vertex_it;
            break;
        }
    }
    if (vertex.shader == NULL) {
        PCDeviceCreateVertexShader(vertex.tokens, &vertex.shader);
        PostVertexShaderCachePc.push_back(vertex);
    }
    std::vector<POST_PIXEL_SHADER_CACHE_ENTRY_PC>::iterator pixel_it = PostPixelShaderCachePc.begin();
    for (; pixel_it != PostPixelShaderCachePc.end(); ++pixel_it) {
        if (pixel_it->tokens == pixel_tokens) {
            pixel = *pixel_it;
            break;
        }
    }
    if (pixel.shader == NULL) {
        PCDeviceCreatePixelShader(pixel.tokens, &pixel.shader);
        PostPixelShaderCachePc.push_back(pixel);
    }
    if (vertex.shader != NULL && pixel.shader != NULL) {
        POST_SHADER_PROGRAM_PC *program = PostShaderProgramPoolPc.Allocate();
        program->vertex_shader = vertex.shader;
        program->pixel_shader = pixel.shader;
        return program;
    }
    return NULL;
}

extern "C" void __cdecl PostFilterInitSharedResourcesPc(void);
extern "C" {
    extern i32 PCDeviceHardwareVertexProcessingPC;
    extern const DWORD PostShaderTokens7656f0[];
    extern const DWORD PostShaderTokens765798[];
    extern const DWORD PostShaderTokens765848[];
    extern const DWORD PostShaderTokens765aa8[];
    extern const DWORD PostShaderTokens765ca0[];
    extern const DWORD PostShaderTokens765d90[];
    extern const DWORD PostShaderTokens765e40[];
    extern const DWORD PostShaderTokens765f30[];
    extern const DWORD PostShaderTokens7660d0[];
#pragma bss_seg(".bss$PostCopyTexProgramPc")
    POST_SHADER_PROGRAM_PC *PostCopyTexProgramPc;
#pragma bss_seg(".bss$PostCopyTexLodProgramPc")
    POST_SHADER_PROGRAM_PC *PostCopyTexLodProgramPc;
#pragma bss_seg(".bss$PostBlendTexProgramPc")
    POST_SHADER_PROGRAM_PC *PostBlendTexProgramPc;
#pragma bss_seg(".bss$PostBlur5x5ProgramPc")
    POST_SHADER_PROGRAM_PC *PostBlur5x5ProgramPc;
#pragma bss_seg(".bss$PostBlur7x7ProgramPc")
    POST_SHADER_PROGRAM_PC *PostBlur7x7ProgramPc;
#pragma bss_seg(".bss$PostCopyAuxProgramPc")
    POST_SHADER_PROGRAM_PC *PostCopyAuxProgramPc;
#pragma bss_seg(".bss$PostBlurAuxProgramPc")
    POST_SHADER_PROGRAM_PC *PostBlurAuxProgramPc;
#pragma bss_seg()
}

extern "C" void __cdecl PostFilterInitProgramsPc(i32 width, i32 height) {
    (void)width;
    (void)height;
    PostFilterInitSharedResourcesPc();
    PostCopyTexProgramPc = PostShaderProgramCreatePc(2, PostShaderTokens7656f0, PostShaderTokens765798);
    PostCopyTexLodProgramPc = NULL;
    PostBlendTexProgramPc = NULL;
    if (PCDeviceHardwareVertexProcessingPC > 1) {
        PostBlur5x5ProgramPc = PostShaderProgramCreatePc(2, PostShaderTokens765848, PostShaderTokens765aa8);
        PostBlur7x7ProgramPc = PostShaderProgramCreatePc(2, PostShaderTokens765ca0, PostShaderTokens765d90);
    }
    PostCopyAuxProgramPc = PostShaderProgramCreatePc(2, PostShaderTokens765f30, PostShaderTokens7660d0);
    PostBlurAuxProgramPc = PostShaderProgramCreatePc(2, PostShaderTokens765e40, PostShaderTokens765d90);
}

extern "C" void __cdecl PostShaderProgramBindPc(POST_SHADER_PROGRAM_PC *program) {
    if (program != NULL) {
        if (NuDisplayHandlerVertexShaderCachePc != program->vertex_shader) {
            PCDeviceSetVertexShaderPc(program->vertex_shader);
            NuDisplayHandlerVertexShaderCachePc = program->vertex_shader;
        }
        if (NuDisplayHandlerPixelShaderCachePc != program->pixel_shader) {
            PCDeviceSetPixelShaderPc(program->pixel_shader);
            NuDisplayHandlerPixelShaderCachePc = program->pixel_shader;
        }
    } else {
        PCDeviceSetVertexShaderPc(NULL);
        PCDeviceSetPixelShaderPc(NULL);
        NuDisplayHandlerVertexShaderCachePc = NULL;
        NuDisplayHandlerPixelShaderCachePc = NULL;
    }
}
