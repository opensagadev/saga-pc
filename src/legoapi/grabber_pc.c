#include "saga.h"

typedef struct grabber_movement_pc_s {
    u8 unknown_00[0x40];
    NUMTX grab_matrix;
    u8 unknown_80[0x40c];
    NUVEC grab_position;
    u8 unknown_498[0x18];
    CHARACTER_MODEL_LAYERS_PC *character_model;
    u8 unknown_4b4[0xa5];
    u8 flags_559;
} GRABBER_MOVEMENT_PC;
STATIC_ASSERT(offsetof(GRABBER_MOVEMENT_PC, grab_matrix) == 0x40);
STATIC_ASSERT(offsetof(GRABBER_MOVEMENT_PC, grab_position) == 0x48c);
STATIC_ASSERT(offsetof(GRABBER_MOVEMENT_PC, character_model) == 0x4b0);
STATIC_ASSERT(offsetof(GRABBER_MOVEMENT_PC, flags_559) == 0x559);

NUVEC *__cdecl Grabber_GetGrabPos(void *context, NUMTX *matrix) {
    GRABBER_MOVEMENT_PC *grabber = (GRABBER_MOVEMENT_PC *)context;
    if (grabber->character_model != NULL && (grabber->flags_559 & 1) != 0 &&
        grabber->character_model->points_of_interest[0] != NULL) {
        if (matrix != NULL)
            *matrix = grabber->grab_matrix;
        return (NUVEC *)&grabber->grab_matrix.m30;
    }
    if (matrix != NULL)
        *matrix = NuMtxSetIdentityTemplate;
    return &grabber->grab_position;
}
