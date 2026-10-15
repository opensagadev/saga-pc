#include "saga.h"

void __cdecl ShadowMapEnablePc(struct PC_SPECIAL_SHADOW_LIGHT *light) {
    if (PCSpecialActiveShadowLights < 4 && light->enabled != 0 && light->used_on_specials == 0) {
        light->used_on_specials = 1;
        PCSpecialShadowLights[PCSpecialActiveShadowLights] = light;
        ++PCSpecialActiveShadowLights;
        PCSpecialShadowClipResultsPresent = 0;
    }
}
void __cdecl ShadowMapResetPc(void) {
    i32 i = 0;
    while (i != PCSpecialActiveShadowLights) {
        PCSpecialShadowLights[i]->used_on_specials = 0;
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

void PC_SPECIAL_SHADOW_LIGHT::SetEnabledPc(i32 enabled) {
    if (enabled != 0 && this->enabled == 0) {
        i32 index;
        for (index = 0; index < render_set_count; ++index) {
            render_sets[index].shadow_lists_pc[0].material_last = render_sets[index].shadow_lists_pc[0].first;
            render_sets[index].shadow_lists_pc[1].material_last = render_sets[index].shadow_lists_pc[1].first;
        }
    }
    this->enabled = enabled;
}
void PC_SPECIAL_SHADOW_LIGHT::SetParameterfPc(i32 parameter, f32 value) {
    switch (parameter) {
        case 0:
            parameter_0 = value;
            break;
        case 1:
            parameter_1 = value;
            break;
        case 2:
            parameter_2 = value;
            break;
        case 3:
            parameter_3 = value;
            break;
        case 6:
        case 7:
            render_sets[0].parameter_110 = value;
            break;
        case 8:
            render_sets[1].parameter_110 = value;
            break;
        case 9:
        case 10:
        case 11:
            render_sets[0].parameter_104 = value;
            break;
        case 15:
        case 16:
            render_sets[1].parameter_104 = value;
            break;
        case 12:
        case 13:
        case 14:
            render_sets[0].parameter_100 = value;
            break;
        case 17:
        case 18:
            render_sets[1].parameter_100 = value;
            break;
        case 19:
            parameter_19 = value;
            break;
        case 20:
            parameter_20 = value;
            break;
    }
}

// Append to the current shadowmap.cpp owner; real existing definitions above.
void __cdecl ResetShadowMapRenderingFn(void) {
    ResetShadowMapRendering();
}
void __cdecl EnableShadowMapRenderingFn(void) {
    EnableShadowMapRendering(0);
}

extern "C" i32 __cdecl ShadowSetClipStatePc(i32 enabled, i32 state) {
    i32 previous = PCSpecialClipState;
    PCSpecialClipState = enabled == 0 ? -1 : state;
    return previous;
}
extern "C" void __cdecl ShadowResetClipResultsPc(void) {
    PCSpecialShadowClipResultsPresent = 0;
}
extern "C" void __cdecl RenderShadowLights(i32 paused) {
    if (PcWindowOptions.shadowsenabled != 0 && PcWindowOptions.shadowmethod == 2 && PCShadowCurrentLight != NULL &&
        PCShadowCurrentLight->enabled != 0) {
        i32 previous = ShadowSetClipStatePc(1, 1);
        ShadowMapResetPc();
        ShadowMapEnablePc(PCShadowCurrentLight);
        ShadowResetClipResultsPc();
        NuRndrBeginScene(1);
        currentScene.dyn_light_count = 1;
        currentScene.dyn_light_ptr = PCShadowCurrentLight;
        currentScene.unknown_40 = 0;
        if (WORLD->current_gscn != NULL)
            PCShadowCurrentLight->RenderSceneShadowPc(WORLD->current_gscn);
        ShadowSetClipStatePc(0, previous);
        NuRndrEndScene();
        ShadowMapResetPc();
    }
}
