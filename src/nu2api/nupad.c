#include "saga.h"
#define NUGENERICPAD FRAME_SCANNED_PAD_PC
#define ANALOG_CENTER 0x80
#define NUPAD_PLAY 2
#define NUPAD_RECORD 1
#define NUPAD_NORM 0
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
typedef struct FRAME_PAD_RAMP_PC {
    f32 threshold, value;
} FRAME_PAD_RAMP_PC;

static FRAME_SCANNED_PAD_PC g_nupadScannedPads[16];
static f32 FramePadRampRatePc = 0.1f;
static FRAME_PAD_RAMP_PC FramePadRampValuesPc[5] = {
    {0.0f, 0.0f}, {0.5f, 0.1f}, {0.75f, 0.25f}, {0.85f, 0.5f}, {1.0f, 1.0f}};
static i32 FramePadDirectMappingPc, FramePadActivatedPc;

static void DeadZoneValueXYProper(i32 *x_value, i32 *y_value, i32 deadzone) {
    i32 inverse;
    i32 cur_value;

    inverse = ANALOG_CENTER - deadzone;

    cur_value = *x_value;
    if (cur_value >= deadzone) {
        cur_value++;

        *x_value = ((cur_value - deadzone) * 0x7f) / inverse;
    } else if (cur_value < -deadzone) {
        *x_value = ((cur_value + deadzone) * 0x7f) / inverse;
    } else {
        *x_value = 0;
    }

    cur_value = *y_value;
    if (cur_value >= deadzone) {
        cur_value++;

        *y_value = ((cur_value - deadzone) * 0x7f) / inverse;
    } else if (cur_value < -deadzone) {
        *y_value = ((cur_value + deadzone) * 0x7f) / inverse;
    } else {
        *y_value = 0;
    }
}

static void DeadZoneValueXY(i32 *x_value, i32 *y_value, i32 deadzone) {
    f32 x_normal;
    f32 y_normal;
    f32 deadzone_normal;
    f32 magnitude;

    if (FramePadCorrectDeadZoningPc) {
        DeadZoneValueXYProper(x_value, y_value, deadzone);

        return;
    }

    x_normal = *x_value / 127.0f;
    y_normal = *y_value / 127.0f;
    deadzone_normal = deadzone / 127.0f;

    if (x_normal > 1.0f) {
        x_normal = 1.0f;
    }

    if (x_normal < -1.0f) {
        x_normal = -1.0f;
    }

    if (y_normal > 1.0f) {
        y_normal = 1.0f;
    }

    if (y_normal < -1.0f) {
        y_normal = -1.0f;
    }

    magnitude = NuFsqrt(x_normal * x_normal + y_normal * y_normal);

    if (magnitude < deadzone_normal) {
        *x_value = 0;
        *y_value = 0;
    }
}

static f32 CalcRamp(f32 ramp) {
    i32 i;
    if (!(ramp < 1.0f))
        ramp = 1.0f;
    for (i = 0; ramp > FramePadRampValuesPc[i].threshold; ++i) {
    }
    return FramePadRampValuesPc[i].value;
}
enum {
    PAD_BUTTON_JUMP_SELECT = 1 << 0x6,
    PAD_BUTTON_START = 1 << 0xb,
    PAD_BUTTON_UP = 1 << 0xc,
    PAD_BUTTON_RIGHT = 1 << 0xd,
    PAD_BUTTON_DOWN = 1 << 0xe,
    PAD_BUTTON_LEFT = 1 << 0xf,
};

void NuPadProcessReadData(NUGENERICPAD *pad) {
    f32 ramp_rate_normal;

    pad->digital_buttons_pressed = pad->digital_buttons & ~pad->digital_buttons_prev;
    pad->digital_buttons_released = ~pad->digital_buttons & pad->digital_buttons_prev;

    pad->digital_buttons_prev = pad->digital_buttons;

    pad->digital_left_x = pad->analog_left_x - ANALOG_CENTER;
    pad->digital_left_y = ANALOG_CENTER - pad->analog_left_y;

    pad->digital_right_x = pad->analog_right_x - ANALOG_CENTER;
    pad->digital_right_y = ANALOG_CENTER - pad->analog_right_y;

    if (pad->deadzone != 0) {
        DeadZoneValueXY(&pad->digital_left_x, &pad->digital_left_y, pad->deadzone);
        DeadZoneValueXY(&pad->digital_right_x, &pad->digital_right_y, pad->deadzone);

        // Keep analog values consistent after deadzone modification.
        pad->analog_left_x = pad->digital_left_x + ANALOG_CENTER;
        pad->analog_left_y = ANALOG_CENTER - pad->digital_left_y;

        pad->analog_right_x = pad->digital_right_x + ANALOG_CENTER;
        pad->analog_right_y = ANALOG_CENTER - pad->digital_right_y;
    }

    pad->left_x_normal = (f32)pad->digital_left_x;
    pad->left_y_normal = (f32)pad->digital_left_y;

    pad->left_x_normal /= 128.0f;
    pad->left_y_normal /= 128.0f;

    if (pad->left_x_normal > 0.0f) {
        pad->left_x_normal = MAX(0.0f, pad->left_x_normal - pad->left_x_unknown2);
    }

    if (pad->left_x_normal < 0.0f) {
        pad->left_x_normal = MIN(0.0f, pad->left_x_normal + pad->left_x_unknown2);
    }

    if (pad->left_y_normal > 0.0f) {
        pad->left_y_normal = MAX(0.0f, pad->left_y_normal - pad->left_y_unknown2);
    }

    if (pad->left_y_normal < 0.0f) {
        pad->left_y_normal = MIN(0.0f, pad->left_y_normal + pad->left_y_unknown2);
    }

    pad->right_x_normal = (f32)pad->digital_right_x;
    pad->right_y_normal = (f32)pad->digital_right_y;

    pad->right_x_normal /= 128.0f;
    pad->right_y_normal /= 128.0f;

    if (pad->right_x_normal > 0.0f) {
        pad->right_x_normal = MAX(0.0f, pad->right_x_normal - pad->right_x_unknown2);
    }

    if (pad->right_x_normal < 0.0f) {
        pad->right_x_normal = MIN(0.0f, pad->right_x_normal + pad->right_x_unknown2);
    }

    if (pad->right_y_normal > 0.0f) {
        pad->right_y_normal = MAX(0.0f, pad->right_y_normal - pad->right_y_unknown2);
    }

    if (pad->right_y_normal < 0.0f) {
        pad->right_y_normal = MIN(0.0f, pad->right_y_normal + pad->right_y_unknown2);
    }

    if ((pad->analog_button_flags & 1) != 0) {
        // The right-hand side values never appear to be written. This may be a
        // platform difference.
        pad->analog_left_pad_left = pad->unknown_7c;
        pad->analog_left_pad_right = pad->unknown_7d;
        pad->analog_left_pad_up = pad->unknown_7e;
        pad->analog_left_pad_down = pad->unknown_7f;
    } else {
        ramp_rate_normal = FramePadRampRatePc / 30.0f;

        if ((pad->digital_buttons & PAD_BUTTON_UP) != 0) {
            pad->left_up_ramp = MIN(1.0f, pad->left_up_ramp + ramp_rate_normal);
        } else {
            pad->left_up_ramp = 0.0f;
        }

        if ((pad->digital_buttons & PAD_BUTTON_DOWN) != 0) {
            pad->left_down_ramp = MIN(1.0f, pad->left_down_ramp + ramp_rate_normal);
        } else {
            pad->left_down_ramp = 0.0f;
        }

        if ((pad->digital_buttons & PAD_BUTTON_LEFT) != 0) {
            pad->left_left_ramp = MIN(1.0f, pad->left_left_ramp + ramp_rate_normal);
        } else {
            pad->left_left_ramp = 0.0f;
        }

        if ((pad->digital_buttons & PAD_BUTTON_RIGHT) != 0) {
            pad->left_right_ramp = MIN(1.0f, pad->left_right_ramp + ramp_rate_normal);
        } else {
            pad->left_right_ramp = 0.0f;
        }

        pad->analog_left_pad_left = CalcRamp(pad->left_left_ramp) * 255.0f;
        pad->analog_left_pad_right = CalcRamp(pad->left_right_ramp) * 255.0f;
        pad->analog_left_pad_up = CalcRamp(pad->left_up_ramp) * 255.0f;
        pad->analog_left_pad_down = CalcRamp(pad->left_down_ramp) * 255.0f;
    }

    if ((pad->analog_button_flags & 2) != 0) {
        pad->unknown_82 = pad->unknown_80;
        pad->unknown_83 = pad->unknown_81;
    } else {
        pad->unknown_82 = 0x7f;
        pad->unknown_83 = 0x7f;
    }
}

void NuPadRecordPlay(NUGENERICPAD *pad) {
    FRAME_RECORD_CURSOR_PC *record_ptr;

    if (pad->is_valid && nuapi.pad_record.mode == NUPAD_PLAY &&
        (pad->digital_buttons & nuapi.pad_record.end_play_buttons) != 0) {
        nuapi.pad_record.mode = NUPAD_NORM;
    }

    if (pad->is_valid && nuapi.pad_record.mode == NUPAD_RECORD &&
        (pad->digital_buttons & nuapi.pad_record.end_record_buttons) != 0) {
        nuapi.pad_record.should_end = 1;
    }

    record_ptr = &nuapi.pad_record.record;

    switch (nuapi.pad_record.mode) {
        case NUPAD_RECORD:
            *record_ptr->u32_ptr++ = pad->is_valid;

            if (pad->is_valid) {
                *record_ptr->u8_ptr++ = pad->analog_left_x;
                *record_ptr->u8_ptr++ = pad->analog_left_y;
                *record_ptr->u8_ptr++ = pad->analog_right_x;
                *record_ptr->u8_ptr++ = pad->analog_right_y;

                *record_ptr->u8_ptr++ = pad->analog_l1;
                *record_ptr->u8_ptr++ = pad->analog_l2;
                *record_ptr->u8_ptr++ = pad->analog_r1;
                *record_ptr->u8_ptr++ = pad->analog_r2;

                *record_ptr->u32_ptr++ = pad->digital_buttons;
            }
            break;
        case NUPAD_PLAY:
            pad->is_valid = *record_ptr->u32_ptr++;

            if (pad->is_valid) {
                pad->analog_left_x = *record_ptr->u8_ptr++;
                pad->analog_left_y = *record_ptr->u8_ptr++;
                pad->analog_right_x = *record_ptr->u8_ptr++;
                pad->analog_right_y = *record_ptr->u8_ptr++;

                pad->analog_l1 = *record_ptr->u8_ptr++;
                pad->analog_l2 = *record_ptr->u8_ptr++;
                pad->analog_r1 = *record_ptr->u8_ptr++;
                pad->analog_r2 = *record_ptr->u8_ptr++;

                pad->digital_buttons = *record_ptr->u32_ptr++;
            }
            break;
        default:
            break;
    }
}

void NuPadRecordEndFrame() {
    FRAME_RECORD_CURSOR_PC *cursor;
    if (nuapi.pad_record.is_recording) {
        cursor = (FRAME_RECORD_CURSOR_PC *)&nuapi.pad_record.buf_start;
        nuapi.pad_record.is_recording = 0;
    }
    cursor = &nuapi.pad_record.record;
    switch (nuapi.pad_record.mode) {
        case NUPAD_PLAY:
            if (cursor->u8_ptr == nuapi.pad_record.buf_end) {
                nuapi.pad_record.mode = NUPAD_NORM;
            } else {
                nuapi.frame_time = *cursor->f32_ptr++;
            }
            break;
        case NUPAD_RECORD:
            *cursor->f32_ptr++ = nuapi.frame_time;
            if (cursor->u8_ptr > nuapi.pad_record.buf_end - 0x1840) {
                nuapi.pad_record.mode = NUPAD_NORM;
            }
            break;
        default:
            break;
    }
}

void NuPadResetState(i32 preserve_scanned_state) {
    FramePadActivatedPc = 0;
    FramePadProfilePortPc = -1;
    TextInputDeviceLinksPc[0].device = -1;
    TextInputDeviceLinksPc[1].device = -1;

    if (preserve_scanned_state == 0) {
        i32 i;
        for (i = 0; i <= 3; i++) {
            g_nupadScannedPads[i].digital_buttons_pressed = 0;
            g_nupadScannedPads[i].digital_buttons_prev = g_nupadScannedPads[i].digital_buttons_pressed;
            g_nupadScannedPads[i].digital_buttons = g_nupadScannedPads[i].digital_buttons_prev;
        }
    }
}

void NuPadSetStatus(i32 pad, i32 status) {
    i32 device = TextInputDeviceLinksPc[pad].device;
    if (device >= 0) {
        if (status != 0)
            g_nupadScannedPads[device].mapped_to_pad = pad;
        else {
            TextInputDeviceLinksPc[pad].available = status;
            g_nupadScannedPads[device].mapped_to_pad = -1;
            return;
        }
    }
    if (status == 1) {
        if (FramePadActivatedPc == 0)
            FramePadProfilePortPc = device;
        FramePadActivatedPc = 1;
    }
    TextInputDeviceLinksPc[pad].available = status;
}
i32 NuPadRead(FRAME_PAD_PC *pad) {
    i32 device;

    if (TextInputDeviceLinksPc[pad->device].device != -1 ||
        (FramePadDirectMappingPc && g_nupadScannedPads[pad->device].is_valid)) {
        if (FramePadDirectMappingPc != 0) {
            device = pad->device;
        } else {
            device = TextInputDeviceLinksPc[pad->device].device;
        }

        pad->analog_left_x = g_nupadScannedPads[device].analog_left_x;
        pad->analog_left_y = g_nupadScannedPads[device].analog_left_y;
        pad->analog_right_x = g_nupadScannedPads[device].analog_right_x;
        pad->analog_right_y = g_nupadScannedPads[device].analog_right_y;

        pad->analog_l1 = g_nupadScannedPads[device].analog_l1;
        pad->analog_l2 = g_nupadScannedPads[device].analog_l2;
        pad->analog_r1 = g_nupadScannedPads[device].analog_r1;
        pad->analog_r2 = g_nupadScannedPads[device].analog_r2;

        pad->digital_buttons = g_nupadScannedPads[device].digital_buttons;
        pad->digital_buttons_pressed = g_nupadScannedPads[device].digital_buttons_pressed;
        pad->digital_buttons_released = g_nupadScannedPads[device].digital_buttons_released;
        pad->digital_buttons_prev = g_nupadScannedPads[device].digital_buttons_prev;

        pad->has_analog_buttons = g_nupadScannedPads[device].analog_button_flags;

        pad->deadzone = g_nupadScannedPads[device].deadzone;

        pad->digital_left_x = g_nupadScannedPads[device].digital_left_x;
        pad->digital_left_y = g_nupadScannedPads[device].digital_left_y;
        pad->digital_right_x = g_nupadScannedPads[device].digital_right_x;
        pad->digital_right_y = g_nupadScannedPads[device].digital_right_y;

        pad->left_x_normal = g_nupadScannedPads[device].left_x_normal;
        pad->left_y_normal = g_nupadScannedPads[device].left_y_normal;
        pad->left_x_unknown = g_nupadScannedPads[device].left_x_unknown;
        pad->left_y_unknown = g_nupadScannedPads[device].left_y_unknown;
        pad->left_x_unknown2 = g_nupadScannedPads[device].left_x_unknown2;
        pad->left_y_unknown2 = g_nupadScannedPads[device].left_y_unknown2;

        pad->right_x_unknown2 = g_nupadScannedPads[device].right_x_unknown2;
        pad->right_y_unknown2 = g_nupadScannedPads[device].right_y_unknown2;
        pad->right_x_normal = g_nupadScannedPads[device].right_x_normal;
        pad->right_y_normal = g_nupadScannedPads[device].right_y_normal;
        pad->right_x_unknown = g_nupadScannedPads[device].right_x_unknown;
        pad->right_y_unknown = g_nupadScannedPads[device].right_y_unknown;

        pad->analog_left_pad_left = g_nupadScannedPads[device].analog_left_pad_left;
        pad->analog_left_pad_right = g_nupadScannedPads[device].analog_left_pad_right;
        pad->analog_left_pad_up = g_nupadScannedPads[device].analog_left_pad_up;
        pad->analog_left_pad_down = g_nupadScannedPads[device].analog_left_pad_down;

        pad->unknown_b0 = g_nupadScannedPads[device].unknown_82;
        pad->unknown_b1 = g_nupadScannedPads[device].unknown_83;

        pad->is_valid = g_nupadScannedPads[device].is_valid;

        return pad->is_valid;
    }

    pad->is_valid = 0;
    pad->digital_buttons = pad->digital_buttons_pressed = pad->digital_buttons_released = pad->digital_buttons_prev = 0;

    return 0;
}

void NuPadInit(void) {
    i32 i;
    memset(g_nupadScannedPads, 0, sizeof(g_nupadScannedPads));
    for (i = 0; i < 16; ++i) {
        g_nupadScannedPads[i].mapped_to_pad = -1;
        NuPadInitPS(&g_nupadScannedPads[i]);
    }
    for (i = 0; i < 3; ++i) {
        TextInputDeviceLinksPc[i].unknown_0000 = -1;
        TextInputDeviceLinksPc[i].device = -1;
        TextInputDeviceLinksPc[i].available = 0;
    }
    FramePadActivatedPc = 0;
}
void NuPadSetMaxGamePads(i32 count) {
    FramePadMaxGamePadsPc = count;
}

extern void NuPadOpenPS(FRAME_PAD_PC *pad);
FRAME_PAD_PC *NuPadOpen(i32 device, i32 slot) {
    FRAME_PAD_PC *pad = (FRAME_PAD_PC *)NuAlloc(sizeof(FRAME_PAD_PC) + 0x800,
                                                "d:\\projects\\game_saga\\nu2api.saga\\nucore\\nupad.cpp", 180);
    if (pad != NULL) {
        memset(pad, 0, sizeof(*pad));
        pad->device = device;
        pad->slot = slot;
        pad->stage = 0;
        pad->analog_right_x = pad->analog_right_y = pad->analog_left_x = pad->analog_left_y = 128;
    }
    NuPadOpenPS(pad);
    return pad;
}

void __cdecl NuPadSetValid(i32 port, i32 valid) {
    g_nupadScannedPads[port].is_valid = (u8)valid;
}

void __cdecl NuPadMapPlayerToPort(i32 player, i32 port) {
    i32 previous;
    if (port < 0) {
        previous = TextInputDeviceLinksPc[player].device;
        if (previous >= 0)
            g_nupadScannedPads[previous].mapped_to_pad = -1;
        TextInputDeviceLinksPc[player].available = 0;
        return;
    }
    NuKeyboardClearActiveDevicesPc();
    previous = TextInputDeviceLinksPc[player].device;
    if (previous >= 0)
        g_nupadScannedPads[previous].mapped_to_pad = -1;
    TextInputDeviceLinksPc[player].available = 0;
    if (previous >= 0)
        g_nupadScannedPads[previous].mapped_to_pad = -1;
    g_nupadScannedPads[port].digital_buttons_pressed = 0;
    TextInputDeviceLinksPc[player].device = port;
    g_nupadScannedPads[port].mapped_to_pad = player;
    if (FramePadActivatedPc == 0)
        FramePadProfilePortPc = port;
    PlayerLastPort[player] = (i16)port;
    FramePadActivatedPc = 1;
    TextInputDeviceLinksPc[player].available = 1;
    NuKeyboardApplyActionKeys_candidate(PlayerLastPort, 8);
}
