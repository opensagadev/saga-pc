#pragma once

#include "nu2api/nucore/common.h"

typedef struct bgprocinfo_s BGPROCINFO;

struct bgprocinfo_s;
typedef void(bgprocdofn)(struct bgprocinfo_s *);

typedef struct bgvar_s { // size 0x8
    i32 type;            // 0x0

    union { // size 0x4
        i32 i32_value;
        u32 u32_value;
        f32 float_value;
        void *pointer_value;
        char *string_value;
    } value; // 0x4
} BGVAR;

struct bgprocinfo_s {      // size 0x118
    bgprocdofn *do_fn;     // 0x000
    bgprocdofn *ack_fn;    // 0x004
    i32 work_started : 1;  // 0x008:0
    i32 work_finished : 1; // 0x008:1
    i32 padding;           // 0x00c
    u8 data[0x100];        // 0x010
    BGVAR *vars;           // 0x110
    i32 var_count;         // 0x114
};

#ifdef __cplusplus
extern "C" {
#endif
    BGPROCINFO *bgPostRequest(bgprocdofn *do_fn, bgprocdofn *ack_fn, void *data, i32 data_size);
    void PcDebugPrintLock(void);
    void PcDebugPrintUnlock(void);
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
