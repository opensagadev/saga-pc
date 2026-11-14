#ifndef SAGA_LEGOAPI_WORLD_AREA_H
#define SAGA_LEGOAPI_WORLD_AREA_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/lib/hub.h, legoapi/world/level.h (not all of it earlier in the shim). */

#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "legoapi/lib/hub.h"
#include "legoapi/world/level.h"
#endif

typedef struct area_fixup_pc_entry_s {
    char *name;
    AREA_DATA_PC **area;
} AREAFIXUP_PC;

STATIC_ASSERT(sizeof(AREAFIXUP_PC) == 8);
STATIC_ASSERT(offsetof(AREAFIXUP_PC, area) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    i32 AreaFromMiniKitID(i32);
    extern AREA_DATA_PC *BONUS_GUNSHIP_ADATA;
    extern AREA_DATA_PC *ADataList;
    extern i32 AREACOUNT;
    AREA_DATA_PC *Area_FindByName(char *name, i32 *index_out);
    LEVELDATA_PC *Area_FindStatusLevel(AREA_DATA_PC *area, i32 *index_out);
    LEVELDATA_PC *Area_FindNextPlayLevel(i32 level_index);
    void Areas_FixUp(AREAFIXUP_PC *fixup);
    void FixUpAreas(void);
    extern u16 AREA_DEFAULTBONUSTIMETRIALTIME;
    extern u16 AREA_DEFAULTCHALLENGETIME;
    extern AREA_DATA_PC *LastAData;
    extern AREA_DATA_PC *HUB_ADATA;
    extern AREA_DATA_PC *VADER_ADATA;
    extern i32 Area;
    extern i32 loadareacharacters_no_backdrop_reset;
#ifdef __cplusplus
}
#endif

#endif
