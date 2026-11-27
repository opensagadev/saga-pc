#include <stdio.h>
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/characters/motion/chris.h"
#include "legoapi/gizmo/base/gizmessage.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/levels/episodeI.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec4.h"
#include <math.h>

/* Episode I level callbacks installed by FixUpLevels. */

void RetakeG_Reset(WORLDINFO *world) {
}

void MaulA_Update(WORLDINFO *world) {
}

void MaulD_Init(WORLDINFO *world) {
}

void MaulD_Update(WORLDINFO *world) {
}

void MaulE_Init(WORLDINFO *world) {
}

void MaulE_Update(WORLDINFO *world) {
}

void MaulF_Update(WORLDINFO *world) {
}

void AnakinsFlightB_Update(WORLDINFO *world) {
}

void AnakinsFlightB_Draw(WORLDINFO *world) {
}

/* Empty level debug hook shared with the Episode II callbacks. */
void KaminoEGizmoMissing(char *name) {
}
