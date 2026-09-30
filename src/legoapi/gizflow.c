#include "saga.h"
#include <string.h>

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
        if ((box->state_flags_low & 1) != 0)
            progress->active[word] |= bit;
        if ((box->state_flags_high & 1) != 0)
            progress->triggered[word] |= bit;
        if ((box->state_flags_low & 2) != 0)
            progress->completed[word] |= bit;
        if ((box->state_flags_low & 0x20) != 0)
            progress->latched[word] |= bit;
        if ((((u16)box->state_flags_high << 8) | box->state_flags_low) & 0x400)
            progress->output_state[word] |= bit;
    }
}

void __cdecl ResetGizmoFlowBox(GIZFLOW_PC *system, FLOWBOX_PC *box) {
    FLOWBOXGIZMODATA_PC *data;
    i32 i;

    if ((box->state_flags_high & 0x10) != 0)
        return;

    data = (FLOWBOXGIZMODATA_PC *)box->data;
    for (i = 0; i < data->gizmo_count; ++i) {
        GizmoSetVisibility(system->gizmo_sys, data->gizmos[i]->gizmo, 0, 1);
    }
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
                    STUBBED_NAMED("flowbox reset callback table dispatch at PC 0x0059f356");
                    if ((box->state_flags_low & 5) == 4)
                        GizmoActivate(system->gizmo_sys, ref->gizmo, 0, 1);
                }
            }
        }
    }
}

i32 __cdecl CheckOutputActionFlowBox(GIZFLOW_PC *system, FLOWBOX_PC *box, u8 output) {
    (void)system;
    (void)output;
    return (box->state_flags_low >> 1) & 1;
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
        if (box->last_process_frame != system->frame &&
            ((((u16)box->state_flags_high << 8) | box->state_flags_low) & 0x101) != 0) {
            STUBBED_NAMED("ProcessFlowBox at PC 0x0059f77b");
        }
        box->last_process_frame = system->frame;
    }
}
