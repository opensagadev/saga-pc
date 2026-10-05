#include "saga.h"
#include <string.h>

extern "C" const char PCKeyboardMouse1Name_candidate[15], PCKeyboardMouse2Name_candidate[16],
    PCKeyboard1Name_candidate[10], PCKeyboard2Name_candidate[10];
PC_INPUT_CONFIG_PC *PC_INPUT_RUNTIME_PC::FindConfig(u8 mask, i32 port, i32 key) {
    if (port < 0)
        return NULL;
    u32 product = devices[port].product.Data1;
    if (mask & 4)
        for (i32 i = 0; i < 64; ++i)
            if (secondary[i].product_id == product && (key < 0 || secondary[i].key == key))
                return &secondary[i];
    if (mask & 2)
        for (i32 i = 0; i < 64; ++i)
            if (primary[i].product_id == product && (key < 0 || primary[i].key == key))
                return &primary[i];
    if (mask & 1)
        for (i32 i = 0; i < 15; ++i)
            if (PcStaticInputConfigs[i].product_id == product && (key < 0 || PcStaticInputConfigs[i].key == key))
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
        record->left_trigger = record->right_trigger = 0xffff;
    } else {
        record->left_trigger = caps->motor_left;
        record->right_trigger = caps->motor_right;
        for (i32 bit = 0; bit < 32; ++bit)
            if ((u32)caps->gamepad.buttons & (1u << bit))
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
        for (i32 i = 0; i < 32; ++i) {
            if (!commands[i].active)
                continue;
            i32 axis = commands[i].axis;
            if (axis >= 0) {
                u8 kind = config->axes[axis][0];
                u16 primary = config->axes[axis][1];
                u16 secondary = config->axes[axis][2];
                switch (kind) {
                    case 10:
                        bindings[i].kind = 7;
                        bindings[i].value = commands[i].active == 2 ? secondary : primary;
                        break;
                    case 11:
                        bindings[i].kind = 1;
                        bindings[i].value = commands[i].active == 2 ? secondary : primary;
                        break;
                    case 2:
                    case 3:
                    case 5:
                    case 6:
                    case 12:
                    case 13:
                        bindings[i].kind = kind;
                        bindings[i].value = secondary;
                        break;
                }
            }
            i32 button = commands[i].button;
            if ((u32)button <= 15 && bindings[i].kind == 0) {
                u8 kind = config->buttons[button][0];
                if (kind >= 1 && kind <= 7) {
                    bindings[i].kind = kind;
                    bindings[i].value = config->buttons[button][1];
                }
            }
        }
    }
}

void PC_INPUT_RUNTIME_PC::InitKeyboardProfiles() {
    {
        PC_INPUT_DEVICE_PC *record = &devices[10];
        NuStrCpy(record->name, PCKeyboardMouse1Name_candidate);
        record->kind = 1;
        record->product.Data1 = 1;
        record->config_dirty = 1;
        record->capabilities_valid = 1;
        record->button_count = 255;
        PC_INPUT_CONFIG_PC *config = FindConfig(3, 10, preferred_key);
        if (!config)
            config = FindConfig(3, 10, -1);
        if (config)
            InstallConfig(config, 10, config->key, 0, 0);
    }
    {
        PC_INPUT_DEVICE_PC *record = &devices[11];
        NuStrCpy(record->name, PCKeyboard1Name_candidate);
        record->kind = 3;
        record->product.Data1 = 3;
        record->device = nu_keyboard_owner.device;
        record->config_dirty = 1;
        record->capabilities_valid = 1;
        record->button_count = 255;
        PC_INPUT_CONFIG_PC *config = FindConfig(3, 11, preferred_key);
        if (!config)
            config = FindConfig(3, 11, -1);
        if (config)
            InstallConfig(config, 11, config->key, 0, 0);
    }
    {
        PC_INPUT_DEVICE_PC *record = &devices[12];
        NuStrCpy(record->name, PCKeyboardMouse2Name_candidate);
        record->kind = 2;
        record->product.Data1 = 2;
        record->config_dirty = 1;
        record->capabilities_valid = 1;
        record->button_count = 255;
        PC_INPUT_CONFIG_PC *config = FindConfig(3, 12, preferred_key);
        if (!config)
            config = FindConfig(3, 12, -1);
        if (config)
            InstallConfig(config, 12, config->key, 0, 0);
    }
    {
        PC_INPUT_DEVICE_PC *record = &devices[13];
        NuStrCpy(record->name, PCKeyboard2Name_candidate);
        record->kind = 4;
        record->product.Data1 = 4;
        record->device = nu_keyboard_owner.device;
        record->config_dirty = 1;
        record->capabilities_valid = 1;
        record->button_count = 255;
        PC_INPUT_CONFIG_PC *config = FindConfig(3, 13, preferred_key);
        if (!config)
            config = FindConfig(3, 13, -1);
        if (config)
            InstallConfig(config, 13, config->key, 0, 0);
    }
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
extern "C" void ConfigureJoystickRangesPc(NuKeyboardDevice_PC *device) {
    if (device == NULL)
        return;
    PC_DI_RANGE_PC range = {24, 16, 0, 1, -32768, 32767};
    for (DWORD offset = 0; offset <= 0x1c; offset += 4) {
        range.object = offset;
        device->methods->set_property(device, *(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    }
    for (DWORD offset = 0xb0; offset <= 0x10c; offset += 4) {
        range.object = offset;
        device->methods->set_property(device, *(const GUID *)4, (const PC_DI_PROPERTY_PC *)&range);
    }
}

extern "C" BOOL __stdcall InputEnumDevicesPc(const PC_DI_INSTANCE_PC *instance, void *unused) {
    PC_INPUT_RUNTIME_PC *state = (PC_INPUT_RUNTIME_PC *)nu_input_state;
    if (IsXInputProductPc(&instance->product)) {
        i32 port = PcXInputDeviceCount;
        PcInputSelectedPort = port;
        PC_INPUT_DEVICE_PC *record = &state->devices[port];
        record->product = instance->product;
        i32 length = (i32)strlen(instance->product_name);
        if (length > 63)
            length = 63;
        memset(record->name, 0, 63);
        PCByteCopy_candidate(record->name, instance->product_name, length);
        record->kind = 6;
        record->capabilities_valid = 0;
        record->deadzone = 64;
        PC_XINPUT_CAPABILITIES_PC capabilities;
        if (XInputGetCapabilities(port, 1, (XINPUT_CAPABILITIES *)&capabilities) == 0) {
            state->InitXInputCaps(&capabilities, record);
            if (record->button_count == 10 && record->axis_count == 5 && record->pov_count == 1)
                record->product.Data1 = 0x028e045e;
        }
        PC_INPUT_CONFIG_PC *config = state->FindConfig(7, port, state->preferred_key);
        if (!config)
            config = state->FindConfig(7, port, -1);
        if (config)
            state->InstallConfig(config, port, config->key, 0, 0);
        ++PcXInputDeviceCount;
        return TRUE;
    }
    u8 type = (u8)instance->device_type;
    if (type == 20 || type == 21) {
        NuKeyboardDevice_PC *device;
        if (nu_keyboard_owner.factory->methods->create_device(nu_keyboard_owner.factory, instance->instance, &device,
                                                              NULL) < 0)
            return TRUE;
        i32 port = 4;
        while (port < 10 && state->devices[port].kind != 0)
            ++port;
        if (port >= 10)
            return TRUE;
        if (PcInputSelectedPort == -1)
            PcInputSelectedPort = port;
        PC_INPUT_DEVICE_PC *record = &state->devices[port];
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
    } else if (type == 19) {
        if (nu_keyboard_owner.factory->methods->create_device(nu_keyboard_owner.factory, PcSystemKeyboardId,
                                                              &nu_keyboard_owner.device, NULL) != 0)
            return TRUE;
        PcKeyboardCapabilities.size = sizeof(PcKeyboardCapabilities);
        nu_keyboard_owner.device->methods->set_cooperative_level(nu_keyboard_owner.device,
                                                                 (HWND)PCInputInstance_candidate, 22);
        nu_keyboard_owner.device->methods->set_format(nu_keyboard_owner.device, &PcKeyboardDataFormat);
        nu_keyboard_owner.device->methods->acquire(nu_keyboard_owner.device);
        nu_keyboard_owner.device->methods->get_caps(nu_keyboard_owner.device, &PcKeyboardCapabilities);
        state->InitKeyboardProfiles();
        for (i32 port = 10; port < 14; ++port) {
            state->devices[port].device = nu_keyboard_owner.device;
            state->devices[port].button_count = (i16)PcKeyboardCapabilities.buttons;
        }
        nu_keyboard_owner.key_count = 256;
    }
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
        PC_DI_PROPERTY_PC property = {20, 16, 0, 0, 0};
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
    PC_INPUT_RUNTIME_PC *allocated = new PC_INPUT_RUNTIME_PC;
    nu_input_state = (NuKeyboardState_PC *)allocated;
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
    for (i32 index = 10; index < 14; ++index)
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
