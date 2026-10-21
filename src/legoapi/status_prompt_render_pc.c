#include "saga.h"

extern i16 TextIdPc8806f8, TextIdPc880704, TextIdPc880744, TextIdPc88078c, TextIdPc880790, TextIdPc88083c;
f32 StatusPromptSingleYPc = 0.125f;
f32 StatusPromptHintYPc = 0.39f;
f32 StatusPromptStepYPc = -0.2f;

void __cdecl Status_DrawPromptMenu(STATUSPACKET_s *packet, i32 selected, f32 alpha) {
    AREA_DATA_PC *areas = ADataList;
    i32 labels[2];
    i32 locked = 0;
    i32 count;
    i32 row;
    volatile f32 title;
    volatile f32 step;
    volatile f32 y;
    labels[1] = -1;
    if ((packet->flags_word & 0x100u) != 0 || packet->challenge_state != 0 || packet->mission_state != 0) {
        labels[0] = TextIdPc88083c;
        labels[1] = TextIdPc8806f8;
    } else if ((packet->flags_word & 0x40u) != 0 ||
               (packet->area_id != -1 && (u8)areas[packet->area_id].episode_index == 0xff)) {
        labels[0] = TextIdPc8806f8;
    } else if ((packet->flags_word & 8u) != 0 && packet->next_area != -1 &&
               (areas[packet->next_area].flags & 2u) == 0 && Game.area_save[packet->next_area].complete == 0) {
        labels[0] = TextIdPc8806f8;
        locked = 1;
    } else {
        if (packet->next_area != -1 && (areas[packet->next_area].flags & 2u) != 0)
            labels[0] = TextIdPc880744;
        else
            labels[0] = TextIdPc880704;
        if ((packet->flags_word & 8u) != 0)
            labels[1] = TextIdPc8806f8;
    }
    title = StatusCoinsTitleYPc;
    if (StartupGameFlag1 != 0)
        step = StatusPromptStepYPc;
    else
        step = -((f64)title + (f64)title);
    if (labels[1] == -1) {
        i16 label;
        count = 1;
        y = StartupGameFlag1 != 0 ? StatusPromptSingleYPc : title;
        if (locked != 0) {
            label = areas[packet->next_area].area_index == 5 ? TextIdPc88078c : TextIdPc880790;
            SmartTextEx(TTab[label], 0.0f, 0.325f, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, 63, 0, 1.7f, 1, NULL, 0,
                        (i32)((f64)alpha * 128.0));
        }
    } else {
        count = 2;
        y = 0.0f;
        if (StartupGameFlag1 != 0 && packet->newly_completed != 0 && packet->area != NULL &&
            packet->area == NEGOTIATIONS_ADATA) {
            Hint_SetHintFromId(0x26c, 0, 1);
            Hint_SetComplete(0x26c);
            y = StatusPromptHintYPc;
            Hint_Draw(-1);
        }
        y = (f64)y - (f64)step * 0.5;
    }
    if ((packet->flags_word & 0x100u) != 0)
        y = (f64)y + (f64)0.075f;
    for (row = 0; row < count; ++row) {
        i32 red;
        i32 green;
        i32 blue;
        if (selected != 0 && status_prompt == row) {
            if (menu_flash != 0) {
                red = MENUFLASH0R;
                green = MENUFLASH0G;
                blue = MENUFLASH0B;
            } else {
                red = MENUFLASH1R;
                green = MENUFLASH1G;
                blue = MENUFLASH1B;
            }
            if (menu_pulsate > 0.0f) {
                f64 inverse = 1.0 - (f64)menu_pulsate;
                red = (i32)((f64)(u32)MENUFLASH0R * (f64)menu_pulsate + (f64)(u32)MENUFLASH1R * inverse);
                green = (i32)((f64)(u32)MENUFLASH0G * (f64)menu_pulsate + (f64)(u32)MENUFLASH1G * inverse);
                blue = (i32)((f64)(u32)MENUFLASH0B * (f64)menu_pulsate + (f64)(u32)MENUFLASH1B * inverse);
            }
        } else {
            red = MENUENTRYR;
            green = MENUENTRYG;
            blue = MENUENTRYB;
        }
        SmartTextEx(TTab[labels[row]], 0.0f, y, 1.0f, 0.7f, 0.7f, 0.7f, 0, red, green, blue, 1.7f, 1, NULL, 0,
                    (u8)(i32)((f64)alpha * 128.0));
        y = (f64)step + (f64)y;
    }
}

void __cdecl Prompt_LSW_Draw(STATUS_STAGE_s *stage, STATUSPACKET_s *packet, i32 active) {
    if (active != 0) {
        if (stage->field_0x18 < 0.5f) {
            i32 angle = (i32)(((f64)stage->field_0x18 + (f64)stage->field_0x18) * 16384.0);
            volatile f32 alpha = NuTrigTable[(angle >> 1) & 0x7fff];
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
