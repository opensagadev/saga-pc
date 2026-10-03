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
