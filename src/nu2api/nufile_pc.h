#ifndef SAGA_NU2API_NUFILE_PC_H
#define SAGA_NU2API_NUFILE_PC_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

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

#ifdef __cplusplus
extern "C" {
#endif
    extern struct nufile_device_s host_device;
#ifdef __cplusplus
}
#endif

#endif
