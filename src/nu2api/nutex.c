#include "saga.h"

void __cdecl NuTexDestroy(i32 texture_id) {
    NUTEXDIM_PC *texture;

    if (texture_id <= 0) {
        return;
    }
    texture = NuTextureDimensions[texture_id - 1];
    if (texture != NULL && texture->ref_count > 0) {
        if (texture != NULL) {
            --texture->ref_count;
        }
        return;
    }
    NuTexDestroyPS(texture);
    NuTextureDimensions[texture_id - 1] = NULL;
}

i32 __cdecl NuTexCreateNative(NUTEXDIM_PC *texture) {
    i32 index;
    if (texture == NULL)
        return 0;
    for (index = 0; index < NuTextureCapacityPC; ++index) {
        if (NuTextureDimensions[index] == NULL) {
            NuTextureDimensions[index] = texture;
            NuTextureLoadOrderPC[index] = NuTextureLoadCountPC++;
            NuTexCreatePS(texture);
            return index + 1;
        }
    }
    AnimationDiagnosticNoOpPc();
    return 0;
}

NUTEXDIM_PC *__cdecl NuTextureGetDimensions(i32 id) {
    if (id <= 0)
        return NULL;
    return NuTextureDimensions[id - 1];
}
