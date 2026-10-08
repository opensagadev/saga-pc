#include "saga.h"

extern "C" void __cdecl NuStrFixExt(char *, char *, char *, i32);

void __cdecl NuTexDestroyPS(NUTEXDIM_PC *texture) {
    if (texture != NULL) {
        PCFrameCriticalSectionEnter_candidate();
        PCDeviceReleaseTexture(&texture->resource);
        PCFrameCriticalSectionLeave_candidate();
    }
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
