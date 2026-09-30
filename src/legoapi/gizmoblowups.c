#include "saga.h"

i32 __cdecl GizmoBlowupCheckProximity(WORLDINFO *world, GAMEOBJECT *object) {
    i32 count = 0;
    i32 special;
    i32 destroy = 0;
    i32 index;
    i32 hit_type;
    GIZMOBLOWUP_PC *blowup;
    void *animation;
    f32 x, y, z;
    if (object == NULL || (object->flags_1fc & 0x80) == 0)
        return 0;
    special = (object->character_data->model_flags & 0x2000) != 0 || object->takeover_source != NULL;
    blowup = world->gizmo_blowups;
    for (index = 0; index < world->gizmo_blowup_count; ++index, ++blowup) {
        if ((blowup->status_flags & 0x804001) != 0x804000 || (special && (blowup->draw_flags & 0x200) == 0) ||
            (!special && (blowup->draw_flags & 8) == 0) || (blowup->status_flags & 0x2040000) != 0)
            continue;
        x = object->collision_position.x - blowup->mid_position.x;
        y = object->collision_position.y - blowup->mid_position.y;
        z = object->collision_position.z - blowup->mid_position.z;
        if (blowup->proximity_radius != 0.0f &&
            !(x * x + y * y + z * z < blowup->proximity_radius * blowup->proximity_radius))
            continue;
        if (special) {
            blowup->status_flags |= 0x10;
            hit_type = 5;
        } else {
            blowup->status_flags |= 0x20;
            hit_type = 4;
        }
        animation = NuSpecialGetInstAnim(&blowup->type->animated_special);
        if (blowup->proximity_delay > 0.0f) {
            blowup->activation_delay = blowup->proximity_delay;
            blowup->status_flags |= 0x40000;
        } else if (animation == NULL || (blowup->status_flags & 0x80000) != 0) {
            destroy = 1;
        } else {
            blowup->status_flags |= 0x2080000;
            blowup->animation_time = 0.0f;
        }
        if (destroy) {
            /* GizmoBlowupBlowup(blowup, 1, hit_type, 1, object, 1); return value ignored. */
            STUBBED_NAMED("GizmoBlowupBlowup");
            ++count;
        }
    }
    return count;
}
