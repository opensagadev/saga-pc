#include "saga.h"
i32 __cdecl TakeOverYodaSeekDistanceHack(GAMEOBJECT *object, GAMEOBJECT *luke, NUVEC *offset) {
    if ((object->flags_ef4 & 2) == 0 || dagobah_training == 0 || object->rider != NULL || luke == NULL ||
        (luke->flags_1fc_word & 0x80) == 0) {
        return 0;
    }
    if (!((f64)NuVecDistSqr(&object->position, &luke->position, offset) < (f64)(0.7f * 0.7f))) {
        return 0;
    }
    NuVecRotateY(offset, offset, -(i32)luke->field_0x276);
    return offset->z > 0.0f;
}

void __cdecl CharScene_Draw(WORLDINFO *world, i32 character_id, NUMTX *matrix, NUMTX *reflection_matrix) {
    NUSPECIALHANDLE *special = CharScene_FindHSpecial(world, character_id);
    if (special != NULL) {
        if (matrix != NULL)
            NuSpecialDrawAt(special, matrix);
        if (reflection_matrix != NULL)
            NuSpecialDrawAt(special, reflection_matrix);
    }
}
