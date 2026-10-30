#include "saga.h"

typedef struct cut_scene_pre_end_pc_s {
    u8 unknown_00[8];
    f32 duration;
} CUT_SCENE_PRE_END_PC;
STATIC_ASSERT(offsetof(CUT_SCENE_PRE_END_PC, duration) == 8);
typedef struct cut_camera_pre_end_pc_s {
    u8 unknown_00[9];
    i8 camera_index;
} CUT_CAMERA_PRE_END_PC;
STATIC_ASSERT(offsetof(CUT_CAMERA_PRE_END_PC, camera_index) == 9);
typedef struct cut_instance_pre_end_pc_s {
    u8 unknown_00[0x58];
    CUT_SCENE_PRE_END_PC *scene;
    u8 unknown_5c[0x2c];
    u32 flags_88;
    u32 flags_8c;
    f32 current_frame;
    f32 render_frame;
    u8 unknown_98[4];
    CUT_CAMERA_PRE_END_PC *camera_instance;
} CUT_INSTANCE_PRE_END_PC;
STATIC_ASSERT(offsetof(CUT_INSTANCE_PRE_END_PC, scene) == 0x58);
STATIC_ASSERT(offsetof(CUT_INSTANCE_PRE_END_PC, flags_88) == 0x88);
STATIC_ASSERT(offsetof(CUT_INSTANCE_PRE_END_PC, flags_8c) == 0x8c);
STATIC_ASSERT(offsetof(CUT_INSTANCE_PRE_END_PC, current_frame) == 0x90);
STATIC_ASSERT(offsetof(CUT_INSTANCE_PRE_END_PC, render_frame) == 0x94);
STATIC_ASSERT(offsetof(CUT_INSTANCE_PRE_END_PC, camera_instance) == 0x9c);

void __cdecl instNuGCutSceneEndButNotSystems(INSTNUGCUTSCENE *owner) {
    CUT_INSTANCE_PRE_END_PC *instance = (CUT_INSTANCE_PRE_END_PC *)owner;
    instance->flags_88 = (instance->flags_88 & ~2U) | 0x1000;
    instance->current_frame = instance->scene->duration;
    instance->render_frame = instance->scene->duration;
    instance->flags_8c &= ~0x40U;
    if (instance->camera_instance != NULL && instance->camera_instance->camera_index >= 0) {
        CutSceneCameraCTRL = 0;
    }
}

STATIC_ASSERT(offsetof(CUT_PLAYER_ACTIVE_PC, active) == 4);
extern CUT_PLAYER_ACTIVE_PC *CutScenePlayerPc;
void *__cdecl CutScenePlayer_Active(void) {
    return CutScenePlayerPc != NULL ? CutScenePlayerPc->active : NULL;
}

typedef struct CUT_PLAYER_STATE_PC {
    NUSPECIALHANDLE special;
    u8 flags;
    u8 unknown_0d[3];
} CUT_PLAYER_STATE_PC;
STATIC_ASSERT(sizeof(CUT_PLAYER_STATE_PC) == 0x10);
STATIC_ASSERT(offsetof(CUT_PLAYER_STATE_PC, flags) == 0xc);
typedef struct CUT_PLAYER_RECORD_PC {
    u8 unknown_00[0x48];
    CUT_PLAYER_STATE_PC *states;
    u8 state_count;
} CUT_PLAYER_RECORD_PC;
STATIC_ASSERT(offsetof(CUT_PLAYER_RECORD_PC, states) == 0x48);
STATIC_ASSERT(offsetof(CUT_PLAYER_RECORD_PC, state_count) == 0x4c);
typedef struct CUT_PLAYER_INST_ANIM_PC {
    u8 unknown_00[0x4c];
    f32 time;
    u32 flags;
} CUT_PLAYER_INST_ANIM_PC;
STATIC_ASSERT(offsetof(CUT_PLAYER_INST_ANIM_PC, time) == 0x4c);
STATIC_ASSERT(offsetof(CUT_PLAYER_INST_ANIM_PC, flags) == 0x50);
void __cdecl NuSpecialSetVisibility(NUSPECIALHANDLE *, i32);
void __cdecl CutScenePlayer_SetObjects(CUTINFO *owner) {
    CUT_PLAYER_RECORD_PC *cut = (CUT_PLAYER_RECORD_PC *)owner;
    i32 i;
    if (CutScenePlayerPc == NULL || CutScenePlayerPc->active == NULL)
        return;
    if (cut->states == NULL)
        return;
    for (i = 0; i < cut->state_count; ++i) {
        u8 flags = cut->states[i].flags;
        if ((flags & 1) != 0)
            NuSpecialSetVisibility(&cut->states[i].special, 1);
        else if ((flags & 2) != 0)
            NuSpecialSetVisibility(&cut->states[i].special, 0);
        else if ((flags & 4) != 0) {
            f32 time = NuSpecialGetAnimEndFrame(&cut->states[i].special);
            CUT_PLAYER_INST_ANIM_PC *animation =
                (CUT_PLAYER_INST_ANIM_PC *)NuSpecialGetInstAnim(&cut->states[i].special);
            if (animation != NULL) {
                animation->flags |= 1;
                animation->time = time;
            }
        }
    }
}

STATIC_ASSERT(sizeof(CUT_PLAYER_CLIP_CONFIG_PC) == 0x44);
STATIC_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG_PC, level_id) == 0);
STATIC_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG_PC, type) == 2);
STATIC_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG_PC, guest_episode) == 3);
STATIC_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG_PC, name) == 4);

typedef struct cut_player_config_pc_s {
    CUT_PLAYER_CLIP_CONFIG_PC *clips;
    CUT_PLAYER_CLIP_CONFIG_PC *active;
    u16 clip_count;
    i16 return_door;
    i16 *clip_text, *intro_text, *midtro_text, *outro_text, *ending_text;
} CUT_PLAYER_CONFIG_PC;
STATIC_ASSERT(sizeof(CUT_PLAYER_CONFIG_PC) == 0x20);
STATIC_ASSERT(offsetof(CUT_PLAYER_CONFIG_PC, clips) == 0);
STATIC_ASSERT(offsetof(CUT_PLAYER_CONFIG_PC, active) == 4);
STATIC_ASSERT(offsetof(CUT_PLAYER_CONFIG_PC, clip_count) == 8);
STATIC_ASSERT(offsetof(CUT_PLAYER_CONFIG_PC, return_door) == 0xa);
STATIC_ASSERT(offsetof(CUT_PLAYER_CONFIG_PC, clip_text) == 0xc);
STATIC_ASSERT(offsetof(CUT_PLAYER_CONFIG_PC, intro_text) == 0x10);
STATIC_ASSERT(offsetof(CUT_PLAYER_CONFIG_PC, midtro_text) == 0x14);
STATIC_ASSERT(offsetof(CUT_PLAYER_CONFIG_PC, outro_text) == 0x18);
STATIC_ASSERT(offsetof(CUT_PLAYER_CONFIG_PC, ending_text) == 0x1c);
extern i32(__cdecl *CutScenePlayer_AcceptFn)(CUT_PLAYER_CLIP_CONFIG_PC *);

CUT_PLAYER_ACTIVE_PC *__cdecl CutScenePlayer_Available(void) {
    return CutScenePlayerPc;
}

void __cdecl CutScenePlayer_Reset(void) {
    if (CutScenePlayerPc != NULL)
        CutScenePlayerPc->active = NULL;
}

void __cdecl CutScenePlayer_Configure(char *file, VARIPTR *buffer, VARIPTR *end, i16 *clip_text, i16 *intro,
                                      i16 *midtro, i16 *outro, i16 *ending) {
    NUFPAR_PC *fp;
    CUT_PLAYER_CONFIG_PC player;
    CUT_PLAYER_CLIP_CONFIG_PC *clip;
    i32 level;

    fp = NuFParCreate(file);
    if (fp == NULL) {
        CutSceneConfigMissingPc(file);
        return;
    }
    buffer->addr = (buffer->addr + 3) & ~3U;
    player.clips = (CUT_PLAYER_CLIP_CONFIG_PC *)buffer->void_ptr;
    player.active = NULL;
    player.clip_count = 0;
    player.clip_text = clip_text;
    player.intro_text = intro;
    player.midtro_text = midtro;
    player.outro_text = outro;
    player.ending_text = ending;
    while (NuFParGetLine(fp)) {
        if (!NuFParGetWord(fp) || NuStrICmp(fp->word_buffer_ptr, "clip") != 0)
            continue;
        clip = &player.clips[player.clip_count];
        clip->level_id = -1;
        clip->type = 0;
        clip->name[0] = 0;
        while (NuFParGetWord(fp)) {
            if (NuStrICmp(fp->word_buffer_ptr, "in_level") == 0) {
                if (NuFParGetWord(fp)) {
                    if (Level_FindByName(fp->word_buffer_ptr, &level) != NULL)
                        clip->level_id = (i16)level;
                    else
                        CutSceneConfigMissingPc(fp->word_buffer_ptr);
                }
            } else if (NuStrICmp(fp->word_buffer_ptr, "name") == 0) {
                if (NuFParGetWord(fp) && NuStrLen(fp->word_buffer_ptr) < 64)
                    NuStrCpy(clip->name, fp->word_buffer_ptr);
            } else if (NuStrICmp(fp->word_buffer_ptr, "intro") == 0) {
                clip->type = 0;
            } else if (NuStrICmp(fp->word_buffer_ptr, "midtro") == 0) {
                clip->type = 1;
            } else if (NuStrICmp(fp->word_buffer_ptr, "outro") == 0) {
                clip->type = 2;
            } else if (NuStrICmp(fp->word_buffer_ptr, "ending") == 0) {
                clip->type = 3;
            }
        }
        if (clip->level_id != -1 && (CutScenePlayer_AcceptFn == NULL || CutScenePlayer_AcceptFn(clip) != 0))
            ++player.clip_count;
    }
    NuFParDestroy(fp);
    if (player.clip_count > 0) {
        buffer->void_ptr = player.clips + player.clip_count;
        PCByteCopy_candidate(buffer->void_ptr, &player, sizeof(player));
        CutScenePlayerPc = (CUT_PLAYER_ACTIVE_PC *)buffer->void_ptr;
        buffer->addr += sizeof(player);
    } else {
        CutSceneConfigMissingPc(file);
    }
}

STATIC_ASSERT(offsetof(AREA_DATA_PC, episode_index) == 0x86);
STATIC_ASSERT(sizeof(EPISODEDATA_PC) == 0x1c);
STATIC_ASSERT(offsetof(COLLECTION_AREA_SAVE_PC, area_complete) == 1);

/* 0057e720..0057e7d2, ordinary one-argument cdecl. */
i32 __cdecl CutScenePlayer_CanStart(i32 clip_id) {
    CUT_PLAYER_CLIP_CONFIG_PC *clips;
    AREA_DATA_PC *area;
    i32 area_id, episode;
    if (CutScenePlayerPc != NULL && clip_id >= 0 && clip_id < CutScenePlayerPc->clip_count) {
        clips = (CUT_PLAYER_CLIP_CONFIG_PC *)CutScenePlayerPc->clips;
        area_id = LDataList[clips[clip_id].level_id].area_index;
        if (area_id != -1) {
            area = &ADataList[area_id];
            if ((area->flags & 2) != 0) {
                episode = area->episode_index;
                if (episode != -1 && Episode_IsComplete(&EDataList[episode], NULL) != 0)
                    return 2;
            } else if (Game_AreaSave != NULL && Game_AreaSave[area_id].area_complete != 0) {
                return 1;
            }
        }
    }
    return 0;
}

/* 0057e7e0..0057e849. The following 0057e850 text function is a separate
 * native owner. PC accepts one argument and does not write return_door. */
void __cdecl CutScenePlayer_Start(i32 clip_id) {
    CUT_PLAYER_CLIP_CONFIG_PC *clips;
    if (CutScenePlayerPc != NULL && clip_id >= 0 && clip_id < CutScenePlayerPc->clip_count) {
        clips = (CUT_PLAYER_CLIP_CONFIG_PC *)CutScenePlayerPc->clips;
        NewLevelFromMenu(&LDataList[clips[clip_id].level_id], -1, -1, 1);
        CutScenePlayerPc->active = &((CUT_PLAYER_CLIP_CONFIG_PC *)CutScenePlayerPc->clips)[clip_id];
        NextArea_FreePlay = 0;
        FreePlay = 0;
    }
}

extern const char Text_UnknownText[2];
extern const char CutPlayerClipFormatPc[8], CutPlayerTextSeparatorPc[4];
STATIC_ASSERT(offsetof(AREA_DATA_PC, name_id) == 0x78);
STATIC_ASSERT(offsetof(EPISODEDATA_PC, text_id) == 2);
STATIC_ASSERT(offsetof(LEVELDATA_PC, episode) == 0xa2);
STATIC_ASSERT(offsetof(LEVELDATA_PC, area_index) == 0xa3);

/* 0057e850..0057ea41 instructions, alignment to0057ea44, four-entry table
 *0057ea44..0057ea54. PC mode is only a boolean controlling the type suffix. */
void __cdecl CutScenePlayer_GetText(i32 id, char *title, char *subtitle, i32 mode) {
    CUT_PLAYER_CLIP_CONFIG_PC *clips;
    LEVELDATA_PC *level;
    AREA_DATA_PC *area;
    i32 episode;
    if (CutScenePlayerPc == NULL) {
        if (title != NULL)
            NuStrCpy(title, Text_UnknownText);
        if (subtitle != NULL)
            NuStrCpy(subtitle, Text_UnknownText);
        return;
    }
    if (title != NULL) {
        sprintf(title, CutScenePlayerPc->clip_text != NULL ? TTab[*CutScenePlayerPc->clip_text] : CutPlayerClipFormatPc,
                id + 1);
        if (mode != 0) {
            clips = (CUT_PLAYER_CLIP_CONFIG_PC *)CutScenePlayerPc->clips;
            switch (clips[id].type) {
                case 0:
                    if (CutScenePlayerPc->intro_text != NULL) {
                        NuStrCat(title, CutPlayerTextSeparatorPc);
                        NuStrCat(title, TTab[*CutScenePlayerPc->intro_text]);
                    }
                    break;
                case 1:
                    if (CutScenePlayerPc->midtro_text != NULL) {
                        NuStrCat(title, CutPlayerTextSeparatorPc);
                        NuStrCat(title, TTab[*CutScenePlayerPc->midtro_text]);
                    }
                    break;
                case 2:
                    if (CutScenePlayerPc->outro_text != NULL) {
                        NuStrCat(title, CutPlayerTextSeparatorPc);
                        NuStrCat(title, TTab[*CutScenePlayerPc->outro_text]);
                    }
                    break;
                case 3:
                    if (CutScenePlayerPc->ending_text != NULL) {
                        NuStrCat(title, CutPlayerTextSeparatorPc);
                        NuStrCat(title, TTab[*CutScenePlayerPc->ending_text]);
                    }
                    break;
            }
        }
    }
    if (subtitle != NULL) {
        clips = (CUT_PLAYER_CLIP_CONFIG_PC *)CutScenePlayerPc->clips;
        level = &LDataList[clips[id].level_id];
        if (level->area_index != -1) {
            area = &ADataList[level->area_index];
            if ((area->flags & 2) == 0) {
                NuStrCpy(subtitle, TTab[area->name_id]);
                return;
            }
            episode = (i8)level->episode;
            if (episode != -1) {
                NuStrCpy(subtitle, TTab[EDataList[episode].text_id]);
                return;
            }
        }
        NuStrCpy(subtitle, Text_UnknownText);
    }
}
typedef struct cut_average_rigid_system_pc_s {
    void *rigids;
    u16 count;
} CUT_AVERAGE_RIGID_SYSTEM_PC;
STATIC_ASSERT(offsetof(CUT_AVERAGE_RIGID_SYSTEM_PC, count) == 4);
typedef struct cut_average_scene_pc_s {
    u8 unknown_00[0x14];
    CUT_AVERAGE_RIGID_SYSTEM_PC *rigid_system;
} CUT_AVERAGE_SCENE_PC;
STATIC_ASSERT(offsetof(CUT_AVERAGE_SCENE_PC, rigid_system) == 0x14);
typedef struct cut_average_rigid_pc_s {
    NUSPECIALHANDLE special;
    u8 unknown_0c[4];
} CUT_AVERAGE_RIGID_PC;
STATIC_ASSERT(sizeof(CUT_AVERAGE_RIGID_PC) == 0x10);
typedef struct cut_average_rigid_instance_pc_s {
    CUT_AVERAGE_RIGID_PC *rigids;
} CUT_AVERAGE_RIGID_INSTANCE_PC;
typedef struct cut_average_instance_pc_s {
    u8 unknown_00[0x58];
    CUT_AVERAGE_SCENE_PC *scene;
    u8 unknown_5c[4];
    NUVEC center;
    u8 unknown_6c[0x34];
    CUT_AVERAGE_RIGID_INSTANCE_PC *rigid_instance;
} CUT_AVERAGE_INSTANCE_PC;
STATIC_ASSERT(offsetof(CUT_AVERAGE_INSTANCE_PC, scene) == 0x58);
STATIC_ASSERT(offsetof(CUT_AVERAGE_INSTANCE_PC, center) == 0x60);
STATIC_ASSERT(offsetof(CUT_AVERAGE_INSTANCE_PC, rigid_instance) == 0xa0);

void __cdecl instNuGCutSceneCalculateAverageCentre(INSTNUGCUTSCENE *owner, NUMTX *matrix, NUVEC *out) {
    CUT_AVERAGE_INSTANCE_PC *instance = (CUT_AVERAGE_INSTANCE_PC *)owner;
    f32 count = 0.0f;
    i32 i;
    out->x = 0.0f;
    out->y = 0.0f;
    out->z = 0.0f;
    if (instance == NULL || instance->scene == NULL || instance->scene->rigid_system == NULL ||
        instance->rigid_instance->rigids == NULL)
        return;
    for (i = 0; i < instance->scene->rigid_system->count; ++i) {
        if (NuSpecialExistsFn(&instance->rigid_instance->rigids[i].special) != 0) {
            NUVEC *position = NuSpecialGetPos(&instance->rigid_instance->rigids[i].special);
            out->x += position->x;
            out->y += position->y;
            out->z += position->z;
            count += 1.0;
        }
    }
    if (count > 0.0f) {
        f32 inverse = 1.0f / count;
        out->x *= inverse;
        out->y *= inverse;
        out->z *= inverse;
    }
    if (matrix != NULL)
        NuVecMtxTransform(&instance->center, &instance->center, matrix);
}

typedef struct cut_memory_scene_pc_s {
    u8 unknown_00[0x58];
    u32 loaded_size;
} CUT_MEMORY_SCENE_PC;
typedef struct cut_memory_instance_pc_s {
    u8 unknown_00[0x58];
    CUT_MEMORY_SCENE_PC *scene;
    u8 unknown_5c[0x94];
    u32 allocation_size;
} CUT_MEMORY_INSTANCE_PC;
typedef struct cut_memory_entry_pc_s {
    void *handle;
    CUT_MEMORY_SCENE_PC *scene;
    f32 accumulated_duration;
    u8 flags;
    u8 unknown_0d[3];
} CUT_MEMORY_ENTRY_PC;
typedef struct cut_memory_vertex_pc_s {
    f32 x, y, z;
    u32 colour;
} CUT_MEMORY_VERTEX_PC;
STATIC_ASSERT(offsetof(CUT_MEMORY_SCENE_PC, loaded_size) == 0x58);
STATIC_ASSERT(offsetof(CUT_MEMORY_INSTANCE_PC, scene) == 0x58);
STATIC_ASSERT(offsetof(CUT_MEMORY_INSTANCE_PC, allocation_size) == 0xf0);
STATIC_ASSERT(sizeof(CUT_MEMORY_ENTRY_PC) == 0x10);
STATIC_ASSERT(offsetof(CUT_MEMORY_ENTRY_PC, flags) == 0x0c);
STATIC_ASSERT(offsetof(CUT_MEMORY_VERTEX_PC, colour) == 0x0c);

#pragma section(".bss$DefragCutSceneEndMem", read, write)
__declspec(allocate(".bss$DefragCutSceneEndMem")) void *DefragCutSceneEndMem = NULL;
#pragma section(".bss$DefragCutSceneListSize", read, write)
__declspec(allocate(".bss$DefragCutSceneListSize")) i32 DefragCutSceneListSize = 0;
#pragma section(".bss$DefragCutSceneBaseMem", read, write)
__declspec(allocate(".bss$DefragCutSceneBaseMem")) void *DefragCutSceneBaseMem = NULL;
#pragma section(".bss$DefragInstBaseMem", read, write)
__declspec(allocate(".bss$DefragInstBaseMem")) void *DefragInstBaseMem = NULL;
#pragma section(".bss$DefragInstEndMem", read, write)
__declspec(allocate(".bss$DefragInstEndMem")) void *DefragInstEndMem = NULL;
#pragma section(".bss$DefragCutSceneListBase", read, write)
__declspec(allocate(".bss$DefragCutSceneListBase")) CUT_MEMORY_ENTRY_PC *DefragCutSceneListBase = NULL;
#pragma section(".bss$DefragGetInstFn", read, write)
__declspec(allocate(".bss$DefragGetInstFn")) CUT_MEMORY_INSTANCE_PC *(__cdecl *DefragGetInstFn)(void *) = NULL;

static void CutSceneSetPrimitiveColourPc(i32 colour) {
    u32 converted;
    if (NuPrimNeedsOverbrightening != 0) {
        converted = ((colour >> 16) & 0xff) + ((colour & 0xff) << 16) + (colour & 0xff000000u) + (colour & 0xff00);
    } else {
        converted =
            ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000u);
    }
    ((CUT_MEMORY_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour = converted;
}

void __cdecl DisplayCutSceneMemory(void) {
    CUT_MEMORY_ENTRY_PC *entry;
    CUT_MEMORY_ENTRY_PC *end;
    CUT_MEMORY_INSTANCE_PC *instance;
    CUT_MEMORY_SCENE_PC *scene;
    u32 start;
    u32 finish;
    f32 scale;
    f32 start_x;
    f32 end_x;
    if (DefragCutSceneEndMem == NULL || DefragCutSceneListSize == 0 || DefragCutSceneBaseMem == NULL)
        return;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuPrim2DBegin(16, 5, NULL);
    if (DefragInstBaseMem == NULL) {
        scale = (f32)(580.0 / (f64)((u32)DefragCutSceneEndMem - (u32)DefragCutSceneBaseMem));
        CutSceneSetPrimitiveColourPc(0x40000080);
        NuPrim2DAddXYZ(30.0f, 206.0f, 0.0f);
        CutSceneSetPrimitiveColourPc(0x40000080);
        NuPrim2DAddXYZ(610.0f, 210.0f, 0.0f);
        end = DefragCutSceneListBase + DefragCutSceneListSize;
        for (entry = DefragCutSceneListBase; entry < end; ++entry) {
            if ((entry->flags & 4) == 0)
                continue;
            instance = DefragGetInstFn(entry->handle);
            scene = instance->scene;
            start = (u32)scene - (u32)DefragCutSceneBaseMem;
            finish = start + scene->loaded_size;
            start_x = (f32)((f64)start * scale);
            end_x = (f32)((f64)finish * scale);
            CutSceneSetPrimitiveColourPc(0x40008080);
            start_x = (f32)(start_x + 30.0);
            NuPrim2DAddXYZ(start_x, 206.0f, 0.0f);
            CutSceneSetPrimitiveColourPc(0x40008080);
            end_x = (f32)(end_x + 30.0);
            NuPrim2DAddXYZ(end_x, 210.0f, 0.0f);
            start = (u32)instance - (u32)DefragCutSceneBaseMem;
            finish = start + instance->allocation_size;
            start_x = (f32)((f64)start * scale);
            end_x = (f32)((f64)finish * scale);
            CutSceneSetPrimitiveColourPc(0x40800080);
            start_x = (f32)(start_x + 30.0);
            NuPrim2DAddXYZ(start_x, 206.0f, 0.0f);
            CutSceneSetPrimitiveColourPc(0x40800080);
            end_x = (f32)(end_x + 30.0);
            NuPrim2DAddXYZ(end_x, 210.0f, 0.0f);
        }
    } else {
        scale = (f32)(580.0 / (f64)((u32)DefragCutSceneEndMem - (u32)DefragCutSceneBaseMem));
        CutSceneSetPrimitiveColourPc(0x40000080);
        NuPrim2DAddXYZ(30.0f, 206.0f, 0.0f);
        CutSceneSetPrimitiveColourPc(0x40000080);
        NuPrim2DAddXYZ(610.0f, 208.0f, 0.0f);
        end = DefragCutSceneListBase + DefragCutSceneListSize;
        for (entry = DefragCutSceneListBase; entry < end; ++entry) {
            if ((entry->flags & 4) == 0)
                continue;
            instance = DefragGetInstFn(entry->handle);
            scene = instance->scene;
            start = (u32)scene - (u32)DefragCutSceneBaseMem;
            finish = start + scene->loaded_size;
            start_x = (f32)((f64)start * scale);
            end_x = (f32)((f64)finish * scale);
            CutSceneSetPrimitiveColourPc(0x40008080);
            start_x = (f32)(start_x + 30.0);
            NuPrim2DAddXYZ(start_x, 206.0f, 0.0f);
            CutSceneSetPrimitiveColourPc(0x40008080);
            end_x = (f32)(end_x + 30.0);
            NuPrim2DAddXYZ(end_x, 208.0f, 0.0f);
        }
        scale = (f32)(580.0 / (f64)((u32)DefragInstEndMem - (u32)DefragInstBaseMem));
        CutSceneSetPrimitiveColourPc(0x40000060);
        NuPrim2DAddXYZ(30.0f, 209.0f, 0.0f);
        CutSceneSetPrimitiveColourPc(0x40000060);
        NuPrim2DAddXYZ(610.0f, 211.0f, 0.0f);
        for (entry = DefragCutSceneListBase; entry < end; ++entry) {
            if ((entry->flags & 4) == 0)
                continue;
            instance = DefragGetInstFn(entry->handle);
            start = (u32)instance - (u32)DefragInstBaseMem;
            finish = start + instance->allocation_size;
            start_x = (f32)((f64)start * scale);
            end_x = (f32)((f64)finish * scale);
            CutSceneSetPrimitiveColourPc(0x40800080);
            start_x = (f32)(start_x + 30.0);
            NuPrim2DAddXYZ(start_x, 209.0f, 0.0f);
            CutSceneSetPrimitiveColourPc(0x40800080);
            end_x = (f32)(end_x + 30.0);
            NuPrim2DAddXYZ(end_x, 211.0f, 0.0f);
        }
    }
    NuPrim2DEnd();
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}
