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
    horizontal = (f64)PS2_REZ_W / 640.0;
    vertical = (f64)PS2_REZ_H / 224.0;
    render_state.viewport_x = (f32)(render_state.viewport_x * horizontal);
    render_state.viewport_width = (f32)(render_state.viewport_width * horizontal);
    render_state.viewport_y = (f32)(render_state.viewport_y * vertical);
    render_state.viewport_height = (f32)(render_state.viewport_height * vertical);
    render_state.camera_packet_cache = NULL;
    ++render_state.counter_0020;
    ++render_state.camera_id;
    return 1;
}

#include <string.h>

static NUMTX RndrStateTransformPc = {0};
static NUMTX RndrStateScaledTransformPc = {0};
static f32 RndrStateTransformScalePc = 1.0f;
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, counter_0020) == 0x20);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, fog_id) == 0x2e);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, fog_packet_cache) == 0x168);
STATIC_ASSERT(offsetof(NUGLOBALRNDRSTATE_PC, fog_state_word) == 0x16c);
void __cdecl NuRndrStateInit(void) {
    memset(&render_state, 0, sizeof(render_state));
    NuMtxSetIdentity(&RndrStateTransformPc);
    NuMtxSetIdentity(&RndrStateScaledTransformPc);
    RndrStateTransformScalePc = 1.0f;
    ++render_state.counter_0020;
    ++render_state.fog_id;
    render_state.fog_state_word = 0;
    render_state.fog_packet_cache = NULL;
    NuRndrStateUpdateCameraState();
}

/* Native PC material-dependent transform state belongs to this original owner. */
typedef struct PC_TRANSFORM_MATERIAL_VIEW PC_TRANSFORM_MATERIAL_VIEW;
struct PC_TRANSFORM_MATERIAL_VIEW {
    u8 unknown_00[0x30];
    PC_TRANSFORM_MATERIAL_VIEW *next;
    u8 unknown_34[0x2c];
    f32 transform_scale;
    u8 unknown_64[0x35];
    u8 transform_kind;
};
STATIC_ASSERT(offsetof(PC_TRANSFORM_MATERIAL_VIEW, next) == 0x30);
STATIC_ASSERT(offsetof(PC_TRANSFORM_MATERIAL_VIEW, transform_scale) == 0x60);
STATIC_ASSERT(offsetof(PC_TRANSFORM_MATERIAL_VIEW, transform_kind) == 0x99);

void __cdecl NuRndrStateGetTransformPc(NUMTX *output, const NUMTL *material) {
    const PC_TRANSFORM_MATERIAL_VIEW *selected = (const PC_TRANSFORM_MATERIAL_VIEW *)material;
    f32 scale;
    NUVEC scaling;
    if (selected->next != NULL)
        selected = selected->next;
    if (selected->transform_kind != 0x81 && selected->transform_kind != 0x85) {
        *output = RndrStateTransformPc;
        return;
    }
    scale = selected->transform_scale != 0.0f ? selected->transform_scale : 1.0f;
    if (scale != RndrStateTransformScalePc) {
        RndrStateTransformScalePc = scale;
        RndrStateScaledTransformPc = RndrStateTransformPc;
        if (scale != 1.0f) {
            scaling.z = scale;
            scaling.y = scale;
            scaling.x = scale;
            NuMtxPreScale(&RndrStateScaledTransformPc, &scaling);
        }
    }
    *output = RndrStateScaledTransformPc;
}

void __cdecl NuRndrStateSetTransformPc(const NUMTX *matrix) {
    f32 temporary;
    RndrStateTransformScalePc = 1.0f;
    RndrStateTransformPc = *matrix;
    temporary = RndrStateTransformPc.m01;
    RndrStateTransformPc.m01 = RndrStateTransformPc.m10;
    RndrStateTransformPc.m10 = temporary;
    temporary = RndrStateTransformPc.m02;
    RndrStateTransformPc.m02 = RndrStateTransformPc.m20;
    RndrStateTransformPc.m20 = temporary;
    temporary = RndrStateTransformPc.m21;
    RndrStateTransformPc.m21 = RndrStateTransformPc.m12;
    RndrStateTransformPc.m12 = temporary;
    RndrStateScaledTransformPc = RndrStateTransformPc;
}
