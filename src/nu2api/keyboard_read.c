#include "saga.h"
#include <string.h>

typedef struct NuKeyboardDeviceVTable_PC {
    u8 unknown_0000_to_001c[0x1c];
    i32(__stdcall *acquire)(NuKeyboardDevice_PC *device);
    u8 unknown_0020_to_0024[4];
    i32(__stdcall *get_state)(NuKeyboardDevice_PC *device, u32 size, void *state);
    u8 unknown_0028_to_0064[0x64 - 0x28];
    i32(__stdcall *poll)(NuKeyboardDevice_PC *device);
} NuKeyboardDeviceVTable_PC;

struct NuKeyboardDevice_PC {
    NuKeyboardDeviceVTable_PC *vtable;
};

STATIC_ASSERT(offsetof(NuKeyboardDeviceVTable_PC, acquire) == 0x1c);
STATIC_ASSERT(offsetof(NuKeyboardDeviceVTable_PC, get_state) == 0x24);
STATIC_ASSERT(offsetof(NuKeyboardDeviceVTable_PC, poll) == 0x64);

u8 *__fastcall NuKeyboardPoll(NuKeyboardOwner_PC *owner) {
    NuKeyboardDevice_PC *device;
    u8 *page;
    u8 page_index;
    i32 i;
    i32 next_key_index;
    i32 previous_key_index;

    owner->poll_status = 0;
    if ((nu_keyboard_device_flags & 2) != 0 || owner->device == 0) {
        page_index = nu_keyboard_page_index;
        page = nu_keyboard_pages + (i8)page_index * 0x100;
        memset(page, 0, nu_keyboard_owner.key_count);
        return page;
    }

    owner->poll_status = 1;
    device = owner->device;
    if (device->vtable->poll(device) < 0) {
        if (device->vtable->acquire(device) < 0) {
            page_index = nu_keyboard_page_index;
            page = nu_keyboard_pages + (i8)page_index * 0x100;
            memset(page, 0, nu_keyboard_owner.key_count);
            return page;
        }
        device->vtable->poll(device);
    }

    page_index = (u8)(nu_keyboard_page_index ^ 1);
    nu_keyboard_page_index = page_index;
    page = nu_keyboard_pages + (i8)page_index * 0x100;
    if (device->vtable->get_state(device, 0x100, page) != 0) {
        memset(page, 0, nu_keyboard_owner.key_count);
        return page;
    }

    previous_key_index = nu_keyboard_key_index;
    nu_keyboard_previous_key_index = previous_key_index;
    next_key_index = 0;
    for (i = 0; i < nu_keyboard_owner.key_count; ++i) {
        if (page[i] != 0 && i != previous_key_index) {
            next_key_index = i;
        }
    }

    if (nu_keyboard_repeat_countdown != 0) {
        --nu_keyboard_repeat_countdown;
        if (next_key_index != 0) {
            nu_keyboard_repeat_countdown = 6;
        }
        memset(page, 0, nu_keyboard_owner.key_count);
        if (previous_key_index != 0 && page[previous_key_index] == 0) {
            nu_keyboard_key_index = 0;
        }
    } else if (next_key_index != 0) {
        nu_keyboard_key_index = next_key_index;
    }

    if (nu_keyboard_repeat_countdown == 0 && next_key_index == 0) {
        /* Preserve the last key while it remains down; otherwise clear it. */
        if (nu_keyboard_key_index != 0 && page[nu_keyboard_key_index] == 0) {
            nu_keyboard_key_index = 0;
        }
    } else if (nu_keyboard_key_index != 0 && page[nu_keyboard_key_index] == 0) {
        nu_keyboard_key_index = 0;
    }

    if (nu_keyboard_key_index == 0) {
        nu_keyboard_no_key_state = 0;
    }
    return page;
}

/* Observed mouse delta fields of the existing PC input-state allocation.
 * This view is not a separate object or a claim about the allocation extent. */
typedef struct NuMouseVelocityStateViewPc {
    u8 unknown_0000_to_6314[0x6314];
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
} NuMouseVelocityStateViewPc;
STATIC_ASSERT(offsetof(NuMouseVelocityStateViewPc, delta_x) == 0x6314);
STATIC_ASSERT(offsetof(NuMouseVelocityStateViewPc, delta_y) == 0x6318);
STATIC_ASSERT(offsetof(NuMouseVelocityStateViewPc, delta_z) == 0x631c);
STATIC_ASSERT(sizeof(NuMouseVelocityStateViewPc) == 0x6320);

f32 NuMouseReadXVel(void) {
    return (f32)(((NuMouseVelocityStateViewPc *)nu_input_state)->delta_x * -4.0);
}

f32 NuMouseReadYVel(void) {
    return (f32)(((NuMouseVelocityStateViewPc *)nu_input_state)->delta_y * -4.0);
}

f32 NuMouseReadZVel(void) {
    return (f32)(((NuMouseVelocityStateViewPc *)nu_input_state)->delta_z * -4.0);
}

void __cdecl NuKeyboardRead(void) {
    NuKeyboardState_PC *state;

    state = nu_input_state;
    if (!state->keyboard_polled) {
        state->keyboard_polled = 1;
        state->keyboard_state = NuKeyboardPoll(&nu_keyboard_owner);
    }
}

i32 NuKeyToAscii(u32 key, i32 shifted) {
    NUKEYCHAR *entry;

    for (entry = NuKeyChar; entry->key != 0; ++entry) {
        if (entry->key == key) {
            return shifted ? entry->shifted : entry->normal;
        }
    }
    return 0;
}

void cbSel(eduimenu_s *unused0, eduiitem_s *item) {
    selection_value = item->data;
}

void cbGradChange(void) {
}

f32 __cdecl NuMouseReadX(void);
f32 __cdecl NuMouseReadY(void);

i32 cbInteractMenuTitle(edui_interact_s *interact) {
    static f32 offsetx = 0;
    static f32 offsety = 0;
    i32 result = (edui_cursor_buttons & EDUI_CURSOR_PRIMARY) != 0;
    eduimenu_s *menu = interact->menu;
    if (!(menu->flags & 1)) {
        f32 mouse_x = (f32)(NuMouseReadX() * 640.0);
        f32 mouse_y = (f32)(NuMouseReadY() * 448.0);
        if (!eduiInteractLocked) {
            offsetx = mouse_x - menu->x;
            offsety = mouse_y - menu->y;
        }
        menu->x = (i32)(mouse_x - offsetx);
        menu->y = (i32)(mouse_y - offsety);
    }
    return result;
}

i32 cbInteractMenuScrollUp(edui_interact_s *interact) {
    eduimenu_s *menu = interact->menu;
    if (menu->field_0c) {
        if (menu->field_0c->previous)
            menu->field_0c = menu->field_0c->previous;
        menu->selected = menu->field_0c;
    }
    return 0;
}

i32 cbInteractMenuScrollDown(edui_interact_s *interact) {
    eduimenu_s *menu = interact->menu;
    if (menu->field_10) {
        if (menu->field_10->next)
            menu->field_10 = menu->field_10->next;
        menu->selected = menu->field_10;
    }
    return 0;
}

i32 cbInteractMenuCancelChild(edui_interact_s *interact) {
    eduimenu_s *menu = interact->menu;
    if (menu && menu->child && menu->child->callback)
        menu->child->callback(menu->child, menu);
    return 0;
}

void do_mouse_flymode_camera(edcam_s *camera, f32 delta_time) {
    NUVEC opposite_offset;
    NUMTX rotation;
    NUVEC opposite_positions[2];
    NUVEC *old_opposite;
    i32 pitch;
    NUVEC *new_opposite;
    f32 zoom_scale = camera->auto_zoom_dist_scale == 0.0f
                         ? 1.0f
                         : fabsf(camera->distance) * camera->auto_zoom_dist_scale + camera->auto_zoom_base;

    f32 mouse_y = NuMouseReadYRel();
    f32 mouse_x = NuMouseReadXRel();
    f32 mouse_z = NuMouseReadZRel();
    u32 buttons = NuMouseReadButtons();
    if (camera->allow_distance) {
        f32 distance = camera->distance - mouse_z * camera->distance_speed * zoom_scale;
        camera->distance = MIN(-camera->minimum_distance, distance);
    }

    opposite_offset.z = 0.0f;
    opposite_offset.y = 0.0f;
    opposite_offset.x = 0.0f;
    rotation = numtx_identity;
    NuMtxRotateX(&rotation, camera->pitch);
    NuMtxRotateY(&rotation, camera->yaw);
    opposite_offset.z = -camera->distance;
    NuVecMtxRotate(&opposite_offset, &opposite_offset, &rotation);

    old_opposite = &(opposite_positions[0]);
    NuVecAdd(&(*old_opposite), &camera->position, &opposite_offset);

    if (!camera->allow_pitch)
        pitch = camera->pitch;
    else
        pitch = ((i32)(mouse_y * 16.0f)) + camera->pitch;
    pitch = MIN(0x4000, pitch);
    pitch = MAX(pitch, -0x4000);
    camera->pitch = pitch;
    if (camera->allow_yaw)
        camera->yaw += ((i32)(mouse_x * 16.0f));

    rotation = numtx_identity;
    NuMtxRotateX(&rotation, camera->pitch);
    NuMtxRotateY(&rotation, camera->yaw);
    opposite_offset.z = -camera->distance;
    opposite_offset.y = 0.0f;
    opposite_offset.x = 0.0f;
    NuVecMtxRotate(&opposite_offset, &opposite_offset, &rotation);
    new_opposite = &(opposite_positions[1]);
    NuVecAdd(&(*new_opposite), &camera->position, &opposite_offset);
    NuVecAdd(&camera->position, &camera->position, &(*new_opposite));
    NuVecSub(&camera->position, &camera->position, &(*old_opposite));

    {
        NUVEC movement = {0.0f, 0.0f, 0.0f};
        if (buttons == 2)
            movement.z = delta_time * camera->distance;
        else if (buttons == 1)
            movement.z = -delta_time * camera->distance;
        NuVecMtxRotate(&movement, &movement, &rotation);
        if (camera->allow_position_x)
            camera->position.x += movement.x;
        if (camera->allow_position_y)
            camera->position.y += movement.y;
        if (camera->allow_position_z)
            camera->position.z += movement.z;
    }
}

extern i32 edmain_cursor_enabled;

void edcamMoveEx(FRAME_PAD_PC *pad, f32 delta_time) {
    if (edmain_cursor_enabled != 0) {
        if (PadFlyMode == 0 || NuKeyboard(0x38) != 0) {
            do_maya_mouse_camera(gp_cam);
        } else {
            do_mouse_flymode_camera(gp_cam, delta_time);
        }
    }
    if (pad != NULL) {
        if (PadFlyMode == 0) {
            do_Pad_Standard_camera(gp_cam, delta_time, pad);
        } else {
            do_Pad_flymode_camera(gp_cam, delta_time, pad);
        }
    }
}

void edcamMove(FRAME_PAD_PC *pad) {
    edcamMoveEx(pad, NuTimeGetFrameTime());
}

void edcamSetPos(NUVEC *position) {
    edcam_s *camera = gp_cam;
    camera->position = *position;
    camera->offset.z = 0.0f;
    camera->offset.y = 0.0f;
    camera->offset.x = 0.0f;
}

void edcamSetAng(i32 pitch, i32 yaw) {
    edcam_s *camera = gp_cam;
    camera->pitch = pitch;
    camera->yaw = yaw;
}

void edcamGetPosAng(NUVEC *position, i32 *pitch, i32 *yaw) {
    edcam_s *camera = gp_cam;
    if (position != NULL) {
        *position = camera->position;
    }
    if (pitch != NULL) {
        *pitch = camera->pitch;
    }
    if (yaw != NULL) {
        *yaw = camera->yaw;
    }
}

void edcamGetPosAngSnap(NUVEC *position, i32 *pitch, i32 *yaw) {
    edcam_s *camera = gp_cam;
    if (position != NULL) {
        *position = camera->snapped_position;
    }
    if (pitch != NULL) {
        *pitch = camera->snapped_pitch;
    }
    if (yaw != NULL) {
        *yaw = camera->snapped_yaw;
    }
}

void edcamSetDist(f32 distance) {
    gp_cam->distance = distance;
}

void edcamMtx(NUMTX *matrix) {
    NUVEC distance = {0.0f, 0.0f, gp_cam->distance};
    NuMtxSetTranslation(matrix, &distance);
    NuMtxRotateX(matrix, gp_cam->pitch);
    NuMtxRotateY(matrix, gp_cam->yaw);
    NuMtxTranslate(matrix, &gp_cam->position);
    NuMtxTranslate(matrix, &gp_cam->offset);
    if (ed_remap_mtx != NULL) {
        NuMtxMul(matrix, matrix, ed_remap_mtx);
        ed_remap_mtx = NULL;
    }
}

extern NUCAMERA *EdMainCurrentCameraPc;

void edcamSet(void) {
    NUMTX matrix;
    NUCAMERA *camera;
    edcamMtx(&matrix);
    camera = EdMainCurrentCameraPc;
    camera->mtx = matrix;
    NuCameraSet(camera);
}

/* PC input startup remains structurally incomplete at the marked platform
 * dependencies; these candidates preserve the verified wrapper ABI/order. */
extern HINSTANCE PCInputWindow_candidate;
extern HWND PCInputInstance_candidate;
i32 __cdecl PCDeviceShowCursorPc(i32 show);

i32 __cdecl NuKeyboardPlatformStartup_candidate(void) {
    PcConfigureInputGraphicsCaps();
    NuKeyboardCreateWindowState_candidate(PCInputInstance_candidate, PCInputWindow_candidate);
    SetCursor(NULL);
    PCDeviceShowCursorPc(0);
    PCDeviceSetRenderState(0x16, 1);
    return 1;
}

i32 __cdecl NuKeyboard_db(i32 key) {
    i32 page = (i8)nu_keyboard_page_index;
    u8 current = nu_keyboard_pages[(page << 8) + key];
    if (current != 0 && (current ^ nu_keyboard_pages[((page ^ 1) << 8) + key]) != 0)
        return 1;
    return 0;
}

/* Private Source proposal; append in keyboard_read.c after old source. */
void do_Pad_flymode_camera(edcam_s *camera, f32 delta_time, FRAME_PAD_PC *pad) {
    f32 frame_scale = (f32)(delta_time / (f64)(1.0f / 60.0f));
    f32 move_speed = camera->auto_move_dist_scale == 0.0f
                         ? 1.0f
                         : camera->auto_move_dist_scale * fabsf(camera->distance) + camera->auto_move_base;
    f32 zoom_speed = camera->auto_zoom_dist_scale == 0.0f
                         ? 1.0f
                         : fabsf(camera->distance) * camera->auto_zoom_dist_scale + camera->auto_zoom_base;
    NUVEC opposite_offset;
    NUMTX rotation;
    NUVEC old_opposite;
    NUVEC new_opposite;
    NUVEC movement;
    i32 pad_yaw;
    i32 yaw_delta;
    i32 pad_pitch;
    i32 pitch_delta;
    i32 pad_move_x;
    i32 pad_move_z;

    opposite_offset.z = 0.0f;
    opposite_offset.y = 0.0f;
    opposite_offset.x = 0.0f;
    rotation = numtx_identity;
    NuMtxRotateX(&rotation, camera->pitch);
    NuMtxRotateY(&rotation, camera->yaw);
    opposite_offset.z = -camera->distance;
    NuVecMtxRotate(&opposite_offset, &opposite_offset, &rotation);
    NuVecAdd(&old_opposite, &camera->position, &opposite_offset);

    pad_yaw = NuPs2ApplyDeadZone(pad->analog_right_x, 32);
    yaw_delta = (i32)((f64)pad_yaw * camera->pad_yaw_speed * delta_time * 64.0);
    pad_pitch = NuPs2ApplyDeadZone(pad->analog_right_y, 32);
    pitch_delta = (i32)((f64)pad_pitch * camera->pad_pitch_speed * delta_time * 32.0);
    if (camera->freedoms & EDCAM_FREEDOM_INVERT_PAD_PITCH)
        pitch_delta = -pitch_delta;
    if (camera->freedoms & EDCAM_FREEDOM_PITCH) {
        camera->pitch += pitch_delta;
        camera->pitch = MIN(0x4000, camera->pitch);
        camera->pitch = MAX(-0x4000, camera->pitch);
    }
    if (camera->freedoms & EDCAM_FREEDOM_YAW)
        camera->yaw += yaw_delta;

    rotation = numtx_identity;
    NuMtxRotateX(&rotation, camera->pitch);
    NuMtxRotateY(&rotation, camera->yaw);
    opposite_offset.z = -camera->distance;
    opposite_offset.y = 0.0f;
    opposite_offset.x = 0.0f;
    NuVecMtxRotate(&opposite_offset, &opposite_offset, &rotation);
    NuVecAdd(&new_opposite, &camera->position, &opposite_offset);
    NuVecAdd(&camera->position, &camera->position, &new_opposite);
    NuVecSub(&camera->position, &camera->position, &old_opposite);

    movement.z = 0.0f;
    movement.y = 0.0f;
    movement.x = 0.0f;
    pad_move_x = NuPs2ApplyDeadZone(pad->analog_left_x, 32);
    movement.x -= (f32)pad_move_x * delta_time * camera->distance * (f64)0.01f;
    pad_move_z = NuPs2ApplyDeadZone(pad->analog_left_y, 32);
    movement.z += (f32)pad_move_z * delta_time * camera->distance * (f64)0.01f;
    movement.y += (f32)pad->analog_l1 * camera->position_speed.y * move_speed * frame_scale * 0.5;
    movement.y -= (f32)pad->analog_l2 * camera->position_speed.y * move_speed * frame_scale * 0.5;
    if (camera->allow_distance) {
        camera->distance += (f32)pad->analog_r1 * camera->distance_speed * zoom_speed * frame_scale;
        camera->distance -= (f32)pad->analog_r2 * camera->distance_speed * zoom_speed * frame_scale;
        camera->distance = MIN(-camera->minimum_distance, camera->distance);
    }
    NuVecMtxRotate(&movement, &movement, &rotation);
    if (camera->freedoms & EDCAM_FREEDOM_POSITION_X)
        camera->position.x += movement.x;
    if (camera->freedoms & EDCAM_FREEDOM_POSITION_Y)
        camera->position.y += movement.y;
    if (camera->freedoms & EDCAM_FREEDOM_POSITION_Z)
        camera->position.z += movement.z;
}

void do_Pad_Standard_camera(edcam_s *camera, f32 delta_time, FRAME_PAD_PC *pad) {
    NUVEC movement = {0.0f, 0.0f, 0.0f};
    f32 frame_scale;
    f32 move_speed;
    f32 zoom_speed;
    u8 pad_yaw_axis;
    u8 pad_pitch_axis;
    NUVEC snap_delta;
    if (!pad->is_valid)
        return;

    frame_scale = (f32)(delta_time / (f64)(1.0f / 60.0f));
    move_speed = camera->auto_move_dist_scale == 0.0f
                     ? 1.0f
                     : camera->auto_move_dist_scale * fabsf(camera->distance) + camera->auto_move_base;
    zoom_speed = camera->auto_zoom_dist_scale == 0.0f
                     ? 1.0f
                     : camera->auto_zoom_dist_scale * fabsf(camera->distance) + camera->auto_zoom_base;
    pad_yaw_axis = pad->analog_right_x;
    pad_pitch_axis = pad->analog_right_y;
    if (camera->freedoms & EDCAM_FREEDOM_DISTANCE) {
        f32 zoom_in = (f32)pad->analog_r1 * camera->distance_speed * zoom_speed;
        f32 zoom_out;
        if (zoom_in > 1.0f)
            zoom_in = 1.0f;
        camera->distance += zoom_in * frame_scale;
        zoom_out = (f32)pad->analog_r2 * camera->distance_speed * zoom_speed;
        if (zoom_out > 1.0f)
            zoom_out = 1.0f;
        camera->distance -= zoom_out * frame_scale;
        camera->distance = MIN(-camera->minimum_distance, camera->distance);
    }
    if (camera->freedoms & EDCAM_FREEDOM_PITCH) {
        i32 pitch_input = NuPs2ApplyDeadZone(pad_pitch_axis, 32);
        camera->pitch += (i32)((f32)(pitch_input * camera->pad_pitch_speed) * delta_time * -64.0);
        camera->pitch = MIN(0x4000, camera->pitch);
        camera->pitch = MAX(-0x4000, camera->pitch);
    }
    if (camera->freedoms & EDCAM_FREEDOM_YAW) {
        i32 yaw_input = NuPs2ApplyDeadZone(pad_yaw_axis, 32);
        camera->yaw += (i32)((f32)(yaw_input * camera->pad_yaw_speed) * delta_time * -64.0);
    }
    if (camera->freedoms & EDCAM_FREEDOM_POSITION_Y) {
        movement.y += (f32)pad->analog_l1 * camera->position_speed.y * move_speed * frame_scale * 0.5;
        movement.y -= (f32)pad->analog_l2 * camera->position_speed.y * move_speed * frame_scale * 0.5;
    }
    if (camera->freedoms & EDCAM_FREEDOM_POSITION_Z) {
        movement.z =
            -(f32)NuPs2ApplyDeadZone(pad->analog_left_y, 32) * camera->position_speed.y * move_speed * frame_scale;
    }
    if (camera->freedoms & EDCAM_FREEDOM_POSITION_X) {
        movement.x =
            (f32)NuPs2ApplyDeadZone(pad->analog_left_x, 32) * camera->position_speed.y * move_speed * frame_scale;
    }
    NuVecRotateY(&movement, &movement, camera->yaw);
    NuVecAdd(&camera->position, &camera->position, &movement);
    NuVecSub(&snap_delta, &camera->position, &camera->snap_origin);
    snap_delta.x /= camera->snap_step.x;
    snap_delta.y /= camera->snap_step.y;
    snap_delta.z /= camera->snap_step.z;
    camera->snapped_position.x = NuFloor(snap_delta.x) * camera->snap_step.x + camera->snap_origin.x;
    camera->snapped_position.y = NuFloor(snap_delta.y) * camera->snap_step.y + camera->snap_origin.y;
    camera->snapped_position.z = NuFloor(snap_delta.z) * camera->snap_step.z + camera->snap_origin.z;
}

/* PC Maya drag angles are observed through DWORD reads and low-WORD writes.
 * The upper halves remain part of each view and are preserved by updates. */
typedef union EdcamMayaAngleObservedPc {
    u32 word;
    struct {
        u16 angle;
        u16 upper;
    } parts;
} EdcamMayaAngleObservedPc;
STATIC_ASSERT(sizeof(EdcamMayaAngleObservedPc) == 4);
STATIC_ASSERT(offsetof(EdcamMayaAngleObservedPc, parts.angle) == 0);
STATIC_ASSERT(offsetof(EdcamMayaAngleObservedPc, parts.upper) == 2);
STATIC_ASSERT(offsetof(edcam_s, distance) == 0x14);
STATIC_ASSERT(offsetof(edcam_s, distance_speed) == 0x6c);
STATIC_ASSERT(offsetof(edcam_s, minimum_distance) == 0x70);
STATIC_ASSERT(offsetof(edcam_s, mouse_pitch_speed) == 0x74);
STATIC_ASSERT(offsetof(edcam_s, mouse_yaw_speed) == 0x78);
STATIC_ASSERT(offsetof(edcam_s, mouse_move_speed) == 0x7c);
STATIC_ASSERT(offsetof(edcam_s, auto_zoom_base) == 0x88);
STATIC_ASSERT(offsetof(edcam_s, auto_zoom_dist_scale) == 0x8c);
STATIC_ASSERT(offsetof(edcam_s, freedoms) == 0x90);

#pragma section(".bss$EdcamMayaSmoothedAnglePc", read, write)
#pragma section(".bss$EdcamMayaRawAnglePc", read, write)
void do_maya_mouse_camera(edcam_s *camera) {
    /* The historical mapped name TargetAng denotes the smoothed PC slot. */
    __declspec(allocate(".bss$EdcamMayaSmoothedAnglePc")) static EdcamMayaAngleObservedPc TargetAng = {0};
    __declspec(allocate(".bss$EdcamMayaRawAnglePc")) static EdcamMayaAngleObservedPc RawTargetAngPc = {0};
    f32 zoom_scale = camera->auto_zoom_dist_scale == 0.0f
                         ? 1.0f
                         : fabsf(camera->distance) * camera->auto_zoom_dist_scale + camera->auto_zoom_base;
    NUVEC movement = {0.0f, 0.0f, 0.0f};
    f32 mouse_x;
    f32 mouse_y;
    f32 mouse_z;
    u32 buttons;
    f32 minimum;
    NUMTX rotation;

    if (NuKeyboard(0x38) == 0)
        return;
    mouse_x = NuMouseReadXVel() * camera->mouse_pitch_speed;
    mouse_y = NuMouseReadYVel() * camera->mouse_yaw_speed;
    mouse_z = NuMouseReadZVel() * camera->mouse_move_speed;
    buttons = NuMouseReadButtons();
    if (buttons == 2 || buttons == 5) {
        if (camera->allow_distance) {
            i32 difference;
            f32 sine;
            if (mouse_x != 0.0f || mouse_y != 0.0f)
                RawTargetAngPc.parts.angle = (u16)NuAtan2D(-mouse_x, -mouse_y);
            difference = (RawTargetAngPc.word - TargetAng.word) & 0xffff;
            if (difference >= 0x8000)
                difference -= 0x10000;
            if (abs(difference) > 20000)
                TargetAng.parts.angle = RawTargetAngPc.parts.angle;
            else
                TargetAng.parts.angle += difference / 4;
            sine = NuTrigTable[((i32)(TargetAng.word + 0x2000) >> 1) & 0x7fff];
            if (fabsf(sine) > (f64)0.1f) {
                f32 length_squared;
                f32 zoom_delta;
                if (sine > 0.0f)
                    sine = (f32)(sine - (f64)0.1f);
                else
                    sine = (f32)(sine + (f64)0.1f);
                length_squared = mouse_x * mouse_x + mouse_y * mouse_y;
                zoom_delta = NuFsqrt(length_squared) * sine;
                camera->distance =
                    camera->distance + camera->distance_speed * zoom_scale * zoom_delta * camera->mouse_move_speed;
                minimum = -camera->minimum_distance;
                if (minimum < camera->distance)
                    camera->distance = minimum;
            }
        }
    } else if (buttons == 1) {
        if (camera->allow_pitch) {
            camera->pitch += (i32)(mouse_y * -16.0);
            if (camera->pitch > 0x4000)
                camera->pitch = 0x4000;
            if (camera->pitch < -0x4000)
                camera->pitch = -0x4000;
        }
        if (camera->allow_yaw)
            camera->yaw += (i32)(mouse_x * -16.0);
    } else if (buttons == 4 || buttons == 3) {
        movement.x = (f32)(movement.x + mouse_x * 0.0078125);
        movement.y = (f32)(movement.y - mouse_y * 0.0078125);
    }

    camera->distance = camera->distance - camera->distance_speed * zoom_scale * mouse_z;
    minimum = -camera->minimum_distance;
    if (minimum < camera->distance)
        camera->distance = minimum;
    rotation = NuMtxSetIdentityTemplate;
    NuMtxRotateX(&rotation, camera->pitch);
    NuMtxRotateY(&rotation, camera->yaw);
    NuVecMtxRotate(&movement, &movement, &rotation);
    if (camera->allow_position_x)
        camera->position.x += movement.x;
    if (camera->allow_position_y)
        camera->position.y += movement.y;
    if (camera->allow_position_z)
        camera->position.z += movement.z;
}

f32 __cdecl edcamGetDist(void) {
    return gp_cam->distance;
}
