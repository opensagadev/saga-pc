#include "saga.h"

/* Original source name unknown. Retail places this with the editor main code
   at 0x617e20, and SetShadowLight calls it rather than inlining the load, so
   the definition lives in its own unit. */

extern "C" NUCAMERA *EdMainCurrentCameraPc;

extern "C" NUCAMERA *__cdecl edmainGetCameraPc(void) {
    return EdMainCurrentCameraPc;
}
