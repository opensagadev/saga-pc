#include "saga.h"
#include <WbemIdl.h>
#include <stdio.h>
#include <wchar.h>

extern const GUID PcWbemClassId, PcWbemInterfaceId;

extern "C" u8 IsXInputProductPc(const GUID *product) {
    IWbemLocator *locator = NULL;
    IWbemServices *services = NULL;
    IEnumWbemClassObject *enumerator = NULL;
    IWbemClassObject *objects[20] = {NULL};
    BSTR namespace_name = NULL;
    BSTR class_name = NULL;
    BSTR property_name = NULL;
    ULONG returned = 0;
    u8 result = 0;
    u8 initialized_com = CoInitialize(NULL) >= 0;
    VARIANT value;
    if (CoCreateInstance(PcWbemClassId, NULL, 1, PcWbemInterfaceId, (void **)&locator) < 0 || locator == NULL)
        goto cleanup;
    namespace_name = SysAllocString(L"\\\\.\\root\\cimv2");
    if (namespace_name == NULL)
        goto cleanup;
    class_name = SysAllocString(L"Win32_PNPEntity");
    if (class_name == NULL)
        goto free_strings;
    property_name = SysAllocString(L"DeviceID");
    if (property_name == NULL)
        goto free_strings;
    if (locator->ConnectServer(namespace_name, NULL, NULL, NULL, 0, NULL, NULL, &services) < 0 || services == NULL)
        goto free_strings;
    CoSetProxyBlanket(services, 10, 0, NULL, 3, 3, NULL, 0);
    if (services->CreateInstanceEnum(class_name, 0, NULL, &enumerator) < 0 || enumerator == NULL)
        goto free_strings;
    while (enumerator->Next(10000, 20, objects, &returned) >= 0 && returned != 0) {
        for (ULONG index = 0; index < returned; ++index) {
            if (objects[index]->Get(property_name, 0, &value, NULL, NULL) >= 0 && value.vt == VT_BSTR &&
                value.bstrVal != NULL && wcsstr((const wchar_t *)value.bstrVal, L"IG_") != NULL) {
                u32 vendor = 0;
                u32 device = 0;
                const wchar_t *vendor_text = wcsstr((const wchar_t *)value.bstrVal, L"VID_");
                if (vendor_text != NULL && swscanf(vendor_text, L"VID_%4X", &vendor) != 1)
                    vendor = 0;
                const wchar_t *device_text = wcsstr((const wchar_t *)value.bstrVal, L"PID_");
                if (device_text != NULL && swscanf(device_text, L"PID_%4X", &device) != 1)
                    device = 0;
                if (((device & 0xffff) << 16 | (vendor & 0xffff)) == product->Data1) {
                    result = 1;
                    goto free_strings;
                }
            }
            if (objects[index] != NULL) {
                objects[index]->Release();
                objects[index] = NULL;
            }
        }
    }
free_strings:
    SysFreeString(namespace_name);
    if (property_name != NULL)
        SysFreeString(property_name);
    if (class_name != NULL)
        SysFreeString(class_name);
cleanup:
    for (u32 index = 0; index < 20; ++index) {
        if (objects[index] != NULL) {
            objects[index]->Release();
            objects[index] = NULL;
        }
    }
    if (enumerator != NULL)
        enumerator->Release();
    if (services != NULL)
        services->Release();
    if (locator != NULL)
        locator->Release();
    if (initialized_com)
        CoUninitialize();
    return result;
}
