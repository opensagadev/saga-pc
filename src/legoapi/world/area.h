/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

struct area_data_s;
struct level_data_s;

typedef struct area_fixup_pc_entry_s { // size 0x8
    char *name;                        // 0x0
    struct area_data_s **area;         // 0x4
} AREAFIXUP;

#ifdef __cplusplus
extern "C" {
#endif
    i32 AreaFromMiniKitID(i32);
    extern struct area_data_s *BONUS_GUNSHIP_ADATA;
    extern struct area_data_s *ADataList;
    extern i32 AREACOUNT;
    struct area_data_s *Area_FindByName(char *name, i32 *index_out);
    struct level_data_s *Area_FindStatusLevel(struct area_data_s *area, i32 *index_out);
    struct level_data_s *Area_FindNextPlayLevel(i32 level_index);
    void Areas_FixUp(AREAFIXUP *fixup);
    void FixUpAreas(void);
    extern u16 AREA_DEFAULTBONUSTIMETRIALTIME;
    extern u16 AREA_DEFAULTCHALLENGETIME;
    extern struct area_data_s *LastAData;
    extern struct area_data_s *HUB_ADATA;
    extern struct area_data_s *VADER_ADATA;
    extern i32 Area;
    extern i32 loadareacharacters_no_backdrop_reset;
#ifdef __cplusplus
}
#endif
