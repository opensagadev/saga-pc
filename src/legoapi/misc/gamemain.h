#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/core/input/timer.h"

struct FRAME_PAD;

#ifdef __cplusplus
extern "C" {
#endif
    extern struct FRAME_PAD **Game_NuPad;
    void InitOnce(i32 argc, char **argv);
    extern f32 DEFAULTFRAMETIME;
    extern f32 FRAMETIME;
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern i32 app_tbgameset;
extern i32 app_tbplayerset;
extern i32 app_tbaiset;
extern i32 app_tbdrawset;
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern TIMER PauseTimer;
    extern TIMER BonusTimer;
#ifdef __cplusplus
}
#endif
