#include "saga.h"

void __cdecl NuVpSetIdentityPc(NUMTX *matrix) {
    if (matrix != NULL)
        *matrix = NuVpIdentityMatrixPc;
}

void __cdecl NuVpBuildScalingTemplatePc(void) {
    volatile f32 width = (f32)DebrisGlassTextureWidth;
    volatile f32 height = (f32)DebrisGlassTextureHeight;
    volatile f32 min_z = NuVpMinZPc;
    NuCameraScalingTemplate.m00 = (f32)((f64)width * NuQFntHalfScale);
    NuCameraScalingTemplate.m01 = 0.0f;
    NuCameraScalingTemplate.m02 = 0.0f;
    NuCameraScalingTemplate.m03 = 0.0f;
    NuCameraScalingTemplate.m10 = 0.0f;
    NuCameraScalingTemplate.m11 = (f32)(-(f64)height * NuQFntHalfScale);
    NuCameraScalingTemplate.m12 = 0.0f;
    NuCameraScalingTemplate.m13 = 0.0f;
    NuCameraScalingTemplate.m20 = 0.0f;
    NuCameraScalingTemplate.m21 = 0.0f;
    NuCameraScalingTemplate.m22 = (f32)(NuVpDepthRangePc - (f64)min_z);
    NuCameraScalingTemplate.m23 = 0.0f;
    NuCameraScalingTemplate.m30 = (f32)((f64)width * NuQFntHalfScale + 0.0);
    NuCameraScalingTemplate.m31 = (f32)(0.0 + (f64)height * NuQFntHalfScale);
    NuCameraScalingTemplate.m32 = min_z;
    NuCameraScalingTemplate.m33 = 1.0f;
}

void __cdecl NuVpUpdate(void) {
    PC_VIEWPORT_PC viewport;
    i32 x, y, width, height, right, bottom, limit_x, limit_y;
    i32 clipped_left = 0;
    i32 clipped_top = 0;
    volatile f32 rounded_width;
    volatile f32 rounded_height;
    volatile f32 reciprocal;
    if (NuVpDirtyPc == 0)
        return;
    NuCameraProjectionGlobalScaleX = 1.0f;
    NuCameraProjectionGlobalOffsetX = 0.0f;
    NuCameraProjectionGlobalOffsetY = 0.0f;
    NuCameraProjectionGlobalScaleY = 1.0f;
    x = vpCurrent.x;
    y = vpCurrent.y;
    width = vpCurrent.width;
    height = vpCurrent.height;
    right = (i32)((u32)x + (u32)width);
    bottom = (i32)((u32)y + (u32)height);
    limit_x = (i32)((u32)DebrisGlassTextureWidth << 4);
    limit_y = (i32)((u32)DebrisGlassTextureHeight << 4);
    viewport.min_z = NuVpMinZPc;
    viewport.max_z = NuVpMaxZPc;
    if (right < 0 || x >= limit_x || bottom < 0 || y >= limit_y)
        goto empty;
    if (x < 0) {
        width = right;
        x = 0;
        clipped_left = 1;
    }
    if (right > limit_x)
        width = (i32)((u32)width + (u32)limit_x - (u32)right);
    if (y < 0) {
        height = bottom;
        y = 0;
        clipped_top = 1;
    }
    if (bottom > limit_y)
        height = (i32)((u32)height + (u32)limit_y - (u32)bottom);
    if (width <= 0 || height <= 0)
        goto empty;
    rounded_width = (f32)width;
    NuCameraProjectionGlobalScaleX = (f32)((f64)vpCurrent.width / (f64)rounded_width);
    rounded_height = (f32)height;
    NuCameraProjectionGlobalScaleY = (f32)((f64)vpCurrent.height / (f64)rounded_height);
    NuCameraProjectionGlobalOffsetX = (f32)((f64)(i32)((u32)vpCurrent.width - (u32)width) / (f64)rounded_width);
    NuCameraProjectionGlobalOffsetY = (f32)(-(f64)(i32)((u32)vpCurrent.height - (u32)height) / (f64)rounded_height);
    if (clipped_left != 0)
        NuCameraProjectionGlobalOffsetX = -NuCameraProjectionGlobalOffsetX;
    if (clipped_top != 0)
        NuCameraProjectionGlobalOffsetY = -NuCameraProjectionGlobalOffsetY;
    goto submit;
empty:
    x = y = width = height = 0;
submit:
    viewport.x = (u32)(x >> 4);
    viewport.y = (u32)(y >> 4);
    viewport.width = (u32)(width >> 4);
    viewport.height = (u32)(height >> 4);
    NuVpDirtyPc = 0;
    NuVpSubmitPc(&viewport);
    NuMtxSetIdentity(&NuVpIdentityMatrixPc);
    NuVpBuildScalingTemplatePc();
    if (NuVpProjectionMinXPc == 0.0f && NuVpProjectionMaxXPc == 1.0f) {
        NuCameraVPortProjectionScaleX = 1.0f;
        NuCameraScissorProjectionScaleX = (f32)((f64)vpCurrent.width * NuVpFixed16ScalePc);
    } else {
        reciprocal = (f32)(1.0 / ((f64)NuVpProjectionMaxXPc - (f64)NuVpProjectionMinXPc));
        NuCameraScissorProjectionScaleX = reciprocal;
        NuCameraVPortProjectionScaleX = NuCameraScissorProjectionScaleX;
    }
    if (NuVpProjectionMinYPc == 0.0f && NuVpProjectionMaxYPc == 1.0f) {
        NuCameraVPortProjectionScaleY = 1.0f;
        NuCameraScissorProjectionScaleY = (f32)((f64)vpCurrent.height * NuVpFixed16ScalePc);
    } else {
        reciprocal = (f32)(1.0 / ((f64)NuVpProjectionMaxYPc - (f64)NuVpProjectionMinYPc));
        NuCameraScissorProjectionScaleY = reciprocal;
        NuCameraVPortProjectionScaleY = NuCameraScissorProjectionScaleY;
    }
}
