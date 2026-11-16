#include "gameapi/edtools/edui.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolts.h"
#include "nu2api/PCInput.h"
#include "nu2api/huffdecode.h"
#include "nu2api/nucore/numouse.h"
#include "nu2api/nucore/nupad.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nuwin32/d3dCore.h"
#include <string.h>

extern "C" {
    PC_DI_MOUSE_STATE PcMouseSamples[2] = {0};
    i32 nu_keyboard_device_flags = 0;
    i32 nu_keyboard_key_index = 0;
    i32 nu_keyboard_repeat_countdown = 0;
}
static i32 PcXInputDeviceCount; // XInput pads found by the current device enumeration
extern "C" {
    i32 nu_keyboard_previous_key_index = 0;
    i32 nu_keyboard_no_key_state = 0;
    i8 PcMouseSampleIndex = 0;
}
PC_DI_CAPS PcKeyboardCapabilities = {0};

PC_INPUT_CONFIG *PC_INPUT_RUNTIME::FindConfig(u8 mask, i32 port, i32 key) {
    if (port < 0) {
        return NULL;
    }
    if (mask & 4) {
        for (i32 i = 0; i < 64; ++i) {
            if (secondary[i].product_id == devices[port].product.Data1 && (key < 0 || secondary[i].key == key)) {
                return &secondary[i];
            }
        }
    }
    if (mask & 2) {
        for (i32 i = 0; i < 64; ++i) {
            if (primary[i].product_id == devices[port].product.Data1 && (key < 0 || primary[i].key == key)) {
                return &primary[i];
            }
        }
    }
    if (mask & 1) {
        for (u32 i = 0; i < sizeof(PcStaticInputConfigs) / sizeof(PcStaticInputConfigs[0]); ++i) {
            if (PcStaticInputConfigs[i].product_id == devices[port].product.Data1 && (key < 0 || PcStaticInputConfigs[i].key == key)) {
                return &PcStaticInputConfigs[i];
            }
        }
    }
    return NULL;
}

PC_INPUT_CONFIG *PC_INPUT_RUNTIME::fun_006d4500(u8 mask, const char *name) {
    if (mask & 4) {
        for (i32 index = 0; index < 64; ++index) {
            if (NuStrICmp(secondary[index].name, name) == 0) {
                return &secondary[index];
            }
        }
    }
    if (mask & 2) {
        for (i32 index = 0; index < 64; ++index) {
            if (NuStrICmp(primary[index].name, name) == 0) {
                return &primary[index];
            }
        }
    }
    if (mask & 1) {
        for (u32 index = 0; index < sizeof(PcStaticInputConfigs) / sizeof(PcStaticInputConfigs[0]); ++index) {
            if (NuStrICmp(PcStaticInputConfigs[index].name, name) == 0) {
                return &PcStaticInputConfigs[index];
            }
        }
    }
    return NULL;
}

void PC_INPUT_RUNTIME::fun_006d45e0(i32 key) {
    preferred_key = key;
}

void PC_INPUT_RUNTIME::InitXInputCaps(const PC_XINPUT_CAPABILITIES *caps, PC_INPUT_DEVICE *record) {
    record->pov_count = 0;
    record->button_count = 0;
    record->axis_count = 5;
    if (record->product.Data1 == 0x028e045e || record->product.Data1 == 0x0719045e) {
        record->button_count = 10;
        record->pov_count = 1;
        record->left_trigger = 0xffff;
        record->right_trigger = 0xffff;
    } else {
        record->left_trigger = caps->motor_left;
        record->right_trigger = caps->motor_right;
        for (i32 bit = 0; bit < 32; ++bit) {
            if (caps->gamepad.buttons & (1 << bit)) {
                ++record->button_count;
            }
        }
        if ((caps->gamepad.buttons & 15) == 15) {
            record->button_count -= 4;
            record->pov_count = 1;
        }
    }
}

void PC_INPUT_RUNTIME::BindPreferredConfig(i32 mask, i32 port) {
    PC_INPUT_CONFIG *config = FindConfig((u8)mask, port, preferred_key);
    if (!config) {
        config = FindConfig((u8)mask, port, -1);
    }
    if (config) {
        InstallConfig(config, port, config->key, 0, 0);
    }
}

void PC_INPUT_RUNTIME::InstallConfig(PC_INPUT_CONFIG *config, i32 port, i32 key, i32 auxiliary, i32 auxiliary_port) {
    PC_INPUT_BINDING *bindings;
    if (auxiliary) {
        auxiliary_ports[auxiliary_port] = port;
        bindings = auxiliary_bindings[auxiliary_port];
    } else {
        bindings = devices[port].bindings;
    }
    devices[port].config_present = config != NULL;
    devices[port].config_dirty = 0;
    for (i32 i = 0; i < 32; ++i) {
        bindings[i].kind = PC_BINDING_NONE;
        bindings[i].value = 0;
    }
    if (config != NULL) {
        devices[port].config_installed = 1;
        if (!auxiliary) {
            devices[port].deadzone = config->deadzone;
            devices[port].alternate_deadzone = config->alternate_deadzone;
        }
        PC_INPUT_BINDING *binding = bindings;
        for (i32 i = 0; i < 32; ++i, ++binding) {
            if (commands[i].active) {
                i32 axis = commands[i].axis;
                i32 button = commands[i].button;
                if (axis >= 0) {
                    i32 kind = config->axes[axis][0];
                    i32 primary = config->axes[axis][1];
                    i32 secondary = config->axes[axis][2];
                    switch (kind) {
                        case 10:
                            binding->kind = PC_BINDING_KEY;
                            if (commands[i].active == 2) {
                                binding->value = secondary;
                            } else {
                                binding->value = primary;
                            }
                            break;
                        case 11:
                            binding->kind = PC_BINDING_JOYSTICK_BUTTON;
                            if (commands[i].active == 2) {
                                binding->value = secondary;
                            } else {
                                binding->value = primary;
                            }
                            break;
                        case PC_BINDING_JOYSTICK_AXIS:
                        case PC_BINDING_JOYSTICK_AXIS_REVERSED:
                        case PC_BINDING_MOUSE_AXIS:
                        case PC_BINDING_MOUSE_AXIS_REVERSED:
                        case 12:
                        case 13:
                            binding->kind = kind;
                            binding->value = secondary;
                            break;
                    }
                }
                if ((u32)button <= 15 && binding->kind == PC_BINDING_NONE) {
                    u8 kind = config->buttons[button][0];
                    u8 value = config->buttons[button][1];
                    switch (kind) {
                        case PC_BINDING_JOYSTICK_BUTTON:
                        case PC_BINDING_JOYSTICK_AXIS:
                        case PC_BINDING_JOYSTICK_AXIS_REVERSED:
                        case PC_BINDING_MOUSE_BUTTON:
                        case PC_BINDING_MOUSE_AXIS:
                        case PC_BINDING_MOUSE_AXIS_REVERSED:
                        case PC_BINDING_KEY:
                            binding->kind = kind;
                            binding->value = value;
                            break;
                    }
                }
            }
        }
    }
}

i32 PC_INPUT_RUNTIME::fun_006d4790(i32) {
    return 0;
}

extern "C" {
    extern const char PCKeyboardMouse2Name[16] = "KeyboardMouse2";
}

void PC_INPUT_RUNTIME::InitKeyboardProfiles() {
    PC_INPUT_CONFIG *config;
    NuStrCpy(devices[10].name, "KeyboardMouse1");
    devices[10].kind = PC_INPUT_DEVICE_KEYBOARD_MOUSE1;
    devices[10].product.Data1 = 1;
    devices[10].config_dirty = 1;
    devices[10].capabilities_valid = 1;
    devices[10].button_count = 255;
    config = FindConfig(3, 10, preferred_key);
    if (!config) {
        config = FindConfig(3, 10, -1);
    }
    if (config) {
        InstallConfig(config, 10, config->key, 0, 0);
    }
    devices[11].device = nu_keyboard_owner.device;
    NuStrCpy(devices[11].name, "Keyboard1");
    devices[11].kind = PC_INPUT_DEVICE_KEYBOARD1;
    devices[11].product.Data1 = 3;
    devices[11].config_dirty = 1;
    devices[11].capabilities_valid = 1;
    devices[11].button_count = 255;
    config = FindConfig(3, 11, preferred_key);
    if (!config) {
        config = FindConfig(3, 11, -1);
    }
    if (config) {
        InstallConfig(config, 11, config->key, 0, 0);
    }
    NuStrCpy(devices[12].name, PCKeyboardMouse2Name);
    devices[12].kind = PC_INPUT_DEVICE_KEYBOARD_MOUSE2;
    devices[12].product.Data1 = 2;
    devices[12].config_dirty = 1;
    devices[12].capabilities_valid = 1;
    devices[12].button_count = 255;
    config = FindConfig(3, 12, preferred_key);
    if (!config) {
        config = FindConfig(3, 12, -1);
    }
    if (config) {
        InstallConfig(config, 12, config->key, 0, 0);
    }
    devices[13].device = nu_keyboard_owner.device;
    NuStrCpy(devices[13].name, "Keyboard2");
    devices[13].kind = PC_INPUT_DEVICE_KEYBOARD2;
    devices[13].product.Data1 = 4;
    devices[13].config_dirty = 1;
    devices[13].capabilities_valid = 1;
    devices[13].button_count = 255;
    config = FindConfig(3, 13, preferred_key);
    if (!config) {
        config = FindConfig(3, 13, -1);
    }
    if (config) {
        InstallConfig(config, 13, config->key, 0, 0);
    }
}

/* DIJOYSTATE2 (dinput.h): the joystick state whose axes ConfigureJoystickRanges gives a range, by offset. */
struct PC_DI_JOYSTATE2 {          // size 0x110
    LONG x;                       // 0x000
    LONG y;                       // 0x004
    LONG z;                       // 0x008
    LONG rx;                      // 0x00c
    LONG ry;                      // 0x010
    LONG rz;                      // 0x014
    LONG sliders[2];              // 0x018
    DWORD pov[4];                 // 0x020
    BYTE buttons[128];            // 0x030
    LONG velocity_x;              // 0x0b0
    LONG velocity_y;              // 0x0b4
    LONG velocity_z;              // 0x0b8
    LONG angular_velocity_x;      // 0x0bc
    LONG angular_velocity_y;      // 0x0c0
    LONG angular_velocity_z;      // 0x0c4
    LONG velocity_sliders[2];     // 0x0c8
    LONG acceleration_x;          // 0x0d0
    LONG acceleration_y;          // 0x0d4
    LONG acceleration_z;          // 0x0d8
    LONG angular_acceleration_x;  // 0x0dc
    LONG angular_acceleration_y;  // 0x0e0
    LONG angular_acceleration_z;  // 0x0e4
    LONG acceleration_sliders[2]; // 0x0e8
    LONG force_x;                 // 0x0f0
    LONG force_y;                 // 0x0f4
    LONG force_z;                 // 0x0f8
    LONG torque_x;                // 0x0fc
    LONG torque_y;                // 0x100
    LONG torque_z;                // 0x104
    LONG force_sliders[2];        // 0x108
};

struct PC_DI_RANGE {   // size 0x18
    DWORD size;        // 0x00
    DWORD header_size; // 0x04
    DWORD object;      // 0x08
    DWORD how;         // 0x0c
    LONG minimum;      // 0x10
    LONG maximum;      // 0x14
};

/* TODO(header): the DirectInput device is called through its C++ interface in retail. */
struct PC_DI_DEVICE_INTERFACE {
    virtual HRESULT __stdcall QueryInterface(REFIID, void **) = 0;
    virtual ULONG __stdcall AddRef(void) = 0;
    virtual ULONG __stdcall Release(void) = 0;
    virtual HRESULT __stdcall GetCapabilities(PC_DI_CAPS *) = 0;
    virtual HRESULT __stdcall EnumObjects(void *, void *, DWORD) = 0;
    virtual HRESULT __stdcall GetProperty(REFGUID, PC_DI_PROPERTY *) = 0;
    virtual HRESULT __stdcall SetProperty(REFGUID, const PC_DI_PROPERTY *) = 0;
    virtual HRESULT __stdcall Acquire(void) = 0;
    virtual HRESULT __stdcall Unacquire(void) = 0;
    virtual HRESULT __stdcall GetDeviceState(DWORD, void *) = 0;
    virtual HRESULT __stdcall GetDeviceData(DWORD, void *, DWORD *, DWORD) = 0;
    virtual HRESULT __stdcall SetDataFormat(const PC_DI_DATA_FORMAT *) = 0;
    virtual HRESULT __stdcall SetEventNotification(HANDLE) = 0;
    virtual HRESULT __stdcall SetCooperativeLevel(HWND, DWORD) = 0;
};

struct PC_DI_FACTORY_INTERFACE {
    virtual HRESULT __stdcall QueryInterface(REFIID, void **) = 0;
    virtual ULONG __stdcall AddRef(void) = 0;
    virtual ULONG __stdcall Release(void) = 0;
    virtual HRESULT __stdcall CreateDevice(REFGUID, NuKeyboardDevice **, IUnknown *) = 0;
    virtual HRESULT __stdcall EnumDevices(DWORD, BOOL(__stdcall *)(const PC_DI_INSTANCE *, void *), void *, DWORD) = 0;
};

extern "C" void ConfigureJoystickRanges(NuKeyboardDevice *device) {
    if (device == NULL) {
        return;
    }
    PC_DI_RANGE range;
    PC_DI_DEVICE_INTERFACE *input = (PC_DI_DEVICE_INTERFACE *)device;
    range.size = 24;
    range.header_size = 16;
    range.how = 1;
    range.minimum = -32768;
    range.maximum = 32767;
    range.object = offsetof(PC_DI_JOYSTATE2, x);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, y);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, z);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, rx);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, ry);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, rz);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, sliders[0]);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, sliders[1]);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, velocity_x);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, velocity_y);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, velocity_z);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, angular_velocity_x);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, angular_velocity_y);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, angular_velocity_z);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, velocity_sliders[0]);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, velocity_sliders[1]);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, acceleration_x);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, acceleration_y);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, acceleration_z);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, angular_acceleration_x);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, angular_acceleration_y);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, angular_acceleration_z);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, acceleration_sliders[0]);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, acceleration_sliders[1]);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, force_x);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, force_y);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, force_z);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, torque_x);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, torque_y);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, torque_z);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, force_sliders[0]);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
    range.object = offsetof(PC_DI_JOYSTATE2, force_sliders[1]);
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY *)&range);
}

// clang-format on
i32 PcInputSelectedPort = -1;

extern const GUID PcSystemKeyboardId = {
    0x6f1d2b61, 0xd5a0, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID dat_00784e0c = {
    0x55728220, 0xd33c, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern "C" {
    static const PC_DI_OBJECT_FORMAT PcKeyboardDataFormatObjects[256] = {
        {&dat_00784e0c, 0x0,  0x8000000c, 0x0},
        {&dat_00784e0c, 0x1,  0x8000010c, 0x0},
        {&dat_00784e0c, 0x2,  0x8000020c, 0x0},
        {&dat_00784e0c, 0x3,  0x8000030c, 0x0},
        {&dat_00784e0c, 0x4,  0x8000040c, 0x0},
        {&dat_00784e0c, 0x5,  0x8000050c, 0x0},
        {&dat_00784e0c, 0x6,  0x8000060c, 0x0},
        {&dat_00784e0c, 0x7,  0x8000070c, 0x0},
        {&dat_00784e0c, 0x8,  0x8000080c, 0x0},
        {&dat_00784e0c, 0x9,  0x8000090c, 0x0},
        {&dat_00784e0c, 0xa,  0x80000a0c, 0x0},
        {&dat_00784e0c, 0xb,  0x80000b0c, 0x0},
        {&dat_00784e0c, 0xc,  0x80000c0c, 0x0},
        {&dat_00784e0c, 0xd,  0x80000d0c, 0x0},
        {&dat_00784e0c, 0xe,  0x80000e0c, 0x0},
        {&dat_00784e0c, 0xf,  0x80000f0c, 0x0},
        {&dat_00784e0c, 0x10, 0x8000100c, 0x0},
        {&dat_00784e0c, 0x11, 0x8000110c, 0x0},
        {&dat_00784e0c, 0x12, 0x8000120c, 0x0},
        {&dat_00784e0c, 0x13, 0x8000130c, 0x0},
        {&dat_00784e0c, 0x14, 0x8000140c, 0x0},
        {&dat_00784e0c, 0x15, 0x8000150c, 0x0},
        {&dat_00784e0c, 0x16, 0x8000160c, 0x0},
        {&dat_00784e0c, 0x17, 0x8000170c, 0x0},
        {&dat_00784e0c, 0x18, 0x8000180c, 0x0},
        {&dat_00784e0c, 0x19, 0x8000190c, 0x0},
        {&dat_00784e0c, 0x1a, 0x80001a0c, 0x0},
        {&dat_00784e0c, 0x1b, 0x80001b0c, 0x0},
        {&dat_00784e0c, 0x1c, 0x80001c0c, 0x0},
        {&dat_00784e0c, 0x1d, 0x80001d0c, 0x0},
        {&dat_00784e0c, 0x1e, 0x80001e0c, 0x0},
        {&dat_00784e0c, 0x1f, 0x80001f0c, 0x0},
        {&dat_00784e0c, 0x20, 0x8000200c, 0x0},
        {&dat_00784e0c, 0x21, 0x8000210c, 0x0},
        {&dat_00784e0c, 0x22, 0x8000220c, 0x0},
        {&dat_00784e0c, 0x23, 0x8000230c, 0x0},
        {&dat_00784e0c, 0x24, 0x8000240c, 0x0},
        {&dat_00784e0c, 0x25, 0x8000250c, 0x0},
        {&dat_00784e0c, 0x26, 0x8000260c, 0x0},
        {&dat_00784e0c, 0x27, 0x8000270c, 0x0},
        {&dat_00784e0c, 0x28, 0x8000280c, 0x0},
        {&dat_00784e0c, 0x29, 0x8000290c, 0x0},
        {&dat_00784e0c, 0x2a, 0x80002a0c, 0x0},
        {&dat_00784e0c, 0x2b, 0x80002b0c, 0x0},
        {&dat_00784e0c, 0x2c, 0x80002c0c, 0x0},
        {&dat_00784e0c, 0x2d, 0x80002d0c, 0x0},
        {&dat_00784e0c, 0x2e, 0x80002e0c, 0x0},
        {&dat_00784e0c, 0x2f, 0x80002f0c, 0x0},
        {&dat_00784e0c, 0x30, 0x8000300c, 0x0},
        {&dat_00784e0c, 0x31, 0x8000310c, 0x0},
        {&dat_00784e0c, 0x32, 0x8000320c, 0x0},
        {&dat_00784e0c, 0x33, 0x8000330c, 0x0},
        {&dat_00784e0c, 0x34, 0x8000340c, 0x0},
        {&dat_00784e0c, 0x35, 0x8000350c, 0x0},
        {&dat_00784e0c, 0x36, 0x8000360c, 0x0},
        {&dat_00784e0c, 0x37, 0x8000370c, 0x0},
        {&dat_00784e0c, 0x38, 0x8000380c, 0x0},
        {&dat_00784e0c, 0x39, 0x8000390c, 0x0},
        {&dat_00784e0c, 0x3a, 0x80003a0c, 0x0},
        {&dat_00784e0c, 0x3b, 0x80003b0c, 0x0},
        {&dat_00784e0c, 0x3c, 0x80003c0c, 0x0},
        {&dat_00784e0c, 0x3d, 0x80003d0c, 0x0},
        {&dat_00784e0c, 0x3e, 0x80003e0c, 0x0},
        {&dat_00784e0c, 0x3f, 0x80003f0c, 0x0},
        {&dat_00784e0c, 0x40, 0x8000400c, 0x0},
        {&dat_00784e0c, 0x41, 0x8000410c, 0x0},
        {&dat_00784e0c, 0x42, 0x8000420c, 0x0},
        {&dat_00784e0c, 0x43, 0x8000430c, 0x0},
        {&dat_00784e0c, 0x44, 0x8000440c, 0x0},
        {&dat_00784e0c, 0x45, 0x8000450c, 0x0},
        {&dat_00784e0c, 0x46, 0x8000460c, 0x0},
        {&dat_00784e0c, 0x47, 0x8000470c, 0x0},
        {&dat_00784e0c, 0x48, 0x8000480c, 0x0},
        {&dat_00784e0c, 0x49, 0x8000490c, 0x0},
        {&dat_00784e0c, 0x4a, 0x80004a0c, 0x0},
        {&dat_00784e0c, 0x4b, 0x80004b0c, 0x0},
        {&dat_00784e0c, 0x4c, 0x80004c0c, 0x0},
        {&dat_00784e0c, 0x4d, 0x80004d0c, 0x0},
        {&dat_00784e0c, 0x4e, 0x80004e0c, 0x0},
        {&dat_00784e0c, 0x4f, 0x80004f0c, 0x0},
        {&dat_00784e0c, 0x50, 0x8000500c, 0x0},
        {&dat_00784e0c, 0x51, 0x8000510c, 0x0},
        {&dat_00784e0c, 0x52, 0x8000520c, 0x0},
        {&dat_00784e0c, 0x53, 0x8000530c, 0x0},
        {&dat_00784e0c, 0x54, 0x8000540c, 0x0},
        {&dat_00784e0c, 0x55, 0x8000550c, 0x0},
        {&dat_00784e0c, 0x56, 0x8000560c, 0x0},
        {&dat_00784e0c, 0x57, 0x8000570c, 0x0},
        {&dat_00784e0c, 0x58, 0x8000580c, 0x0},
        {&dat_00784e0c, 0x59, 0x8000590c, 0x0},
        {&dat_00784e0c, 0x5a, 0x80005a0c, 0x0},
        {&dat_00784e0c, 0x5b, 0x80005b0c, 0x0},
        {&dat_00784e0c, 0x5c, 0x80005c0c, 0x0},
        {&dat_00784e0c, 0x5d, 0x80005d0c, 0x0},
        {&dat_00784e0c, 0x5e, 0x80005e0c, 0x0},
        {&dat_00784e0c, 0x5f, 0x80005f0c, 0x0},
        {&dat_00784e0c, 0x60, 0x8000600c, 0x0},
        {&dat_00784e0c, 0x61, 0x8000610c, 0x0},
        {&dat_00784e0c, 0x62, 0x8000620c, 0x0},
        {&dat_00784e0c, 0x63, 0x8000630c, 0x0},
        {&dat_00784e0c, 0x64, 0x8000640c, 0x0},
        {&dat_00784e0c, 0x65, 0x8000650c, 0x0},
        {&dat_00784e0c, 0x66, 0x8000660c, 0x0},
        {&dat_00784e0c, 0x67, 0x8000670c, 0x0},
        {&dat_00784e0c, 0x68, 0x8000680c, 0x0},
        {&dat_00784e0c, 0x69, 0x8000690c, 0x0},
        {&dat_00784e0c, 0x6a, 0x80006a0c, 0x0},
        {&dat_00784e0c, 0x6b, 0x80006b0c, 0x0},
        {&dat_00784e0c, 0x6c, 0x80006c0c, 0x0},
        {&dat_00784e0c, 0x6d, 0x80006d0c, 0x0},
        {&dat_00784e0c, 0x6e, 0x80006e0c, 0x0},
        {&dat_00784e0c, 0x6f, 0x80006f0c, 0x0},
        {&dat_00784e0c, 0x70, 0x8000700c, 0x0},
        {&dat_00784e0c, 0x71, 0x8000710c, 0x0},
        {&dat_00784e0c, 0x72, 0x8000720c, 0x0},
        {&dat_00784e0c, 0x73, 0x8000730c, 0x0},
        {&dat_00784e0c, 0x74, 0x8000740c, 0x0},
        {&dat_00784e0c, 0x75, 0x8000750c, 0x0},
        {&dat_00784e0c, 0x76, 0x8000760c, 0x0},
        {&dat_00784e0c, 0x77, 0x8000770c, 0x0},
        {&dat_00784e0c, 0x78, 0x8000780c, 0x0},
        {&dat_00784e0c, 0x79, 0x8000790c, 0x0},
        {&dat_00784e0c, 0x7a, 0x80007a0c, 0x0},
        {&dat_00784e0c, 0x7b, 0x80007b0c, 0x0},
        {&dat_00784e0c, 0x7c, 0x80007c0c, 0x0},
        {&dat_00784e0c, 0x7d, 0x80007d0c, 0x0},
        {&dat_00784e0c, 0x7e, 0x80007e0c, 0x0},
        {&dat_00784e0c, 0x7f, 0x80007f0c, 0x0},
        {&dat_00784e0c, 0x80, 0x8000800c, 0x0},
        {&dat_00784e0c, 0x81, 0x8000810c, 0x0},
        {&dat_00784e0c, 0x82, 0x8000820c, 0x0},
        {&dat_00784e0c, 0x83, 0x8000830c, 0x0},
        {&dat_00784e0c, 0x84, 0x8000840c, 0x0},
        {&dat_00784e0c, 0x85, 0x8000850c, 0x0},
        {&dat_00784e0c, 0x86, 0x8000860c, 0x0},
        {&dat_00784e0c, 0x87, 0x8000870c, 0x0},
        {&dat_00784e0c, 0x88, 0x8000880c, 0x0},
        {&dat_00784e0c, 0x89, 0x8000890c, 0x0},
        {&dat_00784e0c, 0x8a, 0x80008a0c, 0x0},
        {&dat_00784e0c, 0x8b, 0x80008b0c, 0x0},
        {&dat_00784e0c, 0x8c, 0x80008c0c, 0x0},
        {&dat_00784e0c, 0x8d, 0x80008d0c, 0x0},
        {&dat_00784e0c, 0x8e, 0x80008e0c, 0x0},
        {&dat_00784e0c, 0x8f, 0x80008f0c, 0x0},
        {&dat_00784e0c, 0x90, 0x8000900c, 0x0},
        {&dat_00784e0c, 0x91, 0x8000910c, 0x0},
        {&dat_00784e0c, 0x92, 0x8000920c, 0x0},
        {&dat_00784e0c, 0x93, 0x8000930c, 0x0},
        {&dat_00784e0c, 0x94, 0x8000940c, 0x0},
        {&dat_00784e0c, 0x95, 0x8000950c, 0x0},
        {&dat_00784e0c, 0x96, 0x8000960c, 0x0},
        {&dat_00784e0c, 0x97, 0x8000970c, 0x0},
        {&dat_00784e0c, 0x98, 0x8000980c, 0x0},
        {&dat_00784e0c, 0x99, 0x8000990c, 0x0},
        {&dat_00784e0c, 0x9a, 0x80009a0c, 0x0},
        {&dat_00784e0c, 0x9b, 0x80009b0c, 0x0},
        {&dat_00784e0c, 0x9c, 0x80009c0c, 0x0},
        {&dat_00784e0c, 0x9d, 0x80009d0c, 0x0},
        {&dat_00784e0c, 0x9e, 0x80009e0c, 0x0},
        {&dat_00784e0c, 0x9f, 0x80009f0c, 0x0},
        {&dat_00784e0c, 0xa0, 0x8000a00c, 0x0},
        {&dat_00784e0c, 0xa1, 0x8000a10c, 0x0},
        {&dat_00784e0c, 0xa2, 0x8000a20c, 0x0},
        {&dat_00784e0c, 0xa3, 0x8000a30c, 0x0},
        {&dat_00784e0c, 0xa4, 0x8000a40c, 0x0},
        {&dat_00784e0c, 0xa5, 0x8000a50c, 0x0},
        {&dat_00784e0c, 0xa6, 0x8000a60c, 0x0},
        {&dat_00784e0c, 0xa7, 0x8000a70c, 0x0},
        {&dat_00784e0c, 0xa8, 0x8000a80c, 0x0},
        {&dat_00784e0c, 0xa9, 0x8000a90c, 0x0},
        {&dat_00784e0c, 0xaa, 0x8000aa0c, 0x0},
        {&dat_00784e0c, 0xab, 0x8000ab0c, 0x0},
        {&dat_00784e0c, 0xac, 0x8000ac0c, 0x0},
        {&dat_00784e0c, 0xad, 0x8000ad0c, 0x0},
        {&dat_00784e0c, 0xae, 0x8000ae0c, 0x0},
        {&dat_00784e0c, 0xaf, 0x8000af0c, 0x0},
        {&dat_00784e0c, 0xb0, 0x8000b00c, 0x0},
        {&dat_00784e0c, 0xb1, 0x8000b10c, 0x0},
        {&dat_00784e0c, 0xb2, 0x8000b20c, 0x0},
        {&dat_00784e0c, 0xb3, 0x8000b30c, 0x0},
        {&dat_00784e0c, 0xb4, 0x8000b40c, 0x0},
        {&dat_00784e0c, 0xb5, 0x8000b50c, 0x0},
        {&dat_00784e0c, 0xb6, 0x8000b60c, 0x0},
        {&dat_00784e0c, 0xb7, 0x8000b70c, 0x0},
        {&dat_00784e0c, 0xb8, 0x8000b80c, 0x0},
        {&dat_00784e0c, 0xb9, 0x8000b90c, 0x0},
        {&dat_00784e0c, 0xba, 0x8000ba0c, 0x0},
        {&dat_00784e0c, 0xbb, 0x8000bb0c, 0x0},
        {&dat_00784e0c, 0xbc, 0x8000bc0c, 0x0},
        {&dat_00784e0c, 0xbd, 0x8000bd0c, 0x0},
        {&dat_00784e0c, 0xbe, 0x8000be0c, 0x0},
        {&dat_00784e0c, 0xbf, 0x8000bf0c, 0x0},
        {&dat_00784e0c, 0xc0, 0x8000c00c, 0x0},
        {&dat_00784e0c, 0xc1, 0x8000c10c, 0x0},
        {&dat_00784e0c, 0xc2, 0x8000c20c, 0x0},
        {&dat_00784e0c, 0xc3, 0x8000c30c, 0x0},
        {&dat_00784e0c, 0xc4, 0x8000c40c, 0x0},
        {&dat_00784e0c, 0xc5, 0x8000c50c, 0x0},
        {&dat_00784e0c, 0xc6, 0x8000c60c, 0x0},
        {&dat_00784e0c, 0xc7, 0x8000c70c, 0x0},
        {&dat_00784e0c, 0xc8, 0x8000c80c, 0x0},
        {&dat_00784e0c, 0xc9, 0x8000c90c, 0x0},
        {&dat_00784e0c, 0xca, 0x8000ca0c, 0x0},
        {&dat_00784e0c, 0xcb, 0x8000cb0c, 0x0},
        {&dat_00784e0c, 0xcc, 0x8000cc0c, 0x0},
        {&dat_00784e0c, 0xcd, 0x8000cd0c, 0x0},
        {&dat_00784e0c, 0xce, 0x8000ce0c, 0x0},
        {&dat_00784e0c, 0xcf, 0x8000cf0c, 0x0},
        {&dat_00784e0c, 0xd0, 0x8000d00c, 0x0},
        {&dat_00784e0c, 0xd1, 0x8000d10c, 0x0},
        {&dat_00784e0c, 0xd2, 0x8000d20c, 0x0},
        {&dat_00784e0c, 0xd3, 0x8000d30c, 0x0},
        {&dat_00784e0c, 0xd4, 0x8000d40c, 0x0},
        {&dat_00784e0c, 0xd5, 0x8000d50c, 0x0},
        {&dat_00784e0c, 0xd6, 0x8000d60c, 0x0},
        {&dat_00784e0c, 0xd7, 0x8000d70c, 0x0},
        {&dat_00784e0c, 0xd8, 0x8000d80c, 0x0},
        {&dat_00784e0c, 0xd9, 0x8000d90c, 0x0},
        {&dat_00784e0c, 0xda, 0x8000da0c, 0x0},
        {&dat_00784e0c, 0xdb, 0x8000db0c, 0x0},
        {&dat_00784e0c, 0xdc, 0x8000dc0c, 0x0},
        {&dat_00784e0c, 0xdd, 0x8000dd0c, 0x0},
        {&dat_00784e0c, 0xde, 0x8000de0c, 0x0},
        {&dat_00784e0c, 0xdf, 0x8000df0c, 0x0},
        {&dat_00784e0c, 0xe0, 0x8000e00c, 0x0},
        {&dat_00784e0c, 0xe1, 0x8000e10c, 0x0},
        {&dat_00784e0c, 0xe2, 0x8000e20c, 0x0},
        {&dat_00784e0c, 0xe3, 0x8000e30c, 0x0},
        {&dat_00784e0c, 0xe4, 0x8000e40c, 0x0},
        {&dat_00784e0c, 0xe5, 0x8000e50c, 0x0},
        {&dat_00784e0c, 0xe6, 0x8000e60c, 0x0},
        {&dat_00784e0c, 0xe7, 0x8000e70c, 0x0},
        {&dat_00784e0c, 0xe8, 0x8000e80c, 0x0},
        {&dat_00784e0c, 0xe9, 0x8000e90c, 0x0},
        {&dat_00784e0c, 0xea, 0x8000ea0c, 0x0},
        {&dat_00784e0c, 0xeb, 0x8000eb0c, 0x0},
        {&dat_00784e0c, 0xec, 0x8000ec0c, 0x0},
        {&dat_00784e0c, 0xed, 0x8000ed0c, 0x0},
        {&dat_00784e0c, 0xee, 0x8000ee0c, 0x0},
        {&dat_00784e0c, 0xef, 0x8000ef0c, 0x0},
        {&dat_00784e0c, 0xf0, 0x8000f00c, 0x0},
        {&dat_00784e0c, 0xf1, 0x8000f10c, 0x0},
        {&dat_00784e0c, 0xf2, 0x8000f20c, 0x0},
        {&dat_00784e0c, 0xf3, 0x8000f30c, 0x0},
        {&dat_00784e0c, 0xf4, 0x8000f40c, 0x0},
        {&dat_00784e0c, 0xf5, 0x8000f50c, 0x0},
        {&dat_00784e0c, 0xf6, 0x8000f60c, 0x0},
        {&dat_00784e0c, 0xf7, 0x8000f70c, 0x0},
        {&dat_00784e0c, 0xf8, 0x8000f80c, 0x0},
        {&dat_00784e0c, 0xf9, 0x8000f90c, 0x0},
        {&dat_00784e0c, 0xfa, 0x8000fa0c, 0x0},
        {&dat_00784e0c, 0xfb, 0x8000fb0c, 0x0},
        {&dat_00784e0c, 0xfc, 0x8000fc0c, 0x0},
        {&dat_00784e0c, 0xfd, 0x8000fd0c, 0x0},
        {&dat_00784e0c, 0xfe, 0x8000fe0c, 0x0},
        {&dat_00784e0c, 0xff, 0x8000ff0c, 0x0},
    };
}
extern const PC_DI_DATA_FORMAT PcKeyboardDataFormat = {24, 16, 2, 256, 256, PcKeyboardDataFormatObjects};
extern const GUID dat_00784dfc = {
    0xa36d02f2, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID dat_00784e2c = {
    0xa36d02e4, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID dat_00784e3c = {
    0xa36d02e3, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID dat_00784e4c = {
    0xa36d02f5, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID dat_00784e5c = {
    0xa36d02f4, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID dat_00784e6c = {
    0xa36d02e2, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID dat_00784e7c = {
    0xa36d02e1, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID dat_00784e8c = {
    0xa36d02e0, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern "C" {
    static const PC_DI_OBJECT_FORMAT PcJoystickDataFormatObjects[164] = {
        {&dat_00784e8c, 0x0,   0x80ffff03, 0x100},
        {&dat_00784e7c, 0x4,   0x80ffff03, 0x100},
        {&dat_00784e6c, 0x8,   0x80ffff03, 0x100},
        {&dat_00784e5c, 0xc,   0x80ffff03, 0x100},
        {&dat_00784e4c, 0x10,  0x80ffff03, 0x100},
        {&dat_00784e3c, 0x14,  0x80ffff03, 0x100},
        {&dat_00784e2c, 0x18,  0x80ffff03, 0x100},
        {&dat_00784e2c, 0x1c,  0x80ffff03, 0x100},
        {&dat_00784dfc, 0x20,  0x80ffff10, 0x0  },
        {&dat_00784dfc, 0x24,  0x80ffff10, 0x0  },
        {&dat_00784dfc, 0x28,  0x80ffff10, 0x0  },
        {&dat_00784dfc, 0x2c,  0x80ffff10, 0x0  },
        {NULL,          0x30,  0x80ffff0c, 0x0  },
        {NULL,          0x31,  0x80ffff0c, 0x0  },
        {NULL,          0x32,  0x80ffff0c, 0x0  },
        {NULL,          0x33,  0x80ffff0c, 0x0  },
        {NULL,          0x34,  0x80ffff0c, 0x0  },
        {NULL,          0x35,  0x80ffff0c, 0x0  },
        {NULL,          0x36,  0x80ffff0c, 0x0  },
        {NULL,          0x37,  0x80ffff0c, 0x0  },
        {NULL,          0x38,  0x80ffff0c, 0x0  },
        {NULL,          0x39,  0x80ffff0c, 0x0  },
        {NULL,          0x3a,  0x80ffff0c, 0x0  },
        {NULL,          0x3b,  0x80ffff0c, 0x0  },
        {NULL,          0x3c,  0x80ffff0c, 0x0  },
        {NULL,          0x3d,  0x80ffff0c, 0x0  },
        {NULL,          0x3e,  0x80ffff0c, 0x0  },
        {NULL,          0x3f,  0x80ffff0c, 0x0  },
        {NULL,          0x40,  0x80ffff0c, 0x0  },
        {NULL,          0x41,  0x80ffff0c, 0x0  },
        {NULL,          0x42,  0x80ffff0c, 0x0  },
        {NULL,          0x43,  0x80ffff0c, 0x0  },
        {NULL,          0x44,  0x80ffff0c, 0x0  },
        {NULL,          0x45,  0x80ffff0c, 0x0  },
        {NULL,          0x46,  0x80ffff0c, 0x0  },
        {NULL,          0x47,  0x80ffff0c, 0x0  },
        {NULL,          0x48,  0x80ffff0c, 0x0  },
        {NULL,          0x49,  0x80ffff0c, 0x0  },
        {NULL,          0x4a,  0x80ffff0c, 0x0  },
        {NULL,          0x4b,  0x80ffff0c, 0x0  },
        {NULL,          0x4c,  0x80ffff0c, 0x0  },
        {NULL,          0x4d,  0x80ffff0c, 0x0  },
        {NULL,          0x4e,  0x80ffff0c, 0x0  },
        {NULL,          0x4f,  0x80ffff0c, 0x0  },
        {NULL,          0x50,  0x80ffff0c, 0x0  },
        {NULL,          0x51,  0x80ffff0c, 0x0  },
        {NULL,          0x52,  0x80ffff0c, 0x0  },
        {NULL,          0x53,  0x80ffff0c, 0x0  },
        {NULL,          0x54,  0x80ffff0c, 0x0  },
        {NULL,          0x55,  0x80ffff0c, 0x0  },
        {NULL,          0x56,  0x80ffff0c, 0x0  },
        {NULL,          0x57,  0x80ffff0c, 0x0  },
        {NULL,          0x58,  0x80ffff0c, 0x0  },
        {NULL,          0x59,  0x80ffff0c, 0x0  },
        {NULL,          0x5a,  0x80ffff0c, 0x0  },
        {NULL,          0x5b,  0x80ffff0c, 0x0  },
        {NULL,          0x5c,  0x80ffff0c, 0x0  },
        {NULL,          0x5d,  0x80ffff0c, 0x0  },
        {NULL,          0x5e,  0x80ffff0c, 0x0  },
        {NULL,          0x5f,  0x80ffff0c, 0x0  },
        {NULL,          0x60,  0x80ffff0c, 0x0  },
        {NULL,          0x61,  0x80ffff0c, 0x0  },
        {NULL,          0x62,  0x80ffff0c, 0x0  },
        {NULL,          0x63,  0x80ffff0c, 0x0  },
        {NULL,          0x64,  0x80ffff0c, 0x0  },
        {NULL,          0x65,  0x80ffff0c, 0x0  },
        {NULL,          0x66,  0x80ffff0c, 0x0  },
        {NULL,          0x67,  0x80ffff0c, 0x0  },
        {NULL,          0x68,  0x80ffff0c, 0x0  },
        {NULL,          0x69,  0x80ffff0c, 0x0  },
        {NULL,          0x6a,  0x80ffff0c, 0x0  },
        {NULL,          0x6b,  0x80ffff0c, 0x0  },
        {NULL,          0x6c,  0x80ffff0c, 0x0  },
        {NULL,          0x6d,  0x80ffff0c, 0x0  },
        {NULL,          0x6e,  0x80ffff0c, 0x0  },
        {NULL,          0x6f,  0x80ffff0c, 0x0  },
        {NULL,          0x70,  0x80ffff0c, 0x0  },
        {NULL,          0x71,  0x80ffff0c, 0x0  },
        {NULL,          0x72,  0x80ffff0c, 0x0  },
        {NULL,          0x73,  0x80ffff0c, 0x0  },
        {NULL,          0x74,  0x80ffff0c, 0x0  },
        {NULL,          0x75,  0x80ffff0c, 0x0  },
        {NULL,          0x76,  0x80ffff0c, 0x0  },
        {NULL,          0x77,  0x80ffff0c, 0x0  },
        {NULL,          0x78,  0x80ffff0c, 0x0  },
        {NULL,          0x79,  0x80ffff0c, 0x0  },
        {NULL,          0x7a,  0x80ffff0c, 0x0  },
        {NULL,          0x7b,  0x80ffff0c, 0x0  },
        {NULL,          0x7c,  0x80ffff0c, 0x0  },
        {NULL,          0x7d,  0x80ffff0c, 0x0  },
        {NULL,          0x7e,  0x80ffff0c, 0x0  },
        {NULL,          0x7f,  0x80ffff0c, 0x0  },
        {NULL,          0x80,  0x80ffff0c, 0x0  },
        {NULL,          0x81,  0x80ffff0c, 0x0  },
        {NULL,          0x82,  0x80ffff0c, 0x0  },
        {NULL,          0x83,  0x80ffff0c, 0x0  },
        {NULL,          0x84,  0x80ffff0c, 0x0  },
        {NULL,          0x85,  0x80ffff0c, 0x0  },
        {NULL,          0x86,  0x80ffff0c, 0x0  },
        {NULL,          0x87,  0x80ffff0c, 0x0  },
        {NULL,          0x88,  0x80ffff0c, 0x0  },
        {NULL,          0x89,  0x80ffff0c, 0x0  },
        {NULL,          0x8a,  0x80ffff0c, 0x0  },
        {NULL,          0x8b,  0x80ffff0c, 0x0  },
        {NULL,          0x8c,  0x80ffff0c, 0x0  },
        {NULL,          0x8d,  0x80ffff0c, 0x0  },
        {NULL,          0x8e,  0x80ffff0c, 0x0  },
        {NULL,          0x8f,  0x80ffff0c, 0x0  },
        {NULL,          0x90,  0x80ffff0c, 0x0  },
        {NULL,          0x91,  0x80ffff0c, 0x0  },
        {NULL,          0x92,  0x80ffff0c, 0x0  },
        {NULL,          0x93,  0x80ffff0c, 0x0  },
        {NULL,          0x94,  0x80ffff0c, 0x0  },
        {NULL,          0x95,  0x80ffff0c, 0x0  },
        {NULL,          0x96,  0x80ffff0c, 0x0  },
        {NULL,          0x97,  0x80ffff0c, 0x0  },
        {NULL,          0x98,  0x80ffff0c, 0x0  },
        {NULL,          0x99,  0x80ffff0c, 0x0  },
        {NULL,          0x9a,  0x80ffff0c, 0x0  },
        {NULL,          0x9b,  0x80ffff0c, 0x0  },
        {NULL,          0x9c,  0x80ffff0c, 0x0  },
        {NULL,          0x9d,  0x80ffff0c, 0x0  },
        {NULL,          0x9e,  0x80ffff0c, 0x0  },
        {NULL,          0x9f,  0x80ffff0c, 0x0  },
        {NULL,          0xa0,  0x80ffff0c, 0x0  },
        {NULL,          0xa1,  0x80ffff0c, 0x0  },
        {NULL,          0xa2,  0x80ffff0c, 0x0  },
        {NULL,          0xa3,  0x80ffff0c, 0x0  },
        {NULL,          0xa4,  0x80ffff0c, 0x0  },
        {NULL,          0xa5,  0x80ffff0c, 0x0  },
        {NULL,          0xa6,  0x80ffff0c, 0x0  },
        {NULL,          0xa7,  0x80ffff0c, 0x0  },
        {NULL,          0xa8,  0x80ffff0c, 0x0  },
        {NULL,          0xa9,  0x80ffff0c, 0x0  },
        {NULL,          0xaa,  0x80ffff0c, 0x0  },
        {NULL,          0xab,  0x80ffff0c, 0x0  },
        {NULL,          0xac,  0x80ffff0c, 0x0  },
        {NULL,          0xad,  0x80ffff0c, 0x0  },
        {NULL,          0xae,  0x80ffff0c, 0x0  },
        {NULL,          0xaf,  0x80ffff0c, 0x0  },
        {&dat_00784e8c, 0xb0,  0x80ffff03, 0x200},
        {&dat_00784e7c, 0xb4,  0x80ffff03, 0x200},
        {&dat_00784e6c, 0xb8,  0x80ffff03, 0x200},
        {&dat_00784e5c, 0xbc,  0x80ffff03, 0x200},
        {&dat_00784e4c, 0xc0,  0x80ffff03, 0x200},
        {&dat_00784e3c, 0xc4,  0x80ffff03, 0x200},
        {&dat_00784e2c, 0x18,  0x80ffff03, 0x200},
        {&dat_00784e2c, 0x1c,  0x80ffff03, 0x200},
        {&dat_00784e8c, 0xd0,  0x80ffff03, 0x300},
        {&dat_00784e7c, 0xd4,  0x80ffff03, 0x300},
        {&dat_00784e6c, 0xd8,  0x80ffff03, 0x300},
        {&dat_00784e5c, 0xdc,  0x80ffff03, 0x300},
        {&dat_00784e4c, 0xe0,  0x80ffff03, 0x300},
        {&dat_00784e3c, 0xe4,  0x80ffff03, 0x300},
        {&dat_00784e2c, 0x18,  0x80ffff03, 0x300},
        {&dat_00784e2c, 0x1c,  0x80ffff03, 0x300},
        {&dat_00784e8c, 0xf0,  0x80ffff03, 0x400},
        {&dat_00784e7c, 0xf4,  0x80ffff03, 0x400},
        {&dat_00784e6c, 0xf8,  0x80ffff03, 0x400},
        {&dat_00784e5c, 0xfc,  0x80ffff03, 0x400},
        {&dat_00784e4c, 0x100, 0x80ffff03, 0x400},
        {&dat_00784e3c, 0x104, 0x80ffff03, 0x400},
        {&dat_00784e2c, 0x18,  0x80ffff03, 0x400},
        {&dat_00784e2c, 0x1c,  0x80ffff03, 0x400},
    };
}
extern const PC_DI_DATA_FORMAT PcJoystickDataFormat = {24, 16, 1, 272, 164, PcJoystickDataFormatObjects};

extern "C" BOOL __stdcall InputEnumDevices(const PC_DI_INSTANCE *instance, void *unused) {
    PC_INPUT_RUNTIME *state = (PC_INPUT_RUNTIME *)nu_input_state;
    PC_INPUT_DEVICE *devices = state->devices;
    if (IsXInputProduct(&instance->product)) {
        i32 port = PcXInputDeviceCount;
        PcInputSelectedPort = port;
        devices[port].product = instance->product;
        i32 length = (i32)strlen(instance->product_name);
        if (length > 63) {
            length = 63;
        }
        memset(devices[port].name, 0, 63);
        PCByteCopy(devices[port].name, instance->product_name, length);
        devices[port].kind = PC_INPUT_DEVICE_XINPUT;
        devices[port].capabilities_valid = 0;
        devices[port].deadzone = 64;
        PC_XINPUT_CAPABILITIES capabilities;
        if (XInputGetCapabilities(port, 1, (XINPUT_CAPABILITIES *)&capabilities) == 0) {
            state->InitXInputCaps(&capabilities, &devices[port]);
            if (devices[port].button_count == 10 && devices[port].axis_count == 5 && devices[port].pov_count == 1) {
                devices[port].product.Data1 = 0x028e045e;
            }
        }
        PC_INPUT_CONFIG *config = state->FindConfig(7, port, state->preferred_key);
        if (!config) {
            config = state->FindConfig(7, port, -1);
        }
        if (config) {
            state->InstallConfig(config, port, config->key, 0, 0);
        }
        ++PcXInputDeviceCount;
        return TRUE;
    }
    u32 type = (u8)instance->device_type;
    if (type != 19) {
        if (type - 20 > 1) {
            return TRUE;
        }
        NuKeyboardDevice *device;
        if (((PC_DI_FACTORY_INTERFACE *)nu_keyboard_owner.factory)->CreateDevice(instance->instance, &device, NULL) < 0) {
            return TRUE;
        }
        i32 port;
        for (port = 4; port < 10; ++port) {
            if (devices[port].kind == PC_INPUT_DEVICE_NONE) {
                break;
            }
        }
        if (port >= 10) {
            return TRUE;
        }
        PC_INPUT_DEVICE *record = &devices[port];
        if (PcInputSelectedPort == -1) {
            PcInputSelectedPort = port;
        }
        PC_DI_CAPS caps;
        caps.size = sizeof(caps);
        HRESULT result = ((PC_DI_DEVICE_INTERFACE *)device)->GetCapabilities(&caps);
        record->button_count = (i8)caps.buttons;
        record->capabilities_valid = result == 0;
        record->pov_count = (u8)caps.povs;
        record->device = device;
        record->axis_count = (u8)caps.axes;
        record->deadzone = 32;
        record->kind = PC_INPUT_DEVICE_DIRECTINPUT;
        record->config_dirty = 1;
        record->product = instance->product;
        i32 length = (i32)strlen(instance->product_name);
        if (length > 63) {
            length = 63;
        }
        memset(record->name, 0, 63);
        PCByteCopy(record->name, instance->product_name, length);
        if (NuStrIStr((char *)instance->product_name, "Saitek") != NULL &&
            (NuStrIStr((char *)instance->product_name, "P2600") != NULL || NuStrIStr((char *)instance->product_name, "P2900") != NULL)) {
            record->button_count -= 2;
        }
        ((PC_DI_DEVICE_INTERFACE *)device)->SetDataFormat(&PcJoystickDataFormat);
        ((PC_DI_DEVICE_INTERFACE *)device)->SetCooperativeLevel(PCInputInstance, 9);
        ((PC_DI_DEVICE_INTERFACE *)device)->Acquire();
        ConfigureJoystickRanges(device);
        state->BindPreferredConfig(7, port);
        ++nu_keyboard_owner.device_count;
        return TRUE;
    }
    if (nu_keyboard_owner.factory->methods->create_device(nu_keyboard_owner.factory, PcSystemKeyboardId, &nu_keyboard_owner.device, NULL) !=
        0) {
        return TRUE;
    }
    PcKeyboardCapabilities.size = sizeof(PcKeyboardCapabilities);
    nu_keyboard_owner.device->methods->set_cooperative_level(nu_keyboard_owner.device, PCInputInstance, 22);
    nu_keyboard_owner.device->methods->set_format(nu_keyboard_owner.device, &PcKeyboardDataFormat);
    nu_keyboard_owner.device->methods->acquire(nu_keyboard_owner.device);
    nu_keyboard_owner.device->methods->get_caps(nu_keyboard_owner.device, &PcKeyboardCapabilities);
    state->InitKeyboardProfiles();
    devices[10].device = nu_keyboard_owner.device;
    devices[10].button_count = (i16)PcKeyboardCapabilities.buttons;
    devices[12].device = nu_keyboard_owner.device;
    devices[12].button_count = (i16)PcKeyboardCapabilities.buttons;
    devices[11].device = nu_keyboard_owner.device;
    devices[11].button_count = (i16)PcKeyboardCapabilities.buttons;
    devices[13].device = nu_keyboard_owner.device;
    devices[13].button_count = (i16)PcKeyboardCapabilities.buttons;
    nu_keyboard_owner.key_count = 256;
    return TRUE;
}

extern const GUID PcSystemMouseId = {
    0x6f1d2b60, 0xd5a0, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID PcDirectInput8AId = {
    0xbf798030, 0x483a, 0x4da2, {0xaa, 0x99, 0x5d, 0x64, 0xed, 0x36, 0x97, 0x00}
};
extern "C" {
    static const PC_DI_OBJECT_FORMAT PcMouseDataFormatObjects[11] = {
        {&dat_00784e8c, 0x0,  0xffff03,   0x0},
        {&dat_00784e7c, 0x4,  0xffff03,   0x0},
        {&dat_00784e6c, 0x8,  0x80ffff03, 0x0},
        {NULL,          0xc,  0xffff0c,   0x0},
        {NULL,          0xd,  0xffff0c,   0x0},
        {NULL,          0xe,  0x80ffff0c, 0x0},
        {NULL,          0xf,  0x80ffff0c, 0x0},
        {NULL,          0x10, 0x80ffff0c, 0x0},
        {NULL,          0x11, 0x80ffff0c, 0x0},
        {NULL,          0x12, 0x80ffff0c, 0x0},
        {NULL,          0x13, 0x80ffff0c, 0x0},
    };
}
extern const PC_DI_DATA_FORMAT PcMouseDataFormat = {24, 16, 2, 20, 11, PcMouseDataFormatObjects};

void NuKeyboardOwner::Setup(HWND window, HINSTANCE instance) {
    DirectInput8Create(instance, 0x800, PcDirectInput8AId, (void **)&factory, NULL);
    factory->methods->create_device(factory, PcSystemMouseId, &mouse, NULL);
    PC_DI_CAPS capabilities;
    capabilities.size = sizeof(capabilities);
    mouse->methods->get_caps(mouse, &capabilities);
    if (capabilities.flags & 1) {
        mouse_axes = capabilities.axes;
        mouse_buttons = capabilities.buttons;
        mouse->methods->set_cooperative_level(mouse, window, 5);
        mouse->methods->set_format(mouse, &PcMouseDataFormat);
        PC_DI_PROPERTY property;
        property.header_size = 16;
        property.size = 20;
        property.how = 0;
        property.object = 0;
        property.data = 0;
        mouse->methods->set_property(mouse, *(const GUID *)2, &property);
        property.size = 20;
        property.header_size = 16;
        property.object = 0;
        property.how = 0;
        property.data = 16;
        mouse->methods->set_property(mouse, *(const GUID *)1, &property);
        mouse->methods->acquire(mouse);
    } else {
        mouse_axes = 0;
        mouse_buttons = 0;
    }
    nu_input_state = (NuKeyboardState *)new PC_INPUT_RUNTIME;
    PcInput_EnableKeyboards(NULL, 0);
    device_count = 0;
    PcXInputDeviceCount = 0;
    ((PC_DI_FACTORY_INTERFACE *)factory)->EnumDevices(3, InputEnumDevices, NULL, 1);
    ((PC_DI_FACTORY_INTERFACE *)factory)->EnumDevices(2, InputEnumDevices, NULL, 1);
    ((PC_DI_FACTORY_INTERFACE *)factory)->EnumDevices(4, InputEnumDevices, NULL, 1);
    nu_keyboard_owner.unknown_0000 = 0.2f;
    nu_keyboard_owner.unknown_0008 = 0.2f;
    PcInputSelectedPort = -1;
    nu_keyboard_owner.unknown_0004 = 3;
}

void NuKeyboardOwner::Shutdown() {
    if (nu_input_state == NULL) {
        return;
    }
    PC_INPUT_DEVICE *devices = ((PC_INPUT_RUNTIME *)nu_input_state)->devices;
    if (device != NULL) {
        device->methods->unacquire(device);
        if (device != NULL) {
            device->methods->release(device);
            device = NULL;
        }
    }
    for (u16 index = 10; index < 14; ++index) {
        devices[index].device = NULL;
    }
    for (i32 index = 4; index < 14; ++index) {
        if (devices[index].device != NULL) {
            devices[index].device->methods->unacquire(devices[index].device);
            if (devices[index].device != NULL) {
                devices[index].device->methods->release(devices[index].device);
                devices[index].device = NULL;
            }
        }
        devices[index].kind = PC_INPUT_DEVICE_NONE;
        devices[index].capabilities_valid = 0;
    }
    if (mouse != NULL) {
        mouse->methods->unacquire(mouse);
        mouse->methods->release(mouse);
        mouse = NULL;
    }
    if (factory != NULL) {
        factory->methods->release(factory);
        factory = NULL;
    }
    PC_INPUT_RUNTIME *released = (PC_INPUT_RUNTIME *)nu_input_state;
    if (released != NULL) {
        released->shutdown_state = 0;
        ::operator delete(released);
    }
}

extern "C" void PcInput_DisableAllKeyboards(void) {
    PC_INPUT_RUNTIME *state = (PC_INPUT_RUNTIME *)nu_input_state;
    for (i32 device = 10; device < 14; ++device) {
        state->devices[device].action_keys_active = 0;
    }
}

void PC_INPUT_RUNTIME::AutoConfigurePads(i32 count) {
    i32 device = 0;
    for (i32 pad = 0; pad < count; ++pad) {
        i32 linked = NuPadGetPort(pad);
        for (; device < 14; ++device) {
            if (devices[device].kind != PC_INPUT_DEVICE_NONE && (device < 10 || device > 13 || devices[device].action_keys_active)) {
                if (device != linked) {
                    NuPadMapPlayerToPort(pad, device);
                    NuPadSetValid(device, 1);
                    PC_INPUT_CONFIG *config = FindConfig(7, device, preferred_key);
                    if (!config) {
                        config = FindConfig(7, device, -1);
                    }
                    if (config) {
                        InstallConfig(config, device, config->key, 0, 0);
                    }
                }
                ++device;
                break;
            }
        }
    }
}

extern "C" void NuPadAutoConfigure(i32 count) {
    ((PC_INPUT_RUNTIME *)nu_input_state)->AutoConfigurePads(count);
}

extern "C" i32 GamePadReadBackend(FRAME_PAD *pad, i32 ignored) {
    (void)ignored;
    return NuPadRead(pad);
}

extern "C" PC_DI_MOUSE_STATE *fun_006d6080(void) {
    return &PcMouseSamples[PcMouseSampleIndex ^ 1];
}

extern "C" void fun_006d6030(void) {
}

extern "C" i32 fun_006d6040(void) {
    return 0;
}

extern "C" i32 fun_006d6050(void) {
    return 0;
}

extern "C" PC_DI_MOUSE_STATE *fun_006d6060(void) {
    return &PcMouseSamples[PcMouseSampleIndex];
}

extern "C" i32 NuPad_Interface_GetMaxDevices(void) {
    return 14;
}

extern "C" void NuPadOpenPS(FRAME_PAD *pad) {
    pad->flags |= 4;
}

extern "C" void fun_006d60c0(void) {
}

extern "C" void NuPadClosePS(FRAME_PAD *pad) {
    pad->flags &= ~4;
}

extern "C" void NuPadInitPS(FRAME_SCANNED_PAD *pad) {
}

extern "C" i32 fun_006d60f0(void) {
    return nu_input_state->unknown_5d29;
}

extern "C" void fun_006d6100(i32 value) {
    if (value < 1) {
        value = 1;
    } else if (value > 10) {
        value = 10;
    }
    nu_input_state->unknown_5d29 = value;
}

extern "C" void fun_006d6140(void) {
    nu_keyboard_device_flags = 7;
}

extern "C" void fun_006d6150(void) {
    nu_keyboard_device_flags = 3;
}

extern "C" void fun_006d6160(void) {
    nu_keyboard_device_flags = 0;
}

// Native NuKeyboardOwner cohort, append to pc_input.cpp.
i32 NuKeyboardOwner::SetMouseCooperative(i32 value) {
    i32 previous = mouse_cooperative;
    if (previous == value) {
        return value;
    }
    mouse_cooperative = value;
    if (mouse != NULL) {
        mouse->methods->unacquire(mouse);
        if (value) {
            mouse->methods->set_cooperative_level(mouse, PCInputInstance, 5);
        } else {
            mouse->methods->set_cooperative_level(mouse, PCInputInstance, 6);
        }
        mouse->methods->acquire(mouse);
    }
    return previous;
}

const PC_DI_MOUSE_STATE *NuKeyboardOwner::PollMouse() {
    if ((nu_keyboard_device_flags & 1) || mouse == NULL || !mouse_enabled) {
        return NULL;
    }
    if (mouse->methods->poll(mouse) < 0) {
        mouse->methods->acquire(mouse);
        i32 old = mouse_cooperative;
        mouse_cooperative = old == 0;
        SetMouseCooperative(old);
        return NULL;
    }
    PcMouseSampleIndex ^= 1;
    mouse->methods->get_state(mouse, 20, &PcMouseSamples[PcMouseSampleIndex]);
    return &PcMouseSamples[PcMouseSampleIndex];
}

extern "C" void fun_006d5860(void) {
}

extern "C" {
    i32 dat_00824fb8 = 20;
}
// clang-format off: nested arrays wider than the column limit
PC_INPUT_CONFIG PcStaticInputConfigs[24] = {
    {"USB Joystick     ",
     0x30e8f,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"XBOX 360 For Windows (Controller)",
     0x28e045e,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0a, 0x00, 0x01, 0x00, 0x05, 0x00},
     16,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x03, 0x03}, {0x02, 0x04, 0x04}},
       {{0x00, 0x00, 0x00}, {0x02, 0x06, 0x06}, {0x00, 0x00, 0x00}, {0x02, 0x07, 0x07}}}},
     {{0x00, 0x00},
      {0x00, 0x00},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x02},
      {0x01, 0x06},
      {0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x07},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Controller (Xbox 360 Wireless Receiver for Windows)",
     0x719045e,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0a, 0x00, 0x01, 0x00, 0x05, 0x00},
     16,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x03, 0x03}, {0x02, 0x04, 0x04}},
       {{0x00, 0x00, 0x00}, {0x02, 0x06, 0x06}, {0x00, 0x00, 0x00}, {0x02, 0x07, 0x07}}}},
     {{0x00, 0x00},
      {0x00, 0x00},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x02},
      {0x01, 0x06},
      {0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x07},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"PS Vibration Feedback Converter ",
     0x50c12,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x08},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Rockfire PSX\\USB Bridge",
     0x20500583,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x06, 0x06}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x02},
      {0x01, 0x0a},
      {0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x0b},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Logic3 12 button vibration",
     0x31207b5,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x01},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Interact Gaming Device",
     0x262a05fd,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0a, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x02, 0x02}, {0x02, 0x05, 0x05}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x01},
      {0x01, 0x04},
      {0x01, 0x03},
      {0x01, 0x00},
      {0x01, 0x0a},
      {0x01, 0x02},
      {0x01, 0x05},
      {0x01, 0x0b},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Keyboard",
     0x1,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
     0,
     0,
     5,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x0a, 0x1e, 0x20}, {0x0a, 0x11, 0x1f}, {0x0a, 0xcb, 0xcd}, {0x0a, 0xd0, 0xc8}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x07, 0x10},
      {0x07, 0x49},
      {0x07, 0x02},
      {0x07, 0x37},
      {0x04, 0x02},
      {0x04, 0x01},
      {0x04, 0x00},
      {0x07, 0x52},
      {0x07, 0x3b},
      {0x07, 0x1d},
      {0x07, 0x9d},
      {0x07, 0x3c},
      {0x07, 0xc8},
      {0x07, 0xcd},
      {0x07, 0xd0},
      {0x07, 0xcb}}},
    {"Keyboard",
     0x2,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
     0,
     0,
     5,
     0,
     1,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x0a, 0x1e, 0x20}, {0x0a, 0x11, 0x1f}, {0x0a, 0xcb, 0xcd}, {0x0a, 0xd0, 0xc8}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x07, 0x10},
      {0x07, 0x49},
      {0x07, 0x02},
      {0x07, 0x37},
      {0x04, 0x02},
      {0x04, 0x01},
      {0x04, 0x00},
      {0x07, 0x52},
      {0x07, 0x3b},
      {0x07, 0x1d},
      {0x07, 0x9d},
      {0x07, 0x3c},
      {0x07, 0xc8},
      {0x07, 0xcd},
      {0x07, 0xd0},
      {0x07, 0xcb}}},
    {"Keyboard",
     0x3,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
     0,
     0,
     5,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x0a, 0x1e, 0x20}, {0x0a, 0x11, 0x1f}, {0x0a, 0xcb, 0xcd}, {0x0a, 0xd0, 0xc8}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x07, 0x10},
      {0x07, 0x12},
      {0x07, 0x02},
      {0x07, 0x04},
      {0x07, 0x48},
      {0x07, 0x4d},
      {0x07, 0x50},
      {0x07, 0x4b},
      {0x07, 0x3b},
      {0x07, 0x1d},
      {0x07, 0x9d},
      {0x07, 0x3c},
      {0x07, 0x14},
      {0x07, 0x23},
      {0x07, 0x22},
      {0x07, 0x21}}},
    {"Keyboard",
     0x4,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
     0,
     0,
     5,
     0,
     1,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x0a, 0x1e, 0x20}, {0x0a, 0x11, 0x1f}, {0x0a, 0xcb, 0xcd}, {0x0a, 0xd0, 0xc8}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x07, 0x10},
      {0x07, 0x49},
      {0x07, 0x02},
      {0x07, 0x37},
      {0x04, 0x02},
      {0x04, 0x01},
      {0x04, 0x00},
      {0x07, 0x52},
      {0x07, 0x3b},
      {0x07, 0x1d},
      {0x07, 0x9d},
      {0x07, 0x3c},
      {0x07, 0xc8},
      {0x07, 0xcd},
      {0x07, 0xd0},
      {0x07, 0xcb}}},
    {"PLAYSTATION(R)3 Controller",
     0x268054c,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x02, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x00, 0x05, 0x05}, {0x00, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x0c},
      {0x01, 0x0d},
      {0x01, 0x0e},
      {0x01, 0x0f},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07}}},
    {"Saitek P2900 Wireless Pad",
     0x40c06a3,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0e, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x08},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"4 axis 16 button joystick",
     0x6676666,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x10, 0x00, 0x00, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x02, 0x02}, {0x02, 0x05, 0x05}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x0c},
      {0x01, 0x0d},
      {0x01, 0x0e},
      {0x01, 0x0f}}},
    {"MP-8888 USB Joypad",
     0x88880925,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x02, 0x02}, {0x02, 0x05, 0x05}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x08},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"2 in 1 DT",
     0xb320044f,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x06, 0x06}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Logitech RumblePad 2 USB",
     0xc218046d,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     10,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x02, 0x02}, {0x02, 0x05, 0x05}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Logitech Dual Action",
     0xc216046d,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     8,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x02, 0x02}, {0x02, 0x05, 0x05}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Dual Analog Rumble Pad",
     0x1100f30,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     16,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x01},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Thrustmaster vibrating gamepad",
     0xb312044f,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x06, 0x06}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"4-Axis,12-Button with POV",
     0xa0090583,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x06, 0x06}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Saitek P2600 Rumble Force Pad",
     0x5f0d06a3,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x08},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"T Mini",
     0xd001044f,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x06, 0x06}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Dual Analog Pad",
     0x1120f30,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x01},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}}};

extern "C" i32 fun_006d5870(void) {
    return dat_00824fb8;
}

extern "C" i32 fun_006d5880(void) {
    return dat_00824fb8;
}

extern "C" void fun_006d5890(void) {
}

void NuKeyboardOwner::RescanDevices() {
    PC_INPUT_DEVICE *devices = ((PC_INPUT_RUNTIME *)nu_input_state)->devices;
    PC_INPUT_RUNTIME *runtime = (PC_INPUT_RUNTIME *)nu_input_state;
    device_count = 0;
    PcXInputDeviceCount = 0;
    for (i32 port = 0; port < 4; ++port) {
        PC_XINPUT_CAPABILITIES caps;
        PC_INPUT_DEVICE *record = &devices[port];
        if (XInputGetCapabilities(port, 1, (XINPUT_CAPABILITIES *)&caps) == 0) {
            record->capabilities_valid = 1;
            runtime->InitXInputCaps(&caps, record);
            record->config_dirty = 1;
            ++device_count;
        } else {
            record->kind = PC_INPUT_DEVICE_NONE;
            record->capabilities_valid = 0;
            record->device = NULL;
        }
    }
    for (i32 port = 4; port < 10; ++port) {
        PC_INPUT_DEVICE *record = &devices[port];
        if (record->device != NULL) {
            record->device->methods->unacquire(record->device);
            if (record->device != NULL) {
                record->device->methods->release(record->device);
                record->device = NULL;
            }
        }
        record->kind = PC_INPUT_DEVICE_NONE;
        record->capabilities_valid = 0;
    }
    if (nu_keyboard_owner.device != NULL) {
        nu_keyboard_owner.device->methods->unacquire(nu_keyboard_owner.device);
        if (nu_keyboard_owner.device != NULL) {
            nu_keyboard_owner.device->methods->release(nu_keyboard_owner.device);
            nu_keyboard_owner.device = NULL;
        }
        for (i32 port = 10; port < 14; ++port) {
            devices[port].device = NULL;
            devices[port].kind = PC_INPUT_DEVICE_NONE;
            devices[port].capabilities_valid = 0;
        }
    }
    device_count = 0;
    PcXInputDeviceCount = 0;
    ((PC_DI_FACTORY_INTERFACE *)factory)->EnumDevices(3, InputEnumDevices, NULL, 1);
    ((PC_DI_FACTORY_INTERFACE *)factory)->EnumDevices(2, InputEnumDevices, NULL, 1);
    ((PC_DI_FACTORY_INTERFACE *)factory)->EnumDevices(4, InputEnumDevices, NULL, 1);
}

extern "C" void fun_006da360(i32 player) {
    PC_INPUT_RUNTIME *state = (PC_INPUT_RUNTIME *)nu_input_state;
    PC_INPUT_CONFIG config;
    i32 port = NuPadGetPort(player);
    state->ExportConfig(port, &config, 0, 0, 0);
    state->PromoteConfig(state->primary, 64, &config);
    state->SaveConfiguration();
}

/* Add out-of-line definitions to existing pc_input.cpp. The actual global
   nu_keyboard_owner in data.cpp then uses genuine VC8 CRT construction/destruction. */
void NuKeyboardOwner::ScheduleRescan(f32 interval, i32 repetitions) {
    unknown_0000 = interval;
    unknown_0004 = repetitions;
    unknown_0008 = interval;
}

i32 NuKeyboardOwner::fun_006d5970() {
    return mouse != NULL;
}

i32 NuKeyboardOwner::fun_006d5980() {
    return device != NULL;
}

u8 *NuKeyboardOwner::PollKeyboard() {
    u8 *page;
    i8 page_index;
    i32 i;
    i32 next_key_index;
    i32 key_index;
    i32 key_count;

    poll_status = 0;
    if ((nu_keyboard_device_flags & 2) == 0 && device != 0) {
        poll_status = 1;
        if (device->methods->poll(device) < 0) {
            if (device->methods->acquire(device) < 0) {
                page = nu_keyboard_pages + (i8)nu_keyboard_page_index * 0x100;
                memset(page, 0, nu_keyboard_owner.key_count);
                return page;
            }
            device->methods->poll(device);
        }

        nu_keyboard_page_index ^= 1;
        if (device->methods->get_state(device, 0x100, nu_keyboard_pages + (i8)nu_keyboard_page_index * 0x100) == 0) {
            page_index = (i8)nu_keyboard_page_index;
            key_count = nu_keyboard_owner.key_count;
            key_index = nu_keyboard_key_index;
            nu_keyboard_previous_key_index = key_index;
            next_key_index = 0;
            for (i = 0; i < key_count; ++i) {
                if (nu_keyboard_pages[page_index * 0x100 + i] != 0 && i != key_index) {
                    next_key_index = i;
                    break;
                }
            }

            if (nu_keyboard_repeat_countdown != 0) {
                --nu_keyboard_repeat_countdown;
                if (next_key_index != 0) {
                    nu_keyboard_repeat_countdown = 6;
                }
                memset(nu_keyboard_pages + page_index * 0x100, 0, key_count);
            } else if (next_key_index != 0) {
                nu_keyboard_key_index = key_index = next_key_index;
                goto check_none;
            }
            if (key_index != 0 && nu_keyboard_pages[page_index * 0x100 + key_index] == 0) {
                nu_keyboard_key_index = key_index = 0;
            }
        check_none:
            if (key_index == 0) {
                nu_keyboard_no_key_state = 0;
            }
            return nu_keyboard_pages + page_index * 0x100;
        }
    }

    page = nu_keyboard_pages + (i8)nu_keyboard_page_index * 0x100;
    memset(page, 0, nu_keyboard_owner.key_count);
    return page;
}

NuKeyboardOwner::NuKeyboardOwner() {
    unknown_0000 = 0.0f;
    unknown_0008 = 0.0f;
    mouse_enabled = 1;
    unknown_0004 = 0;
    device_count = 0;
    mouse_cooperative = 1;
    poll_status = 0;
    device = NULL;
    nu_keyboard_page_index = 0;
    memset(nu_keyboard_pages, 0, 256);
    memset(nu_keyboard_pages + 256, 0, 256);
}

NuKeyboardOwner::~NuKeyboardOwner() {
    unknown_0000 = 0.0f;
    unknown_0004 = 0;
    unknown_0008 = 0.0f;
    device_count = 0;
}

i32 NuKeyboardOwner::fun_006d5720(i32 *axes, i32 *buttons) {
    if (mouse_axes != 0 && mouse_buttons != 0) {
        *axes = mouse_axes;
        *buttons = mouse_buttons;
        return 1;
    }
    *axes = 0;
    *buttons = 0;
    return 0;
}

void NuKeyboardOwner::SetMouseEnabled(u8 enabled) {
    mouse_enabled = enabled;
}

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static f32 PCInputFabs(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

/* Eases the smoothed mouse axis towards the latest delta and returns it scaled, with a small dead zone. */
extern "C" f32 NuMouseSmoothAxis(i32 axis) {
    f32 value = nu_input_state->mouse_delta[axis] - nu_input_state->mouse_smoothed[axis];
    value = value * 0.125 + nu_input_state->mouse_smoothed[axis];
    nu_input_state->mouse_smoothed[axis] = value;
    value = value * 4.0;
    if (PCInputFabs(value) < 0.01f) {
        value = 0.0f;
    }
    return value;
}

extern "C" f32 NuMouseReadXRel(void) {
    return NuMouseSmoothAxis(0);
}

extern "C" f32 NuMouseReadYRel(void) {
    return NuMouseSmoothAxis(1);
}

extern "C" f32 NuMouseReadZRel(void) {
    return (f32)(nu_input_state->mouse_delta[2] * -4.0);
}

extern "C" f32 NuMouseReadXVel(void) {
    return (f32)(nu_input_state->mouse_delta[0] * -4.0);
}

extern "C" f32 NuMouseReadYVel(void) {
    return (f32)(nu_input_state->mouse_delta[1] * -4.0);
}

extern "C" f32 NuMouseReadZVel(void) {
    return (f32)(nu_input_state->mouse_delta[2] * -4.0);
}

extern "C" f32 fun_006d5c50(void) {
    return nu_input_state->cursor_x;
}

extern "C" f32 fun_006d5c60(void) {
    return nu_input_state->cursor_y;
}

extern "C" void fun_006d5c70(f32 threshold, i32 motion, f32 ramp) {
    ((PC_INPUT_RUNTIME *)nu_input_state)->fun_006d3630(threshold, motion, ramp);
}

extern "C" i32 fun_006d5ca0(i32 device, i32 binding, i32 auxiliary, i32 auxiliary_port) {
    if (auxiliary != 0) {
        return ((PC_INPUT_RUNTIME *)nu_input_state)->auxiliary_bindings[auxiliary_port][binding].kind;
    }
    return ((PC_INPUT_RUNTIME *)nu_input_state)->devices[device].bindings[binding].kind;
}

extern "C" void fun_006d5ce0(i32 device, i32 auxiliary, i32 auxiliary_port) {
    PC_INPUT_BINDING *bindings;
    i32 i;
    if (auxiliary != 0) {
        bindings = ((PC_INPUT_RUNTIME *)nu_input_state)->auxiliary_bindings[auxiliary_port];
    } else {
        if (device < 0) {
            return;
        }
        bindings = ((PC_INPUT_RUNTIME *)nu_input_state)->devices[device].bindings;
    }
    for (i = 0; i < 32; ++i) {
        bindings[i].kind = PC_BINDING_NONE;
        bindings[i].value = 0;
    }
}

extern "C" i32 fun_006d5d60(i32 device) {
    if (device >= 0) {
        return ((PC_INPUT_RUNTIME *)nu_input_state)->devices[device].kind != PC_INPUT_DEVICE_NONE;
    }
    return 0;
}
