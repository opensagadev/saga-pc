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

void __cdecl NuVpGetSize2(f32 *width, f32 *height);
i32 __cdecl NuRndrStateUpdateCameraState(void) {
    NUMTX *projection = NuCameraGetProjectionMtx();
    NUMTX *view = NuCameraGetViewMtx();
    f64 horizontal, vertical;
    render_state.camera_matrix = *view;
    render_state.projection_0154 = projection->m00;
    render_state.projection_0158 = projection->m11;
    render_state.projection_015c = projection->m22;
    render_state.projection_0160 = projection->m23;
    render_state.projection_0164 = projection->m32;
    NuVpGetPosition2(&render_state.viewport_x, &render_state.viewport_y);
    NuVpGetSize2(&render_state.viewport_width, &render_state.viewport_height);
    horizontal = (f64)DebrisGlassTextureWidth / 640.0;
    vertical = (f64)DebrisGlassTextureHeight / 224.0;
    render_state.viewport_x = (f32)(render_state.viewport_x * horizontal);
    render_state.viewport_width = (f32)(render_state.viewport_width * horizontal);
    render_state.viewport_y = (f32)(render_state.viewport_y * vertical);
    render_state.viewport_height = (f32)(render_state.viewport_height * vertical);
    render_state.camera_packet_cache = NULL;
    ++render_state.counter_0020;
    ++render_state.camera_id;
    return 1;
}
