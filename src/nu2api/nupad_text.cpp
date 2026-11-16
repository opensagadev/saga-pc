#include "nu2api/PCInput.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nupad.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include <stdio.h>

#pragma pack(push, 1)

struct TEXT_INPUT_BINDING { // size 0x4
    u8 type;                // 0x0
    u8 unknown_0001;        // 0x1
    i16 value;              // 0x2
};

struct TEXT_INPUT_DEVICE {           // size 0x17c
    u8 unknown_0000[0x6e];           // 0x000
    TEXT_INPUT_BINDING bindings[32]; // 0x06e
    u8 unknown_00ee[0x8e];           // 0x0ee
};

struct TEXT_INPUT_DEVICE_KIND { // size 0x6
    i8 kind;                    // 0x0
    u8 unknown_0001[5];         // 0x1
};

#pragma pack(pop)

class TEXT_INPUT_MANAGER { // size 0x68b0
  public:
    u8 unknown_0000[0x4e34];                   // 0x0000
    TEXT_INPUT_DEVICE devices[14];             // 0x4e34
    u8 unknown_62fc[0x58];                     // 0x62fc
    i32 custom_active;                         // 0x6354
    TEXT_INPUT_BINDING custom_bindings[4][32]; // 0x6358
    u8 unknown_6558[0x288];                    // 0x6558
    u8 use_keyboard_override;                  // 0x67e0
    u8 unknown_67e1[0x0f];                     // 0x67e1
    TEXT_INPUT_DEVICE_KIND device_kind[32];    // 0x67f0
    i32 ButtonName(i32 device, i32 button, char *destination, i32 custom, i32 custom_index, i32 use_callback, i32 alternate_labels,
                   char *delimiters);
    i32 fun_006d9dc0(i32 device, i32 button, char *destination, i32 custom, i32 custom_index);
};

extern "C" {
    i32 PcInput_GetCtrlStringPlayer(void) {
        return TextInputLastDeviceIndex;
    }
}

extern "C" {
    u8 TextInputKeyboardOverride[24] = {0, 0, 0, 0, 0, 0, 0, 0, 200, 205, 208, 203, 60, 1, 28, 59, 0, 0, 0, 0, 0, 0, 0, 0};
    i8 TextInputAxisLabelIndex[16] = {14, 15, 16, 17, 0, 0, 18, 19, 20, 21, 0, 0, 28, 28, 29, 29};
    i8 TextInputButtonLabelIndex[11] = {24, 23, 25, 22, 26, 27, 33, 32, 30, 31, 0};
}

i32 TEXT_INPUT_MANAGER::ButtonName(i32 device, i32 button, char *destination, i32 custom, i32 custom_index, i32 use_callback,
                                   i32 alternate_labels, char *delimiters) {
    i32 result = 0;
    i32 open = 0;
    i32 close = 0;
    if (button >= 256) {
        if (TextInputNameCallback != NULL) {
            TextInputNameCallback(button, destination);
        } else {
            NuStrCpy(destination, "GFX?");
        }
        return 1;
    }
    TEXT_INPUT_LABELS *labels = TextInputDefaultLabels;
    if (alternate_labels != 0) {
        labels = TextInputAlternateLabels;
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
        TextInputKeyboardOverride[button] > 0) {
        type = PC_BINDING_KEY;
        value = TextInputKeyboardOverride[button];
    }
    i32 expected = 2;
    switch (type) {
        case PC_BINDING_JOYSTICK_BUTTON: {
            char text[5];
            *destination = 0;
            if (simple != 0) {
                if (TextInputNameCallback != NULL && use_callback != 0) {
                    TextInputNameCallback(value, text);
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
                    NuStrCpy(destination, labels->indexed_names[TextInputButtonLabelIndex[value]]);
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
        case PC_BINDING_JOYSTICK_AXIS_REVERSED:
            expected = 1;
        case PC_BINDING_JOYSTICK_AXIS: {
            if (value < 0) {
                break;
            }
            TEXT_INPUT_DEVICE_KIND *kind = &device_kind[button];
            if (kind->kind == 3 && type == PC_BINDING_JOYSTICK_AXIS) {
                expected = 3;
            }
            if (simple != 0 && value < 8) {
                char text[5] = "";
                value = value * 2 + (kind->kind == expected);
                *destination = 0;
                if (TextInputNameCallback != NULL) {
                    if (value == 13 || value == 14) {
                        TextInputNameCallback(11, text);
                    } else if (value == 15 || value == 12) {
                        TextInputNameCallback(10, text);
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
                NuStrCpy(destination, labels->indexed_names[TextInputAxisLabelIndex[value]]);
            } else if (value < 6) {
                if (kind->kind == expected) {
                    sprintf(destination, "%s %s", labels->axis_names[value], labels->axis_positive);
                } else {
                    sprintf(destination, "%s %s", labels->axis_names[value], labels->axis_negative);
                }
            } else {
                i32 reversed = NuLanguageGet() == NULANGUAGE_POLISH;
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
        case PC_BINDING_MOUSE_BUTTON:
            if ((u32)value <= 2) {
                NuStrCpy(destination, labels->hat_names[value]);
            } else {
                sprintf(destination, labels->hat_format, value + 1);
            }
            break;
        case PC_BINDING_MOUSE_AXIS_REVERSED:
            if ((u32)value <= 2) {
                if (device_kind[button].kind == 2 || device_kind[button].kind == 3) {
                    NuStrCpy(destination, labels->direction_positive[value]);
                } else {
                    NuStrCpy(destination, labels->direction_negative[value]);
                }
            }
            break;
        case PC_BINDING_MOUSE_AXIS:
            if ((u32)value <= 2) {
                if (device_kind[button].kind == 2 || device_kind[button].kind == 3) {
                    NuStrCpy(destination, labels->direction_negative[value]);
                } else {
                    NuStrCpy(destination, labels->direction_positive[value]);
                }
            }
            break;
        case PC_BINDING_KEY:
        case 8:
        case 9:
            if (value >= 1) {
                NuStrCpy(destination, TextInputKeyNames[value]);
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

extern "C" i32 PcInput_GetCtrlInputString(i32 device, i32 button, char *destination) {
    TEXT_INPUT_MANAGER *manager = (TEXT_INPUT_MANAGER *)nu_input_state;
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
    void TextInputSetKeyboardOverride(i32 mode) {
        TEXT_INPUT_MANAGER *manager = reinterpret_cast<TEXT_INPUT_MANAGER *>(nu_input_state);
        i32 previous = (i8)manager->use_keyboard_override;
        manager->use_keyboard_override = (u8)mode;
        if (mode != previous) {
            nu_keyboard_repeat_countdown = 6;
        }
    }

    void PcInput_SetCtrlStringPlayer(i32 device) {
        TextInputLastDeviceIndex = (i8)device;
    }

    i32 PcInput_PortIsKeyboard(i32 device) {
        return (u32)device - 10u <= 3u;
    }
}

extern "C" {
    char **PcInput_GetKeyNameTable(void) {
        return TextInputKeyNames;
    }

    i32 PcInput_PortIsUC(i32 device) {
        return (u32)device <= 3u;
    }
}

extern "C" i32 AreaGetKeyboardOverride(void) {
    return (i8)((TEXT_INPUT_MANAGER *)nu_input_state)->use_keyboard_override;
}

extern "C" f32 fun_006d6360(f32 period, f32 time) {
    time /= period;
    return 1.0f - 1.0f / NuPow(2.0f, time);
}

extern "C" void fun_006d63a0(i32 *value, i32 target, f32 rate) {
    i32 current = *value;
    f32 step = (target - current) * rate;
    step += current;
    *value = step;
}

extern "C" void fun_006d63e0(f32 *value, f32 target, f32 rate) {
    target -= *value;
    target *= rate;
    *value += target;
}

extern "C" i32 fun_006d94b0(i32 player, i32 button, char *destination, i32, i32, i32 use_callback, i32 alternate_labels, char *delimiters) {
    TEXT_INPUT_MANAGER *manager = (TEXT_INPUT_MANAGER *)nu_input_state;
    i32 port = NuPadGetPort(player);
    return manager->ButtonName(port, button, destination, 0, 0, use_callback, alternate_labels, delimiters);
}

extern "C" i32 PcInputBindingButtonName(i32 action, i32 port, char *destination, i32 auxiliary, i32 bank, i32 a, i32 b, char *c) {
    return ((TEXT_INPUT_MANAGER *)nu_input_state)->ButtonName(action, port, destination, auxiliary, bank, a, b, c);
}

i32 TEXT_INPUT_MANAGER::fun_006d9dc0(i32 device, i32 button, char *destination, i32 custom, i32 custom_index) {
    return ButtonName(device, button, destination, custom, custom_index, 1, 1, "()");
}

/* Append to nupad_text.cpp. Native6da3d0,51 bytes all stack cdecl. */
extern "C" char TextInputActionLabelBuffer[128]; /*2975970..29759ef*/

extern "C" {
    char TextInputActionLabelBuffer[128] = {0};
}

extern "C" char *TextInputActionLabel(i32 device, i32 action, i32 custom, i32 custom_index) {
    ((TEXT_INPUT_MANAGER *)nu_input_state)->ButtonName(device, action, TextInputActionLabelBuffer, custom, custom_index, 1, 1, "()");
    return TextInputActionLabelBuffer;
}

extern "C" void fun_006da460(i32 player) {
    PC_INPUT_RUNTIME *runtime = (PC_INPUT_RUNTIME *)nu_input_state;
    PC_INPUT_CONFIG config;
    for (i32 port = 0; port < 14; ++port) {
        i32 owner = NuPadGetPlayer(port);
        if (runtime->devices[port].kind != PC_INPUT_DEVICE_NONE && (owner == -1 || owner == player)) {
            PC_INPUT_CONFIG *found = runtime->FindConfig(3, port, player);
            if (found == NULL) {
                found = runtime->FindConfig(3, port, -1);
                if (found == NULL) {
                    continue;
                }
                runtime->InstallConfig(found, port, player, 1, player);
                runtime->ExportConfig(port, &config, 1, player, player);
                runtime->PromoteConfig(runtime->primary, 64, &config);
                runtime->SaveConfiguration();
                runtime->InstallConfig(&config, port, player, 0, -1);
            }
            NuPadMapPlayerToPort(player, port);
            NuPadSetValid(port, 1);
            return;
        }
    }
}
