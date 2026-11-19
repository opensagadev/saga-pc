#include "gamelib/nuwind/nuwind.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/nuprim_gen.h"
#include "nu2api/nustencilshadows_PC.h"

extern i32 dat_009739d4;

const f64 nu_wind_phase_scale = 1.0 / 256.0;

void NuWindAnimate(NUWIND *wind, f32 frame_time) {
    if (wind != NULL) {
        wind->phase += nu_wind_phase_scale * wind->phase_speed_scale * frame_time;
        if (wind->phase >= 1.0f) {
            wind->phase = NuFmod(wind->phase, 1.0f);
        }
        wind->scaled_accumulated_time = 5.0 * frame_time + wind->scaled_accumulated_time;
        wind->accumulated_time = frame_time + wind->accumulated_time;
    }
}

i32 dat_009739d4 = 0;

void NuWindSetDlistValue(i32 value) {
    dat_009739d4 = value;
}

VARIPTR *NuWindDlistCursor(void) {
    return &nudlist_stream_cursor;
}

i32 NuWindCurrent(NUWIND *wind) {
    if (wind == NULL || wind->current < 0) {
        return -1;
    }
    return wind->texture_ids[wind->current];
}

/* Original source: gamelib/nuwind/nuwind.cpp. */
void NuWindInitialise(NUWIND *wind) {
    if (wind != NULL) {
        i32 index;
        for (index = 0; index < 8; ++index) {
            wind->texture_ids[index] = -1;
        }
        wind->parameter_24 = 1.0f;
        wind->phase_speed_scale = 1.0f;
        wind->current = -1;
        wind->phase = 0.0f;
        wind->accumulated_time = 0.0f;
        wind->scaled_accumulated_time = 0.0f;
    }
}

void NuWindSetCurrent(NUWIND *wind, u32 index) {
    if (wind == NULL || index >= 8) {
        nuapi.wind = NULL;
    } else if (wind->texture_ids[index] >= 0) {
        wind->current = index;
        nuapi.wind = wind;
    }
}

void NuWindSetWorldSize(NUWIND *wind, f32 value) {
    if (wind != NULL) {
        if (value >= 1.0f) {
            wind->parameter_24 = value;
        } else {
            wind->parameter_24 = 1.0f;
        }
    }
}

void NuWindSetSpeed(NUWIND *wind, f32 value) {
    if (wind != NULL) {
        if (value >= 1.0f) {
            wind->phase_speed_scale = value;
        } else {
            wind->phase_speed_scale = 1.0f;
        }
    }
}
