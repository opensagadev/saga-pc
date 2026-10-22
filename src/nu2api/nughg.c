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

/* Retail returns the empty diagnostic's EAX, which still holds the fallback TID. */
extern i32 __cdecl PCSpecialEmptyHandler_candidate();

i32 __cdecl NuGScnFixupTID(NUGSCN *scene, i32 tid) {
    i32 resolved;
    if (tid == -1)
        return 0;
    if (tid & 0x4000) {
        resolved = NuTexResolveReference(scene, tid);
        if (resolved == 0) {
            NUTEXDIM_PC *texture;
            resolved = NuTexGetUnresolvedTextureTIDPS();
            if (resolved > 0) {
                texture = NuTextureDimensions[resolved - 1];
                if (texture != NULL)
                    ++texture->ref_count;
            }
            texture = scene->textures[tid & 0x3fff];
            return PCSpecialEmptyHandler_candidate(
                texture->checksum[0], texture->checksum[1], texture->checksum[2], texture->checksum[3],
                texture->checksum[4], texture->checksum[5], texture->checksum[6], texture->checksum[7],
                texture->checksum[8], texture->checksum[9], texture->checksum[10], texture->checksum[11],
                texture->checksum[12], texture->checksum[13], texture->checksum[14], texture->checksum[15]);
        }
        return resolved;
    }
    return scene->texture_ids[tid];
}

void __cdecl NuGScnFixupTIDs(NUGSCN *scene) {
    i32 i;
    for (i = 0; i < scene->material_count; ++i) {
        PC_SCENE_MTL_VIEW *material = (PC_SCENE_MTL_VIEW *)scene->materials[i];
        ((PC_SCENE_MTL_VIEW *)scene->materials[i])->texture_id = (i16)NuGScnFixupTID(scene, material->texture_id);
        material->specular_map_tid = NuGScnFixupTID(scene, material->specular_map_tid);
        material->lightmap_tex_id[0] = NuGScnFixupTID(scene, material->lightmap_tex_id[0]);
        material->normal_map_tid = NuGScnFixupTID(scene, material->normal_map_tid);
        material->lightmap_tex_id[1] = NuGScnFixupTID(scene, material->lightmap_tex_id[1]);
        material->envmap_cubic_tid = NuGScnFixupTID(scene, material->envmap_cubic_tid);
        material->unknown_texture_24c = NuGScnFixupTID(scene, material->unknown_texture_24c);
        material->shine_map_tid = NuGScnFixupTID(scene, material->shine_map_tid);
        material->diffuse_map_tex_id[0] = NuGScnFixupTID(scene, material->diffuse_map_tex_id[0]);
        material->diffuse_map_tex_id[1] = NuGScnFixupTID(scene, material->diffuse_map_tex_id[1]);
        material->diffuse_map_tex_id[2] = NuGScnFixupTID(scene, material->diffuse_map_tex_id[2]);
        material->diffuse_map_tex_id[3] = NuGScnFixupTID(scene, material->diffuse_map_tex_id[3]);
        material->vtf_height_map_tid = NuGScnFixupTID(scene, material->vtf_height_map_tid);
        material->vtf_normal_map_tid = NuGScnFixupTID(scene, material->vtf_normal_map_tid);
        material->unknown_texture_298 = NuGScnFixupTID(scene, material->unknown_texture_298);
        material->unknown_texture_29c = NuGScnFixupTID(scene, material->unknown_texture_29c);
        ((SCENE_MATERIAL_FLAGS_PC *)scene->materials[i])->flags &= ~0x400000u;
        material = (PC_SCENE_MTL_VIEW *)scene->materials[i];
        NuMtlUpdatePS((NUMTL *)material);
        ++material->version;
    }
    NuGScnFixupTIDsPS(scene);
}
#define NUGSCN_RELEASE_TEXTURE_REF(tid)                                                                                \
    {                                                                                                                  \
        if ((tid) > 0) {                                                                                               \
            NUTEXDIM_PC *ref_texture = NuTextureDimensions[(tid) - 1];                                                 \
            if (ref_texture != NULL)                                                                                   \
                --ref_texture->ref_count;                                                                              \
        }                                                                                                              \
    }

i32 __cdecl NuGScnRestoreTID(NUGSCN *scene, i32 tid) {
    NUTEXDIM_PC *texture;
    i32 result = -1;
    u32 i;
    i32 j, matched;
    if (tid == 0)
        return result;
    if (tid == NuTexGetUnresolvedTextureTIDPS()) {
        NUGSCN_RELEASE_TEXTURE_REF(tid);
        return result;
    }
    texture = tid <= 0 ? NULL : NuTextureDimensions[tid - 1];
    for (i = 0; i != (u32)scene->texture_count; ++i) {
        matched = 1;
        for (j = 0; j < 16; ++j) {
            if (texture->checksum[j] != scene->textures[i]->checksum[j]) {
                matched = 0;
                break;
            }
        }
        if (!matched)
            continue;
        if (scene->texture_ids[i] != tid)
            continue;
        if (scene->textures[i]->ref_count < 0) {
            NUGSCN_RELEASE_TEXTURE_REF(tid);
            i |= 0x4000;
        }
        return (i32)i;
    }
    return result;
}

void __cdecl NuGScnRestoreTIDs(NUGSCN *scene) {
    i32 i;
    for (i = 0; i < scene->material_count; ++i) {
        PC_SCENE_MTL_VIEW *material;
        if (scene->texture_count == 0)
            continue;
        material = (PC_SCENE_MTL_VIEW *)scene->materials[i];
        ((PC_SCENE_MTL_VIEW *)scene->materials[i])->texture_id = (i16)NuGScnRestoreTID(scene, material->texture_id);
        material->specular_map_tid = NuGScnRestoreTID(scene, material->specular_map_tid);
        material->lightmap_tex_id[0] = NuGScnRestoreTID(scene, material->lightmap_tex_id[0]);
        material->normal_map_tid = NuGScnRestoreTID(scene, material->normal_map_tid);
        material->lightmap_tex_id[1] = NuGScnRestoreTID(scene, material->lightmap_tex_id[1]);
        material->envmap_cubic_tid = NuGScnRestoreTID(scene, material->envmap_cubic_tid);
        material->unknown_texture_24c = NuGScnRestoreTID(scene, material->unknown_texture_24c);
        material->shine_map_tid = NuGScnRestoreTID(scene, material->shine_map_tid);
        material->diffuse_map_tex_id[0] = NuGScnRestoreTID(scene, material->diffuse_map_tex_id[0]);
        material->diffuse_map_tex_id[1] = NuGScnRestoreTID(scene, material->diffuse_map_tex_id[1]);
        material->diffuse_map_tex_id[2] = NuGScnRestoreTID(scene, material->diffuse_map_tex_id[2]);
        material->diffuse_map_tex_id[3] = NuGScnRestoreTID(scene, material->diffuse_map_tex_id[3]);
        material->vtf_height_map_tid = NuGScnRestoreTID(scene, material->vtf_height_map_tid);
        material->vtf_normal_map_tid = NuGScnRestoreTID(scene, material->vtf_normal_map_tid);
        material->unknown_texture_298 = NuGScnRestoreTID(scene, material->unknown_texture_298);
        material->unknown_texture_29c = NuGScnRestoreTID(scene, material->unknown_texture_29c);
    }
    NuGScnRestoreTIDsPS(scene);
}

void __cdecl NuGScnRemove(NUGSCN *scene) {
    i32 i;
    if (scene->texture_animations != NULL)
        NuTexAnimRemoveList(scene->texture_animations);
    NuGScnRestoreTIDs(scene);
    for (i = 0; i < scene->texture_count; ++i) {
        i32 texture_id;
        NUTEXDIM_PC *texture;
        if (scene->textures[i]->ref_count < 0)
            continue;
        texture_id = scene->texture_ids[i];
        if (texture_id <= 0)
            continue;
        texture = NuTextureDimensions[texture_id - 1];
        if (texture != NULL && texture->ref_count > 0) {
            NUGSCN_RELEASE_TEXTURE_REF(texture_id);
        } else {
            NuTexDestroyPS(texture);
            NuTextureDimensions[texture_id - 1] = NULL;
        }
    }
    if (scene->additional_scenes != NULL) {
        for (i = 0; scene->additional_scenes[i] != NULL; ++i)
            NuDisplaySceneDestroy(scene->additional_scenes[i]->display_list);
    }
    NuDisplaySceneDestroy(scene->display_list);
    NuGScnDestroyPS(scene);
}
