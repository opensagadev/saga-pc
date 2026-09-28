#include "saga.h"

NULNKHDR *__cdecl NuLstAllocTail(NULSTHDR *list) {
    u32 current_thread;
    NULNKHDR *node;

    if (list->free != NULL) {
        current_thread = nu_current_thread_id;

        if (list->safe_thread != current_thread) {
            NuThreadDisableThreadSwap();
        }

        node = list->free;
        list->free = list->free->next;

        if (list->free == NULL) {
            list->free_tail = NULL;
        } else {
            list->free->prev = NULL;
        }

        node->prev = list->tail;

        if (list->tail != NULL) {
            list->tail->next = node;
        } else {
            list->head = node;
        }

        node->next = NULL;

        list->tail = node;

        node->is_used = 1;
        list->used_count++;

        if (list->safe_thread != current_thread) {
            NuThreadEnableThreadSwap();
        }

        return node + 1;
    }

    return NULL;
}