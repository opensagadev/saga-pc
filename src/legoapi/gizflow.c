#include "saga.h"
#include <string.h>

#define FLOWBOX_FLAGS(box) ((u16)(((u16)(box)->state_flags_high << 8) | (box)->state_flags_low))
#define FLOWBOX_SET_FLAGS(box, flags)                                                                                  \
    do {                                                                                                               \
        (box)->state_flags_low = (u8)(flags);                                                                          \
        (box)->state_flags_high = (u8)((flags) >> 8);                                                                  \
    } while (0)

void __cdecl GizFlowStoreProgress(GIZFLOW_PC *system, GIZFLOWPROGRESS_PC *progress) {
    FLOWBOX_PC *box;
    i32 i;

    if (progress == NULL || system == NULL)
        return;

    memset(progress, 0, 0x144);
    progress->valid = 1;
    box = system->flowboxes;
    for (i = 0; i < system->flowbox_count; ++i, ++box) {
        i32 word = i >> 5;
        u32 bit = 1u << (i & 31);
        u16 flags = FLOWBOX_FLAGS(box);
        if ((box->state_flags_low & 1) != 0)
            progress->active[word] |= bit;
        if ((box->state_flags_high & 1) != 0)
            progress->triggered[word] |= bit;
        if ((box->state_flags_low & 2) != 0)
            progress->completed[word] |= bit;
        if ((box->state_flags_low & 0x20) != 0)
            progress->latched[word] |= bit;
        if ((flags & 0x400) != 0)
            progress->output_state[word] |= bit;
    }
}

void __cdecl ResetGizmoFlowBox(GIZFLOW_PC *system, FLOWBOX_PC *box) {
    FLOWBOXGIZMODATA_PC *data;
    i32 i;

    if ((FLOWBOX_FLAGS(box) & 0x1000) != 0)
        return;
    data = (FLOWBOXGIZMODATA_PC *)box->data;
    for (i = 0; i < data->gizmo_count; ++i)
        GizmoActivate(system->gizmo_sys, data->gizmos[i]->gizmo, 0, 1);
}

i32 __cdecl CheckOutputGizmoFlowBox(GIZFLOW_PC *system, FLOWBOX_PC *box, u8 output) {
    FLOWBOXGIZMODATA_PC *data;
    i32 result = 1;
    i32 i;

    if (((box->state_flags_high & 0x40) == 0 && FreePlay == 0) ||
        ((box->state_flags_high & 0x20) == 0 && FreePlay != 0))
        return 1;
    data = (FLOWBOXGIZMODATA_PC *)box->data;
    for (i = 0; i < data->gizmo_count; ++i) {
        if (!GizmoGetOutput(system->gizmo_sys, data->gizmos[i]->gizmo, output, (box->state_flags_high >> 4) & 1))
            result = 0;
    }
    return result;
}

static i32 LoopCountLoopingInputsEx(FLOWBOX_PC *loop, FLOWBOX_PC *box, i32 count, u8 checksum) {
    i32 i;
    if (loop == box) {
        --box->loop_checksum;
        return count + 1;
    }
    for (i = 0; i < box->child_count; ++i) {
        FLOWBOX_PC *child = box->children[i];
        if (child->loop_checksum != checksum) {
            child->loop_checksum = checksum;
            count = LoopCountLoopingInputsEx(loop, child, count, checksum);
        }
    }
    return count;
}

#define COUNT_LOOPING_INPUTS(loop, checksum, result)                                                                   \
    do {                                                                                                               \
        i32 _loop_count = 0;                                                                                           \
        i32 _loop_index;                                                                                               \
        for (_loop_index = 0; _loop_index < (loop)->child_count; ++_loop_index)                                        \
            _loop_count = LoopCountLoopingInputsEx((loop), (loop)->children[_loop_index], _loop_count, (checksum));    \
        (result) = _loop_count;                                                                                        \
    } while (0)

static void ResetForLoopEx(GIZFLOW_PC *flow, FLOWBOX_PC *root, FLOWBOX_PC *box, u8 checksum) {
    FLOWBOXGIZMODATA_PC *data;
    i32 i;

    if (box == root || box->loop_checksum == checksum)
        return;
    box->loop_checksum = checksum;
    if (box->type == 0 && (FLOWBOX_FLAGS(box) & 0x1000) == 0 && box->data != NULL) {
        data = (FLOWBOXGIZMODATA_PC *)box->data;
        for (i = 0; i < data->gizmo_count; ++i)
            GizmoActivate(flow->gizmo_sys, data->gizmos[i]->gizmo, 0, 1);
    }
    for (i = 0; i < box->child_count; ++i)
        ResetForLoopEx(flow, root, box->children[i], checksum);
}

i32 __cdecl ProcessConditionFlowBox(GIZFLOW_PC *flow, FLOWBOX_PC *box, u8 frame) {
    FLOWBOXCONDITION_PC *condition = (FLOWBOXCONDITION_PC *)box->data;
    i32 i;

    if (condition == NULL) {
        for (i = 0; i < box->parent_count; ++i) {
            FLOWBOX_PC *parent = box->parents[i];
            if (!flowbox_callbacks[parent->type].check_output(flow, parent, box->output_indices[i]))
                return 0;
        }
        return 1;
    }

    switch (condition->operation) {
        case 0:
            for (i = 0; i < box->parent_count; ++i) {
                FLOWBOX_PC *parent = box->parents[i];
                if (!flowbox_callbacks[parent->type].check_output(flow, parent, box->output_indices[i]))
                    return 0;
            }
            break;
        case 2:
            for (i = 0; i < box->parent_count; ++i) {
                FLOWBOX_PC *parent = box->parents[i];
                if (flowbox_callbacks[parent->type].check_output(flow, parent, box->output_indices[i]))
                    return 0;
            }
            break;
        case 1:
        case 3:
        case 5: {
            i32 required = condition->operation == 1 ? 1 : condition->threshold;
            i32 count = 0;
            for (i = 0; i < box->parent_count; ++i) {
                FLOWBOX_PC *parent = box->parents[i];
                count += flowbox_callbacks[parent->type].check_output(flow, parent, box->output_indices[i]) != 0;
            }
            if (condition->operation == 5) {
                if (count != required)
                    return 0;
            } else if (count < required) {
                return 0;
            }
            break;
        }
        case 4: {
            i32 required = box->parent_count;
            i32 count = 0;
            if ((box->state_flags_high & 4) != 0)
                required -= box->loop_parent_count;
            for (i = 0; i < box->parent_count; ++i) {
                FLOWBOX_PC *parent = box->parents[i];
                count += flowbox_callbacks[parent->type].check_output(flow, parent, box->output_indices[i]) != 0;
            }
            if (count < required)
                return 0;
            break;
        }
        default:
            break;
    }

    if (condition->operation == 4) {
        for (i = 0; i < box->child_count; ++i) {
            ++gizflow_loop_checksum;
            ResetForLoopEx(flow, box, box->children[i], gizflow_loop_checksum);
        }
    }
    (void)frame;
    return 1;
}

i32 __cdecl CheckOutputConditionFlowBox(GIZFLOW_PC *system, FLOWBOX_PC *box, u8 output) {
    u16 flags = FLOWBOX_FLAGS(box);
    u16 saved;
    i32 result;

    if ((flags & 0x200) == 0)
        return (flags & 2) != 0;
    saved = flags & 0x400;
    FLOWBOX_SET_FLAGS(box, flags | 0x400);
    result = flowbox_callbacks[box->type].process(system, box, 0) != 0;
    flags = FLOWBOX_FLAGS(box);
    FLOWBOX_SET_FLAGS(box, (u16)((flags & ~0x400) | saved));
    (void)output;
    return result;
}

void __cdecl CheckIfParentsFinished(GIZFLOW_PC *system, FLOWBOX_PC *box) {
    i32 i;
    for (i = 0; i < box->parent_count; ++i) {
        FLOWBOX_PC *parent = box->parents[i];
        i32 child_index;
        if ((parent->state_flags_low & 0x20) == 0)
            continue;
        for (child_index = 0; child_index < parent->child_count; ++child_index) {
            FLOWBOX_PC *child = parent->children[child_index];
            if ((child->state_flags_low & 0xc0) != 0) {
                if ((FLOWBOX_FLAGS(child) & 0x102) != 2)
                    break;
            } else if ((child->state_flags_low & 1) != 0 && child != box) {
                break;
            }
        }
        if (child_index != parent->child_count)
            continue;
        if ((parent->state_flags_low & 8) != 0) {
            FLOWBOXGIZMODATA_PC *data = (FLOWBOXGIZMODATA_PC *)parent->data;
            i32 j;
            for (j = 0; j < data->gizmo_count; ++j)
                GizmoSetVisibility(system->gizmo_sys, data->gizmos[j]->gizmo, 0, 1);
        } else if ((parent->state_flags_low & 0x10) != 0) {
            FLOWBOXGIZMODATA_PC *data = (FLOWBOXGIZMODATA_PC *)parent->data;
            i32 j;
            for (j = 0; j < data->gizmo_count; ++j)
                GizmoActivate(system->gizmo_sys, data->gizmos[j]->gizmo, 0, 1);
        }
        if ((parent->state_flags_low & 0x40) != 0) {
            parent->state_flags_high &= (u8)~1;
            CheckIfParentsFinished(system, parent);
        }
        parent->state_flags_low &= (u8)~0x20;
    }
}

void __cdecl PerformActionFlowBox(GIZFLOW_PC *system, FLOWBOX_PC *box) {
    FLOWBOXACTION_PC *action;
    if (box == NULL || box->type != 2)
        return;
    for (action = (FLOWBOXACTION_PC *)box->data; action != NULL; action = action->next) {
        if (action->definition != NULL && action->definition->action_fn != NULL)
            action->definition->action_fn(system, box, action->parameters, action->parameter_count);
    }
}

i32 __cdecl CheckOutputActionFlowBox(GIZFLOW_PC *system, FLOWBOX_PC *box, u8 output) {
    (void)system;
    (void)output;
    return (box->state_flags_low >> 1) & 1;
}

i32 __cdecl ProcessGizmoFlowBox(GIZFLOW_PC *system, FLOWBOX_PC *box, u8 frame) {
    FLOWBOXGIZMODATA_PC *data;
    i32 i;
    for (i = 0; i < box->parent_count; ++i) {
        FLOWBOX_PC *parent = box->parents[i];
        if (!flowbox_callbacks[parent->type].check_output(system, parent, box->output_indices[i]))
            return 0;
    }
    CheckIfParentsFinished(system, box);
    if ((box->state_flags_high & 0x10) != 0 || ((box->state_flags_high & 0x40) == 0 && FreePlay == 0) ||
        ((box->state_flags_high & 0x20) == 0 && FreePlay != 0))
        return 1;
    data = (FLOWBOXGIZMODATA_PC *)box->data;
    for (i = 0; i < data->gizmo_count; ++i)
        GizmoActivate(system->gizmo_sys, data->gizmos[i]->gizmo, 1, 1);
    (void)frame;
    return 1;
}

i32 __cdecl ProcessActionFlowBox(GIZFLOW_PC *system, FLOWBOX_PC *box, u8 frame) {
    i32 i;
    for (i = 0; i < box->parent_count; ++i) {
        FLOWBOX_PC *parent = box->parents[i];
        if (!flowbox_callbacks[parent->type].check_output(system, parent, box->output_indices[i]))
            return 0;
    }
    PerformActionFlowBox(system, box);
    (void)frame;
    return 1;
}

void __cdecl ProcessFlowBox(GIZFLOW_PC *system, FLOWBOX_PC *box, u8 frame) {
    FLOWBOXGIZMODATA_PC *data;
    i32 i;
    i32 parent_checks_passed;

    if ((box->state_flags_high & 1) != 0) {
        parent_checks_passed = 1;
        for (i = 0; i < box->parent_count; ++i) {
            FLOWBOX_PC *parent = box->parents[i];
            if (!flowbox_callbacks[parent->type].check_output(system, parent, box->output_indices[i])) {
                parent_checks_passed = 0;
                break;
            }
        }
        data = (FLOWBOXGIZMODATA_PC *)box->data;
        for (i = 0; i < data->gizmo_count; ++i)
            GizmoActivateReverse(system->gizmo_sys, data->gizmos[i]->gizmo, parent_checks_passed ? 0 : 1,
                                 (box->state_flags_low >> 7) & 1, 1);
    }
    if ((box->state_flags_low & 1) == 0 || !flowbox_callbacks[box->type].process(system, box, frame))
        return;

    {
        u16 flags = FLOWBOX_FLAGS(box);
        if ((flags & 0xc00) == 0xc00)
            flags &= (u16)~0x400;
        flags = (u16)((flags & (u16)~1) | 0x802);
        FLOWBOX_SET_FLAGS(box, flags);
    }

    if (box->type == 0) {
        if ((box->state_flags_low & 0x18) != 0) {
            if (box->child_count != 0) {
                box->state_flags_low |= 0x20;
            } else {
                data = (FLOWBOXGIZMODATA_PC *)box->data;
                if ((box->state_flags_low & 8) != 0) {
                    for (i = 0; i < data->gizmo_count; ++i)
                        GizmoSetVisibility(system->gizmo_sys, data->gizmos[i]->gizmo, 0, 1);
                } else if ((box->state_flags_low & 0x10) != 0) {
                    for (i = 0; i < data->gizmo_count; ++i)
                        GizmoActivate(system->gizmo_sys, data->gizmos[i]->gizmo, 0, 1);
                }
                box->state_flags_low &= (u8)~0x20;
            }
        }
        if ((box->state_flags_low & 0xc0) != 0) {
            box->state_flags_low |= 0x20;
            box->state_flags_high |= 1;
            for (i = 0; i < box->parent_count; ++i) {
                FLOWBOX_PC *parent = box->parents[i];
                if (parent->type == 1)
                    parent->state_flags_high |= 2;
                if (parent->type == 0 && (parent->state_flags_low & 0x40) != 0) {
                    parent->state_flags_high |= 1;
                    parent->state_flags_low |= 0x20;
                }
            }
        }
    }
    if (box->type != 0)
        CheckIfParentsFinished(system, box);
    for (i = 0; i < box->child_count; ++i) {
        FLOWBOX_PC *child = box->children[i];
        child->state_flags_low |= 1;
        if (child->last_process_frame != frame || box->type == 1) {
            child->last_process_frame = frame;
            ProcessFlowBox(system, child, frame);
        }
    }
}

void __cdecl ProcessGizFlow(GIZFLOW_PC *system, f32 delta_time) {
    FLOWBOX_PC *box;
    i32 i;
    (void)delta_time;
    if (system == NULL)
        return;
    ++system->frame;
    box = system->flowboxes;
    for (i = 0; i < system->flowbox_count; ++i, ++box) {
        if (box->last_process_frame != system->frame && (FLOWBOX_FLAGS(box) & 0x101) != 0)
            ProcessFlowBox(system, box, system->frame);
        box->last_process_frame = system->frame;
    }
}

FLOWBOX_PC *__cdecl FlowBoxFindByName(GIZFLOW_PC *system, char *name) {
    FLOWBOX_PC *box;
    i32 i;
    if (system == NULL || name == NULL)
        return NULL;
    box = system->flowboxes;
    for (i = 0; i < system->flowbox_count; ++i, ++box)
        if (NuStrICmp(box->name, name) == 0)
            return box;
    return NULL;
}

void __cdecl ResetGizFlowPointers(GIZFLOW_PC *system) {
    FLOWBOX_PC *box;
    i32 i;
    if (system == NULL || system->flowbox_count == 0)
        return;
    box = system->flowboxes;
    for (i = 0; i < system->flowbox_count; ++i, ++box) {
        FLOWBOXGIZMODATA_PC *data;
        i32 j;
        if (box->type != 0)
            continue;
        data = (FLOWBOXGIZMODATA_PC *)box->data;
        for (j = 0; j < data->gizmo_count; ++j) {
            FLOWBOXGIZMOREF_PC *ref = data->gizmos[j];
            if (ref->gizmo != NULL) {
                GIZMO_PC *gizmo = (GIZMO_PC *)ref->gizmo;
                ref->gizmo = GizmoFindByName(system->gizmo_sys, gizmo->type_id, ref->name);
            } else {
                ref->gizmo = GizmoFindByName(system->gizmo_sys, -1, ref->name);
                if (ref->gizmo != NULL && (box->state_flags_low & 1) == 0) {
                    if (flowbox_callbacks[box->type].reset != NULL)
                        flowbox_callbacks[box->type].reset(system, box);
                    if ((box->state_flags_low & 5) == 4)
                        GizmoSetVisibility(system->gizmo_sys, ref->gizmo, 0, 1);
                }
            }
        }
    }
}

void __cdecl ResetGizFlow(GIZFLOW_PC *system, GIZFLOWPROGRESS_PC *progress) {
    FLOWBOX_PC *box;
    i32 i;
    if (system == NULL)
        return;
    system->frame = 0;
    ResetGizFlowPointers(system);
    box = system->flowboxes;
    for (i = 0; i < system->flowbox_count; ++i, ++box) {
        u16 flags = FLOWBOX_FLAGS(box);
        box->last_process_frame = 0;
        flags &= (u16)~0x20;

        if (progress != NULL && progress->valid != 0) {
            i32 word = i >> 5;
            u32 bit = 1u << (i & 31);
            flags = (u16)((flags & (u16)~1) | ((progress->active[word] & bit) != 0));
            flags = (u16)((flags & (u16)~2) | (((progress->completed[word] & bit) != 0) << 1));
            flags |= (u16)(((progress->latched[word] & bit) != 0) << 5);
            flags = (u16)((flags & (u16)~0x400) | (((progress->output_state[word] & bit) != 0) << 10));
            FLOWBOX_SET_FLAGS(box, flags);
            if (box->type == 1 && ((FLOWBOXCONDITION_PC *)box->data)->operation == 4) {
                ++gizflow_loop_checksum;
                {
                    i32 loop_count;
                    COUNT_LOOPING_INPUTS(box, gizflow_loop_checksum, loop_count);
                    box->loop_parent_count = (u8)loop_count;
                }
            }
            continue;
        }

        flags &= (u16) ~(2 | 0x100);
        FLOWBOX_SET_FLAGS(box, flags);
        if (box->type == 1 && ((FLOWBOXCONDITION_PC *)box->data)->operation == 4) {
            ++gizflow_loop_checksum;
            {
                i32 loop_count;
                COUNT_LOOPING_INPUTS(box, gizflow_loop_checksum, loop_count);
                box->loop_parent_count = (u8)loop_count;
            }
            flags = (u16)(FLOWBOX_FLAGS(box) | 0x400);
            if (box->parent_count == box->loop_parent_count)
                flags |= 0x801;
            else
                flags &= (u16)~1;
            FLOWBOX_SET_FLAGS(box, flags);
        } else {
            flags = FLOWBOX_FLAGS(box);
            flags = (u16)((flags & (u16)~1) | (box->parent_count == 0));
            FLOWBOX_SET_FLAGS(box, flags);
        }

        if (flowbox_callbacks[box->type].reset != NULL && (FLOWBOX_FLAGS(box) & 0x1000) == 0)
            flowbox_callbacks[box->type].reset(system, box);
        if (box->type == 0 && (((FLOWBOX_FLAGS(box) & 0x4000) == 0 && FreePlay == 0) ||
                               ((FLOWBOX_FLAGS(box) & 0x2000) == 0 && FreePlay != 0) ||
                               ((box->state_flags_low & 1) == 0 && (box->state_flags_low & 4) != 0))) {
            FLOWBOXGIZMODATA_PC *data = (FLOWBOXGIZMODATA_PC *)box->data;
            i32 j;
            for (j = 0; j < data->gizmo_count; ++j)
                GizmoSetVisibility(system->gizmo_sys, data->gizmos[j]->gizmo, 0, 1);
        }
    }
}

void *__cdecl GizmoBufferAlloc(VARIPTR *, VARIPTR *, i32);
i32 __cdecl GizmoGetTypeIDByName(void *, char *);

typedef struct GIZFLOW_BOX_LOADER_PREFIX_PC {
    u8 unknown_000[4];
    u8 assist_id;
    u8 unknown_005[5];
    u16 state_flags;
} GIZFLOW_BOX_LOADER_PREFIX_PC;
STATIC_ASSERT(offsetof(GIZFLOW_BOX_LOADER_PREFIX_PC, assist_id) == 4);
STATIC_ASSERT(sizeof(((GIZFLOW_BOX_LOADER_PREFIX_PC *)0)->assist_id) == 1);
STATIC_ASSERT(offsetof(GIZFLOW_BOX_LOADER_PREFIX_PC, state_flags) == 0xa);
STATIC_ASSERT(sizeof(((GIZFLOW_BOX_LOADER_PREFIX_PC *)0)->state_flags) == 2);
static VARIPTR *load_buff;
static VARIPTR *load_endbuff;
static FLOWBOX_PC *load_flowbox;
static i32 load_conditiontype;
static u8 load_conditionParam;
static char load_gizmoname[32];
static GIZFLOW_PC *load_gizflow;
static i32 load_gizmotype;
static i32 load_numgizmos;
static u16 load_g_flags;
static i8 load_t_randomTime;
static float load_t_time;
static i32 load_r_noutputs;
static i32 load_r_outputChance[8];
static i32 load_nflowboxes;
static char load_name[32];
GIZACTIONDEF_PC *gizactiondefs;

static void GizFlowConfigGizmoTypePc(NUFPAR_PC *parser) {

    NuFParGetWord(parser);
    if (parser->word_buffer != NULL)
        load_gizmotype = GizmoGetTypeIDByName(load_gizflow->gizmo_sys, parser->word_buffer);
}
static void GizFlowConfigGizmoNamePc(NUFPAR_PC *parser) {

    NuFParGetWord(parser);
    if (parser->word_buffer != NULL)
        NuStrNCpy(load_gizmoname, parser->word_buffer, 32);
}
static void GizFlowConfigStartInvisPc(NUFPAR_PC *parser) {

    load_g_flags |= 1;
}
static void GizFlowConfigEndInvisPc(NUFPAR_PC *parser) {

    load_g_flags |= 2;
}
static void GizFlowConfigReversePc(NUFPAR_PC *parser) {

    load_g_flags |= 4;
}
static void GizFlowConfigEndDeactPc(NUFPAR_PC *parser) {

    load_g_flags |= 8;
}
static void GizFlowConfigOutputOnlyPc(NUFPAR_PC *parser) {

    load_g_flags = 0x10;
}
static void GizFlowConfigNotStoryModePc(NUFPAR_PC *parser) {

    load_g_flags |= 0x20;
}
static void GizFlowConfigNotFreeplayPc(NUFPAR_PC *parser) {

    load_g_flags |= 0x40;
}
static void GizFlowConfigReverseInvisPc(NUFPAR_PC *parser) {

    load_g_flags |= 0x80;
}
static void GizFlowConfigGizRandomTimePc(NUFPAR_PC *parser) {

    load_t_randomTime = 1;
}
static void GizFlowConfigGizTimerPc(NUFPAR_PC *parser) {

    load_t_time = NuFParGetFloat(parser);
}
static void GizFlowConfigRand_NumOutputsPc(NUFPAR_PC *parser) {

    load_r_noutputs = NuFParGetInt(parser);
}
static void GizFlowConfigRand_OutputChancePc(NUFPAR_PC *parser) {
    i32 index;

    index = NuFParGetInt(parser);
    load_r_outputChance[index] = NuFParGetInt(parser);
}
static NUFPCOMJMP cfgtab_gizmo_pc[] = {
    {"Type", GizFlowConfigGizmoTypePc},
    {"Name", GizFlowConfigGizmoNamePc},
    {"StartInvisible", GizFlowConfigStartInvisPc},
    {"FinishedInvisible", GizFlowConfigEndInvisPc},
    {"FinishedDeactive", GizFlowConfigEndDeactPc},
    {"Reverse", GizFlowConfigReversePc},
    {"RevInvis", GizFlowConfigReverseInvisPc},
    {"NotStoryMode", GizFlowConfigNotStoryModePc},
    {"NotFreeplay", GizFlowConfigNotFreeplayPc},
    {"OutputOnly", GizFlowConfigOutputOnlyPc},
    {"Timer", GizFlowConfigGizTimerPc},
    {"RandomTime", GizFlowConfigGizRandomTimePc},
    {"NumRandomOutputs", GizFlowConfigRand_NumOutputsPc},
    {"RandomOutputChance", GizFlowConfigRand_OutputChancePc},
    {NULL, NULL},
};

static void GizFlowConfigGizmoPc(NUFPAR_PC *parser) {
    FLOWBOXGIZMODATA_PC *data;
    i32 name_size;

    if (load_flowbox == NULL)
        return;
    data = load_flowbox->data;
    load_flowbox->type = 0;
    load_gizmotype = -1;
    load_g_flags = 0;
    load_t_randomTime = 0;
    if (data == NULL) {
        load_flowbox->data =
            (FLOWBOXGIZMODATA_PC *)(GizmoBufferAlloc(load_buff, load_endbuff, sizeof(FLOWBOXGIZMODATA_PC)));
        data = load_flowbox->data;
        data->capacity = load_numgizmos;
        data->gizmos = (FLOWBOXGIZMOREF_PC **)(GizmoBufferAlloc(load_buff, load_endbuff,
                                                                load_numgizmos * sizeof(FLOWBOXGIZMOREF_PC *)));
        data->gizmo_count = 0;
    }
    NuStrCpy(load_gizmoname, "");
    NuFParPushCom(parser, cfgtab_gizmo_pc);
    while (NuFParGetLine(parser)) {
        NuFParGetWord(parser);
        if (NuStrICmp(parser->word_buffer, "}") == 0)
            break;
        NuFParInterpretWord(parser);
    }
    NuFParPopCom(parser);
    if (load_gizmotype < 0 || NuStrLen(load_gizmoname) == 0)
        return;
    name_size = NuStrLen(load_gizmoname) + NuStrLen(((GIZMOTYPES_PC *)gizmotypes)->types[load_gizmotype].prefix) + 1;
    data->gizmos[data->gizmo_count] =
        (FLOWBOXGIZMOREF_PC *)(GizmoBufferAlloc(load_buff, load_endbuff, sizeof(FLOWBOXGIZMOREF_PC)));
    data->gizmos[data->gizmo_count]->name = (char *)(GizmoBufferAlloc(load_buff, load_endbuff, name_size));
    NuStrCpy(data->gizmos[data->gizmo_count]->name, load_gizmoname);
    if (load_gizmotype == giztimer_gizmotype_id) {
        data->gizmos[data->gizmo_count]->gizmo = createGizTimer(NULL, load_t_time, load_t_randomTime, load_gizmoname);
    } else if (load_gizmotype == gizrandom_gizmotype_id) {
        data->gizmos[data->gizmo_count]->gizmo =
            createGizRandom(NULL, load_r_noutputs, load_r_outputChance, load_gizmoname);
    } else if (load_gizmotype == gizspecial_gizmotype_id) {
        data->gizmos[data->gizmo_count]->gizmo = createGizSpecial(NULL, load_gizmoname);
        if (data->gizmos[data->gizmo_count]->gizmo != NULL) {
            NuStrCpy(data->gizmos[data->gizmo_count]->name,
                     ((GIZMOTYPES_PC *)gizmotypes)->types[load_gizmotype].prefix);
            NuStrNCat(data->gizmos[data->gizmo_count]->name, load_gizmoname,
                      32 - NuStrLen(((GIZMOTYPES_PC *)gizmotypes)->types[load_gizmotype].prefix));
        }
    } else {
        data->gizmos[data->gizmo_count]->gizmo =
            GizmoFindByName(load_gizflow->gizmo_sys, load_gizmotype, load_gizmoname);
        if (data->gizmos[data->gizmo_count]->gizmo == NULL)
            GizFlowMissingNamePc(load_gizmoname, load_gizmotype);
    }
    ++data->gizmo_count;
    load_flowbox->state_flags_low = (load_flowbox->state_flags_low & ~0xdc) | ((load_g_flags & 1) << 2) |
                                    ((load_g_flags & 2) << 2) | ((load_g_flags & 8) << 1) | ((load_g_flags & 4) << 4) |
                                    (load_g_flags & 0x80);
    load_flowbox->state_flags_high = (load_flowbox->state_flags_high & ~0x70) | (load_g_flags & 0x10) |
                                     (((load_g_flags >> 5) ^ 1) & 1) << 6 | (((load_g_flags >> 6) ^ 1) & 1) << 5;
}

typedef struct GIZFLOW_REMAP_PC GIZFLOW_REMAP_PC;
struct GIZFLOW_REMAP_PC {
    i16 parents[16];
    i16 children[32];
    u8 parent_outputs[16];
};
STATIC_ASSERT(sizeof(GIZFLOW_REMAP_PC) == 0x70);
STATIC_ASSERT(offsetof(GIZFLOW_REMAP_PC, children) == 0x20);
STATIC_ASSERT(offsetof(GIZFLOW_REMAP_PC, parent_outputs) == 0x60);
static GIZFLOW_REMAP_PC *remap;
static i32 numRemaps;
static i32 load_nparents;
static i32 load_nchildren;
static i32 load_parents[16];
static i32 load_children[32];
static u8 load_parent_output_ix[16];

static void GizFlowConfigRemapChildrenPc(i32 id) {
    i32 i;
    i32 child;

    for (i = 0; load_nchildren < 32 && remap[~id].children[i] != id; ++i) {
        child = remap[~id].children[i];
        if (child < 0)
            GizFlowConfigRemapChildrenPc(child);
        else
            load_children[load_nchildren++] = child;
    }
}

static void GizFlowConfigRemapParentPc(i32 id) {
    i32 i;
    i32 parent;

    for (i = 0; load_nparents < 16 && remap[~id].parents[i] != id; ++i) {
        parent = remap[~id].parents[i];
        if (parent < 0)
            GizFlowConfigRemapParentPc(parent);
        else {
            load_parents[load_nparents] = parent;
            load_parent_output_ix[load_nparents] = remap[~id].parent_outputs[i];
            ++load_nparents;
        }
    }
}

static void GizFlowConfigChildPc(NUFPAR_PC *parser) {
    i32 child;

    if (load_nchildren < 32) {
        child = NuFParGetInt(parser);
        if (child < 0)
            GizFlowConfigRemapChildrenPc(child);
        else
            load_children[load_nchildren++] = child;
    }
}

static void GizFlowConfigParentPc(NUFPAR_PC *parser) {
    i32 parent;
    i32 index;

    if (load_nparents < 16) {
        parent = NuFParGetInt(parser);
        if (parent < 0) {
            NuFParGetInt(parser);
            GizFlowConfigRemapParentPc(parent);
        } else {
            index = load_nparents;
            load_parents[index] = parent;
            load_parent_output_ix[index] = NuFParGetInt(parser);
            ++load_nparents;
        }
    }
}

static void GizFlowConfigChildCollapsePc(NUFPAR_PC *parser) {
    GIZFLOW_REMAP_PC *entry;
    i32 index;
    i32 value;

    if (load_nchildren <= 31) {
        value = NuFParGetInt(parser);
        entry = &remap[numRemaps];
        index = load_nchildren;
        entry->children[index] = value;
        ++load_nchildren;
    }
}

static void GizFlowConfigParentCollapsePc(NUFPAR_PC *parser) {
    GIZFLOW_REMAP_PC *entry;
    i32 index;
    i32 value;

    if (load_nparents <= 15) {
        value = NuFParGetInt(parser);
        entry = &remap[numRemaps];
        index = load_nparents;
        entry->parents[index] = value;
        value = NuFParGetInt(parser);
        entry = &remap[numRemaps];
        index = load_nparents;
        entry->parent_outputs[index] = value;
        ++load_nparents;
    }
}

static NUFPCOMJMP cfgtab_collapse_pc[] = {
    {"Parent", GizFlowConfigParentCollapsePc},
    {"Child", GizFlowConfigChildCollapsePc},
    {NULL, NULL},
};

static void GizFlowConfigCollapsePc(NUFPAR_PC *parser) {
    GIZFLOW_REMAP_PC *entry;
    i32 sentinel;

    load_nparents = 0;
    load_nchildren = 0;
    NuFParPushCom(parser, cfgtab_collapse_pc);
    while (NuFParGetLine(parser)) {
        NuFParGetWord(parser);
        if (NuStrICmp(parser->word_buffer, "}") == 0)
            break;
        NuFParInterpretWord(parser);
    }
    NuFParPopCom(parser);
    entry = &remap[numRemaps];
    sentinel = ~numRemaps;
    ++numRemaps;
    entry->parents[load_nparents] = sentinel;
    entry->children[load_nchildren] = sentinel;
}

static void GizFlowConfigSumBoxPc(NUFPAR_PC *parser) {

    load_conditionParam = NuFParGetInt(parser);
}

typedef struct GIZFLOW_CONDITION_TYPE_PC GIZFLOW_CONDITION_TYPE_PC;
struct GIZFLOW_CONDITION_TYPE_PC {
    i32 type;
    char *name;
    void(__cdecl *load)(NUFPAR_PC *);
};
static GIZFLOW_CONDITION_TYPE_PC gizflow_condition_types_pc[] = {
    {0, "All", NULL},  {1, "Any", NULL},
    {2, "None", NULL}, {3, "Sum", GizFlowConfigSumBoxPc},
    {4, "loop", NULL}, {5, "exactly", GizFlowConfigSumBoxPc},
    {-1, NULL, NULL},
};
STATIC_ASSERT(sizeof(GIZFLOW_CONDITION_TYPE_PC) == 12);

static void GizFlowConfigConditionTypePc(NUFPAR_PC *parser) {
    i32 index;
    GIZFLOW_CONDITION_TYPE_PC *type;

    NuFParGetWord(parser);
    index = 0;
    type = gizflow_condition_types_pc;
    while (load_conditiontype == -1) {
        if (NuStrICmp(type->name, parser->word_buffer) == 0) {
            load_conditiontype = index;
            if (type->load != NULL)
                type->load(parser);
        }
        ++index;
        ++type;
        if (type->type == -1)
            break;
    }
}

static void GizFlowConfigMonitorInputsPc(NUFPAR_PC *parser) {

    ((GIZFLOW_BOX_LOADER_PREFIX_PC *)load_flowbox)->state_flags |= 0x200;
}

static NUFPCOMJMP cfgtab_condition_pc[] = {
    {"Type", GizFlowConfigConditionTypePc},
    {"MonitorInputs", GizFlowConfigMonitorInputsPc},
    {NULL, NULL},
};

static void GizFlowConfigConditionPc(NUFPAR_PC *parser) {
    u8 *condition;

    if (load_flowbox == NULL)
        return;
    load_flowbox->type = 1;
    load_conditiontype = -1;
    load_conditionParam = 0xff;
    NuStrCpy(load_gizmoname, "");
    NuFParPushCom(parser, cfgtab_condition_pc);
    while (NuFParGetLine(parser)) {
        NuFParGetWord(parser);
        if (NuStrICmp(parser->word_buffer, "}") == 0)
            break;
        NuFParInterpretWord(parser);
    }
    NuFParPopCom(parser);
    if (load_conditiontype >= 0) {
        condition = (u8 *)(GizmoBufferAlloc(load_buff, load_endbuff, 4));
        if (condition != NULL) {
            condition[0] = load_conditiontype;
            condition[1] = load_conditionParam;
        }
        load_flowbox->data = condition;
    }
}

static void GizFlowConfigActionPc(NUFPAR_PC *parser) {
    i32 i;
    FLOWBOXACTION_PC *previous;
    char parameters[16][64];
    GIZACTIONDEF_PC *definition;
    FLOWBOXACTION_PC *action;
    i32 count;
    char **destination;
    VARIPTR *end;
    VARIPTR *buffer;
    i32 length;
    char *parameter;

    if (load_flowbox == NULL)
        return;
    load_flowbox->type = 2;
    previous = NULL;

    while (NuFParGetLine(parser)) {
        NuFParGetWord(parser);
        if (NuStrICmp(parser->word_buffer, "}") == 0)
            break;
        definition = gizactiondefs;
        if (definition == NULL)
            continue;
        while (definition->name != NULL && NuStrICmp(parser->word_buffer, definition->name) != 0)
            ++definition;
        if (definition->name == NULL)
            continue;
        action = (FLOWBOXACTION_PC *)(GizmoBufferAlloc(load_buff, load_endbuff, sizeof(FLOWBOXACTION_PC)));
        if (action == NULL)
            continue;
        action->next = NULL;
        action->parameters = NULL;
        action->parameter_count = 0;
        action->definition = NULL;
        if (previous != NULL)
            previous->next = action;
        else
            load_flowbox->data = action;
        action->definition = definition;
        count = 0;
        while (NuFParGetWord(parser)) {
            while (NuStrCmp(parser->word_buffer, "\\") == 0) {
                NuFParGetLine(parser);
                if (!NuFParGetWord(parser))
                    goto parameters_done;
            }
            NuStrCpy(parameters[count++], parser->word_buffer);
        }
    parameters_done:
        if (count != 0) {
            action->parameters = (char **)(GizmoBufferAlloc(load_buff, load_endbuff, count * sizeof(char *)));
            if (action->parameters != NULL) {
                action->parameter_count = count;
                for (i = 0; i < count; ++i) {
                    destination = &action->parameters[i];
                    end = load_endbuff;
                    buffer = load_buff;
                    length = NuStrLen(parameters[i]);
                    parameter = NULL;
                    if (length != 0) {
                        parameter = (char *)(GizmoBufferAlloc(buffer, end, length + 1));
                        NuStrCpy(parameter, parameters[i]);
                    }
                    *destination = parameter;
                }
            }
        }
        previous = action;
    }
}

void RegisterGizActions(GIZACTIONDEF_PC *definitions) {
    gizactiondefs = definitions;
}

static void GizFlowConfigNamePc(NUFPAR_PC *parser) {

    NuFParGetWord(parser);
    if (parser->word_buffer != NULL)
        NuStrNCpy(load_name, parser->word_buffer, 32);
}
static void GizFlowConfigNumGizmosPc(NUFPAR_PC *parser) {

    load_numgizmos = NuFParGetInt(parser);
}
static void GizFlowConfigAIAssistIDPc(NUFPAR_PC *parser) {
    i32 value;
    value = NuFParGetInt(parser);
    ((GIZFLOW_BOX_LOADER_PREFIX_PC *)load_flowbox)->assist_id = (u8)value;
}
static void GizFlowConfigFlowBoxCountPc(NUFPAR_PC *parser) {

    ++load_nflowboxes;
}
static NUFPCOMJMP cfgtab_flowbox_pc[] = {
    {"Parent", GizFlowConfigParentPc},
    {"Child", GizFlowConfigChildPc},
    {"Gizmo", GizFlowConfigGizmoPc},
    {"Condition", GizFlowConfigConditionPc},
    {"Action", GizFlowConfigActionPc},
    {"Name", GizFlowConfigNamePc},
    {"Num_Gizmos", GizFlowConfigNumGizmosPc},
    {"AIAssistId", GizFlowConfigAIAssistIDPc},
    {NULL, NULL},
};

static void GizFlowConfigFlowBoxPc(NUFPAR_PC *parser) {
    i32 i;

    load_nparents = 0;
    load_nchildren = 0;
    load_numgizmos = 1;
    NuFParPushCom(parser, cfgtab_flowbox_pc);
    while (NuFParGetLine(parser)) {
        NuFParGetWord(parser);
        if (NuStrICmp(parser->word_buffer, "}") == 0)
            break;
        NuFParInterpretWord(parser);
    }
    NuFParPopCom(parser);
    if (load_gizflow != NULL && load_flowbox != NULL) {
        if (load_nparents != 0) {
            load_flowbox->parent_count = load_nparents;
            load_flowbox->parents =
                (FLOWBOX_PC **)(GizmoBufferAlloc(load_buff, load_endbuff, load_nparents * sizeof(FLOWBOX_PC *)));
            load_flowbox->output_indices = (u8 *)(GizmoBufferAlloc(load_buff, load_endbuff, load_nparents));
            for (i = 0; i < load_flowbox->parent_count; ++i) {
                load_flowbox->parents[i] = &load_gizflow->flowboxes[load_parents[i]];
                load_flowbox->output_indices[i] = load_parent_output_ix[i];
            }
        }
        if (load_nchildren != 0) {
            load_flowbox->child_count = load_nchildren;
            load_flowbox->children =
                (FLOWBOX_PC **)(GizmoBufferAlloc(load_buff, load_endbuff, load_nchildren * sizeof(FLOWBOX_PC *)));
            for (i = 0; i < load_flowbox->child_count; ++i)
                load_flowbox->children[i] = &load_gizflow->flowboxes[load_children[i]];
        }
        load_flowbox->name = (char *)(GizmoBufferAlloc(load_buff, load_endbuff, strlen(load_name) + 1));
        NuStrCpy(load_flowbox->name, load_name);
    }
    ++load_flowbox;
    --load_nflowboxes;
}

static NUFPCOMJMP cfgtab_gitcount_pc[] = {{"FlowBox", GizFlowConfigFlowBoxCountPc}, {NULL, NULL}};
static NUFPCOMJMP cfgtab_git_pc[] = {
    {"FlowBox", GizFlowConfigFlowBoxPc}, {"Collapse", GizFlowConfigCollapsePc}, {NULL, NULL}};

void *LoadGizFlow(void *world, void *system, char *path, VARIPTR *buffer, VARIPTR *end) {
    GIZFLOW_REMAP_PC remaps[96];
    GIZFLOW_PC *flow;
    NUFILE file;
    NUFPAR_PC *parser;

    numRemaps = 0;
    remap = remaps;
    load_buff = buffer;
    load_endbuff = end;
    load_nflowboxes = 0;
    flow = NULL;
    file = NuFileOpen(path, 0);
    if (file != 0) {
        parser = NuFParOpen(file);
        if (parser != NULL) {
            NuFParPushCom(parser, cfgtab_gitcount_pc);
            while (NuFParGetLine(parser)) {
                NuFParGetWord(parser);
                NuFParInterpretWord(parser);
            }
            NuFreeFPar(parser);
        }
        if (load_nflowboxes != 0) {
            parser = NuFParOpen(file);
            if (parser != NULL) {
                flow = (GIZFLOW_PC *)(GizmoBufferAlloc(load_buff, load_endbuff, sizeof(GIZFLOW_PC)));
                if (flow != NULL) {
                    flow->flowbox_count = load_nflowboxes;
                    flow->gizmo_sys = system;
                    flow->flowboxes =
                        (FLOWBOX_PC *)(GizmoBufferAlloc(load_buff, load_endbuff, load_nflowboxes * sizeof(FLOWBOX_PC)));
                    load_flowbox = flow->flowboxes;
                    load_gizflow = flow;
                    NuFParPushCom(parser, cfgtab_git_pc);
                    while (NuFParGetLine(parser)) {
                        NuFParGetWord(parser);
                        NuFParInterpretWord(parser);
                    }
                }
                NuFreeFPar(parser);
            }
        }
        if (load_nflowboxes != 0)
            GizFlowLoadDiagnosticPc(path);
        NuFileClose(file);
        if (flow != NULL)
            flow->pointers_need_reset = 1;
    }
    load_buff = NULL;
    load_endbuff = NULL;
    load_gizflow = NULL;
    load_flowbox = NULL;
    return flow;
}
