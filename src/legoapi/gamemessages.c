#include <stdio.h>
#include "saga.h"
#include <string.h>
GAME_MESSAGE_PC *__cdecl AddGameMsg(ADD_GAME_MESSAGE_PC *message) {
    GAME_MESSAGE_PC *slot;
    GAME_MESSAGE_PC fallback;
    i32 temporary = 0;
    i32 slot_index = -1;
    f32 best = 1000000.0f;
    f32 value;
    i32 index = GameMessageIndex;
    i32 count, text_index, length;
    for (count = 0; count < 128; ++count) {
        slot = &GameMessage[index];
        if (slot->active == 0) {
            slot_index = index;
            break;
        }
        if (!(slot->flags & 0x1000)) {
            if (slot->expired != 0)
                value = 1.0f;
            else if (slot->elapsed == 0.0f || slot->duration == 0.0f)
                value = 0.0f;
            else
                value = slot->elapsed / slot->duration;
            if (slot_index == -1 || value > best) {
                best = value;
                slot_index = index;
            }
        }
        ++index;
        if (index == 128)
            index = 0;
    }
    if (slot_index == -1 && message->allow_temporary != 0) {
        memset(&fallback, 0, sizeof(fallback));
        slot = &fallback;
        temporary = 1;
    } else {
        if (slot_index != -1)
            GameMessageIndex = slot_index;
        slot = &GameMessage[GameMessageIndex];
    }
    if (slot->active != 0 && slot->score != 0 && (u8)slot->player_index <= 1 && slot->end_fn != NULL)
        slot->end_fn(slot);
    slot->text = NULL;
    text_index = 0;
    if (message->special != NULL) {
        if (!NuSpecialExistsFn((NUSPECIALHANDLE *)message->special))
            return NULL;
    } else {
        if (message->text == NULL)
            return NULL;
        length = NuStrLen(message->text);
        if (message->flags & 0x2000) {
            if (length < 1)
                return NULL;
            slot->text = message->text;
        } else {
            for (; text_index < length && text_index < 0x77; ++text_index)
                slot->text_buffer[text_index] = message->text[text_index];
            if (text_index == 0)
                return NULL;
        }
    }
    slot->text_buffer[text_index] = 0;
    slot->parameter_d4 = 0.0f;
    slot->position = *message->position;
    slot->position_a = slot->position;
    slot->scale_start = message->scale;
    slot->scale_initial = message->scale;
    slot->elapsed = 0.0f;
    slot->duration = (message->flags & 0x80) ? 0.0f : message->duration;
    slot->red = message->red;
    slot->green = message->green;
    slot->blue = message->blue;
    slot->alpha = message->alpha;
    slot->flags = message->flags;
    slot->target_position = message->target_position != NULL ? *message->target_position : *message->position;
    slot->target_scale = message->target_scale;
    if (!(slot->flags & 1)) {
        slot->scale_current = slot->scale_initial;
        slot->start_position = slot->position;
        slot->has_start_position = 1;
        slot->parameter_fb = 0;
    } else
        slot->has_start_position = 0;
    slot->expired = 0;
    slot->active = 1;
    slot->parameter_fc = message->unknown_4f;
    slot->icon = (u16)message->icon;
    if (message->special != NULL)
        slot->special = *(NUSPECIALHANDLE *)message->special;
    else
        memset(&slot->special, 0, sizeof(slot->special));
    slot->player_index = message->player_index;
    slot->score = message->score;
    slot->parameter_c4 = message->unknown_30;
    slot->parameter_c8 = message->unknown_34;
    slot->parameter_e0 = slot->parameter_e2 = slot->parameter_e4 = 0;
    slot->delay_fn = message->delay_fn;
    slot->tick_fn = message->tick_fn;
    slot->update_fn = message->update_fn;
    slot->end_fn = message->end_fn;
    slot->parameter_10c = message->unknown_44;
    slot->allow_temporary = message->allow_temporary;
    slot->target_is_coin_total = message->target_is_coin_total;
    slot->delay = message->delay;
    slot->scale_multiplier = 1.0f;
    if (temporary) {
        if (slot->end_fn != NULL)
            slot->end_fn(slot);
        return NULL;
    }
    return slot;
}

void __cdecl ResetGameMessages(void) {
    i32 i;
    for (i = 0; i < 128; ++i)
        GameMessage[i].active = 0;
}

typedef struct coin_packet_pc_s {
    u32 coins;
    f32 scale;
    i16 last_coin;
    u8 unknown_0a[2];
} COIN_PACKET_PC;
typedef struct object_coin_packet_pc_s {
    u8 unknown_000[0x7fc];
    COIN_PACKET_PC *coin_packet;
} OBJECT_COIN_PACKET_PC;
typedef struct game_save_score_pc_s {
    u8 unknown_0000[0x7c14];
    u32 coins;
} GAME_SAVE_SCORE_PC;
STATIC_ASSERT(offsetof(COIN_PACKET_PC, coins) == 0);
STATIC_ASSERT(offsetof(COIN_PACKET_PC, scale) == 4);
STATIC_ASSERT(offsetof(COIN_PACKET_PC, last_coin) == 8);
STATIC_ASSERT(offsetof(OBJECT_COIN_PACKET_PC, coin_packet) == 0x7fc);
STATIC_ASSERT(offsetof(GAME_SAVE_SCORE_PC, coins) == 0x7c14);
void __cdecl EndScoreMessage(GAME_MESSAGE_PC *message) {
    i32 icon;
    i8 player_index;
    GAMEOBJECT *player;
    COIN_PACKET_PC *packet;
    u32 old_coins;
    WORLDINFO *world;
    if (CoinsGoToMainTotal()) {
        icon = (i16)message->icon;
        if (message->icon != 0xffff) {
            if ((u32)(icon - 0xb7) < 4)
                icon -= 4;
            else if ((u32)(icon - 0xbf) < 4)
                icon -= 4;
            else if ((u32)(icon - 0xc7) < 4)
                icon -= 4;
            else if ((u32)(icon - 0xd5) < 4)
                icon += 4;
            cointotal_i_obj[message->player_index] = icon;
        }
        CoinTotalScale = 1.5f;
        if (SuperStory != 0)
            IncreaseScore(&SuperStoryScore, (u64)message->score, 0);
        else {
            world = WorldInfo_CurrentlyActive();
            if (world->area != NULL && (world->area->flags & 0x100)) {
                if ((u32)BonusCoinTotal < BonusCoinTarget) {
                    IncreaseScore((u32 *)&BonusCoinTotal, (u64)message->score, 0);
                    if ((u32)BonusCoinTotal >= BonusCoinTarget) {
                        BonusCoinTotal = (i32)BonusCoinTarget;
                        SetBonusWinner(message->player_index);
                        BonusWinFlag = 0;
                        NewMenu(20, -1, -1);
                        PlaySfx((char *)ScorePanelName, NULL);
                        ResetGameMessages();
                    }
                }
            } else
                IncreaseScore(&((GAME_SAVE_SCORE_PC *)&Game)->coins, (u64)message->score, 1);
        }
    } else {
        player_index = message->player_index;
        if ((u8)player_index > 1 || Player[player_index] == NULL)
            goto counter_sound;
        player = (GAMEOBJECT *)Player[player_index];
        packet = ((OBJECT_COIN_PACKET_PC *)player)->coin_packet;
        if (packet == NULL)
            goto counter_sound;
        old_coins = packet->coins;
        if (message->icon != 0xffff)
            packet->last_coin = (i16)message->icon;
        if (BonusWinner == -1) {
            IncreaseScore(&packet->coins, (u64)message->score, 1);
            packet->scale = 1.5f;
        }
        if (BuildUpTotal == 0)
            BuildUpScale = 1.5f;
        if (Arcade != 0)
            Arcade_CoinCollected(player_index, &packet->coins, old_coins);
        else if (BonusArea != 0 && BonusWinner == -1 && packet->coins >= 1000000u) {
            packet->coins = 1000000;
            SetBonusWinner(player_index);
            BonusWinFlag = 0;
            NewMenu(19, -1, -1);
            PlaySfx((char *)ScorePanelName, NULL);
            ResetGameMessages();
        }
    }
counter_sound:
    if (DoubleScoreTime > 0.0f)
        PlayRepeatSfx((char *)ScoreCounterName, -1, 0.0f, 2, 0.1f, NULL);
    else
        PlaySfx((char *)ScoreCounterName, NULL);
}

void __cdecl GameMsg_DrawAdjustNewPos_CoinToTotal(GAME_MESSAGE_PC *message) {
    message->target_position.x = cointotal_x[message->player_index];
}

i32 __cdecl FindGameMsgsWithID(i32 id, i32 remove, i32 player, GAME_MESSAGE_PC *exclude) {
    GAME_MESSAGE_PC *message = GameMessage;
    i32 count = 0, index;
    for (index = 0; index < 128; ++index, ++message) {
        if (message->active != 0 && (i8)message->target_is_coin_total == id && message != exclude &&
            (player == -1 || message->player_index == player)) {
            ++count;
            if (remove != 0)
                message->active = 0;
        }
    }
    return count;
}

void AddGameMsgCount(NUVEC *position, i32 count, i32 total, unsigned char red, unsigned char green, unsigned char blue,
                     float field_0xd4) {
    void *message;
    char text[32];
    sprintf(text, "%i/%i", count, total);

    message = position != NULL ? AddGameMessage(text, position, 0.6f, NULL, 0.8f, red, green, blue, 0x4023, 1.0f)
                               : AddGameMessage(text, &v001, 0.6f, NULL, 0.8f, red, green, blue, 0x4020, 1.0f);
    if (message != NULL) {
        *(float *)((char *)(message) + 0xd4) = field_0xd4;
    }
}

void AddFancyMessage(char *text, float x, float y, float scale, float duration, i32 message_id, i32 colour_type) {
    const i32 red = 0xff;
    const i32 green = 0xff - ((((u32)(colour_type)) < 1) << 6);
    const i32 blue = ~(-((i32)(((u32)(colour_type)) < 1))) & 0xff;
    AddFancyMessageRGB(text, x, y, scale, duration, message_id, red, green, blue);
}

GAME_MESSAGE_PC *__cdecl AddGameMessage(char *text, NUVEC *position, f32 scale, NUVEC *target_position,
                                        f32 target_scale, u8 red, u8 green, u8 blue, u32 flags, f32 duration) {
    ADD_GAME_MESSAGE_PC message = AddGameMsg_Default;
    message.text = text;
    message.position = position;
    message.scale = scale;
    message.target_position = target_position;
    message.target_scale = target_scale;
    message.red = red;
    message.green = green;
    message.blue = blue;
    message.flags = flags;
    message.duration = duration;
    return AddGameMsg(&message);
}

/* PC5612f0/267; append to gamemessages.c. Native cdecl3 pointers, all stack. */
void __cdecl TransformGameMessages(NUVEC *camera_position, NUVEC *camera_right, NUVEC *camera_direction) {
    GAME_MESSAGE_PC *message = GameMessage;
    GAME_MESSAGE_PC *end = message + 128;
    for (; message != end; ++message) {
        NUVEC target, transformed;
        union {
            f32 value;
            u32 bits;
        } difference;
        if (message->active == 0 || message->has_start_position != 0)
            continue;
        NuCameraTransformScreenClip(&message->start_position, &message->position, 1, NULL);
        if ((message->position.y - camera_position->y) * camera_direction->y +
                (message->position.x - camera_position->x) * camera_direction->x +
                (message->position.z - camera_position->z) * camera_direction->z <=
            0.0) {
            message->parameter_fb = 1;
        } else {
            message->parameter_fb = 0;
            if ((message->flags & 4) != 0) {
                target.x = message->position.x + message->scale_initial * camera_right->x;
                target.y = message->position.y + message->scale_initial * camera_right->y;
                target.z = message->position.z + message->scale_initial * camera_right->z;
                NuCameraTransformScreenClip(&transformed, &target, 1, NULL);
                difference.value = message->start_position.x - transformed.x;
                difference.bits &= 0x7fffffff;
                message->scale_current = (f32)(difference.value * 0.5249999761581421);
            }
        }
        if ((message->flags & 2) == 0)
            message->has_start_position = 1;
        message->start_position.z = 1.0f;
    }
}
