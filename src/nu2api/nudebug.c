#include "saga.h"

void __cdecl PCAnimationLoadFailureNoOp(char *path) {
    (void)path;
}

void __cdecl PCCharacterSystemExistingNoOp(void) {
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

/* PC has no PS2 screen grab; the dump is empty. */
void __cdecl NuPs2VideoScreenDump(char *filename, i32 format, f32 scale_x, f32 scale_y, i32 face, i32 x, i32 y) {
}

void __cdecl PCAnimationEffectCheckpoint(void) {
}

void __cdecl PCCharacterModelDiagnosticNoOp(void) {
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

/* Empty release diagnostics with an argument; the linker folds them into
   NuFileError and NuFileRetry. */
void __cdecl NuRDPIVariableErrorPc(char *name) {
    (void)name;
}

void __cdecl NuDatFileMissingPc(char *filepath) {
    (void)filepath;
}

void __cdecl NuAllocBadPointer(void) {
}

void __cdecl NuAllocReport(const void *message, ...) {
    (void)message;
}
