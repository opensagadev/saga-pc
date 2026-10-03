#include "saga.h"
#include <string.h>

void __cdecl NuGScnCreatePS(NUGSCN *scene, VARIPTR *cursor, VARIPTR *end) {
    i32 i;
    (void)cursor;
    (void)end;
    for (i = 0; i != scene->texture_count; ++i) {
        scene->textures[i]->resource = SceneVideoResourcesPC.textures[i];
        scene->textures[i]->image_data = NULL;
        scene->textures[i]->image_size = 0;
    }
}

typedef struct SCENE_FILE_TEXTURE_HEADER_PC {
    i32 width;
    u32 height;
    u32 unknown_08;
    u32 unknown_0c;
    u32 flags;
    u32 size;
} SCENE_FILE_TEXTURE_HEADER_PC;
STATIC_ASSERT(sizeof(SCENE_FILE_TEXTURE_HEADER_PC) == 0x18);
STATIC_ASSERT(offsetof(SCENE_FILE_TEXTURE_HEADER_PC, width) == 0);
STATIC_ASSERT(offsetof(SCENE_FILE_TEXTURE_HEADER_PC, height) == 4);
STATIC_ASSERT(offsetof(SCENE_FILE_TEXTURE_HEADER_PC, flags) == 0x10);
STATIC_ASSERT(offsetof(SCENE_FILE_TEXTURE_HEADER_PC, size) == 0x14);

i32 __cdecl NuGScnUploadGfxDataFromFilePS(VARIPTR *cursor, VARIPTR end, NUFILE file) {
    i32 section_size, bytes_read, i;
    u32 size;
    void *data, *locked_data;
    u8 padding;
    i32 skip_texture, second_offset, third_offset, fourth_offset;
    SCENE_FILE_TEXTURE_HEADER_PC texture;
    (void)cursor;
    (void)end;
    if (SceneUploadBufferStartPC.addr == 0) {
        SceneUploadBufferStartPC.void_ptr = NuAlloc(SceneUploadBufferSizePC, SceneUploadSourcePathPC, 0x3f);
        SceneUploadBufferEndPC.addr = SceneUploadBufferStartPC.addr + SceneUploadBufferSizePC;
    }
    SceneUploadBufferCursorPC = SceneUploadBufferStartPC;
    memset(&SceneVideoResourcesPC, 0, sizeof(SceneVideoResourcesPC));
    bytes_read = NuFileRead(file, &section_size, sizeof(section_size));
    bytes_read += NuFileRead(file, &SceneVideoResourcesPC.texture_count, sizeof(SceneVideoResourcesPC.texture_count));
    SceneVideoResourcesPC.textures = (PC_TEXTURE_RESOURCE_PC **)SceneUploadBufferCursorPC.void_ptr;
    SceneUploadBufferCursorPC.addr += SceneVideoResourcesPC.texture_count * sizeof(*SceneVideoResourcesPC.textures);
    memset(SceneVideoResourcesPC.textures, 0,
           SceneVideoResourcesPC.texture_count * sizeof(*SceneVideoResourcesPC.textures));
    second_offset = 8;
    third_offset = 12;
    fourth_offset = 16;
    for (i = 0; i != SceneVideoResourcesPC.texture_count; ++i) {
        bytes_read += NuFileRead(file, &texture, sizeof(texture));
        if (texture.size == 0) {
            SceneVideoResourcesPC.textures[i] = NULL;
            if (texture.width < 0) {
                SceneVideoResourcesPC.textures[i + 1] = NULL;
                SceneVideoResourcesPC.textures[second_offset / sizeof(*SceneVideoResourcesPC.textures)] = NULL;
                SceneVideoResourcesPC.textures[third_offset / sizeof(*SceneVideoResourcesPC.textures)] = NULL;
                SceneVideoResourcesPC.textures[fourth_offset / sizeof(*SceneVideoResourcesPC.textures)] = NULL;
                SceneVideoResourcesPC.textures[i + 5] = NULL;
                i += 5;
                second_offset += 20;
                third_offset += 20;
                fourth_offset += 20;
            }
        } else if (texture.width == 0) {
            NuPtlDmaSlotsExhaustedPc();
            SceneVideoResourcesPC.textures[i] = NULL;
        } else {
            skip_texture = 0;
            if (PCUseHiresFlag == 0 && texture.width > 0 &&
                ((texture.flags & 0xc) != 0 || (texture.flags & 0x3c00) != 0))
                skip_texture = 1;
            data = SceneUploadBufferCursorPC.void_ptr;
            SceneUploadBufferCursorPC.addr += texture.size;
            bytes_read += NuFileRead(file, data, texture.size);
            if (skip_texture == 0) {
                PCFrameCriticalSectionEnter_candidate();
                if (texture.width < 0) {
                    texture.width = -texture.width;
                    PCDeviceCreateCubeTextureFromMemory(data, texture.size, &SceneVideoResourcesPC.textures[i]);
                    SceneVideoResourcesPC.textures[i + 1] = NULL;
                    SceneVideoResourcesPC.textures[second_offset / sizeof(*SceneVideoResourcesPC.textures)] = NULL;
                    SceneVideoResourcesPC.textures[third_offset / sizeof(*SceneVideoResourcesPC.textures)] = NULL;
                    SceneVideoResourcesPC.textures[fourth_offset / sizeof(*SceneVideoResourcesPC.textures)] = NULL;
                    SceneVideoResourcesPC.textures[i + 5] = NULL;
                    i += 5;
                    second_offset += 20;
                    third_offset += 20;
                    fourth_offset += 20;
                } else {
                    PCDeviceCreateTextureFromMemoryEx(data, texture.size, texture.width, texture.height, -3, 0, 0, 1,
                                                      -1, -1, 0, NULL, NULL, &SceneVideoResourcesPC.textures[i]);
                }
                PCFrameCriticalSectionLeave_candidate();
                if (bgProcIsBgThread() != 0 && PCFrameLoadingYieldEnabled() != 0)
                    Sleep(10);
            }
            SceneUploadBufferCursorPC.addr -= texture.size;
        }
        second_offset += 4;
        third_offset += 4;
        fourth_offset += 4;
    }
    bytes_read +=
        NuFileRead(file, &SceneVideoResourcesPC.vertex_buffer_count, sizeof(SceneVideoResourcesPC.vertex_buffer_count));
    SceneVideoResourcesPC.vertex_buffers = (PC_BUFFER_RESOURCE_PC **)SceneUploadBufferCursorPC.void_ptr;
    SceneUploadBufferCursorPC.addr += SceneVideoResourcesPC.vertex_buffer_count * sizeof(PC_BUFFER_RESOURCE_PC *);
    for (i = 0; i != SceneVideoResourcesPC.vertex_buffer_count; ++i) {
        bytes_read += NuFileRead(file, &size, sizeof(size));
        if (size == 0) {
            NuPtlDmaSlotsExhaustedPc();
            SceneVideoResourcesPC.vertex_buffers[i] = NULL;
        } else {
            data = SceneUploadBufferCursorPC.void_ptr;
            SceneUploadBufferCursorPC.addr += size;
            bytes_read += NuFileRead(file, data, size);
            PCFrameCriticalSectionEnter_candidate();
            PCDeviceCreateVertexBuffer(size, 0, 0, 1, &SceneVideoResourcesPC.vertex_buffers[i]);
            SceneVideoResourcesPC.vertex_buffers[i]->methods->lock(SceneVideoResourcesPC.vertex_buffers[i], 0, 0,
                                                                   &locked_data, 0);
            memcpy(locked_data, data, size);
            SceneVideoResourcesPC.vertex_buffers[i]->methods->unlock(SceneVideoResourcesPC.vertex_buffers[i]);
            PCFrameCriticalSectionLeave_candidate();
            SceneUploadBufferCursorPC.addr -= size;
        }
    }
    bytes_read +=
        NuFileRead(file, &SceneVideoResourcesPC.index_buffer_count, sizeof(SceneVideoResourcesPC.index_buffer_count));
    SceneVideoResourcesPC.index_buffers = (PC_BUFFER_RESOURCE_PC **)SceneUploadBufferCursorPC.void_ptr;
    SceneUploadBufferCursorPC.addr += SceneVideoResourcesPC.index_buffer_count * sizeof(PC_BUFFER_RESOURCE_PC *);
    for (i = 0; i != SceneVideoResourcesPC.index_buffer_count; ++i) {
        bytes_read += NuFileRead(file, &size, sizeof(size));
        if (size == 0) {
            NuPtlDmaSlotsExhaustedPc();
            SceneVideoResourcesPC.index_buffers[i] = NULL;
        } else {
            data = SceneUploadBufferCursorPC.void_ptr;
            SceneUploadBufferCursorPC.addr += size;
            bytes_read += NuFileRead(file, data, size);
            PCFrameCriticalSectionEnter_candidate();
            PCDeviceCreateIndexBuffer(size, 0, 0x65, 1, &SceneVideoResourcesPC.index_buffers[i], NULL);
            SceneVideoResourcesPC.index_buffers[i]->methods->lock(SceneVideoResourcesPC.index_buffers[i], 0, 0,
                                                                  &locked_data, 0);
            memcpy(locked_data, data, size);
            SceneVideoResourcesPC.index_buffers[i]->methods->unlock(SceneVideoResourcesPC.index_buffers[i]);
            PCFrameCriticalSectionLeave_candidate();
            SceneUploadBufferCursorPC.addr -= size;
        }
    }
    section_size += 4;
    for (; bytes_read < section_size; ++bytes_read)
        NuFileRead(file, &padding, 1);
    return section_size;
}

typedef struct SCENE_FIXUP_GEOMETRY_PC {
    u8 unknown_00[8];
    u16 vertex_stride;
    u8 unknown_0a[0xe];
    u32 vertex_count;
    u8 unknown_1c[4];
    PC_BUFFER_RESOURCE_PC *index_buffer;
    PC_BUFFER_RESOURCE_PC *vertex_buffer;
    u32 vertex_format;
    u8 unknown_2c[4];
    PC_BUFFER_RESOURCE_PC *dynamic_vertex_buffer;
} SCENE_FIXUP_GEOMETRY_PC;
STATIC_ASSERT(offsetof(SCENE_FIXUP_GEOMETRY_PC, vertex_stride) == 8);
STATIC_ASSERT(offsetof(SCENE_FIXUP_GEOMETRY_PC, vertex_count) == 0x18);
STATIC_ASSERT(offsetof(SCENE_FIXUP_GEOMETRY_PC, index_buffer) == 0x20);
STATIC_ASSERT(offsetof(SCENE_FIXUP_GEOMETRY_PC, vertex_buffer) == 0x24);
STATIC_ASSERT(offsetof(SCENE_FIXUP_GEOMETRY_PC, vertex_format) == 0x28);
STATIC_ASSERT(offsetof(SCENE_FIXUP_GEOMETRY_PC, dynamic_vertex_buffer) == 0x30);
typedef struct SCENE_FIXUP_VERTEX_STREAM_PC {
    u8 unknown_00[8];
    PC_BUFFER_RESOURCE_PC *vertex_buffer;
} SCENE_FIXUP_VERTEX_STREAM_PC;
STATIC_ASSERT(offsetof(SCENE_FIXUP_VERTEX_STREAM_PC, vertex_buffer) == 8);
typedef struct SCENE_FIXUP_NATIVE_PC {
    u16 vertex_buffer_count;
    u8 unknown_02[2];
    PC_BUFFER_RESOURCE_PC **vertex_buffers;
    u16 index_buffer_count;
    u8 unknown_0a[2];
    PC_BUFFER_RESOURCE_PC **index_buffers;
    SCENE_FIXUP_GEOMETRY_PC **geometries;
    u32 geometry_count;
    SCENE_FIXUP_VERTEX_STREAM_PC **vertex_streams;
    u32 vertex_stream_count;
} SCENE_FIXUP_NATIVE_PC;
STATIC_ASSERT(offsetof(SCENE_FIXUP_NATIVE_PC, vertex_buffers) == 4);
STATIC_ASSERT(offsetof(SCENE_FIXUP_NATIVE_PC, index_buffer_count) == 8);
STATIC_ASSERT(offsetof(SCENE_FIXUP_NATIVE_PC, index_buffers) == 0xc);
STATIC_ASSERT(offsetof(SCENE_FIXUP_NATIVE_PC, geometries) == 0x10);
STATIC_ASSERT(offsetof(SCENE_FIXUP_NATIVE_PC, geometry_count) == 0x14);
STATIC_ASSERT(offsetof(SCENE_FIXUP_NATIVE_PC, vertex_streams) == 0x18);
STATIC_ASSERT(offsetof(SCENE_FIXUP_NATIVE_PC, vertex_stream_count) == 0x1c);
typedef struct SCENE_FIXUP_PORTAL_FIELDS_PC {
    u8 unknown_00[0x6c];
    i32 max_portals;
} SCENE_FIXUP_PORTAL_FIELDS_PC;
STATIC_ASSERT(offsetof(SCENE_FIXUP_PORTAL_FIELDS_PC, max_portals) == 0x6c);
void __cdecl NuMtlUpdate(NUMTL *);
void __cdecl NuPortalMaxDepth(NUGSCN *, i32);

void __cdecl NuGScnFixupPS(NUGSCN *scene) {
    SCENE_FIXUP_NATIVE_PC *native = scene->native_scene;
    SCENE_FIXUP_GEOMETRY_PC *geometry;
    SCENE_FIXUP_VERTEX_STREAM_PC *stream;
    u32 i;
    u8 dynamic = 0;
    i32 material_index, registry_index;
    for (i = 0; i != native->geometry_count; ++i) {
        geometry = native->geometries[i];
        geometry->index_buffer = SceneVideoResourcesPC.index_buffers[(u32)geometry->index_buffer];
        geometry->vertex_buffer = SceneVideoResourcesPC.vertex_buffers[(u32)geometry->vertex_buffer];
        if (geometry->vertex_format != 0) {
            dynamic = 1;
            PCFrameCriticalSectionEnter_candidate();
            PCDeviceCreateVertexBuffer(geometry->vertex_stride * geometry->vertex_count, 0x208, 0, 0,
                                       &geometry->dynamic_vertex_buffer);
            PCFrameCriticalSectionLeave_candidate();
        }
    }
    if (dynamic != 0) {
        for (registry_index = 0; registry_index < 100; ++registry_index) {
            if (PCRegisteredNativeScenes[registry_index] == NULL) {
                PCRegisteredNativeScenes[registry_index] = scene;
                break;
            }
        }
    }
    for (i = 0; i != native->vertex_stream_count; ++i) {
        stream = native->vertex_streams[i];
        stream->vertex_buffer = SceneVideoResourcesPC.vertex_buffers[(u32)stream->vertex_buffer];
    }
    native->vertex_buffer_count = SceneVideoResourcesPC.vertex_buffer_count;
    native->index_buffer_count = SceneVideoResourcesPC.index_buffer_count;
    memcpy(native->vertex_buffers, SceneVideoResourcesPC.vertex_buffers,
           SceneVideoResourcesPC.vertex_buffer_count * sizeof(*native->vertex_buffers));
    memcpy(native->index_buffers, SceneVideoResourcesPC.index_buffers,
           SceneVideoResourcesPC.index_buffer_count * sizeof(*native->index_buffers));
    for (material_index = 0; material_index < scene->material_count; ++material_index)
        NuMtlUpdate(scene->materials[material_index]);
    NuPortalMaxDepth(scene, ((SCENE_FIXUP_PORTAL_FIELDS_PC *)scene)->max_portals);
}
