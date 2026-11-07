// PC platform unit linked after nu3d (retail 0x5f7490-0x60df00), being rebuilt one module at a time.
// Retail compiled the per-module PS/Pc backends (nugscn_pc, nuportal_pc, numtl PS, the nudlist and
// nupostfilter platform halves, ...) as one C++ unit; nu3d only ever calls into it, never inlines from it.
// Definitions follow retail address order.

#include "saga.h"

#include <string.h>

extern "C" {

    // @@PREAMBLE
    struct POST_SHADER_PROGRAM_PC;
    POST_SHADER_PROGRAM_PC *__cdecl PostShaderProgramCreatePc(i32, const DWORD *, const DWORD *);
    extern const DWORD PostShaderTokens7662d8[168];
    extern const DWORD PostShaderTokens766578[94];
    extern const DWORD PostShaderTokens7666f0[75];
    extern const DWORD PostShaderTokens766820[261];
    extern const DWORD PostShaderTokens766c38[168];
    extern const DWORD PostShaderTokens766ed8[94];
    extern const DWORD PostShaderTokens767050[75];
    extern const DWORD PostShaderTokens767180[261];
    extern const DWORD PostShaderTokens767598[85];
    extern const DWORD PostShaderTokens7676f0[566];
    extern const DWORD PostShaderTokens767fc8[554];
    extern const DWORD PostShaderTokens768870[155];
    extern const DWORD PostShaderTokens768ae0[143];
    extern const DWORD PostShaderTokens768d20[48];
    extern const DWORD PostShaderTokens768de0[180];
    extern const DWORD PostShaderTokens7690b0[131];
    extern const DWORD PostShaderTokens7692c0[175];
    extern const DWORD PostShaderTokens769580[100];
    PC_FRAMEBUFFER_PC *__cdecl PCGraphicsCreateFramebuffer(void);
    void __cdecl NuPostEffectReleasePortsPc(void);
    extern GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsPostEffectBackBufferTexturePc;

    typedef struct SCENE_TEXTURE_PACKET_B0_PC {
        i32 kind;
        i32 texture_id;
        i32 extra_texture_ids[3];
    } SCENE_TEXTURE_PACKET_B0_PC;
    typedef struct SCENE_TEXTURE_PACKET_AE_PC {
        i32 texture_ids[3];
    } SCENE_TEXTURE_PACKET_AE_PC;
    STATIC_ASSERT(sizeof(SCENE_TEXTURE_PACKET_B0_PC) == 20);
    STATIC_ASSERT(offsetof(SCENE_TEXTURE_PACKET_B0_PC, kind) == 0);
    STATIC_ASSERT(offsetof(SCENE_TEXTURE_PACKET_B0_PC, texture_id) == 4);
    STATIC_ASSERT(offsetof(SCENE_TEXTURE_PACKET_B0_PC, extra_texture_ids) == 8);
    STATIC_ASSERT(sizeof(SCENE_TEXTURE_PACKET_AE_PC) == 12);
    STATIC_ASSERT(offsetof(SCENE_TEXTURE_PACKET_AE_PC, texture_ids) == 0);
    STATIC_ASSERT(offsetof(NUDISPLAYLISTITEM, type) == 0);
    STATIC_ASSERT(offsetof(NUDISPLAYLISTITEM, next) == 4);
    STATIC_ASSERT(sizeof(NUDISPLAYLISTITEM) == 16);

    /* nuportal PC backend (0x600bd0-0x600f20). */

    typedef struct PORTAL_BOX_PC {
        NUVEC first;
        f32 first_w;
        NUVEC second;
        f32 second_w;
    } PORTAL_BOX_PC;
    STATIC_ASSERT(sizeof(PORTAL_BOX_PC) == 0x20);
    STATIC_ASSERT(offsetof(PORTAL_BOX_PC, second) == 0x10);
    STATIC_ASSERT(offsetof(NUGSCN, num_instances) == 0x1c);
    STATIC_ASSERT(offsetof(NUGSCN, sphere_count) == 0xf0);
    STATIC_ASSERT(offsetof(NUGSCN, portal_instance_count) == 0xf4);
    STATIC_ASSERT(offsetof(NUGSCN, portal_spheres) == 0xf8);
    STATIC_ASSERT(offsetof(NUGSCN, portal_boxes) == 0xfc);
    STATIC_ASSERT(offsetof(NUGSCN, instance_visibility_flags) == 0x13c);
    STATIC_ASSERT(offsetof(NUINSTANCE, flags) == 0x44);
    STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, visibility_flags) == 0x48);
    STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, render_flags) == 0x74);
    STATIC_ASSERT(offsetof(NUDISPLAY_LIST_PC, portal_visibility_overrides) == 0x88);

    /* numtl PS half (0x601210). */

    typedef struct FRAME_MTL_ANIM_DESC_PC {
        u8 u_mode, v_mode, reserved02[2];
        f32 u_amplitude, v_amplitude, u_frequency, v_frequency;
    } FRAME_MTL_ANIM_DESC_PC;
    STATIC_ASSERT(sizeof(FRAME_MTL_ANIM_DESC_PC) == 0x14);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_frequency) == 0xc);
    typedef struct FRAME_MTL_ANIM_PC {
        u8 reserved00[0x7b];
        u8 disable_u_animation, disable_v_animation;
        u8 reserved7d[0x143];
        i32 texture_animation_data[4];
        f32 texture_animation_offsets[4][2];
        u8 reserved1f0[8];
        FRAME_MTL_ANIM_DESC_PC animation[4];
    } FRAME_MTL_ANIM_PC;
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, disable_u_animation) == 0x7b);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, disable_v_animation) == 0x7c);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, texture_animation_data) == 0x1c0);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, texture_animation_offsets) == 0x1d0);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, animation) == 0x1f8);
    typedef struct FRAME_MTL_ANIM_SET_PC {
        NUDISPLAY_LIST_PC *scene;
        i32 material_count;
        i32 *material_indices;
        struct FRAME_MTL_ANIM_SET_PC *next;
    } FRAME_MTL_ANIM_SET_PC;
    STATIC_ASSERT(sizeof(FRAME_MTL_ANIM_SET_PC) == 0x10);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, next) == 0xc);
    typedef struct FRAME_MTL_SCENE_PC {
        u8 reserved00[0x158];
        i32 shader_texture_animation_enabled;
    } FRAME_MTL_SCENE_PC;
    STATIC_ASSERT(offsetof(FRAME_MTL_SCENE_PC, shader_texture_animation_enabled) == 0x158);

    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_mode) == 0x0);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_mode) == 0x1);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_amplitude) == 0x4);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_amplitude) == 0x8);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_frequency) == 0x10);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, scene) == 0x0);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, material_count) == 0x4);
    STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, material_indices) == 0x8);
    // @@FUNCTIONS

    /* The PC backend's transform-update leaf is a single ret instruction; retail
     * reaches it through a jump thunk, so callers in other units never inline it. */
    void __cdecl DisplayListUpdateSpecialTransformPS(NUSPECIALHANDLE *special, NUMTX *matrix) {
        (void)special;
        (void)matrix;
    }

    void __cdecl PCGraphicsDestroyPostEffectTexturesPc(void) {
        NuPostEffectReleasePortsPc();
        PCGraphicsDestroyRenderTexture(PCGraphicsPostEffectBackBufferTexturePc);
    }

    void NuDeferredFilter::initTextureResources(i32 width, i32 height) {
        NuDeferredFilterGen::initTextureResources(width, height);
    }

    void NuMotionAccumFilter::initResources() {
        NuMotionAccumFilterGen::initResources();
        program = NULL;
    }

    void NuSpeedBlurFilter::initTextureResources(i32, i32) {
        texture_surface.Attach(PCGraphicsPostEffectBackBufferTexturePc, 0, false);
    }

    void NuSpeedBlurFilter::destroyTextureResources() {
        texture_surface.Reset();
    }

    i32 __cdecl NuSpecialDrawAtAlpha(NUSPECIALHANDLE *handle, NUMTX *matrix, f32 alpha) {
        i32 result;
        if (handle->scene == NULL || alpha <= 0.0f)
            return 0;
        if (alpha < 1.0f) {
            NuSpecialConstAlpha(1, alpha);
            result = NuDisplayListRndrSpecial(handle, matrix, 0, NULL, NULL);
            NuSpecialConstAlpha(0, 0.0f);
            return result;
        }
        return NuDisplayListRndrSpecial(handle, matrix, 0, NULL, NULL);
    }

    void __cdecl PortalInitialisePsPc(NUGSCN *scene) {
        scene->instance_visibility_flags = NuPortalVisibilityBitsPc;
    }

    void __cdecl PortalFlagRoomVisiblePc(NUGSCNROOM *room, NUGSCN *scene) {
        i32 i;
        if (scene->display_list != NULL) {
            for (i = 0; i < room->instance_count; ++i) {
                i32 index = room->instance_indices[i];
                NuPortalVisibilityBitsPc[index >> 3] |= (u8)(1 << (index & 7));
            }
        } else {
            for (i = 0; i < room->instance_count; ++i)
                scene->instances[room->instance_indices[i]].flags |= 4;
        }
    }

    void __cdecl PortalSetAllVisiblePc(NUGSCN *scene) {
        i32 i;
        if (scene->display_list != NULL) {
            memset(NuPortalVisibilityBitsPc, 255, 625);
        } else {
            for (i = 0; i < scene->num_instances; ++i)
                scene->instances[i].flags |= 4;
        }
    }

    void __cdecl PortalSetAllHiddenPc(NUGSCN *scene) {
        i32 i;
        if (scene->display_list != NULL) {
            memset(NuPortalVisibilityBitsPc, 0, 625);
        } else {
            for (i = 0; i < scene->num_instances; ++i)
                scene->instances[i].flags &= ~4U;
        }
    }

    void __cdecl PortalClipRoomPc(NUGSCN *scene, NUGSCNROOM *room, NUPORTAL_FRUSTUM_PC *frustum) {
        NUDISPLAY_LIST_PC *display = scene->display_list;
        u32 i;
        if (display != NULL && (display->render_flags & 0x4000) != 0) {
            for (i = 0; i < (u32)(i32)room->instance_count; ++i) {
                u32 index = (u32)(i32)room->instance_indices[i];
                u32 byte_index = index >> 3;
                u32 bit = index & 7;
                if (((NuPortalVisibilityBitsPc[byte_index] >> bit) & 1) != 0)
                    continue;
                display = scene->display_list;
                if (((display->portal_visibility_overrides[byte_index] >> bit) & 1) != 0) {
                    NuPortalVisibilityBitsPc[byte_index] |= (u8)(1 << bit);
                } else {
                    u8 flags = display->visibility_flags[index];
                    if ((flags & 1) != 0) {
                        if ((flags & 8) != 0)
                            NuPortalVisibilityBitsPc[byte_index] |= (u8)(1 << bit);
                        else if (NuPortalClipTestBox(&((PORTAL_BOX_PC *)scene->portal_boxes)[index].first,
                                                     &((PORTAL_BOX_PC *)scene->portal_boxes)[index].second,
                                                     frustum) != 0)
                            NuPortalVisibilityBitsPc[byte_index] |= (u8)(1 << bit);
                    }
                }
            }
        } else {
            for (i = 0; i < (u32)(i32)room->instance_count; ++i) {
                u32 index = (u32)(i32)room->instance_indices[i];
                u32 byte_index = index >> 3;
                u32 bit = index & 7;
                u8 bits = NuPortalVisibilityBitsPc[byte_index];
                if (((bits >> bit) & 1) != 0)
                    continue;
                /* Native dereferences a null display only for an unseen instance. */
                display = scene->display_list;
                if (((display->portal_visibility_overrides[byte_index] >> bit) & 1) != 0) {
                    NuPortalVisibilityBitsPc[byte_index] = bits | (u8)(1 << bit);
                    continue;
                }
                if ((display->visibility_flags[index] & 8) != 0) {
                    NuPortalVisibilityBitsPc[byte_index] = bits | (u8)(1 << bit);
                    continue;
                }
                switch (clipTestSphere(&scene->portal_spheres[index], frustum)) {
                    case 1:
                        NuPortalVisibilityBitsPc[byte_index] |= (u8)(1 << bit);
                        break;
                    case 2:
                        if (clipTestBox(&((PORTAL_BOX_PC *)scene->portal_boxes)[index].first,
                                        &((PORTAL_BOX_PC *)scene->portal_boxes)[index].second, frustum->planes,
                                        frustum->plane_count) != 0)
                            NuPortalVisibilityBitsPc[byte_index] |= (u8)(1 << bit);
                        break;
                }
            }
        }
    }

    void NuMtlSetUVOffsetPS(NUMTL *mtl, u32 layer, f32 u, f32 v) {
        FRAME_MTL_ANIM_PC *view = (FRAME_MTL_ANIM_PC *)mtl;
        view->texture_animation_offsets[layer][0] = u;
        view->texture_animation_offsets[layer][1] = v;
    }

    void __cdecl NuGScnFixupTIDsPS(NUGSCN *scene) {
        i32 i, j;
        if (scene->display_list == NULL)
            return;
        for (i = 0; i < scene->display_list->item_count; ++i) {
            NUDISPLAYLISTITEM *item = &((NUDISPLAYLISTITEM *)scene->display_list->items)[i];
            if (item->type == 0xb0) {
                SCENE_TEXTURE_PACKET_B0_PC *packet = (SCENE_TEXTURE_PACKET_B0_PC *)item->next;
                if (packet->kind == 2) {
                    for (j = 0; j < 3; ++j)
                        packet->extra_texture_ids[j] = NuGScnFixupTID(scene, packet->extra_texture_ids[j]);
                }
                packet->texture_id = NuGScnFixupTID(scene, packet->texture_id);
            } else if (item->type == 0xae || item->type == 0xaf) {
                SCENE_TEXTURE_PACKET_AE_PC *packet = (SCENE_TEXTURE_PACKET_AE_PC *)item->next;
                if (packet != NULL) {
                    for (j = 0; j < 3; ++j)
                        packet->texture_ids[j] = NuGScnFixupTID(scene, packet->texture_ids[j]);
                }
            }
        }
    }

    void __cdecl NuGScnRestoreTIDsPS(NUGSCN *scene) {
        i32 i, j;
        if (scene->display_list == NULL)
            return;
        for (i = 0; i < scene->display_list->item_count; ++i) {
            NUDISPLAYLISTITEM *item = &((NUDISPLAYLISTITEM *)scene->display_list->items)[i];
            if (item->type == 0xb0) {
                SCENE_TEXTURE_PACKET_B0_PC *packet = (SCENE_TEXTURE_PACKET_B0_PC *)item->next;
                if (packet->kind == 2) {
                    for (j = 0; j < 3; ++j)
                        packet->extra_texture_ids[j] = NuGScnRestoreTID(scene, packet->extra_texture_ids[j]);
                }
                packet->texture_id = NuGScnRestoreTID(scene, packet->texture_id);
            } else if (item->type == 0xae || item->type == 0xaf) {
                SCENE_TEXTURE_PACKET_AE_PC *packet = (SCENE_TEXTURE_PACKET_AE_PC *)item->next;
                if (packet != NULL) {
                    for (j = 0; j < 3; ++j)
                        packet->texture_ids[j] = NuGScnRestoreTID(scene, packet->texture_ids[j]);
                }
            }
        }
    }

    void __cdecl NuGScnDestroyPS(NUGSCN *scene) {
        PC_GSCN_NATIVE_VIEW *native = (PC_GSCN_NATIVE_VIEW *)scene->native_scene;
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

    void __cdecl NuDisplayListDrawItemsPc(NUDISPLAYLISTITEM *items) {
        NuDisplayListExecute(items, PCDisplayItemHandlersPc);
    }

    i32 __cdecl DisplayListDebugPS(NUDISPLAYLISTITEM *item, char *text) {
        (void)item;
        (void)text;
        return 0;
    }

    void __cdecl NuDisplayListAddClut(NUDISPLAYLISTITEM *item, i32 texture_id) {
        (void)texture_id;
        item->type = 0x87;
        item->id = 0;
        item->next = NULL;
    }

    void __cdecl NuDisplayListAddTexture(NUDISPLAYLISTITEM *item, i32 texture_id) {
        (void)texture_id;
        item->type = 0x87;
        item->id = 0;
        item->next = NULL;
    }

    void __cdecl NuDisplayListAddMaterialState(NUDISPLAYLISTITEM *item, NUMTL *material) {
        item->type = 0x80;
        item->id = 3;
        item->next = material;
    }

    void __cdecl NuDisplayListAddMicrocode(NUDISPLAYLISTITEM *item, NUMTL *material) {
        (void)material;
        item->type = 0x87;
        item->id = 0;
        item->next = NULL;
    }

    void __cdecl NuDisplayListAddLightState(NUDISPLAYLISTITEM *item, NUMTL *material) {
        (void)material;
        item->type = 0x87;
        item->id = 0;
        item->next = NULL;
    }

    void __cdecl DisplayListPrintItemPS(NUDISPLAYLISTITEM *item, i32 file_handle) {
        (void)item;
        (void)file_handle;
    }

    void __cdecl DisplayListSwapBuffersPS(void) {
    }

    void __cdecl NuDisplaySceneAddPS(NUDISPLAY_LIST_PC *scene) {
        i32 i;
        for (i = 0; i < scene->item_count; ++i) {
            NUDISPLAYLISTITEM *item = &((NUDISPLAYLISTITEM *)scene->items)[i];
            if (item->type == 0x82)
                item->type = 0x82;
        }
    }

    void __cdecl NuDisplaySceneDestroyPS(NUDISPLAY_LIST_PC *scene) {
        (void)scene;
    }

    void __cdecl DisplayListSetAlphaPS(NUDISPLAYLISTITEM *item, NUDISPLAYLISTITEM *geometry, f32 alpha) {
        NUMTX *matrix = (NUMTX *)item->next;
        (void)geometry;
        if (alpha < 0.0f)
            alpha = 0.0f;
        if (alpha > 1.0f)
            alpha = 1.0f;
        matrix->m33 = alpha;
    }

    void NuMainFilter::render() {
        if (bloom_enabled)
            RenderBloomPc();
    }

    void NuMainFilter::initResources() {
        NuMainFilterGen::initResources();
        void *last_program;
        if (PCDeviceHardwareVertexProcessingPC > 1) {
            programs[6] = PostShaderProgramCreatePc(2, PostShaderTokens7662d8, PostShaderTokens766578);
            programs[8] = PostShaderProgramCreatePc(2, PostShaderTokens766820, PostShaderTokens766578);
            platform_programs[0] = PostShaderProgramCreatePc(2, PostShaderTokens7662d8, PostShaderTokens7666f0);
            last_program = PostShaderProgramCreatePc(2, PostShaderTokens766820, PostShaderTokens7666f0);
            platform_uniforms[0] = 0x82;
            platform_uniforms[1] = 0x88;
            platform_uniforms[2] = 0x89;
            platform_uniforms[3] = 0x8a;
        } else {
            programs[6] = PostShaderProgramCreatePc(2, PostShaderTokens766c38, PostShaderTokens766ed8);
            programs[8] = PostShaderProgramCreatePc(2, PostShaderTokens767180, PostShaderTokens766ed8);
            platform_programs[0] = PostShaderProgramCreatePc(2, PostShaderTokens766c38, PostShaderTokens767050);
            last_program = PostShaderProgramCreatePc(2, PostShaderTokens767180, PostShaderTokens767050);
            platform_uniforms[0] = 0;
            platform_uniforms[1] = 1;
            platform_uniforms[2] = 2;
            platform_uniforms[3] = 3;
        }
        platform_programs[1] = last_program;
        platform_fbo = PCGraphicsCreateFramebuffer();
    }

    void NuDeferredFilter::initResources() {
        NuDeferredFilterGen::initResources();
        programs[0] = PostShaderProgramCreatePc(2, PostShaderTokens767598, PostShaderTokens7676f0);
        programs[1] = PostShaderProgramCreatePc(2, PostShaderTokens767598, PostShaderTokens767fc8);
        programs[2] = PostShaderProgramCreatePc(2, PostShaderTokens768d20, PostShaderTokens768870);
        programs[3] = PostShaderProgramCreatePc(2, PostShaderTokens768d20, PostShaderTokens768ae0);
    }

    void NuSpeedBlurFilter::initResources() {
        NuPostFilterGen::initResources();
        void *program;
        if (PCDeviceHardwareVertexProcessingPC > 1) {
            program = PostShaderProgramCreatePc(2, PostShaderTokens768de0, PostShaderTokens7690b0);
            platform_uniform = 0x80;
        } else {
            program = PostShaderProgramCreatePc(2, PostShaderTokens7692c0, PostShaderTokens769580);
            platform_uniform = 0;
        }
        programs[0] = program;
    }
    // @@END

} // extern "C"
