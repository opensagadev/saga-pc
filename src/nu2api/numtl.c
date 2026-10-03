#include "saga.h"

typedef struct material_cleanup_flags_pc_s {
    u16 flags;
} MATERIAL_CLEANUP_FLAGS_PC;
STATIC_ASSERT(offsetof(MATERIAL_CLEANUP_FLAGS_PC, flags) == 0);

typedef struct material_override_cleanup_pc_s {
    NUMTL *material;
    NUMTL *original;
} MATERIAL_OVERRIDE_CLEANUP_PC;
STATIC_ASSERT(offsetof(MATERIAL_OVERRIDE_CLEANUP_PC, material) == 0);
STATIC_ASSERT(offsetof(MATERIAL_OVERRIDE_CLEANUP_PC, original) == 4);

void __cdecl NuMtlDestroy(NUMTL *material) {
    NULNKHDR *node;
    ((MATERIAL_CLEANUP_FLAGS_PC *)material)->flags &= ~1U;
    NuDisplayListDestroyMtl(material);
    node = NuLstGetNext(g_overrideList, NULL);
    while (node != NULL) {
        MATERIAL_OVERRIDE_CLEANUP_PC *entry = (MATERIAL_OVERRIDE_CLEANUP_PC *)node;
        node = NuLstGetNext(g_overrideList, node);
        if (entry->original == material) {
            ((MATERIAL_CLEANUP_FLAGS_PC *)entry->material)->flags &= ~1U;
            NuDisplayListDestroyMtl(entry->material);
            NuLstFree((NULNKHDR *)entry);
        }
    }
}

void __cdecl NuMtlUpdate(NUMTL *material) {
    NuMtlUpdatePS(material);
    ++((PC_SCENE_MTL_VIEW *)material)->version;
}
