#ifndef SAGA_NU2API_NUCORE_NULIST_H
#define SAGA_NU2API_NUCORE_NULIST_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

typedef struct nulistlnk_s {
    struct nulistlnk_s *next;
    struct nulistlnk_s *prev;
} NULISTLNK;

typedef struct nulisthdr_s {
    NULISTLNK *head;
    NULISTLNK *tail;
} NULISTHDR;

STATIC_ASSERT(sizeof(NULISTLNK) == 8);
STATIC_ASSERT(offsetof(NULISTLNK, next) == 0);
STATIC_ASSERT(offsetof(NULISTLNK, prev) == 4);
STATIC_ASSERT(sizeof(NULISTHDR) == 8);
STATIC_ASSERT(offsetof(NULISTHDR, head) == 0);
STATIC_ASSERT(offsetof(NULISTHDR, tail) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    void NuLinkedListAppend(NULISTHDR *list, NULISTLNK *node);
    void NuLinkedListRemove(NULISTHDR *list, NULISTLNK *node);
    NULISTLNK *NuLinkedListGetHead(NULISTHDR *list);
    NULISTLNK *NuLinkedListGetTail(NULISTHDR *list);
    NULISTLNK *NuLinkedListGetNext(NULISTHDR *list, NULISTLNK *node);
#ifdef __cplusplus
}
#endif

#endif
