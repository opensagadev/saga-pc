#include "saga.h"
void __cdecl QuatInterpolateRotationMatrix(NUMTX *out, NUMTX *left, NUMTX *right, f32 fraction) {
    NUQUAT a, b, value;
    NuMtxToQuat(right, &b);
    NuMtxToQuat(left, &a);
    NuQuatSlerp(&value, &a, &b, fraction);
    NuQuatToMtx(&value, out);
}

STATIC_ASSERT(sizeof(GAMEOBJECT) == 0x10d8);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, game_character) == 0x24);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
STATIC_ASSERT(offsetof(GAMEOBJECT, transform_alpha) == 0xd80);
STATIC_ASSERT(offsetof(GAMEOBJECT, transform_target_scale) == 0xd8c);
STATIC_ASSERT(offsetof(GAMEOBJECT, transform_target) == 0xd14);

GAMEOBJECT *__cdecl Transform_TargettedByObj(void *target) {
    GAMEOBJECT *object = Obj;
    i32 count = HIGHGAMEOBJECT;
    i32 index;
    for (index = 0; index < count; ++index, ++object) {
        if ((object->flags_1fc_word & 0x1001) == 0x1001 && object->dead == 0 &&
            (object->character_data->game_character->flags_090 & 0x01000000) != 0 && object->transform_alpha > 0.0f &&
            object->transform_target_scale > 0.0f && object->transform_target == target)
            return object;
    }
    return NULL;
}
