#include "saga.h"
typedef struct CAMERA_RENDER_SPECULAR_READER_PC {
    u8 unknown_0000[0xdc];
    NUVEC direction;
} CAMERA_RENDER_SPECULAR_READER_PC;
STATIC_ASSERT(offsetof(CAMERA_RENDER_SPECULAR_READER_PC, direction) == 0xdc);
void __cdecl NuRndrStateSetSpecularLightEx(const NUVEC *direction, const NUMTX *matrix, const NUVEC *colour) {
    CAMERA_RENDER_SPECULAR_READER_PC *state = (CAMERA_RENDER_SPECULAR_READER_PC *)&render_state;
    render_state.specular_mtx = *matrix;
    render_state.specular_colour = *colour;
    state->direction = *direction;
    ++render_state.counter_0020;
    ++render_state.lights_id;
    render_state.light_packet_cache = NULL;
}
