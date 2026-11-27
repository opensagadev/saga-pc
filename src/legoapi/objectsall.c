#include "gameapi/apitext.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nustring.h"

void ObjectsallNuError(void) {
}

/* Do not split a "~~" pair, and keep a single '~' with the text after it. */
