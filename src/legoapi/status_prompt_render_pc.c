#include "saga.h"

void __cdecl Status_DrawPromptMenu(STATUSPACKET_s *packet, i32 selected, f32 alpha);

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
