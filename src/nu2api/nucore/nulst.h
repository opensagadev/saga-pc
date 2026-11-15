#pragma once

#include "nu2api/nucore/common.h"

typedef struct nulsthdr_s NULSTHDR;

typedef struct nulnkhdr_s {   // size 0x10
    struct nulsthdr_s *owner; // 0x0
    struct nulnkhdr_s *next;  // 0x4
    struct nulnkhdr_s *prev;  // 0x8
    u16 id;                   // 0xc
    u16 is_used : 1;          // 0xe:0
} NULNKHDR;

struct nulsthdr_s {         // size 0x20
    NULNKHDR *free;         // 0x00
    NULNKHDR *free_tail;    // 0x04
    NULNKHDR *head;         // 0x08
    NULNKHDR *tail;         // 0x0c
    u16 element_count;      // 0x10
    u16 element_size;       // 0x12
    u16 element_size_total; // 0x14
    i16 used_count;         // 0x16
    u32 safe_thread;        // 0x18
    u16 padding[2];         // 0x1c
};

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
