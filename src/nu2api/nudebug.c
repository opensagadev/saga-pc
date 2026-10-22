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

void __cdecl NuFrameEndLeafPc(void) {
}

void __cdecl NuTexAnimScriptMissingPc(char *path) {
    (void)path;
}

void __cdecl PCAnimationEffectCheckpoint(void) {
}

void __cdecl PCCharacterModelDiagnosticNoOp(void) {
}

void __cdecl CutSceneLoadDiagnosticPc(void *unused, ...) {
    (void)unused;
}

void __cdecl CutSceneMissingObjectDiagnosticPc(char *unused) {
    (void)unused;
}

void __cdecl CutSceneConfigMissingPc(char *unused) {
    (void)unused;
}

void __cdecl CutSceneStreamDiagnosticPc(char *unused) {
    (void)unused;
}

void __cdecl NuAllocBadPointer(void) {
}

void __cdecl NuAllocReport(const void *message, ...) {
    (void)message;
}
