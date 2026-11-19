#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/ai/aisys/aiscript_types.h"
#include "legoapi/text_tables.h"
#include "nu2api/nufile/nufile.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern NUDATHDR *StartupDatList[7];
    extern i32 StartupDatCount;
#ifdef __cplusplus
}
#endif

typedef struct HOTH_WORLD_SCRIPT {
    u8 reserved[0x2b00];
    AISCRIPTPROCESS processor;
} HOTH_WORLD_SCRIPT;

typedef struct {
    BUILD_PROGRESS_RECORD *records;
    u16 count;
} BUILD_PROGRESS_SYSTEM;
