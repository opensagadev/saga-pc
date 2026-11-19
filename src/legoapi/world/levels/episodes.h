#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/lib/hub.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 EpCompleteCount;
    extern i32 EpCompleteTotal;
    extern i32 EpBuildUpCount;
    extern i32 EpBuildUpTotal;
    extern i32 EpGoldBrickCount;
    extern i32 EpGoldBrickTotal;
    i32 Episodes_Completed(void);
    void EpisodesConfigDiagnostic(char *path);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
i32 Episode_CountOpenAreas(i32, i32, COLLECTION_AREA_SAVE *);
#endif
