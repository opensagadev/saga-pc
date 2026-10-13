#include "saga.h"

void __cdecl NuVpSetIdentityPc(NUMTX *matrix) {
    if (matrix != NULL)
        *matrix = NuVpIdentityMatrixPc;
}

NUVPCURRENT *NuVpGetCurrentViewport(void) {
    return &vpCurrent;
}

void __cdecl NuVpBuildScalingTemplatePc(void) {
    volatile f32 width = (f32)PS2_REZ_W;
    volatile f32 height = (f32)PS2_REZ_H;
    volatile f32 min_z = vpCurrent.min_z;
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
    limit_x = (i32)((u32)PS2_REZ_W << 4);
    limit_y = (i32)((u32)PS2_REZ_H << 4);
    viewport.min_z = vpCurrent.min_z;
    viewport.max_z = vpCurrent.max_z;
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
    if (vpCurrent.clip_min_x == 0.0f && vpCurrent.clip_max_x == 1.0f) {
        vpCurrent.projection_scale_x = 1.0f;
        vpCurrent.scissor_scale_x = (f32)((f64)vpCurrent.width * NuVpFixed16ScalePc);
    } else {
        reciprocal = (f32)(1.0 / ((f64)vpCurrent.clip_max_x - (f64)vpCurrent.clip_min_x));
        vpCurrent.scissor_scale_x = reciprocal;
        vpCurrent.projection_scale_x = vpCurrent.scissor_scale_x;
    }
    if (vpCurrent.clip_min_y == 0.0f && vpCurrent.clip_max_y == 1.0f) {
        vpCurrent.projection_scale_y = 1.0f;
        vpCurrent.scissor_scale_y = (f32)((f64)vpCurrent.height * NuVpFixed16ScalePc);
    } else {
        reciprocal = (f32)(1.0 / ((f64)vpCurrent.clip_max_y - (f64)vpCurrent.clip_min_y));
        vpCurrent.scissor_scale_y = reciprocal;
        vpCurrent.projection_scale_y = vpCurrent.scissor_scale_y;
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
    *width = (f32)((vpCurrent.width * 0.0625) / ((f64)PS2_REZ_W / PS2_VREZ_W));
    *height = (f32)((vpCurrent.height * 0.0625) / ((f64)PS2_REZ_H / PS2_VREZ_H));
}

STATIC_ASSERT(offsetof(NUVPCURRENT, x) == 0);
STATIC_ASSERT(offsetof(NUVPCURRENT, y) == 4);
STATIC_ASSERT(offsetof(NUVPCURRENT, width) == 8);
STATIC_ASSERT(offsetof(NUVPCURRENT, height) == 0xc);
void __cdecl NuVpSetZRange(f32 minimum, f32 maximum) {
    NuVpDirtyPc = 1;
    vpCurrent.min_z = minimum;
    vpCurrent.max_z = maximum;
}
void __cdecl NuVpSetPosition2(f32 x, f32 y) {
    x += RenderPostViewportPrefixPc.x_offset;
    y += RenderPostViewportPrefixPc.y_offset;
    vpCurrent.x = (i32)(((f64)PS2_REZ_W * x / PS2_VREZ_W + ((f64)PS2_VCNTR_X - (PS2_REZ_W >> 1))) * 16.0);
    vpCurrent.y = (i32)(((f64)PS2_REZ_H * y / PS2_VREZ_H + ((f64)PS2_VCNTR_Y - (PS2_REZ_H >> 1))) * 16.0);
    NuVpDirtyPc = 1;
}
void __cdecl NuVpSetPosition(f32 x, f32 y) {
    f32 pixel_height = (f32)PS2_REZ_H;
    f32 virtual_height = (f32)PS2_VREZ_H;
    f32 y_ratio = pixel_height != 0.0f && virtual_height != 0.0f ? (f32)((f64)pixel_height / virtual_height) : 0.0f;
    f32 y_offset = (f32)((f64)y * 0.0625 - ((f64)PS2_VCNTR_Y - (PS2_REZ_H >> 1)));
    f32 new_y = y_offset != 0.0f && y_ratio != 0.0f ? (f32)((f64)y_offset / y_ratio) : 0.0f;
    f32 pixel_width = (f32)PS2_REZ_W;
    f32 virtual_width = (f32)PS2_VREZ_W;
    f32 x_ratio = pixel_width != 0.0f && virtual_width != 0.0f ? (f32)((f64)pixel_width / virtual_width) : 0.0f;
    f32 x_offset = (f32)((f64)x * 0.0625 - ((f64)PS2_VCNTR_X - (PS2_REZ_W >> 1)));
    f32 new_x = x_offset != 0.0f && x_ratio != 0.0f ? (f32)((f64)x_offset / x_ratio) : 0.0f;
    NuVpSetPosition2(new_x, new_y);
}
void __cdecl NuVpSetSize2(f32 width, f32 height) {
    width *= RenderPostViewportPrefixPc.width_scale;
    height *= RenderPostViewportPrefixPc.height_scale;
    vpCurrent.width = (i32)((f64)PS2_REZ_W * width / PS2_VREZ_W * 16.0);
    vpCurrent.height = (i32)((f64)PS2_REZ_H * height / PS2_VREZ_H * 16.0);
    NuVpDirtyPc = 1;
}

void __cdecl NuVpInit(void) {
    NuPs2GetViewport(&vpDevice);
    vpDevice.center_x = 0.5f;
    vpDevice.center_y = 0.5f;
    vpDevice.clip_min_x = 0.0f;
    vpDevice.clip_min_y = 0.0f;
    vpDevice.clip_max_x = 1.0f;
    vpDevice.clip_max_y = 1.0f;
    vpDevice.x = (i32)((u32)vpDevice.x << 4);
    vpDevice.y = (i32)((u32)vpDevice.y << 4);
    vpDevice.width = (i32)((u32)vpDevice.width << 4);
    vpDevice.height = (i32)((u32)vpDevice.height << 4);
    vpCurrent = vpDevice;
    NuVpDirtyPc = 1;
    NuVpUpdate();
}
