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
    if (PcWindowOptions.shadowsenabled != 0 && PcWindowOptions.shadowmethod == 2)
        ShadowMapEnablePc(PCShadowCurrentLight);
}
void __cdecl ResetShadowMapRendering(void) {
    ShadowMapResetPc();
}

typedef struct SHADOW_ENABLE_SLOT_PC {
    u8 unknown_000[0x2b4];
    u32 field_2b4;
    u32 field_2b8;
    u8 unknown_2bc[0x3c];
    u32 field_2f8;
    u32 field_2fc;
    u8 unknown_300[0x80];
} SHADOW_ENABLE_SLOT_PC;
typedef struct SHADOW_ENABLE_STATE_PC {
    SHADOW_ENABLE_SLOT_PC slots[2];
    u8 unknown_700[0xc];
    i32 slot_count;
    u8 unknown_710[0xf8];
    i32 enabled;
} SHADOW_ENABLE_STATE_PC;
STATIC_ASSERT(sizeof(SHADOW_ENABLE_SLOT_PC) == 0x380);
STATIC_ASSERT(offsetof(SHADOW_ENABLE_SLOT_PC, field_2b4) == 0x2b4);
STATIC_ASSERT(offsetof(SHADOW_ENABLE_SLOT_PC, field_2b8) == 0x2b8);
STATIC_ASSERT(offsetof(SHADOW_ENABLE_SLOT_PC, field_2f8) == 0x2f8);
STATIC_ASSERT(offsetof(SHADOW_ENABLE_SLOT_PC, field_2fc) == 0x2fc);
STATIC_ASSERT(offsetof(SHADOW_ENABLE_STATE_PC, slot_count) == 0x70c);
STATIC_ASSERT(offsetof(SHADOW_ENABLE_STATE_PC, enabled) == 0x808);
void PC_SPECIAL_SHADOW_LIGHT::SetEnabledPc(i32 enabled) {
    SHADOW_ENABLE_STATE_PC *state = (SHADOW_ENABLE_STATE_PC *)this;
    if (enabled != 0 && state->enabled == 0) {
        i32 index;
        for (index = 0; index < state->slot_count; ++index) {
            state->slots[index].field_2b8 = state->slots[index].field_2b4;
            state->slots[index].field_2fc = state->slots[index].field_2f8;
        }
    }
    state->enabled = enabled;
}

// Append to the current shadowmap.cpp owner; real existing definitions above.
void __cdecl ResetShadowMapRenderingFn(void) {
    ResetShadowMapRendering();
}
void __cdecl EnableShadowMapRenderingFn(void) {
    EnableShadowMapRendering(0);
}
