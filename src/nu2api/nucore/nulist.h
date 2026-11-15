#pragma once

#include "nu2api/nucore/common.h"

typedef struct nulistlnk_s {  // size 0x8
    struct nulistlnk_s *next; // 0x0
    struct nulistlnk_s *prev; // 0x4
} NULISTLNK;

typedef struct nulisthdr_s { // size 0x8
    NULISTLNK *head;         // 0x0
    NULISTLNK *tail;         // 0x4
} NULISTHDR;

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

typedef struct ai_control_slot_s { // size 0xa8
    NULISTLNK links;               // 0x00
    u8 unknown_08[0xa0];           // 0x08
} AI_CONTROL_SLOT;
