#include "saga.h"

extern "C" {
    extern NUVEC4 PcGeometryDefaultOriginPc;
    extern NUMTX PcGeometryViewMatrixPc;
    extern NUMTX PcGeometryProjectionMatrixPc;
    extern NUMTX PcGeometryViewProjectionMatrixPc;
    extern NUMTX PcGeometryInverseViewProjectionMatrixPc;
    extern NUMTX PcGeometryResetMatrixPc;
    extern NUMTX PcGeometryAdjustedProjectionMatrixPc;
    extern NUMTX PcGeometryTranslationMatrixPc;
    extern NUMTX PcGeometryScaleMatrixPc;
    void __cdecl PcGetGeometryPerspectivePc(NUMTX *, f32 *, f32 *, f32 *, f32 *);
}

extern "C" void __cdecl PcResetGeometryOriginStatePc(void) {
    PcGeometryViewMatrixPc = NuMtxSetIdentityTemplate;
    PcGeometryProjectionMatrixPc = NuMtxSetIdentityTemplate;
    PcGeometryViewProjectionMatrixPc = NuMtxSetIdentityTemplate;
    PcGeometryInverseViewProjectionMatrixPc = NuMtxSetIdentityTemplate;
    PcGeometryResetMatrixPc = NuMtxSetIdentityTemplate;
    PcGeometryDistanceOriginPc = PcGeometryDefaultOriginPc;
    for (i32 i = 0; i < 16; ++i) {
        PCDeviceSamplerInvalidation[i][0] = 0x012449d0;
        PCDeviceSamplerInvalidation[i][1] = 0;
    }
}

extern "C" void __cdecl PcUpdateGeometryCameraOriginPc(NUMTX *view, NUMTX *projection) {
    NUVEC scale = {PCSpecialProjectionAdjust.x_scale, PCSpecialProjectionAdjust.y_scale, 1.0f};
    NUVEC translation = {PCSpecialProjectionAdjust.x_offset, PCSpecialProjectionAdjust.y_offset, 0.0f};
    NuMtxSetScale(&PcGeometryScaleMatrixPc, &scale);
    NuMtxSetTranslation(&PcGeometryTranslationMatrixPc, &translation);
    NuMtxMulH(&PcGeometryAdjustedProjectionMatrixPc, projection, &PcGeometryScaleMatrixPc);
    NuMtxMulH(&PcGeometryAdjustedProjectionMatrixPc, &PcGeometryAdjustedProjectionMatrixPc,
              &PcGeometryTranslationMatrixPc);
    PcGeometryViewMatrixPc = *view;
    PcGeometryProjectionMatrixPc = PcGeometryAdjustedProjectionMatrixPc;
    NUMTX inverse_view;
    NuMtxInvH(&inverse_view, &PcGeometryViewMatrixPc);
    PcGeometryDistanceOriginPc.x = inverse_view.m30 / inverse_view.m33;
    PcGeometryDistanceOriginPc.y = inverse_view.m31 / inverse_view.m33;
    PcGeometryDistanceOriginPc.z = inverse_view.m32 / inverse_view.m33;
    PcGeometryDistanceOriginPc.w = 1.0f;
    NuMtxMulH(&PcGeometryViewProjectionMatrixPc, &PcGeometryViewMatrixPc, &PcGeometryProjectionMatrixPc);
    NuMtxInvH(&PcGeometryInverseViewProjectionMatrixPc, &PcGeometryViewProjectionMatrixPc);
    NuShaderManagerSetfv(0x12, &PcGeometryViewMatrixPc.m00);
    NuShaderManagerSetfv(0x13, &PcGeometryViewProjectionMatrixPc.m00);
    NuShaderManagerSetfv(0x5a, &PcGeometryDistanceOriginPc.x);
    f32 fovy, aspect, z_near, z_far;
    PcGetGeometryPerspectivePc(&PcGeometryProjectionMatrixPc, &fovy, &aspect, &z_near, &z_far);
    NUVEC4 depth = {z_near, z_far, z_far - z_near, 1.0f / PcGeometryProjectionMatrixPc.m22};
    NuShaderManagerSetfv(0x46, &depth.x);
    f32 left, right, bottom, top;
    NuMtxGetFrustumD3D(&PcGeometryProjectionMatrixPc, &left, &right, &bottom, &top, &z_near, &z_far);
    NUVEC4 frustum = {left, right - left, bottom, top - bottom};
    NuShaderManagerSetfv(0x47, &frustum.x);
}
