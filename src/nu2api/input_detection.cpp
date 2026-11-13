#include "saga.h"
#include <WbemIdl.h>
#include <stdio.h>
#include <wchar.h>

extern const GUID PcWbemClassId, PcWbemInterfaceId;

// The DirectX SDK "XInput and DirectInput" IsXInputDevice sample.
extern "C" BOOL IsXInputProductPc(const GUID *product) {
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
    if (FAILED(hr) || locator == NULL)
        goto cleanup;

    namespace_name = SysAllocString(L"\\\\.\\root\\cimv2");
    if (namespace_name == NULL)
        goto cleanup;
    class_name = SysAllocString(L"Win32_PNPEntity");
    if (class_name == NULL)
        goto cleanup;
    device_id = SysAllocString(L"DeviceID");
    if (device_id == NULL)
        goto cleanup;

    hr = locator->ConnectServer(namespace_name, NULL, NULL, 0L, 0L, NULL, NULL, &services);
    if (FAILED(hr) || services == NULL)
        goto cleanup;

    CoSetProxyBlanket(services, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL,
                      RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

    hr = services->CreateInstanceEnum(class_name, 0, NULL, &enumerator);
    if (FAILED(hr) || enumerator == NULL)
        goto cleanup;

    for (;;) {
        hr = enumerator->Next(10000, 20, devices, &returned);
        if (FAILED(hr))
            goto cleanup;
        if (returned == 0)
            break;

        for (index = 0; index < returned; index++) {
            hr = devices[index]->Get(device_id, 0L, &value, NULL, NULL);
            if (SUCCEEDED(hr) && value.vt == VT_BSTR && value.bstrVal != NULL) {
                if (wcsstr(value.bstrVal, L"IG_")) {
                    DWORD pid = 0, vid = 0;
                    WCHAR *vid_text = wcsstr(value.bstrVal, L"VID_");
                    if (vid_text && swscanf(vid_text, L"VID_%4X", &vid) != 1)
                        vid = 0;
                    WCHAR *pid_text = wcsstr(value.bstrVal, L"PID_");
                    if (pid_text && swscanf(pid_text, L"PID_%4X", &pid) != 1)
                        pid = 0;

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
    if (namespace_name)
        SysFreeString(namespace_name);
    if (device_id)
        SysFreeString(device_id);
    if (class_name)
        SysFreeString(class_name);
    for (index = 0; index < 20; index++) {
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

    if (cleanup_com)
        CoUninitialize();

    return is_xinput;
}
