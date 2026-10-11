#include "saga.h"

void __cdecl NuGobjReleaseChildPc(PC_GOBJ_REGISTRY_CHILD_PC *child) {
    PC_GOBJ_REGISTRY_AUX_PC *aux = child->auxiliary;
    if (child->owned_buffer != NULL) {
        NuFree(child->owned_buffer, NuGobjSourcePathPc, 605);
        child->owned_buffer = NULL;
    }
    while (aux != NULL) {
        PC_GOBJ_REGISTRY_AUX_PC *next = aux->next;
        if (aux->payload != NULL)
            aux->payload = NULL;
        aux = next;
    }
    if (child->native_resource != NULL && child->native_resource->resource != NULL)
        PCDeviceReleaseTexture(&child->native_resource->resource);
}
void __cdecl NuGobjRegistryCleanupPc(void) {
    if (NuGobjRegistryInitializedPc != 0) {
        PC_GOBJ_REGISTRY_ENTRY_PC *entry = NuGobjRegistryFirstPc;
        while (entry != NULL) {
            PC_GOBJ_REGISTRY_ENTRY_PC *next = entry->next;
            PC_GOBJ_REGISTRY_CHILD_PC *child = entry->children;
            PC_GOBJ_REGISTRY_AUX_PC *aux = entry->auxiliary;
            while (child != NULL) {
                PC_GOBJ_REGISTRY_CHILD_PC *next_child = child->next;
                NuGobjReleaseChildPc(child);
                child = next_child;
            }
            while (aux != NULL)
                aux = aux->next;
            if (entry->previous != NULL)
                entry->previous->next = entry->next;
            if (entry->next != NULL)
                entry->next->previous = entry->previous;
            --NuGobjRegistryCountPc;
            entry = next;
            NuGobjRegistryFirstPc = entry;
        }
    }
}
void __cdecl NuGobjRegistryInitPc(void) {
    if (NuGobjRegistryInitializedPc != 0)
        NuGobjRegistryCleanupPc();
    NuGobjRegistryInitializedPc = 1;
    NuGobjRegistryFirstPc = NULL;
}
