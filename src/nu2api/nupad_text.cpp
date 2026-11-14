#include "saga.h"
#include <stdio.h>

#pragma pack(push, 1)

struct TEXT_INPUT_BINDING_PC {
    u8 type;         // 0x0
    u8 unknown_0001; // 0x1
    i16 value;       // 0x2
}; // size 0x4

struct TEXT_INPUT_DEVICE_PC {
    u8 unknown_0000[0x6e];              // 0x000
    TEXT_INPUT_BINDING_PC bindings[32]; // 0x06e
    u8 unknown_00ee[0x8e];              // 0x0ee
}; // size 0x17c

struct TEXT_INPUT_DEVICE_KIND_PC {
    i8 kind;            // 0x0
    u8 unknown_0001[5]; // 0x1
}; // size 0x6

#pragma pack(pop)

class TEXT_INPUT_MANAGER_PC {
  public:
    u8 unknown_0000[0x4e34];                      // 0x0000
    TEXT_INPUT_DEVICE_PC devices[14];             // 0x4e34
    u8 unknown_62fc[0x58];                        // 0x62fc
    i32 custom_active;                            // 0x6354
    TEXT_INPUT_BINDING_PC custom_bindings[4][32]; // 0x6358
    u8 unknown_6558[0x288];                       // 0x6558
    u8 use_keyboard_override;                     // 0x67e0
    u8 unknown_67e1[0x0f];                        // 0x67e1
    TEXT_INPUT_DEVICE_KIND_PC device_kind[32];    // 0x67f0
    i32 __thiscall ButtonName(i32 device, i32 button, char *destination, i32 custom, i32 custom_index, i32 use_callback,
                              i32 alternate_labels, char *delimiters);
}; // size 0x68b0

extern "C" {
    extern void(__cdecl *TextInputNameCallbackPc)(i32, char *);

    i32 __cdecl PcInput_GetCtrlStringPlayer(void) {
        return TextInputLastDeviceIndexPc;
    }
}

i32 __thiscall TEXT_INPUT_MANAGER_PC::ButtonName(i32 device, i32 button, char *destination, i32 custom,
                                                 i32 custom_index, i32 use_callback, i32 alternate_labels,
                                                 char *delimiters) {
    i32 result = 0;
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
        open = delimiters[0];
        close = delimiters[1];
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
                        result = 1;
                        break;
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
                        result = 1;
                        break;
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
    return result;
}

extern "C" i32 __cdecl PcInput_GetCtrlInputString(i32 device, i32 button, char *destination) {
    TEXT_INPUT_MANAGER_PC *manager = (TEXT_INPUT_MANAGER_PC *)nu_input_state;
    if (device < 0) {
        device = 0;
    }
    i32 linked_device = NuPadGetPort(device);
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
        if (mode != previous) {
            nu_keyboard_repeat_countdown = 6;
        }
    }

    void __cdecl PcInput_SetCtrlStringPlayer(i32 device) {
        TextInputLastDeviceIndexPc = (i8)device;
    }

    i32 __cdecl PcInput_PortIsKeyboard(i32 device) {
        return (u32)device - 10u <= 3u;
    }
}

extern "C" {
    char **__cdecl PcInput_GetKeyNameTable(void) {
        return TextInputKeyNamesPc;
    }

    i32 __cdecl PcInput_PortIsUC(i32 device) {
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
