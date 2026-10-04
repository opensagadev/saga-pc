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

STATIC_ASSERT(sizeof(RENDER_POST_VIEWPORT_PREFIX_PC) == 0x30);
STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, source_x) == 0);
STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, source_y) == 4);
STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, source_right) == 8);
STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, source_bottom) == 0xc);
STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, dest_x) == 0x10);
STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, dest_y) == 0x14);
STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, dest_right) == 0x18);
STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, dest_bottom) == 0x1c);
STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, width_scale) == 0x20);
STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, height_scale) == 0x24);
STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, x_offset) == 0x28);
STATIC_ASSERT(offsetof(RENDER_POST_VIEWPORT_PREFIX_PC, y_offset) == 0x2c);
extern RENDER_POST_VIEWPORT_PREFIX_PC RenderPostViewportPrefixPc;
void __cdecl RenderPostViewportSetPc(f32 source_x, f32 source_y, f32 source_right, f32 source_bottom, f32 dest_x,
                                     f32 dest_y, f32 dest_right, f32 dest_bottom);
void __cdecl RenderPostViewportResetPc(void);

void __cdecl RenderPostViewportSetPc(f32 source_x, f32 source_y, f32 source_right, f32 source_bottom, f32 dest_x,
                                     f32 dest_y, f32 dest_right, f32 dest_bottom) {
    f32 width = (f32)PS2_VREZ_W;
    f32 height = (f32)PS2_VREZ_H;
    RenderPostViewportPrefixPc.source_x = source_x;
    RenderPostViewportPrefixPc.source_y = source_y;
    RenderPostViewportPrefixPc.source_right = source_right;
    RenderPostViewportPrefixPc.source_bottom = source_bottom;
    RenderPostViewportPrefixPc.dest_x = dest_x;
    RenderPostViewportPrefixPc.dest_y = dest_y;
    RenderPostViewportPrefixPc.dest_right = dest_right;
    RenderPostViewportPrefixPc.dest_bottom = dest_bottom;
    RenderPostViewportPrefixPc.x_offset = dest_x;
    RenderPostViewportPrefixPc.y_offset = dest_y;
    RenderPostViewportPrefixPc.width_scale = (dest_right - dest_x) / width;
    RenderPostViewportPrefixPc.height_scale = (dest_bottom - dest_y) / height;
    PCSpecialProjectionAdjust.x_scale = width / (source_right - source_x);
    PCSpecialProjectionAdjust.y_scale = height / (source_bottom - source_y);
    PCSpecialProjectionAdjust.x_offset = ((width - source_right) - source_x) * 2.0f / width;
    PCSpecialProjectionAdjust.y_offset = ((height - source_bottom) - source_y) * -2.0f / height;
}

void __cdecl RenderPostViewportResetPc(void) {
    f32 width = (f32)PS2_VREZ_W;
    f32 height = (f32)PS2_VREZ_H;
    RenderPostViewportSetPc(0.0f, 0.0f, width, height, 0.0f, 0.0f, width, height);
}

void __cdecl NuVpGetSize2(f32 *width, f32 *height) {
    *width = (f32)((vpCurrent.width * 0.0625) / ((f64)DebrisGlassTextureWidth / PS2_VREZ_W));
    *height = (f32)((vpCurrent.height * 0.0625) / ((f64)DebrisGlassTextureHeight / PS2_VREZ_H));
}
