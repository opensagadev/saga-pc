#include <stdio.h>
#include "saga.h"
#include <string.h>
#include <float.h>

/* Retail unit 0x46a8d0-0x46d7f0 (legoapi/gizmo/object/gizmopickup.cpp): definitions in retail address order. */

void __cdecl ResetGameMessages(void);
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
void __cdecl ConstantRumble(GAMEOBJECT *, f32, f32);
typedef struct lose_coins_packet_pc_s {
    u32 coins;
} LOSE_COINS_PACKET_PC;
void __cdecl MiniKit_GameMsg_Update(GAME_MESSAGE_PC *message);
void __cdecl MiniKit_GameMsg_End(GAME_MESSAGE_PC *message);
extern const char PowerUpLoopSfx_PC[18];
PART_PC *__cdecl AddPart(ADDPART_PC *parameters);
void __cdecl PartStop_Flickerer(PART_PC *part);
i32 __cdecl PartDraw_Flickerer(PART_PC *part);
void __cdecl PartCollide_3D(PART_PC *part);
i32 __cdecl AddGameDebris(APIDEBRISSYS_PC *, i32, NUVEC *);
void __cdecl AddVariableShotDebrisEffect(i32 effect, NUVEC *position, i32 count, i16 z_rotation, i16 y_rotation);
void __cdecl PartUpdate_Heart(PART_PC *part);
typedef struct coinpacket_pickup_pc_s {
    u8 unknown0[0xa];
    u8 multiplier, unknown_b;
    f32 timer;
} COINPACKET_PICKUP_PC;
u32 __cdecl GizmoPickups_TotalScore(WORLDINFO *world);
u32 __cdecl GizmoBlowups_TotalScore(WORLDINFO *world);
u32 __cdecl GizBuildIts_TotalScore(WORLDINFO *world);
u32 __cdecl GizForce_TotalScore(WORLDINFO *world);
u32 __cdecl GizObstacles_TotalScore(WORLDINFO *world);
u32 __cdecl GizTurrets_TotalScore(WORLDINFO *world);
u32 __cdecl GameAI_TotalScore(void);

void __cdecl PowerUp_Particles(WORLDINFO *world, NUVEC *position) {
    if (VehicleArea == 0 && ParticlesPerSecond(10.0f, FRAMETIME) > 0)
        AddGameDebris(world->game_debris_sys, GizBuilditGDeb[qrand() / 10923], position);
}

void __cdecl Pup_UpdatePowerUp(WORLDINFO *world, GIZMOPICKUP_PC *pickup) {
    PowerUp_Particles(world, &pickup->position);
    PlaySfx((char *)PowerUpLoopSfx_PC, &pickup->position);
}

static void __cdecl PowerUp_UpdateMsg(GAME_MESSAGE_PC *message) {
    message->parameter_e2 = (u16)(i32)(message->parameter_e2 + 16384.0 * FRAMETIME);
    if (message->player_index == 0 || message->player_index == 1)
        message->target_position.y = PowerUp_GetPanelY(message->player_index) + STATSPOSY;
}

static void PowerUp_ExistingIconPrivate(i32 player_index) {
    NUVEC position;
    ADD_GAME_MESSAGE_PC message;
    position.x = player_index == 0 ? -ICONX : ICONX;
    position.y = PowerUp_GetPanelY(player_index) + STATSPOSY;
    position.z = 1.0f;
    message = AddGameMsg_Default;
    message.icon = 0x35;
    message.special = &WORLD->level_objects[0x35].special;
    message.position = &position;
    message.flags = 0x20;
    message.scale = 0.6f;
    message.target_scale = 0.0f;
    message.duration = 0.3f;
    AddGameMsg(&message);
}

static void __cdecl PowerUp_EndMsg(GAME_MESSAGE_PC *message) {
    GameAudio_PlaySfx(0x26, NULL, 0, 0);
    if (FindGameMsgsWithID(7, 0, message->player_index, message))
        PowerUp_ExistingIconPrivate(message->player_index);
}

void __cdecl CollectPowerUp(GAMEOBJECT *object, NUVEC *position, u16 rotation, i32 flags) {
    NUVEC target;
    ADD_GAME_MESSAGE_PC message;
    GAME_MESSAGE_PC *queued;
    u16 panel_rotation;
    (void)flags;
    if (object == NULL || (u8)(i8)object->field_280 >= 2)
        return;
    if (ONEPLAYERPOWERUPS != 0) {
        panel_rotation = (u16)(rotation - RotDiff(0, GameCam_GetAdjustedYRot(NULL)));
        target.x = (i8)object->field_280 == 0 ? -ICONX : ICONX;
        target.y = PowerUp_GetPanelY((i8)object->field_280) + STATSPOSY;
        target.z = 1.0f;
        if (object->powerup_time > 0.0f)
            PowerUp_ExistingIconPrivate((i8)object->field_280);
        message = AddGameMsg_Default;
        message.position = position;
        message.target_position = &target;
        message.scale = AreaPickupScale;
        message.target_scale = POWERUPOBJSIZE;
        message.flags = 0x2112d;
        message.duration = 1.0f;
        message.icon = 0xd0;
        message.special = &WORLD->level_objects[0xd0].special;
        message.update_fn = PowerUp_UpdateMsg;
        message.end_fn = PowerUp_EndMsg;
        message.player_index = (i8)object->field_280;
        message.allow_temporary = 1;
        message.target_is_coin_total = 7;
        queued = AddGameMsg(&message);
        if (queued != NULL) {
            queued->parameter_e2 = panel_rotation;
            PowerUp_PanelYRot[(i8)object->field_280] = panel_rotation;
        }
    }
    Cheat_StartPowerUp(position, object);
    NewRumble(object->pad->bound_pad, 1.0f, 0);
    NewBuzz(object->pad->bound_pad, 0.2f, 0);
}

void __cdecl Pup_CollectPowerUp(WORLDINFO *world, GIZMOPICKUP_PC *pickup, i32 type, GAMEOBJECT *object, i32 flags) {
    (void)world;
    (void)type;
    CollectPowerUp(object, &pickup->position, pickup->rotation, flags);
}

void __cdecl PowerUp_Update(GAMEOBJECT *object) {
    if (object->powerup_time > 0.0f) {
        object->powerup_time -= FRAMETIME;
        if (object->powerup_time <= 0.0f)
            GameAudio_PlaySfx(0x52, NULL, 0, 0);
        else {
            GameAudio_PlaySfx(0x51, NULL, 0, 0);
            ConstantRumble(object, (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * .5), 0.0f);
        }
    }
}

void __cdecl Pup_CollectCharKit(WORLDINFO *world, GIZMOPICKUP_PC *pickup, i32 type, GAMEOBJECT *object, i32 flags) {
    NUVEC target;
    ADD_GAME_MESSAGE_PC message;
    GAME_MESSAGE_PC *queued;
    (void)world;
    (void)type;
    (void)flags;
    NewBuzz(object->pad->bound_pad, 0.2f, 0);
    AddGameDebris(WORLD->game_debris_sys, 0x13, &pickup->position);
    if (WORLD->level_objects[0xcf].active == 0)
        return;
    target.x = KITPOSX;
    target.y = KITPOSY + PANEL_MINIKITY;
    target.z = 1.0f;
    message = AddGameMsg_Default;
    message.position = &pickup->position;
    message.target_position = &target;
    message.scale = AreaPickupScale;
    message.target_scale = PANEL_MINIKITSCALE;
    message.flags = 0x2112d;
    message.duration = 1.0f;
    message.icon = 0xcf;
    message.special = &WORLD->level_objects[0xcf].special;
    message.tick_fn = MiniKit_GameMsg_Update;
    message.end_fn = MiniKit_GameMsg_End;
    message.allow_temporary = 1;
    queued = AddGameMsg(&message);
    if (queued != NULL)
        queued->target_is_coin_total = 6;
    DrawMiniKitTime = 2.0f;
    if (AreaGlobals_PC.field_1c < 10)
        ++AreaGlobals_PC.field_1c;
}

static void PartExtraCoinsPrivate(NUVEC *position, i32 index) {
    i32 effect = WORLD->game_debris_sys->entries[index].effect;
    f32 rate;
    i32 count;
    if (effect == -1)
        return;
    rate = PartExtraCoinRate_PC;
    if (WORLD->area != NULL && (WORLD->area->flags & 0x104) == 4)
        rate = (f32)((f64)rate * 0.5);
    count = ParticlesPerSecond(rate, FRAMETIME);
    if (count > 0)
        AddVariableShotDebrisEffect(effect, position, count, 0, 0);
}

void __cdecl Pup_UpdateBlueCoin(WORLDINFO *world, GIZMOPICKUP_PC *pickup) {
    (void)world;
    if ((pickup->flags & 0x10) != 0 && (pickup->flags & 0x20) != 0)
        PartExtraCoinsPrivate(&pickup->position, 56);
}

void __cdecl Pup_UpdatePurpleCoin(WORLDINFO *world, GIZMOPICKUP_PC *pickup) {
    (void)world;
    if ((pickup->flags & 0x10) != 0 && (pickup->flags & 0x20) != 0)
        PartExtraCoinsPrivate(&pickup->position, 57);
}

void __cdecl GameMsg_DrawAdjustNewPos_CoinToTotal(GAME_MESSAGE_PC *message) {
    message->target_position.x = cointotal_x[message->player_index];
}

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

void __cdecl GizmoPickup_CollectCoin(WORLDINFO *world, NUVEC *position, i32 type_index, i32 model_variant,
                                     GAMEOBJECT *object, i32 ignored) {
    i32 player_index, player0_active, player1_active, main_total, model_id, multiplier = 0;
    NUVEC target;
    f32 target_scale;
    u32 score;
    ADD_GAME_MESSAGE_PC message;
    COINPACKET_PICKUP_PC *packet;
    char text[8];
    (void)ignored;
    if (object != NULL)
        player_index = (i8)object->field_280;
    else {
        player0_active = Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0;
        player1_active = Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0;
        if (player0_active && player1_active)
            player_index = qrand() / 32768;
        else if (player0_active)
            player_index = 0;
        else if (player1_active)
            player_index = 1;
        else
            return;
    }
    if (player_index != 0 && player_index != 1)
        return;
    if (type_index >= 2 && type_index <= 3 && object != NULL) {
        if (world->area != NULL && (world->area->flags & 0x104) == 4)
            NewBuzzFrames(object->pad->bound_pad, 1, 0);
        else
            NewBuzz(object->pad->bound_pad, 0.1f, 0);
    }
    if (DoubleScoreTime > 0.0f || type_index == 2 || type_index == 3)
        PlaySfx((char *)PickupTypeLiteral758c5c, position);
    else
        PlaySfx((char *)PickupCoinSfx_PC, position);
    if (CoinsGoToMainTotal()) {
        target.x = cointotal_x[player_index];
        main_total = 1;
        target.y = STATSPOSY;
        DrawCoinTotalTime = (f32)(COINMSGTIME + 1.0);
        target_scale = COINTOTAL_COINSIZE;
    } else {
        target.x = player_index == 1 ? PANEL_COINX : -PANEL_COINX;
        main_total = 0;
        target.y = STATSPOSY + PANEL_COINY;
        DrawBuildUpTime = (f32)(COINMSGTIME + 1.0);
        target_scale = PANEL_COINSCALE_END;
    }
    target.z = 1.0f;
    model_id = GizmoPickupType[type_index].first_model_id + model_variant;
    if (model_id == -1 || world->level_objects[model_id].active == 0)
        return;
    score = GizmoPickupType[type_index].score;
    message = AddGameMsg_Default;
    message.icon = (i16)model_id;
    message.special = &world->level_objects[message.icon];
    message.player_index = (i8)player_index;
    message.score = score;
    if (object != NULL && object->coin_packet != NULL) {
        if (DoubleScore & (1 << player_index)) {
            if (StartupGameFlag1) {
                score += score;
                message.score = score;
            } else {
                packet = (COINPACKET_PICKUP_PC *)object->coin_packet;
                multiplier = packet->multiplier;
                score *= multiplier;
                message.score = score;
                if (packet->multiplier < 10)
                    ++packet->multiplier;
                ((COINPACKET_PICKUP_PC *)object->coin_packet)->timer = 0.5f;
                score = message.score;
            }
        }
        if (object->powerup_time > 0.0f) {
            score += score;
            message.score = score;
        }
    }
    if (main_total)
        message.update_fn = GameMsg_DrawAdjustNewPos_CoinToTotal;
    message.scale = AreaPickupScale * PANEL_COINSCALE_START;
    message.position = position;
    message.target_position = &target;
    message.target_scale = target_scale;
    message.flags = 0x2012d;
    message.duration = COINMSGTIME;
    message.end_fn = EndScoreMessage;
    AddGameMsg(&message);
    if (multiplier > 0) {
        sprintf(text, PickupMultiplierFormat, multiplier);
        message = AddGameMsg_Default;
        message.scale = message.target_scale = 2.0f;
        message.duration = 0.25f;
        message.text = text;
        message.position = position;
        message.target_position = position;
        message.flags = 0x8007;
        AddGameMsg(&message);
    }
}

static void __cdecl EndRedBrickMessage(GAME_MESSAGE_PC *message) {
    (void)message;
    if (AreaGlobals_PC.field_08 != 0)
        AreaGlobals_PC.field_08 = 2;
    RedBrickScale = 2.0f;
    GameAudio_PlaySfx(0x26, NULL, 0, 0);
    NewRumbleAllPlayers(0.6f, 0.0f, 0, 0);
    GameCam_Judder(GameCam, -0.2f, 0, NULL);
}

void __cdecl CollectRedBrick(NUVEC *position) {
    ADD_GAME_MESSAGE_PC message;
    NUVEC target;
    AddGameDebris(WORLD->game_debris_sys, 0x62, position);
    GameAudio_PlaySfxById(GetSfxId(PickupTypeLiteral758c14), position, 3, 0);
    if (WORLD->area_index == -1 || Game.area_save[WORLD->area_index].red_brick_collected == 0) {
        target.x = REDBRICKPOSX;
        target.y = REDBRICKPOSY;
        target.z = 1.0f;
        message = AddGameMsg_Default;
        message.position = position;
        message.target_position = &target;
        message.scale = AreaPickupScale;
        message.target_scale = PANEL_REDBRICKSCALE;
        message.flags = 0x2112d;
        message.duration = 1.0f;
        message.icon = 0xd2;
        message.special = &WORLD->level_objects[0xd2].special;
        message.tick_fn = MiniKit_GameMsg_Update;
        message.end_fn = EndRedBrickMessage;
        message.allow_temporary = 1;
        AddGameMsg(&message);
        DrawRedBrickTime = 2.0f;
        if (WORLD->area_index != -1)
            AreaGlobals_PC.field_08 = 1;
    }
}

void __cdecl CollectMinikit(NUVEC *position, char *name, i32 flags) {
    ADD_GAME_MESSAGE_PC message;
    NUVEC target;
    i32 area_id;
    (void)flags;
    if (WORLD->level_objects[0xce].active == 0)
        return;
    area_id = WORLD->area_index;
    target.x = KITPOSX;
    target.y = KITPOSY + PANEL_MINIKITY;
    target.z = 1.0f;
    message = AddGameMsg_Default;
    message.position = position;
    message.target_position = &target;
    message.scale = AreaPickupScale;
    message.target_scale = PANEL_MINIKITSCALE;
    message.flags = 0x2112d;
    message.duration = 1.0f;
    message.icon = 0xce;
    message.special = &WORLD->level_objects[0xce].special;
    message.tick_fn = MiniKit_GameMsg_Update;
    message.end_fn = MiniKit_GameMsg_End;
    message.allow_temporary = 1;
    AddGameMsg(&message);
    DrawMiniKitTime = 2.0f;
    if (area_id != -1 && Game.area_save[area_id].minikit_count < 10) {
        if (NuStrLen(name) <= 8)
            NuStrCpy(NewMiniPiece[AreaGlobals_PC.new_minikit_count].name, name);
        else
            NewMiniPiece[AreaGlobals_PC.new_minikit_count].name[0] = 0;
        NewMiniPiece[AreaGlobals_PC.new_minikit_count].level = (i16)WORLD->level_idx;
        ++AreaGlobals_PC.new_minikit_count;
        ++AreaGlobals_PC.area_phase;
        if (GAMEDEMO != 0) {
            if (AreaGlobals_PC.area_phase == 1)
                Hint_SetHintFromId(0x26d, 0, 0);
            else if (AreaGlobals_PC.area_phase == 10)
                Hint_SetHintFromId(0x26e, 0, 0);
        }
    }
    AddGameDebris(WORLD->game_debris_sys, 0x13, position);
    GameAudio_PlaySfxById(GetSfxId(PickupTypeLiteral758c14), position, 3, 0);
}

static void __cdecl PartImpact_Coin(PART_PC *part) {
    f32 speed, volume;
    i32 angle;
    if (!(coinimpactwait <= 0.0f))
        return;
    speed = NuVecMag(&part->velocity);
    if (!(speed > 0.0f))
        return;
    if (speed > 1.5f)
        speed = 1.5f;
    angle = (i32)(((1.5 - (f64)speed) / 1.5) * 16384.0 + 16384.0);
    volume = NuTrigTable[(angle >> 1) & 0x7fff];
    if (GAMEDEMO != 0) {
        PlaySfxAndSetVolume((char *)PickupCoinSfx_PC, &part->position, volume);
        coinimpactwait = 0.1f;
    } else if (part->part_id >= 0xb7 && part->part_id <= 0xba) {
        PlaySfxAndSetVolume((char *)CoinDropSilverSfx_PC, &part->position, volume);
    } else {
        PlaySfxAndSetVolume((char *)CoinDropGoldSfx_PC, &part->position, volume);
    }
}

static void __cdecl PartExtra_BlueCoin(PART_PC *part) {
    if ((part->active & 0x200) != 0)
        PartExtraCoinsPrivate(&part->position, 56);
}

void __cdecl PartExtra_PurpleCoin(PART_PC *part) {
    if ((part->active & 0x200) != 0)
        PartExtraCoinsPrivate(&part->position, 57);
}

static void __cdecl PartMove_VehiclePickup(PART_PC *part, f32 timestep) {
    f32 gain;
    GAMEOBJECT *object;
    (void)timestep;
    if (part->initial_time == 0.0f)
        gain = 0.0f;
    else
        gain = 1.0f - (part->scale_time != 0.0f ? part->scale_time / part->initial_time : 0.0f);
    if (gain <= 0.0f) {
        part->active |= 2;
        if (part->stop_fn != NULL)
            part->stop_fn(part);
        return;
    }
    object = FindNearestGameObject(&part->position, NULL, 0, 0.0f, 0.0f, -1, -1, 99, NULL, 0, NULL);
    if (object != NULL)
        part->velocity.y =
            SeekValF(part->velocity.y, (f32)((f64)(object->collision_position.y - part->position.y) * 3.0), 3.0f);
    part->position.x += part->velocity.x * gain * FRAMETIME;
    part->position.y += part->velocity.y * gain * FRAMETIME;
    part->position.z += part->velocity.z * gain * FRAMETIME;
}

static void __cdecl PartMove_VehicleHeart(PART_PC *part, f32 timestep) {
    GAMEOBJECT *recipient = part->recipient;
    f32 progress = 1.0f - part->time_remaining / PartVehicleHeartLifetime_PC;
    NUVEC start;
    f32 height;
    (void)timestep;
    if (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA) {
        NuVecAdd(&start, &recipient->collision_position, &part->previous_position);
        height = PartVehicleHeartPodraceHeight_PC;
    } else {
        start = part->previous_position;
        height = PartVehicleHeartHeight_PC;
    }
    part->position.x = start.x + (recipient->collision_position.x - start.x) * progress;
    part->position.y = start.y + (recipient->collision_position.y - start.y) * progress +
                       NuTrigTable[((i32)((f64)progress * 32768.0) >> 1) & 0x7fff] * height;
    part->position.z = start.z + (recipient->collision_position.z - start.z) * progress;
}

static i32 __cdecl PartDraw_VehicleHeart(PART_PC *part) {
    NUVEC scaling;
    NUMTX matrix;
    if (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA)
        scaling.x = PartVehicleHeartPodraceScale_PC;
    else if ((BONUS_GUNSHIP_ADATA != NULL && WORLD->area == BONUS_GUNSHIP_ADATA) ||
             (GUNSHIP_ADATA != NULL && WORLD->area == GUNSHIP_ADATA))
        scaling.x = PartVehicleHeartGunshipScale_PC;
    else
        scaling.x = PartVehicleHeartScale_PC;
    scaling.x = (1.0f - part->time_remaining / PartVehicleHeartLifetime_PC) * scaling.x;
    if (scaling.x > 0.0f) {
        scaling.y = scaling.z = scaling.x;
        matrix = part->transform;
        NuMtxPreScale(&matrix, &scaling);
        if (NuSpecialDrawAt(&part->special, &matrix) != 0)
            part->active |= 0x600;
    }
    return 0;
}

void __cdecl AddTorpedoAsPart(NUVEC *position, NUVEC *velocity, f32 scale, f32 lifetime) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    ADDPART_PC params = Default_ADDPART;
    PART_PC *part;
    params.position = position;
    params.velocity = velocity;
    params.field_14 = (f32)((f64)WORLD->torpedo_machine_sys->scale * 4.0 * (f64)GizmoPickupType[8].shadow_radius_x);
    params.field_18 = (f32)((f64)WORLD->torpedo_machine_sys->scale * 4.0 * (f64)GizmoPickupType[8].shadow_extent_x);
    params.gravity = AreaPickupGravity;
    params.special = &world->level_objects[0x79].special;
    params.part_id = 0x79;
    params.flags = 0x256;
    if (params.gravity == 0.0f) {
        params.field_a4 = 5.0f;
    } else {
        params.stop_fn = PartStop_Flickerer;
    }
    params.collision_fn = PartCollide_3D;
    params.update_fn = NULL;
    params.draw_fn = PartDraw_Torp;
    params.field_88 = lifetime;
    if (params.gravity == 0.0f)
        params.move_fn = PartMove_VehiclePickup;
    params.time_step = FRAMETIME;
    params.field_c0 = scale;
    part = AddPart(&params);
    if (part != NULL) {
        part->rotation_y = (u16)qrand();
        part->field_206 = 3;
        part->field_214 = (f32)((f64)WORLD->torpedo_machine_sys->scale * 4.0 * (f64)NuTrigTable[0x2000]);
    }
}

void __cdecl AddHeartAsPart(GAMEOBJECT *recipient, NUVEC *position, NUVEC *velocity, f32 scale, f32 lifetime) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    ADDPART_PC params;
    PART_PC *part;
    if (recipient == NULL)
        return;
    params = Default_ADDPART;
    params.position = position;
    params.velocity = velocity;
    params.part_id = 0xcb;
    params.flags = 0x256;
    params.field_18 = params.field_14 = scale * GizmoPickupType[5].shadow_radius_x;
    params.gravity = AreaPickupGravity;
    params.special = &world->level_objects[0xcb].special;
    params.field_88 = lifetime;
    params.collision_fn = PartCollide_3D;
    params.update_fn = PartUpdate_Heart;
    if ((world->current_level->flags & 0x80000) != 0) {
        params.field_a4 = PartVehicleHeartLifetime_PC;
        params.move_fn = PartMove_VehicleHeart;
        params.flags |= 0x400;
        params.recipient = recipient;
        params.draw_fn = PartDraw_VehicleHeart;
    } else {
        params.stop_fn = PartStop_Flickerer;
        params.draw_fn = PartDraw_Flickerer;
        if (params.gravity == 0.0f)
            params.move_fn = PartMove_VehiclePickup;
    }
    params.time_step = FRAMETIME;
    params.field_c0 = scale;
    part = AddPart(&params);
    if (part != NULL)
        part->rotation_y = (u16)qrand();
}

static void __cdecl PowerUp_ImpactPart(PART_PC *part) {
    f32 minimum = (f32)((f64)AreaPickupScale * (f64)0.1f);
    f32 maximum = (f32)((f64)AreaPickupScale * 2.5);
    f32 speed = NuVecMag(&part->velocity);
    f32 volume;
    if (speed >= maximum)
        volume = 1.0f;
    else {
        if (!(speed >= minimum))
            return;
        volume = (speed - minimum) / (maximum - minimum);
    }
    if (volume > 0.0f)
        PlaySfxAndSetVolume((char *)PowerUpBounceSfx_PC, &part->position, volume);
}

static void __cdecl PowerUp_UpdatePart(PART_PC *part) {
    NUVEC position = part->position;
    part->rotation_y = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
    NuMtxSetRotationY(&part->transform, part->rotation_y);
    NuMtxTranslate(&part->transform, &position);
    PowerUp_Particles(WorldInfo_CurrentlyActive(), &position);
    PlaySfx((char *)PowerUpLoopSfx_PC, &position);
}

static i32 __cdecl PowerUp_DrawPart(PART_PC *part) {
    i32 draw = PartDraw_Flickerer(part);
    if (draw && WORLD->level_objects[0xd1].active != 0) {
        NUMTX matrix = part->transform;
        NuSpecialDrawAt(&WORLD->level_objects[0xd1].special, &matrix);
    }
    return draw;
}

void __cdecl PowerUp_AddPart(NUVEC *position, NUVEC *velocity, f32 scale, f32 lifetime) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    ADDPART_PC params = Default_ADDPART;
    PART_PC *part;
    params.position = position;
    params.velocity = velocity;
    params.special = &world->level_objects[0xd0].special;
    params.field_18 = params.field_14 = scale * GizmoPickupType[9].shadow_radius_x;
    params.gravity = AreaPickupGravity;
    params.part_id = 0xd0;
    params.flags = 0x256;
    params.impact_fn = PowerUp_ImpactPart;
    params.collision_fn = PartCollide_3D;
    params.update_fn = PowerUp_UpdatePart;
    params.stop_fn = PartStop_Flickerer;
    params.draw_fn = PowerUp_DrawPart;
    if (params.gravity == 0.0f)
        params.move_fn = PartMove_VehiclePickup;
    params.field_88 = lifetime;
    params.time_step = FRAMETIME;
    params.field_c0 = scale;
    part = AddPart(&params);
    if (part != NULL)
        part->rotation_y = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
}

/* Every stud the level can release, optionally split per source. AI
 * characters count only in areas flagged 0x100. */
u32 __cdecl TotalLevelCoinTally(WORLDINFO *world, u32 *pickups, u32 *blowups, u32 *buildits, u32 *forces,
                                u32 *obstacles, u32 *turrets, u32 *characters) {
    u32 value = GizmoPickups_TotalScore(world);
    u32 total = value;
    if (pickups != NULL)
        *pickups = value;
    value = GizmoBlowups_TotalScore(world);
    total += value;
    if (blowups != NULL)
        *blowups = value;
    value = GizBuildIts_TotalScore(world);
    total += value;
    if (buildits != NULL)
        *buildits = value;
    value = GizForce_TotalScore(world);
    total += value;
    if (forces != NULL)
        *forces = value;
    value = GizObstacles_TotalScore(world);
    total += value;
    if (obstacles != NULL)
        *obstacles = value;
    value = GizTurrets_TotalScore(world);
    total += value;
    if (turrets != NULL)
        *turrets = value;
    if (world->area != NULL && (world->area->flags & 0x100) != 0)
        value = GameAI_TotalScore();
    else
        value = 0;
    total += value;
    if (characters != NULL)
        *characters = value;
    return total;
}

void __cdecl CollectHitPoint(GAMEOBJECT *object, NUVEC *position, i32 flags) {
    NUVEC momentum;
    (void)flags;
    if (object->current_hp > 0 && object->current_hp < object->field_107e)
        ++object->current_hp;
    if (object->saved_hitpoints < 4)
        ++object->saved_hitpoints;
    if (object->character_data->model_flags & 0x2000) {
        NuVecScale(&momentum, &object->velocity, 0.5f);
        AddGameDebrisMom(WORLD->game_debris_sys, 0x10, position, 15, &momentum);
    } else
        AddGameDebris(WORLD->game_debris_sys, 15, position);
    GameAudio_PlaySfx(0x27, position, 0, 0);
    NewBuzz(object->pad->bound_pad, 0.1f, 0);
}

i32 __cdecl ReleaseHearts(void) {
    GAMEOBJECT *player = (GAMEOBJECT *)Player[0];
    i32 total_hitpoints = 0;
    i32 missing_hitpoints = 0;
    i32 i, hitpoints, chance, adjustment;
    if (player != NULL && player->dead == 0 && player->field_107e > 0) {
        total_hitpoints = player->field_107e;
        missing_hitpoints = total_hitpoints - player->current_hp;
    }
    player = (GAMEOBJECT *)Player[1];
    if (player != NULL && player->dead == 0 && player->field_107e > 0) {
        hitpoints = player->field_107e;
        missing_hitpoints += hitpoints - player->current_hp;
        total_hitpoints += hitpoints;
    }
    if (total_hitpoints == 0)
        return 0;
    if (missing_hitpoints > 0 && MAXPARTS > 0) {
        for (i = 0; i < MAXPARTS; ++i)
            if ((Part[i].active & 1) && Part[i].part_id == 0xcb)
                --missing_hitpoints;
    }
    missing_hitpoints += (Game.difficulty <= 5 ? 8 : 9) - Game.difficulty;
    if (missing_hitpoints > total_hitpoints)
        missing_hitpoints = total_hitpoints;
    else if (missing_hitpoints < 0)
        missing_hitpoints = 0;
    chance = (missing_hitpoints << 16) / total_hitpoints;
    if (Cheat_IsOn(0x2c)) {
        adjustment = adtab[adaptivedifficulty.level][1];
        if (adjustment == 1)
            chance /= 2;
        else if (adjustment == -1)
            chance += chance;
    }
    return chance > qrand();
}

void __cdecl SetCoinType(i32 model, GIZMOPICKUP_PC *pickup) {
    u8 count;
    if ((u32)(model - 0xb7) <= 3) {
        pickup->type_index = 0;
        pickup->model_variant = (u8)(model - 0xb7);
    } else if ((u32)(model - 0xbf) <= 3) {
        pickup->type_index = 1;
        pickup->model_variant = (u8)(model - 0xbf);
    } else if ((u32)(model - 0xc7) <= 3) {
        pickup->type_index = 2;
        pickup->model_variant = (u8)(model - 0xc7);
    } else {
        pickup->type_index = 3;
        pickup->model_variant = (u8)(model - 0xd5);
    }
    count = GizmoPickupType[pickup->type_index].random_model_count;
    pickup->model_variant = count ? pickup->model_variant % count : 0;
}

i32 GetRandomCoinType() {
    return CoinTab[qrand() / 16384];
}

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

/* Native 0046c7d0..0046ca7d. */
void __cdecl GizmoPickups_SetOnOff(void) {
    u32 arcade_flags;
    i32 index;
    i32 off;
    i32 challenge;
    WORLDINFO *world;
    Arcade_GetMode(&arcade_flags);
    challenge = ChallengeMode;
    world = WORLD;
    for (index = 0; index < 10; ++index) {
        if (index == 7) {
            off = challenge != 0;
        } else if (challenge != 0) {
            off = 0;
        } else {
            off = 1;
            if (index == 6 && world->area_index != -1 && Game.area_save[world->area_index].red_brick_collected != 0)
                off = 0;
            else if (index == 4 && SuperStory != 0)
                off = 0;
            else if (index == 9 && world->area != NULL && (world->area->flags & 0x100) != 0)
                off = 0;
            else if ((arcade_flags & 0x10) != 0 && index != 5 && index != 9)
                off = 0;
        }
        GizmoPickupType[index].field_0f = off == 0;
    }
}

void __cdecl Pup_CollectCoin(WORLDINFO *world, GIZMOPICKUP_PC *pickup, i32 type, GAMEOBJECT *object, i32 flags) {
    GizmoPickup_CollectCoin(world, &pickup->position, type, pickup->model_variant, object, flags);
}

void __cdecl Pup_CollectMinikit(WORLDINFO *world, GIZMOPICKUP_PC *pickup, i32 type, GAMEOBJECT *object, i32 flags) {
    (void)world;
    (void)type;
    CollectMinikit(&pickup->position, pickup->name, flags);
    NewBuzz(object->pad->bound_pad, 0.2f, 0);
}

void __cdecl Pup_CollectRedBrick(WORLDINFO *world, GIZMOPICKUP_PC *pickup, i32 type, GAMEOBJECT *object, i32 flags) {
    (void)world;
    (void)type;
    (void)flags;
    CollectRedBrick(&pickup->position);
    NewBuzz(object->pad->bound_pad, 0.2f, 0);
}

void __cdecl Pup_CollectHeart(WORLDINFO *world, GIZMOPICKUP_PC *pickup, i32 type, GAMEOBJECT *object, i32 flags) {
    (void)world;
    (void)type;
    CollectHitPoint(object, &pickup->position, flags);
}

static void __cdecl PartStolen_Coin(PART_PC *part) {
    GAMEOBJECT *recipient;
    i32 first_active, second_active;
    GIZMOPICKUP_PC pickup;
    if (netclient != 0 || (part->active & 0x200) == 0)
        return;
    recipient = Player[0];
    first_active = recipient != NULL && (recipient->flags_1fc & 0x80) != 0;
    second_active = Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0;
    if (first_active) {
        if (second_active)
            recipient = Player[qrand() / 32768];
    } else if (second_active) {
        recipient = Player[1];
    } else {
        return;
    }
    if (recipient != NULL) {
        SetCoinType(part->part_id, &pickup);
        GizmoPickup_CollectCoin(WORLD, &part->position, pickup.type_index, pickup.model_variant, recipient, 1);
    }
}

static void __cdecl PartStop_Coin(PART_PC *part) {
    if (netclient == 0) {
        GIZMOPICKUPRUNTIMESYS_s *system = WORLD->pickup_runtime_sys;
        GIZMOPICKUP_PC *pickup = &system->temporary_pickups[system->temporary_cursor];
        ++system->temporary_cursor;
        if (WORLD->pickup_runtime_sys->temporary_cursor >= 64)
            WORLD->pickup_runtime_sys->temporary_cursor = 0;
        if ((pickup->flags & 1) != 0)
            GizmoPickup_CollectCoin(WORLD, &pickup->position, pickup->type_index, pickup->model_variant, NULL, 1);
        SetCoinType(part->part_id, pickup);
        pickup->flags = (pickup->flags & 0xf7) | 0x17;
        pickup->position = part->position;
        pickup->rotation = 0;
        pickup->room = WORLD->current_gscn != NULL ? (i8)NuPortalWhichRoom(WORLD->current_gscn, &pickup->position) : -1;
        pickup->floor = 2000000.0f;
        pickup->timer = 5.0f;
        pickup->shadow_x = 0;
        pickup->shadow_z = 0;
        pickup->flags = (pickup->flags | 0x20) & ~0x40;
    }
    part->active &= ~1;
}

void __cdecl AddCoinsAsParts(i32 type_id, NUVEC *position, NUVEC *velocity, f32 lifetime, f32 scale) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    GIZMO_PICKUP_TYPE_PC *type = &GizmoPickupType[type_id];
    i32 model = type->first_model_id;
    LEVEL_OBJECT_RUNTIME_PC *object;
    ADDPART_PC params;
    PART_PC *part;
    if (type->random_model_count != 0)
        model += qrand() / (65535 / type->random_model_count + 1);
    object = &world->level_objects[model];
    if (object->active == 0)
        return;
    params = Default_ADDPART;
    params.special = &object->special;
    params.part_id = model;
    params.position = position;
    params.velocity = velocity;
    params.field_14 = params.field_18 = scale * type->shadow_radius_x;
    params.flags = 0x56;
    params.gravity = AreaPickupGravity;
    if (model >= 0xd5 && model <= 0xd8) {
        params.flags |= 0x200;
    } else if (model >= 0xc7 && model <= 0xca) {
        params.flags |= 0x200;
        params.callback_30 = PartExtra_BlueCoin;
    } else if (model >= 0xd5 && model <= 0xd8) {
        params.flags |= 0x200;
        params.callback_30 = PartExtra_PurpleCoin;
    } else {
        params.destroy_fn = PartStolen_Coin;
    }
    params.field_88 = lifetime;
    params.collision_fn = PartCollide_3D;
    params.stop_fn = PartStop_Coin;
    params.impact_fn = PartImpact_Coin;
    params.time_step = FRAMETIME;
    params.field_c0 = scale;
    part = AddPart(&params);
    if (part != NULL) {
        part->time_remaining = 30.0f;
        if (params.gravity == 0.0f) {
            part->move_fn = PartMove_VehiclePickup;
            part->initial_time = (f32)((f64)qrand() * (f64)(f32)(1.0f / 65535.0f) + 4.0);
        }
    }
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

void AddMiscPickups(NUVEC *position, i32 player_id, i32 coins, i32 torpedoes) {
    if (torpedoes != 0) {
        AddPickups(coins, 0, torpedoes, 0, position, &v010, 5.0f, player_id, 1.0f, 2000000.0f, NULL, 1, 0);
    } else {
        AddPickups(coins, ReleaseHearts(), 0, 0, position, &v010, 5.0f, player_id, 1.0f, 2000000.0f, NULL, 1, 0);
    }
}
