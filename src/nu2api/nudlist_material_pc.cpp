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
