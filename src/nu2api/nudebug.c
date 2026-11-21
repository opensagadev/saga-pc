#include "gameapi/edtools/edui.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/items/base/apiobject.h"
#include "nu2api/PCInput.h"
#include "nu2api/cutscene_include.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d_pc_includes.h"
#include "nu2api/nucore/numem.h"
#include "nu2api/nucore/nuvideo.h"
#include "nu2api/numath/numaths.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/nuwin32/Stubs.h"
#include <math.h>

void PCAnimationLoadFailureNoOp(char *path) {
}

void PCCharacterSystemExistingNoOp() {
}

void NuPtlDmaSlotsExhausted(void) {
}

void PCClipScreenUnsupported(void) {
}

void NuQFntAtlasError(void) {
}

void PCGraphicsRenderTextureAllocationError(void) {
}

void NuTexBmpUnsupportedDepth(i32 bit_count, const char *path) {
}

void NuFrameEndLeaf(void) {
}

void NuDebugToggle(void) {
    i32 enabled;
    if (NuDebugDisabled != 0) {
        NuDebugDisabled = 0;
        enabled = 1;
    } else {
        NuDebugDisabled = 1;
        enabled = 0;
    }
    NuDebugGeometrySortOutput = enabled;
    dat_00806b90 = enabled;
    dat_00806b8c = enabled;
    dat_00806b88 = enabled;
    dat_00806b84 = enabled;
}

void DisplayListSpecialTransformNoOp(nuhspecial_s *special, NUMTX *matrix) {
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

i32 NuDebugPadRead(FRAME_PAD *pad) {
    return GamePadReadBackend(pad, -1);
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

u32 dat_008277f0[4] = {0x60000000, 0, 0, 0};
u32 *dat_0297ad00 = 0;

u32 *fun_006dea90(u32 *end) {
    *(u16 *)dat_0297ad00 = (u16)(((u32)end - (u32)dat_0297ad00) / 16 - 1);
    dat_0297ad00 = NULL;
    return end;
}

u32 *fun_006deac0(u32 *packet, u32 word2, u32 word3) {
    dat_0297ad00 = packet;
    packet[0] = dat_008277f0[0];
    packet[1] = dat_008277f0[1];
    packet[2] = word2;
    packet[3] = word3;
    dat_008277f0[2] = word2;
    dat_008277f0[3] = word3;
    return packet + 4;
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

NUMTX dat_0297ad08 = {0};

void fun_006deb50(NUVEC *out, NUVEC *in) {
    out->x = in->x * dat_0297ad08.m00 + in->y * dat_0297ad08.m10 + in->z * dat_0297ad08.m20 + dat_0297ad08.m30;
    out->y = in->x * dat_0297ad08.m01 + in->y * dat_0297ad08.m11 + in->z * dat_0297ad08.m21 + dat_0297ad08.m31;
    out->z = in->x * dat_0297ad08.m02 + in->y * dat_0297ad08.m12 + in->z * dat_0297ad08.m22 + dat_0297ad08.m32;
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

void CutSceneLoadDiagnostic(void *unused, ...) {
}

void CutSceneConfigMissing(char *unused) {
}

void CutSceneStreamDiagnostic(char *unused) {
}

void NuFileError(void) {
}

void NuFileRetry(void) {
}

void fun_004ef7c0(void) {
}

/* Empty release diagnostics with an argument; the linker folds them into
   NuFileError and NuFileRetry. */
void NuRDPIVariableError(char *name) {
}

void fun_004ef7a0(void) {
}

void NuDatFileMissing(char *filepath) {
}

void NuAllocBadPointer(void) {
}

void fun_004eedb0(void) {
}

void NuAllocReport(const void *message, ...) {
}

/* Two uncalled copies of the SDK's NuFabs (nufloat.h). */
f32 PCNativeDebugFabs(f32 value) {
    return fabs(value);
}

f32 PCNativeDebugNuFabs(f32 value) {
    return fabs(value);
}
