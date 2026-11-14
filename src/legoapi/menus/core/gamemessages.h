#ifndef SAGA_LEGOAPI_MENUS_CORE_GAMEMESSAGES_H_PART
#define SAGA_LEGOAPI_MENUS_CORE_GAMEMESSAGES_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/gizmo/object/gizmopickup.h, legoapi/gizmos/fx/gizmopickups.h,
 * nu2api/nu3d/nu3d_includes.h, nu2api/nucore/common.h, nu2api/nucore/fixed_width.h, nu2api/numath/nuvec.h (not all of
 * it earlier in the shim). */

#ifndef SAGA_SHIM
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"
#endif
/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_MENUS_CORE_GAMEMESSAGES_H_PART == 1 && !defined(SAGA_LEGOAPI_MENUS_CORE_GAMEMESSAGES_H_STOP)
typedef void (*GAME_MESSAGE_CALLBACK_PC)(GAME_MESSAGE_PC *);
#ifdef __cplusplus
extern "C" {
#endif
    void AddGameMsgCount(NUVEC *position, i32 count, i32 total, u8 red, u8 green, u8 blue, f32 field_0xd4);
    void AddFancyMessage(char *text, f32 x, f32 y, f32 scale, f32 duration, i32 message_id, i32 colour_type);
    extern i32 GameMessageIndex;
    void ResetGameMessages(void);
    void AddFancyMessageRGB(char *arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, i32 arg5, u8 arg6, u8 arg7, u8 arg8);
    void UpdateGameMessages(void);
    void DrawGameMessages(void);
    void TransformGameMessages(NUVEC *, NUVEC *, NUVEC *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_MENUS_CORE_GAMEMESSAGES_H_PART
#define SAGA_LEGOAPI_MENUS_CORE_GAMEMESSAGES_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_MENUS_CORE_GAMEMESSAGES_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_MENUS_CORE_GAMEMESSAGES_H_PART == 2 && !defined(SAGA_LEGOAPI_MENUS_CORE_GAMEMESSAGES_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    i32 FindGameMsgsWithID(i32, i32, i32, GAME_MESSAGE_PC *);
    void EndScoreMessage(GAME_MESSAGE_PC *);
    void GameMsg_DrawAdjustNewPos_CoinToTotal(GAME_MESSAGE_PC *);
    void EndScoreMessage(GAME_MESSAGE_PC *message);
    void GameMsg_DrawAdjustNewPos_CoinToTotal(GAME_MESSAGE_PC *message);
    extern ADD_GAME_MESSAGE_PC AddGameMsg_Default;

    extern GAME_MESSAGE_PC GameMessage[128];
    GAME_MESSAGE_PC *AddGameMsg(ADD_GAME_MESSAGE_PC *message);
    extern NUSPECIALHANDLE *(*GameMsg_GetExtraObjFn)(GAME_MESSAGE_PC *message);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_MENUS_CORE_GAMEMESSAGES_H_PART
#define SAGA_LEGOAPI_MENUS_CORE_GAMEMESSAGES_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_MENUS_CORE_GAMEMESSAGES_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_MENUS_CORE_GAMEMESSAGES_H_STOP
