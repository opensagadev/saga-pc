#include "saga.h"

PC_SHADER_RECORD_VIEW *PCShaderRecordFromHandle(u32 handle) {
    if (handle <= 400) {
        return &PCShaderSlots[handle].record;
    }
    return NULL;
}

void NuShaderManagerReleaseShader(PC_SHADER_RECORD_VIEW *shader) {
    --shader->ref_count;
}
