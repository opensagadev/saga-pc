#include "saga.h"

static void ZeroLightSetupPrivatePc(NUVEC *direction0, NUVEC *colour0, NUVEC *direction1, NUVEC *colour1,
                                    NUVEC *direction2, NUVEC *colour2) {
    PCGlobalLightingState.direction[0] = *direction0;
    PCGlobalLightingState.direction[1] = *direction1;
    PCGlobalLightingState.direction[2] = *direction2;
    PCGlobalLightingState.intensity[0] = *colour0;
    PCGlobalLightingState.intensity[1] = *colour1;
    PCGlobalLightingState.intensity[2] = *colour2;
    NuRndrSetDirectionalLightsPS(direction0, colour0, direction1, colour1, direction2, colour2);
}
void __cdecl SetZeroLights(void) {
    NUVEC black = {0, 0, 0};
    ZeroLightSetupPrivatePc(&LightingUnitDirectionPc, &black, &LightingUnitDirectionPc, &black,
                            &LightingUnitDirectionPc, &black);
    PCGlobalLightingState.ambient = APIObjectZeroMomentumPC;
    NuRndrSetAmbientLightPS(&APIObjectZeroMomentumPC);
}

void __cdecl FindAndSetLights(NUVEC *position, f32 scale, void *set) {
    RTL_DATA_PC lights;
    rtlResetEx(&lights, 1);
    rtlDynamicMasterEnable(0);
    rtlApplySetScale(set, &lights, position, NULL, -1, scale);
    rtlDynamicMasterEnable(1);
    rtlSetLights(&lights);
}
void __cdecl ResetLights(NUVEC *position, RTL_DATA_PC *packet, void *set) {
    rtlResetEx(packet, 1);
    if (position != NULL)
        rtlApplySetScale(set, packet, position, NULL, -1, 1.0f);
}
