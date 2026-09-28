#include "saga.h"

extern void *debris_cutoff_camera_vec;
extern i32 debris_render_group;

void DebrisSetSeed(i32 seed) {
    debrisseed = (u32)seed;
}

u32 DebrisGetSeed(void) {
    return debrisseed;
}

void DebrisSetTimeIncrement(f32 increment) {
    debris_time_increment = increment;
}

void DebrisSetCutSceneMode(i32 enabled) {
    DebrisCutSceneMode = enabled;
}

void DebrisRegisterCutoffCameraVec(void *position) {
    debris_cutoff_camera_vec = position;
}

void DebrisSetRenderGroup(i32 group) {
    debris_render_group = group;
}

i32 __cdecl PartDebrisCreatePart_candidate(i32 id, NUVEC *position, i32 flag) {
    STUBBED_NAMED("PartDebrisCreatePart_candidate: part template lookup and spawn");
    return -1;
}
