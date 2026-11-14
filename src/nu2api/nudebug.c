#include "saga.h"

void __cdecl PCAnimationLoadFailureNoOp(char *path) {
    (void)path;
}

void __cdecl PCCharacterSystemExistingNoOp() {
}

void __cdecl NuPtlDmaSlotsExhaustedPc(void) {
}

void __cdecl PCClipScreenUnsupported(void) {
}

void __cdecl NuQFntAtlasErrorPc(void) {
}

void __cdecl PCGraphicsRenderTextureAllocationError(void) {
}

void __cdecl NuTexBmpUnsupportedDepthPc(i32 bit_count, const char *path) {
    (void)bit_count;
    (void)path;
}

void __cdecl NuFrameEndLeafPc(void) {
}

void __cdecl DisplayListSpecialTransformNoOpPc(NUSPECIALHANDLE *special, NUMTX *matrix) {
    (void)special;
    (void)matrix;
}

/* PC has no PS2 screen grab; the dump is empty. */
void __cdecl NuPs2VideoScreenDump(char *filename, i32 format, f32 scale_x, f32 scale_y, i32 face, i32 x, i32 y) {
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

void __cdecl PCAnimationEffectCheckpoint(void) {
}

void fun_006dea70(void) {
}

void fun_006dea80(void) {
}

void fun_006deb00(void) {
}

void fun_006deb20(void) {
}

i32 fun_006deb30(void) {
    return 0;
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

void __cdecl PCCharacterModelDiagnosticNoOp() {
}

void __cdecl CutSceneLoadDiagnosticPc(void *unused, ...) {
    (void)unused;
}

void __cdecl CutSceneConfigMissingPc(char *unused) {
    (void)unused;
}

void __cdecl CutSceneStreamDiagnosticPc(char *unused) {
    (void)unused;
}

void __cdecl NuFileError(void) {
}

void __cdecl NuFileRetry(void) {
}

void fun_004ef7c0(void) {
}

/* Empty release diagnostics with an argument; the linker folds them into
   NuFileError and NuFileRetry. */
void __cdecl NuRDPIVariableErrorPc(char *name) {
    (void)name;
}

void fun_004ef7a0(void) {
}

void __cdecl NuDatFileMissingPc(char *filepath) {
    (void)filepath;
}

void __cdecl NuAllocBadPointer(void) {
}

void fun_004eedb0(void) {
}

void __cdecl NuAllocReport(const void *message, ...) {
    (void)message;
}
