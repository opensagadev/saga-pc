#include "saga.h"

extern const f32 PlayerPanelVariantZeroX, PlayerPanelVariantZeroZ, PlayerPanelVariantOneX, PlayerPanelVariantOneZ;

typedef struct panel_movement_pc_s {
    u8 unknown_00[0x50];
    NUVEC position;
    u16 y_rotation;
    u8 unknown_5e[3];
    u8 model_variant;
    u8 unknown_62[0xe];
    NUVEC target_offset;
} PANEL_MOVEMENT_PC;
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, position) == 0x50);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, y_rotation) == 0x5c);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, model_variant) == 0x61);
STATIC_ASSERT(offsetof(PANEL_MOVEMENT_PC, target_offset) == 0x70);

void __cdecl GizPanel_GetAbsTargetPos(void *context, NUVEC *target, i32 player_position) {
    PANEL_MOVEMENT_PC *panel = (PANEL_MOVEMENT_PC *)context;
    NUVEC offset;
    if (panel == NULL || target == NULL)
        return;
    if (player_position != 0 && panel->model_variant == 0) {
        offset.x = PlayerPanelVariantZeroX;
        offset.y = panel->target_offset.y;
        offset.z = PlayerPanelVariantZeroZ;
    } else if (player_position != 0 && panel->model_variant == 1) {
        offset.x = PlayerPanelVariantOneX;
        offset.y = panel->target_offset.y;
        offset.z = PlayerPanelVariantOneZ;
    } else {
        offset = panel->target_offset;
    }
    NuVecRotateY(&offset, &offset, panel->y_rotation);
    offset.x += panel->position.x;
    offset.z += panel->position.z;
    *target = offset;
}
