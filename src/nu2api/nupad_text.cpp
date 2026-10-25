#include "saga.h"
#include <stdio.h>

#pragma pack(push, 1)
struct TEXT_INPUT_BINDING_PC {
    u8 type;
    u8 unknown_0001;
    i16 value;
};
struct TEXT_INPUT_DEVICE_PC {
    u8 unknown_0000[0x6e];
    TEXT_INPUT_BINDING_PC bindings[32];
    u8 unknown_00ee[0x8e];
};
struct TEXT_INPUT_DEVICE_KIND_PC {
    i8 kind;
    u8 unknown_0001[5];
};
#pragma pack(pop)
STATIC_ASSERT(sizeof(TEXT_INPUT_BINDING_PC) == 4);
STATIC_ASSERT(offsetof(TEXT_INPUT_BINDING_PC, value) == 2);
STATIC_ASSERT(sizeof(TEXT_INPUT_DEVICE_PC) == 0x17c);
STATIC_ASSERT(offsetof(TEXT_INPUT_DEVICE_PC, bindings) == 0x6e);
STATIC_ASSERT(sizeof(TEXT_INPUT_DEVICE_KIND_PC) == 6);

class TEXT_INPUT_MANAGER_PC {
  public:
    u8 unknown_0000[0x4e34];
    TEXT_INPUT_DEVICE_PC devices[14];
    u8 unknown_62fc[0x58];
    i32 custom_active;
    TEXT_INPUT_BINDING_PC custom_bindings[4][32];
    u8 unknown_6558[0x288];
    u8 use_keyboard_override;
    u8 unknown_67e1[0x0f];
    TEXT_INPUT_DEVICE_KIND_PC device_kind[32];
    i32 __thiscall ButtonName(i32 device, i32 button, char *destination, i32 custom, i32 custom_index, i32 use_callback,
                              i32 alternate_labels, char *delimiters);
};
STATIC_ASSERT(offsetof(TEXT_INPUT_MANAGER_PC, devices) == 0x4e34);
STATIC_ASSERT(offsetof(TEXT_INPUT_MANAGER_PC, custom_active) == 0x6354);
STATIC_ASSERT(offsetof(TEXT_INPUT_MANAGER_PC, custom_bindings) == 0x6358);
STATIC_ASSERT(offsetof(TEXT_INPUT_MANAGER_PC, use_keyboard_override) == 0x67e0);
STATIC_ASSERT(offsetof(TEXT_INPUT_MANAGER_PC, device_kind) == 0x67f0);

extern "C" {
    extern i8 TextInputLastDeviceIndexPc;
    extern TEXT_INPUT_LABELS_PC *TextInputDefaultLabelsPc;
    extern TEXT_INPUT_LABELS_PC *TextInputAlternateLabelsPc;
    extern void(__cdecl *TextInputNameCallbackPc)(i32, char *);
    extern i8 TextInputButtonLabelIndexPc[11];
    extern i8 TextInputAxisLabelIndexPc[16];
    extern u8 TextInputKeyboardOverridePc[32];
    extern char **TextInputKeyNamesPc;
    extern TEXT_INPUT_DEVICE_LINK_PC TextInputDeviceLinksPc[3];
    i32 __cdecl TextInputLastDevicePc(void) {
        return TextInputLastDeviceIndexPc;
    }
    i32 __cdecl TextInputLinkedDevicePc(i32 index) {
        return TextInputDeviceLinksPc[index].available != 0 ? TextInputDeviceLinksPc[index].device : -1;
    }
}

i32 __thiscall TEXT_INPUT_MANAGER_PC::ButtonName(i32 device, i32 button, char *destination, i32 custom,
                                                 i32 custom_index, i32 use_callback, i32 alternate_labels,
                                                 char *delimiters) {
    i32 open = 0;
    i32 close = 0;
    if (button >= 256) {
        if (TextInputNameCallbackPc != NULL) {
            TextInputNameCallbackPc(button, destination);
        } else {
            NuStrCpy(destination, "GFX?");
        }
        return 1;
    }
    TEXT_INPUT_LABELS_PC *labels = TextInputDefaultLabelsPc;
    if (alternate_labels != 0) {
        labels = TextInputAlternateLabelsPc;
    }
    i32 simple = device < 4;
    if (delimiters != NULL && delimiters[0] != 0 && delimiters[1] != 0) {
        open = (i8)delimiters[0];
        close = (i8)delimiters[1];
        if (open != 0) {
            *destination++ = (char)open;
        }
    }
    i32 type;
    i32 value;
    if (custom != 0) {
        type = custom_bindings[custom_index][button].type;
        value = custom_bindings[custom_index][button].value;
    } else {
        type = devices[device].bindings[button].type;
        value = devices[device].bindings[button].value;
    }
    if (use_keyboard_override != 0 && custom_active == 0 && ((u32)(device - 10) <= 3 || device < 0) &&
        TextInputKeyboardOverridePc[button] > 0) {
        type = 7;
        value = TextInputKeyboardOverridePc[button];
    }
    i32 expected = 2;
    switch (type) {
        case 1: {
            char text[5];
            *destination = 0;
            if (simple != 0) {
                if (TextInputNameCallbackPc != NULL && use_callback != 0) {
                    TextInputNameCallbackPc(value, text);
                    if (text[0] != 0) {
                        if (open != 0) {
                            --destination;
                        }
                        NuStrCpy(destination, text);
                        return 1;
                    }
                }
                if (value <= 10) {
                    NuStrCpy(destination, labels->indexed_names[TextInputButtonLabelIndexPc[value]]);
                    break;
                }
            }
            if ((u32)(value - 128) <= 3) {
                NuStrCpy(destination, labels->indexed_names[value - 128]);
            } else {
                sprintf(destination, "%s %d", labels->button_prefix, value + 1);
            }
            break;
        }
        case 3:
            expected = 1;
        case 2: {
            if (value < 0) {
                break;
            }
            TEXT_INPUT_DEVICE_KIND_PC *kind = &device_kind[button];
            if (kind->kind == 3 && type == 2) {
                expected = 3;
            }
            if (simple != 0 && value < 8) {
                char text[5] = "";
                value = value * 2 + (kind->kind == expected);
                *destination = 0;
                if (TextInputNameCallbackPc != NULL) {
                    if (value == 13 || value == 14) {
                        TextInputNameCallbackPc(11, text);
                    } else if (value == 15 || value == 12) {
                        TextInputNameCallbackPc(10, text);
                    }
                    if (text[0] != 0) {
                        if (open != 0) {
                            --destination;
                        }
                        NuStrCpy(destination, text);
                        return 1;
                    }
                }
                NuStrCpy(destination, labels->indexed_names[TextInputAxisLabelIndexPc[value]]);
            } else if (value < 6) {
                if (kind->kind == expected) {
                    sprintf(destination, "%s %s", labels->axis_names[value], labels->axis_positive);
                } else {
                    sprintf(destination, "%s %s", labels->axis_names[value], labels->axis_negative);
                }
            } else {
                i32 reversed = NuLanguageGet() == 11;
                if (kind->kind == expected) {
                    if (reversed == 0) {
                        sprintf(destination, labels->axis_format, value, labels->axis_positive);
                    } else {
                        sprintf(destination, labels->axis_format, labels->axis_positive, value);
                    }
                } else {
                    if (reversed == 0) {
                        sprintf(destination, labels->axis_format, value, labels->axis_negative);
                    } else {
                        sprintf(destination, labels->axis_format, labels->axis_negative, value);
                    }
                }
            }
            break;
        }
        case 12:
            if (device_kind[button].kind == 1) {
                NuStrCpy(destination, labels->direction_positive[0]);
            } else {
                NuStrCpy(destination, labels->direction_negative[0]);
            }
            break;
        case 13:
            if (device_kind[button].kind == 1) {
                NuStrCpy(destination, labels->direction_positive[1]);
            } else {
                NuStrCpy(destination, labels->direction_negative[1]);
            }
            break;
        case 4:
            if ((u32)value <= 2) {
                NuStrCpy(destination, labels->hat_names[value]);
            } else {
                sprintf(destination, labels->hat_format, value + 1);
            }
            break;
        case 6:
            if ((u32)value <= 2) {
                if (device_kind[button].kind == 2 || device_kind[button].kind == 3) {
                    NuStrCpy(destination, labels->direction_positive[value]);
                } else {
                    NuStrCpy(destination, labels->direction_negative[value]);
                }
            }
            break;
        case 5:
            if ((u32)value <= 2) {
                if (device_kind[button].kind == 2 || device_kind[button].kind == 3) {
                    NuStrCpy(destination, labels->direction_negative[value]);
                } else {
                    NuStrCpy(destination, labels->direction_positive[value]);
                }
            }
            break;
        case 7:
        case 8:
        case 9:
            if (value >= 1) {
                NuStrCpy(destination, TextInputKeyNamesPc[value]);
            }
            break;
        default:
            NuStrCpy(destination, labels->unknown);
            break;
    }
    if (close != 0) {
        i32 length = NuStrLen(destination);
        destination[length] = (char)close;
        destination[length + 1] = 0;
    }
    return 0;
}

extern "C" i32 __cdecl TextInputButtonNamePc(i32 device, i32 button, char *destination) {
    TEXT_INPUT_MANAGER_PC *manager = (TEXT_INPUT_MANAGER_PC *)nu_input_state;
    if (device < 0) {
        device = 0;
    }
    i32 linked_device = TextInputLinkedDevicePc(device);
    if (button >= 0) {
        return manager->ButtonName(linked_device, button, destination, 0, 0, 1, 1, "()");
    }
    *destination = 0;
    return 0;
}

extern "C" {
    void __cdecl TextInputSetKeyboardOverridePc(i32 mode) {
        TEXT_INPUT_MANAGER_PC *manager = reinterpret_cast<TEXT_INPUT_MANAGER_PC *>(nu_input_state);
        i32 previous = (i8)manager->use_keyboard_override;
        manager->use_keyboard_override = (u8)mode;
        if (mode != previous)
            nu_keyboard_repeat_countdown = 6;
    }
    void __cdecl TextInputSetLastDevicePc(i32 device) {
        TextInputLastDeviceIndexPc = (i8)device;
    }
    i32 __cdecl TextInputIsKeyboardDevicePc(i32 device) {
        return (u32)device - 10u <= 3u;
    }
}

extern "C" {
    char **__cdecl TextInputGetKeyNamesPc(void) {
        return TextInputKeyNamesPc;
    }
    i32 __cdecl TextInputIsPrimaryDevicePc(i32 device) {
        return (u32)device <= 3u;
    }
}

extern "C" i32 __cdecl AreaGetKeyboardOverridePc(void) {
    return (i8)((TEXT_INPUT_MANAGER_PC *)nu_input_state)->use_keyboard_override;
}

extern "C" i32 __cdecl PcInputBindingButtonNamePc(i32 action, i32 port, char *destination, i32 auxiliary, i32 bank,
                                                  i32 a, i32 b, char *c) {
    return ((TEXT_INPUT_MANAGER_PC *)nu_input_state)->ButtonName(action, port, destination, auxiliary, bank, a, b, c);
}

/* Append to nupad_text.cpp. Native6da3d0,51 bytes all stack cdecl. */
extern "C" char TextInputActionLabelBufferPc[128]; /*2975970..29759ef*/
extern "C" char *TextInputActionLabelPc(i32 device, i32 action, i32 custom, i32 custom_index) {
    ((TEXT_INPUT_MANAGER_PC *)nu_input_state)
        ->ButtonName(device, action, TextInputActionLabelBufferPc, custom, custom_index, 1, 1, "()");
    return TextInputActionLabelBufferPc;
}
