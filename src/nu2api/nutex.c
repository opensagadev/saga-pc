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

i32 __cdecl NuTexResolveReference(NUGSCN *scene, i32 tid) {
    i32 index, byte, resolved = 0;
    u32 latest = 0;
    if ((tid & 0x4000) == 0)
        return scene->texture_ids[tid];
    if (NuTextureCapacityPC == 0)
        return 0;
    for (index = 0; index != NuTextureCapacityPC; ++index) {
        NUTEXDIM_PC *texture = NuTextureDimensions[index];
        NUTEXDIM_PC *reference;
        if (texture == NULL || texture->ref_count < 0)
            continue;
        reference = scene->textures[tid & 0x3fff];
        for (byte = 0; byte < 16; ++byte) {
            if (texture->checksum[byte] != reference->checksum[byte])
                break;
        }
        if (byte == 16 && (u32)NuTextureLoadOrderPC[index] > latest) {
            resolved = index + 1;
            latest = NuTextureLoadOrderPC[index];
        }
    }
    if (resolved != 0) {
        if (resolved > 0 && NuTextureDimensions[resolved - 1] != NULL)
            ++NuTextureDimensions[resolved - 1]->ref_count;
        scene->texture_ids[tid & 0x3fff] = resolved;
    }
    return resolved;
}
