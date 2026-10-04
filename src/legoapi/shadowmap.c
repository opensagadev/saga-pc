#include "saga.h"

typedef struct PC_SHADOW_ELIGIBILITY_VIEW {
    u8 unknown_0[0x808];
    i32 eligible;
    u8 unknown_80c[0x18];
    i32 listed;
} PC_SHADOW_ELIGIBILITY_VIEW;
STATIC_ASSERT(offsetof(PC_SHADOW_ELIGIBILITY_VIEW, eligible) == 0x808);
STATIC_ASSERT(offsetof(PC_SHADOW_ELIGIBILITY_VIEW, listed) == 0x824);
void __cdecl ShadowMapEnablePc(struct PC_SPECIAL_SHADOW_LIGHT *light) {
    PC_SHADOW_ELIGIBILITY_VIEW *view = (PC_SHADOW_ELIGIBILITY_VIEW *)light;
    if (PCSpecialActiveShadowLights < 4 && view->eligible != 0 && view->listed == 0) {
        view->listed = 1;
        PCSpecialShadowLights[PCSpecialActiveShadowLights] = light;
        ++PCSpecialActiveShadowLights;
        PCSpecialShadowClipResultsPresent = 0;
    }
}
void __cdecl ShadowMapResetPc(void) {
    i32 i = 0;
    while (i != PCSpecialActiveShadowLights) {
        ((PC_SHADOW_ELIGIBILITY_VIEW *)PCSpecialShadowLights[i])->listed = 0;
        ++i;
    }
    PCSpecialActiveShadowLights = 0;
    PCSpecialShadowClipResultsPresent = 0;
}
void __cdecl EnableShadowMapRendering(i32 unused) {
    if (PCShadowDrawEnabled != 0 && PCShadowDrawMode == 2)
        ShadowMapEnablePc(PCShadowCurrentLight);
}
void __cdecl ResetShadowMapRendering(void) {
    ShadowMapResetPc();
}
