/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

struct area_data_pc_s;
struct level_data_pc_s;

typedef struct area_fixup_pc_entry_s {
    char *name;
    struct area_data_pc_s **area;
} AREAFIXUP_PC;

STATIC_ASSERT(sizeof(AREAFIXUP_PC) == 8);
STATIC_ASSERT(offsetof(AREAFIXUP_PC, area) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    i32 AreaFromMiniKitID(i32);
    extern struct area_data_pc_s *BONUS_GUNSHIP_ADATA;
    extern struct area_data_pc_s *ADataList;
    extern i32 AREACOUNT;
    struct area_data_pc_s *Area_FindByName(char *name, i32 *index_out);
    struct level_data_pc_s *Area_FindStatusLevel(struct area_data_pc_s *area, i32 *index_out);
    struct level_data_pc_s *Area_FindNextPlayLevel(i32 level_index);
    void Areas_FixUp(AREAFIXUP_PC *fixup);
    void FixUpAreas(void);
    extern u16 AREA_DEFAULTBONUSTIMETRIALTIME;
    extern u16 AREA_DEFAULTCHALLENGETIME;
    extern struct area_data_pc_s *LastAData;
    extern struct area_data_pc_s *HUB_ADATA;
    extern struct area_data_pc_s *VADER_ADATA;
    extern i32 Area;
    extern i32 loadareacharacters_no_backdrop_reset;
#ifdef __cplusplus
}
#endif
