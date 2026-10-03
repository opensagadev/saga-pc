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
