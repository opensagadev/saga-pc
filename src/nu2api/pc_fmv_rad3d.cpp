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

extern "C" i32 __cdecl RadFmvInitializeFormatsPc(GRAPHICS_FACTORY_PC *factory, u32 display_format,
                                                 RADFMV_CONTEXT_PC *context) {
    const u32 formats[7][4] = {{22, 0, 4, 0}, {20, 8, 3, 0}, {23, 3, 2, 0}, {24, 2, 2, 0},
                               {21, 1, 4, 1}, {26, 5, 2, 1}, {25, 4, 2, 1}};
    u32 i;
    const RADFMV_FACTORY_METHODS_PC *methods = reinterpret_cast<const RADFMV_FACTORY_METHODS_PC *>(factory->methods);
    for (i = 0; i < 7; ++i) {
        if (methods->check_format(factory, 0, 1, display_format, 0, 3, formats[i][0]) >= 0) {
            context->opaque_format = formats[i][0];
            context->opaque_bink_format = formats[i][1];
            context->opaque_bytes = formats[i][2];
            break;
        }
    }
    if (i == 7)
        return 0;
    for (i = 0; i < 7; ++i) {
        if (formats[i][3] && methods->check_format(factory, 0, 1, display_format, 0, 3, formats[i][0]) >= 0) {
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
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = width < 1 ? 1 : width;
    viewport.height = height < 1 ? 1 : height;
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
static RADFMV_TEXTURE_PC *RadFmvCreateTexturePc(PC_D3D9_DEVICE_PC *device, u32 format, u32 pitch, u32 bytes, u32 width,
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
    for (u32 row = 0; row < height; ++row) {
        memset(destination, 0, bytes * width);
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
    vertices[0].diffuse = ((u32)(i64)(alpha * 255.0) << 24) | 0xffffff;
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
    device->methods->set_vertex_shader(device, NULL);
    RAD_DEVICE_METHODS(device)->set_fvf(device, 0x1c4);
    RAD_DEVICE_METHODS(device)->draw_primitive_up(device, 5, 2, vertices, 32);
}
static void RadFmvDrawOutlinePc(PC_D3D9_DEVICE_PC *device, f32 x, f32 y, f32 xs, f32 ys, i32 w, i32 h) {
    RADFMV_VERTEX_PC vertices[12];
    RADFMV_VERTEX_PC a;
    a.x = x;
    a.y = y;
    a.z = 0;
    a.rhw = 1;
    a.diffuse = 0xffffffff;
    a.specular = 0xffffffff;
    a.u = 0;
    a.v = 0;
    vertices[0] = a;
    a.x = x + w * xs;
    vertices[1] = a;
    vertices[2] = a;
    a.y = y + h * ys;
    vertices[3] = a;
    vertices[4] = a;
    vertices[5] = a;
    vertices[5].x = x;
    vertices[6] = vertices[5];
    vertices[7] = vertices[0];
    vertices[8] = vertices[0];
    vertices[9] = vertices[3];
    vertices[10] = vertices[1];
    vertices[11] = vertices[5];
    for (u32 i = 8; i < 12; ++i)
        vertices[i].diffuse = vertices[i].specular = 0xffff0000;
    device->methods->set_texture_stage_state(device, 0, 2, 0);
    device->methods->set_texture_stage_state(device, 0, 5, 0);
    device->methods->set_vertex_shader(device, NULL);
    RAD_DEVICE_METHODS(device)->set_fvf(device, 0x1c4);
    RAD_DEVICE_METHODS(device)->draw_primitive_up(device, 2, 6, vertices, 32);
}
static void RadFmvSetDrawStatePc(RADFMV_IMAGE_PC *image, f32 alpha) {
    if (alpha >= 1.0 && image->alpha == 0) {
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
static RADFMV_IMAGE_PC *RadFmvCreateTiledPc(RADFMV_CONTEXT_PC *context, u32 width, u32 height, u32 alpha, u32 maximum) {
    u32 bytes = alpha ? context->alpha_bytes : context->opaque_bytes;
    u32 pitch = (bytes * width + 15) & ~15u;
    u32 columns = width / maximum, rows = height / maximum;
    u32 rw = width % maximum, rh = height % maximum;
    u32 count = ((width + maximum - 1) / maximum) * ((height + maximum - 1) / maximum);
    RADFMV_IMAGE_PC *image = static_cast<RADFMV_IMAGE_PC *>(malloc(0x50 + count * 4));
    if (!image)
        return NULL;
    memset(image, 0, 0x50);
    image->textures = reinterpret_cast<RADFMV_TEXTURE_PC **>(image + 1);
    image->device = context->device;
    image->bytes_per_pixel = bytes;
    image->width = width;
    image->height = height;
    image->pitch = pitch;
    image->texture_count = count;
    image->full_columns = columns;
    image->full_rows = rows;
    image->remainder_width = RadFmvRoundTexturePc(rw);
    image->remainder_height = RadFmvRoundTexturePc(rh);
    image->source_remainder_width = rw;
    image->source_remainder_height = rh;
    rw = image->remainder_width;
    rh = image->remainder_height;
    image->maximum_texture = maximum;
    image->alpha = alpha;
    image->bink_format = alpha ? context->alpha_bink_format : context->opaque_bink_format;
    image->format = alpha ? context->alpha_format : context->opaque_format;
    RADFMV_TEXTURE_PC **out = image->textures;
    for (u32 row = 0; row < rows; ++row) {
        for (u32 col = 0; col < columns; ++col)
            *out++ = RadFmvCreateTexturePc(context->device, image->format, pitch, bytes, maximum, maximum);
        if (rw)
            *out++ = RadFmvCreateTexturePc(context->device, image->format, pitch, bytes, rw, maximum);
    }
    if (rh) {
        for (u32 col = 0; col < columns; ++col)
            *out++ = RadFmvCreateTexturePc(context->device, image->format, pitch, bytes, maximum, rh);
        if (rw)
            *out++ = RadFmvCreateTexturePc(context->device, image->format, pitch, bytes, rw, rh);
    }
    for (u32 i = 0; i < count; ++i) {
        if (image->textures[i] == NULL) {
            for (u32 j = 0; j < count; ++j) {
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
static void RadFmvDestroyTiledPc(RADFMV_IMAGE_PC *image) {
    if (image->textures) {
        for (u32 i = 0; i < image->texture_count; ++i)
            if (image->textures[i])
                image->textures[i]->methods->release(image->textures[i]);
        image->textures = NULL;
        free(image);
    }
}
static i32 RadFmvLockTiledPc(RADFMV_IMAGE_PC *image, void **pixels, u32 *pitch, u32 *surface, u32 *x, u32 *y, u32 *w,
                             u32 *h, u32 discard) {
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
        *w = col < image->full_columns ? image->maximum_texture : image->remainder_width;
    if (h)
        *h = row < image->full_rows ? image->maximum_texture : image->remainder_height;
    ++image->lock_index;
    return 1;
}
static void RadFmvUnlockTiledPc(RADFMV_IMAGE_PC *image) {
    image->textures[image->lock_index - 1]->methods->unlock(image->textures[image->lock_index - 1], 0);
}
static void RadFmvBlitTiledPc(RADFMV_IMAGE_PC *image, f32 x, f32 y, f32 xs, f32 ys, f32 alpha) {
    RadFmvSetDrawStatePc(image, alpha);
    RADFMV_TEXTURE_PC **texture = image->textures;
    f32 x_step = (f32)image->maximum_texture * xs, y_step = (f32)image->maximum_texture * ys;
    f32 py = y;
    for (u32 row = 0; row < image->full_rows; ++row) {
        f32 px = x;
        for (u32 col = 0; col < image->full_columns; ++col) {
            image->device->methods->set_texture(image->device, 0, *texture++);
            RadFmvDrawQuadPc(image->device, px, py, xs, ys, image->maximum_texture, image->maximum_texture, alpha);
            px += x_step;
        }
        if (image->remainder_width) {
            image->device->methods->set_texture(image->device, 0, *texture++);
            RadFmvDrawQuadPc(image->device, px, py, xs, ys, image->remainder_width, image->maximum_texture, alpha);
        }
        py += y_step;
    }
    if (image->remainder_height) {
        f32 px = x;
        for (u32 col = 0; col < image->full_columns; ++col) {
            image->device->methods->set_texture(image->device, 0, *texture++);
            RadFmvDrawQuadPc(image->device, px, py, xs, ys, image->maximum_texture, image->remainder_height, alpha);
            px += x_step;
        }
        if (image->remainder_width) {
            image->device->methods->set_texture(image->device, 0, *texture);
            RadFmvDrawQuadPc(image->device, px, py, xs, ys, image->remainder_width, image->remainder_height, alpha);
        }
    }
}
static void RadFmvOutlineTiledPc(RADFMV_IMAGE_PC *image, f32 x, f32 y, f32 xs, f32 ys) {
    f32 x_step = (f32)image->maximum_texture * xs, y_step = (f32)image->maximum_texture * ys;
    f32 py = y;
    for (u32 row = 0; row < image->full_rows; ++row) {
        f32 px = x;
        for (u32 col = 0; col < image->full_columns; ++col) {
            RadFmvDrawOutlinePc(image->device, px, py, xs, ys, image->maximum_texture, image->maximum_texture);
            px += x_step;
        }
        if (image->remainder_width)
            RadFmvDrawOutlinePc(image->device, px, py, xs, ys, image->remainder_width, image->maximum_texture);
        py += y_step;
    }
    if (image->remainder_height) {
        f32 px = x;
        for (u32 col = 0; col < image->full_columns; ++col) {
            RadFmvDrawOutlinePc(image->device, px, py, xs, ys, image->maximum_texture, image->remainder_height);
            px += x_step;
        }
        if (image->remainder_width)
            RadFmvDrawOutlinePc(image->device, px, py, xs, ys, image->remainder_width, image->remainder_height);
    }
}
static void RadFmvBlitSinglePc(RADFMV_IMAGE_PC *image, f32 x, f32 y, f32 xs, f32 ys, f32 alpha) {
    RadFmvSetDrawStatePc(image, alpha);
    image->device->methods->set_texture(image->device, 0, image->texture);
    RadFmvDrawQuadPc(image->device, x, y, xs, ys, image->width, image->height, alpha);
}
static void RadFmvOutlineSinglePc(RADFMV_IMAGE_PC *image, f32 x, f32 y, f32 xs, f32 ys) {
    RadFmvDrawOutlinePc(image->device, x, y, xs, ys, image->width, image->height);
}
static i32 RadFmvLockSinglePc(RADFMV_IMAGE_PC *image, void **pixels, u32 *pitch, u32 *surface, u32 *x, u32 *y, u32 *w,
                              u32 *h, u32 discard) {
    if (image->lock_index) {
        image->lock_index = 0;
        return 0;
    }
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
static void RadFmvUnlockSinglePc(RADFMV_IMAGE_PC *image) {
    image->texture->methods->unlock(image->texture, 0);
}
static RADFMV_IMAGE_PC *RadFmvCreateSinglePc(RADFMV_CONTEXT_PC *context, u32 width, u32 height, u32 alpha,
                                             u32 maximum) {
    (void)maximum;
    RADFMV_IMAGE_PC *image = static_cast<RADFMV_IMAGE_PC *>(malloc(0x50));
    if (!image)
        return NULL;
    memset(image, 0, 0x50);
    image->device = context->device;
    image->width = width;
    image->height = height;
    image->alpha = alpha;
    image->bytes_per_pixel = alpha ? context->alpha_bytes : context->opaque_bytes;
    image->bink_format = alpha ? context->alpha_bink_format : context->opaque_bink_format;
    image->format = alpha ? context->alpha_format : context->opaque_format;
    image->lock_index = 0;
    image->texture =
        RadFmvCreateTexturePc(context->device, image->format, image->pitch, image->bytes_per_pixel, width, height);
    if (!image->texture) {
        free(image);
        image = NULL;
    }
    image->single_texture = 1;
    return image;
}
static void RadFmvDestroySinglePc(RADFMV_IMAGE_PC *image) {
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
