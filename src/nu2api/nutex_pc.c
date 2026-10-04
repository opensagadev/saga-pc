#include "saga.h"

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
