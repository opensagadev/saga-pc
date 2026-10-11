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

char *GizAIMessage_GetName(GIZAIMESSAGE *message) {
    return message != NULL ? message->name : NULL;
}
