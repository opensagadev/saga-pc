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
