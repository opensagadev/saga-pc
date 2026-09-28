#include "saga.h"

NURNDRLIBSCENE currentScene;
i32 sceneParametersCount;
i32 nurndr_back_w;
i32 nurndr_back_h;

i32 __cdecl NuRndrBeginScene(i32 begin_flags) {
    currentScene.clear_alpha_4c = 0.0f;
    currentScene.unknown_24 = 0;
    currentScene.unknown_28 = 0;
    currentScene.unknown_58 = 0;
    currentScene.dyn_light_count = 0;
    currentScene.dyn_light_ptr = NULL;
    currentScene.unknown_48 = 0;
    currentScene.unknown_ac = 0;
    currentScene.unknown_c4 = 0;
    currentScene.unknown_e4 = 0;
    currentScene.unknown_e0 = 0;
    currentScene.unknown_178 = 0;
    currentScene.unknown_174 = 0;
    currentScene.unknown_188 = 0;
    currentScene.unknown_214 = 0;
    currentScene.scene_id = -1;
    currentScene.unknown_40 = -1;
    return 1;
}

void __cdecl NuRndrClear(i32 clear_flags, i32 bg_colour, f32 alpha) {
    currentScene.clear_alpha = alpha;
    currentScene.clear_flags_08 |= clear_flags;
    STUBBED_NAMED("NuRndrClear: NuVpGetPosition2/NuVpGetSize2 into scene +0x14/+0x18");
    currentScene.bg_colour = bg_colour;
    currentScene.calc_1c = (f32)((f64)nurndr_back_w * 0.0625 / ((f64)DebrisGlassTextureWidth / PS2_VREZ_W));
    currentScene.calc_20 = (f32)((f64)nurndr_back_w * nurndr_back_h / ((f64)DebrisGlassTextureHeight / PS2_VREZ_H));
}

void __cdecl NuRndrGradClear(i32 clear_flags, i32 top_colour, i32 bottom_colour, f32 alpha) {
    NuRndrClear(clear_flags, bottom_colour, alpha);
}
