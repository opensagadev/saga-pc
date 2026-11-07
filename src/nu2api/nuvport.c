#include "saga.h"

void __cdecl NuVpSetIdentityPc(NUMTX *matrix) {
    if (matrix != NULL)
        *matrix = NuVpIdentityMatrixPc;
}

NUVPCURRENT *NuVpGetCurrentViewport(void) {
    return &vpCurrent;
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

typedef struct screengrab_tile_params_pc_s SCREENGRAB_TILE_PARAMS_PC;
