#include "saga.h"

void PC_SPECIAL_SHADOW_LIGHT::SetEnabledPc(i32 enabled) {
    if (enabled != 0 && this->enabled == 0) {
        i32 index;
        for (index = 0; index < render_set_count; ++index) {
            render_sets[index].shadow_lists_pc[0].material_last = render_sets[index].shadow_lists_pc[0].first;
            render_sets[index].shadow_lists_pc[1].material_last = render_sets[index].shadow_lists_pc[1].first;
        }
    }
    this->enabled = enabled;
}
