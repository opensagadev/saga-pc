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
    if (button >= 256) {
        if (TextInputNameCallbackPc != NULL) {
            TextInputNameCallbackPc(button, destination);
        } else {
            NuStrCpy(destination, "GFX?");
        }
        return 1;
    }
    TEXT_INPUT_LABELS_PC *labels = alternate_labels != 0 ? TextInputAlternateLabelsPc : TextInputDefaultLabelsPc;
    i32 simple = device < 4;
    i32 open = 0;
    i32 close = 0;
    if (delimiters != NULL && delimiters[0] != 0 && delimiters[1] != 0) {
        open = (i8)delimiters[0];
        close = (i8)delimiters[1];
        if (open != 0) {
            *destination++ = (char)open;
        }
    }
    TEXT_INPUT_BINDING_PC *binding =
        custom != 0 ? &custom_bindings[custom_index][button] : &devices[device].bindings[button];
    i32 type = binding->type;
    i32 value = binding->value;
    if (use_keyboard_override != 0 && custom_active == 0 && ((u32)(device - 10) <= 3 || device < 0) &&
        TextInputKeyboardOverridePc[button] > 0) {
        type = 7;
        value = TextInputKeyboardOverridePc[button];
    }
    char text[5];
    i32 expected = 2;
    i32 kind;
    switch (type) {
        case 1:
            *destination = 0;
            if (simple != 0 && TextInputNameCallbackPc != NULL && use_callback != 0) {
                text[0] = 0;
                text[1] = text[2] = text[3] = text[4] = 0;
                TextInputNameCallbackPc(value, text);
                if (text[0] != 0) {
                    if (open != 0) {
                        --destination;
                    }
                    NuStrCpy(destination, text);
                    return 1;
                }
            }
            if (simple != 0 && value <= 10) {
                NuStrCpy(destination, labels->indexed_names[TextInputButtonLabelIndexPc[value]]);
            } else if ((u32)(value - 128) <= 3) {
                NuStrCpy(destination, labels->indexed_names[value - 128]);
            } else {
                sprintf(destination, "%s %d", labels->button_prefix, value + 1);
            }
            break;
        case 3:
            expected = 1;
        case 2:
            if (value < 0) {
                break;
            }
            kind = device_kind[button].kind;
            if (kind == 3 && type == 2) {
                expected = 3;
            }
            if (simple != 0 && value < 8) {
                i32 index = value * 2 + (kind == expected);
                text[0] = 0;
                text[1] = text[2] = text[3] = text[4] = 0;
                *destination = 0;
                if (TextInputNameCallbackPc != NULL) {
                    if (index == 12 || index == 15) {
                        TextInputNameCallbackPc(10, text);
                    } else if (index == 13 || index == 14) {
                        TextInputNameCallbackPc(11, text);
                    }
                    if (text[0] != 0) {
                        if (open != 0) {
                            --destination;
                        }
                        NuStrCpy(destination, text);
                        return 1;
                    }
                }
                NuStrCpy(destination, labels->indexed_names[TextInputAxisLabelIndexPc[index]]);
            } else if (value < 6) {
                sprintf(destination, "%s %s", labels->axis_names[value],
                        kind == expected ? labels->axis_positive : labels->axis_negative);
            } else if (NuLanguageGet() != 11) {
                sprintf(destination, labels->axis_format, value,
                        kind == expected ? labels->axis_positive : labels->axis_negative);
            } else {
                sprintf(destination, labels->axis_format,
                        kind == expected ? labels->axis_positive : labels->axis_negative, value);
            }
            break;
        case 4:
            if ((u32)value <= 2) {
                NuStrCpy(destination, labels->hat_names[value]);
            } else {
                sprintf(destination, labels->hat_format, value + 1);
            }
            break;
        case 5:
            if ((u32)value <= 2) {
                NuStrCpy(destination, device_kind[button].kind == 2 || device_kind[button].kind == 3
                                          ? labels->direction_negative[value]
                                          : labels->direction_positive[value]);
            }
            break;
        case 6:
            if ((u32)value <= 2) {
                NuStrCpy(destination, device_kind[button].kind == 2 || device_kind[button].kind == 3
                                          ? labels->direction_positive[value]
                                          : labels->direction_negative[value]);
            }
            break;
        case 7:
        case 8:
        case 9:
            if (value >= 1) {
                NuStrCpy(destination, TextInputKeyNamesPc[value]);
            }
            break;
        case 12:
            NuStrCpy(destination,
                     device_kind[button].kind == 1 ? labels->direction_positive[0] : labels->direction_negative[0]);
            break;
        case 13:
            NuStrCpy(destination,
                     device_kind[button].kind == 1 ? labels->direction_positive[1] : labels->direction_negative[1]);
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
    if (device < 0) {
        device = 0;
    }
    i32 linked_device = TextInputLinkedDevicePc(device);
    if (button >= 0) {
        return ((TEXT_INPUT_MANAGER_PC *)nu_input_state)
            ->ButtonName(linked_device, button, destination, 0, 0, 1, 1, "()");
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
