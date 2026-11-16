#include "globals.h"
#include "legoapi/cutscenes/gcutscn.h"
#include "nu2api/cutscene_include.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/nuprim.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nugcutscene.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/trigger.h"
#include <stdio.h>

void instNuGCutSceneEndButNotSystems(INSTNUGCUTSCENE *instance) {
    instance->current_frame = instance->scene->duration;
    instance->render_frame = instance->scene->duration;
    instance->flags_8c &= ~NUGCUTSCENE_PLAY_HOLD_LAST_FRAME;
    instance->flags_88 = (instance->flags_88 & ~NUGCUTSCENE_INST_PLAYING) | NUGCUTSCENE_INST_ENDED;
    if (instance->camera_instance != NULL && instance->camera_instance->camera_index >= 0) {
        CutSceneCameraCTRL = 0;
    }
}

void fun_00500400(INSTNUGCUTSCENE *instance) {
    instance->flags_88 &= ~NUGCUTSCENE_INST_SUSPENDED;
}

void fun_00500410(INSTNUGCUTSCENE *instance) {
    instance->flags_88 |= NUGCUTSCENE_INST_SUSPENDED;
}

void fun_00500420(INSTNUGCUTSCENE *instance, i32 value) {
    if (value > 31) {
        value = 31;
    }
    instance->flags_88 = (instance->flags_88 & ~0x3e000U) | ((value << 13) & 0x3e000);
}

void fun_00500450(INSTNUGCUTSCENE *instance, i32 value) {
    instance->flags_8c = (instance->flags_8c & ~NUGCUTSCENE_PLAY_HOLD_LAST_FRAME) | ((value << 6) & 0x40);
}

void fun_00500470(INSTNUGCUTSCENE *instance, INSTNUGCUTSCENE *chained) {
    instance->chained_instance = chained;
}

void fun_00500480(INSTNUGCUTSCENE *instance, void (*end_callback)(INSTNUGCUTSCENE *)) {
    instance->end_callback = end_callback;
}

void fun_00500490(INSTNUGCUTSCENE *instance, i32 count, VARIPTR *buffer) {
    CUT_LOAD_CAMERA_INSTANCE *camera = instance->camera_instance;
    if (camera != NULL && count != 0) {
        camera->target_capacity = count;
        buffer->addr = (buffer->addr + 15) & ~15;
        camera->targets = buffer->void_ptr;
        buffer->addr += count * sizeof(CUT_LOAD_CAMERA_TARGET);
        memset(camera->targets, 0, count * sizeof(CUT_LOAD_CAMERA_TARGET));
    }
}

void fun_005004d0(void) {
}

void instNuGCutSceneCalculateAverageCentre(INSTNUGCUTSCENE *instance, NUMTX *matrix, NUVEC *out) {
    f32 count = 0.0f;
    i32 i;
    out->x = 0.0f;
    out->y = 0.0f;
    out->z = 0.0f;
    if (instance == NULL || instance->scene == NULL || instance->scene->rigid_system == NULL || instance->rigid_instance->rigids == NULL) {
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

void *DefragCutSceneEndMem = NULL;
void (*ReleaseLocatorVfxFn)(i32) = 0;
NUVEC CutSceneFocusLocation = {0};
i32 DefragCutSceneListSize = 0;
void *DefragCutSceneBaseMem = NULL;
void *DefragInstBaseMem = NULL;
void *DefragInstEndMem = NULL;
CUT_CLEANUP_ENTRY *DefragCutSceneListBase = NULL;
i32 (*TriggerLocatorVfxFn)(i32, NUMTX *) = 0;
INSTNUGCUTSCENE *(*DefragGetInstFn)(void *) = NULL;
CUT_CLEANUP_ENTRY *DefragCutSceneList = NULL;
INSTNUGCUTSCENE *(*DefragCreateInstFn)(void *, CUT_SCENE_LOAD *, VARIPTR *) = NULL;
void (*UpdateLocatorVfxFn)(i32, NUMTX *) = 0;
void (*DefragInstDestroyedFn)(void *) = NULL;

void instNuGCutSceneResetCleanUp(void *list_storage, void *cut_base, void *cut_end, void *inst_base, void *inst_end,
                                 INSTNUGCUTSCENE *(*get_inst)(void *), INSTNUGCUTSCENE *(*create_inst)(void *, CUT_SCENE_LOAD *, VARIPTR *),
                                 void (*inst_destroyed)(void *)) {
    DefragCutSceneList = (CUT_CLEANUP_ENTRY *)(((u32)list_storage + 3) & ~3);
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
        ((void (*)(void))CutSceneLoadDiagnostic)();
    }
    if (get_inst == NULL || create_inst == NULL || inst_destroyed == NULL) {
        ((void (*)(void))CutSceneLoadDiagnostic)();
    }
}

void instNuGCutSceneAddCleanUpItem(void *handle, i32 enabled) {
    INSTNUGCUTSCENE *instance = DefragGetInstFn(handle);
    CUT_CLEANUP_ENTRY *entry;
    if ((instance->flags_88 & NUGCUTSCENE_INST_PLAYING) != 0) {
        ((void (*)(void))CutSceneLoadDiagnostic)();
    }
    entry = DefragCutSceneList++;
    entry->handle = handle;
    entry->scene = instance->scene;
    entry->accumulated_duration = instance->accumulated_duration;
    entry->flags = enabled ? 7 : 0;
    ++DefragCutSceneListSize;
}

static void CutSceneSetPrimitiveColour(i32 colour) {
    i32 converted;
    if (NuPrimNeedsOverbrightening != 0) {
        converted = ((colour >> 16) & 0xff) + ((colour & 0xff) << 16) + (colour & 0xff00) + (colour & 0xff000000u);
    } else {
        converted = ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000u);
    }
    NuPrimStreamBufferPointer->vertex_ptr->colour = converted;
}

void DisplayCutSceneMemory(void) {
    CUT_CLEANUP_ENTRY *entry;
    CUT_CLEANUP_ENTRY *end;
    INSTNUGCUTSCENE *instance;
    CUT_SCENE_LOAD *scene;
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
        CutSceneSetPrimitiveColour(0x40000080);
        NuPrim2DAddXYZ(30.0f, 206.0f, 0.0f);
        CutSceneSetPrimitiveColour(0x40000080);
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
            CutSceneSetPrimitiveColour(0x40008080);
            start_x = (f32)(start_x + 30.0);
            NuPrim2DAddXYZ(start_x, 206.0f, 0.0f);
            CutSceneSetPrimitiveColour(0x40008080);
            end_x = (f32)(end_x + 30.0);
            NuPrim2DAddXYZ(end_x, 210.0f, 0.0f);
            start = (u32)instance - (u32)DefragCutSceneBaseMem;
            finish = start + instance->allocation_size;
            start_x = (f32)((f64)start * scale);
            end_x = (f32)((f64)finish * scale);
            CutSceneSetPrimitiveColour(0x40800080);
            start_x = (f32)(start_x + 30.0);
            NuPrim2DAddXYZ(start_x, 206.0f, 0.0f);
            CutSceneSetPrimitiveColour(0x40800080);
            end_x = (f32)(end_x + 30.0);
            NuPrim2DAddXYZ(end_x, 210.0f, 0.0f);
        }
    } else {
        scale = (f32)(580.0 / (f64)((u32)DefragCutSceneEndMem - (u32)DefragCutSceneBaseMem));
        CutSceneSetPrimitiveColour(0x40000080);
        NuPrim2DAddXYZ(30.0f, 206.0f, 0.0f);
        CutSceneSetPrimitiveColour(0x40000080);
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
            CutSceneSetPrimitiveColour(0x40008080);
            start_x = (f32)(start_x + 30.0);
            NuPrim2DAddXYZ(start_x, 206.0f, 0.0f);
            CutSceneSetPrimitiveColour(0x40008080);
            end_x = (f32)(end_x + 30.0);
            NuPrim2DAddXYZ(end_x, 208.0f, 0.0f);
        }
        scale = (f32)(580.0 / (f64)((u32)DefragInstEndMem - (u32)DefragInstBaseMem));
        CutSceneSetPrimitiveColour(0x40000060);
        NuPrim2DAddXYZ(30.0f, 209.0f, 0.0f);
        CutSceneSetPrimitiveColour(0x40000060);
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
            CutSceneSetPrimitiveColour(0x40800080);
            start_x = (f32)(start_x + 30.0);
            NuPrim2DAddXYZ(start_x, 209.0f, 0.0f);
            CutSceneSetPrimitiveColour(0x40800080);
            end_x = (f32)(end_x + 30.0);
            NuPrim2DAddXYZ(end_x, 211.0f, 0.0f);
        }
    }
    NuPrim2DEnd();
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}

typedef struct cut_script_node_s CUT_SCRIPT_NODE;

struct cut_script_node_s { // size 0x18
    CUT_SCRIPT_NODE *next; // 0x00
    i32 type;              // 0x04

    union { // size 0x4
        char *name;
        i32 param;
        u8 bytes[4];
    } a; // 0x08

    union { // size 0x4
        char *name;
        i32 value;
        CUT_SCRIPT_NODE *block;
    } b; // 0x0c

    CUT_SCRIPT_NODE *c; // 0x10
    CUT_SCRIPT_NODE *d; // 0x14
};

typedef struct cut_script_s CUT_SCRIPT;

struct cut_script_s {       // size 0xc
    CUT_SCRIPT *next;       // 0x0
    char *name;             // 0x4
    CUT_SCRIPT_NODE *nodes; // 0x8
};

#define CUT_SCRIPT_NODE_ALLOC(node, buffer, size)                                                                                          \
    (node) = (CUT_SCRIPT_NODE *)(buffer)->void_ptr;                                                                                        \
    (buffer)->addr += (size);                                                                                                              \
    memset((node), 0, (size))

static CUT_SCRIPT_NODE *CutScriptNodeAlloc(i32 type, VARIPTR *buffer, CUT_SCRIPT_NODE *prev, CUT_SCRIPT_NODE **head) {
    CUT_SCRIPT_NODE *node = NULL;
    buffer->addr = (buffer->addr + 0xf) & ~0xfU;
    switch (type) {
        case CUTSCRIPT_START:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0xc);
            break;
        case CUTSCRIPT_PLAY:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0x10);
            break;
        case CUTSCRIPT_CHAIN:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0x10);
            break;
        case CUTSCRIPT_REPEAT:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0x10);
            break;
        case CUTSCRIPT_RESET:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0xc);
            break;
        case CUTSCRIPT_SET_TRIGGER_FLAG:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0x10);
            break;
        case CUTSCRIPT_RESET_TRIGGER:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0xc);
            break;
        case CUTSCRIPT_SET_VISIBILITY:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0x10);
            break;
        case CUTSCRIPT_IF_PARAM:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0x18);
            break;
        case CUTSCRIPT_SET_PARAM:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0x10);
            break;
        case CUTSCRIPT_IF_TRIGGER_PULLED:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0x14);
            break;
        case CUTSCRIPT_ENABLE_TRIGGER:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0xc);
            break;
        case CUTSCRIPT_PAUSE:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0xc);
            break;
        case CUTSCRIPT_UNPAUSE:
            CUT_SCRIPT_NODE_ALLOC(node, buffer, 0xc);
            break;
        case CUTSCRIPT_APP_COMMAND:
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

#define CUT_SCRIPT_READ_WORD(fp, buffer, field)                                                                                            \
    NuFParGetWord(fp);                                                                                                                     \
    (field) = (buffer)->char_ptr;                                                                                                          \
    (buffer)->addr += strlen((fp)->word_buffer_ptr) + 1;                                                                                   \
    sprintf((field), (fp)->word_buffer_ptr)

CUT_SCRIPT_NODE *CutScriptParseBlock(NUFPAR *fp, VARIPTR *buffer, i32 unused) {
    CUT_SCRIPT_NODE *head = NULL;
    CUT_SCRIPT_NODE *prev = NULL;
    CUT_SCRIPT_NODE *node;
    while (NuFParGetLine(fp) != 0) {
        NuFParGetWord(fp);
        if (NuStrICmp(fp->word_buffer_ptr, "ENDSCRIPT") == 0) {
            break;
        }
        if (NuStrICmp(fp->word_buffer_ptr, "STARTCUTSCENEINST") == 0) {
            node = CutScriptNodeAlloc(CUTSCRIPT_START, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
        } else if (NuStrICmp(fp->word_buffer_ptr, "PLAYCUTSCENEINST") == 0) {
            node = CutScriptNodeAlloc(CUTSCRIPT_PLAY, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
            NuFParGetWord(fp);
            if (NuStrICmp(fp->word_buffer_ptr, "forward") == 0) {
                node->b.value = 1;
            } else if (NuStrICmp(fp->word_buffer_ptr, "backward") == 0) {
                node->b.value = 0;
            } else {
                ((void (*)(void))CutSceneMissingObjectDiagnostic)();
            }
        } else if (NuStrICmp(fp->word_buffer_ptr, "CHAINCUTSCENEINST") == 0) {
            node = CutScriptNodeAlloc(CUTSCRIPT_CHAIN, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
            CUT_SCRIPT_READ_WORD(fp, buffer, node->b.name);
        } else if (NuStrICmp(fp->word_buffer_ptr, "REPEATCUTSCENEINST") == 0) {
            node = CutScriptNodeAlloc(CUTSCRIPT_REPEAT, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
            node->b.value = NuFParGetInt(fp);
        } else if (NuStrICmp(fp->word_buffer_ptr, "RESETCUTSCENEINST") == 0) {
            node = CutScriptNodeAlloc(CUTSCRIPT_RESET, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
        } else if (NuStrICmp(fp->word_buffer_ptr, "PAUSECUTSCENEINST") == 0) {
            node = CutScriptNodeAlloc(CUTSCRIPT_PAUSE, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
        } else if (NuStrICmp(fp->word_buffer_ptr, "UNPAUSECUTSCENEINST") == 0) {
            node = CutScriptNodeAlloc(CUTSCRIPT_UNPAUSE, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
        } else if (NuStrICmp(fp->word_buffer_ptr, "SETTRIGGERFLAG") == 0) {
            node = CutScriptNodeAlloc(CUTSCRIPT_SET_TRIGGER_FLAG, buffer, prev, &head);
            prev = node;
            node->a.bytes[0] = (u8)NuFParGetInt(fp);
            CUT_SCRIPT_READ_WORD(fp, buffer, node->b.name);
            NuFParGetWord(fp);
            node->a.bytes[1] = NuStrICmp(fp->word_buffer_ptr, "ON") == 0;
        } else if (NuStrICmp(fp->word_buffer_ptr, "RESETTRIGGER") == 0) {
            node = CutScriptNodeAlloc(CUTSCRIPT_RESET_TRIGGER, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
        } else if (NuStrICmp(fp->word_buffer_ptr, "SETVISIBILITY") == 0) {
            node = CutScriptNodeAlloc(CUTSCRIPT_SET_VISIBILITY, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->b.name);
            NuFParGetWord(fp);
            node->a.bytes[0] = NuStrICmp(fp->word_buffer_ptr, "ON") == 0;
        } else if (NuStrICmp(fp->word_buffer_ptr, "IFPARAM") == 0) {
            node = CutScriptNodeAlloc(CUTSCRIPT_IF_PARAM, buffer, prev, &head);
            prev = node;
            node->a.param = NuFParGetInt(fp);
            node->b.value = NuFParGetInt(fp);
            node->c = CutScriptParseBlock(fp, buffer, unused);
            if (NuStrICmp(fp->word_buffer_ptr, "ELSE") == 0) {
                node->d = CutScriptParseBlock(fp, buffer, unused);
            }
        } else if (NuStrICmp(fp->word_buffer_ptr, "SETPARAM") == 0) {
            node = CutScriptNodeAlloc(CUTSCRIPT_SET_PARAM, buffer, prev, &head);
            prev = node;
            node->a.param = NuFParGetInt(fp);
            node->b.value = NuFParGetInt(fp);
        } else if (NuStrICmp(fp->word_buffer_ptr, "IFTRIGGERPULLED") == 0) {
            node = CutScriptNodeAlloc(CUTSCRIPT_IF_TRIGGER_PULLED, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
            node->b.block = CutScriptParseBlock(fp, buffer, unused);
            if (NuStrICmp(fp->word_buffer_ptr, "ELSE") == 0) {
                node->c = CutScriptParseBlock(fp, buffer, unused);
            }
        } else if (NuStrICmp(fp->word_buffer_ptr, "IFTRIGGERNOTPULLED") == 0) {
            node = CutScriptNodeAlloc(CUTSCRIPT_IF_TRIGGER_PULLED, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
            node->c = CutScriptParseBlock(fp, buffer, unused);
            if (NuStrICmp(fp->word_buffer_ptr, "ELSE") == 0) {
                node->b.block = CutScriptParseBlock(fp, buffer, unused);
            }
        } else if (NuStrICmp(fp->word_buffer_ptr, "ENABLETRIGGER") == 0) {
            node = CutScriptNodeAlloc(CUTSCRIPT_ENABLE_TRIGGER, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
        } else if (NuStrICmp(fp->word_buffer_ptr, "ELSE") == 0) {
            break;
        } else if (NuStrICmp(fp->word_buffer_ptr, "ENDIF") == 0) {
            break;
        } else if (NuStrICmp(fp->word_buffer_ptr, "APPCOMMAND") == 0) {
            node = CutScriptNodeAlloc(CUTSCRIPT_APP_COMMAND, buffer, prev, &head);
            prev = node;
            CUT_SCRIPT_READ_WORD(fp, buffer, node->a.name);
            CUT_SCRIPT_READ_WORD(fp, buffer, node->b.name);
        }
    }
    return head;
}

static CUT_SCRIPT *CutScriptRead(NUFPAR *fp, VARIPTR *buffer, i32 unused) {
    CUT_SCRIPT *script;
    buffer->addr = (buffer->addr + 0xf) & ~0xfU;
    script = buffer->void_ptr;
    buffer->addr += sizeof(CUT_SCRIPT);
    memset(script, 0, sizeof(CUT_SCRIPT));
    CUT_SCRIPT_READ_WORD(fp, buffer, script->name);
    script->nodes = CutScriptParseBlock(fp, buffer, unused);
    return script;
}

CUT_SCRIPT *CutScriptLoad(char *file, VARIPTR *buffer, i32 unused) {
    CUT_SCRIPT *head = NULL;
    CUT_SCRIPT *prev = NULL;
    CUT_SCRIPT *script;
    NUFPAR *fp = NuFParCreate(file);
    if (fp != NULL) {
        NuFParGetWord(fp);
        while (NuFParGetLine(fp) != 0) {
            NuFParGetWord(fp);
            if (NuStrICmp(fp->word_buffer_ptr, "SCRIPT") != 0) {
                continue;
            }
            script = CutScriptRead(fp, buffer, unused);
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

typedef struct cut_trigger_shape_s { // size 0x8
    i32 type;                        // 0x0
    void *data;                      // 0x4
} CUT_TRIGGER_SHAPE;

typedef struct cut_trigger_sphere_s {
    NUVEC centre;
    f32 radius;
} CUT_TRIGGER_SPHERE;

typedef struct cut_trigger_box_s { // size 0x58
    NUMTX matrix;                  // 0x00
    NUVEC min;                     // 0x40
    NUVEC max;                     // 0x4c
} CUT_TRIGGER_BOX;

typedef struct cut_trigger_cylinder_s { // size 0x14
    NUVEC base;                         // 0x00
    f32 height;                         // 0x0c
    f32 radius;                         // 0x10
} CUT_TRIGGER_CYLINDER;

typedef struct cut_trigger_entry_s { // size 0x34
    char *name;                      // 0x00
    i32 kind;                        // 0x04
    i16 threshold;                   // 0x08
    u8 initial_state;                // 0x0a
    u8 unknown_0b;                   // 0x0b
    u8 flags;                        // 0x0c
    u8 unknown_0d[3];                // 0x0d
    f32 radius;                      // 0x10
    NUVEC min;                       // 0x14
    NUVEC max;                       // 0x20
    i16 shape_count;                 // 0x2c
    i16 scene_instance;              // 0x2e
    CUT_TRIGGER_SHAPE *shapes;       // 0x30
} CUT_TRIGGER_ENTRY;

typedef struct cut_trigger_file_s {
    i32 version;
    i32 delta;
    i32 count;
    CUT_TRIGGER_ENTRY *entries;
} CUT_TRIGGER_FILE;

typedef struct cut_trigger_state_s { // size 0x4
    i16 count;                       // 0x0
    u8 state;                        // 0x2
    u8 pulled;                       // 0x3
} CUT_TRIGGER_STATE;

typedef struct cut_trigger_scene_instance_s { // size 0x50
    NUMTX matrix;                             // 0x00
    i32 unknown_40;                           // 0x40
    u32 flags;                                // 0x44
    NUMTX *animated_matrix;                   // 0x48
    u32 unknown_4c;                           // 0x4c
} CUT_TRIGGER_SCENE_INSTANCE;

typedef struct cut_trigger_set_s CUT_TRIGGER_SET;

struct cut_trigger_set_s {     // size 0x3c
    CUT_TRIGGER_SET *next;     // 0x00
    CUT_TRIGGER_SET *prev;     // 0x04
    CUT_TRIGGER_FILE *file;    // 0x08
    CUT_TRIGGER_STATE *states; // 0x0c
    NUGSCN *scene;             // 0x10
    CUT_SCRIPT *scripts;       // 0x14
    i8 params[32];             // 0x18
    u32 flags;                 // 0x38
};

CUT_TRIGGER_SET *CutTriggerSetList = NULL;
CUT_TRIGGER_SET *CutTriggerPendingSetList = NULL;
i32 (*LookupLocatorVfxFn)(char *) = 0;

typedef struct cut_app_command_s { // size 0x8
    char *name;                    // 0x0
    void (*fn)(char *);            // 0x4
} CUT_APP_COMMAND;

CUT_APP_COMMAND *CutScriptAppCommands = NULL;

void CutScriptSetAppCommands(CUT_APP_COMMAND *commands) {
    CutScriptAppCommands = commands;
}

void CutTriggerClearPending(void) {
    CutTriggerPendingSetList = NULL;
}

void CutTriggerClearLists(void) {
    CutTriggerPendingSetList = NULL;
    CutTriggerSetList = NULL;
}

void CutTriggerActivatePending(void) {
    CUT_TRIGGER_SET *set = CutTriggerPendingSetList;
    CUT_TRIGGER_SET *list;
    CUT_TRIGGER_SET *current;
    if (set != NULL) {
        list = CutTriggerSetList;
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
        CutTriggerSetList = list;
        CutTriggerPendingSetList = set;
    }
}

CUT_TRIGGER_SET *CutTriggerSetCreate(CUT_TRIGGER_FILE *file, NUGSCN *scene, CUT_SCRIPT *scripts, VARIPTR *buffer, VARIPTR *end,
                                     i32 pending) {
    CUT_TRIGGER_SET *set = NULL;
    CUT_TRIGGER_SET *list;
    i32 i;
    (void)end;
    if (file != NULL && scene != NULL && scripts != NULL) {
        buffer->addr = (buffer->addr + 0xf) & ~0xfU;
        set = buffer->void_ptr;
        buffer->addr += sizeof(CUT_TRIGGER_SET);
        memset(set, 0, sizeof(CUT_TRIGGER_SET));
        set->prev = NULL;
        if (pending != 0) {
            list = CutTriggerPendingSetList;
            set->next = list;
            if (list != NULL) {
                list->prev = set;
            }
            CutTriggerPendingSetList = set;
        } else {
            list = CutTriggerSetList;
            set->next = list;
            if (list != NULL) {
                list->prev = set;
            }
            CutTriggerSetList = set;
        }
        set->scene = scene;
        set->file = file;
        set->scripts = scripts;
        set->states = buffer->void_ptr;
        buffer->addr += file->count * sizeof(CUT_TRIGGER_STATE);
        memset(set->states, 0, file->count * sizeof(CUT_TRIGGER_STATE));
        for (i = 0; i < file->count; ++i) {
            set->states[i].state = file->entries[i].initial_state;
        }
    }
    return set;
}

void CutTriggerSetUnlink(CUT_TRIGGER_SET *set) {
    if (set->next != NULL) {
        set->next->prev = set->prev;
    }
    if (set->prev != NULL) {
        set->prev->next = set->next;
    } else {
        CutTriggerSetList = set->next;
    }
}

void CutTriggerSetEnable(CUT_TRIGGER_SET *set) {
    set->flags &= ~1U;
}

void CutTriggerSetDisable(CUT_TRIGGER_SET *set) {
    set->flags |= 1;
}

CUT_TRIGGER_FILE *CutTriggerFileLoad(char *path, VARIPTR *buffer, VARIPTR *end) {
    CUT_TRIGGER_FILE *file;
    CUT_TRIGGER_ENTRY *entry;
    CUT_TRIGGER_SHAPE *shape;
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
        CutSceneLoadDiagnostic(path, file->version);
    }
    file->delta = (i32)file - file->delta;
    file->entries = file->entries != NULL ? (CUT_TRIGGER_ENTRY *)((u32)file->entries + file->delta) : NULL;
    if (file->entries != NULL) {
        for (i = 0; i < file->count; ++i) {
            entry = &file->entries[i];
            entry->name = entry->name != NULL ? (char *)((u32)entry->name + file->delta) : NULL;
            entry->shapes = entry->shapes != NULL ? (CUT_TRIGGER_SHAPE *)((u32)entry->shapes + file->delta) : NULL;
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

void CutTriggerSysNull(void) {
}

void CutTriggerSetFlag(CUT_TRIGGER_SET *set, char *name, i32 flag, i32 on) {
    CUT_TRIGGER_FILE *file = set->file;
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

void CutTriggerEnable(CUT_TRIGGER_SET *set, char *name) {
    CUT_TRIGGER_FILE *file = set->file;
    i32 i;
    for (i = 0; i < file->count; ++i) {
        if (NuStrICmp(file->entries[i].name, name) == 0) {
            set->states[i].state = 0xff;
            return;
        }
    }
}

i32 CutTriggerIsPulled(CUT_TRIGGER_SET *set, char *name) {
    CUT_TRIGGER_FILE *file = set->file;
    i32 i;
    for (i = 0; i < file->count; ++i) {
        if (NuStrICmp(file->entries[i].name, name) == 0) {
            return set->states[i].pulled & 1;
        }
    }
    return 0;
}

void CutTriggerPull(CUT_TRIGGER_SET *set, char *name) {
    CUT_TRIGGER_FILE *file = set->file;
    i32 i;
    for (i = 0; i < file->count; ++i) {
        if (NuStrICmp(file->entries[i].name, name) == 0) {
            set->states[i].pulled |= 1;
            return;
        }
    }
}

void CutTriggerReset(CUT_TRIGGER_SET *set, char *name) {
    CUT_TRIGGER_FILE *file;
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

static f32 CutTriggerFabs(f32 value) {
    i32 bits = *(i32 *)&value;
    bits &= 0x7fffffff;
    return *(f32 *)&bits;
}

i32 CutTriggerEntryTest(CUT_TRIGGER_ENTRY *entry, NUVEC *pos, f32 radius) {
    NUVEC extent;
    NUVEC local_pos;
    NUVEC box_extent;
    NUVEC delta;
    CUT_TRIGGER_SHAPE *shape;
    CUT_TRIGGER_SPHERE *sphere;
    CUT_TRIGGER_BOX *box;
    CUT_TRIGGER_CYLINDER *cylinder;
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
                box_extent.x = CutTriggerFabs(box_extent.x);
                box_extent.y = CutTriggerFabs(box_extent.y);
                box_extent.z = CutTriggerFabs(box_extent.z);
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
                dist =
                    (pos->x - cylinder->base.x) * (pos->x - cylinder->base.x) + (pos->z - cylinder->base.z) * (pos->z - cylinder->base.z);
                if (dist < (cylinder->radius + radius) * (cylinder->radius + radius)) {
                    return 1;
                }
                break;
        }
    }
    return 0;
}

i32 CutTriggerEntryTestMtx(CUT_TRIGGER_ENTRY *entry, NUMTX *mtx, NUVEC *pos, f32 radius) {
    NUMTX inverse;
    NUVEC delta;
    NUVEC local_pos;
    NUVEC extent;
    NUVEC centre;
    NUVEC box_pos;
    NUVEC box_extent;
    CUT_TRIGGER_SHAPE *shape;
    CUT_TRIGGER_SPHERE *sphere;
    CUT_TRIGGER_BOX *box;
    CUT_TRIGGER_CYLINDER *cylinder;
    f32 dist;
    i32 i;
    delta.x = pos->x - mtx->m30;
    delta.y = pos->y - mtx->m31;
    delta.z = pos->z - mtx->m32;
    if (delta.x * delta.x + delta.y * delta.y + delta.z * delta.z > (entry->radius + radius) * (entry->radius + radius)) {
        return 0;
    }
    if ((entry->flags & 1) != 0) {
        NuMtxInvH(&inverse, mtx);
        NuVecMtxTransform(&local_pos, pos, &inverse);
        extent.x = extent.y = extent.z = radius;
        NuVecMtxTransform(&extent, &extent, &inverse);
        NuVecSub(&extent, &extent, &inverse.translation);
        extent.x = CutTriggerFabs(extent.x);
        extent.y = CutTriggerFabs(extent.y);
        extent.z = CutTriggerFabs(extent.z);
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
                box_extent.x = CutTriggerFabs(box_extent.x);
                box_extent.y = CutTriggerFabs(box_extent.y);
                box_extent.z = CutTriggerFabs(box_extent.z);
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

i32 CutTriggerSetTest(CUT_TRIGGER_SET *set, char *name, NUVEC *pos, f32 radius, i32 include_hidden) {
    CUT_TRIGGER_FILE *file = set->file;
    NUGSCN *gscene;
    CUT_TRIGGER_ENTRY *entry;
    CUT_TRIGGER_SCENE_INSTANCE *instance;
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
                        instance = &((CUT_TRIGGER_SCENE_INSTANCE *)gscene->instances)[entry->scene_instance];
                        if ((instance->flags & 1) == 0) {
                            break;
                        }
                        if (instance->animated_matrix != NULL) {
                            hit = CutTriggerEntryTestMtx(entry, instance->animated_matrix, pos, radius);
                        } else {
                            hit = CutTriggerEntryTestMtx(entry, &instance->matrix, pos, radius);
                        }
                    } else {
                        hit = CutTriggerEntryTest(entry, pos, radius);
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

/* Empty on PC. It lives in this unit and takes variable arguments: once a call
   passes a pointer through the ellipsis (CutScriptExecute), VC8 keeps every
   later call and knows the body preserves eax/ecx/edx, which CutTriggerFire
   and CutTriggerUpdate rely on. Calls compiled before the definition stay
   ordinary external calls (CutScriptParseBlock), so it is defined here rather
   than at the top. */
void CutSceneMissingObjectDiagnostic(char *format, ...) {
}

#define CUT_SCRIPT_DIAG2(a, b) CutSceneMissingObjectDiagnostic((char *)(a), (b))

void CutScriptExecute(CUT_SCRIPT_NODE *node, NUGSCN *scene, CUT_TRIGGER_SET *set) {
    INSTNUGCUTSCENE *instance;
    INSTNUGCUTSCENE *chain;
    nuhspecial_s special;
    CUT_APP_COMMAND *command;
    i32 count;
    i32 done;
    for (; node != NULL; node = node->next) {
        done = 0;
        switch (node->type) {
            case CUTSCRIPT_START:
                instance = instNuGCutSceneFindActive(node->a.name);
                if (instance != NULL) {
                    instNuGCutSceneStart(instance);
                    done = 1;
                }
                CutSceneMissingObjectDiagnostic(node->a.name);
                break;
            case CUTSCRIPT_PLAY:
                instance = instNuGCutSceneFindActive(node->a.name);
                if (instance != NULL) {
                    instNuGCutScenePlay(instance, node->b.value);
                    done = 1;
                }
                if (node->b.value != 0) {
                    CutSceneMissingObjectDiagnostic(node->a.name);
                } else {
                    CutSceneMissingObjectDiagnostic(node->a.name);
                }
                break;
            case CUTSCRIPT_CHAIN:
                instance = instNuGCutSceneFindActive(node->a.name);
                chain = instNuGCutSceneFindActive(node->b.name);
                if (instance != NULL && chain != NULL) {
                    if (instance->scene == NULL) {
                        ((void (*)(void))CutSceneLoadDiagnostic)();
                    }
                    instance->chained_instance = chain;
                    done = 1;
                }
                CUT_SCRIPT_DIAG2(node->a.name, node->b.name);
                break;
            case CUTSCRIPT_REPEAT:
                instance = instNuGCutSceneFindActive(node->a.name);
                if (instance != NULL) {
                    count = node->b.value;
                    if (count > 31) {
                        ((void (*)(i32))CutSceneMissingObjectDiagnostic)(count);
                        count = 31;
                    }
                    instance->flags_88 ^= ((u32)(count << 13) ^ instance->flags_88) & 0x3e000;
                    done = 1;
                }
                CUT_SCRIPT_DIAG2(node->a.name, node->b.value);
                break;
            case CUTSCRIPT_RESET:
                instance = instNuGCutSceneFindActive(node->a.name);
                if (instance != NULL) {
                    instNuGCutSceneReset(instance);
                    done = 1;
                }
                CutSceneMissingObjectDiagnostic(node->a.name);
                break;
            case CUTSCRIPT_SET_TRIGGER_FLAG:
                if (set != NULL) {
                    CutTriggerSetFlag(set, node->b.name, (i8)node->a.bytes[0], (i8)node->a.bytes[1]);
                    done = 1;
                }
                if (node->a.bytes[1] != 0) {
                    CUT_SCRIPT_DIAG2((i8)node->a.bytes[0], node->b.name);
                } else {
                    CUT_SCRIPT_DIAG2((i8)node->a.bytes[0], node->b.name);
                }
                break;
            case CUTSCRIPT_RESET_TRIGGER:
                if (set == NULL) {
                    CutSceneMissingObjectDiagnostic(node->a.name);
                    break;
                }
                CutTriggerReset(set, node->a.name);
                done = 1;
                CutSceneMissingObjectDiagnostic(node->a.name);
                break;
            case CUTSCRIPT_SET_VISIBILITY:
                if (scene != NULL && NuSpecialFind(scene, &special, node->b.name, 1) != 0) {
                    if (node->a.bytes[0] != 0) {
                        ((NUSPECIAL_LEGACY *)special.special)->instance->flags |= 1;
                    } else {
                        ((NUSPECIAL_LEGACY *)special.special)->instance->flags &= ~1;
                    }
                    done = 1;
                }
                CutSceneMissingObjectDiagnostic(node->b.name);
                break;
            case CUTSCRIPT_IF_PARAM:
                if (node->a.param < 32 && set->params[node->a.param] == node->b.value) {
                    CUT_SCRIPT_DIAG2(node->a.param, node->b.value);
                    CutScriptExecute(node->c, scene, set);
                } else {
                    CUT_SCRIPT_DIAG2(node->a.param, node->b.value);
                    CutScriptExecute(node->d, scene, set);
                }
                break;
            case CUTSCRIPT_SET_PARAM:
                if (node->a.param < 32) {
                    set->params[node->a.param] = (i8)node->b.value;
                    done = 1;
                }
                CUT_SCRIPT_DIAG2(node->a.param, node->b.value);
                break;
            case CUTSCRIPT_IF_TRIGGER_PULLED:
                if (CutTriggerIsPulled(set, node->a.name) != 0) {
                    CutSceneMissingObjectDiagnostic(node->a.name);
                    CutScriptExecute(node->b.block, scene, set);
                } else {
                    CutSceneMissingObjectDiagnostic(node->a.name);
                    CutScriptExecute(node->c, scene, set);
                }
                break;
            case CUTSCRIPT_ENABLE_TRIGGER:
                CutTriggerEnable(set, node->a.name);
                CutSceneMissingObjectDiagnostic(node->a.name);
                continue;
            case CUTSCRIPT_PAUSE:
                instance = instNuGCutSceneFindActive(node->a.name);
                if (instance != NULL) {
                    instance->flags_88 |= NUGCUTSCENE_INST_PAUSED;
                    done = 1;
                }
                CutSceneMissingObjectDiagnostic(node->a.name);
                break;
            case CUTSCRIPT_UNPAUSE:
                instance = instNuGCutSceneFindActive(node->a.name);
                if (instance != NULL) {
                    instance->flags_88 &= ~NUGCUTSCENE_INST_PAUSED;
                    done = 1;
                }
                CutSceneMissingObjectDiagnostic(node->a.name);
                break;
            case CUTSCRIPT_APP_COMMAND:
                command = CutScriptAppCommands;
                CutSceneMissingObjectDiagnostic(command->name);
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
            ((void (*)(void))CutSceneMissingObjectDiagnostic)();
        }
    }
}

i32 CutScriptRun(CUT_SCRIPT *scripts, char *name, NUGSCN *scene, CUT_TRIGGER_SET *set) {
    CUT_APP_COMMAND *command = CutScriptAppCommands;
    if (NuStrNICmp(name, "Fn", 2) == 0) {
        for (; command != NULL && command->name != NULL; ++command) {
            if (NuStrICmp(name + 2, command->name) == 0) {
                CutSceneMissingObjectDiagnostic(name);
                if (command->fn != NULL) {
                    command->fn(NULL);
                }
                return 0;
            }
        }
    } else {
        for (; scripts != NULL; scripts = scripts->next) {
            if (NuStrICmp(scripts->name, name) == 0) {
                CutSceneMissingObjectDiagnostic(name);
                CutScriptExecute(scripts->nodes, scene, set);
                return 0;
            }
        }
        CutSceneMissingObjectDiagnostic(name);
    }
    return 0;
}

void CutTriggerFire(CUT_TRIGGER_SET *set, CUT_TRIGGER_ENTRY *entry, CUT_TRIGGER_STATE *state, i32 on) {
    char name[32];
    if (on != 0) {
        CUT_SCRIPT_DIAG2(entry->name, entry->name);
        state->pulled |= 1;
        CutScriptRun(set->scripts, entry->name, set->scene, set);
    } else {
        sprintf(name, "reset_%s", entry->name);
        CUT_SCRIPT_DIAG2(entry->name, name);
        state->pulled &= ~1;
        CutScriptRun(set->scripts, name, set->scene, set);
    }
}

void CutTriggerUpdate(NUVEC *pos, f32 radius, i32 include_hidden) {
    CUT_TRIGGER_SET *set;
    CUT_TRIGGER_FILE *file;
    NUGSCN *gscene;
    CUT_TRIGGER_ENTRY *entry;
    CUT_TRIGGER_STATE *state;
    CUT_TRIGGER_SCENE_INSTANCE *instance;
    i32 inside;
    i32 i;
    for (set = CutTriggerSetList; set != NULL; set = set->next) {
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
                            instance = &((CUT_TRIGGER_SCENE_INSTANCE *)gscene->instances)[entry->scene_instance];
                            if ((instance->flags & 1) == 0) {
                                break;
                            }
                            if (instance->animated_matrix != NULL) {
                                inside = CutTriggerEntryTestMtx(entry, instance->animated_matrix, pos, radius);
                            } else {
                                inside = CutTriggerEntryTestMtx(entry, &instance->matrix, pos, radius);
                            }
                        } else {
                            inside = CutTriggerEntryTest(entry, pos, radius);
                        }
                        break;
                }
            }
            if (inside != 0) {
                if ((state->pulled & 1) == 0) {
                    CutTriggerFire(set, entry, state, 1);
                }
            } else if ((state->pulled & 1) != 0 && (entry->flags & 4) != 0) {
                CutTriggerFire(set, entry, state, 0);
            }
        }
    }
}

i32 CutTriggerHit(NUVEC *pos, f32 radius, i16 amount) {
    CUT_TRIGGER_SET *set;
    CUT_TRIGGER_FILE *file;
    NUGSCN *gscene;
    CUT_TRIGGER_ENTRY *entry;
    CUT_TRIGGER_STATE *state;
    CUT_TRIGGER_SCENE_INSTANCE *instance;
    i32 i;
    for (set = CutTriggerSetList; set != NULL; set = set->next) {
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
                        instance = &((CUT_TRIGGER_SCENE_INSTANCE *)gscene->instances)[entry->scene_instance];
                        if ((instance->flags & 1) == 0) {
                            break;
                        }
                        if (instance->animated_matrix != NULL) {
                            if (CutTriggerEntryTestMtx(entry, instance->animated_matrix, pos, radius) != 0) {
                                state->count += amount;
                                if (state->count >= entry->threshold) {
                                    CutTriggerFire(set, entry, state, 1);
                                }
                                return 1;
                            }
                        } else if (CutTriggerEntryTestMtx(entry, &instance->matrix, pos, radius) != 0) {
                            state->count += amount;
                            if (state->count >= entry->threshold) {
                                CutTriggerFire(set, entry, state, 1);
                            }
                            return 1;
                        }
                    } else if (CutTriggerEntryTest(entry, pos, radius) != 0) {
                        state->count += amount;
                        if (state->count >= entry->threshold) {
                            CutTriggerFire(set, entry, state, 1);
                        }
                        return 1;
                    }
                    break;
            }
        }
    }
    return 0;
}
