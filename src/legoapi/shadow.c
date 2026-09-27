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
