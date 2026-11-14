#include "saga.h"

void PCAnimationLoadFailureNoOp(char *path) {
    (void)path;
}

void PCCharacterSystemExistingNoOp() {
}

void NuPtlDmaSlotsExhaustedPc(void) {
}

void PCClipScreenUnsupported(void) {
}

void NuQFntAtlasErrorPc(void) {
}

void PCGraphicsRenderTextureAllocationError(void) {
}

void NuTexBmpUnsupportedDepthPc(i32 bit_count, const char *path) {
    (void)bit_count;
    (void)path;
}

void NuFrameEndLeafPc(void) {
}

void DisplayListSpecialTransformNoOpPc(NUSPECIALHANDLE *special, NUMTX *matrix) {
    (void)special;
    (void)matrix;
}

/* PC has no PS2 screen grab; the dump is empty. */
void NuPs2VideoScreenDump(char *filename, i32 format, f32 scale_x, f32 scale_y, i32 face, i32 x, i32 y) {
}

i32 fun_006de9b0(void) {
    return -1;
}

i32 fun_006de9c0(void) {
    return 1;
}

i32 fun_006de9d0(FRAME_PAD_PC *pad) {
    return GamePadReadBackendPc(pad, -1);
}

void fun_006de9e0(void) {
}

void fun_006de9f0(void) {
}

void fun_006dea00(void) {
}

void fun_006dea10(void) {
}

i32 fun_006dea20(void) {
    return 0;
}

void fun_006dea30(void) {
}

void fun_006dea40(void) {
}

i32 fun_006dea50(void) {
    return 0;
}

void PCAnimationEffectCheckpoint(void) {
}

void fun_006dea70(void) {
}

void fun_006dea80(void) {
}

void fun_006deb00(void) {
}

i32 dat_02976ce8 = 0;

i32 fun_006deb10(void) {
    return dat_02976ce8;
}

void fun_006deb20(void) {
}

i32 fun_006deb30(void) {
    return 0;
}

i32 dat_0297ad84 = 0;

void fun_006deb40(void) {
    dat_0297ad84 = 1;
}

i32 fun_006debd0(void) {
    return 0;
}

i32 fun_006debe0(void) {
    return 0;
}

void fun_006debf0(void) {
}

void fun_006dec00(void) {
}

void PCCharacterModelDiagnosticNoOp() {
}

void CutSceneLoadDiagnosticPc(void *unused, ...) {
    (void)unused;
}

void CutSceneConfigMissingPc(char *unused) {
    (void)unused;
}

void CutSceneStreamDiagnosticPc(char *unused) {
    (void)unused;
}

void NuFileError(void) {
}

void NuFileRetry(void) {
}

void fun_004ef7c0(void) {
}

/* Empty release diagnostics with an argument; the linker folds them into
   NuFileError and NuFileRetry. */
void NuRDPIVariableErrorPc(char *name) {
    (void)name;
}

void fun_004ef7a0(void) {
}

void NuDatFileMissingPc(char *filepath) {
    (void)filepath;
}

void NuAllocBadPointer(void) {
}

void fun_004eedb0(void) {
}

void NuAllocReport(const void *message, ...) {
    (void)message;
}
