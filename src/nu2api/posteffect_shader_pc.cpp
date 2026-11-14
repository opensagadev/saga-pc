#include "saga.h"
#include <vector>

struct POST_VERTEX_SHADER_CACHE_ENTRY_PC { // size 0x8
    PC_TEXTURE_RESOURCE *shader;           // 0x0
    const DWORD *tokens;                   // 0x4
};

struct POST_PIXEL_SHADER_CACHE_ENTRY_PC { // size 0x8
    PC_TEXTURE_RESOURCE *shader;          // 0x0
    const DWORD *tokens;                  // 0x4
};

struct POST_SHADER_PROGRAM_POOL_PC {     // size 0x40c
    POST_SHADER_PROGRAM_PC programs[64]; // 0x000
    u8 occupied[8];                      // 0x400
    i32 cursor;                          // 0x408
    POST_SHADER_PROGRAM_PC *Allocate(void);
    POST_SHADER_PROGRAM_POOL_PC *fun_006051a0();
    i32 fun_00605480(i32 index);
    void fun_006054a0(i32 index, bool set);
};

extern "C" {
#pragma bss_seg(".bss$PostShaderProgramPoolPc")
    POST_SHADER_PROGRAM_POOL_PC PostShaderProgramPoolPc = {0};
#pragma bss_seg(".bss$PostVertexShaderCachePc")
    std::vector<POST_VERTEX_SHADER_CACHE_ENTRY_PC> PostVertexShaderCachePc;
#pragma bss_seg(".bss$PostPixelShaderCachePc")
    std::vector<POST_PIXEL_SHADER_CACHE_ENTRY_PC> PostPixelShaderCachePc;
#pragma bss_seg()
}

POST_SHADER_PROGRAM_PC *POST_SHADER_PROGRAM_POOL_PC::Allocate(void) {
    i32 index = cursor;
    for (; index < 64; ++index) {
        if (((i8)occupied[index >> 3] >> (index & 7) & 1) == 0) {
            goto found;
        }
    }
    for (index = 0; index < cursor; ++index) {
        if (((i8)occupied[index >> 3] >> (index & 7) & 1) == 0) {
            goto found;
        }
    }
    return NULL;
found:
    occupied[index >> 3] |= (u8)(1 << (index & 7));
    cursor = (index + 1) % 64;
    return &programs[index];
}

FRAME_NATIVE_TEXTURE_POOL *FRAME_NATIVE_TEXTURE_POOL::fun_00605150() {
    memset(occupied, 0, sizeof(occupied));
    cursor = 0;
    return this;
}

i32 FRAME_NATIVE_TEXTURE_POOL::fun_00605170(FRAME_NATIVE_TEXTURE *record) {
    u32 index = record - records;
    return index <= 15;
}

POST_SHADER_PROGRAM_POOL_PC *POST_SHADER_PROGRAM_POOL_PC::fun_006051a0() {
    memset(occupied, 0, sizeof(occupied));
    cursor = 0;
    return this;
}

void FRAME_NATIVE_TEXTURE_POOL::fun_00605300(i32 index, bool set) {
    if (set) {
        occupied[index >> 3] |= 1 << (index & 7);
    } else {
        occupied[index >> 3] &= ~(1 << (index & 7));
    }
}

i32 FRAME_NATIVE_TEXTURE_POOL::fun_00605460(i32 index) {
    return occupied[index >> 3] >> (index & 7) & 1;
}

i32 POST_SHADER_PROGRAM_POOL_PC::fun_00605480(i32 index) {
    return (i8)occupied[index >> 3] >> (index & 7) & 1;
}

void POST_SHADER_PROGRAM_POOL_PC::fun_006054a0(i32 index, bool set) {
    if (set) {
        occupied[index >> 3] |= 1 << (index & 7);
    } else {
        occupied[index >> 3] &= ~(1 << (index & 7));
    }
}

void FRAME_NATIVE_TEXTURE_POOL::fun_0060a010(FRAME_NATIVE_TEXTURE *record) {
    i32 index = record - records;
    occupied[index >> 3] &= ~(1 << (index & 7));
    cursor = index;
}

extern "C" POST_SHADER_PROGRAM_PC *PostShaderProgramCreatePc(i32 mode, const DWORD *vertex_tokens, const DWORD *pixel_tokens) {
    POST_VERTEX_SHADER_CACHE_ENTRY_PC vertex = {NULL, vertex_tokens};
    POST_PIXEL_SHADER_CACHE_ENTRY_PC pixel = {NULL, pixel_tokens};
    std::vector<POST_VERTEX_SHADER_CACHE_ENTRY_PC>::iterator vertex_it = PostVertexShaderCachePc.begin();
    for (; vertex_it != PostVertexShaderCachePc.end(); ++vertex_it) {
        if (vertex_it->tokens == vertex.tokens) {
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
        if (pixel_it->tokens == pixel.tokens) {
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

extern "C" void PostFilterInitSharedResources(void);
extern "C" {
    extern const DWORD dat_007656f0[];
    extern const DWORD dat_00765798[];
    extern const DWORD dat_00765848[];
    extern const DWORD dat_00765aa8[];
    extern const DWORD dat_00765ca0[];
    extern const DWORD dat_00765d90[];
    extern const DWORD dat_00765e40[];
    extern const DWORD dat_00765f30[];
    extern const DWORD dat_007660d0[];
#pragma bss_seg(".bss$PostCopyTexProgramPc")
    POST_SHADER_PROGRAM_PC *PostCopyTexProgram = 0;
#pragma bss_seg(".bss$PostCopyTexLodProgramPc")
    POST_SHADER_PROGRAM_PC *PostCopyTexLodProgram = 0;
#pragma bss_seg(".bss$PostBlendTexProgramPc")
    POST_SHADER_PROGRAM_PC *PostBlendTexProgram = 0;
#pragma bss_seg(".bss$PostBlur5x5ProgramPc")
    POST_SHADER_PROGRAM_PC *PostBlur5x5Program = 0;
#pragma bss_seg(".bss$PostBlur7x7ProgramPc")
    POST_SHADER_PROGRAM_PC *PostBlur7x7Program = 0;
#pragma bss_seg(".bss$PostCopyAuxProgramPc")
    POST_SHADER_PROGRAM_PC *PostCopyAuxProgram = 0;
#pragma bss_seg(".bss$PostBlurAuxProgramPc")
    POST_SHADER_PROGRAM_PC *PostBlurAuxProgram = 0;
#pragma bss_seg()
}

extern "C" void PostFilterInitPrograms(i32 width, i32 height) {
    (void)width;
    (void)height;
    PostFilterInitSharedResources();
    PostCopyTexProgram = PostShaderProgramCreatePc(2, dat_007656f0, dat_00765798);
    PostCopyTexLodProgram = NULL;
    PostBlendTexProgram = NULL;
    if (PCDeviceHardwareVertexProcessingPC > 1) {
        PostBlur5x5Program = PostShaderProgramCreatePc(2, dat_00765848, dat_00765aa8);
        PostBlur7x7Program = PostShaderProgramCreatePc(2, dat_00765ca0, dat_00765d90);
    }
    PostCopyAuxProgram = PostShaderProgramCreatePc(2, dat_00765f30, dat_007660d0);
    PostBlurAuxProgram = PostShaderProgramCreatePc(2, dat_00765e40, dat_00765d90);
}

extern "C" void NuShaderProgramMakeCurrent(POST_SHADER_PROGRAM_PC *program) {
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

extern "C" void fun_005fa210(void) {
}

extern "C" void fun_00605510(void) {
}

extern "C" void fun_00605540(void) {
}

extern "C" FRAME_NATIVE_TEXTURE *fun_0060bf20(void *, FRAME_NATIVE_TEXTURE_POOL *pool) {
    return pool->Allocate();
}

extern "C" POST_SHADER_PROGRAM_PC *fun_0060bf30(void *, POST_SHADER_PROGRAM_POOL_PC *pool) {
    return pool->Allocate();
}

extern "C" void fun_0060c0c0(void) {
}

extern "C" void fun_0060c110(void) {
}

extern "C" void fun_0060c970(void) {
}

extern "C" void fun_0060c9c0(void) {
}
