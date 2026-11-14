#ifndef SAGA_NU2API_NUFILE_PC_H_PART
#define SAGA_NU2API_NUFILE_PC_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: nu2api/nufile/nufile.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "nu2api/nufile/nufile.h"
#endif
/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NUFILE_PC_H_PART == 1 && !defined(SAGA_NU2API_NUFILE_PC_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif

    /* NuFileGetInfo result; the AI editor's file browser reads it. */
    typedef struct AI_EDITOR_FILE_INFO_PC {
        u32 flags;
        u32 size;
        i8 created_second;
        i8 created_minute;
        i8 created_hour;
        i8 created_day;
        i8 created_month;
        u8 unknown_0d;
        i16 created_year;
        i8 second;
        i8 minute;
        i8 hour;
        i8 day;
        i8 month;
        u8 unknown_15;
        i16 year;
        char name[256]; /* NuFileGetInfo (PC) only clears name[0] */
    } AI_EDITOR_FILE_INFO_PC;

    STATIC_ASSERT(sizeof(AI_EDITOR_FILE_INFO_PC) == 0x118);
    STATIC_ASSERT(offsetof(AI_EDITOR_FILE_INFO_PC, second) == 0x10);
    STATIC_ASSERT(offsetof(AI_EDITOR_FILE_INFO_PC, year) == 0x16);
    STATIC_ASSERT(offsetof(AI_EDITOR_FILE_INFO_PC, name) == 0x18);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUFILE_PC_H_PART
#define SAGA_NU2API_NUFILE_PC_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUFILE_PC_H_STOP
#endif
#endif

#if SAGA_NU2API_NUFILE_PC_H_PART == 2 && !defined(SAGA_NU2API_NUFILE_PC_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern NUFILE_DEVICE host_device;
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUFILE_PC_H_PART
#define SAGA_NU2API_NUFILE_PC_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUFILE_PC_H_STOP
#endif
#endif

#undef SAGA_NU2API_NUFILE_PC_H_STOP
