#include "saga.h"

void __cdecl NuLinkedListAppend(NULISTHDR *list, NULISTLNK *node) {
    node->next = NULL;
    node->prev = list->tail;
    if (list->tail != NULL) {
        list->tail->next = node;
    }
    list->tail = node;
    if (list->head == NULL) {
        list->head = node;
    }
}

void NuLinkedListInsert(NULISTHDR *list, NULISTLNK *node) {
    node->next = list->head;
    node->prev = NULL;

    if (list->head != NULL) {
        list->head->prev = node;
    }

    if (list->tail == NULL) {
        list->tail = node;
    }

    list->head = node;
}

void __cdecl NuLinkedListRemove(NULISTHDR *list, NULISTLNK *node) {
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
}

NULISTLNK *__cdecl NuLinkedListGetHead(NULISTHDR *list) {
    return list->head;
}

NULISTLNK *__cdecl NuLinkedListGetTail(NULISTHDR *list) {
    return list->tail;
}

NULISTLNK *__cdecl NuLinkedListGetNext(NULISTHDR *list, NULISTLNK *node) {
    return node != NULL ? node->next : list->head;
}

NULISTLNK *__cdecl NuLinkedListGetPrev(NULISTHDR *list, NULISTLNK *node) {
    return node != NULL ? node->prev : list->tail;
}

i32 __cdecl NuLinkedListCheck(NULISTHDR *list) {
    NULISTLNK *node;
    i32 count;
    NULISTLNK *previous = NULL;
    count = 0;
    node = list->head;
    while (node != NULL) {
        count++;
        previous = node;
        node = node->next;
    }
    return count;
}

void __cdecl NuLinkedListInsertBefore(NULISTHDR *list, NULISTLNK *position, NULISTLNK *node) {
    if (position != NULL) {
        node->next = position;
        node->prev = position->prev;
        if (position->prev != NULL) {
            position->prev->next = node;
        } else {
            list->head = node;
        }
        position->prev = node;
    } else {
        NuLinkedListInsert(list, node);
    }
}

void __cdecl NuLinkedListInsertAfter(NULISTHDR *list, NULISTLNK *position, NULISTLNK *node) {
    if (position != NULL) {
        node->prev = position;
        node->next = position->next;
        if (position->next != NULL) {
            position->next->prev = node;
        } else {
            list->tail = node;
        }
        position->next = node;
    } else {
        NuLinkedListAppend(list, node);
    }
}
