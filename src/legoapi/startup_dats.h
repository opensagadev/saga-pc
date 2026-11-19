#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/ai/aisys/aiscript_types.h"
#include "legoapi/text_tables.h"
#include "nu2api/nufile/nufile.h"

struct area_gizmoset_s;
struct area_gizmotype_s;

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

typedef struct area_gizmosys_prefix_s { // size 0x4
    struct area_gizmoset_s *sets;       // 0x0
} AREA_GIZMOSYS_PREFIX;

typedef struct area_gizmotypes_s {  // size 0xc
    i32 count;                      // 0x0
    i32 reserved_04;                // 0x4
    struct area_gizmotype_s *types; // 0x8
} AREA_GIZMOTYPES;

typedef struct {
    BUILD_PROGRESS_RECORD *records;
    u16 count;
} BUILD_PROGRESS_SYSTEM;
