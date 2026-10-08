#include "saga.h"

STATIC_ASSERT(sizeof(CHARPIVOT_PC) == 0x14);
STATIC_ASSERT(offsetof(CHARPIVOT_PC, animation) == 4);
STATIC_ASSERT(offsetof(CHARPIVOT_PC, point_of_interest) == 6);
STATIC_ASSERT(offsetof(CHARPIVOT_PC, start_frame) == 8);
STATIC_ASSERT(offsetof(CHARPIVOT_PC, end_frame) == 0xc);
STATIC_ASSERT(offsetof(CHARPIVOT_PC, character_id) == 0x10);

static CHARPIVOT_PC *CharPivot;

void __cdecl CharPivot_Init(CHARPIVOT_PC *table) {
    CharPivot = table;
    if (table != NULL) {
        for (; table->name != NULL; ++table)
            table->character_id = (u16)CharIDFromName(table->name);
    }
}

void __cdecl CharPivot_Check(GAMEOBJECT *object, NUVEC *velocity) {
    CHARPIVOT_PC *pivot;
    CHARACTER_MODEL_LAYERS_PC *model;
    f32 *frame;
    i32 rotation;
    NUMTX *matrix;
    NUVEC point;
    NUVEC offset;
    NUVEC previous_point;

    if (object->field_0x276 == object->previous_movement_angle || CharPivot == NULL)
        return;
    for (pivot = CharPivot; pivot->name != NULL; ++pivot) {
        if (object->id != pivot->character_id)
            continue;
        if (pivot->animation != -1) {
            frame = AnimPlaying(&object->anim_packet, pivot->animation, 1, 1);
            if (frame == NULL || !(*frame >= pivot->start_frame) || !(*frame <= pivot->end_frame))
                continue;
        }
        if (pivot->point_of_interest == -1) {
            object->field_0x276 = object->previous_movement_angle;
            object->movement_facing_angle = object->previous_movement_angle;
            object->facing_angle = object->previous_movement_angle;
        } else {
            model = (CHARACTER_MODEL_LAYERS_PC *)object->character_model;
            if (model->points_of_interest[pivot->point_of_interest] != NULL) {
                rotation = -RotDiff(object->previous_movement_angle, object->field_0x276);
                matrix = &object->locator_matrices[pivot->point_of_interest];
                point.x = matrix->m30;
                point.y = matrix->m31;
                point.z = matrix->m32;
                NuVecSub(&offset, &point, &object->position);
                NuVecRotateY(&previous_point, &offset, rotation);
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
struct CHARPIVOT_PC CharPivot_LSW[32] = {
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
struct CHARPIVOT_PC CharPivot_Batman = {NULL, 0, 0, 0.0f, 0.0f, 0, {0, 0}};
