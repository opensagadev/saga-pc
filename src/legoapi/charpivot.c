#include "legoapi/characters/core/character.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/charpivot.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

static CHARPIVOT *CharPivot = 0;

void CharPivot_Init(CHARPIVOT *table) {
    CharPivot = table;
    if (table != NULL) {
        for (; table->name != NULL; ++table) {
            table->character_id = (u16)CharIDFromName(table->name);
        }
    }
}

void CharPivot_Check(GAMEOBJECT *object, NUVEC *velocity) {
    CHARPIVOT *pivot;
    CHARACTER_MODEL_LAYERS *model;
    f32 *frame;
    i32 rotation;
    NUVEC point;
    NUVEC offset;
    NUVEC previous_point;

    if (object->unknown_276 == object->previous_movement_angle || CharPivot == NULL) {
        return;
    }
    for (pivot = CharPivot; pivot->name != NULL; ++pivot) {
        if (object->id != pivot->character_id) {
            continue;
        }
        if (pivot->animation != -1) {
            frame = AnimPlaying(&object->anim_packet, pivot->animation, 1, 1);
            if (frame == NULL || !(*frame >= pivot->start_frame) || !(*frame <= pivot->end_frame)) {
                continue;
            }
        }
        if (pivot->point_of_interest == -1) {
            object->unknown_276 = object->previous_movement_angle;
            object->movement_facing_angle = object->previous_movement_angle;
            object->facing_angle = object->previous_movement_angle;
        } else {
            model = object->character_model;
            if (model->points_of_interest[pivot->point_of_interest] != NULL) {
                rotation = RotDiff(object->previous_movement_angle, object->unknown_276);
                point = *(NUVEC *)&object->player.locator_matrices[pivot->point_of_interest].m30;
                NuVecSub(&offset, &point, &object->position);
                NuVecRotateY(&previous_point, &offset, -rotation);
                NuVecAdd(&previous_point, &previous_point, &object->start_position);
                NuVecSub(&offset, &point, &previous_point);
                NuVecScale(&offset, &offset, 1.0f / FRAMETIME);
                velocity->x -= offset.x;
                velocity->z -= offset.z;
            }
        }
        return;
    }
}

// Proposed original CharPivot owner definitions; true selected sentinel domain.
struct CHARPIVOT CharPivot_LSW[32] = {
    {"atst", 0, -1, 7.0f, 11.0f, 0, {0, 0}},         {"atst", 0, -1, 21.0f, 25.0f, 0, {0, 0}},
    {"atst", 0, 2, 8.0f, 25.0f, 0, {0, 0}},          {"atst", 0, 3, 0.0f, 11.0f, 0, {0, 0}},
    {"atst", 0, 3, 22.0f, 999.0f, 0, {0, 0}},        {"atst_lowres", 0, -1, 7.0f, 11.0f, 0, {0, 0}},
    {"atst_lowres", 0, -1, 21.0f, 25.0f, 0, {0, 0}}, {"atst_lowres", 0, 2, 8.0f, 25.0f, 0, {0, 0}},
    {"atst_lowres", 0, 3, 0.0f, 11.0f, 0, {0, 0}},   {"atst_lowres", 0, 3, 22.0f, 999.0f, 0, {0, 0}},
    {"mini_atst", 0, 2, 15.0f, 999.0f, 0, {0, 0}},   {"mini_atst", 0, 3, 0.0f, 15.0f, 0, {0, 0}},
    {"rancor", 0, -1, 0.0f, 3.0f, 0, {0, 0}},        {"rancor", 0, -1, 19.0f, 24.0f, 0, {0, 0}},
    {"rancor", 0, -1, 40.0f, 999.0f, 0, {0, 0}},     {"rancor", 64, -1, 0.0f, 3.0f, 0, {0, 0}},
    {"rancor", 64, -1, 19.0f, 24.0f, 0, {0, 0}},     {"rancor", 64, -1, 40.0f, 999.0f, 0, {0, 0}},
    {"rancor", 0, 0, 0.0f, 3.0f, 0, {0, 0}},         {"rancor", 0, 0, 20.0f, 999.0f, 0, {0, 0}},
    {"rancor", 0, 1, 0.0f, 24.0f, 0, {0, 0}},        {"rancor", 0, 1, 0.0f, 999.0f, 0, {0, 0}},
    {"rancor", 64, 0, 0.0f, 3.0f, 0, {0, 0}},        {"rancor", 64, 0, 20.0f, 999.0f, 0, {0, 0}},
    {"rancor", 64, 1, 0.0f, 24.0f, 0, {0, 0}},       {"rancor", 64, 1, 41.0f, 999.0f, 0, {0, 0}},
    {"clonewalker", 0, -1, 1.0f, 4.0f, 0, {0, 0}},   {"clonewalker", 0, -1, 15.0f, 18.0f, 0, {0, 0}},
    {"clonewalker", 0, 0, 2.0f, 18.0f, 0, {0, 0}},   {"clonewalker", 0, 1, 16.0f, 999.0f, 0, {0, 0}},
    {"clonewalker", 0, 1, 0.0f, 4.0f, 0, {0, 0}},    {NULL, 0, 0, 0.0f, 0.0f, 0, {0, 0}},
};
struct CHARPIVOT CharPivot_Batman = {NULL, 0, 0, 0.0f, 0.0f, 0, {0, 0}};
