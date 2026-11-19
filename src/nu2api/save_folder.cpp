#include "nu2api/PCInput.h"
#include "nu2api/nuwin32/nu2api_windows.h"
#include "nu2api/nuwin32/pcapi.h"
#include "nu2api/pc_options.h"
#include "nu2api/platform_files.h"
#include "nu2api/save_folder.h"

extern i32 PcSaveFolderRegistered;

i32 PcSaveFolderRegistered = 0;

extern "C" void PcRegisterSaveFolder(const char *company, const char *product, const char *parameter, i32 personal) {
    PcCompanyName = company;
    PcProductName = product;
    NuHwDefaultParameter = parameter;
    PcUsePersonalFolder = personal;
    PcSaveFolderRegistered = 1;
}

extern "C" void PcSetDefaultParameter(const char *parameter) {
    NuHwDefaultParameter = parameter;
}

extern "C" void *NuHwParameter33;

extern "C" {
    i32 dat_02975c40 = 0;
    void *NuHwParameter33 = 0;
}

extern "C" void NuHwSetParameter33(void *value) {
    NuHwParameter33 = value;
}

extern "C" void fun_006dddc0(void) {
    if (NuHwParameter33 != NULL) {
        ((void (*)(void))NuHwParameter33)();
    }
}

extern "C" WNDCLASSEXA *PcGetWindowClass(void) {
    return &PcWindowClass;
}

extern "C" i32 dat_02975c40;

extern "C" void fun_006ddde0(i32 value) {
    dat_02975c40 = value;
}
