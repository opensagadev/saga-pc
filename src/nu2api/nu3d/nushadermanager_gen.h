#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nushaderobject.h"

typedef struct PC_SHADER_RECORD_VIEW { // size 0x8
    u32 unknown_0;                     // 0x0
    i32 ref_count;                     // 0x4
} PC_SHADER_RECORD_VIEW;

typedef struct PC_SHADER_SLOT_VIEW { // size 0x4d8
    u32 unknown_0;                   // 0x000
    PC_SHADER_RECORD_VIEW record;    // 0x004
    u8 unknown_c[0x4cc];             // 0x00c
} PC_SHADER_SLOT_VIEW;

#ifdef __cplusplus
extern "C" {
#endif
    extern PC_SHADER_SLOT_VIEW *PCShaderSlots;
    PC_SHADER_RECORD_VIEW *PCShaderRecordFromHandle(u32);
    NuShaderObject *PCShaderCurrentObject(void);
    extern f32 PCShaderShininessFactorStorage;
    f32 NuShaderManagerGetShininessFactor(void);
#ifdef __cplusplus
}
#endif
