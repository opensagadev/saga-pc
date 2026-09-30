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
        GizmoSetVisibility(system->gizmo_sys, data->gizmos[i]->gizmo, 0, 1);
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
            GizmoSetVisibility(flow->gizmo_sys, data->gizmos[i]->gizmo, 0, 1);
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
                GizmoActivate(system->gizmo_sys, data->gizmos[j]->gizmo, 0, 1);
        } else if ((parent->state_flags_low & 0x10) != 0) {
            FLOWBOXGIZMODATA_PC *data = (FLOWBOXGIZMODATA_PC *)parent->data;
            i32 j;
            for (j = 0; j < data->gizmo_count; ++j)
                GizmoSetVisibility(system->gizmo_sys, data->gizmos[j]->gizmo, 0, 1);
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
                        GizmoActivate(system->gizmo_sys, data->gizmos[i]->gizmo, 0, 1);
                } else if ((box->state_flags_low & 0x10) != 0) {
                    for (i = 0; i < data->gizmo_count; ++i)
                        GizmoSetVisibility(system->gizmo_sys, data->gizmos[i]->gizmo, 0, 1);
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
                        GizmoActivate(system->gizmo_sys, ref->gizmo, 0, 1);
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
                GizmoActivate(system->gizmo_sys, data->gizmos[j]->gizmo, 0, 1);
        }
    }
}
