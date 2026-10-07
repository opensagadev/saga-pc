#include "saga.h"
#include <string.h>

typedef struct SCENE_INGEST_EXTRA_FIELDS_PC {
    u8 unknown_00[0x60];
    i32 texture_animation_count;
    void *texture_animations;
    u16 *texture_animation_ids;
    u8 unknown_6c[0xfc];
    i32 character_version_selector;
    u8 unknown_16c[0x68];
} SCENE_INGEST_EXTRA_FIELDS_PC;
STATIC_ASSERT(sizeof(SCENE_INGEST_EXTRA_FIELDS_PC) == 0x1d4);
STATIC_ASSERT(offsetof(SCENE_INGEST_EXTRA_FIELDS_PC, texture_animation_count) == 0x60);
STATIC_ASSERT(offsetof(SCENE_INGEST_EXTRA_FIELDS_PC, texture_animations) == 0x64);
STATIC_ASSERT(offsetof(SCENE_INGEST_EXTRA_FIELDS_PC, texture_animation_ids) == 0x68);
STATIC_ASSERT(offsetof(SCENE_INGEST_EXTRA_FIELDS_PC, character_version_selector) == 0x168);
typedef struct SCENE_INGEST_PC {
    union {
        NUGSCN scene;
        SCENE_INGEST_EXTRA_FIELDS_PC ingestion;
    };
    u8 unknown_1d4[0x20];
    u32 legacy_format;
    u8 unknown_1f8[0x14];
} SCENE_INGEST_PC;
STATIC_ASSERT(sizeof(NUGSCN) == 0x1d4);
STATIC_ASSERT(sizeof(SCENE_INGEST_PC) == 0x20c);
STATIC_ASSERT(offsetof(SCENE_INGEST_PC, legacy_format) == 0x1f4);
typedef struct SCENE_INGEST_PTR_BLOCK_PC {
    u32 table_relative_offset;
    SCENE_INGEST_PC *scene;
} SCENE_INGEST_PTR_BLOCK_PC;
STATIC_ASSERT(sizeof(SCENE_INGEST_PTR_BLOCK_PC) == 8);
STATIC_ASSERT(offsetof(SCENE_INGEST_PTR_BLOCK_PC, scene) == 4);
typedef struct SCENE_INGEST_FILE_PC {
    u8 unknown_00[8];
    i32 version;
    u8 unknown_0c[0xc];
    SCENE_INGEST_PTR_BLOCK_PC pointer_block;
} SCENE_INGEST_FILE_PC;
STATIC_ASSERT(offsetof(SCENE_INGEST_FILE_PC, version) == 8);
STATIC_ASSERT(offsetof(SCENE_INGEST_FILE_PC, pointer_block) == 0x18);
typedef struct SCENE_INGEST_TEXTURE_ANIMATION_PC {
    struct SCENE_INGEST_TEXTURE_ANIMATION_PC *next, *previous;
    u16 *texture_ids;
    i16 texture_count;
    u8 flags, reserved_0f;
    NUMTL *material;
    PC_TEXTURE_ANIM_ENV *environment;
    char *name, *program_name;
} SCENE_INGEST_TEXTURE_ANIMATION_PC;
STATIC_ASSERT(sizeof(SCENE_INGEST_TEXTURE_ANIMATION_PC) == 0x20);
STATIC_ASSERT(offsetof(SCENE_INGEST_TEXTURE_ANIMATION_PC, next) == 0);
STATIC_ASSERT(offsetof(SCENE_INGEST_TEXTURE_ANIMATION_PC, previous) == 4);
STATIC_ASSERT(offsetof(SCENE_INGEST_TEXTURE_ANIMATION_PC, texture_ids) == 8);
STATIC_ASSERT(offsetof(SCENE_INGEST_TEXTURE_ANIMATION_PC, texture_count) == 0xc);
STATIC_ASSERT(offsetof(SCENE_INGEST_TEXTURE_ANIMATION_PC, flags) == 0xe);
STATIC_ASSERT(offsetof(SCENE_INGEST_TEXTURE_ANIMATION_PC, material) == 0x10);
STATIC_ASSERT(offsetof(SCENE_INGEST_TEXTURE_ANIMATION_PC, environment) == 0x14);
STATIC_ASSERT(offsetof(SCENE_INGEST_TEXTURE_ANIMATION_PC, program_name) == 0x1c);
typedef struct SCENE_INGEST_TEXTURE_PROGRAM_PC {
    struct SCENE_INGEST_TEXTURE_PROGRAM_PC *next, *previous;
    char name[32];
} SCENE_INGEST_TEXTURE_PROGRAM_PC;
STATIC_ASSERT(offsetof(SCENE_INGEST_TEXTURE_PROGRAM_PC, next) == 0);
STATIC_ASSERT(offsetof(SCENE_INGEST_TEXTURE_PROGRAM_PC, name) == 8);
void __cdecl NuGScnFixupPS(NUGSCN *);

static const char SceneIngestCharacterVersionWarningPc[] =
    "Your Character version is %d, whereas the Engine only accepts Character versions from %d to %d";
static const char SceneIngestSceneVersionWarningPc[] =
    "Your Scene version is %d, whereas the Engine only accepts Scene versions from %d to %d";

static NUGSCN *NuReadGraphicsData(VARIPTR *buffer, VARIPTR *buffer_end, char *path, SCENE_INGEST_FILE_PC *scene_data) {
    SCENE_INGEST_PC *scene;
    SCENE_INGEST_TEXTURE_ANIMATION_PC *animations;
    SCENE_INGEST_TEXTURE_PROGRAM_PC *program;
    i32 version, i;
    u32 texture_count, end;
    if (scene_data == NULL) {
        char converted_path[1024];
        NUFILE file;
        i32 remaining, uploaded;
        NuFileExtConvert(converted_path, path);
        file = NuFileOpen(converted_path, 0);
        if (file == 0) {
            PCAnimationLoadFailureNoOp(path);
            return NULL;
        }
        remaining = (i32)NuFileOpenSize(file);
        buffer->addr = (buffer->addr + 31) & ~31u;
        uploaded = NuGScnUploadGfxDataFromFilePS(buffer, *buffer_end, file);
        remaining -= uploaded;
        buffer->addr = (buffer->addr + 31) & ~31u;
        scene_data = (SCENE_INGEST_FILE_PC *)buffer->void_ptr;
        buffer->addr += remaining;
        NuFileRead(file, scene_data, remaining);
        NuFileClose(file);
    }
    version = scene_data->version;
    scene = *(SCENE_INGEST_PC **)NuPtrBlockFix(&scene_data->pointer_block);
    if (scene->ingestion.character_version_selector > 0) {
        if (version < 0 || version > 2)
            NuSevereWarning(SceneIngestCharacterVersionWarningPc, version, 0, 2);
    } else if (version < 0 || version > 2) {
        NuSevereWarning(SceneIngestSceneVersionWarningPc, version, 0, 2);
    }
    if (version < 2)
        scene->legacy_format = 1;
    NuGScnCreatePS(&scene->scene, buffer, buffer_end);
    for (i = 0; i < scene->scene.texture_count; ++i) {
        NUTEXDIM_PC *texture = scene->scene.textures[i];
        if (texture->ref_count < 0)
            scene->scene.texture_ids[i] = (i32)0xabcdabcd;
        else
            scene->scene.texture_ids[i] = NuTexCreateNative(texture);
    }
    NuGScnFixupTIDs(&scene->scene);
    animations = (SCENE_INGEST_TEXTURE_ANIMATION_PC *)scene->scene.texture_animations;
    texture_count = 0;
    for (i = 0; i < scene->ingestion.texture_animation_count; ++i) {
        end = (u32)animations[i].texture_ids + (i32)animations[i].texture_count;
        if (texture_count < end)
            texture_count = end;
    }
    for (end = 0; end < texture_count; ++end) {
        u16 index = scene->ingestion.texture_animation_ids[end];
        scene->ingestion.texture_animation_ids[end] = (u16)scene->scene.texture_ids[index];
    }
    for (i = 0; i < scene->ingestion.texture_animation_count; ++i) {
        SCENE_INGEST_TEXTURE_ANIMATION_PC *animation =
            &((SCENE_INGEST_TEXTURE_ANIMATION_PC *)scene->scene.texture_animations)[i];
        animation->next = NULL;
        animation->previous = NULL;
        animation->texture_ids = scene->ingestion.texture_animation_ids + (u32)animation->texture_ids;
        animation->flags = 0;
        animation->material = scene->scene.materials[(u32)animation->material];
        program = (SCENE_INGEST_TEXTURE_PROGRAM_PC *)GHGTextureProgramsPC;
        while (program != NULL) {
            if (NuStrICmp(animation->program_name, program->name) == 0)
                break;
            program = program->next;
        }
        animation->environment = NuTexAnimEnvCreate(buffer, animation->material, animation->texture_ids, program);
    }
    for (i = 0; i < scene->ingestion.texture_animation_count - 1; ++i) {
        animations = (SCENE_INGEST_TEXTURE_ANIMATION_PC *)scene->scene.texture_animations;
        animations[i].next = &animations[i + 1];
        animations[i + 1].previous = &animations[i];
    }
    NuTexAnimAddList(scene->scene.texture_animations);
    if (scene->scene.display_list != NULL)
        NuDisplaySceneAdd(scene->scene.display_list);
    NuGScnFixupPS(&scene->scene);
    scene->scene.additional_scenes = NULL;
    scene->scene.frame_reset_marker = 0;
    return &scene->scene;
}

NUGSCN *__cdecl NuGScnRead(VARIPTR *buffer, VARIPTR buffer_end, char *path) {
    return NuReadGraphicsData(buffer, &buffer_end, path, NULL);
}

NUGSCN *__cdecl PCGraphicsSceneReadClones(VARIPTR *buffer, VARIPTR buffer_end, char *path, i32 count) {
    NUGSCN *scene = NuReadGraphicsData(buffer, &buffer_end, path, NULL);
    SCENE_INGEST_PC **clones;
    i32 i;
    if (scene == NULL)
        return NULL;
    if (count > 1) {
        buffer->addr = (buffer->addr + 31) & ~31u;
        clones = (SCENE_INGEST_PC **)buffer->void_ptr;
        buffer->addr += count * sizeof(*clones);
        for (i = 0; i < count - 1; ++i) {
            buffer->addr = (buffer->addr + 31) & ~31u;
            clones[i] = (SCENE_INGEST_PC *)buffer->void_ptr;
            buffer->addr += sizeof(*clones[i]);
            memcpy(clones[i], scene, sizeof(*clones[i]));
            clones[i]->scene.display_list = NuDisplaySceneClone(scene->display_list, buffer);
            clones[i]->scene.display_list->gscene = &clones[i]->scene;
        }
        clones[count - 1] = NULL;
        scene->additional_scenes = (NUGSCN **)clones;
    }
    return scene;
}

typedef struct CHARACTER_GHG_API_STATE_PC {
    u8 unknown_00[0x80];
    i32 loading_hgobj;
} CHARACTER_GHG_API_STATE_PC;
STATIC_ASSERT(offsetof(CHARACTER_GHG_API_STATE_PC, loading_hgobj) == 0x80);
NUHGOBJ_EVAL_PC *__cdecl NuGHGRead(char *path, VARIPTR *buffer, VARIPTR end) {
    NUGSCN *scene;
    i32 i;
    ((CHARACTER_GHG_API_STATE_PC *)&nuapi)->loading_hgobj = 1;
    scene = NuReadGraphicsData(buffer, &end, path, NULL);
    if (scene != NULL && nuapi.force_shadows_on_characters != 0) {
        for (i = 0; i != scene->display_list->special_count; ++i)
            scene->display_list->visibility_flags[i] |= 0x20;
    }
    ((CHARACTER_GHG_API_STATE_PC *)&nuapi)->loading_hgobj = 0;
    return (NUHGOBJ_EVAL_PC *)scene;
}

void __cdecl NuGScnFixupTIDsPS(NUGSCN *scene);
typedef struct SCENE_MATERIAL_FLAGS_PC {
    u8 unknown_00[0x44];
    u32 flags;
} SCENE_MATERIAL_FLAGS_PC;
STATIC_ASSERT(offsetof(SCENE_MATERIAL_FLAGS_PC, flags) == 0x44);
STATIC_ASSERT(offsetof(NUTEXDIM_PC, checksum) == 8);
STATIC_ASSERT(offsetof(NUGSCN, texture_ids) == 0);
STATIC_ASSERT(offsetof(NUGSCN, texture_count) == 4);
STATIC_ASSERT(offsetof(NUGSCN, textures) == 8);
STATIC_ASSERT(offsetof(NUGSCN, materials) == 0xc);
STATIC_ASSERT(offsetof(NUGSCN, material_count) == 0x10);

/* Partial native void caller. Successful native remap operations are expanded
 * in place; only a missing fallback value leaves its destination untouched.
 * The return-valued NuGScnFixupTID entry remains held with its true i32 ABI. */
void __cdecl NuGScnFixupTIDs(NUGSCN *scene) {
    i32 i;
    for (i = 0; i < scene->material_count; ++i) {
        PC_SCENE_MTL_VIEW *material = (PC_SCENE_MTL_VIEW *)scene->materials[i];
        {
            i32 tid = material->texture_id;
            if (tid == -1) {
                ((PC_SCENE_MTL_VIEW *)scene->materials[i])->texture_id = 0;
            } else if ((tid & 0x4000) == 0) {
                ((PC_SCENE_MTL_VIEW *)scene->materials[i])->texture_id = (i16)scene->texture_ids[tid];
            } else {
                i32 resolved = NuTexResolveReference(scene, tid);
                if (resolved != 0) {
                    ((PC_SCENE_MTL_VIEW *)scene->materials[i])->texture_id = (i16)resolved;
                } else {
                    STUBBED_NAMED(
                        "Unresolved fallback and assignment held at native 0x5eee0e: no source-owned DDS value");
                }
            }
        }
        {
            i32 tid = material->specular_map_tid;
            if (tid == -1) {
                material->specular_map_tid = 0;
            } else if ((tid & 0x4000) == 0) {
                material->specular_map_tid = scene->texture_ids[tid];
            } else {
                i32 resolved = NuTexResolveReference(scene, tid);
                if (resolved != 0) {
                    material->specular_map_tid = resolved;
                } else {
                    STUBBED_NAMED(
                        "Unresolved fallback and assignment held at native 0x5eee25: no source-owned DDS value");
                }
            }
        }
        {
            i32 tid = material->lightmap_tex_id[0];
            if (tid == -1) {
                material->lightmap_tex_id[0] = 0;
            } else if ((tid & 0x4000) == 0) {
                material->lightmap_tex_id[0] = scene->texture_ids[tid];
            } else {
                i32 resolved = NuTexResolveReference(scene, tid);
                if (resolved != 0) {
                    material->lightmap_tex_id[0] = resolved;
                } else {
                    STUBBED_NAMED(
                        "Unresolved fallback and assignment held at native 0x5eee38: no source-owned DDS value");
                }
            }
        }
        {
            i32 tid = material->normal_map_tid;
            if (tid == -1) {
                material->normal_map_tid = 0;
            } else if ((tid & 0x4000) == 0) {
                material->normal_map_tid = scene->texture_ids[tid];
            } else {
                i32 resolved = NuTexResolveReference(scene, tid);
                if (resolved != 0) {
                    material->normal_map_tid = resolved;
                } else {
                    STUBBED_NAMED(
                        "Unresolved fallback and assignment held at native 0x5eee4b: no source-owned DDS value");
                }
            }
        }
        {
            i32 tid = material->lightmap_tex_id[1];
            if (tid == -1) {
                material->lightmap_tex_id[1] = 0;
            } else if ((tid & 0x4000) == 0) {
                material->lightmap_tex_id[1] = scene->texture_ids[tid];
            } else {
                i32 resolved = NuTexResolveReference(scene, tid);
                if (resolved != 0) {
                    material->lightmap_tex_id[1] = resolved;
                } else {
                    STUBBED_NAMED(
                        "Unresolved fallback and assignment held at native 0x5eee5e: no source-owned DDS value");
                }
            }
        }
        {
            i32 tid = material->envmap_cubic_tid;
            if (tid == -1) {
                material->envmap_cubic_tid = 0;
            } else if ((tid & 0x4000) == 0) {
                material->envmap_cubic_tid = scene->texture_ids[tid];
            } else {
                i32 resolved = NuTexResolveReference(scene, tid);
                if (resolved != 0) {
                    material->envmap_cubic_tid = resolved;
                } else {
                    STUBBED_NAMED(
                        "Unresolved fallback and assignment held at native 0x5eee71: no source-owned DDS value");
                }
            }
        }
        {
            i32 tid = material->unknown_texture_24c;
            if (tid == -1) {
                material->unknown_texture_24c = 0;
            } else if ((tid & 0x4000) == 0) {
                material->unknown_texture_24c = scene->texture_ids[tid];
            } else {
                i32 resolved = NuTexResolveReference(scene, tid);
                if (resolved != 0) {
                    material->unknown_texture_24c = resolved;
                } else {
                    STUBBED_NAMED(
                        "Unresolved fallback and assignment held at native 0x5eee84: no source-owned DDS value");
                }
            }
        }
        {
            i32 tid = material->shine_map_tid;
            if (tid == -1) {
                material->shine_map_tid = 0;
            } else if ((tid & 0x4000) == 0) {
                material->shine_map_tid = scene->texture_ids[tid];
            } else {
                i32 resolved = NuTexResolveReference(scene, tid);
                if (resolved != 0) {
                    material->shine_map_tid = resolved;
                } else {
                    STUBBED_NAMED(
                        "Unresolved fallback and assignment held at native 0x5eee97: no source-owned DDS value");
                }
            }
        }
        {
            i32 tid = material->diffuse_map_tex_id[0];
            if (tid == -1) {
                material->diffuse_map_tex_id[0] = 0;
            } else if ((tid & 0x4000) == 0) {
                material->diffuse_map_tex_id[0] = scene->texture_ids[tid];
            } else {
                i32 resolved = NuTexResolveReference(scene, tid);
                if (resolved != 0) {
                    material->diffuse_map_tex_id[0] = resolved;
                } else {
                    STUBBED_NAMED(
                        "Unresolved fallback and assignment held at native 0x5eeead: no source-owned DDS value");
                }
            }
        }
        {
            i32 tid = material->diffuse_map_tex_id[1];
            if (tid == -1) {
                material->diffuse_map_tex_id[1] = 0;
            } else if ((tid & 0x4000) == 0) {
                material->diffuse_map_tex_id[1] = scene->texture_ids[tid];
            } else {
                i32 resolved = NuTexResolveReference(scene, tid);
                if (resolved != 0) {
                    material->diffuse_map_tex_id[1] = resolved;
                } else {
                    STUBBED_NAMED(
                        "Unresolved fallback and assignment held at native 0x5eeec0: no source-owned DDS value");
                }
            }
        }
        {
            i32 tid = material->diffuse_map_tex_id[2];
            if (tid == -1) {
                material->diffuse_map_tex_id[2] = 0;
            } else if ((tid & 0x4000) == 0) {
                material->diffuse_map_tex_id[2] = scene->texture_ids[tid];
            } else {
                i32 resolved = NuTexResolveReference(scene, tid);
                if (resolved != 0) {
                    material->diffuse_map_tex_id[2] = resolved;
                } else {
                    STUBBED_NAMED(
                        "Unresolved fallback and assignment held at native 0x5eeed3: no source-owned DDS value");
                }
            }
        }
        {
            i32 tid = material->diffuse_map_tex_id[3];
            if (tid == -1) {
                material->diffuse_map_tex_id[3] = 0;
            } else if ((tid & 0x4000) == 0) {
                material->diffuse_map_tex_id[3] = scene->texture_ids[tid];
            } else {
                i32 resolved = NuTexResolveReference(scene, tid);
                if (resolved != 0) {
                    material->diffuse_map_tex_id[3] = resolved;
                } else {
                    STUBBED_NAMED(
                        "Unresolved fallback and assignment held at native 0x5eeee6: no source-owned DDS value");
                }
            }
        }
        {
            i32 tid = material->vtf_height_map_tid;
            if (tid == -1) {
                material->vtf_height_map_tid = 0;
            } else if ((tid & 0x4000) == 0) {
                material->vtf_height_map_tid = scene->texture_ids[tid];
            } else {
                i32 resolved = NuTexResolveReference(scene, tid);
                if (resolved != 0) {
                    material->vtf_height_map_tid = resolved;
                } else {
                    STUBBED_NAMED(
                        "Unresolved fallback and assignment held at native 0x5eeef9: no source-owned DDS value");
                }
            }
        }
        {
            i32 tid = material->vtf_normal_map_tid;
            if (tid == -1) {
                material->vtf_normal_map_tid = 0;
            } else if ((tid & 0x4000) == 0) {
                material->vtf_normal_map_tid = scene->texture_ids[tid];
            } else {
                i32 resolved = NuTexResolveReference(scene, tid);
                if (resolved != 0) {
                    material->vtf_normal_map_tid = resolved;
                } else {
                    STUBBED_NAMED(
                        "Unresolved fallback and assignment held at native 0x5eef0c: no source-owned DDS value");
                }
            }
        }
        {
            i32 tid = material->unknown_texture_298;
            if (tid == -1) {
                material->unknown_texture_298 = 0;
            } else if ((tid & 0x4000) == 0) {
                material->unknown_texture_298 = scene->texture_ids[tid];
            } else {
                i32 resolved = NuTexResolveReference(scene, tid);
                if (resolved != 0) {
                    material->unknown_texture_298 = resolved;
                } else {
                    STUBBED_NAMED(
                        "Unresolved fallback and assignment held at native 0x5eef1f: no source-owned DDS value");
                }
            }
        }
        {
            i32 tid = material->unknown_texture_29c;
            if (tid == -1) {
                material->unknown_texture_29c = 0;
            } else if ((tid & 0x4000) == 0) {
                material->unknown_texture_29c = scene->texture_ids[tid];
            } else {
                i32 resolved = NuTexResolveReference(scene, tid);
                if (resolved != 0) {
                    material->unknown_texture_29c = resolved;
                } else {
                    STUBBED_NAMED(
                        "Unresolved fallback and assignment held at native 0x5eef32: no source-owned DDS value");
                }
            }
        }
        ((SCENE_MATERIAL_FLAGS_PC *)scene->materials[i])->flags &= ~0x400000u;
        material = (PC_SCENE_MTL_VIEW *)scene->materials[i];
        NuMtlUpdatePS((NUMTL *)material);
        ++material->version;
    }
    NuGScnFixupTIDsPS(scene);
}
void __cdecl NuGScnRemove(NUGSCN *scene) {
    i32 i;
    if (scene->texture_animations != NULL)
        NuTexAnimRemoveList(scene->texture_animations);
    STUBBED_NAMED("Native TID restoration at 0x5ef898: unresolved texture value supplier held");
    for (i = 0; i < scene->texture_count; ++i) {
        if (scene->textures[i]->ref_count >= 0)
            NuTexDestroy(scene->texture_ids[i]);
    }
    if (scene->additional_scenes != NULL) {
        for (i = 0; scene->additional_scenes[i] != NULL; ++i)
            NuDisplaySceneDestroy(scene->additional_scenes[i]->display_list);
    }
    NuDisplaySceneDestroy(scene->display_list);
    NuGScnDestroyPS(scene);
}
