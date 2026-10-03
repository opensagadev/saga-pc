#include "saga.h"
typedef struct lose_coins_packet_pc_s {
    u32 coins;
} LOSE_COINS_PACKET_PC;
STATIC_ASSERT(offsetof(LOSE_COINS_PACKET_PC, coins) == 0);
STATIC_ASSERT(offsetof(GAMEOBJECT, coin_packet) == 0x7fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, game_character) == 0x24);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
i32 __cdecl LoseCoins(GAMEOBJECT *object, i32 cause) {
    LOSE_COINS_PACKET_PC *packet;
    u32 lost = 0;
    i32 adjustment;
    if (Player_HasInvincibility(object) || (object->character_data->game_character->flags_090 & 0x8000) ||
        object->coin_packet == NULL)
        return 0;
    if (cause == 1)
        lost = BonusArea != 0 ? 25000 : 2000;
    else if (cause == 2)
        lost = BonusArea != 0 ? 12500 : 1000;
    if (Cheat_IsOn(0x2c)) {
        adjustment = adtab[adaptivedifficulty.level][0];
        if (adjustment == 1)
            lost += lost;
        else if (adjustment == -1)
            lost >>= 1;
    }
    packet = (LOSE_COINS_PACKET_PC *)object->coin_packet;
    if (lost > packet->coins) {
        lost = packet->coins;
        packet->coins = 0;
    } else if (lost > 0)
        packet->coins -= lost;
    if (Cheats_CheckFlags(0x7c) || DoubleScoreTime > 0.0f)
        return 0;
    return (i32)lost;
}

void __cdecl AddCoinsToPanel(i32 coins, NUVEC *position, i32 player, f32 unused, GAMEOBJECT *owner, i32 random_types) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 remainder, remaining, type, score, i, j, base_model, model, main_total;
    u8 counts[10] = {0};
    NUVEC target;
    f32 target_scale;
    GIZMO_PICKUP_TYPE_PC *pickup;
    ADD_GAME_MESSAGE_PC message;
    (void)unused;
    (void)owner;
    if (coins == 0)
        return;
    if (ChallengeMode != 0 || Mission_Active(NULL) != NULL)
        coins = 0;
    remainder = coins % 10;
    if (remainder > 0)
        coins -= remainder;
    if (coins > 512000)
        coins = 512000;
    if (coins > 0)
        PlaySfx((char *)PickupCoinsLandName, position);
    remaining = coins;
    if (random_types)
        while (remaining > 0) {
            type = CoinTab[qrand() / 16384];
            score = GizmoPickupType[type].score;
            if (remaining - score < 0)
                continue;
            ++counts[type];
            remaining -= score;
        }
    else if (remaining > 0)
        for (i = 3; remaining > 0; --i) {
            type = CoinTab[i];
            score = GizmoPickupType[type].score;
            while (remaining - score >= 0) {
                remaining -= score;
                ++counts[type];
            }
        }
    if (player != 0 && player != 1)
        player = -1;
    target.x = player == 1 ? PANEL_COINX : -PANEL_COINX;
    main_total = CoinsGoToMainTotal() != 0;
    if (main_total) {
        target.y = STATSPOSY;
        target_scale = COINTOTAL_COINSIZE;
    } else {
        target.y = STATSPOSY + PANEL_COINY;
        target_scale = PANEL_COINSCALE_END;
    }
    target.z = 1.0f;
    DrawBuildUpTime = (f32)(COINMSGTIME + 1.0);
    for (i = 0; i < 4; ++i) {
        pickup = &GizmoPickupType[CoinTab[i]];
        base_model = pickup->first_model_id;
        for (j = 0; j < counts[CoinTab[i]]; ++j) {
            model = base_model;
            if (pickup->random_model_count > 0)
                model += qrand() / (65535 / pickup->random_model_count + 1);
            if (world->level_objects[model].active == 0 || player == -1)
                continue;
            message = AddGameMsg_Default;
            message.position = position;
            message.target_position = &target;
            message.target_scale = target_scale;
            message.icon = (i16)model;
            message.special = &world->level_objects[message.icon];
            message.score = pickup->score;
            message.end_fn = EndScoreMessage;
            message.player_index = (i8)player;
            message.allow_temporary = 1;
            message.delay = AddCoinDelay[player];
            message.flags = 0x12d;
            message.scale = 1.0f;
            message.duration = COINMSGTIME;
            if (main_total) {
                message.update_fn = GameMsg_DrawAdjustNewPos_CoinToTotal;
                message.target_is_coin_total = 1;
            }
            AddGameMsg(&message);
            AddCoinDelay[player] = (f32)(AddCoinDelay[player] + 0.10000000149011612);
        }
    }
}

void __cdecl AddPickups(i32 coins, i32 hearts, i32 torpedoes, i32 powerups, NUVEC *position, NUVEC *direction,
                        f32 unused, i32 player_id, f32 speed, f32 lifetime, GAMEOBJECT *owner, i32 random_types,
                        i32 panel) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    const f32 scale = AreaPickupScale;
    u32 arcade_flags;
    i32 to_panel, remainder, remaining, i, j, type, score, count, both_players;
    u8 counts[10] = {0};
    NUVEC velocity;
    GAMEOBJECT *recipient;
    GAMEOBJECT *eligible[2];
    (void)unused;
    Arcade_GetMode(&arcade_flags);
    if (arcade_flags & 0x10)
        coins = torpedoes = 0;
    if (world->area != NULL && (world->area->flags & 0x104) == 4)
        random_types = 0;
    if (world->current_level->flags & 0x80000) {
        to_panel = 1;
        if (player_id == -1) {
            if (player2 != NULL)
                player_id = qrand() / 32768;
            else if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc_word & 0x80))
                player_id = 0;
            else
                player_id = 1;
        }
    } else if (world->area != NULL && (world->area->flags & 0x100)) {
        player_id = qrand() / 32768;
        to_panel = 1;
    } else
        to_panel = player_id == -1 ? 0 : panel;
    if (ChallengeMode != 0 || Mission_Active(NULL) != NULL)
        coins = 0;
    if (speed == 1.0f)
        speed *= scale;
    if (netclient != 0)
        return;
    remainder = coins % 10;
    if (remainder > 0)
        coins -= remainder;
    if (coins > 512000)
        coins = 512000;
    if (coins > 0)
        PlaySfx((char *)PickupCoinsLandName, position);
    remaining = coins;
    if (random_types) {
        while (remaining > 0) {
            type = CoinTab[qrand() / 16384];
            score = GizmoPickupType[type].score;
            if (remaining - score < 0)
                continue;
            ++counts[type];
            remaining -= score;
        }
    } else if (remaining > 0) {
        for (i = 3; remaining > 0; --i) {
            type = CoinTab[i];
            score = GizmoPickupType[type].score;
            while (remaining - score >= 0) {
                remaining -= score;
                ++counts[type];
            }
        }
    }
    if (player_id != 0 && player_id != 1)
        player_id = -1;
    if (to_panel) {
        if (coins > 0)
            DrawBuildUpTime = (f32)(COINMSGTIME + 1.0);
        AddCoinsToPanel(coins, position, player_id, speed, owner, random_types);
    } else {
        for (i = 0; i < 4; ++i) {
            type = CoinTab[i];
            for (j = 0; j < counts[type]; ++j) {
                MakePartVector(&velocity, direction, speed);
                AddCoinsAsParts(type, position, &velocity, lifetime, scale);
            }
        }
    }
    both_players = 0;
    if (hearts > 0 && VehicleArea != 0 && Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc_word & 0x80) &&
        Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc_word & 0x80)) {
        both_players = 1;
        hearts = 2;
    }
    for (i = 0; i < hearts; ++i) {
        if (to_panel)
            velocity = v000;
        else
            MakePartVector(&velocity, direction, speed);
        recipient = NULL;
        if (both_players)
            recipient = (GAMEOBJECT *)Player[i == 0 ? 0 : 1];
        else if (owner != NULL)
            recipient = owner;
        else {
            count = 0;
            if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc_word & 0x80))
                eligible[count++] = (GAMEOBJECT *)Player[0];
            if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc_word & 0x80))
                eligible[count++] = (GAMEOBJECT *)Player[1];
            if (count > 0)
                recipient = eligible[count == 1 ? 0 : qrand() / 32768];
        }
        AddHeartAsPart(recipient, position, &velocity, scale, lifetime);
    }
    for (i = 0; i < torpedoes; ++i) {
        MakePartVector(&velocity, direction, speed);
        AddTorpedoAsPart(position, &velocity, scale, lifetime);
    }
    if (!(world->current_level->flags & 0x80000))
        for (i = 0; i < powerups; ++i) {
            MakePartVector(&velocity, direction, speed);
            PowerUp_AddPart(position, &velocity, scale, lifetime);
        }
}
