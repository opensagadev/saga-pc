#include "saga.h"

void __cdecl NuWindAnimate(NUWIND *wind, f32 frame_time) {
    if (wind != NULL) {
        wind->phase += nu_wind_phase_scale * wind->phase_speed_scale * frame_time;
        if (wind->phase >= 1.0f)
            wind->phase = NuFmod(wind->phase, 1.0f);
        wind->scaled_accumulated_time = nu_wind_scaled_time_multiplier * frame_time + wind->scaled_accumulated_time;
        wind->accumulated_time = frame_time + wind->accumulated_time;
    }
}

i32 __cdecl NuWindCurrent(NUWIND *wind) {
    if (wind == NULL || wind->current < 0)
        return -1;
    return wind->texture_ids[wind->current];
}

/* Original source: gamelib/nuwind/nuwind.cpp. */
void __cdecl NuWindInitialise(NUWIND *wind) {
    if (wind != NULL) {
        i32 index;
        for (index = 0; index < 8; ++index)
            wind->texture_ids[index] = -1;
        wind->parameter_24 = 1.0f;
        wind->phase_speed_scale = 1.0f;
        wind->current = -1;
        wind->phase = 0.0f;
        wind->accumulated_time = 0.0f;
        wind->scaled_accumulated_time = 0.0f;
    }
}
