#include "saga.h"

/* Original source name unknown. Retail links a second status-screen stage set at
   0x458470-0x45e362, between the LSW hint code and the LSW status stages. Its stage
   table (0x7f8280) is registered from the non-LSW branch of LoadPermData. The
   functions below compile to the same code as their LSW counterparts; the stage
   type in the comment comes from that table. */

f32 __cdecl StatusCoinsRatioAltPc(f32 numerator, f32 denominator) {
    if (numerator == 0.0f || denominator == 0.0f)
        return 0.0f;
    {
        f32 result = numerator / denominator;
        return result;
    }
}

void __cdecl ResetStatusPacket_Alt(STATUSPACKET_s *packet) {
    packet->lsw_packet->field_0x00 = 0;
}

/* Stage types 1 and 2. */
void __cdecl TrueHero_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    NextStatusStage(packet);
}

/* Stage type 4. */
void __cdecl MiniKit_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    if ((packet->field_0xb0 & 2) == 0) {
        StatusCurrentMinikitPc = (i32)((u32)StatusCurrentMinikitPc + (u32)StatusNewMinikitCountPc);
        if ((packet->field_0xb0 & 0x10) != 0 && (stage->field_0x14 != 6 || !(stage->field_0x18 >= stage->field_0x1c)))
            IncreaseScore(packet->score, 50000, 0);
    }
    NextStatusStage(packet);
}

/* Stage type 17. */
void __cdecl AllMiniKits_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    AddToCollection(VehicleBoltId7f195cPc);
    NextStatusStage(packet);
}

void __cdecl AllMiniKits_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    if (stage->field_0x14 == 0) {
        stage->field_0x14 = 1;
        stage->field_0x18 = 0.0f;
        stage->field_0x1c = 6.5f;
    } else if (stage->field_0x14 == 1) {
        f32 previous = stage->field_0x18;
        f32 accumulated = stage->field_0x18;
        accumulated += elapsed;
        stage->field_0x18 = accumulated;
        if (stage->field_0x18 >= stage->field_0x1c)
            NextStatusStage(packet);
        else if (previous < 0.5f && stage->field_0x18 >= 0.5f) {
            PlaySfx("StatusAward", NULL);
            NewStatusRumbleBuzz(-1, 0.6f, 0.0f, 0);
        } else if (previous < 4.0f && stage->field_0x18 >= 4.0f)
            PlaySfx("Char_Icon_App", NULL);
    }
}

/* Stage type 18. */
void __cdecl RedBrick_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    NextStatusStage(packet);
}

/* Stage type 19. */
void __cdecl GoldBrick_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    if (Game.gold_bricks <= 60)
        NextStatusStage(packet);
}

/* Stage type 9. */
void __cdecl Prompt_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    if (stage->field_0x14 == 0) {
        f32 accumulated = (f64)elapsed + (f64)stage->field_0x18;
        stage->field_0x18 = accumulated;
        if (stage->field_0x18 >= 0.5f) {
            i32 player = 0;
            i32 selected = 0;
            i32 moved_up = 0;
            i32 moved_down = 0;
            GAMEPAD_INIT_RECORD *pad = &GamePadStorage[0];
            do {
                if (selected != 0)
                    break;
                if (moved_up != 0 || moved_down != 0)
                    break;
                if (packet->player_active[player] != 0) {
                    u32 buttons = pad->menu_buttons_pressed;
                    if ((buttons & GAMEPAD_MENUSELECT) != 0) {
                        selected = 1;
                    } else if (((packet->flags_word & 0x40u) == 0 && (packet->flags_word & 8u) != 0 &&
                                (packet->next_area == -1 || (packet->area->flags & 2u) != 0 ||
                                 Game.area_save[packet->next_area].complete != 0)) ||
                               (packet->flags_word & 0x100u) != 0 || packet->challenge_state != 0 ||
                               packet->mission_state != 0) {
                        if (status_prompt > 0) {
                            if (((pad->menu_directions | buttons) & GAMEPAD_DUP) != 0) {
                                GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                                status_prompt = (i32)((u32)status_prompt - 1u);
                                moved_up = 1;
                            }
                        } else if (status_prompt < 1 && ((pad->menu_directions | buttons) & GAMEPAD_DDOWN) != 0) {
                            GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                            status_prompt = (i32)((u32)status_prompt + 1u);
                            moved_down = 1;
                        }
                    }
                }
                ++player;
                ++pad;
            } while (player < 2);
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

/* Stage type 5. */
void __cdecl Stage5_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    NextStatusStage(packet);
}

/* Stage types 8, 13, 14, 23 and 26. */
void __cdecl LevelComplete_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    if (stage->type == 26 && packet->mission_state == 2)
        *packet->score = packet->reward_score;
    if (stage->type == 23 && packet->challenge_state == 2)
        *packet->score = packet->reward_score;
    NextStatusStage(packet);
}

/* Stage type 22. */
void __cdecl BonusComplete_Alt_Skip(STATUS_STAGE_s *stage, STATUSPACKET_s *packet) {
    (void)stage;
    NextStatusStage(packet);
}

/* Stage type 10. */
void __cdecl Exit_Alt_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
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
