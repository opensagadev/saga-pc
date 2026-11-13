// RAD's Direct3D blitter (Bink SDK sample code) was built without the engine's SSE vector header: retail
// RadFmvDrawOutlinePc still takes a static-call argument in EBX (see saga.h).
#define SAGA_VENDORED_LIBRARY
#include "pc_fmv_native.h"
#include <stdlib.h>
#include <string.h>

struct RADFMV_DEVICE_METHODS_PC {
    void *unknown_000[17];
    HRESULT(__stdcall *present)(PC_D3D9_DEVICE_PC *, const RECT *, const RECT *, HWND, const RGNDATA *);
    void *unknown_048[5];
    HRESULT(__stdcall *create_texture)
    (PC_D3D9_DEVICE_PC *, u32, u32, u32, u32, u32, u32, RADFMV_TEXTURE_PC **, HANDLE *);
    void *unknown_060[17];
    HRESULT(__stdcall *begin_scene)(PC_D3D9_DEVICE_PC *);
    HRESULT(__stdcall *end_scene)(PC_D3D9_DEVICE_PC *);
    HRESULT(__stdcall *clear)(PC_D3D9_DEVICE_PC *, u32, const void *, u32, u32, f32, u32);
    void *unknown_0b0[39];
    HRESULT(__stdcall *draw_primitive_up)(PC_D3D9_DEVICE_PC *, u32, u32, const void *, u32);
    void *unknown_150[5];
    HRESULT(__stdcall *set_fvf)(PC_D3D9_DEVICE_PC *, u32);
};
struct RADFMV_FACTORY_METHODS_PC {
    void *unknown_000[10];
    HRESULT(__stdcall *check_format)(GRAPHICS_FACTORY_PC *, u32, u32, u32, u32, u32, u32);
};
STATIC_ASSERT(offsetof(RADFMV_DEVICE_METHODS_PC, create_texture) == 0x5c);
STATIC_ASSERT(offsetof(RADFMV_DEVICE_METHODS_PC, begin_scene) == 0xa4);
STATIC_ASSERT(offsetof(RADFMV_DEVICE_METHODS_PC, clear) == 0xac);
STATIC_ASSERT(offsetof(RADFMV_DEVICE_METHODS_PC, draw_primitive_up) == 0x14c);
STATIC_ASSERT(offsetof(RADFMV_DEVICE_METHODS_PC, set_fvf) == 0x164);
STATIC_ASSERT(offsetof(RADFMV_FACTORY_METHODS_PC, check_format) == 0x28);
#define RAD_DEVICE_METHODS(device) reinterpret_cast<const RADFMV_DEVICE_METHODS_PC *>((device)->methods)
#define RAD_FACTORY_METHODS(factory) reinterpret_cast<const RADFMV_FACTORY_METHODS_PC *>((factory)->methods)

extern "C" i32 __cdecl RadFmvInitializeFormatsPc(GRAPHICS_FACTORY_PC *factory, u32 display_format,
                                                 RADFMV_CONTEXT_PC *context) {
    const u32 formats[7][4] = {{22, 0, 4, 0}, {20, 8, 3, 0}, {23, 3, 2, 0}, {24, 2, 2, 0},
                               {21, 1, 4, 1}, {26, 5, 2, 1}, {25, 4, 2, 1}};
    u32 i;
    for (i = 0; i < 7; ++i) {
        if (RAD_FACTORY_METHODS(factory)->check_format(factory, 0, 1, display_format, 0, 3, formats[i][0]) >= 0) {
            context->opaque_format = formats[i][0];
            context->opaque_bink_format = formats[i][1];
            context->opaque_bytes = formats[i][2];
            break;
        }
    }
    if (i == 7)
        return 0;
    for (i = 0; i < 7; ++i) {
        if (formats[i][3] &&
            RAD_FACTORY_METHODS(factory)->check_format(factory, 0, 1, display_format, 0, 3, formats[i][0]) >= 0) {
            context->alpha_format = formats[i][0];
            context->alpha_bink_format = formats[i][1];
            context->alpha_bytes = formats[i][2];
            break;
        }
    }
    return i != 7;
}
extern "C" void __cdecl RadFmvBeginScenePc(RADFMV_CONTEXT_PC *context) {
    if (context) {
        RAD_DEVICE_METHODS(context->device)->clear(context->device, 0, NULL, 1, 0xff000000, 1.0f, 0);
        RAD_DEVICE_METHODS(context->device)->begin_scene(context->device);
    }
}
extern "C" void __cdecl RadFmvEndScenePc(RADFMV_CONTEXT_PC *context) {
    if (context) {
        RAD_DEVICE_METHODS(context->device)->end_scene(context->device);
        RAD_DEVICE_METHODS(context->device)->present(context->device, NULL, NULL, NULL, NULL);
    }
}
extern "C" void __cdecl RadFmvResizePc(RADFMV_CONTEXT_PC *context, u32 width, u32 height) {
    PC_VIEWPORT_PC viewport;
    if (width < 1)
        width = 1;
    if (height < 1)
        height = 1;
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = width;
    viewport.height = height;
    viewport.min_z = 0;
    viewport.max_z = 1;
    context->device->methods->set_viewport(context->device, &viewport);
}
static u32 RadFmvRoundTexturePc(u32 size) {
    if (size > 16) {
        if (size > 64) {
            if (size > 128) {
                if (size > 256)
                    return size > 512 ? 1024 : 512;
                return 256;
            }
            return 128;
        }
        return size > 32 ? 64 : 32;
    }
    if (size > 4)
        return size > 8 ? 16 : 8;
    return size > 2 ? 4 : size;
}
RADFMV_TEXTURE_PC *RadFmvCreateTexturePc(PC_D3D9_DEVICE_PC *device, u32 format, u32 pitch, u32 bytes, u32 width,
                                         u32 height) {
    (void)pitch;
    RADFMV_TEXTURE_PC *texture = NULL;
    RADFMV_LOCK_PC locked;
    if (RAD_DEVICE_METHODS(device)->create_texture(device, width, height, 1, 0x200, format, 0, &texture, NULL) < 0) {
        if (RAD_DEVICE_METHODS(device)->create_texture(device, width, height, 1, 0, format, 1, &texture, NULL) < 0)
            return texture;
    }
    if (texture->methods->lock(texture, 0, &locked, NULL, 0) < 0)
        return texture;
    u8 *destination = static_cast<u8 *>(locked.bits);
    u32 size = bytes * width;
    for (u32 row = 0; row < height; ++row) {
        memset(destination, 0, size);
        destination += locked.pitch;
    }
    texture->methods->unlock(texture, 0);
    return texture;
}
struct RADFMV_VERTEX_PC {
    f32 x, y, z, rhw;
    u32 diffuse, specular;
    f32 u, v;
};
STATIC_ASSERT(sizeof(RADFMV_VERTEX_PC) == 32);
static void RadFmvDrawQuadPc(PC_D3D9_DEVICE_PC *device, f32 x, f32 y, f32 x_scale, f32 y_scale, i32 width, i32 height,
                             f32 alpha) {
    RADFMV_VERTEX_PC vertices[4];
    vertices[0].x = (f32)(x - 0.5);
    vertices[0].y = (f32)(y - 0.5);
    vertices[0].z = 0;
    vertices[0].rhw = 1;
    vertices[0].diffuse = ((u32)(alpha * 255.0) << 24) | 0xffffff;
    vertices[0].specular = vertices[0].diffuse;
    vertices[0].u = 0;
    vertices[0].v = 0;
    vertices[1] = vertices[0];
    vertices[1].x = (f32)(x + width * x_scale - 0.5);
    vertices[1].u = 1;
    vertices[2] = vertices[0];
    vertices[2].y = (f32)(y + height * y_scale - 0.5);
    vertices[2].v = 1;
    vertices[3] = vertices[1];
    vertices[3].y = vertices[2].y;
    vertices[3].v = 1;
    SAGA_SKIN_CALL(skin_trace_invalidate, (unsigned)device);
    device->methods->set_vertex_shader(device, NULL);
    RAD_DEVICE_METHODS(device)->set_fvf(device, 0x1c4);
    RAD_DEVICE_METHODS(device)->draw_primitive_up(device, 5, 2, vertices, 32);
}
static void RadFmvDrawOutlinePc(PC_D3D9_DEVICE_PC *device, f32 x, f32 y, f32 xs, f32 ys, i32 w, i32 h) {
    RADFMV_VERTEX_PC vertices[12];
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0;
    vertices[0].rhw = 1;
    vertices[0].diffuse = 0xffffffff;
    vertices[0].specular = 0xffffffff;
    vertices[0].u = 0;
    vertices[0].v = 0;
    vertices[1] = vertices[0];
    vertices[1].x = x + w * xs;
    vertices[1].y = y;
    vertices[2] = vertices[1];
    vertices[3] = vertices[1];
    vertices[3].y = y + h * ys;
    vertices[4] = vertices[3];
    vertices[5] = vertices[3];
    vertices[5].x = x;
    vertices[6] = vertices[5];
    vertices[7] = vertices[0];
    vertices[8] = vertices[0];
    vertices[9] = vertices[3];
    vertices[10] = vertices[1];
    vertices[11] = vertices[5];
    for (u32 i = 8; i < 12; ++i)
        vertices[i].specular = vertices[i].diffuse = 0xffff0000;
    device->methods->set_texture_stage_state(device, 0, 2, 0);
    device->methods->set_texture_stage_state(device, 0, 5, 0);
    SAGA_SKIN_CALL(skin_trace_invalidate, (unsigned)device);
    device->methods->set_vertex_shader(device, NULL);
    RAD_DEVICE_METHODS(device)->set_fvf(device, 0x1c4);
    RAD_DEVICE_METHODS(device)->draw_primitive_up(device, 2, 6, vertices, 32);
}
static void RadFmvSetDrawStatePc(RADFMV_IMAGE_PC *image, f32 alpha) {
    if (alpha >= 0.9999 && image->alpha == 0) {
        image->device->methods->set_render_state(image->device, 27, 0);
        image->device->methods->set_texture_stage_state(image->device, 0, 4, 2);
        image->device->methods->set_texture_stage_state(image->device, 0, 2, 2);
    } else {
        image->device->methods->set_render_state(image->device, 27, 1);
        image->device->methods->set_render_state(image->device, 19, 5);
        image->device->methods->set_render_state(image->device, 20, 6);
        image->device->methods->set_texture_stage_state(image->device, 0, 4, 4);
        image->device->methods->set_texture_stage_state(image->device, 0, 2, 2);
        image->device->methods->set_texture_stage_state(image->device, 0, 3, 0);
    }
    image->device->methods->set_texture_stage_state(image->device, 0, 5, 2);
    image->device->methods->set_texture_stage_state(image->device, 0, 1, 4);
}
RADFMV_IMAGE_PC *RadFmvCreateTiledPc(RADFMV_CONTEXT_PC *context, u32 width, u32 height, u32 alpha, u32 maximum) {
    u32 bytes = alpha ? context->alpha_bytes : context->opaque_bytes;
    u32 pitch = (bytes * width + 15) & ~15u;
    u32 columns = width / maximum;
    u32 rw = width % maximum;
    u32 texture_width = RadFmvRoundTexturePc(rw);
    u32 rows = height / maximum;
    u32 rh = height % maximum;
    u32 texture_height = RadFmvRoundTexturePc(rh);
    u32 count = ((width + maximum - 1) / maximum) * ((height + maximum - 1) / maximum);
    RADFMV_IMAGE_PC *image = static_cast<RADFMV_IMAGE_PC *>(malloc(0x50 + count * 4));
    if (!image)
        return NULL;
    memset(image, 0, 0x50);
    image->textures = reinterpret_cast<RADFMV_TEXTURE_PC **>(image + 1);
    image->pitch = pitch;
    image->bytes_per_pixel = bytes;
    image->full_columns = columns;
    image->device = context->device;
    image->remainder_width = texture_width;
    image->width = width;
    image->remainder_height = texture_height;
    image->height = height;
    image->texture_count = count;
    image->source_remainder_width = rw;
    image->alpha = alpha;
    image->full_rows = rows;
    image->source_remainder_height = rh;
    image->maximum_texture = maximum;
    image->bink_format = alpha ? context->alpha_bink_format : context->opaque_bink_format;
    image->format = alpha ? context->alpha_format : context->opaque_format;
    RADFMV_TEXTURE_PC **out = image->textures;
    for (u32 row = 0; row < image->full_rows; ++row) {
        for (u32 col = 0; col < image->full_columns; ++col)
            *out++ = RadFmvCreateTexturePc(context->device, image->format, image->pitch, image->bytes_per_pixel,
                                           image->maximum_texture, image->maximum_texture);
        if (image->remainder_width)
            *out++ = RadFmvCreateTexturePc(context->device, image->format, image->pitch, image->bytes_per_pixel,
                                           image->remainder_width, image->maximum_texture);
    }
    if (image->remainder_height) {
        for (u32 col = 0; col < image->full_columns; ++col)
            *out++ = RadFmvCreateTexturePc(context->device, image->format, image->pitch, image->bytes_per_pixel,
                                           image->maximum_texture, image->remainder_height);
        if (image->remainder_width)
            *out++ = RadFmvCreateTexturePc(context->device, image->format, image->pitch, image->bytes_per_pixel,
                                           image->remainder_width, image->remainder_height);
    }
    for (u32 i = 0; i < image->texture_count; ++i) {
        if (image->textures[i] == NULL) {
            for (u32 j = 0; j < image->texture_count; ++j) {
                if (image->textures[j]) {
                    image->textures[j]->methods->release(image->textures[j]);
                    image->textures[j] = NULL;
                }
            }
            free(image);
            return NULL;
        }
    }
    image->single_texture = 0;
    return image;
}
void RadFmvDestroyTiledPc(RADFMV_IMAGE_PC *image) {
    if (image->textures) {
        for (u32 i = 0; i < image->texture_count; ++i)
            if (image->textures[i])
                image->textures[i]->methods->release(image->textures[i]);
        image->textures = NULL;
        free(image);
    }
}
i32 RadFmvLockTiledPc(RADFMV_IMAGE_PC *image, void **pixels, u32 *pitch, u32 *surface, u32 *x, u32 *y, u32 *w, u32 *h,
                      u32 discard) {
    RADFMV_LOCK_PC locked;
    if (image->lock_index >= image->texture_count ||
        image->textures[image->lock_index]->methods->lock(image->textures[image->lock_index], 0, &locked, NULL,
                                                          discard ? 0x2000 : 0) < 0) {
        image->lock_index = 0;
        return 0;
    }
    if (pixels)
        *pixels = locked.bits;
    if (pitch)
        *pitch = locked.pitch;
    if (surface)
        *surface = image->bink_format;
    u32 columns = image->full_columns + (image->remainder_width != 0);
    u32 row = image->lock_index / columns, col = image->lock_index - row * columns;
    if (x)
        *x = col * image->maximum_texture;
    if (y)
        *y = row * image->maximum_texture;
    if (w)
        *w = col >= image->full_columns ? image->remainder_width : image->maximum_texture;
    if (h)
        *h = row >= image->full_rows ? image->remainder_height : image->maximum_texture;
    ++image->lock_index;
    return 1;
}
void RadFmvUnlockTiledPc(RADFMV_IMAGE_PC *image) {
    image->textures[image->lock_index - 1]->methods->unlock(image->textures[image->lock_index - 1], 0);
}
void RadFmvBlitTiledPc(RADFMV_IMAGE_PC *image, f32 x, f32 y, f32 xs, f32 ys, f32 alpha) {
    RadFmvSetDrawStatePc(image, alpha);
    RADFMV_TEXTURE_PC **texture = image->textures;
    f32 x_step = (f32)image->maximum_texture * xs, y_step = (f32)image->maximum_texture * ys;
    f32 px;
    f32 py = y;
    for (u32 row = 0; row < image->full_rows; ++row) {
        px = x;
        for (u32 col = 0; col < image->full_columns; ++col) {
            image->device->methods->set_texture(image->device, 0, *texture);
            ++texture;
            RadFmvDrawQuadPc(image->device, px, py, xs, ys, image->maximum_texture, image->maximum_texture, alpha);
            px += x_step;
        }
        if (image->remainder_width) {
            image->device->methods->set_texture(image->device, 0, *texture);
            ++texture;
            RadFmvDrawQuadPc(image->device, px, py, xs, ys, image->remainder_width, image->maximum_texture, alpha);
        }
        py += y_step;
    }
    if (image->remainder_height) {
        px = x;
        for (u32 col = 0; col < image->full_columns; ++col) {
            image->device->methods->set_texture(image->device, 0, *texture);
            ++texture;
            RadFmvDrawQuadPc(image->device, px, py, xs, ys, image->maximum_texture, image->remainder_height, alpha);
            px += x_step;
        }
        if (image->remainder_width) {
            image->device->methods->set_texture(image->device, 0, *texture);
            RadFmvDrawQuadPc(image->device, px, py, xs, ys, image->remainder_width, image->remainder_height, alpha);
        }
    }
}
void RadFmvOutlineTiledPc(RADFMV_IMAGE_PC *image, f32 x, f32 y, f32 xs, f32 ys) {
    f32 x_step = (f32)image->maximum_texture * xs, y_step = (f32)image->maximum_texture * ys;
    f32 px;
    f32 py = y;
    for (u32 row = 0; row < image->full_rows; ++row) {
        px = x;
        for (u32 col = 0; col < image->full_columns; ++col) {
            RadFmvDrawOutlinePc(image->device, px, py, xs, ys, image->maximum_texture, image->maximum_texture);
            px += x_step;
        }
        if (image->remainder_width)
            RadFmvDrawOutlinePc(image->device, px, py, xs, ys, image->remainder_width, image->maximum_texture);
        py += y_step;
    }
    if (image->remainder_height) {
        px = x;
        for (u32 col = 0; col < image->full_columns; ++col) {
            RadFmvDrawOutlinePc(image->device, px, py, xs, ys, image->maximum_texture, image->remainder_height);
            px += x_step;
        }
        if (image->remainder_width)
            RadFmvDrawOutlinePc(image->device, px, py, xs, ys, image->remainder_width, image->remainder_height);
    }
}
void RadFmvBlitSinglePc(RADFMV_IMAGE_PC *image, f32 x, f32 y, f32 xs, f32 ys, f32 alpha) {
    RadFmvSetDrawStatePc(image, alpha);
    image->device->methods->set_texture(image->device, 0, image->texture);
    RadFmvDrawQuadPc(image->device, x, y, xs, ys, image->width, image->height, alpha);
}
void RadFmvOutlineSinglePc(RADFMV_IMAGE_PC *image, f32 x, f32 y, f32 xs, f32 ys) {
    RadFmvDrawOutlinePc(image->device, x, y, xs, ys, image->width, image->height);
}
i32 RadFmvLockSinglePc(RADFMV_IMAGE_PC *image, void **pixels, u32 *pitch, u32 *surface, u32 *x, u32 *y, u32 *w, u32 *h,
                       u32 discard) {
    if (!image->lock_index) {
        if (pixels || pitch) {
            RADFMV_LOCK_PC locked;
            if (image->texture->methods->lock(image->texture, 0, &locked, NULL, discard ? 0x2000 : 0) < 0)
                return 0;
            if (pixels)
                *pixels = locked.bits;
            if (pitch)
                *pitch = locked.pitch;
            if (x)
                *x = 0;
            if (y)
                *y = 0;
            if (w)
                *w = image->width;
            if (h)
                *h = image->height;
        }
        if (surface)
            *surface = image->bink_format;
        image->lock_index = 1;
        return 1;
    }
    image->lock_index = 0;
    return 0;
}
void RadFmvUnlockSinglePc(RADFMV_IMAGE_PC *image) {
    image->texture->methods->unlock(image->texture, 0);
}
RADFMV_IMAGE_PC *RadFmvCreateSinglePc(RADFMV_CONTEXT_PC *context, u32 width, u32 height, u32 alpha, u32 maximum) {
    (void)maximum;
    RADFMV_IMAGE_PC *image = static_cast<RADFMV_IMAGE_PC *>(malloc(0x50));
    if (!image)
        return NULL;
    memset(image, 0, 0x50);
    u32 bytes = alpha ? context->alpha_bytes : context->opaque_bytes;
    image->device = context->device;
    image->width = width;
    image->height = height;
    image->alpha = alpha;
    image->bytes_per_pixel = bytes;
    image->bink_format = alpha ? context->alpha_bink_format : context->opaque_bink_format;
    image->format = alpha ? context->alpha_format : context->opaque_format;
    image->lock_index = 0;
    image->texture = RadFmvCreateTexturePc(context->device, image->format, image->pitch, bytes, width, height);
    if (!image->texture) {
        free(image);
        image = NULL;
    }
    image->single_texture = 1;
    return image;
}
void RadFmvDestroySinglePc(RADFMV_IMAGE_PC *image) {
    if (image->texture)
        image->texture->methods->release(image->texture);
    free(image);
}
extern "C" void __cdecl RadFmvBlitImagePc(RADFMV_IMAGE_PC *image, f32 x, f32 y, f32 xs, f32 ys, f32 alpha) {
    if (image) {
        if (image->single_texture)
            RadFmvBlitSinglePc(image, x, y, xs, ys, alpha);
        else
            RadFmvBlitTiledPc(image, x, y, xs, ys, alpha);
    }
}
extern "C" void __cdecl RadFmvDrawLinesPc(RADFMV_IMAGE_PC *image, f32 x, f32 y, f32 xs, f32 ys) {
    if (image) {
        if (image->single_texture)
            RadFmvOutlineSinglePc(image, x, y, xs, ys);
        else
            RadFmvOutlineTiledPc(image, x, y, xs, ys);
    }
}
extern "C" i32 __cdecl RadFmvLockImagePc(RADFMV_IMAGE_PC *image, void **pixels, u32 *pitch, u32 *surface, u32 *x,
                                         u32 *y, u32 *w, u32 *h, u32 discard) {
    if (!image)
        return 0;
    return image->single_texture ? RadFmvLockSinglePc(image, pixels, pitch, surface, x, y, w, h, discard)
                                 : RadFmvLockTiledPc(image, pixels, pitch, surface, x, y, w, h, discard);
}
extern "C" void __cdecl RadFmvUnlockImagePc(RADFMV_IMAGE_PC *image) {
    if (image) {
        if (image->single_texture)
            RadFmvUnlockSinglePc(image);
        else
            RadFmvUnlockTiledPc(image);
    }
}
extern "C" RADFMV_IMAGE_PC *__cdecl RadFmvCreateImagePc(RADFMV_CONTEXT_PC *context, u32 width, u32 height, u32 alpha,
                                                        u32 maximum) {
    if ((RadFmvRoundTexturePc(width) == width && RadFmvRoundTexturePc(height) == height) || context->permits_nonpow2)
        return RadFmvCreateSinglePc(context, width, height, alpha, maximum);
    return RadFmvCreateTiledPc(context, width, height, alpha, maximum);
}
extern "C" void __cdecl RadFmvDestroyImagePc(RADFMV_IMAGE_PC *image) {
    if (image) {
        if (image->single_texture)
            RadFmvDestroySinglePc(image);
        else
            RadFmvDestroyTiledPc(image);
    }
}
