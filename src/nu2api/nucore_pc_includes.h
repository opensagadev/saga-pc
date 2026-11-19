#pragma once

#include "nu2api/nucore/common.h"

typedef void (*NUDEBUGMSGFUNCTION)(char *, ...);
typedef void (*NUONLINEACHIEVEMENTCALLBACK)(i32, i32);

#ifdef __cplusplus
extern "C" {
#endif
    void bgProcDataOversize(i32 size);
    void FileDirectoryError(i32 error);
#ifdef __cplusplus
}
#endif
