#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/ai/core/gameai.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/items/base/apiobject.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>
#include <stdio.h>

/* Native PC startup and callback providers; insert in gameai.c before GameAISysSetGame. */

i32 StarWars_PreparingForSpecialMove(AIPACKET *, APIOBJECT *, i32);
i32 StarWars_MidSpecialMove(AISYS *, AIPACKET *, APIOBJECT *);

AI_CHARACTER_NAME LevelCharacterNameFn = 0;
AI_CHARACTER_NAME SpecialRouteCharacterNameFn = 0;
AI_LEVEL_CHARACTER_GLOBAL_ID LevelCharacterGlobalIDFn = 0;
i32 (*GlobalCharacterTypeIDFn)(char *) = NULL;
AI_GLOBAL_CHARACTER_NAME GlobalCharacterNameFn = 0;
AI_GLOBAL_CHARACTER_HGOBJ GlobalCharacterHGobjFn = 0;
AI_GLOBAL_CHARACTER_RENDER GlobalCharacterRenderFn = 0;
AI_CLEAR_CREATURES ClearAICreaturesFn = 0;

i32 NuHGobjRndr(NUHGOBJ_EVAL *object, NUMTX *matrix, i32 count, i16 *layers) {
    __declspec(align(16)) NUMTX matrices[256];
    NuHGobjEval(object, 0, NULL, matrices);
    return NuHGobjRndrMtxDwa(object, matrix, count, layers, matrices, NULL, 0);
}
