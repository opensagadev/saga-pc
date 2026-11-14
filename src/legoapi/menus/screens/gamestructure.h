#ifndef SAGA_LEGOAPI_MENUS_SCREENS_GAMESTRUCTURE_H
#define SAGA_LEGOAPI_MENUS_SCREENS_GAMESTRUCTURE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/world/world.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    i32 InStory(void);
    void RestoreOptions(void);
    void NewGameMode(void);
    extern i32 reset_load;
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

// These retain C++ linkage and are not exposed as C declarations.
void ClearPause(void);
#endif
#ifdef __cplusplus
extern "C" {
#endif
    i32 Game_100PercentComplete(void);
    extern void (*Game_100PercentFn)(void);
    extern void (*Game_AllGoldBricksFn)(void);

    /* Pause callbacks and immediate rectangle rendering. */
    extern void (*ResumeGame_ExtraCodeFn)(void);
    void ResumeGame(i32 play_sound);
#ifdef __cplusplus
}
#endif
typedef struct STATUSCOLLECT_s STATUSCOLLECT_s;

struct STATUSCOLLECT_s {
    u16 completion_points;
    u8 gold_bricks;
    u8 flags;
};

STATIC_ASSERT(sizeof(struct STATUSCOLLECT_s) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 LEGOMENU_PAUSESYNC;
    struct level_data_pc_s *CanSaveAndExit(struct WORLDINFO_s *world);
    void AddToGoldBricks(void);
    void PauseGame(i32);
#ifdef __cplusplus
}
#endif

#endif
