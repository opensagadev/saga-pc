/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

typedef struct nulsthdr_s NULSTHDR;

typedef struct nulnkhdr_s {
    struct nulsthdr_s *owner;
    struct nulnkhdr_s *next;
    struct nulnkhdr_s *prev;
    u16 id;
    u16 is_used : 1;
} NULNKHDR;

STATIC_ASSERT(sizeof(NULNKHDR) == 0x10);

struct nulsthdr_s {
    NULNKHDR *free;
    NULNKHDR *free_tail;
    NULNKHDR *head;
    NULNKHDR *tail;
    u16 element_count;
    u16 element_size;
    u16 element_size_total;
    i16 used_count;
    u32 safe_thread;
    u16 padding[2];
};

STATIC_ASSERT(sizeof(NULSTHDR) == 0x20);
STATIC_ASSERT(offsetof(NULSTHDR, tail) == 0xc);
STATIC_ASSERT(offsetof(NULSTHDR, used_count) == 0x16);
STATIC_ASSERT(offsetof(NULSTHDR, safe_thread) == 0x18);
#ifdef __cplusplus
extern "C" {
#endif
    NULNKHDR *NuLstAllocHead(NULSTHDR *);
    NULNKHDR *NuLstAlloc(NULSTHDR *);
    NULNKHDR *NuLstGetByIdx(NULSTHDR *list, i32 index);
    NULNKHDR *NuLstAllocTail(NULSTHDR *list);
    void NuLstFree(NULNKHDR *node);
    NULNKHDR *NuLstGetNext(NULSTHDR *list, NULNKHDR *node);
    NULSTHDR *NuLstCreate(i32 element_count, i32 element_size);
    NULSTHDR *NuLstCreateBuff(i32, i32, VARIPTR *, VARIPTR, i32);
#ifdef __cplusplus
}
#endif
