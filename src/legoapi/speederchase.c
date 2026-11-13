#include "saga.h"

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, opponent) == 0x3ac);
STATIC_ASSERT(offsetof(APIOBJECT_PC, owner) == 0);
STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, model_data_b) == 0xc);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_107e) == 0x107e);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7b3) == 0x7b3);
STATIC_ASSERT(offsetof(GAMEOBJECT, rider) == 0xcd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, bounds_radius) == 0x1dc);

i32 PodRaceKeyPc[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
void __cdecl PodDust(WORLDINFO *world, GAMEOBJECT *object) {
    NUVEC positions[2], previous[2], delta, position;
    NUMTX matrix;
    f32 rate, fraction;
    i32 count, i, locator, key, particles;
    if (object->floor_height == 2000000.0f)
        return;
    if (world->game_debris_sys->entries[25].effect == -1)
        return;
    count = 0;
    for (i = 0; i < 2; ++i) {
        locator = PodDustLocatorIndicesPc[i];
        if (object->origin_joint_ready == 0 || object->character_model->points_of_interest[locator] == NULL)
            continue;
        positions[count].x = object->locator_matrices[locator].m30;
        positions[count].y = (f32)((f64)object->floor_height + (f64).1f);
        positions[count].z = object->locator_matrices[locator].m32;
        ++count;
        if (!(object->flags_1fc_word & 0x80))
            continue;
        if (object != (GAMEOBJECT *)Player[0] && object != (GAMEOBJECT *)Player[1])
            continue;
        key = object == (GAMEOBJECT *)Player[0] ? i : i + 4;
        matrix.m00 = object->locator_matrices[locator].m00;
        matrix.m01 = object->locator_matrices[locator].m01;
        matrix.m02 = object->locator_matrices[locator].m02;
        matrix.m03 = object->locator_matrices[locator].m03;
        matrix.m10 = object->locator_matrices[locator].m20;
        matrix.m11 = object->locator_matrices[locator].m21;
        matrix.m12 = object->locator_matrices[locator].m22;
        matrix.m13 = object->locator_matrices[locator].m23;
        matrix.m20 = -object->locator_matrices[locator].m10;
        matrix.m21 = -object->locator_matrices[locator].m11;
        matrix.m22 = -object->locator_matrices[locator].m12;
        matrix.m23 = -object->locator_matrices[locator].m13;
        matrix.m30 = object->locator_matrices[locator].m30;
        matrix.m31 = object->locator_matrices[locator].m31;
        matrix.m32 = object->locator_matrices[locator].m32;
        matrix.m33 = object->locator_matrices[locator].m33;
        if (PodRaceKeyPc[key] == -1)
            AddDebrisEffect(&PodRaceKeyPc[key], world->game_debris_sys->entries[53].effect, 0, 0, 0);
        else
            DebrisPosOrientationMtx(PodRaceKeyPc[key], &matrix);
        if (PodRaceKeyPc[key + 2] == -1)
            AddDebrisEffect(&PodRaceKeyPc[key + 2], world->game_debris_sys->entries[54].effect, 0, 0, 0);
        else {
            matrix.m00 = -NuTrigTable[((object->field_0x276 + 0x4000) >> 1) & 0x7fff];
            matrix.m01 = 0;
            matrix.m02 = NuTrigTable[object->field_0x276 >> 1];
            matrix.m10 = -matrix.m02;
            matrix.m11 = 0;
            matrix.m12 = matrix.m00;
            matrix.m20 = 0;
            matrix.m21 = 1;
            matrix.m22 = 0;
            matrix.m31 = (f32)((f64)object->floor_height + (f64).1f);
            DebrisPosOrientationMtx(PodRaceKeyPc[key + 2], &matrix);
        }
    }
    if (count == 0) {
        positions[0].x = object->collision_position.x;
        positions[0].y = object->floor_height;
        positions[0].z = object->collision_position.z;
        count = 1;
    }
    NuVecSub(&delta, &object->start_position, &object->position);
    rate = (f32)(((f64)PodDustEmissionRatePc * (f64)avg_currentspeed_mul) *
                 (1.0 - (((f64)object->collision_min.y - (f64)object->floor_height) / (f64)PodDustHeightScalePc)));
    for (i = 0; i < count; ++i) {
        NuVecAdd(&previous[i], &positions[i], &delta);
        particles = ParticlesPerSecond(rate, FRAMETIME);
        for (; particles > 0; --particles) {
            fraction = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f));
            position.x = (f32)((((f64)positions[i].x - (f64)previous[i].x) * (f64)fraction) + (f64)previous[i].x);
            position.y = (f32)((((f64)positions[i].y - (f64)previous[i].y) * (f64)fraction) + (f64)previous[i].y);
            position.z = (f32)((((f64)positions[i].z - (f64)previous[i].z) * (f64)fraction) + (f64)previous[i].z);
            AddVariableShotDebrisEffect(world->game_debris_sys->entries[25].effect, &position, 1, 0, 0);
        }
    }
}

STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_dc8) == 0xdc8);
