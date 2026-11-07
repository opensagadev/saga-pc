#define _CRT_SECURE_NO_DEPRECATE // NOLINT(readability-identifier-naming): VC8 CRT selector.
#include "saga.h"
#include <stdio.h>

extern "C" i32 __cdecl NuTexCreateFramebufferCopy(VARIPTR *buffer, VARIPTR end) {
    NuEffectTexLockVP(buffer, end);
    PC_FRAMEBUFFER_PC *framebuffer = NuFramebufferGetObject(1);
    i32 width = NuFramebufferGetWidth(framebuffer);
    i32 height = NuFramebufferGetHeight(framebuffer);
    GRAPHICS_RENDER_TEXTURE_PC *texture = PCGraphicsCreateEffectTexture(width, height, 1, 1, 1);
    NuEffectTexUnlockVP();
    return NuTexRegisterRenderTargetPc(texture);
}

extern "C" void __cdecl NuStrFixExt(char *, char *, char *, i32);

void __cdecl NuTexDestroyPS(NUTEXDIM_PC *texture) {
    if (texture != NULL) {
        PCFrameCriticalSectionEnter_candidate();
        PCDeviceReleaseTexture(&texture->resource);
        PCFrameCriticalSectionLeave_candidate();
    }
}

extern "C" i32 __cdecl NuTexGetReqSize(i32, i32) {
    return 0;
}

extern "C" i32 __cdecl NuTexReserve(i32) {
    return -1;
}

extern "C" void __cdecl NuTexUnReserve(void) {
}

void __cdecl NuTexCreatePS(NUTEXDIM_PC *texture) {
    PC_TEXTURE_LEVEL_DESC_PC description;
    if (texture != NULL && texture->image_data != NULL && texture->image_size != 0) {
        PCFrameCriticalSectionEnter_candidate();
        PCDeviceCreateTextureFromMemory(texture->image_data, texture->image_size, &texture->resource);
        texture->resource->methods->get_level_desc(texture->resource, 0, &description);
        PCFrameCriticalSectionLeave_candidate();
        texture->width = (i32)description.width;
        texture->height = (i32)description.height;
        texture->image_data = NULL;
        texture->image_size = 0;
    }
}

#include <string.h>
extern const char NuTextureSourcePathPC[];
extern const char NuTextureDdsExtensionPc[];
i32 __cdecl NuTexReadPc(char *name, VARIPTR *buffer, VARIPTR end) {
    char filename[1024];
    NUFILE file;
    i32 size, id;
    void *data;
    NUTEXDIM_PC *texture;
    (void)end;
    NuStrFixExt(filename, name, (char *)NuTextureDdsExtensionPc, sizeof(filename));
    file = NuFileOpen(filename, 0);
    if (file == 0)
        return 0;
    size = (i32)NuFileOpenSize(file);
    data = NuAlloc(size, NuTextureSourcePathPC, 0x2f);
    NuFileRead(file, data, size);
    NuFileClose(file);
    texture = (NUTEXDIM_PC *)buffer->addr;
    buffer->addr += sizeof(NUTEXDIM_PC);
    memset(texture, 0, sizeof(*texture));
    texture->image_data = data;
    texture->image_size = size;
    id = NuTexCreateNative(texture);
    NuFree(data, NuTextureSourcePathPC, 0x4d);
    return id;
}

#include <string.h>
extern const char NuTextureEnvironmentCubePathPc[];
extern const char NuTexturePhongCubePathPc[];
extern PC_TEXTURE_RESOURCE_PC *NuTextureEnvironmentCubePc, *NuTexturePhongCubePc;
void __cdecl PCDeviceCreateCubeTextureFromFile(char *path, PC_TEXTURE_RESOURCE_PC **resource);

void __cdecl NuTexInitExPS(VARIPTR *buffer) {
    (void)buffer;
    PCDeviceCreateCubeTextureFromFile((char *)NuTextureEnvironmentCubePathPc, &NuTextureEnvironmentCubePc);
    PCDeviceCreateCubeTextureFromFile((char *)NuTexturePhongCubePathPc, &NuTexturePhongCubePc);
}

/* Creates, once, the placeholder texture used for references that fail to resolve. */
extern "C" i32 NuTexGetUnresolvedTextureTIDPS() {
    if (NuUnresolvedTextureIDPC == -1) {
        NUTEXDIM_PC *texture = (NUTEXDIM_PC *)NuAlloc(sizeof(NUTEXDIM_PC), NuTextureSourcePathPC, 0xd9);
        memset(texture, 0, sizeof(*texture));
        texture->image_data = NuUnresolvedTextureDdsPc;
        texture->image_size = sizeof(NuUnresolvedTextureDdsPc);
        NuUnresolvedTextureIDPC = NuTexCreateNative(texture);
    }
    return NuUnresolvedTextureIDPC;
}

/* Bits per texel of a source image type. */
extern "C" i32 __cdecl NuTexPixelBitsPc(i32 type) {
    switch (type) {
        case 0:
        case 1:
            return 16;
        case 2:
            return 24;
        case 3:
            return 32;
        case 5:
        case 20:
            return 8;
        case 4:
            return 4;
        default:
            return 0;
    }
}

extern "C" i32 __cdecl NuTexImageSizePc(i32 type, i32 width, i32 height) {
    return NuTexPixelBitsPc(type) * width * height / 8;
}

extern "C" i32 __cdecl NuTexImageSizeMipsPc(i32 type, i32 width, i32 height, i32 mips) {
    i32 bits = NuTexPixelBitsPc(type);
    i32 size = (bits * width * height) >> 3;
    i32 level;

    for (level = 0; level < mips;) {
        level++;
        size += ((width >> level) * (height >> level) * bits) >> 3;
    }
    return size;
}

/* The leading pad keeps the 14-byte file header's 32-bit fields naturally aligned. */
typedef struct nutexbmp_file_header_pc_s {
    u16 pad_00;
    u16 type;
    u32 size;
    u16 reserved_08;
    u16 reserved_0a;
    u32 data_offset;
} NUTEXBMP_FILE_HEADER_PC;

typedef struct nutexbmp_info_header_pc_s {
    u32 size;
    i32 width;
    i32 height;
    u16 planes;
    u16 bit_count;
    u32 compression;
    u32 image_size;
    i32 x_resolution;
    i32 y_resolution;
    u32 colours_used;
    u32 colours_important;
} NUTEXBMP_INFO_HEADER_PC;

/* Loads an uncompressed BMP into a bitmap record, flipping it top to bottom. */
extern "C" i32 __cdecl NuTexLoadBmpPc(const char *path, i32 mips, NUTEXBMP_PC *bitmap) {
    NUFILE file;
    NUTEXBMP_FILE_HEADER_PC file_header;
    NUTEXBMP_INFO_HEADER_PC info_header;
    i32 palette_size;
    i32 width, height, levels;
    i32 row_size, size;
    u8 *row;
    u8 *top;
    u8 *bottom;
    NUTEXBMP_COLOUR_PC swapped;
    u8 blue;
    i32 i;

    file = NuFileOpen(path, 0);
    if (file != 0) {
        if (bitmap != NULL) {
            memset(bitmap, 0, sizeof(*bitmap));
            NuFilePos(file);
            NuFileRead(file, &file_header.type, 14);
            NuFileRead(file, &info_header, sizeof(info_header));
            switch (info_header.bit_count) {
                case 32:
                    bitmap->type = 3;
                    palette_size = 0;
                    break;
                case 24:
                    bitmap->type = 2;
                    palette_size = 0;
                    break;
                case 4:
                    bitmap->type = 4;
                    palette_size = 0x40;
                    NuFileRead(file, NuTexBmpPalettePc, 0x40);
                    for (i = 0; i < 0x10; i++) {
                        swapped = NuTexBmpPalettePc[i];
                        blue = swapped.blue;
                        swapped.blue = swapped.red;
                        swapped.red = blue;
                        NuTexBmpPalettePc[i] = swapped;
                    }
                    break;
                case 8:
                    bitmap->type = 5;
                    palette_size = 0x400;
                    NuFileRead(file, NuTexBmpPalettePc, 0x400);
                    for (i = 0; i < 0x100; i++) {
                        swapped = NuTexBmpPalettePc[i];
                        blue = swapped.blue;
                        swapped.blue = swapped.red;
                        swapped.red = blue;
                        NuTexBmpPalettePc[i] = swapped;
                    }
                    break;
                default:
                    NuTexBmpUnsupportedDepthPc(info_header.bit_count, path);
                    break;
            }
            bitmap->width = width = info_header.width;
            bitmap->height = height = info_header.height;
            bitmap->mips = mips;
            row_size = (NuTexPixelBitsPc(bitmap->type) * width + 7) / 8;
            size = 0;
            levels = mips + 1;
            do {
                size += NuTexPixelBitsPc(bitmap->type) * height * width / 8;
                width >>= 1;
                height >>= 1;
            } while (--levels > 0);
            bitmap->data = (u8 *)NuAlloc(size, NuTextureSourcePathPC, 0x195);
            if (palette_size != 0) {
                bitmap->palette = (u8 *)NuAlloc(palette_size, NuTextureSourcePathPC, 0x198);
            } else {
                bitmap->palette = NULL;
            }
            _flushall();
            NuFileSeek(file, file_header.data_offset, 0);
            NuFileRead(file, bitmap->data, NuTexImageSizePc(bitmap->type, bitmap->height, bitmap->width));
            row = (u8 *)NuAlloc(row_size, NuTextureSourcePathPC, 0x1a0);
            top = bitmap->data;
            bottom = top + NuTexImageSizePc(bitmap->type, bitmap->height, bitmap->width) - row_size;
            while (bottom > top) {
                memcpy(row, top, row_size);
                memcpy(top, bottom, row_size);
                memcpy(bottom, row, row_size);
                top += row_size;
                bottom -= row_size;
            }
            NuFree(row, NuTextureSourcePathPC, 0x1af);
            if (palette_size != 0) {
                memcpy(bitmap->palette, NuTexBmpPalettePc, palette_size);
            }
        }
        NuFileClose(file);
        return 1;
    }
    sprintf(NuTexBmpErrorTextPc, "Cannot open file: %s", path);
    return 0;
}

extern "C" NUTEXBMP_PC *__cdecl NuTexCreateBmpPc(const char *path) {
    NUTEXBMP_PC *bitmap = (NUTEXBMP_PC *)NuAlloc(sizeof(NUTEXBMP_PC), NuTextureSourcePathPC, 0x1e1);
    if (!NuTexLoadBmpPc(path, 0, bitmap)) {
        NuFree(bitmap, NuTextureSourcePathPC, 0x1e5);
        return NULL;
    }
    return bitmap;
}

void __cdecl PCGraphicsDestroyRenderTexture(GRAPHICS_RENDER_TEXTURE_PC *texture) {
    i32 i;
    i32 memory_delta;
    f32 multiplier;
    if (texture == NULL)
        return;
    for (i = 0; i != 32; ++i)
        if (&PCGraphicsRenderTextures[i] == texture)
            break;
    if (i == 32)
        return;
    if (texture->external_id == -1) {
        if (PCGraphicsRenderTextures[i].resource != NULL) {
            PCGraphicsRenderTextures[i].resource->methods->release(PCGraphicsRenderTextures[i].resource);
            PCGraphicsRenderTextures[i].resource = NULL;
        }
        if (PCGraphicsRenderTextures[i].surface != NULL) {
            PCGraphicsRenderTextures[i].surface->methods->release(PCGraphicsRenderTextures[i].surface);
            PCGraphicsRenderTextures[i].surface = NULL;
        }
    }
    multiplier = texture->mip_count == 1 ? 1.0f : 1.5f;
    memory_delta = (i32)((0u - (u32)texture->width * (u32)texture->height) * 4u);
    PCGraphicsRenderTextureBytes += (i32)((f64)memory_delta * (f64)multiplier);
    memset(&PCGraphicsRenderTextures[i], 0, sizeof(GRAPHICS_RENDER_TEXTURE_PC));
    PCGraphicsRenderTextureActive[i] = 0;
}

STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, original_mip_count) == 0xc);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, field_10) == 0x10);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, format) == 0x14);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, usage) == 0x18);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, pool) == 0x1c);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, flags) == 0x20);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, field_24) == 0x24);
typedef struct PC_TEXTURE_LEVEL_COUNT_METHODS_PC {
    void *reserved00[13];
    u32(__stdcall *get_level_count)(PC_TEXTURE_RESOURCE_PC *);
} PC_TEXTURE_LEVEL_COUNT_METHODS_PC;
STATIC_ASSERT(offsetof(PC_TEXTURE_LEVEL_COUNT_METHODS_PC, get_level_count) == 0x34);

static i32 PCGraphicsFormatToD3d(i32 format) {
    switch (format) {
        case 1:
            return 22;
        case 2:
            return 21;
        case 4:
            return 75;
        case 5:
            return 83;
        case 10:
            return 114;
        default:
            return -1;
    }
}
static i32 PCGraphicsFormatFromD3d(i32 format) {
    switch (format) {
        case 21:
        case 22:
            return 2;
        case 75:
            return 4;
        case 83:
            return 5;
        default:
            return 0;
    }
}
extern "C" void __cdecl PCGraphicsRenderTextureAllocationError(void) {
}
extern "C" void __cdecl PCGraphicsInitRenderTexture(GRAPHICS_RENDER_TEXTURE_PC *texture, i32 width, i32 height,
                                                    i32 levels, i32 usage, i32 pool, i32 format) {
    u32 d3d_usage;
    i32 memory_delta;
    f32 multiplier;
    texture->width = (u16)width;
    texture->height = (u16)height;
    texture->mip_count = (u16)levels;
    texture->original_mip_count = (u16)levels;
    texture->unknown_08 = 0;
    texture->field_10 = 1;
    texture->format = format;
    texture->usage = usage;
    texture->pool = pool;
    texture->external_id = -1;
    texture->surface = NULL;
    texture->field_24 = 0;
    texture->flags = 0x012449d0;
    if (format == 4) {
        texture->pool = 0;
        texture->mip_count = 1;
        PCDeviceCreateDepthSurface(
            texture->width, texture->height, PCGraphicsFormatToD3d(4), PCDeviceBackend.GetMultisampleType(),
            PCDeviceBackend.quality[PCDeviceBackend.multisample_index], FALSE, &texture->surface, NULL);
        PCGraphicsRenderTextureBytes += (i32)((u32)texture->width * texture->height * 4u);
        return;
    }
    d3d_usage = 0;
    if (texture->usage == 1)
        d3d_usage = format == 5 || format == 6 || format == 7 ? 2 : 1;
    PCDeviceCreateTexture(texture->width, texture->height, texture->mip_count, d3d_usage, PCGraphicsFormatToD3d(format),
                          texture->pool, &texture->resource);
    if (texture->resource != NULL) {
        texture->mip_count = (u16)((const PC_TEXTURE_LEVEL_COUNT_METHODS_PC *)texture->resource->methods)
                                 ->get_level_count(texture->resource);
        multiplier = texture->mip_count == 1 ? 1.0f : 1.5f;
        memory_delta = (i32)((u32)texture->width * texture->height * 4u);
        PCGraphicsRenderTextureBytes += (i32)((f64)memory_delta * (f64)multiplier);
    }
}
extern "C" GRAPHICS_RENDER_TEXTURE_PC *__cdecl PCGraphicsCreateColourTarget(i32 divider, i32 levels) {
    i32 format = PCGraphicsFormatFromD3d(PCDeviceBackend.parameters.format);
    u32 height = PCDeviceBackend.parameters.height / (u32)divider;
    u32 width = PCDeviceBackend.parameters.width / (u32)divider;
    GRAPHICS_RENDER_TEXTURE_PC *texture;
    i32 i;
    for (i = 0; i != 32; ++i)
        if (PCGraphicsRenderTextureActive[i] == 0)
            break;
    if (i == 32) {
        PCGraphicsRenderTextureAllocationError();
        texture = NULL;
    } else {
        PCGraphicsRenderTextureActive[i] = 1;
        texture = &PCGraphicsRenderTextures[i];
    }
    PCGraphicsInitRenderTexture(texture, width, height, levels, 1, 0, format);
    texture->flags = (texture->flags & 0xffffdbffu) | 0x1b00;
    return texture;
}
extern "C" GRAPHICS_RENDER_TEXTURE_PC *__cdecl PCGraphicsCreateColourTargetFormat(i32 divider, i32 levels, i32 format) {
    u32 height = PCDeviceBackend.parameters.height / (u32)divider;
    u32 width = PCDeviceBackend.parameters.width / (u32)divider;
    GRAPHICS_RENDER_TEXTURE_PC *texture;
    i32 i;
    for (i = 0; i != 32; ++i)
        if (PCGraphicsRenderTextureActive[i] == 0)
            break;
    if (i == 32) {
        PCGraphicsRenderTextureAllocationError();
        texture = NULL;
    } else {
        PCGraphicsRenderTextureActive[i] = 1;
        texture = &PCGraphicsRenderTextures[i];
    }
    PCGraphicsInitRenderTexture(texture, width, height, levels, 1, 0, format);
    texture->flags = (texture->flags & 0xffffdbffu) | 0x1b00;
    return texture;
}

extern "C" void __cdecl PCGraphicsResetRenderTexture(GRAPHICS_RENDER_TEXTURE_PC *texture) {
    i32 memory_delta;
    f32 multiplier;
    multiplier = texture->mip_count == 1 ? 1.0f : 1.5f;
    memory_delta = (i32)((0u - (u32)texture->width * (u32)texture->height) * 4u);
    PCGraphicsRenderTextureBytes += (i32)(i64)((f64)memory_delta * (f64)multiplier);
    if (texture->external_id == -1 && texture->resource != NULL) {
        texture->resource->methods->release(texture->resource);
        texture->resource = NULL;
    }
    texture->width = 0;
    texture->height = 0;
    texture->unknown_08 = 0;
    texture->mip_count = 0;
    texture->original_mip_count = 0;
    texture->field_10 = 1;
    texture->format = 0;
    texture->usage = 0;
    texture->pool = 0;
    texture->resource = NULL;
    texture->external_id = -1;
    texture->field_24 = 0;
    texture->flags = 0x012449d0;
    if (texture->surface != NULL) {
        texture->surface->methods->release(texture->surface);
        texture->surface = NULL;
    }
}

extern "C" void __cdecl PCGraphicsInitRenderTexturePool(VARIPTR *buffer, VARIPTR end) {
    i32 index;
    (void)buffer;
    (void)end;
    memset(PCGraphicsRenderTextures, 0, sizeof(PCGraphicsRenderTextures));
    for (index = 0; index != 32; ++index) {
        PCGraphicsRenderTextureActive[index] = 0;
        PCGraphicsResetRenderTexture(&PCGraphicsRenderTextures[index]);
    }
}

extern "C" GRAPHICS_RENDER_TEXTURE_PC *__cdecl PCGraphicsCreateEffectTexture(i32 width, i32 height, i32 levels,
                                                                             i32 usage, i32 format) {
    GRAPHICS_RENDER_TEXTURE_PC *texture;
    i32 index;
    for (index = 0; index != 32; ++index)
        if (PCGraphicsRenderTextureActive[index] == 0)
            break;
    if (index == 32) {
        PCGraphicsRenderTextureAllocationError();
        texture = NULL;
    } else {
        PCGraphicsRenderTextureActive[index] = 1;
        texture = &PCGraphicsRenderTextures[index];
    }
    usage &= ~0x10;
    if (usage != 0 && usage != 1) {
        PCGraphicsRenderTextureAllocationError();
        return NULL;
    }
    PCGraphicsInitRenderTexture(texture, width, height, levels, usage, 0, format);
    return texture;
}
extern "C" void __cdecl PCGraphicsResizeEffectTexture(GRAPHICS_RENDER_TEXTURE_PC *texture, i32 width, i32 height,
                                                      i32 levels) {
    if (texture == NULL)
        return;
    f32 multiplier = texture->mip_count == 1 ? 1.0f : 1.5f;
    i32 memory_delta = (i32)((0u - (u32)texture->width * texture->height) * 4u);
    PCGraphicsRenderTextureBytes += (i32)(i64)((f64)memory_delta * (f64)multiplier);
    if (texture->external_id == -1 && texture->resource != NULL) {
        texture->resource->methods->release(texture->resource);
        texture->resource = NULL;
    }
    if (texture->surface != NULL) {
        texture->surface->methods->release(texture->surface);
        texture->surface = NULL;
    }
    if (width <= 0)
        width = texture->width;
    if (height <= 0)
        height = texture->height;
    if (levels <= 0)
        levels = texture->original_mip_count;
    PCGraphicsInitRenderTexture(texture, width, height, levels, texture->usage, texture->pool, texture->format);
}
extern "C" void __cdecl PCGraphicsReleaseEffectTextures(void) {
    for (i32 index = 0; index != 32; ++index) {
        GRAPHICS_RENDER_TEXTURE_PC *texture = &PCGraphicsRenderTextures[index];
        if (PCGraphicsRenderTextureActive[index] && texture->external_id == -1) {
            if (texture->resource != NULL) {
                texture->resource->methods->release(texture->resource);
                texture->resource = NULL;
            }
            if (texture->surface != NULL) {
                texture->surface->methods->release(texture->surface);
                texture->surface = NULL;
            }
            f32 multiplier = texture->mip_count == 1 ? 1.0f : 1.5f;
            i32 memory_delta = (i32)((0u - (u32)texture->width * texture->height) * 4u);
            PCGraphicsRenderTextureBytes += (i32)(i64)((f64)memory_delta * (f64)multiplier);
        }
    }
}
extern "C" void __cdecl PCGraphicsRestoreEffectTextures(i32 unused) {
    (void)unused;
    for (i32 index = 0; index != 32; ++index) {
        GRAPHICS_RENDER_TEXTURE_PC *texture = &PCGraphicsRenderTextures[index];
        if (PCGraphicsRenderTextureActive[index] && texture->usage == 1 && texture->external_id == -1 &&
            texture->resource == NULL && texture->surface == NULL)
            PCGraphicsInitRenderTexture(texture, texture->width, texture->height, texture->mip_count, texture->usage,
                                        texture->pool, texture->format);
    }
}
extern "C" void __cdecl PCGraphicsBindEffectTexture(GRAPHICS_RENDER_TEXTURE_PC *texture, u32 stage) {
    PCDeviceSetTexture(stage, texture != NULL ? texture->resource : NULL);
}
extern "C" i32 __cdecl PCGraphicsEffectTextureWidth(GRAPHICS_RENDER_TEXTURE_PC *texture) {
    return texture->width;
}
extern "C" i32 __cdecl PCGraphicsEffectTextureHeight(GRAPHICS_RENDER_TEXTURE_PC *texture) {
    return texture->height;
}
extern "C" void __cdecl PCGraphicsEffectTextureDimensions(GRAPHICS_RENDER_TEXTURE_PC *texture, u16 level, i32 *width,
                                                          i32 *height) {
    *width = (texture->width >> (u8)level) & ~1;
    *height = (texture->height >> (u8)level) & ~1;
}

FRAME_NATIVE_TEXTURE_PC *FRAME_NATIVE_TEXTURE_POOL_PC::Allocate() {
    i32 index;
    for (index = cursor; index < 16; ++index) {
        if (((occupied[index >> 3] >> (index & 7)) & 1) == 0) {
            occupied[index >> 3] |= (u8)(1u << (index & 7));
            cursor = (index + 1) % 16;
            return &records[index];
        }
    }
    for (index = 0; index < cursor; ++index) {
        if (((occupied[index >> 3] >> (index & 7)) & 1) == 0) {
            occupied[index >> 3] |= (u8)(1u << (index & 7));
            cursor = (index + 1) % 16;
            return &records[index];
        }
    }
    return NULL;
}

extern "C" i32 __cdecl NuTexRegisterRenderTargetPc(GRAPHICS_RENDER_TEXTURE_PC *target) {
    FRAME_NATIVE_TEXTURE_PC *texture = FrameNativeTexturePoolPc.Allocate();
    texture->render_texture = target;
    texture->texture_id = NuTexRegisterDimensionsPc(&texture->dimensions);
    if (texture->texture_id == 0) {
        i32 index = (i32)((u32)texture - (u32)&FrameNativeTexturePoolPc.records[0]) / 0x30;
        FrameNativeTexturePoolPc.occupied[index >> 3] &= (u8) ~(1u << (index & 7));
        FrameNativeTexturePoolPc.cursor = index;
        return 0;
    }
    texture->dimensions.width = target->width;
    texture->dimensions.height = target->height;
    texture->dimensions.ref_count = 1;
    memset(texture->dimensions.checksum, 0, sizeof(texture->dimensions.checksum));
    texture->dimensions.image_data = NULL;
    texture->dimensions.image_size = 0;
    texture->dimensions.resource = target->resource;
    return texture->texture_id;
}
extern "C" void __cdecl PCGraphicsBindTextureDimensionsPc(NUTEXDIM_PC *texture, u32 stage) {
    PCDeviceSetTexture(stage, texture->resource);
}

STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, caps) == 8);
STATIC_ASSERT(sizeof(GRAPHICS_RENDER_TEXTURE_PC) == 0x30);

extern "C" {
    extern u32 PCGraphicsPostEffectWidthPc;
    extern u32 PCGraphicsPostEffectHeightPc;
    extern GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsPostEffectWorkTexturePc;
    extern GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsPostEffectBackBufferTexturePc;
}

extern "C" void __cdecl PCGraphicsCreatePostEffectWorkTexture(i32 width, i32 height) {
    (void)width;
    (void)height;
    PCGraphicsPostEffectWorkTexturePc =
        PCGraphicsCreateEffectTexture(PCGraphicsPostEffectWidthPc, PCGraphicsPostEffectHeightPc, 1, 0x11, 2);
}

STATIC_ASSERT(sizeof(GRAPHICS_RENDER_TEXTURE_PC) == 0x30);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, mip_count) == 0xa);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, flags) == 0x20);
STATIC_ASSERT(offsetof(GRAPHICS_RENDER_TEXTURE_PC, field_24) == 0x24);

extern "C" i32 __cdecl PCGraphicsEffectTextureMipCount(GRAPHICS_RENDER_TEXTURE_PC *texture) {
    return texture->mip_count;
}

extern "C" void __cdecl PCGraphicsEffectTextureSetLowNibbles(GRAPHICS_RENDER_TEXTURE_PC *texture, i32 low, i32 high) {
    texture->flags = (texture->flags & ~0xffu) | (((u32)high & 0xfu) << 4) | ((u32)low & 0xfu);
}

extern "C" void __cdecl PCGraphicsEffectTextureSetField24(GRAPHICS_RENDER_TEXTURE_PC *texture, i32 value) {
    texture->field_24 = (u32)value;
}

extern "C" void __cdecl PCGraphicsEffectTextureSetFilterFields(GRAPHICS_RENDER_TEXTURE_PC *texture, i32 first,
                                                               i32 second, i32 third) {
    i32 value;
    switch (first) {
        case 0:
            value = 0;
            break;
        case 1:
            value = 1;
            break;
        case 2:
            value = 2;
            break;
        case 3:
            value = 3;
            break;
        default:
            value = -1;
            break;
    }
    texture->flags ^= (((u32)value << 17) ^ texture->flags) & 0xe0000u;
    switch (second) {
        case 0:
            value = 0;
            break;
        case 1:
            value = 1;
            break;
        case 2:
            value = 2;
            break;
        case 3:
            value = 3;
            break;
        default:
            value = -1;
            break;
    }
    texture->flags ^= (((u32)value << 20) ^ texture->flags) & 0x700000u;
    switch (third) {
        case 0:
            value = 0;
            break;
        case 1:
            value = 1;
            break;
        case 2:
            value = 2;
            break;
        case 3:
            value = 3;
            break;
        default:
            value = -1;
            break;
    }
    texture->flags ^= (((u32)value << 23) ^ texture->flags) & 0x3800000u;
}

extern "C" void __cdecl PCGraphicsEffectTextureSetWrapFields(GRAPHICS_RENDER_TEXTURE_PC *texture, i32 first,
                                                             i32 second) {
    i32 value;
    switch (first) {
        case 0:
            value = 1;
            break;
        case 1:
            value = 2;
            break;
        case 2:
            value = 3;
            break;
        case 3:
            value = 4;
            break;
        default:
            value = -1;
            break;
    }
    texture->flags ^= (((u32)value << 8) ^ texture->flags) & 0x700u;
    switch (second) {
        case 0:
            value = 1;
            break;
        case 1:
            value = 2;
            break;
        case 2:
            value = 3;
            break;
        case 3:
            value = 4;
            break;
        default:
            value = -1;
            break;
    }
    texture->flags ^= (((u32)value << 11) ^ texture->flags) & 0x3800u;
}

extern "C" u32 *__cdecl PCGraphicsEffectTextureSamplerPc(GRAPHICS_RENDER_TEXTURE_PC *texture) {
    return &texture->flags;
}
extern "C" GRAPHICS_RENDER_TEXTURE_PC *__cdecl PCGraphicsGetFramebufferAttachmentPc(PC_FRAMEBUFFER_PC *framebuffer,
                                                                                    i32 index, i32 *level, i32 *field) {
    PC_EFFECT_SURFACE_PC *target = &framebuffer->targets[index];
    GRAPHICS_RENDER_TEXTURE_PC *texture = target->texture;
    if (texture == NULL) {
        if (level != NULL)
            *level = 0;
        if (field != NULL)
            *field = 0;
        return NULL;
    }
    if (level != NULL)
        *level = target->level;
    /* Retail tests field but writes through level, including when level is null. */
    if (field != NULL)
        *level = target->field_10;
    return texture;
}
extern "C" void __cdecl PCGraphicsFramebufferBarrierPc(i32, i32) {
}
extern "C" void __cdecl PCGraphicsFramebufferActionPc(i32) {
}
extern "C" void __cdecl PCGraphicsSubmitViewportPc(i32 x, i32 y, i32 width, i32 height) {
    PC_VIEWPORT_PC viewport;
    viewport.x = x;
    viewport.y = y;
    viewport.width = width;
    viewport.height = height;
    viewport.min_z = 0.0f;
    viewport.max_z = 1.0f;
    NuVpSubmitPc(&viewport);
}
extern "C" void __cdecl PCGraphicsSetSamplerTuplePc(u32 stage, const u32 *tuple) {
    union {
        f32 value;
        u32 bits;
    } bias;
    PCDeviceSetSamplerState(stage, 9, tuple[0] & 15);
    bias.value = (((tuple[0] >> 4) ^ tuple[0]) & 15) == 0 ? -10.0f : 0.0f;
    PCDeviceSetSamplerState(stage, 8, bias.bits);
    PCDeviceSetSamplerState(stage, 1, (tuple[0] >> 8) & 7);
    PCDeviceSetSamplerState(stage, 2, (tuple[0] >> 11) & 7);
    PCDeviceSetSamplerState(stage, 3, (tuple[0] >> 14) & 7);
    PCDeviceSetSamplerState(stage, 5, (tuple[0] >> 17) & 7);
    PCDeviceSetSamplerState(stage, 6, (tuple[0] >> 20) & 7);
    PCDeviceSetSamplerState(stage, 7, (tuple[0] >> 23) & 7);
    PCDeviceSetSamplerState(stage, 4, tuple[1]);
    PCDeviceSamplerInvalidation[stage][0] = tuple[0];
    PCDeviceSamplerInvalidation[stage][1] = tuple[1];
}
extern "C" void __cdecl PCGraphicsBindEffectTexturePc(u32 stage, GRAPHICS_RENDER_TEXTURE_PC *texture) {
    if (texture != NULL) {
        const u32 *tuple = &texture->flags;
        if ((u32)PCDeviceSamplerInvalidation[stage][0] != tuple[0] ||
            (u32)PCDeviceSamplerInvalidation[stage][1] != tuple[1])
            PCGraphicsSetSamplerTuplePc(stage, tuple);
        PCDeviceSetTexture(stage, texture->resource);
    } else {
        PCDeviceSetTexture(stage, NULL);
    }
}

/* Font atlas builder: scans a BMP glyph strip for glyph cells and builds a font around it. */
extern "C" i32 NuStrLenW(const NUWCHAR *str);

/* Converts a byte string into glyph indices of the font. */
extern "C" void __cdecl NuQFntAsciiToQCodePc(VUFNT *font, u16 *encoded, const char *text) {
    while (*text != '\0') {
        *encoded = UnicodeToIndexFast(font->unicode_map, font->unicode_count, (u16)(i16)*text);
        text++;
        encoded++;
    }
    *encoded = 0;
}

/* Nonzero when any texel of the 8-bit column at (x, y) differs from the key value. */
extern "C" i32 __cdecl NuQFntColumnDiffers8Pc(NUTEXBMP_PC *bitmap, i32 x, i32 y, i32 count, i32 value) {
    i32 width = bitmap->width;
    u8 *texel = bitmap->data + width * y + x;
    i32 i;

    for (i = 0; i < count; i++) {
        if (*texel != value) {
            return 1;
        }
        texel += width;
    }
    return 0;
}

/* Nonzero when any texel of the 32-bit column at (x, y) has a nonzero alpha. */
static __forceinline i32 NuQFntColumnOpaque32Inline(NUTEXBMP_PC *bitmap, i32 x, i32 y, i32 count) {
    i32 width = bitmap->width;
    u8 *alpha = bitmap->data + (width * y + x) * 4 + 3;
    i32 i;

    for (i = 0; i < count && y + i < bitmap->height; i++) {
        if (*alpha != 0) {
            return 1;
        }
        alpha += width * 4;
    }
    return 0;
}

extern "C" i32 __cdecl NuQFntColumnOpaque32Pc(NUTEXBMP_PC *bitmap, i32 x, i32 y, i32 count) {
    return NuQFntColumnOpaque32Inline(bitmap, x, y, count);
}

extern "C" i32 __cdecl NuQFntFindGlyphStart8Pc(NUTEXBMP_PC *bitmap, i32 count, i32 *x, i32 *y, i32 value) {
    do {
        do {
            if (NuQFntColumnDiffers8Pc(bitmap, *x, *y, count, value)) {
                return 1;
            }
            (*x)++;
        } while (*x < bitmap->width);
        *x = 0;
        *y += count;
    } while (*y + count < bitmap->height);
    return 0;
}

extern "C" i32 __cdecl NuQFntFindGlyphEnd8Pc(NUTEXBMP_PC *bitmap, i32 count, i32 *x, i32 *y, i32 value) {
    do {
        do {
            if (!NuQFntColumnDiffers8Pc(bitmap, *x, *y, count, value)) {
                return 1;
            }
            (*x)++;
        } while (*x < bitmap->width);
        *x = 0;
        *y += count;
    } while (*y + count < bitmap->height);
    return 0;
}

extern "C" i32 __cdecl NuQFntFindGlyphStart32Pc(NUTEXBMP_PC *bitmap, i32 count, i32 *x, i32 *y) {
    do {
        do {
            if (NuQFntColumnOpaque32Inline(bitmap, *x, *y, count)) {
                return 1;
            }
            (*x)++;
        } while (*x < bitmap->width);
        *x = 0;
        *y += count;
    } while (*y + count < bitmap->height);
    return 0;
}

extern "C" i32 __cdecl NuQFntFindGlyphEnd32Pc(NUTEXBMP_PC *bitmap, i32 count, i32 *x, i32 *y) {
    do {
        do {
            if (!NuQFntColumnOpaque32Inline(bitmap, *x, *y, count)) {
                return 1;
            }
            (*x)++;
        } while (*x < bitmap->width);
        *x = 0;
        *y += count;
    } while (*y + count < bitmap->height);
    return 0;
}

extern "C" void __cdecl NuQFntScanResetPc(void) {
    NuQFntScanYPc = 0;
    NuQFntScanXPc = 0;
}

/* Finds the next glyph cell in an 8-bit strip; returns nonzero once the strip is exhausted. */
extern "C" i32 __cdecl NuQFntNextGlyph8Pc(NUTEXBMP_PC *bitmap, i32 count, i32 value, f32 *x, f32 *y, f32 *width) {
    i32 exhausted = 0;
    i32 start;

    if (!NuQFntFindGlyphStart8Pc(bitmap, count, &NuQFntScanXPc, &NuQFntScanYPc, value)) {
        exhausted = 1;
        NuQFntScanXPc = 0;
        NuQFntScanYPc = 0;
    }
    *x = (f32)NuQFntScanXPc;
    *y = (f32)NuQFntScanYPc;
    start = NuQFntScanXPc;
    if (!NuQFntFindGlyphEnd8Pc(bitmap, count, &NuQFntScanXPc, &NuQFntScanYPc, value)) {
        exhausted = 1;
        NuQFntScanXPc = bitmap->width;
        NuQFntScanYPc = bitmap->height;
    }
    *width = (f32)(NuQFntScanXPc - start);
    return exhausted;
}

/* Finds the next glyph cell in a 32-bit strip; returns nonzero once the strip is exhausted. */
extern "C" i32 __cdecl NuQFntNextGlyph32Pc(NUTEXBMP_PC *bitmap, i32 count, f32 *x, f32 *y, f32 *width) {
    i32 exhausted = 0;
    i32 start;

    if (!NuQFntFindGlyphStart32Pc(bitmap, count, &NuQFntScanXPc, &NuQFntScanYPc)) {
        exhausted = 1;
        NuQFntScanYPc = 0;
        NuQFntScanXPc = 0;
    }
    *x = (f32)NuQFntScanXPc;
    *y = (f32)NuQFntScanYPc;
    start = NuQFntScanXPc;
    if (!NuQFntFindGlyphEnd32Pc(bitmap, count, &NuQFntScanXPc, &NuQFntScanYPc)) {
        exhausted = 1;
        NuQFntScanXPc = bitmap->width;
        NuQFntScanYPc = bitmap->height;
    }
    *width = (f32)(NuQFntScanXPc - start);
    return exhausted;
}

extern "C" u16 __cdecl NuQFntMapLookupPc(VUCHARIDX *map, i32 count, u16 unicode) {
    u16 i;

    for (i = 0; i < count; i++) {
        if (map[i].unicode == unicode) {
            return map[i].index;
        }
    }
    return 0xffff;
}

extern "C" i32 __cdecl NuQFntMapAddPc(VUCHARIDX *map, i32 slot, u16 unicode, u16 index) {
    if (slot > 808 || slot < 0) {
        NuQFntAtlasErrorPc();
    }
    map[slot].unicode = unicode;
    map[slot].index = index;
    return slot + 1;
}

/* The material fields the atlas font set-up writes. */
typedef struct nuqfnt_atlas_material_pc_s {
    u8 unknown_000[0x40];
    u32 attributes;
    u32 attributes_high;
    u8 unknown_048[0xc];
    NUVEC diffuse;
    u8 unknown_060[0x10];
    f32 opacity;
    i16 texture_id;
} NUQFNT_ATLAS_MATERIAL_PC;
STATIC_ASSERT(offsetof(NUQFNT_ATLAS_MATERIAL_PC, attributes) == 0x40);
STATIC_ASSERT(offsetof(NUQFNT_ATLAS_MATERIAL_PC, diffuse) == 0x54);
STATIC_ASSERT(offsetof(NUQFNT_ATLAS_MATERIAL_PC, opacity) == 0x70);
STATIC_ASSERT(offsetof(NUQFNT_ATLAS_MATERIAL_PC, texture_id) == 0x74);

/* Font material set-up used by the atlas builder; mirrors NuQFntReadPS. */
extern "C" i32 __cdecl NuQFntAtlasReadPSPc(VUFNT *font, i32 texture_id, i32 flags, i32 plane, VARIPTR *buffer,
                                           VARIPTR end) {
    NUQFNT_RENDER_DATA_PC *platform;
    NUQFNT_ATLAS_MATERIAL_PC *material;

    (void)end;
    NuTexGetWidth(texture_id);
    NuTexGetHeight(texture_id);
    buffer->addr = (buffer->addr + 15) & ~15U;
    font->platform_data = (NUQFNT_RENDER_DATA_PC *)buffer->void_ptr;
    buffer->addr += 0x58;
    platform = font->platform_data;
    font->color_abgr = &platform->colour;
    font->x_scale = (f32 *)((u8 *)platform + 0x50);
    font->y_scale = (f32 *)((u8 *)platform + 0x54);
    font->material = NuMtlCreateEx(1, plane);
    if (font->material == NULL) {
        return 0;
    }
    ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->diffuse.x = 1.0f;
    ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->diffuse.y = 1.0f;
    ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->diffuse.z = 1.0f;
    ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes =
        (((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes & ~0x1000U) | 0x2000U;
    ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes |= 0xc000U;
    ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->opacity = 1.0f;
    ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes =
        (((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes & ~14U) | 1U;
    ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes |= 0x40000U;
    ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->texture_id = (i16)texture_id;
    material = (NUQFNT_ATLAS_MATERIAL_PC *)font->material;
    if (flags & 8) {
        material->attributes_high &= ~4U;
    } else {
        material->attributes_high |= 4U;
    }
    ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes =
        (((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes & ~0x10000U) | 0x20000U;
    if (flags & 2) {
        ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes &= ~15U;
    } else {
        ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes =
            (((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes & ~14U) | 1U;
    }
    if (flags & 0x40) {
        ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes &= ~0xc000U;
    } else if (flags & 0x20) {
        ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes =
            (((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes & ~0x8000U) | 0x4000U;
    } else {
        ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes |= 0xc000U;
    }
    if (flags & 4) {
        ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes =
            (((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes & ~0x20U) | 0x10U;
    } else if (flags & 0x10) {
        ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes =
            (((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes & ~0x10U) | 0x20U;
    } else {
        ((NUQFNT_ATLAS_MATERIAL_PC *)font->material)->attributes &= ~0x30U;
    }
    NuMtlUpdate(font->material);
    return 1;
}

extern "C" int __cdecl NuQFntMapComparePc(const void *a, const void *b) {
    u16 left = ((const VUCHARIDX *)a)->unicode;
    u16 right = ((const VUCHARIDX *)b)->unicode;

    if (left < right) {
        return -1;
    }
    return left != right;
}

#define NUQFNT_ATLAS_MAP_SIZE 808
#define NUQFNT_ATLAS_GLYPH_SIZE 750

/* Finds the next glyph cell with the scanner that suits the strip's format. */
#define NUQFNT_ATLAS_NEXT_GLYPH()                                                                                      \
    if (bitmap->type == 5) {                                                                                           \
        NuQFntNextGlyph8Pc(bitmap, cell_height, key_colour, &x, &y, &width);                                           \
    } else {                                                                                                           \
        NuQFntNextGlyph32Pc(bitmap, cell_height, &x, &y, &width);                                                      \
    }

/* Builds a font from a BMP glyph strip, one cell per character of text in strip order. */
extern "C" VUFNT *__cdecl NuQFntCreateFromBmpPc(const char *path, i32 cell_height, i32 baseline, i32 key_colour,
                                                u16 *text, i32 flags, i32 plane, VARIPTR *buffer, VARIPTR end) {
    f32 width, y, x;
    i32 max_count;
    i32 count;
    NUTEXBMP_PC *bitmap;
    VUCHARIDX map[NUQFNT_ATLAS_MAP_SIZE];
    VUFNTCHAR glyphs[NUQFNT_ATLAS_GLYPH_SIZE];
    PC_LOCKED_RECT_PC locked;
    PC_TEXTURE_RESOURCE_PC *resource;
    NUTEXDIM_PC *texture;
    VUFNT *font;
    u8 *source, *destination;
    i32 row_size, row;
    i32 index, next, c, size;
    u16 glyph;
    i32 texture_id;

    NuStrLenW(text);
    memset(glyphs, 0, sizeof(glyphs));
    memset(map, 0xff, sizeof(map));
    bitmap = NuTexCreateBmpPc(path);
    if (bitmap == NULL) {
        return NULL;
    }
    if (bitmap->type != 5) {
        if (bitmap->type != 3) {
            NuQFntAtlasErrorPc();
        }
    } else {
        for (c = 0; c < 256; c++) {
            if (c == key_colour) {
                bitmap->palette[c * 4 + 3] = 0;
            } else {
                ((u32 *)bitmap->palette)[c] |= 0x7f000000;
            }
        }
    }

    NuQFntScanResetPc();
    index = 1;
    max_count = 0;
    for (; *text != 0; text++) {
        /* Skip the slots that characters with their own fixed slot occupy. */
        if (index == ' ') {
            index = '!';
        }
        if (index == '!') {
            index = '"';
        }
        if (index == '%') {
            index = '&';
        }
        if (index == '?') {
            index = '@';
        }
        if (index == '.') {
            index = '/';
        }
        if (index == ',') {
            index = '-';
        }
        if (index == '0') {
            index = ':';
        }
        if (index == 'A') {
            index = '[';
        }
        if (index == 'a') {
            index = '{';
        }
        if (index == '[') {
            index = '\\';
        }
        if (index == ']') {
            index = '^';
        }
        if (*text == ' ') {
            continue;
        }
        if ((*text >= 'a' && *text <= 'z') || (*text >= 'A' && *text <= 'Z') || (*text >= '0' && *text <= '9') ||
            *text == '!' || *text == '?' || *text == '.' || *text == ',' || *text == '%' || *text == '[' ||
            *text == ']') {
            NUQFNT_ATLAS_NEXT_GLYPH();
            glyphs[*text].x = x;
            glyphs[*text].y = y;
            glyphs[*text].width = width;
            next = NuQFntMapAddPc(map, *text, *text, *text);
        } else {
            NUQFNT_ATLAS_NEXT_GLYPH();
            glyphs[index].x = x;
            glyphs[index].y = y;
            glyphs[index].width = width;
            index = NuQFntMapAddPc(map, index, *text, (u16)index);
            next = index + 1;
        }
        if (max_count <= next) {
            max_count = next;
        }
    }

    glyphs[' '].x = 0.0f;
    glyphs[' '].y = 0.0f;
    glyphs[' '].width = (f32)(cell_height >> 2);
    count = max_count;
    map[' '].unicode = ' ';
    map[' '].index = ' ';

    /* Lower case letters without a cell borrow the upper case glyph. */
    for (c = 'a'; c <= 'z'; c++) {
        if (map[c].unicode == 0xffff) {
            glyph = NuQFntMapLookupPc(map, count, NuToUpper((u8)c));
            if (glyph != 0xffff) {
                NuQFntMapAddPc(map, c, (u16)c, glyph);
                if (max_count <= c) {
                    max_count = c;
                }
            }
        }
    }
    /* Accented lower case letters fall back to their upper case forms. */
    for (c = 0xe0; c <= 0xff; c++) {
        if (NuQFntMapLookupPc(map, index, (u16)c) == 0xffff) {
            glyph = NuQFntMapLookupPc(map, count, NuToUpper((u8)c));
            if (glyph != 0xffff) {
                index = NuQFntMapAddPc(map, index, (u16)c, glyph);
            }
        }
    }
    /* Curly quotes reuse the apostrophe. */
    if (NuQFntMapLookupPc(map, index, 0x91) == 0xffff) {
        glyph = NuQFntMapLookupPc(map, count, '\'');
        if (glyph != 0xffff) {
            index = NuQFntMapAddPc(map, index, 0x91, glyph);
        }
    }
    if (NuQFntMapLookupPc(map, index, 0x92) == 0xffff) {
        glyph = NuQFntMapLookupPc(map, count, '\'');
        if (glyph != 0xffff) {
            index = NuQFntMapAddPc(map, index, 0x92, glyph);
        }
    }
    if (index <= 'z') {
        index = '{';
    }
    qsort(map, index, sizeof(VUCHARIDX), NuQFntMapComparePc);

    texture = (NUTEXDIM_PC *)buffer->void_ptr;
    buffer->addr += sizeof(NUTEXDIM_PC);
    memset(texture, 0, sizeof(*texture));
    texture->width = bitmap->width;
    texture->height = bitmap->height;
    PCDeviceCreateTexture(bitmap->width, bitmap->height, bitmap->mips, 0, 0x15, 1, &texture->resource);
    texture_id = NuTexCreateNative(texture);
    if (texture_id == 0) {
        NuQFntAtlasErrorPc();
    }
    resource = NuTextureGetDimensions(texture_id)->resource;
    if (resource->methods->lock_rect(resource, 0, &locked, NULL, 0) >= 0) {
        source = bitmap->data;
        row_size = bitmap->width * 4;
        destination = (u8 *)locked.bits;
        for (row = 0; row < bitmap->height; row++) {
            memcpy(destination, source, row_size);
            destination += locked.pitch;
            source += row_size;
        }
        resource = NuTextureGetDimensions(texture_id)->resource;
        resource->methods->unlock_rect(resource, 0);
    }
    NuFree(bitmap->data, NuQFntSourcePathPC, 0x264);
    NuFree(bitmap, NuQFntSourcePathPC, 0x265);
    NuTexGetWidth(texture_id);
    NuTexGetHeight(texture_id);

    font = (VUFNT *)((buffer->addr + 15) & ~15U);
    buffer->addr = ((buffer->addr + 15) & ~15U) + sizeof(VUFNT);
    memset(font, 0, sizeof(VUFNT));
    font->glyph_count = count;
    font->height = (f32)cell_height;
    font->unicode_count = index;
    font->legacy_font = NULL;
    font->baseline = (f32)baseline;
    font->ic_gap = 0.0f;
    font->space_width = glyphs[' '].width;

    buffer->addr = (buffer->addr + 15) & ~15U;
    font->glyphs = (VUFNTCHAR *)buffer->void_ptr;
    buffer->addr += font->glyph_count * sizeof(VUFNTCHAR);
    memcpy(font->glyphs, glyphs, font->glyph_count * sizeof(VUFNTCHAR));
    for (size = font->glyph_count * sizeof(VUFNTCHAR); size & 15; size += 4) {
        *(u32 *)buffer->void_ptr = 0;
        buffer->addr += 4;
    }

    buffer->addr = (buffer->addr + 15) & ~15U;
    font->unicode_map = (VUCHARIDX *)buffer->void_ptr;
    buffer->addr += font->unicode_count * sizeof(VUCHARIDX);
    memcpy(font->unicode_map, map, font->unicode_count * sizeof(VUCHARIDX));
    buffer->addr = (buffer->addr + 15) & ~15U;
    font->size = buffer->addr - (u32)font;
    NuQFntAtlasReadPSPc(font, texture_id, flags, plane, buffer, end);
    return font;
}
