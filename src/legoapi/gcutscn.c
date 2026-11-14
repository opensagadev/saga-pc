#include "saga.h"
#include <stdio.h>

typedef struct cut_scene_pre_end_pc_s {
    u8 unknown_00[8];   // 0x0
    f32 duration;       // 0x8
} CUT_SCENE_PRE_END_PC; // size 0xc

typedef struct cut_camera_pre_end_pc_s {
    void *keys;          // 0x0
    u8 unknown_04[5];    // 0x4
    i8 camera_index;     // 0x9
    u8 unknown_0a;       // 0xa
    u8 key_count;        // 0xb
} CUT_CAMERA_PRE_END_PC; // size 0xc

typedef struct cut_instance_pre_end_pc_s {
    u8 unknown_00[0x58];                    // 0x00
    CUT_SCENE_PRE_END_PC *scene;            // 0x58
    u8 unknown_5c[0x2c];                    // 0x5c
    u32 flags_88;                           // 0x88
    u32 flags_8c;                           // 0x8c
    f32 current_frame;                      // 0x90
    f32 render_frame;                       // 0x94
    u8 unknown_98[4];                       // 0x98
    CUT_CAMERA_PRE_END_PC *camera_instance; // 0x9c
} CUT_INSTANCE_PRE_END_PC;                  // size 0xa0

void instNuGCutSceneEndButNotSystems(INSTNUGCUTSCENE *owner) {
    CUT_INSTANCE_PRE_END_PC *instance = (CUT_INSTANCE_PRE_END_PC *)owner;
    instance->current_frame = instance->scene->duration;
    instance->render_frame = instance->scene->duration;
    instance->flags_8c &= ~0x40U;
    instance->flags_88 = (instance->flags_88 & ~2U) | 0x1000;
    if (instance->camera_instance != NULL && instance->camera_instance->camera_index >= 0) {
        CutSceneCameraCTRL = 0;
    }
}

void fun_00500400(INSTNUGCUTSCENE *instance) {
    instance->flags_88 &= ~0x800;
}

void fun_00500410(INSTNUGCUTSCENE *instance) {
    instance->flags_88 |= 0x800;
}

void fun_00500420(INSTNUGCUTSCENE *instance, i32 value) {
    if (value > 31) {
        value = 31;
    }
    instance->flags_88 = (instance->flags_88 & ~0x3e000U) | ((value << 13) & 0x3e000);
}

void fun_00500450(INSTNUGCUTSCENE *instance, i32 value) {
    instance->flags_8c = (instance->flags_8c & ~0x40U) | ((value << 6) & 0x40);
}

void fun_00500470(INSTNUGCUTSCENE *instance, INSTNUGCUTSCENE *chained) {
    instance->chained_instance = chained;
}

void fun_00500480(INSTNUGCUTSCENE *instance, i32 value) {
    instance->unknown_b4 = value;
}

void fun_00500490(INSTNUGCUTSCENE *owner, i32 count, VARIPTR *buffer) {
    CUT_CAMERA_PRE_END_PC *camera = ((CUT_INSTANCE_PRE_END_PC *)owner)->camera_instance;
    if (camera != NULL && count != 0) {
        camera->key_count = count;
        buffer->addr = (buffer->addr + 15) & ~15;
        camera->keys = buffer->void_ptr;
        buffer->addr += count * 16;
        memset(camera->keys, 0, count * 16);
    }
}

void fun_005004d0(void) {
}

STATIC_ASSERT(offsetof(CUT_PLAYER_ACTIVE_PC, active) == 4);

typedef struct CUT_PLAYER_STATE_PC {
    NUSPECIALHANDLE special; // 0x0
    u8 flags;                // 0xc
    u8 unknown_0d[3];        // 0xd
} CUT_PLAYER_STATE_PC;       // size 0x10

typedef struct CUT_PLAYER_RECORD_PC {
    u8 unknown_00[0x48];         // 0x00
    CUT_PLAYER_STATE_PC *states; // 0x48
    u8 state_count;              // 0x4c
} CUT_PLAYER_RECORD_PC;          // size 0x50

typedef struct CUT_PLAYER_INST_ANIM_PC {
    u8 unknown_00[0x4c];   // 0x00
    f32 time;              // 0x4c
    u32 flags;             // 0x50
} CUT_PLAYER_INST_ANIM_PC; // size 0x54

STATIC_ASSERT(sizeof(CUT_PLAYER_CLIP_CONFIG_PC) == 0x44);
STATIC_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG_PC, level_id) == 0);
STATIC_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG_PC, type) == 2);
STATIC_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG_PC, guest_episode) == 3);
STATIC_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG_PC, name) == 4);

typedef struct cut_player_config_pc_s {
    CUT_PLAYER_CLIP_CONFIG_PC *clips;  // 0x00
    CUT_PLAYER_CLIP_CONFIG_PC *active; // 0x04
    u16 clip_count;                    // 0x08
    i16 return_door;                   // 0x0a
    i16 *clip_text;                    // 0x0c
    i16 *intro_text;                   // 0x10
    i16 *midtro_text;                  // 0x14
    i16 *outro_text;                   // 0x18
    i16 *ending_text;                  // 0x1c
} CUT_PLAYER_CONFIG_PC;                // size 0x20

STATIC_ASSERT(offsetof(AREA_DATA_PC, episode_index) == 0x86);
STATIC_ASSERT(sizeof(EPISODEDATA_PC) == 0x1c);
STATIC_ASSERT(offsetof(COLLECTION_AREA_SAVE_PC, area_complete) == 1);

STATIC_ASSERT(offsetof(AREA_DATA_PC, name_id) == 0x78);
STATIC_ASSERT(offsetof(EPISODEDATA_PC, text_id) == 2);
STATIC_ASSERT(offsetof(LEVELDATA_PC, episode) == 0xa2);
STATIC_ASSERT(offsetof(LEVELDATA_PC, area_index) == 0xa3);

typedef struct cut_average_rigid_system_pc_s {
    void *rigids;              // 0x0
    u16 count;                 // 0x4
} CUT_AVERAGE_RIGID_SYSTEM_PC; // size 0x8

typedef struct cut_average_scene_pc_s {
    u8 unknown_00[0x14];                       // 0x00
    CUT_AVERAGE_RIGID_SYSTEM_PC *rigid_system; // 0x14
} CUT_AVERAGE_SCENE_PC;                        // size 0x18

typedef struct cut_average_rigid_pc_s {
    NUSPECIALHANDLE special; // 0x0
    u8 unknown_0c[4];        // 0xc
} CUT_AVERAGE_RIGID_PC;      // size 0x10

typedef struct cut_average_rigid_instance_pc_s {
    CUT_AVERAGE_RIGID_PC *rigids;
} CUT_AVERAGE_RIGID_INSTANCE_PC;

typedef struct cut_average_instance_pc_s {
    u8 unknown_00[0x58];                           // 0x00
    CUT_AVERAGE_SCENE_PC *scene;                   // 0x58
    u8 unknown_5c[4];                              // 0x5c
    NUVEC center;                                  // 0x60
    u8 unknown_6c[0x34];                           // 0x6c
    CUT_AVERAGE_RIGID_INSTANCE_PC *rigid_instance; // 0xa0
} CUT_AVERAGE_INSTANCE_PC;                         // size 0xa4

void instNuGCutSceneCalculateAverageCentre(INSTNUGCUTSCENE *owner, NUMTX *matrix, NUVEC *out) {
    CUT_AVERAGE_INSTANCE_PC *instance = (CUT_AVERAGE_INSTANCE_PC *)owner;
    f32 count = 0.0f;
    i32 i;
    out->x = 0.0f;
    out->y = 0.0f;
    out->z = 0.0f;
    if (instance == NULL || instance->scene == NULL || instance->scene->rigid_system == NULL ||
        instance->rigid_instance->rigids == NULL) {
        return;
    }
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
        count = 1.0f / count;
        out->x *= count;
        out->y *= count;
        out->z *= count;
    }
    if (matrix != NULL) {
        NuVecMtxTransform(&instance->center, &instance->center, matrix);
    }
}

typedef struct cut_memory_scene_pc_s {
    u8 unknown_00[0x58]; // 0x00
    u32 loaded_size;     // 0x58
} CUT_MEMORY_SCENE_PC;   // size 0x5c

typedef struct cut_memory_instance_pc_s {
    u8 unknown_00[0x58];             // 0x00
    CUT_MEMORY_SCENE_PC *scene;      // 0x58
    u8 unknown_5c[0x2c];             // 0x5c
    u32 flags_88;                    // 0x88
    u8 unknown_8c[0x54];             // 0x8c
    f32 accumulated_stream_duration; // 0xe0
    u8 unknown_e4[0xc];              // 0xe4
    u32 allocation_size;             // 0xf0
} CUT_MEMORY_INSTANCE_PC;            // size 0xf4

typedef struct cut_memory_entry_pc_s {
    void *handle;               // 0x0
    CUT_MEMORY_SCENE_PC *scene; // 0x4
    f32 accumulated_duration;   // 0x8
    u8 flags;                   // 0xc
    u8 unknown_0d[3];           // 0xd
} CUT_MEMORY_ENTRY_PC;          // size 0x10

typedef struct cut_memory_vertex_pc_s {
    f32 x;              // 0x0
    f32 y;              // 0x4
    f32 z;              // 0x8
    u32 colour;         // 0xc
} CUT_MEMORY_VERTEX_PC; // size 0x10

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
__declspec(allocate(".bss$DefragGetInstFn")) CUT_MEMORY_INSTANCE_PC *(*DefragGetInstFn)(void *) = NULL;
#pragma section(".bss$DefragCutSceneList", read, write)
__declspec(allocate(".bss$DefragCutSceneList")) CUT_MEMORY_ENTRY_PC *DefragCutSceneList = NULL;
#pragma section(".bss$DefragCreateInstFn", read, write)
__declspec(allocate(".bss$DefragCreateInstFn"))
CUT_MEMORY_INSTANCE_PC *(*DefragCreateInstFn)(void *, CUT_MEMORY_SCENE_PC *, VARIPTR *) = NULL;
#pragma section(".bss$DefragInstDestroyedFn", read, write)
__declspec(allocate(".bss$DefragInstDestroyedFn")) void (*DefragInstDestroyedFn)(void *) = NULL;

void instNuGCutSceneResetCleanUp(void *list_storage, void *cut_base, void *cut_end, void *inst_base, void *inst_end,
                                 CUT_MEMORY_INSTANCE_PC *(*get_inst)(void *),
                                 CUT_MEMORY_INSTANCE_PC *(*create_inst)(void *, CUT_MEMORY_SCENE_PC *, VARIPTR *),
                                 void (*inst_destroyed)(void *)) {
    DefragCutSceneList = (CUT_MEMORY_ENTRY_PC *)(((u32)list_storage + 3) & ~3);
    DefragCutSceneListBase = DefragCutSceneList;
    DefragCutSceneBaseMem = cut_base;
    DefragCutSceneEndMem = cut_end;
    DefragInstBaseMem = inst_base;
    DefragInstEndMem = inst_end;
    DefragGetInstFn = get_inst;
    DefragCreateInstFn = create_inst;
    DefragInstDestroyedFn = inst_destroyed;
    DefragCutSceneListSize = 0;
    if (list_storage == NULL || cut_base == NULL) {
        ((void (*)(void))CutSceneLoadDiagnosticPc)();
    }
    if (get_inst == NULL || create_inst == NULL || inst_destroyed == NULL) {
        ((void (*)(void))CutSceneLoadDiagnosticPc)();
    }
}

void instNuGCutSceneAddCleanUpItem(void *handle, i32 enabled) {
    CUT_MEMORY_INSTANCE_PC *instance = DefragGetInstFn(handle);
    CUT_MEMORY_ENTRY_PC *entry;
    if ((instance->flags_88 & 2) != 0) {
        ((void (*)(void))CutSceneLoadDiagnosticPc)();
    }
    entry = DefragCutSceneList++;
    entry->handle = handle;
    entry->scene = instance->scene;
    entry->accumulated_duration = instance->accumulated_stream_duration;
    entry->flags = enabled ? 7 : 0;
    ++DefragCutSceneListSize;
}

static void CutSceneSetPrimitiveColourPc(i32 colour) {
    i32 converted;
    if (NuPrimNeedsOverbrightening != 0) {
        converted = ((colour >> 16) & 0xff) + ((colour & 0xff) << 16) + (colour & 0xff00) + (colour & 0xff000000u);
    } else {
        converted =
            ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000u);
    }
    ((CUT_MEMORY_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour = converted;
}

void DisplayCutSceneMemory(void) {
    CUT_MEMORY_ENTRY_PC *entry;
    CUT_MEMORY_ENTRY_PC *end;
    CUT_MEMORY_INSTANCE_PC *instance;
    CUT_MEMORY_SCENE_PC *scene;
    u32 start;
    u32 finish;
    f32 scale;
    f32 start_x;
    f32 end_x;
    if (DefragCutSceneEndMem == NULL || DefragCutSceneListSize == 0 || DefragCutSceneBaseMem == NULL) {
        return;
    }
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
            if ((entry->flags & 4) == 0) {
                continue;
            }
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
            if ((entry->flags & 4) == 0) {
                continue;
            }
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
            if ((entry->flags & 4) == 0) {
                continue;
            }
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

typedef struct cut_script_node_pc_s CUT_SCRIPT_NODE_PC;

struct cut_script_node_pc_s {
    CUT_SCRIPT_NODE_PC *next; // 0x00
    i32 type;                 // 0x04

    union {          // 0x08
        char *name;  // 0x08
        i32 param;   // 0x08
        u8 bytes[4]; // 0x08
    } a;

    union {                        // 0x0c
        char *name;                // 0x0c
        i32 value;                 // 0x0c
        CUT_SCRIPT_NODE_PC *block; // 0x0c
    } b;

    CUT_SCRIPT_NODE_PC *c; // 0x10
    CUT_SCRIPT_NODE_PC *d; // 0x14
}; // size 0x18

typedef struct cut_script_pc_s CUT_SCRIPT_PC;

struct cut_script_pc_s {
    CUT_SCRIPT_PC *next;       // 0x0
    char *name;                // 0x4
    CUT_SCRIPT_NODE_PC *nodes; // 0x8
}; // size 0xc

#define CUT_SCRIPT_NODE_ALLOC(node, buffer, size)                                                                      \
    (node) = (CUT_SCRIPT_NODE_PC *)(buffer)->void_ptr;                                                                 \
    (buffer)->addr += (size);                                                                                          \
    memset((node), 0, (size))

static CUT_SCRIPT_NODE_PC *CutScriptNodeAllocPc(i32 type, VARIPTR *buffer, CUT_SCRIPT_NODE_PC *prev,
                                                CUT_SCRIPT_NODE_PC **head) {
    CUT_SCRIPT_NODE_PC *node = NULL;
    buffer->addr = (buffer->addr + 0xf) & ~0xfU;
    switch (type) {
        case 0:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0xc);
            break;
        case 1:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0x10);
            break;
        case 2:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0x10);
            break;
        case 3:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0x10);
            break;
        case 4:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0xc);
            break;
        case 5:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0x10);
            break;
        case 6:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0xc);
            break;
        case 7:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0x10);
            break;
        case 8:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0x18);
            break;
        case 9:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0x10);
            break;
        case 10:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0x14);
            break;
        case 11:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0xc);
            break;
        case 12:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0xc);
            break;
        case 13:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0xc);
            break;
        case 14:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0x10);
            break;
    }
    node->type = type;
    if (prev != NULL) {
        prev->next = node;
    } else {
        *head = node;
    }
    return node;
}

#define CUT_SCRIPT_READ_WORD(fp, buffer, field)                                                                        \
    NuFParGetWord(fp);                                                                                                 \
    (field) = (buffer)->char_ptr;                                                                                      \
    (buffer)->addr += strlen((fp)->word_buffer_ptr) + 1;                                                               \
    sprintf((field), (fp)->word_buffer_ptr)

CUT_SCRIPT_NODE_PC *CutScriptParseBlockPc(NUFPAR_PC *fp, VARIPTR *buffer, i32 unused) {
    CUT_SCRIPT_NODE_PC *head = NULL;
    CUT_SCRIPT_NODE_PC *prev = NULL;
    CUT_SCRIPT_NODE_PC *node;
    while (NuFParGetLine(fp) != 0) {
        NuFParGetWord(fp);
        if (NuStrICmp(fp->word_buffer_ptr, "ENDSCRIPT") == 0) {
            break;
        }
        if (NuStrICmp(fp->word_buffer_ptr, "STARTCUTSCENEINST") == 0) {
            node = CutScriptNodeAllocPc(0, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
        } else if (NuStrICmp(fp->word_buffer_ptr, "PLAYCUTSCENEINST") == 0) {
            node = CutScriptNodeAllocPc(1, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
            NuFParGetWord(fp);
            if (NuStrICmp(fp->word_buffer_ptr, "forward") == 0) {
                node->b.value = 1;
            } else if (NuStrICmp(fp->word_buffer_ptr, "backward") == 0) {
                node->b.value = 0;
            } else {
                ((void (*)(void))CutSceneMissingObjectDiagnosticPc)();
            }
        } else if (NuStrICmp(fp->word_buffer_ptr, "CHAINCUTSCENEINST") == 0) {
            node = CutScriptNodeAllocPc(2, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
            CUT_SCRIPT_READ_WORD(fp, buffer, node->b.name);
        } else if (NuStrICmp(fp->word_buffer_ptr, "REPEATCUTSCENEINST") == 0) {
            node = CutScriptNodeAllocPc(3, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
            node->b.value = NuFParGetInt(fp);
        } else if (NuStrICmp(fp->word_buffer_ptr, "RESETCUTSCENEINST") == 0) {
            node = CutScriptNodeAllocPc(4, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
        } else if (NuStrICmp(fp->word_buffer_ptr, "PAUSECUTSCENEINST") == 0) {
            node = CutScriptNodeAllocPc(12, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
        } else if (NuStrICmp(fp->word_buffer_ptr, "UNPAUSECUTSCENEINST") == 0) {
            node = CutScriptNodeAllocPc(13, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
        } else if (NuStrICmp(fp->word_buffer_ptr, "SETTRIGGERFLAG") == 0) {
            node = CutScriptNodeAllocPc(5, buffer, prev, &head);
            prev = node;
            node->a.bytes[0] = (u8)NuFParGetInt(fp);
            CUT_SCRIPT_READ_WORD(fp, buffer, node->b.name);
            NuFParGetWord(fp);
            node->a.bytes[1] = NuStrICmp(fp->word_buffer_ptr, "ON") == 0;
        } else if (NuStrICmp(fp->word_buffer_ptr, "RESETTRIGGER") == 0) {
            node = CutScriptNodeAllocPc(6, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
        } else if (NuStrICmp(fp->word_buffer_ptr, "SETVISIBILITY") == 0) {
            node = CutScriptNodeAllocPc(7, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->b.name);
            NuFParGetWord(fp);
            node->a.bytes[0] = NuStrICmp(fp->word_buffer_ptr, "ON") == 0;
        } else if (NuStrICmp(fp->word_buffer_ptr, "IFPARAM") == 0) {
            node = CutScriptNodeAllocPc(8, buffer, prev, &head);
            prev = node;
            node->a.param = NuFParGetInt(fp);
            node->b.value = NuFParGetInt(fp);
            node->c = CutScriptParseBlockPc(fp, buffer, unused);
            if (NuStrICmp(fp->word_buffer_ptr, "ELSE") == 0) {
                node->d = CutScriptParseBlockPc(fp, buffer, unused);
            }
        } else if (NuStrICmp(fp->word_buffer_ptr, "SETPARAM") == 0) {
            node = CutScriptNodeAllocPc(9, buffer, prev, &head);
            prev = node;
            node->a.param = NuFParGetInt(fp);
            node->b.value = NuFParGetInt(fp);
        } else if (NuStrICmp(fp->word_buffer_ptr, "IFTRIGGERPULLED") == 0) {
            node = CutScriptNodeAllocPc(10, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
            node->b.block = CutScriptParseBlockPc(fp, buffer, unused);
            if (NuStrICmp(fp->word_buffer_ptr, "ELSE") == 0) {
                node->c = CutScriptParseBlockPc(fp, buffer, unused);
            }
        } else if (NuStrICmp(fp->word_buffer_ptr, "IFTRIGGERNOTPULLED") == 0) {
            node = CutScriptNodeAllocPc(10, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
            node->c = CutScriptParseBlockPc(fp, buffer, unused);
            if (NuStrICmp(fp->word_buffer_ptr, "ELSE") == 0) {
                node->b.block = CutScriptParseBlockPc(fp, buffer, unused);
            }
        } else if (NuStrICmp(fp->word_buffer_ptr, "ENABLETRIGGER") == 0) {
            node = CutScriptNodeAllocPc(11, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
        } else if (NuStrICmp(fp->word_buffer_ptr, "ELSE") == 0) {
            break;
        } else if (NuStrICmp(fp->word_buffer_ptr, "ENDIF") == 0) {
            break;
        } else if (NuStrICmp(fp->word_buffer_ptr, "APPCOMMAND") == 0) {
            node = CutScriptNodeAllocPc(14, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
            CUT_SCRIPT_READ_WORD(fp, buffer, node->b.name);
        }
    }
    return head;
}

static CUT_SCRIPT_PC *CutScriptReadPc(NUFPAR_PC *fp, VARIPTR *buffer, i32 unused) {
    CUT_SCRIPT_PC *script;
    buffer->addr = (buffer->addr + 0xf) & ~0xfU;
    script = buffer->void_ptr;
    buffer->addr += sizeof(CUT_SCRIPT_PC);
    memset(script, 0, sizeof(CUT_SCRIPT_PC));
    CUT_SCRIPT_READ_WORD(fp, buffer, script->name);
    script->nodes = CutScriptParseBlockPc(fp, buffer, unused);
    return script;
}

CUT_SCRIPT_PC *CutScriptLoadPc(char *file, VARIPTR *buffer, i32 unused) {
    CUT_SCRIPT_PC *head = NULL;
    CUT_SCRIPT_PC *prev = NULL;
    CUT_SCRIPT_PC *script;
    NUFPAR_PC *fp = NuFParCreate(file);
    if (fp != NULL) {
        NuFParGetWord(fp);
        while (NuFParGetLine(fp) != 0) {
            NuFParGetWord(fp);
            if (NuStrICmp(fp->word_buffer_ptr, "SCRIPT") != 0) {
                continue;
            }
            script = CutScriptReadPc(fp, buffer, unused);
            if (script == NULL) {
                continue;
            }
            if (prev != NULL) {
                prev->next = script;
            } else {
                head = script;
            }
            prev = script;
        }
        NuFParDestroy(fp);
        buffer->addr = (buffer->addr + 0xf) & ~0xfU;
    }
    return head;
}

typedef struct cut_trigger_shape_pc_s {
    i32 type;           // 0x0
    void *data;         // 0x4
} CUT_TRIGGER_SHAPE_PC; // size 0x8

typedef struct cut_trigger_sphere_pc_s {
    NUVEC centre;
    f32 radius;
} CUT_TRIGGER_SPHERE_PC;

typedef struct cut_trigger_box_pc_s {
    NUMTX matrix;     // 0x00
    NUVEC min;        // 0x40
    NUVEC max;        // 0x4c
} CUT_TRIGGER_BOX_PC; // size 0x58

typedef struct cut_trigger_cylinder_pc_s {
    NUVEC base;            // 0x00
    f32 height;            // 0x0c
    f32 radius;            // 0x10
} CUT_TRIGGER_CYLINDER_PC; // size 0x14

typedef struct cut_trigger_entry_pc_s {
    char *name;                   // 0x00
    i32 kind;                     // 0x04
    i16 threshold;                // 0x08
    u8 initial_state;             // 0x0a
    u8 unknown_0b;                // 0x0b
    u8 flags;                     // 0x0c
    u8 unknown_0d[3];             // 0x0d
    f32 radius;                   // 0x10
    NUVEC min;                    // 0x14
    NUVEC max;                    // 0x20
    i16 shape_count;              // 0x2c
    i16 scene_instance;           // 0x2e
    CUT_TRIGGER_SHAPE_PC *shapes; // 0x30
} CUT_TRIGGER_ENTRY_PC;           // size 0x34

typedef struct cut_trigger_file_pc_s {
    i32 version;
    i32 delta;
    i32 count;
    CUT_TRIGGER_ENTRY_PC *entries;
} CUT_TRIGGER_FILE_PC;

typedef struct cut_trigger_state_pc_s {
    i16 count;          // 0x0
    u8 state;           // 0x2
    u8 pulled;          // 0x3
} CUT_TRIGGER_STATE_PC; // size 0x4

typedef struct cut_trigger_scene_instance_pc_s {
    NUMTX matrix;                // 0x00
    i32 unknown_40;              // 0x40
    u32 flags;                   // 0x44
    NUMTX *animated_matrix;      // 0x48
    u32 unknown_4c;              // 0x4c
} CUT_TRIGGER_SCENE_INSTANCE_PC; // size 0x50

typedef struct cut_trigger_set_pc_s CUT_TRIGGER_SET_PC;

struct cut_trigger_set_pc_s {
    CUT_TRIGGER_SET_PC *next;     // 0x00
    CUT_TRIGGER_SET_PC *prev;     // 0x04
    CUT_TRIGGER_FILE_PC *file;    // 0x08
    CUT_TRIGGER_STATE_PC *states; // 0x0c
    NUGSCN *scene;                // 0x10
    CUT_SCRIPT_PC *scripts;       // 0x14
    i8 params[32];                // 0x18
    u32 flags;                    // 0x38
}; // size 0x3c

#pragma section(".bss$CutTriggerSetListPc", read, write)
__declspec(allocate(".bss$CutTriggerSetListPc")) CUT_TRIGGER_SET_PC *CutTriggerSetListPc = NULL;
#pragma section(".bss$CutTriggerPendingSetListPc", read, write)
__declspec(allocate(".bss$CutTriggerPendingSetListPc")) CUT_TRIGGER_SET_PC *CutTriggerPendingSetListPc = NULL;

typedef struct cut_app_command_pc_s {
    char *name;         // 0x0
    void (*fn)(char *); // 0x4
} CUT_APP_COMMAND_PC;   // size 0x8

#pragma section(".bss$CutScriptAppCommandsPc", read, write)
__declspec(allocate(".bss$CutScriptAppCommandsPc")) CUT_APP_COMMAND_PC *CutScriptAppCommandsPc = NULL;

void CutScriptSetAppCommandsPc(CUT_APP_COMMAND_PC *commands) {
    CutScriptAppCommandsPc = commands;
}

void CutTriggerClearPendingPc(void) {
    CutTriggerPendingSetListPc = NULL;
}

void CutTriggerClearListsPc(void) {
    CutTriggerPendingSetListPc = NULL;
    CutTriggerSetListPc = NULL;
}

void CutTriggerActivatePendingPc(void) {
    CUT_TRIGGER_SET_PC *set = CutTriggerPendingSetListPc;
    CUT_TRIGGER_SET_PC *list;
    CUT_TRIGGER_SET_PC *current;
    if (set != NULL) {
        list = CutTriggerSetListPc;
        do {
            current = set;
            set = set->next;
            current->next = list;
            if (list != NULL) {
                list->prev = current;
            }
            current->prev = NULL;
            list = current;
        } while (set != NULL);
        CutTriggerSetListPc = list;
        CutTriggerPendingSetListPc = set;
    }
}

CUT_TRIGGER_SET_PC *CutTriggerSetCreatePc(CUT_TRIGGER_FILE_PC *file, NUGSCN *scene, CUT_SCRIPT_PC *scripts,
                                          VARIPTR *buffer, VARIPTR *end, i32 pending) {
    CUT_TRIGGER_SET_PC *set = NULL;
    CUT_TRIGGER_SET_PC *list;
    i32 i;
    (void)end;
    if (file != NULL && scene != NULL && scripts != NULL) {
        buffer->addr = (buffer->addr + 0xf) & ~0xfU;
        set = buffer->void_ptr;
        buffer->addr += sizeof(CUT_TRIGGER_SET_PC);
        memset(set, 0, sizeof(CUT_TRIGGER_SET_PC));
        set->prev = NULL;
        if (pending != 0) {
            list = CutTriggerPendingSetListPc;
            set->next = list;
            if (list != NULL) {
                list->prev = set;
            }
            CutTriggerPendingSetListPc = set;
        } else {
            list = CutTriggerSetListPc;
            set->next = list;
            if (list != NULL) {
                list->prev = set;
            }
            CutTriggerSetListPc = set;
        }
        set->scene = scene;
        set->file = file;
        set->scripts = scripts;
        set->states = buffer->void_ptr;
        buffer->addr += file->count * sizeof(CUT_TRIGGER_STATE_PC);
        memset(set->states, 0, file->count * sizeof(CUT_TRIGGER_STATE_PC));
        for (i = 0; i < file->count; ++i) {
            set->states[i].state = file->entries[i].initial_state;
        }
    }
    return set;
}

void CutTriggerSetUnlinkPc(CUT_TRIGGER_SET_PC *set) {
    if (set->next != NULL) {
        set->next->prev = set->prev;
    }
    if (set->prev != NULL) {
        set->prev->next = set->next;
    } else {
        CutTriggerSetListPc = set->next;
    }
}

void CutTriggerSetEnablePc(CUT_TRIGGER_SET_PC *set) {
    set->flags &= ~1U;
}

void CutTriggerSetDisablePc(CUT_TRIGGER_SET_PC *set) {
    set->flags |= 1;
}

CUT_TRIGGER_FILE_PC *CutTriggerFileLoadPc(char *path, VARIPTR *buffer, VARIPTR *end) {
    CUT_TRIGGER_FILE_PC *file;
    CUT_TRIGGER_ENTRY_PC *entry;
    CUT_TRIGGER_SHAPE_PC *shape;
    i32 size;
    i32 i;
    i32 j;
    buffer->addr = (buffer->addr + 0xf) & ~0xfU;
    size = NuFileLoadBuffer(path, buffer->void_ptr, end->addr - buffer->addr);
    if (size == 0) {
        return NULL;
    }
    file = buffer->void_ptr;
    buffer->addr += size;
    if (file->version != 1) {
        CutSceneLoadDiagnosticPc(path, file->version);
    }
    file->delta = (i32)file - file->delta;
    file->entries = file->entries != NULL ? (CUT_TRIGGER_ENTRY_PC *)((u32)file->entries + file->delta) : NULL;
    if (file->entries != NULL) {
        for (i = 0; i < file->count; ++i) {
            entry = &file->entries[i];
            entry->name = entry->name != NULL ? (char *)((u32)entry->name + file->delta) : NULL;
            entry->shapes = entry->shapes != NULL ? (CUT_TRIGGER_SHAPE_PC *)((u32)entry->shapes + file->delta) : NULL;
            if (entry->shapes != NULL) {
                for (j = 0; j < entry->shape_count; ++j) {
                    shape = &entry->shapes[j];
                    shape->data = shape->data != NULL ? (void *)((u32)shape->data + file->delta) : NULL;
                }
            }
        }
    }
    return file;
}

void CutTriggerSysNullPc(void) {
}

void CutTriggerSetFlagPc(CUT_TRIGGER_SET_PC *set, char *name, i32 flag, i32 on) {
    CUT_TRIGGER_FILE_PC *file = set->file;
    i32 i;
    u8 mask;
    for (i = 0; i < file->count; ++i) {
        if (NuStrICmp(file->entries[i].name, name) == 0) {
            mask = (u8)(1 << (flag - 1));
            if (on != 0) {
                set->states[i].state |= mask;
            } else {
                set->states[i].state &= ~mask;
            }
            return;
        }
    }
}

void CutTriggerEnablePc(CUT_TRIGGER_SET_PC *set, char *name) {
    CUT_TRIGGER_FILE_PC *file = set->file;
    i32 i;
    for (i = 0; i < file->count; ++i) {
        if (NuStrICmp(file->entries[i].name, name) == 0) {
            set->states[i].state = 0xff;
            return;
        }
    }
}

i32 CutTriggerIsPulledPc(CUT_TRIGGER_SET_PC *set, char *name) {
    CUT_TRIGGER_FILE_PC *file = set->file;
    i32 i;
    for (i = 0; i < file->count; ++i) {
        if (NuStrICmp(file->entries[i].name, name) == 0) {
            return set->states[i].pulled & 1;
        }
    }
    return 0;
}

void CutTriggerPullPc(CUT_TRIGGER_SET_PC *set, char *name) {
    CUT_TRIGGER_FILE_PC *file = set->file;
    i32 i;
    for (i = 0; i < file->count; ++i) {
        if (NuStrICmp(file->entries[i].name, name) == 0) {
            set->states[i].pulled |= 1;
            return;
        }
    }
}

void CutTriggerResetPc(CUT_TRIGGER_SET_PC *set, char *name) {
    CUT_TRIGGER_FILE_PC *file;
    i32 i;
    if (set == NULL) {
        return;
    }
    file = set->file;
    for (i = 0; i < file->count; ++i) {
        if (NuStrICmp(file->entries[i].name, name) == 0) {
            set->states[i].state = file->entries[i].initial_state;
            set->states[i].pulled &= ~1;
            return;
        }
    }
}

static f32 CutTriggerFabsPc(f32 value) {
    i32 bits = *(i32 *)&value;
    bits &= 0x7fffffff;
    return *(f32 *)&bits;
}

i32 CutTriggerEntryTestPc(CUT_TRIGGER_ENTRY_PC *entry, NUVEC *pos, f32 radius) {
    NUVEC extent;
    NUVEC local_pos;
    NUVEC box_extent;
    NUVEC delta;
    CUT_TRIGGER_SHAPE_PC *shape;
    CUT_TRIGGER_SPHERE_PC *sphere;
    CUT_TRIGGER_BOX_PC *box;
    CUT_TRIGGER_CYLINDER_PC *cylinder;
    f32 dist;
    i32 i;
    if (pos->x - radius > entry->max.x) {
        return 0;
    }
    if (pos->x + radius < entry->min.x) {
        return 0;
    }
    if (pos->y - radius > entry->max.y) {
        return 0;
    }
    if (pos->y + radius < entry->min.y) {
        return 0;
    }
    if (pos->z - radius > entry->max.z) {
        return 0;
    }
    if (pos->z + radius < entry->min.z) {
        return 0;
    }
    extent.x = extent.y = extent.z = radius;
    for (i = 0; i < entry->shape_count; ++i) {
        shape = &entry->shapes[i];
        switch (shape->type) {
            case 1:
                sphere = shape->data;
                dist = NuVecDistSqr(pos, &sphere->centre, &delta);
                if (dist < (sphere->radius + radius) * (sphere->radius + radius)) {
                    return 1;
                }
                break;
            case 2:
                box = shape->data;
                NuVecMtxTransform(&local_pos, pos, &box->matrix);
                NuVecMtxTransform(&box_extent, &extent, &box->matrix);
                NuVecSub(&box_extent, &box_extent, &box->matrix.translation);
                box_extent.x = CutTriggerFabsPc(box_extent.x);
                box_extent.y = CutTriggerFabsPc(box_extent.y);
                box_extent.z = CutTriggerFabsPc(box_extent.z);
                if (local_pos.x - box_extent.x > box->max.x || local_pos.x + box_extent.x < box->min.x ||
                    local_pos.y - box_extent.y > box->max.y || local_pos.y + box_extent.y < box->min.y ||
                    local_pos.z - box_extent.z > box->max.z || local_pos.z + box_extent.z < box->min.z) {
                    break;
                }
                return 1;
            case 3:
                cylinder = shape->data;
                if (pos->y < cylinder->base.y - radius || pos->y > cylinder->height + cylinder->base.y + radius) {
                    break;
                }
                dist = (pos->x - cylinder->base.x) * (pos->x - cylinder->base.x) +
                       (pos->z - cylinder->base.z) * (pos->z - cylinder->base.z);
                if (dist < (cylinder->radius + radius) * (cylinder->radius + radius)) {
                    return 1;
                }
                break;
        }
    }
    return 0;
}

i32 CutTriggerEntryTestMtxPc(CUT_TRIGGER_ENTRY_PC *entry, NUMTX *mtx, NUVEC *pos, f32 radius) {
    NUMTX inverse;
    NUVEC delta;
    NUVEC local_pos;
    NUVEC extent;
    NUVEC centre;
    NUVEC box_pos;
    NUVEC box_extent;
    CUT_TRIGGER_SHAPE_PC *shape;
    CUT_TRIGGER_SPHERE_PC *sphere;
    CUT_TRIGGER_BOX_PC *box;
    CUT_TRIGGER_CYLINDER_PC *cylinder;
    f32 dist;
    i32 i;
    delta.x = pos->x - mtx->m30;
    delta.y = pos->y - mtx->m31;
    delta.z = pos->z - mtx->m32;
    if (delta.x * delta.x + delta.y * delta.y + delta.z * delta.z >
        (entry->radius + radius) * (entry->radius + radius)) {
        return 0;
    }
    if ((entry->flags & 1) != 0) {
        NuMtxInvH(&inverse, mtx);
        NuVecMtxTransform(&local_pos, pos, &inverse);
        extent.x = extent.y = extent.z = radius;
        NuVecMtxTransform(&extent, &extent, &inverse);
        NuVecSub(&extent, &extent, &inverse.translation);
        extent.x = CutTriggerFabsPc(extent.x);
        extent.y = CutTriggerFabsPc(extent.y);
        extent.z = CutTriggerFabsPc(extent.z);
    }
    for (i = 0; i < entry->shape_count; ++i) {
        shape = &entry->shapes[i];
        switch (shape->type) {
            case 1:
                sphere = shape->data;
                NuVecMtxTransform(&centre, &sphere->centre, mtx);
                dist = NuVecDistSqr(pos, &centre, &delta);
                if (dist < (sphere->radius + radius) * (sphere->radius + radius)) {
                    return 1;
                }
                break;
            case 2:
                box = shape->data;
                NuVecMtxTransform(&box_pos, &local_pos, &box->matrix);
                NuVecMtxTransform(&box_extent, &extent, &box->matrix);
                NuVecSub(&box_extent, &box_extent, &box->matrix.translation);
                box_extent.x = CutTriggerFabsPc(box_extent.x);
                box_extent.y = CutTriggerFabsPc(box_extent.y);
                box_extent.z = CutTriggerFabsPc(box_extent.z);
                if (box_pos.x - box_extent.x > box->max.x || box_pos.x + box_extent.x < box->min.x ||
                    box_pos.y - box_extent.y > box->max.y || box_pos.y + box_extent.y < box->min.y ||
                    box_pos.z - box_extent.z > box->max.z || box_pos.z + box_extent.z < box->min.z) {
                    break;
                }
                return 1;
            case 3:
                cylinder = shape->data;
                NuVecMtxTransform(&centre, &cylinder->base, mtx);
                if (pos->y < centre.y - radius || pos->y > centre.y + cylinder->height + radius) {
                    break;
                }
                dist = (pos->x - centre.x) * (pos->x - centre.x) + (pos->z - centre.z) * (pos->z - centre.z);
                if (dist < (cylinder->radius + radius) * (cylinder->radius + radius)) {
                    return 1;
                }
                break;
        }
    }
    return 0;
}

i32 CutTriggerSetTestPc(CUT_TRIGGER_SET_PC *set, char *name, NUVEC *pos, f32 radius, i32 include_hidden) {
    CUT_TRIGGER_FILE_PC *file = set->file;
    NUGSCN *gscene;
    CUT_TRIGGER_ENTRY_PC *entry;
    CUT_TRIGGER_SCENE_INSTANCE_PC *instance;
    i32 hit;
    i32 i;
    for (i = 0; i < file->count; ++i) {
        if (NuStrICmp(file->entries[i].name, name) == 0) {
            entry = &file->entries[i];
            gscene = set->scene;
            switch (entry->kind) {
                case 1:
                    if ((entry->flags & 8) != 0 && include_hidden == 0) {
                        break;
                    }
                    if (entry->scene_instance >= 0) {
                        instance = &((CUT_TRIGGER_SCENE_INSTANCE_PC *)gscene->instances)[entry->scene_instance];
                        if ((instance->flags & 1) == 0) {
                            break;
                        }
                        if (instance->animated_matrix != NULL) {
                            hit = CutTriggerEntryTestMtxPc(entry, instance->animated_matrix, pos, radius);
                        } else {
                            hit = CutTriggerEntryTestMtxPc(entry, &instance->matrix, pos, radius);
                        }
                    } else {
                        hit = CutTriggerEntryTestPc(entry, pos, radius);
                    }
                    if (hit != 0) {
                        return 1;
                    }
                    break;
            }
        }
    }
    return 0;
}

/* Empty on PC (a bare RET at the start of this unit in retail). It lives in this
   unit and takes variable arguments: once a call passes a pointer through the
   ellipsis (CutScriptExecutePc), VC8 keeps every later call and knows the body
   preserves eax/ecx/edx, which CutTriggerFirePc and CutTriggerUpdatePc rely on.
   Calls compiled before the definition stay ordinary external calls
   (CutScriptParseBlockPc), so it is defined here rather than at the top. */
void CutSceneMissingObjectDiagnosticPc(char *format, ...) {
}

#define CUT_SCRIPT_DIAG0() ((void (*)(void))CutSceneMissingObjectDiagnosticPc)()
#define CUT_SCRIPT_DIAG2(a, b) CutSceneMissingObjectDiagnosticPc((char *)(a), (b))

void CutScriptExecutePc(CUT_SCRIPT_NODE_PC *node, NUGSCN *scene, CUT_TRIGGER_SET_PC *set) {
    INSTNUGCUTSCENE *instance;
    INSTNUGCUTSCENE *chain;
    NUSPECIALHANDLE special;
    CUT_APP_COMMAND_PC *command;
    i32 count;
    i32 done;
    for (; node != NULL; node = node->next) {
        done = 0;
        switch (node->type) {
            case 0:
                instance = instNuGCutSceneFindActivePc(node->a.name);
                if (instance != NULL) {
                    instNuGCutSceneStart(instance);
                    done = 1;
                }
                CutSceneMissingObjectDiagnosticPc(node->a.name);
                break;
            case 1:
                instance = instNuGCutSceneFindActivePc(node->a.name);
                if (instance != NULL) {
                    instNuGCutScenePlay(instance, node->b.value);
                    done = 1;
                }
                if (node->b.value != 0) {
                    CutSceneMissingObjectDiagnosticPc(node->a.name);
                } else {
                    CutSceneMissingObjectDiagnosticPc(node->a.name);
                }
                break;
            case 2:
                instance = instNuGCutSceneFindActivePc(node->a.name);
                chain = instNuGCutSceneFindActivePc(node->b.name);
                if (instance != NULL && chain != NULL) {
                    if (instance->scene == NULL) {
                        ((void (*)(void))CutSceneLoadDiagnosticPc)();
                    }
                    instance->chained_instance = chain;
                    done = 1;
                }
                CUT_SCRIPT_DIAG2(node->a.name, node->b.name);
                break;
            case 3:
                instance = instNuGCutSceneFindActivePc(node->a.name);
                if (instance != NULL) {
                    count = node->b.value;
                    if (count > 31) {
                        ((void (*)(i32))CutSceneMissingObjectDiagnosticPc)(count);
                        count = 31;
                    }
                    instance->flags_88 ^= ((u32)(count << 13) ^ instance->flags_88) & 0x3e000;
                    done = 1;
                }
                CUT_SCRIPT_DIAG2(node->a.name, node->b.value);
                break;
            case 4:
                instance = instNuGCutSceneFindActivePc(node->a.name);
                if (instance != NULL) {
                    instNuGCutSceneReset(instance);
                    done = 1;
                }
                CutSceneMissingObjectDiagnosticPc(node->a.name);
                break;
            case 5:
                if (set != NULL) {
                    CutTriggerSetFlagPc(set, node->b.name, (i8)node->a.bytes[0], (i8)node->a.bytes[1]);
                    done = 1;
                }
                if (node->a.bytes[1] != 0) {
                    CUT_SCRIPT_DIAG2((i8)node->a.bytes[0], node->b.name);
                } else {
                    CUT_SCRIPT_DIAG2((i8)node->a.bytes[0], node->b.name);
                }
                break;
            case 6:
                if (set == NULL) {
                    CutSceneMissingObjectDiagnosticPc(node->a.name);
                    break;
                }
                CutTriggerResetPc(set, node->a.name);
                done = 1;
                CutSceneMissingObjectDiagnosticPc(node->a.name);
                break;
            case 7:
                if (scene != NULL && NuSpecialFind(scene, &special, node->b.name, 1) != 0) {
                    if (node->a.bytes[0] != 0) {
                        ((NUSPECIAL_LEGACY_PC *)special.special)->instance->flags |= 1;
                    } else {
                        ((NUSPECIAL_LEGACY_PC *)special.special)->instance->flags &= ~1;
                    }
                    done = 1;
                }
                CutSceneMissingObjectDiagnosticPc(node->b.name);
                break;
            case 8:
                if (node->a.param < 32 && set->params[node->a.param] == node->b.value) {
                    CUT_SCRIPT_DIAG2(node->a.param, node->b.value);
                    CutScriptExecutePc(node->c, scene, set);
                } else {
                    CUT_SCRIPT_DIAG2(node->a.param, node->b.value);
                    CutScriptExecutePc(node->d, scene, set);
                }
                break;
            case 9:
                if (node->a.param < 32) {
                    set->params[node->a.param] = (i8)node->b.value;
                    done = 1;
                }
                CUT_SCRIPT_DIAG2(node->a.param, node->b.value);
                break;
            case 10:
                if (CutTriggerIsPulledPc(set, node->a.name) != 0) {
                    CutSceneMissingObjectDiagnosticPc(node->a.name);
                    CutScriptExecutePc(node->b.block, scene, set);
                } else {
                    CutSceneMissingObjectDiagnosticPc(node->a.name);
                    CutScriptExecutePc(node->c, scene, set);
                }
                break;
            case 11:
                CutTriggerEnablePc(set, node->a.name);
                CutSceneMissingObjectDiagnosticPc(node->a.name);
                continue;
            case 12:
                instance = instNuGCutSceneFindActivePc(node->a.name);
                if (instance != NULL) {
                    instance->flags_88 |= 4;
                    done = 1;
                }
                CutSceneMissingObjectDiagnosticPc(node->a.name);
                break;
            case 13:
                instance = instNuGCutSceneFindActivePc(node->a.name);
                if (instance != NULL) {
                    instance->flags_88 &= ~4;
                    done = 1;
                }
                CutSceneMissingObjectDiagnosticPc(node->a.name);
                break;
            case 14:
                command = CutScriptAppCommandsPc;
                CutSceneMissingObjectDiagnosticPc(command->name);
                for (; command != NULL && command->name != NULL; ++command) {
                    if (NuStrICmp(node->a.name, command->name) == 0) {
                        if (command->fn != NULL) {
                            command->fn(node->b.name);
                            done = 1;
                        }
                        break;
                    }
                }
                if (done) {
                    continue;
                }
                break;
        }
        if (done == 0) {
            CUT_SCRIPT_DIAG0();
        }
    }
}

i32 CutScriptRunPc(CUT_SCRIPT_PC *scripts, char *name, NUGSCN *scene, CUT_TRIGGER_SET_PC *set) {
    CUT_APP_COMMAND_PC *command = CutScriptAppCommandsPc;
    if (NuStrNICmp(name, "Fn", 2) == 0) {
        for (; command != NULL && command->name != NULL; ++command) {
            if (NuStrICmp(name + 2, command->name) == 0) {
                CutSceneMissingObjectDiagnosticPc(name);
                if (command->fn != NULL) {
                    command->fn(NULL);
                }
                return 0;
            }
        }
    } else {
        for (; scripts != NULL; scripts = scripts->next) {
            if (NuStrICmp(scripts->name, name) == 0) {
                CutSceneMissingObjectDiagnosticPc(name);
                CutScriptExecutePc(scripts->nodes, scene, set);
                return 0;
            }
        }
        CutSceneMissingObjectDiagnosticPc(name);
    }
    return 0;
}

void CutTriggerFirePc(CUT_TRIGGER_SET_PC *set, CUT_TRIGGER_ENTRY_PC *entry, CUT_TRIGGER_STATE_PC *state, i32 on) {
    char name[32];
    if (on != 0) {
        CUT_SCRIPT_DIAG2(entry->name, entry->name);
        state->pulled |= 1;
        CutScriptRunPc(set->scripts, entry->name, set->scene, set);
    } else {
        sprintf(name, "reset_%s", entry->name);
        CUT_SCRIPT_DIAG2(entry->name, name);
        state->pulled &= ~1;
        CutScriptRunPc(set->scripts, name, set->scene, set);
    }
}

void CutTriggerUpdatePc(NUVEC *pos, f32 radius, i32 include_hidden) {
    CUT_TRIGGER_SET_PC *set;
    CUT_TRIGGER_FILE_PC *file;
    NUGSCN *gscene;
    CUT_TRIGGER_ENTRY_PC *entry;
    CUT_TRIGGER_STATE_PC *state;
    CUT_TRIGGER_SCENE_INSTANCE_PC *instance;
    i32 inside;
    i32 i;
    for (set = CutTriggerSetListPc; set != NULL; set = set->next) {
        file = set->file;
        gscene = set->scene;
        if ((set->flags & 1) != 0) {
            continue;
        }
        for (i = 0; i < file->count; ++i) {
            entry = &file->entries[i];
            state = &set->states[i];
            inside = 0;
            if (state->state == 0xff && ((state->pulled & 1) == 0 || (entry->flags & 4) != 0)) {
                switch (entry->kind) {
                    case 0:
                        inside = 1;
                        break;
                    case 1:
                        if ((entry->flags & 8) != 0 && include_hidden == 0 && (state->pulled & 1) == 0) {
                            break;
                        }
                        if (entry->scene_instance >= 0) {
                            instance = &((CUT_TRIGGER_SCENE_INSTANCE_PC *)gscene->instances)[entry->scene_instance];
                            if ((instance->flags & 1) == 0) {
                                break;
                            }
                            if (instance->animated_matrix != NULL) {
                                inside = CutTriggerEntryTestMtxPc(entry, instance->animated_matrix, pos, radius);
                            } else {
                                inside = CutTriggerEntryTestMtxPc(entry, &instance->matrix, pos, radius);
                            }
                        } else {
                            inside = CutTriggerEntryTestPc(entry, pos, radius);
                        }
                        break;
                }
            }
            if (inside != 0) {
                if ((state->pulled & 1) == 0) {
                    CutTriggerFirePc(set, entry, state, 1);
                }
            } else if ((state->pulled & 1) != 0 && (entry->flags & 4) != 0) {
                CutTriggerFirePc(set, entry, state, 0);
            }
        }
    }
}

#define CUT_TRIGGER_HIT_PC(set, entry, state, amount)                                                                  \
    {                                                                                                                  \
        (state)->count += (amount);                                                                                    \
        if ((state)->count >= (entry)->threshold)                                                                      \
            CutTriggerFirePc((set), (entry), (state), 1);                                                              \
        return 1;                                                                                                      \
    }

i32 CutTriggerHitPc(NUVEC *pos, f32 radius, i16 amount) {
    CUT_TRIGGER_SET_PC *set;
    CUT_TRIGGER_FILE_PC *file;
    NUGSCN *gscene;
    CUT_TRIGGER_ENTRY_PC *entry;
    CUT_TRIGGER_STATE_PC *state;
    CUT_TRIGGER_SCENE_INSTANCE_PC *instance;
    i32 i;
    for (set = CutTriggerSetListPc; set != NULL; set = set->next) {
        file = set->file;
        gscene = set->scene;
        if ((set->flags & 1) != 0) {
            continue;
        }
        for (i = 0; i < file->count; ++i) {
            entry = &file->entries[i];
            state = &set->states[i];
            if ((state->pulled & 1) != 0 || state->state != 0xff) {
                continue;
            }
            switch (entry->kind) {
                case 2:
                    if (entry->scene_instance >= 0) {
                        instance = &((CUT_TRIGGER_SCENE_INSTANCE_PC *)gscene->instances)[entry->scene_instance];
                        if ((instance->flags & 1) == 0) {
                            break;
                        }
                        if (instance->animated_matrix != NULL) {
                            if (CutTriggerEntryTestMtxPc(entry, instance->animated_matrix, pos, radius) != 0) {
                                CUT_TRIGGER_HIT_PC(set, entry, state, amount);
                            }
                        } else if (CutTriggerEntryTestMtxPc(entry, &instance->matrix, pos, radius) != 0) {
                            CUT_TRIGGER_HIT_PC(set, entry, state, amount);
                        }
                    } else if (CutTriggerEntryTestPc(entry, pos, radius) != 0) {
                        CUT_TRIGGER_HIT_PC(set, entry, state, amount);
                    }
                    break;
            }
        }
    }
    return 0;
}
