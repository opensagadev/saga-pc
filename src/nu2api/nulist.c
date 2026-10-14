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
