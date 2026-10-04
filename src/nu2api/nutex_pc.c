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
