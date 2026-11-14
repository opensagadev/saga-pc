#ifndef SAGA_NU2API_NUCORE_BGPROC_H
#define SAGA_NU2API_NUCORE_BGPROC_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

typedef struct bgprocinfo_s BGPROCINFO;

struct bgprocinfo_s;
typedef void(bgprocdofn)(struct bgprocinfo_s *);

typedef struct bgvar_s {
    i32 type;

    union {
        i32 i32_value;
        u32 u32_value;
        f32 float_value;
        void *pointer_value;
        char *string_value;
    } value;
} BGVAR;

STATIC_ASSERT(sizeof(BGVAR) == 0x8);

struct bgprocinfo_s {
    bgprocdofn *do_fn;
    bgprocdofn *ack_fn;
    i32 work_started : 1;
    i32 unknown_flag_2 : 1;
    i32 padding;
    u8 data[0x100];
    BGVAR *vars;
    i32 var_count;
};

STATIC_ASSERT(sizeof(BGPROCINFO) == 0x118);
STATIC_ASSERT(offsetof(BGPROCINFO, do_fn) == 0);
STATIC_ASSERT(offsetof(BGPROCINFO, ack_fn) == 4);
STATIC_ASSERT(offsetof(BGPROCINFO, data) == 0x10);
STATIC_ASSERT(offsetof(BGPROCINFO, vars) == 0x110);
STATIC_ASSERT(offsetof(BGPROCINFO, var_count) == 0x114);
#ifdef __cplusplus
extern "C" {
#endif
    BGPROCINFO *bgPostRequest(bgprocdofn *do_fn, bgprocdofn *ack_fn, void *data, i32 data_size);
    extern const char GetSfxSwPPadName[7];
    extern i32 bgproc_frozen;
    extern i32 bgproc_freeze;
    extern const char bgproc_resume_event_name[9];
    extern const char bgproc_work_event_name[11];
    void bgProcInit(void);
    i32 bgProcIsBgThread(void);
    BGPROCINFO *bgGetProcActive(void);
#ifdef __cplusplus
}
#endif

#endif
