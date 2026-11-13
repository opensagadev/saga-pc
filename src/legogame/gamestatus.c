#include <stdio.h>
#include "saga.h"

/* Retail unit 0x45e350-0x46433e: definitions in retail address order. */

void __cdecl Status_DrawPromptMenu(STATUSPACKET_s *packet, i32 selected, f32 alpha);
void __cdecl Text_FillInExtendedSaveInfo(void);
i32 __cdecl TriggerAutoSave(void);

void __cdecl Exit_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed);
void __cdecl Fade_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed);
void __cdecl Prompt_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 active);
void __cdecl Prompt_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed);

void __cdecl ResetStatusPacket_LSW(STATUSPACKET_s *packet) {
    packet->lsw_packet->field_0x00 = 0;
}

i32 __cdecl InitStatusScreen_LSW(WORLDINFO *world, STATUSPACKET_s *packet) {
    (void)world;
    (void)packet;
    return 0;
}

i32 __cdecl FinishStatusPacket_LSW(WORLDINFO *world, STATUSPACKET_s *packet, i32 result) {
    (void)world;
    (void)packet;
    (void)result;
    return 0;
}

void __cdecl DrawStatusBG_LSW(STATUSPACKET_s *packet) {
    if (packet->stage->field_0x14 == -1) {
        packet->stage->field_0x18 = 0.0f;
        packet->stage->field_0x14 = 0;
    }
}

f32 __cdecl getFinishedStatusAlpha(STATUSPACKET_s *packet) {
    STATUS_STAGE_s *stage = packet->stage;
    if (stage->type == 12 || stage->type == 11)
        return 0.0f;
    if (stage->type == 10) {
        if (stage->field_0x18 < 1.0f) {
            f32 alpha = 1.0 - (f64)stage->field_0x18;
            return alpha;
        }
        return 0.0f;
    }
    if (stage->type == 19) {
        if (stage->field_0x14 == 0)
            return 1.0f;
        if (stage->field_0x18 < 1.0f) {
            f32 alpha = 1.0 - (f64)stage->field_0x18;
            return alpha;
        } else {
            f32 fade_start = (f64)stage->field_0x1c - 1.0;
            if (stage->field_0x18 >= fade_start) {
                f32 alpha = ((f64)stage->field_0x18 - (f64)fade_start) / ((f64)stage->field_0x1c - (f64)fade_start);
                return alpha;
            }
            return 0.0f;
        }
    }
    if (stage == NULL)
        return 0.0f;
    return 1.0f;
}

void __cdecl AllMiniKits_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    AddToCollection(VehicleBoltId7f195cPc);
    NextStatusStage(packet);
}

void __cdecl AllMiniKits_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0:
            stage->field_0x14 = 1;
            stage->field_0x18 = 0.0f;
            stage->field_0x1c = 6.5f;
            break;
        case 1: {
            f32 previous = stage->field_0x18;
            stage->field_0x18 = stage->field_0x18 + elapsed;
            if (stage->field_0x18 >= stage->field_0x1c)
                NextStatusStage(packet);
            else if (previous < 0.5f && stage->field_0x18 >= 0.5f) {
                PlaySfx("StatusAward", NULL);
                NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
            } else if (previous < 4.0f && stage->field_0x18 >= 4.0f)
                PlaySfx("Char_Icon_App", NULL);
            break;
        }
    }
}

void __cdecl RedBrick_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    NextStatusStage(packet);
}

void __cdecl RedBrick_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0:
            stage->field_0x14 = 1;
            stage->field_0x18 = 0.0f;
            stage->field_0x1c = 5.0f;
            break;
        case 1: {
            f32 previous = stage->field_0x18;
            stage->field_0x18 = stage->field_0x18 + elapsed;
            if (stage->field_0x18 >= stage->field_0x1c)
                NextStatusStage(packet);
            else if (previous < 0.75f && stage->field_0x18 >= 0.75f) {
                PlaySfx("TrueJedi_100pc", NULL);
                NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
            }
            break;
        }
    }
}
f32 StatusIconYPc = 0;

void __cdecl Coins_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    u32 remaining;
    (void)stage;
    StatusIconYPc = StatusIconsOnOff(1.0f);
    remaining = packet->coins_remaining[0] + packet->coins_remaining[1];
    IncreaseScore(packet->score, (u64)remaining, 0);
    packet->coins_remaining[0] = 0;
    packet->coins_remaining[1] = 0;
    NextStatusStage(packet);
}
i32 StatusDrawPlayerIconsPc = 0;

void __cdecl Coins_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    i32 finished = 0;
    switch (stage->field_0x14) {
        case 0:
            stage->field_0x18 = 0.0f;
            stage->field_0x14 = 1;
            stage->field_0x1c = 1.0f;
            StatusDrawPlayerIconsPc = 1;
            break;
        case 1: {
            f32 accumulated = elapsed + stage->field_0x18;
            stage->field_0x18 = accumulated;
            if (accumulated >= stage->field_0x1c)
                stage->field_0x14 = 2;
            break;
        }
        case 2: {
            i32 player;
            for (player = 0; player < 2; ++player) {
                u32 remaining = packet->coins_remaining[player];
                if (remaining != 0) {
                    u32 threshold = 100, rate = 35, amount;
                    while (remaining >= threshold) {
                        threshold *= 10u;
                        rate *= 10u;
                    }
                    amount = (u32)((f64)rate * (f64)elapsed);
                    if (amount > remaining)
                        amount = remaining;
                    else if (amount < 1)
                        amount = 1;
                    packet->coins_remaining[player] = remaining - amount;
                    IncreaseScore(packet->score, (u64)amount, 0);
                }
                if (packet->coins_remaining[player] == 0)
                    ++finished;
            }
            PlaySfx("PickupCoin", NULL);
            if (finished == 2) {
                stage->field_0x14 = 3;
                stage->field_0x18 = 0.0f;
                stage->field_0x1c = 1.0f;
                if (packet->coins_collected[0] + packet->coins_collected[1] == 0)
                    GameAudio_PlaySfx(0x32, NULL, 0, 0);
                else
                    PlaySfx("Shop_BuyCheat", NULL);
            }
            break;
        }
        case 3: {
            f32 accumulated = elapsed + stage->field_0x18;
            stage->field_0x18 = accumulated;
            if (accumulated >= stage->field_0x1c)
                NextStatusStage(packet);
            break;
        }
    }
}

void __cdecl GoldBrick_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    if (Game.gold_bricks <= 60)
        NextStatusStage(packet);
}

void __cdecl GoldBrick_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    i32 last = (i32)Game.gold_bricks - (i32)packet->previous_gold_bricks - 1;
    char text[256];
    NUVEC position;
    switch (stage->field_0x14) {
        case 0: {
            stage->field_0x18 = 0.0f;
            stage->field_0x14 = 1;
            stage->field_0x1c = (((f64)last * 2.5 + 2.0) + 3.0) + 1.0 + 1.0;
            break;
        }
        case 1: {
            const f32 previous = stage->field_0x18;
            f32 accumulated = stage->field_0x18 + elapsed;
            i32 index;
            stage->field_0x18 = accumulated;
            if (stage->field_0x18 >= stage->field_0x1c) {
                NextStatusStage(packet);
                return;
            }
            for (index = 0; index <= last; ++index) {
                f32 award_time = (f64)index * 2.5 + 2.0;
                if (previous < award_time && stage->field_0x18 >= award_time) {
                    i32 total;
                    i16 message_id;
                    char *message;
                    PlaySfx("TrueJedi_100pc", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                    total = (i32)packet->previous_gold_bricks + index + 1;
                    sprintf(text, "%i/%i", total, GOLDBRICKPOINTS);
                    AddFancyMessage(text, 0.0f, 0.3f, 0.75f, 1.25f, 1, 1);
                    message_id = ((i16 *)packet->field_0x12c)[index];
                    position.x = 0.0f;
                    position.y = 0.0375f;
                    position.z = 1.0f;
                    if (message_id == TextIdPc880f9c) {
                        NuStrCpy(text, TTab[message_id]);
                        if (HubBothTrueJediGoldBricksPc != 0) {
                            i16 name_id;
                            NuStrCat(text, CutPlayerTextSeparatorPc);
                            name_id = (packet->field_0xb0 & 0x40) != 0 ? TextIdPc8808a8 : TextIdPc8808a4;
                            NuStrCat(text, TTab[name_id]);
                        }
                        message = text;
                    } else
                        message = TTab[message_id];
                    AddGameMessage(message, &position, 0.7f, &position, 0.7f, 255, 0, 127, 0x4020, 2.0f);
                    if (total > 60) {
                        IncreaseScore(packet->score, 100000, 0);
                        CoinTotalScale = 1.5f;
                        PlaySfx("Shop_BuyCheat", NULL);
                        Text_MakeScore(100000, text);
                        AddFancyMessage(text, 0.0f, -STATSPOSY, 0.5f, 1.25f, 0, 0);
                    }
                }
            }
        }
    }
}

void __cdecl Prompt_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    i32 selected = 0;
    i32 moved_up = 0;
    i32 moved_down = 0;
    if (stage->field_0x14 == 0) {
        f32 accumulated = (f64)elapsed + (f64)stage->field_0x18;
        stage->field_0x18 = accumulated;
        if (stage->field_0x18 >= 0.5f) {
            i32 player;
            for (player = 0; player < 2 && selected == 0 && moved_up == 0 && moved_down == 0; ++player) {
                GAMEPAD_INIT_RECORD *pad = &GamePadStorage[player];
                if (packet->player_active[player] != 0) {
                    if ((pad->menu_buttons_pressed & GAMEPAD_MENUSELECT) != 0) {
                        selected = 1;
                    } else if (((packet->flags_word & 0x40u) == 0 && (packet->flags_word & 8u) != 0 &&
                                (packet->next_area == -1 || (packet->area->flags & 2u) != 0 ||
                                 Game.area_save[packet->next_area].complete != 0)) ||
                               (packet->flags_word & 0x100u) != 0 || packet->challenge_state != 0 ||
                               packet->mission_state != 0) {
                        if (status_prompt > 0 && ((pad->menu_directions & GAMEPAD_DUP) != 0 ||
                                                  (pad->menu_buttons_pressed & GAMEPAD_DUP) != 0)) {
                            GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                            status_prompt--;
                            moved_up = 1;
                        } else if (status_prompt < 1 && ((pad->menu_directions & GAMEPAD_DDOWN) != 0 ||
                                                         (pad->menu_buttons_pressed & GAMEPAD_DDOWN) != 0)) {
                            GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                            status_prompt++;
                            moved_down = 1;
                        }
                    }
                }
            }
            if (selected != 0) {
                if ((packet->flags_word & 8u) != 0 && packet->next_area != -1 && (packet->area->flags & 2u) == 0 &&
                    Game.area_save[packet->next_area].complete == 0)
                    packet->prompt_choice = 1;
                else
                    packet->prompt_choice = (u8)status_prompt;
                GameAudio_PlaySfx(0x30, NULL, 0, 0);
                stage->field_0x18 = 0.0f;
                stage->field_0x14 = 1;
                stage->field_0x1c = 0.5f;
            }
        }
    } else if (stage->field_0x14 == 1) {
        f32 accumulated = (f64)elapsed + (f64)stage->field_0x18;
        stage->field_0x18 = accumulated;
        if (stage->field_0x18 > stage->field_0x1c)
            NextStatusStage(packet);
    }
}

void __cdecl Prompt_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 active) {
    if (active != 0) {
        if (stage->field_0x18 < 0.5f) {
            i32 angle = (i32)(((f64)stage->field_0x18 + (f64)stage->field_0x18) * 16384.0);
            f32 alpha = NuTrigTable[(angle >> 1) & 0x7fff];
            if (stage->field_0x14 == 1)
                alpha = 1.0 - (f64)alpha;
            Status_DrawPromptMenu(packet, 0, alpha);
            StatusDrawPlayerIconsPc = 1;
        } else {
            Status_DrawPromptMenu(packet, 1, 1.0f);
            DrawPlayerIconPrompts(packet->player_active[0], TextIdPc880888, 1.0f, -1, -1, -1, TextIdPc880eac,
                                  packet->player_active[1], TextIdPc880888, 1.0f, -1, -1, -1, TextIdPc880eac);
        }
    }
}

void __cdecl CollectCharactersOff_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0:
            stage->field_0x14 = 1;
            stage->field_0x18 = 0.0f;
            stage->field_0x1c = 1.0f;
            break;
        case 1: {
            f32 accumulated = stage->field_0x18;
            accumulated += elapsed;
            stage->field_0x18 = accumulated;
            if (stage->field_0x18 >= stage->field_0x1c)
                NextStatusStage(packet);
            break;
        }
    }
}

void __cdecl BonusComplete_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    NextStatusStage(packet);
}

void __cdecl BonusComplete_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0: {
            stage->field_0x18 = 0.0f;
            stage->field_0x14 = 1;
            stage->field_0x1c = packet->newly_completed != 0 ? 4.0f : 0.5f;
            StatusDrawPlayerIconsPc = 1;
            break;
        }
        case 1: {
            SetDrawGoldBrick(packet, packet->current_gold_brick);
            {
                f32 previous = stage->field_0x18;
                f32 accumulated = stage->field_0x18 + elapsed;
                stage->field_0x18 = accumulated;
                if (stage->field_0x1c <= accumulated)
                    NextStatusStage(packet);
                else if (packet->newly_completed != 0 && previous < 0.5f && accumulated >= 0.5f) {
                    PlaySfx("StatusAward", NULL);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                }
            }
        }
    }
}
i32 StatusRedPc = 255;
i32 StatusGreenPc = 191;
i32 StatusBluePc = 0;
f32 StatusTitleYPc = 0.1f;
f32 StatusTextScalePc = 0.7f;

void __cdecl BonusComplete_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    if (current == 0)
        return;
    if (stage->field_0x14 > 0 && packet->newly_completed != 0) {
        f32 alpha;
        if (stage->field_0x18 < 0.5f)
            alpha = (f64)stage->field_0x18 * 2.0;
        else if (stage->field_0x18 < 3.5)
            alpha = 1.0f;
        else
            alpha = 1.0 - ((f64)stage->field_0x18 - 3.5) * 2.0;
        if (alpha > 0.0f) {
            i32 opacity = (i32)((f64)alpha * 128.0);
            SmartTextEx(TTab[TextIdPc880728], 0.0f, StatusTitleYPc, 1.0f, StatusTextScalePc, StatusTextScalePc,
                        StatusTextScalePc, 0, (u8)StatusRedPc, (u8)StatusGreenPc, (u8)StatusBluePc, 1.7f, 1, NULL, 0,
                        (u32)opacity);
        }
    }
    if (stage->field_0x14 == 0)
        iconalphaoverride = 0.0f;
    else if (packet->newly_completed == 0)
        iconalphaoverride = stage->field_0x18 / stage->field_0x1c;
    else {
        f32 fade_start = (f64)stage->field_0x1c - 0.5;
        if (stage->field_0x18 < fade_start)
            iconalphaoverride = 0.0f;
        else
            iconalphaoverride = ((f64)stage->field_0x18 - (f64)fade_start) * 2.0;
    }
}

void __cdecl BonusTime_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0: {
            stage->field_0x14 = 1;
            stage->field_0x18 = 0.0f;
            stage->field_0x1c = 4.0f;
            break;
        }
        case 1: {
            SetDrawGoldBrick(packet, packet->current_gold_brick);
            {
                f32 previous = stage->field_0x18;
                f32 accumulated = stage->field_0x18 + elapsed;
                stage->field_0x18 = accumulated;
                if (stage->field_0x1c <= accumulated)
                    NextStatusStage(packet);
                else if (previous < 0.5f && accumulated >= 0.5f) {
                    if (packet->new_best_time != 0.0f)
                        PlaySfx("StatusAward", NULL);
                    else
                        GameAudio_PlaySfx(0x32, NULL, 0, 0);
                    NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                }
            }
        }
    }
}

void __cdecl BonusTime_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    if (current != 0) {
        if (stage->field_0x14 > 0) {
            f32 alpha, time;
            if (stage->field_0x18 < 0.5f) {
                alpha = (f64)stage->field_0x18 + (f64)stage->field_0x18;
                time = 0.0f;
            } else {
                alpha = 1.0f;
                if (stage->field_0x18 < 3.0f)
                    time = 0.0f;
                else
                    time = (f64)stage->field_0x18 - 3.0;
            }
            {
                i32 opacity = (i32)((f64)alpha * 128.0);
                i32 angle = (i32)((f64)time * 32768.0 + 16384.0);
                f32 position = 1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5;
                DrawBonusTime(packet, position, opacity);
            }
        }
    } else if (stage->field_0x12 != 0) {
        i32 opacity = (i32)((f64)getFinishedStatusAlpha(packet) * 128.0);
        DrawBonusTime(packet, 1.0f, opacity);
    }
}

void __cdecl SuperStoryTime_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    *packet->score = packet->time_reward_score;
    NextStatusStage(packet);
}

void __cdecl SuperStoryTime_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0: {
            stage->field_0x18 = 0.0f;
            stage->field_0x14 = 1;
            stage->field_0x1c = 5.0f;
            packet->original_score = *packet->score;
            break;
        }
        case 1: {
            f32 previous = stage->field_0x18;
            f32 accumulated = stage->field_0x18 + elapsed;
            stage->field_0x18 = accumulated;
            if (stage->field_0x1c <= accumulated) {
                *packet->score = packet->time_reward_score;
                NextStatusStage(packet);
            } else if (previous < 0.5f && accumulated >= 0.5f) {
                if (packet->new_best_time != 0.0f)
                    PlaySfx("StatusAward", NULL);
                else
                    GameAudio_PlaySfx(0x32, NULL, 0, 0);
                NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
            }
            if (packet->new_best_time != 0.0f) {
                if (previous < 4.0f && stage->field_0x18 >= 4.0f) {
                    CoinTotalScale = 1.5f;
                    NewStatusRumbleBuzz(-1, 0.0f, 0.1f, 0);
                    PlaySfx("Shop_BuyCheat", NULL);
                }
                if (stage->field_0x18 >= 4.5f)
                    *packet->score = packet->time_reward_score;
                else if (stage->field_0x18 >= 0.5f && stage->field_0x18 < 4.0f) {
                    u32 original = packet->original_score;
                    f32 progress = ((f64)stage->field_0x18 - 0.5) / 3.5;
                    i32 angle = (i32)((f64)progress * 16384.0);
                    u32 difference = packet->time_reward_score - original;
                    i32 amount = (i32)((f32)difference * NuTrigTable[(angle >> 1) & 0x7fff]);
                    *packet->score = original;
                    IncreaseScore(packet->score, (u64)(i64)amount, 0);
                    {
                        i32 current_bucket = (i32)((f64)stage->field_0x18 / (f64)0.2f);
                        i32 previous_bucket = (i32)((f64)previous / (f64)0.2f);
                        if (previous_bucket != current_bucket)
                            PlaySfx("PickupCoin", NULL);
                    }
                }
            }
        }
    }
}

void __cdecl SuperStoryTime_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    char text[256], previous[256];
    f32 alpha, coin_progress;
    i32 opacity;
    char *title;
    i32 red, green;
    if (current == 0 || stage->field_0x14 <= 0)
        return;
    if (stage->field_0x18 < 0.5f) {
        alpha = (f64)stage->field_0x18 + (f64)stage->field_0x18;
        coin_progress = alpha;
    } else {
        coin_progress = 1.0f;
        if (stage->field_0x18 < 4.5f)
            alpha = 1.0f;
        else
            alpha = 1.0 - (((f64)stage->field_0x18 - 4.5) + ((f64)stage->field_0x18 - 4.5));
    }
    opacity = (i32)((f64)alpha * 128.0);
    {
        f32 coin_scale = CoinTotalScale;
        i32 angle = (i32)((f64)coin_progress * 16384.0);
        f32 coin_y = ((f64)STATSPOSY - (f64)STATSPOS2YPc) * (f64)NuTrigTable[(angle >> 1) & 0x7fff] + (f64)STATSPOS2YPc;
        CoinTotal_Draw((i32)*packet->score, coin_y, coin_scale, 1, 1.0f, 255, 191, 0);
    }
    if (packet->new_best_time != 0.0f) {
        title = TTab[TextIdPc88071c];
        red = 0;
        green = 255;
    } else {
        title = TTab[TextIdPc880734];
        red = 255;
        green = 0;
    }
    SmartTextEx(title, 0.0f, 0.2f, 1.0f, 0.7f, 0.7f, 0.7f, 0, (u8)red, (u8)green, 0, 1.7f, 1, NULL, 0, (u32)opacity);
    {
        f32 displayed_time;
        if (packet->new_best_time != 0.0f) {
            f32 blend;
            if (stage->field_0x18 < 0.5f)
                blend = 0.0f;
            else if (stage->field_0x18 < 4.0f)
                blend = ((f64)stage->field_0x18 - 0.5) / 3.5;
            else
                blend = 1.0f;
            displayed_time = ((f64)packet->new_best_time - (f64)packet->previous_best_time) * (f64)blend +
                             (f64)packet->previous_best_time;
        } else
            displayed_time = packet->superstory_time;
        Text_MakeTime(displayed_time, 1, 1, 1, text);
    }
    Text3DEx(text, 0.0f, 0.0f, 1.0f, 0.7f, 0.7f, 0.7f, 0, 255, 255, 255, opacity);
    Text_MakeTime(packet->previous_best_time, 1, 1, 1, text);
    NuStrCpy(previous, "(");
    NuStrCat(previous, text);
    NuStrCat(previous, ")");
    Text3DEx(previous, 0.0f, -0.2f, 1.0f, 0.7f, 0.7f, 0.7f, 0, 255, 255, 255, opacity / 2);
}

void __cdecl SuperStoryScore_LSW_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    *packet->score = packet->final_reward_score;
    NextStatusStage(packet);
}

void __cdecl SuperStoryScore_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0: {
            stage->field_0x18 = 0.0f;
            stage->field_0x14 = 1;
            stage->field_0x1c = 5.0f;
            packet->original_score = *packet->score;
            break;
        }
        case 1: {
            f32 previous = stage->field_0x18;
            f32 accumulated = stage->field_0x18 + elapsed;
            stage->field_0x18 = accumulated;
            if (stage->field_0x1c <= accumulated) {
                *packet->score = packet->final_reward_score;
                NextStatusStage(packet);
            } else if (previous < 0.5f && accumulated >= 0.5f) {
                if (packet->new_best_score != 0)
                    PlaySfx("StatusAward", NULL);
                else
                    GameAudio_PlaySfx(0x32, NULL, 0, 0);
                NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
            }
            if (packet->final_reward_score != packet->original_score) {
                if (previous < 4.0f && stage->field_0x18 >= 4.0f) {
                    CoinTotalScale = 1.5f;
                    NewStatusRumbleBuzz(-1, 0.0f, 0.1f, 0);
                    PlaySfx("Shop_BuyCheat", NULL);
                }
                if (stage->field_0x18 >= 4.0f)
                    *packet->score = packet->final_reward_score;
                else if (stage->field_0x18 >= 0.5f && stage->field_0x18 < 4.0f) {
                    u32 original = packet->original_score;
                    f32 progress = ((f64)stage->field_0x18 - 0.5) / 3.5;
                    i32 angle = (i32)((f64)progress * 16384.0 + 16384.0);
                    u32 difference = packet->final_reward_score - original;
                    f32 blend = 1.0 - (f64)NuTrigTable[(angle >> 1) & 0x7fff];
                    i32 amount = (i32)((f32)difference * blend);
                    *packet->score = original;
                    IncreaseScore(packet->score, (u64)(i64)amount, 0);
                    {
                        i32 current_bucket = (i32)((f64)stage->field_0x18 / (f64)0.2f);
                        i32 previous_bucket = (i32)((f64)previous / (f64)0.2f);
                        if (previous_bucket != current_bucket)
                            PlaySfx("PickupCoin", NULL);
                    }
                }
            }
        }
    }
}

void __cdecl SuperStoryScore_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    char text[256], previous[256];
    f32 coin_progress;
    if (current == 0)
        return;
    if (stage->field_0x14 > 0) {
        f32 alpha, blend;
        i32 opacity;
        char *title;
        u8 red, green;
        if (stage->field_0x18 < 0.5f) {
            alpha = (f64)stage->field_0x18 + (f64)stage->field_0x18;
            coin_progress = 1.0f;
        } else if (stage->field_0x18 < 4.5f) {
            alpha = coin_progress = 1.0f;
        } else {
            coin_progress = 1.0 - (((f64)stage->field_0x18 - 4.5) + ((f64)stage->field_0x18 - 4.5));
            alpha = coin_progress;
        }
        opacity = (i32)((f64)alpha * 128.0);
        if (packet->new_best_score != 0) {
            title = TTab[TextIdPc880720];
            red = 0;
            green = 255;
        } else {
            title = TTab[TextIdPc880738];
            red = 255;
            green = 0;
        }
        SmartTextEx(title, 0.0f, 0.2f, 1.0f, 0.7f, 0.7f, 0.7f, 0, red, green, 0, 1.7f, 1, NULL, 0, (u32)opacity);
        if (stage->field_0x18 < 0.5f)
            blend = 0.0f;
        else if (stage->field_0x18 < 4.0f)
            blend = ((f64)stage->field_0x18 - 0.5) / 3.5;
        else
            blend = 1.0f;
        Text_MakeScore((u32)(packet->superstory_score * blend), text);
        Text3DEx(text, 0.0f, 0.0f, 1.0f, 0.7f, 0.7f, 0.7f, 0, 255, 255, 255, opacity);
        Text_MakeScore(packet->previous_best_score, text);
        NuStrCpy(previous, "(");
        NuStrCat(previous, text);
        NuStrCat(previous, ")");
        Text3DEx(previous, 0.0f, -0.2f, 1.0f, 0.7f, 0.7f, 0.7f, 0, 255, 255, 255, opacity / 2);
    } else
        coin_progress = 1.0f;
    {
        f32 coin_scale = CoinTotalScale;
        i32 angle = (i32)((f64)coin_progress * 16384.0);
        f32 coin_y = ((f64)STATSPOSY - (f64)STATSPOS2YPc) * (f64)NuTrigTable[(angle >> 1) & 0x7fff] + (f64)STATSPOS2YPc;
        CoinTotal_Draw((i32)*packet->score, coin_y, coin_scale, 1, 1.0f, 255, 191, 0);
    }
}

void __cdecl BonusWin_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0: {
            stage->field_0x14 = 1;
            stage->field_0x18 = 0.0f;
            stage->field_0x1c = 4.0f;
            StatusDrawPlayerIconsPc = 1;
            break;
        }
        case 1: {
            f32 previous = stage->field_0x18;
            f32 accumulated = stage->field_0x18 + elapsed;
            stage->field_0x18 = accumulated;
            if (stage->field_0x1c <= accumulated) {
                NextStatusStage(packet);
            } else if (previous < 2.0f && accumulated >= 2.0f) {
                ADD_GAME_MESSAGE_PC message = AddGameMsg_Default;
                char text[256];
                NUVEC position;
                PlaySfx("TrueJedi_100pc", NULL);
                NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
                BonusScore[(packet->flags_word >> 5) & 1]++;
                sprintf(text, "%i", BonusScore[(packet->flags_word >> 5) & 1]);
                if ((packet->field_0xb0 & 0x20) == 0) {
                    position.x = -0.015f;
                    message.unknown_4f = 8;
                } else {
                    position.x = 0.015f;
                    message.unknown_4f = 2;
                }
                position.y = STATSPOSY;
                position.z = 1.0f;
                message.text = text;
                message.position = &position;
                message.target_position = &position;
                message.scale = 0.7f;
                message.target_scale = 2.1f;
                message.flags = 0x4028;
                message.duration = 1.0f;
                AddGameMsg(&message);
            } else if (accumulated < 2.0f && (f64)NuFmod(previous, 0.2f) < (f64)0.1f &&
                       (f64)NuFmod(stage->field_0x18, 0.2f) >= (f64)0.1f) {
                PlaySfx((char *)GameAudioName00758838Pc, NULL);
            }
        }
    }
}

void __cdecl BonusWin_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 current) {
    if (current != 0) {
        if (stage->field_0x14 == 0) {
            iconalphaoverride = 0.0f;
            return;
        }
        if (stage->field_0x18 >= 0.5f)
            iconalphaoverride = 1.0f;
        else
            iconalphaoverride = (f64)stage->field_0x18 + (f64)stage->field_0x18;
        if (stage->field_0x18 < 2.0f && (f64)NuFmod(stage->field_0x18, 0.2f) >= (f64)0.1f) {
            i32 second = (packet->flags_word >> 5) & 1;
            f32 winner_x = second == 0 ? -0.675f : 0.675f;
            i32 opacity = (i32)((f64)iconalphaoverride * 128.0);
            SmartTextEx(TTab[TextIdPc88075c], winner_x, STATSPOSY, 1.0f, 0.7f, 0.7f, 0.7f, second != 0 ? 8 : 2, 0, 255,
                        0, 0.35f, 1, NULL, 0, (u32)opacity);
        }
        DrawBonusScore(STATSPOSY, StatusPacket.player0_active, StatusPacket.player1_active, iconalphaoverride,
                       BonusScore);
    } else if (stage->field_0x12 != 0 && packet->stage->type != 25) {
        f32 alpha = getFinishedStatusAlpha(packet);
        DrawBonusScore(STATSPOSY, StatusPacket.player0_active, StatusPacket.player1_active, alpha, BonusScore);
    }
}

void __cdecl Exit_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    if (stage->field_0x14 >= 0)
        stage->field_0x18 = (f64)elapsed + (f64)stage->field_0x18;
    if (stage->field_0x18 >= stage->field_0x1c) {
        NextStatusStage(packet);
        if ((i32)packet->current_gold_brick < (i32)packet->stage_count - 1)
            stage->field_0x1c = 0.1f;
        else
            FinishStatusPacket(packet->prompt_choice);
    }
}

void __cdecl Save_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    if (packet->save_state == 3) {
        f32 accumulated = elapsed + stage->field_0x18;
        stage->field_0x18 = accumulated;
        if (stage->field_0x18 >= stage->field_0x1c) {
            u32 flags = packet->flags_word;
            if ((flags & 0x400) == 0 && packet->prompt_choice == 0 && (flags & 0x40) == 0 &&
                packet->mission_state == 0 && packet->challenge_state == 0)
                packet->flags_word = flags | 0x1000;
            NextStatusStage(packet);
        }
    } else if (packet->save_state == 2) {
        if (MenuInMemoryCard() == 0)
            packet->save_state = 3;
    } else if (packet->save_state == 0) {
        packet->save_state = 1;
        Text_FillInExtendedSaveInfo();
        if (TriggerAutoSave() == 0) {
            packet->save_state = 2;
            NewMenu(1000, -1, -1);
        }
    } else if (memcard_autosaveinprogress == 0)
        packet->save_state = 3;
}

void __cdecl Fade_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    switch (stage->field_0x14) {
        case 0:
            stage->field_0x14 = 1;
            stage->field_0x18 = 0.0f;
            stage->field_0x1c = 0.6f;
            break;
        case 1: {
            f32 accumulated = stage->field_0x18;
            accumulated += elapsed;
            stage->field_0x18 = accumulated;
            if (stage->field_0x18 >= stage->field_0x1c)
                FinishStatusPacket(packet->prompt_choice);
            {
                f32 numerator = stage->field_0x18;
                f32 denominator = stage->field_0x1c;
                f32 blend;
                if (numerator == 0.0f || denominator == 0.0f)
                    blend = 0.0f;
                else
                    blend = numerator / denominator;
                StatusIconYPc = -StatusIconsOnOff(blend);
            }
            break;
        }
    }
}
static STATUS_STAGE_s StatusStagesLswPc[34] = {
    {RedBrick_LSW_Draw, RedBrick_LSW_Update, RedBrick_LSW_Skip, 18, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {CollectCharcters_Draw, CollectCharcters_Update, CollectCharcters_Skip, 35, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {CollectCharactersOff_Draw, CollectCharactersOff_Update, CollectCharactersOff_Skip, 36, 0xffff, 0, 0, 0, 0.0f,
     0.0f},
    {Coins_LSW_Draw, Coins_LSW_Update, Coins_LSW_Skip, 3, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {TrueHero_LSW_Draw, TrueHero_LSW_Update, TrueHero_LSW_Skip, 1, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {TrueHero_LSW_Draw, TrueHero_LSW_Update, TrueHero_LSW_Skip, 2, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {MiniKit_LSW_Draw, MiniKit_LSW_Update, MiniKit_LSW_Skip, 4, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {AllMiniKits_LSW_Draw, AllMiniKits_LSW_Update, AllMiniKits_LSW_Skip, 17, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {GoldBrick_LSW_Draw, GoldBrick_LSW_Update, GoldBrick_LSW_Skip, 19, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 13, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 8, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 28, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 29, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 30, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 31, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 32, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 33, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 34, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 14, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 23, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {BonusTime_LSW_Draw, BonusTime_LSW_Update, BonusTime_LSW_Skip, 24, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {ChallangeCash_Draw, ChallangeCash_Update, ChallangeCash_Skip, 25, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {LevelComplete_LSW_Draw, LevelComplete_LSW_Update, LevelComplete_LSW_Skip, 26, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {BonusTime_LSW_Draw, BonusTime_LSW_Update, BonusTime_LSW_Skip, 27, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {SuperStoryTime_LSW_Draw, SuperStoryTime_LSW_Update, SuperStoryTime_LSW_Skip, 15, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {SuperStoryScore_LSW_Draw, SuperStoryScore_LSW_Update, SuperStoryScore_LSW_Skip, 16, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {BonusComplete_LSW_Draw, BonusComplete_LSW_Update, BonusComplete_LSW_Skip, 22, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {BonusWin_LSW_Draw, BonusWin_LSW_Update, BonusWin_LSW_Skip, 20, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {BonusTime_LSW_Draw, BonusTime_LSW_Update, BonusTime_LSW_Skip, 21, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {Prompt_LSW_Draw, Prompt_LSW_Update, NULL, 9, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {NULL, Exit_LSW_Update, NULL, 10, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {NULL, Save_LSW_Update, NULL, 11, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {NULL, Fade_LSW_Update, NULL, 12, 0xffff, 0, 0, 0, 0.0f, 0.0f},
    {NULL, NULL, NULL, -1, 0xffff, 0, 0, 0, 0.0f, 0.0f},
};
static struct STATUSPACKET_LSW_s StatusPacketLswPc = {0};

void __cdecl LSW_registerStatusScreen(void) {
    REGISTER_STATUS_PACKET_PC registration;
    memset(&registration, 0, sizeof(registration));
    registration.lsw_packet = &StatusPacketLswPc;
    registration.init_callback = InitStatusScreen_LSW;
    registration.reset_callback = ResetStatusPacket_LSW;
    registration.finish_callback = FinishStatusPacket_LSW;
    registration.draw_background_callback = DrawStatusBG_LSW;
    registration.stage_delay = 0.0f;
    RegisterStatusScreen(StatusStagesLswPc, NULL, &registration);
}
