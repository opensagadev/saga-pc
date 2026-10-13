#include "saga.h"

STATIC_ASSERT(2 * sizeof(NURNDRLIGHTRECORD) + offsetof(NURNDRLIGHTRECORD, enabled) == 0x808);
extern "C" i32 __cdecl NuDynamicLightIsEnabled(NURNDRLIGHTRECORD *records) {
    return records[2].enabled != 0;
}
extern "C" void __cdecl NuDynamicLightSetParameterf(PC_SPECIAL_SHADOW_LIGHT *light, i32 parameter, f32 value) {
    light->SetParameterfPc(parameter, value);
}
extern "C" void __cdecl NuDynamicLightSetParameteri(PC_SPECIAL_SHADOW_LIGHT *light, i32 parameter, i32 value) {
    switch (parameter) {
        case 4:
            light->parameter_4 = value;
            break;
        case 5:
            light->parameter_5 = value;
            break;
    }
}
extern "C" void __cdecl NuDynamicLightAddRenderScene(NURNDRLIGHTRECORD *records, i32 index, i32 scene_id) {
    NURNDRLIGHTRECORD *record = &records[index];
    record->pending[record->count] = scene_id;
    ++record->count;
}
