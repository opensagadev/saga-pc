#include "nu2api/PCInput.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucamera_dlist.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nuwin32/d3dCalls.h"
#include "nu2api/nuwin32/d3dCore.h"
#include <vector>

struct POST_VERTEX_SHADER_CACHE_ENTRY { // size 0x8
    PC_TEXTURE_RESOURCE *shader;        // 0x0
    const DWORD *tokens;                // 0x4
};

struct POST_PIXEL_SHADER_CACHE_ENTRY { // size 0x8
    PC_TEXTURE_RESOURCE *shader;       // 0x0
    const DWORD *tokens;               // 0x4
};

struct POST_SHADER_PROGRAM_POOL {     // size 0x40c
    POST_SHADER_PROGRAM programs[64]; // 0x000
    u8 occupied[8];                   // 0x400
    i32 cursor;                       // 0x408
    POST_SHADER_PROGRAM *Allocate(void);
    POST_SHADER_PROGRAM_POOL *ResetPrograms();
    i32 IsProgramOccupied(i32 index);
    void SetProgramOccupied(i32 index, bool set);
};

extern "C" {
    POST_SHADER_PROGRAM_POOL PostShaderProgramPool = {0};
    std::vector<POST_VERTEX_SHADER_CACHE_ENTRY> PostVertexShaderCache;
    std::vector<POST_PIXEL_SHADER_CACHE_ENTRY> PostPixelShaderCache;
}

POST_SHADER_PROGRAM *POST_SHADER_PROGRAM_POOL::Allocate(void) {
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

FRAME_NATIVE_TEXTURE_POOL *FRAME_NATIVE_TEXTURE_POOL::ResetTextures() {
    memset(occupied, 0, sizeof(occupied));
    cursor = 0;
    return this;
}

i32 FRAME_NATIVE_TEXTURE_POOL::fun_00605170(FRAME_NATIVE_TEXTURE *record) {
    u32 index = record - records;
    return index <= 15;
}

POST_SHADER_PROGRAM_POOL *POST_SHADER_PROGRAM_POOL::ResetPrograms() {
    memset(occupied, 0, sizeof(occupied));
    cursor = 0;
    return this;
}

void FRAME_NATIVE_TEXTURE_POOL::SetTextureOccupied(i32 index, bool set) {
    if (set) {
        occupied[index >> 3] |= 1 << (index & 7);
    } else {
        occupied[index >> 3] &= ~(1 << (index & 7));
    }
}

i32 FRAME_NATIVE_TEXTURE_POOL::IsTextureOccupied(i32 index) {
    return occupied[index >> 3] >> (index & 7) & 1;
}

i32 POST_SHADER_PROGRAM_POOL::IsProgramOccupied(i32 index) {
    return (i8)occupied[index >> 3] >> (index & 7) & 1;
}

void POST_SHADER_PROGRAM_POOL::SetProgramOccupied(i32 index, bool set) {
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

extern "C" POST_SHADER_PROGRAM *PostShaderProgramCreate(i32 mode, const DWORD *vertex_tokens, const DWORD *pixel_tokens) {
    POST_VERTEX_SHADER_CACHE_ENTRY vertex = {NULL, vertex_tokens};
    POST_PIXEL_SHADER_CACHE_ENTRY pixel = {NULL, pixel_tokens};
    std::vector<POST_VERTEX_SHADER_CACHE_ENTRY>::iterator vertex_it = PostVertexShaderCache.begin();
    for (; vertex_it != PostVertexShaderCache.end(); ++vertex_it) {
        if (vertex_it->tokens == vertex.tokens) {
            vertex = *vertex_it;
            break;
        }
    }
    if (vertex.shader == NULL) {
        PCDeviceCreateVertexShader(vertex.tokens, &vertex.shader);
        PostVertexShaderCache.push_back(vertex);
    }
    std::vector<POST_PIXEL_SHADER_CACHE_ENTRY>::iterator pixel_it = PostPixelShaderCache.begin();
    for (; pixel_it != PostPixelShaderCache.end(); ++pixel_it) {
        if (pixel_it->tokens == pixel.tokens) {
            pixel = *pixel_it;
            break;
        }
    }
    if (pixel.shader == NULL) {
        PCDeviceCreatePixelShader(pixel.tokens, &pixel.shader);
        PostPixelShaderCache.push_back(pixel);
    }
    if (vertex.shader != NULL && pixel.shader != NULL) {
        POST_SHADER_PROGRAM *program = PostShaderProgramPool.Allocate();
        program->vertex_shader = vertex.shader;
        program->pixel_shader = pixel.shader;
        return program;
    }
    return NULL;
}

extern "C" {
    POST_SHADER_PROGRAM *PostCopyTexProgram = 0;
    POST_SHADER_PROGRAM *dat_00973820 = 0;
    POST_SHADER_PROGRAM *PostCopyTexLodProgram = 0;
    POST_SHADER_PROGRAM *dat_0097382c = 0;
    POST_SHADER_PROGRAM *PostBlendTexProgram = 0;
    POST_SHADER_PROGRAM *PostBlur5x5Program = 0;
    POST_SHADER_PROGRAM *PostBlur7x7Program = 0;
    PC_TEXTURE_RESOURCE *NuDisplayHandlerVertexShaderCache = NULL;
    PC_TEXTURE_RESOURCE *NuDisplayHandlerPixelShaderCache = NULL;
    POST_SHADER_PROGRAM *PostCopyAuxProgram = 0;
    POST_SHADER_PROGRAM *PostBlurAuxProgram = 0;
}

extern "C" void PostFilterInitPrograms(i32 width, i32 height) {
    PostFilterInitSharedResources();
    PostCopyTexProgram = PostShaderProgramCreate(2, dat_007656f0, dat_00765798);
    PostCopyTexLodProgram = NULL;
    PostBlendTexProgram = NULL;
    if (PCDeviceHardwareVertexProcessingPC > 1) {
        PostBlur5x5Program = PostShaderProgramCreate(2, dat_00765848, dat_00765aa8);
        PostBlur7x7Program = PostShaderProgramCreate(2, dat_00765ca0, dat_00765d90);
    }
    PostCopyAuxProgram = PostShaderProgramCreate(2, dat_00765f30, dat_007660d0);
    PostBlurAuxProgram = PostShaderProgramCreate(2, dat_00765e40, dat_00765d90);
}

extern "C" void NuShaderProgramMakeCurrent(POST_SHADER_PROGRAM *program) {
    if (program != NULL) {
        if (NuDisplayHandlerVertexShaderCache != program->vertex_shader) {
            PCDeviceSetVertexShader(program->vertex_shader);
            NuDisplayHandlerVertexShaderCache = program->vertex_shader;
        }
        if (NuDisplayHandlerPixelShaderCache != program->pixel_shader) {
            PCDeviceSetPixelShader(program->pixel_shader);
            NuDisplayHandlerPixelShaderCache = program->pixel_shader;
        }
    } else {
        PCDeviceSetVertexShader(NULL);
        PCDeviceSetPixelShader(NULL);
        NuDisplayHandlerVertexShaderCache = NULL;
        NuDisplayHandlerPixelShaderCache = NULL;
    }
}

extern "C" void fun_005fa210(void) {
}

extern "C" void fun_00605510(void) {
}

extern "C" void fun_00605540(void) {
}

extern "C" FRAME_NATIVE_TEXTURE *FrameNativeTexturePoolAllocate(void *, FRAME_NATIVE_TEXTURE_POOL *pool) {
    return pool->Allocate();
}

extern "C" POST_SHADER_PROGRAM *PostShaderProgramPoolAllocate(void *, POST_SHADER_PROGRAM_POOL *pool) {
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
