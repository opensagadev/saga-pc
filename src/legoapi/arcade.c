#include "saga.h"

i32 __cdecl Arcade_GetMode(u32 *flags) {
    if (Arcade == 0) {
        if (flags != NULL) {
            *flags = 0;
        }
        return -1;
    }
    if (flags != NULL) {
        *flags = Arcade_Mode[ArcadeItem.mode].flags;
    }
    return ArcadeItem.mode;
}

void Arcade_ResetPanel() {
    Arcade_NeedTwoPlayers_Scale = 1.0f;
}

typedef struct ARCADE_COIN_PACKET_PC {
    u32 coins;
    f32 scale;
} ARCADE_COIN_PACKET_PC;
STATIC_ASSERT(offsetof(ARCADE_COIN_PACKET_PC, coins) == 0);
STATIC_ASSERT(offsetof(ARCADE_COIN_PACKET_PC, scale) == 4);
i32 __cdecl Arcade_BothPlayersActive(void) {
    return Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0 && Player[1] != NULL &&
           (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0;
}

void __cdecl Arcade_AwardPoint(i32 player_index, i32 reset_score, i32 unused) {
    ADD_GAME_MESSAGE_PC message = PCArcadeMessageTemplate;
    NUVEC position;
    char text[16];
    i32 earned, goal, icon, character_id;
    LEVEL_OBJECT_RUNTIME_PC *special;
    union {
        u32 bits;
        f32 value;
    } priority;
    (void)unused;
    priority.bits = 1;
    if (reset_score != 0) {
        Arcade_Score[player_index] = 0;
        earned = 1;
    } else {
        if (BonusWinner != -1)
            return;
        if (Arcade_BothPlayersActive()) {
            goal = PCArcadePointGoals[(i8)ArcadeItem.play];
            Arcade_Points[player_index]++;
            if (goal > 0 && Arcade_Points[player_index] >= goal) {
                SetBonusWinner(player_index);
                BonusWinFlag = 0;
                NewMenu(11, -1, -1);
                Arcade_Score[player_index]++;
            }
            earned = 1;
        } else {
            GameAudio_PlaySfx(0x32, NULL, 0, 0);
            NewRumbleAllPlayers(0.0f, 0.2f, 0, priority.value);
            earned = 0;
        }
    }
    if (earned != 0) {
        GameCam_Judder(NULL, qrand() < 0x8000 ? -0.6f : 0.6f, 2, 0);
        NewRumbleAllPlayers(1.0f, 0.1f, 0, priority.value);
        GameCam_NewShake(GameCam, 1.0f, 1.0f, 1.0f);
        GameAudio_PlaySfx(0x2c, NULL, 0, 0);
        GameAudio_PlaySfx(0x26, NULL, 0, 0);
    }
    NuSPrintf(text, Text_TimeS, reset_score != 0 ? Arcade_Score[player_index] : Arcade_Points[player_index]);
    position.x = player_index == 0 ? -0.015f : 0.015f;
    message.unknown_4f = player_index == 0 ? 8 : 2;
    position.y = PCArcadePointMessageY;
    position.z = 1.0f;
    message.text = text;
    message.position = message.target_position = &position;
    message.scale = 0.7f;
    message.target_scale = 2.1f;
    message.flags = 0x4028;
    message.duration = 2.0f;
    AddGameMsg(&message);
    if (reset_score != 0)
        return;
    if (earned == 0) {
        Arcade_NeedTwoPlayers_Scale = 1.5f;
        return;
    }
    character_id = ((GAMEOBJECT *)Player[player_index])->id;
    icon = CDataList[character_id].icon_model;
    if (icon == -1)
        return;
    if (GCDataList[character_id].weapon_action == 0 && icon != PCArcadeUnpairedIconModel)
        icon++;
    position.x = 0.0f;
    position.y = PCArcadeIconMessageY;
    position.z = 1.0f;
    message = PCArcadeMessageTemplate;
    message.scale = 0.333f;
    message.target_scale = 0.555f;
    message.duration = 2.5f;
    message.flags = 0x14120;
    message.position = &position;
    message.special = &WORLD->level_objects[icon];
    AddGameMsg(&message);
    special = &WORLD->level_objects[player_index == 0 ? 0xa6 : 0xa5];
    if (special->active != 0) {
        message.special = special;
        AddGameMsg(&message);
    }
}

void __cdecl Arcade_CoinCollected(i32 player_index, u32 *score, u32 previous_score) {
    GAMEOBJECT *object;
    ARCADE_COIN_PACKET_PC *packet;
    if ((u32)player_index > 1 || (Arcade_Mode[(i8)ArcadeItem.mode].flags & 4) == 0)
        return;
    if (Arcade_BothPlayersActive()) {
        object = (GAMEOBJECT *)Player[player_index];
        if (object == NULL || object->coin_packet == NULL)
            return;
        packet = (ARCADE_COIN_PACKET_PC *)object->coin_packet;
        if (packet->coins < (u32)Arcade_Mode[(i8)ArcadeItem.mode].target)
            return;
        Arcade_AwardPoint(player_index, 0, 0);
        object = (GAMEOBJECT *)Player[0];
        if (object != NULL && object->coin_packet != NULL)
            ((ARCADE_COIN_PACKET_PC *)object->coin_packet)->coins = 0;
        object = (GAMEOBJECT *)Player[1];
        if (object != NULL && object->coin_packet != NULL)
            ((ARCADE_COIN_PACKET_PC *)object->coin_packet)->coins = 0;
        object = (GAMEOBJECT *)Player[player_index == 0 ? 1 : 0];
        if (object != NULL && object->coin_packet != NULL)
            ((ARCADE_COIN_PACKET_PC *)object->coin_packet)->scale = 1.5f;
    } else {
        Arcade_NeedTwoPlayers_Scale = 1.25f;
        *score = previous_score;
    }
}

void __cdecl Arcade_PlayerKilled(i32 player_index, i32 extra) {
    if ((u32)player_index > 1 || (Arcade_Mode[ArcadeItem.mode].flags & 1) == 0)
        return;
    if (Arcade_BothPlayersActive()) {
        ++AreaGlobals_PC.arcade_player_kills[player_index];
        Arcade_AwardPoint(player_index, 0, extra);
    } else {
        Arcade_AwardPoint(player_index, 0, 0);
    }
}

extern const char ArcadeAiKilledSoundNamePc[];
void __cdecl Arcade_AIKilled(i32 player_index) {
    GAMEOBJECT *object;
    if ((u32)player_index > 1 || (Arcade_Mode[(i8)ArcadeItem.mode].flags & 2) == 0)
        return;
    if (Arcade_BothPlayersActive()) {
        ++AreaGlobals_PC.arcade_ai_kills[player_index];
        PlaySfx((char *)ArcadeAiKilledSoundNamePc, NULL);
        object = (GAMEOBJECT *)Player[player_index];
        if (object != NULL && object->coin_packet != NULL)
            ((ARCADE_COIN_PACKET_PC *)object->coin_packet)->scale = 1.5f;
        if (AreaGlobals_PC.arcade_ai_kills[player_index] >= (u32)Arcade_Mode[(i8)ArcadeItem.mode].target) {
            Arcade_AwardPoint(player_index, 0, 0);
            AreaGlobals_PC.arcade_ai_kills[0] = 0;
            AreaGlobals_PC.arcade_ai_kills[1] = 0;
            object = (GAMEOBJECT *)Player[player_index == 0 ? 1 : 0];
            if (object != NULL && object->coin_packet != NULL)
                ((ARCADE_COIN_PACKET_PC *)object->coin_packet)->scale = 1.5f;
        }
    } else {
        GameAudio_PlaySfx(0x32, NULL, 0, 0);
        object = (GAMEOBJECT *)Player[player_index];
        if (object != NULL && object->coin_packet != NULL)
            ((ARCADE_COIN_PACKET_PC *)object->coin_packet)->scale = 1.5f;
        Arcade_NeedTwoPlayers_Scale = 1.5f;
    }
}
void __cdecl Arcade_Kill(i32 player_index, i32 killed_player) {
    if (Arcade != 0 && (u32)player_index <= 1) {
        if ((u32)killed_player <= 1)
            Arcade_PlayerKilled(player_index, 0);
        else if (killed_player == -1)
            Arcade_AIKilled(player_index);
    }
}
