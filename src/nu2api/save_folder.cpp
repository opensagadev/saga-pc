#include "saga.h"

extern i32 PcSaveFolderRegisteredPc;

extern "C" void __cdecl PcRegisterSaveFolderPc(const char *company, const char *product, const char *parameter,
                                               i32 personal) {
    PcCompanyName = company;
    PcProductName = product;
    NuHwDefaultParameterPc = parameter;
    PcUsePersonalFolder = personal;
    PcSaveFolderRegisteredPc = 1;
}

extern "C" void __cdecl PcSetDefaultParameterPc(const char *parameter) {
    NuHwDefaultParameterPc = parameter;
}

extern "C" void *NuHwParameter33Pc;

extern "C" void __cdecl NuHwSetParameter33Pc(void *value) {
    NuHwParameter33Pc = value;
}

extern "C" void fun_006dddc0(void) {
    if (NuHwParameter33Pc != NULL) {
        ((void (*)(void))NuHwParameter33Pc)();
    }
}

extern "C" WNDCLASSEXA *fun_006dddd0(void) {
    return &PcWindowClass;
}
