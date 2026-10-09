#include "saga.h"

STATIC_ASSERT(2 * sizeof(NURNDRLIGHTRECORD) + offsetof(NURNDRLIGHTRECORD, enabled) == 0x808);
i32 __cdecl NuDynamicLightIsEnabled(NURNDRLIGHTRECORD *records) {
    return records[2].enabled != 0;
}
void __cdecl NuDynamicLightAddRenderScene(NURNDRLIGHTRECORD *records, i32 index, i32 scene_id) {
    NURNDRLIGHTRECORD *record = &records[index];
    record->pending[record->count] = scene_id;
    ++record->count;
}
