#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct add_game_message_s;
struct game_message_s;

struct nuhspecial_s;

#ifdef __cplusplus
extern "C" {
#endif
    void AddGameMsgCount(NUVEC *position, i32 count, i32 total, u8 red, u8 green, u8 blue, f32 value);
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

#ifdef __cplusplus
extern "C" {
#endif
    i32 FindGameMsgsWithID(i32, i32, i32, struct game_message_s *);
    void EndScoreMessage(struct game_message_s *);
    void GameMsg_DrawAdjustNewPos_CoinToTotal(struct game_message_s *);
    void EndScoreMessage(struct game_message_s *message);
    void GameMsg_DrawAdjustNewPos_CoinToTotal(struct game_message_s *message);

    struct game_message_s *AddGameMsg(struct add_game_message_s *message);
    extern struct nuhspecial_s *(*GameMsg_GetExtraObjFn)(struct game_message_s *message);
#ifdef __cplusplus
}
#endif
