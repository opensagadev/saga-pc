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

void __cdecl DisplayListSetAlphaPS(NUDISPLAYLISTITEM *item, NUDISPLAYLISTITEM *geometry, f32 alpha) {
    NUMTX *matrix = (NUMTX *)item->next;
    (void)geometry;
    if (alpha < 0.0f)
        alpha = 0.0f;
    if (alpha > 1.0f)
        alpha = 1.0f;
    matrix->m33 = alpha;
}

void __cdecl DisplayListSwapBuffersPS(void) {
}

typedef struct SCENE_INSTANCE_ANIMATION_PC {
    NUMTX matrix;
    f32 tfactor, first_frame, interval, time;
    u32 flags;
    f32 parameter, previous_frame;
    u16 animation_index, instance_index;
} SCENE_INSTANCE_ANIMATION_PC;
STATIC_ASSERT(sizeof(SCENE_INSTANCE_ANIMATION_PC) == 0x60);
STATIC_ASSERT(offsetof(SCENE_INSTANCE_ANIMATION_PC, tfactor) == 0x40);
STATIC_ASSERT(offsetof(SCENE_INSTANCE_ANIMATION_PC, first_frame) == 0x44);
STATIC_ASSERT(offsetof(SCENE_INSTANCE_ANIMATION_PC, interval) == 0x48);
STATIC_ASSERT(offsetof(SCENE_INSTANCE_ANIMATION_PC, time) == 0x4c);
STATIC_ASSERT(offsetof(SCENE_INSTANCE_ANIMATION_PC, flags) == 0x50);
STATIC_ASSERT(offsetof(SCENE_INSTANCE_ANIMATION_PC, previous_frame) == 0x58);
STATIC_ASSERT(offsetof(SCENE_INSTANCE_ANIMATION_PC, animation_index) == 0x5c);
STATIC_ASSERT(offsetof(SCENE_INSTANCE_ANIMATION_PC, instance_index) == 0x5e);
typedef struct SCENE_ANIMATION_LOOKUP_PC {
    u16 count, end_frame;
    f32 *times;
    u8 *values;
} SCENE_ANIMATION_LOOKUP_PC;
STATIC_ASSERT(sizeof(SCENE_ANIMATION_LOOKUP_PC) == 12);
STATIC_ASSERT(offsetof(SCENE_ANIMATION_LOOKUP_PC, end_frame) == 2);
STATIC_ASSERT(offsetof(SCENE_ANIMATION_LOOKUP_PC, times) == 4);
STATIC_ASSERT(offsetof(SCENE_ANIMATION_LOOKUP_PC, values) == 8);
typedef struct SCENE_ANIMATION_LOOKUP_ALLOCATION_PC {
    i32 references;
    SCENE_ANIMATION_LOOKUP_PC first;
} SCENE_ANIMATION_LOOKUP_ALLOCATION_PC;
STATIC_ASSERT(offsetof(SCENE_ANIMATION_LOOKUP_ALLOCATION_PC, first) == 4);
typedef struct SCENE_UPDATE_READER_PC {
    u8 unknown_00[0x48];
    i16 instance_count, animation_count;
    SCENE_INSTANCE_ANIMATION_PC *instances;
    NUMTX *matrices;
    ANI3_HEADER_PC **animations;
    u8 unknown_58[0xb8];
    NUDISPLAY_LIST_PC *display_list;
    u8 unknown_114[0xcc];
    SCENE_ANIMATION_LOOKUP_PC *end_frame_lookup;
} SCENE_UPDATE_READER_PC;
STATIC_ASSERT(offsetof(SCENE_UPDATE_READER_PC, instance_count) == 0x48);
STATIC_ASSERT(offsetof(SCENE_UPDATE_READER_PC, animation_count) == 0x4a);
STATIC_ASSERT(offsetof(SCENE_UPDATE_READER_PC, instances) == 0x4c);
STATIC_ASSERT(offsetof(SCENE_UPDATE_READER_PC, matrices) == 0x50);
STATIC_ASSERT(offsetof(SCENE_UPDATE_READER_PC, animations) == 0x54);
STATIC_ASSERT(offsetof(SCENE_UPDATE_READER_PC, display_list) == 0x110);
STATIC_ASSERT(offsetof(SCENE_UPDATE_READER_PC, end_frame_lookup) == 0x1e0);
typedef struct SCENE_SPECIAL_TRANSFORM_PC {
    u8 unknown_00[0x40];
    NUMTX draw_matrix;
    u8 unknown_80[0x38];
    u32 flags;
    u8 unknown_bc[0x14];
} SCENE_SPECIAL_TRANSFORM_PC;
STATIC_ASSERT(sizeof(SCENE_SPECIAL_TRANSFORM_PC) == 0xd0);
STATIC_ASSERT(offsetof(SCENE_SPECIAL_TRANSFORM_PC, draw_matrix) == 0x40);
STATIC_ASSERT(offsetof(SCENE_SPECIAL_TRANSFORM_PC, flags) == 0xb8);

static void SceneUpdateMtxCopyPrivatePc(NUMTX *destination, const NUMTX *source) {
    destination->m00 = source->m00;
    destination->m01 = source->m01;
    destination->m02 = source->m02;
    destination->m03 = source->m03;
    destination->m10 = source->m10;
    destination->m11 = source->m11;
    destination->m12 = source->m12;
    destination->m13 = source->m13;
    destination->m20 = source->m20;
    destination->m21 = source->m21;
    destination->m22 = source->m22;
    destination->m23 = source->m23;
    destination->m30 = source->m30;
    destination->m31 = source->m31;
    destination->m32 = source->m32;
    destination->m33 = source->m33;
}

void __cdecl NuGScnUpdate(NUGSCN *gscn, f32 delta) {
    SCENE_UPDATE_READER_PC *scene = (SCENE_UPDATE_READER_PC *)gscn;
    SCENE_INSTANCE_ANIMATION_PC *instance = scene->instances;
    NUMTX *matrix = scene->matrices;
    NUDISPLAY_LIST_PC *display = scene->display_list;
    i32 i;
    if (scene->animations == NULL)
        return;
    if (scene->end_frame_lookup != NULL) {
        SCENE_ANIMATION_LOOKUP_ALLOCATION_PC *allocation =
            (SCENE_ANIMATION_LOOKUP_ALLOCATION_PC *)((u32)scene->end_frame_lookup -
                                                     offsetof(SCENE_ANIMATION_LOOKUP_ALLOCATION_PC, first));
        if (allocation->references < 2)
            scene->end_frame_lookup = NULL;
    }
    for (i = 0; i < scene->instance_count; ++i, ++instance, ++matrix) {
        SCENE_SPECIAL_TRANSFORM_PC *special;
        SCENE_ANIMATION_LOOKUP_PC *lookup = NULL;
        ANI3_HEADER_PC *animation;
        f32 end_frame, frame;
        u32 flags;
        if (instance->animation_index >= scene->animation_count)
            continue;
        special = &((SCENE_SPECIAL_TRANSFORM_PC *)display->specials)[instance->instance_index];
        if ((instance->flags >> 22) != 0 && scene->end_frame_lookup != NULL)
            lookup = &scene->end_frame_lookup[(instance->flags >> 22) - 1];
        else if ((special->flags & 2) == 0)
            continue;
        animation = scene->animations[instance->animation_index];
        if (animation != NULL)
            end_frame = NuAnimEndFrameOld(animation);
        else if (lookup != NULL)
            end_frame = (f32)lookup->end_frame;
        else
            continue;
        if (end_frame == 0.0f)
            continue;
        flags = instance->flags;
        if ((flags & 1) != 0) {
            i32 waiting = 0;
            instance->time += delta * instance->tfactor;
            frame = instance->time;
            if ((flags & 4) != 0) {
                if (!(frame >= instance->first_frame))
                    waiting = 1;
                else {
                    flags &= ~4U;
                    instance->flags = flags;
                    instance->time -= instance->first_frame - 1.0;
                    frame = instance->time;
                }
            }
            if (waiting == 0) {
                if (frame >= instance->interval + end_frame) {
                    if ((flags & 8) != 0) {
                        f64 period = (f64)instance->interval + end_frame - 1.0;
                        f32 cycles = NuFloor((instance->time - 1.0) / period);
                        f32 elapsed = cycles * period;
                        instance->time -= elapsed;
                        frame = instance->time;
                    } else {
                        flags &= ~1U;
                        instance->flags = flags;
                        instance->time = end_frame;
                        frame = end_frame;
                    }
                } else if (frame > end_frame)
                    frame = end_frame;
                else if (frame < 1.0f) {
                    flags &= ~1U;
                    instance->flags = flags;
                    instance->time = 1.0f;
                    frame = 1.0f;
                }
                if ((flags & 2) != 0)
                    frame = end_frame + 1.0 - frame;
            }
        } else
            frame = instance->time;
        if (frame != instance->previous_frame) {
            if (lookup != NULL) {
                u8 index = (u8)(instance->flags >> 14), value;
                i32 changed = StateAnimEvaluate2((STATE_ANIM_PC *)lookup, &index, &value, frame);
                instance->flags = (instance->flags & ~0x3fc000U) | ((u32)index << 14);
                if (changed != 0) {
                    if (instance->instance_index != 0xffff && value != 0)
                        special->flags |= 2;
                    else if (value == 0) {
                        if (instance->instance_index != 0xffff)
                            special->flags &= ~2U;
                        animation = NULL;
                    }
                }
            }
            if (animation != NULL)
                NuAnimCurve2SetApplyToMatrix_3(animation, 0, frame, matrix);
            instance->previous_frame = frame;
        }
        if (animation != NULL) {
            SceneUpdateMtxCopyPrivatePc(&instance->matrix, matrix);
            instance->matrix.m30 += special->draw_matrix.m30;
            instance->matrix.m31 += special->draw_matrix.m31;
            instance->matrix.m32 += special->draw_matrix.m32;
        }
    }
}
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
