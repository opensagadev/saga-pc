#include "nu2api/PCInput.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucamera_dlist.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nuwin32/d3dCalls.h"
#include "nu2api/nuwin32/d3dCore.h"
#include <vector>

extern "C" {
    POST_SHADER_PROGRAM *PostDepthResolveProgram = 0;
    POST_SHADER_PROGRAM *PostCombineProgram = 0;
    PC_TEXTURE_RESOURCE *NuDisplayHandlerVertexShaderCache = NULL;
    PC_TEXTURE_RESOURCE *NuDisplayHandlerPixelShaderCache = NULL;
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
