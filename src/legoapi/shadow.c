#include "saga.h"

void __cdecl InitShadowLights(void) {
    /*
     * PC extent: 0x00596530..0x005967a7 (632 bytes).
     *
     * Retail initialization order:
     * 1. Create and configure the first shadow-light object, including
     *    parameter slots 0, 1, 2, 3, 14, 11, 18 and 16. The observed values
     *    are 0.85, 3.5, 0.4, 0.2 and 0.0001.
     * 2. Initialize its light parameters with values at 0x00751d98 (300.0),
     *    0x0075194c (60.0), 0x00763954 (-60.0) and the double at
     *    0x007517a0 (2.0).
     * 3. Create and configure the second object, with slots 0, 1, 2, 3, 14
     *    and 11 set from 0.25, 3.5, 0.85, 0.2 and 0.0001; initialize its
     *    light parameters in the same order as the first object.
     * 4. If the second object's count at +0x808 is zero, shift its existing
     *    entries from +0x70c in 0x380-byte steps and mark the second object's
     *    +0x808 field initialized.
     *
     * The PC path depends on Windows renderer object construction and field
     * setters that are not reconstructed yet. Android's InitShadowLights is
     * empty, so these PC-only operations remain intentionally stubbed.
     */
    STUBBED_NAMED("PC InitShadowLights renderer initialization");
}

f32 __cdecl NewShadow(NUVEC *position, f32 unused, f32 height, i32 mask) {
    return NewShadowEx(position, unused, height, height, mask);
}

STATIC_ASSERT(offsetof(GAMEOBJECT, suit) == 0xcbc);
STATIC_ASSERT(offsetof(SUIT_LAYERS_PC, flags) == 0xc);
void __cdecl Shadow_SetMode(void) {
    i32 i;
    ShadowMode = 0;
    for (i = 0; i < 8; ++i) {
        GAMEOBJECT *player = Player[i];
        if (player != NULL && player->suit != NULL && (player->suit->flags & 1) != 0) {
            ShadowMode = 1;
            return;
        }
    }
}

f32 __cdecl BlobShadowFade(NUVEC *position, f32 fade_start, f32 fade_end, f32 alpha) {
    NUVEC delta;
    f32 distance_squared;
    if (alpha <= 0.0f)
        return 0.0f;
    NuVecSub(&delta, position, (NUVEC *)&pNuCam->mtx.m30);
    distance_squared = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
    if (!(distance_squared < fade_end * fade_end))
        return 0.0f;
    if (!(distance_squared > fade_start * fade_start))
        return alpha;
    return (1.0f - (NuFsqrt(distance_squared) - fade_start) / (fade_end - fade_start)) * alpha;
}
