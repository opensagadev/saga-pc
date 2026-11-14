#ifndef SAGA_LEGOGAME_GAMESTATUS_H
#define SAGA_LEGOGAME_GAMESTATUS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nucore/common.h (not all of it earlier in the shim). */

#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "nu2api/nucore/common.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 StatusCollectionIconYPc;
    extern f32 StatusCollectionTextYPc;
    extern f32 StatusCollectionYPc;
    extern i32 StatusCollectionAppearSoundPlayedPc;
    extern i32 StatusCollectionMoveSoundPlayedPc;
    extern f32 StatusMiniKitEntranceHeightPc;
    extern f32 StatusMiniKitScalePc;
    extern f32 StatusMiniKitXPc;
    extern f32 StatusMiniKitYPc;
    extern i16 TextIdPc880728;
    extern i16 TextIdPc880f9c;
    extern i16 TextIdPc880720;
    extern i16 TextIdPc880738;
    extern i16 TextIdPc880734;
    extern f32 StatusIconYPc;
    extern f32 STATSPOS2YPc;
    extern i32 StatusDrawPlayerIconsPc;
    extern f32 StatusTrueHeroBarScalePc;
    extern f32 StatusTrueHeroBarYPc;
    extern f32 StatusTrueHeroTextScalePc;
    extern f32 StatusTrueHeroTextYPc;
    extern f32 StatusTrueHeroNextSoundPercentPc;
    extern f32 StatusTrueHeroSpDelayPc;
    extern const char StatusTrueHeroPercentFormatPc[5];
    extern const char StatusTrueHeroFullPercentTextPc[5];
    extern f32 StatusTitleYPc;
    extern f32 StatusSecondaryTitleYPc;
    extern i16 TextIdPc8806e8;
    extern i16 TextIdPc880710;
    extern i16 TextIdPc880730;
    extern i16 TextIdPc88100c;
    extern i16 TextIdPc881014;
    extern i16 TextIdPc88101c;
    extern i16 TextIdPc881024;
    extern i16 TextIdPc88102c;
    extern i16 TextIdPc881034;
    extern i16 TextIdPc88103c;
    extern i16 TextIdPc8810cc;
    extern i16 TextIdPc8810d0;
    extern f32 HudScoreScalePc;
    extern f32 StatusCoinsTotalYPc;
    extern f32 StatusCoinsScalePc;
    extern f32 StatusCoinsTitleScalePc;
    extern f32 StatusCoinsTitleYPc;
    extern f32 StatusTextScalePc;
    extern i32 StatusRedPc;
    extern i32 StatusGreenPc;
    extern i32 StatusBluePc;
    extern f32 StatusAllMiniKitsTitleScalePc;
    extern f32 StatusAllMiniKitsCharacterTextScalePc;
    extern f32 StatusAllMiniKitsXPc;
    extern f32 StatusAllMiniKitsYPc;
    extern f32 StatusAllMiniKitsScalePc;
    extern f32 StatusAllMiniKitsZPc;
    extern f32 StatusAllMiniKitsCountYOffsetPc;
    extern f32 StatusAllMiniKitsCountScalePc;
    extern f32 StatusAllMiniKitsQuestionScalePc;
    extern f32 StatusAllMiniKitsQuestionYOffsetPc;
    extern f32 StatusAllMiniKitsCompleteScalePc;
    extern f32 StatusAllMiniKitsCompleteYOffsetPc;
    extern const char GameAudioName00758838Pc[12];
#ifdef __cplusplus
}
#endif

#endif
