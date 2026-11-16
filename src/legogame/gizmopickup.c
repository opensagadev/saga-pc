#include <stdio.h>
#include "batman.h"
#include "gameapi/edptl.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/ai/core/gameai.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/playeritems.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmo/object/giztorpedo.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/gizmos/object/gizbuildits.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolt.h"
#include "legoapi/items/collect/minikits.h"
#include "legoapi/items/collect/shard.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/camera.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/core/gamehint.h"
#include "legoapi/menus/core/gamemessages.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/menus/screens/arcade.h"
#include "legoapi/menus/screens/gamestatus_lsw.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/render.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/area.h"
#include "legoapi/world/areas.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "legogame/gamestatus.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nuportal.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic/sfx.h"
#include <string.h>
#include <float.h>
#include "legoapi/gizmos/object/gizobstacle.h"

u32 GizTurrets_TotalScore(WORLDINFO *world);

PICKUP_TYPE_SYSTEM GizmoPickupSys_Game = {GizmoPickupType, CoinTab, 10, 4, 4, 2, 0, 0};

void PowerUp_Particles(WORLDINFO *world, NUVEC *position) {
    if (VehicleArea == 0 && ParticlesPerSecond(10.0f, FRAMETIME) > 0) {
        AddGameDebris(world->game_debris_sys, GizBuilditGDeb[qrand() / 10923], position);
    }
}

void Pup_UpdatePowerUp(WORLDINFO *world, GIZMOPICKUP *pickup) {
    PowerUp_Particles(world, &pickup->position);
    PlaySfx("Grv_GuardWeaponLp", &pickup->position);
}

static void PowerUp_UpdateMsg(GAME_MESSAGE *message) {
    message->parameter_e2 = (u16)(i32)(message->parameter_e2 + 16384.0 * FRAMETIME);
    if (message->player_index == 0 || message->player_index == 1) {
        message->target_position.y = PowerUp_GetPanelY(message->player_index) + STATSPOSY;
    }
}

static void PowerUp_ExistingIconPrivate(i32 player_index) {
    NUVEC position;
    ADD_GAME_MESSAGE message;
    position.x = player_index == 0 ? -ICONX : ICONX;
    position.y = PowerUp_GetPanelY(player_index) + STATSPOSY;
    position.z = 1.0f;
    message = AddGameMsg_Default;
    message.icon = 0x35;
    message.special = &WORLD->level_objects[0x35].special;
    message.position = &position;
    message.flags = GAMEMSG_DRAW_SCALE;
    message.scale = 0.6f;
    message.target_scale = 0.0f;
    message.duration = 0.3f;
    AddGameMsg(&message);
}

static void PowerUp_EndMsg(GAME_MESSAGE *message) {
    GameAudio_PlaySfx(0x26, NULL, 0, 0);
    if (FindGameMsgsWithID(7, 0, message->player_index, message)) {
        PowerUp_ExistingIconPrivate(message->player_index);
    }
}

void CollectPowerUp(GAMEOBJECT *object, NUVEC *position, u16 rotation, i32 flags) {
    NUVEC target;
    ADD_GAME_MESSAGE message;
    GAME_MESSAGE *queued;
    u16 panel_rotation;
    if (object == NULL || (object->player_index != 0 && object->player_index != 1)) {
        return;
    }
    if (ONEPLAYERPOWERUPS != 0) {
        panel_rotation = (u16)(rotation - RotDiff(0, GameCam_GetAdjustedYRot(NULL)));
        target.x = object->player_index == 0 ? -ICONX : ICONX;
        target.y = PowerUp_GetPanelY(object->player_index) + STATSPOSY;
        target.z = 1.0f;
        if (object->player.powerup_time > 0.0f) {
            PowerUp_ExistingIconPrivate(object->player_index);
        }
        message = AddGameMsg_Default;
        message.icon = 0xd0;
        message.special = &WORLD->level_objects[0xd0].special;
        message.scale = AreaPickupScale;
        message.target_scale = POWERUPOBJSIZE;
        message.position = position;
        message.duration = 1.0f;
        message.target_position = &target;
        message.tick_fn = PowerUp_UpdateMsg;
        message.end_fn = PowerUp_EndMsg;
        message.allow_temporary = 1;
        message.flags = GAMEMSG_WORLD_POSITION | GAMEMSG_WORLD_SCALE | GAMEMSG_DRAW_MOVE | GAMEMSG_DRAW_SCALE | GAMEMSG_EASE_IN |
                        GAMEMSG_KEEP | GAMEMSG_OBJECT_NO_ALPHA;
        message.player_index = object->player_index;
        message.target_is_coin_total = 7;
        queued = AddGameMsg(&message);
        if (queued != NULL) {
            queued->parameter_e2 = panel_rotation;
            PowerUp_PanelYRot[object->player_index] = panel_rotation;
        }
    }
    Cheat_StartPowerUp(position, object);
    NewRumble(object->player.pad->bound_pad, 1.0f, 0);
    NewBuzz(object->player.pad->bound_pad, 0.2f, 0);
}

void Pup_CollectPowerUp(WORLDINFO *world, GIZMOPICKUP *pickup, i32 type, GAMEOBJECT *object, i32 flags) {
    CollectPowerUp(object, &pickup->position, pickup->rotation, flags);
}

void PowerUp_Update(GAMEOBJECT *object) {
    if (object->player.powerup_time > 0.0f) {
        object->player.powerup_time -= FRAMETIME;
        if (object->player.powerup_time <= 0.0f) {
            GameAudio_PlaySfx(0x52, NULL, 0, 0);
        } else {
            GameAudio_PlaySfx(0x51, NULL, 0, 0);
            ConstantRumble(object, (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * .5), 0.0f);
        }
    }
}

void Pup_CollectCharKit(WORLDINFO *world, GIZMOPICKUP *pickup, i32 type, GAMEOBJECT *object, i32 flags) {
    NUVEC target;
    ADD_GAME_MESSAGE message;
    GAME_MESSAGE *queued;
    LEVEL_OBJECT_RUNTIME *objects;
    NewBuzz(object->player.pad->bound_pad, 0.2f, 0);
    AddGameDebris(WORLD->game_debris_sys, 0x13, &pickup->position);
    objects = WORLD->level_objects;
    if (objects[0xcf].active == 0) {
        return;
    }
    target.x = KITPOSX;
    target.y = KITPOSY + PANEL_MINIKITY;
    target.z = 1.0f;
    message = AddGameMsg_Default;
    message.position = &pickup->position;
    message.target_position = &target;
    message.scale = AreaPickupScale;
    message.target_scale = PANEL_MINIKITSCALE;
    message.flags = GAMEMSG_WORLD_POSITION | GAMEMSG_WORLD_SCALE | GAMEMSG_DRAW_MOVE | GAMEMSG_DRAW_SCALE | GAMEMSG_EASE_IN | GAMEMSG_KEEP |
                    GAMEMSG_OBJECT_NO_ALPHA;
    message.duration = 1.0f;
    message.icon = 0xcf;
    message.special = &objects[0xcf].special;
    message.tick_fn = MiniKit_GameMsg_Update;
    message.end_fn = MiniKit_GameMsg_End;
    message.allow_temporary = 1;
    queued = AddGameMsg(&message);
    if (queued != NULL) {
        queued->target_is_coin_total = 6;
    }
    DrawMiniKitTime = 2.0f;
    if (AreaGlobals.challenge_minikits < 10) {
        ++AreaGlobals.challenge_minikits;
    }
}

f32 PartExtraCoinRate = 5.0f;

static void PartExtraCoinsPrivate(NUVEC *position, i32 index) {
    i32 effect = WORLD->game_debris_sys->entries[index].effect;
    i32 count;
    if (effect == -1) {
        return;
    }
    count = ParticlesPerSecond(WORLD->area != NULL && (WORLD->area->flags & AREAFLAG_SUPER_BONUS_AREA) == 4 ? PartExtraCoinRate * 0.5
                                                                                                            : PartExtraCoinRate,
                               FRAMETIME);
    if (count > 0) {
        AddVariableShotDebrisEffect(effect, position, count, 0, 0);
    }
}

void Pup_UpdateBlueCoin(WORLDINFO *world, GIZMOPICKUP *pickup) {
    if ((pickup->flags & GIZMOPICKUP_STATE_DRAWN) != 0 && (pickup->flags & GIZMOPICKUP_STATE_DRAW_VISIBLE) != 0) {
        PartExtraCoinsPrivate(&pickup->position, 56);
    }
}

void Pup_UpdatePurpleCoin(WORLDINFO *world, GIZMOPICKUP *pickup) {
    if ((pickup->flags & GIZMOPICKUP_STATE_DRAWN) != 0 && (pickup->flags & GIZMOPICKUP_STATE_DRAW_VISIBLE) != 0) {
        PartExtraCoinsPrivate(&pickup->position, 57);
    }
}

void GameMsg_DrawAdjustNewPos_CoinToTotal(GAME_MESSAGE *message) {
    message->target_position.x = cointotal_x[message->player_index];
}

void EndScoreMessage(GAME_MESSAGE *message) {
    i32 icon;
    GAMEOBJECT *player;
    u32 old_coins;
    WORLDINFO *world;
    if (CoinsGoToMainTotal()) {
        if (message->icon != 0xffff) {
            icon = (i16)message->icon;
            if ((u32)(icon - 0xb7) <= 3) {
                icon -= 4;
            } else if ((u32)(icon - 0xbf) <= 3) {
                icon -= 4;
            } else if ((u32)(icon - 0xc7) <= 3) {
                icon -= 4;
            } else if ((u32)(icon - 0xd5) <= 3) {
                icon += 4;
            }
            cointotal_i_obj[message->player_index] = icon;
        }
        CoinTotalScale = 1.5f;
        if (SuperStory != 0) {
            IncreaseScore(&SuperStoryScore, message->score, 0);
        } else {
            world = WorldInfo_CurrentlyActive();
            if (world->area != NULL && (world->area->flags & AREAFLAG_SUPER_BONUS)) {
                if ((u32)BonusCoinTotal < BonusCoinTarget) {
                    IncreaseScore((u32 *)&BonusCoinTotal, message->score, 0);
                    if ((u32)BonusCoinTotal >= BonusCoinTarget) {
                        BonusCoinTotal = (i32)BonusCoinTarget;
                        SetBonusWinner(message->player_index);
                        BonusWinFlag = 0;
                        NewMenu(MENU_BONUS_COMPLETE, -1, -1);
                        PlaySfx("MK-Panel", NULL);
                        ResetGameMessages();
                    }
                }
            } else {
                IncreaseScore(&Game.coins, message->score, 1);
            }
        }
    } else {
        if (message->player_index != 0 && message->player_index != 1) {
            goto counter_sound;
        }
        player = Player[message->player_index];
        if (player == NULL || player->player.coin_packet == NULL) {
            goto counter_sound;
        }
        old_coins = player->player.coin_packet->coins;
        if ((i16)message->icon != -1) {
            player->player.coin_packet->last_coin = (i16)message->icon;
        }
        if (BonusWinner == -1) {
            IncreaseScore(&player->player.coin_packet->coins, message->score, 1);
            player->player.coin_packet->scale = 1.5f;
        }
        if (BuildUpTotal == 0) {
            BuildUpScale = 1.5f;
        }
        if (Arcade != 0) {
            Arcade_CoinCollected(message->player_index, &player->player.coin_packet->coins, old_coins);
        } else if (BonusArea != 0 && BonusWinner == -1 && player->player.coin_packet->coins >= 1000000u) {
            player->player.coin_packet->coins = 1000000;
            SetBonusWinner(message->player_index);
            BonusWinFlag = 0;
            NewMenu(MENU_BONUS_WIN, -1, -1);
            PlaySfx("MK-Panel", NULL);
            ResetGameMessages();
        }
    }
counter_sound:
    if (DoubleScoreTime > 0.0f) {
        PlayRepeatSfx("Counter", -1, 0.0f, 2, 0.1f, NULL);
    } else {
        PlaySfx("Counter", NULL);
    }
}

GIZMO_PICKUP_TYPE GizmoPickupType[10] = {
    {"Silver Coin", NULL, NULL, 's', 4, 64, 0, 183, 10, 0.045f, 0.045f, 10.0f, 0.0f, NULL, Pup_CollectCoin, -1, -1, 0.0f, 0.0f},
    {"Gold Coin", NULL, NULL, 'g', 4, 64, 0, 191, 100, 0.045f, 0.045f, 10.0f, 0.0f, NULL, Pup_CollectCoin, -1, -1, 0.0f, 0.0f},
    {"Blue Coin", "PickupCoinB", NULL, 'b', 4, 64, 0, 199, 1000, 0.045f, 0.045f, 6.0f, 0.0f, Pup_UpdateBlueCoin, Pup_CollectCoin, 8, -1,
     0.0f, 0.0f},
    {"Purple Coin", "PickupCoinB", NULL, 'p', 4, 64, 0, 213, 10000, 0.045f, 0.045f, 6.0f, 0.0f, Pup_UpdatePurpleCoin, Pup_CollectCoin, 8,
     -1, 0.0f, 0.0f},
    {"Minikit", "MK-Appear", NULL, 'm', 0, 69, 0, 206, 0, 0.125f, 0.15f, 2.0f, 0.05f, NULL, Pup_CollectMinikit, 20, -1, 0.0f, 0.0f},
    {"Heart", NULL, NULL, 'h', 0, 2, 0, 203, 0, 0.1f, 0.08f, 4.0f, 0.0f, NULL, Pup_CollectHeart, -1, -1, 0.0f, 0.0f},
    {"Red Brick", "MK-Appear", NULL, 'r', 0, 73, 0, 210, 0, 0.12f, 0.06f, 2.0f, 0.1f, NULL, Pup_CollectRedBrick, 99, -1, 0.0f, 0.0f},
    {"Charkit", "MK-Appear", "MK-Pickup", 'c', 0, 33, 0, 207, 0, 0.125f, 0.15f, 2.0f, 0.05f, NULL, Pup_CollectCharKit, 20, -1, 0.0f, 0.0f},
    {"Torpedo", NULL, NULL, 't', 0, 2, 0, 121, 0, 0.1f, 0.1f, 5.0f, 0.0f, NULL, NULL, -1, -1, 0.0f, 0.0f},
    {"Power Up", "MK-Appear", NULL, 'u', 0, 1, 0, 208, 0, 0.125f, 0.125f, 2.0f, 0.05f, Pup_UpdatePowerUp, Pup_CollectPowerUp, -1, 209, 0.0f,
     0.0f},
};
f32 PANEL_COINSCALE_START = 1.0f;

void GizmoPickup_CollectCoin(WORLDINFO *world, NUVEC *position, i32 type_index, i32 model_variant, GAMEOBJECT *object, i32 ignored) {
    i32 player_index;
    i32 player0_active;
    i32 player1_active;
    i32 main_total;
    i32 model_id;
    i32 multiplier;
    NUVEC target;
    f32 target_scale;
    u32 score;
    ADD_GAME_MESSAGE message;
    char text[8];
    if (object != NULL) {
        player_index = object->player_index;
    } else {
        player0_active = Player[0] != NULL && (Player[0]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0;
        player1_active = Player[1] != NULL && (Player[1]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0;
        if (player0_active && player1_active) {
            player_index = qrand() / 32768;
        } else if (player0_active) {
            player_index = 0;
        } else if (player1_active) {
            player_index = 1;
        } else {
            return;
        }
    }
    if (player_index != 0 && player_index != 1) {
        return;
    }
    switch (type_index) {
        case 2:
        case 3:
            if (object != NULL) {
                if (world->area != NULL && (world->area->flags & AREAFLAG_SUPER_BONUS_AREA) == 4) {
                    NewBuzzFrames(object->player.pad->bound_pad, 1, 0);
                } else {
                    NewBuzz(object->player.pad->bound_pad, 0.1f, 0);
                }
            }
            break;
    }
    if (DoubleScoreTime > 0.0f || type_index == 2 || type_index == 3) {
        PlaySfx("PickupCoinB", position);
    } else {
        PlaySfx("PickupCoin", position);
    }
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
    if (model_id == -1 || world->level_objects[model_id].active == 0) {
        return;
    }
    score = GizmoPickupType[type_index].score;
    message = AddGameMsg_Default;
    message.icon = (i16)model_id;
    message.special = &world->level_objects[message.icon];
    message.player_index = (i8)player_index;
    message.score = score;
    multiplier = 0;
    if (object != NULL && object->player.coin_packet != NULL) {
        if (DoubleScore & (1 << player_index)) {
            if (StartupGameFlag1) {
                score += score;
                message.score = score;
            } else {
                multiplier = object->player.coin_packet->multiplier;
                score *= multiplier;
                message.score = score;
                if (object->player.coin_packet->multiplier < PLAYER_COIN_MAX_MULTIPLIER) {
                    ++object->player.coin_packet->multiplier;
                }
                object->player.coin_packet->multiplier_time = 0.5f;
                score = message.score;
            }
        }
        if (object->player.powerup_time > 0.0f) {
            score += score;
            message.score = score;
        }
    }
    if (main_total) {
        message.update_fn = GameMsg_DrawAdjustNewPos_CoinToTotal;
    }
    message.scale = PANEL_COINSCALE_START * AreaPickupScale;
    message.position = position;
    message.target_position = &target;
    message.target_scale = target_scale;
    message.flags =
        GAMEMSG_WORLD_POSITION | GAMEMSG_WORLD_SCALE | GAMEMSG_DRAW_MOVE | GAMEMSG_DRAW_SCALE | GAMEMSG_EASE_IN | GAMEMSG_OBJECT_NO_ALPHA;
    message.duration = COINMSGTIME;
    message.end_fn = EndScoreMessage;
    AddGameMsg(&message);
    if (multiplier > 0) {
        sprintf(text, "x%i", multiplier);
        message = AddGameMsg_Default;
        message.scale = message.target_scale = 2.0f;
        message.duration = 0.25f;
        message.text = text;
        message.position = position;
        message.target_position = position;
        message.flags = GAMEMSG_WORLD_POSITION | GAMEMSG_FOLLOW | GAMEMSG_WORLD_SCALE | GAMEMSG_FADE_SMOOTH;
        AddGameMsg(&message);
    }
}

static void EndRedBrickMessage(GAME_MESSAGE *message) {
    if (AreaGlobals.red_brick_state != 0) {
        AreaGlobals.red_brick_state = 2;
    }
    RedBrickScale = 2.0f;
    GameAudio_PlaySfx(0x26, NULL, 0, 0);
    NewRumbleAllPlayers(0.6f, 0.0f, 0, 0);
    GameCam_Judder(GameCam, -0.2f, 0, NULL);
}

void CollectRedBrick(NUVEC *position) {
    ADD_GAME_MESSAGE message;
    NUVEC target;
    AddGameDebris(WORLD->game_debris_sys, 0x62, position);
    GameAudio_PlaySfxById(GetSfxId("MK-Pickup"), position, 3, 0);
    if (WORLD->area_index == -1 || Game.area_save[WORLD->area_index].red_brick_collected == 0) {
        target.x = REDBRICKPOSX;
        target.y = REDBRICKPOSY;
        target.z = 1.0f;
        message = AddGameMsg_Default;
        message.icon = 0xd2;
        message.special = &WORLD->level_objects[0xd2].special;
        message.position = position;
        message.target_position = &target;
        message.tick_fn = MiniKit_GameMsg_Update;
        message.end_fn = EndRedBrickMessage;
        message.scale = AreaPickupScale;
        message.allow_temporary = 1;
        message.flags = GAMEMSG_WORLD_POSITION | GAMEMSG_WORLD_SCALE | GAMEMSG_DRAW_MOVE | GAMEMSG_DRAW_SCALE | GAMEMSG_EASE_IN |
                        GAMEMSG_KEEP | GAMEMSG_OBJECT_NO_ALPHA;
        message.target_scale = PANEL_REDBRICKSCALE;
        message.duration = 1.0f;
        AddGameMsg(&message);
        DrawRedBrickTime = 2.0f;
        if (WORLD->area_index != -1) {
            AreaGlobals.red_brick_state = 1;
        }
    }
}

void CollectMinikit(NUVEC *position, char *name, i32 flags) {
    ADD_GAME_MESSAGE message;
    NUVEC target;
    i32 area_id;
    if (WORLD->level_objects[0xce].active == 0) {
        return;
    }
    target.x = KITPOSX;
    target.y = KITPOSY + PANEL_MINIKITY;
    target.z = 1.0f;
    area_id = WORLD->area_index;
    message = AddGameMsg_Default;
    message.position = position;
    message.target_position = &target;
    message.scale = AreaPickupScale;
    message.target_scale = PANEL_MINIKITSCALE;
    message.flags = GAMEMSG_WORLD_POSITION | GAMEMSG_WORLD_SCALE | GAMEMSG_DRAW_MOVE | GAMEMSG_DRAW_SCALE | GAMEMSG_EASE_IN | GAMEMSG_KEEP |
                    GAMEMSG_OBJECT_NO_ALPHA;
    message.duration = 1.0f;
    message.icon = 0xce;
    message.special = &WORLD->level_objects[0xce].special;
    message.tick_fn = MiniKit_GameMsg_Update;
    message.end_fn = MiniKit_GameMsg_End;
    message.allow_temporary = 1;
    AddGameMsg(&message);
    DrawMiniKitTime = 2.0f;
    if (area_id != -1 && Game.area_save[area_id].minikit_count < 10) {
        if (NuStrLen(name) <= 8) {
            NuStrCpy(NewMiniPiece[AreaGlobals.new_minikit_count].name, name);
        } else {
            NewMiniPiece[AreaGlobals.new_minikit_count].name[0] = 0;
        }
        NewMiniPiece[AreaGlobals.new_minikit_count].level = (i16)WORLD->level_idx;
        ++AreaGlobals.new_minikit_count;
        ++AreaGlobals.area_phase;
        if (GAMEDEMO != 0) {
            if (AreaGlobals.area_phase == 1) {
                Hint_SetHintFromId(0x26d, 0, 0);
            } else if (AreaGlobals.area_phase == 10) {
                Hint_SetHintFromId(0x26e, 0, 0);
            }
        }
    }
    AddGameDebris(WORLD->game_debris_sys, 0x13, position);
    GameAudio_PlaySfxById(GetSfxId("MK-Pickup"), position, 3, 0);
}

f32 coinimpactwait = 0;

static void PartImpact_Coin(PART *part) {
    f32 speed;
    f32 volume;
    i32 angle;
    if (!(coinimpactwait <= 0.0f)) {
        return;
    }
    speed = NuVecMag(&part->velocity);
    if (!(speed > 0.0f)) {
        return;
    }
    if (speed > 1.5f) {
        speed = 1.5f;
    }
    angle = (i32)(((1.5 - (f64)speed) / 1.5) * 16384.0 + 16384.0);
    volume = NuTrigTable[(angle >> 1) & 0x7fff];
    if (GAMEDEMO != 0) {
        PlaySfxAndSetVolume("PickupCoin", &part->position, volume);
        coinimpactwait = 0.1f;
    } else if (part->part_id >= 0xb7 && part->part_id <= 0xba) {
        PlaySfxAndSetVolume("CoinDropS", &part->position, volume);
    } else {
        PlaySfxAndSetVolume("CoinDropG", &part->position, volume);
    }
}

static void PartExtra_BlueCoin(PART *part) {
    if ((part->active & PART_STATE_DRAWN) != 0) {
        PartExtraCoinsPrivate(&part->position, 56);
    }
}

void PartExtra_PurpleCoin(PART *part) {
    if ((part->active & PART_STATE_DRAWN) != 0) {
        PartExtraCoinsPrivate(&part->position, 57);
    }
}

static f32 GizmopickupFdiv(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f) {
        return 0.0f;
    }
    return a / b;
}

static void PartMove_VehiclePickup(PART *part, f32 timestep) {
    f32 gain;
    f32 target;
    GAMEOBJECT *object;
    if (part->initial_time == 0.0f) {
        gain = 0.0f;
    } else {
        gain = 1.0f - GizmopickupFdiv(part->scale_time, part->initial_time);
    }
    if (gain <= 0.0f) {
        part->active |= PART_STATE_STOPPED;
        if (part->stop_fn != NULL) {
            part->stop_fn(part);
        }
        return;
    }
    object = FindNearestGameObject(&part->position, NULL, 0, 0.0f, 0.0f, -1, -1, 99, NULL, 0, NULL);
    if (object != NULL) {
        target = (f32)((f64)(object->collision_position.y - part->position.y) * 3.0);
        part->velocity.y = SeekValF(part->velocity.y, target, 3.0f);
    }
    part->position.x += part->velocity.x * gain * FRAMETIME;
    part->position.y += part->velocity.y * gain * FRAMETIME;
    part->position.z += part->velocity.z * gain * FRAMETIME;
}

f32 PartVehicleHeartLifetime = 1.0f;
f32 PartVehicleHeartPodraceHeight = 2.0f;
f32 PartVehicleHeartHeight = 3.0f;

static void PartMove_VehicleHeart(PART *part, f32 timestep) {
    GAMEOBJECT *recipient = part->recipient;
    f32 progress = 1.0f - part->time_remaining / PartVehicleHeartLifetime;
    NUVEC start;
    f32 height;
    if (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA) {
        NuVecAdd(&start, &recipient->collision_position, &part->previous_position);
        height = PartVehicleHeartPodraceHeight;
    } else {
        start = part->previous_position;
        height = PartVehicleHeartHeight;
    }
    part->position.x = start.x + (recipient->collision_position.x - start.x) * progress;
    part->position.y = start.y + (recipient->collision_position.y - start.y) * progress +
                       NuTrigTable[((i32)((f64)progress * 32768.0) >> 1) & 0x7fff] * height;
    part->position.z = start.z + (recipient->collision_position.z - start.z) * progress;
}

f32 PartVehicleHeartPodraceScale = 4.0f;
f32 PartVehicleHeartGunshipScale = 7.5f;
f32 PartVehicleHeartScale = 5.0f;

static i32 PartDraw_VehicleHeart(PART *part) {
    NUVEC scaling;
    NUMTX matrix;
    if (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA) {
        scaling.x = PartVehicleHeartPodraceScale;
    } else if ((BONUS_GUNSHIP_ADATA != NULL && WORLD->area == BONUS_GUNSHIP_ADATA) ||
               (GUNSHIP_ADATA != NULL && WORLD->area == GUNSHIP_ADATA)) {
        scaling.x = PartVehicleHeartGunshipScale;
    } else {
        scaling.x = PartVehicleHeartScale;
    }
    scaling.x = (1.0f - part->time_remaining / PartVehicleHeartLifetime) * scaling.x;
    if (scaling.x > 0.0f) {
        scaling.y = scaling.z = scaling.x;
        matrix = part->transform;
        NuMtxPreScale(&matrix, &scaling);
        if (NuSpecialDrawAt(&part->special, &matrix) != 0) {
            part->active |= 0x600;
        }
    }
    return 0;
}

void AddTorpedoAsPart(NUVEC *position, NUVEC *velocity, f32 scale, f32 lifetime) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    ADDPART params = Default_ADDPART;
    PART *part;
    params.position = position;
    params.velocity = velocity;
    params.start_radius = (f32)((f64)WORLD->torpedo_machine_sys->scale * 4.0 * (f64)GizmoPickupType[8].shadow_radius_x);
    params.end_radius = (f32)((f64)WORLD->torpedo_machine_sys->scale * 4.0 * (f64)GizmoPickupType[8].shadow_extent_x);
    params.part_id = 0x79;
    params.special = &world->level_objects[0x79].special;
    params.gravity = AreaPickupGravity;
    if (params.gravity == 0.0f) {
        params.fade_time = 5.0f;
    } else {
        params.stop_fn = PartStop_Flickerer;
    }
    params.collision_fn = PartCollide_3D;
    params.draw_fn = PartDraw_Torp;
    params.update_fn = NULL;
    params.flags = PART_FLAG_COLLIDE_WITH_CHARACTERS | PART_FLAG_ACTIVE_CHARACTERS_ONLY | PART_FLAG_KEEP_OFF_SCREEN |
                   PART_FLAG_COLLECTIBLE | PART_FLAG_SLOT_CANNOT_BE_STOLEN;
    params.lifetime = lifetime;
    if (params.gravity == 0.0f) {
        params.move_fn = PartMove_VehiclePickup;
    }
    params.time_step = FRAMETIME;
    params.model_scale = scale;
    part = AddPart(&params);
    if (part != NULL) {
        part->rotation_y = (u16)qrand();
        part->player_mask = 3;
        part->unknown_214 = (f32)((f64)WORLD->torpedo_machine_sys->scale * 4.0 * (f64)NuTrigTable[0x2000]);
    }
}

void AddHeartAsPart(GAMEOBJECT *recipient, NUVEC *position, NUVEC *velocity, f32 scale, f32 lifetime) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    ADDPART params;
    PART *part;
    if (recipient == NULL) {
        return;
    }
    params = Default_ADDPART;
    params.position = position;
    params.velocity = velocity;
    params.part_id = 0xcb;
    params.end_radius = params.start_radius = scale * GizmoPickupType[5].shadow_radius_x;
    params.gravity = AreaPickupGravity;
    params.special = &world->level_objects[0xcb].special;
    params.lifetime = lifetime;
    params.collision_fn = PartCollide_3D;
    params.update_fn = PartUpdate_Heart;
    params.flags = PART_FLAG_COLLIDE_WITH_CHARACTERS | PART_FLAG_ACTIVE_CHARACTERS_ONLY | PART_FLAG_KEEP_OFF_SCREEN |
                   PART_FLAG_COLLECTIBLE | PART_FLAG_SLOT_CANNOT_BE_STOLEN;
    if ((world->current_level->flags & LEVEL_PICKUPS_TO_PANEL) != 0) {
        params.fade_time = PartVehicleHeartLifetime;
        params.move_fn = PartMove_VehicleHeart;
        params.recipient = recipient;
        params.draw_fn = PartDraw_VehicleHeart;
        params.flags |= PART_FLAG_IGNORE_TERRAIN;
    } else {
        params.stop_fn = PartStop_Flickerer;
        params.draw_fn = PartDraw_Flickerer;
        if (params.gravity == 0.0f) {
            params.move_fn = PartMove_VehiclePickup;
        }
    }
    params.time_step = FRAMETIME;
    params.model_scale = scale;
    part = AddPart(&params);
    if (part != NULL) {
        part->rotation_y = (u16)qrand();
    }
}

static void PowerUp_ImpactPart(PART *part) {
    f32 minimum = (f32)((f64)AreaPickupScale * (f64)0.1f);
    f32 maximum = (f32)((f64)AreaPickupScale * 2.5);
    f32 speed = NuVecMag(&part->velocity);
    f32 volume;
    if (speed >= maximum) {
        volume = 1.0f;
    } else {
        if (!(speed >= minimum)) {
            return;
        }
        volume = (speed - minimum) / (maximum - minimum);
    }
    if (volume > 0.0f) {
        PlaySfxAndSetVolume("Gungan_BlueOrbBounce", &part->position, volume);
    }
}

static void PowerUp_UpdatePart(PART *part) {
    NUVEC position = part->position;
    part->rotation_y = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
    NuMtxSetRotationY(&part->transform, part->rotation_y);
    NuMtxTranslate(&part->transform, &position);
    PowerUp_Particles(WorldInfo_CurrentlyActive(), &position);
    PlaySfx("Grv_GuardWeaponLp", &position);
}

static i32 PowerUp_DrawPart(PART *part) {
    i32 draw = PartDraw_Flickerer(part);
    if (draw && WORLD->level_objects[0xd1].active != 0) {
        NUMTX matrix = part->transform;
        NuSpecialDrawAt(&WORLD->level_objects[0xd1].special, &matrix);
    }
    return draw;
}

void PowerUp_AddPart(NUVEC *position, NUVEC *velocity, f32 scale, f32 lifetime) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    ADDPART params = Default_ADDPART;
    PART *part;
    params.end_radius = params.start_radius = scale * GizmoPickupType[9].shadow_radius_x;
    params.gravity = AreaPickupGravity;
    params.position = position;
    params.velocity = velocity;
    params.part_id = 0xd0;
    params.special = &world->level_objects[0xd0].special;
    params.collision_fn = PartCollide_3D;
    params.update_fn = PowerUp_UpdatePart;
    params.impact_fn = PowerUp_ImpactPart;
    params.flags = PART_FLAG_COLLIDE_WITH_CHARACTERS | PART_FLAG_ACTIVE_CHARACTERS_ONLY | PART_FLAG_KEEP_OFF_SCREEN |
                   PART_FLAG_COLLECTIBLE | PART_FLAG_SLOT_CANNOT_BE_STOLEN;
    params.stop_fn = PartStop_Flickerer;
    params.draw_fn = PowerUp_DrawPart;
    params.lifetime = lifetime;
    if (params.gravity == 0.0f) {
        params.move_fn = PartMove_VehiclePickup;
    }
    params.time_step = FRAMETIME;
    params.model_scale = scale;
    part = AddPart(&params);
    if (part != NULL) {
        part->rotation_y = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
    }
}

/* Every stud the level can release, optionally split per source. AI
 * characters count only in areas flagged 0x100. */
u32 TotalLevelCoinTally(WORLDINFO *world, u32 *pickups, u32 *blowups, u32 *buildits, u32 *forces, u32 *obstacles, u32 *turrets,
                        u32 *characters) {
    u32 value = GizmoPickups_TotalScore(world);
    u32 total = value;
    if (pickups != NULL) {
        *pickups = value;
    }
    value = GizmoBlowups_TotalScore(world);
    total += value;
    if (blowups != NULL) {
        *blowups = value;
    }
    value = GizBuildIts_TotalScore(world);
    total += value;
    if (buildits != NULL) {
        *buildits = value;
    }
    value = GizForce_TotalScore(world);
    total += value;
    if (forces != NULL) {
        *forces = value;
    }
    value = GizObstacles_TotalScore(world);
    total += value;
    if (obstacles != NULL) {
        *obstacles = value;
    }
    value = GizTurrets_TotalScore(world);
    total += value;
    if (turrets != NULL) {
        *turrets = value;
    }
    if (world->area != NULL && (world->area->flags & AREAFLAG_SUPER_BONUS) != 0) {
        value = GameAI_TotalScore();
    } else {
        value = 0;
    }
    total += value;
    if (characters != NULL) {
        *characters = value;
    }
    return total;
}

void CollectHitPoint(GAMEOBJECT *object, NUVEC *position, i32 flags) {
    NUVEC momentum;
    // MATCH-EXCEPTION: the original tests the same range twice (likely a macro/PC variant pair that
    // expanded to the same check); a single test drops the second compare.
    if ((object->current_hp > 0 && object->current_hp < object->max_hitpoints) ||
        (object->current_hp > 0 && object->current_hp < object->max_hitpoints)) {
        ++object->current_hp;
    }
    if (object->player.saved_hitpoints < 4) {
        ++object->player.saved_hitpoints;
    }
    if (object->character_data->model_flags & CHARACTER_MODEL_VEHICLE) {
        NuVecScale(&momentum, &object->velocity, 0.5f);
        AddGameDebrisMom(WORLD->game_debris_sys, 0x10, position, 15, &momentum);
    } else {
        AddGameDebris(WORLD->game_debris_sys, 15, position);
    }
    GameAudio_PlaySfx(0x27, position, 0, 0);
    NewBuzz(object->player.pad->bound_pad, 0.1f, 0);
}

i32 ReleaseHearts(void) {
    GAMEOBJECT *player = Player[0];
    i32 missing_hitpoints = 0;
    i32 total_hitpoints = 0;
    i32 i;
    i32 hitpoints;
    i32 base;
    i32 chance;
    i32 adjustment;
    if (player != NULL && player->dead == 0) {
        hitpoints = player->max_hitpoints;
        if (hitpoints > 0) {
            missing_hitpoints = hitpoints - player->current_hp;
            total_hitpoints = hitpoints;
        }
    }
    player = Player[1];
    if (player != NULL && player->dead == 0) {
        hitpoints = player->max_hitpoints;
        if (hitpoints > 0) {
            missing_hitpoints += hitpoints - player->current_hp;
            total_hitpoints += hitpoints;
        }
    }
    if (total_hitpoints == 0) {
        return 0;
    }
    if (missing_hitpoints > 0 && MAXPARTS > 0) {
        for (i = 0; i < MAXPARTS; ++i) {
            if ((Part[i].active & PART_IN_USE) && Part[i].part_id == 0xcb) {
                --missing_hitpoints;
            }
        }
    }
    base = 8;
    if (Game.difficulty > 5) {
        base = 9;
    }
    missing_hitpoints += base - Game.difficulty;
    if (missing_hitpoints > total_hitpoints) {
        missing_hitpoints = total_hitpoints;
    } else if (missing_hitpoints < 0) {
        missing_hitpoints = 0;
    }
    chance = (missing_hitpoints << 16) / total_hitpoints;
    if (Cheat_IsOn(0x2c)) {
        adjustment = adtab[adaptivedifficulty.level][1];
        if (adjustment == 1) {
            chance /= 2;
        } else if (adjustment == -1) {
            chance += chance;
        }
    }
    return qrand() < chance;
}

void SetCoinType(i32 model, GIZMOPICKUP *pickup) {
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
    if (count > 0) {
        pickup->model_variant %= count;
    } else {
        pickup->model_variant = 0;
    }
}

u8 CoinTab[4] = {0, 1, 2, 3};

i32 GetRandomCoinType() {
    return CoinTab[qrand() / 16384];
}

i32 fun_0046c6b0(i32 type) {
    i32 i;
    for (i = 0; i < 4; ++i) {
        if (CoinTab[i] == type) {
            return 1;
        }
    }
    return 0;
}

i32 LoseCoins(GAMEOBJECT *object, i32 cause) {
    u32 lost;
    i32 adjustment;
    if (Player_HasInvincibility(object)) {
        return 0;
    }
    if (object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_GHOST) {
        return 0;
    }
    if (object->player.coin_packet == NULL) {
        return 0;
    }
    switch (cause) {
        case 1:
            lost = BonusArea != 0 ? 25000 : 2000;
            break;
        case 2:
            lost = BonusArea != 0 ? 12500 : 1000;
            break;
        default:
            lost = 0;
            break;
    }
    if (Cheat_IsOn(0x2c)) {
        adjustment = adtab[adaptivedifficulty.level][0];
        if (adjustment == 1) {
            lost += lost;
        } else if (adjustment == -1) {
            lost >>= 1;
        }
    }
    if (lost > object->player.coin_packet->coins) {
        lost = object->player.coin_packet->coins;
        object->player.coin_packet->coins = 0;
    } else if (lost > 0) {
        object->player.coin_packet->coins -= lost;
    }
    if (Cheats_CheckFlags(CHEAT_SCORE_X2 | CHEAT_SCORE_X4 | CHEAT_SCORE_X6 | CHEAT_SCORE_X8 | CHEAT_SCORE_X10) || DoubleScoreTime > 0.0f) {
        lost = 0;
    }
    return (i32)lost;
}

void GizmoPickups_SetOnOff(void) {
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
            off = challenge != CHALLENGE_OFF;
        } else if (challenge != CHALLENGE_OFF) {
            off = 0;
        } else {
            off = 1;
            if (index == 6 && world->area_index != -1 && Game.area_save[world->area_index].red_brick_collected != 0) {
                off = 0;
            } else if (index == 4 && SuperStory != 0) {
                off = 0;
            } else if (index == 9 && WORLD->area != NULL && (WORLD->area->flags & AREAFLAG_SUPER_BONUS) != 0) {
                off = 0;
            } else if ((arcade_flags & 0x10) != 0 && index != 5 && index != 9) {
                off = 0;
            }
        }
        GizmoPickupType[index].disabled = off == 0;
    }
}

void Pup_CollectCoin(WORLDINFO *world, GIZMOPICKUP *pickup, i32 type, GAMEOBJECT *object, i32 flags) {
    GizmoPickup_CollectCoin(world, &pickup->position, type, pickup->model_variant, object, flags);
}

void Pup_CollectMinikit(WORLDINFO *world, GIZMOPICKUP *pickup, i32 type, GAMEOBJECT *object, i32 flags) {
    CollectMinikit(&pickup->position, pickup->name, flags);
    NewBuzz(object->player.pad->bound_pad, 0.2f, 0);
}

void Pup_CollectRedBrick(WORLDINFO *world, GIZMOPICKUP *pickup, i32 type, GAMEOBJECT *object, i32 flags) {
    CollectRedBrick(&pickup->position);
    NewBuzz(object->player.pad->bound_pad, 0.2f, 0);
}

void Pup_CollectHeart(WORLDINFO *world, GIZMOPICKUP *pickup, i32 type, GAMEOBJECT *object, i32 flags) {
    CollectHitPoint(object, &pickup->position, flags);
}

static void PartStolen_Coin(PART *part) {
    GAMEOBJECT *recipient;
    i32 first_active;
    i32 second_active;
    GIZMOPICKUP pickup;
    if (netclient != 0 || (part->active & PART_STATE_DRAWN) == 0) {
        return;
    }
    recipient = Player[0];
    first_active = recipient != NULL && (recipient->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0;
    second_active = Player[1] != NULL && (Player[1]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0;
    if (first_active) {
        if (second_active) {
            recipient = Player[qrand() / 32768];
        }
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

static void PartStop_Coin(PART *part) {
    GIZMOPICKUPRUNTIMESYS_s *system;
    GIZMOPICKUP *pickup;
    if (netclient != 0) {
        part->active &= ~PART_IN_USE;
        return;
    }
    system = WORLD->pickup_runtime_sys;
    pickup = &system->temporary_pickups[system->temporary_cursor];
    ++system->temporary_cursor;
    if (WORLD->pickup_runtime_sys->temporary_cursor >= 64) {
        WORLD->pickup_runtime_sys->temporary_cursor = 0;
    }
    if ((pickup->flags & GIZMOPICKUP_STATE_ACTIVE) != 0) {
        GizmoPickup_CollectCoin(WORLD, &pickup->position, pickup->type_index, pickup->model_variant, NULL, 1);
    }
    SetCoinType(part->part_id, pickup);
    pickup->flags =
        (pickup->flags & 0xf7) | GIZMOPICKUP_STATE_ACTIVE | GIZMOPICKUP_STATE_ENABLED | GIZMOPICKUP_STATE_VISIBLE | GIZMOPICKUP_STATE_DRAWN;
    pickup->position = part->position;
    pickup->rotation = 0;
    pickup->room = WORLD->current_gscn != NULL ? NuPortalWhichRoom(WORLD->current_gscn, &pickup->position) : -1;
    pickup->floor = 2000000.0f;
    pickup->timer = 5.0f;
    pickup->shadow_x = 0;
    pickup->shadow_z = 0;
    pickup->flags = (pickup->flags | GIZMOPICKUP_STATE_DRAW_VISIBLE) & ~GIZMOPICKUP_STATE_ALTERNATE_TYPE;
    part->active &= ~PART_IN_USE;
}

void AddCoinsAsParts(i32 type_id, NUVEC *position, NUVEC *velocity, f32 lifetime, f32 scale) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    GIZMO_PICKUP_TYPE *type = &GizmoPickupType[type_id];
    i32 model = type->first_model_id;
    ADDPART params;
    PART *part;
    if (type->random_model_count > 0) {
        model += qrand() / (65535 / type->random_model_count + 1);
    }
    if (world->level_objects[model].active == 0) {
        return;
    }
    params = Default_ADDPART;
    params.position = position;
    params.velocity = velocity;
    params.part_id = model;
    params.end_radius = params.start_radius = scale * type->shadow_radius_x;
    params.gravity = AreaPickupGravity;
    params.special = &world->level_objects[model].special;
    params.flags = PART_FLAG_COLLIDE_WITH_CHARACTERS | PART_FLAG_ACTIVE_CHARACTERS_ONLY | PART_FLAG_KEEP_OFF_SCREEN | PART_FLAG_COLLECTIBLE;
    if (params.part_id >= 0xd5 && params.part_id <= 0xd8) {
        params.flags |= PART_FLAG_SLOT_CANNOT_BE_STOLEN;
    } else if (params.part_id >= 0xc7 && params.part_id <= 0xca) {
        params.flags |= PART_FLAG_SLOT_CANNOT_BE_STOLEN;
        params.callback_30 = PartExtra_BlueCoin;
    } else if (params.part_id >= 0xd5 && params.part_id <= 0xd8) {
        params.flags |= PART_FLAG_SLOT_CANNOT_BE_STOLEN;
        params.callback_30 = PartExtra_PurpleCoin;
    } else {
        params.destroy_fn = PartStolen_Coin;
    }
    params.lifetime = lifetime;
    params.collision_fn = PartCollide_3D;
    params.stop_fn = PartStop_Coin;
    params.impact_fn = PartImpact_Coin;
    params.time_step = FRAMETIME;
    params.model_scale = scale;
    part = AddPart(&params);
    if (part != NULL) {
        part->time_remaining = 30.0f;
        if (params.gravity == 0.0f) {
            part->move_fn = PartMove_VehiclePickup;
            part->initial_time = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) + 4.0);
        }
    }
}

f32 AddCoinDelay[2] = {0};

void AddCoinsToPanel(i32 coins, NUVEC *position, i32 player, f32 unused, GAMEOBJECT *owner, i32 random_types) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 remainder;
    i32 remaining;
    i32 type;
    i32 score;
    i32 i;
    i32 base_model;
    i32 model;
    i32 main_total;
    u32 j;
    u8 counts[10];
    NUVEC target;
    f32 target_scale;
    GIZMO_PICKUP_TYPE *pickup;
    ADD_GAME_MESSAGE message;
    if (coins == 0) {
        return;
    }
    if (ChallengeMode != CHALLENGE_OFF || Mission_Active(NULL) != NULL) {
        coins = 0;
    }
    memset(counts, 0, sizeof(counts));
    remainder = coins % 10;
    if (remainder > 0) {
        coins -= remainder;
    }
    if (coins > 512000) {
        coins = 512000;
    }
    if (coins > 0) {
        PlaySfx("CoinsLand", position);
    }
    remaining = coins;
    if (random_types) {
        while (remaining > 0) {
            type = CoinTab[qrand() / 16384];
            score = GizmoPickupType[type].score;
            if (remaining - score < 0) {
                continue;
            }
            ++counts[type];
            remaining -= score;
        }
    } else if (remaining > 0) {
        for (i = 3; remaining > 0; --i) {
            type = CoinTab[i];
            score = GizmoPickupType[type].score;
            while (remaining - score >= 0) {
                ++counts[type];
                remaining -= score;
            }
        }
    }
    if (player != 0 && player != 1) {
        player = -1;
    }
    target.x = player == 1 ? PANEL_COINX : -PANEL_COINX;
    if (CoinsGoToMainTotal()) {
        main_total = 1;
        target.y = STATSPOSY;
        target_scale = COINTOTAL_COINSIZE;
    } else {
        main_total = 0;
        target.y = STATSPOSY + PANEL_COINY;
        target_scale = PANEL_COINSCALE_END;
    }
    target.z = 1.0f;
    DrawBuildUpTime = (f32)(COINMSGTIME + 1.0);
    for (i = 0; i < 4; ++i) {
        pickup = &GizmoPickupType[CoinTab[i]];
        base_model = pickup->first_model_id;
        for (j = counts[CoinTab[i]]; j > 0; --j) {
            model = base_model;
            if (pickup->random_model_count > 0) {
                model += qrand() / (65535 / pickup->random_model_count + 1);
            }
            if (world->level_objects[model].active == 0 || player == -1) {
                continue;
            }
            message = AddGameMsg_Default;
            message.scale = 1.0f;
            message.target_scale = target_scale;
            message.position = position;
            message.target_position = &target;
            message.icon = (i16)model;
            message.special = &world->level_objects[message.icon];
            message.score = pickup->score;
            message.end_fn = EndScoreMessage;
            message.player_index = (i8)player;
            message.allow_temporary = 1;
            message.delay = AddCoinDelay[player];
            message.flags = GAMEMSG_WORLD_POSITION | GAMEMSG_WORLD_SCALE | GAMEMSG_DRAW_MOVE | GAMEMSG_DRAW_SCALE | GAMEMSG_EASE_IN;
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

void AddPickups(i32 coins, i32 hearts, i32 torpedoes, i32 powerups, NUVEC *position, NUVEC *direction, f32 unused, i32 player_id, f32 speed,
                f32 lifetime, GAMEOBJECT *owner, i32 random_types, i32 panel) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    const f32 scale = AreaPickupScale;
    u32 arcade_flags;
    i32 to_panel;
    i32 remainder;
    i32 remaining;
    i32 i;
    i32 j;
    i32 type;
    i32 score;
    i32 count;
    i32 both_players;
    u8 counts[10];
    NUVEC velocity;
    GAMEOBJECT *recipient;
    GAMEOBJECT *eligible[2];
    Arcade_GetMode(&arcade_flags);
    if (arcade_flags & 0x10) {
        coins = torpedoes = 0;
    }
    if (world->area != NULL && (world->area->flags & AREAFLAG_SUPER_BONUS_AREA) == 4) {
        random_types = 0;
    }
    if (world->current_level->flags & LEVEL_PICKUPS_TO_PANEL) {
        to_panel = 1;
        if (player_id == -1) {
            if (player2 != NULL) {
                player_id = qrand() / 32768;
            } else if (Player[0] != NULL && (Player[0]->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE)) {
                player_id = 0;
            } else {
                player_id = 1;
            }
        }
    } else if (world->area != NULL && (world->area->flags & AREAFLAG_SUPER_BONUS)) {
        player_id = qrand() / 32768;
        to_panel = 1;
    } else {
        to_panel = player_id == -1 ? 0 : panel;
    }
    if (ChallengeMode != CHALLENGE_OFF || Mission_Active(NULL) != NULL) {
        coins = 0;
    }
    if (speed == 1.0f) {
        speed *= scale;
    }
    memset(counts, 0, sizeof(counts));
    if (netclient != 0) {
        return;
    }
    remainder = coins % 10;
    if (remainder > 0) {
        coins -= remainder;
    }
    if (coins > 512000) {
        coins = 512000;
    }
    if (coins > 0) {
        PlaySfx("CoinsLand", position);
    }
    remaining = coins;
    if (random_types) {
        while (remaining > 0) {
            type = CoinTab[qrand() / 16384];
            score = GizmoPickupType[type].score;
            if (remaining - score < 0) {
                continue;
            }
            ++counts[type];
            remaining -= score;
        }
    } else if (remaining > 0) {
        for (i = 3; remaining > 0; --i) {
            type = CoinTab[i];
            score = GizmoPickupType[type].score;
            while (remaining - score >= 0) {
                ++counts[type];
                remaining -= score;
            }
        }
    }
    if (player_id != 0 && player_id != 1) {
        player_id = -1;
    }
    if (to_panel) {
        if (coins > 0) {
            DrawBuildUpTime = (f32)(COINMSGTIME + 1.0);
        }
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
    if (hearts > 0 && VehicleArea != 0 && Player[0] != NULL && (Player[0]->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) &&
        Player[1] != NULL && (Player[1]->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE)) {
        hearts = 2;
        both_players = 1;
    } else {
        both_players = 0;
    }
    for (i = 0; i < hearts; ++i) {
        if (to_panel) {
            velocity = v000;
        } else {
            MakePartVector(&velocity, direction, speed);
        }
        recipient = NULL;
        if (both_players) {
            recipient = i == 0 ? Player[0] : Player[1];
        } else if (owner != NULL) {
            recipient = owner;
        } else {
            count = 0;
            if (Player[0] != NULL && (Player[0]->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE)) {
                eligible[count++] = Player[0];
            }
            if (Player[1] != NULL && (Player[1]->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE)) {
                eligible[count++] = Player[1];
            }
            if (count > 0) {
                recipient = eligible[count == 1 ? 0 : qrand() / 32768];
            }
        }
        AddHeartAsPart(recipient, position, &velocity, scale, lifetime);
    }
    for (i = 0; i < torpedoes; ++i) {
        MakePartVector(&velocity, direction, speed);
        AddTorpedoAsPart(position, &velocity, scale, lifetime);
    }
    if (!(world->current_level->flags & LEVEL_PICKUPS_TO_PANEL)) {
        for (i = 0; i < powerups; ++i) {
            MakePartVector(&velocity, direction, speed);
            PowerUp_AddPart(position, &velocity, scale, lifetime);
        }
    }
}

void AddMiscPickups(NUVEC *position, i32 player_id, i32 coins, i32 torpedoes) {
    if (torpedoes != 0) {
        AddPickups(coins, 0, torpedoes, 0, position, &v010, 5.0f, player_id, 1.0f, 2000000.0f, NULL, 1, 0);
    } else {
        AddPickups(coins, ReleaseHearts(), 0, 0, position, &v010, 5.0f, player_id, 1.0f, 2000000.0f, NULL, 1, 0);
    }
}
