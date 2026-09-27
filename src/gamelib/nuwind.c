#include "saga.h"

void __cdecl NuWindAnimate(NUWIND *wind, f32 frame_time) {
    if (wind != NULL) {
        union {
            f32 value;
            u32 bits;
        } phase_wrap_period;
        union {
            f32 value;
            u32 bits;
        } phase_bits;
        wind->phase += nu_wind_phase_scale * wind->phase_speed_scale * frame_time;
        phase_wrap_period.bits = 0x3f800000u;
        phase_bits.value = wind->phase;
        if ((phase_bits.bits & 0x80000000u) == 0 && phase_bits.bits >= 0x3f800000u && phase_bits.bits <= 0x7f800000u) {
            wind->phase = NuFmod(wind->phase, phase_wrap_period.value);
        }
        wind->scaled_accumulated_time = nu_wind_scaled_time_multiplier * frame_time + wind->scaled_accumulated_time;
        wind->accumulated_time = frame_time + wind->accumulated_time;
    }
}

u32 NuWindRand(void) {
    NuWindQS = (NuWindQS * 0x24cd + 1) & 0xffff;
    return NuWindQS;
}
