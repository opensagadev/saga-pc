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

/* Empty level debug hook shared with the Episode II callbacks. */
void KaminoEGizmoMissing(char *name) {
}
