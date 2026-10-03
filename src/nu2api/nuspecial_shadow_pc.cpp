#include "saga.h"

extern "C" i32 NuSpecialClipTestShadowLights(NUVEC *minimum, NUVEC *maximum, i32 force) {
    if (force)
        PCSpecialShadowClipResultsPresent = 0;
    if (PCSpecialShadowClipResultsPresent) {
        for (i32 light = 0; light != PCSpecialActiveShadowLights; ++light) {
            if (static_cast<i32>(PCSpecialShadowClipResults[light]) > 0)
                return 1;
        }
        return 0;
    }
    i32 any = 0;
    for (i32 light = 0; light != PCSpecialActiveShadowLights; ++light) {
        NUVEC min_copy = *minimum;
        NUVEC max_copy = *maximum;
        i32 result =
            reinterpret_cast<PCFrustumClipSet *>(PCSpecialShadowLights[light])->TestBoxes(&min_copy, &max_copy);
        PCSpecialShadowClipResults[light] = static_cast<u32>(result);
        if (result > 0)
            any = 1;
    }
    PCSpecialShadowClipResultsPresent = 1;
    return any;
}
