#include "saga.h"
#include <string.h>

extern "C" const char PCKeyboardMouse1Name_candidate[15], PCKeyboardMouse2Name_candidate[16],
    PCKeyboard1Name_candidate[10], PCKeyboard2Name_candidate[10];
PC_INPUT_CONFIG_PC *PC_INPUT_RUNTIME_PC::FindConfig(u8 mask, i32 port, i32 key) {
    if (port < 0)
        return NULL;
    if (mask & 4)
        for (i32 i = 0; i < 64; ++i)
            if (secondary[i].product_id == devices[port].product.Data1 && (key < 0 || secondary[i].key == key))
                return &secondary[i];
    if (mask & 2)
        for (i32 i = 0; i < 64; ++i)
            if (primary[i].product_id == devices[port].product.Data1 && (key < 0 || primary[i].key == key))
                return &primary[i];
    if (mask & 1)
        for (u32 i = 0; i < sizeof(PcStaticInputConfigs) / sizeof(PcStaticInputConfigs[0]); ++i)
            if (PcStaticInputConfigs[i].product_id == devices[port].product.Data1 &&
                (key < 0 || PcStaticInputConfigs[i].key == key))
                return &PcStaticInputConfigs[i];
    return NULL;
}
void PC_INPUT_RUNTIME_PC::InitXInputCaps(const PC_XINPUT_CAPABILITIES_PC *caps, PC_INPUT_DEVICE_PC *record) {
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
        for (i32 bit = 0; bit < 32; ++bit)
            if (caps->gamepad.buttons & (1 << bit))
                ++record->button_count;
        if ((caps->gamepad.buttons & 15) == 15) {
            record->button_count -= 4;
            record->pov_count = 1;
        }
    }
}
void PC_INPUT_RUNTIME_PC::BindPreferredConfig(i32 mask, i32 port) {
    PC_INPUT_CONFIG_PC *config = FindConfig((u8)mask, port, preferred_key);
    if (!config)
        config = FindConfig((u8)mask, port, -1);
    if (config)
        InstallConfig(config, port, config->key, 0, 0);
}
void PC_INPUT_RUNTIME_PC::InstallConfig(PC_INPUT_CONFIG_PC *config, i32 port, i32 key, i32 auxiliary,
                                        i32 auxiliary_port) {
    PC_INPUT_BINDING_PC *bindings;
    if (auxiliary) {
        auxiliary_ports[auxiliary_port] = port;
        bindings = auxiliary_bindings[auxiliary_port];
    } else
        bindings = devices[port].bindings;
    devices[port].config_present = config != NULL;
    devices[port].config_dirty = 0;
    for (i32 i = 0; i < 32; ++i) {
        bindings[i].kind = 0;
        bindings[i].value = 0;
    }
    if (config != NULL) {
        devices[port].config_installed = 1;
        if (!auxiliary) {
            devices[port].deadzone = config->deadzone;
            devices[port].alternate_deadzone = config->alternate_deadzone;
        }
        PC_INPUT_BINDING_PC *binding = bindings;
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
                            binding->kind = 7;
                            if (commands[i].active == 2)
                                binding->value = secondary;
                            else
                                binding->value = primary;
                            break;
                        case 11:
                            binding->kind = 1;
                            if (commands[i].active == 2)
                                binding->value = secondary;
                            else
                                binding->value = primary;
                            break;
                        case 2:
                        case 3:
                        case 5:
                        case 6:
                        case 12:
                        case 13:
                            binding->kind = kind;
                            binding->value = secondary;
                            break;
                    }
                }
                if ((u32)button <= 15 && binding->kind == 0) {
                    i32 kind = config->buttons[button][0];
                    i32 value = config->buttons[button][1];
                    switch (kind) {
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                        case 5:
                        case 6:
                        case 7:
                            binding->kind = kind;
                            binding->value = value;
                            break;
                    }
                }
            }
        }
    }
}

void PC_INPUT_RUNTIME_PC::InitKeyboardProfiles() {
    PC_INPUT_CONFIG_PC *config;
    NuStrCpy(devices[10].name, PCKeyboardMouse1Name_candidate);
    devices[10].kind = 1;
    devices[10].product.Data1 = 1;
    devices[10].config_dirty = 1;
    devices[10].capabilities_valid = 1;
    devices[10].button_count = 255;
    config = FindConfig(3, 10, preferred_key);
    if (!config)
        config = FindConfig(3, 10, -1);
    if (config)
        InstallConfig(config, 10, config->key, 0, 0);
    devices[11].device = nu_keyboard_owner.device;
    NuStrCpy(devices[11].name, PCKeyboard1Name_candidate);
    devices[11].kind = 3;
    devices[11].product.Data1 = 3;
    devices[11].config_dirty = 1;
    devices[11].capabilities_valid = 1;
    devices[11].button_count = 255;
    config = FindConfig(3, 11, preferred_key);
    if (!config)
        config = FindConfig(3, 11, -1);
    if (config)
        InstallConfig(config, 11, config->key, 0, 0);
    NuStrCpy(devices[12].name, PCKeyboardMouse2Name_candidate);
    devices[12].kind = 2;
    devices[12].product.Data1 = 2;
    devices[12].config_dirty = 1;
    devices[12].capabilities_valid = 1;
    devices[12].button_count = 255;
    config = FindConfig(3, 12, preferred_key);
    if (!config)
        config = FindConfig(3, 12, -1);
    if (config)
        InstallConfig(config, 12, config->key, 0, 0);
    devices[13].device = nu_keyboard_owner.device;
    NuStrCpy(devices[13].name, PCKeyboard2Name_candidate);
    devices[13].kind = 4;
    devices[13].product.Data1 = 4;
    devices[13].config_dirty = 1;
    devices[13].capabilities_valid = 1;
    devices[13].button_count = 255;
    config = FindConfig(3, 13, preferred_key);
    if (!config)
        config = FindConfig(3, 13, -1);
    if (config)
        InstallConfig(config, 13, config->key, 0, 0);
}

struct PC_DI_RANGE_PC {
    DWORD size, header_size, object, how;
    LONG minimum, maximum;
};
STATIC_ASSERT(sizeof(PC_DI_RANGE_PC) == 24);
STATIC_ASSERT(offsetof(PC_DI_RANGE_PC, object) == 8);
STATIC_ASSERT(offsetof(PC_DI_RANGE_PC, how) == 12);
STATIC_ASSERT(offsetof(PC_DI_RANGE_PC, minimum) == 16);
STATIC_ASSERT(offsetof(PC_DI_RANGE_PC, maximum) == 20);
/* TODO(saga.h): the DirectInput device is called through its C++ interface in retail. */
struct PC_DI_DEVICE_INTERFACE_PC {
    virtual HRESULT __stdcall QueryInterface(REFIID, void **) = 0;
    virtual ULONG __stdcall AddRef(void) = 0;
    virtual ULONG __stdcall Release(void) = 0;
    virtual HRESULT __stdcall GetCapabilities(PC_DI_CAPS_PC *) = 0;
    virtual HRESULT __stdcall EnumObjects(void *, void *, DWORD) = 0;
    virtual HRESULT __stdcall GetProperty(REFGUID, PC_DI_PROPERTY_PC *) = 0;
    virtual HRESULT __stdcall SetProperty(REFGUID, const PC_DI_PROPERTY_PC *) = 0;
};
struct PC_DI_FACTORY_INTERFACE_PC {
    virtual HRESULT __stdcall QueryInterface(REFIID, void **) = 0;
    virtual ULONG __stdcall AddRef(void) = 0;
    virtual ULONG __stdcall Release(void) = 0;
    virtual HRESULT __stdcall CreateDevice(REFGUID, NuKeyboardDevice_PC **, IUnknown *) = 0;
    virtual HRESULT __stdcall EnumDevices(DWORD, BOOL(__stdcall *)(const PC_DI_INSTANCE_PC *, void *), void *,
                                          DWORD) = 0;
};
extern "C" void ConfigureJoystickRangesPc(NuKeyboardDevice_PC *device) {
    if (device == NULL)
        return;
    PC_DI_RANGE_PC range;
    PC_DI_DEVICE_INTERFACE_PC *input = (PC_DI_DEVICE_INTERFACE_PC *)device;
    range.size = 24;
    range.header_size = 16;
    range.how = 1;
    range.minimum = -32768;
    range.maximum = 32767;
    range.object = 0;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0x4;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0x8;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xc;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0x10;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0x14;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0x18;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0x1c;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xb0;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xb4;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xb8;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xbc;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xc0;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xc4;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xc8;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xcc;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xd0;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xd4;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xd8;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xdc;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xe0;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xe4;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xe8;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xec;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xf0;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xf4;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xf8;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0xfc;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0x100;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0x104;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0x108;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    range.object = 0x10c;
    input->SetProperty(*(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
}

extern "C" BOOL __stdcall InputEnumDevicesPc(const PC_DI_INSTANCE_PC *instance, void *unused) {
    PC_INPUT_RUNTIME_PC *state = (PC_INPUT_RUNTIME_PC *)nu_input_state;
    PC_INPUT_DEVICE_PC *devices = state->devices;
    if (IsXInputProductPc(&instance->product)) {
        i32 port = PcXInputDeviceCount;
        PcInputSelectedPort = port;
        devices[port].product = instance->product;
        i32 length = (i32)strlen(instance->product_name);
        if (length > 63)
            length = 63;
        memset(devices[port].name, 0, 63);
        PCByteCopy_candidate(devices[port].name, instance->product_name, length);
        devices[port].kind = 6;
        devices[port].capabilities_valid = 0;
        devices[port].deadzone = 64;
        PC_XINPUT_CAPABILITIES_PC capabilities;
        if (XInputGetCapabilities(port, 1, (XINPUT_CAPABILITIES *)&capabilities) == 0) {
            state->InitXInputCaps(&capabilities, &devices[port]);
            if (devices[port].button_count == 10 && devices[port].axis_count == 5 && devices[port].pov_count == 1)
                devices[port].product.Data1 = 0x028e045e;
        }
        PC_INPUT_CONFIG_PC *config = state->FindConfig(7, port, state->preferred_key);
        if (!config)
            config = state->FindConfig(7, port, -1);
        if (config)
            state->InstallConfig(config, port, config->key, 0, 0);
        ++PcXInputDeviceCount;
        return TRUE;
    }
    u32 type = (u8)instance->device_type;
    if (type != 19) {
        if (type - 20 > 1)
            return TRUE;
        NuKeyboardDevice_PC *device;
        if (nu_keyboard_owner.factory->methods->create_device(nu_keyboard_owner.factory, instance->instance, &device,
                                                              NULL) < 0)
            return TRUE;
        i32 port = 4;
        PC_INPUT_DEVICE_PC *record = &devices[port];
        while (record->kind != 0) {
            ++port;
            ++record;
            if (port >= 10)
                return TRUE;
        }
        if (PcInputSelectedPort == -1)
            PcInputSelectedPort = port;
        PC_DI_CAPS_PC caps;
        caps.size = sizeof(caps);
        HRESULT result = device->methods->get_caps(device, &caps);
        record->button_count = (i8)caps.buttons;
        record->capabilities_valid = result == 0;
        record->pov_count = (u8)caps.povs;
        record->device = device;
        record->axis_count = (u8)caps.axes;
        record->deadzone = 32;
        record->kind = 5;
        record->config_dirty = 1;
        record->product = instance->product;
        i32 length = (i32)strlen(instance->product_name);
        if (length > 63)
            length = 63;
        memset(record->name, 0, 63);
        PCByteCopy_candidate(record->name, instance->product_name, length);
        if (NuStrIStr((char *)instance->product_name, "Saitek") != NULL &&
            (NuStrIStr((char *)instance->product_name, "P2600") != NULL ||
             NuStrIStr((char *)instance->product_name, "P2900") != NULL))
            record->button_count -= 2;
        device->methods->set_format(device, &PcJoystickDataFormat);
        device->methods->set_cooperative_level(device, (HWND)PCInputInstance_candidate, 9);
        device->methods->acquire(device);
        ConfigureJoystickRangesPc(device);
        state->BindPreferredConfig(7, port);
        ++nu_keyboard_owner.device_count;
        return TRUE;
    }
    if (nu_keyboard_owner.factory->methods->create_device(nu_keyboard_owner.factory, PcSystemKeyboardId,
                                                          &nu_keyboard_owner.device, NULL) != 0)
        return TRUE;
    PcKeyboardCapabilities.size = sizeof(PcKeyboardCapabilities);
    nu_keyboard_owner.device->methods->set_cooperative_level(nu_keyboard_owner.device, (HWND)PCInputInstance_candidate,
                                                             22);
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

void *__cdecl operator new(size_t size);

void NuKeyboardOwner_PC::Setup(HWND window, HINSTANCE instance) {
    DirectInput8Create(instance, 0x800, PcDirectInput8AId, (void **)&factory, NULL);
    factory->methods->create_device(factory, PcSystemMouseId, &mouse, NULL);
    PC_DI_CAPS_PC capabilities;
    capabilities.size = sizeof(capabilities);
    mouse->methods->get_caps(mouse, &capabilities);
    if (capabilities.flags & 1) {
        mouse_axes = capabilities.axes;
        mouse_buttons = capabilities.buttons;
        mouse->methods->set_cooperative_level(mouse, window, 5);
        mouse->methods->set_format(mouse, &PcMouseDataFormat);
        PC_DI_PROPERTY_PC property;
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
    nu_input_state = (NuKeyboardState_PC *)new PC_INPUT_RUNTIME_PC;
    NuKeyboardApplyActionKeys_candidate(NULL, 0);
    device_count = 0;
    PcXInputDeviceCount = 0;
    factory->methods->enum_devices(factory, 3, InputEnumDevicesPc, NULL, 1);
    factory->methods->enum_devices(factory, 2, InputEnumDevicesPc, NULL, 1);
    factory->methods->enum_devices(factory, 4, InputEnumDevicesPc, NULL, 1);
    nu_keyboard_owner.unknown_0000 = 0.2f;
    nu_keyboard_owner.unknown_0008 = 0.2f;
    PcInputSelectedPort = -1;
    nu_keyboard_owner.unknown_0004 = 3;
}

void NuKeyboardOwner_PC::Shutdown() {
    if (nu_input_state == NULL)
        return;
    PC_INPUT_DEVICE_PC *devices = ((PC_INPUT_RUNTIME_PC *)nu_input_state)->devices;
    if (device != NULL) {
        device->methods->unacquire(device);
        if (device != NULL) {
            device->methods->release(device);
            device = NULL;
        }
    }
    for (u16 index = 10; index < 14; ++index)
        devices[index].device = NULL;
    for (i32 index = 4; index < 14; ++index) {
        if (devices[index].device != NULL) {
            devices[index].device->methods->unacquire(devices[index].device);
            if (devices[index].device != NULL) {
                devices[index].device->methods->release(devices[index].device);
                devices[index].device = NULL;
            }
        }
        devices[index].kind = 0;
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
    PC_INPUT_RUNTIME_PC *released = (PC_INPUT_RUNTIME_PC *)nu_input_state;
    if (released != NULL) {
        released->shutdown_state = 0;
        ::operator delete(released);
    }
}

STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, kind) == 0);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, device) == 4);
STATIC_ASSERT(offsetof(PC_INPUT_DEVICE_PC, capabilities_valid) == 0x2d);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, devices) == 0x4e34);
STATIC_ASSERT(offsetof(PC_INPUT_RUNTIME_PC, shutdown_state) == 0x67e4);
STATIC_ASSERT(offsetof(PC_DI_FACTORY_METHODS_PC, release) == 8);

extern "C" void __cdecl NuKeyboardClearActiveDevicesPc(void) {
    PC_INPUT_RUNTIME_PC *state = (PC_INPUT_RUNTIME_PC *)nu_input_state;
    for (i32 device = 10; device < 14; ++device)
        state->devices[device].action_keys_active = 0;
}

void PC_INPUT_RUNTIME_PC::AutoConfigurePadsPc(i32 count) {
    i32 device = 0;
    for (i32 pad = 0; pad < count; ++pad) {
        i32 linked = NuPadGetPort(pad);
        for (; device < 14; ++device) {
            if (devices[device].kind != 0 && (device < 10 || device > 13 || devices[device].action_keys_active)) {
                if (device != linked) {
                    NuPadMapPlayerToPort(pad, device);
                    NuPadSetValid(device, 1);
                    PC_INPUT_CONFIG_PC *config = FindConfig(7, device, preferred_key);
                    if (!config)
                        config = FindConfig(7, device, -1);
                    if (config)
                        InstallConfig(config, device, config->key, 0, 0);
                }
                ++device;
                break;
            }
        }
    }
}

extern "C" void __cdecl NuPadAutoConfigurePc(i32 count) {
    ((PC_INPUT_RUNTIME_PC *)nu_input_state)->AutoConfigurePadsPc(count);
}

extern "C" i32 __cdecl GamePadReadBackendPc(FRAME_PAD_PC *pad, i32 ignored) {
    (void)ignored;
    return NuPadRead(pad);
}

// Native NuKeyboardOwner_PC cohort, append to pc_input.cpp.
i32 NuKeyboardOwner_PC::SetMouseCooperativePc(i32 value) {
    i32 previous = mouse_cooperative;
    if (previous == value)
        return value;
    mouse_cooperative = value;
    if (mouse != NULL) {
        mouse->methods->unacquire(mouse);
        if (value)
            mouse->methods->set_cooperative_level(mouse, PCInputInstance_candidate, 5);
        else
            mouse->methods->set_cooperative_level(mouse, PCInputInstance_candidate, 6);
        mouse->methods->acquire(mouse);
    }
    return previous;
}

const PC_DI_MOUSE_STATE_PC *NuKeyboardOwner_PC::PollMousePc() {
    if ((nu_keyboard_device_flags & 1) || mouse == NULL || !mouse_enabled)
        return NULL;
    if (mouse->methods->poll(mouse) < 0) {
        mouse->methods->acquire(mouse);
        i32 old = mouse_cooperative;
        mouse_cooperative = old == 0;
        SetMouseCooperativePc(old);
        return NULL;
    }
    PcMouseSampleIndexPc ^= 1;
    mouse->methods->get_state(mouse, 20, &PcMouseSamplesPc[PcMouseSampleIndexPc]);
    return &PcMouseSamplesPc[PcMouseSampleIndexPc];
}

void NuKeyboardOwner_PC::RescanDevicesPc() {
    PC_INPUT_DEVICE_PC *devices = ((PC_INPUT_RUNTIME_PC *)nu_input_state)->devices;
    PC_INPUT_RUNTIME_PC *runtime = (PC_INPUT_RUNTIME_PC *)nu_input_state;
    device_count = 0;
    PcXInputDeviceCount = 0;
    for (i32 port = 0; port < 4; ++port) {
        PC_XINPUT_CAPABILITIES_PC caps;
        PC_INPUT_DEVICE_PC *record = &devices[port];
        if (XInputGetCapabilities(port, 1, (XINPUT_CAPABILITIES *)&caps) == 0) {
            record->capabilities_valid = 1;
            runtime->InitXInputCaps(&caps, record);
            record->config_dirty = 1;
            ++device_count;
        } else {
            record->kind = 0;
            record->capabilities_valid = 0;
            record->device = NULL;
        }
    }
    for (i32 port = 4; port < 10; ++port) {
        PC_INPUT_DEVICE_PC *record = &devices[port];
        if (record->device != NULL) {
            record->device->methods->unacquire(record->device);
            if (record->device != NULL) {
                record->device->methods->release(record->device);
                record->device = NULL;
            }
        }
        record->kind = 0;
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
            devices[port].kind = 0;
            devices[port].capabilities_valid = 0;
        }
    }
    device_count = 0;
    PcXInputDeviceCount = 0;
    ((PC_DI_FACTORY_INTERFACE_PC *)factory)->EnumDevices(3, InputEnumDevicesPc, NULL, 1);
    ((PC_DI_FACTORY_INTERFACE_PC *)factory)->EnumDevices(2, InputEnumDevicesPc, NULL, 1);
    ((PC_DI_FACTORY_INTERFACE_PC *)factory)->EnumDevices(4, InputEnumDevicesPc, NULL, 1);
}

/* Add out-of-line definitions to existing pc_input.cpp. The actual global
   nu_keyboard_owner in data.cpp then uses genuine VC8 CRT construction/destruction. */
void NuKeyboardOwner_PC::ScheduleRescan(f32 interval, i32 repetitions) {
    unknown_0000 = interval;
    unknown_0004 = repetitions;
    unknown_0008 = interval;
}

NuKeyboardOwner_PC::NuKeyboardOwner_PC() {
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
NuKeyboardOwner_PC::~NuKeyboardOwner_PC() {
    unknown_0000 = 0.0f;
    unknown_0004 = 0;
    unknown_0008 = 0.0f;
    device_count = 0;
}
void __thiscall NuKeyboardOwner_PC::SetMouseEnabledPc(u8 enabled) {
    mouse_enabled = enabled;
}

STATIC_ASSERT(offsetof(NuKeyboardState_PC, mouse_delta) == 0x6314);
STATIC_ASSERT(offsetof(NuKeyboardState_PC, mouse_smoothed) == 0x6320);

/* Eases the smoothed mouse axis towards the latest delta and returns it scaled, with a small dead zone. */
extern "C" f32 __cdecl NuMouseSmoothAxisPc(i32 axis) {
    f32 value = nu_input_state->mouse_delta[axis] - nu_input_state->mouse_smoothed[axis];
    f32 magnitude;
    u32 magnitude_bits;
    value = value * 0.125 + nu_input_state->mouse_smoothed[axis];
    nu_input_state->mouse_smoothed[axis] = value;
    value = value * 4.0;
    magnitude = value;
    magnitude_bits = *(u32 *)&magnitude & 0x7fffffff;
    if (*(f32 *)&magnitude_bits < 0.01f) {
        value = 0.0f;
    }
    return value;
}

extern "C" f32 __cdecl NuMouseReadXRel(void) {
    return NuMouseSmoothAxisPc(0);
}

extern "C" f32 __cdecl NuMouseReadYRel(void) {
    return NuMouseSmoothAxisPc(1);
}

extern "C" f32 __cdecl NuMouseReadZRel(void) {
    return (f32)(nu_input_state->mouse_delta[2] * -4.0);
}

extern "C" f32 __cdecl NuMouseReadXVel(void) {
    return (f32)(nu_input_state->mouse_delta[0] * -4.0);
}

extern "C" f32 __cdecl NuMouseReadYVel(void) {
    return (f32)(nu_input_state->mouse_delta[1] * -4.0);
}

extern "C" f32 __cdecl NuMouseReadZVel(void) {
    return (f32)(nu_input_state->mouse_delta[2] * -4.0);
}
