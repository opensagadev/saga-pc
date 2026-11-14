#ifndef SAGA_LEGOAPI_CORE_CONFIG_FILESELECT_H
#define SAGA_LEGOAPI_CORE_CONFIG_FILESELECT_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/fixed_width.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern u8 FS_Active;
    extern void (*FS_Callback)(char *arg0, char *arg1);
    extern char FS_Filter[256];
    extern char FS_FilterOut[256];
    extern char FS_LastFileName[64];
    extern char FS_Path[256];
    extern u8 FS_RefreshDir;
    extern u8 FS_ShowVolumes;
#ifdef __cplusplus
}
#endif

#endif
