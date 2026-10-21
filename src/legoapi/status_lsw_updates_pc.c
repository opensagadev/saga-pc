#include "saga.h"

void __cdecl Prompt_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    if (stage->field_0x14 == 0) {
        volatile f32 accumulated = (f64)elapsed + (f64)stage->field_0x18;
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
        volatile f32 accumulated = (f64)elapsed + (f64)stage->field_0x18;
        stage->field_0x18 = accumulated;
        if (stage->field_0x18 > stage->field_0x1c)
            NextStatusStage(packet);
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

void __cdecl Fade_LSW_Update(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, f32 elapsed) {
    if (stage->field_0x14 == 0) {
        stage->field_0x14 = 1;
        stage->field_0x18 = 0.0f;
        stage->field_0x1c = 0.6f;
    } else if (stage->field_0x14 == 1) {
        volatile f32 accumulated = stage->field_0x18;
        accumulated += elapsed;
        stage->field_0x18 = accumulated;
        if (stage->field_0x18 >= stage->field_0x1c)
            FinishStatusPacket(packet->prompt_choice);
        {
            f32 numerator = stage->field_0x18;
            f32 denominator = stage->field_0x1c;
            f32 blend = 0.0f;
            if (numerator != 0.0f && denominator != 0.0f) {
                volatile f32 result = (f64)numerator / (f64)denominator;
                blend = result;
            }
            StatusIconYPc = -StatusIconsOnOff(blend);
        }
    }
}
