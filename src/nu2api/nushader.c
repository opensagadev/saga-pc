#include "saga.h"

PC_SHADER_RECORD_VIEW *__cdecl PCShaderRecordFromHandle(u32 handle) {
    if (handle <= 400)
        return &PCShaderSlots[handle].record;
    return NULL;
}

void __cdecl NuShaderManagerReleaseShader(PC_SHADER_RECORD_VIEW *shader) {
    --shader->ref_count;
}
