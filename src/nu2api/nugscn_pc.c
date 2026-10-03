#include "saga.h"

void __cdecl NuGScnDestroyPS(NUGSCN *scene) {
    PC_GSCN_NATIVE_VIEW *native = scene->native_scene;
    PC_SCENE_MTL_VIEW *material;
    i32 i, j;
    u8 released = 0;
    for (i = 0; i != native->vertex_buffer_count; ++i) {
        PCFrameCriticalSectionEnter_candidate();
        PCDeviceReleaseTexture(&native->vertex_buffers[i]);
        PCFrameCriticalSectionLeave_candidate();
    }
    for (i = 0; i != native->index_buffer_count; ++i) {
        PCFrameCriticalSectionEnter_candidate();
        PCDeviceReleaseTexture(&native->index_buffers[i]);
        PCFrameCriticalSectionLeave_candidate();
    }
    for (i = 0; i != scene->texture_count; ++i) {
        PCFrameCriticalSectionEnter_candidate();
        PCDeviceReleaseTexture(&scene->textures[i]->resource);
        PCFrameCriticalSectionLeave_candidate();
    }
    for (i = 0; i != native->geometry_count; ++i) {
        PC_GSCN_GEOMETRY_VIEW *geometry = native->geometries[i];
        if (geometry->vertex_format != 0 && (geometry->vertex_format & 0xf000) == 0) {
            PC_TEXTURE_RESOURCE_PC *resource;
            released = 1;
            PCFrameCriticalSectionEnter_candidate();
            resource = geometry->resource;
            resource->methods->release(resource);
            PCFrameCriticalSectionLeave_candidate();
        }
    }
    if (released) {
        for (i = 0; i < 100; ++i) {
            if (PCRegisteredNativeScenes[i] == scene) {
                PCRegisteredNativeScenes[i] = NULL;
                break;
            }
        }
    }
    material = (PC_SCENE_MTL_VIEW *)scene->materials[0];
    for (i = 0; i < scene->material_count; ++i, ++material) {
        for (j = 0; j < 2; ++j) {
            if (material->shader_handles[j] > 0)
                --PCShaderRecordFromHandle(material->shader_handles[j])->ref_count;
        }
    }
}
