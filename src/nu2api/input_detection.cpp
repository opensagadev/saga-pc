#include "nu2api/PCInput.h"
#include <WbemIdl.h>
#include <stdio.h>
#include <wchar.h>

extern "C" void fun_006d3560(void) {
}

void PC_INPUT_RUNTIME::SetSuppressKey(u8 value) {
    suppress_key_1 = value;
}

void PC_INPUT_RUNTIME::SetActionKeysActive(i32 port, u8 value) {
    devices[port].action_keys_active = value;
}

void PC_INPUT_RUNTIME::SetCapturedPort(i32 port) {
    captured_port = port;
}

i32 PC_INPUT_RUNTIME::GetCapturedPort() {
    return captured_port;
}

void PC_INPUT_RUNTIME::ResetShutdownState() {
    shutdown_state = 0;
}

void PC_INPUT_RUNTIME::SetThresholds(f32 threshold, i32 motion, f32 ramp) {
    if (threshold >= 0.0f && threshold <= 1.0f) {
        axis_threshold = threshold * 32767.0;
    }
    if (motion > 0) {
        motion_threshold = motion;
    }
    if (ramp >= 0.0f) {
        ramp_seconds = ramp;
    }
}

extern "C" PC_INPUT_RUNTIME *PcInputGetRuntime(void) {
    return nu_input_state;
}

void PC_INPUT_RUNTIME::fun_006d36d0() {
    for (i32 port = 0; port < 14; ++port) {
        for (i32 action = 0; action < 32; ++action) {
            devices[port].reset_pairs[action].first = 0;
            devices[port].reset_pairs[action].second = 0;
        }
    }
}

void PC_INPUT_RUNTIME::GetDeviceCounts(i32 port, i32 *buttons, i32 *axes, i32 *povs) {
    *buttons = devices[port].button_count;
    *axes = devices[port].axis_count;
    *povs = devices[port].pov_count;
}

i32 PC_INPUT_RUNTIME::fun_006d3750(PC_INPUT_CONFIG *records, i32 count, PC_INPUT_CONFIG *config) {
    for (i32 index = 0; index < count; ++index) {
        if (config->product_id == records[index].product_id && config->key == records[index].key) {
            return index;
        }
    }
    return -1;
}

i32 PC_INPUT_RUNTIME::fun_006d37a0(i32 previous, i32 value) {
    if (previous & 0x80000000) {
        return (previous * 2 + 1) ^ value ^ 0x12345678;
    }
    return (previous * 2) ^ value ^ 0x12345678;
}

extern const GUID PcWbemClassId = {0x4590f811, 0x1d3a, 0x11d0, {0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};
extern const GUID PcWbemInterfaceId = {0xdc12a687, 0x737f, 0x11cf, {0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}};

// The DirectX SDK "XInput and DirectInput" IsXInputDevice sample.
extern "C" BOOL IsXInputProduct(const GUID *product) {
    IWbemLocator *locator = NULL;
    IEnumWbemClassObject *enumerator = NULL;
    IWbemClassObject *devices[20] = {NULL};
    IWbemServices *services = NULL;
    BSTR namespace_name = NULL;
    BSTR device_id = NULL;
    BSTR class_name = NULL;
    DWORD returned = 0;
    bool is_xinput = false;
    UINT index = 0;
    VARIANT value;
    HRESULT hr;

    hr = CoInitialize(NULL);
    bool cleanup_com = SUCCEEDED(hr);

    hr = CoCreateInstance(PcWbemClassId, NULL, CLSCTX_INPROC_SERVER, PcWbemInterfaceId, (LPVOID *)&locator);
    if (FAILED(hr) || locator == NULL) {
        goto cleanup;
    }

    namespace_name = SysAllocString(L"\\\\.\\root\\cimv2");
    if (namespace_name == NULL) {
        goto cleanup;
    }
    class_name = SysAllocString(L"Win32_PNPEntity");
    if (class_name == NULL) {
        goto cleanup;
    }
    device_id = SysAllocString(L"DeviceID");
    if (device_id == NULL) {
        goto cleanup;
    }

    hr = locator->ConnectServer(namespace_name, NULL, NULL, 0L, 0L, NULL, NULL, &services);
    if (FAILED(hr) || services == NULL) {
        goto cleanup;
    }

    CoSetProxyBlanket(services, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL,
                      EOAC_NONE);

    hr = services->CreateInstanceEnum(class_name, 0, NULL, &enumerator);
    if (FAILED(hr) || enumerator == NULL) {
        goto cleanup;
    }

    for (;;) {
        hr = enumerator->Next(10000, 20, devices, &returned);
        if (FAILED(hr)) {
            goto cleanup;
        }
        if (returned == 0) {
            break;
        }

        for (index = 0; index < returned; index++) {
            hr = devices[index]->Get(device_id, 0L, &value, NULL, NULL);
            if (SUCCEEDED(hr) && value.vt == VT_BSTR && value.bstrVal != NULL) {
                if (wcsstr(value.bstrVal, L"IG_")) {
                    DWORD pid = 0;
                    DWORD vid = 0;
                    WCHAR *vid_text = wcsstr(value.bstrVal, L"VID_");
                    if (vid_text && swscanf(vid_text, L"VID_%4X", &vid) != 1) {
                        vid = 0;
                    }
                    WCHAR *pid_text = wcsstr(value.bstrVal, L"PID_");
                    if (pid_text && swscanf(pid_text, L"PID_%4X", &pid) != 1) {
                        pid = 0;
                    }

                    DWORD vid_pid = MAKELONG(vid, pid);
                    if (vid_pid == product->Data1) {
                        is_xinput = true;
                        goto cleanup;
                    }
                }
            }
            if (devices[index]) {
                devices[index]->Release();
                devices[index] = NULL;
            }
        }
    }

cleanup:
    if (namespace_name) {
        SysFreeString(namespace_name);
    }
    if (device_id) {
        SysFreeString(device_id);
    }
    if (class_name) {
        SysFreeString(class_name);
    }
    for (index = 0; index < sizeof(devices) / sizeof(devices[0]); index++) {
        if (devices[index]) {
            devices[index]->Release();
            devices[index] = NULL;
        }
    }
    if (enumerator) {
        enumerator->Release();
        enumerator = NULL;
    }
    if (locator) {
        locator->Release();
        locator = NULL;
    }
    if (services) {
        services->Release();
        services = NULL;
    }

    if (cleanup_com) {
        CoUninitialize();
    }

    return is_xinput;
}
