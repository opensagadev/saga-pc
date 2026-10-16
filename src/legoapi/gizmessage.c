#include <stdio.h>
#include "saga.h"

void ResetGizAIMessageSys(GIZAIMESSAGESYS *sys) {
    i32 i;
    if (sys == NULL) {
        return;
    }
    sys->free_list.head = NULL;
    sys->free_list.tail = NULL;
    sys->active_list.head = NULL;
    sys->active_list.tail = NULL;
    memset(sys->messages, 0, (u32)sys->count * sizeof(GIZAIMESSAGE));
    for (i = 0; i < sys->count; ++i) {
        NuLinkedListAppend(&sys->free_list, &sys->messages[i].links);
    }
}

GIZAIMESSAGESYS *CreateGizAIMessageSys(VARIPTR *buffer, VARIPTR *buffer_end, i32 count) {
    GIZAIMESSAGESYS *sys = (GIZAIMESSAGESYS *)AISysBufferAlloc(buffer, buffer_end, sizeof(GIZAIMESSAGESYS));
    if (sys != NULL) {
        memset(sys, 0, sizeof(GIZAIMESSAGESYS));
        sys->messages = (GIZAIMESSAGE *)AISysBufferAlloc(buffer, buffer_end, (u32)count * sizeof(GIZAIMESSAGE));
        if (sys->messages != NULL) {
            sys->count = count;
            ResetGizAIMessageSys(sys);
        }
    }
    return sys;
}

void ClearGizAIMessageSys(GIZAIMESSAGESYS *sys) {
    NULISTLNK *node;
    if (sys == NULL) {
        return;
    }
    for (node = NuLinkedListGetHead(&sys->active_list); node != NULL;
         node = NuLinkedListGetNext(&sys->active_list, node)) {
        ((GIZAIMESSAGE *)node)->value = 0.0f;
    }
}

GIZAIMESSAGE *CheckGizAIMessage(GIZAIMESSAGESYS *sys, const char *name, GIZAIMESSAGE *message) {
    char local[32];
    NULISTLNK *node;
    if (sys == NULL) {
        return NULL;
    }
    if (message != NULL) {
        return message;
    }
    if (name == NULL) {
        return NULL;
    }

    if (NuStrIStr((char *)name, gizaimessage_prefix) != NULL) {
        sprintf(local, GizAIMessage_NameFormat, name);
    } else {
        if (NuStrLen(name) + NuStrLen(gizaimessage_prefix) + 1 >= (i32)sizeof(local)) {
            return NULL;
        }
        sprintf(local, GizAIMessage_PrefixedNameFormat, gizaimessage_prefix, name);
    }

    for (node = NuLinkedListGetHead(&sys->active_list); node != NULL;
         node = NuLinkedListGetNext(&sys->active_list, node)) {
        if (NuStrNICmp(local, ((GIZAIMESSAGE *)node)->name, sizeof(local)) == 0) {
            return (GIZAIMESSAGE *)node;
        }
    }

    node = NuLinkedListGetHead(&sys->free_list);
    if (node == NULL) {
        return NULL;
    }
    NuLinkedListRemove(&sys->free_list, node);
    NuLinkedListAppend(&sys->active_list, node);
    NuStrNCpy(((GIZAIMESSAGE *)node)->name, local, sizeof(local));
    return (GIZAIMESSAGE *)node;
}

GIZAIMESSAGE *SetGizAIMessage(GIZAIMESSAGESYS *sys, const char *name, f32 value, GIZAIMESSAGE *message) {
    GIZAIMESSAGE *result = CheckGizAIMessage(sys, name, message);
    if (result != NULL) {
        result->value = value;
    }
    return result;
}

f32 GetGizAIMessage(GIZAIMESSAGESYS *sys, const char *name, GIZAIMESSAGE *message) {
    GIZAIMESSAGE *result = CheckGizAIMessage(sys, name, message);
    return result != NULL ? result->value : 0.0f;
}

GIZAIMESSAGE *QueryGizAIMessage(GIZAIMESSAGESYS *sys, GIZAIMESSAGE *message) {
    if (message != NULL) {
        return (GIZAIMESSAGE *)NuLinkedListGetNext(&sys->active_list, &message->links);
    }
    return (GIZAIMESSAGE *)NuLinkedListGetHead(&sys->active_list);
}

static i32 GizAIMessage_GetMaxGizmos(void *unused0) {
    return 0x40;
}

char *GizAIMessage_GetName(GIZAIMESSAGE *message) {
    return message != NULL ? message->name : NULL;
}

static char *GizAIMessage_GetGizmoName(GIZMO_PC *gizmo) {
    if (gizmo == NULL) {
        return NULL;
    }
    return GizAIMessage_GetName((GIZAIMESSAGE *)gizmo->object);
}

static i32 GizAIMessage_GetOutput(GIZMO_PC *gizmo, i32 output_index, i32 unused2) {
    struct gizaimessage_s *message;
    if ((gizmo == NULL) || (gizmo->object == NULL) || ((u32)output_index > 7)) {
        return 0;
    }

    message = (GIZAIMESSAGE *)gizmo->object;
    return message->value == (f32)message->output_values[output_index];
}

static char *GizAIMessage_GetOutputName(GIZMO_PC *gizmo, i32 output_index) {
    struct gizaimessage_s *message;
    static char returnstr[4] = {0};

    if ((gizmo == NULL) || (gizmo->object == NULL) || ((u32)output_index > 7)) {
        return NULL;
    }

    message = (GIZAIMESSAGE *)gizmo->object;
    sprintf(returnstr, "%d", (i32)message->output_values[output_index]);
    return returnstr;
}

static i32 GizAIMessage_GetNumOutputs(GIZMO_PC *gizmo) {
    if ((gizmo == NULL) || (gizmo->object == NULL)) {
        return 0;
    }
    return ((GIZAIMESSAGE *)gizmo->object)->output_count;
}

ADDGIZMOTYPE_PC *GizAIMessage_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE_PC addtype = {0};

    addtype = Default_ADDGIZMOTYPE;
    addtype.name = "Message";
    addtype.prefix = gizaimessage_prefix;
    addtype.fns.options = 0;
    addtype.fns.get_max_gizmos_fn = GizAIMessage_GetMaxGizmos;
    addtype.fns.add_gizmos_fn = GizAIMessage_AddGizmos;
    addtype.fns.early_update_fn = NULL;
    addtype.fns.late_update_fn = NULL;
    addtype.fns.draw_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = GizAIMessage_GetGizmoName;
    addtype.fns.get_output_fn = GizAIMessage_GetOutput;
    addtype.fns.get_output_name_fn = GizAIMessage_GetOutputName;
    addtype.fns.get_num_outputs_fn = GizAIMessage_GetNumOutputs;
    addtype.fns.activate_fn = NULL;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = NULL;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.allocate_progress_data_fn = NULL;
    addtype.fns.clear_progress_fn = NULL;
    addtype.fns.store_progress_fn = NULL;
    addtype.fns.reset_fn = NULL;
    addtype.fns.reserve_buffer_space_fn = NULL;
    addtype.fns.load_fn = NULL;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    gizaimessage_gizmotype_id = type_id;

    return &addtype;
}
