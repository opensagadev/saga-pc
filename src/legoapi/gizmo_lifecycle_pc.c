#include "saga.h"

STATIC_ASSERT(sizeof(GIZSPINNER_s) == 0x304);
STATIC_ASSERT(offsetof(GIZSPINNER_s, anim_set) == 0x68);
STATIC_ASSERT(sizeof(GIZBOMBGEN_s) == 0x2c);
STATIC_ASSERT(offsetof(GIZBOMBGEN_s, anim_set) == 0x1c);
STATIC_ASSERT(sizeof(GIZBOMBGENSYS_s) == 0x14);
STATIC_ASSERT(offsetof(GIZBOMBGENSYS_s, capacity) == 6);
STATIC_ASSERT(offsetof(GIZBOMBGENSYS_s, anim_object_pool) == 8);
STATIC_ASSERT(sizeof(GIZTORPMACHINE_s) == 0x2c);
STATIC_ASSERT(sizeof(GIZTORPMACHINESYS_s) == 0xc);
STATIC_ASSERT(offsetof(GIZTORPMACHINESYS_s, machines) == 0);
STATIC_ASSERT(offsetof(GIZTORPMACHINESYS_s, scale) == 8);
STATIC_ASSERT(sizeof(GRAPPLE_DYNAMIC_PC) == 0xf8);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLDINFO, buffer_end) == 0x108);
STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
STATIC_ASSERT(offsetof(WORLDINFO, game_anim_sys) == 0x2960);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buildit_sys) == 0x46a8);
STATIC_ASSERT(offsetof(WORLDINFO, giz_force_sys) == 0x46ac);
STATIC_ASSERT(offsetof(WORLDINFO, spinners) == 0x46e0);
STATIC_ASSERT(offsetof(WORLDINFO, spinner_count) == 0x46e4);
STATIC_ASSERT(offsetof(WORLDINFO, spinner_anim_pool) == 0x46e8);
STATIC_ASSERT(offsetof(WORLDINFO, giz_panel_sys) == 0x5070);
STATIC_ASSERT(offsetof(WORLDINFO, grapples) == 0x5080);
STATIC_ASSERT(offsetof(WORLDINFO, grapple_count) == 0x5084);
STATIC_ASSERT(offsetof(WORLDINFO, torpedo_machine_sys) == 0x5100);
STATIC_ASSERT(offsetof(WORLDINFO, giz_bombgen_sys) == 0x5114);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_buildit_objects) == 0xe2);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_force_objects) == 0xe4);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_bombgen_objects) == 0xe6);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_levers) == 0xeb);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_grapples) == 0xee);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_buildits) == 0xf0);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_spinners) == 0xf2);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_gizpanels) == 0xfc);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_force) == 0xfe);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_torpmachines) == 0x104);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_spinneranim_objs) == 0x105);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_turrets) == 0x106);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_bombgens) == 0x107);

void GizmoPickups_PostLoad(void *world, void *system) {
    (void)world;
    (void)system;
}
