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

f32 NuMouseReadXVel(void) {
    return 0.0f;
}

f32 NuMouseReadYVel(void) {
    return 0.0f;
}

f32 NuMouseReadZVel(void) {
    return 0.0f;
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

static void cbSel(eduimenu_s *unused0, eduiitem_s *item) {
    selection_value = item->data;
}

static void cbGradChange(void) {
}

i32 cbInteractMenuTitle(edui_interact_s *interact) {
    static f32 offsetx;
    static f32 offsety;
    i32 result = (edui_cursor_buttons & EDUI_CURSOR_PRIMARY) != 0;
    eduimenu_s *menu = interact->menu;
    if (!(menu->flags & 1)) {
        if (!eduiInteractLocked) {
            offsetx = 0.0f - menu->x;
            offsety = 0.0f - menu->y;
        }
        menu->x = ((i32)(0.0f - offsetx));
        menu->y = ((i32)(0.0f - offsety));
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

void edcamMoveEx(FRAME_PAD_PC *pad, f32 delta_time) {
    if (edmainGetCursorEnabled() != 0) {
        if (PadFlyMode == 0 || NuKeyboard(0x38) != 0) {
            do_maya_mouse_camera(&gp_cam);
        } else {
            do_mouse_flymode_camera(&gp_cam, delta_time);
        }
    }
    if (pad != NULL) {
        if (PadFlyMode == 0) {
            do_Pad_Standard_camera(&gp_cam, delta_time, pad);
        } else {
            do_Pad_flymode_camera(&gp_cam, delta_time, pad);
        }
    }
}

void edcamMove(FRAME_PAD_PC *pad) {
    edcamMoveEx(pad, NuTimeGetFrameTime());
}

void edcamSetAng(i32 pitch, i32 yaw) {
    gp_cam.pitch = pitch;
    gp_cam.yaw = yaw;
}

void edcamGetPosAng(NUVEC *position, i32 *pitch, i32 *yaw) {
    if (position != NULL) {
        *position = gp_cam.position;
    }
    if (pitch != NULL) {
        *pitch = gp_cam.pitch;
    }
    if (yaw != NULL) {
        *yaw = gp_cam.yaw;
    }
}

void edcamGetPosAngSnap(NUVEC *position, i32 *pitch, i32 *yaw) {
    if (position != NULL) {
        *position = gp_cam.snapped_position;
    }
    if (pitch != NULL) {
        *pitch = gp_cam.snapped_pitch;
    }
    if (yaw != NULL) {
        *yaw = gp_cam.snapped_yaw;
    }
}

void edcamSetDist(f32 distance) {
    gp_cam.distance = distance;
}

void edcamMtx(NUMTX *matrix) {
    NUVEC distance = {0.0f, 0.0f, gp_cam.distance};
    NuMtxSetTranslation(matrix, &distance);
    NuMtxRotateX(matrix, gp_cam.pitch);
    NuMtxRotateY(matrix, gp_cam.yaw);
    NuMtxTranslate(matrix, &gp_cam.position);
    NuMtxTranslate(matrix, &gp_cam.offset);
    if (ed_remap_mtx != NULL) {
        NuMtxMul(matrix, matrix, ed_remap_mtx);
        ed_remap_mtx = NULL;
    }
}

/* PC input startup remains structurally incomplete at the marked platform
 * dependencies; these candidates preserve the verified wrapper ABI/order. */
extern HINSTANCE PCInputWindow_candidate;
extern HWND PCInputInstance_candidate;

i32 __cdecl NuKeyboardPlatformStartup_candidate(void) {
    STUBBED_NAMED(0x006d3150); /* PC input subsystem pre-configuration */
    NuKeyboardCreateWindowState_candidate(PCInputInstance_candidate, PCInputWindow_candidate);
    SetCursor(NULL);
    STUBBED_NAMED(0x006e57d0);
    STUBBED_NAMED(0x006e5340);
    return 1;
}

i32 __cdecl NuKeyboard_db(i32 key) {
    i32 page = (i8)nu_keyboard_page_index;
    u8 current = nu_keyboard_pages[(page << 8) + key];
    if (current != 0 && (current ^ nu_keyboard_pages[((page ^ 1) << 8) + key]) != 0)
        return 1;
    return 0;
}
