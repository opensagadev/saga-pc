#include "saga.h"
#include <string.h>
STATIC_ASSERT(offsetof(WORLDINFO, giz_flow) == 0x2acc);
STATIC_ASSERT(offsetof(WORLDINFO, ai_sys) == 0x2ae8);
STATIC_ASSERT(offsetof(FLOWBOX_PC, type) == 3);
STATIC_ASSERT(offsetof(FLOWBOX_PC, ai_trigger_group) == 4);
STATIC_ASSERT(offsetof(FLOWBOX_PC, data) == 0xc);
STATIC_ASSERT(sizeof(FLOWBOX_PC) == 0x20);
STATIC_ASSERT(offsetof(AITRIGGERSET_s, flags_word) == 0x210);
STATIC_ASSERT(sizeof(AITRIGGERSET_s) == 0x214);
i32 __cdecl AITriggerSetAddTrigger(AISYS_PC *, AITRIGGERSET_s *, GIZMO_PC *);
AITRIGGERSET_s *__cdecl AITriggerSetCreate(AITRIGGERSETSYS_s *system, FLOWBOX_PC *box) {
    i32 index;
    AITRIGGERSET_s *set;
    if (system != NULL) {
        for (index = 0; index < 32; ++index) {
            set = &system->sets[index];
            if (!(set->flags & 1)) {
                set->flags_word |= 1;
                set->flowbox = box;
                return set;
            }
        }
    }
    return NULL;
}
void __cdecl AITriggerSysAutoSetUp(WORLDINFO *world, AITRIGGERSETSYS_s *system) {
    AITRIGGERSET_s *groups[32] = {0};
    GIZFLOW_PC *flow;
    FLOWBOX_PC *box;
    FLOWBOXGIZMODATA_PC *data;
    AITRIGGERSET_s *set;
    i32 index, group_index, gizmo_index;
    if (world != NULL && world->giz_flow != NULL) {
        flow = world->giz_flow;
        box = flow->flowboxes;
        for (index = 0; index < flow->flowbox_count; ++index, ++box) {
            if (box->type == 0 && box->ai_trigger_group != 0 && box->ai_trigger_group <= 32) {
                group_index = box->ai_trigger_group - 1;
                data = (FLOWBOXGIZMODATA_PC *)box->data;
                set = groups[group_index];
                if (set == NULL) {
                    set = AITriggerSetCreate(system, NULL);
                    groups[group_index] = set;
                }
                if (set != NULL) {
                    for (gizmo_index = 0; gizmo_index < data->gizmo_count; ++gizmo_index)
                        AITriggerSetAddTrigger(world->ai_sys, set, (GIZMO_PC *)data->gizmos[gizmo_index]->gizmo);
                }
            }
        }
    }
}
