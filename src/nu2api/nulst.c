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

NULNKHDR *__cdecl NuLstGetByIdx(NULSTHDR *list, i32 index) {
    NULNKHDR *node = (NULNKHDR *)((u8 *)(list + 1) + (i16)list->element_size_total * index);
    return (node->is_used & 1) != 0 ? node + 1 : NULL;
}

NULSTHDR *__cdecl NuLstCreate(i32 element_count, i32 element_size) {
    NULSTHDR *list;
    i32 element_size_total;
    i32 index;
    NULNKHDR *current;
    NULNKHDR *next;

    element_size_total = element_size + sizeof(NULNKHDR);
    list = (NULSTHDR *)NuAlloc(element_count * element_size_total + sizeof(NULSTHDR), NuLstCreateTag, 0x67);
    if (list == NULL) {
        return NULL;
    }
    list->free = (NULNKHDR *)(list + 1);
    list->head = NULL;
    list->tail = NULL;
    list->element_count = (u16)element_count;
    list->element_size = (u16)element_size;
    list->element_size_total = (u16)element_size_total;
    list->used_count = 0;

    current = list->free;
    next = (NULNKHDR *)((u8 *)current + element_size_total);
    for (index = 1; index < element_count; index++) {
        current->next = next;
        current->id = (u16)(index - 1);
        current->owner = list;
        current = next;
        next = (NULNKHDR *)((u8 *)next + element_size_total);
    }
    current->next = NULL;
    list->free_tail = current;
    current->id = (u16)(element_count - 1);
    current->owner = list;
    list->safe_thread = nu_current_thread_id;
    return list;
}

void NuLstFree(NULNKHDR *node) {
    u32 current_thread;
    NULSTHDR *list;

    node--;
    list = node->owner;

    current_thread = nu_current_thread_id;

    if (list->safe_thread != current_thread) {
        NuThreadDisableThreadSwap();
    }

    if (node->next != NULL) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;
    }

    if (node->prev != NULL) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }

    node->prev = list->free_tail;
    node->next = NULL;

    if (list->free_tail != NULL) {
        list->free_tail->next = node;
    } else {
        list->free_tail = list->free = node;
    }

    list->free_tail = node;

    list->used_count--;

    node->is_used = 0;

    if (list->safe_thread != current_thread) {
        NuThreadEnableThreadSwap();
    }
}

NULNKHDR *NuLstGetNext(NULSTHDR *list, NULNKHDR *node) {
    if (node != NULL) {
        node--;

        if (node->next != NULL) {
            return node->next + 1;
        }
    } else if (list->head != NULL) {
        return list->head + 1;
    }

    return NULL;
}

NULNKHDR *__cdecl NuLstAllocHead(NULSTHDR *list) {
    u32 current_thread;
    NULNKHDR *node;
    if (list->free == NULL)
        return NULL;
    current_thread = nu_current_thread_id;
    if (list->safe_thread != current_thread)
        NuThreadDisableThreadSwap();
    node = list->free;
    list->free = node->next;
    if (list->free == NULL)
        list->free_tail = NULL;
    else
        list->free->prev = NULL;
    node->next = list->head;
    if (list->head != NULL)
        list->head->prev = node;
    else
        list->tail = node;
    node->prev = NULL;
    list->head = node;
    node->is_used = 1;
    ++list->used_count;
    if (list->safe_thread != current_thread)
        NuThreadEnableThreadSwap();
    return node + 1;
}

NULNKHDR *__cdecl NuLstAlloc(NULSTHDR *list) {
    return NuLstAllocHead(list);
}
